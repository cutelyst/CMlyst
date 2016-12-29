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

#ifndef MENU_H
#define MENU_H

#include <QObject>
#include <QStringList>

namespace CMS {

class MenuPrivate;
class Menu : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Menu)
    Q_PROPERTY(QString id READ id)
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QStringList locations READ locations WRITE setLocations)
    Q_PROPERTY(QList<QVariantHash> entries READ entries)
public:
    explicit Menu(const QString &name, QObject *parent = 0);
    ~Menu();

    QString id() const;

    QString name() const;
    void setName(const QString &name);

    bool autoAddPages() const;
    void setAutoAddPages(bool enable);

    QStringList locations() const;
    void setLocations(const QStringList &locations);

    void appendEntry(const QString &text, const QString &url, const QString &attr = QString());
    virtual QList<QVariantHash> entries() const;
    virtual void setEntries(const QList<QVariantHash> &entries);

protected:
    MenuPrivate *d_ptr;
};

}

Q_DECLARE_METATYPE(CMS::Menu *)

#endif // MENU_H
