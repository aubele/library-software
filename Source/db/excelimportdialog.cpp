#include "excelimportdialog.h"
#include "ui_excelimportdialog.h"
#include "configcache.h"

#include <QFileDialog>
#include <QDir>
#include <QMap>
#include <QRegExp>
#include <QVBoxLayout>
#include <QDate>


//=======================================================
// Implementations of ExcelImportThread || Thread for Importing Excelfile
//=======================================================

//-------------------------------------------------------
// Constructor
//-------------------------------------------------------
ExcelImportThread::ExcelImportThread(QString asvfile, QString q11file, QString q12file, StudentTable* studTable, LendingTable* lendTable) :
    mAsvFile(asvfile),
    mQ11File(q11file),
    mQ12File(q12file),
    mStudTable(studTable),
    mLendTable(lendTable),
    cfg(ConfigCache::getConfig("liborg.config"))
{
}

//-------------------------------------------------------
// getErrors(): returns the errors that occured
//-------------------------------------------------------
ExcelImportErrors ExcelImportThread::getErrors()
{
    return mErrors;
}

//--------------------------------------------------------
// run(): main function (run) of the thread
//--------------------------------------------------------
void ExcelImportThread::run()
{
    emit progress(0, "Import wird gestartet");

    // first step asv import; return if an error appears
    if(mAsvFile.isEmpty())
    {
        emit progress(100, "Kein ASV-Import gewählt. Import wird abbgebrochen.");
        return;
    }
    else
    {
        if(importASV())
        {
            emit progress (100, "Fehler beim Import der ASV-Datei. Import wird abbgebrochen. Informieren Sie sich über die Fehler mithilfe von 'Fehler anzeigen'.");
            return;
        }
    }

    // q11 import
    bool q11Error = false;
    if (mQ11File.isEmpty())
        emit progress(66, "Kein Q11-Import gewählt. Überspringe Q11-Import.");
    else
    {
        q11Error = importWinQd(mQ11File, 0);
        if(q11Error)
            emit progress (66, "Fehler beim Import der Q11-Datei.");
    }

    // q12 import
    bool q12Error = false;
    if (mQ12File.isEmpty())
        emit progress(99, "Kein Q12-Import gewählt. Überspringe Q12-Import");
    else
    {
        q12Error = importWinQd(mQ12File, 1);
        if(q12Error)
            emit progress (99, "Fehler beim Import der Q12-Datei.");
    }

    // Check for errors in q11/q12
    if (q11Error || q12Error)
        emit progress(100, "Import fehlerhaft. Informieren Sie sich über die Fehler mithilfe von 'Fehler anzeigen'.");
    else
        emit progress(100, "Import erfolgreich abgeschlossen.");
}

//--------------------------------------------------------
// importASV(): imports the asv
//--------------------------------------------------------
bool ExcelImportThread::importASV()
{
    bool containsOnlyName = true;
    ExcelReader reader;

    // Open file
    if (reader.openXLS(mAsvFile) != FREEXL_OK || reader.selectworksheed(0) == -1)
    {
        mErrors.addError("Die angegebene Datei " + mAsvFile + " konnte nicht geöffnet werden!");
        return true;
    }

    QMap<QString, int> mapping;
    unsigned int numberofrows = reader.rowsofselectedsheet();
    // Iterate through rows
    for (unsigned int row = 0; row < numberofrows; row++)
    {
        emit progress((int)((33.0/numberofrows)*(row+1)), QString("Import Zeile %1 von %2 (ASV)").arg(row+1).arg(numberofrows));

        // QString array
        QString values[reader.columnsofselectedsheet()];
        FreeXL_CellValue cell;

        // Iterate through columns
        for (unsigned short col = 1; col < reader.columnsofselectedsheet(); col++)
        {
            cell = reader.getCellValue (row, col);
            if (cell.type != FREEXL_CELL_TEXT && cell.type != FREEXL_CELL_SST_TEXT && cell.type != FREEXL_CELL_NULL
                    && cell.type != FREEXL_CELL_DATE && cell.type != FREEXL_CELL_INT && cell.type != FREEXL_CELL_DOUBLE)
            {
                mErrors.addError("In der Datei " + mAsvFile + " enthält die Zelle in Reihe " + QString::number(row+1) +
                                 " und Spalte " + QString::number(col+1) + " einen ungültigen Wert!");
                return true;
            }

            // the first row contains the headers, they must be read to create a mapping for all columns
            if (row == 0)
            {
                QString value = QString(cell.value.text_value);
                if (value.contains("Familienname"))
                {
                    mapping.insert("Familienname", col);
                    containsOnlyName = false;
                }
                else if (value.contains("Rufname"))
                    mapping.insert("Rufname", col);
                else if (value.contains("Name"))
                    mapping.insert("Name", col);
                else if (value.contains("Klasse"))
                    mapping.insert("Klasse", col);
                else if (value.contains("Geburtsdatum"))
                    mapping.insert("Geburtsdatum", col);
                else if (value.contains("Anschrift"))
                    mapping.insert("Anschrift", col);

                // Check if the file format is correct
                if(col == reader.columnsofselectedsheet()-1)
                {
                    QList<QString> keys = mapping.keys();
                    if(!keys.contains("Klasse") || !keys.contains("Geburtsdatum") || !keys.contains("Anschrift"))
                    {
                        mErrors.addError("Die Datei " + mAsvFile + " besitzt nicht das richtige Format!");
                        return true;
                    }
                    if(mapping.size() == 5)
                    {
                        if(!keys.contains("Rufname") || !keys.contains("Familienname"))
                        {
                            mErrors.addError("Die Datei " + mAsvFile + " besitzt nicht das richtige Format!");
                            return true;
                        }
                    }
                    else if(mapping.size() == 4)
                    {
                        if(!keys.contains("Name"))
                        {
                            mErrors.addError("Die Datei " + mAsvFile + " besitzt nicht das richtige Format!");
                            return true;
                        }
                    }
                    else
                    {
                        mErrors.addError("Die Datei " + mAsvFile + " besitzt nicht das richtige Format!");
                        return true;
                    }

                    // After the file format check is done, we can delete all old students in grade 12, since they left the school
                    remove12thGraders();
                }
            }
            // the other values can be entered in the array
            else
            {
                QString valueS = QString(cell.value.text_value);
                int valueI = cell.value.int_value;
                double valueD = cell.value.double_value;
                if(!valueS.isEmpty())
                    values[col] = valueS;
                else if(valueI != 0)
                    values[col] = QString::number(valueI);
                else if(valueD != 0)
                    values[col] = QString::number(valueD);
            }
        }
        // Create entry in table for the current row
        if (row != 0)
        {
            createTableEntry(values, mapping, containsOnlyName);
        }
    }
    return false;
}

//--------------------------------------------------------
// createTableEntry(): creates the entries in the database
//--------------------------------------------------------
void ExcelImportThread::createTableEntry(QString values[], QMap<QString, int> mapping, bool containsOnlyName)
{
    StudentTable::StudentRow newrow;
    newrow.address = validateAddress(values[mapping["Anschrift"]]);
    // parse date of birth to the right form
    newrow.dateOfBirth = values[mapping["Geburtsdatum"]];

    ClassTable::ClassRow newclass;
    newclass.className = values[mapping["Klasse"]];
    QString classname = newclass.className;
    newclass.grade = classname.remove(QRegExp("[a-zA-Z\\s]")).toInt();
    newrow.classRows.append(newclass);

    if (containsOnlyName)
    {
        QStringList list = values[mapping["Name"]].split(", ");
        if(list.size() == 2)
        {
            newrow.surName = list.at(0);
            newrow.preName = list.at(1);
        }
    }
    else
    {
        newrow.surName = values[mapping["Familienname"]];
        newrow.preName = values[mapping["Rufname"]];
    }
    // This filters irrelevant lines in the asv
    if (newrow.address.isEmpty() || newrow.classRows.size() == 0 || newrow.dateOfBirth.isEmpty() ||
            newrow.preName.isEmpty() || newrow.surName.isEmpty())
    {/* do nothing here */}
    else
    {
        StudentTable::StudentRow *oldrow = mStudTable->getStudent(newrow.preName, newrow.surName, newrow.dateOfBirth);
        if (oldrow->preName.isEmpty() && oldrow->surName.isEmpty() && oldrow->dateOfBirth.isEmpty())
            mStudTable->insertStudent(newrow);
        else
        {
            mStudTable->updateStudentAddress(newrow.preName, newrow.surName, newrow.dateOfBirth, newrow.address);
            ClassTable::ClassRow crow;
            foreach(crow, oldrow->classRows)
                mStudTable->removeStudentClass(newrow.preName, newrow.surName, newrow.dateOfBirth, crow.className);
            foreach(crow, newrow.classRows)
                mStudTable->insertStudentClass(newrow.preName, newrow.surName, newrow.dateOfBirth, crow);
        }
    }
}

//--------------------------------------------------------
// remove12thGraders(): deletes all students which are in the 12th grade and have no lendings
//--------------------------------------------------------
void ExcelImportThread::remove12thGraders()
{
    QList<StudentTable::StudentRow> oldStudents = mStudTable->getStudentsByClassName("12");
    for(StudentTable::StudentRow oldStudent : oldStudents)
    {
        // Delete all lendings from this student
        QList<LendingTable::LendingRow> openLendings = mLendTable->getOpenLendingsFromStudent(oldStudent.preName, oldStudent.surName, oldStudent.dateOfBirth);
        if(openLendings.isEmpty())
        {
            QList<LendingTable::LendingRow> lendList = mLendTable->getLendingsFromStudent(oldStudent.preName, oldStudent.surName, oldStudent.dateOfBirth);
            for(LendingTable::LendingRow lend : lendList)
            {
                mLendTable->removeLending(lend.ID);
            }
            // Delete the student
            mStudTable->removeStudent(oldStudent.preName, oldStudent.surName, oldStudent.dateOfBirth);
        }
        else
        {
            // check if the comment already exists
            if(oldStudent.comment.isEmpty())
                mStudTable->updateStudentComment(oldStudent.preName, oldStudent.surName, oldStudent.dateOfBirth, "Schüler ist bereits ausgetreten, besitzt allerdings noch offene Ausleihen!");
            else if(!oldStudent.comment.contains("Schüler ist bereits ausgetreten, besitzt allerdings noch offene Ausleihen!"))
            {
                QString newComment = oldStudent.comment;
                newComment.append("; Schüler ist bereits ausgetreten, besitzt allerdings noch offene Ausleihen!");
                mStudTable->updateStudentComment(oldStudent.preName, oldStudent.surName, oldStudent.dateOfBirth, newComment);
            }
        }
    }
}

//--------------------------------------------------------
// validateAddress(): checks for commas and adds them if they are missing
//--------------------------------------------------------
QString ExcelImportThread::validateAddress(QString address)
{
    if(address.contains(","))
        return address;
    else
    {
        int index = address.indexOf(QRegExp("\\d{5}"));
        address.insert(index-1, ",");
    }
    return address;
}

//--------------------------------------------------------
// importWinQd(): imports the q11 or q12
//--------------------------------------------------------
bool ExcelImportThread::importWinQd(QString file, int version)
{
    bool error = false;

    // Open the file and try to select it
    ExcelReader reader;
    if (reader.openXLS(file) != FREEXL_OK || reader.selectworksheed(0) == -1)
    {
        mErrors.addError("Die angegebene Datei " + file + " konnte nicht geöffnet werden!");
        return true;
    }
    // Get the name patterns from the q11/q12 files
    QList< QPair<QString, StudentTable::StudentRow> > correctNames = getStudentNamePatterns(version);

    // Iterate through rows
    unsigned int numberofrows = reader.rowsofselectedsheet();
    for (unsigned int row = 0; row < numberofrows; row++)
    {
        error = false;
        if(version == 0)
            emit progress((int)((33.0/numberofrows)*(row+1))+33, QString("Import Zeile %1 von %2 (WinQD 11)").arg(row+1).arg(numberofrows));
        else if(version == 1)
            emit progress((int)((33.0/numberofrows)*(row+1))+66, QString("Import Zeile %1 von %2 (WinQD 12)").arg(row+1).arg(numberofrows));

        FreeXL_CellValue cell;
        // the student from the current row
        StudentTable::StudentRow studentrow;
        // all classes from the current row
        QList<ClassTable::ClassRow> classrows;
        // Iterate through columns
        for (unsigned int col = 0; col < reader.columnsofselectedsheet(); col++)
        {
            cell = reader.getCellValue(row, col);

            // Check format and exit if its not correct
            if(row == 0)
            {
                if(!checkWinQdFormat(col, cell))
                {
                    mErrors.addError("Die Datei " + file + " besitzt nicht das richtige Format!");
                    return true;
                }
            }
            else
            {
                // Do nothing for the first column
                if(col == 0)
                {
                    continue;
                }

                // Only text and null is accepted
                if (cell.type != FREEXL_CELL_TEXT && cell.type != FREEXL_CELL_SST_TEXT && cell.type != FREEXL_CELL_NULL)
                {
                    // If there is an error go to the next row
                    mErrors.addError("In der Datei " + file + " enthält die Zelle in Reihe " + QString::number(row+1) +
                                     " und Spalte " + QString::number(col+1) + " einen ungültigen Wert!");
                    error = true;
                    break;
                }

                // Column for the student name
                if(col == 1)
                {
                    QString studentName = QString(cell.value.text_value);
                    if(studentName.isEmpty())
                        break;
                    studentrow = getValidatedStudent(studentName, correctNames, &(error), file, reader, row);
                    // If there is an error go to the next row
                    if(error)
                        break;
                }
                // Seminare in the second and thrid column dont have books, so we ignore them
                else if(col == 2 || col == 3)
                {/* Do nothing */}
                // The rest are relevant classes
                else
                {
                    ClassTable::ClassRow newclassrow = getClassRow(cell, version);
                    if(!newclassrow.className.isEmpty())
                        classrows.append(newclassrow);
                }
            }
        }

        // Insert all the classes from the student into the db
        for(ClassTable::ClassRow classrow : classrows)
        {
            if(classrow.className.isEmpty() || studentrow.preName.isEmpty() || studentrow.surName.isEmpty() || studentrow.dateOfBirth.isEmpty())
                continue;
            mStudTable->insertStudentClass(studentrow.preName, studentrow.surName, studentrow.dateOfBirth, classrow);
        }
    }
    error = !mErrors.mList.isEmpty();
    return error;
}

//--------------------------------------------------------
// getStudentNamePatterns(): get the correct namepatterns from all the relevant students, so we can compare them with the names from the q11/q12 files
//--------------------------------------------------------
QList< QPair<QString, StudentTable::StudentRow> > ExcelImportThread::getStudentNamePatterns(int version)
{
    QList<StudentTable::StudentRow> studentList;
    // get students from q11
    if(version == 0)
    {
        studentList = mStudTable->getStudentsByClassName("11");
    }
    // get students from q12
    else if(version == 1)
    {
        studentList = mStudTable->getStudentsByClassName("12");
    }
    QList< QPair<QString, StudentTable::StudentRow> > correctNames;
    for(StudentTable::StudentRow student : studentList)
    {
        QString correctName;
        correctName.append(student.surName);
        correctName.append(" ");
        correctName.append(student.preName);
        correctNames.append(QPair<QString, StudentTable::StudentRow>(correctName, student));
    }
    return correctNames;
}

//--------------------------------------------------------
// checkWinQdFormat(): checks if the file is in the correct expected format
//--------------------------------------------------------
bool ExcelImportThread::checkWinQdFormat(int col, FreeXL_CellValue cell)
{
    bool ret = false;
    if(col == 0)
        ret = QString(cell.value.text_value).contains("nr");
    else if(col == 1)
        ret = QString(cell.value.text_value).contains("schuelername");
    else if(col == 2 || col == 3)
    {
        QString val = QString(cell.value.text_value);
        val.remove(QRegExp("[\\d]"));
        ret = val == "s";
    }
    else
    {
        QString val = QString(cell.value.text_value);
        val.remove(QRegExp("[\\d]"));
        ret = val == "f";
    }
    return ret;
}

//--------------------------------------------------------
// getValidatedStudent(): check if the given studentname can get matched to only one studenrow return the studentrow
//--------------------------------------------------------
StudentTable::StudentRow ExcelImportThread::getValidatedStudent(QString studentName, QList< QPair<QString, StudentTable::StudentRow> > correctNames, bool* error, QString file, ExcelReader reader, int row)
{
    StudentTable::StudentRow newrow;

    QList<StudentTable::StudentRow> asvList;
    int count = 0;
    // Check if the given studentname is unique
    for(QPair<QString, StudentTable::StudentRow> student : correctNames)
    {
        // Identical name found
        if(student.first == studentName)
        {
            count ++;
            asvList.append(student.second);
        }
    }
    // It is unique
    if(count == 1)
    {
        newrow = asvList.at(0);
    }
    // There is no student with the given name
    else if(count < 1)
    {
        WinQdEntry entry = getWinQdEntry(reader, row);
        mErrors.addError("Der angegebene Student in der WinQD-Datei " + file + " existiert in der vorher importierten ASV-Datei nicht!",
                         QList<StudentTable::StudentRow>(), entry);
        *error = true;
    }
    // There are multiple student with the given name
    else if(count > 1)
    {
        WinQdEntry entry = getWinQdEntry(reader, row);
        mErrors.addError("Der angegebene Student in der WinQD-Datei " + file + " ist mit mehreren angegebenen Einträgen in der ASV-Datei identisch!",
                         asvList, entry);
        *error = true;
    }

    return newrow;
}

//--------------------------------------------------------
// getWinQdEntry(): get the whole winQd entry to correctly display the error
//--------------------------------------------------------
WinQdEntry ExcelImportThread::getWinQdEntry(ExcelReader reader, int row)
{
    WinQdEntry entry;
    for(unsigned int i = 1; i < reader.columnsofselectedsheet(); i++)
    {
        if(i == 1)
            entry.name = QString(reader.getCellValue(row, i).value.text_value);
        else
        {
            QString classname = QString(reader.getCellValue(row, i).value.text_value);
            if(!classname.isEmpty())
                entry.classes.append(classname);
        }
    }

    return entry;
}

//--------------------------------------------------------
// getClassRow(): get the class from the file as classrow
//--------------------------------------------------------
ClassTable::ClassRow ExcelImportThread::getClassRow(FreeXL_CellValue cell, int version)
{
    QString className = QString(cell.value.text_value);
    ClassTable::ClassRow newclass;

    // Classes with a semicolon at the start dont have books
    if(!className.isEmpty() && className.at(0) == ";")
        return newclass;
    newclass.className = className;

    // Get grade based on the given version
    if(version == 0)
        newclass.grade = 11;
    else if (version == 1)
        newclass.grade = 12;

    // Get real subject name from config
    QString subject = className.remove(QRegExp("[\\d]"));
    newclass.subject = cfg->getValueForKeyInBlock("Fächer", subject, VariousValue(VariousValue::Text, "")).asText();

    return newclass;
}



//=====================================================
// Implementations of ExcelImportDialog
//=====================================================

//--------------------------------------------------------
// Constructor
//--------------------------------------------------------
ExcelImportDialog::ExcelImportDialog(StudentTable* studTable, LendingTable* lendTable, QWidget *parent) :
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    ui(new Ui::ExcelImportDialog)
{
    ui->setupUi(this);
    connect(ui->button_show_error, SIGNAL(clicked()), this, SLOT(button_show_error_clicked()));
    ui->label_asv_warning->setVisible(false);
    ui->label_q11_warning->setVisible(false);
    ui->label_q12_warning->setVisible(false);

    mASVOK = false;
    mQ11OK = true;
    mQ12OK = true;
    mThread = nullptr;
    mStudTable =  studTable;
    mLendTable = lendTable;
}

//--------------------------------------------------------
// Deconstructor
//--------------------------------------------------------
ExcelImportDialog::~ExcelImportDialog()
{
    delete ui;
    delete mThread;
}

//--------------------------------------------------------
// button_select_asv_file_clicked(): select an asv-file
//--------------------------------------------------------
void ExcelImportDialog::button_select_asv_file_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Importdatei (ASV) auswählen:", QDir::homePath(), "Excel 97 (*.xls)");
    if (fileName.contains("asv") || fileName.contains("ASV"))
    {
        mASVOK = true;
        ui->label_asv_warning->setVisible(false);
        ui->button_select_q11_file->setEnabled(true);
        ui->button_select_q12_file->setEnabled(true);
    }
    else if (fileName.isEmpty())
    {
        mASVOK = false;
        ui->label_asv_warning->setVisible(false);
        ui->button_select_q11_file->setEnabled(false);
        ui->button_select_q12_file->setEnabled(false);
    }
    else
    {
        mASVOK = false;
        ui->label_asv_warning->setVisible(true);
        ui->button_select_q11_file->setEnabled(false);
        ui->button_select_q12_file->setEnabled(false);
    }
    ui->label_asv_file->setText(fileName);
    if (mASVOK && mQ11OK && mQ12OK)
        ui->button_start_import->setEnabled(true);
    else
        ui->button_start_import->setEnabled(false);
}

//--------------------------------------------------------
// button_select_q11_file_clicked(): select an q11-file
//--------------------------------------------------------
void ExcelImportDialog::button_select_q11_file_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Importdatei (Q11) auswählen:", QDir::homePath(), "Excel 97 (*.xls)");
    if (fileName.contains("q11") || fileName.contains("Q11") || fileName.isEmpty())
    {
        mQ11OK = true;
        ui->label_q11_warning->setVisible(false);
    }
    else
    {
        mQ11OK = false;
        ui->label_q11_warning->setVisible(true);
    }
    ui->label_q11_file->setText(fileName);
    if (mASVOK && mQ11OK && mQ12OK)
        ui->button_start_import->setEnabled(true);
    else
        ui->button_start_import->setEnabled(false);
}

//--------------------------------------------------------
// button_select_q12_file_clicked(): select an q12-file
//--------------------------------------------------------
void ExcelImportDialog::button_select_q12_file_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Importdatei (Q12) auswählen:", QDir::homePath(), "Excel 97 (*.xls)");
    if (fileName.contains("q12") || fileName.contains("Q12") || fileName.isEmpty())
    {
        mQ12OK = true;
        ui->label_q12_warning->setVisible(false);
    }
    else
    {
        mQ12OK = false;
        ui->label_q12_warning->setVisible(true);
    }
    ui->label_q12_file->setText(fileName);
    if (mASVOK && mQ11OK && mQ12OK)
        ui->button_start_import->setEnabled(true);
    else
        ui->button_start_import->setEnabled(false);
}

//--------------------------------------------------------
// button_start_import_clicked(): start the import
//--------------------------------------------------------
void ExcelImportDialog::button_start_import_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    mThread = new ExcelImportThread(ui->label_asv_file->text(), ui->label_q11_file->text(),
                                                      ui->label_q12_file->text(), mStudTable, mLendTable);
    connect(mThread, SIGNAL(progress(int,QString)), this, SLOT(thread_progress(int,QString)));
    connect(mThread, SIGNAL(finished()), this, SLOT(thread_finished()));
    mThread->start();
    ui->button_close_2->setEnabled(false);
}

//--------------------------------------------------------
// button_show_error_clicked(): show error widget
//--------------------------------------------------------
void ExcelImportDialog::button_show_error_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

//--------------------------------------------------------
// thread_progress(): progressbar
//--------------------------------------------------------
void ExcelImportDialog::thread_progress(int percentage, QString state)
{
    ui->progressbar->setValue(percentage);
    ui->label_status->setText(state);
}

//--------------------------------------------------------
// thread_finished(): setup the error widget + save errors in a file
//--------------------------------------------------------
void ExcelImportDialog::thread_finished()
{
    ui->button_close_2->setEnabled(true);
    ExcelImportErrors errs = mThread->getErrors();
    if(!errs.mList.isEmpty())
        ui->button_show_error->setEnabled(true);

    ExcelErrorTextEdit * widget = new ExcelErrorTextEdit(errs, ui->stackedWidget);
    QVBoxLayout* lay = static_cast<QVBoxLayout*>(ui->pageerrors->layout());
    lay->insertWidget(0, widget);
    errs.save();
}
