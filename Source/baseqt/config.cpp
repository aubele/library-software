#include "config.h"
#include <QFile>
#include <QTextStream>
#include <QDir>

//=====================================
//==== Implementations of Config ======
//=====================================

//--------------------------------------
// constructor
// Parameter: cfgName - name of the config and path - Path to the config
//--------------------------------------
Config::Config(QString cfgName, QString path) :
    mCfgName(cfgName), mPath(path), mIsModified(false)
{
}

//--------------------------------------
// destructor deletes all items in the Hash
//--------------------------------------
Config::~Config()
{
    QList<QString> keys = mBlocklist.keys();

    foreach (QString key, keys)
    {
        CfgBlock* todeleteblock = mBlocklist[key];
        mBlocklist.remove(key);
        delete todeleteblock;
    }
}

//--------------------------------------
// getConfigName returns the name of the config
//--------------------------------------
QString Config::getConfigName() noexcept
{
    return mCfgName;
}

//--------------------------------------
// getConfigComment returns the comment of the config
//--------------------------------------
QString Config::getConfigComment() noexcept
{
    return mComment;
}

//--------------------------------------
// isModified returns if the config file has changed
//--------------------------------------
bool Config::isModified() noexcept
{
    return mIsModified;
}

//--------------------------------------
// attach adds a new observer to the config
// Parameter: observer object
//--------------------------------------
void Config::attach(ConfigObserver* observer) noexcept
{
    if(!mObserver.contains(observer))
    {
        mObserver.append(observer);
    }
}

//--------------------------------------
// detach removes an observer
// Parameter: observer object to remove
//--------------------------------------
void Config::detach(ConfigObserver* observer) noexcept
{
    if (mObserver.contains(observer))
        mObserver.removeOne(observer);
}

//--------------------------------------
// getValueForKeyInBlock returns the value of the specified key or the default value
// Parameter: name of the block, name of the key and the defaultvalue
//--------------------------------------
VariousValue Config::getValueForKeyInBlock(QString blockname, QString keyname, VariousValue defaultvalue) noexcept
{
    if (mBlocklist.contains(blockname))
        return mBlocklist[blockname]->getKeyValue(keyname, defaultvalue);
    else
        return defaultvalue;
}

//--------------------------------------
// getValueForKeyInBlock returns the value of the specified key or the default value
// Parameter: name of the block, name of the key and the defaultvalue
//--------------------------------------
bool Config::addBlock(QString blockname, QString blockcomment) noexcept
{
    if (!mBlocklist.contains(blockname))
    {
        mBlocklist.insert(blockname, new CfgBlock(blockcomment));
        mIsModified = true;

        ConfigObserver *observer;
        foreach(observer, mObserver)
        {
            observer->cfgblockchanged(blockname, ConfigObserver::ADDED);
        }
        return true;
    }
    else
        return false;
}

//--------------------------------------
// removeBlock removes the specified block
// Parameter: name of the block to remove
//--------------------------------------
void Config::removeBlock (QString blockname) noexcept
{
    if (mBlocklist.contains(blockname))
    {
        CfgBlock* toDelete = mBlocklist[blockname];
        mBlocklist.remove(blockname);

        ConfigObserver *observer;
        foreach(observer, mObserver)
        {
            observer->cfgblockchanged(blockname, ConfigObserver::REMOVED);
        }
        delete toDelete;
    }
}

//--------------------------------------
// getCommentOfBlock returns the comment of the specified block
// Parameter: name of the block
//--------------------------------------
QString Config::getCommentOfBlock(QString blockname)
{
    if (mBlocklist.contains(blockname))
        return mBlocklist[blockname]->getBlockComment();
    else
        throw QString("Error in code: Config does not contain block: " + blockname + "!");
}

//--------------------------------------
// getAllBlocks returns a list of all block names
//--------------------------------------
QList<QString> Config::getAllBlocks() noexcept
{
    return mBlocklist.keys();
}

//--------------------------------------
// getAllKeysOfBlock returns a list of all key names of the specified block
// Parameter: name of the block
//--------------------------------------
QList<QString> Config::getAllKeysOfBlock(QString blockname)
{
    if (mBlocklist.contains(blockname))
    {
        return mBlocklist[blockname]->getAllKeys();
    }
    else
        throw QString("Error in code: The block " + blockname + " does not exist in the config!");
}

//--------------------------------------
// addKeyToBlock adds a new key to the specified block
// Parameter: name of the block, name of the key, comment of the key, value of the key and wether the key is readonly
//--------------------------------------
bool Config::addKeyToBlock(QString blockname, QString keyname, QString keycomment, VariousValue value, bool isReadOnly) noexcept
{
    if (mBlocklist.contains(blockname))
    {
        if( mBlocklist[blockname]->addKey(keyname, keycomment, value, isReadOnly))
        {
            ConfigObserver* observer;
            foreach(observer, mObserver)
                observer->cfgkeyofblockchanged(blockname, keyname, ConfigObserver::ADDED);
            return true;
        }
        return false;
    }
    else
        return false;
}

//--------------------------------------
// removeKeyFromBlock removes the specified key form the specified block
// Parameter: name of the block and name of the key to delete
//--------------------------------------
bool Config::removeKeyFromBlock(QString blockname, QString keyname) noexcept
{
    if (mBlocklist.contains(blockname))
    {
        if (mBlocklist[blockname]->removeKey(keyname))
        {
            ConfigObserver* observer;
            foreach (observer, mObserver)
                observer->cfgkeyofblockchanged(blockname, keyname, ConfigObserver::REMOVED);
            return true;
        }
        return false;
    }
    else
        return false;
}

//--------------------------------------
// getCommentOfKeyInBlock returns the comment of the specified key
// Parameter: name of the block and name of the key
//--------------------------------------
QString Config::getCommentOfKeyInBlock(QString blockname, QString keyname)
{
    if (mBlocklist.contains(blockname))
    {
        return mBlocklist[blockname]->getKeyComment(keyname);
    }
    else
        throw QString("Error in code: config does not contain block: " + blockname + " !");
}

//--------------------------------------
// setValueForKeyInBlock sets a new value on the specified key
// Parameter: name of the block and name of the key as well as the value
//--------------------------------------
bool Config::setValueForKeyInBlock(QString blockname, QString keyname, QString value)
{
    if (mBlocklist.contains(blockname))
    {
        if (mBlocklist[blockname]->setKeyValue(keyname, value))
        {
            ConfigObserver* observer;
            foreach(observer, mObserver)
                observer->cfgkeyofblockchanged(blockname, keyname, ConfigObserver::MODIFIED);

            return true;
        }
        return false;
    }
    else
        return false;
}

//--------------------------------------
// getMinValueOfKeyInBlock returns the minimum value of the key
// Parameter: name of the block and name of the key as well as the value
//--------------------------------------
VariousValue Config::getMinValueOfKeyInBlock (QString blockname, QString keyname)
{
    if (mBlocklist.contains(blockname))
    {
        return mBlocklist[blockname]->getMinKeyValue(keyname);
    }
    else
        throw QString("Error in code: config does not contain block " + blockname + "!");
}

//--------------------------------------
// getMaxValueOfKeyInBlock returns the maximum value of the key
// Parameter: name of the block and name of the key as well as the value
//--------------------------------------
VariousValue Config::getMaxValueOfKeyInBlock(QString blockname, QString keyname)
{
    if (mBlocklist.contains(blockname))
    {
        return mBlocklist[blockname]->getMaxKeyValue(keyname);
    }
    else
        throw QString("Error in code: config does not contain block " + blockname + "!");
}

//--------------------------------------
// getMaxLengthOfKeyInBlock returns the maximum length of the key
// Parameter: name of the block and name of the key as well as the value
//--------------------------------------
int Config::getMaxLengthOfKeyInBlock(QString blockname, QString keyname)
{
    if (mBlocklist.contains(blockname))
    {
        return mBlocklist[blockname]->getMaxKeyLength(keyname);
    }
    else
        throw QString("Error in code: config does not contain block " + blockname + "!");
}

//--------------------------------------
// setMinValueForKeyInBlock sets a new minimum value on the key
// Parameter: name of the block and name of the key as well as the value
//--------------------------------------
bool Config::setMinValueForKeyInBlock(QString blockname, QString keyname, VariousValue minValue) noexcept
{
    if (mBlocklist.contains(blockname))
    {
        if(mBlocklist[blockname]->setMinKeyValue(keyname, minValue))
        {
            ConfigObserver* observer;
            foreach(observer, mObserver)
                observer->cfgkeyofblockchanged(blockname, keyname, ConfigObserver::MODIFIED);
            return true;
        }
        return false;
    }
    else
        return false;
}

//--------------------------------------
// setMaxValueForKeyInBlock sets a new maximum value on the key
// Parameter: name of the block and name of the key as well as the value
//--------------------------------------
bool Config::setMaxValueForKeyInBlock(QString blockname, QString keyname, VariousValue maxValue) noexcept
{
    if (mBlocklist.contains(blockname))
    {
        if(mBlocklist[blockname]->setMaxKeyValue(keyname, maxValue))
        {
            ConfigObserver* observer;
            foreach(observer, mObserver)
                observer->cfgkeyofblockchanged(blockname, keyname, ConfigObserver::MODIFIED);
            return true;
        }
        return false;
    }
    else
        return false;
}

//--------------------------------------
// setMaxLengthForKeyInBlock sets a new maximum length on the key
// Parameter: name of the block and name of the key as well as the value
//--------------------------------------
bool Config::setMaxLengthForKeyInBlock(QString blockname, QString keyname, int value) noexcept
{
    if (mBlocklist.contains(blockname))
    {
        if(mBlocklist[blockname]->setMaxKeyLength(keyname, value))
        {
            ConfigObserver* observer;
            foreach(observer, mObserver)
                observer->cfgkeyofblockchanged(blockname, keyname, ConfigObserver::MODIFIED);
            return true;
        }
        return false;
    }
    else
        return false;
}

//--------------------------------------
// getTypeOfKeyInBlcok return the type of the key of block
// Parameter: name of the block and name of the key
//--------------------------------------
VariousValue::type Config::getTypeOfKeyInBlock(QString blockname, QString keyname)
{
    if (mBlocklist.contains(blockname))
    {
        return mBlocklist[blockname]->getKeyType(keyname);
    }
    else
        throw QString("Error in code: config does not contain block " + blockname + "!");
}

//--------------------------------------
// isKeyReadOnlyOfBlock returns if the key is readonly
// Parameter: name of the block and name of the key
//--------------------------------------
bool Config::isKeyReadOnlyOfBlock(QString blockname, QString keyname)
{
    if (mBlocklist.contains(blockname))
    {
        return mBlocklist[blockname]->isKeyReadOnly(keyname);
    }
    else
        throw QString("Error in code: config does not contain block " + blockname + "!");
}

//--------------------------------------
// addPossibleValueToKeyInBlock adds a new possible value to the key
// Parameter: name of the block and name of the key as well as the value
//--------------------------------------
bool Config::addPossibleValueToKeyInBlock(QString blockname, QString keyname, QString value) noexcept
{
    if (mBlocklist.contains(blockname))
    {
        if (mBlocklist[blockname]->addPossibleKeyValue(keyname, value))
        {
            ConfigObserver* observer;
            foreach (observer, mObserver)
                observer->cfgkeyofblockchanged(blockname, keyname, ConfigObserver::MODIFIED);
            return true;
        }
        return false;
    }
    else
        return false;
}

//--------------------------------------
// getPossibleValuesOfKeyInBlock returns a list of all possible values of the key
// Parameter: name of the block and name of the key
//--------------------------------------
QList<QString> Config::getPossibleValuesOfKeyInBlock(QString blockname, QString keyname)
{
    if (mBlocklist.contains(blockname))
    {
        return mBlocklist[blockname]->getPossibleKeyValues(keyname);
    }
    else
        throw QString("Error in code: The config does not contain the block " + blockname + "!");
}

//--------------------------------------
// hasKeyPosibleValuesInBlock returns if the key of block has possible values
// Parameter: name of the block and name of the key as well as the value
//--------------------------------------
bool Config::hasKeyPossibleValuesInBlock(QString blockname, QString keyname)
{
    if (mBlocklist.contains(blockname))
    {
        return mBlocklist[blockname]->hasKeyPossibleValues(keyname);
    }
    else
        throw QString("Error in code: The config does not contain the block " + blockname + "!");
}

//--------------------------------------
// hasKeyMinValueInBlock returns if the key of block has a minimum value
// Parameter: name of the block and name of the key as well as the value
//--------------------------------------
bool Config::hasKeyMinValueInBlock(QString blockname, QString keyname)
{
    if (mBlocklist.contains(blockname))
    {
        return mBlocklist[blockname]->hasKeyMinValue(keyname);
    }
    else
        throw QString("Error in code: The config does not contain the block " + blockname + "!");
}

//--------------------------------------
// hasKeyMaxValueInBlock returns if the key of block has a maximum value
// Parameter: name of the block and name of the key as well as the value
//--------------------------------------
bool Config::hasKeyMaxValueInBlock(QString blockname, QString keyname)
{
    if (mBlocklist.contains(blockname))
    {
        return mBlocklist[blockname]->hasKeyMaxValue(keyname);
    }
    else
        throw QString("Error in code: The config does not contain the block " + blockname + "!");
}

//--------------------------------------
// hasKeyMaxLenghtInBlock returns if the key of block has a maximum length
// Parameter: name of the block and name of the key as well as the value
//--------------------------------------
bool Config::hasKeyMaxLengthInBlock(QString blockname, QString keyname)
{
    if (mBlocklist.contains(blockname))
    {
        return mBlocklist[blockname]->hasKeyMaxLength(keyname);
    }
    else
        throw QString("Error in code: The config does not contain the block " + blockname + "!");
}

//--------------------------------------
// setKeyInReadOnly sets a key as readonly
// Parameter: name of the block, name of the key and readonly flag
//--------------------------------------
bool Config::setKeyInBlockReadOnly(QString blockname, QString keyname, bool value) noexcept
{
    if (mBlocklist.contains(blockname))
        return mBlocklist[blockname]->setKeyReadOnly(keyname, value);
    else
        return false;
}


//======================================
// Private Functions
//======================================
//--------------------------------------
// saveKeys saves all keys of a block
// Parameter: Reference to a QTextStream,
//--------------------------------------
inline void saveKeys(CfgBlock* block, QTextStream &stream)
{
    QString key;
    QList<QString> list = block->getAllKeys();
    foreach(key, list)
    {
        // get the comment
        QString comment = block->getKeyComment(key);
        if (!comment.isEmpty())
            stream << "# " + comment << endl;

        // get readonly, key type, key value and key name
        QString readOnlyFront;
        QString readOnlyBack;
        if (block->isKeyReadOnly(key))
        {
            readOnlyBack = "}";
            readOnlyFront = "{";
        }
        stream << readOnlyFront << key << ": " << block->getKeyTypeAsText(key) + " = " + block->getKeyValue(key, VariousValue(VariousValue::Text, "")).asText() << readOnlyBack << endl;

        // get minimum and maxium value if they exist
        if (block->hasKeyMinValue(key))
        {
            if (block->hasKeyMaxValue(key))
                stream << "(" + block->getMinKeyValue(key).asText() + ";" + block->getMaxKeyValue(key).asText() + ")" << endl;
            else
                stream << "(" + block->getMinKeyValue(key).asText() + ")" << endl;
        }

        // get max key length
        if (block->hasKeyMaxLength(key))
            stream << "(" + QString::number(block->getMaxKeyLength(key)) + ")" << endl;

        // get possible values
        if (block->hasKeyPossibleValues(key))
        {
            QListIterator<QString> i(block->getPossibleKeyValues(key));
            QString toWrite;
            while(i.hasNext())
            {
                toWrite += i.next();
                if (i.hasNext())
                    toWrite += "; ";
            }
            stream << "<" + toWrite + ">" << endl;
        }

        if (list.indexOf(key) <= list.size()-1)
        {
            stream << endl;
        }
    }
}

//--------------------------------------
// saveBlock: saves a block and its keys to the config file
// Parameter: Reference to a QTextStream, blockname - name of the block and a pointer to a CfgBlock
//--------------------------------------
inline void saveBlock(QTextStream &stream, QString blockname, CfgBlock* block)
{
    QString comment = block->getBlockComment();
    if (!comment.isEmpty())
        stream << "# " + block->getBlockComment() << endl;
    stream << "[" + blockname + "]" << endl;
    saveKeys(block, stream);
}

//---------------------------------------------
// Function parseKeyLine returns a key with the initial value, the comment and readonly information
// Parameter: line which should be parsed to a key, comment containing the comment for the key and a placeholder for the keyname
//--------------------------------------------
inline void parseKeyLine(QString line, QString comment, QString *keyname, QString blockname, Config* cfg)
{
    bool isReadOnly = false;

    if (line.startsWith("{") && line.endsWith("}"))
    {
        isReadOnly = true;
        line = line.replace("{", "").replace("}", "");
    }
    int index = line.indexOf(":");
    *keyname = line.left(index);

    QString type = line.mid(index+1, line.indexOf("=") - index - 1).trimmed();
    QString value = line.right(line.length() - 1 - line.indexOf("=")).trimmed();

    if (type == "int")
        cfg->addKeyToBlock(blockname, *keyname, comment, VariousValue(VariousValue::Int, value), isReadOnly);
    else if (type == "double")
        cfg->addKeyToBlock(blockname, *keyname, comment, VariousValue(VariousValue::Double, value), isReadOnly);
    else if (type == "string")
        cfg->addKeyToBlock(blockname, *keyname, comment, VariousValue(VariousValue::Text, value), isReadOnly);
    else if (type == "bool")
        cfg->addKeyToBlock(blockname, *keyname, comment, VariousValue(VariousValue::Bool, value), isReadOnly);
    else if (type == "chosestring")
        cfg->addKeyToBlock(blockname, *keyname, comment, VariousValue(VariousValue::SelectText, value), isReadOnly);
    else if (type == "choseentrystring")
        cfg->addKeyToBlock(blockname, *keyname, comment, VariousValue(VariousValue::SelectEntryText, value), isReadOnly);
    else if (type == "rgbcolor")
        cfg->addKeyToBlock(blockname, *keyname, comment, VariousValue(VariousValue::RGB_Color, value), isReadOnly);
}

//---------------------------------------------
// Function readBlocksAndKeysFromFile reads the config-file and returns the comment for the Config
// Parameter: cfg where the blocks and keys will be added and stream with holds the file
//--------------------------------------------
QString readBlocksAndKeysFromFile(QTextStream &stream, Config* cfg)
{
    QString returnValue;
    QString addcomment;
    QString blockname;
    QString keyname;

    // read each line of the file
    while(!stream.atEnd())
    {
        QString line = stream.readLine();

        if (line.isEmpty()) // line is empty: we do nothing
        {}
        else if (line.startsWith("#!")) // is the line a first comment?
        {
            returnValue = line.replace("#!", "").trimmed();
        }
        else if (line.startsWith("#")) // is the line a comment?
        {
            if (addcomment.isEmpty())
                addcomment = line.replace("#", "").trimmed();
        }
        else if (line.startsWith("[") && line.endsWith("]")) // is the line a block?
        {
            keyname = "";
            blockname = line.replace("[", "").replace("]", "");
            cfg->addBlock(blockname, addcomment);
            addcomment = "";
        }
        else if (line.contains(":") && !blockname.isEmpty()) // is the line a key?
        {
            parseKeyLine(line, addcomment, &keyname, blockname, cfg);
            addcomment = "";
        }
        else if (!keyname.isEmpty() && line.contains("(") && line.contains(")") && line.contains(";")) // is the line a min and max value?
        {
            line = line.replace("(", "").replace(")", "");
            QString value = line.left(line.indexOf(";")).trimmed();
            cfg->setMinValueForKeyInBlock(blockname, keyname, VariousValue(cfg->getTypeOfKeyInBlock(blockname,keyname), value));
            value = line.right(line.length() -1 - line.indexOf(";")).trimmed();
            cfg->setMaxValueForKeyInBlock(blockname, keyname, VariousValue(cfg->getTypeOfKeyInBlock(blockname, keyname), value));
        }
        else if (!keyname.isEmpty() && line.contains("(") && line.contains(")")) // is the line a maximum length?
        {
            line = line.replace("(", "").replace(")", "");
            cfg->setMaxLengthForKeyInBlock(blockname, keyname, line.toInt());
        }
        else if (!keyname.isEmpty() && line.contains("<") && line.contains(">")) // is the line a possible value?
        {
            line = line.replace("<", "").replace(">", "");
            QStringList values = line.split(";");
            QString value;
            foreach (value, values)
                cfg->addPossibleValueToKeyInBlock(blockname, keyname, value);
        }
    }

    return returnValue;
}


//======================================
// Last implementations of Config
//======================================
//--------------------------------------
// save: saves the config to the config file
//--------------------------------------
bool Config::save() noexcept
{
    if (mIsModified)
    {
        mIsModified = false;
        QFile file( mPath + "\\" + mCfgName );
        if (!file.open( QIODevice::WriteOnly ))
            return false;
        else
        {
            QTextStream stream(&file);
            //stream.setCodec("latin1");
            if (!mComment.isEmpty())
                stream << "#! " + mComment << endl << endl;
            QHashIterator<QString, CfgBlock*> i(mBlocklist);
            while (i.hasNext())
            {
                i.next();
                saveBlock(stream, i.key(), i.value());
                if(i.hasNext())
                    stream << endl;
            }
            file.close();
            return true;
        }
    }
    else
        return false;
}

//--------------------------------------
// load loads the given configuration file (.config)
//--------------------------------------
bool Config::load() noexcept
{
    mIsModified = false;
    // first remove everything from config
    QList<QString> keys = mBlocklist.keys();

    foreach (QString key, keys)
    {
        CfgBlock* todeleteblock = mBlocklist[key];
        mBlocklist.remove(key);
        delete todeleteblock;
    }

    // and now get the values from the file
    if (mPath.isEmpty())
        mPath = QDir::currentPath();
    QFile file( mPath + "\\" + mCfgName );
    if (!file.open( QIODevice::ReadOnly ))
        return false;
    else
    {
        QTextStream stream(&file);
        //stream.setCodec("latin1");
        mComment = readBlocksAndKeysFromFile (stream, this);

        file.close();
        return true;
    }
}
