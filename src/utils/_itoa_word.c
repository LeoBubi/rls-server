#include "includes.h"


extern const char _itoa_upper_digits[];
extern const char _itoa_lower_digits[];


char*
_itoa_word (_ITOA_WORD_TYPE value, char *buflim, unsigned int base, int upper_case)
{
    const char *digits = (upper_case
			? _itoa_upper_digits
			: _itoa_lower_digits);

#define SPECIAL(Base)			            \
    case Base:						        \
        do							        \
	*--buflim = digits[value % Base];       \
        while ((value /= Base) != 0);       \
        break
    
    switch (base)
    {
        SPECIAL(10);
        SPECIAL(16);
        SPECIAL(8);

        default:
            do
        *--buflim = digits[value % base];
            while ((value /= base) != 0);
    }

#undef SPECIAL

    return buflim;
}
