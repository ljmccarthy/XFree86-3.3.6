/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach64/mach64cmap.c,v 3.9.2.1 1998/12/22 07:49:52 hohndel Exp $ */
/*
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 * Copyright 1993,1994 by Kevin E. Martin, Chapel Hill, North Carolina.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Thomas Roell not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Thomas Roell makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 *
 * THOMAS ROELL, KEVIN E. MARTIN, AND RICKARD E. FAITH DISCLAIM ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL THOMAS
 * ROELL OR KEVIN E. MARTIN BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 *
 * Rewritten for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 * Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
 * Modified for the Mach64 by Kevin E. Martin (martin@cs.unc.edu)
 *
 */
/* $XConsortium: mach64cmap.c /main/8 1996/10/27 11:46:41 kaleb $ */

#include "X.h"
#include "Xproto.h"
#include "scrnintstr.h"
#include "colormapst.h"
#include "windowstr.h"
#include "compiler.h"
#include "mach64.h"

#ifdef XFreeXDGA
#include "scrnintstr.h"
#include "servermd.h"
#define _XF86DGA_SERVER_
#include "extensions/xf86dgastr.h"
#endif

#define NOMAPYET        (ColormapPtr) 0

static ColormapPtr InstalledMaps[MAXSCREENS];
				/* current colormap for each screen */

int
mach64ListInstalledColormaps(pScreen, pmaps)
     ScreenPtr	pScreen;
     Colormap	*pmaps;
{
  /* By the time we are processing requests, we can guarantee that there
   * is always a colormap installed */
  
  *pmaps = InstalledMaps[pScreen->myNum]->mid;
  return(1);
}

int
mach64GetInstalledColormaps(pScreen, pmap)
     ScreenPtr		pScreen;
     ColormapPtr	*pmap;
{
  *pmap = InstalledMaps[pScreen->myNum];
  return(1);
}

void
mach64StoreColors(pmap, ndef, pdefs)
     ColormapPtr	pmap;
     int		ndef;
     xColorItem	        *pdefs;
{
    int		i;
    xColorItem	directDefs[256];
    extern LUTENTRY mach64savedLUT[256];
    extern unsigned char xf86rGammaMap[], xf86gGammaMap[], xf86bGammaMap[];
    LUTENTRY entry;

    if (pmap != InstalledMaps[pmap->pScreen->myNum])
	return;

    if ((pmap->pVisual->class | DynamicClass) == DirectColor) {
	ndef = cfbExpandDirectColors (pmap, ndef, pdefs, directDefs);
	pdefs = directDefs;
    }

    for (i = 0; i < ndef; i++) {
	/* Return the n most significant bits from a 16-bit value.
	 * For VGA, n = 6.  For 8-bit DACs, n = 8.
	 */
	entry.r = pdefs[i].red >> 8;
	entry.g = pdefs[i].green >> 8;
	entry.b = pdefs[i].blue >> 8;

	/* Include gamma correction if supported */
	if (mach64IntegratedController) {
	    entry.r = xf86rGammaMap[entry.r];
	    entry.g = xf86gGammaMap[entry.g];
	    entry.b = xf86bGammaMap[entry.b];
	}

	if (!mach64DAC8Bit) {
	    entry.r >>= 2;
	    entry.g >>= 2;
	    entry.b >>= 2;
	}

	mach64savedLUT[pdefs[i].pixel].r = entry.r;
	mach64savedLUT[pdefs[i].pixel].g = entry.g;
	mach64savedLUT[pdefs[i].pixel].b = entry.b;

	if (xf86VTSema
#ifdef XFreeXDGA
	    || ((mach64InfoRec.directMode & XF86DGADirectGraphics)
	        && !(mach64InfoRec.directMode & XF86DGADirectColormap))
	    || (mach64InfoRec.directMode & XF86DGAHasColormap)
#endif
	   ) {
            /* WaitQueue(4); */
            outb(ioDAC_REGS, pdefs[i].pixel);
            outb(ioDAC_REGS+1, entry.r);
            outb(ioDAC_REGS+1, entry.g);
            outb(ioDAC_REGS+1, entry.b);
	}
    }
    checkCursorColor = TRUE;
}

void
mach64InstallColormap(pmap)
     ColormapPtr	pmap;
{
  ColormapPtr oldmap = InstalledMaps[pmap->pScreen->myNum];
  int         entries;
  Pixel *     ppix;
  xrgb *      prgb;
  xColorItem *defs;
  int         i;


  if (pmap == oldmap)
    return;

  if ((pmap->pVisual->class | DynamicClass) == DirectColor)
    entries = (pmap->pVisual->redMask |
	       pmap->pVisual->greenMask |
	       pmap->pVisual->blueMask) + 1;
  else
    entries = pmap->pVisual->ColormapEntries;

  ppix = (Pixel *)ALLOCATE_LOCAL( entries * sizeof(Pixel));
  prgb = (xrgb *)ALLOCATE_LOCAL( entries * sizeof(xrgb));
  defs = (xColorItem *)ALLOCATE_LOCAL(entries * sizeof(xColorItem));

  if ( oldmap != NOMAPYET)
    WalkTree( pmap->pScreen, TellLostMap, &oldmap->mid);

  InstalledMaps[pmap->pScreen->myNum] = pmap;

  for ( i=0; i<entries; i++) ppix[i] = i;

  QueryColors( pmap, entries, ppix, prgb);

  for ( i=0; i<entries; i++) /* convert xrgbs to xColorItems */
    {
      defs[i].pixel = ppix[i];
      defs[i].red = prgb[i].red;
      defs[i].green = prgb[i].green;
      defs[i].blue = prgb[i].blue;
      defs[i].flags =  DoRed|DoGreen|DoBlue;
    }

  mach64StoreColors( pmap, entries, defs);

  WalkTree(pmap->pScreen, TellGainedMap, &pmap->mid);

  mach64RenewCursorColor(pmap->pScreen);

  DEALLOCATE_LOCAL(ppix);
  DEALLOCATE_LOCAL(prgb);
  DEALLOCATE_LOCAL(defs);
}

void
mach64UninstallColormap(pmap)
     ColormapPtr pmap;
{
  ColormapPtr defColormap;
  
  if ( pmap != InstalledMaps[pmap->pScreen->myNum] )
    return;

  defColormap = (ColormapPtr) LookupIDByType( pmap->pScreen->defColormap,
					      RT_COLORMAP);

  if (defColormap == InstalledMaps[pmap->pScreen->myNum])
    return;

  (*pmap->pScreen->InstallColormap) (defColormap);
}

/* This is for the screen saver */
void
mach64RestoreColor0(pScreen)
     ScreenPtr pScreen;
{
    Pixel       pix = 0;
    xrgb        rgb;
    extern unsigned char xf86rGammaMap[], xf86gGammaMap[], xf86bGammaMap[];

    if (InstalledMaps[pScreen->myNum] == NOMAPYET)
	return;

    QueryColors(InstalledMaps[pScreen->myNum], 1, &pix, &rgb);

    rgb.red   >>= 8;
    rgb.green >>= 8;
    rgb.blue  >>= 8;

    /* Include gamma correction if supported */
    if (mach64IntegratedController) {
	rgb.red   = xf86rGammaMap[rgb.red];
	rgb.green = xf86gGammaMap[rgb.green];
	rgb.blue  = xf86bGammaMap[rgb.blue];
    }

    if (!mach64DAC8Bit) {
	rgb.red   >>= 2;
	rgb.green >>= 2;
	rgb.blue  >>= 2;
    }

    outb(ioDAC_REGS, 0);
    outb(ioDAC_REGS+1, rgb.red);
    outb(ioDAC_REGS+1, rgb.green);
    outb(ioDAC_REGS+1, rgb.blue);
}
