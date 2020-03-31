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

#ifndef CMS_PAGE_H
#define CMS_PAGE_H

#include <QObject>
#include <QDateTime>
#include <QStringList>
#include <cutelee/safestring.h>

typedef QHash<QString, QString> Author;

namespace CMS {

class PagePrivate;
class Page : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Page)
    Q_PROPERTY(int id READ id WRITE setId)
    Q_PROPERTY(QString uuid READ uuid WRITE setUuid)
    Q_PROPERTY(QString name READ title WRITE setTitle)
    Q_PROPERTY(QString path READ path WRITE setPath)
    Q_PROPERTY(Author author READ author WRITE setAuthor)
    Q_PROPERTY(Cutelee::SafeString content READ content)
    Q_PROPERTY(QDateTime published_at READ publishedAt WRITE setPublishedAt)
    Q_PROPERTY(QDateTime updated_at READ updated WRITE setUpdated)
    Q_PROPERTY(QDateTime created_at READ created WRITE setCreated)
    Q_PROPERTY(bool published READ published WRITE setPublished)
    Q_PROPERTY(bool page READ page WRITE setPage)
    Q_PROPERTY(bool allowComments READ allowComments WRITE setAllowComments)
public:
    Page(QObject *parent);
    virtual ~Page();

    int id() const;
    void setId(int id);

    QString uuid() const;
    void setUuid(const QString &uuid);

    QString title() const;
    void setTitle(const QString &title);

    QString path() const;
    void setPath(const QString &path);

    Author author() const;
    void setAuthor(const Author &author);

    Cutelee::SafeString content() const;
    void setContent(const QString &body, bool safe);
    void updateContent(const Cutelee::SafeString &body);

    bool published() const;
    void setPublished(bool enable);

    QDateTime publishedAt() const;
    void setPublishedAt(const QDateTime &dateTime);

    QDateTime updated() const;
    void setUpdated(const QDateTime &dateTime);

    QDateTime created() const;
    void setCreated(const QDateTime &dateTime);

    bool page() const;
    void setPage(bool enable);

    bool allowComments() const;
    void setAllowComments(bool allow);

protected:
    PagePrivate *d_ptr;
};

}

#endif // CMS_PAGE_H
