/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=10" };
static const char dmenufont[]       = "monospace:size=10";
static const char col_gray1[]       = "#222222"; /* bg color */
static const char col_gray2[]       = "#444444"; /* inactive window border color */
static const char col_gray3[]       = "#bbbbbb"; /* font color */
static const char col_gray4[]       = "#eeeeee"; /* curent tag and current window font color */
static const char col_cyan[]        = "#005577"; /* top bar and active window border color */
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ NULL,       NULL,       NULL,       0,            0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "st", NULL };

static const char *volup[]    = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "+1%",    NULL };
static const char *volup5[]   = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "+5%",    NULL };
static const char *voldown[]  = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "-1%",    NULL };
static const char *voldown5[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "-5%",    NULL };
static const char *volmute[]  = { "pactl", "set-sink-mute", "@DEFAULT_SINK@", "toggle",   NULL };
static const char *micmute[]  = { "pactl", "set-source-mute", "@DEFAULT_SOURCE@", "toggle", NULL };
static const char *brup[]     = { "light", "-A", "1", NULL };
static const char *brup5[]    = { "light", "-A", "5", NULL };
static const char *brdown[]   = { "light", "-U", "1", NULL };
static const char *brdown5[]  = { "light", "-U", "5", NULL };
static const char *maim[]     = { "dmenu-mmaim", NULL };
static const char *maim_acb[] = { "mmaim", "area-clipboard", NULL };
static const char *dsys[]     = { "dmenu-system", NULL };
static const char *browser[]  = { "firefox", NULL };
static const char *prntscr[]  = { "mmaim", "all", NULL };

static Key keys[] = {
	/* modifier          key        function        argument */
	{ MODKEY,            XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY,            XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY|ShiftMask,  XK_b,      togglebar,      {0} },
	{ MODKEY,            XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,            XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,            XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,            XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,            XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,            XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,  XK_Return, zoom,           {0} },
	{ MODKEY,            XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,  XK_q,      killclient,     {0} },
	{ MODKEY,            XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,            XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,            XK_m,      setlayout,      {.v = &layouts[2]} },
//	{ MODKEY,            XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,  XK_space,  togglefloating, {0} },
	{ MODKEY,            XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,  XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,            XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,            XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,  XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,  XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(             XK_1,                      0)
	TAGKEYS(             XK_2,                      1)
	TAGKEYS(             XK_3,                      2)
	TAGKEYS(             XK_4,                      3)
	TAGKEYS(             XK_5,                      4)
	TAGKEYS(             XK_6,                      5)
	TAGKEYS(             XK_7,                      6)
	TAGKEYS(             XK_8,                      7)
	TAGKEYS(             XK_9,                      8)
	{ MODKEY|ShiftMask,  XK_e,      quit,           {0} },

	{ 0,                 XF86XK_AudioRaiseVolume,  spawn, {.v = volup   } },
	{ ShiftMask,         XF86XK_AudioRaiseVolume,  spawn, {.v = volup5  } },
	{ 0,                 XF86XK_AudioLowerVolume,  spawn, {.v = voldown } },
	{ ShiftMask,         XF86XK_AudioLowerVolume,  spawn, {.v = voldown5} },
	{ 0,                 XF86XK_AudioMute,         spawn, {.v = volmute } },
	{ 0,                 XF86XK_AudioMicMute,      spawn, {.v = micmute } },
	{ 0,                 XF86XK_MonBrightnessUp,   spawn, {.v = brup    } },
	{ ShiftMask,         XF86XK_MonBrightnessUp,   spawn, {.v = brup5   } },
	{ 0,                 XF86XK_MonBrightnessDown, spawn, {.v = brdown  } },
	{ ShiftMask,         XF86XK_MonBrightnessDown, spawn, {.v = brdown5 } },
	{ 0,                 XK_Print,                 spawn, {.v = prntscr } },
	{ ShiftMask,         XK_Print,                 spawn, {.v = maim    } },
	{ ControlMask,       XK_Print,                 spawn, {.v = maim_acb} },
	{ MODKEY,            XK_Escape,                spawn, {.v = dsys    } },
	{ MODKEY,            XK_b,                     spawn, {.v = browser } },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

