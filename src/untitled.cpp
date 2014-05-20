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
#include <Cutelyst/Plugin/StaticSimple>
#include <Cutelyst/Plugin/Session>
#include <Cutelyst/Plugin/authentication.h>
#include <Cutelyst/Plugin/Authentication/credentialpassword.h>
#include <Cutelyst/Plugin/Authentication/minimal.h>

#include <QtSql/QSqlDatabase>
#include <QSqlError>
#include <QTimer>

#include "authstoresql.h"

#include "root.h"
#include "admin.h"
#include "adminlogin.h"
#include "adminsetup.h"
#include "blog.h"

Untitled::Untitled(QObject *parent) :
    Cutelyst::Application(parent)
{
}

Untitled::~Untitled()
{
}

bool Untitled::init()
{
    registerController(new Root);
    registerController(new Admin);
    if (qEnvironmentVariableIsSet("SETUP")) {
        registerController(new AdminSetup);
    }
    registerController(new AdminLogin);
    registerController(new Blog);

    AuthStoreSql *storeSql = new AuthStoreSql;

    CredentialPassword *password = new CredentialPassword;
    password->setPasswordField(QLatin1String("userpass"));
    password->setPasswordType(CredentialPassword::Hashed);
    password->setHashType(QCryptographicHash::Sha256);

    Authentication::Realm *realm = new Authentication::Realm(storeSql, password);

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

    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setDatabaseName("untitled");

    if (!db.open()) {
        qDebug() << "Failed to open database";
        qDebug() << db.lastError().databaseText();
        qDebug() << db.lastError().driverText();
//        return false;
    }
//    QTimer::singleShot(1000, this, SLOT(loop()));

    return true;
}

void Untitled::loop()
{
    qDebug() << Q_FUNC_INFO << QCoreApplication::applicationPid();
}
