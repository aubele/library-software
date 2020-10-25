#include "teststudenttable.h"


//--------------------
// Constructor
//--------------------
TestStudentTable::TestStudentTable()
{
    studentclasstable = new StudentClassTable("StudentClassTable", "studentPreName", "studentSurName", "StudentDateOfBirth", "class");
    classtable = new ClassTable("ClassTable", "className", "grade", "subject");
    studenttable = new StudentTable(*studentclasstable, *classtable, "StudentTable", "preName", "surName", "address", "dateOfBirth", "comment");
}

//--------------------
// initTestCase: initiates the testdatabase via sql-scriptfile
//--------------------
void TestStudentTable::initTestCase()
{
    // Create test db
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("../TestDb/TestDatabase.db");
    db.open();

    // Write data from script file into db
    // Wrong queries get displayed via qDebug
    int queryCount = ReadSqlScriptFile(db, "../TestDb/db_with_testdata.db.sql");
    qDebug() << "Successful queries:" << queryCount;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------
// testGetStudent: tests for an existing student and a not existing student
//--------------------
void TestStudentTable::testGetStudent()
{
    // Existing
    StudentTable::StudentRow* row = studenttable->getStudent("Vorname1-5a", "Nachname1-5a", "31.07.2007");
    QVERIFY(row->preName == "Vorname1-5a");
    QVERIFY(row->surName == "Nachname1-5a");
    QVERIFY(row->address == "Musterstr. 1, 86150 Augsburg");
    QVERIFY(row->dateOfBirth == "31.07.2007");
    QVERIFY(row->comment == "TestKommentar");
    QVERIFY(row->classRows.at(0).className == "5a");
    QVERIFY(row->classRows.at(0).grade == 5);
    QVERIFY(row->classRows.at(0).subject == "");
    QVERIFY(row->classRows.at(1).className == "5ab");
    QVERIFY(row->classRows.at(1).grade == 5);
    QVERIFY(row->classRows.at(1).subject == "");

    // Not Existing
    row = studenttable->getStudent("Nicht", "Nicht", "Nicht");
    QVERIFY(row->preName.isEmpty());
    QVERIFY(row->surName.isEmpty());
    QVERIFY(row->dateOfBirth.isEmpty());
    QVERIFY(row->classRows.isEmpty());
}

//--------------------
// testGetAllStudents: tests for all students
//--------------------
void TestStudentTable::testGetAllStudents()
{
    // All students
    QList<StudentTable::StudentRow> rowList = studenttable->getAllStudents();
    QVERIFY(rowList.size() == 12);
    QVERIFY(rowList.at(2).preName == "Vorname3-5a");
    QVERIFY(rowList.at(2).classRows.size() == 2);
}

//--------------------
// testGetByClassName: tests for an existing classname and a not existing classname
//--------------------
void TestStudentTable::testGetByClassName()
{
    // Existing
    QList<StudentTable::StudentRow> rowList = studenttable->getStudentsByClassName("MatheLK");
    QVERIFY(rowList.size() == 2);
    QVERIFY(rowList.at(0).preName == "Vorname1-10a");
    QVERIFY(rowList.at(0).surName == "Nachname1-10a");
    QVERIFY(rowList.at(0).dateOfBirth == "04.05.2001");
    QVERIFY(rowList.at(1).preName == "Vorname2-10a");
    QVERIFY(rowList.at(1).surName == "Nachname2-10a");
    QVERIFY(rowList.at(1).dateOfBirth == "07.03.2000");

    // Not Existing
    rowList = studenttable->getStudentsByClassName("NichtLK");
    QVERIFY(rowList.size() == 0);
}

//--------------------
// testGetClassesFromStudent: tests for an existing student and a not existing student
//--------------------
void TestStudentTable::testGetClassesFromStudent()
{
    // Existing
    QList<ClassTable::ClassRow> classList = studenttable->getClassesFromStudent("Vorname2-10a", "Nachname2-10a", "07.03.2000");
    QVERIFY(classList.size() == 4);
    QVERIFY(classList.at(0).className == "$2Bio");
    QVERIFY(classList.at(0).grade == 12);
    QVERIFY(classList.at(0).subject == "Biologie");
    QVERIFY(classList.at(1).className == "/2Lat");
    QVERIFY(classList.at(1).grade == 12);
    QVERIFY(classList.at(1).subject == "Latein");
    QVERIFY(classList.at(2).className == "10a");
    QVERIFY(classList.at(2).grade == 10);
    QVERIFY(classList.at(2).subject == "");
    QVERIFY(classList.at(3).className == "MatheLK");
    QVERIFY(classList.at(3).grade == 12);
    QVERIFY(classList.at(3).subject == "Mathe");

    // Not Existing
    classList = studenttable->getClassesFromStudent("Nicht", "Nicht", "Nicht");
    QVERIFY(classList.size() == 0);
}

//--------------------
// testGetAllClasses: tests if all classes gets returned
//--------------------
void TestStudentTable::testGetAllClasses()
{
    QList<ClassTable::ClassRow> classList = classtable->getAllClasses();
    QVERIFY(classList.size() == 11);
    QVERIFY(classList.at(0).className == "5a");
    QVERIFY(classList.at(1).className == "5aa");
    QVERIFY(classList.at(2).className == "5ab");
    QVERIFY(classList.at(3).className == "5b");
    QVERIFY(classList.at(4).className == "10a");
    QVERIFY(classList.at(5).className == "MatheLK");
    QVERIFY(classList.at(6).className == "DeutschLK");
    QVERIFY(classList.at(7).className == "EnglischLK");
    QVERIFY(classList.at(8).className == "/2Lat");
    QVERIFY(classList.at(9).className == "$2Bio");
    QVERIFY(classList.at(10).className == "ZuEntfernen");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------
// testInsertStudent: tests for a student with a new class, for a student with an existing class and an already existing student;
// checks for student-, studentclass-, and classtable
//--------------------
void TestStudentTable::testInsertStudent()
{
    // New class
    ClassTable::ClassRow newclassrow = {"ChemieLK", 11, "Chemie"};
    QVERIFY(studenttable->insertStudent("Hans", "Lauter", "Bspstraße 14a", "04.04.1994", "Test", newclassrow));
    // Test student object
    testGetStudent();
    StudentTable::StudentRow* studentrow = studenttable->getStudent("Hans", "Lauter", "04.04.1994");
    QVERIFY(studentrow->preName == "Hans");
    QVERIFY(studentrow->surName == "Lauter");
    QVERIFY(studentrow->address == "Bspstraße 14a");
    QVERIFY(studentrow->dateOfBirth == "04.04.1994");
    QVERIFY(studentrow->comment == "Test");
    QVERIFY(studentrow->classRows.size() == 1);
    QVERIFY(studentrow->classRows.at(0).className == "ChemieLK");
    QVERIFY(studentrow->classRows.at(0).grade == 11);
    QVERIFY(studentrow->classRows.at(0).subject == "Chemie");
    // Test studentclass object
    StudentClassTable::StudentClassRow* studentclassrow = studentclasstable->getStudentClass("Hans", "Lauter", "04.04.1994", "ChemieLK");
    QVERIFY(studentclassrow->studentPreName == "Hans");
    QVERIFY(studentclassrow->studentSurName == "Lauter");
    QVERIFY(studentclassrow->studentDateOfBirth == "04.04.1994");
    QVERIFY(studentclassrow->className == "ChemieLK");
    // Test class object
    ClassTable::ClassRow* classrow = classtable->getClass("ChemieLK");
    QVERIFY(classrow->className == "ChemieLK");
    QVERIFY(classrow->grade == 11);
    QVERIFY(classrow->subject == "Chemie");

    // Existing class
    newclassrow = {"ChemieLK", 11, "Chemie"};
    QVERIFY(studenttable->insertStudent("Bernhard", "Glück", "Neustraße 14a", "04.04.1994", "Test", newclassrow));
    //Test student object
    studentrow = studenttable->getStudent("Bernhard", "Glück", "04.04.1994");
    QVERIFY(studentrow->preName == "Bernhard");
    QVERIFY(studentrow->surName == "Glück");
    QVERIFY(studentrow->address == "Neustraße 14a");
    QVERIFY(studentrow->dateOfBirth == "04.04.1994");
    QVERIFY(studentrow->comment == "Test");
    QVERIFY(studentrow->classRows.size() == 1);
    QVERIFY(studentrow->classRows.at(0).className == "ChemieLK");
    QVERIFY(studentrow->classRows.at(0).grade == 11);
    QVERIFY(studentrow->classRows.at(0).subject == "Chemie");
    // Test studentclass object
    studentclassrow = studentclasstable->getStudentClass("Bernhard", "Glück", "04.04.1994", "ChemieLK");
    QVERIFY(studentclassrow->studentPreName == "Bernhard");
    QVERIFY(studentclassrow->studentSurName == "Glück");
    QVERIFY(studentclassrow->studentDateOfBirth == "04.04.1994");
    QVERIFY(studentclassrow->className == "ChemieLK");

    // Already existing student
    newclassrow = {"NeuLK", 1, "Neu"};
    QVERIFY(studenttable->insertStudent("Bernhard", "Glück", "Anderestraße 14a", "04.04.1994", "TestTest", newclassrow) == false);
    //Test student object
    studentrow = studenttable->getStudent("Bernhard", "Glück", "04.04.1994");
    QVERIFY(studentrow->address == "Neustraße 14a");
    QVERIFY(studentrow->comment == "Test");
    QVERIFY(studentrow->classRows.size() == 1);
    QVERIFY(studentrow->classRows.at(0).className == "ChemieLK");
    QVERIFY(studentrow->classRows.at(0).grade == 11);
    QVERIFY(studentrow->classRows.at(0).subject == "Chemie");
    // Test studentclass object
    studentclassrow = studentclasstable->getStudentClass("Bernhard", "Glück", "04.04.1994", "NeuLK");
    QVERIFY(studentclassrow->studentPreName.isEmpty());
    // Test class object
    classrow = classtable->getClass("NeuLK");
    QVERIFY(classrow->className.isEmpty());
}

//--------------------
// testInsertStudent2: tests for a student with a new class, for a student with an existing class and an already existing student;
// checks for student-, studentclass-, and classtable
//--------------------
void TestStudentTable::testInsertStudent2()
{
    // New class and existing class
    QList<ClassTable::ClassRow> newclasslist = {ClassTable::ClassRow{"ChemieLK", 11, "Chemie"}, ClassTable::ClassRow{"11a", 11, ""}};
    QVERIFY(studenttable->insertStudent(StudentTable::StudentRow{"Neuern", "Neuer", "Bspstraße 14a", "04.04.1994", "Test", newclasslist}));
    // Test student object
    testGetStudent();
    StudentTable::StudentRow* studentrow = studenttable->getStudent("Neuern", "Neuer", "04.04.1994");
    QVERIFY(studentrow->preName == "Neuern");
    QVERIFY(studentrow->surName == "Neuer");
    QVERIFY(studentrow->address == "Bspstraße 14a");
    QVERIFY(studentrow->dateOfBirth == "04.04.1994");
    QVERIFY(studentrow->comment == "Test");
    QVERIFY(studentrow->classRows.size() == 2);
    QVERIFY(studentrow->classRows.at(0).className == "11a");
    QVERIFY(studentrow->classRows.at(0).grade == 11);
    QVERIFY(studentrow->classRows.at(0).subject == "");
    QVERIFY(studentrow->classRows.at(1).className == "ChemieLK");
    QVERIFY(studentrow->classRows.at(1).grade == 11);
    QVERIFY(studentrow->classRows.at(1).subject == "Chemie");
    // Test studentclass object
    StudentClassTable::StudentClassRow* studentclassrow = studentclasstable->getStudentClass("Neuern", "Neuer", "04.04.1994", "ChemieLK");
    QVERIFY(studentclassrow->studentPreName == "Neuern");
    QVERIFY(studentclassrow->studentSurName == "Neuer");
    QVERIFY(studentclassrow->studentDateOfBirth == "04.04.1994");
    QVERIFY(studentclassrow->className == "ChemieLK");
    studentclassrow = studentclasstable->getStudentClass("Neuern", "Neuer", "04.04.1994", "11a");
    QVERIFY(studentclassrow->studentPreName == "Neuern");
    QVERIFY(studentclassrow->studentSurName == "Neuer");
    QVERIFY(studentclassrow->studentDateOfBirth == "04.04.1994");
    QVERIFY(studentclassrow->className == "11a");
    // Test class object
    ClassTable::ClassRow* classrow = classtable->getClass("11a");
    QVERIFY(classrow->className == "11a");
    QVERIFY(classrow->grade == 11);
    QVERIFY(classrow->subject == "");

    // Already existing student
    newclasslist = {ClassTable::ClassRow{"NeuLK", 12, "Neu"}};
    QVERIFY(studenttable->insertStudent(StudentTable::StudentRow{"Neuern", "Neuer", "Anderestraße 14a", "04.04.1994", "TestTest", newclasslist}) == false);
    //Test student object
    studentrow = studenttable->getStudent("Neuern", "Neuer", "04.04.1994");
    QVERIFY(studentrow->address == "Bspstraße 14a");
    QVERIFY(studentrow->comment == "Test");
    QVERIFY(studentrow->classRows.size() == 2);
    QVERIFY(studentrow->classRows.at(0).className == "11a");
    QVERIFY(studentrow->classRows.at(0).grade == 11);
    QVERIFY(studentrow->classRows.at(0).subject == "");
    QVERIFY(studentrow->classRows.at(1).className == "ChemieLK");
    QVERIFY(studentrow->classRows.at(1).grade == 11);
    QVERIFY(studentrow->classRows.at(1).subject == "Chemie");
    // Test studentclass object
    studentclassrow = studentclasstable->getStudentClass("Bernhard", "Glück", "04.04.1994", "NeuLK");
    QVERIFY(studentclassrow->studentPreName.isEmpty());
    // Test class object
    classrow = classtable->getClass("NeuLK");
    QVERIFY(classrow->className.isEmpty());
}

//--------------------
// testInsertClass: tests for an existing student with a new class, with an old class and a student which doesnt exist;
// checks for student-, studentclass-, and classtable
//--------------------
void TestStudentTable::testInsertClass()
{
    // Existing student + new class
    ClassTable::ClassRow newclassrow = {"FranzösischLK", 12, "Französisch"};
    QVERIFY(studenttable->insertStudentClass("Vorname5-5a", "Nachname5-5a", "11.08.2005", newclassrow));
    // Test student object
    testGetStudent();
    StudentTable::StudentRow* studentrow = studenttable->getStudent("Vorname5-5a", "Nachname5-5a", "11.08.2005");
    QVERIFY(studentrow->classRows.size() == 2);
    QVERIFY(studentrow->classRows.at(0).className == "5a");
    QVERIFY(studentrow->classRows.at(0).grade == 5);
    QVERIFY(studentrow->classRows.at(0).subject == "");
    QVERIFY(studentrow->classRows.at(1).className == "FranzösischLK");
    QVERIFY(studentrow->classRows.at(1).grade == 12);
    QVERIFY(studentrow->classRows.at(1).subject == "Französisch");
    // Test studentclass object
    StudentClassTable::StudentClassRow* studentClassRow = studentclasstable->getStudentClass("Vorname5-5a", "Nachname5-5a", "11.08.2005", "FranzösischLK");
    QVERIFY(studentClassRow->studentPreName == "Vorname5-5a");
    QVERIFY(studentClassRow->studentSurName == "Nachname5-5a");
    QVERIFY(studentClassRow->studentDateOfBirth == "11.08.2005");
    QVERIFY(studentClassRow->className == "FranzösischLK");
    // Test class object
    ClassTable::ClassRow* classrow = classtable->getClass("FranzösischLK");
    QVERIFY(classrow->className == "FranzösischLK");
    QVERIFY(classrow->grade == 12);
    QVERIFY(classrow->subject == "Französisch");

    // Existing student + old class
    newclassrow = {"ChemieLK", 11, "Chemie"};
    QVERIFY(studenttable->insertStudentClass("Vorname5-5a", "Nachname5-5a", "11.08.2005", newclassrow));
    // Test student object
    studentrow = studenttable->getStudent("Vorname5-5a", "Nachname5-5a", "11.08.2005");
    QVERIFY(studentrow->classRows.size() == 3);
    QVERIFY(studentrow->classRows.at(0).className == "5a");
    QVERIFY(studentrow->classRows.at(0).grade == 5);
    QVERIFY(studentrow->classRows.at(0).subject == "");
    QVERIFY(studentrow->classRows.at(1).className == "ChemieLK");
    QVERIFY(studentrow->classRows.at(1).grade == 11);
    QVERIFY(studentrow->classRows.at(1).subject == "Chemie");
    QVERIFY(studentrow->classRows.at(2).className == "FranzösischLK");
    QVERIFY(studentrow->classRows.at(2).grade == 12);
    QVERIFY(studentrow->classRows.at(2).subject == "Französisch");
    // Test studentclass object
    studentClassRow = studentclasstable->getStudentClass("Vorname5-5a", "Nachname5-5a", "11.08.2005", "ChemieLK");
    QVERIFY(studentClassRow->studentPreName == "Vorname5-5a");
    QVERIFY(studentClassRow->studentSurName == "Nachname5-5a");
    QVERIFY(studentClassRow->studentDateOfBirth == "11.08.2005");
    QVERIFY(studentClassRow->className == "ChemieLK");

    // Not existing student
    QVERIFY(studenttable->insertStudentClass("Nicht", "Nicht", "Nicht", newclassrow) == false);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------
// testRemoveStudent: tests for an existing student with a class that should be deleted, with a class that should not be deleted
// and a student which does not exist; checks for student-, studentclass-, and classtable
//--------------------
void TestStudentTable::testRemoveStudent()
{
    // Existing student with a class that should be deleted and a class that shouldnt be deleted
    QVERIFY(studenttable->removeStudent("Vorname3-5a", "Nachname3-5a", "13.03.2005"));
    // Test student object
    testGetStudent();
    StudentTable::StudentRow* row = studenttable->getStudent("Vorname3-5a", "Nachname3-5a", "13.03.2005");
    QVERIFY(row->preName.isEmpty());
    QVERIFY(row->surName.isEmpty());
    QVERIFY(row->dateOfBirth.isEmpty());
    // Test studentclass object
    testGetClassesFromStudent();
    QList<ClassTable::ClassRow> classList = studenttable->getClassesFromStudent("Vorname3-5a", "Nachname3-5a", "13.03.2005");
    QVERIFY(classList.isEmpty());
    // Test class object
    // Should be deleted
    ClassTable::ClassRow* classrow = classtable->getClass("5aa");
    QVERIFY(classrow->className.isEmpty());
    // Should not be deleted
    classrow = classtable->getClass("5a");
    QVERIFY(classrow->className == "5a");

    // Not Existing student
    QVERIFY(studenttable->removeStudent("Nop", "Nop", "13.03.2005"));
    row = studenttable->getStudent("Nop", "Nop", "13.03.2005");
    QVERIFY(row->preName.isEmpty());
    QVERIFY(row->surName.isEmpty());
    QVERIFY(row->dateOfBirth.isEmpty());
    // Test studentclass object
    classList = studenttable->getClassesFromStudent("Nop", "Nop", "13.03.2005");
    QVERIFY(classList.isEmpty());
}

//--------------------
// testRemoveClassFromStudent:  tests for an existing student with a class that should be deleted, with a class that should not be deleted,
// with a student which does not exist and a class which does not exist; checks for student-, studentclass-, and classtable
//--------------------
void TestStudentTable::testRemoveClassFromStudent()
{
    // Existing + class should be deleted
    QVERIFY(studenttable->removeStudentClass("Vorname1-10a", "Nachname1-10a", "04.05.2001", "DeutschLK"));
    // Test student object
    testGetStudent();
    StudentTable::StudentRow* studentRow = studenttable->getStudent("Vorname1-10a", "Nachname1-10a", "04.05.2001");
    QVERIFY(studentRow->classRows.size() == 3);
    // Test studentclass object
    StudentClassTable::StudentClassRow* studentClassRow = studentclasstable->getStudentClass("Vorname1-10a", "Nachname1-10a", "04.05.2001", "DeutschLK");
    QVERIFY(studentClassRow->studentPreName.isEmpty());
    QVERIFY(studentClassRow->studentSurName.isEmpty());
    QVERIFY(studentClassRow->studentDateOfBirth.isEmpty());
    QVERIFY(studentClassRow->className.isEmpty());
    // Test class object
    ClassTable::ClassRow* classRow = classtable->getClass("DeutschLK");
    QVERIFY(classRow->className.isEmpty());

    // Existing + class should not be deleted
    QVERIFY(studenttable->removeStudentClass("Vorname1-10a", "Nachname1-10a", "04.05.2001", "EnglischLK"));
    // Test student object
    studentRow = studenttable->getStudent("Vorname1-10a", "Nachname1-10a", "04.05.2001");
    QVERIFY(studentRow->classRows.size() == 2);
    // Test studentclass object
    studentClassRow = studentclasstable->getStudentClass("Vorname1-10a", "Nachname1-10a", "04.05.2001", "EnglischLK");
    QVERIFY(studentClassRow->studentPreName.isEmpty());
    QVERIFY(studentClassRow->studentSurName.isEmpty());
    QVERIFY(studentClassRow->studentDateOfBirth.isEmpty());
    QVERIFY(studentClassRow->className.isEmpty());
    // Test class object
    classRow = classtable->getClass("EnglischLK");
    QVERIFY(classRow->className == "EnglischLK");

    // Not Existing student
    QVERIFY(studenttable->removeStudentClass("Nicht", "Nicht", "Nicht", "MatheLK"));
    studentClassRow = studentclasstable->getStudentClass("Nicht", "Nicht", "Nicht", "MatheLK");
    QVERIFY(studentClassRow->studentPreName.isEmpty());
    QVERIFY(studentClassRow->studentSurName.isEmpty());
    QVERIFY(studentClassRow->studentDateOfBirth.isEmpty());
    QVERIFY(studentClassRow->className.isEmpty());
    // Test class object
    classRow = classtable->getClass("MatheLK");
    QVERIFY(classRow->className == "MatheLK");

    // Not Existing class
    QVERIFY(studenttable->removeStudentClass("Vorname1-10a", "Nachname1-10a", "04.05.2001", "Nicht"));
    // Test student object
    studentRow = studenttable->getStudent("Vorname1-10a", "Nachname1-10a", "04.05.2001");
    QVERIFY(studentRow->classRows.size() == 2);
    // Test studentclass object
    studentClassRow = studentclasstable->getStudentClass("Vorname1-10a", "Nachname1-10a", "04.05.2001", "Nicht");
    QVERIFY(studentClassRow->studentPreName.isEmpty());
    QVERIFY(studentClassRow->studentSurName.isEmpty());
    QVERIFY(studentClassRow->studentDateOfBirth.isEmpty());
    QVERIFY(studentClassRow->className.isEmpty());
}

//--------------------
// testRemoveClass: tests for an existing class and a not existing class; checks for student-, studentclass-, and classtable
//--------------------
void TestStudentTable::testRemoveClass()
{
    // Existing class
    QVERIFY(studenttable->removeClass("ZuEntfernen"));
    // Test student object
    testGetStudent();
    StudentTable::StudentRow* row = studenttable->getStudent("Vorname3-5b", "Nachname3-5b", "01.01.2006");
    QVERIFY(row->classRows.size() == 1);
    QVERIFY(row->classRows.at(0).className == "5b");
    row = studenttable->getStudent("Vorname2-5b", "Nachname2-5b", "25.12.2006");
    QVERIFY(row->classRows.size() == 1);
    QVERIFY(row->classRows.at(0).className == "5b");
    // Test studentclass object
    testGetByClassName();
    QList<StudentTable::StudentRow> rowList = studenttable->getStudentsByClassName("ZuEntfernen");
    QVERIFY(rowList.isEmpty());
    // Test class object
    ClassTable::ClassRow* classrow = classtable->getClass("ZuEntfernen");
    QVERIFY(classrow->className.isEmpty());

    // Not existing class
    QVERIFY(studenttable->removeClass("NichtExistent"));
    rowList = studenttable->getStudentsByClassName("NichtExistent");
    QVERIFY(rowList.isEmpty());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------
// testSearchPreName: tests for an existing student and a not existing student
//--------------------
void TestStudentTable::testSearchPreName()
{
    // Existing
    QList<StudentTable::StudentRow> rowList = studenttable->searchStudentsByPreName("VornameÖ");
    QVERIFY(rowList.size() == 2);
    QVERIFY(rowList.at(0).preName == "VornameÖ-5b");
    QVERIFY(rowList.at(0).surName == "NachnameÖ-5b");
    QVERIFY(rowList.at(0).dateOfBirth == "02.02.2005");
    QVERIFY(rowList.at(0).classRows.size() == 1);
    QVERIFY(rowList.at(0).classRows.at(0).className == "5b");
    QVERIFY(rowList.at(0).classRows.at(0).grade == 5);
    QVERIFY(rowList.at(0).classRows.at(0).subject == "");
    QVERIFY(rowList.at(1).preName == "VornameÖ-5b");
    QVERIFY(rowList.at(1).surName == "NachnameÖ-5b");
    QVERIFY(rowList.at(1).dateOfBirth == "03.03.2006");
    QVERIFY(rowList.at(1).classRows.size() == 1);
    QVERIFY(rowList.at(1).classRows.at(0).className == "5b");
    QVERIFY(rowList.at(1).classRows.at(0).grade == 5);
    QVERIFY(rowList.at(1).classRows.at(0).subject == "");

    // Not Existing
    rowList = studenttable->searchStudentsByPreName("GibtsNicht");
    QVERIFY(rowList.size() == 0);
}

//--------------------
// testSearchSurName: tests for an existing student and a not existing student
//--------------------
void TestStudentTable::testSearchSurName()
{
    // Existing
    QList<StudentTable::StudentRow> rowList = studenttable->searchStudentsBySurName("NachnameÖ");
    QVERIFY(rowList.size() == 2);
    QVERIFY(rowList.at(0).preName == "VornameÖ-5b");
    QVERIFY(rowList.at(0).surName == "NachnameÖ-5b");
    QVERIFY(rowList.at(0).dateOfBirth == "02.02.2005");
    QVERIFY(rowList.at(0).classRows.size() == 1);
    QVERIFY(rowList.at(0).classRows.at(0).className == "5b");
    QVERIFY(rowList.at(0).classRows.at(0).grade == 5);
    QVERIFY(rowList.at(0).classRows.at(0).subject == "");
    QVERIFY(rowList.at(1).preName == "VornameÖ-5b");
    QVERIFY(rowList.at(1).surName == "NachnameÖ-5b");
    QVERIFY(rowList.at(1).dateOfBirth == "03.03.2006");
    QVERIFY(rowList.at(1).classRows.size() == 1);
    QVERIFY(rowList.at(1).classRows.at(0).className == "5b");
    QVERIFY(rowList.at(1).classRows.at(0).grade == 5);
    QVERIFY(rowList.at(1).classRows.at(0).subject == "");

    // Not Existing
    rowList = studenttable->searchStudentsBySurName("GibtsNicht");
    QVERIFY(rowList.size() == 0);
}

//--------------------
// testSearchComment: tests for an existing student and a not existing student
//--------------------
void TestStudentTable::testSearchComment()
{
    // Existing
    QList<StudentTable::StudentRow> rowList = studenttable->searchStudentsByComment("TestKommentar");
    QVERIFY(rowList.size() == 2);
    QVERIFY(rowList.at(0).preName == "Vorname1-5a");
    QVERIFY(rowList.at(0).surName == "Nachname1-5a");
    QVERIFY(rowList.at(0).dateOfBirth == "31.07.2007");
    QVERIFY(rowList.at(0).classRows.size() == 2);
    QVERIFY(rowList.at(0).classRows.at(0).className == "5a");
    QVERIFY(rowList.at(0).classRows.at(0).grade == 5);
    QVERIFY(rowList.at(0).classRows.at(0).subject == "");
    QVERIFY(rowList.at(0).classRows.at(1).className == "5ab");
    QVERIFY(rowList.at(0).classRows.at(1).grade == 5);
    QVERIFY(rowList.at(0).classRows.at(1).subject == "");
    QVERIFY(rowList.at(1).preName == "VornameÖ-5b");
    QVERIFY(rowList.at(1).surName == "NachnameÖ-5b");
    QVERIFY(rowList.at(1).dateOfBirth == "02.02.2005");
    QVERIFY(rowList.at(1).classRows.size() == 1);
    QVERIFY(rowList.at(1).classRows.at(0).className == "5b");
    QVERIFY(rowList.at(1).classRows.at(0).grade == 5);
    QVERIFY(rowList.at(1).classRows.at(0).subject == "");

    // Not Existing
    rowList = studenttable->searchStudentsByComment("GibtsNicht");
    QVERIFY(rowList.size() == 0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------
// testUpdateAddress: tests for an existing student and a not existing student
//--------------------
void TestStudentTable::testUpdateAddress()
{
    // Existing
    QVERIFY(studenttable->updateStudentAddress("Vorname2-5a", "Nachname2-5a", "29.08.2006", "Neue Adresse 5, 32323 Irgendwo"));
    testGetStudent();
    StudentTable::StudentRow* row = studenttable->getStudent("Vorname2-5a", "Nachname2-5a", "29.08.2006");
    QVERIFY(row->preName == "Vorname2-5a");
    QVERIFY(row->surName == "Nachname2-5a");
    QVERIFY(row->dateOfBirth == "29.08.2006");
    QVERIFY(row->address == "Neue Adresse 5, 32323 Irgendwo");
    QVERIFY(row->classRows.size() == 1);
    QVERIFY(row->classRows.at(0).className == "5a");
    QVERIFY(row->classRows.at(0).grade == 5);
    QVERIFY(row->classRows.at(0).subject == "");

    // Not Existing
    QVERIFY(studenttable->updateStudentAddress("Nicht", "Nicht", "Nicht", "Neue Adresse 5, 32323 Irgendwo"));
    row = studenttable->getStudent("Nicht", "Nicht", "Nicht");
    QVERIFY(row->preName.isEmpty());
    QVERIFY(row->surName.isEmpty());
    QVERIFY(row->dateOfBirth.isEmpty());
    QVERIFY(row->address.isEmpty());
    QVERIFY(row->classRows.isEmpty());
}

//--------------------
// testUpdateComment: tests for an existing student and a not existing student
//--------------------
void TestStudentTable::testUpdateComment()
{
    // Existing
    QVERIFY(studenttable->updateStudentComment("Vorname2-5a", "Nachname2-5a", "29.08.2006", "Etwas wichtiges"));
    testGetStudent();
    StudentTable::StudentRow* row = studenttable->getStudent("Vorname2-5a", "Nachname2-5a", "29.08.2006");
    QVERIFY(row->preName == "Vorname2-5a");
    QVERIFY(row->surName == "Nachname2-5a");
    QVERIFY(row->dateOfBirth == "29.08.2006");
    QVERIFY(row->comment == "Etwas wichtiges");
    QVERIFY(row->classRows.size() == 1);
    QVERIFY(row->classRows.at(0).className == "5a");
    QVERIFY(row->classRows.at(0).grade == 5);
    QVERIFY(row->classRows.at(0).subject == "");

    // Not Existing
    QVERIFY(studenttable->updateStudentComment("Nicht", "Nicht", "Nicht", "Etwas wichtiges"));
    row = studenttable->getStudent("Nicht", "Nicht", "Nicht");
    QVERIFY(row->preName.isEmpty());
    QVERIFY(row->surName.isEmpty());
    QVERIFY(row->dateOfBirth.isEmpty());
    QVERIFY(row->comment.isEmpty());
    QVERIFY(row->classRows.isEmpty());
}

//--------------------
// testUpdateClasses: tests for an existing student, a not existing student, for all possible list varieties and a not existing old class
// checks for student-, studentclass-, and classtable
//--------------------
void TestStudentTable::testUpdateClasses()
{
    // Test student object
    testGetStudent();
    // Existing student
    QList<ClassTable::ClassRow> newclassrows = { {"Neu6a", 6, "Neu"}, {"Neu6ab", 6, "NeuNeu"}};
    QList<ClassTable::ClassRow> oldclassnames ={ {"5a", 5, ""}, {"5ab", 5, ""}};
    QVERIFY(studenttable->updateStudentClasses("Vorname1-5a", "Nachname1-5a", "31.07.2007", oldclassnames, newclassrows));
    StudentTable::StudentRow* row = studenttable->getStudent("Vorname1-5a", "Nachname1-5a", "31.07.2007");
    QVERIFY(row->preName == "Vorname1-5a");
    QVERIFY(row->surName == "Nachname1-5a");
    QVERIFY(row->dateOfBirth == "31.07.2007");
    QVERIFY(row->classRows.size() == 2);
    QVERIFY(row->classRows.at(0).className == "Neu6a");
    QVERIFY(row->classRows.at(0).grade == 6);
    QVERIFY(row->classRows.at(0).subject == "Neu");
    QVERIFY(row->classRows.at(1).className == "Neu6ab");
    QVERIFY(row->classRows.at(1).grade == 6);
    QVERIFY(row->classRows.at(1).subject == "NeuNeu");
    // Test studentclass object
    StudentClassTable::StudentClassRow* studentclassrow = studentclasstable->getStudentClass("Vorname1-5a", "Nachname1-5a", "31.07.2007", "Neu6a");
    QVERIFY(studentclassrow->studentPreName == "Vorname1-5a");
    QVERIFY(studentclassrow->studentSurName == "Nachname1-5a");
    QVERIFY(studentclassrow->studentDateOfBirth == "31.07.2007");
    QVERIFY(studentclassrow->className == "Neu6a");
    studentclassrow = studentclasstable->getStudentClass("Vorname1-5a", "Nachname1-5a", "31.07.2007", "Neu6ab");
    QVERIFY(studentclassrow->studentPreName == "Vorname1-5a");
    QVERIFY(studentclassrow->studentSurName == "Nachname1-5a");
    QVERIFY(studentclassrow->studentDateOfBirth == "31.07.2007");
    QVERIFY(studentclassrow->className == "Neu6ab");
    // Test class object
    ClassTable::ClassRow* classrow = classtable->getClass("Neu6a");
    QVERIFY(classrow->className == "Neu6a");
    QVERIFY(classrow->grade == 6);
    QVERIFY(classrow->subject == "Neu");
    classrow = classtable->getClass("Neu6ab");
    QVERIFY(classrow->className == "Neu6ab");
    QVERIFY(classrow->grade == 6);
    QVERIFY(classrow->subject == "NeuNeu");
    // '5ab' should be deleted since there are no students left
    classrow = classtable->getClass("5ab");
    QVERIFY(classrow->className.isEmpty());

    // Not Existing student
    newclassrows = { {"Neu66a", 66, "Neu"}, {"Neu66ab", 66, "NeuNeu"}};
    QVERIFY(studenttable->updateStudentClasses("Nicht", "Nicht", "Nicht", oldclassnames, newclassrows) == false);
    // Empty oldclassname-list
    oldclassnames = {};
    QVERIFY(studenttable->updateStudentClasses("Vorname1-5a", "Nachname1-5a", "31.07.2007", oldclassnames, newclassrows) == false);
    // Empty newclass-list
    oldclassnames = { {"5a", 5, ""}, {"5ab", 5, ""}};
    newclassrows = {};
    QVERIFY(studenttable->updateStudentClasses("Vorname1-5a", "Nachname1-5a", "31.07.2007", oldclassnames, newclassrows) == false);
    // Lists with not the same size
    oldclassnames = { {"5a", 5, ""}, {"5ab", 5, ""}};
    newclassrows = { {"Neu6a", 6, "Neu"} };
    QVERIFY(studenttable->updateStudentClasses("Vorname1-5a", "Nachname1-5a", "31.07.2007", oldclassnames, newclassrows) == false);

    // Not Existing old class
    newclassrows = { {"Neu7a", 7, "Neu"}, {"Neu7ab", 7, "NeuNeu"}};
    oldclassnames = {{"Neu6a", 6, "Neu"}, {"GibtsNicht", 8, ""}};
    QVERIFY(studenttable->updateStudentClasses("Vorname1-5a", "Nachname1-5a", "31.07.2007", oldclassnames, newclassrows) == false);
    row = studenttable->getStudent("Vorname1-5a", "Nachname1-5a", "31.07.2007");
    QVERIFY(row->preName == "Vorname1-5a");
    QVERIFY(row->surName == "Nachname1-5a");
    QVERIFY(row->dateOfBirth == "31.07.2007");
    QVERIFY(row->classRows.size() == 1);
    QVERIFY(row->classRows.at(0).className == "Neu6ab");
    QVERIFY(row->classRows.at(0).grade == 6);
    QVERIFY(row->classRows.at(0).subject == "NeuNeu");
    // Test studentclass object
    studentclassrow = studentclasstable->getStudentClass("Vorname1-5a", "Nachname1-5a", "31.07.2007", "GibtsNicht");
    QVERIFY(studentclassrow->studentPreName.isEmpty());
    studentclassrow = studentclasstable->getStudentClass("Vorname1-5a", "Nachname1-5a", "31.07.2007", "Neu6a");
    QVERIFY(studentclassrow->studentPreName.isEmpty());
    studentclassrow = studentclasstable->getStudentClass("Vorname1-5a", "Nachname1-5a", "31.07.2007", "Neu7ab");
    QVERIFY(studentclassrow->studentPreName.isEmpty());
    studentclassrow = studentclasstable->getStudentClass("Vorname1-5a", "Nachname1-5a", "31.07.2007", "Neu6ab");
    QVERIFY(studentclassrow->className == "Neu6ab");
    // Test class object
    classrow = classtable->getClass("GibtsNicht");
    QVERIFY(classrow->className.isEmpty());
    classrow = classtable->getClass("Neu6a");
    QVERIFY(classrow->className.isEmpty());
    classrow = classtable->getClass("Neu7ab");
    QVERIFY(classrow->className.isEmpty());
    classrow = classtable->getClass("Neu6ab");
    QVERIFY(classrow->className == "Neu6ab");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------
// cleanupTestCase: clean up and close database
//--------------------
void TestStudentTable::cleanupTestCase()
{
    // Save the name in a String, else a warning gets thrown
    QString defaultConnectionName = QSqlDatabase::database().connectionName();
    QSqlDatabase::removeDatabase(defaultConnectionName);
    delete studenttable;
}
