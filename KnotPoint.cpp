#include "KnotPoint.h"

KnotPoint::KnotPoint(QObject *parent)
    : QObject(parent)
    , mSelected(false)
    , mPosition(0, 0, 0)
{}

KnotPoint::KnotPoint(float x, float y, float z, QObject *parent)
    : QObject(parent)
    , mSelected(false)
    , mPosition(x, y, z)
{}

bool KnotPoint::selected() const
{
    return mSelected;
}

void KnotPoint::setSelected(bool newSelected)
{
    mSelected = newSelected;
}

const QVector3D &KnotPoint::position() const
{
    return mPosition;
}

void KnotPoint::setPosition(const QVector3D &newPosition)
{
    mPosition = newPosition;
}
