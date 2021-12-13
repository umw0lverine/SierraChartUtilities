# SierraChartUtilities

SierraChart is a very high performance trading platform written in C++ and it can be downloaded here (account needed):

> www.sierrachart.com

SierraChart gives delayed <b>tick by tick</b> price datafeed including the <b>Depth of the Market</b> for CME Group of exchanges (CME, CBOT, COMEX, and NYMEX) and the EUREX and it comes shipped like a 64 bit Windows application. \
CME is delayed by 10 minutes and 10 seconds while the EUREX is delayed by 15 minutes.

To do predictions and analysis having the correct and <b>reliable</b> data is crucial and SierraChart datafeed just gets it more than right !
SierraChart C++ codes need to be <i>compiled</i> as DLL files and they are supposed to be saved into <b>C:/YOUR_PATH_TO_SIERRACHART/SierraChart/Data/</b> folder.

In order to get SierraCharta C++ code to work, please follow these instructions on Windows or Mac.

# SierraChart C++ code compilation on Mac OSX

<b>1.</b> Install <i>Homebrew</i> on you Mac to manage packages by opening the OSX terminal and typing:
> <b> /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)" </b>

<b>2.</b> Once installed Homebrew, please install <i>Mingw</i> which compiles C++ code for Windows by opening the OSX terminal and typing:
> brew install mingw-w64

<b>3.</b> Once  Mingw compiler is installed correctly, put all SierraCharts files contained into <b>C:/YOUR_PATH_TO_SIERRACHART/SierraChart/Data/ACS_Source</b> and your C++ custom study file into same folder.

<b>4.</b> Now, run OSX terminal <b>from</b> the folder you just created at point 3 and execute teh following compiling instruction code:
> /usr/local/Cellar/mingw-w64/6.0.0_2/bin/x86_64-w64-mingw32-g++ -O2 -shared -static -static-libgcc -static-libstdc++ -s -fno-rtti -fno-exceptions -w MY_STUDY_NAME_HERE.cpp -o MY_STUDY_NAME_HERE.dll

On your Mac, you will have your DLL file compiled properly to run on Windows version of SierraChart.

# SierraChart C++ code compilation on Windows
