#include "coursesubjectcorrectiondialog.h"
#include "ui_coursesubjectcorrectiondialog.h"

//===========================================
// Implementations of ClassTable
//===========================================

EditableClassModel::EditableClassModel(ClassTable* ctable, QObject * parent) :
    QAbstractTableModel(parent), mClassTable(ctable)
{
    mRows = mClassTable->getAllClasses();
}

EditableClassModel::~EditableClassModel()
{
    mClassTable = nullptr;
}

int EditableClassModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mRows.length();
}

int EditableClassModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant EditableClassModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();
    switch(index.column())
    {
    case 0:
        return mRows.at(index.row()).className;
    case 1:
        return mRows.at(index.row()).grade;
    case 2:
        return mRows.at(index.row()).subject;
    default:
        return QVariant();
    }
}

QVariant EditableClassModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section > 2)
        return QVariant();
    if (orientation != Qt::Horizontal)
        return QVariant();
    if (role != Qt::DisplayRole)
        return QVariant();
    switch(section)
    {
    case 0:
        return "Klassename";
    case 1:
        return "Jahrgangsstufe";
    case 2:
        return "Fach (bearbeitbar)";
    default:
        return QVariant();
    }
}

bool EditableClassModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;
    if(role != Qt::EditRole)
        return false;
    if (index.column()!= 2)
        return false;

    ClassTable::ClassRow row = mRows.value(index.row());
    row.subject = value.toString();
    mRows.replace(index.row(), row);
    emit dataChanged(index, index);
    mClassTable->update(row.className, row.subject);
    return true;
}

Qt::ItemFlags EditableClassModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    if (index.column() != 2)
        return QAbstractItemModel::flags(index);

    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

void EditableClassModel::searchClass(QString classname)
{
    emit beginResetModel();
    mRows.clear();
    ClassTable::ClassRow * row = mClassTable->getClass(classname);
    if (row)
        mRows.append(*row);
    emit endResetModel();
}

void EditableClassModel::reset()
{
    emit beginResetModel();
    mRows.clear();
    emit endResetModel();
}

//==============================
// Implementations of CourseSubjectCorrectionDialog
//==============================

CourseSubjectCorrectionDialog::CourseSubjectCorrectionDialog(ClassTable *table, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CourseSubjectCorrectionDialog)
{
    ui->setupUi(this);
    mModel = new EditableClassModel(table, this);
    ui->tableView->setModel(mModel);
}

CourseSubjectCorrectionDialog::~CourseSubjectCorrectionDialog()
{
    delete ui;
}
