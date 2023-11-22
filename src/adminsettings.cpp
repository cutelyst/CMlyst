/***************************************************************************
 *   Copyright (C) 2014-2017 Daniel Nicoletti <dantti12@gmail.com>         *
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
#include <Cutelyst/Upload>
#include <Cutelyst/Plugins/Utils/Sql>
#include <Cutelyst/Plugins/Authentication/credentialpassword.h>
#include <Cutelyst/Plugins/StatusMessage>

#include <QRegularExpression>
#include <QTimeZone>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

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
        engine->setSettingsValue(c, QStringLiteral("timezone"), params.value(QStringLiteral("timezone")));
        engine->setSettingsValue(c, QStringLiteral("posts_per_page"), params.value(QStringLiteral("posts_per_page")));
    }

    QStringList timezones;
    const auto rawTzs = QTimeZone::availableTimeZoneIds();
    for (const QByteArray &rawTz : rawTzs) {
        timezones.push_back(QString::fromUtf8(rawTz));
    }

    QDir themesDir = c->app()->pathTo(QStringLiteral("root/themes"));
    QStringList themes = themesDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot,
                                             QDir::Name | QDir:: IgnoreCase);


    const QList<CMS::Page *> pages = engine->listPagesPublished(c, -1, -1);
    auto settings = engine->settings();
    c->stash({
                 {QStringLiteral("template"), QStringLiteral("settings/general.html")},
                 {QStringLiteral("title"), settings.value(QStringLiteral("title"))},
                 {QStringLiteral("tagline"), settings.value(QStringLiteral("tagline"))},
                 {QStringLiteral("currentTheme"), settings.value(QStringLiteral("theme"))},
                 {QStringLiteral("currentTimezone"), settings.value(QStringLiteral("timezone"))},
                 {QStringLiteral("themes"), themes},
                 {QStringLiteral("pages"), QVariant::fromValue(pages)},
                 {QStringLiteral("timezones"), timezones},
                 {QStringLiteral("show_on_front"), settings.value(QStringLiteral("show_on_front"), QStringLiteral("posts"))},
                 {QStringLiteral("page_on_front"), settings.value(QStringLiteral("page_on_front"))},
                 {QStringLiteral("page_for_posts"), settings.value(QStringLiteral("page_for_posts"))},
                 {QStringLiteral("posts_per_page"), settings.value(QStringLiteral("posts_per_page"), QStringLiteral("10"))},
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
    } else {
        c->setStash(QStringLiteral("user"), params);
    }
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

void AdminSettings::users_new(Context *c)
{
    c->setStash(QStringLiteral("template"), QStringLiteral("settings/user_new.html"));
    if (c->req()->isPost()) {
        const ParamsMultiMap params = c->req()->bodyParameters();
        const QString username = params.value(QStringLiteral("username"));
        const QString email = params.value(QStringLiteral("email"));
        QString password = params.value(QStringLiteral("password"));
        const QString password2 = params.value(QStringLiteral("password2"));
        c->setStash(QStringLiteral("usernname"), username);
        c->setStash(QStringLiteral("email"), email);

        if (password == password2) {
            if (password.size() >= 10) {
                password = QString::fromLatin1(CredentialPassword::createPassword(password.toUtf8(),
                                                                                  QCryptographicHash::Sha256,
                                                                                  1000, 24, 24));

                const QString slug = engine->addUser(c, {
                                                         {QStringLiteral("name"), username},
                                                         {QStringLiteral("email"), email},
                                                         {QStringLiteral("password"), password},
                                                     },
                                                     false);

                if (!slug.isEmpty()) {
                    c->response()->redirect(c->uriFor(CActionFor(QStringLiteral("user")), QStringList{ slug }));
                } else {
                    c->setStash(QStringLiteral("error_msg"), QStringLiteral("Failed to add user, check application logs"));
                }
            } else {
                c->setStash(QStringLiteral("error_msg"), QStringLiteral("Password must be longer than 10 characters"));
            }
        } else {
            c->setStash(QStringLiteral("error_msg"), QStringLiteral("The two password didn't match"));
        }
    }
}

void AdminSettings::users_delete(Context *c, const QString &id)
{
    if (c->request()->isPost()) {
        if (!engine->removeUser(c, id.toInt())) {
            c->response()->setStatus(Response::NoContent);
        }
    } else {
        c->response()->setStatus(Response::BadRequest);
    }
}

void AdminSettings::database(Context *c)
{
    c->setStash(QStringLiteral("users"), engine->users());
    c->setStash(QStringLiteral("template"), QStringLiteral("settings/database.html"));
}

void AdminSettings::json_data(Context *c)
{
    if (c->request()->isPost()) {
        json_import(c);
    } else {
        json_export(c);
    }
}

void AdminSettings::json_import(Context *c)
{
    Upload *json = c->request()->upload(QStringLiteral("json_file"));
    if (!json) {
        c->response()->redirect(c->uriFor(CActionFor(QStringLiteral("database")),
                                          StatusMessage::errorQuery(c, QStringLiteral("Failed to import, missing upload file."))));
        return;
    }

    QJsonParseError error;
    const QJsonDocument doc = QJsonDocument::fromJson(json->readAll(), &error);
    if (error.error) {
        c->response()->redirect(c->uriFor(CActionFor(QStringLiteral("database")),
                                          StatusMessage::errorQuery(c, QStringLiteral("Failed to import, parsing failed: '%1'.")
                                                                    .arg(error.errorString()))));
        return;
    }
    QJsonObject main = doc.object();
    if (main.isEmpty()) {
        c->response()->redirect(c->uriFor(CActionFor(QStringLiteral("database")),
                                          StatusMessage::errorQuery(c, QStringLiteral("Failed to import, empty object."))));
        return;
    }

    QJsonArray dbArray = main.value(QLatin1String("db")).toArray();
    if (dbArray.isEmpty()) {
        c->response()->redirect(c->uriFor(CActionFor(QStringLiteral("database")),
                                          StatusMessage::errorQuery(c, QStringLiteral("Failed to import, empty db array."))));
        return;
    }

    QJsonObject dbObject = dbArray.first().toObject();

    QJsonObject data = dbObject.value(QStringLiteral("data")).toObject();

    auto settingsIt = data.constFind(QStringLiteral("settings"));
    if (settingsIt != data.constEnd()) {
        for (const QJsonValue &jsonValue : settingsIt.value().toArray()) {
            QJsonObject settings = jsonValue.toObject();
            const QString key = settings.value(QStringLiteral("key")).toString();
            if (!key.isEmpty()) {
                const QString value = settings.value(QStringLiteral("value")).toString();
                engine->setSettingsValue(c, key, value);
            }
        }
    }

    auto usersIt = data.constFind(QLatin1String("users"));
    if (usersIt != data.constEnd()) {
        for (const QJsonValue &jsonValue : usersIt.value().toArray()) {
            QJsonObject user = jsonValue.toObject();
            QSqlQuery query = CPreparedSqlQueryThreadForDB(QStringLiteral("INSERT INTO users "
                                                                          "(slug, email, password, json) "
                                                                          "VALUES "
                                                                          "(:slug, :email, :password, :json)"),
                                                           QStringLiteral("cmlyst"));
            query.bindValue(QStringLiteral(":slug"), user.value(QStringLiteral("slug")).toString());
            user.remove(QStringLiteral("slug"));
            query.bindValue(QStringLiteral(":email"), user.value(QStringLiteral("email")).toString());
            user.remove(QStringLiteral("email"));
            query.bindValue(QStringLiteral(":password"), user.value(QStringLiteral("password")).toString());
            user.remove(QStringLiteral("password"));
            query.bindValue(QStringLiteral(":json"), QString::fromUtf8(QJsonDocument(user).toJson(QJsonDocument::Compact)));

            if (!query.exec()) {
                qWarning() << "Failed to import user" << query.lastError().databaseText();
            }
        }
    }

    auto postsIt = data.constFind(QLatin1String("posts"));
    if (postsIt != data.constEnd()) {
        for (const QJsonValue &jsonValue : postsIt.value().toArray()) {
            QJsonObject post = jsonValue.toObject();
            auto page = new CMS::Page(c);
            Author author;
            author.insert(QStringLiteral("id"), QString::number(post.value(QLatin1String("author_id")).toInt()));
            page->setAuthor(author);
            page->setContent(post.value(QStringLiteral("content")).toString(), true);
            page->setTitle(post.value(QStringLiteral("title")).toString());
            page->setUuid(post.value(QStringLiteral("uuid")).toString());
            if (post.contains(QStringLiteral("path"))) {
                page->setPath(post.value(QStringLiteral("path")).toString());
            } else {
                // Ghost compatibility
                page->setPath(post.value(QStringLiteral("slug")).toString());
            }
            page->setPage(post.value(QStringLiteral("page")).toBool());

            auto created = QDateTime::fromString(post.value(QStringLiteral("created_at")).toString(),
                                                 QStringLiteral("yyyy-MM-dd HH:mm:ss"));
            created.setTimeSpec(Qt::UTC);
            page->setCreated(created);

            auto updated = QDateTime::fromString(post.value(QStringLiteral("updated_at")).toString(),
                                                 QStringLiteral("yyyy-MM-dd HH:mm:ss"));
            updated.setTimeSpec(Qt::UTC);
            page->setUpdated(updated);

            auto published = QDateTime::fromString(post.value(QLatin1String("published_at")).toString(),
                                                   QStringLiteral("yyyy-MM-dd HH:mm:ss"));
            published.setTimeSpec(Qt::UTC);
            page->setPublishedAt(published);

            engine->savePage(c, page);
            delete page;
        }
    }

    c->response()->redirect(c->uriFor(CActionFor(QStringLiteral("database")),
                                      StatusMessage::statusQuery(c, QStringLiteral("Data successfully imported."))));
}

void AdminSettings::json_export(Context *c)
{
    QJsonObject data;

    const ParamsMultiMap params = c->request()->queryParams();

    QSqlQuery query;

    if (params.contains(QStringLiteral("posts"))) {
        query = CPreparedSqlQueryThreadForDB(
                    QStringLiteral("SELECT id, name, path, content, html, blog, author, created, modified "
                                   "FROM pages "
                                   ),
                    QStringLiteral("cmlyst"));
        if (query.exec()) {
            QJsonArray posts;
            while (query.next()) {
                QJsonObject post;
                post.insert(QStringLiteral("id"), query.value(QStringLiteral("id")).toLongLong());
                post.insert(QStringLiteral("title"), query.value(QStringLiteral("name")).toString());
                const QString path = query.value(QStringLiteral("path")).toString();
                if (path.isEmpty()) {
                    post.insert(QStringLiteral("slug"), QStringLiteral("index-slug"));
                } else {
                    post.insert(QStringLiteral("slug"), path);
                }
                post.insert(QStringLiteral("path"), path);
                post.insert(QStringLiteral("content"), query.value(QStringLiteral("content")).toString());
                post.insert(QStringLiteral("html"), query.value(QStringLiteral("html")).toString());
                post.insert(QStringLiteral("page"), !query.value(QStringLiteral("blog")).toBool());
                post.insert(QStringLiteral("author_id"), query.value(QStringLiteral("author")).toLongLong());
                const QDateTime modified = QDateTime::fromMSecsSinceEpoch(query.value(QStringLiteral("modified")).toLongLong() * 1000, Qt::UTC);
                post.insert(QStringLiteral("created_at"), modified.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")));
                post.insert(QStringLiteral("updated_at"), modified.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")));
                const QDateTime created = QDateTime::fromMSecsSinceEpoch(query.value(QStringLiteral("created")).toLongLong() * 1000, Qt::UTC);
                post.insert(QStringLiteral("published_at"), created.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")));

                posts.append(post);
            }
            data.insert(QStringLiteral("posts"), posts);
        }
    }

    if (params.contains(QStringLiteral("users"))) {
        query = CPreparedSqlQueryThreadForDB(
                    QStringLiteral("SELECT id, slug, email, password, json "
                                   "FROM users "
                                   ),
                    QStringLiteral("cmlyst"));
        if (query.exec()) {
            QJsonArray users;
            while (query.next()) {
                QJsonDocument doc = QJsonDocument::fromJson(query.value(QStringLiteral("json")).toString().toUtf8());
                QJsonObject user = doc.object();
                user.insert(QStringLiteral("id"), query.value(QStringLiteral("id")).toLongLong());
                user.insert(QStringLiteral("slug"), query.value(QStringLiteral("slug")).toString());
                user.insert(QStringLiteral("email"), query.value(QStringLiteral("email")).toString());
                user.insert(QStringLiteral("password"), query.value(QStringLiteral("password")).toString());

                users.append(user);
            }
            data.insert(QStringLiteral("users"), users);
        }
    }

    if (params.contains(QStringLiteral("settings"))) {
        QJsonArray settingsArray;
        auto settings = engine->settings();
        auto settingsIt = settings.constBegin();
        while (settingsIt != settings.constEnd()) {
            QJsonObject pair;
            pair.insert(QStringLiteral("key"), settingsIt.key());
            pair.insert(QStringLiteral("value"), settingsIt.value());
            settingsArray.append(pair);

            ++settingsIt;
        }
        data.insert(QStringLiteral("settings"), settingsArray);
    }

    QJsonObject dbObject;
    dbObject.insert(QStringLiteral("data"), data);

    QJsonArray dbArray;
    dbArray.append(dbObject);

    QJsonObject main;
    main.insert(QStringLiteral("db"), dbArray);

    c->response()->setJsonObjectBody(main);
    c->response()->headers().setContentDispositionAttachment(QStringLiteral("data.cmlyst.%1.json")
                                                             .arg(QDateTime::currentDateTimeUtc().toString(u"yyyy-MM-dd")).toLatin1());
}

void AdminSettings::db_clean(Context *c)
{
    if (!c->request()->isPost()) {
        c->response()->redirect(c->uriFor(CActionFor(u"database")));
    }

    QSqlQuery query = CPreparedSqlQueryThreadForDB(
                QStringLiteral("DELETE FROM posts"),
                QStringLiteral("cmlyst"));
    if (query.exec()) {
        c->response()->redirect(c->uriFor(CActionFor(QStringLiteral("database")),
                                          StatusMessage::statusQuery(c, QStringLiteral("Database wiped."))));
    } else {
        c->response()->redirect(c->uriFor(CActionFor(QStringLiteral("database")),
                                          StatusMessage::errorQuery(c, QStringLiteral("Failed to wipe database '%1'.")
                                                                    .arg(query.lastError().databaseText()))));
    }
}
