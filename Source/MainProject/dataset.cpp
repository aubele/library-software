#include "dataset.h"

Dataset::Dataset() :
    AbstractDatabase("QSQLITE", "database.db"),
    mUserTable("UserTable", "UserName", "Passwd", "Permissions"),
    mClassTable("ClassTable", "className", "grade", "subject"),
    mStudentClassTable("StudentClassTable", "studentPreName","studentSurName","StudentDateOfBirth", "class"),   
    mStudentTable(mStudentClassTable, mClassTable, "StudentTable", "preName", "surName", "address", "dateOfBirth", "comment"),
    mBookTable("BookTable", "isbn", "title", "subtitle", "publisher", "author", "subject", "grade", "edition", "price", "stock", "available", "comment", "stocktakingCount", "stocktakingDate"),
    mGiveBackTable("ReturnTable", "ID", "damage", "date", "price"),
    mLendTable(mGiveBackTable, "LendingTable", "ID", "ReturnID", "lendDate", "book", "studentSurName", "studentPreName", "StudentDateOfBirth", "bringBackDate", "lendingType")
{
}

Dataset::~Dataset()
{
}
