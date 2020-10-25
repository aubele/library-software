#include "cfgkey.h"


//======================================
//===== CfgKey   =======================
// Implementations of CfgKey methods
//======================================

//--------------------------------------
// constructor
// Parameter: KeyValue object containing value, comment and true if the value cannot be changed
//--------------------------------------
CfgKey::CfgKey(VariousValue initValue, QString comment, bool isReadOnly) :
    mValue(initValue), mIsReadOnly(isReadOnly), mComment(comment), mMinValue(nullptr), mMaxValue(nullptr), mMaxLength(nullptr)
{
}

//--------------------------------------
// destructor
// destroys the min and max value members
//--------------------------------------
CfgKey::~CfgKey()
{
    delete mMaxValue;
    mMaxValue = nullptr;
    delete mMinValue;
    mMinValue = nullptr;
    delete mMaxLength;
    mMaxLength = nullptr;
}


//--------------------------------------
// getValue() returns the value of the key
//--------------------------------------
VariousValue CfgKey::getValue() noexcept
{
    return mValue;
}

//--------------------------------------
// setValue() sets a new value unless the key is readonly
// or the new value is not allowed
// Parameter: value as QString
//--------------------------------------
bool CfgKey::setValue(QString value) noexcept
{
    if(!mIsReadOnly)
    {
        bool allowNewValue = true;
        if (mPossibleValues.size() > 0)
        {
            if (!mPossibleValues.contains(value))
                allowNewValue = false;
        }
        if (allowNewValue && mMinValue != nullptr)
        {
            if (value.toDouble() < mMinValue->asDouble())
                allowNewValue = false;
        }
        if (allowNewValue && mMaxValue != nullptr)
        {
            if (value.toDouble() > mMaxValue->asDouble())
                allowNewValue = false;
        }
        if (allowNewValue && mMaxLength != nullptr)
        {
            if (value.length() > *mMaxLength)
                allowNewValue = false;
        }
        if (allowNewValue)
        {
            return mValue.setValue(value);
        }
    }
    return false;
}

//--------------------------------------
// getMinValue() returns the minimum value for the key
// throws an error if no minvalue is set
//--------------------------------------
VariousValue CfgKey::getMinValue()
{
    if (mMinValue == nullptr)
        throw QString("Error in code: The configuration key does not contain a minimum value! Test it with hasMinValue().");
    return *mMinValue;
}

//--------------------------------------
// getMaxValue() returns the maximum value for the key
// throws an error if no minvalue is set
//--------------------------------------
VariousValue CfgKey::getMaxValue()
{
    if (mMaxValue == nullptr)
        throw QString("Error in code: The configuration key does not contain a maximum value! Test it with hasMaxValue().");
    return *mMaxValue;
}

//--------------------------------------
// getMaxValue() returns the maximum value for the key
// throws an error if no minvalue is set
//--------------------------------------
int CfgKey::getMaxLength()
{
    if (mMaxLength == nullptr)
        throw QString("Error in code: The configuration key does not contain a maximum length! Test it with hasMaxLength().");
    return *mMaxLength;
}

//--------------------------------------
// setMinValue() sets a minimum value
// Parameter: minimum value as KeyValue
//--------------------------------------
bool CfgKey::setMinValue(VariousValue minValue) noexcept
{
    if (minValue.getType() == mValue.getType())
    {
        mMinValue = new VariousValue(mValue.getType(), minValue.asText());
        return true;
    }
    else
        return false;
}

//--------------------------------------
// setMaxValue() sets a maximum value
// Parameter: maximum value as KeyValue
//--------------------------------------
bool CfgKey::setMaxValue(VariousValue maxValue) noexcept
{
    if (maxValue.getType() == mValue.getType())
    {
        mMaxValue = new VariousValue(mValue.getType(), maxValue.asText());
        return true;
    }
    else
        return false;
}

//--------------------------------------
// setMaxLength() sets a maximum length for the string
// Parameter: maximum length as int
//--------------------------------------
bool CfgKey::setMaxLength(int value) noexcept
{
    VariousValue::type type = mValue.getType();
    if (type == VariousValue::Text || type == VariousValue::SelectEntryText || type == VariousValue::SelectText)
    {
        mMaxLength = new int;
        *mMaxLength = value;
        return true;
    }
    return false;
}

//--------------------------------------
// getType() returns the type of the configuration key
//--------------------------------------
VariousValue::type CfgKey::getType() noexcept
{
    return mValue.getType();
}

//--------------------------------------
// getTypeAsText() returns the type of the configuration key as text
//--------------------------------------
QString CfgKey::getTypeAsText() noexcept
{
    QString rueckgabe;

    switch(mValue.getType())
    {
        case VariousValue::Bool:
        {
            rueckgabe = "bool";
            break;
        }
        case VariousValue::Double:
        {
            rueckgabe = "double";
            break;
        }
        case VariousValue::Int:
        {
            rueckgabe = "int";
            break;
        }
        case VariousValue::Text:
        {
            rueckgabe = "string";
            break;
        }
        case VariousValue::SelectText:
        {
            rueckgabe = "chosestring";
            break;
        }
        case VariousValue::SelectEntryText:
        {
            rueckgabe = "choseentrystring";
            break;
        }
        case VariousValue::RGB_Color:
        {
            rueckgabe = "rgbcolor";
            break;
        }
    }
    return rueckgabe;
}

//--------------------------------------
// isReadOnly() returns whether the key is readonly or not
//--------------------------------------
bool CfgKey::isReadOnly() noexcept
{
    return mIsReadOnly;
}

//--------------------------------------
// addPossibleValue() adds a new possible value
// Parameter: value as QString
//--------------------------------------
bool CfgKey::addPossibleValue(QString value) noexcept
{
    bool returnValue = false;
    try
    {
        VariousValue val(getType(), value);
        mPossibleValues.append(val.asText());
        returnValue = true;
    }
    catch (QString)
    {
        returnValue = false;
    }
    return returnValue;
}

//--------------------------------------
// getPossibleValue() returns the possible values
//--------------------------------------
QList<QString> CfgKey::getPossibleValues() noexcept
{
    return mPossibleValues;
}

//--------------------------------------
// hasPossibleValues() returns whether the key has possible values or not
//--------------------------------------
bool CfgKey::hasPossibleValues() noexcept
{
    if (mPossibleValues.size() > 0)
        return true;
    else
        return false;
}

//--------------------------------------
// hasMinValue() returns whether the key has a minimum value or not
//--------------------------------------
bool CfgKey::hasMinValue() noexcept
{
    if (mMinValue)
        return true;
    else
        return false;
}

//--------------------------------------
// hasMaxValue() returns wether the key has a maximum value or not
//--------------------------------------
bool CfgKey::hasMaxValue() noexcept
{
    if (mMaxValue)
        return true;
    else
        return false;
}

//--------------------------------------
// hasMaxLength() returns wether the key has a maximum length or not
//--------------------------------------
bool CfgKey::hasMaxLength() noexcept
{
    if (mMaxLength == nullptr)
        return false;
    else
        return true;
}

//--------------------------------------
// getComment() returns the comment of the key
//--------------------------------------
QString CfgKey::getComment() noexcept
{
    return mComment;
}

//--------------------------------------
// setReadOnly sets wether the key is read only or not
// Parameter: flag if readonly or not
//--------------------------------------
void CfgKey::setReadOnly(bool value) noexcept
{
    mIsReadOnly = value;
}
