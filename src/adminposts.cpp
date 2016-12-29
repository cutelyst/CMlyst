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

#include <Cutelyst/Plugins/Authentication/authentication.h>
#include <Cutelyst/Application>

#include "libCMS/page.h"

#include <QDebug>

AdminPosts::AdminPosts(Application *app) : Controller(app)
{
}

void AdminPosts::index(Context *c)
{
    c->stash()["post_type"] = "post";

    QList<CMS::Page *> pages = engine->listPages(CMS::Engine::Posts);

    c->stash()["posts"] = QVariant::fromValue(pages);

    c->stash()["template"] = "posts/index.html";
}

void AdminPosts::create(Context *c)
{
    c->stash()["post_type"] = "post";
    qDebug() << Q_FUNC_INFO;
    ParamsMultiMap params = c->request()->bodyParams();
    QString title = params.value("title");
    QString path = params.value("path");
    QString content = params.value("edit-content");
    if (c->req()->method() == "POST") {
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
            c->res()->redirect(c->uriFor(actionFor("index")));
        } else {
            qDebug() << "Failed to save page" << savePath;
            c->stash()["error_msg"] = tr("Failed to save page");
        }

//        qDebug() << "saved" << ret;
    }

    // TODO this is hackish...
    c->stash()["date"] = QDate::currentDate().toString("yyyy/MM/dd");

    c->stash()["title"] = title;
    c->stash()["path"] = path;
    c->stash()["edit_content"] = content;
    c->stash()["template"] = "posts/create.html";
}

void AdminPosts::edit(Context *c, const QStringList &args)
{
    qDebug() << Q_FUNC_INFO;
    c->stash()["post_type"] = "post";

    QString path = args.join(QLatin1Char('/'));
    QString title;
    QString content;

//    qDebug() << Q_FUNC_INFO << path <<  c->request()->args();
    CMS::Page *page = engine->getPageToEdit(path);
//    qDebug() << Q_FUNC_INFO << page;

    if (page) {
        path = page->path();
        title = page->name();
        content = page->content();
    }

    if (c->req()->method() == "POST") {
        ParamsMultiMap params = c->request()->bodyParams();
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
            c->res()->redirect(c->uriFor(actionFor("index")));
        } else {
            qDebug() << "Failed to save page" << page;
            c->stash()["error_msg"] = tr("Failed to save page");
        }

//        qDebug() << "saved" << ret;
    }

    c->stash()["title"] = title;
    c->stash()["path"] = path;
    c->stash()["edit_content"] = content;
    c->stash()["template"] = "posts/create.html";
}
