#ifndef CURVE_H
#define CURVE_H

#include <QObject>

class Curve : public QObject
{
    Q_OBJECT
public:
    explicit Curve(QObject *parent = nullptr);
    virtual ~Curve();

    bool selected() const;
    void setSelected(bool newSelected);

    virtual QVector3D valueAt(float t) const = 0;
    virtual QVector3D tangentAt(float t) const = 0;
    virtual QVector3D normalAt(float t) const = 0;
    virtual float closestDistanceToRay(const QVector3D &rayOrigin, const QVector3D &rayDirection, float epsilon = 0.01) = 0;

protected:
    bool mSelected;
};

#endif // CURVE_H
