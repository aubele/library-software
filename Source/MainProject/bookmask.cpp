#include "bookmask.h"
#include "ui_bookmask.h"
#include <configcache.h>

#include <QMessageBox>

//===============================
// Implementations of BookMask
//===============================

//-------------------------------
// Constructor
//-------------------------------
BookMask::BookMask(BookTable *table, QWidget *parent) :
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    ui(new Ui::BookMask), mManipulateRow(false), mRow(nullptr), mTable(table)
{
    ui->setupUi(this);
    setupComboBoxSubject();
    connect(ui->button_save, SIGNAL(clicked()), this, SLOT(button_save_clicked()));

    //Regular Expressions
    QRegExp rxName("^[a-zA-ZöÖäÄüÜß \\-]{1,50}$");
    //QRegExp rxIsbn("^^(97)[8|9][-| ]?[0-9]{1}[-| ]?[0-9]{5}[-| ]?[0-9]{3}[-| ]?[0-9]{1}$"); //without the String "ISBN" in front of the numbers, and with code 978/ 979 in front
    QRegExp rxIsbn("^[0-9]{3}[-| ]?[0-9]{1}[-| ]?[0-9]{5}[-| ]?[0-9]{3}[-| ]?[0-9]{1}$"); //without the String "ISBN" in front of the numbers
    QRegExp rxLastStocktaking("^(3[01]|[12][0-9]|0?[1-9])\\.(1[012]|0?[1-9])\\.((?:20|21)\\d{2})$"); //TT.MM.JJJJ
    QRegExp rxPrice("^\\d{1,10}(\\,\\d{1,2})?\\ {0,1}\\€{0,1}$"); // 999,99 -> , und zwei Stellen danach oder auch ohne Komma
    QRegExp rxPublisher("^[a-zA-ZöÖäÄüÜß1234567890 \\-]{1,50}$");
    //QRegExp rxTitel("[a-zA-ZöÖäÄüÜß1234567890 \\-]{1,50}$");

    //Validators for the Expressions
    QValidator *validatorAuthor = new QRegExpValidator(rxName, this);
    QValidator *validatorAvailable = new QIntValidator(0, 99999);
    QValidator *validatorEdition = new QIntValidator(1, 999);
    QValidator *validatorIsbn = new QRegExpValidator(rxIsbn, this);
    QValidator *validatorLastStocktaking = new QRegExpValidator(rxLastStocktaking, this);
    QValidator *validatorPrice = new QRegExpValidator(rxPrice, this);
    QValidator *validatorPublisher = new QRegExpValidator(rxPublisher, this);
    QValidator *validatorStock = new QIntValidator(0, 99999);
    QValidator *validatorStocktakingCount = new QIntValidator(0, 99999);
    //QValidator *validatorTitel = new QRegExpValidator(rxTitel, this);
    //QValidator *validatorUtitel = new QRegExpValidator(rxTitel, this);

    //connect the Validators and the lineEdits
    ui->lineEdit_author->setValidator(validatorAuthor);
    ui->lineEdit_available->setValidator(validatorAvailable);
    ui->lineEdit_editiion->setValidator(validatorEdition);
    ui->lineEdit_isbn->setValidator(validatorIsbn);
    ui->lineEdit_laststocktaking->setValidator(validatorLastStocktaking);
    ui->lineEdit_price->setValidator(validatorPrice);
    ui->lineEdit_publisher->setValidator(validatorPublisher);
    ui->lineEdit_stock->setValidator(validatorStock);
    ui->lineEdit_stocktakingcount->setValidator(validatorStocktakingCount);
    //ui->lineEdit_titel->setValidator(validatorTitel);
    //ui->lineEdit_utitle->setValidator(validatorUtitel);

    //TODO Überprüfen auf leere Felder
    styleWindow();
}

//------------------------------------
// styleWindow styles the window based on the cfg
//------------------------------------
void BookMask::styleWindow()
{
    /*Config *cfg = ConfigCache::getConfig("liborg.config");
    if (cfg)
    {
        QColor brightcolor = cfg->getValueForKeyInBlock("Einstellungen", "Background-Dark", VariousValue(VariousValue::RGB_Color, "177,216,228")).asColor();
        QColor darkcolor = cfg->getValueForKeyInBlock("Einstellungen", "Background-Light", VariousValue(VariousValue::RGB_Color, "240,253,253")).asColor();

    }*/
}

//-------------------------------
// Destructor
//-------------------------------
BookMask::~BookMask()
{
    delete ui;
    mRow = nullptr;
}

//-------------------------------
// getInsertedBookRow
//-------------------------------
BookTable::BookRow* BookMask::getInsertedBookRow()
{
    if (mManipulateRow)
        return nullptr;
    else
        return mRow;
}

//-------------------------------
// setBookRow reads the values out of the given row and places them on the ui
//-------------------------------
void BookMask::setBookRow(BookTable::BookRow* row)
{
    mManipulateRow = true;
    mRow = row;
    ui->lineEdit_isbn->setEnabled(false);
    ui->lineEdit_available->setText(QString::number(row->available));
    ui->lineEdit_isbn->setText(row->isbn);
    ui->lineEdit_laststocktaking->setText(row->stocktakingDate);
    ui->lineEdit_stock->setText(QString::number(row->stock));
    ui->lineEdit_stocktakingcount->setText(QString::number(row->stocktakingCount));
    ui->lineEdit_titel->setText(row->title);
    ui->lineEdit_utitle->setText(row->subtitle);
    ui->lineEdit_author->setText(row->author);
    ui->lineEdit_editiion->setText(QString::number(row->edition));
    ui->lineEdit_price->setText(row->price);
    ui->lineEdit_publisher->setText(row->publisher);
    ui->textEdit_comment->setText(row->comment);
    checkCheckboxes(row->grade);
    ui->comboBox_subject->setCurrentText(row->subject);
    setWindowTitle("Bearbeiten von " + row->isbn);
}

//--------------------------------
// checkCheckboxes the checkboxes for each class is check based on the grade list
//--------------------------------
void BookMask::checkCheckboxes(QList<int> grades)
{
    int grade;
    foreach(grade, grades)
    {
        switch(grade)
        {
        case 5:
            ui->checkBox_class5->setChecked(true);
            break;
        case 6:
            ui->checkBox_class6->setChecked(true);
            break;
        case 7:
            ui->checkBox_class7->setChecked(true);
            break;
        case 8:
            ui->checkBox_class8->setChecked(true);
            break;
        case 9:
            ui->checkBox_class9->setChecked(true);
            break;
        case 10:
            ui->checkBox_class10->setChecked(true);
            break;
        case 11:
            ui->checkBox_class11->setChecked(true);
            break;
        case 12:
            ui->checkBox_12->setChecked(true);
            break;
        case 13:
            ui->checkBox_class13->setChecked(true);
            break;
        }
    }
}

//--------------------------------
// getSelectedGrade returns a qlist of ints containing the selected grades
//--------------------------------
QList<int> BookMask::getSelectedGrade()
{
    QList<int> returnList;
    if (ui->checkBox_class5->isChecked())
        returnList.append(5);
    if (ui->checkBox_class6->isChecked())
        returnList.append(6);
    if (ui->checkBox_class7->isChecked())
        returnList.append(7);
    if (ui->checkBox_class8->isChecked())
        returnList.append(8);
    if (ui->checkBox_class9->isChecked())
        returnList.append(9);
    if (ui->checkBox_class10->isChecked())
        returnList.append(10);
    if (ui->checkBox_class11->isChecked())
        returnList.append(11);
    if (ui->checkBox_12->isChecked())
        returnList.append(12);
    if(ui->checkBox_class13->isChecked())
        returnList.append(13);
    return returnList;
}

//--------------------------------
// setupComboBoxSubject adds all class names to the combobox
//--------------------------------
void BookMask::setupComboBoxSubject()
{
    Config* cfg = ConfigCache::getConfig("liborg.config");
    if (!cfg)
        return;

    QList<QString> classShortcuts = cfg->getAllKeysOfBlock("Fächer");
    QString classShortcut;
    foreach(classShortcut, classShortcuts)
    {
        QString className = cfg->getValueForKeyInBlock("Fächer", classShortcut,VariousValue(VariousValue::Text, "")).asText();
        ui->comboBox_subject->addItem(className);
    }
    ui->comboBox_subject->setCurrentIndex(-1);
}

//--------------------------------
// button_save_clicked sets the new values on the row and updates the database
//--------------------------------
void BookMask::button_save_clicked()
{
    bool wrong_input = false;
    int failure_type = 0;
    if (mRow == nullptr)
        mRow = new BookTable::BookRow;

    QString author = ui->lineEdit_author->text();
    int available = ui->lineEdit_available->text().toInt();
    QString comment = ui->textEdit_comment->toPlainText();
    int edition = ui->lineEdit_editiion->text().toInt();
    QList<int> grades = getSelectedGrade();
    QString isbn = ui->lineEdit_isbn->text();
    isbn.replace("-","");
    QString price = ui->lineEdit_price->text();
    QString publisher = ui->lineEdit_publisher->text();
    int stock = ui->lineEdit_stock->text().toInt();
    int stocktakingCount = ui->lineEdit_stocktakingcount->text().toInt();
    QString stocktakingDate = ui->lineEdit_laststocktaking->text();
    QString subject = ui->comboBox_subject->currentText();
    QString subtitle = ui->lineEdit_utitle->text();
    QString title = ui->lineEdit_titel->text();
    if(isbn.isEmpty())
    {
        wrong_input = true;
        failure_type = 1;
    }
    else if(!mTable->getBook(isbn)->isbn.isEmpty() && !mManipulateRow)
    {
        wrong_input = true;
        failure_type = 2;
    }
    else if(title.isEmpty())
    {
        wrong_input = true;
        failure_type = 3;
    }
    if(wrong_input)
    {
        // Setup messagebox
        QMessageBox msgBox;
        if(failure_type == 1)
        {
            msgBox.setWindowTitle("Falsche ISBN");
            msgBox.setText("Leider ist die eingegeben ISBN nicht korrekt.");
            ui->label_3->setStyleSheet("color:red");
            ui->lineEdit_isbn->setStyleSheet("border-color:red");
        }
        if(failure_type == 2)
        {
            msgBox.setWindowTitle("ISBN bereits vorhanden");
            msgBox.setText("Es ist bereits das Buch: " + mTable->getBook(mRow->isbn)->title + " mit der ISBN: " + mRow->isbn + " vorhanden.");
            ui->label_3->setStyleSheet("color:red");
            ui->lineEdit_isbn->setStyleSheet("border-color:red");
        }
        if(failure_type == 3)
        {
            msgBox.setWindowTitle("Leerer Titel");
            msgBox.setText("Für jedes Buch muss ein Titel angegeben werden.");
            ui->label->setStyleSheet("color:red");
            ui->lineEdit_titel->setStyleSheet("border-color:red");
        }
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setButtonText(QMessageBox::Ok, "Ok");
        msgBox.exec();
        if(!mManipulateRow)
            delete mRow;
        mRow = nullptr;
        return;
    }

    if(stock == 0 || available == 0 || subject.isEmpty() || grades.length() == 0)
    {
        QMessageBox box;
        box.setWindowTitle("Unausgefüllte Eintragungen");
        box.setIcon(QMessageBox::Warning);
        QString warning;
        if (stock == 0)
            warning += "Es wurde keine Zahl für die Anzahl der vorhandenen Bücher eingetragen!\n";
        if (available == 0)
            warning += "Die Anzahl der verfügbaren Bücher wurde nicht gesetzt!\n";
        if (subject.isEmpty())
            warning += "Das Buch wurde keinem Fach zugewiesen!\n";
        if (grades.length() == 0)
            warning += "Es wurden keine Jahrgangsstufen für das Buch ausgewählt!\n";
        warning += "\nWollen Sie das Buch speichern oder weiter bearbeiten?";
        box.setText(warning);
        box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        box.setButtonText(QMessageBox::Yes,"Speichern");
        box.setButtonText(QMessageBox::No, "Weiter bearbeiten");
        if (box.exec() == QMessageBox::No)
            return;
    }
    bool allwentwell = true;
    if (mManipulateRow)
    {
        allwentwell &= mTable->updateAuthor(isbn, author);
        if (allwentwell)
            mRow->author = author;
        allwentwell &= mTable->updateAvailable(isbn, available);
        if (allwentwell)
            mRow->available = available;
        allwentwell &= mTable->updateComment(isbn, comment);
        if(allwentwell)
            mRow->comment = comment;
        allwentwell &= mTable->updateEdition(isbn, edition);
        if (allwentwell)
            mRow->edition = edition;
        allwentwell &= mTable->updateGrade(isbn, grades);
        if (allwentwell)
            mRow->grade = grades;
        allwentwell &= mTable->updatePrice(isbn, price);
        if (allwentwell)
            mRow->price = price;
        allwentwell &= mTable->updatePublisher(isbn, publisher);
        if(allwentwell)
            mRow->publisher = publisher;
        allwentwell &= mTable->updateStock(isbn, stock);
        if(allwentwell)
            mRow->stock = stock;
        allwentwell &= mTable->updateStocktakingCount(isbn, stocktakingCount);
        if(allwentwell)
            mRow->stocktakingCount = stocktakingCount;
        allwentwell &= mTable->updateStocktakingDate(isbn, stocktakingDate);
        if(allwentwell)
            mRow->stocktakingDate = stocktakingDate;
        allwentwell &= mTable->updateSubject(isbn, subject);
        if(allwentwell)
            mRow->subject = subject;
        allwentwell &= mTable->updateSubTitle(isbn, subtitle);
        if(allwentwell)
            mRow->subtitle = subtitle;
        allwentwell &= mTable->updateTitle(isbn, title);
        if(allwentwell)
            mRow->title = title;
    }
    else
    {
        mRow->author = author;
        mRow->isbn = isbn;
        mRow->available = available;
        mRow->comment = comment;
        mRow->edition = edition;
        mRow->grade = grades;
        mRow->price = price;
        mRow->publisher = publisher;
        mRow->stock = stock;
        mRow->stocktakingCount = stocktakingCount;
        mRow->stocktakingDate = stocktakingDate;
        mRow->subject = subject;
        mRow->subtitle = subtitle;
        mRow->title = title;
        allwentwell &= mTable->insert(*mRow);
    }

    if (!allwentwell)
    {
        QMessageBox box;
        box.setWindowTitle("Datenübertragung nicht möglich!");
        box.setIcon(QMessageBox::Critical);
        box.setText("Die Änderungen könnten unerwarteterweise nicht in die Datenbank geschrieben werden.");
        box.exec();
    }
    if(!allwentwell && !mManipulateRow)
    {
        delete mRow;
        mRow = nullptr;
        QDialog::reject();
    }
    else
    {
        QDialog::accept();
    }
}
