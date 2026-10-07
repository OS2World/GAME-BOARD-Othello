# Othello for OS/2

Classic Reversi board game for OS/2 Presentation Manager.

![Othello ScreenShot](/doc/Othello.png)

**Version:** 1.1
**Original author:** Peter Wansch (1994)
**OW port:** OS2World (2026)
**License:** BSD 3-Clause

## Description

Othello is a classic two-player strategy game played on an 8x8 board.
The goal is to have the most pieces of your color when the board is full.
This version features a three-level AI opponent, multilingual menus,
WAV sound support, and 16 selectable background colors.

## Requirements

- OS/2 Warp 4, eComStation, or ArcaOS
- Presentation Manager subsystem
- Optional: MMPM/2 for WAV sound (via `epsound.dll`)

## Building

Requires Open Watcom C/C++ (1.9 or later recommended).

```
compile-wat.cmd
```

Or directly with wmake:

```
wmake -f makefile.wat
```

Output is placed in `bin\othello.exe`.

## Project Layout

```
src\         - C source files and resources
bin\         - build output
doc\         - documentation
help\        - IPF help source, one file per language (compiled to bin\help\Othello_xx.hlp)
legacy\      - original 1994 ICC source files
```

## Features

- Three AI difficulty levels: Beginner, Advanced, Master
- Runtime language selection: English, Espanol, Nederlands,
  Deutsch, Francais, Italiano
- 16 board background colors
- WAV sound support via MMPM/2 and epsound.dll
- Frame Controls (Ctrl+F) to hide/show title bar and menus
- Background Run (Ctrl+B) option
- Settings persistence via ENTRTAIN.INI

## Controls

| Key    | Action                |
|--------|-----------------------|
| Ctrl+N | New game              |
| Ctrl+H | Hint                  |
| Ctrl+P | Pause                 |
| Ctrl+Q | Quit current game     |
| Ctrl+X | Exit                  |
| Ctrl+B | Toggle background run |
| Ctrl+F | Toggle frame controls |

## Changelog

See `doc\Changelog.txt` for version history.

## Links

- https://www.os2world.com/games/index.php/native-games/board/125-othello

## License

BSD 3-Clause. See `doc\LICENSE.txt`.
