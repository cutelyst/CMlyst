#include "sqluserstore.h"

#include <Cutelyst/Context>
#include <Cutelyst/Plugins/Utils/Sql>

#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include <QLoggingCategory>

using namespace Cutelyst;

SqlUserStore::SqlUserStore(QObject *parent) : AuthenticationStore(parent)
{

}

Cutelyst::AuthenticationUser SqlUserStore::findUser(Cutelyst::Context *c, const Cutelyst::ParamsMultiMap &userinfo)
{
    Q_UNUSED(c)
    QSqlQuery query = CPreparedSqlQueryThreadForDB(
                QStringLiteral("SELECT * FROM users WHERE email = :email"),
                QStringLiteral("cmlyst"));
    query.bindValue(QStringLiteral(":email"), userinfo.value(QStringLiteral("email")));
    if (query.exec() && query.next()) {
        QVariant userId = query.value(QStringLiteral("id"));

        AuthenticationUser user(userId.toString());

        int columns = query.record().count();
        // send column headers
        QStringList cols;
        const QSqlRecord record = query.record();
        for (int j = 0; j < columns; ++j) {
            cols.append(record.fieldName(j));
        }

        for (int j = 0; j < columns; ++j) {
            user.insert(cols.at(j), query.value(j));
        }

        return user;
    }

    return AuthenticationUser();
}

bool SqlUserStore::addUser(const ParamsMultiMap &user)
{
    QSqlQuery query = CPreparedSqlQueryThreadForDB(
                QStringLiteral("INSERT OR REPLACE INTO users "
                               "(name, email, password) "
                               "VALUES "
                               "(:name, :email, :password)"),
                QStringLiteral("cmlyst"));
    query.bindValue(QStringLiteral(":name"), user.value(QStringLiteral("name")));
    query.bindValue(QStringLiteral(":email"), user.value(QStringLiteral("email")));
    query.bindValue(QStringLiteral(":password"), user.value(QStringLiteral("password")));
    if (!query.exec()) {
        qDebug() << "Failed to add new user:" << query.lastError().databaseText() << user;
        return false;
    }
    return true;
}
