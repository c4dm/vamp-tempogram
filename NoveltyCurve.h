//
//  NoveltyCurve.h
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

class NoveltyCurve{
    float m_samplingFrequency;
    int m_fftLength;
    int m_blockSize;
    int m_numberOfBlocks;
    int m_compressionConstant;
    int m_numberOfBands;
    int * m_bandBoundaries;
    int m_hannLength;
    float * m_bandSum;
    std::vector<float> data;
    
    void initialise();
    void cleanup();
    float calculateMax(std::vector< std::vector<float> > &spectrogram);
    void subtractLocalAverage(std::vector<float> &noveltyCurve);
    void smoothedDifferentiator(std::vector< std::vector<float> > &spectrogram, int smoothLength);
    void halfWaveRectify(std::vector< std::vector<float> > &spectrogram);
    
public:
    
    NoveltyCurve(float samplingFrequency, int fftLength, int numberOfBlocks, int compressionConstant);
    ~NoveltyCurve();
    std::vector<float> spectrogramToNoveltyCurve(std::vector< std::vector<float> > spectrogram);
};

#endif /* defined(__Tempogram__NoveltyCurve__) */
