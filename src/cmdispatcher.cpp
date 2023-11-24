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
    if (!m_pageAction) {
        return QByteArray();
    }

    QStringList page = {
        QStringLiteral("Page"),
        QLatin1Char('/') + m_pageAction->reverse()
    };

    return Utils::buildTable({ page },
    { QStringLiteral("Handle"), QStringLiteral("Private") },
                      QStringLiteral("Loaded Content Manager actions:"));
}

DispatchType::MatchType CMDispatcher::match(Context *c, QStringView pathView, const QStringList &args) const
{
    // we only match absolute paths
    // Cutelyst v4 changed path to start with /
    const QString path = pathView.sliced(pathView.size() > 0 ? 1 : 0).toString();
    if (!args.isEmpty() || path.endsWith(u'/')) {
        return NoMatch;
    }

    auto settings = engine->loadSettings(c);

    // See if we are on front page path and the settings says
    // it should show the latest posts, or if the desired page path is set
    // to show the latest posts
    bool showPostsOnFront = settings.value(QStringLiteral("show_on_front"), QStringLiteral("posts")) == QLatin1String("posts");

    Request *req = c->request();
    if ((path.isEmpty() && showPostsOnFront) ||
            (!showPostsOnFront && settings.value(QStringLiteral("page_for_posts")) == path)) {
        req->setArguments(args);
        req->setMatch(path);
        setupMatchedAction(c, m_latestPostsAction);
        return ExactMatch;
    }

    CMS::Page *page;
    if (path.isEmpty() && !showPostsOnFront) {
        page = engine->getPage(settings.value(QStringLiteral("page_on_front")), c);
    } else {
        page = engine->getPage(path, c);
    }

    if (page && page->published()) {
        c->setStash(QStringLiteral("page"), QVariant::fromValue(page));
        req->setArguments(args);
        req->setMatch(path);
        setupMatchedAction(c, m_pageAction);
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

    if (action->attributes().contains(QLatin1String("LatestPosts")) && !m_latestPostsAction) {
        m_latestPostsAction = action;
        return true;
    }

    return false;
}

bool CMDispatcher::inUse()
{
    return m_pageAction && m_latestPostsAction;
}
