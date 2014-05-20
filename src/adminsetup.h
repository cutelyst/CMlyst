#ifndef ADMINSETUP_H
#define ADMINSETUP_H

#include <Cutelyst/Controller>

using namespace Cutelyst;

class AdminSetup : public Controller
{
    Q_OBJECT
    Q_CLASSINFO(C_NAMESPACE, ".admin")
public:
    explicit AdminSetup(QObject *parent = 0);

public slots:
    void setup(Context *ctx, Local, Args);
};

#endif // ADMINSETUP_H
