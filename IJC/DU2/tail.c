// tail.c
// Řešení IJC-DU2, příklad a), 16.4.2022
// Autor: Simona Jánošíková, FIT
// Přeloženo: gcc (server merlin) 19.4.

/*
!!!!! ak mam obmedzenie na dlzku riadku a dany riadok ho presiahne, tak sa ukladaju riadky v divnom/zlom poradi, 
      neprepisuju sa, neprisla som na to, preco sa tak program chova, ak by ste na to prisli, bola by som velmi rada
      za spatnu vazbu, dakujem
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_CHAR_IN_LINE 4095

int main(int argc, char *argv[]) 
{
    long int n = 10; /* pocet poslednych riadkov, ktore sa maju vypisat */
    FILE *file = NULL; /* premenna pre nacitanie a pracovanie so suborom */

    /* 4 pripady nacitania argumentov:
        1.  tail                    => citame z stdin,  vypis 10 riadkov
        2.  tail <subor             => citame subor,    vypis 10 riadkov
        3.  tail -n CISLO           => citame z stdin,  vyppis -n riadkov
        4.  tail -n CISLO subor     => citame subor,    vypis -n riadkov
    co znamena, ze viac ako 4 argumenty nemozem mat */
    if (argc > 4)
    {
        fprintf(stderr,"Nespravny pocet argumentov!\n");
    }

    /* cyklus na nacitanie argumentov */    
    for (int i = 1; i < argc; i++)
    {
        /* ak sa vyskytuje -n tak sa pokusi nacitat cislo */
        if ((strcmp("-n", argv[i]) == 0) && (i+1 < argc))
        {
            long int pom_n = strtol(argv[i+1],NULL,10);
            if ((pom_n <=0) || (pom_n % 1 != 0))
            {
                fprintf(stderr,"Nespravny argument -n!\n");
                exit(1);
            }
            n = pom_n;
            i++;
        }
        else 
        {
            /* ak je subor prazdny, tak sa pokusi otvorit  */
            if (file == NULL)
            {
                file = fopen(argv[i],"r");
                if (file == NULL)
                {
                    fprintf(stderr,"Nepodarilo sa otvorit subor!\n");
                    exit(1);
                }
            }
            /* ak je subor uz prazdny nie je, tak sa jedna o duplicitu suborov  */
            else
            {
                fprintf(stderr,"Viacnasobne zadany argument mozneho suboru\n");
                exit(1);
            }    
        } 
    }

    /* ak sa subor nenasiel, tak sa spracovava stdin  */
    if(file == NULL)
    {
        file = stdin;
    }
    
    /* alokacia pola n-teho poctu riadkov  */
    char **line_array=(char**)malloc((sizeof(char*))*n);
    if (line_array == NULL)
    {
        free(line_array);
        fprintf(stderr,"Chyba pri alokacii pola line_array!\n");
        fclose(file);
        exit(1);
    }
    else
    {
        for(int i = 0; i < n; i++)
        {
            /*ak sa pole alokuje spravne, tak sa kazda jeho polozka alokuje na 4095+2 znakov (max. pocet znakov v riadku + 2 na '\0' a '\n')*/
            line_array[i] = malloc((sizeof(char))*(MAX_CHAR_IN_LINE+2));
            if(line_array[i] == NULL)
            {
                for(int j = 0; j < i; j++)
                {
                    free(line_array[j]);
                }
                free(line_array);
                fclose(file);
                fprintf(stderr,"Chyba pri alokacii polozky line_array!\n");
                exit(1);
            }
        }
    }

    long int riadok = 0; /* pocet riadkov line_array */
    long int pocitadlo = 0; /* premenna, pomocou ktorej zistujem, kde mam v line_array zaciatok vypisu a kde koniec vypisu */
    bool length_error = 0; /* premenna, pomocou ktorej detekujem, ci nastalo prekrocenie dlzky riadku */

    while( fgets(line_array[riadok],MAX_CHAR_IN_LINE+1,file) != NULL)
    {        
        /* overenie, ci je nacitany riadok rovnakej dlzky ako max. pocet znakov v riadku a ak je, tak ci je posledny znal '\n' */
        if(strlen(line_array[riadok]) == MAX_CHAR_IN_LINE && line_array[riadok][MAX_CHAR_IN_LINE+1] != '\n' )
        {
            if(length_error == 0)
            {
                fprintf(stdout,"Prekrocena dlzka retazca!\n");
                length_error = 1;
            }
            while(getc(file) != '\n' && getc(file) != EOF);
        } 

        riadok = riadok + 1;  
        pocitadlo = pocitadlo + 1;
        
        /* ak sa prekroci hranica poctu riadkov, tak sa zapisuje na zaciatok */
        if(riadok >= n)
            {              
                riadok = 0;
                pocitadlo = 0;
            }
    }

    long int i = 0;
    
    /* vypis poslednych n znakov, pomocou pocitadla urcujem, kde je zaciatok a kde koniec */
    if (pocitadlo == 0)
    {
        for(i = 0; i < n; i++)
        {
            fprintf(stdout,"%s",line_array[i]);
            if(length_error == 1)
            {
                fprintf(stdout,"\n");
            }
        }
        if(length_error == 0)
            fprintf(stdout,"\n");
    }
    else
    {
        for(i = pocitadlo; i < n; i++)
        {
            fprintf(stdout,"%s",line_array[i]);
            if(length_error == 1)
            {
                fprintf(stdout,"\n");
            }

            if( i == n-1 )
            {
                i = 0;
                i--;
            }
        
            if (i == pocitadlo-1 )
            {
                break;
            }
        }
        if(length_error == 0)
            fprintf(stdout,"\n");
    }
    
    /* uvolnenie pamati line_array */
    for(int i = 0; i < n; i++)
    {
        free(line_array[i]);
    }
    free(line_array);

    /* uzatvorenie suboru */
    fclose(file);

    exit(0); 
} 