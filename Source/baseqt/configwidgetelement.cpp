#include "configwidgetelement.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QList>
#include <QColorDialog>

//====================================
// Implementations of ConfigWidgetElement
//====================================

//------------------------------------
// Constructor
//------------------------------------
ConfigWidgetElement::ConfigWidgetElement(QString blockname, QString keyname, Config* cfg, QWidget *parent) :
    QWidget(parent), mCfg(cfg), mName(nullptr), mEdit(nullptr), mComment(nullptr), mColorButton(nullptr)
{
    mBlockname = blockname; mKeyname = keyname;

    QVBoxLayout * layout = new QVBoxLayout(this);
    mName = new QLabel(keyname, this);
    mName->setStyleSheet("font-weight: bold;");
    QHBoxLayout *hlayout = new QHBoxLayout(this);
    layout->addLayout(hlayout);
    hlayout->addWidget(mName);
    switch (cfg->getTypeOfKeyInBlock(blockname, keyname))
    {
        case VariousValue::Bool:
        {
            mEdit = new QCheckBox("",this);
            hlayout->addWidget(mEdit);
            dynamic_cast<QCheckBox*>(mEdit)->setChecked(cfg->getValueForKeyInBlock(blockname, keyname, VariousValue(VariousValue::Bool, "0")).asBool());
            connect(dynamic_cast<QCheckBox*>(mEdit), SIGNAL(stateChanged(int)), this, SLOT(checkboxChanged(int)));
            break;
        }
        case VariousValue::Double:
        {
            mEdit = new QDoubleSpinBox(this);
            hlayout->addWidget(mEdit);
            QDoubleSpinBox *box = dynamic_cast<QDoubleSpinBox*>(mEdit);
            box->setValue(cfg->getValueForKeyInBlock(blockname, keyname, VariousValue(VariousValue::Double, "0")).asDouble());
            if (cfg->hasKeyMinValueInBlock(blockname, keyname))
                box->setMinimum(cfg->getMinValueOfKeyInBlock(blockname,keyname).asDouble());
            if (cfg->hasKeyMaxValueInBlock(blockname, keyname))
                box->setMaximum(cfg->getMaxValueOfKeyInBlock(blockname, keyname).asDouble());
            connect(box, SIGNAL(valueChanged(double)), this, SLOT(dspinboxchanged(double)));
            break;
        }
        case VariousValue::Int:
        {
            mEdit = new QSpinBox(this);
            hlayout->addWidget(mEdit);
            QSpinBox *box = dynamic_cast<QSpinBox*>(mEdit);
            box->setValue(cfg->getValueForKeyInBlock(blockname, keyname, VariousValue(VariousValue::Int, "0")).asInt());
            if (cfg->hasKeyMinValueInBlock(blockname, keyname))
                box->setMinimum(cfg->getMinValueOfKeyInBlock(blockname,keyname).asInt());
            if (cfg->hasKeyMaxValueInBlock(blockname, keyname))
                box->setMaximum(cfg->getMaxValueOfKeyInBlock(blockname, keyname).asInt());
            connect(box, SIGNAL(valueChanged(int)), this, SLOT(spinboxchanged(int)));
            break;
        }
        case VariousValue::RGB_Color:
        {
            mEdit = new QLineEdit(cfg->getValueForKeyInBlock(blockname, keyname, VariousValue(VariousValue::RGB_Color, "240,253,253")).asText(), this);
            hlayout->addWidget(mEdit);
            QLineEdit* edit = dynamic_cast<QLineEdit*>(mEdit);
            connect(edit, SIGNAL(textEdited(QString)), this, SLOT(textchanged(QString)));
            edit->setMaxLength(17);
            mColorButton = new QPushButton("Auswählen", this);
            connect(mColorButton, SIGNAL(clicked()), this, SLOT(colorbuttonclicked()));
            hlayout->addWidget(mColorButton);
            break;
        }
        case VariousValue::SelectEntryText:
        {
            mEdit = new QComboBox(this);
            hlayout->addWidget(mEdit);
            QComboBox *box = dynamic_cast<QComboBox*>(mEdit);
            box->setCurrentText(cfg->getValueForKeyInBlock(blockname, keyname, VariousValue(VariousValue::SelectEntryText, "")).asText());
            box->setEditable(true);

            if (cfg->hasKeyMaxLengthInBlock(blockname, keyname))
                box->lineEdit()->setMaxLength(cfg->getMaxLengthOfKeyInBlock(blockname, keyname));

            if (cfg->hasKeyPossibleValuesInBlock(blockname, keyname))
            {
                QList<QString> list = cfg->getPossibleValuesOfKeyInBlock(blockname, keyname);
                QString item;
                foreach (item, list)
                {
                    box->addItem(item);
                }
            }
            connect(box, SIGNAL(currentTextChanged(QString)), this, SLOT(comboboxtextchanged(QString)));

            break;
        }
        case VariousValue::SelectText:
        {
            mEdit = new QComboBox(this);
            hlayout->addWidget(mEdit);
            QComboBox *box = dynamic_cast<QComboBox*>(mEdit);
            box->setCurrentText(cfg->getValueForKeyInBlock(blockname, keyname, VariousValue(VariousValue::SelectText, "")).asText());

            if (cfg->hasKeyMaxLengthInBlock(blockname, keyname))
                box->lineEdit()->setMaxLength(cfg->getMaxLengthOfKeyInBlock(blockname, keyname));

            if (cfg->hasKeyPossibleValuesInBlock(blockname, keyname))
            {
                QList<QString> list = cfg->getPossibleValuesOfKeyInBlock(blockname, keyname);
                QString item;
                foreach (item, list)
                {
                    box->addItem(item);
                }
            }
            connect(box, SIGNAL(currentTextChanged(QString)), this, SLOT(comboboxtextchanged(QString)));

            break;
        }
        case VariousValue::Text:
        {
            mEdit = new QLineEdit(cfg->getValueForKeyInBlock(blockname, keyname, VariousValue(VariousValue::Text, "")).asText(), this);
            hlayout->addWidget(mEdit);
            QLineEdit* edit = dynamic_cast<QLineEdit*>(mEdit);
            connect(edit, SIGNAL(textEdited(QString)), this, SLOT(textchanged(QString)));

            if (cfg->hasKeyMaxLengthInBlock(blockname,keyname))
                edit->setMaxLength(cfg->getMaxLengthOfKeyInBlock(blockname, keyname));

            break;
        }
    }

    if(cfg->isKeyReadOnlyOfBlock(blockname, keyname))
        mEdit->setEnabled(false);

    mComment = new QLabel(cfg->getCommentOfKeyInBlock(blockname, keyname), this);
    layout->addWidget(mComment);
}

//------------------------------------
// Destructor
//------------------------------------
ConfigWidgetElement::~ConfigWidgetElement()
{
    mCfg = nullptr;
    delete mName;
    delete mEdit;
    delete mComment;
    delete mColorButton;
}

//------------------------------------
// Private Slots
//------------------------------------

//- - - - - - - - - - - - - - - - - - -
// checkboxChanged: saves the new value on the config
//- - - - - - - - - - - - - - - - - - -
void ConfigWidgetElement::checkboxChanged(int state)
{
    bool checked = state == Qt::Checked;
    QString value;
    if (checked)
        value = "1";
    else
        value = "0";
    mCfg->setValueForKeyInBlock(mBlockname, mKeyname, value);
}

//- - - - - - - - - - - - - - - - - - -
// dspinboxchanged: saves the new value on the config
//- - - - - - - - - - - - - - - - - - -
void ConfigWidgetElement::dspinboxchanged(double newvalue)
{
    mCfg->setValueForKeyInBlock(mBlockname, mKeyname, QString::number(newvalue, 'g', 2));
}

//- - - - - - - - - - - - - - - - - - -
// spinboxchanged: saves the new value on the config
//- - - - - - - - - - - - - - - - - - -
void ConfigWidgetElement::spinboxchanged(int newvalue)
{
    mCfg->setValueForKeyInBlock(mBlockname, mKeyname, QString::number(newvalue));
}

//- - - - - - - - - - - - - - - - - - -
// textchanged: saves the new value on the config
//- - - - - - - - - - - - - - - - - - -
void ConfigWidgetElement::textchanged(QString newvalue)
{
    mCfg->setValueForKeyInBlock(mBlockname, mKeyname, newvalue);
}

//- - - - - - - - - - - - - - - - - - -
// colorbuttonclicked(): opens a choose color dialog and returns the selected value
//- - - - - - - - - - - - - - - - - - -
void ConfigWidgetElement::colorbuttonclicked()
{
    QLineEdit* edit = dynamic_cast<QLineEdit*>(mEdit);
    QList<QString> list = edit->text().split(",");
    QColor color(list[0].toInt(), list[1].toInt(), list[2].toInt());
    QColorDialog dialog(color, this);
    if(dialog.exec() == QDialog::Accepted)
    {
        color = dialog.selectedColor();
        QString value = QString::number(color.red()) + "," + QString::number(color.green()) + "," + QString::number(color.blue());
        edit->setText(value);
        mCfg->setValueForKeyInBlock(mBlockname, mKeyname, value);
    }
}

//- - - - - - - - - - - - - - - - - - -
// comboboxtextchanged: saves the new value on the config
//- - - - - - - - - - - - - - - - - - -
void ConfigWidgetElement::comboboxtextchanged(QString newvalue)
{
    mCfg->setValueForKeyInBlock(mBlockname, mKeyname, newvalue);
}
