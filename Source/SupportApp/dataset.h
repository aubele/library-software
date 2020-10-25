#ifndef DATASET_H
#define DATASET_H
#include <../db/abstractdatabase.h>
#include <../db/usertable.h>
#include <../db/classtable.h>
#include <../db/studenttable.h>
#include <../db/studentclasstable.h>
#include <../db/lendingtable.h>
#include <../db/booktable.h>
#include <../db/givebacktable.h>


class Dataset : public AbstractDatabase
{
public:
    Dataset();
    virtual ~Dataset();
    UserTable mUserTable;
    ClassTable mClassTable;
    StudentClassTable mStudentClassTable;
    StudentTable mStudentTable;
    BookTable mBookTable;
    GiveBackTable mGiveBackTable;
    LendingTable mLendTable;
};

#endif // DATASET_H
