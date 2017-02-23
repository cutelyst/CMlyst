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

#include "libCMS/page.h"

#include <Cutelyst/Application>
#include <Cutelyst/Plugins/Utils/Sql>

#include <QSqlQuery>
#include <QDir>
#include <QDebug>

AdminSettings::AdminSettings(Application *app) : Controller(app)
{

}

void AdminSettings::general(Context *c)
{
    if (!engine->settingsIsWritable()) {
        c->stash({
                       {QStringLiteral("error_msg"), QStringLiteral("Settings file is read only!")}
                   });
    }

    if (c->req()->isPost()) {
        ParamsMultiMap params = c->request()->bodyParams();
        qDebug() << params;
        engine->setSettingsValue(QStringLiteral("title"), params.value(QStringLiteral("title")));
        engine->setSettingsValue(QStringLiteral("tagline"), params.value(QStringLiteral("tagline")));
        engine->setSettingsValue(QStringLiteral("theme"), params.value(QStringLiteral("theme")));
        engine->setSettingsValue(QStringLiteral("show_on_front"), params.value(QStringLiteral("show_on_front")));
        engine->setSettingsValue(QStringLiteral("page_on_front"), params.value(QStringLiteral("page_on_front")));
        engine->setSettingsValue(QStringLiteral("page_for_posts"), params.value(QStringLiteral("page_for_posts")));
    }

    QDir themesDir = c->app()->pathTo({ QStringLiteral("root"), QStringLiteral("themes") });
    QStringList themes = themesDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot,
                                             QDir::Name | QDir:: IgnoreCase);


    QList<CMS::Page *> pages = engine->listPages(c,
                                                 CMS::Engine::Filters(
                                                     CMS::Engine::Pages |
                                                     CMS::Engine::OnlyPublished));

    c->stash({
                   {QStringLiteral("template"), QStringLiteral("settings/general.html")},
                   {QStringLiteral("title"), engine->settingsValue(QStringLiteral("title"))},
                   {QStringLiteral("tagline"), engine->settingsValue(QStringLiteral("tagline"))},
                   {QStringLiteral("currentTheme"), engine->settingsValue(QStringLiteral("theme"))},
                   {QStringLiteral("themes"), themes},
                   {QStringLiteral("pages"), QVariant::fromValue(pages)},
                   {QStringLiteral("show_on_front"), engine->settingsValue(QStringLiteral("show_on_front"), QStringLiteral("posts"))},
                   {QStringLiteral("page_on_front"), engine->settingsValue(QStringLiteral("page_on_front"))},
                   {QStringLiteral("page_for_posts"), engine->settingsValue(QStringLiteral("page_for_posts"))},
             });
}

void AdminSettings::code_injection(Context *c)
{
    if (c->req()->isPost()) {
        ParamsMultiMap params = c->request()->bodyParams();
        engine->setSettingsValue(QStringLiteral("cms_head"), params.value(QStringLiteral("cms_head")));
        engine->setSettingsValue(QStringLiteral("cms_foot"), params.value(QStringLiteral("cms_foot")));
    }

    c->stash({
                   {QStringLiteral("template"), QStringLiteral("settings/code_injection.html")},
                   {QStringLiteral("cms_head"), engine->settingsValue(QStringLiteral("cms_head"))},
                   {QStringLiteral("cms_foot"), engine->settingsValue(QStringLiteral("cms_foot"))},
                   {QStringLiteral("currentTheme"), engine->settingsValue(QStringLiteral("theme"))},
             });
}

void AdminSettings::users(Context *c)
{
    QSqlQuery query = CPreparedSqlQueryThreadForDB(QStringLiteral("SELECT id, name, email "
                                                                  "FROM users "
                                                                  "ORDER BY name"),
                                                   QStringLiteral("cmlyst"));
    if (query.exec()) {
        c->setStash(QStringLiteral("users"), Sql::queryToHashList(query));
    }

    c->setStash(QStringLiteral("template"), QStringLiteral("settings/users.html"));
}

void AdminSettings::users_edit(Context *c, const QString &id)
{
    QSqlQuery query = CPreparedSqlQueryThreadForDB(QStringLiteral("SELECT id, name, email, json "
                                                                  "FROM users "
                                                                  "WHERE id = :id "
                                                                  "ORDER BY name"),
                                                   QStringLiteral("cmlyst"));
    query.bindValue(QStringLiteral(":id"), id);
    if (query.exec()) {
        c->setStash(QStringLiteral("user"), Sql::queryToHashObject(query));
    }

    c->setStash(QStringLiteral("template"), QStringLiteral("settings/user.html"));
}

void AdminSettings::users_new(Context *c)
{

}
