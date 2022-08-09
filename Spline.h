#ifndef SPLINE_H
#define SPLINE_H

#include "Bezier.h"
#include "Curve.h"
#include "Point.h"

#include <QObject>
#include <QVector3D>

#include <Dense>

class Spline : public Curve
{
    Q_OBJECT
public:
    explicit Spline(QObject *parent = nullptr);
    ~Spline();

    void addKnotPoint(KnotPoint *knotPoint);
    void removeKnotPoint(KnotPoint *knotPoint);
    void removeAllPatches();

    KnotPoint *getClosestKnotPointToRay(const QVector3D &rayOrigin, const QVector3D &rayDirection, float maxDistance = 0.5f, float epsilon = 0.001f);

    const QList<Bezier *> &bezierPatches();
    const QList<KnotPoint *> &knotPoints();

    // Curve interface
    void update();
    QVector3D valueAt(float t) const;
    QVector3D tangentAt(float t) const;
    QVector3D normalAt(float t) const;
    void translate(const QVector3D &translation);
    float closestDistanceToRay(const QVector3D &rayOrigin, const QVector3D &rayDirection, float epsilon = 0.01f);

private:
    Eigen::MatrixXf createCoefficientMatrix();
    QVector<QVector3D> getSplineControlPoints();

private:
    QList<KnotPoint *> mKnotPoints;
    QList<Bezier *> mBezierPatches;
};

#endif // SPLINE_H
