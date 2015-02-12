/***************************************************************************
 *   Copyright (C) 2014-2015 Daniel Nicoletti <dantti12@gmail.com>         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; see the file COPYING. If not, write to       *
 *   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,  *
 *   Boston, MA 02110-1301, USA.                                           *
 ***************************************************************************/

#include "root.h"

#include <Cutelyst/Application>
#include <Cutelyst/Context>
#include <Cutelyst/Plugins/authentication.h>
#include <Cutelyst/Plugins/viewengine.h>

#include <QStringBuilder>
#include <QDebug>

#include "../libCMS/fileengine.h"
#include "../libCMS/page.h"
#include "../libCMS/menu.h"

#include "rsswriter.h"

Root::Root()
{
    qDebug() << Q_FUNC_INFO;
}

Root::~Root()
{
}

//void Root::notFound(Context *c)
//{
//    c->stash()[QLatin1String("template")] = "404.html";
//    c->res()->setStatus(404);
//}

void Root::End(Context *ctx)
{
    Q_UNUSED(ctx)
//    qDebug() << "*** Root::End()" << ctx->view();

    const QString &theme = m_engine->settingsValue(QStringLiteral("theme"), QStringLiteral("default"));

    const QString &themePath = m_rootDir.absoluteFilePath(QLatin1String("themes/") % theme);

    ViewEngine *view = qobject_cast<ViewEngine*>(ctx->view());
    // Check if theme path changed
    if (view->includePath() != themePath) {
        view->setIncludePath(themePath);
    }

    QString staticTheme = QLatin1String("/static/themes/") % theme;
    ctx->stash()["basetheme"] = ctx->uriFor(staticTheme).toString();


}

bool Root::postFork(Application *app)
{
    QDir dataDir = app->config("DataLocation").toString();
    m_rootDir = app->config("RootLocation").toString();

    CMS::FileEngine *engine = new CMS::FileEngine(this);
    engine->init({
                     {"root", dataDir.absolutePath()}
                 });
    m_engine = engine;

    return true;
}

void Root::page(Cutelyst::Context *ctx)
{
//    qDebug() << "*** Root::page()";
//    qDebug() << "*** Root::page()" << ctx->req()->path() << ctx->req()->base();

    Response *res = ctx->res();
    Request *req = ctx->req();

    CMS::Page *page = 0;
    QList<CMS::Page *> posts;
    QString templateFile;

    // if we are at the root check if we should show the posts
    if (req->path().isEmpty() && m_engine->settingsValue("show_on_front", "posts") == QLatin1String("posts")) {
        posts = m_engine->listPages(CMS::Engine::Posts,
                                    CMS::Engine::SortFlags(
                                        CMS::Engine::Name |
                                        CMS::Engine::Date |
                                        CMS::Engine::Reversed),
                                    -1,
                                    10);
        templateFile = QStringLiteral("posts.html");
    } else {
        // Find the desired page
        page = m_engine->getPage(req->path());
        if (!page) {
            ctx->stash({
                           {QStringLiteral("template"), QStringLiteral("404.html")},
                           {QStringLiteral("cms"), QVariant::fromValue(m_engine->settings())}
                       });
            res->setStatus(404);
            return;
        }

        // See if the page has changed, if the settings have changed
        // and have a newer date use that instead
        const QDateTime &currentDateTime = qMax(page->modified(), m_engine->lastModified());
        const QDateTime &clientDate = req->headers().ifModifiedSinceDateTime();
        if (clientDate.isValid()) {
            if (currentDateTime == clientDate && currentDateTime.isValid()) {
                res->setStatus(Response::NotModified);
                return;
            }
        }
        res->headers().setLastModified(currentDateTime);

        templateFile = page->blog() ? QStringLiteral("blog.html") : QStringLiteral("page.html");
    }

    ctx->stash({
                   {QStringLiteral("template"), templateFile},
                   {QStringLiteral("cms"), QVariant::fromValue(m_engine->settings())},
                   {QStringLiteral("menus"), QVariant::fromValue(m_engine->menuLocations())},
                   {QStringLiteral("page"), QVariant::fromValue(page)},
                   {QStringLiteral("posts"), QVariant::fromValue(posts)}
               });
}

void Root::feed(Context *ctx)
{
    Response *res = ctx->res();
    Request *req = ctx->req();

    QList<CMS::Page *> posts;
    posts = m_engine->listPages(CMS::Engine::Posts,
                                CMS::Engine::SortFlags(
                                    CMS::Engine::Name |
                                    CMS::Engine::Date |
                                    CMS::Engine::Reversed),
                                -1,
                                10);
    if (!posts.isEmpty()) {
        // See if the page has changed, if the settings have changed
        // and have a newer date use that instead
        const QDateTime &currentDateTime = posts.first()->created();
        const QDateTime &clientDate = req->headers().ifModifiedSinceDateTime();
        if (clientDate.isValid()) {
            if (currentDateTime == clientDate && currentDateTime.isValid()) {
                res->setStatus(Response::NotModified);
                return;
            }
        }
        res->headers().setLastModified(currentDateTime);
    }

    RSSWriter writer;
    writer.writeStartChannel();
    writer.writeChannelTitle(m_engine->title());
    writer.writeChannelDescription(m_engine->description());
    writer.writeChannelFeedLink(req->base());
    if (!posts.isEmpty()) {
        writer.writeChannelLastBuildDate(posts.first()->created());
    }

    Q_FOREACH (CMS::Page *post, posts) {
        writer.writeStartItem();
        writer.writeItemTitle(post->name());
        writer.writeItemCreator(post->author());
        writer.writeItemPubDate(post->created());
        writer.writeItemDescription(post->content().left(300));
        writer.writeItemContent(post->content());
        writer.writeEndItem();
    }

    writer.writeEndChannel();
    writer.endRSS();

    res->body() = writer.result();
    res->setContentType(QStringLiteral("text/xml; charset=UTF-8"));

    //    qDebug() << writer.result();
}
