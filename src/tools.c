#include "tools.h"

void itoa(long nb, char *str)
{
    char sign = nb < 0;
    unsigned long cp = (sign) ? -nb : nb;
    unsigned len = 0;

    do
    {
        ++len;
        cp /= 10;
    }
    while (cp);
    str[len + sign] = 0;
    if (sign)
        str[0] = '-';

    cp = (sign) ? -nb : nb;
    for (unsigned i = len; i > 0; --i)
    {
        str[i - 1 + sign] = cp % 10 + '0';
        cp /= 10;
    }
}
