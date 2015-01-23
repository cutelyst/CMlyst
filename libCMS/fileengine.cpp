#include "fileengine_p.h"

#include "page.h"

#include <QRegularExpression>
#include <QDirIterator>
#include <QSettings>
#include <QDateTime>
#include <QHash>
#include <QStringBuilder>
#include <QDebug>

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

    if (!d->pagesPath.exists() &&
            !d->pagesPath.mkpath(d->pagesPath.absolutePath())) {
        qWarning() << "Failed to create pages path" << d->pagesPath.absolutePath();
        return false;
    }
    return true;
}

Page *FileEngine::getPage(const QString &path)
{
    Q_D(FileEngine);

    QHash<QString, Page*>::const_iterator it = d->pages.constFind(path);
    if (it != d->pages.constEnd()) {
        QFileInfo fileInfo(d->pagesPath.absoluteFilePath(path));
        if (it.value()->modified() == fileInfo.lastModified()) {
            return it.value();
        }
    }

    Page *page = loadPage(path);
    if (page) {
        d->pages.insert(path, page);
        return page;
    }
    return 0;
}

Page *FileEngine::getPageToEdit(const QString &path) const
{
    Page *page = loadPage(path);
    if (!page) {
        page = new Page;
        page->setPath(path);
    }

    return page;
}

Page *FileEngine::loadPage(const QString &path) const
{
    Q_D(const FileEngine);

    QString normPath = path;
//    qDebug() << "getPageToEdit" << path;
    normPath.remove(QRegularExpression("^/"));
//    qDebug() << "getPageToEdit" << normPath;
    if (normPath.isEmpty()) {
        normPath = QStringLiteral("index.page");
    } else {
        normPath.append(QStringLiteral(".page"));
    }
//    qDebug() << "getPageToEdit" << normPath;


    QString file = d->pagesPath.absoluteFilePath(normPath);
    QFileInfo fileInfo(file);
    if (fileInfo.exists()) {
        QSettings data(file, QSettings::IniFormat);
        Page *page = new Page;

        // TODO save local path
//        QString localPath = path;
//        localPath = localPath.remove(0, d->pagesPath.path().length());
        page->setPath(path);

        page->setName(data.value("Name").toString());
        page->setAuthor(data.value("Author").toString());
        page->setModified(data.value("Modified").toDateTime());
        page->setContent(data.value("Content").toString());
        page->setNavigationLabel(data.value("NavigationLabel").toString());
        page->setTags(data.value("Tags").toStringList());
        return page;
    } else {
        qDebug() << "FileEngine file not found" << file;
    }
    return 0;
}

bool FileEngine::savePage(Page *page)
{
    Q_D(FileEngine);

    QString path = page->path();
    path.remove(QRegularExpression("^/"));
    if (path.isEmpty()) {
        path = QStringLiteral("index.page");
    } else {
        path.append(QStringLiteral(".page"));
    }

    QString file = d->pagesPath.absoluteFilePath(path);
//    qDebug() << "save Page" << page->path() << path;
    QSettings data(file, QSettings::IniFormat);
    data.setValue("Name", page->name());
    data.setValue("Modified", page->modified());
    data.setValue("Author", page->author());
    data.setValue("Content", page->content());
    data.setValue("NavigationLabel", page->navigationLabel());
    data.setValue("Tags", page->tags());
    data.sync();
//    qDebug() << "save page" << file;
    // if it's not writable we can't save the page
    return data.isWritable();
}

QList<Page *> FileEngine::listPages(int depth)
{
    Q_D(const FileEngine);

    QList<Page *> ret;
//    qDebug() << "listpages:" << d->pagesPath.path();

    QDirIterator it(d->pagesPath.path(),
                    QDir::Files | QDir::NoDotAndDotDot,
                    QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString path = it.next();
//        qDebug() << "listpages:" << path;

        QString relpath = d->pagesPath.relativeFilePath(path);
//        qDebug() << "listpages relative:" << relpath << depth << relpath.count(QChar('/'));
        if (depth != -1 && relpath.count(QChar('/')) > depth) {
            continue;
        }

        if (relpath == QLatin1String("index.page")) {
            relpath = QStringLiteral("");
        } else {
            relpath.remove(QRegularExpression(".page$"));
        }

//        qDebug() << "listpages relative ok:" << relpath;
        Page *page = getPage(relpath);
        if (page) {
            ret.append(page);
        }
    }
    return ret;
}

