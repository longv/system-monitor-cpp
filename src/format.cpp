#include <string>

#include "format.h"

using std::string;

string Format::ElapsedTime(long seconds) { 
  int h = seconds / 3600;
  int m = (seconds - (h * 3600)) / 60;
  int s = seconds - h * 3600 - m * 60;

  char formattedTime[100];
  sprintf(formattedTime, "%02d:%02d:%02d", h, m, s);
  
  return string(formattedTime);
}
  