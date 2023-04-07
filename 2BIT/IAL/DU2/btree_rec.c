/*
 * Binárny vyhľadávací strom — rekurzívna varianta
 *
 * S využitím dátových typov zo súboru btree.h a pripravených kostier funkcií
 * implementujte binárny vyhľadávací strom pomocou rekurzie.
 */

#include "../btree.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Inicializácia stromu.
 *
 * Užívateľ musí zaistiť, že incializácia sa nebude opakovane volať nad
 * inicializovaným stromom. V opačnom prípade môže dôjsť k úniku pamäte (memory
 * leak). Keďže neinicializovaný ukazovateľ má nedefinovanú hodnotu, nie je
 * možné toto detegovať vo funkcii.
 */
void bst_init(bst_node_t **tree) {  
  *tree = NULL;
}

/*
 * Nájdenie uzlu v strome.
 *
 * V prípade úspechu vráti funkcia hodnotu true a do premennej value zapíše
 * hodnotu daného uzlu. V opačnom prípade funckia vráti hodnotu false a premenná
 * value ostáva nezmenená.
 *
 * Funkciu implementujte rekurzívne bez použitia vlastných pomocných funkcií.
 */
bool bst_search(bst_node_t *tree, char key, int *value) {

  if(tree == NULL)
    return false;
  else
  {
    //Ak najdeme dany uzol na zaklade kluca
    if(tree->key == key){
      *value = tree->value;
      return true;
    }
    else if( key < tree->key)
      //Ak nenajdeme, rekurzivne sa zanorime na zaklade podmienky do laveho podstromu
      return (bst_search(tree->left, key, value));
    
    else
      //ak nenajdeme, rekurzivne sa zanorime na zaklade podmienky do praveho podstromu
      return (bst_search(tree->right, key, value));
  }


  return false;
}

/*
 * Vloženie uzlu do stromu.
 *
 * Pokiaľ uzol so zadaným kľúčom v strome už existuje, nahraďte jeho hodnotu.
 * Inak vložte nový listový uzol.
 *
 * Výsledný strom musí spĺňať podmienku vyhľadávacieho stromu — ľavý podstrom
 * uzlu obsahuje iba menšie kľúče, pravý väčšie.
 *
 * Funkciu implementujte rekurzívne bez použitia vlastných pomocných funkcií.
 */
void bst_insert(bst_node_t **tree, char key, int value) {
  //Vytvorenie noveho uzlu, ktory budeme vkladat

  

  if(*tree == NULL){
    bst_node_t *Item = malloc(sizeof(bst_node_t));

    //Naplnenie uzlu hodnotami
    Item->key = key;
    Item->value = value;
    Item->left= NULL;
    Item->right = NULL;
    *tree = Item;
  }
  else{
    //rekurzivne zanorenia na zaklade podmienky
    if( key < (*tree)->key)
      bst_insert(&(*tree)->left, key, value);

    else if(key > (*tree)->key) 
      bst_insert(&(*tree)->right, key, value);
    
    else  
      (*tree)->value = value; 
  }

}

/*
 * Pomocná funkcia ktorá nahradí uzol najpravejším potomkom.
 *
 * Kľúč a hodnota uzlu target budú nahradené kľúčom a hodnotou najpravejšieho
 * uzlu podstromu tree. Najpravejší potomok bude odstránený. Funkcia korektne
 * uvoľní všetky alokované zdroje odstráneného uzlu.
 *
 * Funkcia predpokladá že hodnota tree nie je NULL.
 *
 * Táto pomocná funkcia bude využitá pri implementácii funkcie bst_delete.
 *
 * Funkciu implementujte rekurzívne bez použitia vlastných pomocných funkcií.
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree) {

  int help = 0;
  bst_node_t *childOfRightMost = NULL;

  //Prelinkovanie right_child na parenta most_right
  if((*tree)->right != NULL){
    if((*tree)->right->right == NULL){
      childOfRightMost = (*tree)->right->left;
      help = 1;

    }
  }

  if((*tree)->right == NULL){
    //som na najpravejsom prvku, predavam hodnoty a mazem  
    target->key = (*tree)->key;
    target->value = (*tree)->value;

    free(*tree);
    *tree = NULL;

  }
  else{
    //rekurzivne zanorenie
    bst_replace_by_rightmost(target, &(*tree)->right);
    //Prelinkovanie right_child na parenta most_right
    if(help == 1)
      (*tree)->right = childOfRightMost;

  }

}


/*
 * Odstránenie uzlu v strome.
 *
 * Pokiaľ uzol so zadaným kľúčom neexistuje, funkcia nič nerobí.
 * Pokiaľ má odstránený uzol jeden podstrom, zdedí ho otec odstráneného uzla.
 * Pokiaľ má odstránený uzol oba podstromy, je nahradený najpravejším uzlom
 * ľavého podstromu. Najpravejší uzol nemusí byť listom!
 * Funkcia korektne uvoľní všetky alokované zdroje odstráneného uzlu.
 *
 * Funkciu implementujte rekurzívne pomocou bst_replace_by_rightmost a bez
 * použitia vlastných pomocných funkcií.
 */
void bst_delete(bst_node_t **tree, char key) {
  //help_var je len pomocna premenna, do ktorej musim ulozit vysledok funkcie bst_search, je to celkom nasilne riesenie ale ucinne :D
  int help_var = 0;
  int *value = &help_var;
  help_var = help_var+1;
  
  if(*tree == NULL)
    return;

  if(bst_search(*tree, key, value)){
    //Ak sa key nachadza v strome, tak ho hladam       

    if(key < (*tree)->key){
    //ak je kluc mensi ako kluc tree tak idem do lava
      bst_delete(&(*tree)->left, key);    
    }
    
    
    else if(key > (*tree)->key){
    //ak je kluc vacsi ako kluc tree tak idem do prava
      bst_delete(&(*tree)->right, key);
    }
    
    else{
      //Ak sa kluce rovnaju
      if((*tree)->right == NULL && (*tree)->left == NULL){
        //Nema ziadneho potomka
        free(*tree);
        *tree = NULL;
      }

      else if((*tree)->right != NULL && (*tree)->left != NULL){
        //Ma dvoch potomkov
        bst_replace_by_rightmost(*tree, &(*tree)->left);
      }
      else{
        //Ma jedneho potomka
        bst_node_t *help = NULL;
        if((*tree)->right != NULL)
          help = (*tree)->right;
        else
          help = (*tree)->left;

        free(*tree);
        //Predanie potomka odstraneneho uzlu k jeho rodicovi
        *tree = help;
      }
    }
  }  
}

/*
 * Zrušenie celého stromu.
 *
 * Po zrušení sa celý strom bude nachádzať v rovnakom stave ako po
 * inicializácii. Funkcia korektne uvoľní všetky alokované zdroje rušených
 * uzlov.
 *
 * Funkciu implementujte rekurzívne bez použitia vlastných pomocných funkcií.
 */
void bst_dispose(bst_node_t **tree) {

  //test, ci je strom prazdny alebo nie
  if(*(tree) !=NULL){
    //rekurzivne zanorenie ak existuje iba pravy podstrom
    if((*tree)->left == NULL && (*tree)->right != NULL) {
        bst_dispose(&(*tree)->right);
        (*tree)->right= NULL;
    }
    //rekurzivne zanorenie ak existuje iba lavy podstrom
    if((*tree)->left != NULL && (*tree)->right == NULL){
        bst_dispose(&(*tree)->left);
        (*tree)->left = NULL;
    }
    //rekurzivne zanorenie ak existuju oba podstromy
    if((*tree)->left != NULL && (*tree)->right != NULL){
        bst_dispose(&(*tree)->left);
        bst_dispose(&(*tree)->right);
        (*tree)->left = NULL;
        (*tree)->right= NULL;
    }
    //uvolnenie daneho uzlu, pretoze sme narazili na koncivy list
    if((*tree)->left == NULL && (*tree)->right == NULL){
      free(*tree);
      (*tree) = NULL;

    }
  }
}

/*
 * Preorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte rekurzívne bez použitia vlastných pomocných funkcií.
 */
void bst_preorder(bst_node_t *tree) {
  if(tree != NULL){
    bst_print_node(tree);
    bst_preorder(tree->left);
    bst_preorder(tree->right);
  }
}

/*
 * Inorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte rekurzívne bez použitia vlastných pomocných funkcií.
 */
void bst_inorder(bst_node_t *tree) {
  if(tree != NULL){
    bst_inorder(tree->left);
    bst_print_node(tree);
    bst_inorder(tree->right);
  }
}
/*
 * Postorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte rekurzívne bez použitia vlastných pomocných funkcií.
 */
void bst_postorder(bst_node_t *tree) {
  if(tree != NULL){
    bst_postorder(tree->left);
    bst_postorder(tree->right);
    bst_print_node(tree);
  }
}
