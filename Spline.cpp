#include "Spline.h"

#include <QDebug>

Spline::Spline(QObject *parent)
    : QObject(parent)
    , mSelected(false)
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

    mEigenSpline = Eigen::SplineFitting<Eigen::Spline3f>::Interpolate(matrix, 3);
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

float Spline::closestDistanceToRay(const QVector3D &cameraPosition, const QVector3D &rayDirection, float epsilon)
{
    float minDistance = std::numeric_limits<float>::infinity();

    for (float t = 0.0f; t <= 1.0f; t += epsilon)
    {
        Eigen::Vector3f value = mEigenSpline(t);
        QVector3D diff = QVector3D(value.coeff(0), value.coeff(1), value.coeff(2)) - cameraPosition;

        qDebug() << QVector3D(value.coeff(0), value.coeff(1), value.coeff(2));
        float dot = QVector3D::dotProduct(diff, rayDirection);

        if (dot >= 0.0f)
        {
            float distance = (diff - rayDirection * dot).length();
            if (distance < minDistance)
            {
                minDistance = distance;
            }
        }
    }

    return minDistance;
}

bool Spline::selected() const
{
    return mSelected;
}

void Spline::setSelected(bool newSelected)
{
    mSelected = newSelected;
}
