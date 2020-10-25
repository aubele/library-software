#include "studentmask.h"
#include "ui_studentmask.h"
#include <selectclass.h>
#include <delegates.h>
#include <QMessageBox>

//===============================
// helper function
//===============================

//-------------------------------
// getClassesOfUi parses the given string to a QList of ClassTable::ClassRow
//-------------------------------
QList<ClassTable::ClassRow> getClassesOfUi(QString textToParse)
{
    QList<ClassTable::ClassRow> returnRows;
    QStringList classes = textToParse.split("\n");
    foreach (QString classText, classes)
    {
        if (classText.isEmpty())
            continue;

        QStringList classInfo = classText.split(", ");
        ClassTable::ClassRow newRow;
        newRow.className = classInfo[0];
        newRow.grade = classInfo[1].toInt();
        if (classInfo.length() < 3)
            newRow.subject = "";
        else
            newRow.subject = classInfo[2];

        returnRows.append(newRow);
    }
    return returnRows;
}

//===============================
// Implementations of Studentmask
//===============================

//-------------------------------
// StudentMask
//-------------------------------
StudentMask::StudentMask(Dataset* set, QWidget* parent) :
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    mDataset(set), mModify(false), mReturnStudent(nullptr),
    ui(new Ui::StudentMask)
{
    ui->setupUi(this);

    //connect the Buttons to the functions
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->pushButton_editClasses, SIGNAL(clicked()), this, SLOT(editClasses()));
    connect(ui->pushButton_apply, SIGNAL(clicked()), this, SLOT(applyData()));

    //Regular Expressions
    QRegExp rxName("^[a-zA-ZöÖäÄüÜß \\-]{1,50}$");
    QRegExp rxStreet("^[a-zA-Z\\döÖäÄüÜß \\.]{1,50}$");
    QRegExp rxBirth("^(3[01]|[12][0-9]|0?[1-9])\\.(1[012]|0?[1-9])\\.((?:19|20|21)\\d{2})$"); //TT.MM.JJJJ
    QRegExp rxPLZ("^\\d{5}$");
    //Validators for the Expressions
    QValidator *validatorName = new QRegExpValidator(rxName, this);
    QValidator *validatorStreet = new QRegExpValidator(rxStreet, this);
    QValidator *validatorBirth = new QRegExpValidator(rxBirth, this);
    QValidator *validatorPLZ = new QRegExpValidator(rxPLZ, this);
    //connect the Validators and the lineEdits
    ui->lineEdit_prename->setValidator(validatorName);
    ui->lineEdit_surname->setValidator(validatorName);
    ui->lineEdit_birth->setValidator(validatorBirth);
    ui->lineEdit_street->setValidator(validatorStreet);
    ui->lineEdit_PLZ->setValidator(validatorPLZ);
    ui->lineEdit_city->setValidator(validatorName);

    styleWindow();
}

//------------------------------------
// styleWindow styles the window based on the cfg
//------------------------------------
void StudentMask::styleWindow()
{
    /*Config *cfg = ConfigCache::getConfig("liborg.config");
    if (cfg)
    {
        QColor brightcolor = cfg->getValueForKeyInBlock("Einstellungen", "Background-Dark", VariousValue(VariousValue::RGB_Color, "177,216,228")).asColor();
        QColor darkcolor = cfg->getValueForKeyInBlock("Einstellungen", "Background-Light", VariousValue(VariousValue::RGB_Color, "240,253,253")).asColor();

    }*/
}

//---------------------------
// Destructor
//---------------------------
StudentMask::~StudentMask()
{
    delete(ui);
    mReturnStudent = nullptr;
}

//-----------------------------
// editClasses called when the classes should be edited (button)
//-----------------------------
void StudentMask::editClasses()
{
    QList<ClassTable::ClassRow> selectedClasses = getClassesOfUi(ui->label_currentClasses->text());
    SelectClass sclass(mDataset, selectedClasses, this);
    if(sclass.exec() == QDialog::Accepted)
    {
        if( sclass.getSelectedClasses().isEmpty())
        {
            QMessageBox box(QMessageBox::Information, "Keine Klasse ausgewählt", "Es wurde keine Klasse ausgewählt. Die Klassen bleiben unverändert", QMessageBox::Ok, this);
            box.exec();
            return;
        }

        selectedClasses = sclass.getSelectedClasses();
        QString classesTxt;
        for(ClassTable::ClassRow crow : selectedClasses)
        {
            classesTxt.append(crow.className);
            classesTxt.append(", ");
            QString grade = QString::number(crow.grade);
            classesTxt.append(grade);
            classesTxt.append(", ");
            classesTxt.append(crow.subject);
            classesTxt.append("\n");
        }
        ui->label_currentClasses->setText(classesTxt);
    }
}

//-----------------------------
// applyData saves the data from the mask
//-----------------------------
void StudentMask::applyData()
{
    //get Data out of the mask
    QString prenameStr = ui->lineEdit_prename->text();
    QString surnameStr = ui->lineEdit_surname->text();
    QString birthStr = ui->lineEdit_birth->text();
    QString streetStr = ui->lineEdit_street->text();
    QString plzStr = ui->lineEdit_PLZ->text();
    QString cityStr = ui->lineEdit_city->text();
    QString adressStr = streetStr + ", " + plzStr + " " + cityStr;
    /*if (adressStr > 50)*/
    QString commentStr = ui->textEdit_comment->toPlainText();
    /*if (commentStrLen > 200)*/

    if (mModify)
    {
        QList<ClassTable::ClassRow> newClasses = getClassesOfUi(ui->label_currentClasses->text());
        bool okay = mDataset->mStudentTable.updateStudentAddress(prenameStr, surnameStr, birthStr,adressStr);
        if (okay)
            okay = mDataset->mStudentTable.updateStudentClasses(prenameStr,surnameStr, birthStr, classes, newClasses);
        if (okay)
            okay = mDataset->mStudentTable.updateStudentComment(prenameStr, surnameStr, birthStr, commentStr);
        if (okay)
        {
            mReturnStudent->address = adressStr;
            mReturnStudent->classRows = newClasses;
            mReturnStudent->comment = commentStr;
        }
    }
    else
    {
        classes = getClassesOfUi(ui->label_currentClasses->text());
        mReturnStudent = new StudentTable::StudentRow;
        mReturnStudent->preName = prenameStr;
        mReturnStudent->surName = surnameStr;
        mReturnStudent->address = adressStr;
        mReturnStudent->dateOfBirth = birthStr;
        mReturnStudent->comment = commentStr;
        mReturnStudent->classRows = classes;
        mDataset->mStudentTable.insertStudent(*mReturnStudent);
    }
    accept();
}

//-----------------------------
// helper function to get plz, street and city out of adress field
//-----------------------------
void parseAdresss(QString address, QString *plz, QString *street, QString *city)
{
    *plz = "";
    *street = "";
    *city = "";

    if (address.isEmpty())
    {
        return;
    }

    QStringList list = address.split(", ");
    if (list.size() < 2)
    {
        *street = address;
        return;
    }
    QStringList list1 = list.at(1).split(" ");
    if (list1.size() < 2)
        *city = list.at(1);
    else
    {
        *plz = list1.at(0);
        *city = list1.at(1);
    }
    *street = list.at(0);
}

//-----------------------------
// selectStudent places the values of the given student to the ui
//-----------------------------
void StudentMask::selectStudent(StudentTable::StudentRow* row)
{
    mModify = true;
    mReturnStudent = row;
    ui->lineEdit_prename->setText(row->preName);
    ui->lineEdit_prename->setEnabled(false);
    ui->lineEdit_surname->setText(row->surName);
    ui->lineEdit_surname->setEnabled(false);
    ui->lineEdit_birth->setText(row->dateOfBirth);
    ui->lineEdit_birth->setEnabled(false);
    QString plz; QString city; QString street;
    parseAdresss(row->address, &plz, &street, &city);
    ui->lineEdit_street->setText(street);
    ui->lineEdit_PLZ->setText(plz);
    ui->lineEdit_city->setText(city);
    ui->textEdit_comment->setText(row->comment);
    classes = row->classRows;
    QString classesTxt;
    for(ClassTable::ClassRow crow : classes)
    {
        classesTxt.append(crow.className);
        classesTxt.append(", ");
        QString grade = QString::number(crow.grade);
        classesTxt.append(grade);
        classesTxt.append(", ");
        classesTxt.append(crow.subject);
        classesTxt.append("\n");
    }
    ui->label_currentClasses->setText(classesTxt);
}

//-----------------------------
// getNewStudent returns the new created student
//-----------------------------
StudentTable::StudentRow* StudentMask::getNewStudent()
{
    return mReturnStudent;
}
