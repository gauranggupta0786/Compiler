/*
  Group Number: 22
  NAME                 : ID
  Archit Garg          : 2022B2A71602PP 
  Aadit Shah           : 2022B3A70612P 
  Rishant Sharma       : 2022B5A70814P 
  Rishab Chetal        : 2023A7PS0015P 
  Gaurang Gupta        : 2023A7PS0648P
  Ansh Bansal          : 2023A7PS0645P 
*/
#include "string.h"

/* Compare two null-terminated strings
 * Returns 1 if equal, 0 otherwise */
int compareStrings(char *firstStr, char *secondStr)
{
    int idx;
    
    /* Iterate through both strings simultaneously */
    for (idx = 0; ; idx++) {
        char ch1 = firstStr[idx];
        char ch2 = secondStr[idx];
        
        /* Check if we've reached the end of both strings */
        if (ch1 == '\0' && ch2 == '\0') {
            return 1;
        }
        
        /* Check if only one string ended or characters differ */
        if (ch1 != ch2) {
            return 0;
        }
    }
}