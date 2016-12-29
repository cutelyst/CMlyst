#include "cmdispatcher.h"

#include "libCMS/page.h"

#include <Cutelyst/Action>
#include <Cutelyst/Context>
#include <Cutelyst/utils.h>

#include <QStringBuilder>
#include <QStringList>

CMDispatcher::CMDispatcher(QObject *parent) : DispatchType(parent)
{

}

CMDispatcher::~CMDispatcher()
{

}

QByteArray CMDispatcher::list() const
{
    QVector<QStringList> table;
    QStringList l1;
    l1.append(QStringList("Page"));
    l1.append(QLatin1Char('/') % m_pageAction->reverse());
    table.append(l1);

    QStringList post;
    post.append(QStringList("Post"));
    post.append(QLatin1Char('/') % m_postAction->reverse());
    table.append(post);

    return Utils::buildTable(table,
    { "Handle", "Private" },
                      "Loaded Content Manager actions:");
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
        ctx->req()->setArguments(args);
        ctx->req()->setMatch(path);
        setupMatchedAction(ctx, m_latestPostsAction);
        return ExactMatch;
    }

    CMS::Page *page = engine->getPage(path);
    if (page) {
        ctx->stash().insert(QStringLiteral("page"), QVariant::fromValue(page));
        if (page->blog()) {
            ctx->req()->setArguments(args);
            ctx->req()->setMatch(path);
            setupMatchedAction(ctx, m_postAction);
        } else {
            ctx->req()->setArguments(args);
            ctx->req()->setMatch(path);
            setupMatchedAction(ctx, m_pageAction);
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

bool CMDispatcher::inUse()
{
    return m_pageAction && m_postAction && m_latestPostsAction;
}
