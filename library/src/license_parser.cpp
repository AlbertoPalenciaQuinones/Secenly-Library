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
            if (pos >= size) throw std::runtime_error("[ERROR] Unexpected end of data (DER Reader)");
            return data[pos++];
        }
        
        // Lee una secuencia de bytes
        std::vector<uint8_t> ReadBytes(size_t len) {
            if (pos + len > size) throw std::runtime_error("[ERROR] Overflow (DER Reader)");
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
                throw std::runtime_error("[ERROR] Invalid length (DER Reader)");

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
            throw std::runtime_error("[ERROR] Expected UTF8String.");

        size_t len = r.ReadLength();
        auto bytes = r.ReadBytes(len);
        return std::string(bytes.begin(), bytes.end());
    }

    // Lector de GeneralizedTime para extraer las fechas de las licencias
    std::string ReadGeneralizedTime(DerReader& r) {
        if (r.ReadByte() != 0x18)
            throw std::runtime_error("[ERROR] Expected GeneralizedTime.");

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
            throw std::runtime_error("[ERROR] Parsing date into ASN.1 failed.");
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
            throw std::runtime_error("[ERROR] Expected Integer.");

        size_t len = r.ReadLength();
        auto bytes = r.ReadBytes(len);

        int32_t value = 0;
        for (uint8_t b : bytes)
            value = (value << 8) | b;

        return value;
    }

}

/**
 * Parsea la licencia en formato DER a un objeto de tipo licencia.
 *
 * El comportamiento actual es leer byte a byte el archivo de licencia .der.
 *
 * Este comportamiento puede ser modificado por cualquiera que utilice la 
 * biblioteca con el fin de que sea ajustable a las necesidades de cada usuario.
 *
 * La función cumple con las siguientes necesidades:
 *     - Lectura del archivo de licencia
 *     - Convertirlo a un objeto de tipo licencia
 *     - Identificar patrones
 * 
 * Debe saber que puede añadir distintos procedimientos escribiendo nuevas
 * funciones, pero todas ellas deben funcionar de forma correcta para no
 * romper el mecanismo de parseo de licencias.
 * 
 * Si usted decide añadir otro campo a la licencia, el cual es un tipo distinto
 * a los existentes, debe añadir la función de lectura de ese atributo. Tiene
 * como referencia la forma de leer enteros, cadenas y tiempos.
 * 
 * Otro aspecto a tener en cuenta es que la modificación en Secenly-Library
 * de la generación de la estructura de la licencia, crea la necesidad de su
 * modificación en la herramienta Secenly, siempre y cuando se haya optado
 * utilizarla a la hora de generar licencias de software.
 */
License ParseLicense(const uint8_t* data, size_t size) {
    // Lector de la sucesión de bytes codificados en DER
    DerReader r{data, size};

    // Primer bloque ha de ser una secuencia señalando a una sucesión de atributos
    if (r.ReadByte() != 0x30)
        throw std::runtime_error("[ERROR] Expected SEQUENCE.");

    size_t seqLen = r.ReadLength();
    size_t seqEnd = r.pos + seqLen;

    if (r.pos + seqLen > r.size) {
        throw std::runtime_error("[ERROR] Invalid SEQUENCE length.");
    }

    License lic;

    // Leer en orden los atributos de la licencia
    lic.id = ReadUtf8String(r);
    lic.creation_date = ParseGeneralizedTime(ReadGeneralizedTime(r));
    lic.expiration_date = ParseGeneralizedTime(ReadGeneralizedTime(r));
    lic.heartbeat_interval = ReadInteger(r);
    lic.notes = ReadUtf8String(r);

    if (r.pos != seqEnd)
        throw std::runtime_error("[ERROR] There are unexpected extra bytes y the license.");

    return lic;
}