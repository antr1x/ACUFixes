#pragma once

#include <cstdint>
#include <optional>
#include <vector>

class PatternScanner
{
public:
    explicit PatternScanner(const char* patternText);

    uintptr_t FindFirstInExecutableMemory() const;

private:
    using PatternByte = std::optional<std::uint8_t>;

    std::vector<PatternByte> m_Pattern;

    static std::vector<PatternByte> ParsePattern(const char* patternText);
    uintptr_t FindFirstInCommittedMemory(bool executableOnly) const;
};
