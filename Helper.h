#ifndef HELPER_H
#define HELPER_H

#include "Spline.h"

#include <QString>

class Helper
{
private:
    Helper();

public:
    static QByteArray getBytes(QString path);
    static QList<Spline *> loadCurveDataFromJson(const QString &filename);
    static bool saveCurveDataToJson(const QList<Spline *> &curves, const QString &filename);
    static QQuaternion rotateX(float angleRadians);
    static QQuaternion rotateY(float angleRadians);
    static QQuaternion rotateZ(float angleRadians);
};

#endif // HELPER_H
