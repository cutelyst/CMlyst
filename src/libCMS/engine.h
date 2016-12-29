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
#include <QVariant>
#include <QHash>

namespace CMS {

class Page;
class Menu;
class EnginePrivate;
class Engine : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Engine)
    Q_PROPERTY(QVariant settings READ settingsProperty)
    Q_PROPERTY(QVariant menus READ menusProperty)
public:
    enum Filter {
        Pages         = 0x1,
        Posts         = 0x2,
        OnlyPublished = 0x4,

        NoFilter      = -1
    };
    Q_DECLARE_FLAGS(Filters, Filter)

    enum SortFlag {
        Date     = 0x1,
        Name     = 0x2,
        Reversed = 0x4
    };
    Q_DECLARE_FLAGS(SortFlags, SortFlag)

    explicit Engine(QObject *parent = 0);
    virtual ~Engine();

    /**
     * ALWAYS init the engine on post fork, otherwise
     * engines might not perform correctly depending on
     * their implementation
     */
    virtual bool init(const QHash<QString, QString> &settings) = 0;

    QString title() const;
    QString description() const;

    virtual Page *getPage(const QString &path, QObject *parent) = 0;

    virtual QVariantHash getPage(const QString &path);

    Page *getPageToEdit(const QString &path, QObject *parent);

    bool savePage(Page *page);

    /**
     * Returns the available pages,
     * when depth is -1 all pages are listed
     */
    virtual QList<Page *> listPages(QObject *parent,
                                    Filters filters = NoFilter,
                                    SortFlags sort = SortFlags(Date | Reversed),
                                    int depth = -1,
                                    int limit = -1) = 0;

    virtual QList<Menu *> menus() = 0;

    virtual Menu *menu(const QString &id);

    virtual QHash<QString, Menu *> menuLocations() = 0;

    QVariant menusProperty();

    virtual bool saveMenu(Menu *menu, bool replace);
    virtual bool removeMenu(const QString &name);
    bool saveMenus(const QList<Menu *> &menus);

    virtual QDateTime lastModified();

    virtual bool settingsIsWritable() const = 0;
    virtual QHash<QString, QString> settings() = 0;
    virtual QVariant settingsProperty();
    virtual QString settingsValue(const QString &key, const QString &defaultValue = QString()) const = 0;
    virtual bool setSettingsValue(const QString &key, const QString &value) = 0;

    static QString normalizePath(const QString &path);
    static QString normalizeTitle(const QString &path);

protected:
    virtual bool savePageBackend(Page *page) = 0;

    EnginePrivate *d_ptr;
};

typedef QHash<QString, QString> StringHash;

}

Q_DECLARE_OPERATORS_FOR_FLAGS(CMS::Engine::Filters)
Q_DECLARE_OPERATORS_FOR_FLAGS(CMS::Engine::SortFlags)

#endif // ENGINE_H
