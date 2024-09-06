QT += widgets
requires(qtConfig(combobox))

HEADERS     = mainwindow.h
SOURCES     = main.cpp \
              mainwindow.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin

!isEmpty(target.path): INSTALLS += target
