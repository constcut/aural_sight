#include "Tapper.h"

#include <QDebug>

#include "midi/MidiFile.hpp"

using namespace mtherapp;
using namespace std;


void Tapper::pressed(int idx) {
    _events.push_back(TapEvent{idx, true, std::chrono::high_resolution_clock::now()});
}


void Tapper::released(int idx) {
    _events.push_back(TapEvent{idx, false, std::chrono::high_resolution_clock::now()});
}


void Tapper::reset() {
    _events.clear();
}


void Tapper::saveAsMidi(QString filename) {
    //Для начала мы рассмотрим самый простой случай, одной единственной ноты
    //Тогда нам не нужно делать анализ какая нота начала звучать, а какая закончила
    if (_events.size() % 2 != 0)
        qDebug() << "ERROR! Not all tap events where closed!!!";

    std::vector<long> msIntervals;
    moment prevMoment;
    for (size_t i = 0; i < _events.size() / 2; ++i) {
        long fromPrev = 0;
        auto startMoment = _events[i * 2].time;
        if (i != 0)
            fromPrev = chrono::duration_cast<chrono::milliseconds>(startMoment - prevMoment).count();
        msIntervals.push_back(fromPrev);
        auto endMoment = _events[i * 2 + 1].time;
        auto durationMs = chrono::duration_cast<chrono::milliseconds>(endMoment - startMoment).count();
        msIntervals.push_back(durationMs);
        prevMoment = endMoment;
    }

    MidiTrack track;
    track.pushChangeBPM(240, 0); //somehow 240 is realtime

    for (size_t i = 0; i < msIntervals.size(); ++i) {
        if (i % 2 == 0) {
            //Возможно стоит использовать коэфициент 500 преобразовывать в 480
            long fromPrevNote = msIntervals[i];
            qDebug() << "From prev note " << fromPrevNote;
            track.accumulate(fromPrevNote * 2);
            track.pushNoteOn(60, 100, 0);
        }
        else {
            long noteDuration = msIntervals[i];
            qDebug() << "Note duration " << noteDuration;
            track.accumulate(noteDuration * 2);
            track.pushNoteOff(60, 100, 0);
        }
    }

    track.pushEvent47();
    MidiFile midi;
    midi.push_back(track);
    midi.writeToFile(filename.toStdString());
}
