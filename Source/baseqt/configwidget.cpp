#include "configwidget.h"
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QSpacerItem>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include "configwidgetelement.h"
#include <QFont>

//===============================
// Implementations of ConfigWidget
//===============================

//-------------------------------
// Constructor
//-------------------------------
ConfigWidget::ConfigWidget(Config* cfg, ConfigDialog *parentDialog, QWidget *parent) : QWidget(parent), mCfg(cfg), mParentDialog(parentDialog)
{
    cfg->attach(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    this->setLayout(layout);
    addElements(cfg, layout);
}

//-----------------------------
// Destructor
//-----------------------------
ConfigWidget::~ConfigWidget()
{
    mCfg->detach(this);
    mCfg = nullptr;
    mParentDialog = nullptr;
}

//---------------------------
// cfgblockchanged: is called when a block is changed
// Parameter: name of the block which has changed and Action, name of the action (added or removed)
//---------------------------
void ConfigWidget::cfgblockchanged(QString blockname, Action action) noexcept
{
    Q_UNUSED(blockname) Q_UNUSED(action)
    mParentDialog->setModified(true);
}

//---------------------------
// cfgkeyofblockchanged: is called when a key of a block has changed
// Parameter: name of the block, name of the key which has changed and Action (added, modified, removed)
//---------------------------
void ConfigWidget::cfgkeyofblockchanged(QString blockname, QString keyname, Action action) noexcept
{
    Q_UNUSED(blockname) Q_UNUSED(keyname) Q_UNUSED(action)
    mParentDialog->setModified(true);
}

//-------------------------
// addElements: places items on the gui for each config entry
//-------------------------
void ConfigWidget::addElements(Config* cfg, QVBoxLayout* layout)
{
    QList<QString> blocks = cfg->getAllBlocks();
    QString block;
    foreach (block, blocks)
    {
        QGroupBox* groupbox = new QGroupBox(block, this);
        layout->addWidget(groupbox);
        groupbox->setObjectName(block);
        QVBoxLayout *gLayout = new QVBoxLayout();
        groupbox->setLayout(gLayout);
        QHBoxLayout* hLayout = new QHBoxLayout(this);
        gLayout->addLayout(hLayout);
        hLayout->addWidget(new QLabel(cfg->getCommentOfBlock(block),this), Qt::AlignTop | Qt::AlignLeft);
        QPushButton* button = new QPushButton("Verstecken", this);
        button->setObjectName(block + "-Button");
        hLayout->addWidget(button, 0, Qt::AlignRight | Qt::AlignTop);
        connect(button, SIGNAL(clicked(bool)), this, SLOT(hideClicked()));

        QWidget *widget = new QWidget(this);
        widget->setObjectName(block + "-Widget");
        gLayout->addWidget(widget);
        QVBoxLayout* wLayout = new QVBoxLayout(this);
        widget->setLayout(wLayout);

        QString key;
        QList<QString> keys = cfg->getAllKeysOfBlock(block);
        foreach (key, keys)
        {
            ConfigWidgetElement* element = new ConfigWidgetElement(block, key, cfg, this);
            wLayout->addWidget(element);
            element->setObjectName(block + ", " + key);
        }
    }
}

//--------------------------
// hideClicked: hides the elements of the given block
//--------------------------
void ConfigWidget::hideClicked()
{
    QString senderName = sender()->objectName();
    senderName = senderName.replace("-Button", "");
    QGroupBox* box = findChild<QGroupBox*>(senderName);
    if (box)
    {
        QPushButton* button = dynamic_cast<QPushButton*>(sender());
        if (button->text() == "Verstecken")
        {
            button->setText("Einblenden");
            QWidget* widget = box->findChild<QWidget*>(senderName + "-Widget");
            if(widget)
                widget->hide();
        }
        else if (button->text() == "Einblenden")
        {
            button->setText("Verstecken");
            QWidget* widget = box->findChild<QWidget*>(senderName + "-Widget");
            if(widget)
                widget->show();
        }
    }
}
