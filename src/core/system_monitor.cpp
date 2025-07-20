#ifndef GHOSTDESK_EXPORTS
#define GHOSTDESK_EXPORTS
#endif
#include "../../include/ghostdesk_api.h"
#include <psapi.h>
#include <pdh.h>
#include <cstdio>

static PDH_HQUERY cpuQuery;
static PDH_HCOUNTER cpuTotal;
static bool monitorInitialized = false;

BOOL InitSystemMonitor() {
    if (monitorInitialized) return TRUE;
    
    PdhOpenQuery(NULL, 0, &cpuQuery);
    PdhAddEnglishCounterA(cpuQuery, "\\Processor(_Total)\\% Processor Time", 0, &cpuTotal);
    PdhCollectQueryData(cpuQuery);
    
    monitorInitialized = true;
    return TRUE;
}

void CleanupSystemMonitor() {
    if (monitorInitialized) {
        PdhCloseQuery(cpuQuery);
        monitorInitialized = false;
    }
}

double GetCPUUsage() {
    if (!monitorInitialized) return 0.0;
    
    PDH_FMT_COUNTERVALUE counterVal;
    PdhCollectQueryData(cpuQuery);
    PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
    return counterVal.doubleValue;
}

SIZE_T GetMemoryUsage() {
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize;
    }
    return 0;
}

void GetSystemResourceInfo(char* buffer, int bufferSize) {
    if (!monitorInitialized) InitSystemMonitor();
    
    double cpuUsage = GetCPUUsage();
    SIZE_T memUsage = GetMemoryUsage();
    
    // 轉換記憶體使用量為 KB
    double memUsageKB = memUsage / 1024.0;
    
    snprintf(buffer, bufferSize, 
        "CPU: %.1f%% | Memory: %.1f KB", 
        cpuUsage, memUsageKB);
}