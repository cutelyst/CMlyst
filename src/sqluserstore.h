#ifndef SQLUSERSTORE_H
#define SQLUSERSTORE_H

#include <QObject>
#include <Cutelyst/Plugins/Authentication/authenticationstore.h>

class SqlUserStore : public Cutelyst::AuthenticationStore
{
    Q_OBJECT
public:
    explicit SqlUserStore(QObject *parent = 0);

    virtual Cutelyst::AuthenticationUser findUser(Cutelyst::Context *c, const Cutelyst::ParamsMultiMap &userinfo);

    bool addUser(const Cutelyst::ParamsMultiMap &user);
};

#endif // SQLUSERSTORE_H
