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

void AdminSetup::setup(Context *c)
{
    qDebug() << Q_FUNC_INFO;
    if (c->req()->method() == "POST") {
        ParamsMultiMap param = c->req()->params();
        QString email = param.value(QLatin1String("email"));
        QString username = param.value(QLatin1String("username"));
        QString password = param.value(QLatin1String("password"));
        QString password2 = param.value(QLatin1String("password2"));
        c->stash()["username"] = username;
        c->stash()["email"] = email;

        if (password == password2) {
            if (password.size() < 10) {
                c->stash()["error_msg"] = tr("Password must be longer than 10 characters");
            } else {
                Authentication *auth = c->plugin<Authentication*>();

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
            c->stash()["error_msg"] = tr("The two password didn't match");
        }
    }

    c->stash()[QLatin1String("template")] = "setup.html";
}

void AdminSetup::notFound(Context *c)
{
    c->response()->redirect(c->uriFor(actionFor("setup")));
}

void AdminSetup::edit(Context *c, const QString &id)
{
    ParamsMultiMap param = c->req()->params();
    QString email = param.value(QLatin1String("email"));
    QString username = param.value(QLatin1String("username"));
    QString password = param.value(QLatin1String("password"));
    QString password2 = param.value(QLatin1String("password2"));
    c->stash()["username"] = username;
    c->stash()["email"] = email;

    if (c->req()->method() != "POST") {

    }

    if (c->req()->method() == "POST") {
        if (password == password2) {
            if (param.value("password").isEmpty()) {

            } else if (password.size() < 10) {
                c->stash()["error_msg"] = tr("Password must be longer than 10 characters");
            } else {
                password = CredentialPassword::createPassword(password.toUtf8(),
                                                              QCryptographicHash::Sha256,
                                                              1000, 24, 24);
            }
        } else {
            c->stash()["error_msg"] = tr("The two password didn't match");
        }
    }

    c->stash()[QLatin1String("template")] = "setup.html";
}

void AdminSetup::remove_user(Context *c, const QString &id)
{
    c->res()->redirect(c
                       ->uriFor("/"));
}

void AdminSetup::status(Context *c)
{
    c->stash()[QLatin1String("template")] = "setupStatus.html";
}

bool AdminSetup::End(Context *c)
{
    Q_UNUSED(c);
    return true;
}
