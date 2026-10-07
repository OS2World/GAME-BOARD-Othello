/* lang.c - Language string definitions for Othello for OS/2 */

#include "lang.h"

int current_lang = LANG_EN;

const char *lang_strings[LANG_COUNT][STR_COUNT] = {
  { /* EN */
    "~Game", "~New\tCtrl+N", "~Hint\tCtrl+H",
    "~Pause Game\tCtrl+P", "~Quit Game\tCtrl+Q", "E~xit\tCtrl+X",
    "~Options", "~Level",
    "~Beginner", "~Advanced", "~Master",
    "~Player starts", "~Computer starts", "B~ackground color",
    "~Sound...", "~Language", "~Save settings on exit",
    "~Background Run\tCtrl+B", "~Frame Controls\tCtrl+F",
    "~Help", "~About...",
    "You place the black markers.", "You place the blue markers.",
    "The game is a draw.", "You lost by %d.", "You won by %d.",
    "I must pass.", "You must pass.",
    "Othello Help Window",
    "Help ~index", "~General help", "~Keys help", "~Using help",
    "The help file %s was not found.\nPlease put it in the help folder next to the program."
  },
  { /* ES */
    "~Juego", "~Nuevo\tCtrl+N", "~Pista\tCtrl+H",
    "~Pausar Juego\tCtrl+P", "~Abandonar Juego\tCtrl+Q", "Sa~lir\tCtrl+X",
    "~Opciones", "~Nivel",
    "~Principiante", "~Avanzado", "~Maestro",
    "~Jugador empieza", "~Ordenador empieza", "Color de ~fondo",
    "~Sonido...", "~Idioma", "~Guardar ajustes al salir",
    "~Fondo Activo\tCtrl+B", "~Controles de Marco\tCtrl+F",
    "~Ayuda", "~About...",
    "Usted pone las fichas negras.", "Usted pone las fichas azules.",
    "El juego es un empate.", "Perdiste por %d.", "Ganaste por %d.",
    "Debo pasar.", "Debes pasar.",
    "Ventana de Ayuda Othello",
    "~Indice de ayuda", "Ayuda ~general", "Ayuda de ~teclas", "~Usar la ayuda",
    "No se encontro el archivo de ayuda %s.\nPongalo en la carpeta help junto al programa."
  },
  { /* NL */
    "~Spel", "~Nieuw\tCtrl+N", "~Hint\tCtrl+H",
    "~Pauze Spel\tCtrl+P", "~Spel Stoppen\tCtrl+Q", "A~fsluiten\tCtrl+X",
    "~Opties", "~Niveau",
    "~Beginner", "~Gevorderd", "~Meester",
    "~Speler begint", "~Computer begint", "~Achtergrondkleur",
    "~Geluid...", "~Taal", "~Instellingen bewaren bij afsluiten",
    "~Achtergrond Actief\tCtrl+B", "~Kaderinstelling\tCtrl+F",
    "~Help", "~About...",
    "U plaatst de zwarte stenen.", "U plaatst de blauwe stenen.",
    "Het spel staat gelijk.", "Je hebt verloren met %d.", "Je hebt gewonnen met %d.",
    "Ik moet passen.", "U moet passen.",
    "Othello Help Venster",
    "Help~index", "~Algemene help", "~Toetsenhulp", "Help ~gebruiken",
    "Het helpbestand %s is niet gevonden.\nPlaats het in de map help naast het programma."
  },
  { /* DE */
    "~Spiel", "~Neu\tCtrl+N", "~Hinweis\tCtrl+H",
    "~Pause\tCtrl+P", "~Spiel Beenden\tCtrl+Q", "~Beenden\tCtrl+X",
    "~Optionen", "~Stufe",
    "~Anfaenger", "~Fortgeschritten", "~Meister",
    "~Spieler beginnt", "~Computer beginnt", "~Hintergrundfarbe",
    "~Ton...", "~Sprache", "~Einstellungen speichern",
    "~Hintergrundlauf\tCtrl+B", "~Rahmensteuerung\tCtrl+F",
    "~Hilfe", "~About...",
    "Sie spielen mit den schwarzen Steinen.", "Sie spielen mit den blauen Steinen.",
    "Das Spiel ist unentschieden.", "Sie haben um %d Punkte verloren.", "Sie haben um %d Punkte gewonnen.",
    "Ich muss passen.", "Sie muessen passen.",
    "Othello Hilfe Fenster",
    "Hilfe~index", "~Allgemeine Hilfe", "~Tastenhilfe", "Hilfe ~verwenden",
    "Die Hilfedatei %s wurde nicht gefunden.\nBitte in den Ordner help neben dem Programm legen."
  },
  { /* FR */
    "~Jeu", "~Nouveau\tCtrl+N", "~Astuce\tCtrl+H",
    "~Pause Jeu\tCtrl+P", "~Quitter Jeu\tCtrl+Q", "~Quitter\tCtrl+X",
    "~Options", "~Niveau",
    "~Debutant", "~Avance", "~Maitre",
    "~Joueur commence", "~Ordinateur commence", "Couleur de ~fond",
    "~Son...", "~Langue", "~Sauvegarder a la fermeture",
    "~Arriere-plan Actif\tCtrl+B", "~Controles du Cadre\tCtrl+F",
    "~Aide", "~About...",
    "Vous placez les pions noirs.", "Vous placez les pions bleus.",
    "Le jeu est nul.", "Vous avez perdu de %d.", "Vous avez gagne de %d.",
    "Je dois passer.", "Vous devez passer.",
    "Fenetre d'aide Othello",
    "~Index de l'aide", "Aide ~generale", "Aide des ~touches", "~Utiliser l'aide",
    "Le fichier d'aide %s est introuvable.\nPlacez-le dans le dossier help a cote du programme."
  },
  { /* IT */
    "~Gioco", "~Nuovo\tCtrl+N", "~Suggerimento\tCtrl+H",
    "~Pausa Gioco\tCtrl+P", "~Abbandona Gioco\tCtrl+Q", "~Esci\tCtrl+X",
    "~Opzioni", "~Livello",
    "~Principiante", "~Avanzato", "~Maestro",
    "~Giocatore inizia", "~Computer inizia", "Colore di ~sfondo",
    "~Suono...", "~Lingua", "~Salva alla chiusura",
    "~Sfondo Attivo\tCtrl+B", "~Controllo Cornice\tCtrl+F",
    "~Aiuto", "~About...",
    "Lei mette i pezzi neri.", "Lei mette i pezzi blu.",
    "La partita e patta.", "Hai perso di %d.", "Hai vinto di %d.",
    "Devo passare.", "Deve passare.",
    "Finestra Aiuto Othello",
    "~Indice dell'aiuto", "Aiuto ~generale", "Aiuto ~tasti", "~Usare l'aiuto",
    "Il file di aiuto %s non e stato trovato.\nMetterlo nella cartella help accanto al programma."
  }
};
