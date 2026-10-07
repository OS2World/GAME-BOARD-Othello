Othello for OS/2
================

Version 1.1

Othello (also known as Reversi) is a classic two-player strategy board game.
This version runs natively on OS/2 Presentation Manager and includes an
artificial intelligence opponent at three skill levels.

The game was originally written by Peter Wansch in 1994 as part of an
entertainment pack for OS/2. This release is an Open Watcom port with
multilingual support, updated menus, and standard PM conventions.


Requirements
------------
- OS/2 Warp 4, eComStation, or ArcaOS
- Presentation Manager (PM) subsystem
- Optional: MMPM/2 for WAV sound support (via epsound.dll)


Installation
------------
Copy the following files to a directory of your choice:

  othello.exe   - main executable
  epsound.dll   - sound support library (optional)
  *.WAV         - sound files (optional)
  help\Othello_xx.hlp - online help, one file per language (en, es, nl, de, fr, it),
                   in the help folder next to the exe

Create a program object on the Desktop pointing to othello.exe.


How to Play
-----------
Othello is played on an 8x8 board. Players alternate placing pieces.
When you place a piece, all opponent pieces between your new piece and
another of your pieces (horizontally, vertically, or diagonally) are
flipped to your color. The player with the most pieces when the board
is full wins.

Controls:
  Left mouse button  - place a piece
  Ctrl+N             - new game
  Ctrl+H             - hint
  Ctrl+P             - pause
  Ctrl+Q             - quit current game
  Ctrl+X             - exit application
  Ctrl+B             - toggle background run
  Ctrl+F             - toggle frame controls


Options
-------
Level:
  Beginner   - easiest AI
  Advanced   - moderate AI
  Master     - strongest AI

Player/Computer starts - choose who makes the first move.

Background color - 16 color choices for the board background.

Sound - configure WAV files and volume for game events.

Language - choose from English, Espanol, Nederlands, Deutsch,
           Francais, or Italiano.

Save settings on exit - saves all settings to ENTRTAIN.INI.

Background Run - when disabled (default), the game pauses when the
  application loses focus.

Frame Controls - toggles the title bar, system menu, and menu bar.


Settings
--------
Settings are stored in ENTRTAIN.INI in the current directory.
They are saved automatically if "Save settings on exit" is checked,
or on demand via the menu.


License
-------
See LICENSE.txt for the full BSD 3-Clause license text.

Original author: Peter Wansch, 1994
OS2World port: 2026
