#ifndef SQLENGINE_H
#define SQLENGINE_H

#include <QObject>
#include <QDateTime>
#include <QTimeZone>

#include "engine.h"

class QSqlQuery;

namespace Cutelyst {
class Context;
}

namespace CMS {

class FileEnginePrivate;
class SqlEngine : public Engine
{
    Q_OBJECT
public:
    explicit SqlEngine(QObject *parent = 0);

    bool init(const QHash<QString, QString> &settings) override;

    Page *getPage(QStringView path, QObject *parent) override;

    Page *getPageById(const QString &id, QObject *parent) override;

    bool removePage(int id) override;

    /**
     * Returns the available pages,
     * when depth is -1 all pages are listed
     */
    QList<Page *> listPages(QObject *parent,
                                    int offset,
                                    int limit) override;

    QList<Page *> listPagesPublished(QObject *parent,
                                             int offset,
                                             int limit) override;

    QList<Page *> listPosts(QObject *parent,
                                    int offset,
                                    int limit) override;

    QList<Page *> listPostsPublished(QObject *parent,
                                             int offset,
                                             int limit) override;

    QList<Page *> listAuthorPostsPublished(QObject *parent,
                                                   int authorId,
                                                   int offset,
                                                   int limit) override;

    QHash<QString, QString> settings() const override;

    QString settingsValue(const QString &key, const QString &defaultValue = QString()) const override;
    bool setSettingsValue(Cutelyst::Context *c, const QString &key, const QString &value) override;

    QList<Menu *> menus() override;

    bool saveMenu(Cutelyst::Context *c, Menu *menu, bool replace) override;
    bool removeMenu(Cutelyst::Context *c, const QString &name) override;

    QHash<QString, Menu *> menuLocations() override;

    bool settingsIsWritable() const override;

    QHash<QString, QString> loadSettings(Cutelyst::Context *c) override;

    QDateTime lastModified() override;

    QString addUser(Cutelyst::Context *c, const Cutelyst::ParamsMultiMap &user, bool replace) override;
    bool removeUser(Cutelyst::Context *c, int id) override;
    QVariantList users() override;
    QHash<QString, QString> user(const QString &slug) override;
    QHash<QString, QString> user(int id) override;

private:
    int savePageBackend(Page *page) override;

    void loadMenus();
    void loadUsers();
    void configureView(Cutelyst::Context *c);
    void createDb();
    Page *createPageObj(const QSqlQuery &query, QObject *parent);

    QString m_theme;
    QVariantList m_users;
    QHash<QString, QHash<QString, QString> > m_usersSlug;
    QHash<int, QHash<QString, QString> > m_usersId;
    QHash<QString, QString> m_settings;
    QDateTime m_settingsDateTime;
    QTimeZone m_timezone;
    qint64 m_settingsDate = -1;
    QList<CMS::Menu *> m_menus;
    QHash<QString, CMS::Menu *> m_menuLocations;
};

}

#endif // SQLENGINE_H
