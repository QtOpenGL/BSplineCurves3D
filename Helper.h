#ifndef HELPER_H
#define HELPER_H

#include "Spline.h"

#include <QString>

class Helper
{
public:
    Helper();

    static QByteArray getBytes(QString path);
    static QList<Spline *> loadCurveDataFromJson(const QString &filename);
    static bool saveCurveDataToJson(const QList<Spline *> &curves, const QString &filename);
};

#endif // HELPER_H
