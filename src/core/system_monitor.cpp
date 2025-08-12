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
static double lastCpuUsage = 0.0;
static DWORD lastCpuTime = 0;

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
    
    DWORD currentTime = GetTickCount();
    if (currentTime - lastCpuTime < 1000) {
        return lastCpuUsage; // 使用快取值
    }
    
    PDH_FMT_COUNTERVALUE counterVal;
    if (PdhCollectQueryData(cpuQuery) == ERROR_SUCCESS &&
        PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal) == ERROR_SUCCESS) {
        lastCpuUsage = counterVal.doubleValue;
        lastCpuTime = currentTime;
    }
    return lastCpuUsage;
}

SIZE_T GetMemoryUsage() {
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        return pmc.PrivateUsage; // 使用私有記憶體，更精確
    }
    return 0;
}

void GetSystemResourceInfo(char* buffer, int bufferSize) {
    if (!monitorInitialized) InitSystemMonitor();
    
    double cpuUsage = GetCPUUsage();
    SIZE_T memUsage = GetMemoryUsage();
    
    // 轉換記憶體使用量為 KB，使用更簡潔的格式
    int memUsageKB = (int)(memUsage / 1024);
    int cpuPercent = (int)(cpuUsage + 0.5);  // 四捨五入
    
    snprintf(buffer, bufferSize, 
        "CPU: %d%% | Mem: %dKB", 
        cpuPercent, memUsageKB);
}