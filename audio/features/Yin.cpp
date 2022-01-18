#include "Yin.hpp"

#include <QDebug>

#include "PeaksOperations.hpp"

#include "libs/kiss/kiss_fft.h"
#include <complex>


using namespace aural_sight;


Yin::Yin() { //TODO template
    _threshold = 0.15;
}

size_t Yin::getTau() const {
    return _currentTau;
}

double Yin::getThreshold() const {
    return _threshold;
}

void Yin::setThreshold(const double newThresh) {
    _threshold = newThresh;
}

void Yin::init(double yinSampleRate, const size_t yinBufferSize) {
    if (yinSampleRate != _sampleRate)
        _sampleRate = yinSampleRate;
    if (_bufferSize == yinBufferSize)
        return;
    _bufferSize = yinBufferSize;
    _halfBufferSize = _bufferSize / 2;
    _yinBuffer = std::vector<float>(_halfBufferSize, 0.f);
}


double Yin::getPitch(const float *buffer) {

    _yinBuffer = std::vector<float>(_halfBufferSize, 0.f);
    differenceFunction(buffer);
    accMeanNormDifference();
    if (absoluteThresholdFound())  {
    size_t _new = parabolicInterpolation();
        return _sampleRate / _new;
    }
    return -1.0;
}


void Yin::differenceFunction(const float* buffer) {
    float delta = 0.f;
    for(size_t tau = 0 ; tau < _halfBufferSize; tau++) {
        for(size_t index = 0; index < _halfBufferSize; index++){
            delta = buffer[index] - buffer[index + tau];
            _yinBuffer[tau] += delta * delta;
        }
    }
}


double Yin::parabolicInterpolation() const {

    size_t start = _currentTau ? _currentTau - 1 : _currentTau;
    size_t finish = _currentTau + 1 < _halfBufferSize ? _currentTau + 1 : _currentTau;

    auto borderResult = [this](size_t idx) {
        if (_yinBuffer[_currentTau] <= _yinBuffer[idx])
            return _currentTau;
        else
            return idx;
    };

    if (start == _currentTau)
        return borderResult(finish);
    if (finish == _currentTau)
        return borderResult(start);

    double begin = _yinBuffer[start];
    double middle = _yinBuffer[_currentTau];
    double end = _yinBuffer[finish];
    return _currentTau + (end - begin) / (2.0 * (2.0 * middle - end - begin));
}



void Yin::accMeanNormDifference(){
    _yinBuffer[0] = 1;
    float runningSum = 0;
    for (size_t tau = 1; tau < _halfBufferSize; tau++) {
        runningSum += _yinBuffer[tau];
        _yinBuffer[tau] *= tau / runningSum;
    }
}



bool Yin::absoluteThresholdFound(){

    for (_currentTau = 2; _currentTau < _halfBufferSize ; _currentTau++)

        if (_yinBuffer[_currentTau] < _threshold) {

            while (_currentTau + 1 < _halfBufferSize &&
                   _yinBuffer[_currentTau + 1] < _yinBuffer[_currentTau])

                ++_currentTau;

            break;
        }

    if (_currentTau == _halfBufferSize || _yinBuffer[_currentTau] >= _threshold)
        return false; //ELSE find min

    return true;
}



void YinPP::calcBasicACF(const float* buffer) {

    std::vector<std::complex<float>> b(4096, 0.f);

    std::transform(buffer, buffer + 4096,
        b.begin(), [](float x) -> std::complex<float> {
            return std::complex(x, static_cast<float>(0.0));
        });

    kiss_fft_cfg fwd = kiss_fft_alloc(4096, 0, NULL,NULL);
    kiss_fft_cfg inv = kiss_fft_alloc(4096, 1, NULL,NULL);

    std::vector<std::complex<float>> out(4096, 0.f);

    kiss_fft(fwd,(kiss_fft_cpx*)b.data(),(kiss_fft_cpx*)out.data());

    std::complex<float> scale = {
        1.0f / (float)(4096 * 2), static_cast<float>(0.0)};

    for (int i = 0; i < 4096; ++i)
        out[i] *= std::conj(out[i]) * scale;

    kiss_fft(inv,(kiss_fft_cpx*)out.data(),(kiss_fft_cpx*)b.data());

    std::vector<float> realOut(4096, 0.f); //half?

    std::transform(b.begin(), b.begin() + 4096, realOut.begin(),
        [](std::complex<float> cplx) -> float { return std::real(cplx); });

    acfBufer = realOut;
    sumBufV2 = std::vector<float>(2048, 0.f);

    for (size_t i = 0; i < 2048; ++i) { //TODO second term is wrong!
        sumBufV2[i] = 2 * realOut[0]  - 2 * realOut[i];
    }
}


double YinPP::process(const float* buffer) {

    calcBasicACF(buffer);

    _yinBufer1 = std::vector<float>(_bufferSize/2.0, 0.f); //TODO better way!

    differenceFunction(buffer);
    sumBufer = _yinBufer1;

    accMeanDifferenceFunction(_yinBufer1);
    accBufer = _yinBufer1;

    auto maxE = std::max_element(_yinBufer1.begin(), _yinBufer1.end());
    float maxVal = *maxE;

    std::vector<double> invBuffer(_yinBufer1.size(), 0); //TODO inversed function for peaks
    for (size_t i = 0; i < invBuffer.size(); ++i)
        invBuffer[i] = maxVal - _yinBufer1[i];

    auto idx = peakIndexes<double>(invBuffer, 2);

    size_t startIdx = 0;
    if (idx.empty() == false && idx[0] == 0)
        startIdx = 1;

    float minP = 100.0, maxP = -1.0;
    for (size_t i = startIdx; i < idx.size(); ++i) {
        auto id = idx[i];
        const float v = _yinBufer1[id];
        if (v > maxP)
            maxP = v;
        if (v < minP)
            minP = v;
    }

    filteredIdx.clear();
    for (size_t i = startIdx; i < idx.size(); ++i) {
        auto id = idx[i];
        const float val =  _yinBufer1[id];
        const float distMax = maxP - val;
        const float distMin = val - minP;
        if (distMax > distMin)
            filteredIdx.push_back(id);
    }

    mineFound = findPeakCommonDistance(filteredIdx, 3);

    size_t tNew = absoluteThreshold(_yinBufer1);
    stdFound = parabolicInterpolation(tNew, _yinBufer1);

    double foundPitch = _sampleRate / stdFound;
    return foundPitch;
}



size_t YinPP::absoluteThreshold(std::vector<float>& v) {
    double minValue = 5000.;
    size_t minTau = 0;
    size_t found = 0;

    for (found = 2; found < v.size() ; found++) {
        if (v[found] < 0.15) { //TODO установка
            while (found + 1 < v.size() && v[found + 1] < v[found])
                ++found;
            return found;
        }
        else
            if (v[found] < minValue) {
                minValue = v[found];
                minTau = found;
            }
    }

    return minTau;
}



size_t YinPP::parabolicInterpolation(size_t t, std::vector<float>& v) {
    if (t == v.size())
        return t;

    size_t betterTau = 0.f;
    if (t > 0 && t < v.size() - 1) {
        float s0 = v[t - 1];
        float s1 = v[t];
        float s2 = v[t + 1];
        float adjustment = (s2 - s0) / (2.f * (2.f * s1 - s2 - s0));
        if (abs(adjustment) > 1.f)
            adjustment = 0.f;
        betterTau = t + adjustment;
    }
    else
        betterTau =  t;

    return betterTau;
}



void YinPP::accMeanDifferenceFunction(std::vector<float>& v) {
    v[0] = 1;
    float runningSum = 0;
    for (size_t tau = 1; tau < v.size(); tau++) {
        runningSum += v[tau];
        v[tau] *= tau / runningSum;
    }
}


void YinPP::differenceFunction(const float* signal) {
    const size_t n = _bufferSize / 2;
    for(size_t tau = 0 ; tau < n; tau++)
        for(size_t j = 0; j < n; j++)
        {
            auto d = signal[j] - signal[j + tau];
            _yinBufer1[tau] += d * d;
        }
}

