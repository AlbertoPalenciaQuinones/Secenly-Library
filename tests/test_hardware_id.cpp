#include <gtest/gtest.h>

#include "../library/manager/hardware_manager.h"

// g++ -std=c++17 src/library/hardware_manager.cpp src/library/license_manager.cpp src/library/product_identifier.cpp src/sha/*.cpp tests/*.cpp -Isrc -lgtest -lgtest_main -pthread -o runTests

// TEST 1 - El identificador de hardware se inicializa correctamente. Esta 
// verificación incluye que se ha asignado el id de hardware al atributo de la 
// clase correctamente (verificando que el identificador no esté vacío).
//TEST(HardwareIdentifierTest, InitializeReturnsTrue) {
//    HardwareManager hw;

//    EXPECT_TRUE(hw.Initialize());
//}

// TEST 2 - Al inicializar 2 identificadores de hardware con la misma semilla y 
// desde el mismo equipo, estos deben de ser siempre el mismo.
//TEST(HardwareIdentifierTest, HwidIsStable) {
//    HardwareManager hw1, hw2;

//    ASSERT_TRUE(hw1.GetHwid());
//    ASSERT_TRUE(hw2.Initialize());

//    EXPECT_EQ(hw1.getHwid(), hw2.getHwid());
//}


