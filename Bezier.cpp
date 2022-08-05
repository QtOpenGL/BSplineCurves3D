#include "Bezier.h"

Bezier::Bezier(QObject *parent)
    : QObject(parent)
{}

void Bezier::addControlPoint(ControlPoint *controlPoint)
{
    mControlPoints << controlPoint;
}

bool Bezier::selected() const
{
    return mSelected;
}

void Bezier::setSelected(bool newSelected)
{
    mSelected = newSelected;
}

const QList<ControlPoint *> &Bezier::controlPoints() const
{
    return mControlPoints;
}
