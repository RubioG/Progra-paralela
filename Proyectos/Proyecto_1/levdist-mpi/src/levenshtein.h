#ifndef LEVENSHTEIN_H
#define LEVENSHTEIN_H
//#include <pthread.h>

/**
 * @brief calculate_levdist Calculate the distance(differents) between two strings.
 *
 * Algorithm taken from: wikibooks.org :
 * Taken for https://ieeexplore.ieee.org/document/6665373
 * @param first_string The first string to be compared.
 * @param second_string The second string to be compared.
 * @param count of workers (threads)
 * @return The result(Distance) between the strings.
 */
size_t distance_levdist(unsigned char *s1,unsigned char *s2, int worker_count);

/**
 * @brief MIN3 used in (int calculate_levdist(char* s1, char* s2))
 */
size_t MIN3(size_t a, size_t b, size_t c);


#endif // LEVENSHTEIN_H
