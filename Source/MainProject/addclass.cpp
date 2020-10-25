#include "addclass.h"
#include "ui_addclass.h"
#include <configcache.h>

//====================================
// Implementation of addclass
//====================================

//------------------------------------
// Konstructor
//------------------------------------
addclass::addclass(QWidget *parent) :
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    ui(new Ui::addclass)
{
    ui->setupUi(this);

    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui->pushButton_add, SIGNAL(clicked()), this, SLOT(add()));

    QValidator *validatoGrade = new QIntValidator(5, 13, this);
    QRegExp rxClassName("^[\\S]{1,10}$");
    QRegExp rxSubject("^[\\S]{1,50}$");
    QValidator *validatorClassName = new QRegExpValidator(rxClassName, this);
    QValidator *validatorSubject = new QRegExpValidator(rxSubject, this);
    ui->lineEdit_grade->setValidator(validatoGrade);
    ui->lineEdit_className->setValidator(validatorClassName);
    setupComboBoxSubject();
    styleWindow();
}

//--------------------------------
// setupComboBoxSubject adds all class names to the combobox
//--------------------------------
void addclass::setupComboBoxSubject()
{
    Config* cfg = ConfigCache::getConfig("liborg.config");
    if (!cfg)
        return;

    QList<QString> classShortcuts = cfg->getAllKeysOfBlock("Fächer");
    QString classShortcut;
    foreach(classShortcut, classShortcuts)
    {
        QString className = cfg->getValueForKeyInBlock("Fächer", classShortcut,VariousValue(VariousValue::Text, "")).asText();
        ui->comboBox_Subject->addItem(className);
    }
    ui->comboBox_Subject->setCurrentIndex(-1);
}

//------------------------------------
// styleWindow styles the window based on the cfg
//------------------------------------
void addclass::styleWindow()
{
    /*Config *cfg = ConfigCache::getConfig("liborg.config");
    if (cfg)
    {
        QColor brightcolor = cfg->getValueForKeyInBlock("Einstellungen", "Background-Dark", VariousValue(VariousValue::RGB_Color, "177,216,228")).asColor();
        QColor darkcolor = cfg->getValueForKeyInBlock("Einstellungen", "Background-Light", VariousValue(VariousValue::RGB_Color, "240,253,253")).asColor();

    }*/
}

//--------------------------
// Destructor
//--------------------------
addclass::~addclass()
{
    delete ui;
}

//--------------------------
// add() called by the add button
//--------------------------
void addclass::add()
{
    int grade = ui->lineEdit_grade->text().toInt();
    QString classStr = ui->lineEdit_className->text();
    QString subjectStr = ui->comboBox_Subject->currentText();

    newClass.className = classStr;
    newClass.grade = grade;
    newClass.subject = subjectStr;

    accept();
}

//-----------------------------
// getNewClass returns the new class to be inserted
//-----------------------------
ClassTable::ClassRow addclass::getNewClass()
{
    return newClass;
}
