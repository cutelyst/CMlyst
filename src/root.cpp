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

void Root::notFound(Context *ctx)
{
    ctx->stash({
                   {QStringLiteral("template"), QStringLiteral("404.html")},
                   {QStringLiteral("cms"), QVariant::fromValue(engine->settings())},
                   {QStringLiteral("menus"), QVariant::fromValue(engine->menuLocations())},
               });
    ctx->res()->setStatus(404);
}

void Root::End(Context *ctx)
{
    Q_UNUSED(ctx)
//    qDebug() << "*** Root::End()" << ctx->view();

    const QString &theme = engine->settingsValue(QStringLiteral("theme"), QStringLiteral("default"));
    // Check if the theme changed
    if (m_theme != theme) {
        m_theme = theme;

        ViewEngine *view = qobject_cast<ViewEngine*>(ctx->view());
        view->setIncludePath(m_rootDir.absoluteFilePath(QLatin1String("themes/") % theme));
    }

    QString staticTheme = QLatin1String("/static/themes/") % theme;
    ctx->stash()["basetheme"] = ctx->uriFor(staticTheme).toString();
}

bool Root::postFork(Application *app)
{
    m_rootDir = app->config("RootLocation").toString();

    return true;
}

void Root::page(Cutelyst::Context *ctx)
{
//    qDebug() << "*** Root::page()";
//    qDebug() << "*** Root::page()" << ctx->req()->path() << ctx->req()->base();

    Response *res = ctx->res();
    Request *req = ctx->req();

    // Get the desired page (dispatcher already found it)
    CMS::Page *page = ctx->stash(QStringLiteral("page")).value<CMS::Page *>();

    // See if the page has changed, if the settings have changed
    // and have a newer date use that instead
    const QDateTime &currentDateTime = qMax(page->modified(), engine->lastModified());
    const QDateTime &clientDate = req->headers().ifModifiedSinceDateTime();
    if (clientDate.isValid() && currentDateTime == clientDate) {
        res->setStatus(Response::NotModified);
        return;
    }
    res->headers().setLastModified(currentDateTime);

    ctx->stash({
                   {QStringLiteral("template"), QStringLiteral("page.html")},
                   {QStringLiteral("cms"), QVariant::fromValue(engine->settings())},
                   {QStringLiteral("menus"), QVariant::fromValue(engine->menuLocations())},
                   {QStringLiteral("page"), QVariant::fromValue(page)}
               });
}

void Root::post(Context *ctx)
{
    Response *res = ctx->res();
    Request *req = ctx->req();
    CMS::Page *page = ctx->stash(QStringLiteral("page")).value<CMS::Page *>();

    // See if the page has changed, if the settings have changed
    // and have a newer date use that instead
    const QDateTime &currentDateTime = qMax(page->modified(), engine->lastModified());
    const QDateTime &clientDate = req->headers().ifModifiedSinceDateTime();
    if (clientDate.isValid() && currentDateTime == clientDate) {
        res->setStatus(Response::NotModified);
        return;
    }
    res->headers().setLastModified(currentDateTime);

    ctx->stash({
                   {QStringLiteral("template"), QStringLiteral("blog.html")},
                   {QStringLiteral("cms"), QVariant::fromValue(engine->settings())},
                   {QStringLiteral("menus"), QVariant::fromValue(engine->menuLocations())}
               });
}

void Root::lastPosts(Context *ctx)
{
    Response *res = ctx->res();
    Request *req = ctx->req();
    QList<CMS::Page *> posts;
    posts = engine->listPages(CMS::Engine::Posts,
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

    ctx->stash({
                   {QStringLiteral("template"), QStringLiteral("posts.html")},
                   {QStringLiteral("cms"), QVariant::fromValue(engine->settings())},
                   {QStringLiteral("menus"), QVariant::fromValue(engine->menuLocations())},
                   {QStringLiteral("posts"), QVariant::fromValue(posts)}
               });
}

void Root::feed(Context *ctx)
{
    Response *res = ctx->res();
    Request *req = ctx->req();

    QList<CMS::Page *> posts;
    posts = engine->listPages(CMS::Engine::Posts,
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
    writer.writeChannelTitle(engine->title());
    writer.writeChannelDescription(engine->description());
    writer.writeChannelFeedLink(req->base());
    if (!posts.isEmpty()) {
        writer.writeChannelLastBuildDate(posts.first()->created());
    }

    Q_FOREACH (CMS::Page *post, posts) {
        writer.writeStartItem();
        writer.writeItemTitle(post->name());
        QString link = ctx->uriFor(post->path()).toString();
        writer.writeItemLink(link);
        writer.writeItemCommentsLink(link % QLatin1String("#comments"));
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
