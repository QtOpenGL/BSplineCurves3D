#ifndef CURVEMANAGER_H
#define CURVEMANAGER_H

#include "Curve.h"
#include "Point.h"

#include <QObject>

class CurveManager : public QObject
{
    Q_OBJECT
private:
    explicit CurveManager(QObject *parent = nullptr);

public:
    static CurveManager *instance();

    const QList<Curve *> &curves() const;
    void addCurve(Curve *curve);
    void removeCurve(Curve *curve);

    Curve *selectCurve(const QVector3D &rayOrigin, const QVector3D &rayDirection, float maxDistance = 0.5f);
    ControlPoint *selectControlPoint(const QVector3D &rayOrigin, const QVector3D &rayDirection, float maxDistance = 0.5f);

    Curve *selectedCurve() const;
    void setSelectedCurve(Curve *newSelectedCurve);

    Point *selectedPoint() const;
    void setSelectedPoint(Point *newSelectedPoint);

signals:
    void selectedCurveChanged(Curve *curve);
    void selectedPointChanged(Point *point);

private:
    QList<Curve *> mCurves;
    Curve *mSelectedCurve;
    Point *mSelectedPoint;
};

#endif // CURVEMANAGER_H
