#include "Spline.h"

#include <QDebug>

Spline::Spline(QObject *parent)
    : Curve(parent) {}

Spline::~Spline() {}

void Spline::addKnotPoint(KnotPoint *knotPoint) {
    mKnotPoints << knotPoint;
    knotPoint->setParent(this);
    mDirty = true;
}

void Spline::removeKnotPoint(KnotPoint *knotPoint) {
    mKnotPoints.removeAll(knotPoint);

    if (knotPoint)
        knotPoint->deleteLater();

    mDirty = true;
}

void Spline::removeAllPatches() {
    for (auto &patch : mBezierPatches) {
        patch->deleteLater();
    }

    mBezierPatches.clear();
}

KnotPoint *Spline::getClosestKnotPointToRay(const QVector3D &rayOrigin, const QVector3D &rayDirection, float maxDistance) {
    float minDistance = std::numeric_limits<float>::infinity();
    KnotPoint *closestPoint = nullptr;

    for (auto &point : mKnotPoints) {
        QVector3D difference = point->position() - rayOrigin;

        float dot = QVector3D::dotProduct(difference, rayDirection);

        if (dot >= 0.0f) {
            float distance = (difference - rayDirection * dot).length();
            if (distance < minDistance) {
                minDistance = distance;
                closestPoint = point;
            }
        }
    }

    if (minDistance >= maxDistance)
        closestPoint = nullptr;

    return closestPoint;
}

void Spline::update() {
    removeAllPatches();

    if (mKnotPoints.size() <= 2) {
        Bezier *patch = new Bezier;

        for (int i = 0; i < mKnotPoints.size(); ++i) {
            patch->addControlPoint(mKnotPoints[i]);
        }

        mBezierPatches << patch;

    } else {
        QVector<QVector3D> splineControlPoints = getSplineControlPoints();

        // Generate Bezier Patches
        for (int i = 1; i < mKnotPoints.size(); ++i) {
            Bezier *patch = new Bezier;
            ControlPoint *cp0 = new ControlPoint(mKnotPoints[i - 1]->position());
            ControlPoint *cp1 = new ControlPoint((2.0f / 3.0f) * splineControlPoints[i - 1] + (1.0f / 3.0f) * splineControlPoints[i]);
            ControlPoint *cp2 = new ControlPoint((1.0f / 3.0f) * splineControlPoints[i - 1] + (2.0f / 3.0f) * splineControlPoints[i]);
            ControlPoint *cp3 = new ControlPoint(mKnotPoints[i]->position());

            patch->addControlPoint(cp0);
            patch->addControlPoint(cp1);
            patch->addControlPoint(cp2);
            patch->addControlPoint(cp3);

            patch->setParent(this);

            mBezierPatches << patch;
        }
    }

    mDirty = false;
}

QVector3D Spline::valueAt(float t) const {
    int n = qMin((int) t, mBezierPatches.size() - 1);

    return mBezierPatches[n]->valueAt(t - n);
}

QVector3D Spline::tangentAt(float t) const {
    int n = qMin((int) t, mBezierPatches.size() - 1);

    return mBezierPatches[n]->tangentAt(t - n);
}

void Spline::translate(const QVector3D &translation) {
    for (auto &point : mKnotPoints) {
        point->setPosition(point->position() + translation);
    }

    mDirty = true;
}

float Spline::closestDistanceToRay(const QVector3D &cameraPosition, const QVector3D &rayDirection, float epsilon) {
    if (mDirty)
        update();

    float minDistance = std::numeric_limits<float>::infinity();

    for (auto &patch : mBezierPatches) {
        float distance = patch->closestDistanceToRay(cameraPosition, rayDirection);
        if (distance < minDistance) {
            minDistance = distance;
        }
    }

    return minDistance;
}

float Spline::length() {
    if (mDirty)
        update();

    float length = 0.0f;

    for (auto &patch : mBezierPatches) {
        length += patch->length();
    }

    return length;
}

Eigen::MatrixXf Spline::createCoefficientMatrix() {
    int n = mKnotPoints.size() - 2;
    Eigen::MatrixXf coef(n, n);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            coef(i, j) = 0;
        }
    }

    // First row
    coef(0, 0) = 4;
    coef(0, 1) = 1;

    for (int i = 1; i < n - 1; ++i) {
        coef(i, i - 1) = 1;
        coef(i, i) = 4;
        coef(i, i + 1) = 1;
    }

    // Last row
    coef(n - 1, n - 2) = 1;
    coef(n - 1, n - 1) = 4;

    return coef;
}

QVector<QVector3D> Spline::getSplineControlPoints() {
    int n = mKnotPoints.size();

    Eigen::MatrixXf knotPoints(n, 3);

    for (int i = 0; i < n; ++i) {
        knotPoints(i, 0) = mKnotPoints[i]->position().x();
        knotPoints(i, 1) = mKnotPoints[i]->position().y();
        knotPoints(i, 2) = mKnotPoints[i]->position().z();
    }

    // Constants on the right side
    Eigen::MatrixXf constants(n - 2, 3);

    for (int j = 0; j < 3; ++j) {
        constants(0, j) = 6 * knotPoints(1, j) - knotPoints(0, j);
        constants(n - 3, j) = 6 * knotPoints(n - 2, j) - knotPoints(n - 1, j);
    }

    for (int i = 1; i < n - 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            constants(i, j) = 6 * knotPoints(i + 1, j);
        }
    }

    // Compute BSpline control points
    Eigen::MatrixXf coef = createCoefficientMatrix();
    Eigen::MatrixXf controlPoints = coef.inverse() * constants;

    // Result
    QVector<QVector3D> result(n, QVector3D());
    result[0] = QVector3D(knotPoints(0, 0), knotPoints(0, 1), knotPoints(0, 2));
    result[n - 1] = QVector3D(knotPoints(n - 1, 0), knotPoints(n - 1, 1), knotPoints(n - 1, 2));
    for (int i = 0; i < n - 2; ++i)
        result[i + 1] = QVector3D(controlPoints(i, 0), controlPoints(i, 1), controlPoints(i, 2));

    return result;
}

const QList<KnotPoint *> &Spline::knotPoints() {
    if (mDirty)
        update();

    return mKnotPoints;
}

const QList<Bezier *> &Spline::bezierPatches() {
    if (mDirty)
        update();

    return mBezierPatches;
}
