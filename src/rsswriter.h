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
