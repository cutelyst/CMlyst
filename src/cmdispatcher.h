#ifndef CMDISPATCHER_H
#define CMDISPATCHER_H

#include <Cutelyst/DispatchType>

#include "cmengine.h"

using namespace Cutelyst;

class CMDispatcher : public DispatchType, public CMEngine
{
    Q_OBJECT
public:
    explicit CMDispatcher(QObject *parent = nullptr);
    ~CMDispatcher();

    /**
     * @brief list the registered actions
     * To be implemented by subclasses
     */
    virtual QByteArray list() const final;

    /**
     * Return true if the dispatchType matches the given path
     */
    DispatchType::MatchType match(Context *c, QStringView pathView, const QStringList &args) const override final;

    /**
     * Returns an uri for an action
     */
    virtual QString uriForAction(Action *action, const QStringList &captures) const final;

    virtual bool registerAction(Action *action) final;

    virtual bool inUse() final;

private:
    Action *m_pageAction = nullptr;
    Action *m_latestPostsAction = nullptr;
};

#endif // CMDISPATCHER_H
