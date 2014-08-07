
// This is a skeleton file for use in creating your own plugin
// libraries.  Replace MyPlugin and myPlugin throughout with the name
// of your first plugin class, and fill in the gaps as appropriate.


#include "Tempogram.h"

using Vamp::FFT;
using Vamp::RealTime;
using namespace std;

Tempogram::Tempogram(float inputSampleRate) :
    Plugin(inputSampleRate),
    m_blockSize(0),
    m_stepSize(0),
    compressionConstant(1000), //make param
    specMax(0),
    minDB(0),
    tN(128), //make param
    thopSize(64), //make param
    fftInput(NULL),
    fftOutputReal(NULL),
    fftOutputImag(NULL),
    numberOfBlocks(0)

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
    return 0; // 0 means "I can handle any block size"
}

size_t 
Tempogram::getPreferredStepSize() const
{
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
    tN.defaultValue = 128;
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
        compressionConstant = value; // set the actual value of your parameter
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
    float d_sampleRate;
    
    d.identifier = "tempogram";
    d.name = "Tempogram";
    d.description = "Tempogram";
    d.unit = "";
    d.hasFixedBinCount = true;
    d.binCount = tN/2 + 1;
    d.hasKnownExtents = false;
    d.isQuantized = false;
    d.sampleType = OutputDescriptor::FixedSampleRate;
    d_sampleRate = m_inputSampleRate/(m_stepSize * thopSize);
    d.sampleRate = d_sampleRate > 0.0 && !isnan(d_sampleRate) ? d_sampleRate : 0.0;
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
    d_sampleRate = m_inputSampleRate/m_stepSize;
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
    
    specData = vector< vector<float> >(m_blockSize/2 + 1);
    
    return true;
}

void Tempogram::cleanup(){

}

void
Tempogram::reset()
{
    // Clear buffers, reset stored values, etc
    cleanupForGRF();
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

    for (int i = 0; i < n; i++){
        float magnitude = sqrt(in[2*i] * in[2*i] + in[2*i + 1] * in[2*i + 1]);
        magnitude = magnitude > minDB ? magnitude : minDB;
        specData[i].push_back(magnitude);
    }
    
    numberOfBlocks++;
    ncTimestamps.push_back(timestamp);

    return featureSet;
}

void
Tempogram::initialiseForGRF(){
    hannWindowtN = new float[tN];
    fftInput = new double[tN];
    fftOutputReal = new double[tN];
    fftOutputImag = new double[tN];
    
    for (int i = 0; i < tN; i ++){
        hannWindowtN[i] = 0.0;
        fftInput[i] = 0.0;
        fftOutputReal[i] = 0.0;
        fftOutputImag[i] = 0.0;
    }
}

void
Tempogram::cleanupForGRF(){
    delete []hannWindowtN;
    hannWindowtN = NULL;
    fftInput = fftOutputReal = fftOutputImag = NULL;
}

Tempogram::FeatureSet
Tempogram::getRemainingFeatures()
{
    //Make sure this is called at the beginning of the function
    initialiseForGRF();
    FeatureSet featureSet;
    
    NoveltyCurve nc(m_inputSampleRate, m_blockSize, numberOfBlocks, compressionConstant);
    noveltyCurve = nc.spectrogramToNoveltyCurve(specData);
    
    for (int i = 0; i < numberOfBlocks; i++){
        Feature feature;
        feature.values.push_back(noveltyCurve[i]);
        feature.hasTimestamp = true;
        feature.timestamp = ncTimestamps[i];
        featureSet[1].push_back(feature);
    }
    
    WindowFunction::hanning(hannWindowtN, tN);
    Spectrogram * spectrogramProcessor = new Spectrogram(numberOfBlocks, tN, thopSize);
    vector< vector<float> > tempogram = spectrogramProcessor->audioToMagnitudeSpectrogram(&noveltyCurve[0], hannWindowtN);
    delete spectrogramProcessor;
    spectrogramProcessor = NULL;
    
    int timePointer = thopSize-tN/2;
    int tempogramLength = tempogram[0].size();
    
    for (int block = 0; block < tempogramLength; block++){
        Feature feature;
        
        int timeMS = floor(1000*(m_stepSize*timePointer)/m_inputSampleRate + 0.5);
        
        for(int k = 0; k < tN/2 + 1; k++){
            feature.values.push_back(tempogram[k][block]);
        }
        feature.hasTimestamp = true;
        feature.timestamp = RealTime::fromMilliseconds(timeMS);
        featureSet[0].push_back(feature);
        
        timePointer += thopSize;
    }
    
    //Make sure this is called at the end of the function
    cleanupForGRF();
    
    return featureSet;
}
