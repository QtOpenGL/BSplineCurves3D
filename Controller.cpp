#include "Controller.h"
#include "Light.h"

#include <QDebug>

Controller::Controller(QObject *parent)
    : QObject(parent)
    , mSelectedCurve(nullptr)
    , mSelectedKnotPoint(nullptr)
    , mPressedButton(Qt::NoButton) {
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

    connect(mCurveManager, &CurveManager::selectedCurveChanged, this, [=](Spline *selectedCurve) { mSelectedCurve = selectedCurve; });
    connect(mCurveManager, &CurveManager::selectedKnotPointChanged, this, [=](KnotPoint *selectedPoint) { mSelectedKnotPoint = selectedPoint; });
}

void Controller::init() {
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
        mPlane->setVisible(false);
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
        mTestCurve1 = new Spline;
        //        mTestCurve1->addKnotPoint(new KnotPoint(0, 0, 0));
        //        mTestCurve1->addKnotPoint(new KnotPoint(2.5, 2.5, 0));
        //        mTestCurve1->addKnotPoint(new KnotPoint(0, 5, 0));
        //        mTestCurve1->addKnotPoint(new KnotPoint(0, 10, 0));
        //        mTestCurve1->addKnotPoint(new KnotPoint(0, 15, 0));
        //        mTestCurve1->addKnotPoint(new KnotPoint(0, 25, 0));
        //        mTestCurve1->addKnotPoint(new KnotPoint(0, 30, 0));
        //        mTestCurve1->addKnotPoint(new KnotPoint(0, 35, 0));

        mTestCurve1->addKnotPoint(new KnotPoint(1, -1, 0));
        mTestCurve1->addKnotPoint(new KnotPoint(-1, 2, 0));
        mTestCurve1->addKnotPoint(new KnotPoint(1, 4, 0));
        mTestCurve1->addKnotPoint(new KnotPoint(4, 3, 0));
        mTestCurve1->addKnotPoint(new KnotPoint(7, 5, 0));
        mTestCurve1->addKnotPoint(new KnotPoint(7, 7, 0));
        mTestCurve1->addKnotPoint(new KnotPoint(5, 7, 0));

        mCurveManager->addCurve(mTestCurve1);
    }
}

void Controller::run() {
    //    mWindow->showMaximized();
    //    mWindow->showFullScreen();
    mWindow->resize(1024, 800);
    mWindow->show();
}

void Controller::onWheelMoved(QWheelEvent *event) {}

void Controller::onMousePressed(QMouseEvent *event) {
    mPressedButton = event->button();

    if (mPressedButton == Qt::RightButton) {
        mCameraManager->onMousePressed(event);
    } else if (mPressedButton == Qt::LeftButton) {
        QVector3D rayDirection = mCameraManager->getDirectionFromScreen(event->x(), event->y(), mWindow->width(), mWindow->height());
        QVector3D rayOrigin = mCameraManager->activeCamera()->position();

        // FIXME: Logic
        if (mSelectedCurve) {
            mCurveManager->selectKnotPoint(rayOrigin, rayDirection);
        }

        if (!mSelectedKnotPoint) {
            mCurveManager->selectCurve(rayOrigin, rayDirection);
        }
    }

    if (mSelectedKnotPoint) {
        float x = mSelectedKnotPoint->position().x();
        float y = mSelectedKnotPoint->position().y();
        float z = mSelectedKnotPoint->position().z();

        QVector3D viewDirection = mCameraManager->activeCamera()->getViewDirection();
        Eigen::Vector3f normal = Eigen::Vector3f(viewDirection.x(), viewDirection.y(), viewDirection.z());
        Eigen::Vector3f eigenControlPointPosition = Eigen::Vector3f(x, y, z);

        normal.normalize();
        mTranslationPlane = Eigen::Hyperplane<float, 3>(normal, -normal.dot(eigenControlPointPosition));
    }
}

void Controller::onMouseReleased(QMouseEvent *event) {
    mPressedButton = Qt::NoButton;

    if (event->button() == Qt::RightButton) {
        mCameraManager->onMouseReleased(event);
    }
}

void Controller::onMouseMoved(QMouseEvent *event) {
    if (mPressedButton == Qt::RightButton) {
        mCameraManager->onMouseMoved(event);
    } else if (mPressedButton == Qt::LeftButton) {
        if (mSelectedKnotPoint) {
            QVector3D rayDirection = mCameraManager->getDirectionFromScreen(event->x(), event->y(), mWindow->width(), mWindow->height());
            QVector3D rayOrigin = mCamera->position();

            Eigen::Vector3f eigenRayDirection = Eigen::Vector3f(rayDirection.x(), rayDirection.y(), rayDirection.z());
            Eigen::Vector3f eigenRayOrigin = Eigen::Vector3f(rayOrigin.x(), rayOrigin.y(), rayOrigin.z());

            auto line = Eigen::ParametrizedLine<float, 3>(eigenRayOrigin, eigenRayDirection);
            float t = line.intersection(mTranslationPlane);
            Eigen::Vector3f intersection = line.pointAt(t);

            if (!isnan(t) && !isinf(t)) {
                mSelectedKnotPoint->setPosition(QVector3D(intersection.x(), intersection.y(), intersection.z()));
            }
        }
    }
}

void Controller::onKeyPressed(QKeyEvent *event) {
    mCameraManager->onKeyPressed(event);
}

void Controller::onKeyReleased(QKeyEvent *event) {
    mCameraManager->onKeyReleased(event);
}

void Controller::onResized(int w, int h) {
    mCamera->setAspectRatio((float) (w) / h);
}

void Controller::render(float ifps) {
    mCameraManager->update(ifps);

    mRendererManager->render(ifps);
}
