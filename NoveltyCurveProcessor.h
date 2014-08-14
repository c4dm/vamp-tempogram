//
//  NoveltyCurveProcessor.h
//  Tempogram
//
//  Created by Carl Bussey on 10/07/2014.
//  Copyright (c) 2014 Carl Bussey. All rights reserved.
//

// Don't new delete objects

#ifndef __Tempogram__NoveltyCurve__
#define __Tempogram__NoveltyCurve__

#include <iostream>
#include <cmath>
#include <vector>
#include <iostream>
#include "FIRFilter.h"
#include "WindowFunction.h"
#include <cassert>
#include "SpectrogramProcessor.h"

class NoveltyCurveProcessor{
    float m_samplingFrequency;
    int m_fftLength;
    int m_blockSize;
    int m_numberOfBlocks;
    int m_compressionConstant;
    int m_numberOfBands;
    int * m_pBandBoundaries;
    float * m_pBandSum;
    
    void initialise();
    void cleanup();
    float calculateMax(const std::vector< std::vector<float> > &spectrogram) const;
    void subtractLocalAverage(std::vector<float> &noveltyCurve, const size_t &smoothLength) const;
    void smoothedDifferentiator(std::vector< std::vector<float> > &spectrogram, const size_t &smoothLength) const;
    void halfWaveRectify(std::vector< std::vector<float> > &spectrogram) const;
    
public:
    
    NoveltyCurveProcessor(const float &samplingFrequency, const size_t &fftLength, const size_t &numberOfBlocks, const size_t &compressionConstant);
    ~NoveltyCurveProcessor();
    std::vector<float> spectrogramToNoveltyCurve(Spectrogram spectrogram) const;
};

#endif /* defined(__Tempogram__NoveltyCurve__) */
