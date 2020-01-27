#include "../sentence_gen.c"

#include "sys/time.h"

#ifdef _WIN32
#include "win/curses.h"
#else
#include <ncurses.h>
#endif


#define X_OFFSET 3
#define Y_OFFSET 5
#define VIEW_LENGTH 95

#define SCREEN \
"   ┍───────────────────────────────────────────────────────────────────────────────────────────────┑\n"\
"   │                                                                                               │\n"\
"   ┕───────────────────────────────────────────────────────────────────────────────────────────────┙"

static void add_text();
static void init_curses();
static void draw();

size_t current_pos = 0;
char* user_input;

size_t prompt_len = 0;
char* prompt_string = "";

double wpm = 0;

void add_text() {
  char* str = gen_sentence();
  size_t len = strlen(str);
  str[len] = ' '; str[len+1] = '\0';
  prompt_len += strlen(str);
  strcat(prompt_string, str);
  free(str);
}

void init_curses() {
  setlocale(LC_CTYPE, "");
  initscr();
  clear();
  curs_set(0);
  noecho();
  timeout(16);

  use_default_colors();
  start_color();
  init_pair(1, COLOR_RED, COLOR_WHITE);
  init_pair(2, COLOR_GREEN, -1);
  init_pair(3, COLOR_BLACK, COLOR_WHITE);
}

void draw() {
  erase();
  mvaddstr(Y_OFFSET, 0, SCREEN);

  // make sure there is always enough text to show
  while((prompt_len - current_pos) < VIEW_LENGTH) {
    add_text();
  }

  size_t start = 0;
  if (current_pos > VIEW_LENGTH - 8) {
    start = current_pos - (VIEW_LENGTH - 11);
  }

  for (size_t i = start; i < start + VIEW_LENGTH; i++) {
    if (i < current_pos) {
      if (user_input[i] == prompt_string[i])
        attron(COLOR_PAIR(2));
      else attron(COLOR_PAIR(1));
    } else if (i == current_pos) attron(COLOR_PAIR(3));
    mvaddch(Y_OFFSET + 1, X_OFFSET + (i - start) + 1, prompt_string[i]);
    attroff(COLOR_PAIR(1) | COLOR_PAIR(2) | COLOR_PAIR(3));
  }

  char wpm_text[256];
  //sprintf(wpm_text, "%f", _time);
  sprintf(wpm_text, "%.2f", wpm);
  mvaddstr(Y_OFFSET + 4, X_OFFSET, wpm_text);
}

double calc_time(struct timeval *s, struct timeval *e) {
  return (((e->tv_sec * 1000000) + e->tv_usec) - ((s->tv_sec * 1000000) + s->tv_usec)) / 1000000.0;
}
  
int main(void) {
  init(); // initialize the sentence generator

  init_curses();

  bool stopped = false;

  struct timeval start, now;
  gettimeofday(&start, NULL);

  prompt_string = malloc(1024 * 10);
  user_input = malloc(1024 * 10);

  int c;
  while(1) {
    gettimeofday(&now, NULL);
    c = getch();
    switch (c) {
      case 27: 
        if (stopped) {
          endwin(); 
          exit(1);
        } else {
          stopped = true;
        }
      case 8:
      case 127:
        if (stopped) continue;
        if (current_pos > 0) {
          current_pos--;
          user_input[current_pos] = '\0';
        }
        break;
      case ERR:
        break;
      default:
        if (stopped) continue;
        if (current_pos == 0) gettimeofday(&start, NULL);
        user_input[current_pos] = c;
        user_input[current_pos + 1] = '\0';
        current_pos++;
    }

    if (!stopped) {
      //printf("%f\n", calc_time(&start, &now));
      wpm = ((double)current_pos / 5.0) / (calc_time(&start, &now) / 60.0);
    }
    
    draw();
  }

  return 0;
}
