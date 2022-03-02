#include "PatternReceiver.h"

#include <QPainter>
#include <QDebug>

#include "midi/MidiFile.hpp"

using namespace aurals;


void PatternReceiver::paint(QPainter* painter) {

    painter->drawEllipse(0, 0, 100, 100);
    //Концентрические круги
    //Если анимация включена то рассчёт положения на них
}

void PatternReceiver::connectLine(QObject* line) {
    auto patternLine = dynamic_cast<PatternLine*>(line);
    if (patternLine != nullptr) {
        _lines.push_back(patternLine);
        qDebug() << "Connecting pattern line " << patternLine;
    }
}


void PatternReceiver::generateMidi(QString filename) {
    MidiTrack track;

    track.pushChangeBPM(_bpm, 0);
    track.pushChangeInstrument(0, 9);

    struct MiniMidi { //TODO общий для PianoRoll
        bool on;
        uint8_t note;
    };

    std::map<unsigned long, std::vector<MiniMidi>> midiMap;


    for (int t = 0; t < _repeatTimes; ++t)
        for (auto& currentLine: _lines)
        {
            const auto& bricks = currentLine->getBricks();
            const int brickSize = currentLine->getBrickSize();
            const uint8_t midiNote = currentLine->getMidiNote();

            const double barSize = static_cast<double>(currentLine->getNum()) / currentLine->getDenom();

            const unsigned long timesShift = t * 480 * 4 * barSize;

            for (size_t i = 0; i < bricks.size(); ++i)
            {
                if (bricks[i].on == true)
                {
                    unsigned long timeSize = 480 * 4 / brickSize;

                    unsigned long start = timesShift + timeSize * i;
                    unsigned long finish = timesShift + timeSize * (i + 1);

                    if (midiMap.count(start) == 0)
                        midiMap[start] = {};
                    if (midiMap.count(finish) == 0)
                        midiMap[finish] = {};

                    midiMap[start].push_back({true, midiNote});
                    midiMap[finish].push_back({false, midiNote});

                }
            }
        }


    unsigned long prevTime = 0;
    for (const auto& events: midiMap) //Общий код с piano roll - refactoring
    {
        unsigned long currentTime = events.first; //Rewrite structures binding

        for (auto& event: events.second) {

            if (prevTime != currentTime) {
                track.accumulate(currentTime - prevTime);
                prevTime = currentTime;
                //Можно считать TotalTime тут
            }

            if (event.on)
                track.pushNoteOn(event.note, 127, 9);
            else
                track.pushNoteOff(event.note, 127, 9);
        }

    }

    track.accumulate(240); //Hot fix for midi generation swallowed note
    track.pushNoteOff(36, 127, 9);
    track.pushEvent47();

    MidiFile midi;
    midi.push_back(track);
    midi.writeToFile(filename.toStdString());
}
