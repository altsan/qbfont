######################################################################
# Automatically generated by qmake (2.01a) Thu Jul 20 08:15:03 2023
######################################################################
# CONFIG += debug console
CONFIG += map

TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .
os2:QMAKE_CFLAGS   = -Zomf -march=i686 -Wno-pointer-sign
os2:QMAKE_CXXFLAGS = -Zomf -march=i686 -Wno-unused-local-typedefs -Wno-literal-suffix -D OS2EMX_PLAIN_CHAR

# Input
HEADERS += glypheditor.h glyphstatus.h mainwindow.h qbf_const.h
SOURCES += glypheditor.cpp glyphstatus.cpp main.cpp mainwindow.cpp
# RESOURCES += qe.qrc
os2:HEADERS += os2native.h
os2:SOURCES += os2native.cpp
# os2:RC_FILE = qe.rc
# win32:RC_FILE = qe_win.rc