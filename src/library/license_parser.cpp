#include "license_parser.h"
#include <stdexcept>
#include <vector>
#include <sstream>
#include <iomanip>

// Estructura para leer el binario de DER. Recorre un buffer leyendo toda la
// sucesión de bytes de forma ordenada. Funciona como un cursor de lectura.
struct DerReader {
    const uint8_t* data;
    size_t size;
    size_t pos = 0;

    // Lee un byte y avanza su posición en 1
    uint8_t readByte() {
        if (pos >= size) throw std::runtime_error("EOF");
        return data[pos++];
    }
    
    // Lee un bloque de bytes
    std::vector<uint8_t> readBytes(size_t len) {
        if (pos + len > size) throw std::runtime_error("Overflow");
        std::vector<uint8_t> out(data + pos, data + pos + len);
        pos += len;
        return out;
    }

    // Lee la longitud de la secuencia de bytes
    size_t readLength() {
        uint8_t first = readByte();

        if ((first & 0x80) == 0)
            return first;

        size_t numBytes = first & 0x7F;
        if (numBytes == 0 || numBytes > 4)
            throw std::runtime_error("Invalid length");

        size_t len = 0;
        for (size_t i = 0; i < numBytes; i++) {
            len = (len << 8) | readByte();
        }

        return len;
    }
};

// Funciones para parsear las licencias (lectura y conversión de datos)

// Lector de Utf8String para extraer el id y las notas de la licencia
std::string readUtf8String(DerReader& r) {
    if (r.readByte() != 0x0C)
        throw std::runtime_error("Expected UTF8String");

    size_t len = r.readLength();
    auto bytes = r.readBytes(len);
    return std::string(bytes.begin(), bytes.end());
}

// Lector de GeneralizedTime para extraer las fechas de las licencias
std::string readGeneralizedTime(DerReader& r) {
    if (r.readByte() != 0x18)
        throw std::runtime_error("Expected GeneralizedTime");

    size_t len = r.readLength();
    auto bytes = r.readBytes(len);
    return std::string(bytes.begin(), bytes.end());
}

// Parser de GeneralizedTime para convertir el tiempo
std::chrono::system_clock::time_point parseGeneralizedTime(const std::string& str) {
    std::tm tm = {};

    std::istringstream ss(str);
    ss >> std::get_time(&tm, "%Y%m%d%H%M%SZ");

    if (ss.fail()) {
        throw std::runtime_error("Error parseando fecha ASN.1");
    }

    // Convertir a time_point (UTC)
    std::time_t time = timegm(&tm);

    return std::chrono::system_clock::from_time_t(time);
}

// Lector de enteros para extraer el intervalo del latido de la licencia
int32_t readInteger(DerReader& r) {
    if (r.readByte() != 0x02)
        throw std::runtime_error("Expected INTEGER");

    size_t len = r.readLength();
    auto bytes = r.readBytes(len);

    int32_t value = 0;
    for (uint8_t b : bytes)
        value = (value << 8) | b;

    return value;
}

// Parser principal de la licencia
License parseLicense(const uint8_t* data, size_t size) {
    DerReader r{data, size};

    if (r.readByte() != 0x30)
        throw std::runtime_error("Expected SEQUENCE");

    size_t seqLen = r.readLength();
    size_t seqEnd = r.pos + seqLen;

    License lic;

    lic.id = readUtf8String(r);
    lic.creation_date = parseGeneralizedTime(readGeneralizedTime(r));
    lic.expiration_date = parseGeneralizedTime(readGeneralizedTime(r));
    lic.last_use_date = parseGeneralizedTime(readGeneralizedTime(r));
    lic.heartbeat_interval = readInteger(r);
    lic.notes = readUtf8String(r);

    if (r.pos != seqEnd)
        throw std::runtime_error("Unexpected trailing data");

    return lic;
}