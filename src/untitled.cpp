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

#include "untitled.h"

#include <Cutelyst/Application>
#include <Cutelyst/View>
#include <Cutelyst/Plugins/StaticSimple>
#include <Cutelyst/Plugins/Session>
#include <Cutelyst/Plugins/authentication.h>
#include <Cutelyst/Plugins/Authentication/credentialpassword.h>
#include <Cutelyst/Plugins/Authentication/htpasswd.h>

#include <QtSql/QSqlDatabase>
#include <QSqlError>
#include <QTimer>

#include "authstoresql.h"

#include "root.h"
#include "admin.h"
#include "adminlogin.h"
#include "adminposts.h"
#include "adminsetup.h"
#include "blog.h"

#include <QThread>

Untitled::Untitled(QObject *parent) :
    Cutelyst::Application(parent)
{
}

Untitled::~Untitled()
{
}

bool Untitled::init()
{
    View *view = new View("Grantlee", this);
    view->setTemplateExtension(".html");
    view->setWrapper("wrapper.html");

    if (qEnvironmentVariableIsSet("SETUP")) {
        view->setIncludePath("/home/daniel/code/untitled/root/src/admin");

        registerController(new AdminSetup);
    } else {
        view->setIncludePath("/home/daniel/code/untitled/root/src");

        registerController(new Root);
        registerController(new Admin);
        registerController(new AdminLogin);
        registerController(new AdminPosts);
        registerController(new Blog);
    }
    registerView(view);

    StoreHtpasswd *store = new StoreHtpasswd("htpasswd");

    CredentialPassword *password = new CredentialPassword;
    password->setPasswordField(QLatin1String("password"));
    password->setPasswordType(CredentialPassword::Hashed);
    password->setHashType(QCryptographicHash::Sha256);

    Authentication::Realm *realm = new Authentication::Realm(store, password);

    registerPlugin(new StaticSimple("/home/daniel/code/untitled/root"));

    QObject::connect(this, &Application::registerPlugins,
                [=](Context *ctx) {

        ctx->registerPlugin(new Session);

        Authentication *auth = new Authentication;
        auth->addRealm(realm);
        ctx->registerPlugin(auth);
    });

    if (!QSqlDatabase::drivers().contains("QPSQL")) {
        qCritical() << "PostgreSQL driver not available";
        return false;
    }

    return true;
}

bool Untitled::postFork()
{
//    qCritical() << "postFork" << QCoreApplication::applicationPid() << qApp->thread()->currentThread();


    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setDatabaseName("untitled");

//    if (!db.open()) {
//        qDebug() << "Failed to open database";
//        qDebug() << db.lastError().databaseText();
//        qDebug() << db.lastError().driverText();
//        return false;
//    }
//    QTimer::singleShot(1000, this, SLOT(loop()));

    return true;
}

void Untitled::loop()
{
    qDebug() << Q_FUNC_INFO << QCoreApplication::applicationPid();
}
