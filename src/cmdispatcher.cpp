#include "cmdispatcher.h"

#include "../libCMS/page.h"

#include <Cutelyst/Action>

#include <QStringList>

CMDispatcher::CMDispatcher(QObject *parent) : DispatchType(parent)
{

}

CMDispatcher::~CMDispatcher()
{

}

QByteArray CMDispatcher::list() const
{
    QByteArray buffer;
    QTextStream out(&buffer, QIODevice::WriteOnly);

    out << "Loaded Content Manager actions:" << endl;
    out << "Page: " << m_pageAction->reverse() << endl;
    out << "Post: " << m_postAction->reverse() << endl;

    return buffer;
}

bool CMDispatcher::match(Context *ctx, const QString &path, const QStringList &args) const
{
    // we only match absolute paths
    if (!args.isEmpty() && path.endsWith(QChar('/'))) {
        return false;
    }

    CMS::Page *page = engine->getPage(path);
    if (page) {
        ctx->stash().insert(QStringLiteral("page"), QVariant::fromValue(page));
        if (page->blog()) {
            setupMatchedAction(ctx, m_postAction, path);
        } else {
            setupMatchedAction(ctx, m_pageAction, path);
        }
        return true;
    }

    return false;
}

QString CMDispatcher::uriForAction(Action *action, const QStringList &captures) const
{
    return QString();
}

bool CMDispatcher::registerAction(Action *action)
{
    qDebug() << action->attributes();

    if (action->attributes().contains("Page") && !m_pageAction) {
        m_pageAction = action;
        return true;
    }

    if (action->attributes().contains("Post") && !m_postAction) {
        m_postAction = action;
        return true;
    }

    return false;
}

bool CMDispatcher::inUse() const
{
    return m_pageAction && m_postAction;
}

