#include "adminsetup.h"

#include "root.h"

#include <Cutelyst/Plugins/authentication.h>
#include <Cutelyst/Plugins/Authentication/htpasswd.h>
#include <Cutelyst/view.h>

#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QDebug>

AdminSetup::AdminSetup(QObject *parent) :
    Controller(parent)
{
}

void AdminSetup::setup(Context *ctx)
{
    qDebug() << Q_FUNC_INFO;
    if (ctx->req()->method() == "POST") {
        ParamsMultiMap param = ctx->req()->param();
        QString email = param.value(QLatin1String("email"));
        QString username = param.value(QLatin1String("username"));
        QString password = param.value(QLatin1String("password"));
        QString password2 = param.value(QLatin1String("password2"));
        ctx->stash()["username"] = username;
        ctx->stash()["email"] = email;

        if (password == password2) {
            if (password.size() < 10) {
                ctx->stash()["error_msg"] = tr("Password must be longer than 10 characters");
            } else {
                Authentication *auth = ctx->plugin<Authentication*>();

                QCryptographicHash hash(QCryptographicHash::Sha256);
                hash.addData(password.toUtf8());
                password =  hash.result().toHex();

                Authentication::Realm *realm = auth->realm();
                StoreHtpasswd *store = static_cast<StoreHtpasswd*>(realm->store());
                store->addUser({
                                   {"username", username},
                                   {"password", password},
                                   {"email", email}
                               });
            }
        } else {
            ctx->stash()["error_msg"] = tr("The two password didn't match");
        }
    }

    ctx->stash()[QLatin1String("template")] = "setup.html";
}

void AdminSetup::edit(Context *ctx, const QString &id)
{
    ParamsMultiMap param = ctx->req()->param();
    QString email = param.value(QLatin1String("email"));
    QString username = param.value(QLatin1String("username"));
    QString password = param.value(QLatin1String("password"));
    QString password2 = param.value(QLatin1String("password2"));
    ctx->stash()["username"] = username;
    ctx->stash()["email"] = email;

    if (ctx->req()->method() != "POST") {
        QSqlQuery query;
        query.prepare("SELECT username, email FROM u_users WHERE id = :id");
        query.bindValue(":id", id.toInt());
        if (!query.exec()) {
            ctx->stash()["error_msg"] = query.lastError().text();
            return;
        } else if (query.size() == 0){
            ctx->res()->redirect(ctx->uriFor("/"));
            return;
        } else if (query.next()){
            ctx->stash()["username"] = query.value("username");
            ctx->stash()["email"] = query.value("email");
        }
    }

    if (ctx->req()->method() == "POST") {
        if (password == password2) {
            if (param.value("password").isEmpty()) {
                QSqlQuery query;
                query.prepare("UPDATE u_users SET (username, email) "
                              "= (:username, :email) WHERE id = :id");
                query.bindValue(":username", username);
                query.bindValue(":email", email);
                query.bindValue(":id", id.toInt());
                if (!query.exec()) {
                    ctx->stash()["error_msg"] = query.lastError().text();
                } else {
                    ctx->res()->redirect(ctx->uriFor("/"));
                    return;
                }
            } else if (password.size() < 10) {
                ctx->stash()["error_msg"] = tr("Password must be longer than 10 characters");
            } else {
                QCryptographicHash hash(QCryptographicHash::Sha256);
                hash.addData(password.toUtf8());
                password =  hash.result().toHex();

                QSqlQuery query;
                query.prepare("UPDATE u_users SET (username, pass, email) "
                              "= (:username, :pass, :email) WHERE id = :id");
                query.bindValue(":username", username);
                query.bindValue(":pass", password);
                query.bindValue(":email", email);
                query.bindValue(":id", id.toInt());
                if (!query.exec()) {
                    ctx->stash()["error_msg"] = query.lastError().text();
                } else {
                    ctx->res()->redirect(ctx->uriFor("/"));
                    return;
                }
            }
        } else {
            ctx->stash()["error_msg"] = tr("The two password didn't match");
        }
    }

    ctx->stash()[QLatin1String("template")] = "setup.html";
}

void AdminSetup::remove_user(Context *ctx, const QString &id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM u_users WHERE id = :id");
    query.bindValue(":id", id.toInt());
    if (!query.exec()) {
        ctx->stash()["error_msg"] = query.lastError().text();
        return;
    }

    ctx->res()->redirect(ctx->uriFor("/"));
}

void AdminSetup::status(Context *ctx)
{
    ctx->stash()[QLatin1String("template")] = "setupStatus.html";

    QSqlQuery query;
    query.prepare("SELECT id, username, email FROM u_users ORDER BY 2");
    if (!query.exec()) {
        ctx->stash()["error_msg"] = query.lastError().text();
        return;
    } else if (query.size() == 0){
        ctx->res()->redirect("/setup");
        return;
    }

    ctx->stash()["users"] = Root::sqlQueryToStash(&query);
}

void AdminSetup::End(Context *ctx)
{
    qDebug() << "*** AdminSetup::End()";
}
