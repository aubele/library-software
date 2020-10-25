#ifndef VARIOUSVALUE_H
#define VARIOUSVALUE_H

#include "baseqt_global.h"
#include <QString>
#include <QColor>

//==============================================
//=============== VariosValue =====================
// holds the value of multiple types
//==============================================
class BASEQTSHARED_EXPORT VariousValue
{
public:
    enum type {Int, Double, Text, SelectText, SelectEntryText, Bool, RGB_Color};

    VariousValue(type valuetype, QString value);
    int asInt();
    double asDouble();
    QString asText() noexcept;
    bool asBool();
    QColor asColor();
    bool setValue(QString newValue) noexcept;
    VariousValue::type getType();

private:
    VariousValue::type mType;
    QString mValue;
};

#endif // VARIOUSVALUE_H
