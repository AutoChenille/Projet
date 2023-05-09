#include "pretreatment.h"

int str_to_int(char *str)
{
  int res = 0;
  int sign = 1;

  if (str[0] == '-')
  {
      sign = -1;
      str++;
  }

  while (*str != '\0')
  {
      res = res * 10 + *str - 48;
      str++;
  }

  return res * sign;
}
