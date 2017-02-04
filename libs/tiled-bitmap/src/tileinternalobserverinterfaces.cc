/*
 * Scroom - Generic viewer for 2D data
 * Copyright (C) 2009-2017 Kees-Jan Dijkzeul
 *
 * SPDX-License-Identifier: LGPL-2.1
 */

#include "tileinternalobserverinterfaces.hh"

#include "tileinternal.hh"

////////////////////////////////////////////////////////////////////////
/// TileInitialisationObserver

void TileInitialisationObserver::tileFinished(TileInternal::Ptr)
{
}

void TileInitialisationObserver::tileCreated(TileInternal::Ptr)
{
}

