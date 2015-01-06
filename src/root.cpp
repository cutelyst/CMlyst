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
#include <Cutelyst/Plugins/viewengine.h>

#include <QStringBuilder>
#include <QSettings>
#include <QDir>
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
    qDebug() << "*** Root::End()" << ctx->view();

    QDir rootDir = ctx->config("RootLocation").toString();
    QDir dataDir = ctx->config("DataLocation").toString();
    QSettings settings(dataDir.absoluteFilePath("site.conf"), QSettings::IniFormat);
    settings.beginGroup("General");

    QString themePath;
    themePath = rootDir.absoluteFilePath(QLatin1String("themes/") % settings.value("theme", "default").toString());

    ViewEngine *view = qobject_cast<ViewEngine*>(ctx->view());
    // Check if theme path changed
    if (view->includePath() != themePath) {
        view->setIncludePath(themePath);
    }

    ctx->stash()["basetheme"] = ctx->uriFor("/static/" + settings.value("theme", "default").toString().toLatin1()).toString();
}

void Root::page(Cutelyst::Context *ctx)
{
    qDebug() << "*** Root::page()";
    qDebug() << "*** Root::page()" << ctx->req()->path() << ctx->req()->base();

    QDir dataDir = ctx->config("DataLocation").toString();
    CMS::FileEngine *engine = new CMS::FileEngine;
    engine->init({
                     {"root", dataDir.absolutePath()}
                 });

    QString path;

    path = ctx->request()->path();
    if (path.at(0) == QChar('/')) {
        path.remove(0, 1);
    }
    qDebug() << "path" << path << dataDir.absolutePath();

    QList<CMS::Page *> toppages = engine->listPages(0);

    QSettings settings(dataDir.absoluteFilePath("site.conf"), QSettings::IniFormat);
    settings.beginGroup("General");
    ctx->stash({
                   {"title", settings.value("title")},
                   {"tagline", settings.value("tagline")},
                   {"toppages", QVariant::fromValue(toppages)}
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
                   {"template", "page.html"},
                   {"page", QVariant::fromValue(page)}
               });
}
