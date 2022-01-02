#ifndef WAVECONTOUR_H
#define WAVECONTOUR_H
#include <QList>


struct ContourEl{
    qint32 max;
    qint32 min;
    double energy; //TODO альтернативные варианты
};


class WaveContour {

public:
    WaveContour(QString filename);
    WaveContour() {}

    void calculateF0();
    const QVector<double>& getPitch() const { return _yinLine; }

    const QVector<int>& getNoteStarts() const { return _noteStarts; }
    const QVector<int>& getNoteEnds() const { return _noteEnds; }

    const QVector<float>& getFloatSamples() const { return _floatSamples; }

    const QVector<double>& getRMS() const { return _rmsLine; }
    const QVector<double>& getRMS_2() const { return _halfRmsLine; }
    const QVector<double>& getRMS_4() const { return _quaterRmsLine; }
    const QVector<double>& getRMS_8() const { return _8RmsLine; }

    const std::vector<size_t> rmsHigh() const { return _rmsHigh; }
    const std::vector<size_t> rmsLow() const { return _rmsLow; }
    size_t getRmsStep() const { return _rmsStep; }

    void setRmsStep(size_t newRmsStep) {
        _rmsStep = newRmsStep;
        calculateRms();
    }

    void changePeakSence(double newSence) {
        _peakSensetivity = newSence;
        calculateRms();
    }

    void changeYinTheshold(double threshold) {
        _yinTheshold = threshold;
    }

protected:

    QVector<float> _floatSamples;

    size_t _rmsStep = 125 * 4;
    QVector<double> _rmsLine; //0, 1, 2, 3
    QVector<double> _halfRmsLine;
    QVector<double> _quaterRmsLine;
    QVector<double> _8RmsLine;

    double _peakSensetivity = 4.0;
    std::vector<size_t> _rmsHigh;
    std::vector<size_t> _rmsLow;

    double _yinTheshold = 0.15;
    QVector<double> _yinLine;

    QVector<int> _noteStarts;
    QVector<int> _noteEnds;

    QVector<ContourEl> _bpm64; //TODO
    double _totalBPM;


private:

    double findBPM(); //TODO
    bool loadWavFile(QString filename);

    void calculateRms();
    void createSubRms();
};

#endif // WAVECONTOUR_H
