#ifndef RENDERERMANAGER_H
#define RENDERERMANAGER_H

#include "BasicShader.h"
#include "CameraManager.h"
#include "CurveManager.h"
#include "LightManager.h"
#include "ModelData.h"
#include "ModelManager.h"
#include "PathShader.h"
#include "Ticks.h"

#include <QMap>
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
    void renderCurves(float ifps);

private:
    QMap<Model::Type, ModelData *> mTypeToModelData;
    BasicShader *mBasicShader;

    PathShader *mPathShader;
    Ticks *mTicks;

    ModelManager *mModelManager;
    CameraManager *mCameraManager;
    LightManager *mLightManager;
    CurveManager *mCurveManager;

    Curve *mSelectedCurve;
    Point *mSelectedPoint;

    // FIXME
    Model *mKnotPointModel;
    Model *mControlPointModel;
};

#endif // RENDERERMANAGER_H
