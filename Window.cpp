#include "Window.h"
#include "Controller.h"
#include "qmath.h"

#include <imgui.h>
#include <QDateTime>
#include <QKeyEvent>
#include <QtImGui.h>

Window::Window(QWindow *parent)
    : QOpenGLWindow(QOpenGLWindow::UpdateBehavior::NoPartialUpdate, parent)
    , mMode(Mode::Select)
{
    QSurfaceFormat format;
    format.setMajorVersion(3);
    format.setMinorVersion(3);
    format.setSamples(16);
    format.setSwapInterval(1);
    setFormat(format);

    connect(this, &QOpenGLWindow::frameSwapped, this, [=]() { update(); });
}

void Window::initializeGL()
{
    initializeOpenGLFunctions();

    QtImGui::initialize(this);

    mController = new Controller;
    mController->setWindow(this);
    mController->init();

    mRendererManager = RendererManager::instance();
    mCurveManager = CurveManager::instance();
    mLightManager = LightManager::instance();

    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    mPreviousTime = mCurrentTime;
}

void Window::resizeGL(int w, int h)
{
    mController->resize(w, h);
}

void Window::paintGL()
{
    mActiveLight = mLightManager->activeLight();
    mRenderPaths = mRendererManager->getRenderPaths();
    mRenderPipes = mRendererManager->getRenderPipes();
    mGlobalPipeRadius = mCurveManager->globalPipeRadius();
    mGlobalPipeSectorCount = mCurveManager->globalPipeSectorCount();
    mSelectedCurve = mCurveManager->selectedCurve();
    mSelectedKnotPoint = mCurveManager->selectedKnotPoint();

    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    float ifps = (mCurrentTime - mPreviousTime) * 0.001f;
    mPreviousTime = mCurrentTime;

    mController->render(ifps);

    mImguiWantCapture = ImGui::GetIO().WantCaptureMouse;

    QtImGui::newFrame();
    ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
    ImGui::Begin("Controls", NULL, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Import"))
                mController->onAction(Action::ShowImportWindow);

            if (ImGui::MenuItem("Export"))
                mController->onAction(Action::ShowExportWindow);

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    // Mode
    if (!ImGui::CollapsingHeader("Mode"))
    {
        int mode = (int) mMode;

        ImGui::RadioButton("Select (Z)", &mode, 0);
        ImGui::RadioButton("Add Knot (C)", &mode, 1);

        if (mode != (int) mMode)
        {
            mMode = (Mode) mode;
            mController->onAction(Action::UpdateMode, mode);
        }
    }

    // Render Settings
    if (!ImGui::CollapsingHeader("Render Settings"))
    {
        if (ImGui::SliderFloat("Pipe Radius (Global)", &mGlobalPipeRadius, 0.001f, 1.0f, "%.3f"))
            mController->onAction(Action::UpdateGlobalPipeRadius, mGlobalPipeRadius);

        if (ImGui::SliderInt("Pipe Sector Count (Global)", &mGlobalPipeSectorCount, 3, 192))
            mController->onAction(Action::UpdateGlobalPipeSectorCount, mGlobalPipeSectorCount);

        if (ImGui::Checkbox("Render Paths", &mRenderPaths))
            mController->onAction(Action::UpdateRenderPaths, mRenderPaths);

        ImGui::SameLine();

        if (ImGui::Checkbox("Render Pipes", &mRenderPipes))
            mController->onAction(Action::UpdateRenderPipes, mRenderPipes);
    }

    // Light
    if (!ImGui::CollapsingHeader("Light"))
    {
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

            if (ImGui::SliderFloat("Ambient", &ambient, 0.0f, 1.0f, "%.3f"))
                mActiveLight->setAmbient(ambient);

            if (ImGui::SliderFloat("Diffuse", &diffuse, 0.0f, 1.0f, "%.3f"))
                mActiveLight->setDiffuse(diffuse);

            if (ImGui::SliderFloat("Specular", &specular, 0.0f, 1.0f, "%.3f"))
                mActiveLight->setSpecular(specular);

            float color[4] = {mActiveLight->color().x(), mActiveLight->color().y(), mActiveLight->color().z(), mActiveLight->color().w()};

            if (ImGui::ColorEdit4("Color##Light", (float *) &color))
                mActiveLight->setColor(QVector4D(color[0], color[1], color[2], color[3]));
        }
    }

    // Curve
    if (!ImGui::CollapsingHeader("Curve"))
    {
        int n = mSelectedCurve ? mSelectedCurve->knotPoints().size() : 0;
        float length = mSelectedCurve ? mSelectedCurve->length() : 0.0f;
        float radius = mSelectedCurve ? mSelectedCurve->radius() : 0.0f;
        int sectorCount = mSelectedCurve ? mSelectedCurve->sectorCount() : 0;

        ImGui::BeginDisabled(!mSelectedCurve);

        ImGui::Text("Knots: %d", n);
        ImGui::Text("Length: %.2f", length);

        if (ImGui::SliderFloat("Pipe Radius", &radius, 0.001f, 1.0f, "%.3f"))
            mController->onAction(Action::UpdateSelectedCurvePipeRadius, radius);

        if (ImGui::SliderInt("Pipe Sector Count", &sectorCount, 3, 192))
            mController->onAction(Action::UpdateSelectedCurvePipeSectorCount, sectorCount);

        ImGui::Text("Shading Parameters:");
        float ambient = mSelectedCurve ? mSelectedCurve->material().ambient() : 0.0f;
        float diffuse = mSelectedCurve ? mSelectedCurve->material().diffuse() : 0.0f;
        float specular = mSelectedCurve ? mSelectedCurve->material().specular() : 0.0f;
        float x = mSelectedCurve ? mSelectedCurve->material().color().x() : 0.0f;
        float y = mSelectedCurve ? mSelectedCurve->material().color().y() : 0.0f;
        float z = mSelectedCurve ? mSelectedCurve->material().color().z() : 0.0f;
        float w = mSelectedCurve ? mSelectedCurve->material().color().w() : 0.0f;
        float color[4] = {x, y, z, w};

        ImGui::SliderFloat("Ambient##Curve", &ambient, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Diffuse##Curve", &diffuse, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("Specular##Curve", &specular, 0.0f, 1.0f, "%.3f");
        ImGui::ColorEdit4("Color##Curve", (float *) &color);

        if (mSelectedCurve)
        {
            Material material;
            material.setAmbient(ambient);
            material.setDiffuse(diffuse);
            material.setSpecular(specular);
            material.setColor(QVector4D(color[0], color[1], color[2], color[3]));
            mSelectedCurve->setMaterial(material);
        }

        if (ImGui::Button("Remove Curve"))
            mController->onAction(Action::RemoveSelectedCurve);

        ImGui::EndDisabled();
    }

    // Knot
    if (!ImGui::CollapsingHeader("Knot"))
    {
        float x = mSelectedKnotPoint ? mSelectedKnotPoint->position().x() : 0;
        float y = mSelectedKnotPoint ? mSelectedKnotPoint->position().y() : 0;
        float z = mSelectedKnotPoint ? mSelectedKnotPoint->position().z() : 0;
        bool update = false;

        ImGui::BeginDisabled(!mSelectedKnotPoint);

        ImGui::Text("Position:");

        if (ImGui::DragFloat("x##Knot", &x, 0.001f, -1000, 1000, "%.3f"))
            update = true;

        if (ImGui::DragFloat("y##Knot", &y, 0.001f, -1000, 1000, "%.3f"))
            update = true;

        if (ImGui::DragFloat("z##Knot", &z, 0.001f, -1000, 1000, "%.3f"))
            update = true;

        if (update)
        {
            QList<QVariant> list;
            list << x;
            list << y;
            list << z;
            mController->onAction(Action::UpdateKnotPointPositionFromGui, list);
        }

        if (ImGui::Button("Remove Knot"))
            mController->onAction(Action::RemoveSelectedKnotPoint);

        ImGui::EndDisabled();
    }

    ImGui::Spacing();

    if (ImGui::Button("Clear Scene"))
        mController->onAction(Action::ClearScene);

    ImGui::Spacing();

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    glViewport(0, 0, width(), height());
    ImGui::Render();
    QtImGui::render();
}

void Window::keyPressEvent(QKeyEvent *event)
{
    mController->keyPressed(event);
}

void Window::keyReleaseEvent(QKeyEvent *event)
{
    mController->keyReleased(event);
}

void Window::mousePressEvent(QMouseEvent *event)
{
    mController->mousePressed(event);
}

void Window::mouseReleaseEvent(QMouseEvent *event)
{
    mController->mouseReleased(event);
}

void Window::mouseMoveEvent(QMouseEvent *event)
{
    mController->mouseMoved(event);
}

void Window::wheelEvent(QWheelEvent *event)
{
    mController->wheelMoved(event);
}

void Window::mouseDoubleClickEvent(QMouseEvent *event)
{
    mController->mouseDoubleClicked(event);
}

bool Window::imguiWantCapture() const
{
    return mImguiWantCapture;
}

void Window::onModeChanged(Mode newMode)
{
    mMode = newMode;
}
