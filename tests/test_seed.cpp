#include <gtest/gtest.h>
#include <filesystem>

#define private public
#include "../library/manager/product_manager.h"
#undef private

// TEST 1 - Al inicializar el identificador de producto, se comprueba que se 
// haga correctamente.
TEST(SeedTest, CreationOfSeedFile) {
    ProductManager product;
    std::string path = "/tmp/test_seed_creation";
    product.SetPath(path);

    std::filesystem::remove_all(path);
    std::filesystem::create_directory(path);

    EXPECT_TRUE(product.Initialize("AABB")); 

    std::filesystem::remove_all(path);
}

// TEST 2 - La semilla debe ser persistente. Al inicializar el producto (y por
// ello, la semilla), esa debe ser la misma que si se vuelve a inicializar el
// producto.
TEST(SeedTest, SeedIsPersistent) {
    ProductManager product;
    std::string path = "/tmp";
    product.SetPath(path);

    EXPECT_TRUE(product.Initialize("AABB"));

    auto seed1 = product.ReadSeedBytes();

    product.Initialize("AABB");

    auto seed2 = product.ReadSeedBytes();

    EXPECT_EQ(seed1, seed2);
}

// TEST 3 - La semilla que se almacena como atributo en ProductManager,
// debe tener una longitud correcta.
TEST(SeedTest, SeedHasCorrectSize) {
    ProductManager product;
    product.SetPath("/tmp");

    EXPECT_TRUE(product.Initialize("AABB"));

    std::vector<unsigned char> seed = product.ReadSeedBytes();

    EXPECT_FALSE(seed.empty());
    EXPECT_EQ(seed.size(), 256); 
}

// TEST 4 - Al generar 2 semillas distintas e inicializar el producto, las
// semillas generadas deben de ser distintas ya que es un método que genera
// una semilla aleatoria.
TEST(SeedTest, SeedIsRandom) {
    ProductManager p1;
    ProductManager p2;

    std::filesystem::create_directory("/tmp/test_seed1");
    std::filesystem::create_directory("/tmp/test_seed2");

    std::string path1 = "/tmp/test_seed1";
    std::string path2 = "/tmp/test_seed2";

    p1.SetPath(path1);
    p2.SetPath(path2);

    EXPECT_TRUE(p1.Initialize("AABB"));
    EXPECT_TRUE(p2.Initialize("AABB"));

    EXPECT_NE(p1.ReadSeedBytes(), p2.ReadSeedBytes()); 

    std::filesystem::remove_all(path1);
    std::filesystem::remove_all(path2);
}
