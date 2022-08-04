#include "DummyCamera.h"
#include "CameraManager.h"

DummyCamera::DummyCamera(QObject *parent)
    : Camera(parent)
{}

DummyCamera::~DummyCamera() {}

DummyCamera *DummyCamera::create()
{
    DummyCamera *camera = new DummyCamera;
    CAMERA_MANAGER->addCamera(camera);
    return camera;
}

void DummyCamera::remove()
{
    CAMERA_MANAGER->removeCamera(this);
}

CameraManager *DummyCamera::CAMERA_MANAGER = CameraManager::instance();
