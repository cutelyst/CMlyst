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

#include "adminposts.h"

#include "root.h"

#include <Cutelyst/Plugins/authentication.h>
#include <Cutelyst/Application>

#include "../libCMS/page.h"

#include <QDebug>

AdminPosts::AdminPosts(QObject *parent) :
    Controller(parent)
{
}

void AdminPosts::index(Context *ctx)
{
    ctx->stash()["post_type"] = "post";    

    QList<CMS::Page *> pages = engine->listPages(CMS::Engine::Posts);

    ctx->stash()["posts"] = QVariant::fromValue(pages);

    ctx->stash()["template"] = "posts/index.html";
}

void AdminPosts::create(Context *ctx)
{
    ctx->stash()["post_type"] = "post";
    qDebug() << Q_FUNC_INFO;
    ParamsMultiMap params = ctx->request()->bodyParams();
    QString title = params.value("title");
    QString path = params.value("path");
    QString content = params.value("edit-content");
    if (ctx->req()->method() == "POST") {
        QString savePath;
        if (path.isEmpty()) {
            savePath = CMS::Engine::normalizeTitle(title);
        } else {
            savePath = CMS::Engine::normalizeTitle(path);
        }
        savePath.prepend(QDate::currentDate().toString("yyyy/MM/dd/"));
//        qDebug() << "save path"  << savePath;

        CMS::Page *page = engine->getPageToEdit(savePath);
        page->setContent(content.toUtf8());
        page->setName(title);
        page->setBlog(true);
//        qDebug() << page->path();

        bool ret = engine->savePage(page);
        if (ret) {
            ctx->res()->redirect(ctx->uriFor(actionFor("index")));
        } else {
            qDebug() << "Failed to save page" << savePath;
            ctx->stash()["error_msg"] = tr("Failed to save page");
        }

//        qDebug() << "saved" << ret;
    }

    // TODO this is hackish...
    ctx->stash()["date"] = QDate::currentDate().toString("yyyy/MM/dd");

    ctx->stash()["title"] = title;
    ctx->stash()["path"] = path;
    ctx->stash()["edit_content"] = content;
    ctx->stash()["template"] = "posts/create.html";
}

void AdminPosts::edit(Context *ctx, const QStringList &args)
{
    qDebug() << Q_FUNC_INFO;
    ctx->stash()["post_type"] = "post";

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
        ParamsMultiMap params = ctx->request()->bodyParams();
        title = params.value("title");
        content = params.value("edit-content");

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
            ctx->res()->redirect(ctx->uriFor(actionFor("index")));
        } else {
            qDebug() << "Failed to save page" << page;
            ctx->stash()["error_msg"] = tr("Failed to save page");
        }

//        qDebug() << "saved" << ret;
    }

    ctx->stash()["title"] = title;
    ctx->stash()["path"] = path;
    ctx->stash()["edit_content"] = content;
    ctx->stash()["template"] = "posts/create.html";
}
