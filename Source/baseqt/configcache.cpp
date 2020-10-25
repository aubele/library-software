#include "configcache.h"
#include <QDir>

//========================
//==== ConfigCache =======
// holds up to three configs
//========================

//------------------------
//-- Initialization ------
//------------------------

QList<Config*> ConfigCache::mListOfConfigs = QList<Config*>();

//------------------------
// Konstruktor
//------------------------
ConfigCache::ConfigCache()
{
}

//------------------------
// getConfig returns the specified config
// Parameter: Name of the Config file
//------------------------
Config* ConfigCache::getConfig (QString configName)
{
    Config* returnValue = nullptr;

    // look in the list if the config is already there
    for (int i = 0; i < mListOfConfigs.size() && returnValue; ++i)
    {
        if (mListOfConfigs.at(i)->getConfigName() == configName)
        {
            returnValue = mListOfConfigs.at(i);
        }
    }

    // is there a returnValue already? If not the config file must be loaded
    if (returnValue == nullptr)
    {
        QString filename = "";
        QString path = "";
        int lastIndex = configName.lastIndexOf("\\");

        if (lastIndex == -1) // the configName does not contain a filepath
        {
            filename = configName;
            path = QDir::currentPath();
        }
        else
        {
            int laenge = configName.length() - lastIndex - 1;
            filename = configName.right(laenge);
            path = configName.left(lastIndex + 1);
        }
        Config *cfg = new Config(filename, path);
        if (cfg->load())
            returnValue = cfg;
        else
            returnValue = nullptr;

        if (returnValue == nullptr)
            throw QString("The config file: " + configName + "cannot be loaded! Does the file exist?");

        mListOfConfigs.append(cfg);
        returnValue = cfg;
    }

    return returnValue;
}

//------------------------
// deleteCache destroys the elements in the cache
//------------------------
void ConfigCache::deleteCache() noexcept
{
    for(int i = 0; i < mListOfConfigs.size(); i++)
    {
        delete mListOfConfigs.at(i);
        mListOfConfigs.removeAt(i);
    }
}
