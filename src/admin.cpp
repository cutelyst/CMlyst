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
#include <Cutelyst/Plugins/authentication.h>

#include <QStringBuilder>
#include <QDebug>

Admin::Admin()
{
}

bool Admin::Auto(Context *c)
{
    if (*c->controller() == "AdminLogin" ||
            c->actionName() == QLatin1String("logout")) {
        return true;
    }

    if (!Authentication::userExists(c)) {
        qDebug() << "*** Admin::Auto() User not found forwarding to /.admin/login/index";
        c->res()->redirect(c->uriForAction("/.admin/login/index"));
        return false;
    }

    c->setObjectName("CMlyst");

    c->stash()["adminbase"] = true;

    return true;
}

void Admin::notFound(Context *c)
{
    c->stash()[QLatin1String("template")] = "404.html";
    c->res()->setStatus(404);
}

void Admin::End(Context *c)
{
    Q_UNUSED(c)
}

void Admin::logout(Cutelyst::Context *c)
{
    Authentication *auth = c->plugin<Authentication*>();
    if (auth) {
        auth->logout(c);
    }
    c->res()->redirect(c->uriFor("/.admin/login"));
}
