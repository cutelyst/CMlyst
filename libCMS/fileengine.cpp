/***************************************************************************
 *   Copyright (C) 2014-2015 Daniel Nicoletti <dantti12@gmail.com>         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; see the file COPYING. If not, write to       *
 *   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,  *
 *   Boston, MA 02110-1301, USA.                                           *
 ***************************************************************************/

#include "fileengine_p.h"

#include "page.h"
#include "menu.h"

#include <QRegularExpression>
#include <QDirIterator>
#include <QSettings>
#include <QDateTime>
#include <QHash>
#include <QStringBuilder>
#include <QFileSystemWatcher>
#include <QCoreApplication>
#include <QDebug>

#include <unistd.h>

#include <sys/types.h>
#include <utime.h>

using namespace CMS;

FileEngine::FileEngine(QObject *parent) :
    Engine(parent),
    d_ptr(new FileEnginePrivate)
{
}

FileEngine::~FileEngine()
{
    delete d_ptr;
}

bool FileEngine::init(const QHash<QString, QString> &settings)
{
    Q_D(FileEngine);

    QString root = settings["root"];
    if (root.isEmpty()) {
        root = QDir::currentPath();
    }
    d->rootPath = root;
    d->pagesPath = root % QLatin1String("/pages");
    d->settingsInfo = d->rootPath.absoluteFilePath("site.conf");
    d->settings = new QSettings(d->settingsInfo.absoluteFilePath(), QSettings::IniFormat);

    if (!d->pagesPath.exists() &&
            !d->pagesPath.mkpath(d->pagesPath.absolutePath())) {
        qWarning() << "Failed to create pages path" << d->pagesPath.absolutePath();
        return false;
    }

    QFileSystemWatcher *watch = new QFileSystemWatcher(this);
    watch->addPath(d->pagesPath.absolutePath());
    connect(watch, &QFileSystemWatcher::directoryChanged,
            this, &FileEngine::loadPages);
    loadPages();

    QFileSystemWatcher *settingsWatch = new QFileSystemWatcher(this);
    settingsWatch->addPath(d->settingsInfo.absoluteFilePath());
    connect(settingsWatch, &QFileSystemWatcher::fileChanged,
            this, &FileEngine::loadSettings);
    loadSettings();

    if (!d->settings->isWritable()) {
        qWarning() << "Settings file is not writable!" << d->settingsInfo.absoluteFilePath();
        return false;
    }

    return true;
}

Page *FileEngine::getPage(const QString &path)
{
    Q_D(FileEngine);

    QHash<QString, Page*>::ConstIterator it = d->pathPages.constFind(path);
    if (it != d->pathPages.constEnd()) {
        return it.value();
    }
    return 0;
}

Page *FileEngine::getPageToEdit(const QString &path)
{
    Page *page = getPage(path);
    if (!page) {
        page = new Page;
        page->setPath(path);
        QDateTime dt = QDateTime::currentDateTimeUtc();
        page->setCreated(dt);
        page->setModified(dt);
    }

    return page;
}

Page *FileEngine::loadPage(const QString &filename)
{
    Q_D(FileEngine);

    // skip temporary files
    if (filename.endsWith(QLatin1Char('~'))) {
        return 0;
    }

    const QString &relPathPercent = d->pagesPath.relativeFilePath(filename);
    QString relPath = QUrl::fromPercentEncoding(relPathPercent.toLatin1());

    // Paths like http://foo.com or http://foo.com/bar/
    // have files that ends with index
    if (relPath.endsWith(QLatin1String("index"))) {
        relPath.remove(relPath.size() - 5, 5);
    }

    Page *page = 0;
    QFileInfo fileInfo(filename);
    QHash<QString, Page*>::ConstIterator it = d->pathPages.constFind(relPath);
    if (it != d->pathPages.constEnd()) {
        page = it.value();

        if (page->modified() != fileInfo.lastModified()) {
            d->pathPages.remove(relPath);
            d->posts.removeOne(page);
            d->pages.removeOne(page);
            delete page;
            page = 0;
        }
    }

    if (!page) {
        QSettings data(filename, QSettings::IniFormat);
        page = new Page;

        page->setPath(relPath);
        page->setName(data.value("Name").toString());

        QString author = data.value("Author").toString();
        if (author.isEmpty()) {
            author = fileInfo.owner();
        }
        page->setAuthor(author);

        QDateTime modified = QDateTime::fromString(data.value("Modified").toString(), Qt::ISODate);
        if (modified.isValid()) {
        } else {
            modified = fileInfo.lastModified().toUTC();
        }
        page->setModified(modified);

        QDateTime created = QDateTime::fromString(data.value("Created").toString(), Qt::ISODate);
        if (created.isValid()) {
        } else {
            created = fileInfo.created().toUTC();
        }
        page->setCreated(created);

        page->setNavigationLabel(data.value("NavigationLabel").toString());
        page->setTags(data.value("Tags").toStringList());
        page->setBlog(data.value("Blog").toBool());
        page->setAllowComments(data.value("AllowComments").toBool());

        data.beginGroup("Body");
        page->setContent(data.value("Content").toString());
        data.endGroup();

        d->pathPages.insert(relPath, page);
        if (page->blog()) {
            d->posts.append(page);
        } else {
            d->pages.append(page);
        }
    }

    return page;
}

bool FileEngine::savePage(Page *page)
{
    Q_D(FileEngine);

    QString path = page->path();
    path.remove(QRegularExpression("^/"));
    if (path.isEmpty()) {
        path = QStringLiteral("index");
    }

    const QString &file = d->pagesPath.absoluteFilePath(path.toLatin1().toPercentEncoding());
//    qDebug() << "save Page" << page->path() << path;
    QSettings data(file, QSettings::IniFormat);
    data.setValue("Name", page->name());
    data.setValue("Modified", QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    data.setValue("Created", page->created().toUTC().toString(Qt::ISODate));
    data.setValue("Author", page->author());
    data.setValue("NavigationLabel", page->navigationLabel());
    data.setValue("Tags", page->tags());
    data.setValue("Blog", page->blog());
    data.setValue("AllowComments", page->allowComments());

    data.beginGroup("Body");
    data.setValue("Content", page->content());
    data.endGroup();
    data.sync();

#if (QT_VERSION <= QT_VERSION_CHECK(5, 4, 0))
    // Force a change to notify cache that something changed
    utime(file.toLatin1().data(), NULL);
#endif

//    qDebug() << "save page" << file;
    // if it's not writable we can't save the page
    return data.isWritable();
}

bool dateLessThan(Page *page1, Page *page2)
{
    return page1->created() < page2->created();
}

bool dateGreaterThan(Page *page1, Page *page2)
{
    return page1->created() > page2->created();
}

bool nameLessThan(Page *page1, Page *page2)
{
    return page1->name() < page2->name();
}

bool nameGreaterThan(Page *page1, Page *page2)
{
    return page1->name() > page2->name();
}

bool dateNameLessThan(Page *page1, Page *page2)
{
    const QDateTime &dt1 = page1->created();
    const QDateTime &dt2 = page2->created();
    if (dt1 == dt2) {
        return page1->name() < page2->name();
    } else {
        return dt1 < dt2;
    }
}

bool dateNameGreaterThan(Page *page1, Page *page2)
{
    const QDateTime &dt1 = page1->created();
    const QDateTime &dt2 = page2->created();
    if (dt1 == dt2) {
        return page1->name() > page2->name();
    } else {
        return dt1 > dt2;
    }
}

QList<Page *> FileEngine::listPages(Engine::Filters filters, Engine::SortFlags sort, int depth, int limit)
{
    Q_D(const FileEngine);

    QList<Page *> ret;
    QList<Page *> pages;

    if (filters != NoFilter) {
        if (filters & Posts) {
            pages.append(d->posts);
        }

        if (filters & Pages) {
            pages.append(d->pages);
        }
    } else {
        pages = d->pages;
    }


    if (depth == -1) {
        ret = pages;
    } else {
        Q_FOREACH (Page *page, pages) {
            if (depth != -1 && page->path().count(QChar('/')) > depth) {
                continue;
            }

            ret.append(page);
        }
    }

    // apply the sorting
    if (sort & Date && sort & Name) {
        qSort(ret.begin(), ret.end(), sort & Reversed ? dateNameGreaterThan : dateNameLessThan);
    } else if (sort & Date) {
        qSort(ret.begin(), ret.end(), sort & Reversed ? dateGreaterThan : dateLessThan);
    } else if (sort & Name) {
        qSort(ret.begin(), ret.end(), sort & Reversed ? nameGreaterThan : nameLessThan);
    }

    // Limit the list
    if (limit == -1) {
        return ret;
    } else {
        return ret.mid(0, limit);
    }
}

QList<Menu *> FileEngine::menus()
{
    Q_D(FileEngine);
    return d->menus;
}

QHash<QString, Menu *> FileEngine::menuLocations()
{
    Q_D(FileEngine);
    return d->menuLocations;
}

bool FileEngine::saveMenu(Menu *menu, bool replace)
{
    Q_D(const FileEngine);

    bool ret = false;
    d->settings->beginGroup(QStringLiteral("Menus"));

    if (replace || !d->settings->childGroups().contains(menu->name())) {
        d->settings->beginGroup(menu->name());

        d->settings->setValue("AutoAddPages", menu->autoAddPages());
        d->settings->setValue("Locations", menu->locations());

        QList<QVariantHash> urls = menu->entries();
        d->settings->beginWriteArray("urls", urls.size());
        for (int i = 0; i < urls.size(); ++i) {
            d->settings->setArrayIndex(i);
            // TODO save all values
            d->settings->setValue("text", urls.at(i).value("text"));
            d->settings->setValue("url", urls.at(i).value("url"));
        }
        d->settings->endArray();

        d->settings->endGroup();
        ret = true;
    }

    d->settings->endGroup(); // Menus

    return ret && d->settings->isWritable();
}

bool FileEngine::removeMenu(const QString &name)
{
    Q_D(FileEngine);

    d->settings->beginGroup(QStringLiteral("Menus"));
    if (d->settings->childGroups().contains(name)) {
        d->settings->remove(name);
    }
    d->settings->endGroup();

    return d->settings->isWritable();
}

QDateTime FileEngine::lastModified()
{
    Q_D(FileEngine);
    return d->mainSettingsDT;
}

bool FileEngine::settingsIsWritable()
{
    Q_D(FileEngine);
    return d->settings->isWritable();
}

QHash<QString, QString> FileEngine::settings()
{
    Q_D(FileEngine);
    return d->mainSettings;
}

QString FileEngine::settingsValue(const QString &key, const QString &defaultValue)
{
    Q_D(FileEngine);

    QHash<QString, QString>::ConstIterator it = d->mainSettings.constFind(key);
    if (it != d->mainSettings.constEnd()) {
        return it.value();
    }
    return defaultValue;
}

bool FileEngine::setSettingsValue(const QString &key, const QString &value)
{
    Q_D(FileEngine);
    d->settings->beginGroup(QStringLiteral("Main"));
    d->settings->setValue(key, value);
    d->settings->endGroup();

#if (QT_VERSION <= QT_VERSION_CHECK(5, 4, 0))
    // Force a change to notify cache that something changed
    utime(d->settingsInfo.absoluteFilePath().toLatin1().data(), NULL);
#endif

    // this is needed due to a new
    // call to value() does not return old values
    d->mainSettings.insert(key, value);

    return d->settings->isWritable();
}

void FileEngine::loadPages()
{
    Q_D(FileEngine);

    qDebug() << "loading pages..." << QCoreApplication::applicationPid();

    QDirIterator it(d->pagesPath.path(),
                    QDir::Files | QDir::NoDotAndDotDot,
                    QDirIterator::Subdirectories);
    while (it.hasNext()) {
        loadPage(it.next());
    }
}

void FileEngine::loadSettings()
{
    Q_D(FileEngine);

    // Make sure we read new things
    d->settings->sync();

    // Load main settings
    QHash<QString, QString> settings;
    d->settings->beginGroup(QStringLiteral("Main"));
    Q_FOREACH (const QString &key, d->settings->allKeys()) {
        settings.insert(key, d->settings->value(key).toString());
    }
    d->settings->endGroup();
    d->mainSettings = settings;

    // Load menus and menu locations
    d->settings->beginGroup(QStringLiteral("Menus"));
    QList<CMS::Menu *> menus;
    QHash<QString, CMS::Menu *> menuLocations;
    foreach (const QString &menu, d->settings->childGroups()) {
        Menu *obj = d->createMenu(menu, this);
        menus.append(obj);

        bool added = false;
        Q_FOREACH (const QString &location, obj->locations()) {
            if (!menuLocations.contains(location)) {
                menuLocations.insert(location, obj);
                added = true;
            }
        }

        if (!added) {
            delete obj;
        }
    }
    d->settings->endGroup();
    d->menus = menus;
    d->menuLocations = menuLocations;

    // Store the last modified date
    d->mainSettingsDT = d->settingsInfo.lastModified();
}

Menu *FileEnginePrivate::createMenu(const QString &name, QObject *parent)
{
    settings->beginGroup(name);

    Menu *menu = new Menu(name, parent);

    menu->setLocations(settings->value("Locations").toStringList());
    menu->setAutoAddPages(settings->value("AutoAddPages").toBool());

    QList<QVariantHash> urls;

    int size = settings->beginReadArray("urls");
    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        QVariantHash data;
        // TODO read all data
        data.insert("id", i);
        data.insert("text", settings->value("text"));
        data.insert("url", settings->value("url"));
        data.insert("attr", settings->value("attr"));
        urls.append(data);
    }
    settings->endArray();

    menu->setEntries(urls);

    settings->endGroup(); // menu name

    return menu;
}
