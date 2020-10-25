#include "delegates.h"
#include <QCheckBox>
#include <QApplication>
#include <QPainter>
#include <QKeyEvent>
#include <QCheckBox>

CheckBoxDelegate::CheckBoxDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{

}


void CheckBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 viewItemOption(option);

    if (index.column() == 0)
    {
        const int textMargin = 3;
        QSize size (20,20);
        QRect newRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
                                                        QSize(62,20),
                                                        QRect(option.rect.x() + textMargin, option.rect.y(),
                                                              option.rect.width() - (2 * textMargin), option.rect.height()));
        int amount = index.data(Qt::CheckStateRole).toInt();
        QStyleOptionButton checkBox;
        checkBox.iconSize = size;
        checkBox.rect = QRect(newRect.x()+size.width()+1, newRect.y(), size.width(), size.height());
        checkBox.state = QStyle::State_Enabled | QStyle::State_Active;
        if (amount == 1)
            checkBox.state |= QStyle::State_On; // checkBox.state = checkBox.state | QStyle::State_On
        else
            checkBox.state |= QStyle::State_Off;
        //QVariant test = index.data(Qt::CheckStateRole);
        //bool test1 = test.isValid();
        QApplication::style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &checkBox, painter);
    }
    if (index.column() > 0)
    {
        QStyledItemDelegate::paint(painter, viewItemOption, index);
    }
}

bool CheckBoxDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    // make sure that the item is checkable
    Qt::ItemFlags flags = model->flags(index);
    if (!(flags & Qt::ItemIsUserCheckable) || !(flags & Qt::ItemIsEnabled))
        return false;

    // make sure that we have a check state
    QVariant value = index.data(Qt::CheckStateRole);
    if (!value.isValid())
        return false;

    // make sure that we have the right event type
    if (event->type() == QEvent::MouseButtonRelease)
    {
        if (index.column() == 0)
        {
            const int textMargin = 3;
            QSize size (20,20);
            QRect newRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
                             QSize(62,20),QRect(option.rect.x() + textMargin, option.rect.y(),
                             option.rect.width() - (2 * textMargin), option.rect.height()));
            bool state = index.data(Qt::CheckStateRole).toBool();
            QRect checkbox(newRect.x()+size.width()+1, newRect.y(), size.width(), size.height());
                if (checkbox.contains(static_cast<QMouseEvent*>(event)->pos()))
                {
                    bool value = Qt::Unchecked;
                    if (state == Qt::Unchecked)
                        value = Qt::Checked;
                    else
                        value = Qt::Unchecked;
                    return model->setData(index, value, Qt::CheckStateRole);
                }
        }
        else
        {
            const int textMargin = 3;
            QRect checkRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
                                option.decorationSize,
                                QRect(option.rect.x() + (2 * textMargin), option.rect.y(),
                                option.rect.width() - (2 * textMargin),
                                option.rect.height()));
            if (!checkRect.contains(static_cast<QMouseEvent*>(event)->pos()))
                return false;
        }

    }
    else if (event->type() == QEvent::KeyPress)
    {
        if (static_cast<QKeyEvent*>(event)->key() != Qt::Key_Space&& static_cast<QKeyEvent*>(event)->key() != Qt::Key_Select)
            return false;
    }
    else
    {
            return false;
    }
    return false;
}





LendDelegate::LendDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{

}

void LendDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    {
        QStyleOptionViewItemV4 viewItemOption(option);

        if (index.column() == 0)
        {
            const int textMargin = 3;
            QStyleOptionButton button;
            button.state = QStyle::State_Enabled;
            QSize size (20,20);
            button.iconSize = size;
            button.direction = option.direction;
            QRect newRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
                                                            QSize(62,20),
                                                            QRect(option.rect.x() + textMargin, option.rect.y(),
                                                                  option.rect.width() - (2 * textMargin), option.rect.height()));
            button.text = "-";
            button.rect = QRect(newRect.x(),newRect.y(), size.width(), size.height());
            QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
            int amount = index.data(Qt::CheckStateRole).toInt();
            if (amount>1)
                painter->drawText(QRect(newRect.x()+size.width()+1, newRect.y(), size.width(), size.height()), Qt::AlignLeft, QString::number(amount));
            else
            {
                QStyleOptionButton checkBox;
                checkBox.iconSize = size;
                checkBox.rect = QRect(newRect.x()+size.width()+1, newRect.y(), size.width(), size.height());
                checkBox.state = QStyle::State_Enabled | QStyle::State_Active;
                if (amount ==1)
                    checkBox.state |= QStyle::State_On;
                else
                    checkBox.state |= QStyle::State_Off;

                QApplication::style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &checkBox, painter);
            }
            button.text = "+";
            button.rect = QRect(newRect.x()+2*(size.width()+1), newRect.y(), size.width(), size.height());
            QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
        }
        if (index.column() > 0)
        {
            QStyledItemDelegate::paint(painter, viewItemOption, index);
        }
    }
}

bool LendDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    // make sure that the item is checkable
    Qt::ItemFlags flags = model->flags(index);
    if (!(flags & Qt::ItemIsUserCheckable) || !(flags & Qt::ItemIsEnabled))
        return false;

    // make sure that we have a check state
    QVariant value = index.data(Qt::CheckStateRole);
    if (!value.isValid())
        return false;

    // make sure that we have the right event type
    if (event->type() == QEvent::MouseButtonRelease)
    {
        if (index.column() == 0)
        {
            const int textMargin = 3;
            QSize size (20,20);
            QRect newRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
                             QSize(62,20),QRect(option.rect.x() + textMargin, option.rect.y(),
                             option.rect.width() - (2 * textMargin), option.rect.height()));
            QRect buttonMinus (newRect.x(),newRect.y(), size.width(), size.height());
            if (buttonMinus.contains(static_cast<QMouseEvent*>(event)->pos()))
                return model->setData(index, QVariant(-1), Qt::CheckStateRole);

            int amount = index.data(Qt::CheckStateRole).toInt();
            if (amount > 1)
                return false;
            else
            {
                QRect checkbox(newRect.x()+size.width()+1, newRect.y(), size.width(), size.height());
                if (checkbox.contains(static_cast<QMouseEvent*>(event)->pos()))
                {
                    int value = 0;
                    if (amount == 0)
                        value = 1;
                    else
                        value = -1;
                    return model->setData(index, value, Qt::CheckStateRole);
                }
            }
            QRect buttonPlus(newRect.x()+ 2*(size.width()+1),newRect.y(), size.width(), size.height());
            if (buttonPlus.contains(static_cast<QMouseEvent*>(event)->pos()))
                return model->setData(index, 1, Qt::CheckStateRole);
        }
        else
        {
            const int textMargin = 3;
            QRect checkRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
                                option.decorationSize,
                                QRect(option.rect.x() + (2 * textMargin), option.rect.y(),
                                option.rect.width() - (2 * textMargin),
                                option.rect.height()));
            if (!checkRect.contains(static_cast<QMouseEvent*>(event)->pos()))
                return false;
        }




    }
    else if (event->type() == QEvent::KeyPress)
    {
        if (static_cast<QKeyEvent*>(event)->key() != Qt::Key_Space&& static_cast<QKeyEvent*>(event)->key() != Qt::Key_Select)
            return false;
    }
    else
    {
            return false;
    }
    return false;
}
