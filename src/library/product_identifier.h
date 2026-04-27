#ifndef PRODUCT_IDENTIFIER_H
#define PRODUCT_IDENTIFIER_H

#include <string>
#include <vector>

class ProductIdentifier {
public:
    ProductIdentifier();

    bool initialize();
    std::vector<unsigned char> readSeedBytes();
    std::string getProductId() const;
    void setProductId(const std::string& hwid);
    void setPath(const std::string& p);

private:
    std::string product_id;
    std::vector<unsigned char> seed;
    std::string path;

    bool hasSeed();
    bool createSeed();
};

#endif // PRODUCT_IDENTIFIER_H