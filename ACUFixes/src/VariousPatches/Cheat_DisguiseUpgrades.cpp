#include "pch.h"

#include "Cheat_DisguiseUpgrades.h"

#include "ACU/BhvAssassin.h"
#include "ACU/ACUGetSingletons.h"

UnbreakableDisguise::UnbreakableDisguise()
{
    DEFINE_ADDR(whenCheckingIfDisguiseShouldBeBrokenByCombat, 0x141ac66b0);
    whenCheckingIfDisguiseShouldBeBrokenByCombat = {
        "C7 02 00000000"        // - mov [rdx],00000000
        "C3"                    // - ret
    };
    DEFINE_ADDR(whenCheckingIfDisguiseShouldBeBrokenByParkour, 0x141AC1930);
    whenCheckingIfDisguiseShouldBeBrokenByParkour = {
        "C7 02 00000000"        // - mov [rdx],00000000
        "C3"                    // - ret
    };
    DEFINE_ADDR(whenCheckingIfDisguiseShouldBeBrokenByBeingSeenToCloselyForTooLong, 0x141AB1920);
    whenCheckingIfDisguiseShouldBeBrokenByBeingSeenToCloselyForTooLong = {
        "48 B8 0000000000000000"    // - mov rax,0000000000000000
        "C3"                        // - ret
        , nop(2)
    };
}
DisableDisguiseCooldown::DisableDisguiseCooldown()
{
    DEFINE_ADDR(whenCheckingIfDisguiseAvailable_part1, 0x142661A4A);
    whenCheckingIfDisguiseAvailable_part1 = {
        "C6 45 E8 01"          // - mov byte ptr [rbp-18],01
        "66 0F1F 44 00 00"     // - nop 6
    };
    DEFINE_ADDR(whenCheckingIfDisguiseAvailable_part2, 0x142661B54);
    whenCheckingIfDisguiseAvailable_part2 = {
        "66 0F1F 44 00 00"     // - nop 6
    };
}
DisguiseDoesntMakeYouInvisible::DisguiseDoesntMakeYouInvisible()
{
    uintptr_t whenDisguiseActivated_MakeActorInvisible = 0x141AD5FD0;
    PresetScript_NOP(whenDisguiseActivated_MakeActorInvisible, 5);
}
void Cheat_Invisibility_DrawImGui()
{
    BhvAssassin* bhv = ACU::GetPlayerBhvAssassin();
    if (!bhv)
    {
        return;
    }
    bool isInvisible = bhv->bInvisible & 1;
    if (ImGui::Checkbox("Invisible to enemies", &isInvisible))
    {
        bhv->bInvisible = isInvisible;
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip(
            "Makes you just as invisible as if you were using Disguise."
        );
    }
}
