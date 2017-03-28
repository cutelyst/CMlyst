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
#include <Cutelyst/Plugins/Authentication/authentication.h>
#include <Cutelyst/Plugins/Authentication/credentialpassword.h>
#include <Cutelyst/Plugins/StatusMessage>

#include <QRegularExpression>

#include <QJsonDocument>
#include <QJsonObject>

#include <QSqlQuery>
#include <QSqlError>

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
        engine->setSettingsValue(c, QStringLiteral("title"), params.value(QStringLiteral("title")));
        engine->setSettingsValue(c, QStringLiteral("tagline"), params.value(QStringLiteral("tagline")));
        engine->setSettingsValue(c, QStringLiteral("theme"), params.value(QStringLiteral("theme")));
        engine->setSettingsValue(c, QStringLiteral("show_on_front"), params.value(QStringLiteral("show_on_front")));
        engine->setSettingsValue(c, QStringLiteral("page_on_front"), params.value(QStringLiteral("page_on_front")));
        engine->setSettingsValue(c, QStringLiteral("page_for_posts"), params.value(QStringLiteral("page_for_posts")));
    }

    QDir themesDir = c->app()->pathTo({ QStringLiteral("root"), QStringLiteral("themes") });
    QStringList themes = themesDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot,
                                             QDir::Name | QDir:: IgnoreCase);


    QList<CMS::Page *> pages = engine->listPages(c,
                                                 CMS::Engine::Filters(
                                                     CMS::Engine::Pages |
                                                     CMS::Engine::OnlyPublished));
    auto settings = engine->settings();
    c->stash({
                   {QStringLiteral("template"), QStringLiteral("settings/general.html")},
                   {QStringLiteral("title"), settings.value(QStringLiteral("title"))},
                   {QStringLiteral("tagline"), settings.value(QStringLiteral("tagline"))},
                   {QStringLiteral("currentTheme"), settings.value(QStringLiteral("theme"))},
                   {QStringLiteral("themes"), themes},
                   {QStringLiteral("pages"), QVariant::fromValue(pages)},
                   {QStringLiteral("show_on_front"), settings.value(QStringLiteral("show_on_front"), QStringLiteral("posts"))},
                   {QStringLiteral("page_on_front"), settings.value(QStringLiteral("page_on_front"))},
                   {QStringLiteral("page_for_posts"), settings.value(QStringLiteral("page_for_posts"))},
             });
}

void AdminSettings::code_injection(Context *c)
{
    if (c->req()->isPost()) {
        ParamsMultiMap params = c->request()->bodyParams();
        engine->setSettingsValue(c, QStringLiteral("cms_head"), params.value(QStringLiteral("cms_head")));
        engine->setSettingsValue(c, QStringLiteral("cms_foot"), params.value(QStringLiteral("cms_foot")));
    }

    auto settings = engine->settings();
    c->stash({
                   {QStringLiteral("template"), QStringLiteral("settings/code_injection.html")},
                   {QStringLiteral("cms_head"), settings.value(QStringLiteral("cms_head"))},
                   {QStringLiteral("cms_foot"), settings.value(QStringLiteral("cms_foot"))},
                   {QStringLiteral("currentTheme"), settings.value(QStringLiteral("theme"))},
             });
}

void AdminSettings::users(Context *c)
{
    c->setStash(QStringLiteral("users"), engine->users());
    c->setStash(QStringLiteral("template"), QStringLiteral("settings/users.html"));
}

void AdminSettings::user(Context *c, const QString &id)
{
    c->setStash(QStringLiteral("template"), QStringLiteral("settings/user.html"));

    c->setStash(QStringLiteral("author_url"), c->uriFor(QStringLiteral("/.author")));

    if (c->request()->isPost()) {
        const ParamsMultiMap params = c->request()->bodyParameters();
        if (params.contains(QStringLiteral("submit"))) {
            changePassword(c, id, params);
        } else {
            updateUserData(c, id, params);
        }
    } else {
        c->setStash(QStringLiteral("user"), QVariant::fromValue(engine->user(id)));
    }
}

void AdminSettings::updateUserData(Context *c, const QString &id, const ParamsMultiMap params)
{
    QJsonObject obj;
    obj.insert(QStringLiteral("location"),
               params.value(QStringLiteral("location")).left(100).toHtmlEscaped());
    obj.insert(QStringLiteral("facebook"),
               params.value(QStringLiteral("facebook")).left(100).toHtmlEscaped());
    obj.insert(QStringLiteral("twitter"),
               params.value(QStringLiteral("twitter")).left(100).toHtmlEscaped());
    obj.insert(QStringLiteral("website"),
               params.value(QStringLiteral("website")).left(100).toHtmlEscaped());
    QString name = params.value(QStringLiteral("name"));
    obj.insert(QStringLiteral("name"),
               name.left(150).toHtmlEscaped());
    obj.insert(QStringLiteral("bio"),
               params.value(QStringLiteral("bio")).left(200).toHtmlEscaped());

    QSqlQuery query = CPreparedSqlQueryThreadForDB(QStringLiteral("UPDATE users SET "
                                                                  "slug = :slug, "
                                                                  "email = :email, "
                                                                  "json = :json "
                                                                  "WHERE slug = :slug_id "),
                                                   QStringLiteral("cmlyst"));
    query.bindValue(QStringLiteral(":slug_id"), id);
    QString slug = params.value(QStringLiteral("slug"));
    if (slug.isEmpty()) {
        slug  = name.section(QLatin1Char(' '), 0, 0);
    }
    slug.remove(QRegularExpression(QStringLiteral("[^\\w]")));
    slug = slug.left(50).toLower().toHtmlEscaped();
    query.bindValue(QStringLiteral(":slug"), slug);
    query.bindValue(QStringLiteral(":email"), params.value(QStringLiteral("email")).left(200).toHtmlEscaped());
    query.bindValue(QStringLiteral(":json"), QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
    if (query.exec()) {
        engine->setSettingsValue(c, QStringLiteral("modified"), QString());
        c->response()->redirect(c->uriFor(CActionFor(QStringLiteral("user")), { slug }));
        return;
    }
    c->setStash(QStringLiteral("user"), QVariant::fromValue(params));
}

void AdminSettings::changePassword(Context *c, const QString &id, const ParamsMultiMap params)
{
    const QString oldPass = params.value(QStringLiteral("pw_old"));
    const QString newPass = params.value(QStringLiteral("pw_new"));
    const QString newPass2 = params.value(QStringLiteral("pw_new_conf"));
    if (newPass == newPass2) {
        if (newPass.size() < 8) {
            c->setStash(QStringLiteral("error_msg"), QStringLiteral("Your password needs to be at least 8 characters long"));
        } else {
            const AuthenticationUser user = Authentication::user(c);

            const QString oldHash = user.value(QStringLiteral("password")).toString();
            if (!CredentialPassword::validatePassword(oldPass.toUtf8(), oldHash.toLatin1())) {
                c->setStash(QStringLiteral("error_msg"), QStringLiteral("Old password does not match"));
                return;
            }

            const QString hashedPassword = QString::fromLatin1(CredentialPassword::createPassword(
                                                                   newPass.toUtf8(),
                                                                   QCryptographicHash::Sha256,
                                                                   100, 24, 24));

            QSqlQuery query = CPreparedSqlQueryThreadForDB(
                        QStringLiteral("UPDATE users SET password = :password "
                                       "WHERE id = :id AND password = :oldpw "),
                        QStringLiteral("cmlyst"));
            query.bindValue(QStringLiteral(":id"), user.id());
            query.bindValue(QStringLiteral(":password"), hashedPassword);
            query.bindValue(QStringLiteral(":oldpw"), oldHash);

            if (query.exec() && query.numRowsAffected() == 1) {
                Authentication::logout(c);
                c->response()->redirect(c->uriFor(QStringLiteral("/.admin/login"),
                                                  StatusMessage::statusQuery(c, QStringLiteral("Password updated"))));
            } else {
                c->setStash(QStringLiteral("error_msg"), query.lastError().text());
            }
        }
    } else {
        c->setStash(QStringLiteral("error_msg"), QStringLiteral("Your new passwords do not match"));
    }
}

void AdminSettings::users_edit(Context *c, const QString &id)
{
    c->response()->redirect(c->uriFor(CActionFor(QStringLiteral("user")), QStringList{ id }));
}

void AdminSettings::users_new(Context *c)
{

}
