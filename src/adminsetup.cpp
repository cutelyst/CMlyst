#include "adminsetup.h"

#include <Cutelyst/Plugin/authentication.h>

#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QDebug>

using namespace Cutelyst::Plugin;

AdminSetup::AdminSetup(QObject *parent) :
    Controller(parent)
{
}

void AdminSetup::setup(Context *ctx, Controller::Local, Controller::Args)
{
    qDebug() << Q_FUNC_INFO;
    if (ctx->req()->method() == "POST") {
        QMultiHash<QString, QString> param = ctx->req()->param();
        QString sitetitle = ctx->req()->param().value(QLatin1String("sitetitle"));
        QString email = ctx->req()->param().value(QLatin1String("email"));
        QString username = ctx->req()->param().value(QLatin1String("username"));
        QString password = ctx->req()->param().value(QLatin1String("password"));
        QString password2 = ctx->req()->param().value(QLatin1String("password2"));
        ctx->stash()["username"] = username;
        ctx->stash()["sitetitle"] = sitetitle;
        ctx->stash()["email"] = email;

        if (password == password2) {
            if (password.size() < 10) {
                ctx->stash()["error_msg"] = tr("Password must be longer than 10 characters");
            } else {
                Authentication *auth = ctx->plugin<Authentication*>();

                QCryptographicHash hash(QCryptographicHash::Sha256);
                hash.addData(password.toUtf8());
                password =  hash.result().toHex();

                QSqlQuery query;
                query.prepare("INSERT INTO u_users (username, pass) "
                              "VALUES (:username, :pass)");
                query.bindValue(":username", username);
                query.bindValue(":pass", password);
                if (!query.exec()) {
                    ctx->stash()["error_msg"] = query.lastError().text();
                } else {
                    ctx->res()->redirect("/");
                    return;
                }
            }
        } else {
            ctx->stash()["error_msg"] = tr("The two password didn't match");
        }
    }

    ctx->stash()[QLatin1String("template")] = "setup.html";
}
