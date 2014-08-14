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

typedef std::vector <std::vector<float> > Spectrogram;
typedef std::vector <std::vector<float> > SpectrogramTransposed;

class SpectrogramProcessor{
    size_t m_windowLength;
    size_t m_fftLength;
    size_t m_hopSize;
    size_t m_numberOfOutputBins;
    double * m_pFftInput;
    double * m_pFftOutputReal;
    double * m_pFftOutputImag;
    
    void initialise();
    void cleanup();
public:
    SpectrogramProcessor(const size_t &windowLength, const size_t &fftLength, const size_t &hopSize);
    ~SpectrogramProcessor();
    
    std::vector< std::vector<float> > process(const float * const pInput, const size_t &inputLength, const float * pWindow, const bool &transposeOutput = false) const;
    static SpectrogramTransposed transpose(const Spectrogram &spectrogram);
};

#endif /* defined(__Tempogram__Spectrogram__) */
