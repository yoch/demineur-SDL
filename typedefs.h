#ifndef _TYPEDEFS_H
#define _TYPEDEFS_H

#include <stdbool.h>


typedef enum
{ discovered = 0, unkown = 1, warning = 2, flagged = 3 }
state;

typedef struct
{
    state state;        /* etat de la case*/
    unsigned neighbors; /* nombre de mines au voisinage */
    bool bomb;          /* la case est mminee */
} box;


#endif
