#ifndef BASEQT_GLOBAL_H
#define BASEQT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(BASEQT_LIBRARY)
#  define BASEQTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define BASEQTSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // BASEQT_GLOBAL_H
