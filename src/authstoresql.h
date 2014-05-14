#ifndef AUTHSTORESQL_H
#define AUTHSTORESQL_H

#include <Cutelyst/Plugin/authentication.h>

using namespace Cutelyst;
using namespace Cutelyst::Plugin;

class AuthStoreSql : public Authentication::Store
{
public:
    AuthStoreSql();

    virtual Authentication::User findUser(Context *ctx, const CStringHash &userinfo);

private:
    QString m_idField;
};

#endif // AUTHSTORESQL_H
