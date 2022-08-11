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
    void recreateBezierPatches();

    KnotPoint *getClosestKnotPointToRay(const QVector3D &rayOrigin, const QVector3D &rayDirection, float maxDistance = 0.5f);

    const QList<Bezier *> &bezierPatches();
    const QList<KnotPoint *> &knotPoints();

    Spline *deepCopy();

    // Curve interface
    void update();
    QVector3D valueAt(float t) const;
    QVector3D tangentAt(float t) const;
    void translate(const QVector3D &translation);
    float closestDistanceToRay(const QVector3D &rayOrigin, const QVector3D &rayDirection, float epsilon = 0.01f);
    float length();

    int sectorCount() const;
    void setSectorCount(int newSectorCount);

    float radius() const;
    void setRadius(float newRadius);

private:
    Eigen::MatrixXf createCoefficientMatrix();
    QVector<QVector3D> getSplineControlPoints();

private:
    QList<KnotPoint *> mKnotPoints;
    QList<Bezier *> mBezierPatches;

    int mSectorCount;
    float mRadius;

    bool mPointRemovedOrAdded;
};

#endif // SPLINE_H
