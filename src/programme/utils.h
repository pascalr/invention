#ifndef UTILS_H
#define UTILS_H

static inline void rtrim(char* str) {
  int len = strlen(str);

  if (len == 0) return;

  if (str[len-1] == '\n') {
    str[len-1] = '\0';
  } else if (str[len-1] == ' ') {
    str[len-1] = '\0';
  } else if (str[len-1] == '\r') {
    str[len-1] = '\0';
  } else {
    return;
  }
  rtrim(str);
}

#endif
