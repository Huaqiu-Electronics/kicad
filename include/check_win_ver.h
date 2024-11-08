#ifndef CAF5A0B3_2520_4DFE_B379_885504035123
#define CAF5A0B3_2520_4DFE_B379_885504035123


#include <windows.h>
#include <VersionHelpers.h>

inline bool IsWindows11OrGreater()
{
    OSVERSIONINFOEXW osvi = {};
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
    osvi.dwMajorVersion = 10; // Windows 10 起始主版本号为10
    osvi.dwMinorVersion = 0;  // Windows 10 和 11 都是 Minor Version 为 0
    osvi.dwBuildNumber = 22000; // Windows 11 起始内部版本号为22000

    DWORDLONG conditionMask = 0;
    conditionMask = VerSetConditionMask(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    conditionMask = VerSetConditionMask(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    conditionMask = VerSetConditionMask(conditionMask, VER_BUILDNUMBER, VER_GREATER_EQUAL);

    return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER, conditionMask);
}

#endif /* CAF5A0B3_2520_4DFE_B379_885504035123 */
