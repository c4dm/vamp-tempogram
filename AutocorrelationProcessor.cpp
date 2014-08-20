//
//  AutocorrelationProcessor.cpp
//  Tempogram
//
//  Created by Carl Bussey on 20/08/2014.
//  Copyright (c) 2014 Carl Bussey. All rights reserved.
//

#include "AutocorrelationProcessor.h"
using namespace std;

AutocorrelationProcessor::AutocorrelationProcessor(const size_t &windowLength, const unsigned int &hopSize, const unsigned int &lagIncrement) :
    m_windowLength(windowLength),
    m_hopSize(hopSize),
    m_lagIncrement(lagIncrement)
{
    //Nothing to do here
}

AutoCorrelation AutocorrelationProcessor::process(float * input, const size_t &inputLength) const
{
    int readBlockPointerIndex = 0;
    AutoCorrelation autocorrelation;
    
    while(readBlockPointerIndex <= (int)inputLength) {
        int readPointer = readBlockPointerIndex - m_windowLength/2;
        
        autocorrelation.push_back(processBlock(&input[readPointer], min(inputLength-readPointer, m_windowLength)));
        readBlockPointerIndex += m_hopSize;
    }
    
    return autocorrelation;
}

vector<float> AutocorrelationProcessor::processBlock(float * blockInput, const size_t &blockLength) const
{
    vector<float> autocorrelation;
    
    int N = m_windowLength/m_lagIncrement;
    
    for (int lag = 0; lag < N; lag++){
        float sum = 0;
        int sampleLag = m_lagIncrement*lag;
        
        for (int n = sampleLag; n < (int)blockLength; n++){
            sum += blockInput[n-sampleLag]*blockInput[n];
        }
        autocorrelation.push_back(sum/(2*N + 1 - lag));
    }
    
    return autocorrelation;
}