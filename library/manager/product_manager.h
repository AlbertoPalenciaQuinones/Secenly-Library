#ifndef PRODUCT_IDENTIFIER_H
#define PRODUCT_IDENTIFIER_H

#include <string>
#include <vector>

class ProductManager {
public:
    bool Initialize(const std::string& hwid);
    std::vector<unsigned char> ReadSeedBytes() const;
    std::string GetProductId() const;
    void SetPath(const std::string& p);

private:
    std::vector<unsigned char> seed;
    std::string path;
    std::string product_id;

    bool HasSeed() const;
    bool CreateSeed();
};

#endif // PRODUCT_IDENTIFIER_H