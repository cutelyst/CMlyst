#ifndef ENGINE_H
#define ENGINE_H

#include <QObject>

class Page;
class EnginePrivate;
class Engine : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Engine)
public:
    explicit Engine(QObject *parent = 0);
    virtual ~Engine();

    bool init(const QHash<QString, QString> &settings);

    virtual Page *getPage(const QString &path);

    virtual Page *getPageToEdit(const QString &path) const;
    virtual bool savePage(Page *page);

    virtual QList<Page *> listPages();

protected:
    EnginePrivate *d_ptr;
};

#endif // ENGINE_H
