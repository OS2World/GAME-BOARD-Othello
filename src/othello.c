/* othello.c - Othello for OS/2, Open Watcom port */
/* Original author: Peter Wansch, 1994              */
/* OW port / plan standardisation: OS2World, 2026   */

#include "othello.h"
#include "lang.h"

static const char bldlevel[] =
    "@#Peter Wansch:1.1#@##1## 11 Sep 2026 00:00:00      "
    "ARCAOS:::0::::@@Othello for OS/2 - Reversi board game\r\n\x1a";

/* ------------------------------------------------------------------ */
/* Globals                                                              */
/* ------------------------------------------------------------------ */

static HWND  hwndHelpInstance = NULLHANDLE;
static HWND  hwndFrame        = NULLHANDLE;
static HWND  hwndClient       = NULLHANDLE;
static HAB   hab               = NULLHANDLE;
static HMQ   hmq               = NULLHANDLE;
static HINI  hini              = NULLHANDLE;

static BOOL  fHelpEnabled  = FALSE;
static BOOL  fHelpInit     = FALSE;      /* InitHelp has run once */
static CHAR  szExeDir[CCHMAXPATH];       /* folder of the program, with a trailing backslash */
static CHAR  szHelpLib[CCHMAXPATH];      /* full name of the help file in use */

static VOID InitExeDir(VOID);
static BOOL  GameOver      = FALSE;
static BOOL  bSaveOnExit   = TRUE;       /* on by default */
static LONG  lColors       = 0;
static LONG  bgcolor       = CLR_DARKGREEN;

static HPOINTER hptrCross, hptrSystem, hptrWait;
static BOARD Board;
static BOARD BoardHilf;
static SHORT sLevel        = 0;
static BOOL  fPlayerStarts = TRUE;
static SHORT sVolumeEve    = 100;
static BOOL  fSound        = TRUE;
static CHAR  szApp[]       = "Othello";

static BOOL  fMMPMPresent  = FALSE;
static HMTX  hmtxSound     = NULLHANDLE;
static HMODULE hmodSound   = NULLHANDLE;
static PFN   pfnLoadWaveFile = NULL;
static PFN   pfnPlayWave     = NULL;
static ULONG ulPreviousWave[NUMBER_OF_EVENTS] = {0};

static PSZ pszEvent[] = {
    "Start game","End game","New game","Place piece",
    "Illegal move","Hint","Pass","Game won","Game lost","Draw game"
};
static PSZ pszDefWAVFiles[] = {
    "START.WAV","END.WAV","NEW.WAV","PLACE.WAV",
    "ILLEGAL.WAV","HINT.WAV","PASS.WAV","WON.WAV","LOST.WAV","DRAW.WAV"
};

/* Frame controls */
static HWND hwndTitleBar = NULLHANDLE;
static HWND hwndSysMenu  = NULLHANDLE;
static HWND hwndMinMax   = NULLHANDLE;
static HWND hwndMenuBar  = NULLHANDLE;
static HWND hwndPark     = NULLHANDLE;   /* hidden frame that holds the controls while they are hidden */
static BOOL bFrameHidden = FALSE;

/* ------------------------------------------------------------------ */
/* Settings persistence                                                 */
/* ------------------------------------------------------------------ */

VOID load_settings(VOID)
{
    ULONG cb;

    cb = sizeof(bgcolor);
    if (!PrfQueryProfileData(hini, szApp, "BGColor", &bgcolor, &cb))
        bgcolor = CLR_DARKGREEN;

    cb = sizeof(sVolumeEve);
    if (!PrfQueryProfileData(hini, szApp, "Volume", &sVolumeEve, &cb))
        sVolumeEve = 100;
    if (sVolumeEve < 0 || sVolumeEve > 100)
        sVolumeEve = 100;

    cb = sizeof(fPlayerStarts);
    if (!PrfQueryProfileData(hini, szApp, "PlayerStarts", &fPlayerStarts, &cb))
        fPlayerStarts = TRUE;

    cb = sizeof(fSound);
    if (!PrfQueryProfileData(hini, szApp, "Sound", &fSound, &cb))
        fSound = TRUE;

    cb = sizeof(sLevel);
    if (!PrfQueryProfileData(hini, szApp, "Level", &sLevel, &cb))
        sLevel = 0;
    sLevel = (SHORT)abs(sLevel % 3);

    cb = sizeof(bSaveOnExit);
    if (!PrfQueryProfileData(hini, szApp, "SaveOnExit", &bSaveOnExit, &cb))
        bSaveOnExit = TRUE;
}

VOID save_settings(VOID)
{
    PrfWriteProfileData(hini, szApp, "BGColor",      &bgcolor,       sizeof(bgcolor));
    PrfWriteProfileData(hini, szApp, "Volume",       &sVolumeEve,    sizeof(sVolumeEve));
    PrfWriteProfileData(hini, szApp, "PlayerStarts", &fPlayerStarts, sizeof(fPlayerStarts));
    PrfWriteProfileData(hini, szApp, "Sound",        &fSound,        sizeof(fSound));
    PrfWriteProfileData(hini, szApp, "Level",        &sLevel,        sizeof(sLevel));
    PrfWriteProfileData(hini, szApp, "SaveOnExit",   &bSaveOnExit,   sizeof(bSaveOnExit));
    save_lang();
}

VOID load_lang(VOID)
{
    ULONG cb = sizeof(current_lang);
    if (!PrfQueryProfileData(hini, szApp, "Language", &current_lang, &cb))
        current_lang = LANG_EN;
    if (current_lang < 0 || current_lang >= LANG_COUNT)
        current_lang = LANG_EN;
}

VOID save_lang(VOID)
{
    PrfWriteProfileData(hini, szApp, "Language", &current_lang, sizeof(current_lang));
}

/* ------------------------------------------------------------------ */
/* Language switching                                                   */
/* ------------------------------------------------------------------ */

VOID set_language(int lang)
{
    HWND hwndMenu, hwndSub;
    MENUITEM mi;
    int i;

    current_lang = lang;
    /* while the frame controls are hidden the menu bar is not a child of the frame */
    hwndMenu = (hwndMenuBar != NULLHANDLE) ? hwndMenuBar : WinWindowFromID(hwndFrame, FID_MENU);
    if (hwndMenu == NULLHANDLE)
        return;

    /* Game submenu */
    mi.iPosition = 0; mi.afStyle = 0; mi.afAttribute = 0;
    mi.id = IDM_SUBMENU_GAME; mi.hwndSubMenu = NULLHANDLE; mi.hItem = 0;
    WinSendMsg(hwndMenu, MM_QUERYITEM, MPFROM2SHORT(IDM_SUBMENU_GAME, TRUE), MPFROMP(&mi));
    hwndSub = mi.hwndSubMenu;
    if (hwndSub) {
        WinSendMsg(hwndSub, MM_SETITEMTEXT, MPFROMSHORT(IDM_NEW),   MPFROMP(tr(STR_MENU_NEW)));
        WinSendMsg(hwndSub, MM_SETITEMTEXT, MPFROMSHORT(IDM_HINT),  MPFROMP(tr(STR_MENU_HINT)));
        WinSendMsg(hwndSub, MM_SETITEMTEXT, MPFROMSHORT(IDM_QUIT),  MPFROMP(tr(STR_MENU_QUIT)));
        WinSendMsg(hwndSub, MM_SETITEMTEXT, MPFROMSHORT(IDM_EXIT),  MPFROMP(tr(STR_MENU_EXIT)));
    }
    WinSendMsg(hwndMenu, MM_SETITEMTEXT, MPFROMSHORT(IDM_SUBMENU_GAME), MPFROMP(tr(STR_MENU_GAME)));

    /* Options submenu */
    mi.hwndSubMenu = NULLHANDLE;
    WinSendMsg(hwndMenu, MM_QUERYITEM, MPFROM2SHORT(IDM_SUBMENU_OPTIONS, TRUE), MPFROMP(&mi));
    hwndSub = mi.hwndSubMenu;
    if (hwndSub) {
        HWND hwndLevel;
        MENUITEM miL;
        /* Level submenu */
        miL.hwndSubMenu = NULLHANDLE;
        WinSendMsg(hwndSub, MM_QUERYITEM, MPFROM2SHORT(IDM_SUBMENU_LEVEL, TRUE), MPFROMP(&miL));
        hwndLevel = miL.hwndSubMenu;
        if (hwndLevel) {
            WinSendMsg(hwndLevel, MM_SETITEMTEXT, MPFROMSHORT(IDM_BEGINNER), MPFROMP(tr(STR_MENU_BEGINNER)));
            WinSendMsg(hwndLevel, MM_SETITEMTEXT, MPFROMSHORT(IDM_ADVANCED), MPFROMP(tr(STR_MENU_ADVANCED)));
            WinSendMsg(hwndLevel, MM_SETITEMTEXT, MPFROMSHORT(IDM_MASTER),   MPFROMP(tr(STR_MENU_MASTER)));
        }
        WinSendMsg(hwndSub, MM_SETITEMTEXT, MPFROMSHORT(IDM_SUBMENU_LEVEL),   MPFROMP(tr(STR_MENU_LEVEL)));
        WinSendMsg(hwndSub, MM_SETITEMTEXT, MPFROMSHORT(IDM_PLAYER),          MPFROMP(tr(STR_MENU_PLAYER)));
        WinSendMsg(hwndSub, MM_SETITEMTEXT, MPFROMSHORT(IDM_COMPUTER),        MPFROMP(tr(STR_MENU_COMPUTER)));
        WinSendMsg(hwndSub, MM_SETITEMTEXT, MPFROMSHORT(IDM_SUBMENU_BGCOLOR), MPFROMP(tr(STR_MENU_BGCOLOR)));
        WinSendMsg(hwndSub, MM_SETITEMTEXT, MPFROMSHORT(IDM_SOUND),           MPFROMP(tr(STR_MENU_SOUND)));
        WinSendMsg(hwndSub, MM_SETITEMTEXT, MPFROMSHORT(IDM_SUBMENU_LANG),    MPFROMP(tr(STR_MENU_LANGUAGE)));
        WinSendMsg(hwndSub, MM_SETITEMTEXT, MPFROMSHORT(IDM_SAVEONEXIT),      MPFROMP(tr(STR_MENU_SAVEONEXIT)));
        WinSendMsg(hwndSub, MM_SETITEMTEXT, MPFROMSHORT(IDM_FRAME),           MPFROMP(tr(STR_MENU_FRAME)));
    }
    WinSendMsg(hwndMenu, MM_SETITEMTEXT, MPFROMSHORT(IDM_SUBMENU_OPTIONS), MPFROMP(tr(STR_MENU_OPTIONS)));

    /* Help submenu */
    mi.hwndSubMenu = NULLHANDLE;
    WinSendMsg(hwndMenu, MM_QUERYITEM, MPFROM2SHORT(IDM_SUBMENU_HELP, TRUE), MPFROMP(&mi));
    hwndSub = mi.hwndSubMenu;
    if (hwndSub) {
        WinSendMsg(hwndSub, MM_SETITEMTEXT, MPFROMSHORT(IDM_HELPINDEX),       MPFROMP(tr(STR_MENU_HELPINDEX)));
        WinSendMsg(hwndSub, MM_SETITEMTEXT, MPFROMSHORT(IDM_HELPEXTENDED),    MPFROMP(tr(STR_MENU_GENHELP)));
        WinSendMsg(hwndSub, MM_SETITEMTEXT, MPFROMSHORT(IDM_HELPKEYS),        MPFROMP(tr(STR_MENU_KEYSHELP)));
        WinSendMsg(hwndSub, MM_SETITEMTEXT, MPFROMSHORT(IDM_HELPHELPFORHELP), MPFROMP(tr(STR_MENU_USINGHELP)));
        WinSendMsg(hwndSub, MM_SETITEMTEXT, MPFROMSHORT(IDM_ABOUT),           MPFROMP(tr(STR_MENU_ABOUT)));
    }
    WinSendMsg(hwndMenu, MM_SETITEMTEXT, MPFROMSHORT(IDM_SUBMENU_HELP), MPFROMP(tr(STR_MENU_HELP)));

    /* Check the active language item */
    for (i = IDM_LANG_EN; i <= IDM_LANG_IT; i++)
        WinCheckMenuItem(hwndMenu, i, (i == IDM_LANG_EN + lang));

    if (hwndClient != NULLHANDLE)
        WinInvalidateRect(hwndClient, NULL, FALSE);

    /* the help file of the new language (same panel numbers in every file) */
    if (fHelpInit)
        InitHelp();
}

/* ------------------------------------------------------------------ */
/* Frame controls toggle (plan §16.10)                                  */
/* ------------------------------------------------------------------ */

VOID toggle_frame(VOID)
{
    RECTL rcl;
    HWND  hwndNew;

    /* The board keeps its size: the frame grows or shrinks around it */
    WinQueryWindowRect(hwndClient, &rcl);

    /* The controls move to the parking frame, never to HWND_OBJECT itself:
       that would clear their WS_VISIBLE flag, so they would not come back */
    hwndNew = bFrameHidden ? hwndFrame : hwndPark;
    WinSetParent(hwndTitleBar, hwndNew, FALSE);
    WinSetParent(hwndSysMenu,  hwndNew, FALSE);
    WinSetParent(hwndMinMax,   hwndNew, FALSE);
    WinSetParent(hwndMenuBar,  hwndNew, FALSE);
    bFrameHidden = !bFrameHidden;

    /* Tell the frame which controls changed */
    WinSendMsg(hwndFrame, WM_UPDATEFRAME,
        MPFROMLONG(FCF_TITLEBAR | FCF_SYSMENU | FCF_MINMAX | FCF_MENU), NULL);

    /* the title bar takes its text again when it comes back to the frame */
    WinSetWindowText(hwndFrame, szApp);
    WinSetWindowText(hwndTitleBar, szApp);

    /* Resize the frame around the unchanged client area */
    WinCalcFrameRect(hwndFrame, &rcl, FALSE);
    WinSetWindowPos(hwndFrame, HWND_TOP, 0, 0, rcl.xRight - rcl.xLeft,
                    rcl.yTop - rcl.yBottom, SWP_SIZE);

    WinInvalidateRect(hwndFrame, NULL, TRUE);
    WinUpdateWindow(hwndFrame);

    /* The menu bar handle is kept: while it is hidden it is not a child of the frame */
    WinCheckMenuItem(hwndMenuBar, IDM_FRAME, bFrameHidden);
}

/* ------------------------------------------------------------------ */
/* Error / message helpers                                              */
/* ------------------------------------------------------------------ */

VOID Error(BOOL fMessage)
{
    if (fMessage)
        DisplayMessage("Unrecoverable Presentation Manager error.");
    else
        WinAlarm(HWND_DESKTOP, WA_ERROR);
    if (hini != NULLHANDLE)
        PrfCloseProfile(hini);
    if (hwndFrame != NULLHANDLE)
        WinDestroyWindow(hwndFrame);
    if (hmodSound != NULLHANDLE)
        DosFreeModule(hmodSound);
    if (hmtxSound != NULLHANDLE)
        DosCloseMutexSem(hmtxSound);
    if (hmq != NULLHANDLE)
        WinDestroyMsgQueue(hmq);
    if (hab != NULLHANDLE)
        WinTerminate(hab);
    exit(RETURN_ERROR);
}

VOID DisplayMessage(PCH pchStr)
{
    WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, (PCH)pchStr, (PCH)szApp,
                  MB_MISC, MB_OK | MB_SYSTEMMODAL | MB_MOVEABLE | MB_ERROR);
}

/* ------------------------------------------------------------------ */
/* main()                                                               */
/* ------------------------------------------------------------------ */

int main(VOID)
{
    QMSG  qmsg;
    BOOL  success;
    HPS   hps;
    HDC   hdc;
    LONG  cxScreen, cyScreen, winW, winH, x, y;
    ULONG flCreate = FCF_TITLEBAR | FCF_SYSMENU | FCF_MINMAX |
                     FCF_MENU | FCF_ACCELTABLE | FCF_ICON |
                     FCF_TASKLIST;

    if (bldlevel[0] != '@')     /* keeps the BLDLEVEL string in the executable */
        return 1;

    InitExeDir();

    DosError(FERR_DISABLEHARDERR);

    hab = WinInitialize(0);
    if (!hab) Error(FALSE);

    hmq = WinCreateMsgQueue(hab, 0);
    if (!hmq) Error(FALSE);

    /* detect MMPM */
    {
        PSZ pcszMMPMBase;
        if (NO_ERROR == DosScanEnv("MMBASE", &pcszMMPMBase))
            fMMPMPresent = TRUE;
    }

    if (0 != DosOpenMutexSem("\\SEM32\\EPSOUND", &hmtxSound))
        if (0 != DosCreateMutexSem("\\SEM32\\EPSOUND", &hmtxSound, 0, FALSE))
            Error(TRUE);

    /* check color depth */
    hps = WinGetPS(HWND_DESKTOP);
    hdc = GpiQueryDevice(hps);
    if (hdc == NULLHANDLE || hdc == HDC_ERROR) { WinReleasePS(hps); Error(TRUE); }
    if (!DevQueryCaps(hdc, CAPS_PHYS_COLORS, CAPS_PHYS_COLORS, &lColors)) { WinReleasePS(hps); Error(TRUE); }
    WinReleasePS(hps);

    /* register window class */
    success = WinRegisterClass(hab, szApp, wpMain, CS_SIZEREDRAW, 0);
    if (!success) Error(TRUE);

    /* create frame (invisible) */
    hwndFrame = WinCreateStdWindow(HWND_DESKTOP, 0UL, &flCreate,
                                   szApp, "Othello", 0L,
                                   (HMODULE)NULL, WD_MAIN, &hwndClient);
    if (!hwndFrame) Error(TRUE);
    /* Set title immediately after creation, before PM can overwrite it */
    WinSetWindowText(hwndFrame, szApp);

    /* open settings INI */
    hini = PrfOpenProfile(hab, "ENTRTAIN.INI");
    if (hini == NULLHANDLE) Error(TRUE);

    load_settings();
    load_lang();

    /* capture frame child handles */
    hwndTitleBar = WinWindowFromID(hwndFrame, FID_TITLEBAR);
    hwndSysMenu  = WinWindowFromID(hwndFrame, FID_SYSMENU);
    hwndMinMax   = WinWindowFromID(hwndFrame, FID_MINMAX);
    hwndMenuBar  = WinWindowFromID(hwndFrame, FID_MENU);

    /* parking frame for the controls while they are hidden (Ctrl+F) */
    hwndPark = WinCreateWindow(HWND_OBJECT, WC_FRAME, "", 0L, 0, 0, 0, 0,
                               NULLHANDLE, HWND_TOP, 0, NULL, NULL);

    /* apply language */
    set_language(current_lang);

    /* apply save-on-exit checkmark */
    {
        HWND hm = WinWindowFromID(hwndFrame, FID_MENU);
        WinCheckMenuItem(hm, IDM_SAVEONEXIT, bSaveOnExit);
    }

    /* size and center window (plan §9) */
    cxScreen = WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN);
    cyScreen = WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN);
    winW = (cxScreen >= 800L) ? 800L : cxScreen;
    winH = (cyScreen >= 620L) ? 620L : cyScreen;
    x    = (cxScreen - winW) / 2;
    y    = (cyScreen - winH) / 2;
    /* Set title BEFORE showing the window so the first visible paint uses
       the correct text.  Also update the switch-list entry for XWorkplace. */
    WinSetWindowText(hwndFrame, szApp);
    WinSetWindowText(hwndTitleBar, szApp);
    {
        HSWITCH hswitch = WinQuerySwitchHandle(hwndFrame, 0);
        if (hswitch != NULLHANDLE) {
            SWCNTRL swctl;
            WinQuerySwitchEntry(hswitch, &swctl);
            strncpy(swctl.szSwtitle, szApp, sizeof(swctl.szSwtitle) - 1);
            swctl.szSwtitle[sizeof(swctl.szSwtitle) - 1] = '\0';
            WinChangeSwitchEntry(hswitch, &swctl);
        }
    }

    /* The help instance takes time to create: do it before the window shows */
    InitHelp();

    WinSetWindowPos(hwndFrame, HWND_TOP, x, y, winW, winH,
                    SWP_SIZE | SWP_MOVE | SWP_ACTIVATE | SWP_SHOW);

    /* Set again after show in case PM reset the text during activation */
    WinSetWindowText(hwndFrame, szApp);
    WinSetWindowText(hwndTitleBar, szApp);

    /* Paint the whole window now, so the board appears together with the
       title bar and not after the messages posted during start-up */
    WinUpdateWindow(hwndFrame);

    while (WinGetMsg(hab, &qmsg, (HWND)NULL, 0, 0))
        WinDispatchMsg(hab, &qmsg);

    DestroyHelpInstance();
    if (bFrameHidden) {            /* give the controls back before the frame goes */
        WinSetParent(hwndTitleBar, hwndFrame, FALSE);
        WinSetParent(hwndSysMenu,  hwndFrame, FALSE);
        WinSetParent(hwndMinMax,   hwndFrame, FALSE);
        WinSetParent(hwndMenuBar,  hwndFrame, FALSE);
        bFrameHidden = FALSE;
    }
    if (hwndPark != NULLHANDLE)
        WinDestroyWindow(hwndPark);
    if (WinIsWindow(hab, hwndFrame))
        WinDestroyWindow(hwndFrame);
    WinDestroyMsgQueue(hmq);
    WinTerminate(hab);
    return 0;
}

/* ------------------------------------------------------------------ */
/* Pointer helper                                                       */
/* ------------------------------------------------------------------ */

VOID SetPointer(SHORT sX, SHORT sY)
{
    if (GameOver || sX == 0 || sY == 0 || sX == 9 || sY == 9) {
        WinSetPointer(HWND_DESKTOP, hptrSystem);
        return;
    }
    if (fIsMovePossible(Board, sX - 1, sY - 1, PLAYER))
        WinSetPointer(HWND_DESKTOP, hptrCross);
    else
        WinSetPointer(HWND_DESKTOP, hptrSystem);
}

/* ------------------------------------------------------------------ */
/* AI thread                                                            */
/* ------------------------------------------------------------------ */

VOID APIENTRY Othello_Thread(ULONG ulParam)
{
    SHORT sX = -1, sY = -1;
    BOOL  fValid = FALSE;

    if (ulParam == 1)
        sOthello(BoardHilf, 0, 0, 0, sLevel + 2, COMPUTER, &sX, &sY, &fValid, 0);
    else {
        sOthello(Board, 0, 0, 0, sLevel + 2, COMPUTER, &sX, &sY, &fValid, 0);
        DosSleep(1000UL);
    }

    while (!WinPostQueueMsg(hmq, WMP_COMP_DONE, MPFROM2SHORT(sX, sY), MPFROMLONG(ulParam)))
        DosSleep(10UL);
    _endthread();
}

/* ------------------------------------------------------------------ */
/* Main window procedure                                                */
/* ------------------------------------------------------------------ */

MRESULT EXPENTRY wpMain(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    POINTL point;
    SHORT  sX, sY, sPlayer, sComputer;
    static CHAR   text[129];
    static BOOL   fPtrInstalled;
    static SHORT  cxBlock, cyBlock;
    static BOOL   fDisplayText;
    static BOOL   fThreadRunning;
    static BOOL   fFirst;
    static BOOL   fInitialMsg;
    static BOOL   fHintPending;
    RECTL  rect;
    TID    tidThread;

    switch (msg)
    {
    case WM_CREATE:
        WinDefWindowProc(hwnd, msg, mp1, mp2);
        fFirst        = TRUE;
        fDisplayText  = TRUE;
        fInitialMsg   = TRUE;
        fHintPending  = FALSE;
        fThreadRunning = FALSE;
        GameOver      = FALSE;

        srand((unsigned int)WinGetCurrentTime(hab));

        if (lColors == 2)
            sprintf(text, tr(STR_PLACE_BLACK));
        else
            sprintf(text, tr(STR_PLACE_BLUE));

        for (sX = 0; sX < 8; sX++)
            for (sY = 0; sY < 8; sY++)
                Board.sField[sX][sY] = EMPTY;
        Board.sField[3][3] = PLAYER;
        Board.sField[4][4] = PLAYER;
        Board.sField[3][4] = COMPUTER;
        Board.sField[4][3] = COMPUTER;

        hptrSystem = WinQuerySysPointer(HWND_DESKTOP, SPTR_ARROW, FALSE);
        hptrWait   = WinQuerySysPointer(HWND_DESKTOP, SPTR_WAIT, FALSE);
        hptrCross  = WinLoadPointer(HWND_DESKTOP, NULLHANDLE, PTR_CROSS);
        if (hptrCross == NULLHANDLE) hptrCross = hptrSystem;

        fPtrInstalled = (BOOL)WinQuerySysValue(HWND_DESKTOP, SV_MOUSEPRESENT);

        WinPostMsg(hwnd, WM_COMMAND, MPFROM2SHORT(IDM_NEW, 0), NULL);
        /* Set title after the message loop processes all init messages */
        WinPostMsg(hwnd, WMP_SETTITLE, 0, 0);
        /* Timer fallback: set title again 500ms after startup in case
           XWorkplace or PM init overwrites the switch entry after WMP_SETTITLE */
        WinStartTimer(hab, hwnd, TI_LOGO, 500);
        if (fSound) {
            PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT));
            if (pSE) { pSE->sEvent = 0; vdGetWavFile(pSE->sEvent, pSE->chWavFile); _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE); }
        }
        return (MRESULT)FALSE;

    case HM_QUERY_KEYS_HELP:
        return (MRESULT)PANEL_KEYSHELP;

    case WM_PRESPARAMCHANGED:
        if (LONGFROMMP(mp1) == PP_BACKGROUNDCOLOR) {
            HPS  hpsClient;
            LONG rgb2;
            if (WinQueryPresParam(hwnd, PP_BACKGROUNDCOLOR, 0, NULL, sizeof(rgb2), &rgb2, 0) > 0) {
                hpsClient = WinGetPS(hwnd);
                if (hpsClient) {
                    bgcolor = GpiQueryColorIndex(hpsClient, 0, rgb2);
                    WinReleasePS(hpsClient);
                    WinInvalidateRect(hwnd, NULL, FALSE);
                }
            }
        }
        break;

    case WM_MOUSEMOVE:
        if (fThreadRunning) { WinSetPointer(HWND_DESKTOP, hptrWait); return (MRESULT)TRUE; }
        if (cxBlock > 0 && cyBlock > 0)
            SetPointer(SHORT1FROMMP(mp1) / cxBlock, SHORT2FROMMP(mp1) / cyBlock);
        return (MRESULT)TRUE;

    case WM_SIZE:
        cxBlock = SHORT1FROMMP(mp2) / (DIVISIONS + 2);
        cyBlock = SHORT2FROMMP(mp2) / (DIVISIONS + 2);
        /* WM_SIZE fires synchronously during WinSetWindowPos — ensure title
           is correct each time the frame is resized or first shown */
        WinSetWindowText(hwndFrame, szApp);
        WinSetWindowText(hwndTitleBar, szApp);
        break;

    case WM_ACTIVATE:
        /* Re-set title on every activation in case XWorkplace overwrites it */
        if (SHORT1FROMMP(mp1)) {
            WinSetWindowText(hwndFrame, szApp);
            WinSetWindowText(hwndTitleBar, szApp);
        }
        break;

    case WM_SETFOCUS:
        if (!fPtrInstalled) WinShowPointer(HWND_DESKTOP, (BOOL)SHORT1FROMMP(mp2));
        break;

    case WM_CHAR:
        if (fThreadRunning) return (MRESULT)FALSE;
        if (cxBlock <= 0 || cyBlock <= 0) return (MRESULT)FALSE;
        WinQueryMsgPos(hab, &point);
        WinMapWindowPoints(HWND_DESKTOP, hwnd, &point, 1L);
        sX = (SHORT)(point.x / cxBlock);
        sY = (SHORT)(point.y / cyBlock);
        sX = max(0, min(DIVISIONS + 2, sX));
        sY = max(0, min(DIVISIONS + 2, sY));
        if ((SHORT1FROMMP(mp1) & KC_VIRTUALKEY) && !(SHORT1FROMMP(mp1) & KC_KEYUP)) {
            switch (SHORT2FROMMP(mp2)) {
            case VK_SPACE: case VK_NEWLINE: case VK_ENTER:
                WinSendMsg(hwnd, WM_USER, MPFROM2SHORT(sX,sY), NULL);
                return (MRESULT)TRUE;
            case VK_UP:    sY++; break;
            case VK_DOWN:  sY--; break;
            case VK_LEFT:  sX--; break;
            case VK_RIGHT: sX++; break;
            case VK_HOME:  sX = 1; sY = 8; break;
            case VK_END:   sX = 8; sY = 1; break;
            default:       return (MRESULT)FALSE;
            }
        } else return (MRESULT)FALSE;
        sX = (sX == 0) ? DIVISIONS : ((sX == DIVISIONS) ? DIVISIONS : (sX + DIVISIONS) % DIVISIONS);
        sY = (sY == 0) ? DIVISIONS : ((sY == DIVISIONS) ? DIVISIONS : (sY + DIVISIONS) % DIVISIONS);
        point.x = sX * cxBlock + cxBlock / 2;
        point.y = sY * cyBlock + cyBlock / 2;
        WinMapWindowPoints(hwnd, HWND_DESKTOP, &point, 1L);
        WinSetPointerPos(HWND_DESKTOP, point.x, point.y);
        return (MRESULT)TRUE;

    case WM_BUTTON1CLICK:
        if (fThreadRunning) break;
        sX = SHORT1FROMMP(mp1) / cxBlock;
        sY = SHORT2FROMMP(mp1) / cyBlock;
        WinSendMsg(hwnd, WM_USER, MPFROM2SHORT(sX,sY), NULL);
        break;

    case WM_USER:
        if (GameOver || fThreadRunning) break;
        sX = SHORT1FROMMP(mp1);
        sY = SHORT2FROMMP(mp1);
        if (fDisplayText) {
            fDisplayText = FALSE;
            fInitialMsg  = FALSE;
            rect.xLeft = 0; rect.xRight = (DIVISIONS+3)*cxBlock;
            rect.yTop = (DIVISIONS+3)*cyBlock; rect.yBottom = (DIVISIONS+1)*cyBlock;
            WinInvalidateRect(hwnd, &rect, FALSE);
        }
        if (sX == 0 || sX == 9 || sY == 0 || sY == 9) {
            if (fSound) { PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT)); if (pSE) { pSE->sEvent = 4; vdGetWavFile(pSE->sEvent, pSE->chWavFile); _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE); } }
            break;
        }
        sX--; sY--;
        if (!fIsMovePossible(Board, sX, sY, PLAYER)) {
            if (fSound) { PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT)); if (pSE) { pSE->sEvent = 4; vdGetWavFile(pSE->sEvent, pSE->chWavFile); _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE); } }
            break;
        }
        if (fSound) { PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT)); if (pSE) { pSE->sEvent = 3; vdGetWavFile(pSE->sEvent, pSE->chWavFile); _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE); } }
        MakeMove(&Board, sX, sY, PLAYER, TRUE, TRUE, cxBlock, cyBlock, hwnd);
        if (fSound) DosSleep(20);
        if (fGameOver(Board)) {
            GameOver = TRUE;
            if (cxBlock > 0 && cyBlock > 0) { WinQueryPointerPos(HWND_DESKTOP, &point); WinMapWindowPoints(HWND_DESKTOP, hwnd, &point, 1L); SetPointer((SHORT)(point.x/cxBlock), (SHORT)(point.y/cyBlock)); }
            Result(Board, &sComputer, &sPlayer);
            if (sComputer == sPlayer) {
                if (fSound) { PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT)); if (pSE) { pSE->sEvent = 9; vdGetWavFile(pSE->sEvent, pSE->chWavFile); _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE); } }
                sprintf(text, "%s", tr(STR_GAME_DRAW));
            } else if (sComputer > sPlayer) {
                if (fSound) { PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT)); if (pSE) { pSE->sEvent = 8; vdGetWavFile(pSE->sEvent, pSE->chWavFile); _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE); } }
                sprintf(text, tr(STR_YOU_LOST), sComputer - sPlayer);
            } else {
                if (fSound) { PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT)); if (pSE) { pSE->sEvent = 7; vdGetWavFile(pSE->sEvent, pSE->chWavFile); _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE); } }
                sprintf(text, tr(STR_YOU_WON), sPlayer - sComputer);
            }
            rect.xLeft = 0; rect.xRight = (DIVISIONS+3)*cxBlock;
            rect.yTop = (DIVISIONS+3)*cyBlock; rect.yBottom = (DIVISIONS+1)*cyBlock;
            WinInvalidateRect(hwnd, &rect, FALSE);
            break;
        }
        if (fMustPass(Board, COMPUTER)) {
            if (fSound) { PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT)); if (pSE) { pSE->sEvent = 6; vdGetWavFile(pSE->sEvent, pSE->chWavFile); _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE); } }
            fDisplayText = TRUE;
            sprintf(text, "%s", tr(STR_I_MUST_PASS));
            rect.xLeft = 0; rect.xRight = (DIVISIONS+3)*cxBlock;
            rect.yTop = (DIVISIONS+3)*cyBlock; rect.yBottom = (DIVISIONS+1)*cyBlock;
            WinInvalidateRect(hwnd, &rect, FALSE);
            if (cxBlock > 0 && cyBlock > 0) { WinQueryPointerPos(HWND_DESKTOP, &point); WinMapWindowPoints(HWND_DESKTOP, hwnd, &point, 1L); SetPointer((SHORT)(point.x/cxBlock), (SHORT)(point.y/cyBlock)); }
            break;
        }
        WinSetPointer(HWND_DESKTOP, hptrWait);
        DosCreateThread(&tidThread, (PFNTHREAD)Othello_Thread, 0UL, 0UL, 12288L);
        fThreadRunning = TRUE;
        break;

    case WMP_SETTITLE:
    case WM_TIMER:
    {
        HSWITCH hswitch;
        SWCNTRL swctl;
        if (msg == WM_TIMER) {
            if (SHORT1FROMMP(mp1) != TI_LOGO) break;
            WinStopTimer(hab, hwnd, TI_LOGO);
        }
        WinSetWindowText(hwndFrame, szApp);
        if (hwndTitleBar != NULLHANDLE)
            WinSetWindowText(hwndTitleBar, szApp);
        /* Update switch-list entry so XWorkplace title bar shows "Othello" */
        hswitch = WinQuerySwitchHandle(hwndFrame, 0);
        if (hswitch != NULLHANDLE) {
            WinQuerySwitchEntry(hswitch, &swctl);
            strncpy(swctl.szSwtitle, szApp, sizeof(swctl.szSwtitle) - 1);
            swctl.szSwtitle[sizeof(swctl.szSwtitle) - 1] = '\0';
            WinChangeSwitchEntry(hswitch, &swctl);
        }
        if (msg == WM_TIMER)
            break;
        return 0;
    }

    case WMP_COMP_DONE:
    {
        BOOL bHint = fHintPending;
        fHintPending = FALSE;
        sX = SHORT1FROMMP(mp1);
        sY = SHORT2FROMMP(mp1);
        fThreadRunning = FALSE;
        if (fDisplayText) {
            fDisplayText = FALSE;
            fInitialMsg  = FALSE;
            rect.xLeft = 0; rect.xRight = (DIVISIONS+3)*cxBlock;
            rect.yTop = (DIVISIONS+3)*cyBlock; rect.yBottom = (DIVISIONS+1)*cyBlock;
            WinInvalidateRect(hwnd, &rect, FALSE);
        }
        if (cxBlock > 0 && cyBlock > 0) { WinQueryPointerPos(HWND_DESKTOP, &point); WinMapWindowPoints(HWND_DESKTOP, hwnd, &point, 1L); SetPointer((SHORT)(point.x/cxBlock), (SHORT)(point.y/cyBlock)); }
        if (bHint) {
            if (sX >= 0 && sX < 8 && sY >= 0 && sY < 8) {
                point.x = (sX+1)*cxBlock + cxBlock/2;
                point.y = (sY+1)*cyBlock + cyBlock/2;
                WinMapWindowPoints(hwnd, HWND_DESKTOP, &point, 1L);
                WinSetPointerPos(HWND_DESKTOP, point.x, point.y);
            }
            break;
        }
        if (fSound) { PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT)); if (pSE) { pSE->sEvent = 3; vdGetWavFile(pSE->sEvent, pSE->chWavFile); _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE); } }
        MakeMove(&Board, sX, sY, COMPUTER, TRUE, TRUE, cxBlock, cyBlock, hwnd);
        if (fGameOver(Board)) {
            GameOver = TRUE;
            Result(Board, &sComputer, &sPlayer);
            if (sComputer == sPlayer) {
                if (fSound) { PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT)); if (pSE) { pSE->sEvent = 9; vdGetWavFile(pSE->sEvent, pSE->chWavFile); _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE); } }
                sprintf(text, "%s", tr(STR_GAME_DRAW));
            } else if (sComputer > sPlayer) {
                if (fSound) { PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT)); if (pSE) { pSE->sEvent = 8; vdGetWavFile(pSE->sEvent, pSE->chWavFile); _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE); } }
                sprintf(text, tr(STR_YOU_LOST), sComputer - sPlayer);
            } else {
                if (fSound) { PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT)); if (pSE) { pSE->sEvent = 7; vdGetWavFile(pSE->sEvent, pSE->chWavFile); _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE); } }
                sprintf(text, tr(STR_YOU_WON), sPlayer - sComputer);
            }
            rect.xLeft = 0; rect.xRight = (DIVISIONS+3)*cxBlock;
            rect.yTop = (DIVISIONS+3)*cyBlock; rect.yBottom = (DIVISIONS+1)*cyBlock;
            WinInvalidateRect(hwnd, &rect, FALSE);
            if (cxBlock > 0 && cyBlock > 0) { WinQueryPointerPos(HWND_DESKTOP, &point); WinMapWindowPoints(HWND_DESKTOP, hwnd, &point, 1L); SetPointer((SHORT)(point.x/cxBlock), (SHORT)(point.y/cyBlock)); }
            break;
        }
        if (fMustPass(Board, PLAYER)) {
            if (fSound) { PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT)); if (pSE) { pSE->sEvent = 6; vdGetWavFile(pSE->sEvent, pSE->chWavFile); _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE); } }
            fDisplayText = TRUE;
            sprintf(text, "%s", tr(STR_YOU_MUST_PASS));
            rect.xLeft = 0; rect.xRight = (DIVISIONS+3)*cxBlock;
            rect.yTop = (DIVISIONS+3)*cyBlock; rect.yBottom = (DIVISIONS+1)*cyBlock;
            WinInvalidateRect(hwnd, &rect, FALSE);
            WinSetPointer(HWND_DESKTOP, hptrWait);
            DosSleep(500UL);
            DosCreateThread(&tidThread, (PFNTHREAD)Othello_Thread, 0UL, 0UL, 12288L);
            fThreadRunning = TRUE;
        }
        break;
    }

    case WM_COMMAND:
        switch (SHORT1FROMMP(mp1))
        {
        case IDM_NEW:
            if (fThreadRunning) break;
            if (!fFirst && fSound) { PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT)); if (pSE) { pSE->sEvent = 2; vdGetWavFile(pSE->sEvent, pSE->chWavFile); _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE); } }
            if (!fFirst && fSound) DosSleep(800);   /* not at start-up: the window must appear at once */
            fFirst   = FALSE;
            GameOver = FALSE;
            for (sX = 0; sX < 8; sX++) for (sY = 0; sY < 8; sY++) Board.sField[sX][sY] = EMPTY;
            Board.sField[3][3] = PLAYER;  Board.sField[4][4] = PLAYER;
            Board.sField[3][4] = COMPUTER; Board.sField[4][3] = COMPUTER;
            WinInvalidateRect(hwnd, NULL, FALSE);
            if (cxBlock > 0 && cyBlock > 0) { WinQueryPointerPos(HWND_DESKTOP, &point); WinMapWindowPoints(HWND_DESKTOP, hwnd, &point, 1L); SetPointer((SHORT)(point.x/cxBlock), (SHORT)(point.y/cyBlock)); }
            if (!fPlayerStarts) {
                WinSetPointer(HWND_DESKTOP, hptrWait);
                DosCreateThread(&tidThread, (PFNTHREAD)Othello_Thread, 0UL, 0UL, 12288L);
                fThreadRunning = TRUE;
            }
            break;

        case IDM_HINT:
            if (fThreadRunning) break;
            if (fSound) { PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT)); if (pSE) { pSE->sEvent = 5; vdGetWavFile(pSE->sEvent, pSE->chWavFile); _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE); } }
            WinSetPointer(HWND_DESKTOP, hptrWait);
            for (sX = 0; sX < 8; sX++) for (sY = 0; sY < 8; sY++) {
                switch (Board.sField[sX][sY]) {
                case COMPUTER: BoardHilf.sField[sX][sY] = PLAYER;   break;
                case PLAYER:   BoardHilf.sField[sX][sY] = COMPUTER; break;
                default:       BoardHilf.sField[sX][sY] = EMPTY;    break;
                }
            }
            fHintPending = TRUE;
            DosCreateThread(&tidThread, (PFNTHREAD)Othello_Thread, 1UL, 0UL, 12288L);
            fThreadRunning = TRUE;
            break;

        case IDM_QUIT:
            if (fThreadRunning) break;
            GameOver = TRUE;
            for (sX = 0; sX < 8; sX++) for (sY = 0; sY < 8; sY++) Board.sField[sX][sY] = EMPTY;
            WinInvalidateRect(hwnd, NULL, FALSE);
            break;

        case IDM_EXIT:
            WinPostMsg(hwnd, WM_QUIT, 0L, 0L);
            break;

        case IDM_BEGINNER: if (!fThreadRunning) sLevel = 0; break;
        case IDM_ADVANCED: if (!fThreadRunning) sLevel = 1; break;
        case IDM_MASTER:   if (!fThreadRunning) sLevel = 2; break;

        case IDM_PLAYER:
            if (!fThreadRunning) { fPlayerStarts = TRUE;  WinSendMsg(hwnd, WM_COMMAND, MPFROM2SHORT(IDM_NEW,0), NULL); }
            break;
        case IDM_COMPUTER:
            if (!fThreadRunning) { fPlayerStarts = FALSE; WinSendMsg(hwnd, WM_COMMAND, MPFROM2SHORT(IDM_NEW,0), NULL); }
            break;

        case IDM_SOUND:
            WinDlgBox(HWND_DESKTOP, hwnd, (PFNWP)dpEve, NULLHANDLE, DB_EVE, NULL);
            break;

        case IDM_SAVEONEXIT:
            bSaveOnExit = !bSaveOnExit;
            WinCheckMenuItem(WinWindowFromID(hwndFrame, FID_MENU), IDM_SAVEONEXIT, bSaveOnExit);
            break;

        case IDM_FRAME:
            toggle_frame();
            break;

        case IDM_LANG_EN: set_language(LANG_EN); break;
        case IDM_LANG_ES: set_language(LANG_ES); break;
        case IDM_LANG_NL: set_language(LANG_NL); break;
        case IDM_LANG_DE: set_language(LANG_DE); break;
        case IDM_LANG_FR: set_language(LANG_FR); break;
        case IDM_LANG_IT: set_language(LANG_IT); break;

        case MI_COLOR0:  bgcolor = CLR_BLACK;    WinInvalidateRect(hwnd, NULL, FALSE); break;
        case MI_COLOR1:  bgcolor = CLR_BLUE;     WinInvalidateRect(hwnd, NULL, FALSE); break;
        case MI_COLOR2:  bgcolor = CLR_RED;      WinInvalidateRect(hwnd, NULL, FALSE); break;
        case MI_COLOR3:  bgcolor = CLR_PINK;     WinInvalidateRect(hwnd, NULL, FALSE); break;
        case MI_COLOR4:  bgcolor = CLR_GREEN;    WinInvalidateRect(hwnd, NULL, FALSE); break;
        case MI_COLOR5:  bgcolor = CLR_CYAN;     WinInvalidateRect(hwnd, NULL, FALSE); break;
        case MI_COLOR6:  bgcolor = CLR_YELLOW;   WinInvalidateRect(hwnd, NULL, FALSE); break;
        case MI_COLOR7:  bgcolor = CLR_WHITE;    WinInvalidateRect(hwnd, NULL, FALSE); break;
        case MI_COLOR8:  bgcolor = CLR_DARKGRAY; WinInvalidateRect(hwnd, NULL, FALSE); break;
        case MI_COLOR9:  bgcolor = CLR_DARKBLUE; WinInvalidateRect(hwnd, NULL, FALSE); break;
        case MI_COLOR10: bgcolor = CLR_DARKRED;  WinInvalidateRect(hwnd, NULL, FALSE); break;
        case MI_COLOR11: bgcolor = CLR_DARKPINK; WinInvalidateRect(hwnd, NULL, FALSE); break;
        case MI_COLOR12: bgcolor = CLR_DARKGREEN;WinInvalidateRect(hwnd, NULL, FALSE); break;
        case MI_COLOR13: bgcolor = CLR_DARKCYAN; WinInvalidateRect(hwnd, NULL, FALSE); break;
        case MI_COLOR14: bgcolor = CLR_BROWN;    WinInvalidateRect(hwnd, NULL, FALSE); break;
        case MI_COLOR15: bgcolor = CLR_PALEGRAY; WinInvalidateRect(hwnd, NULL, FALSE); break;

        case IDM_HELPHELPFORHELP: HelpHelpForHelp(mp2); break;
        case IDM_HELPEXTENDED:    HelpExtended(mp2);    break;
        case IDM_HELPKEYS:        HelpKeys(mp2);        break;
        case IDM_HELPINDEX:       HelpIndex(mp2);       break;

        case IDM_ABOUT:
            WinDlgBox(HWND_DESKTOP, hwnd, (PFNWP)AboutDlgProc, NULLHANDLE, IDD_ABOUT, NULL);
            break;

        default:
            return WinDefWindowProc(hwnd, msg, mp1, mp2);
        }
        break;

    case WM_INITMENU:
        WinEnableMenuItem(HWNDFROMMP(mp2), IDM_BEGINNER, !fThreadRunning);
        WinEnableMenuItem(HWNDFROMMP(mp2), IDM_ADVANCED, !fThreadRunning);
        WinEnableMenuItem(HWNDFROMMP(mp2), IDM_MASTER,   !fThreadRunning);
        WinEnableMenuItem(HWNDFROMMP(mp2), IDM_NEW,      !fThreadRunning);
        WinEnableMenuItem(HWNDFROMMP(mp2), IDM_HINT,     !GameOver && !fThreadRunning);
        WinEnableMenuItem(HWNDFROMMP(mp2), IDM_HELPINDEX,        fHelpEnabled);
        WinEnableMenuItem(HWNDFROMMP(mp2), IDM_HELPEXTENDED,     fHelpEnabled);
        WinEnableMenuItem(HWNDFROMMP(mp2), IDM_HELPKEYS,         fHelpEnabled);
        WinEnableMenuItem(HWNDFROMMP(mp2), IDM_HELPHELPFORHELP,  fHelpEnabled);
        WinEnableMenuItem(HWNDFROMMP(mp2), IDM_PLAYER,   !fPlayerStarts && !fThreadRunning);
        WinEnableMenuItem(HWNDFROMMP(mp2), IDM_COMPUTER, fPlayerStarts  && !fThreadRunning);
        WinCheckMenuItem(HWNDFROMMP(mp2), IDM_BEGINNER, sLevel == 0);
        WinCheckMenuItem(HWNDFROMMP(mp2), IDM_ADVANCED, sLevel == 1);
        WinCheckMenuItem(HWNDFROMMP(mp2), IDM_MASTER,   sLevel == 2);
        WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR0,  bgcolor == CLR_BLACK);
        WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR1,  bgcolor == CLR_BLUE);
        WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR2,  bgcolor == CLR_RED);
        WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR3,  bgcolor == CLR_PINK);
        WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR4,  bgcolor == CLR_GREEN);
        WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR5,  bgcolor == CLR_CYAN);
        WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR6,  bgcolor == CLR_YELLOW);
        WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR7,  bgcolor == CLR_WHITE);
        WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR8,  bgcolor == CLR_DARKGRAY);
        WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR9,  bgcolor == CLR_DARKBLUE);
        WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR10, bgcolor == CLR_DARKRED);
        WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR11, bgcolor == CLR_DARKPINK);
        WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR12, bgcolor == CLR_DARKGREEN);
        WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR13, bgcolor == CLR_DARKCYAN);
        WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR14, bgcolor == CLR_BROWN);
        WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR15, bgcolor == CLR_PALEGRAY);
        return (MRESULT)0;

    case WM_ERASEBACKGROUND:
        return (MRESULT)FALSE;

    case WM_DESTROY:
        if (bSaveOnExit)
            save_settings();
        else
            save_lang();
        WinDestroyPointer(hptrCross);
        if (fSound) {
            PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT));
            if (pSE) {
                int __tid;
                pSE->sEvent = 1;
                vdGetWavFile(pSE->sEvent, pSE->chWavFile);
                __tid = _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE);
                if (__tid != -1) DosSleep(2500);
            }
        }
        if (hini != NULLHANDLE) PrfCloseProfile(hini);
        break;

    case WM_PAINT:
    {
        HPS    hps;
        RECTL  rclUpdate, dest, src;
        POINTL pt[4];
        AREABUNDLE area;
        SHORT  sOffset;
        LONG   xAxis, yAxis;
        POINTL ptl;

        hps = WinBeginPaint(hwnd, 0L, &rclUpdate);

        src.xLeft = 0; src.yBottom = 0;
        src.xRight = cxBlock; src.yTop = (DIVISIONS+3)*cyBlock;
        if (WinIntersectRect(hab, &dest, &src, &rclUpdate)) WinFillRect(hps, &src, bgcolor);

        src.xLeft = cxBlock; src.yBottom = (DIVISIONS+1)*cyBlock+1;
        src.xRight = (DIVISIONS+3)*cxBlock; src.yTop = (DIVISIONS+3)*cyBlock;
        if (WinIntersectRect(hab, &dest, &src, &rclUpdate)) WinFillRect(hps, &src, bgcolor);

        src.xLeft = (DIVISIONS+1)*cxBlock+1; src.yBottom = 0;
        src.xRight = (DIVISIONS+3)*cxBlock; src.yTop = (DIVISIONS+2)*cyBlock;
        if (WinIntersectRect(hab, &dest, &src, &rclUpdate)) WinFillRect(hps, &src, bgcolor);

        src.xLeft = cxBlock; src.yBottom = 0;
        src.xRight = (DIVISIONS+1)*cxBlock+1; src.yTop = cyBlock+1;
        if (WinIntersectRect(hab, &dest, &src, &rclUpdate)) WinFillRect(hps, &src, bgcolor);

        /* draw board cells */
        GpiSetBackColor(hps, CLR_PALEGRAY);
        for (sX = 1; sX <= DIVISIONS; sX++)
            for (sY = 1; sY <= DIVISIONS; sY++) {
                src.xLeft = sX*cxBlock; src.yBottom = sY*cyBlock;
                src.xRight = (sX+1)*cxBlock+1; src.yTop = (sY+1)*cyBlock+1;
                if (WinIntersectRect(hab, &dest, &src, &rclUpdate))
                    WinDrawBorder(hps, &src, 1L, 1L, 0L, 0L, DB_INTERIOR|DB_AREAATTRS);
            }

        /* frame shadow */
        area.lColor = CLR_DARKGRAY;
        GpiSetAttrs(hps, PRIM_AREA, ABB_COLOR, 0UL, &area);
        src.xLeft = pt[0].x = cxBlock; src.yTop = pt[0].y = cyBlock;
        pt[1].x = cxBlock+cxBlock/4; pt[1].y = cyBlock-cyBlock/4;
        src.xRight = pt[2].x = (DIVISIONS+1)*cxBlock+cxBlock/4; src.yBottom = pt[2].y = cyBlock-cyBlock/4;
        pt[3].x = (DIVISIONS+1)*cxBlock; pt[3].y = cyBlock;
        if (WinIntersectRect(hab, &dest, &src, &rclUpdate)) { GpiBeginArea(hps,BA_BOUNDARY); GpiMove(hps,&pt[0]); GpiPolyLine(hps,4L,pt); GpiEndArea(hps); }
        pt[0].x=(DIVISIONS+1)*cxBlock; src.yTop=pt[0].y=(DIVISIONS+1)*cyBlock;
        pt[1].x=(DIVISIONS+1)*cxBlock+cxBlock/4; pt[1].y=(DIVISIONS+1)*cyBlock-cyBlock/4;
        pt[2].x=(DIVISIONS+1)*cxBlock+cxBlock/4; pt[2].y=cyBlock-cyBlock/4;
        src.xLeft=pt[3].x=(DIVISIONS+1)*cxBlock; pt[3].y=cyBlock;
        if (WinIntersectRect(hab, &dest, &src, &rclUpdate)) { GpiBeginArea(hps,BA_BOUNDARY); GpiMove(hps,&pt[0]); GpiPolyLine(hps,4L,pt); GpiEndArea(hps); }

        area.lColor = CLR_BLACK;
        GpiSetAttrs(hps, PRIM_AREA, ABB_COLOR, 0UL, &area);
        src.xLeft=cxBlock+cxBlock/2; src.yBottom=cyBlock/2;
        src.xRight=(DIVISIONS+1)*cxBlock+cxBlock/2+1; src.yTop=3*cyBlock/4+1;
        if (WinIntersectRect(hab,&dest,&src,&rclUpdate)) { GpiSetBackColor(hps,CLR_BLACK); WinDrawBorder(hps,&src,1L,1L,0L,0L,DB_INTERIOR|DB_AREAATTRS); }
        src.xLeft=(DIVISIONS+1)*cxBlock+cxBlock/4; src.yBottom=cyBlock/2;
        src.xRight=(DIVISIONS+1)*cxBlock+cxBlock/2+1; src.yTop=(DIVISIONS+1)*cyBlock-cyBlock/2+1;
        if (WinIntersectRect(hab,&dest,&src,&rclUpdate)) { GpiSetBackColor(hps,CLR_BLACK); WinDrawBorder(hps,&src,1L,1L,0L,0L,DB_INTERIOR|DB_AREAATTRS); }

        /* draw pieces */
        sOffset = (SHORT)(min(cxBlock, cyBlock) / 10);
        for (sX = 1; sX <= DIVISIONS; sX++)
            for (sY = 1; sY <= DIVISIONS; sY++)
                if (Board.sField[sX-1][sY-1] != EMPTY) {
                    src.xLeft = sX*cxBlock; src.yTop = (sY+1)*cyBlock;
                    src.xRight = (sX+1)*cxBlock; src.yBottom = sY*cyBlock;
                    if (WinIntersectRect(hab, &dest, &src, &rclUpdate)) {
                        if (Board.sField[sX-1][sY-1] == COMPUTER)
                            area.lColor = (lColors == 2) ? CLR_WHITE : CLR_RED;
                        else
                            area.lColor = (lColors == 2) ? CLR_BLACK : CLR_BLUE;
                        xAxis = src.xRight - src.xLeft;
                        yAxis = src.yTop - src.yBottom;
                        if (xAxis < yAxis) { src.yTop -= yAxis/2-xAxis/2; src.yBottom += yAxis/2-xAxis/2; }
                        else               { src.xRight -= xAxis/2-yAxis/2; src.xLeft += xAxis/2-yAxis/2; }
                        src.xLeft += sOffset; src.yTop -= sOffset;
                        src.xRight -= sOffset; src.yBottom += sOffset;
                        ptl.x = src.xLeft; ptl.y = src.yBottom;
                        GpiMove(hps, &ptl);
                        ptl.x = src.xRight; ptl.y = src.yTop;
                        GpiSetAttrs(hps, PRIM_AREA, ABB_COLOR, 0UL, &area);
                        GpiBox(hps, DRO_OUTLINEFILL, &ptl, xAxis, yAxis);
                    }
                }

        /* status text */
        if (GameOver || fDisplayText) {
            const char *pmsg = (fInitialMsg && !GameOver)
                ? tr(lColors == 2 ? STR_PLACE_BLACK : STR_PLACE_BLUE)
                : text;
            WinQueryWindowRect(hwnd, &src);
            src.yBottom = src.yTop - cyBlock;
            GpiSetColor(hps, (bgcolor != CLR_WHITE) ? CLR_WHITE : CLR_BLACK);
            WinDrawText(hps, -1L, (PCH)pmsg, &src, 0L, 0L, DT_TEXTATTRS|DT_CENTER|DT_VCENTER);
        }
        WinEndPaint(hps);
    }
    break;

    case WM_CLOSE:
        WinPostMsg(hwnd, WM_QUIT, NULL, NULL);
        break;

    default:
        return WinDefWindowProc(hwnd, msg, mp1, mp2);
    }
    return (MRESULT)FALSE;
}

/* ------------------------------------------------------------------ */
/* About dialog (plan §11)                                              */
/* ------------------------------------------------------------------ */

MRESULT EXPENTRY AboutDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg) {
    case WM_COMMAND:
        switch (COMMANDMSG(&msg)->cmd) {
        case DID_OK:
        case DID_CANCEL:
            WinDismissDlg(hwnd, TRUE);
            return 0L;
        }
    }
    return WinDefDlgProc(hwnd, msg, mp1, mp2);
}

/* ------------------------------------------------------------------ */
/* Help system                                                          */
/* ------------------------------------------------------------------ */

/* One help file per language, in the help folder next to the program */
static const char *pszHelpFiles[LANG_COUNT] = {
    "Othello_en.hlp", "Othello_es.hlp", "Othello_nl.hlp",
    "Othello_de.hlp", "Othello_fr.hlp", "Othello_it.hlp"
};

static VOID InitExeDir(VOID)
{
    PTIB  ptib;
    PPIB  ppib;
    PCHAR p;

    szExeDir[0] = '\0';
    if (DosGetInfoBlocks(&ptib, &ppib) == 0)
        if (DosQueryModuleName(ppib->pib_hmte, sizeof(szExeDir), szExeDir) == 0) {
            p = strrchr(szExeDir, '\\');
            if (p) *(p + 1) = '\0';
            else   szExeDir[0] = '\0';
        }
}

VOID InitHelp(VOID)
{
    HELPINIT heini;
    FILE    *fp;
    CHAR     szMsg[CCHMAXPATH + 200];
    const char *pszFile = pszHelpFiles[(current_lang >= 0 && current_lang < LANG_COUNT) ? current_lang : LANG_EN];

    /* drop the help instance of the previous language */
    if (hwndHelpInstance != 0L) {
        WinAssociateHelpInstance(NULLHANDLE, hwndFrame);
        WinDestroyHelpInstance(hwndHelpInstance);
        hwndHelpInstance = 0L;
    }
    fHelpEnabled = FALSE;
    fHelpInit    = TRUE;

    strcpy(szHelpLib, szExeDir);
    strcat(szHelpLib, "help\\");
    strcat(szHelpLib, pszFile);
    fp = fopen(szHelpLib, "rb");
    if (fp)
        fclose(fp);
    else
        strcpy(szHelpLib, pszFile);      /* let the system look along HELP and BOOKSHELF */

    memset(&heini, 0, sizeof(heini));
    heini.cb = sizeof(HELPINIT);
    heini.phtHelpTable = (PHELPTABLE)MAKELONG(MAIN_HELP_TABLE, 0xFFFF);
    heini.pszHelpWindowTitle = tr(STR_HELP_TITLE);
    heini.fShowPanelId = CMIC_HIDE_PANEL_ID;
    heini.pszHelpLibraryName = szHelpLib;
    hwndHelpInstance = WinCreateHelpInstance(hab, &heini);
    if (hwndHelpInstance == 0L || heini.ulReturnCode) {
        hwndHelpInstance = 0L;
        sprintf(szMsg, tr(STR_HELP_MISSING), pszFile);
        WinMessageBox(HWND_DESKTOP, hwndFrame, szMsg, szApp,
                      MB_MISC, MB_OK|MB_APPLMODAL|MB_MOVEABLE|MB_ERROR);
        return;
    }
    if (!WinAssociateHelpInstance(hwndHelpInstance, hwndFrame)) {
        WinDestroyHelpInstance(hwndHelpInstance);
        hwndHelpInstance = 0L;
        sprintf(szMsg, tr(STR_HELP_MISSING), pszFile);
        WinMessageBox(HWND_DESKTOP, hwndFrame, szMsg, szApp,
                      MB_MISC, MB_OK|MB_APPLMODAL|MB_MOVEABLE|MB_ERROR);
        return;
    }
    fHelpEnabled = TRUE;
}

VOID HelpHelpForHelp(MPARAM mp2)
{
    if (fHelpEnabled)
        if ((LONG)WinSendMsg(hwndHelpInstance, HM_DISPLAY_HELP, 0L, 0L))
            WinMessageBox(HWND_DESKTOP, hwndFrame, "Unable to display help information.", szApp,
                          MB_MISC, MB_OK|MB_APPLMODAL|MB_MOVEABLE|MB_ERROR);
}

VOID HelpExtended(MPARAM mp2)
{
    if (fHelpEnabled)
        if ((LONG)WinSendMsg(hwndHelpInstance, HM_EXT_HELP, 0L, 0L))
            WinMessageBox(HWND_DESKTOP, hwndFrame, "Unable to display help information.", szApp,
                          MB_MISC, MB_OK|MB_APPLMODAL|MB_MOVEABLE|MB_ERROR);
}

VOID HelpKeys(MPARAM mp2)
{
    if (fHelpEnabled)
        if ((LONG)WinSendMsg(hwndHelpInstance, HM_KEYS_HELP, 0L, 0L))
            WinMessageBox(HWND_DESKTOP, hwndFrame, "Unable to display help information.", szApp,
                          MB_MISC, MB_OK|MB_APPLMODAL|MB_MOVEABLE|MB_ERROR);
}

VOID HelpIndex(MPARAM mp2)
{
    if (fHelpEnabled)
        if ((LONG)WinSendMsg(hwndHelpInstance, HM_HELP_INDEX, 0L, 0L))
            WinMessageBox(HWND_DESKTOP, hwndFrame, "Unable to display help information.", szApp,
                          MB_MISC, MB_OK|MB_APPLMODAL|MB_MOVEABLE|MB_ERROR);
}

VOID DestroyHelpInstance(VOID)
{
    if (hwndHelpInstance != 0L)
        WinDestroyHelpInstance(hwndHelpInstance);
}

/* ------------------------------------------------------------------ */
/* Sound system                                                         */
/* ------------------------------------------------------------------ */

VOID vdGetWavFile(USHORT usEvent, PSZ pszWavFile)
{
    CHAR  szWavFile[CCHMAXPATHCOMP];
    ULONG ulBufSize = sizeof(szWavFile);
    if (!PrfQueryProfileData(hini, szApp, pszEvent[usEvent], &szWavFile, &ulBufSize)) {
        if (fMMPMPresent)
            strcpy(pszWavFile, pszDefWAVFiles[usEvent]);
        else
            strcpy(pszWavFile, "<Beep>");
    } else
        strcpy(pszWavFile, szWavFile);
}

VOID vdSetWavFile(USHORT usEvent, PSZ pszWavFile)
{
    CHAR szWavFile[CCHMAXPATHCOMP];
    strcpy(szWavFile, pszWavFile);
    PrfWriteProfileData(hini, szApp, pszEvent[usEvent], &szWavFile, sizeof(szWavFile));
}

static ULONG GPFHandler(PEXCEPTIONREPORTRECORD pxcptrec, PEXCEPTIONREGISTRATIONRECORD prr, PCONTEXTRECORD pcr, PVOID pv)
{
    if (pxcptrec->ExceptionNum == XCPT_ACCESS_VIOLATION) {
        fMMPMPresent = FALSE;
        _endthread();
    }
    return XCPT_CONTINUE_SEARCH;
}

VOID vdPlayWavFileAsyncEve(void *arg)
{
    SHORT  i;
    CHAR   LoadError[100];
    CHAR   szTemp[CCHMAXPATH];
    APIRET rc;
    PSOUNDEVENT pSE = (PSOUNDEVENT)arg;
    ULONG  ulTemp;
    EXCEPTIONREGISTRATIONRECORD xcpthand = {(PEXCEPTIONREGISTRATIONRECORD)0, (_ERR * volatile)GPFHandler};

    rc = DosRequestMutexSem(hmtxSound, 100);
    if (NO_ERROR != rc) { free(pSE); _endthread(); return; }

    DosSetExceptionHandler(&xcpthand);

    if (strcmp(pSE->chWavFile, "<No sound>") == 0) goto _release;

    if (strcmp(pSE->chWavFile, "<Beep>") == 0) {
        switch (pSE->sEvent) {
        case 0: case 7: case 2:
            DosBeep(261,100); DosBeep(330,100); DosBeep(392,100); DosBeep(523,500); break;
        case 3: DosBeep(261,50); break;
        case 4: DosBeep(523,50); break;
        case 5: DosBeep(261,100); DosBeep(392,150); break;
        case 6: DosBeep(523,100); DosBeep(392,150); break;
        case 1: case 8: case 9:
            DosBeep(523,100); DosBeep(392,100); DosBeep(330,100); DosBeep(261,500); break;
        }
        goto _release;
    }

    if (!fMMPMPresent) goto _release;

    if (hmodSound == NULLHANDLE) {
        rc = DosLoadModule(LoadError, sizeof(LoadError), "EPSOUND", &hmodSound);
        if (rc != 0) goto _release;
        if (0 != DosQueryProcAddr(hmodSound, 1L, NULL, &pfnLoadWaveFile)) goto _release;
        if (0 != DosQueryProcAddr(hmodSound, 2L, NULL, &pfnPlayWave))     goto _release;
    }

    if (pSE->sEvent == -1 || pSE->sEvent == 1) {
        ulTemp = pfnLoadWaveFile(pSE->chWavFile, sVolumeEve);
        if (ulTemp != 0) pfnPlayWave(ulTemp, hwndFrame, TRUE);
        goto _release;
    } else {
        if (ulPreviousWave[pSE->sEvent] == 0) {
            for (i = 0; i < NUMBER_OF_EVENTS; i++) {
                if (ulPreviousWave[i] == 0 || i == pSE->sEvent) continue;
                vdGetWavFile(i, szTemp);
                if (0 == strcmpi(szTemp, pSE->chWavFile)) {
                    ulPreviousWave[pSE->sEvent] = ulPreviousWave[i];
                    goto _weiter;
                }
            }
            ulPreviousWave[pSE->sEvent] = pfnLoadWaveFile(pSE->chWavFile, sVolumeEve);
            if (ulPreviousWave[pSE->sEvent] == 0) goto _release;
        }
    }
_weiter:
    pfnPlayWave(ulPreviousWave[pSE->sEvent], hwndFrame, FALSE);

_release:
    free(pSE);
    DosUnsetExceptionHandler(&xcpthand);
    DosReleaseMutexSem(hmtxSound);
    _endthread();
}

/* ------------------------------------------------------------------ */
/* Sound dialog                                                         */
/* ------------------------------------------------------------------ */

MRESULT EXPENTRY dpEve(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    MRESULT mr;
    SHORT   j, i;
    HDIR    hDir;
    APIRET  rc;
    static ULONG          ulWavCount;
    static FILEFINDBUF3   findWavBuffer[MAX_NUMBER_OF_WAV_FILES];
    static CHAR           szTemp[CCHMAXPATH];
    static SHORT          sVolumeEveTemp;

    switch (msg)
    {
    case WM_INITDLG:
        sVolumeEveTemp = sVolumeEve;
        WinSendDlgItemMsg(hwnd, SL_EVESOUNDS, SLM_SETSLIDERINFO,
            MPFROM2SHORT(SMA_SLIDERARMPOSITION, SMA_INCREMENTVALUE),
            MPFROMSHORT(sVolumeEve));
        WinSendDlgItemMsg(hwnd, CB_EVESOUNDS, BM_SETCHECK, MPFROMSHORT(fSound), NULL);
        WinEnableWindow(WinWindowFromID(hwnd, LB_EVESOUNDS), fSound);
        WinEnableWindow(WinWindowFromID(hwnd, SL_EVESOUNDS), fSound);
        if (!fMMPMPresent) { ulWavCount = 0UL; goto _liste; }
        hDir = HDIR_SYSTEM;
        ulWavCount = 1UL;
        rc = DosFindFirst("*.WAV", &hDir, 0UL, (PVOID)&findWavBuffer[0], sizeof(findWavBuffer[0]), &ulWavCount, FIL_STANDARD);
        if (0 != rc && 2 != rc && 18 != rc) { ulWavCount = 0; goto _liste; }
        if (rc == 2 || rc == 18) ulWavCount = 0;
        else {
            do {
                ULONG ulNextCount = 1UL;
                rc = DosFindNext(hDir, (PVOID)&findWavBuffer[ulWavCount], sizeof(findWavBuffer[ulWavCount]), &ulNextCount);
                if (0 != rc && rc != 18) goto _liste;
                if (rc != 18) ulWavCount++;
            } while (rc != 18 && ulWavCount < MAX_NUMBER_OF_WAV_FILES);
        }
_liste:
        WinInsertLboxItem(WinWindowFromID(hwnd, LB_EVESOUNDS), LIT_END, "<No sound>");
        WinInsertLboxItem(WinWindowFromID(hwnd, LB_EVESOUNDS), LIT_END, "<Beep>");
        for (i = 0; i < (SHORT)ulWavCount; i++)
            WinInsertLboxItem(WinWindowFromID(hwnd, LB_EVESOUNDS), LIT_END, findWavBuffer[i].achName);
        for (i = 0; i < NUMBER_OF_EVENTS; i++)
            WinInsertLboxItem(WinWindowFromID(hwnd, LB_EVE), LIT_END, pszEvent[i]);
        WinSendMsg(WinWindowFromID(hwnd, LB_EVE), LM_SELECTITEM, MPFROMSHORT(0), MPFROMLONG(TRUE));
        break;

    case WM_CONTROL:
    {
        BOOL fSoundsCheck = (BOOL)WinSendDlgItemMsg(hwnd, CB_EVESOUNDS, BM_QUERYCHECK, NULL, NULL) && fMMPMPresent;
        mr = WinSendMsg(WinWindowFromID(hwnd, LB_EVE), LM_QUERYSELECTION, MPFROMSHORT(0), MPFROMLONG(0));
        i  = SHORT1FROMMP(mr);
        mr = WinSendMsg(WinWindowFromID(hwnd, LB_EVESOUNDS), LM_QUERYSELECTION, MPFROMSHORT(0), MPFROMLONG(0));
        j  = SHORT1FROMMP(mr);
        switch (SHORT1FROMMP(mp1)) {
        case LB_EVESOUNDS:
            WinEnableWindow(WinWindowFromID(hwnd, PB_PLAYEVE), (j > 1) && fSoundsCheck);
            WinEnableWindow(WinWindowFromID(hwnd, SL_EVESOUNDS), (j > 1) && fSoundsCheck);
            if (SHORT2FROMMP(mp1) == LN_ENTER) {
                WinSendDlgItemMsg(hwnd, LB_EVESOUNDS, LM_QUERYITEMTEXT, MPFROM2SHORT(j, sizeof(szTemp)), MPFROMP(szTemp));
                vdSetWavFile(i, szTemp);
            }
            break;
        case LB_EVE:
            if (SHORT2FROMMP(mp1) == LN_SELECT) {
                BOOL fFound = FALSE;
                CHAR szWavFile[CCHMAXPATHCOMP];
                vdGetWavFile(i, (PSZ)&szWavFile);
                if (strcmp(szWavFile, "") == 0)
                    WinSendMsg(WinWindowFromID(hwnd, LB_EVESOUNDS), LM_SELECTITEM, MPFROMSHORT(0), MPFROMLONG(TRUE));
                else if (strcmp(szWavFile, "<Beep>") == 0)
                    WinSendMsg(WinWindowFromID(hwnd, LB_EVESOUNDS), LM_SELECTITEM, MPFROMSHORT(1), MPFROMLONG(TRUE));
                else {
                    for (j = 0; j < (SHORT)ulWavCount; j++) {
                        if (strcmpi(szWavFile, findWavBuffer[j].achName) == 0) {
                            fFound = TRUE;
                            WinSendMsg(WinWindowFromID(hwnd, LB_EVESOUNDS), LM_SELECTITEM, MPFROMSHORT(j+2), MPFROMLONG(TRUE));
                            break;
                        }
                    }
                    if (!fFound)
                        WinSendMsg(WinWindowFromID(hwnd, LB_EVESOUNDS), LM_SELECTITEM, MPFROMSHORT(0), MPFROMLONG(TRUE));
                }
            }
            break;
        case CB_EVESOUNDS:
            WinEnableWindow(WinWindowFromID(hwnd, LB_EVE),      fSoundsCheck);
            WinEnableWindow(WinWindowFromID(hwnd, LB_EVESOUNDS),fSoundsCheck);
            WinEnableWindow(WinWindowFromID(hwnd, SL_EVESOUNDS),(j > 1) && fSoundsCheck);
            WinEnableWindow(WinWindowFromID(hwnd, PB_PLAYEVE),  (j > 1) && fSoundsCheck);
            break;
        case SL_EVESOUNDS:
            sVolumeEve = (SHORT)WinSendDlgItemMsg(hwnd, SL_EVESOUNDS, SLM_QUERYSLIDERINFO,
                MPFROM2SHORT(SMA_SLIDERARMPOSITION, SMA_INCREMENTVALUE), NULL);
            break;
        }
    }
    break;

    case WM_COMMAND:
    {
        mr = WinSendMsg(WinWindowFromID(hwnd, LB_EVE), LM_QUERYSELECTION, MPFROMSHORT(0), MPFROMLONG(0));
        i  = SHORT1FROMMP(mr);
        mr = WinSendMsg(WinWindowFromID(hwnd, LB_EVESOUNDS), LM_QUERYSELECTION, MPFROMSHORT(0), MPFROMLONG(0));
        j  = SHORT1FROMMP(mr);
        switch (SHORT1FROMMP(mp1)) {
        case DID_OK:
            fSound = (BOOL)WinSendDlgItemMsg(hwnd, CB_EVESOUNDS, BM_QUERYCHECK, NULL, NULL);
            WinSendDlgItemMsg(hwnd, LB_EVESOUNDS, LM_QUERYITEMTEXT, MPFROM2SHORT(j, sizeof(szTemp)), MPFROMP(szTemp));
            vdSetWavFile(i, szTemp);
            WinDismissDlg(hwnd, TRUE);
            break;
        case DID_CANCEL:
            sVolumeEve = sVolumeEveTemp;
            WinDismissDlg(hwnd, FALSE);
            break;
        case PB_PLAYEVE:
            if (j > 1) {
                PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT));
                if (pSE) {
                    pSE->sEvent = -1;
                    WinSendDlgItemMsg(hwnd, LB_EVESOUNDS, LM_QUERYITEMTEXT, MPFROM2SHORT(j, sizeof(szTemp)), MPFROMP(szTemp));
                    strcpy(pSE->chWavFile, szTemp);
                    _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE);
                }
            }
            break;
        }
    }
    break;

    default:
        return WinDefDlgProc(hwnd, msg, mp1, mp2);
    }
    return (MRESULT)0;
}
