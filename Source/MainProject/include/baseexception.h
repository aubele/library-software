#ifndef EXCEPTION_H
#define EXCEPTION_H
#include <exception>
#include "baseqt_global.h"
#include <QString>

class BASEQTSHARED_EXPORT BaseException : std::exception
{
public:
    enum ErrorCode {ErrorInCode = 100};
    BaseException(ErrorCode code, QString explanation);
    ~BaseException();
    const char* what() const noexcept;

private:
    QString m_explanation;
    ErrorCode m_code;
};

#endif // EXCEPTION_H
