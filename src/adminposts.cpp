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

#include "../libCMS/fileengine.h"
#include "../libCMS/page.h"

#include <QDebug>

AdminPosts::AdminPosts(QObject *parent) :
    Controller(parent)
{
}

void AdminPosts::index(Context *ctx)
{
    ctx->stash()["post_type"] = "post";    

    CMS::FileEngine *engine = new CMS::FileEngine(ctx);
    engine->init({
                     {"root", ctx->config("DataLocation").toString()}
                 });
    QList<CMS::Page *> pages = engine->listPosts();
    ctx->stash()["posts"] = QVariant::fromValue(pages);

    ctx->stash()["template"] = "posts/index.html";
}

void AdminPosts::create(Context *ctx)
{
    ctx->stash()["post_type"] = "post";
    qDebug() << Q_FUNC_INFO;
    ParamsMultiMap params = ctx->request()->bodyParam();
    QString title = params.value("title");
    QString path = params.value("path");
    QString content = params.value("content");
    if (ctx->req()->method() == "POST") {
        Authentication *auth = ctx->plugin<Authentication*>();

        qDebug() << title;
        qDebug() << path;
        qDebug() << content;

        QString savePath = path;
        if (savePath.isEmpty()) {
            savePath = title;
        }
        savePath.prepend(QDate::currentDate().toString("yyyy/MM/dd/"));
        qDebug() << "save path"  << savePath;

        CMS::FileEngine *engine = new CMS::FileEngine(ctx);
        engine->init({
                         {"root", ctx->config("DataLocation").toString()}
                     });

        CMS::Page *page = engine->getPageToEdit(savePath);
        page->setContent(content.toUtf8());
        page->setName(title);
        page->setBlog(true);
        qDebug() << page->path();

        bool ret = engine->savePage(page);
        if (ret) {
            ctx->stash()["status"] = "Page saved";
        }

        qDebug() << "saved" << ret;
    }

    // TODO this is hackish...
    ctx->stash()["date"] = QDate::currentDate().toString("yyyy/MM/dd");

    ctx->stash()["title"] = title;
    ctx->stash()["path"] = path;
    ctx->stash()["content"] = content;
    ctx->stash()["template"] = "posts/create.html";
}
