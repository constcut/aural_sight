QT += quick multimedia
CONFIG += c++17
#CONFIG += no_keywords #was used only for python interpreter
DEFINES += QT_DEPRECATED_WARNINGS
QMAKE_CXXFLAGS += -std=c++17

SOURCES += \
    audio/audiospeaker.cpp \
    audio/fft.cpp \
    audio/waveanalys.cpp \
    main.cpp \
    app/loghandler.cpp \
    midi/MidiFile.cpp \
    midi/MidiMessage.cpp \
    midi/MidiTrack.cpp \
    midi/NBytesInt.cpp \
    music/graphicmap.cpp \
    music/midiengine.cpp \
    music/midifile.cpp \
    music/midirender.cpp \
    libs/sf/tsf.cpp \
    app/init.cpp

RESOURCES += qml.qrc \
    fonts.qrc \
    soundfonts.qrc

windows:DEFINES += __WINDOWS_MM__
windows:LIBS += -lWINMM

QML_IMPORT_PATH =
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


HEADERS += \
    app/loghandler.h \
    audio/audiospeaker.h \
    audio/fft.h \
    audio/waveanalys.h \
    libs/stft/HannWindow.h \
    libs/stft/Ooura_FFT.h \
    libs/stft/PostProcessor.h \
    libs/stft/STFT.h \
    libs/stft/WAV.h \
    log.hpp \
    midi/MidiFile.hpp \
    midi/MidiMessage.hpp \
    midi/MidiTrack.hpp \
    midi/NBytesInt.hpp \
    music/graphicmap.h \
    music/midiengine.h \
    music/midifile.h \
    music/midirender.h \
    libs/sf/tml.h \
    libs/sf/tsf.h \
    app/init.h


DISTFILES += \
    ConsoleLog.qml \
    OpenTab.qml \
    TODO \
    Tablature.qml \
    PianoMap.qml \
    main.qml \
    math/cutoffs \
    current \
    qml/ConsoleLog.qml \
    qml/PianoMap.qml \
    qml/main.qml

