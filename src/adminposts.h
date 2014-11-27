#ifndef ADMINPOSTS_H
#define ADMINPOSTS_H

#include <Cutelyst/Controller>

using namespace Cutelyst;

class AdminPosts : public Controller
{
    Q_OBJECT
    C_NAMESPACE(".admin/posts")
    Q_CLASSINFO("create_Path", "new")
public:
    explicit AdminPosts(QObject *parent = 0);

public:
    C_ATTR(index, :Path :Args)
    void index(Cutelyst::Context *ctx);

    C_ATTR(create, :Local)
    void create(Context *ctx);

    void mySLOT();

};

#endif // ADMINPOSTS_H
