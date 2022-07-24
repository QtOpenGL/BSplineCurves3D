#ifndef RENDERERMANAGER_H
#define RENDERERMANAGER_H

#include "BasicShader.h"
#include "Camera.h"
#include "Light.h"
#include "Model.h"
#include "ModelData.h"
#include "PathShader.h"
#include "Spline.h"
#include "Ticks.h"

#include <QMap>
#include <QObject>

class RendererManager : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit RendererManager(QObject *parent = nullptr);
    ~RendererManager();

    bool init();
    void render();
    void addModel(Model *model);
    bool removeModel(Model *model);

    void addCurve(Spline *curve);

    Light *light();
    void setLight(Light *newLight);

    Camera *camera();
    void setCamera(Camera *newCamera);

private:
    QList<Model *> mModels;
    QList<Spline *> mCurves;
    QMap<Model::Type, ModelData *> mTypeToModelData;
    BasicShader *mBasicShader;
    Light *mLight;
    Camera *mCamera;

    PathShader *mPathShader;
    Ticks *mTicks;

    // FIXME
    Model *mPlane;
    Model *mCube;
    Model *mKnotModel;
};

#endif // RENDERERMANAGER_H
