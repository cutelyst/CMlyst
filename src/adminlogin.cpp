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

#include "adminlogin.h"

#include <Cutelyst/Plugins/Session>
#include <Cutelyst/Plugins/authentication.h>

#include <QCryptographicHash>
#include <QFile>
#include <QEventLoop>
#include <QTimer>
#include <QThread>
#include <QUrl>

#include <QDebug>

AdminLogin::AdminLogin(QObject *parent)
{
}

void AdminLogin::index(Context *ctx)
{
    QString username = ctx->req()->param().value(QLatin1String("username"));
    if (ctx->req()->method() == "POST") {
        QString password = ctx->req()->param().value(QLatin1String("password"));
        if (!username.isEmpty() && !password.isEmpty()) {
            Authentication *auth = ctx->plugin<Authentication*>();
            CStringHash userinfo;
            userinfo["username"] = username;
            userinfo["password"] = password;
            qDebug() << Q_FUNC_INFO << auth;

            // Authenticate
            bool succeed = auth && !auth->authenticate(userinfo).isNull();
            if (succeed) {
                qDebug() << Q_FUNC_INFO << username << "is now Logged in";
                ctx->res()->redirect(ctx->uriFor("/.admin"));

                return;
            } else {
                ctx->stash()["error_msg"] = trUtf8("Wrong password or username");
                qDebug() << Q_FUNC_INFO << username << "user or password invalid";
            }
        }
    }

    ctx->stash()["username"] = username;
    ctx->stash()["no_wrapper"] = "1";
    ctx->stash()["template"] = "login.html";
}
