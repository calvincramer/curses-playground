
#include <form.h>
#include <math.h>
#include <menu.h>
#include <ncurses.h>
#include <panel.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

void wait_advance() {
  while (true) {
    // Space key
    if (getch() == 0x20) {
      return;
    }
  }
}

void fill_screen() {
  int max_y = 0;
  int max_x = 0;
  getmaxyx(stdscr, max_y, max_x);  // Gets the size of the window
  for (int i = 0; i < max_y; i++) {
    for (int j = 0; j < max_x; j++) {
      mvaddch(i, j, '.' | A_REVERSE | A_BOLD);
    }
  }
}

void fill_window(WINDOW* win, chtype ch) {
  int max_y = 0;
  int max_x = 0;
  getmaxyx(win, max_y, max_x);  // Gets the size of the window
  for (int i = 0; i < max_y; i++) {
    for (int j = 0; j < max_x; j++) {
      mvwaddch(win, i, j, ch);
    }
  }
}

void basics() {
  // Print
  printw("Basics\n");
  printw("Foo bar\n");
  refresh();
  wait_advance();

  // Fill
  int max_y = 0;
  int max_x = 0;
  getmaxyx(stdscr, max_y, max_x);  // Gets the size of the window
  fill_screen();
  mvprintw(max_y / 2, (max_x / 2) - 3, "Center");
  refresh();
  wait_advance();

  // Window
  clear();
  refresh();
  WINDOW* window = newwin(12, 40, 22, 50);
  int pos = 1;
  box(window, 0, 0);
  wattrset(window, A_NORMAL);
  mvwprintw(window, pos++, 1, "This is normal text");
  wattrset(window, A_STANDOUT);
  mvwprintw(window, pos++, 1, "This is standout text");
  wattrset(window, A_UNDERLINE);
  mvwprintw(window, pos++, 1, "This is underline text");
  wattrset(window, A_REVERSE);
  mvwprintw(window, pos++, 1, "This is reverse text");
  wattrset(window, A_BLINK);
  mvwprintw(window, pos++, 1, "This is blink text");
  wattrset(window, A_DIM);
  mvwprintw(window, pos++, 1, "This is dim text");
  wattrset(window, A_BOLD);
  mvwprintw(window, pos++, 1, "This is bold text");
  wattrset(window, A_PROTECT);
  mvwprintw(window, pos++, 1, "This is protected text");
  wattrset(window, A_INVIS);
  mvwprintw(window, pos++, 1, "This is invisible text");
  wattrset(window, A_ALTCHARSET);
  mvwprintw(window, pos++, 1, "This is alternate charater set text");
  wrefresh(window);
  wait_advance();

  // Destroy window
  wclear(window);
  wrefresh(window);
  delwin(window);
  refresh();
  doupdate();
  clear();
  wait_advance();

  // Create a new window
  WINDOW* win2 = newwin(20, 60, 15, 5);
  box(win2, 0, 0);
  int win_max_y = 0;
  int win_max_x = 0;
  getmaxyx(win2, win_max_y, win_max_x);
  mvprintw(2, 0, "The window size is %d rows by %d columns", win_max_y, win_max_x);
  mvwvline(win2, 1, 5, ACS_VLINE, 8);
  wrefresh(win2);
  wait_advance();

  // Color
  if (has_colors() == true) {
    short my_color = COLOR_WHITE + 2;
    start_color();
    init_color(my_color, 800, 15, 50);
    init_pair(1, my_color, COLOR_BLACK);
    wattr_on(win2, COLOR_PAIR(1), NULL);
    mvwprintw(win2, 8, 1, "Color!!!");
    // Get the color pair information
    short pindex = 1, foreground, background;
    pair_content(pindex, &foreground, &background);
    mvprintw(4, 1, "The color pair 1 has foreground %d and background %d", foreground, background);
    // Get the color information
    short red, green, blue;
    color_content(my_color, &red, &green, &blue);
    mvprintw(5, 1, "The color pair %d has red %d, green %d, and blue %d", my_color, red, green, blue);
    wrefresh(win2);
    wait_advance();
  } else {
    mvwprintw(win2, 3, 0, "This terminal does not support color!");
  }

  wclear(win2);
  wrefresh(win2);
  delwin(win2);
  refresh();
  doupdate();
  clear();
  wait_advance();
}

void mouse() {
  int ch = 0;
  MEVENT event;

  mmask_t mask, oldmask;
  mask = BUTTON1_PRESSED | BUTTON_SHIFT | BUTTON2_CLICKED | REPORT_MOUSE_POSITION | ALL_MOUSE_EVENTS;
  mousemask(mask, &oldmask);  // Assigns new mask and saves previous one
  printf("\033[?1002h\n");    // mouse drag (mouse movement when mouse button)
  // printf("\033[?1003h\n"); // all mouse movement events
  mouseinterval(0);  // No delay for mouse clicks

  mvprintw(0, 0, "Click the left mouse button in and outside the window. q to go next");
  WINDOW* win = newwin(20, 60, 15, 5);
  box(win, 0, 0);
  wrefresh(win);

  while (true) {
    ch = getch();
    if (ch == 'q' || ch == 'Q' || ch == 0x20)
      break;
    if (ch != KEY_MOUSE)
      continue;
    if (getmouse(&event) != OK)
      continue;

    bool inwindow = wenclose(win, event.y, event.x);
    int wx = event.x, wy = event.y, sx, sy;
    mvprintw(11, 1, "The event coordinates are %d, %d", event.y, event.x);
    clrtoeol();
    if (wmouse_trafo(win, &wy, &wx, FALSE) == TRUE) {  // Convert screen -> window coordinates
      mvprintw(12, 1, "The window coordinates are %d, %d and the screen coordinates are %d, %d", wy, wx, event.y, event.x);
      sx = wx;
      sy = wy;
      wmouse_trafo(win, &sy, &sx, TRUE);  // Convert screen -> window coordinates
      mvprintw(13, 1, "The window coordinates are %d, %d(converted back)", sy, sx);
      clrtoeol();
    } else {
      mvprintw(12, 1, "The screen coordinates are %d, %d and not in the window.", wy, wx);
      clrtoeol();
    }
    wrefresh(win);
    refresh();
    if (event.bstate & BUTTON1_PRESSED) {
      mvwprintw(win, 8, 1, "You pressed the left mouse button at %d, %d [in window? %d]", event.x, event.y, inwindow);
      if (inwindow == TRUE) {
        wmouse_trafo(win, &event.y, &event.x, FALSE);
        mvwaddch(win, event.y, event.x, ACS_DIAMOND | A_BOLD);
        wmove(win, event.y, event.x);
        wrefresh(win);
      } else {
        mvaddch(event.y, event.x, 'O' | A_DIM);
        move(event.y, event.x);
        refresh();
      }
    }
    if (event.bstate & BUTTON2_CLICKED) {
      mvwprintw(win, 9, 1, "You clicked the right mouse button at %d, %d", event.x, event.y);
      clrtoeol();
      wrefresh(win);
    }
    if (event.bstate & REPORT_MOUSE_POSITION) {
      mvwprintw(win, 10, 1, "You moved the mouse to %d, %d", event.x, event.y);
      clrtoeol();
      wrefresh(win);
      mvaddch(event.y, event.x, '.' | A_BOLD);
      refresh();
    }
  }

  wclear(win);
  wrefresh(win);
  delwin(win);
  refresh();
  doupdate();
  clear();
}

void panel() {
  const int NUM_PANELS = 5;
  PANEL* panels[NUM_PANELS];

  fill_screen();
  mvprintw(0, 0, "Panels!");
  refresh();

  // Create
  WINDOW* win = newwin(16, 40, 5, 5);
  wrefresh(win);
  fill_window(win, '0');
  box(win, 0, 0);
  panels[0] = new_panel(win);

  win = newwin(16, 40, 5, 50);
  wrefresh(win);
  fill_window(win, '1');
  box(win, 0, 0);
  panels[1] = new_panel(win);

  win = newwin(16, 40, 24, 5);
  wrefresh(win);
  fill_window(win, '2');
  box(win, 0, 0);
  panels[2] = new_panel(win);

  win = newwin(16, 40, 24, 50);
  wrefresh(win);
  fill_window(win, '3');
  box(win, 0, 0);
  panels[3] = new_panel(win);

  win = newwin(16, 40, 15, 27);
  wrefresh(win);
  fill_window(win, '4');
  box(win, 0, 0);
  panels[4] = new_panel(win);

  update_panels();
  doupdate();
  wait_advance();

  // Move through the panels
  for (int i = 0; i < NUM_PANELS; i++) {
    top_panel(panels[i]);
    update_panels();
    doupdate();
    wait_advance();
  }

  // Move
  for (int x = 27; x < 100; x++) {
    move_panel(panels[4], 15, x);
    update_panels();
    doupdate();
    usleep(1000 * 20);
  }
  move_panel(panels[4], 15, 27);
  update_panels();
  doupdate();
  wait_advance();

  // Clean up the panels, return
  for (int i = 0; i < NUM_PANELS; i++) {
    if (panels[i] == NULL) {
      continue;
    }
    del_panel(panels[i]);
  }
}

void menu() {
  fill_screen();
  refresh();

  // Create the basic items, menu
  ITEM* basic_items[21];
  basic_items[0] = new_item("Item 0", "Description 0");
  basic_items[1] = new_item("Item 1", "Description 1");
  basic_items[2] = new_item("Item 2", "Description 2");
  basic_items[3] = new_item("Item 3", "Description 3");
  basic_items[4] = new_item("Item 4", "Description 4");
  basic_items[5] = new_item("Item 5", "Description 5");
  basic_items[6] = new_item("Item 6", "Description 6");
  basic_items[7] = new_item("Item 7", "Description 7");
  basic_items[8] = new_item("Item 8", "Description 8");
  basic_items[9] = new_item("Item 9", "Description 9");
  basic_items[10] = new_item("Item 10", "Description 10");
  basic_items[11] = new_item("Item 11", "Description 11");
  basic_items[12] = new_item("Item 12", "Description 12");
  basic_items[13] = new_item("Item 13", "Description 13");
  basic_items[14] = new_item("Item 14", "Description 14");
  basic_items[15] = new_item("Item 15", "Description 15");
  basic_items[16] = new_item("Item 16", "Description 16");
  basic_items[17] = new_item("Item 17", "Description 17");
  basic_items[18] = new_item("Item 18", "Description 18");
  basic_items[19] = new_item("Item 19", "Description 19");
  basic_items[20] = NULL;
  MENU* basic_menu = new_menu(basic_items);
  set_menu_spacing(basic_menu, 4, 1, 2);
  set_menu_format(basic_menu, 4, 1);  // 4 items at most visible

  // Add some color to the menu
  start_color();
  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_BLUE, COLOR_BLACK);
  init_pair(3, COLOR_WHITE, COLOR_BLACK);
  set_menu_fore(basic_menu, COLOR_PAIR(1));
  set_menu_back(basic_menu, COLOR_PAIR(2));
  set_menu_grey(basic_menu, COLOR_PAIR(3));

  // Window
  WINDOW* win = newwin(20, 60, 1, 1);
  box(win, 0, 0);
  scrollok(win, true);
  set_menu_win(basic_menu, win);
  set_menu_sub(basic_menu, derwin(win, 18, 38, 1, 1));

  keypad(win, TRUE);
  post_menu(basic_menu);
  wrefresh(win);

  // Get input for the menu
  int done = 0;
  while (!done) {
    int ch = wgetch(win);
    switch (ch) {
      case KEY_DOWN:
        mvwprintw(win, 16, 1, "You pressed the down arrow key");
        menu_driver(basic_menu, REQ_DOWN_ITEM);
        break;
      case KEY_UP:
        mvwprintw(win, 16, 1, "You pressed the up arrow key  ");
        menu_driver(basic_menu, REQ_UP_ITEM);
        break;
      case ' ':
        mvwprintw(win, 16, 1, "toggle                        ");
        menu_driver(basic_menu, REQ_TOGGLE_ITEM);
        break;
      case 'w':
        mvwprintw(win, 16, 1, "scroll up                     ");
        menu_driver(basic_menu, REQ_SCR_ULINE);
        break;
      case 's':
        mvwprintw(win, 16, 1, "scroll down                   ");
        menu_driver(basic_menu, REQ_SCR_DLINE);
        break;
      case KEY_PPAGE:
        mvwprintw(win, 16, 1, "page up                       ");
        menu_driver(basic_menu, REQ_SCR_UPAGE);
        break;
      case KEY_NPAGE:
        mvwprintw(win, 16, 1, "page down                     ");
        menu_driver(basic_menu, REQ_SCR_DPAGE);
        break;
      case 'q':  // Quit
        done = 1;
        break;
    }
    wrefresh(win);
  }

  // Cleanup the menu
  unpost_menu(basic_menu);
  for (int i = 0; i < 21; i++) {
    free_item(basic_items[i]);
  }
  free_menu(basic_menu);
}

void form() {
  fill_screen();
  refresh();

  // Create the fields, then form
  FIELD* test_fields[5];
  test_fields[0] = new_field(1, 5, 0, 0, 0, 0);   // label
  test_fields[1] = new_field(1, 5, 1, 0, 0, 0);   // label
  test_fields[2] = new_field(1, 10, 0, 6, 0, 0);  // input
  test_fields[3] = new_field(1, 10, 1, 6, 0, 0);  // input
  test_fields[4] = NULL;
  FORM* test_form = new_form(test_fields);

  // Setup the labels
  field_opts_off(test_fields[0], O_EDIT | O_ACTIVE);
  set_field_buffer(test_fields[0], 0, "FLD1:");
  field_opts_off(test_fields[1], O_EDIT | O_ACTIVE);
  set_field_buffer(test_fields[1], 0, "FLD2:");

  // Setup some colors
  start_color();
  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_BLUE, COLOR_BLACK);
  init_pair(3, COLOR_BLACK, COLOR_WHITE);

  // Set the field options
  set_field_back(test_fields[0], COLOR_PAIR(3));
  set_field_back(test_fields[1], COLOR_PAIR(3));
  set_field_back(test_fields[2], COLOR_PAIR(3));
  set_field_back(test_fields[3], COLOR_PAIR(3));

  set_field_fore(test_fields[0], COLOR_PAIR(2));
  set_field_fore(test_fields[1], COLOR_PAIR(2));
  set_field_fore(test_fields[2], COLOR_PAIR(2));
  set_field_fore(test_fields[3], COLOR_PAIR(2));

  // Window
  WINDOW* win = newwin(20, 60, 3, 3);
  keypad(win, TRUE);
  wrefresh(win);
  int rows = 0;
  int cols = 0;
  scale_form(test_form, &rows, &cols);
  set_form_win(test_form, win);
  set_form_sub(test_form, derwin(win, rows, cols, 2, 2));
  post_form(test_form);
  wrefresh(win);

  // Wait for a key press
  int done = 0;
  while (!done) {
    int ch = getch();
    switch (ch) {
      case KEY_DOWN:
        form_driver(test_form, REQ_NEXT_FIELD);
        break;
      case KEY_UP:
        form_driver(test_form, REQ_PREV_FIELD);
        break;
      case KEY_LEFT:
        form_driver(test_form, REQ_PREV_CHAR);
        break;
      case KEY_RIGHT:
        form_driver(test_form, REQ_NEXT_CHAR);
        break;
      case KEY_BACKSPACE:
        form_driver(test_form, REQ_DEL_PREV);
        break;
      case 'q':  // Quit
        done = 1;
        break;
      default:
        form_driver(test_form, ch);
        break;
    }
    wrefresh(win);
  }

  // Cleanup
  unpost_form(test_form);
  free_form(test_form);
  free_field(test_fields[0]);
  free_field(test_fields[1]);
  free_field(test_fields[2]);
  free_field(test_fields[3]);
  delwin(win);
}

void handle_winch(int sig) {
  endwin();
  // Needs to be called after an endwin() so ncurses will initialize
  // itself with the new terminal dimensions.
  refresh();
  clear();

  mvprintw(0, 0, "COLS = %d, LINES = %d", COLS, LINES);

  int mid = COLS / 2;
  int pad = 10;

  for (int y = (LINES / 2) - 2; y <= (LINES / 2) + 2; y++) {
    for (int i = pad; i < mid - (pad / 2); i++) {
      mvaddch(y, i, '-');
    }
    for (int i = mid + (pad / 2); i < COLS - pad; i++) {
      mvaddch(y, i, '+');
    }
  }

  refresh();
}

void resize_respond() {
  fill_screen();
  refresh();
  curs_set(0);

  struct sigaction sa;
  memset(&sa, 0, sizeof(struct sigaction));
  sa.sa_handler = handle_winch;
  sigaction(SIGWINCH, &sa, NULL);

  handle_winch(0);
  wait_advance();
}

int main(int argc, char* argv[]) {
  // Setup ncurses library
  initscr();
  noecho();
  cbreak();
  raw();
  keypad(stdscr, TRUE);
  curs_set(2);

  basics();
  mouse();
  panel();
  menu();
  form();
  resize_respond();

  // End the curses library
  endwin();
  echo();
  curs_set(1);

  // Return successfully
  printf("Done\n");
  return 0;
}
