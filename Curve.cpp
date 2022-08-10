#include "Curve.h"

Curve::Curve(QObject *parent)
    : QObject(parent)
    , mSelected(false)
    , mDirty(false)
    , mSectorCount(128)
    , mRadius(0.25f) {}

Curve::~Curve() {}

bool Curve::selected() const {
    return mSelected;
}

void Curve::setSelected(bool newSelected) {
    mSelected = newSelected;
}

bool Curve::dirty() const {
    return mDirty;
}

void Curve::setDirty(bool newDirty) {
    mDirty = newDirty;
}

const Material &Curve::material() const {
    return mMaterial;
}

void Curve::setMaterial(const Material &newMaterial) {
    mMaterial = newMaterial;
}

int Curve::sectorCount() const {
    return mSectorCount;
}

void Curve::setSectorCount(int newSectorCount) {
    mSectorCount = newSectorCount;
}

float Curve::radius() const {
    return mRadius;
}

void Curve::setRadius(float newRadius) {
    mRadius = newRadius;
}
