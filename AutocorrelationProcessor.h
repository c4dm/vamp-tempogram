//
//  AutocorrelationProcessor.h
//  Tempogram
//
//  Created by Carl Bussey on 20/08/2014.
//  Copyright (c) 2014 Carl Bussey. All rights reserved.
//

#ifndef __Tempogram__Autocorrelation__
#define __Tempogram__Autocorrelation__

#include <iostream>
#include <vector>

typedef std::vector< std::vector<float> > AutoCorrelation;

class AutocorrelationProcessor{
public:
    AutocorrelationProcessor(const size_t &windowLength, const unsigned int &hopSize);
    ~AutocorrelationProcessor();
    AutoCorrelation process(float * input, const size_t &inputLength) const;
private:
    size_t m_windowLength;
    unsigned int m_hopSize;
    unsigned int m_lagIncrement;
    float * m_blockInput;
    
    std::vector<float> processBlock() const;
};

#endif /* defined(__Tempogram__Autocorrelation__) */
