/* file: othello.C                                      */
/* source code file                                     */
/* author: Peter Wansch                                 */

#define INCL_DOSEXCEPTIONS
#include "othello.h"

/* global variables */
static HWND hwndHelpInstance;
static HWND hwndFrame = NULLHANDLE;                        /* frame window handle          */
static HAB hab = NULLHANDLE;                               /* anchor block handle          */
static BOOL fHelpEnabled = FALSE;
static BOOL GameOver = FALSE;
static LONG lColors;
static HPOINTER hptrCross, hptrSystem, hptrWait;
static BOARD Board;                           /* game board */
static BOARD BoardHilf;
static SHORT sLevel;
static HMQ hmq = NULLHANDLE;                               /* message queue handle */
static HINI hini = NULLHANDLE;
static BOOL fMMPMPresent = FALSE;
static HMTX hmtxSound = NULLHANDLE;
#define NUMBER_OF_EVENTS 10
#ifdef GERMAN
static PSZ pszEvent[] = {"Spielbeginn", "Spielende", "Neues Spiel", "Spielstein positionieren", "UngÅltiger Spielzug", "Hinweis", "Passen", "Spiel gewonnen", "Spiel verloren", "Spiel unentschieden"};
static PSZ pszDefWAVFiles[] = {"BEGINN.WAV", "ENDE.WAV", "NEU.WAV", "HINLEGEN.WAV", "UNGUELTG.WAV", "HINWEIS.WAV", "PASSEN.WAV", "GEWONNEN.WAV", "VERLOREN.WAV", "UNENTSCH.WAV"};
#else
static PSZ pszEvent[] = {"Start game", "End game", "New game", "Place piece", "Illegal move", "Hint", "Pass", "Game won", "Game lost", "Draw game"};
static PSZ pszDefWAVFiles[] = {"START.WAV", "END.WAV", "NEW.WAV", "PLACE.WAV", "ILLEGAL.WAV", "HINT.WAV", "PASS.WAV", "WON.WAV", "LOST.WAV", "DRAW.WAV"};
#endif
static SHORT sVolumeEve;
static BOOL fSound = TRUE;
static HMODULE hmodSound = NULLHANDLE;
static CHAR szApp[] = "Othello";
static LONG bgcolor = CLR_DARKGREEN;
static PFN pfnLoadWaveFile = NULL;
static PFN pfnPlayWave = NULL;
static ULONG ulPreviousWave[NUMBER_OF_EVENTS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/* ctldata */
struct _ctldata {
  USHORT cb;
  BOOL fParam;
};

/* error procedure */
VOID Error(BOOL fMessage)
{
  if(fMessage)
#ifdef GERMAN
    DisplayMessage("Nicht behebbarer Anwendungsfehler.");
#else
    DisplayMessage("Unrecoverable Presentation Manager error.");
#endif
  else
    WinAlarm(HWND_DESKTOP, WA_ERROR);
  if(hini != NULLHANDLE)
    PrfCloseProfile(hini);
  if (hwndFrame != NULLHANDLE)
    WinDestroyWindow(hwndFrame);
  if(hmodSound != NULLHANDLE)
    DosFreeModule(hmodSound);
  if(hmtxSound != NULLHANDLE)
    DosCloseMutexSem(hmtxSound);
  if (hmq != NULLHANDLE)
    WinDestroyMsgQueue(hmq);
  if (hab != NULLHANDLE)
    WinTerminate(hab);
  exit(RETURN_ERROR);
}

/* main procedure */
int main(VOID)
{
  QMSG qmsg;
  BOOL success;
  HPS hps;
  HWND hwndClient;                    /* client window handle */
  ULONG flCreate = FCF_STANDARD & ~FCF_SHELLPOSITION;
  HDC hdc;

  DosError(FERR_DISABLEHARDERR);

  /* initialize the application */
  hab = WinInitialize(0);
  if(!hab)
    Error(FALSE);
  /* create message queue for application */
  hmq = WinCreateMsgQueue(hab, 0);
  if(!hmq)
    Error(FALSE);

  /* is multimedia support installed? */
  {
    PSZ pcszMMPMBase;
    if (NO_ERROR == DosScanEnv("MMBASE", &pcszMMPMBase))
      fMMPMPresent = TRUE;
  }

  if(0 != DosOpenMutexSem("\\SEM32\\EPSOUND", &hmtxSound))
  {
    if(0 != DosCreateMutexSem("\\SEM32\\EPSOUND", &hmtxSound, 0, FALSE))
      Error(TRUE);
  }

  /* display timed logo */
  {
    struct _ctldata c;
    c.cb = sizeof(c);
    c.fParam = TRUE;
    if (!WinDlgBox(HWND_DESKTOP, HWND_DESKTOP, (PFNWP)dpProdInfo, NULLHANDLE, DB_PRODINFO, (PVOID)&c))
      Error(TRUE);
  }

  hps = WinGetPS(HWND_DESKTOP);
  hdc = GpiQueryDevice(hps);
  if (hdc == NULLHANDLE || hdc == HDC_ERROR)
    Error(TRUE);

  /* if lColors is 2 there is a display driver installed for a monochrome device */
  success = DevQueryCaps(hdc, CAPS_PHYS_COLORS, CAPS_PHYS_COLORS, &lColors);
  if (!success)
  {
    WinReleasePS (hps);
    Error(TRUE);
  }

  WinReleasePS (hps);

  /* register private window class */
  success = WinRegisterClass(hab, szApp, wpMain, CS_SIZEREDRAW, 0);
  if (!success)
    Error(TRUE);

  /* create frame and client window */
  hwndFrame = WinCreateStdWindow(HWND_DESKTOP, 0UL, &flCreate, szApp, "", WS_SYNCPAINT, (HMODULE)NULL, WD_MAIN, &hwndClient);

  if (!hwndFrame)
    Error(TRUE);

  if (!WinRestoreWindowPos(szApp, "WinParams", hwndFrame))
  {
    SWP swp;

    /* get the dimensions and the shell's suggested location for the window */
    if (WinQueryTaskSizePos(hab, 0, &swp) != 0)
      Error(TRUE);

    /* set the frame window position */
    swp.fl = SWP_SIZE | SWP_MOVE | SWP_ZORDER;
    if (!WinSetWindowPos(hwndFrame, HWND_TOP, swp.x, swp.y, swp.cx, swp.cy, swp.fl))
      Error(TRUE);
  }

  if (!WinSetWindowPos(hwndFrame, NULLHANDLE, 0L, 0L, 0L, 0L, SWP_ACTIVATE))
    Error(TRUE);

  if (!WinShowWindow(hwndFrame,TRUE))
    Error(TRUE);

  /* set window text */
  WinSetWindowText(hwndFrame, szApp);

  /* initialize help instance */
  InitHelp();

  /* get and dispatch messages from the application message queue       */
  /* until WinGetMsg returns FALSE, indicating a WM_QUIT message.       */
  while(WinGetMsg(hab, &qmsg, (HWND)NULL, 0, 0))
    WinDispatchMsg(hab, &qmsg);

  DestroyHelpInstance();
  if(WinIsWindow(hab, hwndFrame))
    WinDestroyWindow(hwndFrame);        /* tidy up...                   */
  WinDestroyMsgQueue(hmq);            /* and                          */
  WinTerminate(hab);                  /* terminate the application    */
  return(0);
}

/* main window procedure */
MRESULT EXPENTRY wpMain(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  POINTL point;
  SHORT sX, sY, sPlayer, sComputer;
  static CHAR text[129];
  static BOOL fPtrInstalled;
  static SHORT cxBlock, cyBlock;
  static BOOL fPlayerStarts;
  static BOOL fDisplayText;
  ULONG ulBufSize;
  RECTL rect;
  static BOOL fThreadRunning;
  static BOOL fFirst;
  TID tidThread;

  switch( msg )
  {
    case WM_CREATE:
      WinDefWindowProc(hwnd, msg, mp1, mp2);
      fFirst = TRUE;
      hini = PrfOpenProfile(hab, "ENTRTAIN.INI");
      if (NULLHANDLE == hini)
        return ((MRESULT)TRUE);
      /* initialize random nummer generator */
      srand ((unsigned int)WinGetCurrentTime(hab));

      fDisplayText = TRUE;
      if (lColors == 2)
#ifdef GERMAN
        sprintf(text, "Sie spielen mit den schwarzen Steinen.");
      else
        sprintf(text, "Sie spielen mit den blauen Steinen.");
#else
        sprintf(text, "You place the black markers.");
      else
        sprintf(text, "You place the blue markers.");
#endif

      fThreadRunning = FALSE;
      GameOver = FALSE;
      for(sX = 0; sX < 8; sX++)
        for(sY = 0; sY < 8; sY++)
          Board.sField[sX][sY] = EMPTY;

      Board.sField[3][3] = PLAYER;
      Board.sField[4][4] = PLAYER;
      Board.sField[3][4] = COMPUTER;
      Board.sField[4][3] = COMPUTER;

      /* retrieve settings from profile */
      ulBufSize = sizeof(bgcolor);
      if(!PrfQueryProfileData(hini, szApp, "BGColor", &bgcolor, &ulBufSize))
        bgcolor = CLR_DARKGREEN;

      ulBufSize = sizeof(sVolumeEve);
      if (!PrfQueryProfileData(hini, szApp, "Volume", &sVolumeEve, &ulBufSize))
        sVolumeEve = 100;
      if(sVolumeEve < 0 || sVolumeEve > 100)
        sVolumeEve = 100;

      ulBufSize = sizeof(fPlayerStarts);
      if (!PrfQueryProfileData(hini, szApp, "PlayerStarts", &fPlayerStarts, &ulBufSize))
        fPlayerStarts = TRUE;

      ulBufSize = sizeof(fSound);
      if (!PrfQueryProfileData(hini, szApp, "Sound", &fSound, &ulBufSize))
        fSound = TRUE;

      ulBufSize = sizeof(sLevel);
      if (!PrfQueryProfileData(hini, szApp, "Level", &sLevel, &ulBufSize))
        sLevel = 0;
      sLevel = abs(sLevel % 3);

      /* load pointers */
      hptrSystem = WinQuerySysPointer(HWND_DESKTOP, SPTR_ARROW, FALSE);
      hptrWait = WinQuerySysPointer(HWND_DESKTOP, SPTR_WAIT, FALSE);
      if ((hptrCross = WinLoadPointer(HWND_DESKTOP, NULLHANDLE, PTR_CROSS)) == NULLHANDLE)
        hptrCross = hptrSystem;

      if (WinQuerySysValue(HWND_DESKTOP, SV_MOUSEPRESENT))
        fPtrInstalled = TRUE;
      else
        fPtrInstalled = FALSE;
      WinPostMsg(hwnd, WM_COMMAND, MPFROM2SHORT(MI_NEW, 0), NULL);
      if(fSound)
      {
        PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT));
        if(pSE != NULL)
        {
          pSE->sEvent = 0;
          vdGetWavFile(pSE->sEvent, pSE->chWavFile);
          _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE);
        }
      } 
      return ((MRESULT)FALSE);
    
    case HM_QUERY_KEYS_HELP:
      return ((MRESULT)PANEL_KEYSHELP);

    case WM_PRESPARAMCHANGED:
      if(LONGFROMMP(mp1) == PP_BACKGROUNDCOLOR)
      {
        HPS hpsClient;
        LONG rgb2;
        LONG cbRetLen = WinQueryPresParam(hwnd, PP_BACKGROUNDCOLOR, 0, NULL, sizeof(rgb2), &rgb2, 0);
        if(cbRetLen > 0)
        {
          hpsClient = WinGetPS(hwnd);
          if(hpsClient != NULLHANDLE)
          {
            bgcolor = GpiQueryColorIndex(hpsClient, 0, rgb2);
            WinReleasePS(hpsClient);
            WinInvalidateRect(hwnd, NULL, FALSE);
          }
        }
      }
      break;

    case WM_MOUSEMOVE:
      if (fThreadRunning)
      {
        WinSetPointer(HWND_DESKTOP, hptrWait);
        return ((MRESULT) TRUE);
      }
      if (cxBlock > 0 && cyBlock > 0)
        SetPointer(SHORT1FROMMP(mp1) / cxBlock, SHORT2FROMMP(mp1) / cyBlock);
      return ((MRESULT) TRUE);

    case WM_SIZE:
      cxBlock = SHORT1FROMMP(mp2) / (DIVISIONS + 2);
      cyBlock = SHORT2FROMMP(mp2) / (DIVISIONS + 2);
      break;

    case WM_CHAR:
      if (fThreadRunning)
        return ((MRESULT)FALSE);

      if (cxBlock <= 0 || cyBlock <= 0)
        return ((MRESULT)FALSE);
      WinQueryMsgPos(hab, &point);
      WinMapWindowPoints(HWND_DESKTOP, hwnd, &point, 1L);
      sX = point.x / cxBlock;
      sY = point.y / cyBlock;
      sX = max(0, min(DIVISIONS + 2, sX));
      sY = max(0, min(DIVISIONS + 2, sY));

      if ((SHORT1FROMMP(mp1) & KC_VIRTUALKEY) && !(SHORT1FROMMP(mp1) & KC_KEYUP))
        switch (SHORT2FROMMP(mp2)) 
        {
          case VK_SPACE:
          case VK_NEWLINE:
          case VK_ENTER:
            WinSendMsg(hwnd, WM_USER, MPFROM2SHORT(sX,sY), NULL);
            return ((MRESULT)TRUE);

          case VK_UP:
            sY++;
            break;

          case VK_DOWN:
            sY--;
            break;

          case VK_LEFT:
            sX--;
            break;

          case VK_RIGHT:
            sX++;
            break;

          case VK_HOME:
            sX = 1;
            sY = 8;
            break;

          case VK_END:
            sX = 8;
            sY = 1;
            break;

          default: 
            return ((MRESULT)FALSE);
        } 
        else 
          return ((MRESULT)FALSE);

        switch (sX)
        {
          case 0:
            sX = DIVISIONS;
            break;

          case DIVISIONS:
            break;

          default:
            sX = (sX + DIVISIONS) % (DIVISIONS);
        }

        switch (sY)
        {
          case 0:
            sY = DIVISIONS;
            break;

          case DIVISIONS: 
            break;

          default:
            sY = (sY + DIVISIONS) % (DIVISIONS);
        }

        point.x = sX * cxBlock + cxBlock / 2;
        point.y = sY * cyBlock + cyBlock / 2;
        WinMapWindowPoints(hwnd, HWND_DESKTOP, &point, 1L);
        WinSetPointerPos(HWND_DESKTOP, point.x, point.y);
        return ((MRESULT)TRUE);

    case WM_SETFOCUS:
      if (!fPtrInstalled)
        if ((USHORT)SHORT1FROMMP(mp2))
          WinShowPointer(HWND_DESKTOP, TRUE);
        else
          WinShowPointer(HWND_DESKTOP, FALSE);
      break;

    case WM_BUTTON1CLICK:    
      if (fThreadRunning)
        break;
      sX = SHORT1FROMMP(mp1) / cxBlock;
      sY = SHORT2FROMMP(mp1) / cyBlock;
      WinSendMsg(hwnd, WM_USER, MPFROM2SHORT(sX,sY), NULL);
      break;

    case WM_USER:
      /* benutzer mîchte einen Zug machen */
      if (GameOver || fThreadRunning)
        break;

      sX = SHORT1FROMMP(mp1);
      sY = SHORT2FROMMP(mp1);

      if (fDisplayText)
      {
        /* erase text */
        fDisplayText = FALSE;
        rect.xLeft = 0;
        rect.xRight = (DIVISIONS + 3) * cxBlock;
        rect.yTop = (DIVISIONS + 3) * cyBlock;
        rect.yBottom = (DIVISIONS+1) * cyBlock;
        WinInvalidateRect(hwnd, &rect, FALSE);
      }

      if (sX == 0 || sX == 9 || sY == 0 || sY == 9)
      {
        if(fSound)
        {
          PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT));
          if(pSE != NULL)
          {
            pSE->sEvent = 4;
            vdGetWavFile(pSE->sEvent, pSE->chWavFile);
            _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE);
          }
        } 
        break;
      }

      /* Mauszeiger ist am Spielbrett, berechnen der Brett-Koordinaten */
      sX--;
      sY--;

      if(!fIsMovePossible(Board, sX, sY, PLAYER))
      {
        /* Zug ist ungÅltig */
        if(fSound)
        {
          PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT));
          if(pSE != NULL)
          {
            pSE->sEvent = 4;
            vdGetWavFile(pSE->sEvent, pSE->chWavFile);
            _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE);
          }
        } 
        break;
      }

      /* Zug ist gÅltig und Spieler zieht */
      if(fSound)
      {
        PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT));
        if(pSE != NULL)
        {
          pSE->sEvent = 3;
          vdGetWavFile(pSE->sEvent, pSE->chWavFile);
          _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE);
        }
      }
      MakeMove(&Board, sX, sY, PLAYER, TRUE, TRUE, cxBlock, cyBlock, hwnd);
      if(fSound)
        DosSleep(20);

      /* ist das Spiel schon vorbei? */
      if (fGameOver(Board))
      {
        /* Spiel ist vorbei */
        GameOver = TRUE;
        if (cxBlock > 0 && cyBlock > 0)
        {
          WinQueryPointerPos(HWND_DESKTOP, &point);
          WinMapWindowPoints(HWND_DESKTOP, hwnd, &point, 1L);
          point.x /= cxBlock;
          point.y /= cyBlock;
          SetPointer(point.x, point.y);
        }
        Result(Board, &sComputer, &sPlayer);
        if(sComputer == sPlayer)
        {
          if(fSound)
          {
            PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT));
            if(pSE != NULL)
            {
              pSE->sEvent = 9;
              vdGetWavFile(pSE->sEvent, pSE->chWavFile);
              _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE);
            }
          }
#ifdef GERMAN
          sprintf(text, "Das Spiel ist unentschieden.");
#else
          sprintf(text, "The game is a draw.");
#endif
          rect.xLeft = 0;
          rect.xRight = (DIVISIONS + 3) * cxBlock;
          rect.yTop = (DIVISIONS + 3) * cyBlock;
          rect.yBottom = (DIVISIONS+1) * cyBlock;
          WinInvalidateRect(hwnd, &rect, FALSE);
        }
        else
        {
          if(sComputer > sPlayer)
          {
            if(fSound)
            {
              PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT));
              if(pSE != NULL)
              {
                pSE->sEvent = 8;
                vdGetWavFile(pSE->sEvent, pSE->chWavFile);
                _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE);
              }
            }
#ifdef GERMAN
            sprintf(text, "Sie haben um %d Punkte verloren.", sComputer - sPlayer);
#else
            sprintf(text, "You lost by %d.", sComputer - sPlayer);
#endif
            /* Text anzeigen */
            rect.xLeft = 0;
            rect.xRight = (DIVISIONS + 3) * cxBlock;
            rect.yTop = (DIVISIONS + 3) * cyBlock;
            rect.yBottom = (DIVISIONS+1) * cyBlock;
            WinInvalidateRect(hwnd, &rect, FALSE);
          }
          else
          {
            if(fSound)
            {
              PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT));
              if(pSE != NULL)
              {
                pSE->sEvent = 7;
                vdGetWavFile(pSE->sEvent, pSE->chWavFile);
                _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE);
              }
            }
#ifdef GERMAN
            sprintf(text, "Sie haben um %d Punkte gewonnen.", sPlayer - sComputer);
#else
            sprintf(text, "You won by %d.", sPlayer - sComputer);
#endif
            rect.xLeft = 0;
            rect.xRight = (DIVISIONS + 3) * cxBlock;
            rect.yTop = (DIVISIONS + 3) * cyBlock;
            rect.yBottom = (DIVISIONS+1) * cyBlock;
            WinInvalidateRect(hwnd, &rect, FALSE);
          }
        }
        break;
      }

      /* muss der Computer passen? */
      if (fMustPass(Board, COMPUTER))
      {
        if(fSound)
        {
          PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT));
          if(pSE != NULL)
          {
            pSE->sEvent = 6;
            vdGetWavFile(pSE->sEvent, pSE->chWavFile);
            _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE);
          }
        }
        /* display I must pass text */
        fDisplayText = TRUE;
#ifdef GERMAN
        sprintf(text, "Ich mu· passen.");
#else
        sprintf(text, "I must pass.");
#endif
        rect.xLeft = 0;
        rect.xRight = (DIVISIONS + 3) * cxBlock;
        rect.yTop = (DIVISIONS + 3) * cyBlock;
        rect.yBottom = (DIVISIONS+1) * cyBlock;
        WinInvalidateRect(hwnd, &rect, FALSE);
        /* cursor richten */
        if (cxBlock > 0 && cyBlock > 0)
        {
          WinQueryPointerPos(HWND_DESKTOP, &point);
          WinMapWindowPoints(HWND_DESKTOP, hwnd, &point, 1L);
          point.x /= cxBlock;
          point.y /= cyBlock;
          SetPointer(point.x, point.y);
        }
        break;
      }

      /* der Computer macht einen Zug */
      WinSetPointer(HWND_DESKTOP, hptrWait);
      DosCreateThread(&tidThread, &Othello_Thread, 0UL, 0UL, 12288L);
      fThreadRunning = TRUE;
      break;

    case WMP_COMP_DONE:
      sX = SHORT1FROMMP(mp1);
      sY = SHORT2FROMMP(mp1);

      fThreadRunning = FALSE;

      if (fDisplayText)
      {
        /* erase text */
        fDisplayText = FALSE;
        rect.xLeft = 0;
        rect.xRight = (DIVISIONS + 3) * cxBlock;
        rect.yTop = (DIVISIONS + 3) * cyBlock;
        rect.yBottom = (DIVISIONS+1) * cyBlock;
        WinInvalidateRect(hwnd, &rect, FALSE);
      }

      if (cxBlock > 0 && cyBlock > 0)
      {
        WinQueryPointerPos(HWND_DESKTOP, &point);
        WinMapWindowPoints(HWND_DESKTOP, hwnd, &point, 1L);
        point.x /= cxBlock;
        point.y /= cyBlock;
        SetPointer(point.x, point.y);
      }

      if(LONGFROMMP(mp2) == 1L)
      {
        /* vorgeschlagener Move ist jetzt in sX, sY */
        point.x = (sX + 1) * cxBlock + cxBlock / 2;
        point.y = (sY + 1) * cyBlock + cyBlock / 2;
        WinMapWindowPoints(hwnd, HWND_DESKTOP, &point, 1L);
        WinSetPointerPos(HWND_DESKTOP, point.x, point.y);
        break;
      }

      if(fSound)
      {
        PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT));
        if(pSE != NULL)
        {
          pSE->sEvent = 3;
          vdGetWavFile(pSE->sEvent, pSE->chWavFile);
          _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE);
        }
      }
      MakeMove(&Board, sX, sY, COMPUTER, TRUE, TRUE, cxBlock, cyBlock, hwnd);
      /* ist das Spiel schon vorbei? */
      if (fGameOver(Board))
      {
        /* Spiel ist vorbei */
        GameOver = TRUE;
        Result(Board, &sComputer, &sPlayer);
        if(sComputer == sPlayer)
        {
          if(fSound)
          {
            PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT));
            if(pSE != NULL)
            {
              pSE->sEvent = 9;
              vdGetWavFile(pSE->sEvent, pSE->chWavFile);
              _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE);
            }
          }
#ifdef GERMAN
          sprintf(text, "Das Spiel ist unentschieden.");
#else
          sprintf(text, "The game is a draw.");
#endif
        }
        else
        {
          if(sComputer > sPlayer)
          {
            if(fSound)
            {
              PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT));
              if(pSE != NULL)
              {
                pSE->sEvent = 8;
                vdGetWavFile(pSE->sEvent, pSE->chWavFile);
                _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE);
              }
            }
#ifdef GERMAN
            sprintf(text, "Sie haben um %d Punkte verloren.", sComputer - sPlayer);
#else
            sprintf(text, "You lost by %d.", sComputer - sPlayer);
#endif
          }
          else
          {
            if(fSound)
            {
              PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT));
              if(pSE != NULL)
              {
                pSE->sEvent = 7;
                vdGetWavFile(pSE->sEvent, pSE->chWavFile);
                _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE);
              }
            }
#ifdef GERMAN
            sprintf(text, "Sie haben um %d Punkte gewonnen.", sPlayer - sComputer);
#else
            sprintf(text, "You won by %d.", sPlayer - sComputer);
#endif
          }
        }
        /* Text anzeigen */
        rect.xLeft = 0;
        rect.xRight = (DIVISIONS + 3) * cxBlock;
        rect.yTop = (DIVISIONS + 3) * cyBlock;
        rect.yBottom = (DIVISIONS+1) * cyBlock;
        WinInvalidateRect(hwnd, &rect, FALSE);
        if (cxBlock > 0 && cyBlock > 0)
        {
          WinQueryPointerPos(HWND_DESKTOP, &point);
          WinMapWindowPoints(HWND_DESKTOP, hwnd, &point, 1L);
          point.x /= cxBlock;
          point.y /= cyBlock;
          SetPointer(point.x, point.y);
        }
        break;
      }
      if (fMustPass(Board, PLAYER))
      {
        if(fSound)
        {
          PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT));
          if(pSE != NULL)
          {
            pSE->sEvent = 6;
            vdGetWavFile(pSE->sEvent, pSE->chWavFile);
            _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE);
          }
        }
        /* display You must pass text */
        fDisplayText = TRUE;
#ifdef GERMAN
        sprintf(text, "Sie mÅssen passen.");
#else
        sprintf(text, "You must pass.");
#endif
        rect.xLeft = 0;
        rect.xRight = (DIVISIONS + 3) * cxBlock;
        rect.yTop = (DIVISIONS + 3) * cyBlock;
        rect.yBottom = (DIVISIONS+1) * cyBlock;
        WinInvalidateRect(hwnd, &rect, FALSE);
        WinSetPointer(HWND_DESKTOP, hptrWait);
        DosSleep(500UL);
        DosCreateThread(&tidThread, &Othello_Thread, 0UL, 0UL, 12288L);
        fThreadRunning = TRUE;
      }
      break;

    case WM_COMMAND:
      switch (SHORT1FROMMP(mp1))
      {
        case MI_COLOR0:
          bgcolor = CLR_BLACK;
          WinInvalidateRect(hwnd, NULL, FALSE);
          break;

        case MI_COLOR1:
          bgcolor = CLR_BLUE;
          WinInvalidateRect(hwnd, NULL, FALSE);
          break;

        case MI_COLOR2:
          bgcolor = CLR_RED;
          WinInvalidateRect(hwnd, NULL, FALSE);
          break;

        case MI_COLOR3:
          bgcolor = CLR_PINK;
          WinInvalidateRect(hwnd, NULL, FALSE);
          break;

        case MI_COLOR4:
          bgcolor = CLR_GREEN;
          WinInvalidateRect(hwnd, NULL, FALSE);
          break;

        case MI_COLOR5:
          bgcolor = CLR_CYAN;
          WinInvalidateRect(hwnd, NULL, FALSE);
          break;

        case MI_COLOR6:
          bgcolor = CLR_YELLOW;
          WinInvalidateRect(hwnd, NULL, FALSE);
          break;

        case MI_COLOR7:
          bgcolor = CLR_WHITE;
          WinInvalidateRect(hwnd, NULL, FALSE);
          break;

        case MI_COLOR8:
          bgcolor = CLR_DARKGRAY;
          WinInvalidateRect(hwnd, NULL, FALSE);
          break;

        case MI_COLOR9:
          bgcolor = CLR_DARKBLUE;
          WinInvalidateRect(hwnd, NULL, FALSE);
          break;

        case MI_COLOR10:
          bgcolor = CLR_DARKRED;
          WinInvalidateRect(hwnd, NULL, FALSE);
          break;

        case MI_COLOR11:
          bgcolor = CLR_DARKPINK;
          WinInvalidateRect(hwnd, NULL, FALSE);
          break;

        case MI_COLOR12:
          bgcolor = CLR_DARKGREEN;
          WinInvalidateRect(hwnd, NULL, FALSE);
          break;

        case MI_COLOR13:
          bgcolor = CLR_DARKCYAN;
          WinInvalidateRect(hwnd, NULL, FALSE);
          break;

        case MI_COLOR14:
          bgcolor = CLR_BROWN;
          WinInvalidateRect(hwnd, NULL, FALSE);
          break;

        case MI_COLOR15:
          bgcolor = CLR_PALEGRAY;
          WinInvalidateRect(hwnd, NULL, FALSE);
          break;

        case MI_HELPHELPFORHELP:
          HelpHelpForHelp(mp2);
          break;

        case MI_HELPEXTENDED:
          HelpExtended(mp2);
          break;

        case MI_HELPKEYS:
          HelpKeys(mp2);
          break;

        case HM_QUERY_KEYS_HELP:
          return ((MRESULT)PANEL_KEYSHELP);

        case MI_HELPINDEX:
          HelpIndex(mp2);
          break;

        case MI_HELPPRODUCTINFO:
          {
            struct _ctldata c;
            c.cb = sizeof(c);
            c.fParam = FALSE;
            WinDlgBox(HWND_DESKTOP, hwnd, (PFNWP)dpProdInfo, NULLHANDLE, DB_PRODINFO, (PVOID)&c);
          }
          break;

        case MI_BEGINNER:
          if (fThreadRunning)
            break;
          sLevel = 0;
          break;

        case MI_ADVANCED:
          if (fThreadRunning)
            break;
          sLevel = 1;
          break;

        case MI_MASTER:
          if (fThreadRunning)
            break;
          sLevel = 2;
          break;

        case MI_SOUND:
          WinDlgBox(HWND_DESKTOP, hwnd, (PFNWP)dpEve, NULLHANDLE, DB_EVE, NULL);
          break;

        case MI_PLAYER:
          if (fThreadRunning)
            break;
          fPlayerStarts = TRUE;
          WinSendMsg(hwnd, WM_COMMAND, MPFROM2SHORT(MI_NEW, 0), NULL);
          break;

        case MI_COMPUTER:
          if (fThreadRunning)
            break;
          fPlayerStarts = FALSE;
          WinSendMsg(hwnd, WM_COMMAND, MPFROM2SHORT(MI_NEW, 0), NULL);
          break;

        case MI_HINT:
          if (fThreadRunning)
            break;
          if(fSound)
          {
            PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT));
            if(pSE != NULL)
            {
              pSE->sEvent = 5;
              vdGetWavFile(pSE->sEvent, pSE->chWavFile);
              _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE);
            }
          }
          WinSetPointer(HWND_DESKTOP, hptrWait);
          for(sX = 0; sX < 8; sX++)
            for(sY = 0; sY < 8; sY++)
            {
              switch(Board.sField[sX][sY])
              {
                case COMPUTER:
                  BoardHilf.sField[sX][sY] = PLAYER;
                  break;

                case PLAYER:
                  BoardHilf.sField[sX][sY] = COMPUTER;
                  break;

                default:
                  BoardHilf.sField[sX][sY] = EMPTY;
                  break;
              }
            }

         /* der Computer macht einen Zug */
         WinSetPointer(HWND_DESKTOP, hptrWait);
         DosCreateThread(&tidThread, &Othello_Thread, 1UL, 0UL, 12288L);
         fThreadRunning = TRUE;
         break;

        case MI_NEW:
          if (fThreadRunning)
            break;
          if(!fFirst && fSound)
          {
            PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT));
            if(pSE != NULL)
            {
              pSE->sEvent = 2;
              vdGetWavFile(pSE->sEvent, pSE->chWavFile);
              _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE);
            }
          }
          if(fSound)
            DosSleep(800);
          fFirst = FALSE;
          GameOver = FALSE;
          for(sX = 0; sX < 8; sX++)
            for(sY = 0; sY < 8; sY++)
              Board.sField[sX][sY] = EMPTY;

          Board.sField[3][3] = PLAYER;
          Board.sField[4][4] = PLAYER;
          Board.sField[3][4] = COMPUTER;
          Board.sField[4][3] = COMPUTER;

          WinInvalidateRect(hwnd, NULL, FALSE);
          if (cxBlock > 0 && cyBlock > 0)
          {
            WinQueryPointerPos(HWND_DESKTOP, &point);
            WinMapWindowPoints(HWND_DESKTOP, hwnd, &point, 1L);
            point.x /= cxBlock;
            point.y /= cyBlock;
            SetPointer(point.x, point.y);
          }

          if(!fPlayerStarts)
          {
            /* computer starts */
            /* der Computer macht einen Zug */
            WinSetPointer(HWND_DESKTOP, hptrWait);
            DosCreateThread(&tidThread, &Othello_Thread, 0UL, 0UL, 12288L);
            fThreadRunning = TRUE;
          }
          break;

        default:
          return WinDefWindowProc(hwnd, msg, mp1, mp2);
      }
      break;

    case WM_INITMENU:
      WinEnableMenuItem (HWNDFROMMP(mp2), MI_BEGINNER, TRUE);
      WinEnableMenuItem (HWNDFROMMP(mp2), MI_ADVANCED, TRUE);
      WinEnableMenuItem (HWNDFROMMP(mp2), MI_MASTER, TRUE);
      WinEnableMenuItem (HWNDFROMMP(mp2), MI_NEW, TRUE);
      WinEnableMenuItem (HWNDFROMMP(mp2), MI_HELPINDEX, fHelpEnabled);
      WinEnableMenuItem (HWNDFROMMP(mp2), MI_HELPEXTENDED, fHelpEnabled);
      WinEnableMenuItem (HWNDFROMMP(mp2), MI_HELPKEYS, fHelpEnabled);
      WinEnableMenuItem (HWNDFROMMP(mp2), MI_HELPHELPFORHELP, fHelpEnabled);
      WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR0 , bgcolor == CLR_BLACK    );
      WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR1 , bgcolor == CLR_BLUE     );
      WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR2 , bgcolor == CLR_RED      );
      WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR3 , bgcolor == CLR_PINK     );
      WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR4 , bgcolor == CLR_GREEN    );
      WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR5 , bgcolor == CLR_CYAN     );
      WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR6 , bgcolor == CLR_YELLOW   );
      WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR7 , bgcolor == CLR_WHITE    );
      WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR8 , bgcolor == CLR_DARKGRAY );
      WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR9 , bgcolor == CLR_DARKBLUE );
      WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR10, bgcolor == CLR_DARKRED  );
      WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR11, bgcolor == CLR_DARKPINK );
      WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR12, bgcolor == CLR_DARKGREEN);
      WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR13, bgcolor == CLR_DARKCYAN );
      WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR14, bgcolor == CLR_BROWN    );
      WinCheckMenuItem(HWNDFROMMP(mp2), MI_COLOR15, bgcolor == CLR_PALEGRAY );
      WinEnableMenuItem (HWNDFROMMP(mp2), MI_HINT, !GameOver);
      WinCheckMenuItem (HWNDFROMMP(mp2), MI_BEGINNER, FALSE);
      WinCheckMenuItem (HWNDFROMMP(mp2), MI_ADVANCED, FALSE);
      WinCheckMenuItem (HWNDFROMMP(mp2), MI_MASTER, FALSE);
      switch(sLevel)
      {
        case 0:
          WinCheckMenuItem (HWNDFROMMP(mp2), MI_BEGINNER, TRUE);
          break;
        case 1:
          WinCheckMenuItem (HWNDFROMMP(mp2), MI_ADVANCED, TRUE);
          break;
        default:
          WinCheckMenuItem (HWNDFROMMP(mp2), MI_MASTER, TRUE);
          break;
      }
      /* player or computer starts */
      WinEnableMenuItem (HWNDFROMMP(mp2), MI_PLAYER, !fPlayerStarts && !fThreadRunning);
      WinEnableMenuItem (HWNDFROMMP(mp2), MI_COMPUTER, fPlayerStarts && !fThreadRunning);
      if (fThreadRunning)
      {
        WinEnableMenuItem (HWNDFROMMP(mp2), MI_BEGINNER, FALSE);
        WinEnableMenuItem (HWNDFROMMP(mp2), MI_ADVANCED, FALSE);
        WinEnableMenuItem (HWNDFROMMP(mp2), MI_MASTER, FALSE);
        WinEnableMenuItem (HWNDFROMMP(mp2), MI_NEW, FALSE);
        WinEnableMenuItem (HWNDFROMMP(mp2), MI_HINT, FALSE);
      }
      return (MRESULT)(0);

    case WM_ERASEBACKGROUND:
      return (MRESULT)(FALSE);

    case WM_DESTROY:
      WinStoreWindowPos(szApp, "WinParams", hwndFrame);
      PrfWriteProfileData(hini, szApp, "BGColor", &bgcolor, sizeof(bgcolor));
      PrfWriteProfileData(hini, szApp, "Volume", &sVolumeEve, sizeof(sVolumeEve));
      PrfWriteProfileData(hini, szApp, "PlayerStarts", &fPlayerStarts, sizeof(fPlayerStarts));
      PrfWriteProfileData(hini, szApp, "Sound", &fSound, sizeof(fSound));
      PrfWriteProfileData(hini, szApp, "Level", &sLevel, sizeof(sLevel));
      /* destroy pointers */
      WinDestroyPointer(hptrCross);
      if(fSound)
      {
        PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT));
        if(pSE != NULL)
        {
          int __tid;
          pSE->sEvent = 1;
          vdGetWavFile(pSE->sEvent, pSE->chWavFile);
          __tid = _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE);
          if(__tid != -1)
            DosSleep(2500);
        }
      }

      if(hini != NULLHANDLE)
        PrfCloseProfile(hini);
      break;

    case WM_PAINT:
      {
        HPS hps; 
        RECTL rclUpdate, dest, src;     /* rectangles used for drawing */
        POINTL pt[4];
        AREABUNDLE area;
        SHORT sOffset;
        LONG xAxis, yAxis;
        POINTL ptl;

        hps = WinBeginPaint(hwnd, 0L, &rclUpdate);

        /* Hintergrund wird aktualisiert */
        src.xLeft = 0;
        src.yBottom = 0;
        src.xRight = cxBlock;
        src.yTop = (DIVISIONS + 3) * cyBlock;
        if (WinIntersectRect(hab, &dest, &src, &rclUpdate))
          WinFillRect (hps, &src, bgcolor);

        src.xLeft = cxBlock;
        src.yBottom = (DIVISIONS + 1) * cyBlock + 1;
        src.xRight = (DIVISIONS + 3) * cxBlock;
        src.yTop = (DIVISIONS + 3) * cyBlock;
        if (WinIntersectRect(hab, &dest, &src, &rclUpdate))
          WinFillRect (hps, &src, bgcolor);

        src.xLeft = (DIVISIONS + 1) * cxBlock + 1;
        src.yBottom = 0;
        src.xRight = (DIVISIONS + 3) * cxBlock;
        src.yTop = (DIVISIONS + 2) * cyBlock;
        if (WinIntersectRect(hab, &dest, &src, &rclUpdate))
          WinFillRect (hps, &src, bgcolor);

        src.xLeft = cxBlock;
        src.yBottom = 0;
        src.xRight = (DIVISIONS + 1) * cxBlock + 1;
        src.yTop = cyBlock + 1;
        if (WinIntersectRect(hab, &dest, &src, &rclUpdate))
          WinFillRect (hps, &src, bgcolor);

        /* Spielbrett wird gezeichnet */
        GpiSetBackColor(hps, CLR_PALEGRAY);
        for (sX = 1; sX <= DIVISIONS; sX++) 
          for (sY = 1; sY <= DIVISIONS; sY++)
          {
            src.xLeft = sX * cxBlock;
            src.yBottom = sY * cyBlock;
            src.xRight = (sX + 1) * cxBlock + 1;
            src.yTop = (sY + 1) * cyBlock + 1;
            if (WinIntersectRect(hab, &dest, &src, &rclUpdate))
              WinDrawBorder (hps, &src, 1L, 1L, 0L, 0L, DB_INTERIOR | DB_AREAATTRS);
          }

        /* Rahmen wird gezeichnet */
        area.lColor = CLR_DARKGRAY;
        GpiSetAttrs(hps, PRIM_AREA, ABB_COLOR, 0UL, &area);

        src.xLeft = pt[0].x = cxBlock;
        src.yTop = pt[0].y = cyBlock;
        pt[1].x = cxBlock + cxBlock / 4;
        pt[1].y = cyBlock - cyBlock / 4;
        src.xRight = pt[2].x = (DIVISIONS + 1) * cxBlock + cxBlock / 4;
        src.yBottom = pt[2].y = cyBlock - cyBlock / 4;
        pt[3].x = (DIVISIONS + 1) * cxBlock;
        pt[3].y = cyBlock;
        if (WinIntersectRect(hab, &dest, &src, &rclUpdate)) 
        {
          GpiBeginArea(hps, BA_BOUNDARY);
          GpiMove (hps, &pt[0]);
          GpiPolyLine (hps, 4L, pt);
          GpiEndArea (hps);
        }
        pt[0].x = (DIVISIONS + 1) * cxBlock;
        src.yTop = pt[0].y = (DIVISIONS + 1) * cyBlock;
        pt[1].x = (DIVISIONS + 1) * cxBlock + cxBlock / 4;
        pt[1].y = (DIVISIONS + 1) * cyBlock - cyBlock / 4;
        pt[2].x = (DIVISIONS + 1) * cxBlock + cxBlock / 4;
        pt[2].y = cyBlock - cyBlock / 4;
        src.xLeft = pt[3].x = (DIVISIONS + 1) * cxBlock;
        pt[3].y = cyBlock;
        if (WinIntersectRect(hab, &dest, &src, &rclUpdate)) 
        {
          GpiBeginArea(hps, BA_BOUNDARY);
          GpiMove (hps, &pt[0]);
          GpiPolyLine (hps, 4L, pt);
          GpiEndArea (hps);
        }

        /* jetzt wird der Schatten gezeichnet */
        area.lColor = CLR_BLACK;
        GpiSetAttrs(hps, PRIM_AREA, ABB_COLOR, 0UL, &area);
        src.xLeft = cxBlock + cxBlock / 2;
        src.yBottom = cyBlock / 2;
        src.xRight = (DIVISIONS + 1) * cxBlock + cxBlock / 2 + 1;
        src.yTop = 3 * cyBlock / 4 + 1;
        if (WinIntersectRect(hab, &dest, &src, &rclUpdate)) 
        {
          GpiSetBackColor(hps, CLR_BLACK);
          WinDrawBorder (hps, &src, 1L, 1L, 0L, 0L, DB_INTERIOR | DB_AREAATTRS);
        }
        src.xLeft = (DIVISIONS + 1) * cxBlock + cxBlock / 4;
        src.yBottom = cyBlock / 2;
        src.xRight = (DIVISIONS + 1) * cxBlock + cxBlock / 2 + 1;
        src.yTop = (DIVISIONS + 1) * cyBlock - cyBlock / 2 + 1;
        if (WinIntersectRect(hab, &dest, &src, &rclUpdate)) 
        {
          GpiSetBackColor(hps, CLR_BLACK);
          WinDrawBorder (hps, &src, 1L, 1L, 0L, 0L, DB_INTERIOR | DB_AREAATTRS);
        }

        /* jetzt werden die Spielsteine gezeichnet, falls noetig */
        sOffset = min(cxBlock, cyBlock) / 10;
        for (sX = 1; sX <= DIVISIONS; sX++)
          for (sY = 1; sY <= DIVISIONS; sY++)
            if (Board.sField[sX - 1][sY - 1] != EMPTY)
            {
              src.xLeft = sX * cxBlock;
              src.yTop = (sY + 1) * cyBlock;
              src.xRight = (sX + 1) * cxBlock;
              src.yBottom = sY * cyBlock;
              if (WinIntersectRect(hab, &dest, &src, &rclUpdate))
              {
                switch (Board.sField[sX - 1][sY - 1])
                {
                  case COMPUTER:
                    if (lColors == 2)
                      area.lColor = CLR_WHITE;
                    else
                      area.lColor = CLR_RED;
                    break;

                  case PLAYER:
                    if (lColors == 2)
                      area.lColor = CLR_BLACK;
                    else
                      area.lColor = CLR_BLUE;
                    break;
                }
                xAxis = src.xRight - src.xLeft;
                yAxis = src.yTop - src.yBottom;
                if (xAxis < yAxis) 
                {
                  src.yTop = src.yTop - yAxis / 2 + xAxis / 2;
                  src.yBottom = src.yBottom + yAxis / 2 - xAxis / 2;
                }
                else 
                {
                  src.xRight = src.xRight - xAxis / 2 + yAxis / 2;
                  src.xLeft = src.xLeft + xAxis / 2 - yAxis / 2;
                }
                src.xLeft += sOffset;
                src.yTop -= sOffset;
                src.xRight -= sOffset;
                src.yBottom += sOffset;
                ptl.x = src.xLeft;
                ptl.y = src.yBottom;
                GpiMove(hps, &ptl);
                ptl.x = src.xRight;
                ptl.y = src.yTop;
                GpiSetAttrs(hps, PRIM_AREA, ABB_COLOR, 0UL, &area);
                GpiBox(hps, DRO_OUTLINEFILL, &ptl, xAxis, yAxis);
              }
            }

        /* zeichnen des Ausgabetextes */
        if (GameOver || fDisplayText)
        {
          WinQueryWindowRect (hwnd, &src);
          src.yBottom= src.yTop - cyBlock;
          GpiSetColor(hps, (bgcolor != CLR_WHITE) ? CLR_WHITE : CLR_BLACK);
          WinDrawText (hps, -1L, text, &src, 0L, 0L, DT_TEXTATTRS | DT_CENTER | DT_VCENTER);
        }
        WinEndPaint(hps);
      }
      break;

    case WM_CLOSE:
      WinPostMsg( hwnd, WM_QUIT, NULL, NULL);
      break;

    default:
      return WinDefWindowProc( hwnd, msg, mp1, mp2 );
  }
  return (MRESULT) FALSE;
}

void SetPointer(SHORT sX, SHORT sY)
{
  if (GameOver || sX == 0 || sY == 0 || sX == 9 || sY == 9)
  {
    WinSetPointer(HWND_DESKTOP, hptrSystem);
    return;
  }
  if(fIsMovePossible(Board, sX - 1, sY - 1, PLAYER))
    WinSetPointer(HWND_DESKTOP, hptrCross);
  else
    WinSetPointer(HWND_DESKTOP, hptrSystem);
}

VOID InitHelp(VOID)
{
  HELPINIT heini;

  /* if we return because of an error, Help will be disabled */
  fHelpEnabled = FALSE;

  /* initialize help init structure */
  heini.cb = sizeof(HELPINIT);
  heini.ulReturnCode = 0;
  heini.pszTutorialName = (PSZ)NULL;
  heini.phtHelpTable = (PHELPTABLE)MAKELONG(MAIN_HELP_TABLE, 0xFFFF);
  heini.hmodHelpTableModule = 0;
  heini.hmodAccelActionBarModule = 0;
  heini.idAccelTable = 0;
  heini.idActionBar = 0;
#ifdef GERMAN
  heini.pszHelpWindowTitle = "Othello Hilfe";
#else
  heini.pszHelpWindowTitle = "Othello Help Window";
#endif
  heini.fShowPanelId = CMIC_HIDE_PANEL_ID;
  heini.pszHelpLibraryName = "OTHELLO.HLP";

  /* creating help instance */
  hwndHelpInstance = WinCreateHelpInstance(hab, &heini);

  if(hwndHelpInstance == 0L || heini.ulReturnCode)
  {
#ifdef GERMAN
    WinMessageBox(HWND_DESKTOP, hwndFrame, (PCH) "Hilfe ist nicht verfÅgbar.", (PCH) szApp,
                  MB_MISC, MB_OK | MB_APPLMODAL | MB_MOVEABLE | MB_ERROR);
#else
    WinMessageBox(HWND_DESKTOP, hwndFrame, (PCH) "Help is not available.", (PCH) szApp,
                  MB_MISC, MB_OK | MB_APPLMODAL | MB_MOVEABLE | MB_ERROR);
#endif
    return;
  }

  /* associate help instance with main frame window */
  if(!WinAssociateHelpInstance(hwndHelpInstance, hwndFrame))
  {
#ifdef GERMAN
    WinMessageBox(HWND_DESKTOP, hwndFrame, (PCH) "Hilfe ist nicht verfÅgbar.", (PCH) szApp,
                  MB_MISC, MB_OK | MB_APPLMODAL | MB_MOVEABLE | MB_ERROR);
#else
    WinMessageBox(HWND_DESKTOP, hwndFrame, (PCH) "Help is not available.", (PCH) szApp,
                  MB_MISC, MB_OK | MB_APPLMODAL | MB_MOVEABLE | MB_ERROR);
#endif
    return;
  }
  /* Help manager is successfully initialized so set flag to TRUE */
  fHelpEnabled = TRUE;
  return;
}

VOID HelpHelpForHelp(MPARAM mp2)
{
  /* this just displays the system help for help panel */
  if(fHelpEnabled)
    if((LONG)WinSendMsg(hwndHelpInstance, HM_DISPLAY_HELP, (MPARAM)0L, (MPARAM)0L))
#ifdef GERMAN
      WinMessageBox(HWND_DESKTOP, hwndFrame, (PCH) "Hilfe kann nicht angezeigt werden.", (PCH) szApp,
                    MB_MISC, MB_OK | MB_APPLMODAL | MB_MOVEABLE | MB_ERROR);
#else
      WinMessageBox(HWND_DESKTOP, hwndFrame, (PCH) "Unable to display help information.", (PCH) szApp,
                    MB_MISC, MB_OK | MB_APPLMODAL | MB_MOVEABLE | MB_ERROR);
#endif
   return;
}

VOID HelpExtended(MPARAM mp2)
{
  /* this just displays the system extended help panel */
  if(fHelpEnabled)
    if((LONG)WinSendMsg(hwndHelpInstance, HM_EXT_HELP, (MPARAM)0L,(MPARAM)0L))
#ifdef GERMAN
      WinMessageBox(HWND_DESKTOP, hwndFrame, (PCH) "Hilfe kann nicht angezeigt werden.", (PCH) szApp,
                    MB_MISC, MB_OK | MB_APPLMODAL | MB_MOVEABLE | MB_ERROR);
#else
      WinMessageBox(HWND_DESKTOP, hwndFrame, (PCH) "Unable to display help information.", (PCH) szApp,
                    MB_MISC, MB_OK | MB_APPLMODAL | MB_MOVEABLE | MB_ERROR);
#endif
   return;
}

VOID HelpKeys(MPARAM mp2)
{
  /* this just displays the system keys help panel */
  if(fHelpEnabled)
    if((LONG)WinSendMsg(hwndHelpInstance, HM_KEYS_HELP, (MPARAM)0L,(MPARAM)0L))
#ifdef GERMAN
      WinMessageBox(HWND_DESKTOP, hwndFrame, (PCH) "Hilfe kann nicht angezeigt werden.", (PCH) szApp,
                    MB_MISC, MB_OK | MB_APPLMODAL | MB_MOVEABLE | MB_ERROR);
#else
      WinMessageBox(HWND_DESKTOP, hwndFrame, (PCH) "Unable to display help information.", (PCH) szApp,
                    MB_MISC, MB_OK | MB_APPLMODAL | MB_MOVEABLE | MB_ERROR);
#endif
    return;
}

VOID HelpIndex(MPARAM mp2)
{
  /* this just displays the system help index panel */
  if(fHelpEnabled)
    if((LONG)WinSendMsg(hwndHelpInstance, HM_HELP_INDEX, (MPARAM)0L,(MPARAM)0L))
#ifdef GERMAN
      WinMessageBox(HWND_DESKTOP, hwndFrame, (PCH) "Hilfe kann nicht angezeigt werden.", (PCH) szApp,
                    MB_MISC, MB_OK | MB_APPLMODAL | MB_MOVEABLE | MB_ERROR);
#else
      WinMessageBox(HWND_DESKTOP, hwndFrame, (PCH) "Unable to display help information.", (PCH) szApp,
                    MB_MISC, MB_OK | MB_APPLMODAL | MB_MOVEABLE | MB_ERROR);
#endif
    return;
}

VOID DestroyHelpInstance(VOID)
{
  if(hwndHelpInstance != 0L)
  {
    WinDestroyHelpInstance(hwndHelpInstance);
  }
  return;
}

MRESULT EXPENTRY dpProdInfo(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  LONG logoDisplayTime;
  static LONG ulTimerId;
  struct _ctldata *c;

  switch(msg)
  {
    case WM_INITDLG:
      FixSysMenu(hwnd);
      c = (struct _ctldata *)mp2;
      WinShowWindow(WinWindowFromID(hwnd, PB_HELP), fHelpEnabled);
      if (c->fParam)
      {
        logoDisplayTime = PrfQueryProfileInt(HINI_USERPROFILE, "PM_ControlPanel", "LogoDisplayTime", -1L);
        switch(logoDisplayTime)
        {
          case 0:
            /* no logo is displayed */
            WinDismissDlg(hwnd, TRUE);
            break;
          case -1:
            /* indefinite logo display */
            break;
          default:
            ulTimerId = WinStartTimer(hab, hwnd, TI_LOGO, logoDisplayTime);
            break;
        }
      }
      break;

      case WM_TIMER:
        if (SHORT1FROMMP(mp1) == TI_LOGO)
          WinPostMsg(hwnd, WM_COMMAND, NULL, NULL);
        break;

      case WM_COMMAND:
        /* no matter what the command, close the dialog */
        WinStopTimer(hab, hwnd, ulTimerId);
        WinDismissDlg(hwnd, TRUE);
        break;

      default:
        return(WinDefDlgProc(hwnd, msg, mp1, mp2));
  }
  return (MRESULT)0;
}

VOID DisplayMessage(PCH pchStr)
{
  WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, (PCH) pchStr, (PCH) szApp, MB_MISC,
                MB_OK | MB_SYSTEMMODAL | MB_MOVEABLE | MB_ERROR);
}

VOID FixSysMenu(HWND hwndDlg)
{
  HWND hwndMenu; /* handle to system menu */

  hwndMenu = WinWindowFromID(hwndDlg, FID_SYSMENU);
  WinSendMsg(hwndMenu, MM_DELETEITEM, MPFROM2SHORT(SC_RESTORE, TRUE), MPFROMP(NULL));
  WinSendMsg(hwndMenu, MM_DELETEITEM, MPFROM2SHORT(SC_SIZE, TRUE), MPFROMP(NULL));
  WinSendMsg(hwndMenu, MM_DELETEITEM, MPFROM2SHORT(SC_MINIMIZE, TRUE), MPFROMP(NULL));
  WinSendMsg(hwndMenu, MM_DELETEITEM, MPFROM2SHORT(SC_MAXIMIZE, TRUE), MPFROMP(NULL));
  WinSendMsg(hwndMenu, MM_DELETEITEM, MPFROM2SHORT(SC_HIDE, TRUE), MPFROMP(NULL));
  return;
}

static void Othello_Thread(ULONG ulParam)
{
  SHORT sX, sY;
  BOOL fValid;

  if (ulParam == 1)
    sOthello(BoardHilf, 0, 0, 0, sLevel + 2, COMPUTER, &sX, &sY, &fValid, 0);
  else
    sOthello(Board, 0, 0, 0, sLevel + 2, COMPUTER, &sX, &sY, &fValid, 0);
  while (!WinPostQueueMsg(hmq, WMP_COMP_DONE, MPFROM2SHORT(sX, sY), MPFROMLONG(ulParam)))
    DosSleep(10UL);
  _endthread();
} 

VOID vdGetWavFile(USHORT usEvent, PSZ pszWavFile)
{
  CHAR szWavFile[CCHMAXPATHCOMP];
  ULONG ulBufSize;

  ulBufSize = sizeof(szWavFile);
  if (!PrfQueryProfileData(hini, szApp, pszEvent[usEvent], &szWavFile, &ulBufSize))
  {
    if(fMMPMPresent)
      strcpy(pszWavFile, pszDefWAVFiles[usEvent]);
    else
#ifdef GERMAN
      strcpy(pszWavFile, "<Biep>");
#else
      strcpy(pszWavFile, "<Beep>");
#endif
  }
  else
    strcpy(pszWavFile, szWavFile);
}

VOID vdSetWavFile(USHORT usEvent, PSZ pszWavFile)
{
  CHAR szWavFile[CCHMAXPATHCOMP];

  strcpy(szWavFile, pszWavFile);
  PrfWriteProfileData(hini, szApp, pszEvent[usEvent], &szWavFile, sizeof(szWavFile));
}

MRESULT EXPENTRY dpEve(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  MRESULT mr;
  SHORT j, i;
  HDIR hDir;
  APIRET rc;
  static ULONG ulWavCount;
  static FILEFINDBUF3 findWavBuffer[MAX_NUMBER_OF_WAV_FILES];
  static CHAR szTemp[CCHMAXPATH];
  static SHORT sVolumeEveTemp;

  switch(msg)
  {
    case WM_INITDLG:
      FixSysMenu(hwnd);
      WinShowWindow(WinWindowFromID(hwnd, PB_HELP), fHelpEnabled);
      sVolumeEveTemp = sVolumeEve;

      /* load profile */
      WinSendDlgItemMsg(hwnd, SL_EVESOUNDS, SLM_SETSLIDERINFO, MPFROM2SHORT(SMA_SLIDERARMPOSITION, SMA_INCREMENTVALUE), MPFROMSHORT(sVolumeEve));
      WinSendDlgItemMsg(hwnd, CB_EVESOUNDS, BM_SETCHECK, MPFROMSHORT(fSound), NULL);
      WinEnableWindow(WinWindowFromID(hwnd, LB_EVESOUNDS), fSound);
      WinEnableWindow(WinWindowFromID(hwnd, SL_EVESOUNDS), fSound);
      if(!fMMPMPresent)
      {
        ulWavCount = 0UL;
        goto _liste;
      }
      else
      {
        hDir = HDIR_SYSTEM;
        ulWavCount = 1UL;

        rc = DosFindFirst("*.WAV", &hDir, 0UL, (PVOID)&findWavBuffer[0], sizeof(findWavBuffer[0]), &ulWavCount, FIL_STANDARD);
        if (0 != rc && 2 != rc && 18 != rc)
        {
          ulWavCount = 0;
          goto _liste;
        }

        if(rc == 2 || rc == 18)
          ulWavCount = 0;
        else
        {
          do
          {
            ULONG ulNextCount;

            ulNextCount = 1UL;
            rc = DosFindNext(hDir, (PVOID)&findWavBuffer[ulWavCount], sizeof(findWavBuffer[ulWavCount]), &ulNextCount);
            if (0 != rc && rc != 18)
              goto _liste;
            if (rc != 18)
              ulWavCount++;
          } while (rc != 18 && ulWavCount < MAX_NUMBER_OF_WAV_FILES);
        }
      }
_liste:
#ifdef GERMAN
      WinInsertLboxItem(WinWindowFromID(hwnd, LB_EVESOUNDS), LIT_END, "<Kein Ton>");
      WinInsertLboxItem(WinWindowFromID(hwnd, LB_EVESOUNDS), LIT_END, "<Biep>");
#else
      WinInsertLboxItem(WinWindowFromID(hwnd, LB_EVESOUNDS), LIT_END, "<No sound>");
      WinInsertLboxItem(WinWindowFromID(hwnd, LB_EVESOUNDS), LIT_END, "<Beep>");
#endif
      for(i = 0; i < ulWavCount; i++)
        WinInsertLboxItem(WinWindowFromID(hwnd, LB_EVESOUNDS), LIT_END, findWavBuffer[i].achName);

      for(i = 0; i < NUMBER_OF_EVENTS; i++)
        WinInsertLboxItem(WinWindowFromID(hwnd, LB_EVE), LIT_END, pszEvent[i]);

      /* select first event */
      WinSendMsg(WinWindowFromID(hwnd, LB_EVE), LM_SELECTITEM, MPFROMSHORT(0), MPFROMLONG(TRUE));
      break;

   case WM_CONTROL:
   {
      BOOL fSoundsCheck = (BOOL)WinSendDlgItemMsg(hwnd, CB_EVESOUNDS, BM_QUERYCHECK, NULL, NULL) && fMMPMPresent;
      mr = WinSendMsg(WinWindowFromID(hwnd, LB_EVE), LM_QUERYSELECTION, MPFROMSHORT(0), MPFROMLONG(0));
      i = SHORT1FROMMP(mr);
      mr = WinSendMsg(WinWindowFromID(hwnd, LB_EVESOUNDS), LM_QUERYSELECTION, MPFROMSHORT(0), MPFROMLONG(0));
      j = SHORT1FROMMP(mr);

      switch (SHORT1FROMMP(mp1)) 
      {
        case LB_EVESOUNDS:
          WinEnableWindow(WinWindowFromID(hwnd, PB_PLAYEVE), (j > 1) && fSoundsCheck);
          WinEnableWindow(WinWindowFromID(hwnd, SL_EVESOUNDS), (j > 1) && fSoundsCheck);
          if(SHORT2FROMMP(mp1) == LN_ENTER)
          {
            WinSendDlgItemMsg(hwnd, LB_EVESOUNDS, LM_QUERYITEMTEXT, MPFROM2SHORT(j, sizeof(szTemp)), MPFROMP(szTemp));
            vdSetWavFile(i, szTemp);
          }
          break;

        case LB_EVE:
          if(SHORT2FROMMP(mp1) == LN_SELECT)
          {
            {
              BOOL fFound = FALSE;
              CHAR szWavFile[CCHMAXPATHCOMP];

              vdGetWavFile(i, (PSZ)&szWavFile);
              /* falls da jetzt der Null String zurueckkommt ist kein WAV file selected */

              if (strcmp(szWavFile, "") == 0)
                WinSendMsg(WinWindowFromID(hwnd, LB_EVESOUNDS), LM_SELECTITEM, MPFROMSHORT(0), MPFROMLONG(TRUE));
              else
              {
#ifdef GERMAN
                if (strcmp(szWavFile, "<Biep>") == 0)
#else
                if (strcmp(szWavFile, "<Beep>") == 0)
#endif
                  WinSendMsg(WinWindowFromID(hwnd, LB_EVESOUNDS), LM_SELECTITEM, MPFROMSHORT(1), MPFROMLONG(TRUE));
                else
                {
                  /* jetzt muessen wir die ganzen WAV files durchsuchen */
                  for(j = 0; j < ulWavCount; j++)
                  {
                    if (strcmpi(szWavFile, findWavBuffer[j].achName) == 0)
                    {
                      fFound = TRUE;
                      WinSendMsg(WinWindowFromID(hwnd, LB_EVESOUNDS), LM_SELECTITEM, MPFROMSHORT(j + 2), MPFROMLONG(TRUE));
                      break;
                    }
                  }
                  if (!fFound)
                    WinSendMsg(WinWindowFromID(hwnd, LB_EVESOUNDS), LM_SELECTITEM, MPFROMSHORT(0), MPFROMLONG(TRUE));
                }
              }
            }
          }
          break;

        case CB_EVESOUNDS:
          {
            WinEnableWindow(WinWindowFromID(hwnd, LB_EVE), fSoundsCheck);
            WinEnableWindow(WinWindowFromID(hwnd, LB_EVESOUNDS), fSoundsCheck);
            WinEnableWindow(WinWindowFromID(hwnd, SL_EVESOUNDS), fSoundsCheck);
            WinEnableWindow(WinWindowFromID(hwnd, PB_PLAYEVE), (j > 1) && fSoundsCheck);
          }
          break;

        case SL_EVESOUNDS:
          sVolumeEve = (SHORT)WinSendDlgItemMsg(hwnd, SL_EVESOUNDS, SLM_QUERYSLIDERINFO, MPFROM2SHORT(SMA_SLIDERARMPOSITION, SMA_INCREMENTVALUE), NULL);
          break;
      }
    }
    break;

   case WM_COMMAND:                    /* posted by push button or key */
   {
      mr = WinSendMsg(WinWindowFromID(hwnd, LB_EVE), LM_QUERYSELECTION, MPFROMSHORT(0), MPFROMLONG(0));
      i = SHORT1FROMMP(mr);
      mr = WinSendMsg(WinWindowFromID(hwnd, LB_EVESOUNDS), LM_QUERYSELECTION, MPFROMSHORT(0), MPFROMLONG(0));
      j = SHORT1FROMMP(mr);
      switch(SHORT1FROMMP(mp1))       /* extract the command value */
      {

        case DID_OK:
          fSound = (BOOL)WinSendDlgItemMsg(hwnd, CB_EVESOUNDS, BM_QUERYCHECK, NULL, NULL);
          WinSendDlgItemMsg(hwnd, LB_EVESOUNDS, LM_QUERYITEMTEXT, MPFROM2SHORT(j, sizeof(szTemp)), MPFROMP(szTemp));
          vdSetWavFile(i, szTemp);
          WinDismissDlg(hwnd, TRUE);
          break;

        case DID_CANCEL:
          /* close dialog */
          sVolumeEve = sVolumeEveTemp;
          WinDismissDlg(hwnd, FALSE);
          break;

        case PB_PLAYEVE:
          if(j > 1)
          {
            PSOUNDEVENT pSE = malloc(sizeof(SOUNDEVENT));
            if(pSE != NULL)
            {
              pSE->sEvent = -1;
              WinSendDlgItemMsg(hwnd, LB_EVESOUNDS, LM_QUERYITEMTEXT, MPFROM2SHORT(j, sizeof(szTemp)), MPFROMP(szTemp));
              strcpy(pSE->chWavFile, szTemp);
              _beginthread(vdPlayWavFileAsyncEve, NULL, 65536L, pSE);
            } 
          }
          break;

        default:
          break;
      }
    }
    break;
 
    default:
      return(WinDefDlgProc(hwnd, msg, mp1, mp2));
  }
  return (MRESULT)0;
}

ULONG GPFHandler(PEXCEPTIONREPORTRECORD pxcptrec, PEXCEPTIONREGISTRATIONRECORD prr, PCONTEXTRECORD pcr, PVOID pv)
{
  if(pxcptrec->ExceptionNum == XCPT_ACCESS_VIOLATION)
  {
    fMMPMPresent = FALSE;
    _endthread();
  }
  return(XCPT_CONTINUE_SEARCH);
}

VOID vdPlayWavFileAsyncEve(void *arg)
{
  SHORT i;
  CHAR LoadError[100];
  CHAR szTemp[CCHMAXPATH];
  APIRET rc;
  PSOUNDEVENT pSE = arg;
  ULONG ulTemp;
  EXCEPTIONREGISTRATIONRECORD xcpthand = {(PEXCEPTIONREGISTRATIONRECORD)0, (_ERR * volatile)GPFHandler};

  rc = DosRequestMutexSem(hmtxSound, 100);
  if (NO_ERROR != rc)
  {
    free(pSE);
    return;
  }

  DosSetExceptionHandler(&xcpthand);

#ifdef GERMAN
  if(strcmp(pSE->chWavFile, "<Kein Ton>") == 0)
#else
  if(strcmp(pSE->chWavFile, "<No sound>") == 0)
#endif
    goto _release;
 
#ifdef GERMAN
  if(strcmp(pSE->chWavFile, "<Biep>") == 0)
#else
  if(strcmp(pSE->chWavFile, "<Beep>") == 0)
#endif
  {
    switch (pSE->sEvent)
    {
      case 0:
      case 7:
      case 2:
        DosBeep(261, 100);
        DosBeep(330, 100);
        DosBeep(392, 100);
        DosBeep(523, 500);
        break;

      case 3:
        DosBeep(261, 50);
        break;

      case 4:
        DosBeep(523, 50);
        break;

      case 5:
        DosBeep(261, 100);
        DosBeep(392, 150);
        break;

      case 6:
        DosBeep(523, 100);
        DosBeep(392, 150);
        break;

      case 1:
      case 8:
      case 9:
        DosBeep(523, 100);
        DosBeep(392, 100);
        DosBeep(330, 100);
        DosBeep(261, 500);
        break;
    }
    goto _release;
  }

  if(!fMMPMPresent)
    goto _release;

  /* load sound library */
  if(hmodSound == NULLHANDLE)
  {
    rc = DosLoadModule(LoadError, sizeof(LoadError), "EPSOUND", &hmodSound);
    if (rc != 0)
      goto _release;

    /* sound library successfully loaded */
    /* load function pointers */
    if (0 != DosQueryProcAddr(hmodSound, 1L, NULL, &pfnLoadWaveFile))
      goto _release;

    if (0 != DosQueryProcAddr(hmodSound, 2L, NULL, &pfnPlayWave))
      goto _release;
  }

  if(pSE->sEvent == -1 || pSE->sEvent == 1)
  {
    ulTemp = pfnLoadWaveFile(pSE->chWavFile, sVolumeEve);
    if(ulTemp != 0)
      pfnPlayWave(ulTemp, hwndFrame, TRUE);
    goto _release;
  }
  else
  {
    if(0 == ulPreviousWave[pSE->sEvent])
    {
      /* ist die Datei schon geladen */
      for (i = 0; i < NUMBER_OF_EVENTS; i++)
      {
        if((ulPreviousWave[i] == 0) || (i == pSE->sEvent))
          continue;

        vdGetWavFile(i, szTemp);
        if (0 == strcmpi(szTemp, pSE->chWavFile))
        {
          ulPreviousWave[pSE->sEvent] = ulPreviousWave[i];
          goto _weiter;
        }
      }
 
      /* load wave file */
      ulPreviousWave[pSE->sEvent] = pfnLoadWaveFile(pSE->chWavFile, sVolumeEve);
      if(0 == ulPreviousWave[pSE->sEvent])
        goto _release;
    }
  }

_weiter: /* wave file successfully loaded */
  pfnPlayWave(ulPreviousWave[pSE->sEvent], hwndFrame, FALSE);

_release:
  free(pSE);
  DosUnsetExceptionHandler(&xcpthand);
  DosReleaseMutexSem(hmtxSound);
 _endthread();
}
