#ifndef BEZIER_H
#define BEZIER_H

#include "Curve.h"
#include "KnotPoint.h"
#include <QObject>

class Bezier : public Curve
{
    Q_OBJECT
public:
    explicit Bezier(QObject *parent = nullptr);
    virtual ~Bezier();

    void addControlPoint(ControlPoint *controlPoint);
    void removeControlPoint(ControlPoint *controlPoint);

    void insertControlPoint(int index, ControlPoint *controlPoint);
    void removeControlPoint(int index);

    const QList<ControlPoint *> &controlPoints() const;
    QVector<QVector3D> getControlPointPositions();
    ControlPoint *getClosestControlPointToRay(const QVector3D &rayOrigin,
                                              const QVector3D &rayDirection,
                                              float maxDistance = 0.5f,
                                              float epsilon = 0.001f);

    // Curve interface
    virtual QVector3D valueAt(float t) const override;
    virtual QVector3D tangentAt(float t) const override;
    virtual QVector3D normalAt(float t) const override;
    virtual float closestDistanceToRay(const QVector3D &rayOrigin, const QVector3D &rayDirection, float epsilon) override;

private:
    QList<ControlPoint *> mControlPoints;
};

#endif // BEZIER_H
