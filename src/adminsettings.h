#ifndef ADMINSETTINGS_H
#define ADMINSETTINGS_H

#include <Cutelyst/Controller>

using namespace Cutelyst;

class AdminSettings : public Controller
{
    Q_OBJECT
    C_NAMESPACE(".admin/settings")
public:
    explicit AdminSettings(QObject *parent = 0);

    C_ATTR(index, :Path :Args)
    void index(Context *ctx);
};

#endif // ADMINSETTINGS_H
