//
//  AutocorrelationProcessor.cpp
//  Tempogram
//
//  Created by Carl Bussey on 20/08/2014.
//  Copyright (c) 2014 Carl Bussey. All rights reserved.
//

#include "AutocorrelationProcessor.h"
using namespace std;

AutocorrelationProcessor::AutocorrelationProcessor(const size_t &windowLength, const unsigned int &hopSize) :
    m_windowLength(windowLength),
    m_hopSize(hopSize),
    m_blockInput(0)
{
    m_blockInput = new float [m_windowLength];
}

AutocorrelationProcessor::~AutocorrelationProcessor(){
    delete []m_blockInput;
    m_blockInput = 0;
}

AutoCorrelation AutocorrelationProcessor::process(float * input, const size_t &inputLength) const
{
    int readBlockPointerIndex = 0;
    AutoCorrelation autocorrelation;
    
    while(readBlockPointerIndex <= (int)inputLength) {
        int readPointer = readBlockPointerIndex - m_windowLength/2; //read window centered at readBlockPointerIndex
        
        for (int n = 0; n < (int)m_windowLength; n++){
            if (readPointer < 0 || readPointer >= (int)inputLength) m_blockInput[n] = 0.0f;
            else m_blockInput[n] = input[readPointer];
            
            readPointer++;
        }
        
        autocorrelation.push_back(processBlock());
        readBlockPointerIndex += m_hopSize;
    }
    
    return autocorrelation;
}

vector<float> AutocorrelationProcessor::processBlock() const
{
    vector<float> autocorrelation;
    
    int N = m_windowLength/m_lagIncrement;
    
    for (int lag = 0; lag < N; lag++){
        float sum = 0;

        for (int n = 0; n < (int)m_windowLength-lag; n++){
            sum += m_blockInput[lag]*m_blockInput[n+lag];
        }
        autocorrelation.push_back(sum/(2*N + 1 - lag));
    }
    
    return autocorrelation;
}