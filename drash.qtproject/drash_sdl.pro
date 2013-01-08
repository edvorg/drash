TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../3rd/Box2D/include

LIBS += -L../3rd/Box2D/lib -lBox2D

unix:!macx: LIBS += -lrt -lGL -lSDL -lGLEW -lSDL_image

CONFIG(debug, debug|release) {
    DEFINES += DRASH_DEBUG
    QMAKE_CXXFLAGS += -O0
}

CONFIG(release, debug|release) {
    DEFINES += DRASH_RELEASE
    QMAKE_CXXFLAGS += -O2
}

SOURCES += \
    ../src/ui/uiwidget.cpp \
    ../src/ui/uisystem.cpp \
    ../src/ui/uislider.cpp \
    ../src/ui/uicontrol.cpp \
    ../src/ui/uibutton.cpp \
    ../src/test/test5.cpp \
    ../src/test/test4.cpp \
    ../src/test/test3.cpp \
    ../src/test/test2.cpp \
    ../src/test/test1.cpp \
    ../src/test/test.cpp \
    ../src/templates/templatesystem.cpp \
    ../src/players/playerssystem.cpp \
    ../src/explosion/explosionsystem.cpp \
    ../src/scene/sceneobject.cpp \
    ../src/scene/scene.cpp \
    ../src/players/player.cpp \
    ../src/scene/physobserver.cpp \
    ../src/scene/figure.cpp \
    ../src/misc/segment2.cpp \
    ../src/misc/ray.cpp \
    ../src/misc/plane.cpp \
    ../src/misc/matrix4.cpp \
    ../src/misc/graphics.cpp \
    ../src/greng/renderer.cpp \
    ../src/greng/meshmanager.cpp \
    ../src/diag/timer.cpp \
    ../src/diag/logger.cpp \
    ../src/debugrenderer/debugdrawsystem.cpp \
    ../src/greng/camera.cpp \
    ../src/app/appeventsystem.cpp \
    ../src/app/appeventprocessor.cpp \
    ../src/app/appeventcombination.cpp \
    ../src/app/appevent.cpp \
    ../src/app/app.cpp \
    ../src/sdl/main.cpp \
    ../src/greng/loadmeshobj.cpp \
    ../src/greng/texturemanager.cpp \
    ../src/greng/vertexshadermanager.cpp \
    ../src/greng/fragmentshadermanager.cpp \
    ../src/greng/shaderprogrammanager.cpp \
    ../src/test/test6.cpp \
    ../src/misc/vec2.cpp \
    ../src/scene/joint.cpp \
    ../src/test/test7.cpp \
    ../src/greng/cameramanager.cpp \
    ../src/debugrenderer/debugrenderer.cpp

HEADERS += \
    ../src/ui/uiwidget.h \
    ../src/ui/uisystem.h \
    ../src/ui/uislider.h \
    ../src/ui/uicontrol.h \
    ../src/ui/uibutton.h \
    ../src/test/test5.h \
    ../src/test/test4.h \
    ../src/test/test3.h \
    ../src/test/test2.h \
    ../src/test/test1.h \
    ../src/test/test.h \
    ../src/templates/templatesystem.h \
    ../src/players/playerssystem.h \
    ../src/explosion/explosionsystem.h \
    ../src/scene/sceneobject.h \
    ../src/scene/scene.h \
    ../src/players/player.h \
    ../src/scene/physobserver.h \
    ../src/scene/figure.h \
    ../src/explosion/explosion.h \
    ../src/misc/vec4.h \
    ../src/misc/vec3.h \
    ../src/misc/vec2.h \
    ../src/misc/segment2.h \
    ../src/misc/ray.h \
    ../src/misc/plane.h \
    ../src/misc/matrix4.h \
    ../src/misc/math.h \
    ../src/misc/graphics.h \
    ../src/misc/color4.h \
    ../src/misc/color3.h \
    ../src/greng/vertex.h \
    ../src/greng/renderer.h \
    ../src/greng/meshmanager.h \
    ../src/greng/mesh.h \
    ../src/diag/timer.h \
    ../src/diag/logger.h \
    ../src/diag/assert.h \
    ../src/debugrenderer/debugdrawsystem.h \
    ../src/greng/camera.h \
    ../src/app/appeventsystem.h \
    ../src/app/appeventprocessor.h \
    ../src/app/appeventcombination.h \
    ../src/app/appevent.h \
    ../src/app/app.h \
    ../src/greng/loadmeshobj.h \
    ../src/greng/texturemanager.h \
    ../src/greng/texture.h \
    ../src/misc/objectfactory.h \
    ../src/greng/vertexshadermanager.h \
    ../src/greng/vertexshader.h \
    ../src/greng/fragmentshader.h \
    ../src/greng/fragmentshadermanager.h \
    ../src/greng/shaderprogram.h \
    ../src/greng/shaderprogrammanager.h \
    ../src/greng/pointlight.h \
    ../src/test/test6.h \
    ../src/misc/animator.h \
    ../src/scene/joint.h \
    ../src/test/test7.h \
    ../src/greng/cameramanager.h \
    ../src/debugrenderer/debugrenderer.h
