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

#include <QSettings>
#include <QDir>

AdminSettings::AdminSettings(QObject *parent) :
    Controller(parent)
{

}

void AdminSettings::index(Context *ctx)
{
    QDir dataDir = ctx->config("DataLocation").toString();
    QSettings settings(dataDir.absoluteFilePath("site.conf"), QSettings::IniFormat);
    settings.beginGroup("General");

    if (ctx->req()->method() == "POST") {
        ParamsMultiMap params = ctx->request()->bodyParam();
        settings.setValue("title", params.value("title"));
        settings.setValue("tagline", params.value("tagline"));
        settings.setValue("theme", params.value("theme"));
    }

    QDir rootDir = ctx->config("RootLocation").toString();
    QDir themesDir = rootDir.absoluteFilePath("themes");
    QStringList themes = themesDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot,
                                             QDir::Name | QDir:: IgnoreCase);

    ctx->stash({
                   {"template", "settings/index.html"},
                   {"title", settings.value("title")},
                   {"tagline", settings.value("tagline")},
                   {"currentTheme", settings.value("theme")},
                   {"themes", themes}
               });



    settings.endGroup();
}
