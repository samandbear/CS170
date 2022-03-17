TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        greedysolver.cpp \
        input.cpp \
        main.cpp \
        naivesolver.cpp \
        output.cpp \
        sasolver.cpp \
        tests.cpp

HEADERS += \
    greedysolver.h \
    input.h \
    naivesolver.h \
    output.h \
    sasolver.h \
    tests.h
