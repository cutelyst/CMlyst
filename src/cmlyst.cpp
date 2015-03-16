/***************************************************************************
 *   Copyright (C) 2014-2015 Daniel Nicoletti <dantti12@gmail.com>         *
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

#include "cmlyst.h"

#include <Cutelyst/Application>
#include <Cutelyst/Plugins/viewengine.h>
#include <Cutelyst/Plugins/StaticSimple>
#include <Cutelyst/Plugins/Session>
#include <Cutelyst/Plugins/authentication.h>
#include <Cutelyst/Plugins/Authentication/credentialpassword.h>
#include <Cutelyst/Plugins/Authentication/htpasswd.h>

#include <QStandardPaths>
#include <QDir>

#include "authstoresql.h"

#include "root.h"
#include "admin.h"
#include "adminlogin.h"
#include "adminappearance.h"
#include "adminposts.h"
#include "adminpages.h"
#include "adminmedia.h"
#include "adminsettings.h"
#include "adminsetup.h"

#include "cmdispatcher.h"

#include "../libCMS/fileengine.h"

CMlyst::CMlyst(QObject *parent) :
    Cutelyst::Application(parent)
{
}

CMlyst::~CMlyst()
{
}

bool CMlyst::init()
{
    ViewEngine *view = new ViewEngine("Grantlee", this);
    view->setTemplateExtension(".html");
    view->setWrapper("wrapper.html");
    view->setCache(true);

    QDir dataDir = config("DataLocation", QStandardPaths::writableLocation(QStandardPaths::DataLocation)).toString();
    if (!dataDir.exists() && !dataDir.mkpath(dataDir.absolutePath())) {
        qCritical() << "Could not create DataLocation" << dataDir.absolutePath();
        return false;
    }
    setConfig("DataLocation", dataDir.absolutePath());

    QDir rootDir = config("RootLocation", QDir::currentPath()).toString();
    setConfig("RootLocation", rootDir.absolutePath());

    view->setIncludePaths({ rootDir.absoluteFilePath("src/themes/default") });
    registerView(view);

    ViewEngine *adminView = new ViewEngine("Grantlee", this);
    adminView->setTemplateExtension(".html");
    adminView->setWrapper("wrapper.html");
    adminView->setIncludePaths({ rootDir.absoluteFilePath("admin") });
    registerView(adminView, "admin");

    if (qEnvironmentVariableIsSet("SETUP")) {
        registerController(new AdminSetup);
    } else {
        registerController(new Root);
        registerController(new Admin);
        registerController(new AdminLogin);
        registerController(new AdminAppearance);
        registerController(new AdminPosts);
        registerController(new AdminPages);
        registerController(new AdminMedia);
        registerController(new AdminSettings);
    }

    registerDispatcher(new CMDispatcher);

    StoreHtpasswd *store = new StoreHtpasswd(dataDir.absoluteFilePath("htpasswd"));

    CredentialPassword *password = new CredentialPassword;
    password->setPasswordField(QLatin1String("password"));
    password->setPasswordType(CredentialPassword::Hashed);
    password->setHashType(QCryptographicHash::Sha256);

    Authentication::Realm *realm = new Authentication::Realm(store, password);

    StaticSimple *staticSimple = new StaticSimple;
    staticSimple->setIncludePaths({
                                      rootDir.absolutePath(),
                                      dataDir.absolutePath()
                                  });
    staticSimple->setDirs({
                              "static",
                              ".media"
                          });
    registerPlugin(staticSimple);

    QObject::connect(this, &Application::registerPlugins,
                [=](Context *ctx) {

        ctx->registerPlugin(new Session);

        Authentication *auth = new Authentication;
        auth->addRealm(realm);
        ctx->registerPlugin(auth);
    });

    qDebug() << "Root location" << rootDir.absolutePath();
    qDebug() << "Root Admin location" << rootDir.absoluteFilePath("src/admin");
    qDebug() << "Data location" << dataDir.absolutePath();

    return true;
}

bool CMlyst::postFork()
{
    QDir dataDir = config("DataLocation").toString();

    CMS::FileEngine *engine = new CMS::FileEngine(this);
    engine->init({
                     {"root", dataDir.absolutePath()}
                 });

    Q_FOREACH (Controller *controller, controllers()) {
        CMEngine *cmengine = dynamic_cast<CMEngine *>(controller);
        if (cmengine) {
            cmengine->engine = engine;
        }
    }

    Q_FOREACH (DispatchType *type, dispatchers()) {
        CMEngine *cmengine = dynamic_cast<CMEngine *>(type);
        if (cmengine) {
            cmengine->engine = engine;
        }
    }

    return true;
}
