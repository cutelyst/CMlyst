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
    Q_PROPERTY(QString content READ content WRITE setContent)
    Q_PROPERTY(QDateTime modified READ modified WRITE setModified)
    Q_PROPERTY(QDateTime created READ created WRITE setCreated)
    Q_PROPERTY(QStringList tags READ tags WRITE setTags)
    Q_PROPERTY(bool blog READ blog WRITE setBlog)
    Q_PROPERTY(bool allowComments READ allowComments WRITE setAllowComments)
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

    QString content() const;
    void setContent(const QString &body);

    QDateTime modified() const;
    void setModified(const QDateTime &dateTime);

    QDateTime created() const;
    void setCreated(const QDateTime &dateTime);

    QStringList tags() const;
    void setTags(const QStringList &tags);

    bool blog() const;
    void setBlog(bool enable);

    bool allowComments() const;
    void setAllowComments(bool allow);

protected:
    PagePrivate *d_ptr;
};

}

#endif // CMS_PAGE_H
