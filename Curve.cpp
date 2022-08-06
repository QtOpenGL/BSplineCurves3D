#include "Curve.h"

Curve::Curve(QObject *parent)
    : QObject(parent)
    , mSelected(false)
    , mDirty(false)
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

bool Curve::dirty() const
{
    return mDirty;
}

void Curve::setDirty(bool newDirty)
{
    mDirty = newDirty;
}

const Material &Curve::material() const
{
    return mMaterial;
}

void Curve::setMaterial(const Material &newMaterial)
{
    mMaterial = newMaterial;
}
