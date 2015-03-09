QT += widgets
#qtHaveModule(printsupport): QT += printsupport

HEADERS       = imageviewer.h \
    ibf.h \
    canvas.h
SOURCES       = imageviewer.cpp \
                main.cpp \
    ibf.cpp \
    canvas.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/imageviewer
INSTALLS += target


wince*: {
   DEPLOYMENT_PLUGIN += qjpeg qgif
}

RESOURCES += \
    resources.qrc

DISTFILES += \
    images/Action Undo.png
