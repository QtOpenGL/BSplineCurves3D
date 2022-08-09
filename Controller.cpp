#include "Controller.h"
#include "Light.h"

#include <QDebug>

Controller::Controller(QObject *parent)
    : QObject(parent)
    , mSelectedCurve(nullptr)
    , mSelectedPoint(nullptr)
    , mPressedButton(Qt::NoButton)
{
    mRendererManager = RendererManager::instance();
    mCameraManager = CameraManager::instance();
    mLightManager = LightManager::instance();
    mModelManager = ModelManager::instance();
    mCurveManager = CurveManager::instance();
    mWindow = new Window;

    connect(mWindow, &Window::wheelMoved, this, &Controller::onWheelMoved);
    connect(mWindow, &Window::mousePressed, this, &Controller::onMousePressed);
    connect(mWindow, &Window::mouseReleased, this, &Controller::onMouseReleased);
    connect(mWindow, &Window::mouseMoved, this, &Controller::onMouseMoved);
    connect(mWindow, &Window::keyPressed, this, &Controller::onKeyPressed);
    connect(mWindow, &Window::keyReleased, this, &Controller::onKeyReleased);
    connect(mWindow, &Window::resized, this, &Controller::onResized);
    connect(mWindow, &Window::init, this, &Controller::init);
    connect(mWindow, &Window::render, this, &Controller::render);

    connect(mCurveManager, &CurveManager::selectedCurveChanged, this, [=](Curve *selectedCurve) { mSelectedCurve = selectedCurve; });
    connect(mCurveManager, &CurveManager::selectedPointChanged, this, [=](Point *selectedPoint) { mSelectedPoint = selectedPoint; });
}

void Controller::init()
{
    // Initialize Managers
    mRendererManager->init();

    mCamera = FreeCamera::create();
    mCamera->setPosition(QVector3D(0, 10, 10));
    mCamera->setVerticalFov(60.0f);
    mCamera->setZNear(0.1f);
    mCamera->setZFar(10000.0f);
    mCameraManager->setActiveCamera(mCamera);

    mLight = Light::create();
    mLight->setPosition(QVector3D(5, 20, 35));
    mLightManager->setActiveLight(mLight);

    // FIXME

    // Sphere
    {
        //        mSphere = Model::create(Model::Sphere);
        //        mSphere->setObjectName("Sphere");
        //        mSphere->setPosition(QVector3D(0, 0, 0));
        //        mSphere->setScale(QVector3D(0.001f, 0.001f, 0.001f));
        //        mSphere->material().setColor(QVector4D(0, 1, 0, 1));
    }

    // Plane
    {
        mPlane = Model::create(Model::Plane);
        mPlane->setObjectName("Plane");
        mPlane->setPosition(QVector3D(0, 0, 0));
    }

    // Cube
    {
        //        mCube = Model::create(Model::Cube);
        //        mCube->setObjectName("Cube");
        //        mCube->material().setColor(QVector4D(1, 0, 0, 1));
        //        mCube->setScale(QVector3D(0.01f, 0.01f, 0.01f));
        //        mCube->setPosition(QVector3D(0, 2, 0));
    }

    // Test Curves
    {
        mBezierTestCurve1 = new Bezier;
        mBezierTestCurve1->addControlPoint(new ControlPoint(1, 1, 1));
        mBezierTestCurve1->addControlPoint(new ControlPoint(0, 0, 5));
        mBezierTestCurve1->addControlPoint(new ControlPoint(0, 5, 0));
        mBezierTestCurve1->addControlPoint(new ControlPoint(5, 5, 5));

        mCurveManager->addCurve(mBezierTestCurve1);

        mBezierTestCurve2 = new Bezier;
        mBezierTestCurve2->addControlPoint(new ControlPoint(5, 0, 5));
        mBezierTestCurve2->addControlPoint(new ControlPoint(5, 5, 5));

        //mCurveManager->addCurve(mBezierTestCurve2);
    }
}

void Controller::run()
{
    //    mWindow->showMaximized();
    //    mWindow->showFullScreen();
    mWindow->resize(1024, 800);
    mWindow->show();
}

void Controller::onWheelMoved(QWheelEvent *event) {}

void Controller::onMousePressed(QMouseEvent *event)
{
    mPressedButton = event->button();

    if (mPressedButton == Qt::RightButton)
    {
        mCameraManager->onMousePressed(event);
    }
    else if (mPressedButton == Qt::LeftButton)
    {
        QVector3D rayDirection = mCameraManager->getDirectionFromScreen(event->x(), event->y(), mWindow->width(), mWindow->height());
        QVector3D rayOrigin = mCameraManager->activeCamera()->position();

        // FIXME: Logic
        if (mSelectedCurve)
        {
            mCurveManager->selectControlPoint(rayOrigin, rayDirection);
        }

        if (!mSelectedPoint)
        {
            mCurveManager->selectCurve(rayOrigin, rayDirection);
        }
    }

    if (mSelectedPoint)
    {
        QVector3D viewDirection = mCameraManager->activeCamera()->getViewDirection();
        Eigen::Vector3f normal = Eigen::Vector3f(viewDirection.x(), viewDirection.y(), viewDirection.z());
        Eigen::Vector3f eigenControlPointPosition = Eigen::Vector3f(mSelectedPoint->position().x(),
                                                                    mSelectedPoint->position().y(),
                                                                    mSelectedPoint->position().z());

        normal.normalize();
        mTranslationPlane = Eigen::Hyperplane<float, 3>(normal, -normal.dot(eigenControlPointPosition));
    }
}

void Controller::onMouseReleased(QMouseEvent *event)
{
    mPressedButton = Qt::NoButton;

    if (event->button() == Qt::RightButton)
    {
        mCameraManager->onMouseReleased(event);
    }
}

void Controller::onMouseMoved(QMouseEvent *event)
{
    if (mPressedButton == Qt::RightButton)
    {
        mCameraManager->onMouseMoved(event);
    }
    else if (mPressedButton == Qt::LeftButton)
    {
        if (mSelectedPoint)
        {
            QVector3D rayDirection = mCameraManager->getDirectionFromScreen(event->x(), event->y(), mWindow->width(), mWindow->height());
            QVector3D rayOrigin = mCamera->position();

            Eigen::Vector3f eigenRayDirection = Eigen::Vector3f(rayDirection.x(), rayDirection.y(), rayDirection.z());
            Eigen::Vector3f eigenRayOrigin = Eigen::Vector3f(rayOrigin.x(), rayOrigin.y(), rayOrigin.z());

            auto line = Eigen::ParametrizedLine<float, 3>(eigenRayOrigin, eigenRayDirection);
            float t = line.intersection(mTranslationPlane);
            Eigen::Vector3f intersection = line.pointAt(t);

            if (!isnan(t) && !isinf(t))
            {
                mSelectedPoint->setPosition(QVector3D(intersection.x(), intersection.y(), intersection.z()));
            }
        }
    }
}

void Controller::onKeyPressed(QKeyEvent *event)
{
    mCameraManager->onKeyPressed(event);
}

void Controller::onKeyReleased(QKeyEvent *event)
{
    mCameraManager->onKeyReleased(event);
}

void Controller::onResized(int w, int h)
{
    mCamera->setAspectRatio((float) (w) / h);
}

void Controller::render(float ifps)
{
    mCameraManager->update(ifps);

    mRendererManager->render(ifps);
}
