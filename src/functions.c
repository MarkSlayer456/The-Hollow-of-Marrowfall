#include <string.h>
#include <stdlib.h>

int compare2(int a, int b) 
{
	return (a < b) ? a : b;
}

int compare4(int a, int b, int c, int d) 
{
	return compare2(compare2(a, b), compare2(c, d));
}

/**
 * creates a truly unique number from an x,y pair
 **/
unsigned int cantor_pair(int x, int y) {
	return (x + y) * (x + y + 1) / 2 + y;
}

/**
 * cross platform rand_r function
 **/
int rand_r_portable(unsigned int *seed) {
	*seed = *seed * 1103515245 + 12345;
	return (unsigned int)(*seed / 65536) % 32768;
}

char *my_strdup(char *str) {
	if(!str) return NULL;
	int size = strlen(str);
	char *new = calloc(size+1, sizeof(char));
	strcpy(new, str);
	return new;
}

// this function fails if there isn't space
void strcat_front(char *dst, char *prefix, int dst_capacity) {
	int dst_len = strlen(dst);
	int prefix_len = strlen(prefix);

	if(dst_len + prefix_len + 1 < dst_capacity) {
		memmove(dst + prefix_len, dst, dst_len+1);
		memcpy(dst, prefix, prefix_len);
	}
}
