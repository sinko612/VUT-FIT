//======== Copyright (c) 2022, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Test Driven Development - priority queue code
//
// $NoKeywords: $ivs_project_1 $tdd_code.cpp
// $Author:     SIMONA JÁNOŠÍKOVÁ <xjanos19@stud.fit.vutbr.cz>
// $Date:       $2022-01-03
//============================================================================//
/**
 * @file tdd_code.cpp
 * @author SIMONA JÁNOŠÍKOVÁ
 * 
 * @brief Implementace metod tridy prioritni fronty.
 */

#include <stdlib.h>
#include <stdio.h>

#include "tdd_code.h"

//============================================================================//
// ** ZDE DOPLNTE IMPLEMENTACI **
//
// Zde doplnte implementaci verejneho rozhrani prioritni fronty (Priority Queue)
// 1. Verejne rozhrani fronty specifikovane v: tdd_code.h (sekce "public:")
//    - Konstruktor (PriorityQueue()), Destruktor (~PriorityQueue())
//    - Metody Insert/Remove/Find/GetHead ...
//    - Pripadne vase metody definovane v tdd_code.h (sekce "protected:")
//
// Cilem je dosahnout plne funkcni implementace prioritni fronty implementovane
// pomoci tzv. "singly linked list", ktera bude splnovat dodane testy
// (tdd_tests.cpp).
//============================================================================//

PriorityQueue::PriorityQueue()
{
    //konstruktor, fronta je prazdna, takza sa hlavicka nadstavi na NULL
    m_pHead = NULL;
}

PriorityQueue::~PriorityQueue()
{
    //destruktor, prvky fronty sa jeden ppo druhom postupne vymazu
    PriorityQueue::Element_t *prvok = GetHead();
    while (prvok != NULL)
    {
        PriorityQueue::Element_t *vymaz = prvok;
        prvok = prvok->pNext;
        delete vymaz;
    }
}

void PriorityQueue::Insert(int value)
{    
    if (m_pHead == NULL)
    {
        //ak je fronta prazdna, novy prvok sa ulozi na miesto hlavicky
        PriorityQueue::Element_t *prvok0 = new PriorityQueue::Element_t();
        m_pHead = prvok0;
        prvok0->value = value;
        prvok0->pNext = NULL;
        return;
    }

    for(PriorityQueue::Element_t *prechod = GetHead(); prechod != NULL; prechod = prechod->pNext)
    {
        //kedze som uz prazdnu frontu overila, tak uz zostavaju iba 2 moznosti
        if(value >= prechod->value)
        {
            //novy prvok bude mat vacsiu hodnotu ako ten predtym, cize sa ulozi predneho, nemusim riesit ci pNext ukazuje na NULL
            PriorityQueue::Element_t *new_prvok = new PriorityQueue::Element_t;
            //novemu prvku nadstavim hodnoty prechodoveho aby som nestratila ukazatel
            new_prvok->value = prechod->value;
            new_prvok->pNext = prechod->pNext;
            //prechodu nadstavim nove hodnoty
            prechod->value = value;
            prechod->pNext = new_prvok;   
            return;         
        }
        else if(prechod->pNext == NULL)
        {
            /*novy prvok bude mat mensiu hodnotu ako ten predtym, tato situacia nastane iba ak je prvok najmensi, cize sa musi pripisat na koniec a pNext ukazovat na NULL */
            PriorityQueue::Element_t *new_prvok1 = new PriorityQueue::Element_t;
            new_prvok1->value = value;
            new_prvok1->pNext = NULL;  
            prechod->pNext = new_prvok1; 
            return;
        }
    }         
}

bool PriorityQueue::Remove(int value)
{
    PriorityQueue::Element_t *rmv = m_pHead;
    PriorityQueue::Element_t *predchadzajuci = NULL;
    
    while (rmv != NULL && value != rmv->value)
    {
        //prechadzam frontu a hladam prvok s hodnoutou ktoru chcem vymazat
        predchadzajuci = rmv;
        rmv = rmv->pNext;
    }

    if(rmv == NULL)
    {
        //ak sa prvok nenasiel, tak false, nemam co vymazat
        return false;
    }
    //prvok sa nasiel, takze:
    else if (rmv == m_pHead)
    {
        //vymazava sa hlavicka, hlavicka musi nadobudnut nove hodnoty
        m_pHead = rmv->pNext;
        delete(rmv);
        return true;
    }
    else
    {
        //vymazava sa prvok niekde v strede fronty
        predchadzajuci->pNext = rmv->pNext;
        delete(rmv);
        return true;
    }
}

PriorityQueue::Element_t *PriorityQueue::Find(int value)
{
    for(Element_t *find = GetHead(); find != NULL; find = find->pNext)
    {
        //hladam prvok s hodnotou value, ak sa najde tak return ukazatel na prvok (= find), ak nie, tak return false
        if(find->value == value)
        {
            return find;
        }
    }
    return NULL;
}

size_t PriorityQueue::Length()
{
    int pocetPrvkov = 0;
    for(Element_t *prechod2 = GetHead(); prechod2 != NULL; prechod2 = prechod2->pNext)
    {
        //pokial pNext neukazuje na NULL tak sa pocitaju prvky fronty 
        pocetPrvkov++;
    }
    return pocetPrvkov;
}

PriorityQueue::Element_t *PriorityQueue::GetHead()
{
    //vracia ukazatel na hlavicku
    return m_pHead;
}

/*** Konec souboru tdd_code.cpp ***/
