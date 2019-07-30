/* "algorithm" to generate a sentence based on common english sentences and words
 * this was just a stupid weekend project, and the results are as such. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <locale.h>
#include <time.h>

#define MAX_WORDS     1000
#define WORD_LENGTH   22 
#define PATTERN_COUNT 27

static size_t populate_list(FILE* f, char (*list)[MAX_WORDS][WORD_LENGTH]);
static void init_lists();
static char* convert_to_plural(char* str);
static char* convert_to_sigular(char* str);
static char* convert_to_present(char* str);
static char* get_word(char* p);

/* only public function */
char* gen_sentence();

typedef enum {
  IS, THE, A,
  COMMA,
  NOUN, 
  NOUN_P,
  PRONOUN,
  PRONOUN_P,
  VERB,
  ADJECTIVE,
  ADVERB,
  DETERMINER,
  CONJUNCTION,
  PREPOSITION
} part_of_speech;

static char noun_list[MAX_WORDS][WORD_LENGTH], pronoun_list[MAX_WORDS][WORD_LENGTH],\
    pronoun_p_list[MAX_WORDS][WORD_LENGTH], adjective_list[MAX_WORDS][WORD_LENGTH],\
    verb_list[MAX_WORDS][WORD_LENGTH], adverb_list[MAX_WORDS][WORD_LENGTH],\
    conjunction_list[MAX_WORDS][WORD_LENGTH], determiner_list[MAX_WORDS][WORD_LENGTH],\
    preposition_list[MAX_WORDS][WORD_LENGTH], transition_list[MAX_WORDS][WORD_LENGTH];

static size_t noun_count, pronoun_count, pronoun_p_count, adjective_count, verb_count,\
    adverb_count, conjunction_count, determiner_count, preposition_count, transition_count;

FILE* word_patterns;

size_t populate_list(FILE* f, char (*list)[MAX_WORDS][WORD_LENGTH]) {
  int count = 0; char* buffer = malloc(WORD_LENGTH);
  while (fgets(buffer, WORD_LENGTH, f) != NULL) {
    size_t len = strlen(buffer);
    buffer[len - 1] = '\0';
    strcpy((*list)[count], buffer);
    count++;
  }
  free(buffer);
  return count - 1;
}

void init_lists() {
  FILE* f;

  f = fopen("words/nouns.txt", "r");
  noun_count = populate_list(f, &noun_list);

  f = fopen("words/pronouns.txt", "r");
  pronoun_count = populate_list(f, &pronoun_list);

  f = fopen("words/plural_pronouns.txt", "r");
  pronoun_p_count = populate_list(f, &pronoun_p_list);

  f = fopen("words/adjectives.txt", "r");
  adjective_count = populate_list(f, &adjective_list);

  f = fopen("words/verbs.txt", "r");
  verb_count = populate_list(f, &verb_list);

  f = fopen("words/adverbs.txt", "r");
  adverb_count = populate_list(f, &adverb_list);

  f = fopen("words/determiners.txt", "r");
  determiner_count = populate_list(f, &determiner_list);

  f = fopen("words/conjunctions.txt", "r");
  conjunction_count = populate_list(f, &conjunction_list);

  f = fopen("words/prepositions.txt", "r");
  preposition_count = populate_list(f, &preposition_list);

  f = fopen("words/transitions.txt", "r");
  transition_count = populate_list(f, &transition_list);

  word_patterns = fopen("words/word_patterns.txt", "r");

  fclose(f);
}

char* convert_to_plural(char* str) {
  bool convert = false;
  size_t len = strlen(str);
  char* new_str = malloc(128);
  strcpy(new_str, str);
  switch (str[len - 1]) {
    case 'h':
      switch (str[len - 2]) {
        case 'c':
        case 's':
          convert = true;
      }
      break;
    case 's':
    case 'x':
    case 'z':
      convert = true;
  }
  if (convert) {
    new_str[len] = 'e';
    new_str[len + 1] = 's';
    new_str[len + 2] = '\0';
  } else {
    new_str[len] = 's';
    new_str[len + 1] = '\0';
  }
  return new_str;
}

char* convert_to_sigular(char* str) {
  size_t len = strlen(str);
  char* new_str = malloc(128);
  strcpy(new_str, str);
  new_str[len] = 's';
  new_str[len + 1] = '\0';
  return new_str;
}

char* convert_to_present(char* str) {
  size_t len = strlen(str);
  char* new_str = malloc(128);
  strcpy(new_str, str);
  if (new_str[len - 1] == 'e') {
    new_str[len - 1] = '\0';
  }
  strcat(new_str, "ing");
  return new_str;
}

char* get_word(char* p) {
  int index = rand();
  if (strcmp(p, "IS") == 0)
    return "is";
  else if (strcmp(p, "THE") == 0)
    return "the";
  else if (strcmp(p, "A") == 0)
    return "a";
  else if (strcmp(p, "COMMA") == 0)
    return ",";
  else if (strcmp(p, "NOUN") == 0) 
    return noun_list[index % noun_count];
  else if (strcmp(p, "NOUN_P") == 0)
    return convert_to_plural(noun_list[index % noun_count]);
  else if (strcmp(p, "PRONOUN") == 0)
    return pronoun_list[index % pronoun_count];
  else if (strcmp(p, "PRONOUN_P") == 0)
    return pronoun_p_list[index % pronoun_p_count];
  else if (strcmp(p, "VERB") == 0)
    return verb_list[index % verb_count];
  else if (strcmp(p, "VERB_S") == 0)
    return convert_to_sigular(verb_list[index % verb_count]);
  else if (strcmp(p, "VERB_ING") == 0)
    return convert_to_present(verb_list[index % verb_count]);
  else if (strcmp(p, "ADJECTIVE") == 0)
    return adjective_list[index % adjective_count];
  else if (strcmp(p, "ADVERB") == 0)
    return adverb_list[index % adverb_count];
  else if (strcmp(p, "DETERMINER") == 0)
    return determiner_list[index % determiner_count];
  else if (strcmp(p, "PREPOSITION") == 0)
    return preposition_list[index % preposition_count];
  else if (strcmp(p, "CONJUNCTION") == 0)
    return conjunction_list[index % conjunction_count];
  else if (strcmp(p, "TRANSITION") == 0)
    return transition_list[index % transition_count];
  return NULL;
}

char* gen_sentence(void) {
  char* str = malloc(256);
  memset(str, 0, 256);
  char* pattern = malloc(256);
  int pattern_index = rand() % PATTERN_COUNT;
  for (int i = 0; i <= pattern_index; i++) {
    if (fgets(pattern, 256, word_patterns) == NULL)
      return noun_list[1];
  }
  bool first = true;
  char* pos = strtok(pattern, " \n");
  while (pos != NULL) {
    char* word = get_word(pos);
    if (word == NULL) break;
    if (strcmp(word, ",") == 0) {
      strcat(str, word);
    } else {
      if (first) first = false;
      else strcat(str, " ");
      strcat(str, word);
    }
    pos = strtok(NULL, " \n");
  }
  strcat(str, ".");
  str[0] = toupper(str[0]);
  rewind(word_patterns);
  free(pattern);
  return str;
}

int init(void) {
  setlocale(LC_ALL, "en_us.utf8");
  srand(time(NULL));

  init_lists();

  return 0;
}
