#include "CurveManager.h"
#include <QDebug>

CurveManager::CurveManager(QObject *parent)
    : QObject(parent)
    , mSelectedCurve(nullptr)
    , mSelectedPoint(nullptr)
    , mGlobalPipeRadius(0.125f)
    , mGlobalPipeSectorCount(128)
{}

CurveManager *CurveManager::instance()
{
    static CurveManager instance;

    return &instance;
}

const QList<Spline *> &CurveManager::curves() const
{
    return mCurves;
}

QList<Spline *> &CurveManager::getCurvesNonConst()
{
    return mCurves;
}

void CurveManager::addCurve(Spline *curve)
{
    mCurves << curve;
}

void CurveManager::removeCurve(Spline *curve)
{
    if (curve == mSelectedCurve)
        setSelectedCurve(nullptr);

    mCurves.removeAll(curve);
    curve->deleteLater();
}

void CurveManager::removeAllCurves()
{
    for (auto &curve : mCurves)
    {
        curve->deleteLater();
    }

    mCurves.clear();
    setSelectedCurve(nullptr);
    setSelectedKnotPoint(nullptr);
}

void CurveManager::addCurves(QList<Spline *> curves)
{
    for (auto &curve : curves)
    {
        mCurves << curve;
    }
}

Spline *CurveManager::selectCurve(const QVector3D &rayOrigin, const QVector3D &rayDirection, float maxDistance)
{
    float minDistance = std::numeric_limits<float>::infinity();
    Spline *selectedCurve = nullptr;

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

KnotPoint *CurveManager::selectKnotPoint(const QVector3D &rayOrigin, const QVector3D &rayDirection, float maxDistance)
{
    if (mSelectedCurve)
    {
        KnotPoint *point = mSelectedCurve->getClosestKnotPointToRay(rayOrigin, rayDirection, maxDistance);
        setSelectedKnotPoint(point);
        return point;
    }

    return nullptr;
}

Spline *CurveManager::selectedCurve() const
{
    return mSelectedCurve;
}

void CurveManager::setSelectedCurve(Spline *newSelectedCurve)
{
    if (mSelectedCurve == newSelectedCurve)
        return;

    if (mSelectedCurve)
        mSelectedCurve->setSelected(false);

    if (newSelectedCurve)
        newSelectedCurve->setSelected(true);

    mSelectedCurve = newSelectedCurve;

    emit selectedCurveChanged(mSelectedCurve);

    setSelectedKnotPoint(nullptr);
}

KnotPoint *CurveManager::selectedKnotPoint() const
{
    return mSelectedPoint;
}

void CurveManager::setSelectedKnotPoint(KnotPoint *newSelectedPoint)
{
    if (mSelectedPoint == newSelectedPoint)
        return;

    if (mSelectedPoint)
        mSelectedPoint->setSelected(false);

    if (newSelectedPoint)
        newSelectedPoint->setSelected(true);

    mSelectedPoint = newSelectedPoint;
    emit selectedKnotPointChanged(mSelectedPoint);
}

float CurveManager::globalPipeRadius() const
{
    return mGlobalPipeRadius;
}

void CurveManager::setGlobalPipeRadius(float newGlobalPipeRadius)
{
    mGlobalPipeRadius = newGlobalPipeRadius;

    for (auto &curve : mCurves)
    {
        curve->setRadius(mGlobalPipeRadius);
    }
}

int CurveManager::globalPipeSectorCount() const
{
    return mGlobalPipeSectorCount;
}

void CurveManager::setGlobalPipeSectorCount(int newGlobalPipeSectorCount)
{
    mGlobalPipeSectorCount = newGlobalPipeSectorCount;

    for (auto &curve : mCurves)
    {
        curve->setSectorCount(mGlobalPipeSectorCount);
    }
}
