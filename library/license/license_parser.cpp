#include "license_parser.h"

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace {
// Estructura para leer el binario de DER. Recorre un buffer leyendo toda la
// sucesión de bytes de forma ordenada. Funciona como un cursor de lectura.
struct DerReader {
    const uint8_t* data;
    size_t size;
    size_t pos = 0;

    // Lee un byte y avanza su posición en 1
    uint8_t ReadByte() {
        if (pos >= size) throw std::runtime_error("EOF");
        return data[pos++];
    }
    
    // Lee una secuencia de bytes
    std::vector<uint8_t> ReadBytes(size_t len) {
        if (pos + len > size) throw std::runtime_error("Overflow");
        std::vector<uint8_t> out(data + pos, data + pos + len);
        pos += len;
        return out;
    }

    // Lee la longitud de una secuencia de bytes
    size_t ReadLength() {
        uint8_t first = ReadByte();

        if ((first & 0x80) == 0)
            return first;

        size_t numBytes = first & 0x7F;
        if (numBytes == 0 || numBytes > 4)
            throw std::runtime_error("Invalid length");

        size_t len = 0;
        for (size_t i = 0; i < numBytes; i++) {
            len = (len << 8) | ReadByte();
        }

        return len;
    }
};

// FUNCIONES PARA PARSEAR LAS LICENCIAS:

// Lector de Utf8String para extraer el id y las notas de la licencia
std::string ReadUtf8String(DerReader& r) {
    if (r.ReadByte() != 0x0C)
        throw std::runtime_error("Expected UTF8String");

    size_t len = r.ReadLength();
    auto bytes = r.ReadBytes(len);
    return std::string(bytes.begin(), bytes.end());
}

// Lector de GeneralizedTime para extraer las fechas de las licencias
std::string ReadGeneralizedTime(DerReader& r) {
    if (r.ReadByte() != 0x18)
        throw std::runtime_error("Expected GeneralizedTime");

    size_t len = r.ReadLength();
    auto bytes = r.ReadBytes(len);
    return std::string(bytes.begin(), bytes.end());
}

// Parser de GeneralizedTime para convertir el tiempo
std::chrono::system_clock::time_point ParseGeneralizedTime(const std::string& str) {
    std::tm tm = {};

    std::istringstream ss(str);
    ss >> std::get_time(&tm, "%Y%m%d%H%M%SZ");

    if (ss.fail()) {
        throw std::runtime_error("Error parseando fecha ASN.1");
    }

    // Convertir el tiempo dependiendo del SO
    #ifdef _WIN32
        std::time_t time = _mkgmtime(&tm);
    #else
        std::time_t time = timegm(&tm);
    #endif

    return std::chrono::system_clock::from_time_t(time);
}

// Lector de enteros para extraer el intervalo del latido de la licencia
int32_t ReadInteger(DerReader& r) {
    if (r.ReadByte() != 0x02)
        throw std::runtime_error("Expected INTEGER");

    size_t len = r.ReadLength();
    auto bytes = r.ReadBytes(len);

    int32_t value = 0;
    for (uint8_t b : bytes)
        value = (value << 8) | b;

    return value;
}

}

// Parser principal de la licencia
License ParseLicense(const uint8_t* data, size_t size) {
    // Lector de la sucesión de bytes codificados en DER
    DerReader r{data, size};

    // Primer bloque ha de ser una secuencia señalando a una sucesión de atributos
    if (r.ReadByte() != 0x30)
        throw std::runtime_error("Expected SEQUENCE");

    size_t seqLen = r.ReadLength();
    size_t seqEnd = r.pos + seqLen;

    if (r.pos + seqLen > r.size) {
        throw std::runtime_error("Invalid SEQUENCE length");
    }

    License lic;

    // Leer en orden los atributos de la licencia
    lic.id = ReadUtf8String(r);
    lic.creation_date = ParseGeneralizedTime(ReadGeneralizedTime(r));
    lic.expiration_date = ParseGeneralizedTime(ReadGeneralizedTime(r));
    lic.heartbeat_interval = ReadInteger(r);
    lic.notes = ReadUtf8String(r);

    if (r.pos != seqEnd)
        throw std::runtime_error("Unexpected trailing data");

    return lic;
}