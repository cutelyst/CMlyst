#ifndef SQLUSERSTORE_H
#define SQLUSERSTORE_H

#include <QObject>
#include <Cutelyst/Plugins/Authentication/authenticationstore.h>

class SqlUserStore : public Cutelyst::AuthenticationStore
{
public:
    explicit SqlUserStore();

    Cutelyst::AuthenticationUser findUser(Cutelyst::Context *c, const Cutelyst::ParamsMultiMap &userinfo) override;

    QString addUser(const Cutelyst::ParamsMultiMap &user, bool replace);
};

#endif // SQLUSERSTORE_H
