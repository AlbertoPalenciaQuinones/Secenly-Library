#include <gtest/gtest.h>

#include "src/hardware_manager.h"

using namespace secenly::internal;

// TEST 1 - El identificador de hardware se inicializa correctamente. Esta 
// verificación incluye que se ha asignado el id de hardware al atributo de la 
// clase correctamente (verificando que el identificador no esté vacío).
TEST(HardwareIdentifierTest, InitializeReturnsTrue) {
    HardwareManager hw;

    EXPECT_FALSE(hw.GetHwid().empty());
}

// TEST 2 - Al inicializar 2 identificadores de hardware con la misma semilla y 
// desde el mismo equipo, estos deben de ser siempre el mismo.
TEST(HardwareIdentifierTest, HwidIsStable) {
    HardwareManager hw1, hw2;

    EXPECT_EQ(hw1.GetHwid(), hw2.GetHwid());
}