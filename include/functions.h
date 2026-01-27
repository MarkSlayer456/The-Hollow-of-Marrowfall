#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_
#define MIN(a,b)       (a < b) ? a : b
#define MAX(a,b)       (a > b) ? a : b

int compare4(int a, int b, int c, int d);
unsigned int cantor_pair(int x, int y);
int rand_r_portable(unsigned int *seed);

char *my_strdup(char *str);

void strcat_front(char *dst, char *prefix, int dst_capacity);
#endif
