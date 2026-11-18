QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT       += printsupport
CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    abstracteffect.cpp \
    abstractinstrument.cpp \
    abstractselection.cpp \
    additionaltools.cpp \
    colorchooser.cpp \
    colorpickerinstrument.cpp \
    curvelineinstrument.cpp \
    datasingleton.cpp \
    ellipseinstrument.cpp \
    eraserinstrument.cpp \
    fillinstrument.cpp \
    grayeffect.cpp \
    imagearea.cpp \
    lineinstrument.cpp \
    magnifierinstrument.cpp \
    main.cpp \
    mainwindow.cpp \
    negativeeffect.cpp \
    palettebar.cpp \
    palettebutton.cpp \
    pencilinstrument.cpp \
    rectangleinstrument.cpp \
    resizedialog.cpp \
    selectioninstrument.cpp \
    settingsdialog.cpp \
    shortcutedit.cpp \
    sprayinstrument.cpp \
    textdialog.cpp \
    textinstrument.cpp \
    toolbar.cpp \
    undocommand.cpp

HEADERS += \
    abstracteffect.h \
    abstractinstrument.h \
    abstractselection.h \
    additionaltools.h \
    colorchooser.h \
    colorpickerinstrument.h \
    curvelineinstrument.h \
    datasingleton.h \
    easypaintenums.h \
    ellipseinstrument.h \
    eraserinstrument.h \
    fillinstrument.h \
    grayeffect.h \
    imagearea.h \
    lineinstrument.h \
    magnifierinstrument.h \
    mainwindow.h \
    negativeeffect.h \
    palettebar.h \
    palettebutton.h \
    pencilinstrument.h \
    rectangleinstrument.h \
    resizedialog.h \
    selectioninstrument.h \
    settingsdialog.h \
    shortcutedit.h \
    sprayinstrument.h \
    textdialog.h \
    textinstrument.h \
    toolbar.h \
    undocommand.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    edit.qrc \
    file.qrc \
    images.qrc \
    instruction.qrc \
    logo.qrc \
    tools.qrc
