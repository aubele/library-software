/*
 * IMPORTANT: The Library freexl.h must be reimplemented!
 * THIS MUST BE DONE LATER BUT AS SOON AS POSSIBLE!
*/
#ifndef EXCELREADER_H
#define EXCELREADER_H

#include "excelreader_global.h"
#include "freexl.h"
#include <QString>


class EXCELREADERSHARED_EXPORT ExcelReader
{

public:
    ExcelReader();
    int openXLS(QString filename) noexcept;
    int close() noexcept;
    unsigned int worksheeds() noexcept;
    int selectworksheed(unsigned int index) noexcept;
    unsigned int rowsofselectedsheet();
    unsigned int columnsofselectedsheet();
    FreeXL_CellValue getCellValue(unsigned int row, unsigned short column);
    ~ExcelReader();

private:
    const void *mHandle;
    unsigned int mMax_worksheet;
    unsigned int mCurrentWorksheet;
    unsigned int mRows;
    unsigned short mColumns;
};

#endif // EXCELREADER_H
