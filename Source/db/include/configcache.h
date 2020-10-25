#ifndef CONFIGCACHE_H
#define CONFIGCACHE_H
#include "baseqt_global.h"
#include "config.h"
#include <QString>
#include <QList>

class BASEQTSHARED_EXPORT ConfigCache
{
public:
    static Config* getConfig (QString configName);
    static void deleteCache() noexcept;

private:
    ConfigCache();
    static QList<Config*> mListOfConfigs;
};

#endif // CONFIGCACHE_H
