#include "studenttable.h"

#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QVariant>

//====================
// Implementation of StudentTable
//====================

//--------------------
// Constructor
//--------------------
StudentTable::StudentTable(StudentClassTable studentclasstable, ClassTable classtable, QString tablename, QString prenamecolumn, QString surnamecolumn,
                           QString addresscolumn, QString dateofbirthcolumn, QString commentcolumn) noexcept:
    mStudentClassTable(studentclasstable),
    mClassTable(classtable),
    mTableName(tablename),
    mPreNameColumn(prenamecolumn),
    mSurNameColumn(surnamecolumn),
    mAddressColumn(addresscolumn),
    mDateOfBirthColumn(dateofbirthcolumn),
    mCommentColumn(commentcolumn)
{
}

//------------------
// Destructor
//------------------
StudentTable::~StudentTable() noexcept
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------
// insertStudent: adds a new student and a new studentclass, if the class doesnt exist it also adds a new class
// Parameter: preName, surName, address, dateOfBirth, comment, classRow
//--------------------
bool StudentTable::insertStudent(QString preName, QString surName, QString address, QString dateOfBirth, QString comment, ClassTable::ClassRow classRow) noexcept
{
    bool success = false;
    if(!QSqlDatabase::database().open())
    {
        return false;
    }

    // Insert the student
    QSqlQuery query;
    query.prepare("INSERT INTO " + mTableName + " (" + mPreNameColumn + ", " + mSurNameColumn + ", " + mAddressColumn + ", " +
                  mDateOfBirthColumn + ", " + mCommentColumn + ") VALUES ('" + preName + "', '" + surName  + "', '" +
                  address + "', '" + dateOfBirth + "', '" + comment + "');");
    if(query.exec())
    {
        success = true;
    }
    else
    {
        return false;
    }

    // Insert it also in the studentclasstable
    success = mStudentClassTable.insertStudentClass(preName, surName, dateOfBirth, classRow.className);
    if(!success)
    {
        return false;
    }

    // If the class doesnt exist, insert it in the classtable
    if(mClassTable.getClass(classRow.className)->className.isEmpty())
    {
        success = mClassTable.insert(classRow);
    }

    return success;
}

//--------------------
// insertStudent: adds a new student and a new studentclass, if the class doesnt exist it also adds a new class
// Parameter: student
//--------------------
bool StudentTable::insertStudent(StudentRow student) noexcept
{
    bool success = false;
    for(int i = 0; i<student.classRows.size(); i++)
    {
        // Insert the student only with the first class
        if(i == 0)
        {
            success = insertStudent(student.preName, student.surName, student.address, student.dateOfBirth, student.comment, student.classRows.at(i));
        }
        // Now insert the other classes
        else
        {
            success = insertStudentClass(student.preName, student.surName, student.dateOfBirth, student.classRows.at(i));
        }
        if(!success)
        {
            return false;
        }
    }
    return success;
}

//--------------------
// insertStudentClass: inserts a studentclass when the student is existing, if the class doesnt exist it also adds a new class
// Parameter: preName, surName, dateOfBirth, classRow
//--------------------
bool StudentTable::insertStudentClass(QString preName, QString surName, QString dateOfBirth, ClassTable::ClassRow classRow) noexcept
{
    // If one field from the primary key is empty the student doesnt exist and we can exit the function
    if(this->getStudent(preName, surName, dateOfBirth)->preName.isEmpty())
    {
        return false;
    }
    // If the class doesnt exist, insert it in the classtable
    if(mClassTable.getClass(classRow.className)->className.isEmpty())
    {
        if(mClassTable.insert(classRow))
        {
            return mStudentClassTable.insertStudentClass(preName, surName, dateOfBirth, classRow.className);
        }
        else
        {
            return false;
        }
    }
    return mStudentClassTable.insertStudentClass(preName, surName, dateOfBirth, classRow.className);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------
// removeStudent: deletes a student from the database, removes also empty classes
// Parameter: preName, surName, dateOfBirth
//--------------------
bool StudentTable::removeStudent(QString preName, QString surName, QString dateOfBirth) noexcept
{
    bool success = false;
    if(!QSqlDatabase::database().open())
    {
        return false;
    }

    // Remove student from studenttable
    QSqlQuery query;
    query.prepare("DELETE FROM " + mTableName + " WHERE " + mPreNameColumn + "='" + preName + "' AND " + mSurNameColumn + "='" + surName +
                  "' AND " + mDateOfBirthColumn + "='" + dateOfBirth +"';");
    if(query.exec())
    {
        success = true;
    }
    else
    {
        return false;
    }

    // Get all classes from this student to check them
    QStringList classNames = mStudentClassTable.getClassNamesFromStudent(preName, surName, dateOfBirth);
    // Remove student from studentclasstable
    if(!mStudentClassTable.removeStudent(preName, surName, dateOfBirth))
    {
        return false;
    }

    // Check if all the classes from the student still have member
    for(QString className : classNames)
    {
        if(mStudentClassTable.getStudents(className).isEmpty())
        {
            // If its empty, delete it
            if(!mClassTable.remove(className))
            {
                return false;
            }
        }
    }

    QSqlDatabase::database().close();
    return success;
}

//--------------------
// removeStudentClass: removes one class from a student, removes also the class, if its empty
// Parameter: preName, surName, dateOfBirth, className
//--------------------
bool StudentTable::removeStudentClass(QString preName, QString surName, QString dateOfBirth, QString className) noexcept
{
    // Remove studentclass
    if(!mStudentClassTable.removeStudentClass(preName, surName, dateOfBirth, className))
    {
        return false;
    }

    // Check if the class still has member
    if(mStudentClassTable.getStudents(className).isEmpty())
    {
        // If not delete it
        return mClassTable.remove(className);
    }
    return true;
}

//--------------------
// removeClass: removes one class, removes also the class in the classtable
// Parameter: className
//--------------------
bool StudentTable::removeClass(QString className) noexcept
{
    // Remove studentclasses
    if(!mStudentClassTable.removeClass(className))
    {
        return false;
    }

    // Check if the class still has member
    if(mStudentClassTable.getStudents(className).isEmpty())
    {
        // If not delete it
        return mClassTable.remove(className);
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------
// getStudent: finds a student in the database and returns it as a StudentRow
// Parameter: preName, surName, dateOfBirth
//--------------------
StudentTable::StudentRow* StudentTable::getStudent(QString preName, QString surName, QString dateOfBirth)
{
    if(!QSqlDatabase::database().open())
    {
        throw QString("Error in code: Method getStudent of StudenTable: Database could not be opened!");
    }
    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mPreNameColumn + "='" + preName + "' AND " + mSurNameColumn + "='" + surName +
                    "' AND " + mDateOfBirthColumn + "='" + dateOfBirth +"';");

    int idPreName = query.record().indexOf(mPreNameColumn);
    int idSurName = query.record().indexOf(mSurNameColumn);
    int idAddress = query.record().indexOf(mAddressColumn);
    int idDateOfBirth = query.record().indexOf(mDateOfBirthColumn);
    int idComment = query.record().indexOf(mCommentColumn);

    StudentTable::StudentRow* row = new StudentTable::StudentRow();
    while (query.next())
    {
       row->preName = query.value(idPreName).toString();
       row->surName = query.value(idSurName).toString();
       row->address = query.value(idAddress).toString();
       row->dateOfBirth = query.value(idDateOfBirth).toString();
       row->comment = query.value(idComment).toString();
    }
    // Add classes from StudenClassTable outside of the loop since we need to use
    // a new query here and .next() from the old query wont work with a new one
    QStringList classNames = mStudentClassTable.getClassNamesFromStudent(row->preName, row->surName, row->dateOfBirth);
    QList<ClassTable::ClassRow> classRows;
    for(QString className : classNames)
    {
        classRows.append(*mClassTable.getClass(className));
    }
    row->classRows = classRows;

    QSqlDatabase::database().close();
    return row;
}

//--------------------
// getAllStudents: returns all students
//--------------------
QList<StudentTable::StudentRow> StudentTable::getAllStudents(StudentTableSort studentsorting, StudentClassTable::StudentClassTableSort classsorting)
{
    if(!QSqlDatabase::database().open())
    {
        throw QString("Error in code: Method getAllStudents of StudenTable: Database could not be opened!");
    }
    QString sort = " ORDER BY ";
    switch(studentsorting)
    {
    case StudentTable::StudentNameAscending:
        sort += mSurNameColumn + ", " + mPreNameColumn + " ASC";
        break;
    case StudentTable::StudentNameDescending:
        sort += mSurNameColumn + ", " + mPreNameColumn + " DESC";
        break;
    }
    QSqlQuery query("SELECT * FROM " + mTableName + sort + ";");

    int idPreName = query.record().indexOf(mPreNameColumn);
    int idSurName = query.record().indexOf(mSurNameColumn);
    int idAddress = query.record().indexOf(mAddressColumn);
    int idDateOfBirth = query.record().indexOf(mDateOfBirthColumn);
    int idComment = query.record().indexOf(mCommentColumn);

    QList<StudentTable::StudentRow> rows;
    while (query.next())
    {
       StudentRow row;
       row.preName = query.value(idPreName).toString();
       row.surName = query.value(idSurName).toString();
       row.address = query.value(idAddress).toString();
       row.dateOfBirth = query.value(idDateOfBirth).toString();
       row.comment = query.value(idComment).toString();

       rows.append(row);
    }
    // Add classes from StudenClassTable outside of the loop since we need to use
    // a new query here and .next() from the old query wont work with a new one
    QList<StudentTable::StudentRow> returnRows;
    for(StudentRow row : rows)
    {
        QStringList classNames = mStudentClassTable.getClassNamesFromStudent(row.preName, row.surName, row.dateOfBirth, classsorting);
        for(QString className : classNames)
        {
            row.classRows.append(*mClassTable.getClass(className));
        }
        returnRows.append(row);
    }

    QSqlDatabase::database().close();
    return returnRows;
}

//--------------------
// getStudentsByClassName: finds all students with the given classname and returns them as a list
// Parameter: className
//--------------------
QList<StudentTable::StudentRow> StudentTable::getStudentsByClassName(QString className, StudentClassTable::StudentTableSort sortoption)
{
    QList<StudentClassTable::StudentClassRow> studentClassTable;
    studentClassTable = mStudentClassTable.getStudents(className, sortoption);

    if(studentClassTable.isEmpty())
    {
        return QList<StudentTable::StudentRow>();
    }

    QList<StudentTable::StudentRow> studentTable;
    for(int i = 0; i<studentClassTable.size(); i++)
    {
        StudentRow row = *this->getStudent(studentClassTable[i].studentPreName, studentClassTable[i].studentSurName, studentClassTable[i].studentDateOfBirth);
        studentTable.append(row);
    }

    return studentTable;
}

//--------------------
// getClassesFromStudent: finds all classes from the given student
// Parameter: preName, surName, dateOfBirth
//--------------------
QList<ClassTable::ClassRow> StudentTable::getClassesFromStudent(QString preName, QString surName, QString dateOfBirth, StudentClassTable::StudentClassTableSort sortoption)
{
    QStringList classNameList = mStudentClassTable.getClassNamesFromStudent(preName, surName, dateOfBirth, sortoption);
    QList<ClassTable::ClassRow> classRowList;
    for(QString className : classNameList)
    {
        classRowList.append(*mClassTable.getClass(className));
    }

    return classRowList;
}

//--------------------
// searchStudentsByPreName: finds all students with the given prename pattern and returns them as a list
// Parameter: preName
//--------------------
QList<StudentTable::StudentRow> StudentTable::searchStudentsByPreName(QString preName)
{
    if(!QSqlDatabase::database().open())
    {
        throw QString("Error in code: Method searchStudentsByPreName of StudenTable: Database could not be opened!");
    }

    QSqlQuery pragmaQuery("PRAGMA case_sensitive_like = true;");
    pragmaQuery.exec();
    pragmaQuery.finish();

    // WHERE LIKE is probably slow
    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mPreNameColumn + " LIKE '%" + preName + "%';");

    int idPreName = query.record().indexOf(mPreNameColumn);
    int idSurName = query.record().indexOf(mSurNameColumn);
    int idAddress = query.record().indexOf(mAddressColumn);
    int idDateOfBirth = query.record().indexOf(mDateOfBirthColumn);
    int idComment = query.record().indexOf(mCommentColumn);

    QList<StudentTable::StudentRow> rows;
    while (query.next())
    {
       StudentRow row;
       row.preName = query.value(idPreName).toString();
       row.surName = query.value(idSurName).toString();
       row.address = query.value(idAddress).toString();
       row.dateOfBirth = query.value(idDateOfBirth).toString();
       row.comment = query.value(idComment).toString();

       rows.append(row);
    }
    // Add classes from StudenClassTable outside of the loop since we need to use
    // a new query here and .next() from the old query wont work with a new one
    QList<StudentTable::StudentRow> returnRows;
    for(StudentRow row : rows)
    {
        QStringList classNames = mStudentClassTable.getClassNamesFromStudent(row.preName, row.surName, row.dateOfBirth);
        for(QString className : classNames)
        {
            row.classRows.append(*mClassTable.getClass(className));
        }
        returnRows.append(row);
    }

    QSqlDatabase::database().close();
    return returnRows;
}

//--------------------
// searchStudentsBySurName: finds all students with the given surname pattern and returns them as a list
// Parameter: surName
//--------------------
QList<StudentTable::StudentRow> StudentTable::searchStudentsBySurName(QString surName)
{
    if(!QSqlDatabase::database().open())
    {
        throw QString("Error in code: Method searchStudentsBySurName of StudenTable: Database could not be opened!");
    }

    QSqlQuery pragmaQuery("PRAGMA case_sensitive_like = true;");
    pragmaQuery.exec();
    pragmaQuery.finish();

    // WHERE LIKE is probably slow
    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mSurNameColumn + " LIKE '%" + surName + "%';");

    int idPreName = query.record().indexOf(mPreNameColumn);
    int idSurName = query.record().indexOf(mSurNameColumn);
    int idAddress = query.record().indexOf(mAddressColumn);
    int idDateOfBirth = query.record().indexOf(mDateOfBirthColumn);
    int idComment = query.record().indexOf(mCommentColumn);

    QList<StudentTable::StudentRow> rows;
    while (query.next())
    {
       StudentRow row;
       row.preName = query.value(idPreName).toString();
       row.surName = query.value(idSurName).toString();
       row.address = query.value(idAddress).toString();
       row.dateOfBirth = query.value(idDateOfBirth).toString();
       row.comment = query.value(idComment).toString();

       rows.append(row);
    }
    // Add classes from StudenClassTable outside of the loop since we need to use
    // a new query here and .next() from the old query wont work with a new one
    QList<StudentTable::StudentRow> returnRows;
    for(StudentRow row : rows)
    {
        QStringList classNames = mStudentClassTable.getClassNamesFromStudent(row.preName, row.surName, row.dateOfBirth);
        for(QString className : classNames)
        {
            row.classRows.append(*mClassTable.getClass(className));
        }
        returnRows.append(row);
    }

    QSqlDatabase::database().close();
    return returnRows;
}

//--------------------
// searchStudentsByComment: finds all students with the given comment pattern and returns them as a list
// Parameter: comment
//--------------------
QList<StudentTable::StudentRow> StudentTable::searchStudentsByComment(QString comment)
{
    if(!QSqlDatabase::database().open())
    {
        throw QString("Error in code: Method searchStudentsByComment of StudenTable: Database could not be opened!");
    }

    QSqlQuery pragmaQuery("PRAGMA case_sensitive_like = true;");
    pragmaQuery.exec();
    pragmaQuery.finish();

    // WHERE LIKE should be pretty slow
    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mCommentColumn + " LIKE '%" + comment + "%';");

    int idPreName = query.record().indexOf(mPreNameColumn);
    int idSurName = query.record().indexOf(mSurNameColumn);
    int idAddress = query.record().indexOf(mAddressColumn);
    int idDateOfBirth = query.record().indexOf(mDateOfBirthColumn);
    int idComment = query.record().indexOf(mCommentColumn);

    QList<StudentTable::StudentRow> rows;
    while (query.next())
    {
       StudentRow row;
       row.preName = query.value(idPreName).toString();
       row.surName = query.value(idSurName).toString();
       row.address = query.value(idAddress).toString();
       row.dateOfBirth = query.value(idDateOfBirth).toString();
       row.comment = query.value(idComment).toString();

       rows.append(row);
    }
    // Add classes from StudenClassTable outside of the loop since we need to use
    // a new query here and .next() from the old query wont work with a new one
    QList<StudentTable::StudentRow> returnRows;
    for(StudentRow row : rows)
    {
        QStringList classNames = mStudentClassTable.getClassNamesFromStudent(row.preName, row.surName, row.dateOfBirth);
        for(QString className : classNames)
        {
            row.classRows.append(*mClassTable.getClass(className));
        }
        returnRows.append(row);
    }

    QSqlDatabase::database().close();
    return returnRows;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------
// updateStudentAddress: writes a new address for the given student in the database
// Parameter: preName, surName, dateOfBirth, address
//--------------------
bool StudentTable::updateStudentAddress(QString preName, QString surName, QString dateOfBirth, QString address) noexcept
{
    bool success = false;
    if(!QSqlDatabase::database().open())
    {
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE " + mTableName + " SET " + mAddressColumn + "='" + address + "' WHERE " + mPreNameColumn +
                  "='" + preName + "' AND " + mSurNameColumn + "='" + surName + "' AND " + mDateOfBirthColumn + "='" + dateOfBirth + "';");

    if(query.exec())
    {
        success = true;
    }

    QSqlDatabase::database().close();
    return success;
}

//--------------------
// updateStudentComment: writes a new comment for the given student in the database
// Parameter: preName, surName, dateOfBirth, comment
//--------------------
bool StudentTable::updateStudentComment(QString preName, QString surName, QString dateOfBirth, QString comment) noexcept
{
    bool success = false;
    if(!QSqlDatabase::database().open())
    {
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE " + mTableName + " SET " + mCommentColumn + "='" + comment + "' WHERE " + mPreNameColumn +
                  "='" + preName + "' AND " + mSurNameColumn + "='" + surName + "' AND " + mDateOfBirthColumn + "='" + dateOfBirth + "';");

    if(query.exec())
    {
        success = true;
    }

    QSqlDatabase::database().close();
    return success;
}

//--------------------
// updateStudentClass: removes the given studentclasses and inserts new ones for it; oldClassNameList and newClassList must have the same size
// Parameter: preName, surName, dateOfBirth, oldClassNameList, newClassList
//--------------------
bool StudentTable::updateStudentClasses(QString preName, QString surName, QString dateOfBirth, QList<ClassTable::ClassRow> oldClassNameList, QList<ClassTable::ClassRow> newClassList) noexcept
{
    // return if lists are empty, not the same size or the student doesnt exist
    if(oldClassNameList.isEmpty() || newClassList.isEmpty() || getStudent(preName, surName, dateOfBirth)->preName.isEmpty())
    {
        return false;
    }

    // first delete all classes of the student
    for(int i = 0; i < oldClassNameList.size(); i++)
    {
        // If the class doesnt exist -> return
        if(mClassTable.getClass(oldClassNameList.at(i).className)->className.isEmpty())
        {
            return false;
        }

        // Remove old classes
        if(!removeStudentClass(preName, surName, dateOfBirth, oldClassNameList.at(i).className))
        {
            return false;
        }
    }

    // then insert the new ones
    for (int i = 0; i < newClassList.size(); i++)
    {
        // Insert new class
        if(!insertStudentClass(preName, surName, dateOfBirth, newClassList.at(i)))
        {
            return false;
        }
    }
    return true;
}
