//
//  Spectrogram.h
//  Tempogram
//
//  Created by Carl Bussey on 07/08/2014.
//  Copyright (c) 2014 Carl Bussey. All rights reserved.
//

#ifndef __Tempogram__Spectrogram__
#define __Tempogram__Spectrogram__
#include <vector>
#include <vamp-sdk/FFT.h>
#include <cmath>

class SpectrogramProcessor{
    int m_inputLength;
    int m_windowLength;
    int m_fftLength;
    int m_hopSize;
    int m_numberOfOutputBins;
    double * fftInput;
    double * fftOutputReal;
    double * fftOutputImag;
    std::vector< std::vector <float> > spectrogramOutput;
    
    void initialise();
    void cleanup();
public:
    std::vector< std::vector<float> > process(const float * const input, const float * window);
    SpectrogramProcessor(unsigned int inputLength, unsigned int windowLength, unsigned int fftLength, unsigned int hopSize);
    ~SpectrogramProcessor();
};

#endif /* defined(__Tempogram__Spectrogram__) */
