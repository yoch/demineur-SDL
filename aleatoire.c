#include <stdlib.h>

/***************************************
Generateur de nombres aleatoires uniques
***************************************/

/* Va. globales */
static int nmin, intervalle, pos;
static int* tab;


int InitGenerateur (int min, int max)
{
    int i;
    nmin = min, intervalle = max-min+1;
    pos = 0;
    tab = malloc ( intervalle * sizeof(int) );
    if (tab != NULL)
    {
        /* on remplit le tableau avec des nombres entre min et max */
        for (i=min; i<=max; i++)
        {
            tab[i-min] = i;
        }
        return 1;
    }
    else return 0;
}

/**
retourne a chaque fois un tirage aleatoire
dans un tableau contenant les nombres entre min et max,
puis permute ce nombre avec le debut du tableau et avance dans le tableau.
Si on arrive a la fin, on recommence
**/
int ReturnAleatoire (void)
{
    int n, tmp;
    /* si on est a la fin du tableau */
    if (pos == intervalle)
        pos = 0;
    /* tire un nombre entre pos et l'indice de fin du tableau */
    n = pos + ( rand() % (intervalle-pos) );
    tmp = tab[n];
    tab[n] = tab[pos];
    tab[pos] = tmp;
    pos++;
    return tmp;

}

void TermineGenerateur (void)
{
    if (tab != NULL)
    {
        free (tab), tab = NULL;
    }
}
