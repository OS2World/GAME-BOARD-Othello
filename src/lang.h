#ifndef LANG_H
#define LANG_H

#define LANG_EN    0
#define LANG_ES    1
#define LANG_NL    2
#define LANG_DE    3
#define LANG_FR    4
#define LANG_IT    5
#define LANG_COUNT 6

enum {
    STR_MENU_GAME = 0,
    STR_MENU_NEW,
    STR_MENU_HINT,
    STR_MENU_PAUSE,
    STR_MENU_QUIT,
    STR_MENU_EXIT,
    STR_MENU_OPTIONS,
    STR_MENU_LEVEL,
    STR_MENU_BEGINNER,
    STR_MENU_ADVANCED,
    STR_MENU_MASTER,
    STR_MENU_PLAYER,
    STR_MENU_COMPUTER,
    STR_MENU_BGCOLOR,
    STR_MENU_SOUND,
    STR_MENU_LANGUAGE,
    STR_MENU_SAVEONEXIT,
    STR_MENU_BACKGRND,
    STR_MENU_FRAME,
    STR_MENU_HELP,
    STR_MENU_ABOUT,
    STR_PLACE_BLACK,
    STR_PLACE_BLUE,
    STR_GAME_DRAW,
    STR_YOU_LOST,
    STR_YOU_WON,
    STR_I_MUST_PASS,
    STR_YOU_MUST_PASS,
    STR_HELP_TITLE,
    STR_MENU_HELPINDEX,
    STR_MENU_GENHELP,
    STR_MENU_KEYSHELP,
    STR_MENU_USINGHELP,
    STR_HELP_MISSING,
    STR_COUNT
};

extern int current_lang;
extern const char *lang_strings[LANG_COUNT][STR_COUNT];
#define tr(id) ((char*)lang_strings[current_lang][(id)])

#endif
