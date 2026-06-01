#include <gtest/gtest.h>

#include "src/license_manager.h"
#include "src/utils/crypto_utils.h"

// TEST 1 - Al pasar una cadena de hexadecimal por el flujo: hex -> bytes -> hex, 
// se verifica que el hexadecimal inicial coincida con el resultante del flujo.
TEST(LicenseOperationsTest, HexToBytesAndBack) {
    std::string hex = "deadbeef";
    auto bytes = CryptoUtils::HexToBytes(hex);
    auto result = CryptoUtils::BytesToHex(bytes);

    EXPECT_EQ(hex, result);
}

// TEST 2 - La conversión de hexadecimal a bytes debe fallar cuando se le pasan 
// caracteres inválidos. En el tyest, se le comprueba mandándole la letra Z.
TEST(LicenseOperationsTest, HexToBytesInvalidCharacter) {
    EXPECT_THROW({CryptoUtils::HexToBytes("ZZ");}, std::invalid_argument);
}

// TEST 3 - Comprobar que la operación XOR se realiza correctamente. Para ello, 
// se realiza la operación con cadenas simples esperando su resultado correcto.
TEST(LicenseOperationsTest, XorOperatesWell) {
    std::vector<unsigned char> a = {0xFF, 0x00};
    std::vector<unsigned char> b = {0x0F, 0xF0};

    auto result = CryptoUtils::XorBuffers(a, b);

    ASSERT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], 0xF0);
    EXPECT_EQ(result[1], 0xF0);
}

// TEST 4 - Al utilizar cadenas de distinto tamaño, se espera que se lanze una
// excepción en tiempo de ejecución.
TEST(LicenseOperationsTest, XorDifferentSizes) {
    std::vector<unsigned char> a = {0xFF, 0x00, 0xAA};
    std::vector<unsigned char> b = {0x0F, 0xF0};

    EXPECT_THROW(
        CryptoUtils::XorBuffers(a, b),
        std::invalid_argument
    );
}
