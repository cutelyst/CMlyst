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

#include "admin.h"

#include <Cutelyst/Context>
#include <Cutelyst/Plugins/Authentication/authentication.h>
#include <Cutelyst/Plugins/StatusMessage>

#include <QStringBuilder>
#include <QDebug>

Admin::Admin(QObject *app) : Controller(app)
{
}

bool Admin::Auto(Context *c)
{
    engine->loadSettings(c);

    StatusMessage::load(c);

    if (c->action() == CActionFor(u"login") ||
            c->action() == CActionFor(u"logout")) {
        return true;
    }

    if (!Authentication::userExists(c)) {
        qDebug() << "*** Admin::Auto() User not found forwarding to /.admin/login";
        c->res()->redirect(c->uriFor(CActionFor(u"login")));
        return false;
    }

    c->setObjectName(QStringLiteral("CMlyst"));

    c->setStash(QStringLiteral("adminbase"), true);
    c->setStash(QStringLiteral("user"), Authentication::user(c));

    return true;
}

void Admin::notFound(Context *c)
{
    c->setStash(QStringLiteral("template"), QStringLiteral("404.html"));
    c->res()->setStatus(404);
}

bool Admin::End(Context *c)
{
    Q_UNUSED(c)
    return true;
}

void Admin::logout(Cutelyst::Context *c)
{
    Authentication::logout(c);
    c->res()->redirect(c->uriFor(CActionFor(u"login")));
}

void Admin::login(Context *c)
{
    Request *req = c->request();
    const ParamsMultiMap params = req->bodyParams();
    const QString username = params.value(QStringLiteral("email"));
    if (req->isPost()) {
        const QString password = params.value(QStringLiteral("password"));
        if (!username.isEmpty() && !password.isEmpty()) {

            // Authenticate
            if (Authentication::authenticate(c, params)) {
                qDebug() << Q_FUNC_INFO << username << "is now Logged in";
                c->res()->redirect(c->uriFor(QStringLiteral("/.admin/posts")));
                return;
            } else {
                c->setStash(QStringLiteral("error_msg"), tr("Wrong password or username"));
                qDebug() << Q_FUNC_INFO << username << "user or password invalid";
            }
        } else {
            qWarning() << "Empty username and password";
        }
        c->res()->setStatus(Response::Forbidden);
    } else {
        qWarning() << "Non POST method";
    }

    c->setStash(QStringLiteral("username"), username);
    c->setStash(QStringLiteral("no_wrapper"), true);
    c->setStash(QStringLiteral("template"), QStringLiteral("login.html"));
}
