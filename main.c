#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <getopt.h>
#include <SDL/SDL.h>


#include "typedefs.h"
#include "aleatoire.h"
#include "matrix.h"
#include "evenements.h"



#define UPDATE_TITLE_EVENT  0x01

/* VARIABLES GLOBALES */
SDL_Surface* tileset = NULL;
box** box_array = NULL;
unsigned nbBoxW = 30;
unsigned nbBoxH = 16;
unsigned nbMines = 99;




#define BOX_W   16
#define BOX_H   16

Uint32 timer_callback (Uint32 intervalle, void *param)
{
    unsigned* elapsed = param;
    SDL_Event event;
    event.type = SDL_USEREVENT;
    event.user.code = UPDATE_TITLE_EVENT;
    event.user.data1 = NULL;
    event.user.data2 = NULL;
    
   (*elapsed)++;
    SDL_PushEvent(&event);

    return intervalle;
}

void UpdateTitle (unsigned elapsed)
{
    static char title[256] = "";
    sprintf(title, "%u bombe(s) restante(s) - %.2u:%.2u:%.2u", nbMines, elapsed / 3600, (elapsed % 3600) / 60, elapsed % 60);
    SDL_WM_SetCaption(title, NULL);
}

void Display (void)
{
    unsigned i,j;
    SDL_Rect Rect_dest;
    SDL_Rect Rect_source;
    SDL_Surface* screen = SDL_GetVideoSurface();

    Rect_source.w = BOX_W;
    Rect_source.h = BOX_H;
    Rect_source.y = 0;
    for(i=0; i<nbBoxW; i++)
    {
        for(j=0; j<nbBoxH; j++)
        {
            Rect_dest.x = i*BOX_W;
            Rect_dest.y = j*BOX_H;

            if (box_array[i][j].state == discovered)
                Rect_source.x = box_array[i][j].neighbors * BOX_W;
            else
                Rect_source.x = (8 + box_array[i][j].state) * BOX_W;

            SDL_BlitSurface(tileset,&Rect_source,screen,&Rect_dest);
        }
    }

    SDL_Flip(screen);
}

/* fin du jeu, devoile les cases restantes en cas de perte */
void DisplayEnd (void)
{
    unsigned i,j;
    SDL_Rect Rect_dest;
    SDL_Rect Rect_source;
    SDL_Surface* screen = SDL_GetVideoSurface();

    Rect_source.w = BOX_W;
    Rect_source.h = BOX_H;
    Rect_source.y = 0;
    for(i=0; i<nbBoxW; i++)
    {
        for(j=0; j<nbBoxH; j++)
        {
            Rect_dest.x = i*BOX_W;
            Rect_dest.y = j*BOX_H;

            if (box_array[i][j].bomb && box_array[i][j].state != flagged)
                Rect_source.x = 12 * BOX_W;
            else if (box_array[i][j].state == flagged && !box_array[i][j].bomb)
                Rect_source.x = 13 * BOX_W;
            else if (box_array[i][j].state == discovered)
                Rect_source.x = box_array[i][j].neighbors * BOX_W;
            else
                Rect_source.x = (8 + box_array[i][j].state) * BOX_W;

            SDL_BlitSurface(tileset,&Rect_source,screen,&Rect_dest);
        }
    }

    SDL_Flip(screen);
}

/* incremente le nombre de bombes voisines de 1 autour de la bombe */
void calc_neighbors (int x, int y)
{
    int i,j;
    for (i=x-1; i<=x+1; i++)
    {
        for (j=y-1; j<=y+1; j++)
        {
            if ( i >= 0 && i < (int)nbBoxW && j >= 0 && j < (int)nbBoxH )
                box_array[i][j].neighbors++;
        }
    }
}

void Game_init (unsigned nbBombs)
{
    unsigned i,j,n;
    unsigned random;

    /* charge le tileset */
    tileset = SDL_LoadBMP("fond.bmp");
    if (tileset == NULL)
    {
        fputs("Impossible de charger le tileset", stderr);
        exit(1);
    }

    /* cree un tableau 2D alloue dynamiquement */
    box_array = init_matrix (nbBoxW, nbBoxH);
    if (box_array == NULL)
    {
        fputs("", stderr);
        exit(1);
    }

    /* initialise le tableau */
    for(i=0; i<nbBoxW; i++)
    {
        for(j=0; j<nbBoxH; j++)
        {
            box_array[i][j].state = unkown;
            box_array[i][j].neighbors = 0;
            box_array[i][j].bomb = 0;
        }
    }

    /* charge le générateur aléatoire sans répétitions */
    InitGenerateur(0, (nbBoxW*nbBoxH)-1);
    /* place aléatoirement les mines */
    for (n=0; n<nbBombs; n++)
    {
        random = ReturnAleatoire();
        i = random/nbBoxH, j = random%nbBoxH;
        box_array[i][j].bomb = 1;
        calc_neighbors (i, j); /* actualise les donnees sur les voisins */
    }
    /* termine le générateur aléatoire sans répétitions */
    TermineGenerateur();
}

void Game_end (void)
{
    if (tileset != NULL)
        SDL_FreeSurface(tileset);
    if (box_array != NULL)
        delete_matrix (&box_array);
    SDL_Quit();
}

int main ( int argc, char** argv )
{
    unsigned done = 0, boom = 0, elapsed = 0;
    int mouseX, mouseY;
    SDL_Event event;
    SDL_TimerID timer;
    char title[256] = "";

    /* recupere les arguments de la console */
    int c;
    while (1)
    {
        c = getopt(argc, argv, "w:h:m:");

        if (c == -1)
            break;

        switch (c)
        {
        case 'w':
            nbBoxW = strtol(optarg, NULL, 10);
            break;

        case 'h':
            nbBoxH = strtol(optarg, NULL, 10);
            break;

        case 'm':
            nbMines = strtol(optarg, NULL, 10);
            break;

        default:
            fprintf(stderr, "Usage: %s [-w width] [-h height] [-m nbmines]\n", argv[0]);
            exit(1);
        }
    }

    /* s'il y a des paramètres supplémentaires */
    if (optind < argc)
    {
        fprintf(stderr, "extra operand\n");
        exit(1);
    }

    /* initialise rand */
    srand(time(NULL));

    /*  initialise SDL video */
    if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) < 0 )
    {
        fprintf( stderr, "Unable to init SDL: %s\n", SDL_GetError() );
        return 1;
    }

    atexit( Game_end );
    Game_init( nbMines );

    SDL_SetVideoMode(nbBoxW * BOX_W, nbBoxH * BOX_H, 32, SDL_HWSURFACE);
    timer = SDL_AddTimer(1000, timer_callback, &elapsed);
    UpdateTitle (elapsed);

    while (!done)
    {
        if (!boom && nbMines > 0)  /* le jeu continue */
        {
            Display();
        }
        else  /* fin du jeu */
        {
            DisplayEnd();
            if (boom)
            {
                sprintf(title, "VOUS AVEZ PERDU ! - %u bombe(s) restante(s) - Temps ecoule : %.2u:%.2u:%.2u",
                        nbMines, elapsed / 3600, (elapsed % 3600) / 60, (elapsed % 60));
            }
            else
            {
                sprintf(title, "VOUS AVEZ GAGNE ! - Temps ecoule : %.2u:%.2u:%.2u",
                        elapsed / 3600, (elapsed % 3600) / 60, (elapsed % 60));
            }
            SDL_WM_SetCaption(title, NULL);
            SDL_RemoveTimer(timer);
        }

        SDL_WaitEvent(&event);
        switch (event.type)
        {
        case SDL_QUIT:
            done = 1;
            break;

        case SDL_MOUSEBUTTONUP: /* Clic de la souris */
            mouseX = event.button.x, mouseY = event.button.y;
            switch (event.button.button)
            {
            case SDL_BUTTON_LEFT:
                if (!boom)
                { 
                    boom = traitement_clic_gauche (mouseX/BOX_W, mouseY/BOX_H);
                    if ( !boom && DoubleClickDetected() )
                        boom = traitement_double_clic(mouseX/BOX_W, mouseY/BOX_H);
                }
                break;
            case SDL_BUTTON_RIGHT:
                if (!boom)
                {
                    traitement_clic_droit (mouseX/BOX_W, mouseY/BOX_H);
                }
                break;
            default:
                break;
            }
            break;

            /*  appui d'une touche */
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE: /*  Touche Echap */
                done = 1;
                break;

            default:
                break;
            } /* fin switch (event.key.keysym.sym) */
            break;

        case SDL_USEREVENT:
            if (event.user.code == UPDATE_TITLE_EVENT)
            {
                UpdateTitle (elapsed);
            }
            break;
            
        default:
            break;

        } /* fin switch (event.type) */
    } /* fin boucle evenementielle */

    if (!boom)
        SDL_RemoveTimer(timer);

    (void) argc, (void) argv;    /* prevent warning */

    return 0;
}
