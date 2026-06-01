#ifndef PRODUCT_IDENTIFIER_H
#define PRODUCT_IDENTIFIER_H

#include <string>
#include <vector>

namespace secenly::internal {

class ProductManager {
public:
    bool Initialize(const std::string& hwid);

    std::vector<unsigned char> ReadSeedBytes() const;

    const std::string& GetProductId() const;

    void SetPath(const std::string& p);

private:
    bool HasSeed() const;

    std::string product_id;
    std::vector<unsigned char> seed;
    std::string path;
};

}

#endif // PRODUCT_IDENTIFIER_H