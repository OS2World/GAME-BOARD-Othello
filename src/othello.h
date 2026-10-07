/* othello.h - Othello for OS/2, Open Watcom port */

#define INCL_WIN
#define INCL_WINSWITCHLIST
#define INCL_GPI
#define INCL_DOS
#define INCL_ERRORS
#define INCL_DOSEXCEPTIONS
#define INCL_DOSMODULEMGR
#define INCL_DOSFILEMGR
#define INCL_DOSMISC
#define INCL_DOSPROCESS
#define INCL_DOSSEMAPHORES
#define INCL_WINSHELLDATA

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

/* private messages */
#define WMP_COMP_DONE   (WM_USER + 1)
#define WMP_SETTITLE    (WM_USER + 2)

/* main window class ID / resource ID */
#define WD_MAIN         255

/* message box misc ID */
#define MB_MISC         256

/* dialog IDs */
#define IDD_ABOUT       501
#define DB_EVE          12000

/* help button */
#define PB_HELP         258

/* icon and bitmap resource IDs */
#define IC_PLAY         14000
#define BM_COLOR0       15000
#define BM_COLOR1       15001
#define BM_COLOR2       15002
#define BM_COLOR3       15003
#define BM_COLOR4       15004
#define BM_COLOR5       15005
#define BM_COLOR6       15006
#define BM_COLOR7       15007
#define BM_COLOR8       15008
#define BM_COLOR9       15009
#define BM_COLOR10      15010
#define BM_COLOR11      15011
#define BM_COLOR12      15012
#define BM_COLOR13      15013
#define BM_COLOR14      15014
#define BM_COLOR15      15015

/* sound dialog control IDs */
#define CB_EVESOUNDS    12010
#define LB_EVE          12020
#define LB_EVESOUNDS    12030
#define SL_EVESOUNDS    12040
#define PB_PLAYEVE      12050

/* background color submenu / color items */
#define IDM_SUBMENU_BGCOLOR  1004
#define MI_COLOR0       15100
#define MI_COLOR1       15101
#define MI_COLOR2       15102
#define MI_COLOR3       15103
#define MI_COLOR4       15104
#define MI_COLOR5       15105
#define MI_COLOR6       15106
#define MI_COLOR7       15107
#define MI_COLOR8       15108
#define MI_COLOR9       15109
#define MI_COLOR10      15110
#define MI_COLOR11      15111
#define MI_COLOR12      15112
#define MI_COLOR13      15113
#define MI_COLOR14      15114
#define MI_COLOR15      15115

/* Menu IDs - Game menu (100-199) */
#define IDM_NEW         100
#define IDM_HINT        101
#define IDM_QUIT        103
#define IDM_EXIT        110

/* Menu IDs - Options (200-299) */
#define IDM_BEGINNER    200
#define IDM_ADVANCED    201
#define IDM_MASTER      202
#define IDM_PLAYER      203
#define IDM_COMPUTER    204
#define IDM_SOUND       205
#define IDM_SAVEONEXIT  210
#define IDM_FRAME       212

/* Language menu IDs (300-399) */
#define IDM_LANG_EN     300
#define IDM_LANG_ES     301
#define IDM_LANG_NL     302
#define IDM_LANG_DE     303
#define IDM_LANG_FR     304
#define IDM_LANG_IT     305

/* Help menu IDs (900-999) */
#define IDM_HELPINDEX       901
#define IDM_HELPEXTENDED    902
#define IDM_HELPKEYS        903
#define IDM_HELPHELPFORHELP 904
#define IDM_ABOUT           999

/* Submenu cascade IDs (1000-1099) */
#define IDM_SUBMENU_GAME    1000
#define IDM_SUBMENU_OPTIONS 1001
#define IDM_SUBMENU_HELP    1002
#define IDM_SUBMENU_LANG    1003
#define IDM_SUBMENU_LEVEL   1005

/* pointer ID */
#define PTR_CROSS       275

/* timer */
#define TI_LOGO         277

/* help table / subtable IDs */
#define MAIN_HELP_TABLE         1000
#define SUBTABLE_MAIN           2000
#define SUBTABLE_PRODUCTINFODLG 3000

#define PANEL_MAIN              2100
#define PANEL_PRODUCTINFODLG    3100
#define PANEL_GAME              2110
#define PANEL_GAME_NEW          2111
#define PANEL_GAME_HINT         2112
#define PANEL_OPTIONS           2113
#define PANEL_OPTIONS_BEGINNER  2114
#define PANEL_OPTIONS_ADVANCED  2115
#define PANEL_OPTIONS_MASTER    2116
#define PANEL_OPTIONS_PLAYER    2117
#define PANEL_OPTIONS_COMPUTER  2118
#define PANEL_OPTIONS_SOUND     2119
#define PANEL_HELP_HELP         2120
#define PANEL_HELP_HELPFORHELP  2121
#define PANEL_HELP_EXTENDED     2122
#define PANEL_HELP_KEYS         2123
#define PANEL_HELP_INDEX        2124
#define PANEL_HELP_PRODUCTINFO  2125
#define PANEL_SMBGCOLOR         16000
#define PANEL_BGCOLOR           16001
#define SUBTABLE_EVEDLG         13500
#define PANEL_EVEDLG            13510
#define PANEL_HELPPLAYING       2126
#define PANEL_HELPNOTE          2127
#define PANEL_HELPCREATOR       2128
#define PANEL_KEYSHELP          2129
#define PANEL_GAME_QUIT         2130
#define PANEL_GAME_EXIT         2131
#define PANEL_OPTIONS_LANG      2132
#define PANEL_OPTIONS_SAVE      2133
#define PANEL_OPTIONS_FRAME     2134

/* game constants */
#define ID_NULL         -1
#define DIVISIONS       8
#define RETURN_ERROR    1
#define EMPTY           0
#define PLAYER          1
#define COMPUTER        5
#define IND             32767
#define MAX_NUMBER_OF_WAV_FILES 100
#define NUMBER_OF_EVENTS        10

/* typedefs */
typedef struct {
    SHORT sEvent;
    CHAR  chWavFile[CCHMAXPATH];
} SOUNDEVENT;
typedef SOUNDEVENT *PSOUNDEVENT;

typedef struct _BOARD {
    SHORT sField[8][8];
} BOARD;
typedef BOARD *PBOARD;

/* function prototypes */
VOID   CopyBoard(PBOARD pBoardFrom, PBOARD pBoardTo);
BOOL   fIsMovePossible(BOARD Board, SHORT sX, SHORT sY, SHORT sWho);
SHORT  MakeMove(PBOARD pBoard, SHORT sX, SHORT sY, SHORT sWho, BOOL fSimple, BOOL fInvalidate, SHORT cX, SHORT cY, HWND hwnd);
SHORT  sOthello(BOARD Board, SHORT sX, SHORT sY, SHORT sCurrentLevel, SHORT sMaxLevel, SHORT sWho, PSHORT psX, PSHORT psY, PBOOL pfValid, SHORT sPrevBewertung);
BOOL   fMustPass(BOARD Board, SHORT sWho);
BOOL   fGameOver(BOARD Board);
SHORT  sFlipped(PBOARD pBoard, SHORT sX, SHORT sY, SHORT sWho);
SHORT  sSquare(BOARD Board, SHORT sOwn, SHORT sOpp, SHORT sX, SHORT sY);
VOID   Result(BOARD Board, PSHORT psComputer, PSHORT psPlayer);

int    main(VOID);
MRESULT EXPENTRY wpMain(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY AboutDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY dpEve(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

VOID   SetPointer(SHORT x, SHORT y);
VOID   DisplayMessage(PCH pchStr);
VOID   InitHelp(VOID);
VOID   HelpHelpForHelp(MPARAM mp2);
VOID   HelpExtended(MPARAM mp2);
VOID   HelpKeys(MPARAM mp2);
VOID   HelpIndex(MPARAM mp2);
VOID   DestroyHelpInstance(VOID);

VOID   set_language(int lang);
VOID   load_lang(VOID);
VOID   save_lang(VOID);
VOID   toggle_frame(VOID);
VOID   save_settings(VOID);
VOID   load_settings(VOID);

VOID   vdGetWavFile(USHORT usEvent, PSZ pszWavFile);
VOID   vdSetWavFile(USHORT usEvent, PSZ pszWavFile);
VOID   vdPlayWavFileAsyncEve(void *arg);
VOID APIENTRY Othello_Thread(ULONG ulParam);
