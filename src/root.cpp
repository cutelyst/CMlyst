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

#include "root.h"

#include <Cutelyst/Context>
#include <Cutelyst/Plugins/authentication.h>

#include <QStringBuilder>
#include <QSettings>
#include <QDebug>

#include "../libCMS/fileengine.h"
#include "../libCMS/page.h"

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
    qDebug() << "*** Root::End()";
}

void Root::page(Cutelyst::Context *ctx)
{
    qDebug() << "*** Root::page()";

    CMS::FileEngine *engine = new CMS::FileEngine;
    engine->init({
                     {"root", qgetenv("CMS_ROOT_PATH")}
                 });

    QString path;

    path = ctx->request()->path();
    if (path.at(0) == QChar('/')) {
        path.remove(0, 1);
    }
    qDebug() << "path" << path;

    QSettings settings("site.conf", QSettings::IniFormat);
    settings.beginGroup("General");
    ctx->stash({
                   {"title", settings.value("title")},
                   {"tagline", settings.value("tagline")}
               });
    settings.endGroup();

    CMS::Page *page = engine->getPage(path);
    qDebug() << "page" << page;
    if (!page) {
        ctx->stash()[QLatin1String("template")] = "404.html";
        ctx->res()->setStatus(404);
        delete engine;
        return;
    }

    ctx->stash({
                   {"title", page->name()},
                   {"template", "page.html"},
                   {"page", QVariant::fromValue(page)}
               });
}
