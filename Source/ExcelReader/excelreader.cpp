#include "excelreader.h"


ExcelReader::ExcelReader()
{
    mHandle = nullptr;
    mMax_worksheet = 0;
    mCurrentWorksheet = 0;
    mRows = -1;
    mColumns = -1;
}

int ExcelReader::openXLS(QString filename) noexcept
{
    if (filename.contains(".xlsx"))
        return FREEXL_UNKNOWN;

    int ret = freexl_open (filename.toStdString().c_str(), &mHandle);
    if (ret != FREEXL_OK)
        return ret;

    // check for password
    unsigned int info;
    ret = freexl_get_info (mHandle, FREEXL_BIFF_PASSWORD, &info);
    if (ret != FREEXL_OK)
        return ret;

    switch (info)
    {
        case FREEXL_BIFF_PLAIN:
          break;
        case FREEXL_BIFF_OBFUSCATED:
        default:
          return FREEXL_UNKNOWN;
    }

    ret = freexl_get_info (mHandle, FREEXL_BIFF_SHEET_COUNT, &mMax_worksheet);
    if (ret != FREEXL_OK)
        return ret;

    return FREEXL_OK;
}

int ExcelReader::close() noexcept
{
    int ret = freexl_close (&mHandle);
    mHandle = nullptr;
    return ret;
}

unsigned int ExcelReader::worksheeds() noexcept
{
    return mMax_worksheet;
}

int ExcelReader::selectworksheed(unsigned int index) noexcept
{
    if (mHandle == nullptr)
        return -1;

    if (index >= mMax_worksheet)
        return -1;

    int ret = freexl_select_active_worksheet (mHandle, index);
    if (ret != FREEXL_OK)
    {
        mRows = -1;
        mColumns = -1;
        return -1;
    }

    mCurrentWorksheet = index;

    ret = freexl_worksheet_dimensions (mHandle, &mRows, &mColumns);
    if (ret != FREEXL_OK)
        return -1;

    return FREEXL_OK;
}

unsigned int ExcelReader::rowsofselectedsheet()
{
    if (mHandle == nullptr)
        throw QString("Excel file is not open");
    return mRows;
}

unsigned int ExcelReader::columnsofselectedsheet()
{
    if (mHandle == nullptr)
        throw QString("Excel file is not open");
    return mColumns;
}

FreeXL_CellValue ExcelReader::getCellValue(unsigned int row, unsigned short column)
{
    if (mHandle == nullptr)
        throw QString("Error: Excel file was not opened!");

    FreeXL_CellValue cell = FreeXL_CellValue();
    if (freexl_get_cell_value (mHandle, row, column, &cell) != FREEXL_OK)
        throw QString("Error: code not find row and cell");

    return cell;
}

ExcelReader::~ExcelReader()
{
    if (mHandle != nullptr)
        close();
}
