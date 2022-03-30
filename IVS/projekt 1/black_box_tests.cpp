//======== Copyright (c) 2022, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Red-Black Tree - public interface tests
//
// $NoKeywords: $ivs_project_1 $black_box_tests.cpp
// $Author:     SIMONA JÁNOŠÍKOVÁ <xjanos19@stud.fit.vutbr.cz>
// $Date:       $2022-27-02
//============================================================================//
/**
 * @file black_box_tests.cpp
 * @author SIMONA JÁNOŠÍKOVÁ
 * 
 * @brief Implementace testu binarniho stromu.
 */

#include <vector>

#include "gtest/gtest.h"

#include "red_black_tree.h"

//============================================================================//
// ** ZDE DOPLNTE TESTY **
//
// Zde doplnte testy Red-Black Tree, testujte nasledujici:
// 1. Verejne rozhrani stromu
//    - InsertNode/DeleteNode a FindNode
//    - Chovani techto metod testuje pro prazdny i neprazdny strom.
// 2. Axiomy (tedy vzdy platne vlastnosti) Red-Black Tree:
//    - Vsechny listove uzly stromu jsou *VZDY* cerne.
//    - Kazdy cerveny uzel muze mit *POUZE* cerne potomky.
//    - Vsechny cesty od kazdeho listoveho uzlu ke koreni stromu obsahuji
//      *STEJNY* pocet cernych uzlu.
//============================================================================//


//trieda na testovanie neprazdneho stromu
class NonEmptyTree : public ::testing::Test
{
protected:    
    BinaryTree strom;
    virtual void SetUp()
    {
        int nodes[15] = {10, 3, 42, 20, 5, 38, 14, 7, 25, 17, 29, 12, 35, 21, 15};

        for(auto value :nodes)
        {
            strom.InsertNode(value);
        }
    }
};

//trieda na testovanie prazdneho stromu
class EmptyTree : public ::testing::Test
{
protected:
    BinaryTree prazdny_strom;
};


//trieda na testovanie axiomov neprazdneho stromu
class TreeAxioms : public ::testing::Test
{
protected:  
    BinaryTree axiom_strom;
    virtual void SetUp()
    {
        int uzly[15] = {10, 3, 42, 20, 5, 38, 14, 7, 25, 17, 29, 12, 35, 21, 15};

        for(auto value:uzly)
        {
            axiom_strom.InsertNode(value);
        }
    }
};

TEST_F(EmptyTree, InsertNode)
{
    //test InsertNode prazdneho stromu

    //vlozenie uzol s hodnotou 10
    std::pair<bool, BinaryTree::Node_t *> insert1 = prazdny_strom.InsertNode(10);
    BinaryTree::Node_t *uzol1 = insert1.second;
    ASSERT_TRUE(uzol1 != NULL);
    EXPECT_TRUE(insert1.first);

    //overenie stavu uzla s hodnotou 10
    EXPECT_EQ(uzol1->key, 10);
    EXPECT_TRUE(uzol1->pParent == NULL);
    EXPECT_EQ(uzol1->color, BinaryTree::BLACK);

    //overenie stavu potomkov uzla s hodnotou 10
    BinaryTree::Node_t *lavy1 = uzol1->pLeft;
    ASSERT_TRUE(lavy1 != NULL);
    EXPECT_TRUE(lavy1->pParent != NULL);
    EXPECT_EQ(lavy1->pParent->key, 10);
    EXPECT_EQ(lavy1->color, BinaryTree::BLACK);
    EXPECT_TRUE(lavy1->pLeft == NULL);
    EXPECT_TRUE(lavy1->pRight == NULL);
    BinaryTree::Node_t *pravy1 = uzol1->pRight;
    ASSERT_TRUE(pravy1 != NULL);
    EXPECT_TRUE(pravy1->pParent != NULL);
    EXPECT_EQ(pravy1->pParent->key, 10);
    EXPECT_EQ(pravy1->color, BinaryTree::BLACK);
    EXPECT_TRUE(pravy1->pLeft == NULL);
    EXPECT_TRUE(pravy1->pRight == NULL);

    //vlozenie uzla s hodnotou 9
    std::pair<bool, BinaryTree::Node_t *> insert2 = prazdny_strom.InsertNode(9);
    BinaryTree::Node_t *uzol2 = insert2.second;
    ASSERT_TRUE(uzol2 != NULL);
    EXPECT_TRUE(insert2.first);

    //overenie stavu uzla s hodnotou 9
    EXPECT_EQ(uzol2->key, 9);
    EXPECT_TRUE(uzol2->pParent != NULL);
    EXPECT_EQ(uzol2->color, BinaryTree::RED);

    //overenie stavu potomkov uzla s hodnotou 9
    BinaryTree::Node_t *lavy2 = uzol2->pLeft;
    ASSERT_TRUE(lavy2 != NULL);
    EXPECT_TRUE(lavy2->pParent != NULL);
    EXPECT_EQ(lavy2->pParent->key, 9);
    EXPECT_EQ(lavy2->color, BinaryTree::BLACK);
    EXPECT_TRUE(lavy2->pLeft == NULL);
    EXPECT_TRUE(lavy2->pRight == NULL);
    BinaryTree::Node_t *pravy2 = uzol2->pRight;
    ASSERT_TRUE(pravy2 != NULL);
    EXPECT_TRUE(pravy2->pParent != NULL);
    EXPECT_EQ(pravy2->pParent->key, 9);
    EXPECT_EQ(pravy2->color, BinaryTree::BLACK);
    EXPECT_TRUE(pravy2->pLeft == NULL);
    EXPECT_TRUE(pravy2->pRight == NULL);

    //overenie stavu uzla s hodnotou 10 po pridani uzla s hodnotou 9
    ASSERT_TRUE(uzol1->pLeft != NULL);
    EXPECT_EQ(uzol1->pLeft->key, 9);

    //vlozenie uzla ktoreho hodnota uz bola vlozena
    std::pair<bool, BinaryTree::Node_t *> insert3 = prazdny_strom.InsertNode(10);
    BinaryTree::Node_t *uzol3 = insert3.second;
    ASSERT_TRUE(uzol3 != NULL);
    EXPECT_FALSE(insert3.first);

    //vlozenie uzla s hodnotou 0
    std::pair<bool, BinaryTree::Node_t *> insert4 = prazdny_strom.InsertNode(0);
    BinaryTree::Node_t *uzol4 = insert4.second;
    ASSERT_TRUE(uzol4 != NULL);
    EXPECT_TRUE(insert4.first);

    //overenie stavu uzla s hodnotou 0
    EXPECT_EQ(uzol4->key, 0);
    EXPECT_TRUE(uzol4->pParent != NULL);
    EXPECT_EQ(uzol4->pParent->key, 9);
    EXPECT_EQ(uzol4->color, BinaryTree::RED);

    //vlozenie uzla so zapornou hodnotou
    std::pair<bool, BinaryTree::Node_t *> insert5 = prazdny_strom.InsertNode(-4);
    BinaryTree::Node_t *uzol5 = insert5.second;
    ASSERT_TRUE(uzol5 != NULL);
    EXPECT_TRUE(insert5.first);

    //overenie stavu uzla so zapornou hodnotou
    EXPECT_EQ(uzol5->key, -4);
    EXPECT_TRUE(uzol5->pParent != NULL);
    EXPECT_EQ(uzol5->pParent->key, 0);
    EXPECT_EQ(uzol5->color, BinaryTree::RED);
}

TEST_F(EmptyTree, DeleteNode)
{
    //test DeleteNode prazdneho stromu
    EXPECT_FALSE(prazdny_strom.DeleteNode(10));
    EXPECT_FALSE(prazdny_strom.DeleteNode(-4));
    EXPECT_FALSE(prazdny_strom.DeleteNode(20));
    EXPECT_FALSE(prazdny_strom.DeleteNode(-10));
}

TEST_F(EmptyTree, FindNode)
{
    //test FindNode prazdneho stromu
    EXPECT_TRUE(prazdny_strom.FindNode(9) == NULL);
    EXPECT_TRUE(prazdny_strom.FindNode(0) == NULL);
    EXPECT_TRUE(prazdny_strom.FindNode(20) == NULL);
    EXPECT_TRUE(prazdny_strom.FindNode(-10) == NULL);
}   

TEST_F(NonEmptyTree, InsertNode)
{
    //test InsertNode neprazdneho stromu

    //vlozenie uzla s hodnotou 2 (v strome by sa taka hodnota nemala nachadzat)
    std::pair<bool, BinaryTree::Node_t *> insert6 = strom.InsertNode(2);
    BinaryTree::Node_t *uzol6 = insert6.second;
    ASSERT_TRUE(uzol6 != NULL);
    EXPECT_TRUE(insert6.first);

    //overenie stavu pridaneho uzla s hodnotou 2
    EXPECT_EQ(uzol6->key, 2);
    EXPECT_EQ(uzol6->color, BinaryTree::RED);
    ASSERT_TRUE(uzol6->pParent != NULL);                            //!!!!!!!!!!!!!!!!!!!
    if(uzol6->pParent != NULL)
    {
        EXPECT_EQ(uzol6->pParent->key, 3);
        EXPECT_EQ(uzol6->pParent->color, BinaryTree::BLACK);
    }

    //overenie stavu potomkov pridaneho uzla s hodnotou 2
    BinaryTree::Node_t *lavy6 = uzol6->pLeft;
    ASSERT_TRUE(lavy6 != NULL);
    EXPECT_TRUE(lavy6->pParent != NULL);
    EXPECT_EQ(lavy6->pParent->key, 2);
    EXPECT_EQ(lavy6->color, BinaryTree::BLACK);
    EXPECT_TRUE(lavy6->pLeft == NULL);
    EXPECT_TRUE(lavy6->pRight == NULL);
    BinaryTree::Node_t *pravy6 = uzol6->pRight;
    ASSERT_TRUE(pravy6 != NULL);
    EXPECT_TRUE(pravy6->pParent != NULL);
    EXPECT_EQ(pravy6->pParent->key, 2);
    EXPECT_EQ(pravy6->color, BinaryTree::BLACK);
    EXPECT_TRUE(pravy6->pLeft == NULL);
    EXPECT_TRUE(pravy6->pRight == NULL);

    //vlozenie uzla ktoreho hodnota sa uz v strome nachadza
    std::pair<bool, BinaryTree::Node_t *> insert7 = strom.InsertNode(42);
    BinaryTree::Node_t *uzol7 = insert7.second;
    ASSERT_TRUE(uzol7 != NULL);
    EXPECT_FALSE(insert7.first);                                    //!!!!!!!!!!!!!!!!!!

    //overenie stavu uzla ktory sa uz v strome nachadza
    EXPECT_EQ(uzol7->key, 42);
    EXPECT_EQ(uzol7->color, BinaryTree::BLACK);                     //!!!!!!!!!!!!!!
    ASSERT_TRUE(uzol7->pParent != NULL); 
    EXPECT_EQ(uzol7->pParent->key, 38);                             //!!!!!!!!!!!!!!!!!!
    EXPECT_EQ(uzol7->pParent->color, BinaryTree::BLACK);   

    //overenie stavu uzla ktory sa uz v strome nachadza
    BinaryTree::Node_t *lavy7 = uzol7->pLeft;
    ASSERT_TRUE(lavy7 != NULL);
    EXPECT_TRUE(lavy7->pParent != NULL);
    EXPECT_EQ(lavy7->pParent->key, 42);
    EXPECT_EQ(lavy7->color, BinaryTree::BLACK);
    EXPECT_TRUE(lavy7->pLeft == NULL);
    EXPECT_TRUE(lavy7->pRight == NULL);
    BinaryTree::Node_t *pravy7 = uzol7->pRight;
    ASSERT_TRUE(pravy7 != NULL);
    EXPECT_TRUE(pravy7->pParent != NULL);
    EXPECT_EQ(pravy7->pParent->key, 42);
    EXPECT_EQ(pravy7->color, BinaryTree::BLACK);
    EXPECT_TRUE(pravy7->pLeft == NULL);
    EXPECT_TRUE(pravy7->pRight == NULL);
}

TEST_F(NonEmptyTree, DeleteNode)
{
    //test DeleteNode neprazdneho stromu   
    EXPECT_TRUE(strom.DeleteNode(10));
    EXPECT_TRUE(strom.DeleteNode(38));
    EXPECT_FALSE(strom.DeleteNode(0));
    EXPECT_FALSE(strom.DeleteNode(10));
}

TEST_F(NonEmptyTree, FindNode)
{
    //test FindNode neprazdneho stromu
    EXPECT_TRUE(strom.FindNode(29) != NULL);   
    EXPECT_TRUE(strom.FindNode(17) != NULL);
    EXPECT_TRUE(strom.FindNode(0) == NULL);
    EXPECT_TRUE(strom.FindNode(50) == NULL);
}

TEST_F(TreeAxioms, Axiom1)
{
    //test Axiomu 1 neprazdneho stromu
    //"Vsechny listove uzly (tedy uzly bez potomku) jsou cerne."

    std::vector<BinaryTree::Node_t *> outLeafNodes;
    axiom_strom.GetLeafNodes(outLeafNodes);

    for(BinaryTree::Node_t *uzol : outLeafNodes)
    {
        EXPECT_EQ(uzol->color, BinaryTree::BLACK);
    }
}    

TEST_F(TreeAxioms, Axiom2)
{
    //test Axiomu 2 neprazdneho stromu
    //"Pokud je uzel cerveny, pak jsou jeho oba potomci cerne."

    std::vector<BinaryTree::Node_t *> outAllNodes;
    axiom_strom.GetAllNodes(outAllNodes);

    for(BinaryTree::Node_t *uzol : outAllNodes)
    {
        //hladaju sa cervene uzly
        if (uzol->color == BinaryTree::RED)
        {
            //ak jeho lavy a pravy potomok nie je NULLptr tak predpoklad ze su potomkovia cierny
            ASSERT_TRUE(uzol->pLeft != NULL);
		    EXPECT_EQ(uzol->pLeft->color, BinaryTree::BLACK);
			ASSERT_TRUE(uzol->pRight != NULL);
			EXPECT_EQ(uzol->pRight->color, BinaryTree::BLACK); 
        }
    }
}

TEST_F(TreeAxioms, Axiom3)
{
    //test Axiomu 3 neprazdneho stromu
    //"Kazda cesta od kazdeho listoveho uzlu ke koreni obsahuje stejny pocet cernych uzlu."
    
    std::vector<BinaryTree::Node_t *> outLeafNodes;
    axiom_strom.GetLeafNodes(outLeafNodes);

    int predchadzajuci_pocet_BLACK_listov = -1;
    int pocet_BLACK_listov = 0;
    BinaryTree::Node_t *aktualnyUzol;

    for(BinaryTree::Node_t *uzol : outLeafNodes)
    {
        pocet_BLACK_listov = 0;
        aktualnyUzol = uzol;

        //cyklom prechadzam od listu cey jeho parenta ku korenu stromu
        while(aktualnyUzol)
        {
            if(aktualnyUzol->color == BinaryTree::BLACK)
            {
                //ak je uzol cierny tak sa pripocita 1
                pocet_BLACK_listov++;
            }
            aktualnyUzol = aktualnyUzol->pParent;
        }

        //pokial sa nejdena o prvy cyklus, overujem, ci su rovnake => v prvok cykle nemam s cim porovnavat
        if (predchadzajuci_pocet_BLACK_listov != -1)
        {
            EXPECT_EQ(predchadzajuci_pocet_BLACK_listov,pocet_BLACK_listov);
        }

        //prepisem hodnotu predchazdajuceho na terajsiu
        predchadzajuci_pocet_BLACK_listov = pocet_BLACK_listov;
    }
}

/*** Konec souboru black_box_tests.cpp ***/
