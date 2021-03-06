# Simple makefile showing the commandline arguments to g++
# used to compile a simple example with kodo
# Note, that the paths to the libraries depend
# on your specific machine and will need to be updated.

# By default, we assume that the required libraries (sak, fifi and boost)
# are cloned side-by-side with the kodo folder.
# You can clone the projects like this:
# git clone https://github.com/steinwurf/sak.git
# git clone https://github.com/steinwurf/fifi.git
# git clone https://github.com/steinwurf/platform.git
# git clone https://github.com/steinwurf/cpuid.git
# git clone https://github.com/steinwurf/external-boost-light.git
# git clone https://github.com/steinwurf/kodo.git
# Then build this example:
# cd kodo/examples/sample_makefile
# make

UNAME_S := $(shell uname -s)

# The include path to the kodo sources
KODO_DIR ?= ../../src

# The include path to the sak sources
SAK_DIR ?= ../../../sak/src

# The include path to the fifi sources
FIFI_DIR ?= ../../../fifi/src

# Fifi is a library but instead of compiling the library we just
# compile all the .cpp files. Note that compiling Fifi this way will
# disable all hardware SIMD accelerations.
FIFI_CPP_FILES := $(wildcard $(FIFI_DIR)/fifi/*.cpp)

# The include path to the boost sources
BOOST_DIR ?= ../../../external-boost-light

# The include path for the platform sources
PLATFORM_DIR ?= ../../../platform/src

# The include path for the cpuid sources
CPUID_DIR ?= ../../../cpuid/src

# cpuid is also a library but instead of compiling the library we just
# compile all the .cpp files.
CPUID_CPP_FILES := $(wildcard $(CPUID_DIR)/cpuid/*.cpp)

# Set default flags
INCLUDES = -I $(BOOST_DIR) -I $(SAK_DIR) -I $(FIFI_DIR) -I $(KODO_DIR) \
           -I $(PLATFORM_DIR) -I $(CPUID_DIR)

CXXFLAGS = -std=c++0x -s -O2 -ftree-vectorize

# Choose default compiler based on the OS
ifeq ($(CXX),)
    ifeq ($(UNAME_S),Linux)
        CXX = g++
    endif
    ifeq ($(UNAME_S),Darwin)
        CXX = clang++
    endif
endif

# Set C++ flags for clang on Mac OSX
ifeq ($(UNAME_S),Darwin)
    ifneq (,$(findstring clang++,$(CXX)))
        CXXFLAGS = -std=c++0x -O2 -stdlib=libc++ -lc++
    endif
    ifneq (,$(findstring c++,$(CXX)))
        CXXFLAGS = -std=c++0x -O2 -stdlib=libc++ -lc++
    endif
endif

# Set C++ flags for Android cross-compiler (required for cstdint and boost)
ifneq (,$(findstring android,$(CXX)))
    CXXFLAGS = -std=gnu++0x -s -O2 -ftree-vectorize \
               -DANDROID -DBOOST_ALL_NO_LIB=1 -D__GLIBC__ \
               -D_GLIBCXX_USE_WCHAR_T -D_GLIBCXX_USE_C99_STDINT_TR1
endif

TARGET ?= example

# Invoke the compiler
all:
	$(CXX) $(CPUID_CPP_FILES) $(FIFI_CPP_FILES) main.cpp -o $(TARGET) \
        $(CXXFLAGS) $(INCLUDES)
