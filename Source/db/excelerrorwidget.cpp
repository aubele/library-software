#include "excelerrorwidget.h"
#include <QFile>
#include <QTextStream>
#include <QVBoxLayout>
#include <QSpacerItem>

//===============================
// Implementations of ExcelImportError
//===============================

//-------------------------------
// Constructor
//-------------------------------
ExcelImportErrors::ExcelImportErrors()
{
}

//-------------------------------
// addError(): adds an error the the member-list
//-------------------------------
void ExcelImportErrors::addError(QString message, QList<StudentTable::StudentRow> asvEntries, WinQdEntry winQD)
{
    mList.append(ExcelImportError{message, asvEntries, winQD});
}

//-------------------------------
// save(): saves all errors in a file
//-------------------------------
bool ExcelImportErrors::save()
{
    if (mList.size() == 0)
        return false;
    QFile file("importerror.tmp");
    if (!file.open(QIODevice::WriteOnly))
        return false;

    QTextStream out(&file);
    for(ExcelImportError error : mList)
    {
        out << "<Error>" << endl;

        out << "<Message>" << endl;
        out << error.message << endl;
        out << "</Message>" << endl;

        if(error.asvEntries.isEmpty())
        {
            out << "<ASV/>" << endl;
        }
        else
        {
            out << "<ASV>" << endl;
            for(StudentTable::StudentRow row : error.asvEntries)
            {
                out << "Vorname:" << row.preName << endl;
                out << "Nachname:" << row.surName << endl;
                out << "Geburtsdatum:" << row.dateOfBirth << endl;
                out << "Adresse:" << row.address << endl;
                out << ";" << endl;
            }
            out << "</ASV>" << endl;
        }

        if(error.winQDEntry.name.isEmpty() && error.winQDEntry.classes.isEmpty())
        {
            out << "<WINQD/>" << endl;
        }
        else
        {
            out << "<WINQD>" << endl;
            out << "Name:" << error.winQDEntry.name << endl;
            out << "Klassen:";
            QString classnames;
            for(QString classname : error.winQDEntry.classes)
            {
                classnames.append(classname);
                classnames.append(",");
            }
            classnames.chop(1);
            out << classnames << endl;
            out << "</WINQD>" << endl;
        }
        out << "</Error>" << endl;
    }

    file.close();
    return true;
}

//-------------------------------
// load(): loads all errors from a file
//-------------------------------
// NOT TESTED YET!!!!
bool ExcelImportErrors::load()
{
    QFile file("importerror.tmp");
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();

        if(line == "<Error>")
        {
            ExcelImportError error;
            while(line != "<\Error>")
            {
                line = in.readLine();

                if(line == "</Message>" || line == "<ASV/>" || line == "<WINQD/>")
                    continue;
                if(line == "<Message>")
                    error.message = in.readLine();

                if(line == "<ASV>")
                {
                    StudentTable::StudentRow row;
                    while(line != "</ASV>")
                    {
                        line = in.readLine();
                        if(line.split(":").at(0) == "Vorname")
                            row.preName == line.split(":").at(1);
                        else if(line.split(":").at(0) == "Nachname")
                            row.surName == line.split(":").at(1);
                        else if(line.split(":").at(0) == "Geburtsdatum")
                            row.dateOfBirth == line.split(":").at(1);
                        else if(line.split(":").at(0) == "Adresse")
                            row.address == line.split(":").at(1);
                        else if(line == ";")
                            error.asvEntries.append(row);
                    }
                }

                if(line == "<WINQD")
                {
                    WinQdEntry entry;
                    while(line != "</WINQD>")
                    {
                        line = in.readLine();
                        if(line.split(":").at(0) == "Name")
                            entry.name = line.split(":").at(1);
                        else if(line.split(":").at(0) == "Klassen")
                        {
                            entry.classes = line.split(":").at(1).split(",");
                        }
                    }
                }
            }
        }
    }
    return true;
}

//=================================
// Implementations of ExcelErrorWidget
//=================================

//-------------------------------
// Constructor
//-------------------------------
ExcelErrorTextEdit::ExcelErrorTextEdit(ExcelImportErrors errors, QWidget *parent) :
    QPlainTextEdit(parent)
{
    this->setReadOnly(true);

    if(errors.mList.isEmpty())
        this->appendPlainText("Import erfolgreich abgeschlossen!");
    else
    {
        this->appendPlainText("Import wurde mit folgenden Fehlern abgeschlossen: \n");
    }
    for(ExcelImportError error : errors.mList)
    {
        this->appendPlainText("Fehler: \n" + error.message);
        if(!error.asvEntries.isEmpty())
        {
            if(error.asvEntries.size() == 1)
                this->appendPlainText("ASV-Eintrag:");
            else
                this->appendPlainText("ASV-Einträge:");

            for(StudentTable::StudentRow row : error.asvEntries)
            {
                this->appendPlainText("Vorname: " + row.preName + " | Nachname: " + row.surName + " | Geburtsdatum: " + row.dateOfBirth);
            }
        }
        if(!error.winQDEntry.name.isEmpty())
        {
            this->appendPlainText("WinQD-Eintrag:");
            QString classes;
            for(QString classname : error.winQDEntry.classes)
            {
                classes.append(classname + ", ");
            }
            classes.chop(2);
            this->appendPlainText("Name: " + error.winQDEntry.name + " | Klassen/Kurse: " + classes);
        }
        this->appendPlainText("");
    }
    this->moveCursor(QTextCursor::Start);
}

//-------------------------------
// Deconstructor
//-------------------------------
ExcelErrorTextEdit::~ExcelErrorTextEdit()
{
}
