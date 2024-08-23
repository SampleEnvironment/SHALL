include(gtest_dependency.pri)


QT       += core gui network widgets concurrent
TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += thread


INCLUDEPATH  += ../SHALL-ServerLib
INCLUDEPATH  += ../SHALL-VariantLib




DEFINES += NO_EXPORT

# uses different library names for some architectures
equals(QMAKE_TARGET.arch,x86_64)|equals(QMAKE_TARGET.arch,amd64)|equals(QMAKE_TARGET.arch,x64)|equals(QMAKE_TARGET.arch,ppc64)|equals(QMAKE_TARGET.arch,arm64) {
    SERVERLIB  = SHALLserver64
    VARIANTLIB = SHALLvariant64
} else {
 equals(QMAKE_TARGET.arch,x86)|equals(QMAKE_TARGET.arch,ppc)|equals(QMAKE_TARGET.arch,arm) {
    SERVERLIB  = SHALLserver32
    VARIANTLIB = SHALLvariant32
 } else {
  equals(QMAKE_HOST.arch,x86_64)|equals(QMAKE_HOST.arch,amd64)|equals(QMAKE_HOST.arch,x64)|equals(QMAKE_HOST.arch,ppc64)|equals(QMAKE_HOST.arch,arm64) {
    SERVERLIB  = SHALLserver64
    VARIANTLIB = SHALLvariant64
  } else {
   equals(QMAKE_HOST.arch,x86)|equals(QMAKE_HOST.arch,ppc)|equals(QMAKE_HOST.arch,arm) {
    SERVERLIB  = SHALLserver32
    VARIANTLIB = SHALLvariant32
   } else {
    SERVERLIB  = SHALLserver
    VARIANTLIB = SHALLvariant
   }
  }
 }
}

DESTDIR = ../bin
win32-msvc* {
  LIBS += $$quote(-L../lib) -l$${VARIANTLIB}
} else {
  LIBS += $$quote(-L../lib) -l$${VARIANTLIB}
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
#DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    ../SHALL-ServerLib/SECoP-Command.cpp \
    ../SHALL-ServerLib/SECoP-Main.cpp \
    ../SHALL-ServerLib/SECoP-Module.cpp \
    ../SHALL-ServerLib/SECoP-Node.cpp \
    ../SHALL-ServerLib/SECoP-Parameter.cpp \
    ../SHALL-ServerLib/SECoP-Property.cpp \
    ../SHALL-ServerLib/SECoP-Worker.cpp \
    ../SHALL-ServerLib/SECoP-StatusGui.cpp \
    main.cpp \
    test_node.cpp \
    tst_contextid.cpp \
    tst_protocol.cpp

HEADERS       = \
    ../SHALL-ServerLib/SECoP-defines.h \
    ../SHALL-ServerLib/SECoP-exports.h \
    ../SHALL-ServerLib/SECoP-Command.h \
    ../SHALL-ServerLib/SECoP-Main.h \
    ../SHALL-ServerLib/SECoP-Module.h \
    ../SHALL-ServerLib/SECoP-Node.h \
    ../SHALL-ServerLib/SECoP-Parameter.h \
    ../SHALL-ServerLib/SECoP-Property.h \
    ../SHALL-ServerLib/SECoP-Worker.h \
    ../SHALL-ServerLib/SECoP-types.h \
    ../SHALL-ServerLib/SECoP-StatusGui.h \
    ../SHALL-VariantLib/SECoP-Variant.h \
    test_node.h


FORMS += \
    ../SHALL-ServerLib/SECoP-StatusGui.ui


