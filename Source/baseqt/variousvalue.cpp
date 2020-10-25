#include "variousvalue.h"

//======================================
//===== KeyValue =======================
// Implementations of KeyValue methods
//======================================

//--------------------------------------
// private function to test if the value is okay for the type
// Parameter: VariousValue::type and value
//--------------------------------------
bool isValueOkay(VariousValue::type valueType, QString value)
{
    bool returnValue = false;

    switch(valueType)
    {
    case VariousValue::Bool:
    {
        value.toInt(&returnValue);
        break;
    }
    case VariousValue::Double:
    {
        value.toDouble(&returnValue);
        break;
    }
    case VariousValue::Int:
    {
        value.toInt(&returnValue);
        break;
    }
    case VariousValue::RGB_Color:
    {
        QStringList list = value.split(",");
        if (list.size() != 3)
            returnValue = false;
        else
        {
           returnValue = isValueOkay(VariousValue::Int, list[0]) && isValueOkay(VariousValue::Int, list[1]) && isValueOkay(VariousValue::Int, list[2]);
        }
        break;
    }
    case VariousValue::SelectEntryText:
    case VariousValue::SelectText:
    case VariousValue::Text:
    {
        returnValue = true;
        break;
    }
    }
    return returnValue;
}

//--------------------------------------
// constructor
// Parameter: valuetype Int, Double, Text or Bool and value value as string
//--------------------------------------
VariousValue::VariousValue(VariousValue::type valuetype, QString value):
    mType (valuetype)
{
    if (valuetype == VariousValue::Double)
    {
        value = value.replace(",", ".");
    }
    if (isValueOkay(valuetype, value))
        mValue =value;
    else
        throw QString("Error in code: value and valuetype do not match!");
}

//--------------------------------------
// asInt(): returns the value as an Integer if possible otherwise throws an error
//--------------------------------------
int VariousValue::asInt()
{
    if (mType == VariousValue::Int)
    {
        return mValue.toInt();
    }
    else if (mType == VariousValue::Bool)
    {
        if (mValue == "1" )
            return 1;
        else
            return 0;
    }
    else
    {
        throw QString("Error in Code: Value of Key is not convertable to int!");
    }
}

//--------------------------------------
// asDouble(): returns the value as a Double if possible otherwise throws an error
//--------------------------------------
double VariousValue::asDouble()
{
    if (mType == VariousValue::Int || mType == VariousValue::Double)
    {
        return mValue.toDouble();
    }
    else if (mType == VariousValue::Bool)
    {
        if (mValue == "1" )
            return 1.0;
        else
            return 0.0;
    }
    else
    {
        throw QString("Error in Code: Value of Key is not convertable to double!");
    }
}

//--------------------------------------
// asText(): returns the value as a text
//--------------------------------------
QString VariousValue::asText() noexcept
{
    return mValue;
}

//--------------------------------------
// asBool(): returns the value as a bool if possible otherwise throws an error
//--------------------------------------
bool VariousValue::asBool()
{
    if (mType == VariousValue::Text || mType == VariousValue::SelectText || mType == VariousValue::SelectEntryText || mType == VariousValue::RGB_Color)
        throw QString("Error in Code: Value of Key is not convertable to bool!");
    else
    {
        if (mValue == "0" || mValue == "0.0")
            return false;
        else
            return true;
    }
}

//--------------------------------------
// asColor(): returns the value as a QColor if possible otherwise throws an error
//--------------------------------------
QColor VariousValue::asColor()
{
    if ( mType != VariousValue::RGB_Color)
        throw QString("Error in Code: Value of Key is not convertable to color!");
    else
    {
        int index = mValue.indexOf(",");
        int red = mValue.left(index).toInt();
        int green = mValue.mid(index+1, mValue.indexOf(",", index + 1) - index - 1).toInt();
        index = mValue.indexOf(",", index + 1);
        int blue = mValue.right(mValue.length()-1-index).toInt();
        return QColor(red, green, blue);
    }
}

//--------------------------------------
// setValue sets overwrites the old value with a new value
// Parameter: String containing the new value
//--------------------------------------
bool VariousValue::setValue(QString newValue) noexcept
{
    if (isValueOkay(mType, newValue))
    {
        mValue = newValue;
        return true;
    }
    else
        return false;
}

//--------------------------------------
// getType() returns the type of the value
//--------------------------------------
VariousValue::type VariousValue::getType()
{
    return mType;
}
