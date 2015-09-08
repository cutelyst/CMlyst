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
#include <Cutelyst/Plugins/View/Grantlee/grantleeview.h>
#include <Cutelyst/Plugins/StaticSimple/StaticSimple>
#include <Cutelyst/Plugins/Session/Session>
#include <Cutelyst/Plugins/Authentication/authentication.h>
#include <Cutelyst/Plugins/Authentication/authenticationrealm.h>
#include <Cutelyst/Plugins/Authentication/credentialpassword.h>
#include <Cutelyst/Plugins/Authentication/htpasswd.h>

#include <QStandardPaths>
#include <QDir>
#include <QDebug>

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
    GrantleeView *view = new GrantleeView(this);
    view->setTemplateExtension(".html");
    view->setWrapper("wrapper.html");
    view->setCache(true);

    QDir dataDir = config("DataLocation", QStandardPaths::writableLocation(QStandardPaths::DataLocation)).toString();
    if (!dataDir.exists() && !dataDir.mkpath(dataDir.absolutePath())) {
        qCritical() << "Could not create DataLocation" << dataDir.absolutePath();
        return false;
    }
    setConfig("DataLocation", dataDir.absolutePath());

    view->setIncludePaths({ pathTo({ "root", "themes", "default" }) });

    GrantleeView *adminView = new GrantleeView(this, "admin");
    adminView->setTemplateExtension(".html");
    adminView->setWrapper("wrapper.html");
    adminView->setIncludePaths({ pathTo({ "root", "admin" }) });

    if (qEnvironmentVariableIsSet("SETUP")) {
        new AdminSetup(this);
    } else {
        new Root(this);
        new Admin(this);
        new AdminLogin(this);
        new AdminAppearance(this);
        new AdminPosts(this);
        new AdminPages(this);
        new AdminMedia(this);
        new AdminSettings(this);
    }

    new CMDispatcher(this);

    StoreHtpasswd *store = new StoreHtpasswd(dataDir.absoluteFilePath("htpasswd"));

    CredentialPassword *password = new CredentialPassword;
    password->setPasswordField(QLatin1String("password"));
    password->setPasswordType(CredentialPassword::Hashed);

    AuthenticationRealm *realm = new AuthenticationRealm(store, password);

    StaticSimple *staticSimple = new StaticSimple(this);
    staticSimple->setIncludePaths({
                                      pathTo({ "root" }),
                                      dataDir.absolutePath()
                                  });
    staticSimple->setDirs({
                              "static",
                              ".media"
                          });

    new Session(this);

    Authentication *auth = new Authentication(this);
    auth->addRealm(realm);

    qDebug() << "Root location" << pathTo({ "root" });
    qDebug() << "Root Admin location" << pathTo({ "root", "src", "admin" });
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
