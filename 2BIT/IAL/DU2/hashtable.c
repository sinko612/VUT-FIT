/*
 * Tabuľka s rozptýlenými položkami
 *
 * S využitím dátových typov zo súboru hashtable.h a pripravených kostier
 * funkcií implementujte tabuľku s rozptýlenými položkami s explicitne
 * zreťazenými synonymami.
 *
 * Pri implementácii uvažujte veľkosť tabuľky HT_SIZE.
 */

#include "hashtable.h"
#include <stdlib.h>
#include <string.h>

int HT_SIZE = MAX_HT_SIZE;

/*
 * Rozptyľovacia funkcia ktorá pridelí zadanému kľúču index z intervalu
 * <0,HT_SIZE-1>. Ideálna rozptyľovacia funkcia by mala rozprestrieť kľúče
 * rovnomerne po všetkých indexoch. Zamyslite sa nad kvalitou zvolenej funkcie.
 */
int get_hash(char *key) {
  int result = 1;
  int length = strlen(key);
  for (int i = 0; i < length; i++) {
    result += key[i];
  }
  return (result % HT_SIZE);
}

/*
 * Inicializácia tabuľky — zavolá sa pred prvým použitím tabuľky.
 */
void ht_init(ht_table_t *table) {
  for(int i = 0; i< HT_SIZE; i++)
    //Priradil som kazdej bunke v tabulke NULL
    (*table)[i]=NULL;
  
}

/*
 * Vyhľadanie prvku v tabuľke.
 *
 * V prípade úspechu vráti ukazovateľ na nájdený prvok; v opačnom prípade vráti
 * hodnotu NULL.
 */
ht_item_t *ht_search(ht_table_t *table, char *key) {
  //Inicializoval som pomocne premenne
  //int hash bude znacit index pola na zaklade vratenej hodnoty z get_hash(key)
  int hash = get_hash(key);
  //Pomocny prvok
  ht_item_t *nextItem = (*table)[hash];

  //Prechadzanie synonym az kym nenarazim na dany kluc
  while(nextItem != NULL){
    if(!strcmp(nextItem->key, key))
        return nextItem;

    nextItem = nextItem->next;
  }
  
  return NULL;
}

/*
 * Vloženie nového prvku do tabuľky.
 *
 * Pokiaľ prvok s daným kľúčom už v tabuľke existuje, nahraďte jeho hodnotu.
 *
 * Pri implementácii využite funkciu ht_search. Pri vkladaní prvku do zoznamu
 * synonym zvoľte najefektívnejšiu možnosť a vložte prvok na začiatok zoznamu.
 */
void ht_insert(ht_table_t *table, char *key, float value) {
  //pomocne premenne
  ht_item_t *Item = ht_search(table, key);
  int hash = get_hash(key);

  //Ak sa Item nachadza v tabulke, prepiseme hodnotu danej bunky
  if(Item != NULL)
    Item->value = value;
  else{
    //Mallocovanie novej bunky a priradanie hodnot
    Item = malloc(sizeof(ht_item_t));
    Item->value = value;
    Item->key = key;

    //situacia, kedy vkladame uplne prvu polozku na dany index/hash->dany index nema zatial ziadne synonyma
    if((*table)[hash] == NULL){
      Item->next = NULL;
      (*table)[hash] = Item;
    }
    else{
      //situacia, kedy vkladame do zoznamu synonym, ktory uz mal predtym nejaky item
      Item->next = (*table)[hash];
      (*table)[hash] = Item;
    }
  }
}

/*
 * Získanie hodnoty z tabuľky.
 *
 * V prípade úspechu vráti funkcia ukazovateľ na hodnotu prvku, v opačnom
 * prípade hodnotu NULL.
 *
 * Pri implementácii využite funkciu ht_search.
 */
float *ht_get(ht_table_t *table, char *key) {
  ht_item_t *Item = ht_search(table, key);


  if(Item != NULL)
    return &(Item->value);

  return NULL;
}

/*
 * Zmazanie prvku z tabuľky.
 *
 * Funkcia korektne uvoľní všetky alokované zdroje priradené k danému prvku.
 * Pokiaľ prvok neexistuje, nerobte nič.
 *
 * Pri implementácii NEVYUŽÍVAJTE funkciu ht_search.
 */
void ht_delete(ht_table_t *table, char *key) {
  int hash = get_hash(key);
  ht_item_t *previousItem = (*table)[hash];
  ht_item_t *currentItem = (*table)[hash];

  //Sem vstupujeme, iba ak je na danom indexe nejaky prvok, inak nemame co mazat
  if((*table)[hash] != NULL){
    //situacia, pri ktorej mazeme uplne prvy item v zozname synonym
    if(!strcmp(currentItem->key, key)){
        (*table)[hash] = currentItem->next;
        free(currentItem);
    }

    currentItem = currentItem->next;
    //Mazanie dalsich prvkov v zozname synonym ,ktorych index >0, indexujeme od 0
    while(currentItem != NULL){
      if(!strcmp(currentItem->key, key)){
        previousItem->next = currentItem->next;
        free(currentItem);
        currentItem = previousItem->next;
      }
      else{

        previousItem = previousItem->next;
        currentItem = currentItem->next;
      }
    }
  }


}

/*
 * Zmazanie všetkých prvkov z tabuľky.
 *
 * Funkcia korektne uvoľní všetky alokované zdroje a uvedie tabuľku do stavu po
 * inicializácii.
 */
void ht_delete_all(ht_table_t *table) {
  ht_item_t *currentItem = NULL;
  //Prejdeme kazdu bunku v tabulke 
  for(int i = 0; i< HT_SIZE; i++){
    //Prechadzanie synonym daneho indexu
    while ((*table)[i] !=NULL){
      currentItem = (*table)[i];
      (*table)[i] = (*table)[i]->next;
      free(currentItem);
    }
  }


}
