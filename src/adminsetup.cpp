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

#include "adminsetup.h"

#include "root.h"

#include <Cutelyst/Plugins/Authentication/authentication.h>
#include <Cutelyst/Plugins/Authentication/authenticationrealm.h>
#include <Cutelyst/Plugins/Authentication/htpasswd.h>
#include <Cutelyst/Plugins/Authentication/credentialpassword.h>
#include <Cutelyst/view.h>

#include <QCryptographicHash>
#include <QDebug>

AdminSetup::AdminSetup(QObject *app) : Controller(app)
{
}

void AdminSetup::setup(Context *ctx)
{
    qDebug() << Q_FUNC_INFO;
    if (ctx->req()->method() == "POST") {
        ParamsMultiMap param = ctx->req()->params();
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

                password = CredentialPassword::createPassword(password.toUtf8(),
                                                              QCryptographicHash::Sha256,
                                                              1000, 24, 24);

                AuthenticationRealm *realm = auth->realm();
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

void AdminSetup::notFound(Context *c)
{
    c->response()->redirect(c->uriFor(actionFor("setup")));
}

void AdminSetup::edit(Context *ctx, const QString &id)
{
    ParamsMultiMap param = ctx->req()->params();
    QString email = param.value(QLatin1String("email"));
    QString username = param.value(QLatin1String("username"));
    QString password = param.value(QLatin1String("password"));
    QString password2 = param.value(QLatin1String("password2"));
    ctx->stash()["username"] = username;
    ctx->stash()["email"] = email;

    if (ctx->req()->method() != "POST") {

    }

    if (ctx->req()->method() == "POST") {
        if (password == password2) {
            if (param.value("password").isEmpty()) {

            } else if (password.size() < 10) {
                ctx->stash()["error_msg"] = tr("Password must be longer than 10 characters");
            } else {
                password = CredentialPassword::createPassword(password.toUtf8(),
                                                              QCryptographicHash::Sha256,
                                                              1000, 24, 24);
            }
        } else {
            ctx->stash()["error_msg"] = tr("The two password didn't match");
        }
    }

    ctx->stash()[QLatin1String("template")] = "setup.html";
}

void AdminSetup::remove_user(Context *ctx, const QString &id)
{
    ctx->res()->redirect(ctx->uriFor("/"));
}

void AdminSetup::status(Context *ctx)
{
    ctx->stash()[QLatin1String("template")] = "setupStatus.html";
}

bool AdminSetup::End(Context *ctx)
{
    qDebug() << "*** AdminSetseup::End()";
return true;
}
