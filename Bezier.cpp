#include "Bezier.h"
#include "Helper.h"

#include <QQuaternion>
#include <QtConcurrent>
#include <QtMath>

#include <Dense>

Bezier::Bezier(QObject *parent)
    : Curve(parent)
    , mTickCount(100)
    , mSectorCount(128)
    , mRadius(0.25f)
    , mVertexGenerationStatus(VertexGenerationStatus::Dirty)
    , mInitialized(false)
{
    initializeOpenGLFunctions();
}

Bezier::~Bezier()
{
    mVertexArray.destroy();
    mVertexBuffer.destroy();
    mNormalBuffer.destroy();
}

void Bezier::addControlPoint(ControlPoint *controlPoint)
{
    mControlPoints << controlPoint;
    controlPoint->setParent(this);
    mDirty = true;
}

void Bezier::removeControlPoint(ControlPoint *controlPoint)
{
    mControlPoints.removeAll(controlPoint);
    controlPoint->deleteLater();
    mDirty = true;
}

void Bezier::insertControlPoint(int index, ControlPoint *controlPoint)
{
    mControlPoints.insert(index, controlPoint);
    controlPoint->setParent(this);
    mDirty = true;
    mVertexGenerationStatus = VertexGenerationStatus::Dirty;
}

void Bezier::removeControlPoint(int index)
{
    ControlPoint *controlPoint = mControlPoints[index];
    mControlPoints.removeAt(index);
    controlPoint->setParent(nullptr);
    controlPoint->deleteLater();
    mDirty = true;
    mVertexGenerationStatus = VertexGenerationStatus::Dirty;
}

void Bezier::removeAllControlPoints()
{
    for (auto &point : mControlPoints)
    {
        point->deleteLater();
    }

    mControlPoints.clear();

    mDirty = true;
    mVertexGenerationStatus = VertexGenerationStatus::Dirty;
}

const QList<ControlPoint *> &Bezier::controlPoints() const
{
    return mControlPoints;
}

QVector<QVector3D> Bezier::getControlPointPositions()
{
    QVector<QVector3D> positions;

    for (auto &controlPoint : mControlPoints)
        positions << controlPoint->position();

    return positions;
}

QVector3D Bezier::valueAt(float t) const
{
    QVector3D value = QVector3D(0, 0, 0);
    int n = degree();

    for (int i = 0; i <= n; i++)
        value += choose(n, i) * pow(t, i) * pow(1 - t, n - i) * mControlPoints[i]->position();

    return value;
}

QVector3D Bezier::tangentAt(float t) const
{
    QVector3D tangent = QVector3D(0, 0, 0);
    int order = degree();

    for (int i = 0; i <= order - 1; i++)
    {
        float coefficient = choose(order - 1, i) * pow(t, i) * pow(1 - t, order - 1 - i);
        tangent += coefficient * (mControlPoints.at(i)->position() - mControlPoints.at(i + 1)->position());
    }

    tangent.normalize();

    return tangent;
}

int Bezier::degree() const
{
    return mControlPoints.size() - 1;
}

float Bezier::factorial(int n) const
{
    float result = 1.0f;

    for (int i = 1; i <= n; ++i)
        result *= i;

    return result;
}

float Bezier::choose(int n, int k) const
{
    return factorial(n) / (factorial(k) * factorial(n - k));
}

float Bezier::closestDistanceToRay(const QVector3D &rayOrigin, const QVector3D &rayDirection, float epsilon)
{
    float minDistance = std::numeric_limits<float>::infinity();

    for (float t = 0.0f; t <= 1.0f; t += epsilon)
    {
        QVector3D difference = valueAt(t) - rayOrigin;

        float dot = QVector3D::dotProduct(difference, rayDirection);

        if (dot >= 0.0f)
        {
            float distance = (difference - rayDirection * dot).length();
            if (distance < minDistance)
            {
                minDistance = distance;
            }
        }
    }

    return minDistance;
}

void Bezier::update()
{
    float epsilon = 0.01f;
    float length = 0.0f;

    for (float t = epsilon; t <= 1.0f; t += epsilon)
    {
        float delta = (valueAt(t - epsilon) - valueAt(t)).length();
        length += delta;
    }

    mLength = length;

    mDirty = false;
}

void Bezier::translate(const QVector3D &translation)
{
    for (auto &controlPoint : mControlPoints)
    {
        controlPoint->setPosition(controlPoint->position() + translation);
    }
}

float Bezier::length()
{
    if (mDirty)
        update();

    return mLength;
}

void Bezier::generateVertices()
{
    mVertexGenerationStatus = VertexGenerationStatus::GeneratingVertices;

    QtConcurrent::run([=]() {
        mVertices.clear();
        mNormals.clear();

        mVertices.reserve(4 * mSectorCount * mTickCount);
        mNormals.reserve(4 * mSectorCount * mTickCount);

        float dt = 1.0f / mTickCount;
        float r = mRadius;

        for (float t = 0.0f; t <= 1.0f - dt; t += dt)
        {
            float t0 = t;
            float t1 = t0 + dt;

            QVector3D value0 = valueAt(t0);
            QVector3D value1 = valueAt(t1);

            QVector3D tangent0 = tangentAt(t0);
            QVector3D tangent1 = tangentAt(t1);

            QVector3D axis = QVector3D::crossProduct(QVector3D(1, 0, 0), tangent0);
            float angle = acos(QVector3D::dotProduct(QVector3D(1, 0, 0), tangent0));

            if (abs(angle) < 0.00001f || abs(angle - M_PI) < 0.00001f)
            {
                axis = QVector3D(0, 1, 0);
            }

            QQuaternion rotation = QQuaternion::fromAxisAndAngle(axis, qRadiansToDegrees(angle));

            for (int i = 0; i < mSectorCount; ++i)
            {
                float sectorAngle0 = 2 * float(i) / mSectorCount * M_PI;
                float sectorAngle1 = 2 * float(i + 1) / mSectorCount * M_PI;

                QVector3D position00 = value0 + rotation * QVector3D(0, r * cos(sectorAngle0), r * sin(sectorAngle0));
                QVector3D position01 = value0 + rotation * QVector3D(0, r * cos(sectorAngle1), r * sin(sectorAngle1));
                QVector3D position10 = Helper::projectOntoPlane(tangent1, value1, position00);
                QVector3D position11 = Helper::projectOntoPlane(tangent1, value1, position01);

                QVector3D normal = QVector3D::crossProduct((position10 - position00).normalized(), (position11 - position00).normalized());

                mVertices << position10;
                mVertices << position00;
                mVertices << position11;
                mVertices << position01;

                mNormals << normal;
                mNormals << normal;
                mNormals << normal;
                mNormals << normal;
            }
        }

        mVertexGenerationStatus = VertexGenerationStatus::WaitingForOpenGLUpdate;
    });
}

void Bezier::initializeOpenGLStuff()
{
    mVertexArray.create();
    mVertexArray.bind();

    // Vertices
    mVertexBuffer.create();
    mVertexBuffer.bind();
    mVertexBuffer.setUsagePattern(QOpenGLBuffer::UsagePattern::DynamicDraw);
    mVertexBuffer.allocate(sizeof(QVector3D) * 6 * 256 * mTickCount);
    glVertexAttribPointer(0,
                          3,                 // Size
                          GL_FLOAT,          // Type
                          GL_FALSE,          // Normalized
                          sizeof(QVector3D), // Stride
                          nullptr            // Offset
    );

    glEnableVertexAttribArray(0);
    mVertexBuffer.release();

    // Normals
    mNormalBuffer.create();
    mNormalBuffer.bind();
    mNormalBuffer.setUsagePattern(QOpenGLBuffer::UsagePattern::DynamicDraw);
    mNormalBuffer.allocate(sizeof(QVector3D) * 6 * 256 * mTickCount);

    glVertexAttribPointer(1,
                          3,                 // Size
                          GL_FLOAT,          // Type
                          GL_FALSE,          // Normalized
                          sizeof(QVector3D), // Stride
                          nullptr            // Offset
    );
    glEnableVertexAttribArray(1);
    mNormalBuffer.release();
    mVertexArray.release();

    mInitialized = true;
}

void Bezier::updateOpenGLStuff()
{
    mVertexArray.bind();

    mVertexBuffer.bind();
    mVertexBuffer.write(0, mVertices.constData(), sizeof(QVector3D) * mVertices.size());
    mVertexBuffer.release();

    mNormalBuffer.bind();
    mNormalBuffer.write(0, mNormals.constData(), sizeof(QVector3D) * mNormals.size());
    mNormalBuffer.release();

    mVertexArray.release();

    mVertexGenerationStatus = VertexGenerationStatus::Ready;
}

void Bezier::render()
{
    mVertexArray.bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, mVertices.size());
    mVertexArray.release();
}

int Bezier::sectorCount() const
{
    return mSectorCount;
}

void Bezier::setSectorCount(int newSectorCount)
{
    mSectorCount = newSectorCount;
    mVertexGenerationStatus = VertexGenerationStatus::Dirty;
}

float Bezier::radius() const
{
    return mRadius;
}

void Bezier::setRadius(float newRadius)
{
    mRadius = newRadius;
    mVertexGenerationStatus = VertexGenerationStatus::Dirty;
}

Bezier::VertexGenerationStatus Bezier::vertexGenerationStatus() const
{
    return mVertexGenerationStatus;
}

void Bezier::setVertexGenerationStatus(VertexGenerationStatus newVertexGenerationStatus)
{
    mVertexGenerationStatus = newVertexGenerationStatus;
}

bool Bezier::initialized() const
{
    return mInitialized;
}

void Bezier::setInitialized(bool newInitialized)
{
    mInitialized = newInitialized;
}

ControlPoint *Bezier::getClosestControlPointToRay(const QVector3D &rayOrigin, const QVector3D &rayDirection, float maxDistance)
{
    float minDistance = std::numeric_limits<float>::infinity();
    ControlPoint *closestControlPoint = nullptr;

    for (auto &controlPoint : mControlPoints)
    {
        QVector3D difference = controlPoint->position() - rayOrigin;

        float dot = QVector3D::dotProduct(difference, rayDirection);

        if (dot >= 0.0f)
        {
            float distance = (difference - rayDirection * dot).length();
            if (distance < minDistance)
            {
                minDistance = distance;
                closestControlPoint = controlPoint;
            }
        }
    }

    if (minDistance >= maxDistance)
        closestControlPoint = nullptr;

    return closestControlPoint;
}
