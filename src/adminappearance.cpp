/***************************************************************************
 *   Copyright (C) 2015 Daniel Nicoletti <dantti12@gmail.com>              *
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

#include "adminappearance.h"

#include "../libCMS/menu.h"

#include <QDir>
#include <QSettings>

AdminAppearance::AdminAppearance(QObject *parent) : Controller(parent)
{

}

AdminAppearance::~AdminAppearance()
{

}

void AdminAppearance::index(Context *c)
{
    c->response()->redirect(c->uriFor(actionFor("menus")));
//    QDir dataDir = ctx->config("DataLocation").toString();
//    QSettings settings(dataDir.absoluteFilePath("site.conf"), QSettings::IniFormat);

//    settings.beginGroup("Menus");

//    QList<QObject *> menus;
//    foreach (const QString &menu, settings.childGroups()) {
//        settings.beginGroup(menu);
//        QObject *obj = new QObject(ctx);
//        obj->setProperty("Name", settings.value("Name"));
//        obj->setProperty("Locations", settings.value("Locations").toStringList().join(", "));
//        menus.append(obj);
//        settings.endGroup();
//    }

//    QStringList menu;
//    menu << "Home";
//    menu << "Abra";
//    menu << "Jota";
//    menu << "NOVO";

////    qDebug() << settings.allKeys();
//    qDebug() << menus;
//    qDebug() << settings.childGroups();
//    qDebug() << settings.childKeys();

//    settings.beginGroup("menu2");
//    settings.setValue("AutoAddPages", true);
//    settings.endGroup();

//    int size = settings.beginReadArray("menu2");
//    for (int i = 0; i < size; ++i) {
//        settings.setArrayIndex(i);
//        qDebug() << settings.value("url").toString() << settings.value("text").toString();
//    }
//    settings.endArray();

//    settings.beginWriteArray("menu3");
//    for (int i = 0; i < menu.size(); ++i) {
//        settings.setArrayIndex(i);
//        settings.setValue("url", "http://www.fooo/" + menu.at(i));
//        settings.setValue("text", menu.at(i));
//    }
//    settings.endArray();

//    menu.sort();

//    settings.beginWriteArray("menu2");
//    for (int i = 0; i < menu.size(); ++i) {
//        settings.setArrayIndex(i);
//        settings.setValue("url", "http://www.menu2/" + menu.at(i));
//        settings.setValue("text", menu.at(i));
//    }
//    settings.endArray();

//    settings.remove("menu1");

//    settings.endGroup();

//    ctx->stash({
//                   {"template", "appearance/index.html"},
//                   {"menus", QVariant::fromValue(menus)}
//               });
}

void AdminAppearance::menus(Context *c)
{
    c->stash({
                 {"template", "appearance/menus.html"},
                 {"menus", QVariant::fromValue(engine->menus())}
             });
}

void AdminAppearance::menus_remove(Context *c, const QString &id)
{
    engine->removeMenu(id);

    c->response()->redirect(c->uriFor(actionFor("menus")));
}

void AdminAppearance::menus_new(Context *c)
{
    if (c->req()->method() == "POST") {
        ParamsMultiMap params = c->req()->bodyParam();

        CMS::Menu *menu = new CMS::Menu(params.value("name"), c);
        if (!engine->saveMenu(menu, false)) {
            c->stash({
                         {"template", "appearance/menus_new.html"},
                         {"error_msg", tr("Could not save menu")}
                     });
        }

        c->response()->redirect(c->uriFor(actionFor("menus")));
        return;
    }

    c->stash({
                 {"template", "appearance/menus_new.html"}
             });
}

void AdminAppearance::menus_edit(Context *c, const QString &id)
{
    c->stash()["editing"] = true;

    if (c->req()->method() == "POST") {

        c->response()->redirect(c->uriFor(actionFor("menus")));
    } else {
        CMS::Menu *menu = engine->menu(id);
        if (!menu) {
            c->response()->redirect(c->uriFor(actionFor("menus")));
            return;
        }

        c->stash({
                       {"template", "appearance/menus_new.html"},
                       {"menu", QVariant::fromValue(menu)}
                   });
    }
}
