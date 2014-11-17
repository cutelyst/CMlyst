#ifndef ADMINPOSTS_H
#define ADMINPOSTS_H

#include <Cutelyst/Controller>

using namespace Cutelyst;

namespace Cutelyst {
class View;
}

class AdminPosts : public Controller
{
    Q_OBJECT
    C_NAMESPACE(".admin/posts")
    Q_CLASSINFO("create_Path", "new")
public:
    explicit AdminPosts(QObject *parent = 0);

public slots:
    void index(Cutelyst::Context *ctx, Path, Args);
    void create(Context *ctx, Controller::Args);

    void mySLOT();

};

#endif // ADMINPOSTS_H
