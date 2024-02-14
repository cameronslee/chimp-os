#include <string.h>

char *strcpy(char *dest, const char *src) {
  size_t i;
  size_t n = strlen(src);

  for (i = 0; i < n && src[i] != '\0'; i++) 
    dest[i] = src[i];
  for (; i < n; i++) 
    dest[i] = '\0';
  return dest;
}
