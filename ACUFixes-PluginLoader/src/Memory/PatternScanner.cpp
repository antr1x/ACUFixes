#include "pch.h"

#include "Memory/PatternScanner.h"

#include <cctype>

namespace
{
bool IsReadableCommittedPage(const MEMORY_BASIC_INFORMATION& mbi)
{
    if (mbi.State != MEM_COMMIT)
    {
        return false;
    }
    if ((mbi.Protect & PAGE_GUARD) || (mbi.Protect & PAGE_NOACCESS))
    {
        return false;
    }

    const DWORD protect = mbi.Protect & 0xFF;
    return protect == PAGE_READONLY ||
        protect == PAGE_READWRITE ||
        protect == PAGE_WRITECOPY ||
        protect == PAGE_EXECUTE ||
        protect == PAGE_EXECUTE_READ ||
        protect == PAGE_EXECUTE_READWRITE ||
        protect == PAGE_EXECUTE_WRITECOPY;
}

bool IsExecutableCommittedPage(const MEMORY_BASIC_INFORMATION& mbi)
{
    if (!IsReadableCommittedPage(mbi))
    {
        return false;
    }

    const DWORD protect = mbi.Protect & 0xFF;
    return protect == PAGE_EXECUTE ||
        protect == PAGE_EXECUTE_READ ||
        protect == PAGE_EXECUTE_READWRITE ||
        protect == PAGE_EXECUTE_WRITECOPY;
}
}

PatternScanner::PatternScanner(const char* patternText)
    : m_Pattern(ParsePattern(patternText))
{
}

std::vector<PatternScanner::PatternByte> PatternScanner::ParsePattern(const char* patternText)
{
    std::vector<PatternByte> pattern;
    if (!patternText)
    {
        return pattern;
    }

    std::string token;
    const auto flushToken = [&]()
        {
            if (token.empty())
            {
                return;
            }

            if (token == "?" || token == "??")
            {
                pattern.emplace_back(std::nullopt);
            }
            else
            {
                pattern.emplace_back(static_cast<std::uint8_t>(std::strtoul(token.c_str(), nullptr, 16)));
            }
            token.clear();
        };

    for (const char* p = patternText; *p; ++p)
    {
        const unsigned char c = static_cast<unsigned char>(*p);
        if (std::isxdigit(c) || c == '?')
        {
            token.push_back(static_cast<char>(std::toupper(c)));
            continue;
        }

        flushToken();
    }

    flushToken();
    return pattern;
}

uintptr_t PatternScanner::FindFirstInExecutableMemory() const
{
    return FindFirstInCommittedMemory(true);
}

uintptr_t PatternScanner::FindFirstInCommittedMemory(bool executableOnly) const
{
    if (m_Pattern.empty())
    {
        return 0;
    }

    const std::size_t patternSize = m_Pattern.size();
    auto* cursor = reinterpret_cast<std::uint8_t*>(0x10000);
    MEMORY_BASIC_INFORMATION mbi{};
    while (VirtualQuery(cursor, &mbi, sizeof(mbi)) == sizeof(mbi))
    {
        const bool shouldScan = executableOnly ? IsExecutableCommittedPage(mbi) : IsReadableCommittedPage(mbi);
        if (shouldScan)
        {
            auto* begin = reinterpret_cast<const std::uint8_t*>(mbi.BaseAddress);
            const std::size_t size = mbi.RegionSize;
            if (size >= patternSize)
            {
                auto* end = begin + size;
                for (auto* it = begin; it <= end - patternSize; ++it)
                {
                    bool isMatch = true;
                    for (std::size_t i = 0; i < patternSize; ++i)
                    {
                        const auto& expected = m_Pattern[i];
                        if (expected && it[i] != *expected)
                        {
                            isMatch = false;
                            break;
                        }
                    }
                    if (isMatch)
                    {
                        return reinterpret_cast<uintptr_t>(it);
                    }
                }
            }
        }

        cursor = reinterpret_cast<std::uint8_t*>(reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize);
    }

    return 0;
}
