/*
#include "license.h"
#include <iostream>

extern "C" {
    #include "asn1/LicenseInfo.h"
    #include "asn1/SignedLicense.h"
    #include "asn1/der_encoder.h"
}

License::License() : heartbeat_interval(300), hash_algorithm("SHA256") {}

std::vector<uint8_t> License::serialize() {
    // Convertir los atributos de la clase License a la estructura ASN.1 LicenseInfo
    LicenseInfo_t *info = (LicenseInfo_t*)calloc(1, sizeof(LicenseInfo_t));

    // Asignar valores a la estructura ASN.1
    OCTET_STRING_fromBuf(&info->id, id.c_str(), id.length());
    OCTET_STRING_fromBuf(&info->product, product.c_str(), product.length());
    OCTET_STRING_fromBuf(&info->created_at, created_at.c_str(), created_at.length());
    OCTET_STRING_fromBuf(&info->expires_at, expires_at.c_str(), expires_at.length());

    // last_use_at es opcional
    if(!last_use_at.empty()) {
        OCTET_STRING_fromBuf(&info->last_use_at, last_use_at.c_str(), last_use_at.length());
    }

    OCTET_STRING_fromBuf(&info->hwid, hwid.c_str(), hwid.length());
    info->heartbeat_interval = heartbeat_interval;
    OCTET_STRING_fromBuf(&info->hash_algorithm, hash_algorithm.c_str(), hash_algorithm.length());

    if(!notes.empty()) {
        info->notes = (UTF8String_t*)calloc(1, sizeof(UTF8String_t));
        OCTET_STRING_fromBuf(info->notes, notes.c_str(), notes.length());
    }

    // Codificación DER
    uint8_t buffer[4096];
    asn_enc_rval_t ec = der_encode_to_buffer(&asn_DEF_LicenseInfo, info, buffer, sizeof(buffer));

    std::vector<uint8_t> output;
    if(ec.encoded != -1) {
        output.assign(buffer, buffer + ec.encoded);
    }

    // Liberar memoria
    ASN_STRUCT_FREE(asn_DEF_LicenseInfo, info);
    return output;
}

bool License::generate_binary(const std::string& filename) {
    std::vector<uint8_t> data = serialize();
    if(data.empty()) {
        std::cerr << "Error al serializar la licencia" << std::endl;
        return false;
    }

    SignedLicense_t *signed_license = (SignedLicense_t*)calloc(1, sizeof(SignedLicense_t));

    // Insertar la licencia serializada en la estructura SignedLicense
    OCTET_STRING_fromBuf(&signed_license->binary, (char*)data.data(), data.size());

    // GENERAR FIRMA REAL
    std::string sign = "dummy_signature"; // Aquí deberías generar una firma real
    OCTET_STRING_fromBuf(&signed_license->signature, (char*)sign.c_str(), sign.length());

    // Abrir el archivo para escribir en modo binario
    FILE *fp = fopen(filename.c_str(), "wb");
    if(!fp) {
        std::cerr << "Error al abrir el archivo para escribir" << std::endl;
        ASN_STRUCT_FREE(asn_DEF_SignedLicense, signed_license);
        return false;
    }

    // Codificar la estructura SignedLicense en formato DER y escribirla en el archivo
    der_encode_to_file(&asn_DEF_SignedLicense, signed_license, fp);

    fclose(fp);
    ASN_STRUCT_FREE(asn_DEF_SignedLicense, signed_license);
    return true;
}*/