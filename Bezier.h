#ifndef BEZIER_H
#define BEZIER_H

#include "Curve.h"
#include "Point.h"
#include <QObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>

class Bezier : public Curve, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit Bezier(QObject *parent = nullptr);
    virtual ~Bezier();

    enum class VertexGenerationStatus {
        GeneratingVertices,
        WaitingForOpenGLUpdate,
        Ready,
        Dirty,
    };

    void addControlPoint(ControlPoint *controlPoint);
    void removeControlPoint(ControlPoint *controlPoint);

    void insertControlPoint(int index, ControlPoint *controlPoint);
    void removeControlPoint(int index);
    void removeAllControlPoints();

    const QList<ControlPoint *> &controlPoints() const;
    QVector<QVector3D> getControlPointPositions();
    ControlPoint *getClosestControlPointToRay(const QVector3D &rayOrigin, const QVector3D &rayDirection, float maxDistance = 0.5f);

    int degree() const;
    float factorial(int n) const;
    float choose(int n, int k) const;

    // Curve interface
    virtual void update() override;
    virtual QVector3D valueAt(float t) const override;
    virtual QVector3D tangentAt(float t) const override;
    virtual float closestDistanceToRay(const QVector3D &rayOrigin, const QVector3D &rayDirection, float epsilon = 0.01f) override;
    virtual void translate(const QVector3D &translation) override;
    virtual float length() override;

    void generateVertices();
    void initializeOpenGLStuff();
    void updateOpenGLStuff();

    void bind();
    void release();
    int count();

    int sectorCount() const;
    void setSectorCount(int newSectorCount);

    float radius() const;
    void setRadius(float newRadius);

    VertexGenerationStatus vertexGenerationStatus() const;
    void setVertexGenerationStatus(VertexGenerationStatus newVertexGenerationStatus);

    bool initialized() const;
    void setInitialized(bool newInitialized);

private:
    QList<ControlPoint *> mControlPoints;
    float mLength;
    int mTickCount;
    int mSectorCount;
    float mRadius;

    QOpenGLVertexArrayObject mVertexArray;
    QOpenGLBuffer mVertexBuffer;
    QOpenGLBuffer mNormalBuffer;

    QVector<QVector3D> mVertices;
    QVector<QVector3D> mNormals;

    VertexGenerationStatus mVertexGenerationStatus;

    bool mInitialized;
};

#endif // BEZIER_H
