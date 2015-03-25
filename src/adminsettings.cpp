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

#include "adminsettings.h"

#include "../libCMS/page.h"

#include <Cutelyst/Application>

#include <QDir>

AdminSettings::AdminSettings(QObject *parent) :
    Controller(parent)
{

}

void AdminSettings::index(Context *ctx)
{
    if (!engine->settingsIsWritable()) {
        ctx->stash({
                       {"error_msg", "Settings file is read only!"}
                   });
    }

    if (ctx->req()->method() == "POST") {
        ParamsMultiMap params = ctx->request()->bodyParam();
        qDebug() << params;
        engine->setSettingsValue("title", params.value("title"));
        engine->setSettingsValue("tagline", params.value("tagline"));
        engine->setSettingsValue("theme", params.value("theme"));
        engine->setSettingsValue("show_on_front", params.value("show_on_front"));
    }

    QDir themesDir = ctx->app()->pathTo({ "root", "themes" });
    QStringList themes = themesDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot,
                                             QDir::Name | QDir:: IgnoreCase);


    QList<CMS::Page *> pages = engine->listPages(CMS::Engine::Filters(
                                                     CMS::Engine::Pages |
                                                     CMS::Engine::OnlyPublished));

    ctx->stash({
                   {"template", "settings/index.html"},
                   {"title", engine->settingsValue("title")},
                   {"tagline", engine->settingsValue("tagline")},
                   {"currentTheme", engine->settingsValue("theme")},
                   {"themes", themes},
                   {"pages", QVariant::fromValue(pages)},
                   {"show_on_front", engine->settingsValue("show_on_front", "posts")},
               });
}
