//
//  AutocorrelationProcessor.cpp
//  Tempogram
//
//  Created by Carl Bussey on 20/08/2014.
//  Copyright (c) 2014 Carl Bussey. All rights reserved.
//

#include "AutocorrelationProcessor.h"
using namespace std;
#include <iostream>

AutocorrelationProcessor::AutocorrelationProcessor(const size_t &windowLength, const unsigned int &hopSize) :
    m_windowLength(windowLength),
    m_hopSize(hopSize)
{

}

AutocorrelationProcessor::~AutocorrelationProcessor()
{

}

AutoCorrelation AutocorrelationProcessor::process(float * input, const size_t &inputLength) const
{
    int readBlockPointerIndex = 0;
    AutoCorrelation autocorrelation;
    
    while(readBlockPointerIndex <= (int)inputLength) {
        
        vector<float> autocorrelationBlock;
        
        for (int lag = 0; lag < (int)m_windowLength; lag++){
            float sum = 0;
            int readPointer = readBlockPointerIndex - m_windowLength/2;
            
            for (int n = 0; n < (int)m_windowLength; n++){
                if (readPointer+lag >= (int)inputLength) break;
                else if (readPointer >= 0) sum += input[readPointer]*input[readPointer+lag];
                //else cout << readPointer << " : "<< lag << "/" << m_windowLength << endl;
                
                readPointer++;
            }
            autocorrelationBlock.push_back(sum/(2*m_windowLength + 1 - lag));
        }
        
        //autocorrelation.push_back(processBlock());
        autocorrelation.push_back(autocorrelationBlock);
        readBlockPointerIndex += m_hopSize;
    }
    
    return autocorrelation;
}
