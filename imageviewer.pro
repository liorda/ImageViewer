QT += widgets
#qtHaveModule(printsupport): QT += printsupport

HEADERS       = imageviewer.h \
    ibf.h
SOURCES       = imageviewer.cpp \
                main.cpp \
    ibf.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/imageviewer
INSTALLS += target


wince*: {
   DEPLOYMENT_PLUGIN += qjpeg qgif
}

RESOURCES += \
    resources.qrc

DISTFILES +=
