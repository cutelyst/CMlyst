#include "sqlengine.h"
#include "page.h"
#include "menu.h"

#include <Cutelyst/Plugins/Utils/Sql>

#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include <QDebug>

using namespace CMS;
using namespace Cutelyst;

SqlEngine::SqlEngine(QObject *parent) : Engine(parent)
{

}

bool SqlEngine::init(const QHash<QString, QString> &settings)
{
    QString root = settings.value(QStringLiteral("root"));
    if (root.isEmpty()) {
        root = QDir::currentPath();
    }

    const QString dbPath = root + QLatin1String("/cmlyst.sqlite");
    bool create = !QFile::exists(dbPath);

    if (QSqlDatabase::contains(QStringLiteral("cmlyst"))) {
        loadMenus();
        return true;
    }

    auto db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), Sql::databaseNameThread(QStringLiteral("cmlyst")));
    db.setDatabaseName(dbPath);
    if (db.open()) {
        qDebug() << "Database is open:" << dbPath << db.connectionName();
        if (create) {
            createDb();
            qDebug() << "Database tables created";
        }
    } else {
        qCritical() << "Error opening database" << dbPath << db.lastError().driverText();
        return false;
    }

    loadMenus();

    return true;
}

Page *createPageObj(const QSqlQuery &query)
{
    auto page = new Page;
    page->setAllowComments(query.value(QStringLiteral("allow_comments")).toBool());
    page->setAuthor(query.value(QStringLiteral("author")).toString());
    page->setBlog(query.value(QStringLiteral("blog")).toBool());
    page->setContent(query.value(QStringLiteral("content")).toString());
    page->setCreated(query.value(QStringLiteral("created")).toDateTime());
    page->setModified(query.value(QStringLiteral("modified")).toDateTime());
    page->setName(query.value(QStringLiteral("name")).toString());
    page->setNavigationLabel(query.value(QStringLiteral("navigation_label")).toString());
    page->setPath(query.value(QStringLiteral("path")).toString());
    return page;
}

Page *SqlEngine::getPage(const QString &path)
{
    qDebug() << Q_FUNC_INFO << path;
    QSqlQuery query = CPreparedSqlQueryThreadForDB(QStringLiteral("SELECT path, name, navigation_label, author, content,"
                                                                  " modified, created, tags, blog, allow_comments "
                                                                  "FROM pages "
                                                                  "WHERE path = :path"),
                                                   QStringLiteral("cmlyst"));
    if (!path.isNull()) {
        query.bindValue(QStringLiteral(":path"), path);
    } else {
        query.bindValue(QStringLiteral(":path"), QStringLiteral(""));
    }

    if (query.exec() && query.next()) {
        return createPageObj(query);
    }
    qWarning() << "Failed to get page" << query.lastError().databaseText();
    return 0;
}

Page *SqlEngine::getPageToEdit(const QString &path)
{
    qDebug() << Q_FUNC_INFO;
    return getPage(path);
}

QString sortString(Engine::SortFlags sort)
{
    if (sort & Engine::Reversed) {
        if (sort & Engine::Name) {
            return QStringLiteral(" ORDER BY name ASC ");
        } else if (sort & Engine::Date) {
            return QStringLiteral(" ORDER BY date ASC ");
        }
    } else {
        if (sort & Engine::Name) {
            return QStringLiteral(" ORDER BY name DESC ");
        } else if (sort & Engine::Date) {
            return QStringLiteral(" ORDER BY date DESC ");
        }
    }
    
    return QString();
}

QList<Page *> SqlEngine::listPages(Engine::Filters filters, Engine::SortFlags sort, int depth, int limit)
{
    qDebug() << Q_FUNC_INFO;
    QList<Page *> ret;
    QSqlQuery query;
    if (filters == Engine::Pages) {
        query = CPreparedSqlQueryThreadForDB(QStringLiteral("SELECT path, name, navigation_label, author, content,"
                                                            " modified, created, tags, blog, allow_comments "
                                                            "FROM pages "
                                                            "WHERE blog = 0 "
                                                            "LIMIT :limit "
                                                            ),
                                             QStringLiteral("cmlyst"));
    } else if (filters == Engine::Posts) {
        query = CPreparedSqlQueryThreadForDB(QStringLiteral("SELECT path, name, navigation_label, author, content,"
                                                            " modified, created, tags, blog, allow_comments "
                                                            "FROM pages "
                                                            "WHERE blog = 1 "
                                                            "LIMIT :limit "
                                                            ),
                                             QStringLiteral("cmlyst"));
    } else {
        query = CPreparedSqlQueryThreadForDB(QStringLiteral("SELECT path, name, navigation_label, author, content,"
                                                            " modified, created, tags, blog, allow_comments "
                                                            "FROM pages "
                                                            "LIMIT :limit "
                                                            ),
                                             QStringLiteral("cmlyst"));
    }

    query.bindValue(QStringLiteral(":limit"), limit);
    if (query.exec()) {
        while (query.next()) {
            ret.append(createPageObj(query));
        }
    }
    return ret;
}

QHash<QString, QString> SqlEngine::settings()
{
    QHash<QString, QString> ret;
    QSqlQuery query = CPreparedSqlQueryThreadForDB(QStringLiteral("SELECT name, value FROM settings"),
                                                   QStringLiteral("cmlyst"));
    if (query.exec()) {
        while (query.next()) {
            ret.insert(query.value(0).toString(), query.value(1).toString());
        }
    }
    return ret;
}

QString SqlEngine::settingsValue(const QString &key, const QString &defaultValue) const
{
    QString ret;
    QSqlQuery query = CPreparedSqlQueryThreadForDB(QStringLiteral("SELECT value FROM settings WHERE name = :name"),
                                                   QStringLiteral("cmlyst"));
    query.bindValue(QStringLiteral(":name"), key);
    if (query.exec() && query.next()) {
        ret = query.value(0).toString();
    } else {
        ret = defaultValue;
    }
    return ret;
}

bool SqlEngine::setSettingsValue(const QString &key, const QString &value)
{
    QSqlQuery query = CPreparedSqlQueryThreadForDB(QStringLiteral("INSERT OR REPLACE INTO settings "
                                                                  "(name, value) "
                                                                  "VALUES "
                                                                  "(:key, :value)"),
                                                   QStringLiteral("cmlyst"));
    query.bindValue(QStringLiteral(":key"), key);
    query.bindValue(QStringLiteral(":value"), value);
    if (!query.exec()) {
        qWarning() << "Failed to save settings" << query.lastError().driverText();
        return false;
    }
    return true;
}

QList<Menu *> SqlEngine::menus()
{
    return m_menus;
}

QHash<QString, Menu *> SqlEngine::menuLocations()
{
    return m_menuLocations;
}

bool SqlEngine::settingsIsWritable() const
{
    return true;
}

bool SqlEngine::savePageBackend(Page *page)
{
    qDebug() << Q_FUNC_INFO << page->path();
    QSqlQuery query = CPreparedSqlQueryThreadForDB(QStringLiteral("INSERT OR REPLACE INTO pages "
                                                                  "(path, name, navigation_label, author, content,"
                                                                  " modified, created, tags, blog, allow_comments) "
                                                                  "VALUES "
                                                                  "(:path, :name, :navigation_label, :author, :content,"
                                                                  " :modified, :created, :tags, :blog, :allow_comments)"),
                                                   QStringLiteral("cmlyst"));
    query.bindValue(QStringLiteral(":path"), page->path());
    query.bindValue(QStringLiteral(":name"), page->name());
    query.bindValue(QStringLiteral(":navigation_label"), page->navigationLabel());
    query.bindValue(QStringLiteral(":author"), page->author());
    query.bindValue(QStringLiteral(":content"), page->content());
    query.bindValue(QStringLiteral(":modified"), page->modified());
    query.bindValue(QStringLiteral(":created"), page->created());
    query.bindValue(QStringLiteral(":tags"), page->tags());
    query.bindValue(QStringLiteral(":blog"), page->blog());
    query.bindValue(QStringLiteral(":allow_comments"), page->allowComments());
    if (!query.exec()) {
        qWarning() << "Failed to save page" << query.lastError().driverText();
        return false;
    }
    return true;
}

void SqlEngine::loadMenus()
{
    QList<CMS::Menu *> menus;
    QHash<QString, CMS::Menu *> menuLocations;

    QString menusSetting = settingsValue(QStringLiteral("menus"));
    QJsonDocument doc = QJsonDocument::fromJson(menusSetting.toUtf8());
    const QJsonArray array = doc.array();
    for (const QJsonValue &value : array) {
        QJsonObject obj = value.toObject();
        Menu *menu = new Menu(obj.value(QStringLiteral("id")).toString());

        menu->setName(obj.value(QStringLiteral("name")).toString());
        menu->setAutoAddPages(obj.value(QStringLiteral("autoAddPages")).toBool());

        QList<QVariantHash> urls;
        const QJsonArray urlsJson = obj.value(QStringLiteral("entries")).toArray();
        for (const QJsonValue &url : urlsJson) {
            urls.append(url.toObject().toVariantHash());
        }
        menu->setEntries(urls);

        QStringList locations;
        const QJsonArray locationsJson = obj.value(QStringLiteral("locations")).toArray();
        for (const QJsonValue &location : locationsJson) {
            locations.append(location.toString());
        }
        menu->setLocations(locations);

        bool added = false;
        Q_FOREACH (const QString &location, locations) {
            if (!menuLocations.contains(location)) {
                menuLocations.insert(location, menu);
                added = true;
            }
        }
        qDebug() << "MENU";
        qDebug() << menu->id() << menu->name() << menu->entries() << menu->locations() << menu->autoAddPages();

        menus.push_back(menu);
    }
    qDebug() << "MENUS" << menus;
    m_menus = menus;
    qDebug() << "MENUS" << menuLocations;
    m_menuLocations = menuLocations;
}

void SqlEngine::createDb()
{
    QSqlQuery query(QStringLiteral("cmlyst"));

    bool ret = query.exec("PRAGMA journal_mode = WAL");
    qDebug() << "PRAGMA journal_mode = WAL" << ret << query.lastError().driverText();

    if (!query.exec(QStringLiteral("CREATE TABLE pages ( "
                                   "path TEXT NOT NULL UNIQUE PRIMARY KEY "
                                   ", name TEXT "
                                   ", navigation_label TEXT "
                                   ", author TEXT "
                                   ", content TEXT "
                                   ", modified TEXT "
                                   ", created TEXT "
                                   ", tags TEXT "
                                   ", blog BOOL NOT NULL "
                                   ", allow_comments BOOL NOT NULL "
                                   ")"))) {
        qCritical() << "Error creating database" << query.lastError().text();
        exit(1);
    }

    if (!query.exec(QStringLiteral("CREATE TABLE settings "
                                   "( name TEXT NOT NULL PRIMARY KEY "
                                   ", value TEXT"
                                   ")"))) {
        qCritical() << "Error creating database" << query.lastError().text();
        exit(1);
    }
}
