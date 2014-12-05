#include "fileengine.h"

#include <QDirIterator>

FileEngine::FileEngine()
{

}

FileEngine::~FileEngine()
{

}

bool FileEngine::init(const QHash<QString, QString> &settings)
{
    QString rootPath = settings["root"];

}

Page *FileEngine::getPage(const QString &path) const
{

}

Page *FileEngine::getPageToEdit(const QString &path) const
{

}

bool FileEngine::savePage(Page *page)
{

}

QList<Page *> FileEngine::listPages() const
{

}

