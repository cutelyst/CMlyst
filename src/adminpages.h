#ifndef ADMINPAGES_H
#define ADMINPAGES_H

#include <QObject>
#include <Cutelyst/Controller>

using namespace Cutelyst;

class AdminPages : public Controller
{
    Q_OBJECT
    C_NAMESPACE(".admin/pages")
public:
    AdminPages();
    ~AdminPages();

    C_ATTR(index, :Path :Args)
    void index(Context *ctx);

    C_ATTR(create, :Path(new) :Local)
    void create(Context *ctx);

    C_ATTR(edit, :Local)
    void edit(Context *ctx);
};

#endif // ADMINPAGES_H
