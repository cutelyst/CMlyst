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

inline Page *createPageObj(const QSqlQuery &query, QObject *parent)
{
    auto page = new Page(parent);
    page->setAllowComments(query.value(QStringLiteral("allow_comments")).toBool());
    page->setAuthor(query.value(QStringLiteral("author")).toString());
    page->setBlog(query.value(QStringLiteral("blog")).toBool());
    page->setContent(query.value(QStringLiteral("content")).toString());
    page->setModified(QDateTime::fromMSecsSinceEpoch(query.value(QStringLiteral("modified")).toLongLong() * 1000));
    page->setCreated(QDateTime::fromMSecsSinceEpoch(query.value(QStringLiteral("created")).toLongLong() * 1000));
    page->setName(query.value(QStringLiteral("name")).toString());
    page->setNavigationLabel(query.value(QStringLiteral("navigation_label")).toString());
    page->setPath(query.value(QStringLiteral("path")).toString());
    return page;
}

Page *SqlEngine::getPage(const QString &path, QObject *parent)
{
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
        auto page = new Page(parent);
        page->setPath(query.value(0).toString());
        page->setName(query.value(1).toString());
        page->setNavigationLabel(query.value(2).toString());
        page->setAuthor(query.value(3).toString());
        page->setContent(query.value(4).toString());
        page->setModified(QDateTime::fromMSecsSinceEpoch(query.value(5).toLongLong() * 1000));
        page->setCreated(QDateTime::fromMSecsSinceEpoch(query.value(6).toLongLong() * 1000));
        // tags 7
        page->setBlog(query.value(8).toBool());
        page->setAllowComments(query.value(9).toBool());
        return page;
    }
    qWarning() << "Failed to get page" << path << query.lastError().databaseText();
    return 0;
}

QVariantHash SqlEngine::getPage(const QString &path)
{
    QVariantHash ret;
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
        ret.insert(QStringLiteral("path"), query.value(0));
        ret.insert(QStringLiteral("name"), query.value(1));
        ret.insert(QStringLiteral("navigationLabel"), query.value(2));
        ret.insert(QStringLiteral("author"), query.value(3));
        ret.insert(QStringLiteral("content"), query.value(4));
        ret.insert(QStringLiteral("modified"), QDateTime::fromMSecsSinceEpoch(query.value(5).toLongLong() * 1000));
        ret.insert(QStringLiteral("created"), QDateTime::fromMSecsSinceEpoch(query.value(6).toLongLong() * 1000));
        ret.insert(QStringLiteral("tags"), query.value(7));
        ret.insert(QStringLiteral("blog"), query.value(8));
        ret.insert(QStringLiteral("allowComments"), query.value(9));
    } else {
        qWarning() << "Failed to get page" << path << query.lastError().databaseText();
    }
    return ret;
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

QList<Page *> SqlEngine::listPages(QObject *parent, Engine::Filters filters, Engine::SortFlags sort, int depth, int limit)
{
    qDebug() << Q_FUNC_INFO;
    QList<Page *> ret;
    QSqlQuery query;
    if (filters == Engine::Pages) {
        query = CPreparedSqlQueryThreadForDB(QStringLiteral("SELECT path, name, navigation_label, author, content,"
                                                            " modified, created, tags, blog, allow_comments "
                                                            "FROM pages "
                                                            "WHERE blog = 0 "
                                                            "ORDER BY created DESC "
                                                            "LIMIT :limit "
                                                            ),
                                             QStringLiteral("cmlyst"));
    } else if (filters == Engine::Posts) {
        query = CPreparedSqlQueryThreadForDB(QStringLiteral("SELECT path, name, navigation_label, author, content,"
                                                            " modified, created, tags, blog, allow_comments "
                                                            "FROM pages "
                                                            "WHERE blog = 1 "
                                                            "ORDER BY created DESC "
                                                            "LIMIT :limit "
                                                            ),
                                             QStringLiteral("cmlyst"));
    } else {
        query = CPreparedSqlQueryThreadForDB(QStringLiteral("SELECT path, name, navigation_label, author, content,"
                                                            " modified, created, tags, blog, allow_comments "
                                                            "FROM pages "
                                                            "ORDER BY created DESC "
                                                            "LIMIT :limit "
                                                            ),
                                             QStringLiteral("cmlyst"));
    }

    query.bindValue(QStringLiteral(":limit"), limit);
    if (query.exec()) {
        while (query.next()) {
            ret.append(createPageObj(query, parent));
        }
    }
    return ret;
}

QHash<QString, QString> SqlEngine::settings()
{
    QHash<QString, QString> ret;
    QSqlQuery query = CPreparedSqlQueryThreadForDB(QStringLiteral("SELECT key, value FROM settings"),
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
    QSqlQuery query = CPreparedSqlQueryThreadForDB(QStringLiteral("SELECT value FROM settings WHERE key = :key"),
                                                   QStringLiteral("cmlyst"));
    query.bindValue(QStringLiteral(":key"), key);
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
                                                                  "(key, value) "
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

bool SqlEngine::saveMenu(Menu *menu, bool replace)
{
    QJsonObject menusObj;
    auto menus = m_menus;

    if (menu) {
        for (const auto menuIt : menus) {
            if (menuIt->id() == menu->id()) {
                menus.removeOne(menuIt);
                break;
            }
        }
        menus.push_back(menu);
    }

    for (CMS::Menu *menu : menus) {
        QJsonObject objMenu;
//        objMenu.insert(QStringLiteral("id"), menu->id());
        objMenu.insert(QStringLiteral("name"), menu->name());

        QJsonArray locations;
        const auto menuLocations = menu->locations();
        for (const auto location : menuLocations) {
            locations.append(location);
        }
        objMenu.insert(QStringLiteral("locations"), locations);

        QJsonArray entriesJson;
        const QList<QVariantHash> entries = menu->entries();
        for (const auto entry : entries) {
            entriesJson.append(QJsonObject::fromVariantHash(entry));
        }
        objMenu.insert(QStringLiteral("entries"), entriesJson);

        menusObj.insert(menu->id(), objMenu);
    }

    QJsonDocument doc(menusObj);
    setSettingsValue(QStringLiteral("menus"), QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));

    loadMenus();
}

bool SqlEngine::removeMenu(const QString &name)
{
    for (const auto menuIt : m_menus) {
        if (menuIt->id() == name) {
            m_menus.removeOne(menuIt);
            saveMenu(nullptr, false);
            break;
        }
    }
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
                                                                  "(path, name, navigation_label, author, content, html,"
                                                                  " modified, created, tags, blog, published, allow_comments) "
                                                                  "VALUES "
                                                                  "(:path, :name, :navigation_label, :author, :content, :html,"
                                                                  " :modified, :created, :tags, :blog, :published, :allow_comments)"),
                                                   QStringLiteral("cmlyst"));
    query.bindValue(QStringLiteral(":path"), page->path());
    query.bindValue(QStringLiteral(":name"), page->name());
    query.bindValue(QStringLiteral(":navigation_label"), page->navigationLabel());
    query.bindValue(QStringLiteral(":author"), page->author());
    query.bindValue(QStringLiteral(":content"), page->content());
    query.bindValue(QStringLiteral(":html"), page->content());
    query.bindValue(QStringLiteral(":modified"), page->modified().toMSecsSinceEpoch() / 1000);
    query.bindValue(QStringLiteral(":created"), page->created().toMSecsSinceEpoch() / 1000);
    query.bindValue(QStringLiteral(":tags"), page->tags());
    query.bindValue(QStringLiteral(":blog"), page->blog());
    query.bindValue(QStringLiteral(":published"), true);
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
    const QJsonObject menusObj = doc.object();
    qDebug() << Q_FUNC_INFO << menusObj;
    auto it = menusObj.constBegin();
    while (it != menusObj.constEnd()) {
//    for (const QJsonValue &value : menusObj) {
//        qDebug() << Q_FUNC_INFO << value;
        QJsonObject obj = it.value().toObject();
//        Menu *menu = new Menu(obj.value(QStringLiteral("id")).toString());
        Menu *menu = new Menu(it.key());

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

        ++it;
    }
    qDebug() << "MENUS" << menus;
    m_menus = menus;
    qDebug() << "MENUS" << menuLocations;
    m_menuLocations = menuLocations;
}

void SqlEngine::createDb()
{
    QSqlQuery query(QSqlDatabase::database(Sql::databaseNameThread(QStringLiteral("cmlyst"))));
    qDebug() << "createDb";

    bool ret = query.exec(QStringLiteral("PRAGMA journal_mode = WAL"));
    qDebug() << "PRAGMA journal_mode = WAL" << ret << query.lastError().driverText();

    if (!query.exec(QStringLiteral("CREATE TABLE pages "
                                   "( id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT"
                                   ", path TEXT NOT NULL UNIQUE "
                                   ", name TEXT "
                                   ", navigation_label TEXT "
                                   ", author INTEGER "
                                   ", content TEXT "
                                   ", html TEXT "
                                   ", modified INTEGER "
                                   ", created INTEGER "
                                   ", tags TEXT "
                                   ", blog BOOL NOT NULL "
                                   ", published BOOL NOT NULL "
                                   ", allow_comments BOOL NOT NULL "
                                   ")"))) {
        qCritical() << "Error creating database" << query.lastError().text();
        exit(1);
    }

    if (!query.exec(QStringLiteral("CREATE TABLE settings "
                                   "( key TEXT NOT NULL PRIMARY KEY "
                                   ", value TEXT"
                                   ")"))) {
        qCritical() << "Error creating database" << query.lastError().text();
        exit(1);
    }

    if (!query.exec(QStringLiteral("CREATE TABLE users "
                                   "( id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT "
                                   ", name TEXT NOT NULL "
                                   ", password TEXT NOT NULL "
                                   ", email TEXT NOT NULL UNIQUE "
                                   ", json TEXT "
                                   ")"))) {
        qCritical() << "Error creating database" << query.lastError().text();
        exit(1);
    }
}
