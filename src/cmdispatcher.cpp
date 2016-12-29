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
    QStringList l1 = {
        QStringLiteral("Page"),
        QLatin1Char('/') + m_pageAction->reverse()
    };

    QStringList post = {
        QStringLiteral("Post"),
        QLatin1Char('/') + m_postAction->reverse()
    };

    return Utils::buildTable({ l1, post },
    { QStringLiteral("Handle"), QStringLiteral("Private") },
                      QStringLiteral("Loaded Content Manager actions:"));
}

DispatchType::MatchType CMDispatcher::match(Context *c, const QString &path, const QStringList &args) const
{
    // we only match absolute paths
    if (!args.isEmpty() || path.endsWith(QLatin1Char('/'))) {
        return NoMatch;
    }

    // See if we are on front page path and the settings says
    // it should show the latest posts, or if the desired page path is set
    // to show the latest posts
    bool showOnFront = engine->settingsValue(QStringLiteral("show_on_front"),
                                             QStringLiteral("posts")) == QLatin1String("posts");

    if ((path.isEmpty() && showOnFront) ||
            (!showOnFront && engine->settingsValue(QStringLiteral("page_for_posts")) == path)) {
        c->req()->setArguments(args);
        c->req()->setMatch(path);
        setupMatchedAction(c, m_latestPostsAction);
        return ExactMatch;
    }

    CMS::Page *page = engine->getPage(path);
    if (page) {
        c->stash().insert(QStringLiteral("page"), QVariant::fromValue(page));
        if (page->blog()) {
            c->req()->setArguments(args);
            c->req()->setMatch(path);
            setupMatchedAction(c, m_postAction);
        } else {
            c->req()->setArguments(args);
            c->req()->setMatch(path);
            setupMatchedAction(c, m_pageAction);
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

    if (action->attributes().contains(QLatin1String("Page")) && !m_pageAction) {
        m_pageAction = action;
        return true;
    }

    if (action->attributes().contains(QLatin1String("Post")) && !m_postAction) {
        m_postAction = action;
        return true;
    }

    if (action->attributes().contains(QLatin1String("LatestPosts")) && !m_latestPostsAction) {
        m_latestPostsAction = action;
        return true;
    }

    return false;
}

bool CMDispatcher::inUse()
{
    return m_pageAction && m_postAction && m_latestPostsAction;
}
