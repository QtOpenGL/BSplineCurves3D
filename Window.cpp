#include "Window.h"
#include "qmath.h"

#include <imgui.h>
#include <QDateTime>
#include <QKeyEvent>
#include <QtImGui.h>

Window::Window(QWindow *parent)
    : QOpenGLWindow(QOpenGLWindow::UpdateBehavior::NoPartialUpdate, parent)
{
    QSurfaceFormat format;
    format.setMajorVersion(3);
    format.setMinorVersion(3);
    format.setSamples(16);
    format.setSwapInterval(1);
    setFormat(format);

    connect(this, &QOpenGLWindow::frameSwapped, this, [=]() { update(); });

    mLightManager = LightManager::instance();
}

void Window::initializeGL()
{
    initializeOpenGLFunctions();
    QtImGui::initialize(this);

    emit init();

    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    mPreviousTime = mCurrentTime;
}

void Window::resizeGL(int w, int h)
{
    emit resized(w, h);
}

void Window::paintGL()
{
    mActiveLight = mLightManager->activeLight();
    mCurrentTime = QDateTime::currentMSecsSinceEpoch();

    float ifps = (mCurrentTime - mPreviousTime) * 0.001f;

    mPreviousTime = mCurrentTime;

    emit render(ifps);

    mImguiWantCapture = ImGui::GetIO().WantCaptureMouse;
    QtImGui::newFrame();
    ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
    ImGui::Begin("Controls", NULL, ImGuiWindowFlags_MenuBar);

    if (!ImGui::CollapsingHeader("Light")) {
        {
            ImGui::Text("Position:");
            QVector3D position = mActiveLight->position();
            float x = position.x();
            float y = position.y();
            float z = position.z();
            float r = position.length();
            float theta = qRadiansToDegrees(atan2(z, x));
            float phi = qRadiansToDegrees(atan2(y, sqrt(z * z + x * x)));
            ImGui::SliderFloat("r", &r, 1.0f, 100.0f, "%.1f");
            ImGui::SliderFloat("theta", &theta, -179.9f, 179.9f, "%.1f");
            ImGui::SliderFloat("phi", &phi, -89.9f, 89.9f, "%.1f");

            x = r * cos(qDegreesToRadians(phi)) * cos(qDegreesToRadians(theta));
            y = r * sin(qDegreesToRadians(phi));
            z = r * cos(qDegreesToRadians(phi)) * sin(qDegreesToRadians(theta));

            mActiveLight->setPosition(QVector3D(x, y, z));
        }

        {
            ImGui::Text("Shading Parameters:");
            float ambient = mActiveLight->ambient();
            float diffuse = mActiveLight->diffuse();
            float specular = mActiveLight->specular();

            if (ImGui::SliderFloat("Ambient", &ambient, 0.0f, 1.0f, "%.3f")) {
                mActiveLight->setAmbient(ambient);
            }

            if (ImGui::SliderFloat("Diffuse", &diffuse, 0.0f, 1.0f, "%.3f")) {
                mActiveLight->setDiffuse(diffuse);
            }

            if (ImGui::SliderFloat("Specular", &specular, 0.0f, 1.0f, "%.3f")) {
                mActiveLight->setSpecular(specular);
            }

            float color[4] = {mActiveLight->color().x(),
                              mActiveLight->color().y(),
                              mActiveLight->color().z(),
                              mActiveLight->color().w()};

            if (ImGui::ColorEdit4("Color", (float *) &color)) {
                mActiveLight->setColor(QVector4D(color[0], color[1], color[2], color[3]));
            }
        }
    }

    glViewport(0, 0, width(), height());
    ImGui::Render();
    QtImGui::render();
}

void Window::keyPressEvent(QKeyEvent *event)
{
    emit keyPressed(event);
}

void Window::keyReleaseEvent(QKeyEvent *event)
{
    emit keyReleased(event);
}

void Window::mousePressEvent(QMouseEvent *event)
{
    emit mousePressed(event);
}

void Window::mouseReleaseEvent(QMouseEvent *event)
{
    emit mouseReleased(event);
}

void Window::mouseMoveEvent(QMouseEvent *event)
{
    emit mouseMoved(event);
}

void Window::wheelEvent(QWheelEvent *event)
{
    emit wheelMoved(event);
}

bool Window::imguiWantCapture() const
{
    return mImguiWantCapture;
}
