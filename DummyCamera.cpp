#include "DummyCamera.h"

DummyCamera::DummyCamera(QObject *parent)
    : Camera(parent)
{}

DummyCamera::~DummyCamera() {}

void DummyCamera::keyPressed(QKeyEvent *) {}

void DummyCamera::keyReleased(QKeyEvent *) {}

void DummyCamera::mousePressed(QMouseEvent *) {}

void DummyCamera::mouseReleased(QMouseEvent *) {}

void DummyCamera::mouseMoved(QMouseEvent *) {}

void DummyCamera::update(float) {}
