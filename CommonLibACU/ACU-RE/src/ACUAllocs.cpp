#include "pch.h"

#include "ACU/Memory/ACUAllocs.h"

DEFINE_GAME_FUNCTION(Allocate_mb, 0x1426F3870, void*, __fastcall, (__int64 p_numBytes, __int64 p_alignment_mb, void* currentHeap_mb));
DEFINE_GAME_FUNCTION(AllocString_mb_orBytes, 0x140004510, void*, __fastcall, (__int64 p_numBytes, __int64 p_alignment_mb, void* resultOf0x1426F3870));
DEFINE_GAME_FUNCTION(MemoryManager__GetSomeFlagsBeforeDealloc, 0x142102CB0, __int64, __fastcall, (__int64 a1, byte* allocated));


namespace ACU::Memory
{

byte* ACUAllocateBytes(uint32 numBytes, uint32 alignment)
{
    void* g_currentHeap_mb = *(void**)0x14525a510;
    void* res1 = Allocate_mb(numBytes, alignment, g_currentHeap_mb);
    void* res2 = AllocString_mb_orBytes(numBytes, alignment, res1);
    return (byte*)res2;
}
// Every allocation and deallocation takes 2 calls. What the hell is going on here?
class SomeOTHERMemoryManager
{
public:
    // @vtbl
    virtual void Unk_000();
    virtual void Unk_008();
    virtual void Unk_010();
    virtual void Unk_018();
    virtual void Unk_020();
    virtual void Unk_028_Deallocate(byte* allocated, __int64 resultOf0x142102CB0);

    // @helper_functions
    static SomeOTHERMemoryManager* GetSingleton() { return **(SomeOTHERMemoryManager***)0x0000000145223C68; }
};
// An implementation can be seen at `CString::Alloc()` at 0x14250d030
// or at 0x14271b5e4
void ACUDeallocateBytes(byte* allocated)
{
    __int64 result1 = MemoryManager__GetSomeFlagsBeforeDealloc(*(__int64*)0x145223D78, allocated);
    ACU::Memory::SomeOTHERMemoryManager::GetSingleton()->Unk_028_Deallocate(allocated, result1);
}

} // namespace ACU::Memory
