/* $XConsortium: menu.c /main/66 1996/12/01 23:46:59 swick $ */
/* $XFree86: xc/programs/xterm/menu.c,v 3.9.2.3 1998/10/20 20:51:47 hohndel Exp $ */
/*

Copyright (c) 1989  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.

*/

#include <ptyx.h>
#include <data.h>
#include <menu.h>

#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xmu/CharSet.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>
#include <stdio.h>
#include <signal.h>

#ifdef MINIX
#include <X11/Xos.h>
#endif

#include <xterm.h>

Arg menuArgs[2] = {{ XtNleftBitmap, (XtArgVal) 0 },
		   { XtNsensitive, (XtArgVal) 0 }};

static void do_8bit_control    PROTO_XT_CALLBACK_ARGS;
static void do_allow132        PROTO_XT_CALLBACK_ARGS;
static void do_allowsends      PROTO_XT_CALLBACK_ARGS;
static void do_altscreen       PROTO_XT_CALLBACK_ARGS;
static void do_appcursor       PROTO_XT_CALLBACK_ARGS;
static void do_appkeypad       PROTO_XT_CALLBACK_ARGS;
static void do_autolinefeed    PROTO_XT_CALLBACK_ARGS;
static void do_autowrap        PROTO_XT_CALLBACK_ARGS;
static void do_backarrow       PROTO_XT_CALLBACK_ARGS;
static void do_clearsavedlines PROTO_XT_CALLBACK_ARGS;
static void do_continue        PROTO_XT_CALLBACK_ARGS;
static void do_cursesemul      PROTO_XT_CALLBACK_ARGS;
static void do_hardreset       PROTO_XT_CALLBACK_ARGS;
static void do_interrupt       PROTO_XT_CALLBACK_ARGS;
static void do_jumpscroll      PROTO_XT_CALLBACK_ARGS;
static void do_kill            PROTO_XT_CALLBACK_ARGS;
static void do_marginbell      PROTO_XT_CALLBACK_ARGS;
static void do_print           PROTO_XT_CALLBACK_ARGS;
static void do_quit            PROTO_XT_CALLBACK_ARGS;
static void do_redraw          PROTO_XT_CALLBACK_ARGS;
static void do_reversevideo    PROTO_XT_CALLBACK_ARGS;
static void do_reversewrap     PROTO_XT_CALLBACK_ARGS;
static void do_scrollbar       PROTO_XT_CALLBACK_ARGS;
static void do_scrollkey       PROTO_XT_CALLBACK_ARGS;
static void do_scrollttyoutput PROTO_XT_CALLBACK_ARGS;
static void do_securekbd       PROTO_XT_CALLBACK_ARGS;
static void do_softreset       PROTO_XT_CALLBACK_ARGS;
static void do_sun_fkeys       PROTO_XT_CALLBACK_ARGS;
static void do_sun_kbd         PROTO_XT_CALLBACK_ARGS;
static void do_suspend         PROTO_XT_CALLBACK_ARGS;
static void do_terminate       PROTO_XT_CALLBACK_ARGS;
static void do_visualbell      PROTO_XT_CALLBACK_ARGS;
static void do_vtfont          PROTO_XT_CALLBACK_ARGS;

#ifdef ALLOWLOGGING
static void do_logging         PROTO_XT_CALLBACK_ARGS;
#endif

#ifndef NO_ACTIVE_ICON
static void do_activeicon      PROTO_XT_CALLBACK_ARGS;
#endif /* NO_ACTIVE_ICON */

#if OPT_TEK4014
static void do_tekcopy         PROTO_XT_CALLBACK_ARGS;
static void do_tekhide         PROTO_XT_CALLBACK_ARGS;
static void do_tekmode         PROTO_XT_CALLBACK_ARGS;
static void do_tekonoff        PROTO_XT_CALLBACK_ARGS;
static void do_tekpage         PROTO_XT_CALLBACK_ARGS;
static void do_tekreset        PROTO_XT_CALLBACK_ARGS;
static void do_tekshow         PROTO_XT_CALLBACK_ARGS;
static void do_tektext2        PROTO_XT_CALLBACK_ARGS;
static void do_tektext3        PROTO_XT_CALLBACK_ARGS;
static void do_tektextlarge    PROTO_XT_CALLBACK_ARGS;
static void do_tektextsmall    PROTO_XT_CALLBACK_ARGS;
static void do_vthide          PROTO_XT_CALLBACK_ARGS;
static void do_vtmode          PROTO_XT_CALLBACK_ARGS;
static void do_vtonoff         PROTO_XT_CALLBACK_ARGS;
static void do_vtshow          PROTO_XT_CALLBACK_ARGS;
static void handle_tekshow     (Widget gw, Bool allowswitch);
static void handle_vtshow      (Widget gw, Bool allowswitch);
#endif

/*
 * The order of entries MUST match the values given in menu.h
 */
MenuEntry mainMenuEntries[] = {
    { "securekbd",	do_securekbd,	NULL },
    { "allowsends",	do_allowsends,	NULL },
#ifdef ALLOWLOGGING
    { "logging",	do_logging,	NULL },
#endif
    { "print",		do_print,	NULL },
    { "redraw",		do_redraw,	NULL },
    { "line1",		NULL,		NULL },
    { "8-bit control",	do_8bit_control, NULL },
    { "backarrow key",	do_backarrow,	NULL },
    { "sun function-keys",do_sun_fkeys,	NULL },
    { "sun keyboard",	do_sun_kbd,	NULL },
    { "line2",		NULL,		NULL },
    { "suspend",	do_suspend,	NULL },
    { "continue",	do_continue,	NULL },
    { "interrupt",	do_interrupt,	NULL },
    { "hangup",		do_hangup,	NULL },
    { "terminate",	do_terminate,	NULL },
    { "kill",		do_kill,	NULL },
    { "line3",		NULL,		NULL },
    { "quit",		do_quit,	NULL }};

MenuEntry vtMenuEntries[] = {
    { "scrollbar",	do_scrollbar, NULL },		/*  0 */
    { "jumpscroll",	do_jumpscroll, NULL },		/*  1 */
    { "reversevideo",	do_reversevideo, NULL },	/*  2 */
    { "autowrap",	do_autowrap, NULL },		/*  3 */
    { "reversewrap",	do_reversewrap, NULL },		/*  4 */
    { "autolinefeed",	do_autolinefeed, NULL },	/*  5 */
    { "appcursor",	do_appcursor, NULL },		/*  6 */
    { "appkeypad",	do_appkeypad, NULL },		/*  7 */
    { "scrollkey",	do_scrollkey, NULL },		/*  8 */
    { "scrollttyoutput",do_scrollttyoutput, NULL },	/*  9 */
    { "allow132",	do_allow132, NULL },		/* 10 */
    { "cursesemul",	do_cursesemul, NULL },		/* 11 */
    { "visualbell",	do_visualbell, NULL },		/* 12 */
    { "marginbell",	do_marginbell, NULL },		/* 13 */
    { "altscreen",	do_altscreen, NULL },		/* 14 */
#ifndef NO_ACTIVE_ICON
    { "activeicon",	do_activeicon, NULL },		/* 15 */
#endif /* NO_ACTIVE_ICON */
    { "line1",		NULL, NULL },			/* 16 */
    { "softreset",	do_softreset, NULL },		/* 17 */
    { "hardreset",	do_hardreset, NULL },		/* 18 */
    { "clearsavedlines",do_clearsavedlines, NULL },	/* 19 */
#if OPT_TEK4014
    { "line2",		NULL, NULL },			/* 20 */
    { "tekshow",	do_tekshow, NULL },		/* 21 */
    { "tekmode",	do_tekmode, NULL },		/* 22 */
    { "vthide",		do_vthide, NULL },		/* 23 */
#endif
    };

MenuEntry fontMenuEntries[] = {
    { "fontdefault",	do_vtfont, NULL },		/*  0 */
    { "font1",		do_vtfont, NULL },		/*  1 */
    { "font2",		do_vtfont, NULL },		/*  2 */
    { "font3",		do_vtfont, NULL },		/*  3 */
    { "font4",		do_vtfont, NULL },		/*  4 */
    { "font5",		do_vtfont, NULL },		/*  5 */
    { "font6",		do_vtfont, NULL },		/*  6 */
    { "fontescape",	do_vtfont, NULL },		/*  7 */
    { "fontsel",	do_vtfont, NULL }};		/*  8 */
    /* this should match NMENUFONTS in ptyx.h */

#if OPT_TEK4014
MenuEntry tekMenuEntries[] = {
    { "tektextlarge",	do_tektextlarge, NULL },	/*  0 */
    { "tektext2",	do_tektext2, NULL },		/*  1 */
    { "tektext3",	do_tektext3, NULL },		/*  2 */
    { "tektextsmall",	do_tektextsmall, NULL },	/*  3 */
    { "line1",		NULL, NULL },			/*  4 */
    { "tekpage",	do_tekpage, NULL },		/*  5 */
    { "tekreset",	do_tekreset, NULL },		/*  6 */
    { "tekcopy",	do_tekcopy, NULL },		/*  7 */
    { "line2",		NULL, NULL },			/*  8 */
    { "vtshow",		do_vtshow, NULL },		/*  9 */
    { "vtmode",		do_vtmode, NULL },		/* 10 */
    { "tekhide",	do_tekhide, NULL }};		/* 11 */
#endif

static Widget create_menu (
		XtermWidget xtw,
		Widget toplevelw,
		char *name,
		struct _MenuEntry *entries,
		int nentries);

static void handle_send_signal (Widget gw, int sig);

static void handle_toggle (
		void (*proc)PROTO_XT_CALLBACK_ARGS,
		int var,
		String *params,
		Cardinal nparams,
		Widget w,
		XtPointer closure,
		XtPointer data);

extern Widget toplevel;


/*
 * we really want to do these dynamically
 */
#define check_width 9
#define check_height 8
static unsigned char check_bits[] = {
   0x00, 0x01, 0x80, 0x01, 0xc0, 0x00, 0x60, 0x00,
   0x31, 0x00, 0x1b, 0x00, 0x0e, 0x00, 0x04, 0x00
};


/*
 * public interfaces
 */

/* ARGSUSED */
static Bool domenu (
	Widget w GCC_UNUSED,
	XEvent *event GCC_UNUSED,
	String *params,             /* mainMenu, vtMenu, or tekMenu */
	Cardinal *param_count)      /* 0 or 1 */
{
    TScreen *screen = &term->screen;

    if (*param_count != 1) {
	Bell(XkbBI_MinorError,0);
	return False;
    }

    switch (params[0][0]) {
      case 'm':
	if (!screen->mainMenu) {
	    screen->mainMenu = create_menu (term, toplevel, "mainMenu",
					    mainMenuEntries,
					    XtNumber(mainMenuEntries));
	    update_securekbd();
	    update_allowsends();
#ifdef ALLOWLOGGING
	    update_logging();
#endif
	    update_8bit_control();
	    update_decbkm();
	    update_sun_kbd();
	    if (screen->terminal_id < 200) {
		set_sensitivity (screen->mainMenu,
				 mainMenuEntries[mainMenu_8bit_ctrl].widget,
				 FALSE);
	    }
	    update_sun_fkeys();
#if !defined(SIGTSTP) || defined(AMOEBA)
	    set_sensitivity (screen->mainMenu,
			     mainMenuEntries[mainMenu_suspend].widget, FALSE);
#endif
#if !defined(SIGCONT) || defined(AMOEBA)
	    set_sensitivity (screen->mainMenu, 
			     mainMenuEntries[mainMenu_continue].widget, FALSE);
#endif
	}
	break;

      case 'v':
	if (!screen->vtMenu) {
	    screen->vtMenu = create_menu (term, toplevel, "vtMenu",
					  vtMenuEntries,
					  XtNumber(vtMenuEntries));
	    update_scrollbar();
	    update_jumpscroll();
	    update_reversevideo();
	    update_autowrap();
	    update_reversewrap();
	    update_autolinefeed();
	    update_appcursor();
	    update_appkeypad();
	    update_scrollkey();
	    update_scrollttyoutput();
	    update_allow132();
	    update_cursesemul();
	    update_visualbell();
	    update_marginbell();
#ifndef NO_ACTIVE_ICON
	    if (!screen->fnt_icon || !screen->iconVwin.window) {
		set_sensitivity (screen->vtMenu,
				 vtMenuEntries[vtMenu_activeicon].widget,
				 FALSE);
	    }
	    else
		update_activeicon();
#endif /* NO_ACTIVE_ICON */
	}
	break;

      case 'f':
	if (!screen->fontMenu) {
	    screen->fontMenu = create_menu (term, toplevel, "fontMenu",
					    fontMenuEntries,
					    NMENUFONTS);  
	    set_menu_font (True);
	    set_sensitivity (screen->fontMenu,
			     fontMenuEntries[fontMenu_fontescape].widget,
			     (screen->menu_font_names[fontMenu_fontescape]
			      ? TRUE : FALSE));
	}
	FindFontSelection (NULL, True);
	set_sensitivity (screen->fontMenu,
			 fontMenuEntries[fontMenu_fontsel].widget,
			 (screen->menu_font_names[fontMenu_fontsel]
			  ? TRUE : FALSE));
	break;

#if OPT_TEK4014
      case 't':
	if (!screen->tekMenu) {
	    screen->tekMenu = create_menu (term, toplevel, "tekMenu",
					   tekMenuEntries,
					   XtNumber(tekMenuEntries));
	    set_tekfont_menu_item (screen->cur.fontsize, TRUE);
	}
	break;
#endif

      default:
	Bell(XkbBI_MinorError,0);
	return False;
    }

    return True;
}

void HandleCreateMenu (
	Widget w,
	XEvent *event,              /* unused */
	String *params,             /* mainMenu, vtMenu, or tekMenu */
	Cardinal *param_count)      /* 0 or 1 */
{
    (void) domenu (w, event, params, param_count);
}

void HandlePopupMenu (
	Widget w,
	XEvent *event,              /* unused */
	String *params,             /* mainMenu, vtMenu, or tekMenu */
	Cardinal *param_count)      /* 0 or 1 */
{
    if (domenu (w, event, params, param_count)) {
	XtCallActionProc (w, "XawPositionSimpleMenu", event, params, 1);
	XtCallActionProc (w, "MenuPopup", event, params, 1);
    }
}


/*
 * private interfaces - keep out!
 */

/*
 * create_menu - create a popup shell and stuff the menu into it.
 */

static Widget
create_menu (
	XtermWidget xtw,
	Widget toplevelw,
	char *name,
	struct _MenuEntry *entries,
	int nentries)
{
    Widget m;
    TScreen *screen = &xtw->screen;
    static XtCallbackRec cb[2] = { { NULL, NULL }, { NULL, NULL }};
    static Arg arg = { XtNcallback, (XtArgVal) cb };

    if (screen->menu_item_bitmap == None) {
	screen->menu_item_bitmap =
	  XCreateBitmapFromData (XtDisplay(xtw),
				 RootWindowOfScreen(XtScreen(xtw)),
				 (char *)check_bits, check_width, check_height);
    }

    m = XtCreatePopupShell (name, simpleMenuWidgetClass, toplevelw, NULL, 0);

    for (; nentries > 0; nentries--, entries++) {
	cb[0].callback = (XtCallbackProc) entries->function;
	cb[0].closure = (caddr_t) entries->name;
	entries->widget = XtCreateManagedWidget (entries->name, 
						 (entries->function ?
						  smeBSBObjectClass :
						  smeLineObjectClass), m,
						 &arg, (Cardinal) 1);
    }

    /* do not realize at this point */
    return m;
}

/* ARGSUSED */
static void handle_send_signal (Widget gw GCC_UNUSED, int sig)
{
    register TScreen *screen = &term->screen;

    if (screen->pid > 1) kill_process_group (screen->pid, sig);
}


/*
 * action routines
 */

/* ARGSUSED */
void DoSecureKeyboard (Time tp GCC_UNUSED)
{
    do_securekbd (term->screen.mainMenu, (XtPointer)0, (XtPointer)0);
}

static void do_securekbd (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    register TScreen *screen = &term->screen;
    Time now = CurrentTime;		/* XXX - wrong */

    if (screen->grabbedKbd) {
	XUngrabKeyboard (screen->display, now);
	ReverseVideo (term);
	screen->grabbedKbd = FALSE;
    } else {
	if (XGrabKeyboard (screen->display, term->core.window,
			   True, GrabModeAsync, GrabModeAsync, now)
	    != GrabSuccess) {
	    Bell(XkbBI_MinorError, 100);
	} else {
	    ReverseVideo (term);
	    screen->grabbedKbd = TRUE;
	}
    }
    update_securekbd();
}


static void do_allowsends (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    register TScreen *screen = &term->screen;

    screen->allowSendEvents = !screen->allowSendEvents;
    update_allowsends ();
}

static void do_visualbell (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    register TScreen *screen = &term->screen;

    screen->visualbell = !screen->visualbell;
    update_visualbell();
}

#ifdef ALLOWLOGGING
static void do_logging (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    register TScreen *screen = &term->screen;

    if (screen->logging) {
	CloseLog (screen);
    } else {
	StartLog (screen);
    }
    /* update_logging done by CloseLog and StartLog */
}
#endif

static void do_print (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    xtermPrintScreen ();
}


static void do_redraw (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    Redraw ();
}


void show_8bit_control (Bool value)
{
    if (term->screen.control_eight_bits != value) {
	term->screen.control_eight_bits = value;
	update_8bit_control();
    }
}

static void do_8bit_control (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    show_8bit_control(! term->screen.control_eight_bits);
}

static void do_backarrow (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    term->keyboard.flags ^= MODE_DECBKM;
    update_decbkm();
}

static void do_sun_fkeys (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    sunFunctionKeys = ! sunFunctionKeys;
    update_sun_fkeys();
}


#if OPT_SUNPC_KBD
static void do_sun_kbd (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    sunKeyboard = ! sunKeyboard;
    update_sun_kbd();
}
#endif


/*
 * The following cases use the pid instead of the process group so that we
 * don't get hosed by programs that change their process group
 */


/* ARGSUSED */
static void do_suspend (
	Widget gw,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
#if defined(SIGTSTP) && !defined(AMOEBA)
    handle_send_signal (gw, SIGTSTP);
#endif
}

/* ARGSUSED */
static void do_continue (
	Widget gw,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
#if defined(SIGCONT) && !defined(AMOEBA)
    handle_send_signal (gw, SIGCONT);
#endif
}

/* ARGSUSED */
static void do_interrupt (
	Widget gw,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    handle_send_signal (gw, SIGINT);
}

/* ARGSUSED */
void do_hangup (
	Widget gw,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    handle_send_signal (gw, SIGHUP);
}

/* ARGSUSED */
static void do_terminate (
	Widget gw,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    handle_send_signal (gw, SIGTERM);
}

/* ARGSUSED */
static void do_kill (
	Widget gw,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    handle_send_signal (gw, SIGKILL);
}

static void do_quit (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    Cleanup (0);
}



/*
 * vt menu callbacks
 */

static void do_scrollbar (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    register TScreen *screen = &term->screen;

    if (screen->fullVwin.scrollbar) {
	ScrollBarOff (screen);
    } else {
	ScrollBarOn (term, FALSE, FALSE);
    }
    update_scrollbar();
}


static void do_jumpscroll (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    register TScreen *screen = &term->screen;

    term->flags ^= SMOOTHSCROLL;
    if (term->flags & SMOOTHSCROLL) {
	screen->jumpscroll = FALSE;
	if (screen->scroll_amt) FlushScroll(screen);
    } else {
	screen->jumpscroll = TRUE;
    }
    update_jumpscroll();
}


static void do_reversevideo (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    term->flags ^= REVERSE_VIDEO;
    ReverseVideo (term);
    /* update_reversevideo done in ReverseVideo */
}


static void do_autowrap (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    term->flags ^= WRAPAROUND;
    update_autowrap();
}


static void do_reversewrap (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    term->flags ^= REVERSEWRAP;
    update_reversewrap();
}


static void do_autolinefeed (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    term->flags ^= LINEFEED;
    update_autolinefeed();
}


static void do_appcursor (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    term->keyboard.flags ^= MODE_DECCKM;
    update_appcursor();
}


static void do_appkeypad (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    term->keyboard.flags ^= MODE_DECKPAM;
    update_appkeypad();
}


static void do_scrollkey (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    register TScreen *screen = &term->screen;

    screen->scrollkey = !screen->scrollkey;
    update_scrollkey();
}


static void do_scrollttyoutput (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    register TScreen *screen = &term->screen;

    screen->scrollttyoutput = !screen->scrollttyoutput;
    update_scrollttyoutput();
}


static void do_allow132 (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    register TScreen *screen = &term->screen;

    screen->c132 = !screen->c132;
    update_allow132();
}


static void do_cursesemul (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    register TScreen *screen = &term->screen;

    screen->curses = !screen->curses;
    update_cursesemul();
}


static void do_marginbell (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    register TScreen *screen = &term->screen;

    if (!(screen->marginbell = !screen->marginbell)) screen->bellarmed = -1;
    update_marginbell();
}


#if OPT_TEK4014
static void handle_tekshow (Widget gw GCC_UNUSED, Bool allowswitch)
{
    register TScreen *screen = &term->screen;

    if (!screen->Tshow) {		/* not showing, turn on */
	set_tek_visibility (TRUE);
    } else if (screen->Vshow || allowswitch) {  /* is showing, turn off */
	set_tek_visibility (FALSE);
	end_tek_mode ();		/* WARNING: this does a longjmp */
    } else
      Bell(XkbBI_MinorError, 0);
}

/* ARGSUSED */
static void do_tekshow (
	Widget gw,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    handle_tekshow (gw, True);
}

/* ARGSUSED */
static void do_tekonoff (
	Widget gw,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    handle_tekshow (gw, False);
}
#endif /* OPT_TEK4014 */

/* ARGSUSED */
static void do_altscreen (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    TScreen *screen = &term->screen;
    ToggleAlternate(screen);
}

#ifndef NO_ACTIVE_ICON
/* ARGSUSED */
static void do_activeicon (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    TScreen *screen = &term->screen;

    if (screen->iconVwin.window) {
	Widget shell = term->core.parent;
	term->misc.active_icon = !term->misc.active_icon;
	XtVaSetValues(shell, XtNiconWindow,
		      term->misc.active_icon ? screen->iconVwin.window : None,
		      NULL);
	update_activeicon();
    }
}
#endif /* NO_ACTIVE_ICON */

static void do_softreset (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    VTReset (FALSE, FALSE);
}


static void do_hardreset (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    VTReset (TRUE, FALSE);
}


static void do_clearsavedlines (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    VTReset (TRUE, TRUE); 
}


#if OPT_TEK4014
static void do_tekmode (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    register TScreen *screen = &term->screen;

    switch_modes (screen->TekEmu);	/* switch to tek mode */
}

/* ARGSUSED */
static void do_vthide (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    hide_vt_window();
}
#endif /* OPT_TEK4014 */


/*
 * vtfont menu
 */

static void do_vtfont (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    char *entryname = (char *) closure;
    int i;

    for (i = 0; i < NMENUFONTS; i++) {
	if (strcmp (entryname, fontMenuEntries[i].name) == 0) {
	    SetVTFont (i, True, (char *)0, (char *)0);
	    return;
	}
    }
    Bell(XkbBI_MinorError, 0);
}


/*
 * tek menu
 */

#if OPT_TEK4014
static void do_tektextlarge (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    TekSetFontSize (tekMenu_tektextlarge);
}


static void do_tektext2 (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    TekSetFontSize (tekMenu_tektext2);
}


static void do_tektext3 (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    TekSetFontSize (tekMenu_tektext3);
}


static void do_tektextsmall (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{

    TekSetFontSize (tekMenu_tektextsmall);
}


static void do_tekpage (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    TekSimulatePageButton (False);
}


static void do_tekreset (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    TekSimulatePageButton (True);
}


static void do_tekcopy (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    TekCopy ();
}


static void handle_vtshow (Widget gw GCC_UNUSED, Bool allowswitch)
{
    register TScreen *screen = &term->screen;

    if (!screen->Vshow) {		/* not showing, turn on */
	set_vt_visibility (TRUE);
    } else if (screen->Tshow || allowswitch) {  /* is showing, turn off */
	set_vt_visibility (FALSE);
	if (!screen->TekEmu && TekRefresh) dorefresh ();
	end_vt_mode ();			/* WARNING: this does a longjmp... */
    } else 
      Bell(XkbBI_MinorError, 0);
}

static void do_vtshow (
	Widget gw,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    handle_vtshow (gw, True);
}

static void do_vtonoff (
	Widget gw,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    handle_vtshow (gw, False);
}

static void do_vtmode (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    register TScreen *screen = &term->screen;

    switch_modes (screen->TekEmu);	/* switch to vt, or from */
}


/* ARGSUSED */
static void do_tekhide (
	Widget gw GCC_UNUSED,
	XtPointer closure GCC_UNUSED,
	XtPointer data GCC_UNUSED)
{
    hide_tek_window();
}
#endif /* OPT_TEK4014 */



/*
 * public handler routines
 */

static void handle_toggle (
	void (*proc)PROTO_XT_CALLBACK_ARGS,
	int var,
	String *params,
	Cardinal nparams,
	Widget w,
	XtPointer closure,
	XtPointer data)
{
    int dir = -2;

    switch (nparams) {
      case 0:
	dir = -1;
	break;
      case 1:
	if (XmuCompareISOLatin1 (params[0], "on") == 0) dir = 1;
	else if (XmuCompareISOLatin1 (params[0], "off") == 0) dir = 0;
	else if (XmuCompareISOLatin1 (params[0], "toggle") == 0) dir = -1;
	break;
    }

    switch (dir) {
      case -2:
	Bell(XkbBI_MinorError, 0);
	break;

      case -1:
	(*proc) (w, closure, data);
	break;

      case 0:
	if (var) (*proc) (w, closure, data);
	else Bell(XkbBI_MinorError, 0);
	break;

      case 1:
	if (!var) (*proc) (w, closure, data);
	else Bell(XkbBI_MinorError, 0);
	break;
    }
    return;
}

void HandleAllowSends(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params,
	Cardinal *param_count)
{
    handle_toggle (do_allowsends, (int) term->screen.allowSendEvents,
		   params, *param_count, w, (XtPointer)0, (XtPointer)0);
}

void HandleSetVisualBell(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params,
	Cardinal *param_count)
{
    handle_toggle (do_visualbell, (int) term->screen.visualbell,
		   params, *param_count, w, (XtPointer)0, (XtPointer)0);
}

#ifdef ALLOWLOGGING
void HandleLogging(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params,
	Cardinal *param_count)
{
    handle_toggle (do_logging, (int) term->screen.logging,
		   params, *param_count, w, (XtPointer)0, (XtPointer)0);
}
#endif

/* ARGSUSED */
void HandlePrint(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params GCC_UNUSED,
	Cardinal *param_count GCC_UNUSED)
{
    do_print(w, (XtPointer)0, (XtPointer)0);
}

/* ARGSUSED */
void HandleRedraw(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params GCC_UNUSED,
	Cardinal *param_count GCC_UNUSED)
{
    do_redraw(w, (XtPointer)0, (XtPointer)0);
}

/* ARGSUSED */
void HandleSendSignal(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params,
	Cardinal *param_count)
{
    static struct sigtab {
	char *name;
	int sig;
    } signals[] = {
#ifdef SIGTSTP
	{ "suspend",	SIGTSTP },
	{ "tstp",	SIGTSTP },
#endif
#ifdef SIGCONT
	{ "cont",	SIGCONT },
#endif
	{ "int",	SIGINT },
	{ "hup",	SIGHUP },
	{ "quit",	SIGQUIT },
	{ "alrm",	SIGALRM },
	{ "alarm",	SIGALRM },
	{ "term",	SIGTERM },
	{ "kill",	SIGKILL },
	{ NULL, 0 },
    };

    if (*param_count == 1) {
	struct sigtab *st;

	for (st = signals; st->name; st++) {
	    if (XmuCompareISOLatin1 (st->name, params[0]) == 0) {
		handle_send_signal (w, st->sig);
		return;
	    }
	}
	/* one could allow numeric values, but that would be a security hole */
    }

    Bell(XkbBI_MinorError, 0);
}

/* ARGSUSED */
void HandleQuit(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params GCC_UNUSED,
	Cardinal *param_count GCC_UNUSED)
{
    do_quit(w, (XtPointer)0, (XtPointer)0);
}

void Handle8BitControl(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params,
	Cardinal *param_count)
{
    handle_toggle (do_8bit_control, (int) term->screen.control_eight_bits,
		   params, *param_count, w, (XtPointer)0, (XtPointer)0);
}

void HandleBackarrow(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params,
	Cardinal *param_count)
{
    handle_toggle (do_backarrow, (int) term->keyboard.flags & MODE_DECBKM,
		   params, *param_count, w, (XtPointer)0, (XtPointer)0);
}

void HandleSunFunctionKeys(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params,
	Cardinal *param_count)
{
    handle_toggle (do_sun_fkeys, (int) sunFunctionKeys,
		   params, *param_count, w, (XtPointer)0, (XtPointer)0);
}

#if OPT_SUNPC_KBD
void HandleSunKeyboard(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params,
	Cardinal *param_count)
{
    handle_toggle (do_sun_kbd, (int) sunKeyboard,
		   params, *param_count, w, (XtPointer)0, (XtPointer)0);
}
#endif

void HandleScrollbar(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params,
	Cardinal *param_count)
{
    handle_toggle (do_scrollbar, (int) term->screen.fullVwin.scrollbar,
		   params, *param_count, w, (XtPointer)0, (XtPointer)0);
}

void HandleJumpscroll(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params,
	Cardinal *param_count)
{
    handle_toggle (do_jumpscroll, (int) term->screen.jumpscroll,
		   params, *param_count, w, (XtPointer)0, (XtPointer)0);
}

void HandleReverseVideo(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params,
	Cardinal *param_count)
{
    handle_toggle (do_reversevideo, (int) (term->flags & REVERSE_VIDEO),
		   params, *param_count, w, (XtPointer)0, (XtPointer)0);
}

void HandleAutoWrap(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params,
	Cardinal *param_count)
{
    handle_toggle (do_autowrap, (int) (term->flags & WRAPAROUND),
		   params, *param_count, w, (XtPointer)0, (XtPointer)0);
}

void HandleReverseWrap(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params,
	Cardinal *param_count)
{
    handle_toggle (do_reversewrap, (int) (term->flags & REVERSEWRAP),
		   params, *param_count, w, (XtPointer)0, (XtPointer)0);
}

void HandleAutoLineFeed(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params,
	Cardinal *param_count)
{
    handle_toggle (do_autolinefeed, (int) (term->flags & LINEFEED),
		   params, *param_count, w, (XtPointer)0, (XtPointer)0);
}

void HandleAppCursor(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params,
	Cardinal *param_count)
{
    handle_toggle (do_appcursor, (int) (term->keyboard.flags & MODE_DECCKM),
		   params, *param_count, w, (XtPointer)0, (XtPointer)0);
}

void HandleAppKeypad(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params,
	Cardinal *param_count)
{
    handle_toggle (do_appkeypad, (int) (term->keyboard.flags & MODE_DECKPAM),
		   params, *param_count, w, (XtPointer)0, (XtPointer)0);
}

void HandleScrollKey(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params,
	Cardinal *param_count)
{
    handle_toggle (do_scrollkey, (int) term->screen.scrollkey,
		   params, *param_count, w, (XtPointer)0, (XtPointer)0);
}

void HandleScrollTtyOutput(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params,
	Cardinal *param_count)
{
    handle_toggle (do_scrollttyoutput, (int) term->screen.scrollttyoutput,
		   params, *param_count, w, (XtPointer)0, (XtPointer)0);
}

void HandleAllow132(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params,
	Cardinal *param_count)
{
    handle_toggle (do_allow132, (int) term->screen.c132,
		   params, *param_count, w, (XtPointer)0, (XtPointer)0);
}

void HandleCursesEmul(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params,
	Cardinal *param_count)
{
    handle_toggle (do_cursesemul, (int) term->screen.curses,
		   params, *param_count, w, (XtPointer)0, (XtPointer)0);
}

void HandleMarginBell(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params,
	Cardinal *param_count)
{
    handle_toggle (do_marginbell, (int) term->screen.marginbell,
		   params, *param_count, w, (XtPointer)0, (XtPointer)0);
}

void HandleAltScreen(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params,
	Cardinal *param_count)
{
    /* eventually want to see if sensitive or not */
    handle_toggle (do_altscreen, (int) term->screen.alternate,
		   params, *param_count, w, (XtPointer)0, (XtPointer)0);
}

/* ARGSUSED */
void HandleSoftReset(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params GCC_UNUSED,
	Cardinal *param_count GCC_UNUSED)
{
    do_softreset(w, (XtPointer)0, (XtPointer)0);
}

/* ARGSUSED */
void HandleHardReset(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params GCC_UNUSED,
	Cardinal *param_count GCC_UNUSED)
{
    do_hardreset(w, (XtPointer)0, (XtPointer)0);
}

/* ARGSUSED */
void HandleClearSavedLines(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params GCC_UNUSED,
	Cardinal *param_count GCC_UNUSED)
{
    do_clearsavedlines(w, (XtPointer)0, (XtPointer)0);
}

#if OPT_TEK4014
void HandleSetTerminalType(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params,
	Cardinal *param_count)
{
    if (*param_count == 1) {
	switch (params[0][0]) {
	  case 'v': case 'V':
	    if (term->screen.TekEmu) do_vtmode (w, (XtPointer)0, (XtPointer)0);
	    break;
	  case 't': case 'T':
	    if (!term->screen.TekEmu) do_tekmode (w, (XtPointer)0, (XtPointer)0);
	    break;
	  default:
	    Bell(XkbBI_MinorError, 0);
	}
    } else {
	Bell(XkbBI_MinorError, 0);
    }
}

void HandleVisibility(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params,
	Cardinal *param_count)
{
    if (*param_count == 2) {
	switch (params[0][0]) {
	  case 'v': case 'V':
	    handle_toggle (do_vtonoff, (int) term->screen.Vshow,
			   params+1, (*param_count) - 1, w, (XtPointer)0, (XtPointer)0);
	    break;
	  case 't': case 'T':
	    handle_toggle (do_tekonoff, (int) term->screen.Tshow,
			   params+1, (*param_count) - 1, w, (XtPointer)0, (XtPointer)0);
	    break;
	  default:
	    Bell(XkbBI_MinorError, 0);
	}
    } else {
	Bell(XkbBI_MinorError, 0);
    }
}

/* ARGSUSED */
void HandleSetTekText(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params,
	Cardinal *param_count)
{
    void (*proc)PROTO_XT_CALLBACK_ARGS = 0;

    switch (*param_count) {
      case 0:
	proc = do_tektextlarge;
	break;
      case 1:
	switch (params[0][0]) {
	  case 'l': case 'L': proc = do_tektextlarge; break;
	  case '2': proc = do_tektext2; break;
	  case '3': proc = do_tektext3; break;
	  case 's': case 'S': proc = do_tektextsmall; break;
	}
	break;
    }
    if (proc) (*proc) (w, (XtPointer)0, (XtPointer)0);
    else Bell(XkbBI_MinorError, 0);
}

/* ARGSUSED */
void HandleTekPage(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params GCC_UNUSED,
	Cardinal *param_count GCC_UNUSED)
{
    do_tekpage(w, (XtPointer)0, (XtPointer)0);
}

/* ARGSUSED */
void HandleTekReset(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params GCC_UNUSED,
	Cardinal *param_count GCC_UNUSED)
{
    do_tekreset(w, (XtPointer)0, (XtPointer)0);
}

/* ARGSUSED */
void HandleTekCopy(
	Widget w,
	XEvent *event GCC_UNUSED,
	String *params GCC_UNUSED,
	Cardinal *param_count GCC_UNUSED)
{
    do_tekcopy(w, (XtPointer)0, (XtPointer)0);
}
#endif /* OPT_TEK4014 */
