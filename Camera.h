#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix4x4>
#include <QMouseEvent>
#include <QTimer>

class Camera : public QObject
{
    Q_OBJECT
public:
    explicit Camera(QObject *parent = nullptr);

    QVector3D position() const;
    void setPosition(const QVector3D &newPosition);

    QQuaternion rotation() const;
    void setRotation(const QQuaternion &newRotation);

    const QMatrix4x4 &transformation();
    const QMatrix4x4 &projection() const;

    void onKeyPressed(QKeyEvent *event);
    void onKeyReleased(QKeyEvent *event);
    void onMousePressed(QMouseEvent *event);
    void onMouseReleased(QMouseEvent *event);
    void onMouseMoved(QMouseEvent *event);

    QVector3D getDirectionFromScreen(int x, int y, int width, int height);
    QVector3D getViewDirection();

    float verticalFov() const;
    void setVerticalFov(float newVerticalFov);

    float horizontalFov() const;

    float aspectRatio() const;
    void setAspectRatio(float newAspectRatio);

    float zNear() const;
    void setZNear(float newZNear);

    float zFar() const;
    void setZFar(float newZFar);

private slots:
    void update();
    void updateProjection();

private:
    QMatrix4x4 mProjection;
    QMatrix4x4 mTransformation;
    QVector3D mPosition;
    QQuaternion mRotation;

    float mVerticalFov;
    float mAspectRatio;
    float mZNear;
    float mZFar;

    QTimer mTimer;
    QMap<Qt::Key, bool> mPressedKeys;

    float mMovementSpeed;
    float mAngularSpeed;

    bool mMousePressed;
    float mMousePreviousX;
    float mMousePreviousY;
    float mMouseDeltaX;
    float mMouseDeltaY;

    bool mUpdateRotation;
    bool mUpdatePosition;

    static const QMap<Qt::Key, QVector3D> KEY_BINDINGS;
};

#endif // CAMERA_H
