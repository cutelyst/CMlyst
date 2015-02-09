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

#ifndef ENGINE_H
#define ENGINE_H

#include <QObject>
#include <QHash>

namespace CMS {

typedef QHash<QString, QString> StringHash;

class Page;
class Menu;
class EnginePrivate;
class Engine : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Engine)
    Q_PROPERTY(StringHash settings READ settings)
public:
    explicit Engine(QObject *parent = 0);
    virtual ~Engine();

    bool init(const QHash<QString, QString> &settings);

    QString title();
    QString description();

    virtual Page *getPage(const QString &path);

    virtual Page *getPageToEdit(const QString &path) const;
    virtual bool savePage(Page *page);

    /**
     * Returns the available pages,
     * when depth is -1 all pages are listed
     */
    virtual QList<Page *> listPages(int depth = -1);

    virtual QList<Page *> listPosts(int depth = -1);

    virtual QList<Menu *> menus();

    virtual Menu *menu(const QString &name);

    virtual QHash<QString, Menu *> menuLocations();

    virtual bool saveMenu(Menu *menu, bool replace);
    virtual bool removeMenu(const QString &name);
    bool saveMenus(const QList<Menu *> &menus);

    virtual QDateTime lastModified();

    virtual bool settingsIsWritable();
    virtual QHash<QString, QString> settings();
    virtual QString settingsValue(const QString &key, const QString &defaultValue = QString());
    virtual bool setSettingsValue(const QString &key, const QString &value);

protected:
    EnginePrivate *d_ptr;
};

}

#endif // ENGINE_H
