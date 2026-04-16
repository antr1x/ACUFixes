#include "pch.h"

#include "EarlyHooks/ThreadOperations.h"

bool TerminateThreadIfRunsTheMainIntegrityCheck(ThreadID_t thread_id)
{
    HANDLE thread_handle = OpenThread(THREAD_ALL_ACCESS, 0, thread_id);
    if (!thread_handle)
        return false;

    const uintptr_t startAddress = GetThreadStartAddress(thread_handle);
    const uintptr_t expectedMICStartAddress = ResolveMainIntegrityCheckThreadStartAddress();
    bool wasTerminated = false;
    if (startAddress == expectedMICStartAddress)
    {
        TerminateThread(thread_handle, 0);
        wasTerminated = true;
    }
    CloseHandle(thread_handle);
    return wasTerminated;
}

// I originally wanted to just update old logic as-is, but after some headache
// during the 1.5.1 port I decided to rework it into what feels like better solution.
// The old MIC path was getting too annoying to babysit yet again, so instead of dragging
// that mess any further, so now we are waiting for the MIC thread to show up, kill it by its start
// adress, and only after that let the rest of ACUFixes continue as usual.
//
// Kinda brute-force, but practical, and way less annoying to maintain on this build.
// This implementation is based on the ACU CE table:
// https://fearlessrevolution.com/viewtopic.php?t=12725
bool DisableMainIntegrityCheck()
{
    bool wasFound = false;
    ForEachThreadIDOfThisProcess([&](ThreadID_t thread_id) {
        const bool isFoundAndTerminated = TerminateThreadIfRunsTheMainIntegrityCheck(thread_id);
        if (isFoundAndTerminated)
        {
            wasFound = true;
        }
        return isFoundAndTerminated;
        });
    return wasFound;
}

extern bool g_IsMICDisabled;
void WaitUntilMICIsDisabled()
{
    while (true)
    {
        if (g_IsMICDisabled)
            return;

        if (DisableMainIntegrityCheck())
        {
            g_IsMICDisabled = true;
            break;
        }
        Sleep(10);
    }
}

#include <winternl.h>
void ClearThe_BeingDebugged_Flag()
{
    NtCurrentTeb()->ProcessEnvironmentBlock->BeingDebugged = 0;
}
