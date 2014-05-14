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

#include <QSqlQuery>
#include <Cutelyst/Plugin/authentication.h>

using namespace Plugin;

Admin::Admin()
{
}

bool Admin::Auto(Context *ctx)
{
    qDebug() << "*** Admin::Auto()" << ctx->controller()->objectName() << ctx->actionName();

    if (*ctx->controller() == "AdminLogin" || ctx->actionName() == "logout") {
        return true;
    }

    Authentication *auth = ctx->plugin<Authentication*>();
    if (auth && !auth->userExists()) {
        qDebug() << "*** Admin::Auto() User not found forwarding to /admin/login";
        ctx->forward(QLatin1String("/admin/login/Index"));
        return false;
    }

    ctx->stash()["adminbase"] = true;

    return true;
}
