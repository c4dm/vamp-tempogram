
##  Makefile for Vamp plugin builds using command-line tools.
##
##  This file defines all of the system-independent information about
##  your project: the list of source files, plugin library name, etc.
##  Edit this file to make sure it has all the right information.
##
##  This file does not define the system-specific stuff such as which
##  compiler to use -- that goes into Makefile.osx, Makefile.mingw32,
##  Makefile.linux etc.  Those files all include this file, so that
##  they all have a consistent set of project data.
##
##  To build the plugin project, type
##
##    $ gmake -f Makefile.osx
##
##  or whatever the equivalent filename suffix is for your platform.
##
##  This requires GNU make, which is what you get with OS/X, Linux, or
##  MinGW/Cygwin on Windows.
##
##  (For Windows builds using MS Visual Studio, start instead with the
##  VampExamplePlugins project found in the build directory of the SDK.)


# Edit this to the base name of your plugin library
#
PLUGIN_LIBRARY_NAME := tempogram

# Edit this to list the .cpp or .c files in your plugin project
#
PLUGIN_SOURCES := TempogramPlugin.cpp FIRFilter.cpp WindowFunction.cpp plugins.cpp NoveltyCurveProcessor.cpp SpectrogramProcessor.cpp AutocorrelationProcessor.cpp

# Edit this to list the .h files in your plugin project
#
PLUGIN_HEADERS := TempogramPlugin.h FIRFilter.h WindowFunction.h NoveltyCurveProcessor.h SpectrogramProcessor.h AutocorrelationProcessor.h


##  Normally you should not edit anything below this line

SRC_DIR		:= .

CFLAGS		:= $(ARCHFLAGS) $(CFLAGS)
CXXFLAGS	:= $(CFLAGS) -I. -I$(VAMPSDK_DIR) $(CXXFLAGS)

LDFLAGS		:= $(ARCHFLAGS) $(LDFLAGS) 
PLUGIN_LDFLAGS	:= $(LDFLAGS) $(PLUGIN_LDFLAGS)

# Defaults, overridden from the platform-specific Makefile
VAMPSDK_DIR	?= ../vamp-plugin-sdk
PLUGIN_EXT	?= .so
CXX 		?= g++
CC 		?= gcc

PLUGIN 		:= $(PLUGIN_LIBRARY_NAME)$(PLUGIN_EXT)

PLUGIN_OBJECTS 	:= $(PLUGIN_SOURCES:.cpp=.o)
PLUGIN_OBJECTS 	:= $(PLUGIN_OBJECTS:.c=.o)

$(PLUGIN): $(PLUGIN_OBJECTS) 
	   $(CXX) -o $@ $^ $(PLUGIN_LDFLAGS)

$(PLUGIN_OBJECTS): $(PLUGIN_HEADERS)

clean:
	rm -f $(PLUGIN_OBJECTS)

distclean:	clean
	rm -f $(PLUGIN)

depend:
	makedepend -Y -fMakefile.inc $(PLUGIN_SOURCES) $(PLUGIN_HEADERS)

# DO NOT DELETE

TempogramPlugin.o: TempogramPlugin.h FIRFilter.h WindowFunction.h
TempogramPlugin.o: NoveltyCurveProcessor.h SpectrogramProcessor.h
TempogramPlugin.o: AutocorrelationProcessor.h
FIRFilter.o: FIRFilter.h
WindowFunction.o: WindowFunction.h
plugins.o: TempogramPlugin.h FIRFilter.h WindowFunction.h
plugins.o: NoveltyCurveProcessor.h SpectrogramProcessor.h
plugins.o: AutocorrelationProcessor.h
NoveltyCurveProcessor.o: NoveltyCurveProcessor.h FIRFilter.h WindowFunction.h
NoveltyCurveProcessor.o: SpectrogramProcessor.h
SpectrogramProcessor.o: SpectrogramProcessor.h
AutocorrelationProcessor.o: AutocorrelationProcessor.h
TempogramPlugin.o: FIRFilter.h WindowFunction.h NoveltyCurveProcessor.h
TempogramPlugin.o: SpectrogramProcessor.h AutocorrelationProcessor.h
NoveltyCurveProcessor.o: FIRFilter.h WindowFunction.h SpectrogramProcessor.h
