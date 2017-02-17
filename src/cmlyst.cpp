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
#include <Cutelyst/Plugins/Session/Session>
#include <Cutelyst/Plugins/Authentication/authentication.h>
#include <Cutelyst/Plugins/Authentication/authenticationrealm.h>
#include <Cutelyst/Plugins/Authentication/credentialpassword.h>
#include <Cutelyst/Plugins/Authentication/htpasswd.h>

#include <QStandardPaths>
#include <QDir>
#include <QDebug>

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

#include "libCMS/fileengine.h"
#include "libCMS/sqlengine.h"
#include "libCMS/page.h"
#include "libCMS/menu.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

CMlyst::CMlyst(QObject *parent) :
    Cutelyst::Application(parent)
{
}

CMlyst::~CMlyst()
{
}

bool CMlyst::init()
{
    bool production = config(QStringLiteral("production")).toBool();
    qDebug() << "Production" << production;

    auto view = new GrantleeView(this);
    view->setTemplateExtension(QStringLiteral(".html"));
    view->setWrapper(QStringLiteral("wrapper.html"));
    view->setCache(production);

    QDir dataDir = config(QStringLiteral("DataLocation"), QStandardPaths::writableLocation(QStandardPaths::DataLocation)).toString();
    if (!dataDir.exists() && !dataDir.mkpath(dataDir.absolutePath())) {
        qCritical() << "Could not create DataLocation" << dataDir.absolutePath();
        return false;
    }
    setConfig(QStringLiteral("DataLocation"), dataDir.absolutePath());

    view->setIncludePaths({ pathTo({ QStringLiteral("root"), QStringLiteral("themes"), QStringLiteral("default") }) });

    auto adminView = new GrantleeView(this, QStringLiteral("admin"));
    adminView->setTemplateExtension(QStringLiteral(".html"));
    adminView->setWrapper(QStringLiteral("wrapper.html"));
    adminView->setIncludePaths({ pathTo({ QStringLiteral("root"), QStringLiteral("admin") }) });
    adminView->setCache(production);

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

    auto store = new StoreHtpasswd(dataDir.absoluteFilePath(QStringLiteral("htpasswd")));

    auto password = new CredentialPassword;
    password->setPasswordField(QLatin1String("password"));
    password->setPasswordType(CredentialPassword::Hashed);

    auto realm = new AuthenticationRealm(store, password);

    new Session(this);

    auto auth = new Authentication(this);
    auth->addRealm(realm);

    qDebug() << "Root location" << pathTo({ QStringLiteral("root") });
    qDebug() << "Root Admin location" << pathTo({ QStringLiteral("root"), QStringLiteral("src"), QStringLiteral("admin") });
    qDebug() << "Data location" << dataDir.absolutePath();

    // Migrate
    auto fileEngine = new CMS::FileEngine(this);
    fileEngine->init({
                     {QStringLiteral("root"), dataDir.absolutePath()}
                 });

    auto sqlEngine = new CMS::SqlEngine(this);
    sqlEngine->init({
                     {QStringLiteral("root"), dataDir.absolutePath()}
                 });

    Q_FOREACH (CMS::Page *page, fileEngine->listPages(fileEngine)) {
        qDebug() << page->blog();
        sqlEngine->savePage(page);
    }

    QHash<QString, QString> settings = fileEngine->settings();
    auto it = settings.constBegin();
    while (it != settings.constEnd()) {
        sqlEngine->setSettingsValue(it.key(), it.value());
        ++it;
    }

    QJsonObject menusObj;
    const auto menus = fileEngine->menus();
    for (CMS::Menu *menu : menus) {
        QJsonObject objMenu;
//        objMenu.insert(QStringLiteral("id"), menu->id());
        objMenu.insert(QStringLiteral("name"), menu->name());

        QJsonArray locations;
        const auto menuLocations = menu->locations();
        for (const auto location : menuLocations) {
            locations.append(location);
        }
        objMenu.insert(QStringLiteral("locations"), locations);

        QJsonArray entriesJson;
        const QList<QVariantHash> entries = menu->entries();
        for (const auto entry : entries) {
            entriesJson.append(QJsonObject::fromVariantHash(entry));
        }
        objMenu.insert(QStringLiteral("entries"), entriesJson);

        menusObj.insert(menu->id(), objMenu);
    }
    QJsonDocument doc(menusObj);
    sqlEngine->setSettingsValue(QStringLiteral("menus"), QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));

    qDebug() << "JSON menu" << doc.toVariant();


    return true;
}

bool CMlyst::postFork()
{
    QDir dataDir = config(QStringLiteral("DataLocation")).toString();

//    auto engine = new CMS::FileEngine(this);
    auto engine = new CMS::SqlEngine(this);
    engine->init({
                     {QStringLiteral("root"), dataDir.absolutePath()}
                 });

    Q_FOREACH (Controller *controller, controllers()) {
        auto cmengine = dynamic_cast<CMEngine *>(controller);
        if (cmengine) {
            cmengine->engine = engine;
        }
    }

    Q_FOREACH (DispatchType *type, dispatchers()) {
        auto cmengine = dynamic_cast<CMEngine *>(type);
        if (cmengine) {
            cmengine->engine = engine;
        }
    }

    return true;
}
