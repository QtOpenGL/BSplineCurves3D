#ifndef KNOTPOINT_H
#define KNOTPOINT_H

#include <QObject>
#include <QVector3D>

class KnotPoint : public QObject
{
    Q_OBJECT
public:
    explicit KnotPoint(QObject *parent = nullptr);
    explicit KnotPoint(float x, float y, float z, QObject *parent = nullptr);

    bool selected() const;
    void setSelected(bool newSelected);

    const QVector3D &position() const;
    void setPosition(const QVector3D &newPosition);

private:
    bool mSelected;
    QVector3D mPosition;
};

typedef KnotPoint ControlPoint;

#endif // KNOTPOINT_H
