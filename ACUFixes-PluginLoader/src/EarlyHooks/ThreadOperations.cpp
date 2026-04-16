#include "pch.h"

#include "ThreadOperations.h"
#include "Memory/PatternScanner.h"
#include "ACU/Threads/KnownThreads.h"
#include <cstdint>
#pragma comment(lib, "ntdll")

#define ThreadQuerySetWin32StartAddress 0x9
uintptr_t GetThreadStartAddress(HANDLE thread_handle)
{
    uintptr_t dwStartAddress;
    NTSTATUS nt_status = NtQueryInformationThread(thread_handle, (THREADINFOCLASS)ThreadQuerySetWin32StartAddress, &dwStartAddress, 0x8, 0);
    if (nt_status == 0)
    {
        return dwStartAddress;
    }
    return 0;
}

namespace
{
struct MainIntegrityCheckPattern
{
    const char* m_Label;
    const char* m_Pattern;
};

constexpr MainIntegrityCheckPattern g_MainIntegrityCheckPatterns[] = {
    { "pGameProtect1", "E9 1F 81 3F 05" },
    { "pGameProtect2", "E9 E5 F1 18 03" },
    { "pGameProtect3", "E8 2C A4 27 03" },
};
}

uintptr_t ResolveSingleRel32BranchTarget(uintptr_t address)
{
    if (!address)
    {
        return 0;
    }

    const auto* code = reinterpret_cast<const std::uint8_t*>(address);
    const std::uint8_t opcode = code[0];
    if (opcode != 0xE8 && opcode != 0xE9)
    {
        return 0;
    }

    const std::int32_t displacement = *reinterpret_cast<const std::int32_t*>(code + 1);
    return address + 5 + displacement;
}

bool IsEquivalentThreadStartAddress(uintptr_t candidateStartAddress, uintptr_t expectedStartAddress)
{
    if (!candidateStartAddress || !expectedStartAddress)
    {
        return false;
    }
    if (candidateStartAddress == expectedStartAddress)
    {
        return true;
    }

    const uintptr_t candidateResolvedTarget = ResolveSingleRel32BranchTarget(candidateStartAddress);
    const uintptr_t expectedResolvedTarget = ResolveSingleRel32BranchTarget(expectedStartAddress);

    if (candidateResolvedTarget && candidateResolvedTarget == expectedStartAddress)
    {
        return true;
    }
    if (expectedResolvedTarget && candidateStartAddress == expectedResolvedTarget)
    {
        return true;
    }
    if (candidateResolvedTarget && expectedResolvedTarget && candidateResolvedTarget == expectedResolvedTarget)
    {
        return true;
    }
    return false;
}

uintptr_t ResolveMainIntegrityCheckThreadStartAddress()
{
    static uintptr_t cached = 0;
    static ULONGLONG lastAttemptTick = 0;
    if (cached)
        return cached;

    const ULONGLONG now = GetTickCount64();
    if (lastAttemptTick && (now - lastAttemptTick) < 250)
    {
        return ACU::Threads::ThreadStartAddr_MainIntegrityCheck;
    }
    lastAttemptTick = now;

    for (const MainIntegrityCheckPattern& pattern : g_MainIntegrityCheckPatterns)
    {
        if (const uintptr_t resolved = PatternScanner(pattern.m_Pattern).FindFirstInExecutableMemory())
        {
            cached = resolved;
            return cached;
        }
    }

    return ACU::Threads::ThreadStartAddr_MainIntegrityCheck;
}
