#ifndef LICENSE_H
#define LICENSE_H

#include <chrono>
#include <cstdint>
#include <string>

/**
 * El objeto de la licencia.
 *
 * Este objeto puede ser modificado por cualquiera que utilice la biblioteca, 
 * añadiendo o eliminando los atributos que quiera
 * 
 * Debe saber que puede añadir nuevos atributos a la licencia, supone que
 * también se haga en la herramienta Secenly, siempre y cuando se haya optado
 * utilizarla a la hora de generar licencias de software.
 * 
 * Es obligatorio seguir coherencia con la herramienta de generación de 
 * licencias y la biblioteca, ya que ambos deben manejar los mismos atributos.
 */
struct License {
    std::string id;
    std::chrono::system_clock::time_point creation_date;
    std::chrono::system_clock::time_point expiration_date;
    int32_t heartbeat_interval;
    std::string notes;
};

#endif // LICENSE_H