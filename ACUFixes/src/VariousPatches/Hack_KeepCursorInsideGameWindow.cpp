#include "pch.h"

#include "ACU/ACUGetSingletons.h"
#include "MainConfig.h"

namespace
{
bool g_IsCursorCurrentlyClipped = false;
RECT g_LastClipRect{};
HWND g_LastClippedWindow = nullptr;

bool AreRectsEqual(const RECT& a, const RECT& b)
{
    return
        a.left == b.left
        && a.top == b.top
        && a.right == b.right
        && a.bottom == b.bottom;
}

void ReleaseCursorClip()
{
    if (!g_IsCursorCurrentlyClipped)
    {
        return;
    }

    ::ClipCursor(nullptr);
    g_IsCursorCurrentlyClipped = false;
    g_LastClipRect = {};
    g_LastClippedWindow = nullptr;
}

bool IsGameWindowFocused(HWND hwnd)
{
    HWND focused = ::GetForegroundWindow();
    return focused && (focused == hwnd || ::IsChild(hwnd, focused));
}

std::optional<RECT> GetClientRectInScreenSpace(HWND hwnd)
{
    RECT rect{};
    if (!::GetClientRect(hwnd, &rect))
    {
        return std::nullopt;
    }

    POINT topLeft{ rect.left, rect.top };
    POINT bottomRight{ rect.right, rect.bottom };

    if (!::ClientToScreen(hwnd, &topLeft) || !::ClientToScreen(hwnd, &bottomRight))
    {
        return std::nullopt;
    }

    rect.left = topLeft.x;
    rect.top = topLeft.y;
    rect.right = bottomRight.x;
    rect.bottom = bottomRight.y;

    if (rect.left >= rect.right || rect.top >= rect.bottom)
    {
        return std::nullopt;
    }

    return rect;
}

bool IsClipStillApplied(HWND hwnd, const RECT& expectedRect)
{
    if (!g_IsCursorCurrentlyClipped || hwnd != g_LastClippedWindow)
    {
        return false;
    }

    RECT currentRect{};
    if (!::GetClipCursor(&currentRect))
    {
        return false;
    }

    return AreRectsEqual(currentRect, expectedRect);
}

struct CursorClipCleanup
{
    ~CursorClipCleanup()
    {
        ReleaseCursorClip();
    }
} g_CursorClipCleanup;
}

void DoKeepCursorInsideGameWindow()
{
    if (!g_Config.hacks->keepCursorInsideGameWindow.get())
    {
        ReleaseCursorClip();
        return;
    }

    HWND hwnd = (HWND)ACU::GetWindowHandle();
    if (!::IsWindow(hwnd) || !IsGameWindowFocused(hwnd))
    {
        ReleaseCursorClip();
        return;
    }

    if (g_IsCursorCurrentlyClipped && hwnd != g_LastClippedWindow)
    {
        ReleaseCursorClip();
    }

    std::optional<RECT> clipRect = GetClientRectInScreenSpace(hwnd);
    if (!clipRect)
    {
        ReleaseCursorClip();
        return;
    }

    if (AreRectsEqual(*clipRect, g_LastClipRect) && IsClipStillApplied(hwnd, *clipRect))
    {
        return;
    }

    if (::ClipCursor(&clipRect.value()))
    {
        g_IsCursorCurrentlyClipped = true;
        g_LastClipRect = *clipRect;
        g_LastClippedWindow = hwnd;
    }
}

void DrawKeepCursorInsideGameWindowControls()
{
    ImGui::Checkbox("Keep cursor inside game window", &g_Config.hacks->keepCursorInsideGameWindow.get());
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip(
            "Clips the mouse cursor to the game window while the game is focused.\n"
            "Useful if you play in a window or borderless mode and don't want the cursor\n"
            "to wander onto another monitor or even outside of it."
        );
    }
}
