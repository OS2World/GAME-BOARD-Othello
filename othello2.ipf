:userdoc.

:h1 res=2100 name=PANEL_MAIN.šber Othello
:i1 id=aboutOthello.šber Othello
:p.Othello wir auf einem 8 mal 8 groáem Spielbrett gespielt. Es gibt 64 Spielsteine die auf ihren
beiden Seiten verschiedene Farben haben (entweder Rot und Blau oder Schwarz und Weiss auf Monochrom-
Bildschirmen). Am Beginn des Spieles befinden sich die ersten vier Spielsteine diagonal in der Mitte des Spielbretts.

:p.Der Spieler legt Spielsteine auf unbesetzte Felder. Jeder Spielstein muá so plaziert werden,
daá unmittelbar daneben ein gegnerischer Spielstein liegt (entweder horizontal, vertikal oder diagonal)
und daá in direkter Verbindung ein eigener Spielstein liegt. Zwischen dem zu setzenden und dem anderen
eigenen Spielstein, mssen ein oder mehrere gegnerische Spielsteine liegen (und keine leeren Felder).
Alle gegnerischen Spielsteine, die auf diese Art eingeschlossen werden, werden daraufhin umgedreht.
W„hrend Sich die Position eines Spielsteins nicht „ndert, k”nnen diese aber im Verlauf eines Spieles
”fters umgedreht werden.
:p.Fall man mit dem Plazieren eines Spielsteins gegnerische Spielsteine in mehreren Richtungen einschlieát, so
werden alle Spielsteine in allen Richtungen in denen gegnerische Spielsteine eingeschlossen sind, umgedreht.
:p.Falls ein Spieler keinen Zug machen kann, muá er solange passen, bis er wieder einen Zug machen kann.
:p.Das Spiel ist vorbei, wenn entweder alle 64 Felder besetzt sind, oder wenn keiner der beiden Spieler
noch einen Zug machen kann. Der Gewinner ist derjenige, der mehr Spielsteine in seiner Farbe am Spielbrett hat.

:p.Zus„tzliche Information:
:p.:link reftype=hd res=2126.Spielverlauf:elink.
.br
:link reftype=hd res=2129.Tasten:elink.
.br
:link reftype=hd res=2127.Anmerkung:elink.
.br
:link reftype=hd res=2128.Copyright:elink.

:h1 res=2126 name=PANEL_HELPPLAYING.Spielverlauf
:i1 id=playingHelp.Spielverlauf
:p.W„hlen Sie :hp2.Neu:ehp2. aus dem :hp2.Spiel:ehp2. Men um ein neues Spiel zu beginnen. W„hlen Sie den
Schwierigkeitsgrad (Anf„nger, Fortgeschrittener oder Meister) indem Sie den entpsrechenden
Eintrag im :hp2.Optionen:ehp2. Men markieren.

:p.Um einen Spielstein zu plazieren positionieren Sie den Mauszeiger in dem gewnschten, leeren Feld.
Falls es sich um einen gltigen Zug handelt, wird der Mauszeiger zu einem Kreuz. Jetzt k”nnen Sie das
Feld anklicken um einen Spielstein zu plazieren. Sie k”nnen auch die Pfeiltasten verwenden um den
Mauszeiger zu positionieren. Drcken Sie dann die Enter-, Zeilenumschaltung- oder Leertaste um einen
Spielstein zu plazieren. Falls Sie oder der Computer passen mssen, wird eine entsprechende Nachricht angezeigt.

:p.W„hlen Sie :hp2.Hinweis:ehp2. um einen Hinweis auf einen guten Zug zu erhalten. 
W„hlen Sie :hp2.Computer beginnt:ehp2. falls Sie m”chten, daá der Computer den ersten Zug macht.

.*--------------------------------------------------------------*\
.*  Note panel                                                  *
.*      res = PANEL_HELPNOTE                                    *
.*--------------------------------------------------------------*/
:h1 res=2127 name=PANEL_HELPNOTE.Anmerkung
:i1 id=noteHelp.Anmerkung
:p.Falls Sie Othello auf einem Monochrom-Bildschirm spielen, werden
die Spielsteine in Weiss und Schwarz angezeigt. Othello kann auch auf Computern
gespielt werden, an die keine Zeigereinheit angeschlossen ist.

.*--------------------------------------------------------------*\
.*  Note panel                                                  *
.*      res = PANEL_HELPCREATOR                                 *
.*--------------------------------------------------------------*/
:h1 res=2128 name=PANEL_HELPCREATOR.Copyright
:i1 id=noticeHelp.Copyright
:p.
Entertainment Pack fr OS/2
.br
Othello
.br
Version 1.0
.br
Alle Rechte vorbehalten.
.br
Copyright (C) Peter Wansch, 1994
.br
.br
28. J„nner 1994
.br
Wien, ™sterreich

.*-------------------------------------------------------------------
.* Game menu help panels
.*-------------------------------------------------------------------
:h1 res=2110 name=PANEL_GAME.Hilfe fr Spiel
:p.Verwenden Sie das :hp2.Spiel:ehp2. Men um ein neues Spiel zu beginnen oder um einen Hinweis auf einen
guten Zug zu erhalten.

:h2 res=2111 name=PANEL_GAME_NEW.Hilfe fr Neu
:p.W„hlen Sie :hp2.Neu:ehp2. um ein neues Spiel zu beginnen.

:h2 res=2112 name=PANEL_GAME_HINT.Hilfe fr Hinweis
:p.W„hlen Sie :hp2.Hinweis:ehp2. um einen Hinweis auf einen guten Zug zu erhalten.

.*-------------------------------------------------------------------
.* Options menu help panels
.*-------------------------------------------------------------------
:h1 res=2113 name=PANEL_OPTIONS.Hilfe fr Optionen
:p.Verwenden Sie das :hp2.Optionen:ehp2. Men um den Schwierigkeitsgrad einzustellen oder um Ereignissen
T”ne zuzuweisen.

:h2 res=2114 name=PANEL_OPTIONS_BEGINNER.Hilfe fr Anf„nger
:p.W„hlen Sie :hp2.Anf„nger:ehp2. um den Schwierigkeitsgrad auf Anf„nger zu stellen.
Je h”her der Schwierigkeitsgrad ist, umso l„nger braucht der Computer zur Berechnung seines Zugs.

:h2 res=2115 name=PANEL_OPTIONS_ADVANCED.Hilfe fr Fortgeschrittener
:p.W„hlen Sie :hp2.Fortgeschrittener:ehp2. um den Schwierigkeitsgrad auf Fortgeschrittener zu stellen.
Je h”her der Schwierigkeitsgrad ist, umso l„nger braucht der Computer zur Berechnung seines Zugs.

:h2 res=2116 name=PANEL_OPTIONS_MASTER.Hilfe fr Meister
:p.W„hlen Sie :hp2.Meister:ehp2. um den Schwierigkeitsgrad auf Meister zu stellen.
Je h”her der Schwierigkeitsgrad ist, umso l„nger braucht der Computer zur Berechnung seines Zugs.

:h2 res=2117 name=PANEL_OPTIONS_PLAYER.Hilfe fr Spieler beginnt
:p.W„hlen Sie :hp2.Spieler beginnt:ehp2. falls Sie den ersten Zug machen wollen.

:h2 res=2118 name=PANEL_OPTIONS_COMPUTER.Hilfe fr Computer beginnt
:p.W„hlen Sie :hp2.Computer beginnt:ehp2. falls Sie m”chten, daá der Computer den ersten Zug macht.

:h2 res=16000 name=PANEL_SMBGCOLOR.Hilfe fr Hintergrundfarbe
:p.Verwenden Sie das :hp2.Hintergrundfarbe:ehp2. Men um eine Hintergrundfarbe zu w„hlen.

:h2 res=16001 name=PANEL_BGCOLOR.Hilfe fr Hintergrundfarbe
:p.W„hlen Sie diese Farbe, um den Hintergrund in dieser Farbe anzuzeigen.

:h2 res=2119 name=PANEL_OPTIONS_SOUND.Hilfe fr Ton
:p.W„hlen Sie :hp2.Ton:ehp2. um ein Dialogfenster anzuzeigen, in dem Sie
Ereignissen T”ne zuweisen k”nnen.

:h3 res=13510 name=PANEL_EVEDLG.Hilfe fr Ton Dialogfenster
:p.W„hlen Sie :hp2.Ton:ehp2. falls Sie T”ne fr Ereignisse aktivieren m”chten.
:p.Die Ereignisliste enth„lt alle Ereignisse, denen T”ne zugewiesen werden k”nnen.
:p.Die Tonliste enth„lt die Namen aller installierten Ton-Dateien, sowie einen Eintrag fr <Kein Ton> und einen Eintrag
fr <Biep>. Um einem Ereignis einen Ton zuzuweisen, w„hlen Sie zuerst das Ereignis und klicken dann doppelt auf den Ton.
:p.Verwenden Sie den Schieberegler :hp2.Lautst„rke:ehp2. um die Lautst„rke einzustellen.
:p.Die Schaltfl„che:hp2.Wiedergabe (>):ehp2. spielt den momentan gew„hlten Ton.
:p.Die :hp2.OK:ehp2. Schaltfl„che schlieát das Ton Dialogfenster. Die vorgenommenen
nderungen werden gespeichert.
:p.Die :hp2.Abbruch:ehp2. Schaltfl„che schlieát das Ton Dialogfenster ohne die vorgenommenen nderungen zu speichern.

.*-------------------------------------------------------------------
.* Help menu help panels
.*-------------------------------------------------------------------
:h1 res=2120 name=PANEL_HELP_HELP.Hilfe fr Hilfe
:p.Verwenden Sie das :hp2.Hilfe:ehp2. Men um Hilfe zu erhalten.

:h2 res=2124 name=PANEL_HELPINDEX.Hilfe fr Hilfeindex
:p.W„hlen Sie :hp2.Hilfeindex:ehp2. um den Hilfeindex anzuzeigen.
Der Hilfeindex listet die Titel der verfgbaren Hilfekapitel auf.

:h2 res=2122 name=PANEL_HELPEXTENDED.Hilfe fr Erweiterte Hilfe
:p.W„hlen Sie :hp2.Erweiterte Hilfe:ehp2. um allgemeine Hilfe ber das Programm zu bekommen. Die erweiterte Hilfe bietet 
allgemeine Hilfe ber Othello.

:h2 res=2123 name=PANEL_HELPKEYS.Hilfe fr Hilfe fr Tasten
:p.W„hlen Sie :hp2.Hilfe fr Tasten:ehp2. um eine Liste der Tasten und eine Beschreibung deren Funktion in diesem 
Spiel zu sehen.

.*--------------------------------------------------------------*\
.*  Keys help panel                                             *
.*      res = PANEL_KEYSHELP                                    *
.*--------------------------------------------------------------*/
:h3 res=2129 name=PANEL_KEYSHELP.Hilfe fr Tasten
:i1 id=keysHelp.Hilfe fr Tasten
:hp2.Steuerungstasten:ehp2.
:dl break=all.
:dt.Pfeiltasten
:dd.Plazieren den Mauszeiger entsprechend ihrer Richtung.
:dt.Enter-, Zeilenschaltung-, Leertaste
:dd.Plaziert einen Spielstein.
:dt.Strg+N
:dd.Beginnt ein neues Spiel.
:dt.Strg+H
:dd.Gibt einen Hinweis auf einen guten Zug.
:edl.
:p.:hp2.Hilfetasten:ehp2.
:dl break=all.
:dt.F1
:dd.Zeigt Hilfe.
:dt.Esc
:dd.Zeigt das vorige Hilfekapitel oder beendet die Hilfe..
:dt.Alt+F4
:dd.Beendet die Hilfe oder das Programm.
:edl.
:p.:hp2.Systemtasten:ehp2.
:dl break=all.
:dt.Alt+Esc
:dd.Schaltet zum n„chsten Programm.
:dt.Strg+Esc
:dd.Schaltet zur Fensterliste.
:edl.
:p.:hp2.Fenstertasten:ehp2.
:dl break=all.
:dt.Unterstrichener Buchstabe
:dd.Dient zur Auswahl eines Befehls oder eines Mens.
:dt.Alt+unterstrichener Buchstabe
:dd.Dient zur Ausfhrung eines Befehls auf einem Men.
:edl.

:h2 res=2121 name=PANEL_HELPHELPFORHELP.Hilfe fr Hilfe fr Hilfefunktion
:p.W„hlen Sie :hp2.Hilfe fr Hilfefunktion:ehp2. um Information ber die Hilfefunktion zu erhalten.

:h2 res=2125 name=PANEL_HELPPRODUCTINFO.Hilfe fr Produktinformation
:p.W„hlen Sie :hp2.Produktinformation:ehp2. um ein Dialogfenster anzuzeigen, das Informationen
ber Othello enth„lt, wie etwa das Copyright und die Versionsnummer.

:h3 res=3100 name=PANEL_PRODUCTINFODLG.Hilfe fr Produktinformation Dialogfenster
:p.Dieses Dialogfenster zeigt Informationen ber Othello an, wie etwa das Copyright und
die Versionsnummer.
:p.Die :hp2.OK:ehp2. Schaltfl„che schlieát das Produktinformation Dialogfenster.
:euserdoc.
