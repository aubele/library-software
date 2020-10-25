#include "cfgblock.h"

//======================================
//===== CfgBlock =======================
// Implementations of CfgBlock methods
//======================================

//--------------------------------------
// constructor
// Parameter: comment
//--------------------------------------
CfgBlock::CfgBlock(QString comment) noexcept :
    mComment(comment)
{
}

//--------------------------------------
// destructor deletes all CfgKeys in list
//--------------------------------------
CfgBlock::~CfgBlock() noexcept
{
    QList<QString> keys = mKeylist.keys();

    foreach (QString key, keys)
    {
        CfgKey* todeletekey = mKeylist[key];
        mKeylist.remove(key);
        delete todeletekey;
    }
}

//--------------------------------------
// getKeyValue returns the value of a key or the defaultValue if the key does not exist
// Parameter: keyname - name of the key to get its value and defaultValue - value which is returned
// if key does not exist
//--------------------------------------
VariousValue CfgBlock::getKeyValue(QString keyname, VariousValue defaultValue) noexcept
{
    if (mKeylist.contains(keyname))
        return mKeylist[keyname]->getValue();
    else
        return defaultValue;
}

//--------------------------------------
// addKey adds a new key to the block. Returns true if the key could be added
// Parameter: name of the key, comment for the key, initial value and flag wether the key is readonly
//--------------------------------------
bool CfgBlock::addKey(QString keyname, QString keycomment, VariousValue value, bool isReadOnly) noexcept
{
    if (mKeylist.contains(keyname))
        return false;
    else
    {
        CfgKey* newkey = new CfgKey(value, keycomment, isReadOnly);
        mKeylist.insert(keyname, newkey);
        return true;
    }
}

//--------------------------------------
// removeKey removes a key from the block and returns true if a key was removed
// Parameter: keyname - name of the key which should be removed
//--------------------------------------
bool CfgBlock::removeKey(QString keyname) noexcept
{
    if (mKeylist.contains(keyname))
    {
        CfgKey* keytodelete = mKeylist[keyname];
        mKeylist.remove(keyname);
        delete keytodelete;
        return true;
    }
    return false;
}

//--------------------------------------
// getBlockComment() returns the comment
//--------------------------------------
QString CfgBlock::getBlockComment() noexcept
{
    return mComment;
}

//--------------------------------------
// getKeyComment() returns the comment of the given keyname
// Parameter: name of the key
//--------------------------------------
QString CfgBlock::getKeyComment(QString keyname) noexcept
{
    if (mKeylist.contains(keyname))
        return mKeylist[keyname]->getComment();
    else
        return "";
}

//--------------------------------------
// setKeyValue sets a new value for the given key
// Parameter: name of the key and value as string
// if the value does not fit the method returns false otherwise true
//--------------------------------------
bool CfgBlock::setKeyValue(QString keyname, QString value) noexcept
{
    if(mKeylist.contains(keyname))
    {
        return mKeylist[keyname]->setValue(value);
    }
    else
    {
        return false;
    }
}

//--------------------------------------
// getMinValue returns the minimum value for a given key
// Parameter: name of the key
//--------------------------------------
VariousValue CfgBlock::getMinKeyValue (QString keyname)
{
    if (mKeylist.contains(keyname))
    {
        return mKeylist[keyname]->getMinValue();
    }
    else
    {
        throw QString("Error in code: key: " + keyname + " does not exist in block!");
    }
}

//--------------------------------------
// getMaxValue return the maximum value for a given key
// Parameter: name of the key
//--------------------------------------
VariousValue CfgBlock::getMaxKeyValue(QString keyname)
{
    if (mKeylist.contains(keyname))
    {
        return mKeylist[keyname]->getMaxValue();
    }
    else
    {
        throw QString("Error in code: key: " + keyname + " does not exist in block!");
    }
}

//--------------------------------------
// getMaxLength returns the maximum length of the given key
// Parameter: name of the key
//--------------------------------------
int CfgBlock::getMaxKeyLength(QString keyname)
{
    if (mKeylist.contains(keyname))
    {
        return mKeylist[keyname]->getMaxLength();
    }
    else
    {
        throw QString("Error in code: key: " + keyname + " does not exist in block!");
    }
}

//--------------------------------------
// setMinValue sets a new minValue on the given key
// Parameter: name of the key and minimum value
//--------------------------------------
bool CfgBlock::setMinKeyValue(QString keyname, VariousValue minValue) noexcept
{
    if (mKeylist.contains(keyname))
    {
        return mKeylist[keyname]->setMinValue(minValue);
    }
    else
        return false;
}

//--------------------------------------
// setMaxValue sets a new maxValue on the given key
// Parameter: name of the key and maximum value
//--------------------------------------
bool CfgBlock::setMaxKeyValue(QString keyname, VariousValue maxValue) noexcept
{
    if (mKeylist.contains(keyname))
    {
        return mKeylist[keyname]->setMaxValue(maxValue);
    }
    else
        return false;
}

//--------------------------------------
// setMaxLength sets the maximum length on the given key
// Parameter: name of the key and maximum length
//--------------------------------------
bool CfgBlock::setMaxKeyLength(QString keyname, int value) noexcept
{
    if (mKeylist.contains(keyname))
    {
        return mKeylist[keyname]->setMaxLength(value);
    }
    else
        return false;
}

//--------------------------------------
// getType returns the type of the given key
// Parameter: name of the key
//--------------------------------------
VariousValue::type CfgBlock::getKeyType(QString keyname)
{
    if (mKeylist.contains(keyname))
    {
        return mKeylist[keyname]->getType();
    }
    else
        throw QString("Error in code: key: " + keyname + " does not exist in block!");
}

//--------------------------------------
// getKeyTypeAsText returns the type of the given key
// Parameter: name of the key
//--------------------------------------
QString CfgBlock::getKeyTypeAsText(QString keyname)
{
    if (mKeylist.contains(keyname))
    {
        return mKeylist[keyname]->getTypeAsText();
    }
    else
        throw QString("Error in code: block does not contain " + keyname + "!");
}

//--------------------------------------
// isKeyReadOnly returns wether the given key is readonly or not
// Parameter: name of the key
//--------------------------------------
bool CfgBlock::isKeyReadOnly(QString keyname)
{
    if (mKeylist.contains(keyname))
    {
        return mKeylist[keyname]->isReadOnly();
    }
    else
        throw QString("Error in code: key: " + keyname + " does not exist in block!");
}

//--------------------------------------
// addPossibleKeyValue adds a new possible value to the given key returns true if the value could be added
// Paramter: name of the key and value as string
//--------------------------------------
bool CfgBlock::addPossibleKeyValue(QString keyname, QString value) noexcept
{
    if (mKeylist.contains(keyname))
    {
        return mKeylist[keyname]->addPossibleValue(value);
    }
    else
        return false;
}

//--------------------------------------
// getPossibleValues returns a list of all possible values of the given key
// Parameter: name of the key
//--------------------------------------
QList<QString> CfgBlock::getPossibleKeyValues(QString keyname)
{
    if (mKeylist.contains(keyname))
    {
        return mKeylist[keyname]->getPossibleValues();
    }
    else
        throw QString("Error in code: key: " + keyname + " does not exist in block!");
}

//--------------------------------------
// hasKeyPossibleValues returns wether the key does have possible values
// Paramter: name of the key
//--------------------------------------
bool CfgBlock::hasKeyPossibleValues(QString keyname)
{
    if (mKeylist.contains(keyname))
    {
        return mKeylist[keyname]->hasPossibleValues();
    }
    else
        throw QString("Error in code: key: " + keyname + " does not exist in block!");
}

//--------------------------------------
// hasKeyMinValue returns wether the key does have a minimum value
// Paramter: name of the key
//--------------------------------------
bool CfgBlock::hasKeyMinValue(QString keyname)
{
    if (mKeylist.contains(keyname))
    {
        return mKeylist[keyname]->hasMinValue();
    }
    else
        throw QString("Error in code: key: " + keyname + " does not exist in block!");
}

//--------------------------------------
// hasKeyMaxValue returns wether the key does have a maximum value
// Paramter: name of the key
//--------------------------------------
bool CfgBlock::hasKeyMaxValue(QString keyname)
{
    if (mKeylist.contains(keyname))
    {
        return mKeylist[keyname]->hasMaxValue();
    }
    else
        throw QString("Error in code: key: " + keyname + " does not exist in block!");
}

//--------------------------------------
// hasKeyPossibleValues returns wether the key does have a maximum length
// Paramter: name of the key
//--------------------------------------
bool CfgBlock::hasKeyMaxLength(QString keyname)
{
    if (mKeylist.contains(keyname))
    {
        return mKeylist[keyname]->hasMaxLength();
    }
    else
        throw QString("Error in code: key: " + keyname + " does not exist in block!");
}

//--------------------------------------
// getAllKeys returns a list of all key names
//--------------------------------------
QList<QString> CfgBlock::getAllKeys() noexcept
{
    return mKeylist.keys();
}

//--------------------------------------
// setKeyReadOnly sets a key as readonly
// Parameter: name of the key and readonly flag
//--------------------------------------
bool CfgBlock::setKeyReadOnly(QString keyname, bool value) noexcept
{
    if (mKeylist.contains(keyname))
    {
        mKeylist[keyname]->setReadOnly(value);
        return true;
    }
    return false;
}
