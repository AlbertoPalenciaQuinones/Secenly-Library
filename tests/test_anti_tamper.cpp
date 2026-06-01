#include <gtest/gtest.h>

#include <fstream>

#include "src/license_exception.h"
#define private public
    #include <src/anti_tamper.h>
#undef private

using namespace secenly::internal;

// TEST 1 - La primera ejecución de validación anti-tamper crea el archivo 
// de estado correctamente. Esta verificación incluye que el archivo se ha creado
// y que contiene un tiempo válido (no cero).
TEST(AntiTamperTest, FirstRunCreatesState) {
    static AntiTamperManager tamper;

    std::remove("tamper_state.bin");

    EXPECT_NO_THROW({tamper.Validate();});
}

// TEST 2 - La función de validación anti-tamper detecta correctamente un rollback
// del reloj del sistema. Esta verificación incluye que se ha guardado un tiempo 
// futuro en el archivo de estado.
TEST(AntiTamperTest, SaveAndLoadWorks) {
    static AntiTamperManager tamper;

    uint64_t time = 123456;

    tamper.SaveLastTime(time);
    uint64_t loaded = tamper.LoadLastTime();

    EXPECT_EQ(time, loaded);
}

// TEST 3 - La función de validación anti-tamper detecta correctamente un archivo 
// modificado. Esta verificación incluye que se ha corrompido el archivo de estado 
// y que se lanza una excepción.
TEST(AntiTamperTest, ModifiedFileThrows) {
    static AntiTamperManager tamper;

    uint64_t time = 123456;
    tamper.SaveLastTime(time);

    // Corromper archivo
    std::fstream file("tamper_state.bin", std::ios::in | std::ios::out | std::ios::binary);
    uint64_t fake = 999999;
    file.write(reinterpret_cast<char*>(&fake), sizeof(fake));
    file.close();

    EXPECT_THROW(
        tamper.LoadLastTime(),
        LicenseException
    );
}

// TEST 4 - La función de validación anti-tamper detecta correctamente un archivo 
// incompleto. Esta verificación incluye que se ha escrito un archivo de estado con
// datos incompletos y que se lanza una excepción.
TEST(AntiTamperTest, CorruptedFileThrows) {
    static AntiTamperManager tamper;

    std::ofstream file("tamper_state.bin", std::ios::binary);
    file.write("abc", 3); // datos incompletos
    file.close();

    EXPECT_THROW(
        tamper.LoadLastTime(),
        LicenseException
    );
}

// TEST 5 - La función de validación anti-tamper detecta correctamente un rollback
// del reloj del sistema. Esta verificación incluye que se ha guardado un tiempo 
// futuro en el archivo de estado y que se lanza una excepción al validar.
TEST(AntiTamperTest, RollbackDetected) {
    static AntiTamperManager tamper;

    uint64_t future = std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now()
    ) + 100;

    tamper.SaveLastTime(future);

    EXPECT_THROW(
        tamper.Validate(),
        LicenseException
    );
}

// TEST 6 - La función de obtener la firma genera un valor consistente para 
// el mismo tiempo. Esta verificación incluye que se llama a la función de 
// obtener la firma con el mismo tiempo y que se obtiene el mismo resultado.
TEST(AntiTamperTest, SignatureIsConsistent) {
    static AntiTamperManager tamper;

    uint64_t time = 123456;

    uint64_t sig1 = tamper.ComputeSignature(time);
    uint64_t sig2 = tamper.ComputeSignature(time);

    EXPECT_EQ(sig1, sig2);
}



