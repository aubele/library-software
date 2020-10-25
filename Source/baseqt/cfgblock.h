#ifndef CFGBLOCK_H
#define CFGBLOCK_H
#include "cfgkey.h"
#include <QHash>

class CfgBlock
{
private:
    QString mComment;
    QHash<QString, CfgKey*> mKeylist;

public:
    CfgBlock(QString comment) noexcept;
    ~CfgBlock() noexcept;
    VariousValue getKeyValue(QString keyname, VariousValue defaultValue) noexcept;
    bool addKey(QString keyname, QString keycomment, VariousValue value, bool isReadOnly = false) noexcept;
    bool removeKey(QString keyname) noexcept;
    QString getBlockComment() noexcept;
    QString getKeyComment(QString keyname) noexcept;
    bool setKeyValue(QString keyname, QString value) noexcept;
    VariousValue getMinKeyValue (QString keyname);
    VariousValue getMaxKeyValue(QString keyname);
    int getMaxKeyLength(QString keyname);
    bool setMinKeyValue(QString keyname, VariousValue minValue) noexcept;
    bool setMaxKeyValue(QString keyname, VariousValue maxValue) noexcept;
    bool setMaxKeyLength(QString keyname, int value) noexcept;
    VariousValue::type getKeyType(QString keyname);
    QString getKeyTypeAsText(QString keyname);
    bool isKeyReadOnly(QString keyname);
    bool addPossibleKeyValue(QString keyname, QString value) noexcept;
    QList<QString> getPossibleKeyValues(QString keyname);
    bool hasKeyPossibleValues(QString keyname);
    bool hasKeyMinValue(QString keyname);
    bool hasKeyMaxValue(QString keyname);
    bool hasKeyMaxLength(QString keyname);
    QList<QString> getAllKeys() noexcept;
    bool setKeyReadOnly(QString keyname, bool value) noexcept;
};

#endif // CFGBLOCK_H
