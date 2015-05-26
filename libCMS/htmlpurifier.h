#ifndef HTMLPURIFIER_H
#define HTMLPURIFIER_H

#include <QObject>
#include <QStringList>

class HTMLPurifierNode;
class HTMLPurifier
{
public:
    HTMLPurifier();
    ~HTMLPurifier();

    void setContent(const QString &content);
    void setAllowedHTML(const QStringList &allowedTags);

    QString purify();

private:
    QList<HTMLPurifierNode*> tokens(int lastPos, bool lastWasTag, HTMLPurifierNode *parent);
    HTMLPurifierNode *createNode(int &pos, int lastPos, bool plainText, HTMLPurifierNode *parent);
    void dumpTree(HTMLPurifierNode *root, int level);

    bool m_useAllowed;
    QStringList m_allowed;
    QString m_content;
    int m_pos = 0;
    HTMLPurifierNode *m_root = 0;
    QList<HTMLPurifierNode*> m_nodes;
};

#endif // HTMLPURIFIER_H
