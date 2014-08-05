//
//  NoveltyCurve.h
//  Tempogram
//
//  Created by Carl Bussey on 10/07/2014.
//  Copyright (c) 2014 Carl Bussey. All rights reserved.
//

#ifndef __Tempogram__NoveltyCurve__
#define __Tempogram__NoveltyCurve__

#include <iostream>

class NoveltyCurve{
    float m_samplingFrequency;
    int m_blockSize;
    int m_numberOfBlocks;
    int m_compressionConstant;
    int m_numberOfBands;
    int * m_bandBoundaries;
    int m_hannLength;
    float * m_hannWindow;
    float ** m_bandSum;
    
    void initialise();
    void cleanup();
    float calculateMax(float ** spectrogram);
    void subtractLocalAverage(float * noveltyCurve);
    
public:
    vector<float> data;
    
    NoveltyCurve(float samplingFrequency, int blockSize, int numberOfBlocks, int compressionConstant);
    ~NoveltyCurve();
    vector<float> spectrogramToNoveltyCurve(float ** spectrogram);
};

#endif /* defined(__Tempogram__NoveltyCurve__) */
