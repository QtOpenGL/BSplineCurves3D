#ifndef SPLINE_H
#define SPLINE_H

#include "Point.h"

#include <Eigen/Splines>
#include <QObject>
#include <QVector3D>

class Spline : public QObject
{
    Q_OBJECT
public:
    explicit Spline(QObject *parent = nullptr);
    ~Spline();
    void addKnotPoint(KnotPoint *knotPoint);
    void updateSpline();
    QVector<QVector3D> getControlPointPositions();
    float closestDistanceToRay(const QVector3D &cameraPosition, const QVector3D &rayDirection, float epsilon = 0.01);

    bool selected() const;
    void setSelected(bool newSelected);

public:
    Eigen::Spline3f mEigenSpline;
    QVector<KnotPoint *> mKnotPoints;
    bool mSelected;
};

#endif // SPLINE_H
