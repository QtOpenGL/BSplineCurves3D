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
        NotInitializedYet,
        GeneratingVertices,
        WaitingForInitialization,
        Initialized,
    };

    void addControlPoint(ControlPoint *controlPoint);
    void removeControlPoint(ControlPoint *controlPoint);

    void insertControlPoint(int index, ControlPoint *controlPoint);
    void removeControlPoint(int index);

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

    VertexGenerationStatus vertexGenerationStatus() const;
    void setVertexGenerationStatus(VertexGenerationStatus newVertexGenerationStatus);

    void generateVertices();
    void initializeOpenGLStuff();
    void clearOpenGLStuff();

    void bind();
    void release();
    int count();

private:
    VertexGenerationStatus mVertexGenerationStatus;
    QList<ControlPoint *> mControlPoints;
    float mLength;
    int mTickCount;

    QOpenGLVertexArrayObject mVertexArray;
    QOpenGLBuffer mVertexBuffer;
    QOpenGLBuffer mNormalBuffer;

    QVector<QVector3D> mVertices;
    QVector<QVector3D> mNormals;
};

#endif // BEZIER_H
