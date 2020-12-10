#ifndef LEVENSHTEIN_H
#define LEVENSHTEIN_H

/* `levenshtein.h` - levenshtein
 * MIT licensed.
 * Copyright (c) 2015 Titus Wormer <tituswormer@gmail.com> */

/* Returns an size_t, depicting
 * the difference between `a` and `b`.
 * See https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance
 * for more information. */
#ifdef __cplusplus
extern "C" {
#endif

int levenshtein(char *s1, char *s2);

#ifdef __cplusplus
}
#endif

#endif // LEVENSHTEIN_H
