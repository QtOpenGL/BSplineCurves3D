QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    BasicShader.cpp \
    Bezier.cpp \
    Camera.cpp \
    CameraManager.cpp \
    Controller.cpp \
    Curve.cpp \
    CurveManager.cpp \
    DummyCamera.cpp \
    FreeCamera.cpp \
    Helper.cpp \
    Light.cpp \
    LightManager.cpp \
    Main.cpp \
    Material.cpp \
    Model.cpp \
    ModelData.cpp \
    ModelManager.cpp \
    Node.cpp \
    PathShader.cpp \
    PipeShader.cpp \
    Point.cpp \
    RendererManager.cpp \
    Spline.cpp \
    Ticks.cpp \
    Window.cpp

HEADERS += \
    BasicShader.h \
    Bezier.h \
    Camera.h \
    CameraManager.h \
    Controller.h \
    Curve.h \
    CurveManager.h \
    DummyCamera.h \
    FreeCamera.h \
    Helper.h \
    Light.h \
    LightManager.h \
    Material.h \
    Model.h \
    ModelData.h \
    ModelManager.h \
    Node.h \
    PathShader.h \
    PipeShader.h \
    Point.h \
    RendererManager.h \
    Spline.h \
    Ticks.h \
    Window.h

include(Dependencies/qtimgui/qtimgui.pri)

INCLUDEPATH += $$PWD/Dependencies/Eigen/

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources.qrc
