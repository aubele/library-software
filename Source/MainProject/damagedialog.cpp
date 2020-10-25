#include "damagedialog.h"
#include "ui_damagedialog.h"

DamageDialog::DamageDialog(QString forename, QString surname, QString booktitle, QWidget *parent) :
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    ui(new Ui::DamageDialog)
{
    ui->setupUi(this);
    ui->label_information->setText(ui->label_information->text().arg(booktitle).arg(surname).arg(forename));
    connect(ui->checkBox, SIGNAL(stateChanged(int)), this, SLOT(checkbox_changed(int)));
    styleWindow();
}

//------------------------------------
// styleWindow styles the window based on the cfg
//------------------------------------
void DamageDialog::styleWindow()
{
    /*Config *cfg = ConfigCache::getConfig("liborg.config");
    if (cfg)
    {
        QColor brightcolor = cfg->getValueForKeyInBlock("Einstellungen", "Background-Dark", VariousValue(VariousValue::RGB_Color, "177,216,228")).asColor();
        QColor darkcolor = cfg->getValueForKeyInBlock("Einstellungen", "Background-Light", VariousValue(VariousValue::RGB_Color, "240,253,253")).asColor();

    }*/
}

DamageDialog::~DamageDialog()
{
    delete ui;
}

QString DamageDialog::getDamage()
{
    if (!ui->textEdit->isEnabled())
        return "";
    return ui->textEdit->toPlainText();
}

void DamageDialog::checkbox_changed(int state)
{
    if (state == Qt::Checked)
        ui->textEdit->setEnabled(true);
    else
        ui->textEdit->setEnabled(false);
}
