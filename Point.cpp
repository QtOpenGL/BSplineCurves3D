#include "Point.h"
#include "Curve.h"

Point::Point(QObject *parent)
    : QObject(parent)
    , mSelected(false)
    , mPosition(0, 0, 0)
{}

Point::Point(float x, float y, float z, QObject *parent)
    : QObject(parent)
    , mSelected(false)
    , mPosition(x, y, z)
{}

Point::Point(const QVector3D &position, QObject *parent)
    : QObject(parent)
    , mSelected(false)
    , mPosition(position)
{}

bool Point::selected() const
{
    return mSelected;
}

void Point::setSelected(bool newSelected)
{
    mSelected = newSelected;
}

const QVector3D &Point::position() const
{
    return mPosition;
}

void Point::setPosition(const QVector3D &newPosition)
{
    mPosition = newPosition;

    Curve *curve = dynamic_cast<Curve *>(parent());

    if (curve)
        curve->setDirty(true);
}
