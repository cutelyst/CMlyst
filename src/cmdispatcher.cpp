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

DispatchType::MatchType CMDispatcher::match(Context *ctx, const QString &path, const QStringList &args) const
{
    // we only match absolute paths
    if (!args.isEmpty() || path.endsWith(QChar('/'))) {
        return NoMatch;
    }

    // See if we are on front page path and the settings says
    // it should show the latest posts, or if the desired page path is set
    // to show the latest posts
    bool showOnFront = engine->settingsValue(QStringLiteral("show_on_front"),
                                             QStringLiteral("posts")) == QLatin1String("posts");
    if ((path.isEmpty() && showOnFront) ||
            (!showOnFront && engine->settingsValue(QStringLiteral("page_for_posts")) == path)) {
        setupMatchedAction(ctx, m_latestPostsAction, path, args, QStringList());
        return ExactMatch;
    }

    CMS::Page *page = engine->getPage(path);
    if (page) {
        ctx->stash().insert(QStringLiteral("page"), QVariant::fromValue(page));
        if (page->blog()) {
            setupMatchedAction(ctx, m_postAction, path, args, QStringList());
        } else {
            setupMatchedAction(ctx, m_pageAction, path, args, QStringList());
        }
        return ExactMatch;
    }

    return NoMatch;
}

QString CMDispatcher::uriForAction(Action *action, const QStringList &captures) const
{
    return QString();
}

bool CMDispatcher::registerAction(Action *action)
{
//    qDebug() << action->attributes();

    if (action->attributes().contains("Page") && !m_pageAction) {
        m_pageAction = action;
        return true;
    }

    if (action->attributes().contains("Post") && !m_postAction) {
        m_postAction = action;
        return true;
    }

    if (action->attributes().contains("LatestPosts") && !m_latestPostsAction) {
        m_latestPostsAction = action;
        return true;
    }

    return false;
}

bool CMDispatcher::inUse() const
{
    return m_pageAction && m_postAction && m_latestPostsAction;
}

