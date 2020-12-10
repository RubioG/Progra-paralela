/* `levenshtein.c` - levenshtein
 * MIT licensed.
 * Copyright (c) 2015 Titus Wormer <tituswormer@gmail.com> */

/* Returns an size_t, depicting
 * the difference between `a` and `b`.
 * See https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance
 * for more information. */

#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

int levenshtein(char *s1, char *s2) {
    unsigned int s1len, s2len, x, y, lastdiag, olddiag;
    s1len = strlen(s1);
    s2len = strlen(s2);
    int * column = calloc(s1len+1, sizeof(int));
    for (y = 1; y <= s1len; y++)
        column[y] = y;
    for (x = 1; x <= s2len; x++) {
        column[0] = x;
        for (y = 1, lastdiag = x-1; y <= s1len; y++) {
            olddiag = column[y];
            column[y] = MIN3(column[y] + 1, column[y-1] + 1, lastdiag + (s1[y-1] == s2[x-1] ? 0 : 1));
            lastdiag = olddiag;
        }
    }
    int result = (column[s1len]);
    free(column);
    return result;
}
