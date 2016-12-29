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
    c->setStash(QStringLiteral("post_type"), QStringLiteral("post"));

    QList<CMS::Page *> pages = engine->listPages(CMS::Engine::Posts);

    c->setStash(QStringLiteral("posts"), QVariant::fromValue(pages));

    c->setStash(QStringLiteral("template"), QStringLiteral("posts/index.html"));
}

void AdminPosts::create(Context *c)
{
    c->setStash(QStringLiteral("post_type"), QStringLiteral("post"));
    qDebug() << Q_FUNC_INFO;
    ParamsMultiMap params = c->request()->bodyParams();
    QString title = params.value(QStringLiteral("title"));
    QString path = params.value(QStringLiteral("path"));
    QString content = params.value(QStringLiteral("edit-content"));
    if (c->req()->isPost()) {
        QString savePath;
        if (path.isEmpty()) {
            savePath = CMS::Engine::normalizeTitle(title);
        } else {
            savePath = CMS::Engine::normalizeTitle(path);
        }
        savePath.prepend(QDate::currentDate().toString(QStringLiteral("yyyy/MM/dd/")));
//        qDebug() << "save path"  << savePath;

        CMS::Page *page = engine->getPageToEdit(savePath);
        page->setContent(content);
        page->setName(title);
        page->setBlog(true);
//        qDebug() << page->path();

        bool ret = engine->savePage(page);
        if (ret) {
            c->res()->redirect(c->uriFor(CActionFor(QStringLiteral("index"))));
        } else {
            qDebug() << "Failed to save page" << savePath;
            c->setStash(QStringLiteral("error_msg"), tr("Failed to save page"));
        }

//        qDebug() << "saved" << ret;
    }

    // TODO this is hackish...
    c->setStash(QStringLiteral("date"), QDate::currentDate().toString(QStringLiteral("yyyy/MM/dd")));

    c->setStash(QStringLiteral("title"), title);
    c->setStash(QStringLiteral("path"), path);
    c->setStash(QStringLiteral("edit_content"), content);
    c->setStash(QStringLiteral("template"), QStringLiteral("posts/create.html"));
}

void AdminPosts::edit(Context *c, const QStringList &args)
{
    qDebug() << Q_FUNC_INFO;
    c->setStash(QStringLiteral("post_type"), QStringLiteral("post"));

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

    if (c->req()->isPost()) {
        ParamsMultiMap params = c->request()->bodyParams();
        title = params.value(QStringLiteral("title"));
        content = params.value(QStringLiteral("edit-content"));

//        qDebug() << title;
//        qDebug() << path;
//        qDebug() << content;


        if (page->path() != params.value(QStringLiteral("path"))) {
            qDebug() << "not yet supported";
        }

        page->setContent(content);
        page->setName(title);
//        qDebug() << page->path();

        bool ret = engine->savePage(page);
        if (ret) {
            c->res()->redirect(c->uriFor(CActionFor(QStringLiteral("index"))));
        } else {
            qDebug() << "Failed to save page" << page;
            c->setStash(QStringLiteral("error_msg"), tr("Failed to save page"));
        }

//        qDebug() << "saved" << ret;
    }

    c->setStash(QStringLiteral("title"), title);
    c->setStash(QStringLiteral("path"), path);
    c->setStash(QStringLiteral("edit_content"), content);
    c->setStash(QStringLiteral("template"), QStringLiteral("posts/create.html"));
}
