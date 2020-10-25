#ifndef DELEGATE_H
#define DELEGATE_H

#include <QStyledItemDelegate>

class CheckBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    CheckBoxDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
                const QModelIndex &index) const;
    virtual bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
};

class LendDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    LendDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    virtual bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

};

#endif
