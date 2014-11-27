#ifndef AUTHSTORESQL_H
#define AUTHSTORESQL_H

#include <Cutelyst/Plugins/authentication.h>

using namespace Cutelyst;

class AuthStoreSql : public Authentication::Store
{
public:
    AuthStoreSql();

    virtual Authentication::User findUser(Context *ctx, const CStringHash &userinfo);

private:
    QString m_idField;
};

#endif // AUTHSTORESQL_H
