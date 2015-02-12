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

#ifndef RSSWRITER_H
#define RSSWRITER_H

#include <QObject>
#include <QDateTime>
#include <QXmlStreamWriter>

class RSSWriter : public QObject
{
    Q_OBJECT
public:
    explicit RSSWriter(QObject *parent = 0);
    ~RSSWriter();

    void writeStartChannel();
    void writeChannelTitle(const QString &title);
    void writeChannelLink(const QString &link);
    void writeChannelFeedLink(const QString &link, const QString &mimeType = QString(), const QString &rel = QString());
    void writeChannelDescription(const QString &description);
    void writeChannelLastBuildDate(const QDateTime &lastBuildDate);
    void writeChannelLanguage(const QString &language);

    // Image of the feed
    void writeStartImage();
    void writeImageUrl(const QString &url);
    void writeImageTitle(const QString &title);
    void writeImageLink(const QString &link);
    void writeEndImage();

    void writeStartItem();
    void writeItemTitle(const QString &title);
    /**
     * Item link http://foo.com/2015/12/31/bar
     */
    void writeItemLink(const QString &link);
    /**
     * Item Comments link http://foo.com/2015/12/31/bar#comments
     */
    void writeItemCommentsLink(const QString &link);
    void writeItemNumberOfComments(int number);
    void writeItemCreator(const QString &creator);
    void writeItemCategory(const QString &category);
    void writeItemPubDate(const QDateTime &pubDate);
    void writeItemDescription(const QString &description);
    void writeItemContent(const QString &content);

    void writeEndItem();


    void writeEndChannel();

    void endRSS();

    QByteArray result() const;

private:
    QByteArray m_output;
    QXmlStreamWriter m_stream;
};

#endif // RSSWRITER_H
