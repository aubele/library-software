#ifndef CFGKEY_H
#define CFGKEY_H
#include "variousvalue.h"

//==============================================
//=============== CfgKey =======================
// represents a configuration key
//==============================================
class CfgKey
{
private:
    VariousValue mValue;
    bool mIsReadOnly;
    QString mComment;
    QList<QString> mPossibleValues;
    VariousValue* mMinValue;
    VariousValue* mMaxValue;
    int *mMaxLength;

public:
    CfgKey(VariousValue initValue, QString comment, bool isReadOnly = false);
    ~CfgKey();
    VariousValue getValue() noexcept;
    bool setValue(QString value) noexcept;
    VariousValue getMinValue();
    VariousValue getMaxValue();
    int getMaxLength();
    bool setMinValue(VariousValue minValue) noexcept;
    bool setMaxValue(VariousValue maxValue) noexcept;
    bool setMaxLength(int value) noexcept;
    VariousValue::type getType() noexcept;
    QString getTypeAsText() noexcept;
    bool isReadOnly() noexcept;
    bool addPossibleValue(QString value) noexcept;
    QList<QString> getPossibleValues() noexcept;
    bool hasPossibleValues() noexcept;
    bool hasMinValue() noexcept;
    bool hasMaxValue() noexcept;
    bool hasMaxLength() noexcept;
    QString getComment() noexcept;
    void setReadOnly(bool value) noexcept;
};

#endif // CFGKEY_H
