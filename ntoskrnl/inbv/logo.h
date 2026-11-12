/*
 * PROJECT:     ReactOS Kernel
 * LICENSE:     GPL-2.0-or-later (https://spdx.org/licenses/GPL-2.0-or-later)
 * PURPOSE:     Boot Theme & Animation header
 * COPYRIGHT:   Copyright 2020 Dmitry Borisov (di.sean@protonmail.com)
 *              Copyright 1997-2028 Republic of CATV 95 (catv95@gmail.com)
 */

#pragma once

#include "logo_root.h"
#include "logo_w2k.h"

//
// Boot Splash-Screen Functions
//

CODE_SEG("INIT")
BOOLEAN
NTAPI
BootAnimInitialize(
    _In_ PLOADER_PARAMETER_BLOCK LoaderBlock,
    _In_ ULONG Count);

VOID
NTAPI
BootAnimTickProgressBar(
    _In_ ULONG SubPercentTimes100);

CODE_SEG("INIT")
VOID
NTAPI
DisplayBootBitmap(
    _In_ BOOLEAN TextMode);

VOID
NTAPI
HibernationDisplay(
    _In_ BOOLEAN TextMode
);

VOID
NTAPI
HibernationFinished(
    _In_ BOOLEAN TextMode
);
