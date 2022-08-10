#ifndef POINT_H
#define POINT_H

#include <QObject>
#include <QVector3D>

class Point : public QObject
{
    Q_OBJECT
public:
    explicit Point(QObject *parent = nullptr);
    explicit Point(float x, float y, float z = 0.0f, QObject *parent = nullptr);
    explicit Point(const QVector3D &position, QObject *parent = nullptr);

    bool selected() const;
    void setSelected(bool newSelected);

    const QVector3D &position() const;
    void setPosition(const QVector3D &newPosition);

private:
    bool mSelected;
    QVector3D mPosition;
};

typedef Point ControlPoint;
typedef Point KnotPoint;

#endif // POINT_H
