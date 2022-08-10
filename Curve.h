#ifndef CURVE_H
#define CURVE_H

#include "Material.h"

#include <QObject>

class Curve : public QObject
{
    Q_OBJECT
public:
    explicit Curve(QObject *parent = nullptr);
    virtual ~Curve();

    bool selected() const;
    void setSelected(bool newSelected);

    bool dirty() const;
    void setDirty(bool newDirty);

    const Material &material() const;
    void setMaterial(const Material &newMaterial);

    virtual void update() = 0;
    virtual float closestDistanceToRay(const QVector3D &rayOrigin, const QVector3D &rayDirection, float epsilon = 0.01) = 0;
    virtual QVector3D valueAt(float t) const = 0;
    virtual QVector3D tangentAt(float t) const = 0;
    virtual void translate(const QVector3D &translation) = 0;
    virtual float length() = 0;

protected:
    bool mSelected;
    bool mDirty;
    Material mMaterial;
};

#endif // CURVE_H
