#include <gtest/gtest.h>
#include <filesystem>

#include "library/product_identifier.h"

TEST(SeedTest, CreationOfSeedFile) {
    ProductIdentifier product;
    std::string path = "/tmp/test_seed_creation";
    product.setPath(path);

    std::filesystem::remove_all(path);
    std::filesystem::create_directory(path);

    EXPECT_TRUE(product.initialize()); 

    std::filesystem::remove_all(path);
}

TEST(SeedTest, SeedIsPersistent) {
    ProductIdentifier product;
    std::string path = "/tmp";
    product.setPath(path);

    EXPECT_TRUE(product.initialize());

    auto seed1 = product.readSeedBytes();

    // Volver a crear (no debería cambiar)
    product.initialize();

    auto seed2 = product.readSeedBytes();

    EXPECT_EQ(seed1, seed2);
}

TEST(SeedTest, SeedHasCorrectSize) {
    ProductIdentifier product;
    product.setPath("/tmp");

    EXPECT_TRUE(product.initialize());

    std::vector<unsigned char> seed = product.readSeedBytes();

    EXPECT_FALSE(seed.empty());
    EXPECT_EQ(seed.size(), 256); 
}

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