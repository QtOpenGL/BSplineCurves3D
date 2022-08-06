#include "RendererManager.h"
#include "Bezier.h"
#include "Camera.h"
#include "Light.h"

#include <QtMath>

RendererManager::RendererManager(QObject *parent)
    : QObject(parent)
    , mBasicShader(nullptr)
    , mSelectedCurve(nullptr)
    , mSelectedPoint(nullptr)
{
    mModelManager = ModelManager::instance();
    mCameraManager = CameraManager::instance();
    mLightManager = LightManager::instance();
    mCurveManager = CurveManager::instance();

    connect(mCurveManager, &CurveManager::selectedCurveChanged, this, [=](Curve *selectedCurve) { mSelectedCurve = selectedCurve; });
    connect(mCurveManager, &CurveManager::selectedPointChanged, this, [=](Point *selectedPoint) { mSelectedPoint = selectedPoint; });
}

RendererManager *RendererManager::instance()
{
    static RendererManager instance;
    return &instance;
}

bool RendererManager::init()
{
    initializeOpenGLFunctions();
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glLineWidth(2.5f);

    qInfo() << Q_FUNC_INFO << "Initializing BasicShader...";
    mBasicShader = new BasicShader;

    if (!mBasicShader->init())
    {
        qWarning() << Q_FUNC_INFO << "BasicShader could not be initialized.";
        return false;
    }

    qInfo() << Q_FUNC_INFO << "Loading and creating all models...";

    for (Model::Type type : Model::ALL_MODEL_TYPES)
    {
        ModelData *data = new ModelData(type);
        if (!data->load() || !data->create())
        {
            data->deleteLater();
            continue;
        }

        mTypeToModelData.insert(type, data);
    }

    // FIXME
    // Knot and Control Point Models

    {
        mKnotPointModel = Model::create(Model::Sphere);
        mKnotPointModel->setObjectName("KnotPointModel");
        mKnotPointModel->material().setColor(QVector4D(0, 1, 0, 1));
        mKnotPointModel->setScale(QVector3D(0.0025f, 0.0025f, 0.0025f));
        mKnotPointModel->setVisible(false);
    }

    {
        mControlPointModel = Model::create(Model::Sphere);
        mControlPointModel->setObjectName("ControlPointModel");
        mControlPointModel->material().setColor(QVector4D(0, 1, 0, 1));
        mControlPointModel->setScale(QVector3D(0.0025f, 0.0025f, 0.0025f));
        mControlPointModel->setVisible(false);
    }

    qInfo() << Q_FUNC_INFO << "Initializing PathShader...";
    mPathShader = new PathShader;

    if (!mPathShader->init())
    {
        qWarning() << Q_FUNC_INFO << "PathShader could not be initialized.";
    }

    mPathTicks = new Ticks(0, 1, 500);
    mPathTicks->create();

    qInfo() << Q_FUNC_INFO << "Initializing PipeShader...";
    mPipeShader = new PipeShader;

    if (!mPipeShader->init())
    {
        qWarning() << Q_FUNC_INFO << "PipeShader could not be initialized.";
    }

    mPipeTicks = new Ticks(0, 1, 500);
    mPipeTicks->create();

    return true;
}

void RendererManager::render(float ifps)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderModels(ifps);
    renderCurves(ifps);
}

void RendererManager::renderModels(float ifps)
{
    Camera *camera = mCameraManager->activeCamera();
    Light *light = mLightManager->activeLight();

    mBasicShader->bind();

    if (camera)
    {
        mBasicShader->setUniformValue("projection_matrix", camera->projection());
        mBasicShader->setUniformValue("view_matrix", camera->transformation());
        mBasicShader->setUniformValue("camera_position", camera->position());
    }

    if (light)
    {
        mBasicShader->setUniformValue("light.position", light->position());
        mBasicShader->setUniformValue("light.color", light->color());
        mBasicShader->setUniformValue("light.ambient", light->ambient());
        mBasicShader->setUniformValue("light.diffuse", light->diffuse());
        mBasicShader->setUniformValue("light.specular", light->specular());
    }

    QList<Model *> models = mModelManager->models();

    for (Model *model : qAsConst(models))
    {
        if (!model->visible())
            continue;

        ModelData *data = mTypeToModelData.value(model->type(), nullptr);

        if (data)
        {
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

void RendererManager::renderCurves(float ifps)
{
    Camera *camera = mCameraManager->activeCamera();
    Light *light = mLightManager->activeLight();

    mBasicShader->bind();

    if (camera)
    {
        mBasicShader->setUniformValue("projection_matrix", camera->projection());
        mBasicShader->setUniformValue("view_matrix", camera->transformation());
        mBasicShader->setUniformValue("camera_position", camera->position());
    }

    if (light)
    {
        mBasicShader->setUniformValue("light.position", light->position());
        mBasicShader->setUniformValue("light.color", light->color());
        mBasicShader->setUniformValue("light.ambient", light->ambient());
        mBasicShader->setUniformValue("light.diffuse", light->diffuse());
        mBasicShader->setUniformValue("light.specular", light->specular());
    }

    // Draw Control Point Models
    Bezier *bezier = dynamic_cast<Bezier *>(mSelectedCurve);

    if (bezier)
    {
        mBasicShader->setUniformValue("node.ambient", mControlPointModel->material().ambient());
        mBasicShader->setUniformValue("node.diffuse", mControlPointModel->material().diffuse());
        mBasicShader->setUniformValue("node.specular", mControlPointModel->material().specular());
        mBasicShader->setUniformValue("node.shininess", mControlPointModel->material().shininess());

        ModelData *data = mTypeToModelData.value(mControlPointModel->type(), nullptr);

        for (auto &controlPoint : qAsConst(bezier->controlPoints()))
        {
            mControlPointModel->setPosition(controlPoint->position());
            mControlPointModel->material().setColor(controlPoint->selected() ? QVector4D(1, 1, 0, 1) : QVector4D(0, 1, 0, 1));

            mBasicShader->setUniformValue("node.color", mControlPointModel->material().color());
            mBasicShader->setUniformValue("node.transformation", mControlPointModel->transformation());

            data->bind();

            // Draw Control Point Model
            glDrawArrays(GL_TRIANGLES, 0, data->count());

            data->release();
        }
    }

    mBasicShader->release();

    // Path
    mPathShader->bind();

    if (camera)
    {
        mPathShader->setUniformValue("projection_matrix", camera->projection());
        mPathShader->setUniformValue("view_matrix", camera->transformation());
    }

    mPathShader->setUniformValue("color", QVector4D(1, 0, 0, 1));

    for (auto &curve : qAsConst(mCurveManager->curves()))
    {
        Bezier *bezier = dynamic_cast<Bezier *>(curve);

        if (bezier)
        {
            auto controlPointPositions = bezier->getControlPointPositions();

            mPathShader->setUniformValue("control_points_count", controlPointPositions.size());
            mPathShader->setUniformValueArray("control_points", controlPointPositions);

            mPathTicks->bind();
            glDrawArrays(GL_LINE_STRIP, 0, mPathTicks->size());
            mPathTicks->release();
        }
    }

    mPathShader->release();

    // Pipe
    mPipeShader->bind();
    mPipeTicks->bind();

    if (camera)
    {
        mPipeShader->setUniformValue("projection_matrix", camera->projection());
        mPipeShader->setUniformValue("view_matrix", camera->transformation());
        mPipeShader->setUniformValue("camera_position", camera->position());
    }

    if (light)
    {
        mPipeShader->setUniformValue("light.position", light->position());
        mPipeShader->setUniformValue("light.color", light->color());
        mPipeShader->setUniformValue("light.ambient", light->ambient());
        mPipeShader->setUniformValue("light.diffuse", light->diffuse());
        mPipeShader->setUniformValue("light.specular", light->specular());
    }

    mPipeShader->setUniformValue("dt", mPipeTicks->ticksDelta());
    mPipeShader->setUniformValue("r", 0.125f);

    for (auto &curve : qAsConst(mCurveManager->curves()))
    {
        Bezier *bezier = dynamic_cast<Bezier *>(curve);

        if (bezier)
        {
            auto controlPointPositions = bezier->getControlPointPositions();

            mPipeShader->setUniformValue("control_points_count", controlPointPositions.size());
            mPipeShader->setUniformValueArray("control_points", controlPointPositions);

            mPipeShader->setUniformValue("node.color", bezier->material().color());
            mPipeShader->setUniformValue("node.ambient", bezier->material().ambient());
            mPipeShader->setUniformValue("node.diffuse", bezier->material().diffuse());
            mPipeShader->setUniformValue("node.specular", bezier->material().specular());
            mPipeShader->setUniformValue("node.shininess", bezier->material().shininess());

            int n = 128;
            for (int i = 0; i < n; i++)
            {
                float sectorAngle0 = 2 * float(i) / n * M_PI;
                float sectorAngle1 = 2 * float(i + 1) / n * M_PI;

                mPipeShader->setUniformValue("sector_angle_0", sectorAngle0);
                mPipeShader->setUniformValue("sector_angle_1", sectorAngle1);

                glDrawArrays(GL_POINTS, 0, mPipeTicks->size());
            }
        }
    }

    mPipeTicks->release();
    mPipeShader->release();
}
