#include "baseexception.h"

//===========================
// BaseException
//===========================
//---------------------------
// Konstruktor
//---------------------------
BaseException::BaseException(ErrorCode code, QString explanation)
{
    m_code = code;
    m_explanation = explanation;
}

//----------------------------
// Destruktor
//----------------------------
BaseException::~BaseException()
{

}

const char* BaseException::what() const noexcept
{
    return (QString::number((int) m_code) + ": " + m_explanation).toStdString().c_str();
}
