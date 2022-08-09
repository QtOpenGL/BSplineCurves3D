#include "RendererManager.h"
#include "Bezier.h"
#include "Camera.h"
#include "Light.h"

#include <QtMath>

RendererManager::RendererManager(QObject *parent)
    : QObject(parent)
    , mBasicShader(nullptr)
    , mSelectedCurve(nullptr)
    , mSelectedKnotPoint(nullptr) {
    mModelManager = ModelManager::instance();
    mCameraManager = CameraManager::instance();
    mLightManager = LightManager::instance();
    mCurveManager = CurveManager::instance();

    connect(mCurveManager, &CurveManager::selectedCurveChanged, this, [=](Spline *selectedCurve) { mSelectedCurve = selectedCurve; });
    connect(mCurveManager, &CurveManager::selectedKnotPointChanged, this, [=](KnotPoint *selectedPoint) { mSelectedKnotPoint = selectedPoint; });
}

RendererManager *RendererManager::instance() {
    static RendererManager instance;
    return &instance;
}

bool RendererManager::init() {
    initializeOpenGLFunctions();
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glLineWidth(2.5f);

    qInfo() << Q_FUNC_INFO << "Initializing BasicShader...";
    mBasicShader = new BasicShader;

    if (!mBasicShader->init()) {
        qWarning() << Q_FUNC_INFO << "BasicShader could not be initialized.";
        return false;
    }

    qInfo() << Q_FUNC_INFO << "Loading and creating all models...";

    for (Model::Type type : Model::ALL_MODEL_TYPES) {
        ModelData *data = new ModelData(type);
        if (!data->load() || !data->create()) {
            data->deleteLater();
            continue;
        }

        mTypeToModelData.insert(type, data);
    }

    // Knot Point Model

    {
        mKnotPointModel = Model::create(Model::Sphere);
        mKnotPointModel->setObjectName("KnotPointModel");
        mKnotPointModel->material().setColor(QVector4D(0, 1, 0, 1));
        mKnotPointModel->setScale(QVector3D(0.0025f, 0.0025f, 0.0025f));
        mKnotPointModel->setVisible(false);
        mKnotPointModelData = mTypeToModelData.value(mKnotPointModel->type(), nullptr);
    }

    qInfo() << Q_FUNC_INFO << "Initializing PathShader...";
    mPathShader = new PathShader;

    if (!mPathShader->init()) {
        qWarning() << Q_FUNC_INFO << "PathShader could not be initialized.";
    }

    mPathTicks = new Ticks(0, 1, 500);
    mPathTicks->create();

    qInfo() << Q_FUNC_INFO << "Initializing PipeShader...";
    mPipeShader = new PipeShader;

    if (!mPipeShader->init()) {
        qWarning() << Q_FUNC_INFO << "PipeShader could not be initialized.";
    }

    mPipeTicks = new Ticks(0, 1, 500);
    mPipeTicks->create();

    return true;
}

void RendererManager::render(float ifps) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mCamera = mCameraManager->activeCamera();
    mLight = mLightManager->activeLight();

    renderModels(ifps);
    renderKnotPoints(ifps);
    renderPaths(ifps);
    renderPipes(ifps);
}

void RendererManager::renderModels(float ifps) {
    mBasicShader->bind();

    if (mCamera) {
        mBasicShader->setUniformValue("projection_matrix", mCamera->projection());
        mBasicShader->setUniformValue("view_matrix", mCamera->transformation());
        mBasicShader->setUniformValue("camera_position", mCamera->position());
    }

    if (mLight) {
        mBasicShader->setUniformValue("light.position", mLight->position());
        mBasicShader->setUniformValue("light.color", mLight->color());
        mBasicShader->setUniformValue("light.ambient", mLight->ambient());
        mBasicShader->setUniformValue("light.diffuse", mLight->diffuse());
        mBasicShader->setUniformValue("light.specular", mLight->specular());
    }

    QList<Model *> models = mModelManager->models();

    for (Model *model : qAsConst(models)) {
        if (!model->visible())
            continue;

        ModelData *data = mTypeToModelData.value(model->type(), nullptr);

        if (data) {
            data->bind();

            mBasicShader->setUniformValue("node.transformation", model->transformation());
            mBasicShader->setUniformValue("node.color", model->material().color());
            mBasicShader->setUniformValue("node.ambient", model->material().ambient());
            mBasicShader->setUniformValue("node.diffuse", model->material().diffuse());
            mBasicShader->setUniformValue("node.specular", model->material().specular());
            mBasicShader->setUniformValue("node.shininess", model->material().shininess());

            // Draw
            glDrawArrays(GL_TRIANGLES, 0, data->count());

            data->release();
        }
    }

    mBasicShader->release();
}

void RendererManager::renderKnotPoints(float ifps) {
    mBasicShader->bind();
    mKnotPointModelData->bind();

    if (mCamera) {
        mBasicShader->setUniformValue("projection_matrix", mCamera->projection());
        mBasicShader->setUniformValue("view_matrix", mCamera->transformation());
        mBasicShader->setUniformValue("camera_position", mCamera->position());
    }

    if (mLight) {
        mBasicShader->setUniformValue("light.position", mLight->position());
        mBasicShader->setUniformValue("light.color", mLight->color());
        mBasicShader->setUniformValue("light.ambient", mLight->ambient());
        mBasicShader->setUniformValue("light.diffuse", mLight->diffuse());
        mBasicShader->setUniformValue("light.specular", mLight->specular());
    }

    if (mSelectedCurve) {
        mBasicShader->setUniformValue("node.ambient", mKnotPointModel->material().ambient());
        mBasicShader->setUniformValue("node.diffuse", mKnotPointModel->material().diffuse());
        mBasicShader->setUniformValue("node.specular", mKnotPointModel->material().specular());
        mBasicShader->setUniformValue("node.shininess", mKnotPointModel->material().shininess());

        for (auto &point : qAsConst(mSelectedCurve->knotPoints())) {
            mKnotPointModel->setPosition(point->position());
            mKnotPointModel->material().setColor(point->selected() ? QVector4D(1, 1, 0, 1) : QVector4D(0, 1, 0, 1));

            mBasicShader->setUniformValue("node.color", mKnotPointModel->material().color());
            mBasicShader->setUniformValue("node.transformation", mKnotPointModel->transformation());

            glDrawArrays(GL_TRIANGLES, 0, mKnotPointModelData->count());
        }
    }

    mKnotPointModelData->release();
    mBasicShader->release();
}

void RendererManager::renderPaths(float ifps) {
    mPathShader->bind();
    mPathTicks->bind();

    if (mCamera) {
        mPathShader->setUniformValue("projection_matrix", mCamera->projection());
        mPathShader->setUniformValue("view_matrix", mCamera->transformation());
    }

    mPathShader->setUniformValue("color", QVector4D(1, 0, 0, 1));

    for (auto &curve : qAsConst(mCurveManager->curves())) {
        if (curve) {
            QList<Bezier *> patches = curve->bezierPatches();
            for (auto &patch : qAsConst(patches)) {
                auto controlPointPositions = patch->getControlPointPositions();

                mPathShader->setUniformValue("control_points_count", controlPointPositions.size());
                mPathShader->setUniformValueArray("control_points", controlPointPositions);

                glDrawArrays(GL_LINE_STRIP, 0, mPathTicks->size());
            }
        }
    }

    mPathTicks->release();
    mPathShader->release();
}

void RendererManager::renderPipes(float ifps) {
    // Pipe
    mPipeShader->bind();
    mPipeTicks->bind();

    if (mCamera) {
        mPipeShader->setUniformValue("projection_matrix", mCamera->projection());
        mPipeShader->setUniformValue("view_matrix", mCamera->transformation());
        mPipeShader->setUniformValue("camera_position", mCamera->position());
    }

    if (mLight) {
        mPipeShader->setUniformValue("light.position", mLight->position());
        mPipeShader->setUniformValue("light.color", mLight->color());
        mPipeShader->setUniformValue("light.ambient", mLight->ambient());
        mPipeShader->setUniformValue("light.diffuse", mLight->diffuse());
        mPipeShader->setUniformValue("light.specular", mLight->specular());
    }

    mPipeShader->setUniformValue("dt", mPipeTicks->ticksDelta());

    for (auto &curve : qAsConst(mCurveManager->curves())) {
        if (curve) {
            QList<Bezier *> patches = curve->bezierPatches();

            for (auto &patch : patches) {
                auto controlPointPositions = patch->getControlPointPositions();

                mPipeShader->setUniformValue("control_points_count", controlPointPositions.size());
                mPipeShader->setUniformValueArray("control_points", controlPointPositions);

                mPipeShader->setUniformValue("node.color", curve->material().color());
                mPipeShader->setUniformValue("node.ambient", curve->material().ambient());
                mPipeShader->setUniformValue("node.diffuse", curve->material().diffuse());
                mPipeShader->setUniformValue("node.specular", curve->material().specular());
                mPipeShader->setUniformValue("node.shininess", curve->material().shininess());

                int n = curve->sectorCount();
                float r = curve->radius();

                for (int i = 0; i < n; i++) {
                    float sectorAngle0 = 2 * float(i) / n * M_PI;
                    float sectorAngle1 = 2 * float(i + 1) / n * M_PI;

                    mPipeShader->setUniformValue("r", r);
                    mPipeShader->setUniformValue("sector_angle_0", sectorAngle0);
                    mPipeShader->setUniformValue("sector_angle_1", sectorAngle1);

                    glDrawArrays(GL_POINTS, 0, mPipeTicks->size());
                }
            }
        }
    }

    mPipeTicks->release();
    mPipeShader->release();
}
