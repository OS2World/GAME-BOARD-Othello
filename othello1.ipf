:userdoc.

:h1 res=2100 name=PANEL_MAIN.About Othello
:i1 id=aboutOthello.About Othello
:p.The game of Othello is played on an 8 by 8 board. There are 64 markers which have contrasting
colours on their opposite sides (red and blue or black and white for monomchrome displays). Initially
the first four markers are placed diagonally in the central four squares.
:p.The players place markers on the board in unoccupied squares. Each marker must be placed so that it is
adjacent to an enemy marker, either orthogonally or diagonally. In addition, the marker must be placed
in line with a friendly marker and separated from it by one or more enemy markers (and no empty spaces).
All enemy markers so flanked are reversed so that they become friendly markers. Markers remain fixed during
a game, but a marker may be reversed any number of times.
:p.If the placing of the marker simultaneously captures two or more chains of enemy markers, the markers in all captured chains
are reversed.
:p.Pieces may be captured only by the placing of a marker. Enemy markers flanked at both ends as the result of other causes
(such as the reversal of some marker) are not captured.
:p.If a player cannot make a legal move he loses his turn. He continues to lose
his turn until a legal move becomes possible for him.
:p.The game ends when all 64 squares are filled, or when neither player can move. The winner is the player
with the most markers on the board.

:p.Related information:
:p.:link reftype=hd res=2126.Playing the game:elink.
.br
:link reftype=hd res=2129.Keys help:elink.
.br
:link reftype=hd res=2127.Note:elink.
.br
:link reftype=hd res=2128.Copyright notice:elink.

:h1 res=2126 name=PANEL_HELPPLAYING.Playing the game
:i1 id=playingHelp.Playing the game
:p.Choose :hp2.New:ehp2. from the :hp2.Game:ehp2. menu to start a new game and select the level of play which is either
Beginner, Advanced or Master by selecting the corresponding item on the :hp2.Options:ehp2. menu. The current level
of play is indicated by a check mark left to the menu item.

:p.To place a marker, move the mouse pointer to an empty square. The mouse pointer changes its shape to
a cross to indicate that it would be possible for you to place a marker on the corresponding square.
Press the left mouse button to place a marker. You can also use the arrow keys to move around the board.
Press Enter, Return or Space to place a marker. If you or the computer has to pass, a message will
be displayed.

:p.Choose :hp2.Hint:ehp2. to get a hint on a good move. Choose :hp2.Computer starts:ehp2. if you want the computer to make
the first move.

.*--------------------------------------------------------------*\
.*  Note panel                                                  *
.*      res = PANEL_HELPNOTE                                    *
.*--------------------------------------------------------------*/
:h1 res=2127 name=PANEL_HELPNOTE.Note
:i1 id=noteHelp.Note
:p.If a display driver for a monochrome display is 
installed, the red side of the markers will be displayed white 
and the blue side of the markers will be displayed black. 
Othello can also be played on computers
that have no pointing device installed or attached by 
using the arrow keys instead of the mouse.   

.*--------------------------------------------------------------*\
.*  Note panel                                                  *
.*      res = PANEL_HELPCREATOR                                 *
.*--------------------------------------------------------------*/
:h1 res=2128 name=PANEL_HELPCREATOR.Copyright notice
:i1 id=noticeHelp.Copyright notice
:p.
Entertainment Pack for OS/2
.br
Othello
.br
Version 1.0
.br
All rights reserved.
.br
Copyright (C) Peter Wansch, 1994
.br
.br
January 28, 1994
.br
Vienna, Austria

.*-------------------------------------------------------------------
.* Game menu help panels
.*-------------------------------------------------------------------
:h1 res=2110 name=PANEL_GAME.Help for Game
:p.Use the :hp2.Game:ehp2. menu to start a new game or to get a hint on a good move.

:h2 res=2111 name=PANEL_GAME_NEW.Help for New
:p.Choose :hp2.New:ehp2. to begin a new game.

:h2 res=2112 name=PANEL_GAME_HINT.Help for Hint
:p.Choose :hp2.Hint:ehp2. to get a hint on a good move.

.*-------------------------------------------------------------------
.* Options menu help panels
.*-------------------------------------------------------------------
:h1 res=2113 name=PANEL_OPTIONS.Help for Options
:p.Use the :hp2.Options:ehp2. menu to change the level of play or to assign sounds to events.

:h2 res=2114 name=PANEL_OPTIONS_BEGINNER.Help for Beginner
:p.Choose :hp2.Beginner:ehp2. to switch the current level of play to the Beginner level.
The higher the level of play, the longer the computer spends calculating its move.

:h2 res=2115 name=PANEL_OPTIONS_ADVANCED.Help for Advanced
:p.Choose :hp2.Advanced:ehp2. to switch the current level of play to the Advanced level.
The higher the level of play, the longer the computer spends calculating its move.

:h2 res=2116 name=PANEL_OPTIONS_MASTER.Help for Master
:p.Choose :hp2.Master:ehp2. to switch the current level of play to the Master level.
The higher the level of play, the longer the computer spends calculating its move.

:h2 res=2117 name=PANEL_OPTIONS_PLAYER.Help for Player starts
:p.Choose :hp2.Player starts:ehp2. if you want to make the first move.

:h2 res=2118 name=PANEL_OPTIONS_COMPUTER.Help for Computer starts
:p.Choose :hp2.Computer starts:ehp2. if you want the Computer to make the first move.

:h2 res=16000 name=PANEL_SMBGCOLOR.Help for Background color
:p.Use the :hp2.Background color:ehp2. menu to choose a background color.

:h2 res=16001 name=PANEL_BGCOLOR.Help for Background color
:p.Choose this color to display the background in this color.

:h2 res=2119 name=PANEL_OPTIONS_SOUND.Help for Sound
:p.Choose :hp2.Sound:ehp2. to display a dialog window that lets you assign sounds
to events.

:h3 res=13510 name=PANEL_EVEDLG.Help for Sound dialog
:p.Select :hp2.Sound:ehp2. to activate sound for events.
:p.The events list contains the names of all events you can assign sounds to.
:p.The sounds list contains the names of all available sound files as well as an entry <No sound> and
<Beep>. To assign a sound to an event, select the event first and then double-click on the
sound that you want to assign.
:p.Use the :hp2.Volume:ehp2. slider to set the sound volume.
:p.The :hp2.Play (>):ehp2. button plays the currently selected sound.
:p.The :hp2.OK:ehp2. button closes the Sound dialog and the changed settings
become active.
:p.The :hp2.Cancel:ehp2. button closes the Sound dialog. Any changes that were made
are discarded.


.*-------------------------------------------------------------------
.* Help menu help panels
.*-------------------------------------------------------------------
:h1 res=2120 name=PANEL_HELP_HELP.Help for Help
:p.Use the :hp2.Help:ehp2. menu to obtain help information.

:h2 res=2124 name=PANEL_HELPINDEX.Help for Help index
:p.Choose :hp2.Help index:ehp2. to display the help index.
The help index lists the titles of the help information that is available.

:h2 res=2122 name=PANEL_HELPEXTENDED.Help for General help
:p.Choose :hp2.General help:ehp2. to get general help for the program. General help provides
general information about Othello.

:h2 res=2123 name=PANEL_HELPKEYS.Help for Keys help
:p.Choose :hp2.Keys help:ehp2. to see a list of keys and a description of the function of
the keys.

.*--------------------------------------------------------------*\
.*  Keys help panel                                             *
.*      res = PANEL_KEYSHELP                                    *
.*--------------------------------------------------------------*/
:h3 res=2129 name=PANEL_KEYSHELP.Keys help
:i1 id=keysHelp.Keys help
:hp2.CONTROL KEYS:ehp2.
:dl break=all.
:dt.Arrow keys
:dd.Move you around the board in the direction of the arrow.
:dt.Enter, Spacebar, Return
:dd.Places a marker.
:dt.Ctrl+N
:dd.Starts a new game.
:dt.Ctrl+H
:dd.Gets a hint.
:edl.
:p.:hp2.HELP KEYS:ehp2.
:dl break=all.
:dt.F1
:dd.Gets help
:dt.Esc
:dd.Views previous Help panel or ends help if only one panel has been viewed.
:dt.Alt+F4
:dd.Ends help or ends program.
:edl.
:p.:hp2.SYSTEM KEYS:ehp2.
:dl break=all.
:dt.Alt+Esc
:dd.Switches to the next program.
:dt.Ctrl+Esc
:dd.Switches to the Window List.
:edl.
:p.:hp2.WINDOW KEYS:ehp2.
:dl break=all.
:dt.Underlined letter
:dd.Moves among the choices on the action bar and pull-downs submenu or
executes the function chosen.
:dt.Alt+Underlined letter
:dd.Executes the function chosen immediately.
:edl.

:h2 res=2121 name=PANEL_HELPHELPFORHELP.Help for Using help
:p.Choose :hp2.Using help:ehp2. to obtain information on how to
use the Help facility.

:h2 res=2125 name=PANEL_HELPPRODUCTINFO.Help for Product information
:p.Choose :hp2.Product information:ehp2. to view a dialog that displays information about Othello, such as version
number and copyright notice.

:h3 res=3100 name=PANEL_PRODUCTINFODLG.Help for Product information dialog 
:p.This dialog displays information about Othello, such as version
number and copyright notice.
:p.The :hp2.OK:ehp2. button closes the product information dialog.
:euserdoc.
