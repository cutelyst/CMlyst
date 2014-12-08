#include "fileengine_p.h"

#include "page.h"

#include <QDirIterator>
#include <QSettings>
#include <QDateTime>
#include <QHash>

FileEngine::FileEngine() :
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
    d->rootPath = settings["root"];
    return true;
}

Page *FileEngine::getPage(const QString &path)
{
    Q_D(FileEngine);
    QHash<QString, Page*>::const_iterator it = d->pages.constFind(path);
    if (it != d->pages.constEnd()) {
        QFileInfo fileInfo(d->rootPath.absoluteFilePath(path));
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
    QString normPath = Page::readablePath(path);
    Page *page = loadPage(normPath);
    if (!page) {
        page = new Page;
        page->setPath(normPath);
    }

    return page;
}

Page *FileEngine::loadPage(const QString &path) const
{
    Q_D(const FileEngine);

    QString file = d->rootPath.absoluteFilePath(path);
    QFileInfo fileInfo(file);
    if (fileInfo.exists()) {
        QSettings data(file, QSettings::IniFormat);
        Page *page = new Page;
        page->setPath(path);
        page->setName(data.value("Name").toString());
        page->setAuthor(data.value("Author").toString());
        page->setModified(data.value("Modified").toDateTime());
        page->setContent(data.value("Content").toByteArray());
        page->setNavigationLabel(data.value("NavigationLabel").toString());
        page->setTags(data.value("Tags").toStringList());
        return page;
    }
    return 0;
}

bool FileEngine::savePage(Page *page)
{
    Q_D(FileEngine);

    QString file = d->rootPath.absoluteFilePath(page->path());
    QSettings data(file, QSettings::IniFormat);
    data.setValue("Name", page->name());
    data.setValue("Modified", page->modified());
    data.setValue("Author", page->author());
    data.setValue("Content", page->content());
    data.setValue("NavigationLabel", page->navigationLabel());
    data.setValue("Tags", page->tags());
    data.sync();
    // if it's not writable we can't save the page
    return data.isWritable();
}

QList<Page *> FileEngine::listPages()
{
    Q_D(const FileEngine);

    QList<Page *> ret;
    QDirIterator it(d->rootPath, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString path = it.next();
        Page *page = getPage(path);
        if (page) {
            ret.append(page);
        }
    }
    return ret;
}

