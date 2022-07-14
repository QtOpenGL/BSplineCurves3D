#ifndef SPLINE_H
#define SPLINE_H

#include "KnotPoint.h"

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

private:
    Eigen::Spline3f mEigenSpline;
    QVector<KnotPoint *> mKnotPoints;
};

#endif // SPLINE_H
