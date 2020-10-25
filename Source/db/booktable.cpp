#include "booktable.h"
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QVariant>

//====================
// Implementation of BookTable
//====================

//--------------------
// Constructor
//--------------------
BookTable::BookTable(QString tablename, QString isbncolumn, QString titlecolumn, QString subtitlecolumn,
                     QString publishercolumn, QString authorcolumn, QString subjectcolumn, QString gradecolumn,
                     QString editioncolumn, QString pricecolumn, QString stockcolumn, QString availablecolumn,
                     QString commentcolumn, QString stocktakingcountcolumn, QString stocktakingdatecolumn) noexcept:
    mTableName(tablename),
    mIsbnColumn(isbncolumn),
    mTitleColumn(titlecolumn),
    mSubTitleColumn(subtitlecolumn),
    mPublisherColumn(publishercolumn),
    mAuthorColumn(authorcolumn),
    mSubjectColumn(subjectcolumn),
    mGradeColumn(gradecolumn),
    mEditionColumn(editioncolumn),
    mPriceColumn(pricecolumn),
    mStockColumn(stockcolumn),
    mAvailableColumn(availablecolumn),
    mCommentColumn(commentcolumn),
    mStockTakingCountColumn(stocktakingcountcolumn),
    mStockTakingDateColumn(stocktakingdatecolumn)
{
}

//------------------
// Destructor
//------------------
BookTable::~BookTable() noexcept
{
}

//--------------------
// insert: adds a new book
// Parameter: isbn, title, subtitle, publisher, author, subject, grade, edition, price, stock, available, comment,
// stocktakingCount, stocktakingDate
//--------------------
bool BookTable::insert(QString isbn, QString title, QString subtitle, QString publisher, QString author,
                       QString subject, QList<int> grades, int edition, QString price, int stock, int available,
                       QString comment, int stocktakingCount, QString stocktakingDate) noexcept
{
    bool success = false;
    if(!QSqlDatabase::database().open())
    {
       return false;
    }

    QString dbGrades;
    for(int grade : grades)
    {
        dbGrades.append(QString::number(grade));
        dbGrades.append(",");
    }
    dbGrades.chop(1);
    QSqlQuery query;
    query.prepare("INSERT INTO " + mTableName + " (" + mIsbnColumn + ", " + mTitleColumn + ", " + mSubTitleColumn + ", " + mPublisherColumn
                  + ", " + mAuthorColumn + ", " + mSubjectColumn + ", " + mGradeColumn + ", " + mEditionColumn + ", " + mPriceColumn
                  + ", " + mStockColumn + ", " + mAvailableColumn + ", " + mCommentColumn + ", " + mStockTakingCountColumn + ", " +
                  mStockTakingDateColumn + ") VALUES ('" + isbn + "', '" + title  + "', '" + subtitle + "', '" + publisher + "', '"
                  + author + "', '" + subject + "', '" + dbGrades + "', '" + QString::number(edition) + "', '" + price +
                  "', '" + QString::number(stock) + "', '" + QString::number(available) + "', '" + comment + "', '" +
                  QString::number(stocktakingCount) + "', '" + stocktakingDate + "');");
    if(query.exec())
    {
       success = true;
    }

    QSqlDatabase::database().close();

    return success;
}

//--------------------
// insert: adds a new book
// Parameter: BookRow containing a book
//--------------------
bool BookTable::insert(BookRow book) noexcept
{
    return insert(book.isbn, book.title, book.subtitle, book.publisher, book.author, book.subject, book.grade, book.edition, book.price,
                  book.stock, book.available, book.comment, book.stocktakingCount, book.stocktakingDate);
}

//--------------------
// remove: deletes a book from the database
// Parameter: isbn
//--------------------
bool BookTable::remove(QString isbn) noexcept
{
    bool success = false;
    if(!QSqlDatabase::database().open())
    {
       return false;
    }

    QSqlQuery query;
    query.prepare("DELETE FROM " + mTableName + " WHERE " + mIsbnColumn + "='" + isbn + "';");
    if(query.exec())
    {
       success = true;
    }

    QSqlDatabase::database().close();
    return success;
}

//--------------------
// getBook: finds a book in the database and returns it as a BookRow
// Parameter: isbn
//--------------------
BookTable::BookRow* BookTable::getBook(QString isbn)
{
    if(!QSqlDatabase::database().open())
    {
       throw QString("Error in code: Method getBook of BookTable: Database could not be opened!");
    }
    BookTable::BookRow* row = new BookTable::BookRow();
    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mIsbnColumn + "='" + isbn + "';");

    int idIsbn = query.record().indexOf(mIsbnColumn);
    int idTitle = query.record().indexOf(mTitleColumn);
    int idSubTitle = query.record().indexOf(mSubTitleColumn);
    int idPublisher = query.record().indexOf(mPublisherColumn);
    int idAuthor = query.record().indexOf(mAuthorColumn);
    int idSubject = query.record().indexOf(mSubjectColumn);
    int idGrade = query.record().indexOf(mGradeColumn);
    int idEdition = query.record().indexOf(mEditionColumn);
    int idPrice = query.record().indexOf(mPriceColumn);
    int idStock = query.record().indexOf(mStockColumn);
    int idAvailable = query.record().indexOf(mAvailableColumn);
    int idComment = query.record().indexOf(mCommentColumn);
    int idStockCount = query.record().indexOf(mStockTakingCountColumn);
    int idStockDate = query.record().indexOf(mStockTakingDateColumn);

    while (query.next())
    {
       row->isbn = query.value(idIsbn).toString();
       row->title = query.value(idTitle).toString();
       row->subtitle = query.value(idSubTitle).toString();
       row->publisher = query.value(idPublisher).toString();
       row->author = query.value(idAuthor).toString();
       row->subject = query.value(idSubject).toString();
       QList<QString> grades = query.value(idGrade).toString().split(",");
       QList<int> rowgrades;
       for(QString grade : grades)
       {
            rowgrades.append(grade.toInt());
       }
       row->grade = rowgrades;
       row->edition = query.value(idEdition).toInt();
       row->price = query.value(idPrice).toString();
       row->stock = query.value(idStock).toInt();
       row->available = query.value(idAvailable).toInt();
       row->comment = query.value(idComment).toString();
       row->stocktakingCount = query.value(idStockCount).toInt();
       row->stocktakingDate = query.value(idStockDate).toString();
    }

    QSqlDatabase::database().close();
    return row;
}

//--------------------
// getAllBooks: returns all books in the table
//--------------------
QList<BookTable::BookRow> BookTable::getAllBooks(BookTableOrder order)
{
    if(!QSqlDatabase::database().open())
    {
       throw QString("Error in code: Method getAllBooks of BookTable: Database could not be opened!");
    }
    QList<BookTable::BookRow> rows;
    QString sort = " ORDER BY ";
    switch(order)
    {
    case BookTable::BookTitleAscending:
        sort += mTitleColumn + " ASC";
        break;
    case BookTable::BookTitleDescanding:
        sort += mTitleColumn + " DESC";
        break;
    }
    QSqlQuery query("SELECT * FROM " + mTableName + sort + ";");

    int idIsbn = query.record().indexOf(mIsbnColumn);
    int idTitle = query.record().indexOf(mTitleColumn);
    int idSubTitle = query.record().indexOf(mSubTitleColumn);
    int idPublisher = query.record().indexOf(mPublisherColumn);
    int idAuthor = query.record().indexOf(mAuthorColumn);
    int idSubject = query.record().indexOf(mSubjectColumn);
    int idGrade = query.record().indexOf(mGradeColumn);
    int idEdition = query.record().indexOf(mEditionColumn);
    int idPrice = query.record().indexOf(mPriceColumn);
    int idStock = query.record().indexOf(mStockColumn);
    int idAvailable = query.record().indexOf(mAvailableColumn);
    int idComment = query.record().indexOf(mCommentColumn);
    int idStockCount = query.record().indexOf(mStockTakingCountColumn);
    int idStockDate = query.record().indexOf(mStockTakingDateColumn);

    while (query.next())
    {
       BookRow row;
       row.isbn = query.value(idIsbn).toString();
       row.title = query.value(idTitle).toString();
       row.subtitle = query.value(idSubTitle).toString();
       row.publisher = query.value(idPublisher).toString();
       row.author = query.value(idAuthor).toString();
       row.subject = query.value(idSubject).toString();
       QList<QString> grades = query.value(idGrade).toString().split(",");
       QList<int> rowgrades;
       for(QString grade : grades)
       {
            rowgrades.append(grade.toInt());
       }
       row.grade = rowgrades;
       row.edition = query.value(idEdition).toInt();
       row.price = query.value(idPrice).toString();
       row.stock = query.value(idStock).toInt();
       row.available = query.value(idAvailable).toInt();
       row.comment = query.value(idComment).toString();
       row.stocktakingCount = query.value(idStockCount).toInt();
       row.stocktakingDate = query.value(idStockDate).toString();

       rows.append(row);
    }

    QSqlDatabase::database().close();
    return rows;
}

//--------------------
// searchBookByTitle: finds all books with the given title pattern and returns them as a list
// Parameter: title
//--------------------
QList<BookTable::BookRow> BookTable::searchBookByTitle(QString title)
{
    if(!QSqlDatabase::database().open())
    {
       throw QString("Error in code: Method searchBookByTitle of BookTable: Database could not be opened!");
    }

    QSqlQuery pragmaQuery("PRAGMA case_sensitive_like = true;");
    pragmaQuery.exec();
    pragmaQuery.finish();

    QList<BookTable::BookRow> rows;
    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mTitleColumn + " LIKE '%" + title + "%';");

    int idIsbn = query.record().indexOf(mIsbnColumn);
    int idTitle = query.record().indexOf(mTitleColumn);
    int idSubTitle = query.record().indexOf(mSubTitleColumn);
    int idPublisher = query.record().indexOf(mPublisherColumn);
    int idAuthor = query.record().indexOf(mAuthorColumn);
    int idSubject = query.record().indexOf(mSubjectColumn);
    int idGrade = query.record().indexOf(mGradeColumn);
    int idEdition = query.record().indexOf(mEditionColumn);
    int idPrice = query.record().indexOf(mPriceColumn);
    int idStock = query.record().indexOf(mStockColumn);
    int idAvailable = query.record().indexOf(mAvailableColumn);
    int idComment = query.record().indexOf(mCommentColumn);
    int idStockCount = query.record().indexOf(mStockTakingCountColumn);
    int idStockDate = query.record().indexOf(mStockTakingDateColumn);

    while (query.next())
    {
       BookRow row;
       row.isbn = query.value(idIsbn).toString();
       row.title = query.value(idTitle).toString();
       row.subtitle = query.value(idSubTitle).toString();
       row.publisher = query.value(idPublisher).toString();
       row.author = query.value(idAuthor).toString();
       row.subject = query.value(idSubject).toString();
       QList<QString> grades = query.value(idGrade).toString().split(",");
       QList<int> rowgrades;
       for(QString grade : grades)
       {
            rowgrades.append(grade.toInt());
       }
       row.grade = rowgrades;
       row.edition = query.value(idEdition).toInt();
       row.price = query.value(idPrice).toString();
       row.stock = query.value(idStock).toInt();
       row.available = query.value(idAvailable).toInt();
       row.comment = query.value(idComment).toString();
       row.stocktakingCount = query.value(idStockCount).toInt();
       row.stocktakingDate = query.value(idStockDate).toString();

       rows.append(row);
    }

    QSqlDatabase::database().close();
    return rows;
}

//--------------------
// searchBookBySubTitle: finds all books with the given subtitle pattern and returns them as a list
// Parameter: subtitle
//--------------------
QList<BookTable::BookRow> BookTable::searchBookBySubTitle(QString subtitle)
{
    if(!QSqlDatabase::database().open())
    {
       throw QString("Error in code: Method searchBookBySubTitle of BookTable: Database could not be opened!");
    }

    QSqlQuery pragmaQuery("PRAGMA case_sensitive_like = true;");
    pragmaQuery.exec();
    pragmaQuery.finish();

    QList<BookTable::BookRow> rows;
    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mSubTitleColumn + " LIKE '%" + subtitle + "%';");

    int idIsbn = query.record().indexOf(mIsbnColumn);
    int idTitle = query.record().indexOf(mTitleColumn);
    int idSubTitle = query.record().indexOf(mSubTitleColumn);
    int idPublisher = query.record().indexOf(mPublisherColumn);
    int idAuthor = query.record().indexOf(mAuthorColumn);
    int idSubject = query.record().indexOf(mSubjectColumn);
    int idGrade = query.record().indexOf(mGradeColumn);
    int idEdition = query.record().indexOf(mEditionColumn);
    int idPrice = query.record().indexOf(mPriceColumn);
    int idStock = query.record().indexOf(mStockColumn);
    int idAvailable = query.record().indexOf(mAvailableColumn);
    int idComment = query.record().indexOf(mCommentColumn);
    int idStockCount = query.record().indexOf(mStockTakingCountColumn);
    int idStockDate = query.record().indexOf(mStockTakingDateColumn);

    while (query.next())
    {
       BookRow row;
       row.isbn = query.value(idIsbn).toString();
       row.title = query.value(idTitle).toString();
       row.subtitle = query.value(idSubTitle).toString();
       row.publisher = query.value(idPublisher).toString();
       row.author = query.value(idAuthor).toString();
       row.subject = query.value(idSubject).toString();
       QList<QString> grades = query.value(idGrade).toString().split(",");
       QList<int> rowgrades;
       for(QString grade : grades)
       {
            rowgrades.append(grade.toInt());
       }
       row.grade = rowgrades;
       row.edition = query.value(idEdition).toInt();
       row.price = query.value(idPrice).toString();
       row.stock = query.value(idStock).toInt();
       row.available = query.value(idAvailable).toInt();
       row.comment = query.value(idComment).toString();
       row.stocktakingCount = query.value(idStockCount).toInt();
       row.stocktakingDate = query.value(idStockDate).toString();

       rows.append(row);
    }

    QSqlDatabase::database().close();
    return rows;
}

//--------------------
// searchBookByPublisher: finds all books with the given publisher pattern and returns them as a list
// Parameter: publisher
//--------------------
QList<BookTable::BookRow> BookTable::searchBookByPublisher(QString publisher)
{
    if(!QSqlDatabase::database().open())
    {
       throw QString("Error in code: Method searchBookByPublisher of BookTable: Database could not be opened!");
    }

    QSqlQuery pragmaQuery("PRAGMA case_sensitive_like = true;");
    pragmaQuery.exec();
    pragmaQuery.finish();

    QList<BookTable::BookRow> rows;
    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mPublisherColumn + " LIKE '%" + publisher + "%';");

    int idIsbn = query.record().indexOf(mIsbnColumn);
    int idTitle = query.record().indexOf(mTitleColumn);
    int idSubTitle = query.record().indexOf(mSubTitleColumn);
    int idPublisher = query.record().indexOf(mPublisherColumn);
    int idAuthor = query.record().indexOf(mAuthorColumn);
    int idSubject = query.record().indexOf(mSubjectColumn);
    int idGrade = query.record().indexOf(mGradeColumn);
    int idEdition = query.record().indexOf(mEditionColumn);
    int idPrice = query.record().indexOf(mPriceColumn);
    int idStock = query.record().indexOf(mStockColumn);
    int idAvailable = query.record().indexOf(mAvailableColumn);
    int idComment = query.record().indexOf(mCommentColumn);
    int idStockCount = query.record().indexOf(mStockTakingCountColumn);
    int idStockDate = query.record().indexOf(mStockTakingDateColumn);

    while (query.next())
    {
       BookRow row;
       row.isbn = query.value(idIsbn).toString();
       row.title = query.value(idTitle).toString();
       row.subtitle = query.value(idSubTitle).toString();
       row.publisher = query.value(idPublisher).toString();
       row.author = query.value(idAuthor).toString();
       row.subject = query.value(idSubject).toString();
       QList<QString> grades = query.value(idGrade).toString().split(",");
       QList<int> rowgrades;
       for(QString grade : grades)
       {
            rowgrades.append(grade.toInt());
       }
       row.grade = rowgrades;
       row.edition = query.value(idEdition).toInt();
       row.price = query.value(idPrice).toString();
       row.stock = query.value(idStock).toInt();
       row.available = query.value(idAvailable).toInt();
       row.comment = query.value(idComment).toString();
       row.stocktakingCount = query.value(idStockCount).toInt();
       row.stocktakingDate = query.value(idStockDate).toString();

       rows.append(row);
    }

    QSqlDatabase::database().close();
    return rows;
}

//--------------------
// searchBookByAuthor: finds all books with the given author pattern and returns them as a list
// Parameter: author
//--------------------
QList<BookTable::BookRow> BookTable::searchBookByAuthor(QString author)
{
    if(!QSqlDatabase::database().open())
    {
       throw QString("Error in code: Method searchBookByAuthor of BookTable: Database could not be opened!");
    }

    QSqlQuery pragmaQuery("PRAGMA case_sensitive_like = true;");
    pragmaQuery.exec();
    pragmaQuery.finish();

    QList<BookTable::BookRow> rows;
    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mAuthorColumn + " LIKE '%" + author + "%';");

    int idIsbn = query.record().indexOf(mIsbnColumn);
    int idTitle = query.record().indexOf(mTitleColumn);
    int idSubTitle = query.record().indexOf(mSubTitleColumn);
    int idPublisher = query.record().indexOf(mPublisherColumn);
    int idAuthor = query.record().indexOf(mAuthorColumn);
    int idSubject = query.record().indexOf(mSubjectColumn);
    int idGrade = query.record().indexOf(mGradeColumn);
    int idEdition = query.record().indexOf(mEditionColumn);
    int idPrice = query.record().indexOf(mPriceColumn);
    int idStock = query.record().indexOf(mStockColumn);
    int idAvailable = query.record().indexOf(mAvailableColumn);
    int idComment = query.record().indexOf(mCommentColumn);
    int idStockCount = query.record().indexOf(mStockTakingCountColumn);
    int idStockDate = query.record().indexOf(mStockTakingDateColumn);

    while (query.next())
    {
       BookRow row;
       row.isbn = query.value(idIsbn).toString();
       row.title = query.value(idTitle).toString();
       row.subtitle = query.value(idSubTitle).toString();
       row.publisher = query.value(idPublisher).toString();
       row.author = query.value(idAuthor).toString();
       row.subject = query.value(idSubject).toString();
       QList<QString> grades = query.value(idGrade).toString().split(",");
       QList<int> rowgrades;
       for(QString grade : grades)
       {
            rowgrades.append(grade.toInt());
       }
       row.grade = rowgrades;
       row.edition = query.value(idEdition).toInt();
       row.price = query.value(idPrice).toString();
       row.stock = query.value(idStock).toInt();
       row.available = query.value(idAvailable).toInt();
       row.comment = query.value(idComment).toString();
       row.stocktakingCount = query.value(idStockCount).toInt();
       row.stocktakingDate = query.value(idStockDate).toString();

       rows.append(row);
    }

    QSqlDatabase::database().close();
    return rows;
}

//--------------------
// searchBooksBySubject: finds all books with the given subject and returns them as a list
// Parameter: subject
//--------------------
QList<BookTable::BookRow> BookTable::searchBooksBySubject(QString subject)
{
    if(!QSqlDatabase::database().open())
    {
       throw QString("Error in code: Method searchBooksBySubject of BookTable: Database could not be opened!");
    }

    QSqlQuery pragmaQuery("PRAGMA case_sensitive_like = true;");
    pragmaQuery.exec();
    pragmaQuery.finish();

    QList<BookTable::BookRow> rows;
    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mSubjectColumn + " LIKE '%" + subject + "%';");

    int idIsbn = query.record().indexOf(mIsbnColumn);
    int idTitle = query.record().indexOf(mTitleColumn);
    int idSubTitle = query.record().indexOf(mSubTitleColumn);
    int idPublisher = query.record().indexOf(mPublisherColumn);
    int idAuthor = query.record().indexOf(mAuthorColumn);
    int idSubject = query.record().indexOf(mSubjectColumn);
    int idGrade = query.record().indexOf(mGradeColumn);
    int idEdition = query.record().indexOf(mEditionColumn);
    int idPrice = query.record().indexOf(mPriceColumn);
    int idStock = query.record().indexOf(mStockColumn);
    int idAvailable = query.record().indexOf(mAvailableColumn);
    int idComment = query.record().indexOf(mCommentColumn);
    int idStockCount = query.record().indexOf(mStockTakingCountColumn);
    int idStockDate = query.record().indexOf(mStockTakingDateColumn);

    while (query.next())
    {
       BookRow row;
       row.isbn = query.value(idIsbn).toString();
       row.title = query.value(idTitle).toString();
       row.subtitle = query.value(idSubTitle).toString();
       row.publisher = query.value(idPublisher).toString();
       row.author = query.value(idAuthor).toString();
       row.subject = query.value(idSubject).toString();
       QList<QString> grades = query.value(idGrade).toString().split(",");
       QList<int> rowgrades;
       for(QString grade : grades)
       {
            rowgrades.append(grade.toInt());
       }
       row.grade = rowgrades;
       row.edition = query.value(idEdition).toInt();
       row.price = query.value(idPrice).toString();
       row.stock = query.value(idStock).toInt();
       row.available = query.value(idAvailable).toInt();
       row.comment = query.value(idComment).toString();
       row.stocktakingCount = query.value(idStockCount).toInt();
       row.stocktakingDate = query.value(idStockDate).toString();

       rows.append(row);
    }

    QSqlDatabase::database().close();
    return rows;
}

//--------------------
// searchBooksByComment: finds all books with the given comment pattern and returns them as a list
// Parameter: comment
//--------------------
QList<BookTable::BookRow> BookTable::searchBooksByComment(QString comment)
{
    if(!QSqlDatabase::database().open())
    {
       throw QString("Error in code: Method searchBooksByComment of BookTable: Database could not be opened!");
    }

    QSqlQuery pragmaQuery("PRAGMA case_sensitive_like = true;");
    pragmaQuery.exec();
    pragmaQuery.finish();

    QList<BookTable::BookRow> rows;
    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mCommentColumn + " LIKE '%" + comment + "%';");

    int idIsbn = query.record().indexOf(mIsbnColumn);
    int idTitle = query.record().indexOf(mTitleColumn);
    int idSubTitle = query.record().indexOf(mSubTitleColumn);
    int idPublisher = query.record().indexOf(mPublisherColumn);
    int idAuthor = query.record().indexOf(mAuthorColumn);
    int idSubject = query.record().indexOf(mSubjectColumn);
    int idGrade = query.record().indexOf(mGradeColumn);
    int idEdition = query.record().indexOf(mEditionColumn);
    int idPrice = query.record().indexOf(mPriceColumn);
    int idStock = query.record().indexOf(mStockColumn);
    int idAvailable = query.record().indexOf(mAvailableColumn);
    int idComment = query.record().indexOf(mCommentColumn);
    int idStockCount = query.record().indexOf(mStockTakingCountColumn);
    int idStockDate = query.record().indexOf(mStockTakingDateColumn);

    while (query.next())
    {
       BookRow row;
       row.isbn = query.value(idIsbn).toString();
       row.title = query.value(idTitle).toString();
       row.subtitle = query.value(idSubTitle).toString();
       row.publisher = query.value(idPublisher).toString();
       row.author = query.value(idAuthor).toString();
       row.subject = query.value(idSubject).toString();
       QList<QString> grades = query.value(idGrade).toString().split(",");
       QList<int> rowgrades;
       for(QString grade : grades)
       {
            rowgrades.append(grade.toInt());
       }
       row.grade = rowgrades;
       row.edition = query.value(idEdition).toInt();
       row.price = query.value(idPrice).toString();
       row.stock = query.value(idStock).toInt();
       row.available = query.value(idAvailable).toInt();
       row.comment = query.value(idComment).toString();
       row.stocktakingCount = query.value(idStockCount).toInt();
       row.stocktakingDate = query.value(idStockDate).toString();

       rows.append(row);
    }

    QSqlDatabase::database().close();
    return rows;
}

//--------------------
// getBooksByGrade: finds all books with the given grade and returns them as a list
// Parameter: grade
//--------------------
QList<BookTable::BookRow> BookTable::getBooksByGrade(int grade)
{
    if(!QSqlDatabase::database().open())
    {
       throw QString("Error in code: Method getBooksByGrade of BookTable: Database could not be opened!");
    }
    QList<BookTable::BookRow> rows;
    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mGradeColumn + " LIKE '%" + QString::number(grade) + "%';");

    int idIsbn = query.record().indexOf(mIsbnColumn);
    int idTitle = query.record().indexOf(mTitleColumn);
    int idSubTitle = query.record().indexOf(mSubTitleColumn);
    int idPublisher = query.record().indexOf(mPublisherColumn);
    int idAuthor = query.record().indexOf(mAuthorColumn);
    int idSubject = query.record().indexOf(mSubjectColumn);
    int idGrade = query.record().indexOf(mGradeColumn);
    int idEdition = query.record().indexOf(mEditionColumn);
    int idPrice = query.record().indexOf(mPriceColumn);
    int idStock = query.record().indexOf(mStockColumn);
    int idAvailable = query.record().indexOf(mAvailableColumn);
    int idComment = query.record().indexOf(mCommentColumn);
    int idStockCount = query.record().indexOf(mStockTakingCountColumn);
    int idStockDate = query.record().indexOf(mStockTakingDateColumn);

    while (query.next())
    {
       BookRow row;
       row.isbn = query.value(idIsbn).toString();
       row.title = query.value(idTitle).toString();
       row.subtitle = query.value(idSubTitle).toString();
       row.publisher = query.value(idPublisher).toString();
       row.author = query.value(idAuthor).toString();
       row.subject = query.value(idSubject).toString();
       QList<QString> grades = query.value(idGrade).toString().split(",");
       QList<int> rowgrades;
       for(QString grade : grades)
       {
            rowgrades.append(grade.toInt());
       }
       row.grade = rowgrades;
       row.edition = query.value(idEdition).toInt();
       row.price = query.value(idPrice).toString();
       row.stock = query.value(idStock).toInt();
       row.available = query.value(idAvailable).toInt();
       row.comment = query.value(idComment).toString();
       row.stocktakingCount = query.value(idStockCount).toInt();
       row.stocktakingDate = query.value(idStockDate).toString();

       rows.append(row);
    }

    QSqlDatabase::database().close();
    return rows;
}



//--------------------
// getBooksByEdition: gets all books with the given edition and returns them as a list
// Parameter: edition
//--------------------
QList<BookTable::BookRow> BookTable::getBooksByEdition(int edition)
{
    if(!QSqlDatabase::database().open())
    {
       throw QString("Error in code: Method getBooksByEdition of BookTable: Database could not be opened!");
    }
    QList<BookTable::BookRow> rows;
    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mEditionColumn + "='" + QString::number(edition) + "';");

    int idIsbn = query.record().indexOf(mIsbnColumn);
    int idTitle = query.record().indexOf(mTitleColumn);
    int idSubTitle = query.record().indexOf(mSubTitleColumn);
    int idPublisher = query.record().indexOf(mPublisherColumn);
    int idAuthor = query.record().indexOf(mAuthorColumn);
    int idSubject = query.record().indexOf(mSubjectColumn);
    int idGrade = query.record().indexOf(mGradeColumn);
    int idEdition = query.record().indexOf(mEditionColumn);
    int idPrice = query.record().indexOf(mPriceColumn);
    int idStock = query.record().indexOf(mStockColumn);
    int idAvailable = query.record().indexOf(mAvailableColumn);
    int idComment = query.record().indexOf(mCommentColumn);
    int idStockCount = query.record().indexOf(mStockTakingCountColumn);
    int idStockDate = query.record().indexOf(mStockTakingDateColumn);

    while (query.next())
    {
       BookRow row;
       row.isbn = query.value(idIsbn).toString();
       row.title = query.value(idTitle).toString();
       row.subtitle = query.value(idSubTitle).toString();
       row.publisher = query.value(idPublisher).toString();
       row.author = query.value(idAuthor).toString();
       row.subject = query.value(idSubject).toString();
       QList<QString> grades = query.value(idGrade).toString().split(",");
       QList<int> rowgrades;
       for(QString grade : grades)
       {
            rowgrades.append(grade.toInt());
       }
       row.grade = rowgrades;
       row.edition = query.value(idEdition).toInt();
       row.price = query.value(idPrice).toString();
       row.stock = query.value(idStock).toInt();
       row.available = query.value(idAvailable).toInt();
       row.comment = query.value(idComment).toString();
       row.stocktakingCount = query.value(idStockCount).toInt();
       row.stocktakingDate = query.value(idStockDate).toString();

       rows.append(row);
    }

    QSqlDatabase::database().close();
    return rows;
}

//--------------------
// getBooksByStock: finds all books with the given stock and returns them as a list
// Parameter: stock
//--------------------
QList<BookTable::BookRow> BookTable::getBooksByStock(int stock)
{
    if(!QSqlDatabase::database().open())
    {
       throw QString("Error in code: Method getBooksByStock of BookTable: Database could not be opened!");
    }
    QList<BookTable::BookRow> rows;
    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mStockColumn + "='" + QString::number(stock) + "';");

    int idIsbn = query.record().indexOf(mIsbnColumn);
    int idTitle = query.record().indexOf(mTitleColumn);
    int idSubTitle = query.record().indexOf(mSubTitleColumn);
    int idPublisher = query.record().indexOf(mPublisherColumn);
    int idAuthor = query.record().indexOf(mAuthorColumn);
    int idSubject = query.record().indexOf(mSubjectColumn);
    int idGrade = query.record().indexOf(mGradeColumn);
    int idEdition = query.record().indexOf(mEditionColumn);
    int idPrice = query.record().indexOf(mPriceColumn);
    int idStock = query.record().indexOf(mStockColumn);
    int idAvailable = query.record().indexOf(mAvailableColumn);
    int idComment = query.record().indexOf(mCommentColumn);
    int idStockCount = query.record().indexOf(mStockTakingCountColumn);
    int idStockDate = query.record().indexOf(mStockTakingDateColumn);

    while (query.next())
    {
       BookRow row;
       row.isbn = query.value(idIsbn).toString();
       row.title = query.value(idTitle).toString();
       row.subtitle = query.value(idSubTitle).toString();
       row.publisher = query.value(idPublisher).toString();
       row.author = query.value(idAuthor).toString();
       row.subject = query.value(idSubject).toString();
       QList<QString> grades = query.value(idGrade).toString().split(",");
       QList<int> rowgrades;
       for(QString grade : grades)
       {
            rowgrades.append(grade.toInt());
       }
       row.grade = rowgrades;
       row.edition = query.value(idEdition).toInt();
       row.price = query.value(idPrice).toString();
       row.stock = query.value(idStock).toInt();
       row.available = query.value(idAvailable).toInt();
       row.comment = query.value(idComment).toString();
       row.stocktakingCount = query.value(idStockCount).toInt();
       row.stocktakingDate = query.value(idStockDate).toString();

       rows.append(row);
    }

    QSqlDatabase::database().close();
    return rows;
}

//--------------------
// getBooksByAvailable: finds all books with the given available-number and returns them as a list
// Parameter: available
//--------------------
QList<BookTable::BookRow> BookTable::getBooksByAvailable(int available)
{
    if(!QSqlDatabase::database().open())
    {
       throw QString("Error in code: Method getBooksByAvailable of BookTable: Database could not be opened!");
    }
    QList<BookTable::BookRow> rows;
    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mAvailableColumn + "='" + QString::number(available) + "';");

    int idIsbn = query.record().indexOf(mIsbnColumn);
    int idTitle = query.record().indexOf(mTitleColumn);
    int idSubTitle = query.record().indexOf(mSubTitleColumn);
    int idPublisher = query.record().indexOf(mPublisherColumn);
    int idAuthor = query.record().indexOf(mAuthorColumn);
    int idSubject = query.record().indexOf(mSubjectColumn);
    int idGrade = query.record().indexOf(mGradeColumn);
    int idEdition = query.record().indexOf(mEditionColumn);
    int idPrice = query.record().indexOf(mPriceColumn);
    int idStock = query.record().indexOf(mStockColumn);
    int idAvailable = query.record().indexOf(mAvailableColumn);
    int idComment = query.record().indexOf(mCommentColumn);
    int idStockCount = query.record().indexOf(mStockTakingCountColumn);
    int idStockDate = query.record().indexOf(mStockTakingDateColumn);

    while (query.next())
    {
       BookRow row;
       row.isbn = query.value(idIsbn).toString();
       row.title = query.value(idTitle).toString();
       row.subtitle = query.value(idSubTitle).toString();
       row.publisher = query.value(idPublisher).toString();
       row.author = query.value(idAuthor).toString();
       row.subject = query.value(idSubject).toString();
       QList<QString> grades = query.value(idGrade).toString().split(",");
       QList<int> rowgrades;
       for(QString grade : grades)
       {
            rowgrades.append(grade.toInt());
       }
       row.grade = rowgrades;
       row.edition = query.value(idEdition).toInt();
       row.price = query.value(idPrice).toString();
       row.stock = query.value(idStock).toInt();
       row.available = query.value(idAvailable).toInt();
       row.comment = query.value(idComment).toString();
       row.stocktakingCount = query.value(idStockCount).toInt();
       row.stocktakingDate = query.value(idStockDate).toString();

       rows.append(row);
    }

    QSqlDatabase::database().close();
    return rows;
}

//--------------------
// getBooksByStocktakingCount: finds all books with the given stocktakingCount and returns them as a list
// Parameter: stocktakingCount
//--------------------
QList<BookTable::BookRow> BookTable::getBooksByStocktakingCount(int stocktakingCount)
{
    if(!QSqlDatabase::database().open())
    {
       throw QString("Error in code: Method getBooksByStocktakingCount of BookTable: Database could not be opened!");
    }
    QList<BookTable::BookRow> rows;
    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mStockTakingCountColumn + "='" + QString::number(stocktakingCount) + "';");

    int idIsbn = query.record().indexOf(mIsbnColumn);
    int idTitle = query.record().indexOf(mTitleColumn);
    int idSubTitle = query.record().indexOf(mSubTitleColumn);
    int idPublisher = query.record().indexOf(mPublisherColumn);
    int idAuthor = query.record().indexOf(mAuthorColumn);
    int idSubject = query.record().indexOf(mSubjectColumn);
    int idGrade = query.record().indexOf(mGradeColumn);
    int idEdition = query.record().indexOf(mEditionColumn);
    int idPrice = query.record().indexOf(mPriceColumn);
    int idStock = query.record().indexOf(mStockColumn);
    int idAvailable = query.record().indexOf(mAvailableColumn);
    int idComment = query.record().indexOf(mCommentColumn);
    int idStockCount = query.record().indexOf(mStockTakingCountColumn);
    int idStockDate = query.record().indexOf(mStockTakingDateColumn);

    while (query.next())
    {
       BookRow row;
       row.isbn = query.value(idIsbn).toString();
       row.title = query.value(idTitle).toString();
       row.subtitle = query.value(idSubTitle).toString();
       row.publisher = query.value(idPublisher).toString();
       row.author = query.value(idAuthor).toString();
       row.subject = query.value(idSubject).toString();
       QList<QString> grades = query.value(idGrade).toString().split(",");
       QList<int> rowgrades;
       for(QString grade : grades)
       {
            rowgrades.append(grade.toInt());
       }
       row.grade = rowgrades;
       row.edition = query.value(idEdition).toInt();
       row.price = query.value(idPrice).toString();
       row.stock = query.value(idStock).toInt();
       row.available = query.value(idAvailable).toInt();
       row.comment = query.value(idComment).toString();
       row.stocktakingCount = query.value(idStockCount).toInt();
       row.stocktakingDate = query.value(idStockDate).toString();

       rows.append(row);
    }

    QSqlDatabase::database().close();
    return rows;
}

//--------------------
// getBooksByStocktakingDate: finds all books with the given stocktakingDate and returns them as a list
// Parameter: stocktakingDate
//--------------------
QList<BookTable::BookRow> BookTable::getBooksByStocktakingDate(QString stocktakingDate)
{
    if(!QSqlDatabase::database().open())
    {
       throw QString("Error in code: Method getBooksByStocktakingDate of BookTable: Database could not be opened!");
    }
    QList<BookTable::BookRow> rows;
    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mStockTakingDateColumn + "='" + stocktakingDate + "';");

    int idIsbn = query.record().indexOf(mIsbnColumn);
    int idTitle = query.record().indexOf(mTitleColumn);
    int idSubTitle = query.record().indexOf(mSubTitleColumn);
    int idPublisher = query.record().indexOf(mPublisherColumn);
    int idAuthor = query.record().indexOf(mAuthorColumn);
    int idSubject = query.record().indexOf(mSubjectColumn);
    int idGrade = query.record().indexOf(mGradeColumn);
    int idEdition = query.record().indexOf(mEditionColumn);
    int idPrice = query.record().indexOf(mPriceColumn);
    int idStock = query.record().indexOf(mStockColumn);
    int idAvailable = query.record().indexOf(mAvailableColumn);
    int idComment = query.record().indexOf(mCommentColumn);
    int idStockCount = query.record().indexOf(mStockTakingCountColumn);
    int idStockDate = query.record().indexOf(mStockTakingDateColumn);

    while (query.next())
    {
       BookRow row;
       row.isbn = query.value(idIsbn).toString();
       row.title = query.value(idTitle).toString();
       row.subtitle = query.value(idSubTitle).toString();
       row.publisher = query.value(idPublisher).toString();
       row.author = query.value(idAuthor).toString();
       row.subject = query.value(idSubject).toString();
       QList<QString> grades = query.value(idGrade).toString().split(",");
       QList<int> rowgrades;
       for(QString grade : grades)
       {
            rowgrades.append(grade.toInt());
       }
       row.grade = rowgrades;
       row.edition = query.value(idEdition).toInt();
       row.price = query.value(idPrice).toString();
       row.stock = query.value(idStock).toInt();
       row.available = query.value(idAvailable).toInt();
       row.comment = query.value(idComment).toString();
       row.stocktakingCount = query.value(idStockCount).toInt();
       row.stocktakingDate = query.value(idStockDate).toString();

       rows.append(row);
    }

    QSqlDatabase::database().close();
    return rows;
}

//--------------------
// updateTitle: writes a new title for the given book in the database
// Parameter: isbn, title
//--------------------
bool BookTable::updateTitle(QString isbn, QString title) noexcept
{
    bool success = false;
    if(!QSqlDatabase::database().open())
    {
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE " + mTableName + " SET " + mTitleColumn + "='" + title + "' WHERE " + mIsbnColumn +
                  "='" + isbn + "';");

    if(query.exec())
    {
        success = true;
    }

    QSqlDatabase::database().close();
    return success;
}

//--------------------
// updateSubTitle: writes a new subtitle for the given book in the database
// Parameter: isbn, subtitle
//--------------------
bool BookTable::updateSubTitle(QString isbn, QString subtitle) noexcept
{
    bool success = false;
    if(!QSqlDatabase::database().open())
    {
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE " + mTableName + " SET " + mSubTitleColumn + "='" + subtitle + "' WHERE " + mIsbnColumn +
                  "='" + isbn + "';");

    if(query.exec())
    {
        success = true;
    }

    QSqlDatabase::database().close();
    return success;
}

//--------------------
// updatePublisher: writes a new publisher for the given book in the database
// Parameter: isbn, publisher
//--------------------
bool BookTable::updatePublisher(QString isbn, QString publisher) noexcept
{
    bool success = false;
    if(!QSqlDatabase::database().open())
    {
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE " + mTableName + " SET " + mPublisherColumn + "='" + publisher + "' WHERE " + mIsbnColumn +
                  "='" + isbn + "';");

    if(query.exec())
    {
        success = true;
    }

    QSqlDatabase::database().close();
    return success;
}

//--------------------
// updateAuthor: writes a new author for the given book in the database
// Parameter: isbn, author
//--------------------
bool BookTable::updateAuthor(QString isbn, QString author) noexcept
{
    bool success = false;
    if(!QSqlDatabase::database().open())
    {
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE " + mTableName + " SET " + mAuthorColumn + "='" + author + "' WHERE " + mIsbnColumn +
                  "='" + isbn + "';");

    if(query.exec())
    {
        success = true;
    }

    QSqlDatabase::database().close();
    return success;
}

//--------------------
// updateSubject: writes a new subject for the given book in the database
// Parameter: isbn, subject
//--------------------
bool BookTable::updateSubject(QString isbn, QString subject) noexcept
{
    bool success = false;
    if(!QSqlDatabase::database().open())
    {
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE " + mTableName + " SET " + mSubjectColumn + "='" + subject + "' WHERE " + mIsbnColumn +
                  "='" + isbn + "';");

    if(query.exec())
    {
        success = true;
    }

    QSqlDatabase::database().close();
    return success;
}

//--------------------
// updateGrade: writes a new grade for the given book in the database
// Parameter: isbn, grade
//--------------------
bool BookTable::updateGrade(QString isbn, QList<int> grades) noexcept
{
    bool success = false;
    if(!QSqlDatabase::database().open())
    {
        return false;
    }

    QString dbGrades;
    for(int grade : grades)
    {
        dbGrades.append(QString::number(grade));
        dbGrades.append(",");
    }
    dbGrades.chop(1);
    QSqlQuery query;
    query.prepare("UPDATE " + mTableName + " SET " + mGradeColumn + "='" + dbGrades + "' WHERE " + mIsbnColumn +
                  "='" + isbn + "';");

    if(query.exec())
    {
        success = true;
    }

    QSqlDatabase::database().close();
    return success;
}

//--------------------
// updateEdition: writes a new edition for the given book in the database
// Parameter: isbn, edition
//--------------------
bool BookTable::updateEdition(QString isbn, int edition) noexcept
{
    bool success = false;
    if(!QSqlDatabase::database().open())
    {
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE " + mTableName + " SET " + mEditionColumn + " = '" + QString::number(edition) + "' WHERE " + mIsbnColumn +
                  "='" + isbn + "';");

    if(query.exec())
    {
        success = true;
    }

    QSqlDatabase::database().close();
    return success;
}

//--------------------
// updatePrice: writes a new price for the given book in the database
// Parameter: isbn, price
//--------------------
bool BookTable::updatePrice(QString isbn, QString price) noexcept
{
    bool success = false;
    if(!QSqlDatabase::database().open())
    {
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE " + mTableName + " SET " + mPriceColumn + " = '" + price + "' WHERE " + mIsbnColumn +
                  "='" + isbn + "';");

    if(query.exec())
    {
        success = true;
    }

    QSqlDatabase::database().close();
    return success;
}

//--------------------
// updateStock: writes a new stock for the given book in the database
// Parameter: isbn, stock
//--------------------
bool BookTable::updateStock(QString isbn, int stock) noexcept
{
    bool success = false;
    if(!QSqlDatabase::database().open())
    {
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE " + mTableName + " SET " + mStockColumn + " = '" + QString::number(stock) + "' WHERE " + mIsbnColumn +
                  "='" + isbn + "';");

    if(query.exec())
    {
        success = true;
    }

    QSqlDatabase::database().close();
    return success;
}

//--------------------
// updateAvailable: writes a new available-number for the given book in the database
// Parameter: isbn, available
//--------------------
bool BookTable::updateAvailable(QString isbn, int available) noexcept
{
    bool success = false;
    if(!QSqlDatabase::database().open())
    {
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE " + mTableName + " SET " + mAvailableColumn + "='" + QString::number(available) + "' WHERE " + mIsbnColumn +
                  "='" + isbn + "';");

    if(query.exec())
    {
        success = true;
    }

    QSqlDatabase::database().close();
    return success;
}

//--------------------
// updateComment: writes a new comment for the given book in the database
// Parameter: isbn, comment
//--------------------
bool BookTable::updateComment(QString isbn, QString comment) noexcept
{
    bool success = false;
    if(!QSqlDatabase::database().open())
    {
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE " + mTableName + " SET " + mCommentColumn + "='" + comment + "' WHERE " + mIsbnColumn +
                  "='" + isbn + "';");

    if(query.exec())
    {
        success = true;
    }

    QSqlDatabase::database().close();
    return success;
}

//--------------------
// updateStocktakingCount: writes a new stocktakingCount for the given book in the database
// Parameter: isbn, stocktakingCount
//--------------------
bool BookTable::updateStocktakingCount(QString isbn, int stocktakingCount) noexcept
{
    bool success = false;
    if(!QSqlDatabase::database().open())
    {
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE " + mTableName + " SET " + mStockTakingCountColumn + "='" + QString::number(stocktakingCount) +
                  "' WHERE " + mIsbnColumn + "='" + isbn + "';");

    if(query.exec())
    {
        success = true;
    }

    QSqlDatabase::database().close();
    return success;
}

//--------------------
// updateGrade: writes a new grade for the given book in the database
// Parameter: isbn, stocktakingDate
//--------------------
bool BookTable::updateStocktakingDate(QString isbn, QString stocktakingDate) noexcept
{
    bool success = false;
    if(!QSqlDatabase::database().open())
    {
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE " + mTableName + " SET " + mStockTakingDateColumn + "='" + stocktakingDate + "' WHERE " + mIsbnColumn +
                  "='" + isbn + "';");

    if(query.exec())
    {
        success = true;
    }

    QSqlDatabase::database().close();
    return success;
}
