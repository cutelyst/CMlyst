/***************************************************************************
 *   Copyright (C) 2014 Daniel Nicoletti <dantti12@gmail.com>              *
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

#include "adminpages.h"

#include <Cutelyst/Plugins/authentication.h>
#include <Cutelyst/Application>

#include "../libCMS/fileengine.h"
#include "../libCMS/page.h"

AdminPages::AdminPages()
{

}

AdminPages::~AdminPages()
{

}

void AdminPages::index(Context *ctx)
{
    ctx->stash()["post_type"] = "page";

    CMS::FileEngine *engine = new CMS::FileEngine(ctx);
    engine->init({
                     {"root", ctx->config("DataLocation").toString()}
                 });
    QList<CMS::Page *> pages = engine->listPages(CMS::Engine::Pages);
    ctx->stash()["posts"] = QVariant::fromValue(pages);

    ctx->stash()["template"] = "posts/index.html";
}

void AdminPages::create(Context *ctx)
{
    qDebug() << Q_FUNC_INFO;
    ctx->stash()["post_type"] = "page";

    ParamsMultiMap params = ctx->request()->bodyParam();
    QString title = params.value("title");
    QString path = params.value("path");
    QString content = params.value("content");
    if (ctx->req()->method() == "POST") {
//        qDebug() << title;
//        qDebug() << path;
//        qDebug() << content;


        CMS::FileEngine *engine = new CMS::FileEngine(ctx);
        engine->init({
                         {"root", ctx->config("DataLocation").toString()}
                     });

        CMS::Page *page = engine->getPageToEdit(path);
        page->setContent(content);
        page->setName(title);
//        qDebug() << page->path();

        bool ret = engine->savePage(page);
        if (ret) {
            ctx->stash()["status"] = "Page saved";
        }

//        qDebug() << "saved" << ret;
    }

    ctx->stash()["title"] = title;
    ctx->stash()["path"] = path;
    ctx->stash()["content"] = content;
    ctx->stash()["template"] = "posts/create.html";
}

void AdminPages::edit(Context *ctx)
{
    qDebug() << Q_FUNC_INFO;
    ctx->stash()["post_type"] = "page";

    CMS::FileEngine *engine = new CMS::FileEngine(ctx);
    engine->init({
                     {"root", ctx->config("DataLocation").toString()}
                 });

    QStringList args = ctx->request()->args();
    QString path = args.join(QLatin1Char('/'));
    QString title;
    QString content;

//    qDebug() << Q_FUNC_INFO << path <<  ctx->request()->args();
    CMS::Page *page = engine->getPageToEdit(path);
//    qDebug() << Q_FUNC_INFO << page;

    if (page) {
        path = page->path();
        title = page->name();
        content = page->content();
    }

    if (ctx->req()->method() == "POST") {
        ParamsMultiMap params = ctx->request()->bodyParam();
        title = params.value("title");
        content = params.value("content");

//        qDebug() << title;
//        qDebug() << path;
//        qDebug() << content;


        if (page->path() != params.value("path")) {
            qDebug() << "not yet supported";
        }

        page->setContent(content);
        page->setName(title);
//        qDebug() << page->path();

        bool ret = engine->savePage(page);
        if (ret) {
            ctx->stash()["status"] = "Page saved";
        }

//        qDebug() << "saved" << ret;
    }

    ctx->stash()["title"] = title;
    ctx->stash()["path"] = path;
    ctx->stash()["content"] = content;
    ctx->stash()["template"] = "posts/create.html";
}

