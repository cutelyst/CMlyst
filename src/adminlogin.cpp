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

#include <Cutelyst/Plugins/Session/Session>
#include <Cutelyst/Plugins/Authentication/authentication.h>

#include <QCryptographicHash>
#include <QFile>
#include <QEventLoop>
#include <QTimer>
#include <QThread>
#include <QUrl>

#include <QDebug>

AdminLogin::AdminLogin(QObject *app) : Controller(app)
{
}

void AdminLogin::index(Context *ctx)
{
    Request *req = ctx->request();
    const QString username = req->param(QStringLiteral("username"));
    if (req->isPost()) {
        const QString password = req->param(QStringLiteral("password"));
        if (!username.isEmpty() && !password.isEmpty()) {

            // Authenticate
            if (Authentication::authenticate(ctx, req->bodyParams())) {
                qDebug() << Q_FUNC_INFO << username << "is now Logged in";
                ctx->res()->redirect(ctx->uriFor(QStringLiteral("/.admin")));
                return;
            } else {
                ctx->stash()["error_msg"] = trUtf8("Wrong password or username");
                qDebug() << Q_FUNC_INFO << username << "user or password invalid";
            }
        } else {
            qWarning() << "Empty username and password";
        }
    } else {
        qWarning() << "Non POST method";
    }

    ctx->setStash(QStringLiteral("username"), username);
    ctx->setStash(QStringLiteral("no_wrapper"), true);
    ctx->setStash(QStringLiteral("template"), QStringLiteral("login.html"));
}
