#include "Bezier.h"

Bezier::Bezier(QObject *parent)
    : Curve(parent)
{}

Bezier::~Bezier() {}

void Bezier::addControlPoint(ControlPoint *controlPoint)
{
    mControlPoints << controlPoint;
    controlPoint->setParent(this);
}

void Bezier::removeControlPoint(ControlPoint *controlPoint)
{
    mControlPoints.removeAll(controlPoint);
    controlPoint->deleteLater();
}

void Bezier::insertControlPoint(int index, ControlPoint *controlPoint)
{
    mControlPoints.insert(index, controlPoint);
    controlPoint->setParent(this);
}

void Bezier::removeControlPoint(int index)
{
    ControlPoint *controlPoint = mControlPoints[index];
    mControlPoints.removeAt(index);
    controlPoint->deleteLater();
}

const QList<ControlPoint *> &Bezier::controlPoints() const
{
    return mControlPoints;
}

QVector<QVector3D> Bezier::getControlPointPositions()
{
    QVector<QVector3D> positions;

    for (auto &controlPoint : mControlPoints)
        positions << controlPoint->position();

    return positions;
}

QVector3D Bezier::valueAt(float t) const
{
    QVector3D value = QVector3D(0, 0, 0);
    int n = degree();

    for (int i = 0; i <= n; i++)
        value += choose(n, i) * pow(t, i) * pow(1 - t, n - i) * mControlPoints[i]->position();

    return value;
}

QVector3D Bezier::tangentAt(float t) const
{
    return QVector3D();
}

QVector3D Bezier::normalAt(float t) const
{
    return QVector3D();
}

int Bezier::degree() const
{
    return mControlPoints.size() - 1;
}

float Bezier::factorial(int n) const
{
    float result = 1.0f;

    for (int i = 1; i <= n; ++i)
        result *= i;

    return result;
}

float Bezier::choose(int n, int k) const
{
    return factorial(n) / (factorial(k) * factorial(n - k));
}

float Bezier::closestDistanceToRay(const QVector3D &rayOrigin, const QVector3D &rayDirection, float epsilon)
{
    float minDistance = std::numeric_limits<float>::infinity();

    for (float t = 0.0f; t <= 1.0f; t += epsilon)
    {
        QVector3D difference = valueAt(t) - rayOrigin;

        float dot = QVector3D::dotProduct(difference, rayDirection);

        if (dot >= 0.0f)
        {
            float distance = (difference - rayDirection * dot).length();
            if (distance < minDistance)
            {
                minDistance = distance;
            }
        }
    }

    return minDistance;
}

void Bezier::update() {}

void Bezier::translate(const QVector3D &translation)
{
    for (auto &controlPoint : mControlPoints)
    {
        controlPoint->setPosition(controlPoint->position() + translation);
    }
}

ControlPoint *Bezier::getClosestControlPointToRay(const QVector3D &rayOrigin, const QVector3D &rayDirection, float maxDistance, float epsilon)
{
    float minDistance = std::numeric_limits<float>::infinity();
    ControlPoint *closestControlPoint = nullptr;

    for (auto &controlPoint : mControlPoints)
    {
        QVector3D difference = controlPoint->position() - rayOrigin;

        float dot = QVector3D::dotProduct(difference, rayDirection);

        if (dot >= 0.0f)
        {
            float distance = (difference - rayDirection * dot).length();
            if (distance < minDistance)
            {
                minDistance = distance;
                closestControlPoint = controlPoint;
            }
        }
    }

    if (minDistance >= maxDistance)
        closestControlPoint = nullptr;

    return closestControlPoint;
}
