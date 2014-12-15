#include "engine.h"

using namespace CMS;

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

Page *Engine::getPage(const QString &path)
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

QList<Page *> Engine::listPages()
{
    return QList<Page *>();

}
