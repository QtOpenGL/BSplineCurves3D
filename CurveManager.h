#ifndef CURVEMANAGER_H
#define CURVEMANAGER_H

#include "Point.h"
#include "Spline.h"

#include <QObject>

class CurveManager : public QObject
{
    Q_OBJECT
private:
    explicit CurveManager(QObject *parent = nullptr);

public:
    static CurveManager *instance();

    const QList<Spline *> &curves() const;
    QList<Spline *> &getCurvesNonConst();

    void addCurve(Spline *curve);
    void removeCurve(Spline *curve);
    void removeAllCurves();
    void addCurves(QList<Spline *> curves);

    Spline *selectCurve(const QVector3D &rayOrigin, const QVector3D &rayDirection, float maxDistance = 0.5f);
    ControlPoint *selectKnotPoint(const QVector3D &rayOrigin, const QVector3D &rayDirection, float maxDistance = 0.5f);

    Spline *selectedCurve() const;
    void setSelectedCurve(Spline *newSelectedCurve);

    Point *selectedKnotPoint() const;
    void setSelectedKnotPoint(Point *newSelectedPoint);

    float globalPipeRadius() const;
    void setGlobalPipeRadius(float newGlobalPipeRadius);

    int globalPipeSectorCount() const;
    void setGlobalPipeSectorCount(int newGlobalPipeSectorCount);

signals:
    void selectedCurveChanged(Spline *curve);
    void selectedKnotPointChanged(KnotPoint *point);

private:
    QList<Spline *> mCurves;
    Spline *mSelectedCurve;
    KnotPoint *mSelectedPoint;
    float mGlobalPipeRadius;
    int mGlobalPipeSectorCount;
};

#endif // CURVEMANAGER_H
