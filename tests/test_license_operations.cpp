#include <gtest/gtest.h>

#define private public
#include "library/license_manager.h"
#undef private

// TEST 1 - Al pasar una cadena de hexadecimal por el flujo: hex -> bytes -> hex, 
// se verifica que el hexadecimal inicial coincida con el resultante del flujo.
TEST(LicenseManagerTest, HexToBytesAndBack) {
    LicenseManager manager;

    std::string hex = "deadbeef";
    auto bytes = manager.hexToBytes(hex);
    auto result = manager.bytesToHex(bytes);

    EXPECT_EQ(hex, result);
}

// TEST 2 - La conversión de hexadecimal a bytes debe fallar cuando se le pasan 
// caracteres inválidos. En el tyest, se le comprueba mandándole la letra Z.
TEST(LicenseManagerTest, HexToBytesInvalidCharacter) {
    LicenseManager manager;

    EXPECT_THROW({manager.hexToBytes("ZZ");}, std::invalid_argument);
}

// TEST 3 - Comprobar que la operación XOR se realiza correctamente. Para ello, 
// se realiza la operación con cadenas simples esperando su resultado correcto.
TEST(LicenseManagerTest, XorOperatesWell) {
    LicenseManager manager;

    std::vector<unsigned char> a = {0xFF, 0x00};
    std::vector<unsigned char> b = {0x0F, 0xF0};

    auto result = manager.xorBuffers(a, b);

    ASSERT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], 0xF0);
    EXPECT_EQ(result[1], 0xF0);
}

// TEST 4 - Al utilizar cadenas de distinto tamaño, se genera correctamente la 
// cadena cuando se realiza la operación XOR.
TEST(LicenseManagerTest, XorDifferentSizes) {
    LicenseManager manager;

    std::vector<unsigned char> a = {0xFF, 0x00, 0xAA};
    std::vector<unsigned char> b = {0x0F, 0xF0};

    auto result = manager.xorBuffers(a, b);

    ASSERT_EQ(result.size(), 2);
}