QT += widgets

HEADERS       = digitalclock.h

SOURCES       = digitalclock.cpp \
                main.cpp


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin

!isEmpty(target.path): INSTALLS += target
