#include <stdlib.h>

#include "typedefs.h"


box** init_matrix (unsigned y, unsigned x)
{
    unsigned i, j;
    box** mat = malloc (y * sizeof *mat);   /* index (1ere dimension) */
    if ( mat != NULL )
    {
        *mat = malloc(y * x * sizeof **mat); /* données (2eme dimension) */
        if (*mat != NULL)
        {
            for (i=0,j=0; i<y*x; i+=x,j++)
                mat[j] = (*mat)+i;
        }
    }
    return mat;
}

void delete_matrix (box*** mat)
{
    if (*mat != NULL)
    {
        if (**mat != NULL)
            free(**mat), **mat = NULL;
        free(*mat), *mat = NULL;
    }
}
