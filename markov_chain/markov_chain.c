/* markov chain implementation to generate sentences based on input text */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#define MAX_PER_SENTENCE 10
#define MAX_NEXT_WORDS 500
#define WORD_CHUNK_SIZE 1024
#define BUCKET_CHUNK_SIZE 512

typedef struct {
  char* str;
  double p;
  int count;
  bool stop_word;
} possible_word;

typedef struct {
  char* str;
  int count;
  possible_word n[MAX_NEXT_WORDS];
} word;

typedef struct {
  char letter;
  int count;
  size_t array_size;
  word** list;
} bucket;

static char* load_file(char* file_name);
static word* new_word(char* str);
static word* new_bucket(char* str);
static word* find_or_add_word(char* str);
static possible_word* random_word();
static int remove_lowest(word* w);
static void gen_probablity();
static char* resolve_probablity(word* w, bool stop);
static bool is_stop_word(char* str);
static double parse_text(char* file_name);

static bucket** buckets;
static size_t bucket_array_size = BUCKET_CHUNK_SIZE;
static int bucket_count = 0;
static int word_count = 0;

char* load_file(char* file_name) {
  FILE* f = fopen(file_name, "r");
  if (!f) {
    printf("file not found\n");
    exit(1);
  }
  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET);
  char* buffer = malloc(fsize + 1);
  if (fread(buffer, 1, fsize, f) < 1) {
    printf("file empty\n");
    exit(1);
  }
  fclose(f);
  buffer[fsize] = '\0';
  return buffer;
}

word* new_word(char* str) {
  word* w = malloc(sizeof(word));
  w->str = strdup(str);
  w->count = 0;
  word_count++;
  return w;
}

word* new_bucket(char* str) {
  bucket* b = malloc(sizeof(bucket));
  b->letter = *str;
  b->array_size = WORD_CHUNK_SIZE;
  b->list = malloc(sizeof(word*) * b->array_size);
  b->list[0] = new_word(str);
  b->count = 1;
  buckets[bucket_count] = b;
  bucket_count++;

  // probably should never happen, but just in case
  if (bucket_count >= bucket_array_size) {
    bucket_array_size += BUCKET_CHUNK_SIZE;
    buckets = realloc(buckets, bucket_array_size * sizeof(bucket*));
  }

  return b->list[0];
}

word* find_or_add_word(char* str) {
  bool make_bucket = true;
  for (int i = 0; i < bucket_count; i++) {
    if (buckets[i]->letter == *str) {
      make_bucket = false;
      for (int s = 0; s < buckets[i]->count; s++) {
        if (strcmp(str, buckets[i]->list[s]->str) == 0) {
          return buckets[i]->list[s];
        }
      }
      if (buckets[i]->count >= buckets[i]->array_size) {
        buckets[i]->array_size += WORD_CHUNK_SIZE;
        buckets[i]->list = realloc(buckets[i]->list, 
            buckets[i]->array_size * sizeof(word));
      }
      buckets[i]->list[buckets[i]->count] = new_word(str);
      return buckets[i]->list[buckets[i]->count++];
    }
  }
  if (make_bucket) 
    return new_bucket(str);

  return NULL;
}

possible_word* random_word() {
  int i = rand() % bucket_count;
  int l = rand() % buckets[i]->count;
  int n = rand() % buckets[i]->list[l]->count;
  return &buckets[i]->list[l]->n[n];
}

int remove_lowest(word* w) {
  int min_index = 0;
  for (int i = 0; i < w->count; i++) {
    if (w->n[i].count < w->n[min_index].count) {
      min_index = i;
    }
  }
  return min_index;
}

void gen_probablity() {
  for (int i = 0; i < bucket_count; i++) {
    for (int w = 0; w < buckets[i]->count; w++) {
      word* c = buckets[i]->list[w];
      int total = 0;

      for (int s = 0; s < c->count; s++) {
        total += c->n[s].count + 1;
        c->n[s].count = 0;
      }

      for (int s = 0; s < c->count; s++)
        c->n[s].p = ((double)(c->n[s].count + 1)/ (double)total);
    }
  }
}

char* resolve_probablity(word* w, bool stop) {
  bool has_stop = false;
  for (int i = 0; i < w->count; i++) {
    if (w->n[i].stop_word) has_stop = true;
  }

  possible_word* tmp;
  while(stop && has_stop) {
    tmp = &w->n[rand() % w->count];
    if (tmp->stop_word) {
      tmp->count++;
      return tmp->str;
    }
  }

  double r = rand() / (double)RAND_MAX;
  for (int i = 0; i < w->count; i++) {
    r -= w->n[i].p;
    if (w->n[i].p > r) {
      w->n[i].count++;

      // dont repeat the same word too much
      if (w->n[i].count > 15) {
        w->n[i].count = 0;
        continue;
      }
      return w->n[i].str;
    }
  }

  // take completely random word
  possible_word* p = random_word();  
  return p->str;
}

bool is_stop_word(char* str) {
  size_t len = strlen(str);
  char last = str[len - 1];
  if (last == '.' || last == '!' || last == '?') {
    return true;
  }
  return false;
}

double parse_text(char* file_name) {
  char* buffer = load_file(file_name);

  buckets = malloc(sizeof(bucket*) * BUCKET_CHUNK_SIZE);

  clock_t start, end;
  start = clock();
  
  word* prev = NULL;
  char* pos = strtok(buffer, " \n");
  while (pos != NULL) {
    if (*pos == ' ' || *pos == '\n') 
      continue;

    word* current = find_or_add_word(pos);
    if (prev != NULL) {
      bool new = true;
      for (int i = 0; i < prev->count; i++) {
        if (strcmp(pos, prev->n[i].str) == 0) {
          prev->n[i].count++;
          new = false;
        }
      }
      if (new) {
        int index = prev->count;
        if (prev->count < (MAX_NEXT_WORDS - 1)) {
          prev->count++;
        } else {
          index = remove_lowest(prev);
        }
        prev->n[index].str = current->str;
        prev->n[index].stop_word = is_stop_word(current->str); 
      }
    }
    prev = current;
    pos = strtok(NULL, " \n");
  }
  end = clock();
  return (double)(end - start)/CLOCKS_PER_SEC;
}

int main(int argc, char *argv[]) {
  srand(time(NULL));
  
  if (argc != 3) {
    printf("give input file and sentence count\nexample: %s text.txt 5\n", argv[0]);
    return 1;
  }

  double parse_time = parse_text(argv[1]);
  printf("file \"%s\" parsed in %.2f seconds\n", argv[1], parse_time);
  printf("word_count: %i\n", word_count);
  printf("bucket_count: %i\n", bucket_count);

  int num_sentences = atoi(argv[2]);

  gen_probablity();

  possible_word* p = random_word();
  word* cur = find_or_add_word(p->str);

  bool s_stop = true; int s_total = 0; 
  int w_count = (rand() % MAX_PER_SENTENCE) + 3;
  while(s_total <= num_sentences) {
    if (!s_stop) w_count--;

    if (w_count <= 0) {
      s_stop = true;
      w_count = (rand() % MAX_PER_SENTENCE) + 3;
    }

    char* next = resolve_probablity(cur, s_stop);
    if (is_stop_word(next)) {
      s_stop = false;
      s_total++;
    }
    cur = find_or_add_word(next);

    printf("%s ", cur->str);
  }
  printf("\n");

  return 0;
}
