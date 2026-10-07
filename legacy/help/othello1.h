/* file: othello.h                                      */
/* application include file                             */
/* author: Peter Wansch                                 */

/* include section */
/* #define FULL */
#define GERMAN

#define INCL_PM
#define INCL_DOSMODULEMGR
#define INCL_DOSFILEMGR
#define INCL_DOSMISC
#define INCL_DOSPROCESS
#define INCL_DOSSEMAPHORES

#include <os2.h>
#include <stdio.h> 
#include <string.h>
#include <stdlib.h> 
#include <os2me.h>

/* private messages */
#define WMP_COMP_DONE      WM_USER + 1

/* main window id */
#define WD_MAIN            255

/* message box id */
#define MB_MISC            256

/* dialog id */
#define DB_PRODINFO        257
#define DB_EVE                  12000

/* help button id */
#define PB_HELP            258

/* icon und bitmap ids */
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

/* evesounds dialog ids */
#define CB_EVESOUNDS    12010
#define LB_EVE          12020
#define LB_EVESOUNDS    12030
#define SL_EVESOUNDS    12040
#define PB_PLAYEVE      12050

/* define color menu ids */
#define SM_BGCOLOR      15100
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

/* menu item ids */
#define SM_GAME            259
#define MI_NEW             260
#define MI_HINT            261

#define SM_OPTIONS         262
#define MI_BEGINNER        263
#define MI_ADVANCED        264
#define MI_MASTER          265
#define MI_PLAYER          266
#define MI_COMPUTER        267
#define MI_SOUND           268

#define SM_HELP            269
#define MI_HELPHELPFORHELP 270
#define MI_HELPEXTENDED    271
#define MI_HELPKEYS        272
#define MI_HELPINDEX       273
#define MI_HELPPRODUCTINFO 274

/* pointer id */
#define PTR_CROSS          275

/* control ids */
#define ICO_MAIN           276

/* timer ids */
#define TI_LOGO            277

/* main window help table and subtables */
#define MAIN_HELP_TABLE         1000
#define SUBTABLE_MAIN           2000
#define SUBTABLE_PRODUCTINFODLG 3000

/* main panel */
#define PANEL_MAIN              2100

/* product information dialog help panels */
#define PANEL_PRODUCTINFODLG    3100

/* menu item help panels */
/* menu item ids */
#define PANEL_GAME               2110
#define PANEL_GAME_NEW           2111
#define PANEL_GAME_HINT          2112

#define PANEL_OPTIONS            2113
#define PANEL_OPTIONS_BEGINNER   2114
#define PANEL_OPTIONS_ADVANCED   2115
#define PANEL_OPTIONS_MASTER     2116
#define PANEL_OPTIONS_PLAYER     2117
#define PANEL_OPTIONS_COMPUTER   2118
#define PANEL_OPTIONS_SOUND      2119

#define PANEL_HELP_HELP          2120
#define PANEL_HELP_HELPFORHELP   2121
#define PANEL_HELP_EXTENDED      2122
#define PANEL_HELP_KEYS          2123
#define PANEL_HELP_INDEX         2124
#define PANEL_HELP_PRODUCTINFO   2125
#define PANEL_SMBGCOLOR         16000
#define PANEL_BGCOLOR           16001

/* eve dlg */
#define SUBTABLE_EVEDLG         13500
#define PANEL_EVEDLG            13510

/* additional help panels */
#define PANEL_HELPPLAYING        2126
#define PANEL_HELPNOTE           2127
#define PANEL_HELPCREATOR        2128
#define PANEL_KEYSHELP           2129

/* constants */
#define ID_NULL                  -1
#define DIVISIONS                8
#define RETURN_ERROR             1
#define EMPTY                    0
#define PLAYER                   1
#define COMPUTER                 5
#define IND                      32767
#define MAX_NUMBER_OF_WAV_FILES  100

/* typedefs */
typedef struct {
  SHORT sEvent;
  CHAR chWavFile[CCHMAXPATH];
} SOUNDEVENT;
typedef SOUNDEVENT *PSOUNDEVENT;

/* definition for Othello board */
typedef struct _BOARD
{
  SHORT sField[8][8];
} BOARD;

typedef BOARD *PBOARD;

/* function prototypes */
VOID CopyBoard(PBOARD pBoardFrom, PBOARD pBoardTo);
BOOL fIsMovePossible(BOARD Board, SHORT sX, SHORT sY, SHORT sWho);
SHORT MakeMove(PBOARD pBoard, SHORT sX, SHORT sY, SHORT sWho, BOOL fSimple, BOOL fInvalidate, SHORT cX, SHORT cY, HWND hwnd);
SHORT sOthello(BOARD Board, SHORT sX, SHORT sY, SHORT sCurrentLevel, SHORT sMaxLevel, SHORT sWho, PSHORT psX, PSHORT psY, PBOOL pfValid, SHORT sPrevBewertung);
BOOL fMustPass(BOARD Board, SHORT sWho);
BOOL fGameOver(BOARD Board);
SHORT sFlipped(PBOARD pBoard, SHORT sX, SHORT sY, SHORT sWho);
SHORT sSquare(BOARD Board, SHORT sOwn, SHORT sOpp, SHORT sX, SHORT sY);
int main(VOID);
MRESULT EXPENTRY wpMain(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY dpProdInfo(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
void SetPointer(SHORT x, SHORT y);
VOID DisplayMessage(PCH pchStr);
VOID InitHelp(VOID);
VOID HelpHelpForHelp(MPARAM);
VOID HelpExtended(MPARAM);
VOID HelpKeys(MPARAM);
VOID HelpIndex(MPARAM);
VOID DestroyHelpInstance(VOID);
VOID FixSysMenu(HWND hwndDlg);
VOID Result(BOARD Board, PSHORT psComputer, PSHORT psPlayer);
void Othello_Thread(ULONG ulParam);
MRESULT EXPENTRY dpEve(HWND, ULONG, MPARAM, MPARAM);
VOID vdGetWavFile(USHORT usEvent, PSZ pszWavFile);
VOID vdSetWavFile(USHORT usEvent, PSZ pszWavFile);
VOID _Optlink vdPlayWavFileAsyncEve(void *arg);
