#ifndef AUTHSTORESQL_H
#define AUTHSTORESQL_H

#include <Cutelyst/Plugins/Authentication/authenticationstore.h>

using namespace Cutelyst;

class AuthStoreSql : public AuthenticationStore
{
public:
    AuthStoreSql();

    virtual AuthenticationUser findUser(Context *ctx, const CStringHash &userinfo);

private:
    QString m_idField;
};

#endif // AUTHSTORESQL_H
