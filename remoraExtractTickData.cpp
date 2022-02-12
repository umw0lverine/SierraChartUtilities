#include "header.h"

SCSFExport scsf_WriteTickData(SCStudyInterfaceRef sc) {

    SCString ftext, ltext;
    SCString OutputPathAndFileName;
    DWORD    numOfWrittenByte;

    SCInputRef    MaxDOMLevels = sc.Input[0];

    const int hFileID = 0;

    HANDLE hFile = reinterpret_cast<HANDLE>(sc.GetPersistentInt(0));
    int& Sequence = sc.GetPersistentInt(1);
    int& DepthSequence = sc.GetPersistentInt(2);
    int& vDomLevels = sc.GetPersistentInt(3);
    int& ReadyStatus = sc.GetPersistentInt(4);
    SCString& domftext = sc.GetPersistentSCString(5);
    SCString& domltext = sc.GetPersistentSCString(6);
    int& Decimals = sc.GetPersistentInt(7);

    if (sc.SetDefaults) {
        sc.GraphName = "Write Tick Data including L2 Data";
        sc.StudyDescription = "Write Tick Data including L2 Data";

        sc.GraphRegion = 0;

        // Inputs...
        MaxDOMLevels.Name = "DOM Levels for the instrument used";
        MaxDOMLevels.SetInt(10);
        //MaxDOMLevels.SetIntLimits(1, sc.GetMaximumMarketDepthLevels());

        sc.TextInputName = "File Path";

        sc.AutoLoop = 0;

        sc.UsesMarketDepthData = 1;

        //sc.MaintainAdditionalChartDataArrays = 1;

        return;
    }

    if (sc.UpdateStartIndex == 0) {
        ReadyStatus = 0;
        vDomLevels = MaxDOMLevels.GetInt();
        Decimals = GetDecimals(sc);

        if (sc.TextInput.GetLength() > 0) {
            OutputPathAndFileName = sc.TextInput;
        }
        else {
            int year, month, day;
            int hour, minute, second, microsecond;
            //SCDateTimeMS BaseDateTimeIn = TS[tsSize].DateTime; //sc.LatestDateTimeForLastBar;
            SCDateTimeMS BaseDateTimeIn = sc.LatestDateTimeForLastBar;

            BaseDateTimeIn.GetDateTimeYMDHMS_US(year, month, day, hour, minute, second, microsecond);
            OutputPathAndFileName.Format("%s_%d%02d%02d_%02d%02d%02d.txt", sc.Symbol.GetChars(), year, month, day, hour, minute, second);
        }

        hFile = CreateFileA(OutputPathAndFileName.GetChars(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, nullptr);

        if (hFile == INVALID_HANDLE_VALUE) {
            CloseHandle(hFile);
            ftext.Format("Write Bar Data To File study: Error creating output data file: %s", OutputPathAndFileName.GetChars());
            sc.AddMessageToLog(ftext, 1);

            hFile = nullptr;
            return;
        }

        ftext = "";
        //ltext.Format("Date;Time;Price;Volume;TradeType;AskPrice;AskSize;BidPrice;BidSize;TotalAskDepth;TotalBidDepth"); ftext += ltext;
        //ltext.Format("Date;Time;Price;Volume;TradeType;AskPrice;BidPrice;TotalAskDepth;TotalBidDepth"); ftext += ltext;
        //ltext.Format("Date;Time;Sequence;DepthSequence;Price;Volume;TradeType;AskPrice;BidPrice;PreTradeAskSize;AskSize;PreTradeBidSize;BidSize"); ftext += ltext;
        //ltext.Format("Date;Time;Sequence;DepthSequence;Price;Volume;TradeType;AskPrice;BidPrice;AskSize;BidSize"); ftext += ltext;
        ltext.Format("Date;Time;Sequence;DepthSequence;Price;Volume;TradeType;AskPrice;BidPrice;AskSize;BidSize;TotalAskDepth;TotalBidDepth;AskDOMPrice;BidDOMPrice");
        ftext += ltext;

        for (int domLevel = 0; domLevel < vDomLevels; ++domLevel) {
            ltext.Format(";AskDOM_%d", domLevel);
            ftext += ltext;
            ltext.Format(";BidDOM_%d", domLevel);
            ftext += ltext;
        }

        ltext.Format("\r\n");
        ftext += ltext;

        //SetFilePointer(hFile, 0, NULL, FILE_END);
        SetFilePointer(hFile, 0, nullptr, FILE_BEGIN);
        sc.SetPersistentInt(hFileID, reinterpret_cast<int>(hFile));

        WriteFile(hFile, ftext.GetChars(), static_cast<DWORD>(ftext.GetLength()), &numOfWrittenByte, nullptr);

        return;
    }

    if (sc.LastCallToFunction) {
        if (hFile) {
            CloseHandle(hFile);
        }

        return;
    }

    if (!ReadyStatus) {

        if (sc.Index < sc.ArraySize - 1) return;

        if (!hFile) {
            sc.AddMessageToLog("Fatal Error Creating Datafile", 1);
            return;
        }

        ReadyStatus = IsDepthAvailable(sc,vDomLevels);
        if (!ReadyStatus) return;

        c_SCTimeAndSalesArray TS;
        sc.GetTimeAndSales(TS);
        const int tsSize = TS.Size() - 1;

        Sequence = max(1, static_cast<int>(TS[tsSize].Sequence));
        DepthSequence = Sequence;
    }

    c_SCTimeAndSalesArray TS;
    sc.GetTimeAndSales(TS);
    const int tsSize = TS.Size() - 1;

    if (static_cast<int>(TS[tsSize].Sequence) <= Sequence)
        return;

    const int priorSequence = Sequence;
    Sequence = static_cast<int>(TS[tsSize].Sequence);

    int localIdx = tsSize;

    while (static_cast<int>(TS[localIdx].Sequence) > priorSequence && localIdx > 0) {
        --localIdx;
    }

    for (int TSIndex = localIdx + 1; TSIndex <= tsSize; ++TSIndex) {
        if (TS[TSIndex].Type == SC_TS_BIDASKVALUES) {

            s_MarketDepthEntry DepthEntry;
            DepthSequence = static_cast<int>(TS[TSIndex].Sequence);
            domftext = domltext = "";

            domltext.Format(";%d", TS[TSIndex].TotalAskDepth);
            domftext += domltext;
            domltext.Format(";%d", TS[TSIndex].TotalBidDepth);
            domftext += domltext;
            domltext.Format(";%.*f", Decimals, static_cast<double>(sc.Ask) * sc.RealTimePriceMultiplier);
            domftext += domltext;
            domltext.Format(";%.*f", Decimals, static_cast<double>(sc.Bid) * sc.RealTimePriceMultiplier);
            domftext += domltext;

            for (int domLevel = 0; domLevel < vDomLevels; ++domLevel) {
                sc.GetAskMarketDepthEntryAtLevel(DepthEntry, domLevel);
                const int AskLevelSize = static_cast<int>(DepthEntry.Quantity);
                sc.GetBidMarketDepthEntryAtLevel(DepthEntry, domLevel);
                const int BidLevelSize = static_cast<int>(DepthEntry.Quantity);

                domltext.Format(";%d;%d", AskLevelSize, BidLevelSize);
                domftext += domltext;
            }

            continue;
        }

        int year, month, day;
        int hour, minute, second, microsecond;
        SCDateTimeMS TSRecDT = TS[TSIndex].DateTime; // + sc.TimeScaleAdjustment;

        TSRecDT.GetDateTimeYMDHMS_US(year, month, day, hour, minute, second, microsecond);

        ftext = "";
        ltext.Format("%d-%02d-%02d", year, month, day);
        ftext += ltext;
        ltext.Format(";%02d:%02d:%02d.%03d", hour, minute, second, microsecond);
        ftext += ltext;
        ltext.Format(";%d", TS[TSIndex].Sequence);
        ftext += ltext;
        ltext.Format(";%d", DepthSequence);
        ftext += ltext;
        ltext.Format(";%.*f", Decimals, static_cast<double>(TS[TSIndex].Price) * sc.RealTimePriceMultiplier);
        ftext += ltext;
        ltext.Format(";%d", TS[TSIndex].Volume);
        ftext += ltext;
        ltext.Format(";%d", TS[TSIndex].Type);
        ftext += ltext;
        ltext.Format(";%.*f", Decimals, static_cast<double>(TS[TSIndex].Ask) * sc.RealTimePriceMultiplier);
        ftext += ltext;
        ltext.Format(";%.*f", Decimals, static_cast<double>(TS[TSIndex].Bid) * sc.RealTimePriceMultiplier);
        ftext += ltext;
        ltext.Format(";%d", TS[TSIndex].AskSize);
        ftext += ltext;
        ltext.Format(";%d", TS[TSIndex].BidSize);
        ftext += ltext;

        ftext += domftext;

        ltext.Format("\r\n");
        ftext += ltext;

        WriteFile(hFile, ftext.GetChars(), static_cast<DWORD>(ftext.GetLength()), &numOfWrittenByte, nullptr);
    }

    return;
}
