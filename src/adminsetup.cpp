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
#include "sqluserstore.h"

#include <Cutelyst/Plugins/Authentication/authentication.h>
#include <Cutelyst/Plugins/Authentication/authenticationrealm.h>
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
    if (c->req()->isPost()) {
        ParamsMultiMap params = c->req()->params();
        QString username = params.value(QLatin1String("username"));
        QString email = params.value(QLatin1String("email"));
        QString password = params.value(QLatin1String("password"));
        QString password2 = params.value(QLatin1String("password2"));
        c->setStash(QStringLiteral("usernname"), username);
        c->setStash(QStringLiteral("email"), email);

        if (password == password2) {
            if (password.size() < 10) {
                c->setStash(QStringLiteral("error_msg"), QStringLiteral("Password must be longer than 10 characters"));
            } else {
                password = QString::fromLatin1(CredentialPassword::createPassword(password.toUtf8(),
                                                                                  QCryptographicHash::Sha256,
                                                                                  1000, 24, 24));

                auto auth = c->plugin<Authentication*>();
                AuthenticationRealm *realm = auth->realm();
                auto store = static_cast<SqlUserStore*>(realm->store());
                bool ret = store->addUser({
                                              {QStringLiteral("name"), username},
                                              {QStringLiteral("email"), email},
                                              {QStringLiteral("password"), password},
                                          });
                if (ret) {
                    c->setStash(QStringLiteral("status_msg"), QStringLiteral("User successfuly added, restart the application without SETUP environment set"));
                } else {
                    c->setStash(QStringLiteral("error_msg"), QStringLiteral("Failed to add user, check application logs"));
                }
            }
        } else {
            c->setStash(QStringLiteral("error_msg"), QStringLiteral("The two password didn't match"));
        }
    }

    c->setStash(QStringLiteral("template"), QStringLiteral("setup.html"));
}

void AdminSetup::notFound(Context *c)
{
    c->response()->redirect(c->uriFor(CActionFor(QStringLiteral("setup"))));
}

void AdminSetup::edit(Context *c, const QString &id)
{
    ParamsMultiMap param = c->req()->params();
    QString email = param.value(QStringLiteral("email"));
    QString username = param.value(QStringLiteral("username"));
    QString password = param.value(QStringLiteral("password"));
    QString password2 = param.value(QStringLiteral("password2"));
    c->setStash(QStringLiteral("username"), username);
    c->setStash(QStringLiteral("email"), email);

    if (c->req()->isPost()) {
        if (password == password2) {
            if (param.value(QStringLiteral("password")).isEmpty()) {

            } else if (password.size() < 10) {
                c->setStash(QStringLiteral("error_msg"), tr("Password must be longer than 10 characters"));
            } else {
                password = QString::fromLatin1(CredentialPassword::createPassword(password.toUtf8(),
                                                                                  QCryptographicHash::Sha256,
                                                                                  1000, 24, 24));
            }
        } else {
            c->setStash(QStringLiteral("error_msg"), tr("The two password didn't match"));
        }
    }

    c->setStash(QStringLiteral("template"), QStringLiteral("setup.html"));
}

void AdminSetup::remove_user(Context *c, const QString &id)
{
    c->res()->redirect(c->uriFor(QStringLiteral("/")));
}

void AdminSetup::status(Context *c)
{
    c->setStash(QStringLiteral("template"), QStringLiteral("setupStatus.html"));
}

bool AdminSetup::End(Context *c)
{
    Q_UNUSED(c);
    return true;
}
