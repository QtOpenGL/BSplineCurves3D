#include "RendererManager.h"
#include "Bezier.h"
#include "Camera.h"
#include "Light.h"

#include <QtMath>

RendererManager::RendererManager(QObject *parent)
    : QObject(parent)
    , mSelectedCurve(nullptr)
    , mSelectedKnotPoint(nullptr)
    , mRenderPaths(true)
    , mRenderPipes(true) {
    mModelManager = ModelManager::instance();
    mCameraManager = CameraManager::instance();
    mLightManager = LightManager::instance();
    mCurveManager = CurveManager::instance();
    mShaderManager = ShaderManager::instance();

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

    qInfo() << Q_FUNC_INFO << "Initializing ShaderManager...";

    if (!mShaderManager->init()) {
        qWarning() << Q_FUNC_INFO << "ShaderManager could not be initialized.";
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
        mKnotPointModel->setScale(QVector3D(0.005f, 0.005f, 0.005f));
        mKnotPointModel->setVisible(false);
        mKnotPointModelData = mTypeToModelData.value(mKnotPointModel->type(), nullptr);
    }

    mPathTicks = new Ticks(0, 1, 500);
    mPathTicks->create();

    mPipeTicks = new Ticks(0, 1, 500);
    mPipeTicks->create();

    return true;
}

void RendererManager::render(float ifps) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mCamera = mCameraManager->activeCamera();
    mLight = mLightManager->activeLight();

    renderModels(ifps);

    if (mRenderPaths)
        renderPaths(ifps);

    if (mRenderPipes)
        renderPipes(ifps);

    if (mRenderPaths || mRenderPipes)
        renderKnotPoints(ifps);
}

void RendererManager::renderModels(float ifps) {
    mShaderManager->bind(ShaderManager::Shader::Basic);

    if (mCamera) {
        mShaderManager->setUniformValue(ShaderManager::Shader::Basic, "projection_matrix", mCamera->projection());
        mShaderManager->setUniformValue(ShaderManager::Shader::Basic, "view_matrix", mCamera->transformation());
        mShaderManager->setUniformValue(ShaderManager::Shader::Basic, "camera_position", mCamera->position());
    }

    if (mLight) {
        mShaderManager->setUniformValue(ShaderManager::Shader::Basic, "light.position", mLight->position());
        mShaderManager->setUniformValue(ShaderManager::Shader::Basic, "light.color", mLight->color());
        mShaderManager->setUniformValue(ShaderManager::Shader::Basic, "light.ambient", mLight->ambient());
        mShaderManager->setUniformValue(ShaderManager::Shader::Basic, "light.diffuse", mLight->diffuse());
        mShaderManager->setUniformValue(ShaderManager::Shader::Basic, "light.specular", mLight->specular());
    }

    QList<Model *> models = mModelManager->models();

    for (Model *model : qAsConst(models)) {
        if (!model->visible())
            continue;

        ModelData *data = mTypeToModelData.value(model->type(), nullptr);

        if (data) {
            data->bind();

            mShaderManager->setUniformValue(ShaderManager::Shader::Basic, "node.transformation", model->transformation());
            mShaderManager->setUniformValue(ShaderManager::Shader::Basic, "node.color", model->material().color());
            mShaderManager->setUniformValue(ShaderManager::Shader::Basic, "node.ambient", model->material().ambient());
            mShaderManager->setUniformValue(ShaderManager::Shader::Basic, "node.diffuse", model->material().diffuse());
            mShaderManager->setUniformValue(ShaderManager::Shader::Basic, "node.specular", model->material().specular());
            mShaderManager->setUniformValue(ShaderManager::Shader::Basic, "node.shininess", model->material().shininess());

            // Draw
            glDrawArrays(GL_TRIANGLES, 0, data->count());

            data->release();
        }
    }

    mShaderManager->release(ShaderManager::Shader::Basic);
}

void RendererManager::renderKnotPoints(float ifps) {
    mShaderManager->bind(ShaderManager::Shader::Basic);
    mKnotPointModelData->bind();

    if (mCamera) {
        mShaderManager->setUniformValue(ShaderManager::Shader::Basic, "projection_matrix", mCamera->projection());
        mShaderManager->setUniformValue(ShaderManager::Shader::Basic, "view_matrix", mCamera->transformation());
        mShaderManager->setUniformValue(ShaderManager::Shader::Basic, "camera_position", mCamera->position());
    }

    if (mLight) {
        mShaderManager->setUniformValue(ShaderManager::Shader::Basic, "light.position", mLight->position());
        mShaderManager->setUniformValue(ShaderManager::Shader::Basic, "light.color", mLight->color());
        mShaderManager->setUniformValue(ShaderManager::Shader::Basic, "light.ambient", mLight->ambient());
        mShaderManager->setUniformValue(ShaderManager::Shader::Basic, "light.diffuse", mLight->diffuse());
        mShaderManager->setUniformValue(ShaderManager::Shader::Basic, "light.specular", mLight->specular());
    }

    if (mSelectedCurve) {
        mShaderManager->setUniformValue(ShaderManager::Shader::Basic, "node.ambient", mKnotPointModel->material().ambient());
        mShaderManager->setUniformValue(ShaderManager::Shader::Basic, "node.diffuse", mKnotPointModel->material().diffuse());
        mShaderManager->setUniformValue(ShaderManager::Shader::Basic, "node.specular", mKnotPointModel->material().specular());
        mShaderManager->setUniformValue(ShaderManager::Shader::Basic, "node.shininess", mKnotPointModel->material().shininess());

        for (auto &point : qAsConst(mSelectedCurve->knotPoints())) {
            mKnotPointModel->setPosition(point->position());
            mKnotPointModel->material().setColor(point->selected() ? QVector4D(1, 1, 0, 1) : QVector4D(0, 1, 0, 1));

            mShaderManager->setUniformValue(ShaderManager::Shader::Basic, "node.color", mKnotPointModel->material().color());
            mShaderManager->setUniformValue(ShaderManager::Shader::Basic, "node.transformation", mKnotPointModel->transformation());

            glDrawArrays(GL_TRIANGLES, 0, mKnotPointModelData->count());
        }
    }

    mKnotPointModelData->release();
    mShaderManager->release(ShaderManager::Shader::Basic);
}

void RendererManager::renderPaths(float ifps) {
    mShaderManager->bind(ShaderManager::Shader::Path);
    mPathTicks->bind();

    if (mCamera) {
        mShaderManager->setUniformValue(ShaderManager::Shader::Path, "projection_matrix", mCamera->projection());
        mShaderManager->setUniformValue(ShaderManager::Shader::Path, "view_matrix", mCamera->transformation());
    }

    mShaderManager->setUniformValue(ShaderManager::Shader::Path, "color", QVector4D(1, 0, 0, 1));

    for (auto &curve : qAsConst(mCurveManager->curves())) {
        if (curve) {
            QList<Bezier *> patches = curve->bezierPatches();
            for (auto &patch : qAsConst(patches)) {
                auto controlPointPositions = patch->getControlPointPositions();

                mShaderManager->setUniformValue(ShaderManager::Shader::Path, "control_points_count", controlPointPositions.size());
                mShaderManager->setUniformValueArray(ShaderManager::Shader::Path, "control_points", controlPointPositions);

                glDrawArrays(GL_LINE_STRIP, 0, mPathTicks->size());
            }
        }
    }

    mPathTicks->release();
    mShaderManager->release(ShaderManager::Shader::Path);
}

void RendererManager::renderPipes(float ifps) {
    // Pipe
    mShaderManager->bind(ShaderManager::Shader::Pipe);
    mPipeTicks->bind();

    if (mCamera) {
        mShaderManager->setUniformValue(ShaderManager::Shader::Pipe, "projection_matrix", mCamera->projection());
        mShaderManager->setUniformValue(ShaderManager::Shader::Pipe, "view_matrix", mCamera->transformation());
        mShaderManager->setUniformValue(ShaderManager::Shader::Pipe, "camera_position", mCamera->position());
    }

    if (mLight) {
        mShaderManager->setUniformValue(ShaderManager::Shader::Pipe, "light.position", mLight->position());
        mShaderManager->setUniformValue(ShaderManager::Shader::Pipe, "light.color", mLight->color());
        mShaderManager->setUniformValue(ShaderManager::Shader::Pipe, "light.ambient", mLight->ambient());
        mShaderManager->setUniformValue(ShaderManager::Shader::Pipe, "light.diffuse", mLight->diffuse());
        mShaderManager->setUniformValue(ShaderManager::Shader::Pipe, "light.specular", mLight->specular());
    }

    mShaderManager->setUniformValue(ShaderManager::Shader::Pipe, "dt", mPipeTicks->ticksDelta());

    for (auto &curve : qAsConst(mCurveManager->curves())) {
        if (curve) {
            QList<Bezier *> patches = curve->bezierPatches();

            for (auto &patch : patches) {
                auto controlPointPositions = patch->getControlPointPositions();

                mShaderManager->setUniformValue(ShaderManager::Shader::Pipe, "control_points_count", controlPointPositions.size());
                mShaderManager->setUniformValueArray(ShaderManager::Shader::Pipe, "control_points", controlPointPositions);

                mShaderManager->setUniformValue(ShaderManager::Shader::Pipe, "node.color", curve->material().color());
                mShaderManager->setUniformValue(ShaderManager::Shader::Pipe, "node.ambient", curve->material().ambient());
                mShaderManager->setUniformValue(ShaderManager::Shader::Pipe, "node.diffuse", curve->material().diffuse());
                mShaderManager->setUniformValue(ShaderManager::Shader::Pipe, "node.specular", curve->material().specular());
                mShaderManager->setUniformValue(ShaderManager::Shader::Pipe, "node.shininess", curve->material().shininess());

                int n = curve->sectorCount();
                float r = curve->radius();

                for (int i = 0; i < n; i++) {
                    float sectorAngle0 = 2 * float(i) / n * M_PI;
                    float sectorAngle1 = 2 * float(i + 1) / n * M_PI;

                    mShaderManager->setUniformValue(ShaderManager::Shader::Pipe, "r", r);
                    mShaderManager->setUniformValue(ShaderManager::Shader::Pipe, "sector_angle_0", sectorAngle0);
                    mShaderManager->setUniformValue(ShaderManager::Shader::Pipe, "sector_angle_1", sectorAngle1);

                    glDrawArrays(GL_POINTS, 0, mPipeTicks->size());
                }
            }
        }
    }

    mPipeTicks->release();
    mShaderManager->release(ShaderManager::Shader::Pipe);
}

void RendererManager::setRenderPipes(bool newRenderPipes) {
    mRenderPipes = newRenderPipes;
}

void RendererManager::setRenderPaths(bool newRenderPaths) {
    mRenderPaths = newRenderPaths;
}

bool RendererManager::getRenderPaths() const {
    return mRenderPaths;
}

bool RendererManager::getRenderPipes() const {
    return mRenderPipes;
}
