#include "DummyCamera.h"

DummyCamera::DummyCamera(QObject *parent)
    : Camera(parent)
{}

DummyCamera::~DummyCamera() {}

void DummyCamera::onKeyPressed(QKeyEvent *) {}

void DummyCamera::onKeyReleased(QKeyEvent *) {}

void DummyCamera::onMousePressed(QMouseEvent *) {}

void DummyCamera::onMouseReleased(QMouseEvent *) {}

void DummyCamera::onMouseMoved(QMouseEvent *) {}

void DummyCamera::update(float) {}
