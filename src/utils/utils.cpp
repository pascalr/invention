#include <string.h>
#include <math.h>

unsigned long timeDifference(unsigned long oldTime, unsigned long currentTime) {
  // Check if the time has wrapped around.
  if (currentTime < oldTime) {
    unsigned long maxLong = 0; maxLong--;
    return maxLong - oldTime + currentTime;
  }
  return currentTime - oldTime;
}

void rtrim(char* str) {
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
