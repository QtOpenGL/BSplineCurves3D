#ifndef RENDERERMANAGER_H
#define RENDERERMANAGER_H

#include "BasicShader.h"
#include "CameraManager.h"
#include "CurveManager.h"
#include "Light.h"
#include "LightManager.h"
#include "ModelData.h"
#include "ModelManager.h"
#include "PathShader.h"
#include "PipeShader.h"
#include "Ticks.h"

#include <QMap>
#include <QMatrix4x4>
#include <QObject>

class RendererManager : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT

private:
    explicit RendererManager(QObject *parent = nullptr);

public:
    static RendererManager *instance();

    bool init();
    void render(float ifps);

private slots:
    void renderModels(float ifps);
    void renderKnotPoints(float ifps);
    void renderPaths(float ifps);
    void renderPipes(float ifps);

private:
    QMap<Model::Type, ModelData *> mTypeToModelData;

    BasicShader *mBasicShader;
    PathShader *mPathShader;
    PipeShader *mPipeShader;

    Ticks *mPathTicks;
    Ticks *mPipeTicks;

    ModelManager *mModelManager;
    CameraManager *mCameraManager;
    LightManager *mLightManager;
    CurveManager *mCurveManager;

    Spline *mSelectedCurve;
    KnotPoint *mSelectedKnotPoint;

    Camera *mCamera;
    Light *mLight;

    Model *mKnotPointModel;
    ModelData *mKnotPointModelData;
};

#endif // RENDERERMANAGER_H
