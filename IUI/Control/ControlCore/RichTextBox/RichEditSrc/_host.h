/*	@doc INTERNAL
 *
 *	@module _HOST.H  Text Host for Window's Rich Edit Control |
 *	
 *
 *	Original Author: <nl>
 *		Christian Fortini
 *		Murray Sargent
 *
 *	History: <nl>
 *		8/1/95	ricksa	Revised interface definition
 */
#ifndef _HOST_H
#define _HOST_H

#include "textserv.h"

/*
 *	TXTEFFECT
 *
 *	@enum	Defines different background styles control
 */
enum TXTEFFECT {
	TXTEFFECT_NONE = 0,				//@emem	no special backgoround effect
	TXTEFFECT_SUNKEN,				//@emem	draw a "sunken 3-D" look
};


// @doc EXTERNAL 

// ============================  CTxtWinHost  ================================================
// Implement the windowed version of the Plain Text control


#ifndef DEBUG
#define AttCheckRunTotals(_fCF)
#define AttCheckPFRuns(_fCF)
#endif

#ifdef DBCS
#define ECO_STYLES (ECO_AUTOVSCROLL | ECO_AUTOHSCROLL | ECO_NOHIDESEL | \
						ECO_READONLY | ECO_WANTRETURN | ECO_SAVESEL | \
						ECO_VERTICAL | \
						ECO_SELECTIONBAR)
#else
#define ECO_STYLES (ECO_AUTOVSCROLL | ECO_AUTOHSCROLL | ECO_NOHIDESEL | \
						ECO_READONLY | ECO_WANTRETURN | ECO_SAVESEL | \
						ECO_SELECTIONBAR)
#endif

#endif // _HOST_H
