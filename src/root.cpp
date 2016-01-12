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
#include <Cutelyst/Plugins/Authentication/authentication.h>
#include <Cutelyst/Plugins/View/Grantlee/grantleeview.h>

#include <QStringBuilder>
#include <QBuffer>
#include <QDebug>

#include "../libCMS/page.h"
#include "../libCMS/menu.h"

#include "rsswriter.h"

Root::Root(Application *app) : Controller(app)
{
}

Root::~Root()
{
}

void Root::notFound(Context *c)
{
    c->stash({
                 {QStringLiteral("template"), QStringLiteral("404.html")},
                 {QStringLiteral("cms"), QVariant::fromValue(engine->settings())},
                 {QStringLiteral("menus"), QVariant::fromValue(engine->menuLocations())},
             });
    c->res()->setStatus(404);
}

void Root::End(Context *c)
{
    Q_UNUSED(c)
    //    qDebug() << "*** Root::End()" << c->view();

    const QString &theme = engine->settingsValue(QStringLiteral("theme"), QStringLiteral("default"));
    // Check if the theme changed
    if (m_theme != theme) {
        m_theme = theme;

        GrantleeView *view = qobject_cast<GrantleeView*>(c->app()->view());
        view->setIncludePaths({ m_themeDir.absoluteFilePath(theme) });
    }

    QString staticTheme = QLatin1String("/static/themes/") % theme;
    c->stash()["basetheme"] = c->uriFor(staticTheme).toString();
}

bool Root::postFork(Application *app)
{
    m_themeDir = app->pathTo({ "root", "themes" });

    return true;
}

void Root::page(Cutelyst::Context *c)
{
    //    qDebug() << "*** Root::page()";
    //    qDebug() << "*** Root::page()" << c->req()->path() << c->req()->base();

    Response *res = c->res();
    Request *req = c->req();

    // Get the desired page (dispatcher already found it)
    CMS::Page *page = c->stash(QStringLiteral("page")).value<CMS::Page *>();

    // See if the page has changed, if the settings have changed
    // and have a newer date use that instead
    QDateTime currentDateTime = qMax(page->modified(), engine->lastModified());
    const QDateTime &clientDate = req->headers().ifModifiedSinceDateTime();
    if (clientDate.isValid() && currentDateTime == clientDate) {
        res->setStatus(Response::NotModified);
        return;
    }
    res->headers().setLastModified(currentDateTime);

    QString cmsPagePath = QLatin1Char('/') % c->req()->path();
    engine->setProperty("pagePath", cmsPagePath);
    c->stash({
                 {QStringLiteral("template"), QStringLiteral("page.html")},
                 {QStringLiteral("cms"), QVariant::fromValue(engine)},
             });
}

void Root::post(Context *c)
{
    Response *res = c->res();
    Request *req = c->req();
    CMS::Page *page = c->stash(QStringLiteral("page")).value<CMS::Page *>();

    // See if the page has changed, if the settings have changed
    // and have a newer date use that instead
    QDateTime currentDateTime = qMax(page->modified(), engine->lastModified());
    const QDateTime &clientDate = req->headers().ifModifiedSinceDateTime();
    if (clientDate.isValid() && currentDateTime == clientDate) {
        res->setStatus(Response::NotModified);
        return;
    }
    res->headers().setLastModified(currentDateTime);

    QString cmsPagePath = QLatin1Char('/') % c->req()->path();
    engine->setProperty("pagePath", cmsPagePath);
    c->stash({
                 {QStringLiteral("template"), QStringLiteral("blog.html")},
                 {QStringLiteral("cms"), QVariant::fromValue(engine)},
             });
}

void Root::lastPosts(Context *c)
{
    Response *res = c->res();
    Request *req = c->req();
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

    QString cmsPagePath = QLatin1Char('/') % c->req()->path();
    engine->setProperty("pagePath", cmsPagePath);
    c->stash({
                 {QStringLiteral("template"), QStringLiteral("posts.html")},
                 {QStringLiteral("cms"), QVariant::fromValue(engine)},
                 {QStringLiteral("posts"), QVariant::fromValue(posts)}
             });
}

void Root::feed(Context *c)
{
    Request *req = c->req();
    Response *res = c->res();
    res->setContentType(QStringLiteral("text/xml; charset=UTF-8"));

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

    QBuffer *buffer = new QBuffer(c);
    buffer->open(QBuffer::ReadWrite);

    writer.setDevice(buffer);

    writer.startRSS();
    writer.writeStartChannel();
    writer.writeChannelTitle(engine->title());
    writer.writeChannelFeedLink(c->uriFor(c->action()).toString());
    writer.writeChannelLink(req->base());
    writer.writeChannelDescription(engine->description());
    if (!posts.isEmpty()) {
        writer.writeChannelLastBuildDate(posts.first()->created());
    }

    Q_FOREACH (CMS::Page *post, posts) {
        writer.writeStartItem();
        writer.writeItemTitle(post->name());
        QString link = c->uriFor(post->path()).toString();
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

    res->setBody(buffer);
}
