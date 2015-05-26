#include "htmlpurifier.h"

#include <QDebug>

class HTMLPurifierNode
{
public:
    HTMLPurifierNode *parent = 0;
    QList<HTMLPurifierNode *> children;
    QStringRef type;
    QStringRef text;
    bool end = false;
    bool plainText = true;
};

HTMLPurifier::HTMLPurifier()
{
    m_root = new HTMLPurifierNode;
}

HTMLPurifier::~HTMLPurifier()
{

}

void HTMLPurifier::setContent(const QString &content)
{
    m_content = content;
}

void HTMLPurifier::setAllowedHTML(const QStringList &allowedTags)
{
    m_allowed = allowedTags;
    m_useAllowed = true;
}

QString HTMLPurifier::purify()
{
//    int pos = m_content.indexOf(QLatin1Char('<'), m_pos);
//    HTMLPurifierNode *node = new HTMLPurifierNode;
//    node->text = m_content.midRef(m_pos, pos);

    qDebug() << "purify" << m_content;
    QList<HTMLPurifierNode *> nodes = tokens(0, false, m_root);

    qDebug() << "tokens" << nodes.size();
    m_content.remove(0, 1);

    Q_FOREACH (HTMLPurifierNode *node, nodes) {
        qDebug() << node->text << node->plainText;
    }

    qDebug() << "dumpTree";
    dumpTree(m_root, 0);

    return m_content;
}

QList<HTMLPurifierNode *> HTMLPurifier::tokens(int lastPos, bool lastWasTag, HTMLPurifierNode *parent)
{
    QList<HTMLPurifierNode *> ret;
    if (lastPos == m_content.size()) {
        // Nothing more to parse
        return ret;
    }

    HTMLPurifierNode *node;
    int pos;
    if (lastWasTag) {
        pos = m_content.indexOf(QLatin1Char('>'), lastPos);
        if (pos == -1) {
            // Couldn't find what closes the tag,
            // trying to find if something else is being opened
            pos = m_content.indexOf(QLatin1Char('<'), lastPos);
            if (pos == -1) {
                m_content.append(QLatin1Char('>'));
                pos = m_content.size();
            } else {
                // Fix closing the last tag
                m_content.insert(pos, QLatin1Char('>'));
            }
        } else {
            ++pos;
        }
    } else {
        pos = m_content.indexOf(QLatin1Char('<'), lastPos);
    }
    qDebug() << "token" << m_content.size() << pos << lastPos << lastWasTag << m_content.midRef(lastPos, pos - lastPos);

    if (pos == lastPos) {
        // if the string size would be empty try the next token
        return tokens(pos, !lastWasTag, parent);
    } else if (pos == -1) {
        if (m_content.size() == lastPos - 1) {
            return ret;
        } else {
            node = createNode(pos, lastPos, !lastWasTag, parent);
            if (node) {
                ret.append(node);
            }
        }
    } else {
        node = createNode(pos, lastPos, !lastWasTag, parent);
        if (node) {
            ret.append(node);
        }

        if (node && node->end) {
            // if it's an ending node or it's
            // plain text the parent is still the same
            if (parent && parent != m_root) {
                ret.append(tokens(pos, !lastWasTag, parent->parent));
            } else {
                ret.append(tokens(pos, !lastWasTag, m_root));
            }
        } else if (node && node->plainText) {
            // Plain text nodes use the current parent
            ret.append(tokens(pos, !lastWasTag, parent));
        } else if (node) {
            // A new tag was created use node as parent
            ret.append(tokens(pos, !lastWasTag, node));
        } else {
            // No node was created continue with current parent
            ret.append(tokens(pos, !lastWasTag, parent));
        }
    }

    return ret;
}

HTMLPurifierNode *HTMLPurifier::createNode(int &pos, int lastPos, bool plainText, HTMLPurifierNode *parent)
{
    QStringRef part;
    QStringRef tag;
    bool closeTag = false;
    bool isPlainText = plainText;
    if (!plainText) {
        QCharRef secondTagChar = m_content[lastPos + 1];
        // second char of a tag must be a letter or a /
        // <div> or </div>
        if (secondTagChar.isLetter() || secondTagChar == QLatin1Char('/')) {
            bool lastWasSpace = true;
            int i = lastPos;
            while (i < pos) {
                QCharRef c = m_content[i];
                if (c.isSpace() && lastWasSpace) {
                    // Remove spaces
                    m_content.remove(i, 1);
                    --pos;
                    continue;
                } else if (c.isUpper()) {
                    // Lower case tags
                    c = c.toLower();
                }
                ++i;
            }
            part = m_content.midRef(lastPos, pos - lastPos);

            // Check if it's a closing tag
            int startPos = 1;
            if (part.at(1) == QLatin1Char('/')) {
                startPos = 2;
                closeTag = true;
            } else if (part.at(part.size() - 1) == QLatin1Char('/')) {
                closeTag = true;
            }

            // get the tag type
            int spacePos = part.indexOf(QLatin1Char(' '), startPos);
            if (spacePos == -1) {
                tag = part.mid(startPos, part.size() - startPos - 1);
            } else {
                tag = part.mid(startPos, part.size() - spacePos);
            }

            if (closeTag && parent->type != tag) {
                 qWarning() << "Stray " << tag << parent->type << " tag removed";
                 m_content.remove(lastPos, pos - lastPos);
                 pos = lastPos;
                 return 0;
            }
        } else {
            // tags such as < strong> are not valid
            qDebug() << "------------" << m_content.at(lastPos) << m_content[lastPos + 1] << m_content[lastPos + 1].isLetter();
            qDebug() << "------------" << m_content.mid(0, pos);
            m_content.replace(lastPos, 1, QLatin1String("&lt;"));
            qWarning() << "Unescaped less-than sign (<) should be &lt;";
            pos += 2;
            m_content.replace(pos, 1, QLatin1String("&gt;"));
            pos += 4;
            qDebug() << "------------" << m_content.mid(0, pos);

            part = m_content.midRef(lastPos, pos - lastPos);
            isPlainText = true;
        }
    } else {
        part = m_content.midRef(lastPos, pos - lastPos);
    }
    qDebug() << "create token" << m_content.size() << pos << lastPos << plainText << part << tag << closeTag;

    HTMLPurifierNode *node = new HTMLPurifierNode;
    node->text = part;
    node->plainText = isPlainText;
    node->parent = parent;
    node->end = closeTag;
    node->type = tag;
    parent->children.append(node);

    return node;
}

void HTMLPurifier::dumpTree(HTMLPurifierNode *root, int level)
{
    qDebug() << QByteArray("-").repeated(level).data() << ">" << root->text;
    Q_FOREACH (HTMLPurifierNode *node, root->children) {
        dumpTree(node, level + 1);
    }
}

