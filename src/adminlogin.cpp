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

#include <Cutelyst/Plugin/Session>
#include <Cutelyst/Plugin/authentication.h>

#include <QCryptographicHash>
#include <QFile>
#include <QSqlQuery>
#include <QEventLoop>
#include <QTimer>
#include <QThread>
#include <QUrl>

#include <QDebug>

using namespace Plugin;

AdminLogin::AdminLogin(QObject *parent)
{
}

void AdminLogin::index(Context *c, Controller::Path, Controller::Args)
{
//    qCritical() << "AdminLogin::Index thread" << c << thread()->currentThreadId();

//    c->req()->contentType();

//    QTimer timeout;

//    QThread *test = new QThread(this);
//    test->start();

//    QEventLoop loop;
//    connect(&timeout, SIGNAL(timeout()), &loop, SLOT(quit()));

//    timeout.start(5000);
//    loop.exec();
//    c->req()->body()->readAll();
//    Session *session = c->plugin<Session*>();
//    session->setValue("chave", "senha");
//    c->res()->body().append(session->value("chave").toByteArray());
    c->res()->body() = "Hello World!\n";
    qDebug() << c->req()->headers().authorization();
    qDebug() << c->req()->headers().authorizationBasic();
    qDebug() << Q_FUNC_INFO << c->req()->headers().authorizationBasicPair();
//    c->res()->body() = "<pre>Hello World!\n" + c->req()->uri().url().toLatin1() + "</pre>";

//    c->res()->setContentType("text/html; charset=utf-8");
    c->detach();
     qDebug() << Q_FUNC_INFO << 1;
    return;

//    qDebug() << "AdminLogin::Index" << c;
//    qDebug() << "AdminLogin headers" << c->req()->headers();
//    qDebug() << "AdminLogin content type" << c->req()->contentType();
//    qDebug() << "AdminLogin content encoding" << c->req()->contentEncoding();
//    qDebug() << "AdminLogin:: upload body size" << c->req()->body()->size();
//    qDebug() << "AdminLogin:: upload body pos" << c->req()->body()->pos();
//    qDebug() << "AdminLogin:: upload body sequential" << c->req()->body()->isSequential();
//    qDebug() << "AdminLogin:: upload body readAll" << c->req()->body()->readAll();

    QString username = c->req()->param().value(QLatin1String("username"));
    if (c->req()->method() == "POST") {
        QString password = c->req()->param().value(QLatin1String("password"));
        if (!username.isEmpty() && !password.isEmpty()) {
            Authentication *auth = c->plugin<Authentication*>();
            CStringHash userinfo;
            userinfo["username"] = username;
            userinfo["pass"] = password;
            qDebug() << Q_FUNC_INFO << auth;

            // Authenticate
            bool succeed = auth && !auth->authenticate(userinfo).isNull();
            if (succeed) {
                qDebug() << Q_FUNC_INFO << username << "is now Logged in";
                c->res()->redirect(c->uriFor("/admin"));

                return;
            } else {
                c->stash()["error_msg"] = trUtf8("Wrong password or username");
                qDebug() << Q_FUNC_INFO << username << "user or password invalid";
            }
        }
    }

    c->stash()["username"] = username;
    c->stash()["no_wrapper"] = "1";
    c->stash()["template"] = "login.html";
}
