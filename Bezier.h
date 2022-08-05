#ifndef BEZIER_H
#define BEZIER_H

#include "KnotPoint.h"
#include <QObject>

class Bezier : public QObject
{
    Q_OBJECT
public:
    explicit Bezier(QObject *parent = nullptr);

    void addControlPoint(ControlPoint *controlPoint);

    bool selected() const;
    void setSelected(bool newSelected);

    const QList<ControlPoint *> &controlPoints() const;

private:
    bool mSelected;
    QList<ControlPoint *> mControlPoints;
};

#endif // BEZIER_H
