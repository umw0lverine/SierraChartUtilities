//#include "windows.h"

#ifndef GLOBAL_H

#define GLOBAL_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef UNICODE
#define UNICODE
#endif

//#ifndef _WIN32_WINNT
//#define _WIN32_WINNT 0x0502
//#endif

#include "sierrasource.h"

// Extern Variables

// Extern Functions
extern inline int GetDecimals(SCStudyInterfaceRef sc);
extern int IsDepthAvailable(SCStudyInterfaceRef sc,const int);
extern bool DOMOutlierSpotter(const float sdMultiplier, const int totalDOMVolume, const int DOMFirstLevelVolume, const int DOMLevels);
extern bool WhiteGhostSpotter(SCStudyInterfaceRef sc, const int side, const int DOMLevels);
extern void drawSignalLines(SCStudyInterfaceRef sc, const int tradeType);
extern void initBidAskMaps(SCStudyInterfaceRef sc);
extern void initBidAskDepthArrays(SCStudyInterfaceRef sc, const int vDomLevels);
extern void deleteBidAskMaps(SCStudyInterfaceRef sc);
extern void deleteBidAskDepthArrays(SCStudyInterfaceRef sc);

#define commonVars(sc) \
    int pvars=-1; \
    int subgraphvars=-1; \
    int inputvars=-1; \
    SCString ftext = "", ltext = ""; \

#define commonStruct(sc) \
    std::map <float, int> *p_AskSideBigPrintsMap = static_cast<std::map <float, int>*>(sc.GetPersistentPointer(AMP)); \
    std::map <float, int> *p_BidSideBigPrintsMap = static_cast<std::map <float, int>*>(sc.GetPersistentPointer(BMP)); \
    int * p_AskDepthArray = static_cast<int*>(sc.GetPersistentPointer(ADP)); \
    int * p_BidDepthArray = static_cast<int*>(sc.GetPersistentPointer(BDP)); \
    s_MarketDepthEntry DepthEntry;
#endif // GLOBAL_H
