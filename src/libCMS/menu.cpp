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

#include "menu_p.h"

#include <QVariantHash>
#include <QDebug>

using namespace CMS;

Menu::Menu(const QString &id, QObject *parent) : QObject(parent)
  , d_ptr(new MenuPrivate)
{
    Q_D(Menu);
    d->id = id;
}

Menu::~Menu()
{
    delete d_ptr;
}

QString Menu::id() const
{
    Q_D(const Menu);
    return d->id;
}

QString Menu::name() const
{
    Q_D(const Menu);
    return d->name;
}

void Menu::setName(const QString &name)
{
    Q_D(Menu);
    d->name = name;
}

bool Menu::autoAddPages() const
{
    Q_D(const Menu);
    return d->autoAddPages;
}

void Menu::setAutoAddPages(bool enable)
{
    Q_D(Menu);
    d->autoAddPages = enable;
}

QStringList Menu::locations() const
{
    Q_D(const Menu);
    return d->locations;
}

void Menu::setLocations(const QStringList &locations)
{
    Q_D(Menu);
    d->locations = locations;
}

void Menu::appendEntry(const QString &text, const QString &url, const QString &attr)
{
    Q_D(Menu);
    d->urls.append({
                       {QStringLiteral("text"), text},
                       {QStringLiteral("url"), url},
                       {QStringLiteral("attr"), attr}
                   });
}

QList<QVariantHash> Menu::entries() const
{
    Q_D(const Menu);
    return d->urls;
}

void Menu::setEntries(const QList<QVariantHash> &entries)
{
    Q_D(Menu);
    d->urls = entries;
}

