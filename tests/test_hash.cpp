#include <gtest/gtest.h>

#include "library/license_manager.h"
#include "sha/SHA512.h"

// TEST 1 - El hash se genera con una longitud correcta. Esa longitud es de 128 
// caracteres en hexadecimal.
TEST(HashTest, GeneratesCorrectLength) {
    SHA512 sha;
    std::string input = "test_input";
    std::string hash = sha.hash(input);

    EXPECT_EQ(hash.length(), 128); 
}

// TEST 2 - El hash es consistente, es decir, de un mismo texto, se generan 2 
// hashes iguales.
TEST(HashTest, GeneratesConsistentHash) {
    SHA512 sha;
    std::string input = "test_input";
    std::string hash1 = sha.hash(input);
    std::string hash2 = sha.hash(input);

    EXPECT_EQ(hash1, hash2); 
}

