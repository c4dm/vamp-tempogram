
// This is a skeleton file for use in creating your own plugin
// libraries.  Replace MyPlugin and myPlugin throughout with the name
// of your first plugin class, and fill in the gaps as appropriate.


#include "Tempogram.h"
#include "FIRFilter.h"
#include "WindowFunction.h"
#include <vamp-sdk/FFT.h>
#include <cmath>
#include <fstream>
#include <assert.h>
using Vamp::FFT;
using namespace std;

Tempogram::Tempogram(float inputSampleRate) :
    Plugin(inputSampleRate),
    m_blockSize(0),
    compressionConstant(1000), //make param
    previousY(NULL),
    currentY(NULL),
    tN(1024), //make param
    thopSize(512), //make param
    fftInput(NULL),
    fftOutputReal(NULL),
    fftOutputImag(NULL),
    ncLength(0)

    // Also be sure to set your plugin parameters (presumably stored
    // in member variables) to their default values here -- the host
    // will not do that for you
{
}

Tempogram::~Tempogram()
{
    //delete stuff
}

string
Tempogram::getIdentifier() const
{
    return "tempogram";
}

string
Tempogram::getName() const
{
    return "Tempogram";
}

string
Tempogram::getDescription() const
{
    // Return something helpful here!
    return "Cyclic Tempogram as described by Peter Grosche and Meinard Muller";
}

string
Tempogram::getMaker() const
{
    //Your name here
    return "Carl Bussey";
}

int
Tempogram::getPluginVersion() const
{
    // Increment this each time you release a version that behaves
    // differently from the previous one
    return 1;
}

string
Tempogram::getCopyright() const
{
    // This function is not ideally named.  It does not necessarily
    // need to say who made the plugin -- getMaker does that -- but it
    // should indicate the terms under which it is distributed.  For
    // example, "Copyright (year). All Rights Reserved", or "GPL"
    return "";
}

Tempogram::InputDomain
Tempogram::getInputDomain() const
{
    return FrequencyDomain;
}

size_t
Tempogram::getPreferredBlockSize() const
{
    return 0; // 0 means "I can handle any block size"
}

size_t 
Tempogram::getPreferredStepSize() const
{
    //23 ms?
    return 0; // 0 means "anything sensible"; in practice this
              // means the same as the block size for TimeDomain
              // plugins, or half of it for FrequencyDomain plugins
}

size_t
Tempogram::getMinChannelCount() const
{
    return 1;
}

size_t
Tempogram::getMaxChannelCount() const
{
    return 1;
}

Tempogram::ParameterList
Tempogram::getParameterDescriptors() const
{
    ParameterList list;

    // If the plugin has no adjustable parameters, return an empty
    // list here (and there's no need to provide implementations of
    // getParameter and setParameter in that case either).

    // Note that it is your responsibility to make sure the parameters
    // start off having their default values (e.g. in the constructor
    // above).  The host needs to know the default value so it can do
    // things like provide a "reset to default" function, but it will
    // not explicitly set your parameters to their defaults for you if
    // they have not changed in the mean time.

    ParameterDescriptor C;
    C.identifier = "C";
    C.name = "C";
    C.description = "Spectrogram compression constant, C";
    C.unit = "";
    C.minValue = 2;
    C.maxValue = 10000;
    C.defaultValue = 1000;
    C.isQuantized = false;
    list.push_back(C);
    
    ParameterDescriptor tN;
    tN.identifier = "tN";
    tN.name = "Tempogram FFT length";
    tN.description = "Tempogram FFT length.";
    tN.unit = "";
    tN.minValue = 128;
    tN.maxValue = 4096;
    tN.defaultValue = 1024;
    tN.isQuantized = true;
    tN.quantizeStep = 128;
    list.push_back(tN);

    return list;
}

float
Tempogram::getParameter(string identifier) const
{
    if (identifier == "C") {
        return compressionConstant; // return the ACTUAL current value of your parameter here!
    }
    if (identifier == "tN"){
        return tN;
    }
    
    return 0;
}

void
Tempogram::setParameter(string identifier, float value) 
{
    if (identifier == "C") {
        compressionConstant = value;// set the actual value of your parameter
    }
    if (identifier == "tN") {
        tN = value;
    }
}

Tempogram::ProgramList
Tempogram::getPrograms() const
{
    ProgramList list;

    // If you have no programs, return an empty list (or simply don't
    // implement this function or getCurrentProgram/selectProgram)

    return list;
}

string
Tempogram::getCurrentProgram() const
{
    return ""; // no programs
}

void
Tempogram::selectProgram(string name)
{
}

Tempogram::OutputList
Tempogram::getOutputDescriptors() const
{
    OutputList list;

    // See OutputDescriptor documentation for the possibilities here.
    // Every plugin must have at least one output.

    OutputDescriptor d;
    d.identifier = "output";
    d.name = "Cyclic Tempogram";
    d.description = "Cyclic Tempogram";
    d.unit = "";
    d.hasFixedBinCount = false;
    //d.binCount = 1;
    d.hasKnownExtents = false;
    d.isQuantized = false;
    d.sampleType = OutputDescriptor::VariableSampleRate;
    d.sampleRate = 0.0;
    d.hasDuration = false;
    list.push_back(d);

    return list;
}

bool
Tempogram::initialise(size_t channels, size_t stepSize, size_t blockSize)
{
    if (channels < getMinChannelCount() ||
	channels > getMaxChannelCount()) return false;

    // Real initialisation work goes here!
    m_blockSize = blockSize;
    currentY = new float[m_blockSize];
    previousY = new float[m_blockSize];
    
    return true;
}

void
Tempogram::reset()
{
    // Clear buffers, reset stored values, etc
}

Tempogram::FeatureSet
Tempogram::process(const float *const *inputBuffers, Vamp::RealTime timestamp)
{
    size_t n = m_blockSize/2 + 1;
    
    FeatureSet featureSet;
    Feature feature;
    feature.hasTimestamp = false;
    
    const float *in = inputBuffers[0];
    
    //Calculate log magnitude
    float sum = 0;
    for (int i = 0; i < n; i++){
        float magnitude = sqrt(in[2*i] * in[2*i] + in[2*i + 1] * in[2*i + 1]);
        currentY[i] = log(1+compressionConstant*magnitude); //should be 1+C*magnitude
        if(currentY[i] >= previousY[i]){
            sum += (currentY[i] - previousY[i]);
        }
    }
    
    noveltyCurve.push_back(sum);
    
    float *tmpY = currentY;
    currentY = previousY;
    previousY = tmpY;
    
    ncTimestamps.push_back(timestamp);
    
    return FeatureSet();
}

void
Tempogram::initialiseForGRF(){
    hannN = 129;
    hannWindow = new float[hannN];
    hannWindowtN = new float[tN];
    fftInput = new double[tN];
    fftOutputReal = new double[tN];
    fftOutputImag = new double[tN];
    ncLength = noveltyCurve.size();
    
    WindowFunction::hanning(hannWindow, hannN, true);
}

void
Tempogram::cleanupForGRF(){
    delete []hannWindow;
    hannWindow = NULL;
    delete []hannWindowtN;
    hannWindowtN = NULL;
    delete []fftInput;
    fftInput = NULL;
    delete []fftOutputReal;
    fftOutputReal = NULL;
    delete []fftOutputImag;
    fftOutputImag = NULL;
}

Tempogram::FeatureSet
Tempogram::getRemainingFeatures()
{
    //Make sure this is called at the beginning of the function
    initialiseForGRF();
    
    vector<float> noveltyCurveLocalAverage(ncLength);
    
    FIRFilter *filter = new FIRFilter(ncLength, hannN);
    filter->process(&noveltyCurve[0], hannWindow, &noveltyCurveLocalAverage[0]);
    delete filter;
    
    for(int i = 0; i < ncLength; i++){
        noveltyCurve[i] -= noveltyCurveLocalAverage[i];
        noveltyCurve[i] = noveltyCurve[i] >= 0 ? noveltyCurve[i] : 0;
    }
    
    int i=0;
    WindowFunction::hanning(hannWindowtN, tN);
    
    int index;
    int start = floor(tN/2 + 0.5);
    int timestampInc = floor((((float)ncTimestamps[1].nsec - ncTimestamps[0].nsec)/1e9)*(thopSize) + 0.5);
    //cout << timestampInc << endl;
    
    FeatureSet featureSet;
    
    while(i < ncLength){
        Feature feature;
        Vamp::RealTime timestamp;
        
        for (int n = start; n < tN; n++){
            index = i + n - tN/2;
            assert (index >= 0);
            
            if(index < ncLength){
                fftInput[n] = noveltyCurve[i + n] * hannWindowtN[n];
            }
            else if(index >= ncLength){
                fftInput[n] = 0.0; //pad the end with zeros
            }
            //cout << fftInput[n] << endl;
        }
        if (i+tN/2 > ncLength){
            timestamp = Vamp::RealTime::fromSeconds(ncTimestamps[i].sec + timestampInc);
        }
        else{
            timestamp = ncTimestamps[i + tN/2];
        }
        
        FFT::forward(tN, fftInput, NULL, fftOutputReal, fftOutputImag);
        
        //TODO: sample at logarithmic spacing
        for(int k = 0; k < tN; k++){
            double fftOutputPower = (fftOutputReal[k]*fftOutputReal[k] + fftOutputImag[k]*fftOutputImag[k]); //Magnitude or power?
            assert (!isinf(fftOutputPower));
            
            feature.values.push_back(fftOutputPower);
        }

        i += thopSize;
        start = 0;
        
        feature.timestamp = timestamp;
        feature.hasTimestamp = true;
        featureSet[0].push_back(feature);
    }
    
    //Make sure this is called at the end of the function
    cleanupForGRF();
    
    return featureSet;
}
