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
#include <Cutelyst/view.h>

#include <QStringBuilder>
#include <QDebug>

Admin::Admin()
{
    m_view = new View("Grantlee", this);
    m_view->setIncludePath("/home/daniel/code/untitled/root/src/admin");
    m_view->setTemplateExtension(".html");
    m_view->setWrapper("wrapper.html");
}

bool Admin::Auto(Context *ctx)
{
//    qDebug() << "*** Admin::Auto()" << ctx->controller()->objectName() << ctx->actionName();

    if (*ctx->controller() == "AdminLogin" ||
            ctx->actionName() == "logout") {
        return true;
    }

    Authentication *auth = ctx->plugin<Authentication*>();
    if (auth && !auth->userExists()) {
        qDebug() << "*** Admin::Auto() User not found forwarding to /.admin/login/index";
        ctx->res()->redirect(ctx->uriFor("/.admin/login/index"));
        return false;
    }

    ctx->setObjectName("Untitled");

    ctx->stash()["adminbase"] = true;

    return true;
}

void Admin::notFound(Context *ctx)
{
    ctx->stash()[QLatin1String("template")] = "404.html";
    ctx->res()->setStatus(404);
}

void Admin::End(Context *ctx)
{
    qDebug() << "*** Admin::End()";
    m_view->render(ctx);
}

void Admin::logout(Cutelyst::Context *ctx)
{
    Authentication *auth = ctx->plugin<Authentication*>();
    if (auth) {
        auth->logout();
    }
    ctx->res()->redirect(ctx->uriFor("/.admin/login"));
}
