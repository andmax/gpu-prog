CONFIG       -= console

INCLUDEPATH   = include/

MOC_DIR       = build/

OBJECTS_DIR   = build/

HEADERS       = include/glwidget.h \
                include/glapplication.h \
                include/arcball.h \
                include/rply.h \
                include/texcache.h \
                include/bbox3d.h \
                include/zoom.h \
                include/chronos.h \
                include/model.h

SOURCES       = src/glwidget.cpp \
                src/glapplication.cpp \
                src/viewer.cpp \
                src/chronos.cpp \
                src/rply.c \
                src/model.cpp

QT           += opengl

!macx {
    SOURCES += src/GLee.c
    HEADERS += include/GLee.h
    DEFINES += NEEDGLEE
}
