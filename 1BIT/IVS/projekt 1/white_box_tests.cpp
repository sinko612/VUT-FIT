//======== Copyright (c) 2022, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     White Box - Tests suite
//
// $NoKeywords: $ivs_project_1 $white_box_code.cpp
// $Author:     SIMONA JÁNOŠÍKOVÁ <xjanos19@stud.fit.vutbr.cz>
// $Date:       $2022-05-03
//============================================================================//
/**
 * @file white_box_tests.cpp
 * @author SIMONA JÁNOŠÍKOVÁ
 * 
 * @brief Implementace testu prace s maticemi.
 */

#include "gtest/gtest.h"
#include "white_box_code.h"


//============================================================================//
// ** ZDE DOPLNTE TESTY **
//
// Zde doplnte testy operaci nad maticemi. Cilem testovani je:
// 1. Dosahnout maximalniho pokryti kodu (white_box_code.cpp) testy.
// 2. Overit spravne chovani operaci nad maticemi v zavislosti na rozmerech 
//    matic.
//============================================================================//

class TestMatrix : public ::testing::Test
{
    //v tejto triede incializujem matice roznej velkosti(=> vytvorim funkcie, ktore budem volat v testoch), ktore budem postupne pouzivat v testoch

protected:
    Matrix matica1x1;
    Matrix matica2x2;
    Matrix matica2x3;
    Matrix matica3x2;
    Matrix matica3x3;
    Matrix matica5x5;

    Matrix getMatica1x1()
    {
        matica1x1 = Matrix();
        return matica1x1;
    }    
    void setMatica1x1()
    {
        matica1x1 = getMatica1x1();
    }
    
    Matrix getMatica2x2()
    {
        matica2x2 = Matrix(2,2);
        matica2x2.set(std::vector<std::vector< double > > 
            {{1, 5},
             {7, 8},}
        );
        return matica2x2;
    }    
    void setMatica2x2()
    {
        matica2x2 = getMatica2x2();
    }

    Matrix getMatica2x3()
    {
        matica2x3 = Matrix(2,3);
        matica2x3.set(std::vector<std::vector< double > > 
            {{4, 3, 1},
             {0, 7, 8},}
        );
        return matica2x3;
    }    
    void setMatica2x3()
    {
        matica2x3 = getMatica2x3();
    }

    Matrix getMatica3x2()
    {
        matica3x2 = Matrix(3,2);
        matica3x2.set(std::vector<std::vector< double > > 
            {{2, 9},
             {1, 0},
             {4, 6},}
        );
        return matica3x2;
    }    
    void setMatica3x2()
    {
        matica3x2 = getMatica3x2();
    }

    Matrix getMatica3x3()
    {
        matica3x3 = Matrix(3,3);
        matica3x3.set(std::vector<std::vector< double > > 
            {{1, 2, 3},
             {0, 5, 0},
             {4, 3, 6},}
        );
        return matica3x3;
    }    
    void setMatica3x3()
    {
        matica3x3 = getMatica3x3();
    }

    Matrix getMatica5x5()
    {
        matica5x5 = Matrix(5,5);
        matica5x5.set(std::vector<std::vector< double > > 
            {{1, 2, 3, 4, 5},
             {9.4, 8, 7, -6, 5},
             {454, 3.21, -6.45, 4, 8.21},
             {0, -1.1, -2, -3.4, -4.0},
             {123, -123, 259.21, -259.21, 63},}
        );
        return matica5x5;
    }    
    void setMatica5x5()
    {
        matica5x5 = getMatica5x5();
    }
};

TEST_F(TestMatrix,Matrix)
{/* v tomto teste overujem konstruktor matice => ci sa matica vytvori spravne na zaklade poziadavku(velkost aspon 1x1) */
    
    //predpokladam, za sa matica vytvori spravne
    EXPECT_NO_THROW(Matrix());
    EXPECT_NO_THROW(Matrix(4,3));
    EXPECT_NO_THROW(Matrix(150,150));
    EXPECT_NO_THROW(Matrix(1000,1000));

    //ocakavam, za sa matica nevytvori, pretoze pozadovana velkost je aspon 1x1
    EXPECT_ANY_THROW(Matrix(0, 0));
    EXPECT_ANY_THROW(Matrix(0, 1));
    EXPECT_ANY_THROW(Matrix(1, 0));
    EXPECT_ANY_THROW(Matrix(0, -1));
    EXPECT_ANY_THROW(Matrix(-1, 0));
    EXPECT_ANY_THROW(Matrix(-1, -1));
}

TEST_F(TestMatrix,set_hodnota)
{/* v tomto teste overujem, ci sa do matice vlozi hodnota na urcitu poziciu */
    
    //overnie pre maticu 2x2
    setMatica2x2();
    
    //vlozenie hodnot na existujuce pozicie
    EXPECT_TRUE(matica2x2.set(0,0,2));
    EXPECT_TRUE(matica2x2.set(0,1,2.14));
    EXPECT_TRUE(matica2x2.set(1,0,-2));
    EXPECT_TRUE(matica2x2.set(1,1,-2.14));

    //pokus o vlozenie hodnoty na poziciu, ktora sa v matici nenachadza
    EXPECT_FALSE(matica2x2.set(0,3,2));
    EXPECT_FALSE(matica2x2.set(1,3,2));
    EXPECT_FALSE(matica2x2.set(2,0,2));
    EXPECT_FALSE(matica2x2.set(2,2,2));
    EXPECT_FALSE(matica2x2.set(2,3,2));
    EXPECT_FALSE(matica2x2.set(10,30,2));

    //overenie pre vacsiu maticu 8x7
    Matrix matica8x7 = Matrix(8,7);
    
    //vlozenie hodnot na existujuce pozicie
    EXPECT_TRUE(matica8x7.set(0,0,8));
    EXPECT_TRUE(matica8x7.set(0,6,8.45));
    EXPECT_TRUE(matica8x7.set(4,3,-8));
    EXPECT_TRUE(matica8x7.set(5,6,-8.45));
    EXPECT_TRUE(matica8x7.set(7,0,845));
    EXPECT_TRUE(matica8x7.set(7,6,-845));
    
    //pokus o vlozenie hodnoty na poziciu, ktora sa v matici nenachadza
    EXPECT_FALSE(matica8x7.set(0,7,8));
    EXPECT_FALSE(matica8x7.set(4,7,8));
    EXPECT_FALSE(matica8x7.set(8,7,8));
    EXPECT_FALSE(matica8x7.set(8,0,8));
    EXPECT_FALSE(matica8x7.set(8,4,8));
    EXPECT_FALSE(matica8x7.set(8,7,8));
    EXPECT_FALSE(matica8x7.set(100,7,8));
    EXPECT_FALSE(matica8x7.set(0,700,8));
    EXPECT_FALSE(matica8x7.set(100,700,8));
}

TEST_F(TestMatrix,set_pole)
{/* v tomto teste overujem, ci sa do matice vlozia prvky pola */
    
    setMatica1x1();
    
    //ocakavam, ze sa pole vlozi do matice
    EXPECT_TRUE(matica1x1.set(std::vector<std::vector<double>> {{1}} ));
    EXPECT_TRUE(matica1x1.set(std::vector<std::vector<double>> {{1.34}} ));
    EXPECT_TRUE(matica1x1.set(std::vector<std::vector<double>> {{100000}} ));
    EXPECT_TRUE(matica1x1.set(std::vector<std::vector<double>> {{-1}} ));
    EXPECT_TRUE(matica1x1.set(std::vector<std::vector<double>> {{-1.34}} ));
    EXPECT_TRUE(matica1x1.set(std::vector<std::vector<double>> {{-100000}} ));
    
    //ocakavam, ze sa pole do matice nevlozi
    EXPECT_FALSE(matica1x1.set(std::vector<std::vector<double>> {{1, 2}} ));

    setMatica3x3();
    
    //ocakavam, ze sa pole vlozi do matice
    EXPECT_TRUE(matica3x3.set(std::vector<std::vector< double > > 
            {{1, 2, 3},
             {4.0, 5.87, 6.23},
             {700, 8000, 90000},}
        ));
    EXPECT_TRUE(matica3x3.set(std::vector<std::vector< double > > 
            {{-1, -2, -3},
             {-4.0, -5.87, -6.23},
             {-700, -8000, -90000},}
        ));    
    
    //ocakavam, ze sa pole do matice nevlozi
    EXPECT_FALSE(matica3x3.set(std::vector<std::vector< double > > 
            {{1, 2.045, 3000000},} ));
    EXPECT_FALSE(matica3x3.set(std::vector<std::vector< double > > 
            {{1, 2, 3},
             {1, 2.045, 3000000},} ));        
}

TEST_F(TestMatrix,get)
{/* v tomto teste overujem, ci sa mi vrati hodnota z matice na pozadovanej pozicii */
    
    setMatica1x1();
    
    //testujem poziciu, ktora sa nachadza v matici
    EXPECT_NO_THROW(matica1x1.get(0,0));
    EXPECT_DOUBLE_EQ(0,matica1x1.get(0,0));
    
    //do matice 1x1 vlozim novy prvok
    matica1x1.set(0,0,1);
    EXPECT_TRUE(matica1x1.set(0,0,1));
    EXPECT_NO_THROW(matica1x1.get(0,0));
    EXPECT_DOUBLE_EQ(1,matica1x1.get(0,0));
    
    //testujem poziciu, ktora sa nenachadza v matici
    EXPECT_ANY_THROW(matica1x1.get(0,1));
    EXPECT_ANY_THROW(matica1x1.get(1,0));
    EXPECT_ANY_THROW(matica1x1.get(1,1));
    
    setMatica5x5();
    
    //testujem poziciu, ktora sa nachadza v matici
    EXPECT_NO_THROW(matica5x5.get(0,1));
    EXPECT_DOUBLE_EQ(2,matica5x5.get(0,1));
    EXPECT_NO_THROW(matica5x5.get(2,4));
    EXPECT_DOUBLE_EQ(8.21,matica5x5.get(2,4));
    EXPECT_NO_THROW(matica5x5.get(1,3));
    EXPECT_DOUBLE_EQ(-6,matica5x5.get(1,3));
    EXPECT_NO_THROW(matica5x5.get(4,3));
    EXPECT_DOUBLE_EQ(-259.21,matica5x5.get(4,3));
    
    //testujem poziciu, ktora sa nenachadza v matici
    EXPECT_ANY_THROW(matica5x5.get(0,5));
    EXPECT_ANY_THROW(matica5x5.get(5,0));
    EXPECT_ANY_THROW(matica5x5.get(5,5));
    EXPECT_ANY_THROW(matica5x5.get(500,5000));
    EXPECT_ANY_THROW(matica5x5.get(-5,5));
}

TEST_F(TestMatrix,Equal)
{/* v tomto teste overujem, ci sa matice rovnaju */
    
    setMatica1x1();
    setMatica2x2();
    
    //matice by sa mali rovnat
    EXPECT_NO_THROW(matica1x1 == matica1x1);
    EXPECT_TRUE(matica1x1 == matica1x1);
    EXPECT_NO_THROW(matica2x2 == matica2x2);
    EXPECT_TRUE(matica2x2 == matica2x2);
    
    //matice by sa nemali rovnat lebo nemaju rovnaku velkost
    EXPECT_ANY_THROW(matica1x1 == matica2x2);
    EXPECT_ANY_THROW(matica2x2 == matica1x1);

    setMatica2x3();
    Matrix matica2x3_pom = Matrix(2,3);
    
    //matice by sa nemali rovnat, nie su rovnake
    EXPECT_FALSE(matica2x3 == matica2x3_pom);
    
    //maticu matica2x3_pom naplnim hodnotami matice matica2x3 a mali by sa rovnat
    matica2x3_pom.set(std::vector<std::vector< double > > 
            {{4, 3, 1},
             {0, 7, 8},}
        );
    EXPECT_TRUE(matica2x3 == matica2x3_pom);
}

TEST_F(TestMatrix,Plus)
{/* v tomto teste testujem sucet matic */
    
    setMatica2x3();
    setMatica3x2();
    setMatica3x3();

    //matice nie je mozne scitat, pretoze nie su rovnakej velkosti
    EXPECT_ANY_THROW(matica3x2 + matica2x3);
    EXPECT_ANY_THROW(matica2x3 + matica3x3);

    //skusim scitat maticu samu so sebou, ocakavam true
    EXPECT_NO_THROW(matica3x3 + matica3x3);
    Matrix sucet1 = matica3x3 + matica3x3;
    Matrix vysledok1 = Matrix(3,3);
    vysledok1.set(std::vector<std::vector< double > > 
            {{2, 4, 6},
             {0, 10, 0},
             {8, 6, 12},}
        );
    EXPECT_TRUE(sucet1 == vysledok1);

    //scitanie matic rovnakej velkosti, ale kazda je naplnena inymi hodnotami
    Matrix matica3x2_pom = Matrix(3,2);
    matica3x2_pom.set(std::vector<std::vector< double > > 
            {{2.4, 4},
             {1.5, 1.04},
             {-4, -2.67},}
        );
    EXPECT_NO_THROW(matica3x2 + matica3x2_pom);
    Matrix sucet2 = matica3x2 + matica3x2_pom;
    Matrix vysledok2 = Matrix(3,2);
    vysledok2.set(std::vector<std::vector< double > > 
            {{4.4, 13},
             {2.5, 1.04},
             {0, 3.33},}
        );
    EXPECT_TRUE(sucet2 == vysledok2);
}

TEST_F(TestMatrix,Multiply_matice)
{/* v tomto teste testujem nasobenie matice maticou*/
    
    setMatica2x3();
    setMatica3x2();
    setMatica2x2();
    setMatica5x5();

    //matice nie je mozne vynasobit, pretoze prva mastica nema rovnaky pocet riadkov ako druha stlpcov
    EXPECT_ANY_THROW(matica2x3 * matica2x2);
    EXPECT_ANY_THROW(matica2x3 * matica5x5);
    EXPECT_ANY_THROW(matica3x2 * matica5x5);

    //v ostatnych pripadoch nasobenie je mozne (=pocet riadkov prvej matice sa rovna poctu stlpcov druhej matice)
    EXPECT_NO_THROW(matica3x2 * matica2x2);
    Matrix nasobenie1 = matica3x2 * matica2x2;
    Matrix vysledokn1 = Matrix(3,2);
    vysledokn1.set(std::vector<std::vector< double > > 
            {{65, 82},
             {1, 5},
             {46, 68},}
        );
    EXPECT_TRUE(nasobenie1 == vysledokn1);

    EXPECT_NO_THROW(matica3x2 * matica2x3);
    Matrix nasobenie2 = matica3x2 * matica2x3;
    Matrix vysledokn2 = Matrix(3,3);
    vysledokn2.set(std::vector<std::vector< double > > 
            {{8, 69, 74},
             {4, 3, 1},
             {16, 54, 52},}
        );
    EXPECT_TRUE(nasobenie2 == vysledokn2);   
}

TEST_F(TestMatrix,Multiply_const)
{/* v tomto teste testujem nasobenie matice konstantou */
    
    setMatica1x1();
    setMatica3x3();

    //nasobim maticu 1x1 konstantou
    Matrix nas_const1 = matica1x1 * 4;
    Matrix vysl1 = Matrix();
    EXPECT_TRUE(nas_const1 == vysl1);
    
    //do matice 1x1 vlozim cislo a vynasobim
    matica1x1.set(0,0,4);
    EXPECT_TRUE(matica1x1.set(0,0,4));
    Matrix nas_const2 = matica1x1 * 4.45;
    Matrix vysl2 = Matrix();
    vysl2.set(0,0,17.8);
    EXPECT_TRUE(vysl2.set(0,0,17.8));
    EXPECT_TRUE(nas_const2 == vysl2);

    Matrix nas_const3 = matica3x3 * -2;
    Matrix vysl3 = Matrix(3,3);
    vysl3.set(std::vector<std::vector< double > > 
            {{-2, -4, -6},
             {0, -10, 0},
             {-8, -6, -12},}
        );
    EXPECT_TRUE(nas_const3 == vysl3);
}

TEST_F(TestMatrix,SolveEquation)
{/* v tomto teste testujem riesenie sustavy linearnych rovnic */
   
    setMatica1x1();
    setMatica2x2();
    setMatica3x3();
    setMatica3x2();
    setMatica2x3();
    setMatica5x5();
    
    //nie je mozne vypocitat sustavu linearny rovnic, pretoze matica nie je stvorcova
    EXPECT_ANY_THROW(matica3x2.solveEquation(std::vector<double> {1,1} ));
    EXPECT_ANY_THROW(matica2x3.solveEquation(std::vector<double> {1,1,0}));

    //nie je mozne vypocitat sustavu linearny rovnic, pretoze pocet riadkov vektora sa nerovna poctu riadkov matice
    EXPECT_ANY_THROW(matica3x3.solveEquation(std::vector<double> {1,1} ));

    //nie je mozne vypocitat sustavu linearny rovnic, matica je singularna
    Matrix sust1 = Matrix(2,2);
    sust1.set(std::vector<std::vector< double > > 
            {{1, 2},
            {1, 2,},}
        );
    EXPECT_ANY_THROW(sust1.solveEquation(std::vector<double> {8,2} ));

    //nie je mozne vyriesit sustavu linearny rovnic, sustava linearny rovnic nema riesenie
    Matrix sust = Matrix(2,2);
    sust.set(std::vector<std::vector< double > > 
            {{1, 1},
            {1, 1,},}
        );
    EXPECT_ANY_THROW(sust.solveEquation(std::vector<double> {8,2} ));  

    //vypocet sustavy linearny rovnic pre maticu 1x1, sustava linearny rovnic ma riesenie
    matica1x1.set(0,0,5);
    EXPECT_TRUE(matica1x1.set(0,0,5));
    EXPECT_NO_THROW(matica1x1.solveEquation(std::vector<double> {5}));
    EXPECT_TRUE(matica1x1.solveEquation(std::vector<double> {5}) == (std::vector<double> {1}));
    
    //vypocet sustavy linearny rovnic pre maticu 2x2, sustava linearny rovnic ma riesenie
    EXPECT_NO_THROW(matica2x2.solveEquation(std::vector<double> {8,2}));
    EXPECT_TRUE(matica2x2.solveEquation(std::vector<double> {8,2}) == (std::vector<double> {-2,2}));

    //vypocet sustavy linearny rovnic pre maticu 3x3, sustava linearny rovnic ma riesenie
    EXPECT_NO_THROW(matica3x3.solveEquation(std::vector<double> {1,2,2}));
    EXPECT_TRUE(matica3x3.solveEquation(std::vector<double> {1,2,2}) == (std::vector<double> {0.2,0.4,0}));

    //vyopcet sustavy linearny rovnic pre maticu 5x5, sustava linearny rovnic ma riesenie
    EXPECT_NO_THROW(matica5x5.solveEquation(std::vector<double> {0,0,0,0,0}));
    EXPECT_TRUE(matica5x5.solveEquation(std::vector<double> {0,0,0,0,0}) == (std::vector<double> {0,0,0,0,0}));
}

TEST_F(TestMatrix,Transpose)
{/* v tomto teste testujem vytvorenie transponovanej matice k hocijakej matici */
    
    setMatica2x3();
    setMatica2x2();

    //vytvorenie transponovanej matice k matici 3x2
    Matrix matica2x3_tsp = Matrix(3,2);
    matica2x3_tsp.set(std::vector<std::vector< double > > 
            {{4, 0},
             {3, 7},
             {1, 8},}
        );
    EXPECT_TRUE(matica2x3.transpose() == matica2x3_tsp);   

    //vytvorenie transponovanej matice k matici 2x2
    Matrix matica2x2_tsp = Matrix(2,2);
    matica2x2_tsp.set(std::vector<std::vector< double > > 
            {{1, 7},
             {5, 8},}
        );
    EXPECT_TRUE(matica2x2.transpose() == matica2x2_tsp);    
}

TEST_F(TestMatrix,Inverse)
{/* v tomto teste testujem vytvorenie inverznej matice k matici (ak je to mozne) */
    
    setMatica2x3();

    //inverznu maticu nie je mozne vytvorit, lebo sa nejdna o maticu 2x2 alebo 3x3
    EXPECT_ANY_THROW(matica2x3.inverse());

    //inverznu maticu nie je mozne vypocitat, matica je singularna
    Matrix matica0 = Matrix(2,2);
    matica0.set(std::vector<std::vector< double > > 
            {{1, 2},
            {1, 2,},}
        );
    EXPECT_ANY_THROW(matica0.inverse());    

    //pokus o vypocet inverznej matice k matici 2x2 (inverzna matica neexistuje)
    Matrix matica1 = Matrix(2,2);
    matica1.set(std::vector<std::vector< double > > 
            {{1, 1},
            {1, 1,},}
        );
    EXPECT_ANY_THROW(matica1.inverse());

    //vypocet inverznej matice k matici 2x2, inverzna matica existuje
    Matrix matica2x2_inv = Matrix(2,2);
    matica2x2_inv.set(std::vector<std::vector< double > > 
            {{1,0},
             {1,1},}
        );
    EXPECT_NO_THROW(matica2x2_inv.inverse());  
    Matrix matica2 = Matrix(2,2);
    matica2.set(std::vector<std::vector< double > > 
            {{1,0},
             {-1,1},}
        );
    EXPECT_TRUE(matica2x2_inv.inverse() == matica2);

    //vypocet inverznej matice k matici 3x3, inverzna matica existuje
    Matrix matica3x3_inv = Matrix(3,3);
    matica3x3_inv.set(std::vector<std::vector< double > > 
            {{1,0,1},
             {0,1,1},
             {1,1,1},}
        );
    EXPECT_NO_THROW(matica3x3_inv.inverse());
    Matrix matica3 = Matrix(3,3);
    matica3.set(std::vector<std::vector< double > > 
            {{0,-1,1},
             {-1,0,1},
             {1,1,-1},}
        );
    EXPECT_TRUE(matica3x3_inv.inverse() == matica3);    
}

/*** Konec souboru white_box_tests.cpp ***/