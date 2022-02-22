#ifndef PIANOROLL_H
#define PIANOROLL_H

#include <QQuickPaintedItem>

#include "midi/MidiFile.hpp"


namespace aurals {


    struct RollNote {
        int x;
        int y;
        int w;
        int h;

        int midiNote;

        //fret, string, other data unordered_map<string, int>
    };


    class PianoRoll : public QQuickPaintedItem
    {
        Q_OBJECT
    public:
        PianoRoll() = default;

        Q_INVOKABLE void loadMidi(QString filename);
        Q_INVOKABLE int getContentWidth();
        Q_INVOKABLE int getContentHeight();

        Q_INVOKABLE void reset();

        void paint(QPainter* painter);


        Q_INVOKABLE int getTrackCount() {
            return _mid.size();
        }

        Q_INVOKABLE void setCurrentTrack(int newIdx) {
            _currentTrack = newIdx;
            update();
        }

        Q_INVOKABLE void setHeightStretch(bool value) {
            _fillHeight = value;
        }

        Q_INVOKABLE void zoomIn() {
            _xZoomCoef *= 2;
            update();
        }
        Q_INVOKABLE void zoomOut() {
            _xZoomCoef /= 2;
            update();
        }

        Q_INVOKABLE void ondblclick(int x, int y);

    signals:


    private:

        aurals::MidiFile _mid;

        size_t _currentTrack = 1;

        bool _fillHeight = false;

        double _xZoomCoef = 1.0;
        //double _yZoomCoef = 1.0;

        std::vector<RollNote> _notes;
        int _noteCursor = -1;

    };



}



#endif // PIANOROLL_H
