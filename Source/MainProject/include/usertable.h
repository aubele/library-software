#ifndef USERTABLE_H
#define USERTABLE_H
#include <QString>
#include <QList>
#include <db_global.h>

 enum UserPermissions {Nothing=0, Lending=1, Adding=2, Administrative=4};

// Operator to turn UserPermissions into a flag
inline UserPermissions operator|(UserPermissions a, UserPermissions b)
{
    return static_cast<UserPermissions>(static_cast<int>(a) | static_cast<int>(b));
}

inline UserPermissions operator|=(UserPermissions a, UserPermissions b)
{
    return static_cast<UserPermissions>(static_cast<int>(a) | static_cast<int>(b));
}


class DBSHARED_EXPORT UserTable
{
public:
    struct UserRow
    {
        QString UserName;
        QString Passwd;
        UserPermissions Permissions;
    };
    enum UserTableSortOrder {UserNameAscending, UserNameDescanding};
    UserTable(QString tablename, QString namecolumn, QString passwdcolumn, QString rolecolumn) noexcept;
    virtual ~UserTable() noexcept;

    virtual bool insert(UserRow user) noexcept;
    virtual bool insert(QString username, QString passwd, UserPermissions permissions) noexcept;

    virtual bool remove(QString username) noexcept;

    virtual UserTable::UserRow* getUser(QString username);
    virtual QList<UserRow> getUsers(UserPermissions permissions);
    virtual QList<UserRow> getAllUsers(UserTableSortOrder order = UserNameAscending);

    virtual bool update(QString username, QString passwd) noexcept;
    virtual bool update(QString username, QString passwd, UserPermissions permissions) noexcept;
    virtual bool update(QString username, UserPermissions permissions) noexcept;

private:
    QString mTableName;
    QString mNameColumn;
    QString mPasswdColumn;
    QString mRoleColumn;
};



#endif // USERTABLE_H
