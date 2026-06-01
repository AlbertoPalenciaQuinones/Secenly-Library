#include <gtest/gtest.h>
#include <filesystem>

#include "src/hardware_manager.h"
#include "src/product_manager.h"

using namespace secenly::internal;

// TEST 1 - La semilla debe ser persistente. Al inicializar el producto (y por
// ello, la semilla), esa debe ser la misma que si se vuelve a inicializar el
// producto.
TEST(SeedTest, SeedIsPersistent) {
    ProductManager product;
    HardwareManager hw;
    std::string path = "tests/resources/seed.dat";
    product.SetPath(path);

    EXPECT_TRUE(product.Initialize(hw.GetHwid()));

    auto seed1 = product.ReadSeedBytes();

    product.Initialize(hw.GetHwid());

    auto seed2 = product.ReadSeedBytes();

    EXPECT_EQ(seed1, seed2);
}

// TEST 2 - La semilla que se almacena como atributo en ProductManager,
// debe tener una longitud correcta.
TEST(SeedTest, SeedHasCorrectSize) {
    ProductManager product;
    HardwareManager hw;
    std::string path = "tests/resources/seed.dat";
    
    product.SetPath(path);

    EXPECT_TRUE(product.Initialize(hw.GetHwid()));

    std::vector<unsigned char> seed = product.ReadSeedBytes();

    EXPECT_FALSE(seed.empty());
    EXPECT_EQ(seed.size(), 256); 
}