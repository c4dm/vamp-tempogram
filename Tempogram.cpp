
// This is a skeleton file for use in creating your own plugin
// libraries.  Replace MyPlugin and myPlugin throughout with the name
// of your first plugin class, and fill in the gaps as appropriate.


#include "Tempogram.h"
#include <sstream>
#include <stdexcept>

using Vamp::FFT;
using Vamp::RealTime;
using namespace std;

Tempogram::Tempogram(float inputSampleRate) :
    Plugin(inputSampleRate),
    m_blockSize(0),
    m_stepSize(0),
    compressionConstant(1000), //parameter
    minDB(0),
    windowLength(128), //parameter
    fftLength(4096), //parameter
    thopSize(64), //parameter
    minBPM(30),
    maxBPM(480),
    minBin(0), //set in initialise()
    maxBin(0), //set in initialise()
    numberOfBlocks(0) //incremented in process()

    // Also be sure to set your plugin parameters (presumably stored
    // in member variables) to their default values here -- the host
    // will not do that for you
{
}

Tempogram::~Tempogram()
{
    //delete stuff
    cleanup();
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
    return 2048; // 0 means "I can handle any block size"
}

size_t 
Tempogram::getPreferredStepSize() const
{
    return 1024; // 0 means "anything sensible"; in practice this
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

    ParameterDescriptor d;
    d.identifier = "C";
    d.name = "C";
    d.description = "Spectrogram compression constant, C, used when retrieving the novelty curve from the audio.";
    d.unit = "";
    d.minValue = 2;
    d.maxValue = 10000;
    d.defaultValue = 1000;
    d.isQuantized = false;
    list.push_back(d);

    d.identifier = "TN";
    d.name = "Tempogram Window Length";
    d.description = "FFT window length when analysing the novelty curve and extracting the tempogram time-frequency function.";
    d.unit = "";
    d.minValue = 1024;
    d.maxValue = 4096;
    d.defaultValue = 128;
    d.isQuantized = true;
    d.quantizeStep = 128;
    list.push_back(d);
    
    d.identifier = "minBPM";
    d.name = "Minimum BPM";
    d.description = "The minimum BPM of the tempogram output bins.";
    d.unit = "";
    d.minValue = 0;
    d.maxValue = 2000;
    d.defaultValue = 30;
    d.isQuantized = true;
    d.quantizeStep = 5;
    list.push_back(d);
    
    d.identifier = "maxBPM";
    d.name = "Maximum BPM";
    d.description = "The minimum BPM of the tempogram output bins.";
    d.unit = "";
    d.minValue = 30;
    d.maxValue = 2000;
    d.defaultValue = 480;
    d.isQuantized = true;
    d.quantizeStep = 5;
    list.push_back(d);

    return list;
}

float
Tempogram::getParameter(string identifier) const
{
    if (identifier == "C") {
        return compressionConstant; // return the ACTUAL current value of your parameter here!
    }
    if (identifier == "TN"){
        return windowLength;
    }
    if (identifier == "minBPM") {
        return minBPM;
    }
    if (identifier == "maxBPM"){
        return maxBPM;
    }
    
    return 0;
}

void
Tempogram::setParameter(string identifier, float value) 
{
    
    if (identifier == "C") {
        compressionConstant = value; // set the actual value of your parameter
    }
    if (identifier == "TN") {
        windowLength = value;
    }
    if (identifier == "minBPM") {
        minBPM = value;
    }
    if (identifier == "maxBPM"){
        maxBPM = value;
    }
    
}

void Tempogram::updateBPMParameters(){

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

string Tempogram::floatToString(float value) const
{
    ostringstream ss;
    
    if(!(ss << value)) throw runtime_error("Tempogram::floatToString(): invalid conversion from float to string");
    return ss.str();
}

Tempogram::OutputList
Tempogram::getOutputDescriptors() const
{
    OutputList list;

    // See OutputDescriptor documentation for the possibilities here.
    // Every plugin must have at least one output.
    
    OutputDescriptor d;
    float d_sampleRate;
    float tempogramInputSampleRate = (float)m_inputSampleRate/m_stepSize;
    
    d.identifier = "tempogram";
    d.name = "Tempogram";
    d.description = "Tempogram";
    d.unit = "BPM";
    d.hasFixedBinCount = true;
    d.binCount = maxBin - minBin + 1;
    d.hasKnownExtents = false;
    d.isQuantized = false;
    d.sampleType = OutputDescriptor::FixedSampleRate;
    d_sampleRate = tempogramInputSampleRate/thopSize;
    d.sampleRate = d_sampleRate > 0.0 && !isnan(d_sampleRate) ? d_sampleRate : 0.0;
    for(int i = minBin; i <= maxBin; i++){
        float w = ((float)i/fftLength)*(tempogramInputSampleRate);
        d.binNames.push_back(floatToString(w*60));
    }
    d.hasDuration = false;
    list.push_back(d);
    
    d.identifier = "nc";
    d.name = "Novelty Curve";
    d.description = "Novelty Curve";
    d.unit = "";
    d.hasFixedBinCount = true;
    d.binCount = 1;
    d.hasKnownExtents = false;
    d.isQuantized = false;
    d.sampleType = OutputDescriptor::FixedSampleRate;
    d_sampleRate = tempogramInputSampleRate;
    d.sampleRate = d_sampleRate > 0 && !isnan(d_sampleRate) ? d_sampleRate : 0.0;
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
    m_stepSize = stepSize;
    minDB = pow(10,(float)-74/20);
    
    if (minBPM > maxBPM){
        minBPM = 30;
        maxBPM = 480;
    }
    float tempogramInputSampleRate = (float)m_inputSampleRate/m_stepSize;
    minBin = (unsigned int)(max(floor(((minBPM/60)/tempogramInputSampleRate)*fftLength), (float)0.0));
    maxBin = (unsigned int)(min(ceil(((maxBPM/60)/tempogramInputSampleRate)*fftLength), (float)fftLength/2));
    
    specData = vector< vector<float> >(m_blockSize/2 + 1);
    
    return true;
}

void Tempogram::cleanup(){

}

void
Tempogram::reset()
{
    // Clear buffers, reset stored values, etc
    ncTimestamps.clear();
    specData.clear();
    specData = vector< vector<float> >(m_blockSize/2 + 1);
}

Tempogram::FeatureSet
Tempogram::process(const float *const *inputBuffers, Vamp::RealTime timestamp)
{
    size_t n = m_blockSize/2 + 1;
    
    FeatureSet featureSet;
    Feature feature;
    
    const float *in = inputBuffers[0];

    //calculate magnitude of FrequencyDomain input
    for (int i = 0; i < n; i++){
        float magnitude = sqrt(in[2*i] * in[2*i] + in[2*i + 1] * in[2*i + 1]);
        magnitude = magnitude > minDB ? magnitude : minDB;
        specData[i].push_back(magnitude);
    }
    
    numberOfBlocks++;
    ncTimestamps.push_back(timestamp); //save timestamp

    return featureSet;
}

Tempogram::FeatureSet
Tempogram::getRemainingFeatures()
{
    
    float * hannWindowtN = new float[windowLength];
    for (int i = 0; i < windowLength; i++){
        hannWindowtN[i] = 0.0;
    }
    
    FeatureSet featureSet;
    
    //initialise noveltycurve processor
    NoveltyCurve nc(m_inputSampleRate, m_blockSize, numberOfBlocks, compressionConstant);
    noveltyCurve = nc.spectrogramToNoveltyCurve(specData); //calculate novelty curve from magnitude data
    
    //push novelty curve data to featureset 1 and set timestamps
    for (int i = 0; i < numberOfBlocks; i++){
        Feature feature;
        feature.values.push_back(noveltyCurve[i]);
        feature.hasTimestamp = true;
        feature.timestamp = ncTimestamps[i];
        featureSet[1].push_back(feature);
    }
    
    //window function for spectrogram
    WindowFunction::hanning(hannWindowtN,windowLength);
    
    //initialise spectrogram processor
    SpectrogramProcessor spectrogramProcessor(numberOfBlocks, windowLength, fftLength, thopSize);
    //compute spectrogram from novelty curve data (i.e., tempogram)
    vector< vector<float> > tempogram = spectrogramProcessor.process(&noveltyCurve[0], hannWindowtN);
    
    int timePointer = thopSize-windowLength/2;
    int tempogramLength = tempogram[0].size();
    
    //push tempogram data to featureset 0 and set timestamps.
    for (int block = 0; block < tempogramLength; block++){
        Feature feature;
        
        int timeMS = floor(1000*(m_stepSize*timePointer)/m_inputSampleRate + 0.5);
        
        assert(tempogram.size() == (fftLength/2 + 1));
        for(int k = minBin; k < maxBin; k++){
            feature.values.push_back(tempogram[k][block]);
        }
        feature.hasTimestamp = true;
        feature.timestamp = RealTime::fromMilliseconds(timeMS);
        featureSet[0].push_back(feature);
        
        timePointer += thopSize;
    }
    
    //float func = [](){ cout << "Hello"; };
    
    delete []hannWindowtN;
    hannWindowtN = NULL;
    
    return featureSet;
}
