#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <SDL/SDL.h>

#include "typedefs.h"
extern box** box_array;

extern unsigned nbBoxW;
extern unsigned nbBoxH;
extern unsigned nbMines;



#define DOUBLE_CLICK_MAX_DELAY  250
int DoubleClickDetected (void)
{
    static Uint32 LastClickTicks = 0;
    Uint32 CurrentClickTicks;
    /* The first time this function is called, LastClickTicks has not been initialised yet. */
    if (! LastClickTicks)
    {
        LastClickTicks = SDL_GetTicks ();
        return 0;
    }
    else
    {
        CurrentClickTicks = SDL_GetTicks ();
        /* If the period between the two clicks is smaller or equal to a pre-defined number, we report a DoubleClick event. */
        if (CurrentClickTicks - LastClickTicks <= DOUBLE_CLICK_MAX_DELAY)
        {
            /* Update LastClickTicks and signal a DoubleClick. */
            LastClickTicks = CurrentClickTicks;
            return 1;
        }
        else
        {
            /* Update LastClickTicks and signal a SingleClick. */
            LastClickTicks = CurrentClickTicks;
            return 0;
        }
    }
}

int traitement_clic_gauche (int i, int j)
{
    int _i, _j;

    if ( box_array[i][j].state == unkown )
    {
        if (box_array[i][j].bomb) /* s'il y a une bombe - BOUM ! */
            return 1;
        else /* sinon, on découvre la case */
        {
            box_array[i][j].state = discovered;
            if (box_array[i][j].neighbors == 0) /* si la case est vide (aucun voisin), on découvre les cases adjacentes */
            {
                for (_i=i-1; _i<=i+1; _i++)
                {
                    for (_j=j-1; _j<=j+1; _j++)
                    {
                        if (_i >= 0 && _i < (int)nbBoxW && _j >= 0 && _j < (int)nbBoxH && box_array[_i][_j].state == unkown)
                            traitement_clic_gauche (_i, _j);
                    }
                }
            }
        }
    }
    return 0;
}

int traitement_double_clic (int i, int j)
{
    int _i, _j;
    unsigned count = 0;
    if ( box_array[i][j].state == discovered )
    {
        /* compte le nombre de bombes voisines trouvées */
        for (_i=i-1; _i<=i+1; _i++)
        {
            for (_j=j-1; _j<=j+1; _j++)
            {
                if (_i >= 0 && _i < (int)nbBoxW && _j >= 0 && _j < (int)nbBoxH)
                    count += ( box_array[_i][_j].state == flagged );
            }
        }
        /* si toutes les bombes sont marquées, on découvre les autres cases voisines non marquées */
        if (count == box_array[i][j].neighbors)
        {
            for (_i=i-1; _i<=i+1; _i++)
            {
                for (_j=j-1; _j<=j+1; _j++)
                {
                    if (_i >= 0 && _i < (int)nbBoxW && _j >= 0 && _j < (int)nbBoxH && box_array[_i][_j].state != flagged)
                    {
                        if ( traitement_clic_gauche (_i, _j) ) /* s'il y avait une bombe (marquage erroné) - BOUM ! */
                            return 1;
                    }
                }
            }
        }
    }
    return 0;
}

void traitement_clic_droit (int i, int j)
{
    if (box_array[i][j].state == unkown)
    {
        box_array[i][j].state = flagged;
        nbMines--;
    }
    else if (box_array[i][j].state == flagged)
    {
        box_array[i][j].state = warning;
        nbMines++;
    }
    else if (box_array[i][j].state == warning)
        box_array[i][j].state = unkown;
}
