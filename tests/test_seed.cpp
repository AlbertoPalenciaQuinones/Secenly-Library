#include <gtest/gtest.h>
#include <filesystem>

#define private public
#include "library/product_identifier.h"
#undef private

// TEST 1 - Al inicializar el identificador de producto, se comprueba que se 
// haga correctamente.
TEST(SeedTest, CreationOfSeedFile) {
    ProductIdentifier product;
    std::string path = "/tmp/test_seed_creation";
    product.setPath(path);

    std::filesystem::remove_all(path);
    std::filesystem::create_directory(path);

    EXPECT_TRUE(product.initialize()); 

    std::filesystem::remove_all(path);
}

// TEST 2 - La semilla debe ser persistente. Al inicializar el producto (y por
// ello, la semilla), esa debe ser la misma que si se vuelve a inicializar el
// producto.
TEST(SeedTest, SeedIsPersistent) {
    ProductIdentifier product;
    std::string path = "/tmp";
    product.setPath(path);

    EXPECT_TRUE(product.initialize());

    auto seed1 = product.readSeedBytes();

    product.initialize();

    auto seed2 = product.readSeedBytes();

    EXPECT_EQ(seed1, seed2);
}

// TEST 3 - La semilla que se almacena como atributo en ProductIdentifier,
// debe tener una longitud correcta.
TEST(SeedTest, SeedHasCorrectSize) {
    ProductIdentifier product;
    product.setPath("/tmp");

    EXPECT_TRUE(product.initialize());

    std::vector<unsigned char> seed = product.readSeedBytes();

    EXPECT_FALSE(seed.empty());
    EXPECT_EQ(seed.size(), 256); 
}

// TEST 4 - Al generar 2 semillas distintas e inicializar el producto, las
// semillas generadas deben de ser distintas ya que es un método que genera
// una semilla aleatoria.
TEST(SeedTest, SeedIsRandom) {
    ProductIdentifier p1;
    ProductIdentifier p2;

    std::filesystem::create_directory("/tmp/test_seed1");
    std::filesystem::create_directory("/tmp/test_seed2");

    std::string path1 = "/tmp/test_seed1";
    std::string path2 = "/tmp/test_seed2";

    p1.setPath(path1);
    p2.setPath(path2);

    EXPECT_TRUE(p1.initialize());
    EXPECT_TRUE(p2.initialize());

    EXPECT_NE(p1.readSeedBytes(), p2.readSeedBytes()); 

    std::filesystem::remove_all(path1);
    std::filesystem::remove_all(path2);
}
