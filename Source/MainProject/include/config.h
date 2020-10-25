#ifndef CONFIG_H
#define CONFIG_H
#include "baseqt_global.h"
#include "cfgblock.h"

class BASEQTSHARED_EXPORT ConfigObserver
{
public:
    enum Action {ADDED, REMOVED, MODIFIED};
    virtual void cfgblockchanged(QString blockname, Action action) noexcept = 0;
    virtual void cfgkeyofblockchanged(QString blockname, QString keyname, Action action) noexcept = 0;
};

class BASEQTSHARED_EXPORT Config
{
public:
    Config(QString cfgName, QString path);
     ~Config();
    QString getConfigName() noexcept;
    QString getConfigComment() noexcept;
    bool isModified() noexcept;
    void attach(ConfigObserver* observer) noexcept;
    void detach(ConfigObserver* observer) noexcept;
    VariousValue getValueForKeyInBlock(QString blockname, QString keyname, VariousValue defaultvalue) noexcept;
    bool addBlock(QString blockname, QString blockcomment) noexcept;
    void removeBlock (QString blockname) noexcept;
    QString getCommentOfBlock(QString blockname);
    QList<QString> getAllBlocks() noexcept;
    QList<QString> getAllKeysOfBlock(QString blockname);
    bool addKeyToBlock(QString blockname, QString keyname, QString keycomment, VariousValue value, bool isReadOnly = false) noexcept;
    bool removeKeyFromBlock(QString blockname, QString keyname) noexcept;
    QString getCommentOfKeyInBlock(QString blockname, QString keyname);
    bool setValueForKeyInBlock(QString blockname, QString keyname, QString value);
    VariousValue getMinValueOfKeyInBlock (QString blockname, QString keyname);
    VariousValue getMaxValueOfKeyInBlock(QString blockname, QString keyname);
    int getMaxLengthOfKeyInBlock(QString blockname, QString keyname);
    bool setMinValueForKeyInBlock(QString blockname, QString keyname, VariousValue minValue) noexcept;
    bool setMaxValueForKeyInBlock(QString blockname, QString keyname, VariousValue maxValue) noexcept;
    bool setMaxLengthForKeyInBlock(QString blockname, QString keyname, int value) noexcept;
    VariousValue::type getTypeOfKeyInBlock(QString blockname, QString keyname);
    bool isKeyReadOnlyOfBlock(QString blockname, QString keyname);
    bool addPossibleValueToKeyInBlock(QString blockname, QString keyname, QString value) noexcept;
    QList<QString> getPossibleValuesOfKeyInBlock(QString blockname, QString keyname);
    bool hasKeyPossibleValuesInBlock(QString blockname, QString keyname);
    bool hasKeyMinValueInBlock(QString blockname, QString keyname);
    bool hasKeyMaxValueInBlock(QString blockname, QString keyname);
    bool hasKeyMaxLengthInBlock(QString blockname, QString keyname);
    bool setKeyInBlockReadOnly(QString blockname, QString keyname, bool value) noexcept;
    bool save() noexcept;
    bool load() noexcept;

private:
    QString mCfgName;
    QString mPath;
    QString mComment;
    QHash<QString, CfgBlock*> mBlocklist;
    bool mIsModified;
    QList<ConfigObserver*> mObserver;
};

#endif // CONFIG_H
