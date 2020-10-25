#ifndef DATASET_H
#define DATASET_H
#include <abstractdatabase.h>
#include <usertable.h>
#include <classtable.h>
#include <studenttable.h>
#include <studentclasstable.h>
#include <lendingtable.h>
#include <booktable.h>
#include <givebacktable.h>


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
