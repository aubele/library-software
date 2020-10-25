#ifndef BOOKTABLE_H
#define BOOKTABLE_H

#include <QString>
#include <QList>
#include <db_global.h>

class DBSHARED_EXPORT BookTable
{
public:
    struct BookRow
    {
        QString isbn;
        QString title;
        QString subtitle;
        QString publisher;
        QString author;
        QString subject;
        QList<int> grade;
        int edition;
        QString price;
        int stock;
        int available;
        QString comment;
        int stocktakingCount;
        QString stocktakingDate;

        // TODO: kein discarded feld mehr?
        //bool discarded;
    };

    BookTable(QString tablename, QString isbncolumn, QString titlecolumn, QString subtitlecolumn,
              QString publishercolumn, QString authorcolumn, QString subjectcolumn, QString gradecolumn,
              QString editioncolumn, QString pricecolumn, QString stockcolumn, QString availablecolumn,
              QString commentcolumn, QString stocktakingcountcolumn, QString stocktakingdatecolumn) noexcept;
    virtual ~BookTable() noexcept;

    virtual bool insert(BookRow book) noexcept;
    virtual bool insert(QString isbn, QString title, QString subtitle, QString publisher, QString author,
                        QString subject, QList<int> grades, int edition, QString price, int stock, int available,
                        QString comment, int stocktakingCount, QString stocktakingDate) noexcept;

    virtual bool remove(QString isbn) noexcept;

    virtual BookTable::BookRow* getBook(QString isbn);
    virtual QList<BookRow> getAllBooks();

    virtual QList<BookRow> searchBookByTitle(QString title);
    virtual QList<BookRow> searchBookBySubTitle(QString subtitle);
    virtual QList<BookRow> searchBookByPublisher(QString publisher);
    virtual QList<BookRow> searchBookByAuthor(QString author);
    virtual QList<BookRow> searchBooksBySubject(QString subject);
    virtual QList<BookRow> searchBooksByComment(QString comment);

    virtual QList<BookRow> getBooksByGrade(int grade);
    virtual QList<BookRow> getBooksByEdition(int edition);
    virtual QList<BookRow> getBooksByStock(int stock);
    virtual QList<BookRow> getBooksByAvailable(int available);
    virtual QList<BookRow> getBooksByStocktakingCount(int stocktakingCount);
    virtual QList<BookRow> getBooksByStocktakingDate(QString stocktakingDate);

    virtual bool updateTitle(QString isbn, QString title) noexcept;
    virtual bool updateSubTitle(QString isbn, QString subtitle) noexcept;
    virtual bool updatePublisher(QString isbn, QString publisher) noexcept;
    virtual bool updateAuthor(QString isbn, QString author) noexcept;
    virtual bool updateSubject(QString isbn, QString subject) noexcept;
    virtual bool updateGrade(QString isbn, QList<int> grades) noexcept;
    virtual bool updateEdition(QString isbn, int edition) noexcept;
    virtual bool updatePrice(QString isbn, QString price) noexcept;
    virtual bool updateStock(QString isbn, int stock) noexcept;
    virtual bool updateAvailable(QString isbn, int available) noexcept;
    virtual bool updateComment(QString isbn, QString comment) noexcept;
    virtual bool updateStocktakingCount(QString isbn, int stocktakingCount) noexcept;
    virtual bool updateStocktakingDate(QString isbn, QString stocktakingDate) noexcept;

private:
    QString mTableName;
    QString mIsbnColumn;
    QString mTitleColumn;
    QString mSubTitleColumn;
    QString mPublisherColumn;
    QString mAuthorColumn;
    QString mSubjectColumn;
    QString mGradeColumn;
    QString mEditionColumn;
    QString mPriceColumn;
    QString mStockColumn;
    QString mAvailableColumn;
    QString mCommentColumn;
    QString mStockTakingCountColumn;
    QString mStockTakingDateColumn;
};

#endif // BOOKTABLE_H
