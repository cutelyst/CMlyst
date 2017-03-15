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

#include "libCMS/menu.h"

#include <QDir>
#include <QSettings>
#include <QUuid>
#include <QDebug>

AdminAppearance::AdminAppearance(QObject *app) : Controller(app)
{

}

AdminAppearance::~AdminAppearance()
{

}

void AdminAppearance::index(Context *c)
{
    c->response()->redirect(c->uriFor(CActionFor(QStringLiteral("menus"))));
}

void AdminAppearance::menus(Context *c)
{
    c->stash({
                 {QStringLiteral("template"), QStringLiteral("appearance/menus.html")},
                 {QStringLiteral("menus"), QVariant::fromValue(engine->menus())}
             });
    qDebug() << engine << engine->menus();
}

void AdminAppearance::menus_remove(Context *c, const QString &id)
{
    if (c->req()->isPost()) {
        engine->removeMenu(c, id);
    }

    c->response()->redirect(c->uriFor(CActionFor(QStringLiteral("menus"))));
}

void AdminAppearance::menus_new(Context *c)
{
    if (c->req()->isPost()) {
        ParamsMultiMap params = c->req()->bodyParams();

        QString id = QUuid::createUuid().toString().remove(QLatin1Char('{')).remove(QLatin1Char('}'));
        CMS::Menu *menu = new CMS::Menu(id, c);
        if (saveMenu(menu, params, false)) {
            c->response()->redirect(c->uriFor(CActionFor(QStringLiteral("menus"))));
            return;
        }

        c->stash({
                     {QStringLiteral("error_msg"), tr("Could not save menu")}
                 });

    } else {
        c->setStash(QStringLiteral("no_wrapper"), true);
    }
    c->setStash(QStringLiteral("template"), QStringLiteral("appearance/menus_new.html"));
}

void AdminAppearance::menus_edit(Context *c, const QString &id)
{
    c->setStash(QStringLiteral("editing"), true);

    CMS::Menu *menu = engine->menu(id.toHtmlEscaped());
    if (!menu) {
        qWarning() << "menu not found" << id;
        c->response()->redirect(c->uriFor(CActionFor(QStringLiteral("menus"))));
        return;
    }

    qWarning() << "params" << c->req()->method();
    if (c->req()->isPost()) {
        if (saveMenu(menu, c->req()->bodyParams(), true)) {
            c->response()->redirect(c->uriFor(CActionFor(QStringLiteral("menus"))));
            return;
        } else {
            c->stash({
                         {QStringLiteral("error_msg"), tr("Could not save menu")}
                     });
        }
    }

    c->stash({
                 {QStringLiteral("template"), QStringLiteral("appearance/menus_new.html")},
                 {QStringLiteral("menu"), QVariant::fromValue(menu)},
                 {QStringLiteral("no_wrapper"), true},
             });
}

bool AdminAppearance::saveMenu(CMS::Menu *menu, const ParamsMultiMap &params, bool replace)
{
    qDebug() << "saving menu id" << menu->id();
    menu->setName(params.value(QStringLiteral("name")).toHtmlEscaped());
    qDebug() << "saving menu name" << menu->name();

    // TODO remove this hack
    menu->setLocations({ QStringLiteral("main") });

    QStringList menuNames = params.values(QStringLiteral("menuName"));
    QStringList menuUrl = params.values(QStringLiteral("menuUrl"));
//    QStringList menuExternal = params.values(QStringLiteral("menuExternal"));

    qDebug() << "params" << params << menuNames.size() << menuUrl.size();
    if (menuNames.size() == menuUrl.size()) {
        menu->setEntries(QList<QVariantHash>());
        for (int i = 0; i < menuNames.size(); ++i) {
            qDebug() << "appending" << menuNames.at(i) << menuUrl.at(i);
            QString name = menuNames.at(i).toHtmlEscaped();
            QString url = menuUrl.at(i).toHtmlEscaped();
            if (!name.isEmpty() && !url.isEmpty()) {
                menu->appendEntry(name, url);
            }
        }
    }

    return engine->saveMenu(nullptr, menu, replace);
}
