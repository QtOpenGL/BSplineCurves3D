#include "Spline.h"

Spline::Spline(QObject *parent)
    : QObject(parent)
{}

Spline::~Spline()
{
    for (int i = 0; i < mKnotPoints.size(); ++i)
    {
        if (mKnotPoints[i])
        {
            mKnotPoints[i]->deleteLater();
            mKnotPoints[i] = nullptr;
        }
    }
}

void Spline::addKnotPoint(KnotPoint *knotPoint)
{
    mKnotPoints << knotPoint;

    updateSpline();
}

void Spline::updateSpline()
{
    Eigen::MatrixXf matrix(3, mKnotPoints.size());

    for (int i = 0; i < mKnotPoints.size(); ++i)
    {
        matrix(0, i) = mKnotPoints[i]->position().x();
        matrix(1, i) = mKnotPoints[i]->position().y();
        matrix(2, i) = mKnotPoints[i]->position().z();
    }

    mEigenSpline = Eigen::SplineFitting<Eigen::Spline3f>::Interpolate(matrix, 2);
}

QVector<QVector3D> Spline::getControlPointPositions()
{
    QVector<QVector3D> positions;

    for (int i = 0; i < mEigenSpline.ctrls().cols(); ++i)
    {
        float x = mEigenSpline.ctrls().coeff(0, i);
        float y = mEigenSpline.ctrls().coeff(1, i);
        float z = mEigenSpline.ctrls().coeff(2, i);

        positions << QVector3D(x, y, z);
    }

    return positions;
}
