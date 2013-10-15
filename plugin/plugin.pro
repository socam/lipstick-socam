TEMPLATE = lib
TARGET = lipstickplugin
VERSION = 0.1

CONFIG += qt plugin link_pkgconfig
QT += core gui declarative
PKGCONFIG += qmsystem2

INSTALLS = target qmldirfile
qmldirfile.files = qmldir
qmldirfile.path = $$[QT_INSTALL_IMPORTS]/org/nemomobile/lipstick
target.path = $$[QT_INSTALL_IMPORTS]/org/nemomobile/lipstick

DEPENDPATH += "../src"
INCLUDEPATH += "../src" "../src/utilities" "../src/xtools" /usr/include/meegotouch
LIBS += -L"../src" -llipstick-socam

HEADERS += \
    lipstickplugin.h

SOURCES += \
    lipstickplugin.cpp

OTHER_FILES += \
    qmldir

QMAKE_CXXFLAGS += \
    -Werror \
    -g \
    -std=c++0x \
    -fPIC \
    -fvisibility=hidden \
    -fvisibility-inlines-hidden

QMAKE_LFLAGS += \
    -pie \
    -rdynamic
