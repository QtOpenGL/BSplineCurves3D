#include "Curve.h"

Curve::Curve(QObject *parent)
    : QObject(parent)
    , mSelected(false)
{}

Curve::~Curve() {}

bool Curve::selected() const
{
    return mSelected;
}

void Curve::setSelected(bool newSelected)
{
    mSelected = newSelected;
}
