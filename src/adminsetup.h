#ifndef ADMINSETUP_H
#define ADMINSETUP_H

#include <Cutelyst/Controller>

using namespace Cutelyst;

namespace Cutelyst {
class View;
}

class AdminSetup : public Controller
{
    Q_OBJECT
    Q_CLASSINFO(C_NAMESPACE, "")
public:
    explicit AdminSetup(QObject *parent = 0);

public slots:
    void setup(Context *ctx, Local, Args);

    void edit(Context *ctx, const QString &id, Local, Args);

    void remove_user(Context *ctx, const QString &id, Local, Args);

    void status(Context *ctx, Path);

    void End(Context *ctx);

private:
    View *m_view;
};

#endif // ADMINSETUP_H
