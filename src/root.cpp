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

void Root::init(Application *app)
{
    QDir dataDir = app->config("DataLocation").toString();
    m_rootDir = app->config("RootLocation").toString();

    CMS::FileEngine *engine = new CMS::FileEngine(this);
    engine->init({
                     {"root", dataDir.absolutePath()}
                 });
    m_engine = engine;
}

void Root::page(Cutelyst::Context *ctx)
{
//    qDebug() << "*** Root::page()";
//    qDebug() << "*** Root::page()" << ctx->req()->path() << ctx->req()->base();

    Response *res = ctx->res();
    Request *req = ctx->req();

    // Find the desired page
    CMS::Page *page = m_engine->getPage(req->path());
    if (!page) {
        ctx->stash({
                       {QStringLiteral("template"), QStringLiteral("404.html")},
                       {QStringLiteral("cms"), QVariant::fromValue(m_engine->settings())}
                   });
        ctx->stash()[QLatin1String("template")] = "404.html";
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

    ctx->stash({
                   {QStringLiteral("template"), page->blog() ? QStringLiteral("blog.html") : QStringLiteral("page.html")},
                   {QStringLiteral("cms"), QVariant::fromValue(m_engine->settings())},
                   {QStringLiteral("menus"), QVariant::fromValue(m_engine->menuLocations())},
                   {QStringLiteral("page"), QVariant::fromValue(page)}
               });
}
