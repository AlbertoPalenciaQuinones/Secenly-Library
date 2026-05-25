# Secenly Library
Esta biblioteca forma parte del sistema **Secenly** y fundamentalmente, está diseñada para cumplir con la validación de licencias. La herramienta **Secenly** se encarga de realizar todo tipo de acciones con las licencias: creación, eliminación, modificación y consulta de licencias (entre otras). Además, ella se encarga de generar el **ContentInfo**, el cual contiene el **SignedData**, el cual contiene la licencia, haciendo uso de la serialización en formato *ASN.1* y codificación en *DER*.

# Estructura  
La biblioteca de Secenly se divide en 2 modulos: la parte de la **API pública**, que contiene los métodos que los usuarios pueden utilizar de la biblioteca y la parte interna de la biblioteca, que contiene el **código interno** que el usuario no debe utilizar en su software propietario. Ambas partes se dividen en carpetas, situando la API pública en */include* y la parte interna de la biblioteca en */src*.

Para utilizar la biblioteca en el software que quiera licenciar, únicamente debe incluir la interfaz (cabecera) de la clase **license_api** en el software. De esta forma, sin modificar la estructura que sigue la biblioteca, puede comenzar a utilizar las funciones de la API pública para validar las licencias.

# Funcionamiento
Al utilizar la librería como mecanismo de validación de licencias, ha de tener en cuenta los métodos que pertenecen a la **API pública**, los cuales son sirven para realizar dicho proceso de validación desde cualquier software propietario. Las funciones pertenecientes a la API pública se encuentran dentro de la clase LicenseEngine, cuyos archivos son: *license_engine.cpp y license_engine.h*.

Las funciones que pueden ser utilizadas desde los usuarios para realizar dicho proceso son:
- `ObtainLicense`    →  obtiene la licencia parseándola a un **objeto c++** para realizar los mecanismos de validación.
- `ValidateInitial`  →  realiza la primera validación de la licencia, comprobando que sus campos son correctos. Estos procesos son: **comprobación de que el identificador de licencia coincide con el asociado al equipo** y **comprobación de licencia expirada**.
- `ValidateRuntime`  → valida una licencia en **tiempo de ejecución** para asegurar que una licencia no haya expirado en tiempo de ejecución, obligando detener el software.

# Modificación y personalización
A continuación se expone toda la información relevante sobre **cómo modificar la biblioteca** según las necesidades de cada usuario. La librería es **open-source**, lo que significa que puede ser utilizada libremente y adaptada a distintos entornos o proyectos.

Actualmente, la biblioteca incluye diversos mecanismos de seguridad básicos, como:
- Generación de identificadores de licencia
- Validación de licencias
- Procesamiento y parsing de datos

Sin embargo, algunos de estos procesos han sido diseñados para ser **personalizables**, permitiendo adaptarlos al comportamiento deseado.

Si se quieren llevar a cabo modificaciones en la librería, se recomienda que antes se comprenda el flujo general de ella, leer los comentarios del código fuente e identificar las partes críticas del código. Muchas funciones incluyen comentarios detallados explicando su funcionamiento y sugiriendo posibles modificaciones.

# Componentes personalizables
Las siguientes funciones han sido diseñadas para poder modificarse según las necesidades del usuario:

- `library/manager/license_manager.cpp` → `GenerateLicenseId`
- `library/manager/hardware_manager.cpp` → `ObtainHwid`
- `library/manager/product_manager.cpp` → `Initialize`
- `library/license/license_parser.cpp` → `ParseLicense`
- `library/license/license_service.cpp` → `ValidateLicense`
- `library/license/license.h` → `LicenseError` (enum)
- `library/license/license_exception.h` → `License` (struct)

Estas funciones contienen comentarios explicativos directamente en el código, por lo que no se detalla aquí su funcionamiento. Por motivos de seguridad e integridad, se recomienda **no modificar**: procesos de validación criptográfica (CMS / firma), lógica interna de verificación de integridad y estructuras críticas del sistema sin comprender su impacto. Además, al modificar la biblioteca debe tener en cuenta que puede romperse la compatibilidad con licencias existentes y que cambios en la generación del ID afectan a todo el sistema.
