#include "engine.h"

Engine::Engine(QObject *parent) : QObject(parent)
{

}

Engine::~Engine()
{

}

bool Engine::init(const QHash<QString, QString> &settings)
{
    return true;
}

Page *Engine::getPage(const QString &path) const
{
    return 0;
}

Page *Engine::getPageToEdit(const QString &path) const
{
    return 0;
}

bool Engine::savePage(Page *page)
{
    return false;
}

QList<Page *> Engine::listPages() const
{
    return QList<Page *>();

}
