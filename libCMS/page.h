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

#ifndef CMS_PAGE_H
#define CMS_PAGE_H

#include <QObject>
#include <QDateTime>
#include <QStringList>

namespace CMS {

class PagePrivate;
class Page : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Page)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString navigationLabel READ navigationLabel WRITE setNavigationLabel)
    Q_PROPERTY(QString path READ path WRITE setPath)
    Q_PROPERTY(QString author READ author WRITE setAuthor)
    Q_PROPERTY(QDateTime modified READ modified WRITE setModified)
    Q_PROPERTY(QStringList tags READ tags WRITE setTags)
public:
    Page();
    virtual ~Page();

    QString name() const;
    void setName(const QString &name);

    QString navigationLabel() const;
    void setNavigationLabel(const QString &label);

    QString path() const;
    void setPath(const QString &path);

    QString author() const;
    void setAuthor(const QString &author);

    QByteArray content() const;
    void setContent(const QByteArray &body);

    QDateTime modified() const;
    void setModified(const QDateTime &dateTime);

    QStringList tags() const;
    void setTags(const QStringList &tags);

    static QString readablePath(const QString &path);

protected:
    PagePrivate *d_ptr;
};

}

#endif // CMS_PAGE_H
