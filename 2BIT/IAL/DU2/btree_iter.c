/*
 * Binárny vyhľadávací strom — iteratívna varianta
 *
 * S využitím dátových typov zo súboru btree.h, zásobníkov zo súborov stack.h a
 * stack.c a pripravených kostier funkcií implementujte binárny vyhľadávací
 * strom bez použitia rekurzie.
 */

#include "../btree.h"
#include "stack.h"
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
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
bool bst_search(bst_node_t *tree, char key, int *value) {
  //Pomocny strom, ktory prechadzam, zachovam ukazatel na pociatocny strom
  bst_node_t * copyTree = tree;

  while(copyTree != NULL && copyTree->key != key){
    if(key < copyTree->key)
      //Ak je kluc mensi ako kluc uzlu, tak ideme do lava
      copyTree = copyTree->left;
    else if(key> copyTree->key)
     //ak je kluc vacsi ako kluc uzlo, ideme do prava
     copyTree = copyTree->right;
    else{
      //Ak sa kluce rovnaju
      *value = copyTree->value;
      return true;
    }

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
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
void bst_insert(bst_node_t **tree, char key, int value) {


  bst_node_t** copyTree = tree;
  //Prechadzanie kym nenajdeme miesto podla kluca
  while(*copyTree != NULL && (*copyTree)->key != key){
    if(key < (*copyTree)->key)
      copyTree = &(*copyTree)->left;
    else if(key> (*copyTree)->key)
      copyTree = &(*copyTree)->right;
  }

  // ak sme nenasli zhodny kluc
  if( *copyTree == NULL){
    *copyTree = malloc(sizeof(bst_node_t));
    (*copyTree)->value = value;
    (*copyTree)->key = key;
    (*copyTree)->left = NULL;
    (*copyTree)->right = NULL;
  }
  else if((*copyTree)->key)
     // Ak sme nasli shodny kluc
    (*copyTree)->value = value;
  
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
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree) {

  bst_node_t* copyTree = (*tree)->left;
  //Pomocna premenna pre spravne napojenie deti ak ma najpravejsi uzol este nejakych lavych potomkov
  bst_node_t* parentOfRightMost = *tree;


  //Prechadzanie stromu az k jeho napravejsiemu listu
  while(copyTree->right != NULL){
    if(copyTree->right->right == NULL)
      parentOfRightMost = copyTree;  

    copyTree = copyTree->right;
  }

  // Spravne naviazanie potomkom njapravejsieho uzlu
  if(parentOfRightMost == *tree)
      parentOfRightMost->left = copyTree->left;
  else
      parentOfRightMost->right = copyTree->left; 

  //Prepisanie target hodnot
  target->value = copyTree->value;
  target->key = copyTree->key;


  free(copyTree);
  copyTree = NULL;


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
 * Funkciu implementujte iteratívne pomocou bst_replace_by_rightmost a bez
 * použitia vlastných pomocných funkcií.
 */
void bst_delete(bst_node_t **tree, char key) {

  bst_node_t** copyTree = tree;
  bst_node_t* helpTree = NULL;


    
  //Prechadzanie kym nenajdeme dany kluc alebo kym nezistime, ze kluc sa v strome nenachadza
  while (*copyTree != NULL && (*copyTree)->key != key)
  {
    if(key < (*copyTree)->key)
      copyTree = &(*copyTree)->left;
    else if(key > (*copyTree)->key)
     copyTree = &(*copyTree)->right;
  }
  
  //Ak sa kluc nenachdza, nic nerobime
  if(*copyTree == NULL){
    return;
  }
  else if((*copyTree)->key == key){
    if((*copyTree)->left == NULL && (*copyTree)->right == NULL){
      //Ak dany uzol nema potomkov a je listovy uzol
      free(*copyTree);
      *copyTree = NULL;
      
    }
    else if((*copyTree)->left != NULL && (*copyTree)->right != NULL){
      //AK ma dany uzol oboch potomkov
      bst_replace_by_rightmost(*copyTree, copyTree);

    }
    else if((*copyTree)->left != NULL){
      //AK ma dany uzol iba praveho potomka
      helpTree = (*copyTree)->left;
      free(*copyTree);
      //*copyTree = NULL;
      *copyTree = helpTree;

    }
    else if((*copyTree)->right != NULL){
      //AK ma dany uzol iba laveho potomka
      helpTree = (*copyTree)->right;
      free(*copyTree);
      //*copyTree = NULL;
      *copyTree = helpTree;
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
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_dispose(bst_node_t **tree) {

  stack_bst_t stack;

  stack_bst_init(&stack);
  //Ak je dany strom prazny, nic nerobime
  if((*tree) == NULL)
    return;

  //Ukladanie potomkov uzla na zasobnik
  if((*tree)->right != NULL)
    stack_bst_push(&stack, (*tree)->right);
  if((*tree)->left != NULL)
    stack_bst_push(&stack, (*tree)->left);

  //uvolnenie uzla
  free(*tree);
  //Prechadzanie zasobniu a mazanie uzlov
  while(!stack_bst_empty(&stack)){
    *tree = stack_bst_pop(&stack);  
    //ukladanie potomkov uzal na zasobnik
    if((*tree)->right != NULL)
      stack_bst_push(&stack, (*tree)->right);
    if((*tree)->left != NULL)
      stack_bst_push(&stack, (*tree)->left);
    
    free(*tree);
  }

  *tree = NULL;

}

/*
 * Pomocná funkcia pre iteratívny preorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu.
 * Nad spracovanými uzlami zavola bst_print_node a uloží ich do zásobníku uzlov.
 *
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_preorder(bst_node_t *tree, stack_bst_t *to_visit) {
  bst_node_t* copyTree = tree;
  // najdenie najpravejsieho uzla
  while(copyTree != NULL){
    stack_bst_push(to_visit, copyTree->right);
    bst_print_node(copyTree);
    copyTree = copyTree->left;
  }

}

/*
 * Preorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_preorder a
 * zásobníku uzlov bez použitia vlastných pomocných funkcií.
 */
void bst_preorder(bst_node_t *tree) {
  stack_bst_t stack;
  bst_node_t* copyTree = tree;

  if(tree == NULL)
    return;

  stack_bst_init(&stack);
  bst_leftmost_preorder(copyTree, &stack);
  // Prechadzanie kazdeho uzla v zasobniku
  while(!stack_bst_empty(&stack)){
    copyTree = stack_bst_pop(&stack);
    bst_leftmost_preorder(copyTree, &stack);
  }
}

/*
 * Pomocná funkcia pre iteratívny inorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu a ukladá uzly do
 * zásobníku uzlov.
 *
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_inorder(bst_node_t *tree, stack_bst_t *to_visit) {
  bst_node_t* copyTree = tree;

  while(copyTree != NULL){
    stack_bst_push(to_visit, copyTree);
    copyTree = copyTree->left;
  }
}

/*
 * Inorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_inorder a
 * zásobníku uzlov bez použitia vlastných pomocných funkcií.
 */
void bst_inorder(bst_node_t *tree) {
  stack_bst_t stack;
  bst_node_t* copyTree = tree;

  if(tree == NULL)
    return;

  stack_bst_init(&stack);
  bst_leftmost_inorder(copyTree, &stack);

  // prechadzanie kazdeho uzla v zasobniku
  while(!stack_bst_empty(&stack)){

    copyTree = stack_bst_pop(&stack);
    bst_print_node(copyTree);
    bst_leftmost_inorder(copyTree->right, &stack);
  }

}

/*
 * Pomocná funkcia pre iteratívny postorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu a ukladá uzly do
 * zásobníku uzlov. Do zásobníku bool hodnôt ukladá informáciu že uzol
 * bol navštívený prvý krát.
 *
 * Funkciu implementujte iteratívne pomocou zásobníkov uzlov a bool hodnôt a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_postorder(bst_node_t *tree, stack_bst_t *to_visit, stack_bool_t *first_visit) {
  bst_node_t* copyTree = tree;

  while(copyTree != NULL){

    stack_bst_push(to_visit, copyTree);
    stack_bool_push(first_visit, true);
    copyTree = copyTree->left;
  
  }

}

/*
 * Postorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_postorder a
 * zásobníkov uzlov a bool hodnôt bez použitia vlastných pomocných funkcií.
 */
void bst_postorder(bst_node_t *tree) {
  stack_bst_t stack;
  bst_node_t* copyTree = tree;
  stack_bool_t boolStack;
  bool visited;


  if(tree == NULL)
    return;
  //Inicializacia polí
  stack_bool_init(&boolStack);
  stack_bst_init(&stack);

  //Prve najdenie najlavejsieho prvku
  bst_leftmost_postorder(copyTree, &stack, &boolStack);
  
  //Prechadzam, kym nie je prazdny zasobnik
  while(!stack_bst_empty(&stack)){

    copyTree = stack_bst_top(&stack);
    visited = stack_bool_pop(&boolStack);
    

    //ak je visited true, znamena ze sme v uzle uz raz boli
    if(visited){
      stack_bool_push(&boolStack, false);
      bst_leftmost_postorder(copyTree->right, &stack, &boolStack);
    }
    else{
      stack_bst_pop(&stack);
      bst_print_node(copyTree);
    }
  }
  
}
