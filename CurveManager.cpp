#include "CurveManager.h"
#include "Bezier.h"
#include <QDebug>

CurveManager::CurveManager(QObject *parent)
    : QObject(parent)
{}

CurveManager *CurveManager::instance()
{
    static CurveManager instance;

    return &instance;
}

const QList<Curve *> &CurveManager::curves() const
{
    return mCurves;
}

void CurveManager::addCurve(Curve *curve)
{
    mCurves << curve;
}

void CurveManager::removeCurve(Curve *curve)
{
    mCurves.removeAll(curve);
    curve->deleteLater();
}

Curve *CurveManager::selectCurve(const QVector3D &rayOrigin, const QVector3D &rayDirection, float maxDistance)
{
    float minDistance = std::numeric_limits<float>::infinity();
    Curve *selectedCurve = nullptr;

    for (auto &curve : mCurves)
    {
        float distance = curve->closestDistanceToRay(rayOrigin, rayDirection);

        if (distance < minDistance)
        {
            minDistance = distance;
            selectedCurve = curve;
        }
    }

    if (minDistance >= maxDistance)
    {
        selectedCurve = nullptr;
    }

    setSelectedCurve(selectedCurve);

    return selectedCurve;
}

ControlPoint *CurveManager::selectControlPoint(const QVector3D &rayOrigin, const QVector3D &rayDirection, float maxDistance)
{
    Bezier *bezier = dynamic_cast<Bezier *>(mSelectedCurve);

    if (bezier)
    {
        ControlPoint *point = bezier->getClosestControlPointToRay(rayOrigin, rayDirection, maxDistance);
        setSelectedPoint(point);
        return point;
    }

    return nullptr;
}

Curve *CurveManager::selectedCurve() const
{
    return mSelectedCurve;
}

void CurveManager::setSelectedCurve(Curve *newSelectedCurve)
{
    if (mSelectedCurve == newSelectedCurve)
        return;

    if (mSelectedCurve)
        mSelectedCurve->setSelected(false);

    if (newSelectedCurve)
        newSelectedCurve->setSelected(true);

    mSelectedCurve = newSelectedCurve;

    emit selectedCurveChanged(mSelectedCurve);

    setSelectedPoint(nullptr);
}

Point *CurveManager::selectedPoint() const
{
    return mSelectedPoint;
}

void CurveManager::setSelectedPoint(Point *newSelectedPoint)
{
    if (mSelectedPoint == newSelectedPoint)
        return;

    if (mSelectedPoint)
        mSelectedPoint->setSelected(false);

    if (newSelectedPoint)
        newSelectedPoint->setSelected(true);

    mSelectedPoint = newSelectedPoint;
    emit selectedPointChanged(mSelectedPoint);
}
