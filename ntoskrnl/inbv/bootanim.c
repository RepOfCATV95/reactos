/*
 * PROJECT:     ReactOS Kernel
 * LICENSE:     GPL-2.0-or-later (https://spdx.org/licenses/GPL-2.0-or-later)
 * PURPOSE:     Boot Theme & Animation
 * COPYRIGHT:   Copyright 2007 Alex Ionescu (alex.ionescu@reactos.org)
 *              Copyright 2007 Hervé Poussineau (hpoussin@reactos.org)
 *              Copyright 2012-2022 Hermès Bélusca-Maïto
 *              Copyright 2017-2018 Stanislav Motylkov
 *              Copyright 2019-2020 Yaroslav Kibysh
 *              Copyright 1997-2027 Republic of CATV 95 (catv95@gmail.com)
 */

/* INCLUDES ******************************************************************/

#include <ntoskrnl.h>
#include "inbv/logo.h"

/* GLOBALS *******************************************************************/
/*
 * Enable this define when Inbv will support rotating progress bar.
 */
#define INBV_ROTBAR_IMPLEMENTED

extern ULONG ProgressBarLeft, ProgressBarTop;
extern BOOLEAN ShowProgressBar;

#ifdef INBV_ROTBAR_IMPLEMENTED

static KDPC BootAnimDpc;
static KTIMER BootAnimTimer;
static LARGE_INTEGER BootAnimBaseTimer;

// These are variables being renamed from Windows 2000 counterparts
static ULONG AnimBasePosX;
static ULONG AnimBasePosY;
static ULONG AnimBaseWidth;
static ULONG AnimBaseHeight;
static ULONG AnimBaseHalfWidth;
static ULONG AnimBaseProgress;
static ULONG InTextMode;
static UCHAR RotLineBuffer[3200];
static BOOLEAN BootSeqEntered = FALSE;
static BOOLEAN Haxor = FALSE; // shows ReactOS 0.2.x boot screen if true

#endif // INBV_ROTBAR_IMPLEMENTED

/* FUNCTIONS *****************************************************************/

CODE_SEG("INIT")
BOOLEAN
NTAPI
BootAnimInitialize(
    _In_ PLOADER_PARAMETER_BLOCK LoaderBlock,
    _In_ ULONG Count)
{
#if 0
    ULONG i;

    /* Quit if we're already installed */
    if (InbvBootDriverInstalled) return TRUE;

    /* Find bitmap resources in the kernel */
    ResourceCount = min(Count, RTL_NUMBER_OF(ResourceList) - 1);
    for (i = 1; i <= ResourceCount; i++)
    {
        /* Do the lookup */
        ResourceList[i] = FindBitmapResource(LoaderBlock, i);
    }

    /* Set the progress bar ranges */
    InbvSetProgressBarSubset(0, 100);
#endif

    /* Return install state */
    return TRUE;
}

static
VOID
DrawProgressBars(
    _In_ ULONG   SubPercentTimes100,
    _In_ BOOLEAN InROS,
	_In_ ULONG   FillCount
)
{
    UCHAR PBColor;
    int i;
    if ( InROS && !InTextMode )
    {
        /* Set PBColor to white for ReactOS 0.2.x boot screen */
        PBColor = BV_COLOR_WHITE;

        /* If 0%, draw also the left round corner */
        if (SubPercentTimes100 <= 10*100)
        {
            VidSolidColorFill(ProgressBarLeft - 6,
                              ProgressBarTop + 6,
                              ProgressBarLeft - 6,
                              ProgressBarTop - 6 + VID_PROGRESS_BAR_ROS_HEIGHT,
                              PBColor);

            VidSolidColorFill(ProgressBarLeft - 5,
                              ProgressBarTop + 4,
                              ProgressBarLeft - 5,
                              ProgressBarTop - 4 + VID_PROGRESS_BAR_ROS_HEIGHT,
                              PBColor);

            VidSolidColorFill(ProgressBarLeft - 4,
                              ProgressBarTop + 3,
                              ProgressBarLeft - 4,
                              ProgressBarTop - 3 + VID_PROGRESS_BAR_ROS_HEIGHT,
                              PBColor);

            VidSolidColorFill(ProgressBarLeft - 3,
                              ProgressBarTop + 2 ,
                              ProgressBarLeft - 3,
                              ProgressBarTop - 2 + VID_PROGRESS_BAR_ROS_HEIGHT,
                              PBColor);

            VidSolidColorFill(ProgressBarLeft - 2,
                              ProgressBarTop + 1,
                              ProgressBarLeft - 2,
                              ProgressBarTop - 1 + VID_PROGRESS_BAR_ROS_HEIGHT,
                              PBColor);

            VidSolidColorFill(ProgressBarLeft - 1,
                              ProgressBarTop + 1,
                              ProgressBarLeft - 1,
                              ProgressBarTop - 1 + VID_PROGRESS_BAR_ROS_HEIGHT,
                              PBColor);
        }

        /* Fill the progress bar */
        VidSolidColorFill(ProgressBarLeft,
                          ProgressBarTop,
                          ProgressBarLeft + FillCount,
                          ProgressBarTop + VID_PROGRESS_BAR_ROS_HEIGHT,
                          PBColor);

        /* If 100%, draw also the right round corner */
        if (SubPercentTimes100 >= 100*100)
        {
            VidSolidColorFill(ProgressBarLeft + VID_PROGRESS_BAR_ROS_WIDTH + 1,
                              ProgressBarTop + 1,
                              ProgressBarLeft + VID_PROGRESS_BAR_ROS_WIDTH + 1,
                              ProgressBarTop - 1 + VID_PROGRESS_BAR_ROS_HEIGHT,
                              PBColor);

            VidSolidColorFill(ProgressBarLeft + VID_PROGRESS_BAR_ROS_WIDTH + 2,
                              ProgressBarTop + 1,
                              ProgressBarLeft + VID_PROGRESS_BAR_ROS_WIDTH + 2,
                              ProgressBarTop - 1 + VID_PROGRESS_BAR_ROS_HEIGHT,
                              PBColor);

            VidSolidColorFill(ProgressBarLeft + VID_PROGRESS_BAR_ROS_WIDTH + 3,
                              ProgressBarTop + 2 ,
                              ProgressBarLeft + VID_PROGRESS_BAR_ROS_WIDTH + 3,
                              ProgressBarTop - 2 + VID_PROGRESS_BAR_ROS_HEIGHT,
                              PBColor);

            VidSolidColorFill(ProgressBarLeft + VID_PROGRESS_BAR_ROS_WIDTH + 4,
                              ProgressBarTop + 3,
                              ProgressBarLeft + VID_PROGRESS_BAR_ROS_WIDTH + 4,
                              ProgressBarTop - 3 + VID_PROGRESS_BAR_ROS_HEIGHT,
                              PBColor);

            VidSolidColorFill(ProgressBarLeft + VID_PROGRESS_BAR_ROS_WIDTH + 5,
                              ProgressBarTop + 4,
                              ProgressBarLeft + VID_PROGRESS_BAR_ROS_WIDTH + 5,
                              ProgressBarTop - 4 + VID_PROGRESS_BAR_ROS_HEIGHT,
                              PBColor);

            VidSolidColorFill(ProgressBarLeft + VID_PROGRESS_BAR_ROS_WIDTH + 6,
                              ProgressBarTop + 6,
                              ProgressBarLeft + VID_PROGRESS_BAR_ROS_WIDTH + 6,
                              ProgressBarTop - 6 + VID_PROGRESS_BAR_ROS_HEIGHT,
                              PBColor);
        }
    }
    else
    {
        if ( InTextMode )
        {
            /* Set PBColor for /SOS boot screen */
			PBColor = BV_COLOR_LIGHT_BLUE;
        }
        else
        {
            /* Set PBColor depending on different architecture */
            if (SharedUserData->NtProductType == NtProductWinNt)
            {
#if defined(_M_AMD64) || defined(_M_X64) || defined(_M_ARM64)
                PBColor = BV_COLOR_LIGHT_GREEN;
#else
                PBColor = BV_COLOR_RED;
#endif
            }
            else
            {
#if defined(_M_AMD64) || defined(_M_X64) || defined(_M_ARM64)
                PBColor = BV_COLOR_YELLOW;
#else
                PBColor = BV_COLOR_RED;
#endif
            }
        }
        for (i=0;i<FillCount;i++)
        {
            /* Fill the progress bar */
            VidSolidColorFill(ProgressBarLeft + (i * VID_PROGRESS_BAR_EXTRA_WIDTH),
                              ProgressBarTop,
                              ProgressBarLeft + ((i + 1) * VID_PROGRESS_BAR_EXTRA_WIDTH) - 2,
                              ProgressBarTop + VID_PROGRESS_BAR_GEN_HEIGHT,
                              PBColor);
        }
    }
}

/**
 * @brief
 * Ticks the progress bar. Used by InbvUpdateProgressBar() and related.
 *
 * @param[in]   SubPercentTimes100
 * The progress percentage, scaled up by 100.
 *
 * @return None.
 **/
VOID
NTAPI
BootAnimTickProgressBar(
    _In_ ULONG SubPercentTimes100)
{
    ULONG FillCount;

    /* Make sure the progress bar is enabled, that we own and are installed */
    ASSERT(ShowProgressBar &&
           InbvBootDriverInstalled &&
           (InbvGetDisplayState() == INBV_DISPLAY_STATE_OWNED));

    /* Compute fill count */
    if ( Haxor )
    {
        /* Acquire the lock */
        InbvAcquireLock();

        FillCount = VID_PROGRESS_BAR_ROS_SIZE * SubPercentTimes100 / (100 * 100);
        DrawProgressBars(SubPercentTimes100, TRUE, FillCount);

        /* Release the lock */
        InbvReleaseLock();
    }
    else
    {
        /* Acquire the lock */
        InbvAcquireLock();

        FillCount = VID_PROGRESS_BAR_EXTRA_SIZE * SubPercentTimes100 / (100 * 100);
        DrawProgressBars(SubPercentTimes100, FALSE, FillCount);

        /* Release the lock */
        InbvReleaseLock();
    }   
}

#ifdef INBV_ROTBAR_IMPLEMENTED
static
VOID
NTAPI
InbvRotationThread(
    _In_ PKDPC Dpc,
    _In_ PVOID Context,
    _In_ PVOID SystemArgument1,
    _In_ PVOID SystemArgument2
)
{
    UNREFERENCED_PARAMETER(Dpc);
    UNREFERENCED_PARAMETER(Context);
    UNREFERENCED_PARAMETER(SystemArgument1);
    UNREFERENCED_PARAMETER(SystemArgument2);

    InbvCheckDisplayOwnership();
    if (InbvGetDisplayState() == INBV_DISPLAY_STATE_OWNED)
    {
        /* Acquire the lock */
        InbvAcquireLock();

        /* Left side of bar */
        InbvBufferToScreenBlt(RotLineBuffer,
                              AnimBasePosX + AnimBaseProgress,
                              AnimBasePosY,
                              AnimBaseWidth - AnimBaseProgress,
                              AnimBaseHeight,
                              AnimBaseHalfWidth);

        /* Right side of bar */
        InbvBufferToScreenBlt((PUCHAR)RotLineBuffer + ((ULONG)(AnimBaseWidth - AnimBaseProgress) >> 1),
                              AnimBasePosX,
                              AnimBasePosY,
                              AnimBaseProgress,
                              AnimBaseHeight,
                              AnimBaseHalfWidth);

        /* Increment the anim index */
        AnimBaseProgress+=4;
        if (AnimBaseProgress>=AnimBaseWidth)
            AnimBaseProgress=0;

        /* Release the lock */
        InbvReleaseLock();
    }
    else
    {
        KeCancelTimer(&BootAnimTimer);
    }
}
#endif // INBV_ROTBAR_IMPLEMENTED

CODE_SEG("INIT")
static
VOID
NTAPI
DisplayFilter(
    _Inout_ PCSTR* String)
{
    /* Windows hack to skip first dots displayed by AUTOCHK */
    static BOOLEAN NoNT4 = FALSE;

    /* If "." is given set *String to empty string */
    if (NoNT4==FALSE && strcmp(*String, ".") == 0)
        *String = "";

    if (**String)
    {
        /* Remove the filter */
        InbvInstallDisplayStringFilter(NULL);

        /* Turn off hacks */
        NoNT4 = TRUE;

        /* Avoid drawing progress bar while in process of entering text-mode boot screen */
        if ( !BootSeqEntered )
            BootSeqEntered = TRUE;

        /* If ReactOS 0.2.x boot screen is shown in Embedded edition, then clear the flag first */
        if ( Haxor )
            Haxor = FALSE;

        /* Stop the timer before entering AUTOCHK boot screen */
        KeCancelTimer(&BootAnimTimer);

        /* Draw text screen */
        DisplayBootBitmap(TRUE);
    }
}

CODE_SEG("INIT")
VOID
NTAPI
DisplayBootBitmap(
    _In_ BOOLEAN TextMode)
{
    PUCHAR Header = NULL;
    PUCHAR Screen = NULL;
    static BOOLEAN FirstCall = TRUE;

    if (TextMode)
    {
        InTextMode = TRUE;
        if ( Haxor )
            Haxor = FALSE;

        /* Is the boot driver installed? */
        if (!InbvBootDriverInstalled) return;

        /* Emulate Windows 2000-styled design of text-mode boot screen */
        InbvSetTextColor(BV_COLOR_WHITE);
        InbvSolidColorFill(0,0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1, BV_COLOR_BLACK);
        InbvSolidColorFill(ROTATION_BAR_TEXTMODE_OFFSET, VID_FOOTER_BG_TOP, VID_BOTTOM_BOX_WIDTH, VID_BOTTOM_BOX_HEIGHT, BV_COLOR_LIGHT_RED);

        if (SharedUserData->NtProductType == NtProductWinNt)
        {
            /* Professional - Get resources */
            Header = InbvGetResourceAddress(IDB_PROFESSIONAL_HEADER);
        }
        else
        {
            /* Server - Get resources */
            Header = InbvGetResourceAddress(IDB_SERVER_HEADER);
        }

        /* Set the scrolling region */
        InbvSetScrollRegion(VID_SCROLL_AREA_LEFT,VID_SCROLL_AREA_TOP,
                            VID_SCROLL_AREA_RIGHT,VID_SCROLL_AREA_BOTTOM);

        /* Set the progress bar coordinates */
        InbvSetProgressBarCoordinates(VID_PROGRESS_BAR_TEXT_LEFT,
                                      VID_PROGRESS_BAR_TEXT_TOP);

        /* Make sure we have an AUTOCHK bitmap */
        if (Header)
        {
            /* BitBlt this bitmap on the screen */
            InbvBitBlt(Header,0,0);
        }

#ifdef INBV_ROTBAR_IMPLEMENTED
        /* Set animation offsets */
        AnimBasePosX      =  ROTATION_BAR_TEXTMODE_OFFSET;
        AnimBasePosY      =  ROTATION_BAR_TEXTMODE_HEIGHT;
        AnimBaseWidth     =  ROTATION_BAR_TEXTMODE_WIDTH;
        AnimBaseHeight    =  ROTATION_BAR_TEXTMODE_SIZE;
        AnimBaseHalfWidth =  ROTATION_BAR_GEN_HALFER;
#endif

        /* Draw four blocks of color representing the progress bar if kernel transitions directly into verbose boot screen */
        if ( FirstCall )
        {
            /* Fill the color blocks representing the progress bar */
            InbvSolidColorFill(237,443,401,443,BV_COLOR_BLUE);
            InbvSolidColorFill(237,454,401,454,BV_COLOR_WHITE);
            InbvSolidColorFill(237,443,237,454,BV_COLOR_BLUE);
            InbvSolidColorFill(401,443,401,454,BV_COLOR_WHITE);
        }
    }
    else
    {
        InTextMode = FALSE;

        /* Is the boot driver installed? */
        if (!InbvBootDriverInstalled) return;

        /* Fill the screen with shade of blue color before drawing the boot screen */
        InbvSolidColorFill(0,0,SCREEN_WIDTH-1,SCREEN_HEIGHT-1,BV_COLOR_GREEN);

        if (ExVerifySuite(EmbeddedNT))
            Haxor = TRUE;
        else
            Haxor = FALSE;

        if (Haxor)
        {
            /* Load the embedded edition boot screen */
            Screen = InbvGetResourceAddress(IDB_BOOT_SCREEN_POWERED);

            /* Set progress bar coordinates */
            InbvSetProgressBarCoordinates(VID_PROGRESS_BAR_NORMAL_ROS_LEFT,
                                          VID_PROGRESS_BAR_NORMAL_ROS_TOP);
        }
        else
        {
            /* Load the client/server boot screen */
            if (SharedUserData->NtProductType == NtProductWinNt)
                Screen = InbvGetResourceAddress(IDB_BOOT_SCREEN);
            else
                Screen = InbvGetResourceAddress(IDB_BOOT_SCREEN_SERVER);

            /* Set progress bar coordinates */
            InbvSetProgressBarCoordinates(VID_PROGRESS_BAR_NORMAL_LEFT,
                                          VID_PROGRESS_BAR_NORMAL_TOP);
        }

        /* Make sure we have a logo */
        if (Screen)
        {
            /* Draw a bitmap */
            InbvBitBlt(Screen,0,0);
        }

#ifdef INBV_ROTBAR_IMPLEMENTED
        if ( Haxor )
        {
            /* Set animation offsets */
            AnimBasePosX      =  ROTATION_BAR_NORMAL_ROS_OFFSET;
            AnimBasePosY      =  ROTATION_BAR_NORMAL_ROS_HEIGHT;
            AnimBaseWidth     =  ROTATION_BAR_NORMAL_ROS_WIDTH;
            AnimBaseHeight    =  ROTATION_BAR_NORMAL_ROS_SIZE;
            AnimBaseHalfWidth =  ROTATION_BAR_GEN_HALFER;
        }
        else
        {
            /* Set animation offsets */
            AnimBasePosX      =  ROTATION_BAR_NORMAL_OFFSET;
            AnimBasePosY      =  ROTATION_BAR_NORMAL_HEIGHT;
            AnimBaseWidth     =  ROTATION_BAR_NORMAL_WIDTH;
            AnimBaseHeight    =  ROTATION_BAR_NORMAL_SIZE;
            AnimBaseHalfWidth =  ROTATION_BAR_GEN_HALFER;
	    }
#endif

        /* Set filter which will draw text display if needed */
        InbvInstallDisplayStringFilter(DisplayFilter);
    }

#ifdef INBV_ROTBAR_IMPLEMENTED
    /* Initialize the animation, store the line region to rotate in global buffer, do the rest and set FirstCall to FALSE */
    AnimBaseProgress = 0;
    InbvScreenToBufferBlt(RotLineBuffer,
                          AnimBasePosX,
                          AnimBasePosY,
                          AnimBaseWidth,
                          AnimBaseHeight,
                          AnimBaseHalfWidth);
    KeInitializeDpc(&BootAnimDpc, InbvRotationThread, NULL);
    KeInitializeTimer(&BootAnimTimer);
    BootAnimBaseTimer.QuadPart = 50 * 10000LL;
    KeSetTimerEx(&BootAnimTimer, BootAnimBaseTimer, 50, &BootAnimDpc);
#endif
    if ( !BootSeqEntered && FirstCall )
    {
        BootSeqEntered = TRUE;
        FirstCall = FALSE;
    }
}
