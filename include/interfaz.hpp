#ifndef INTERFAZ_HPP
#define INTERFAZ_HPP

#include <string>
#include <functional>

namespace interfaz {

  using Transformador = std::function<std::string(
    const std::string&,
    const std::string&
  )>;

  // Ejecuta una única ventana para el criptosistema indicado.
  void ejecutar(
    const std::string& titulo,
    Transformador cifrar,
    Transformador descifrar
  );

}

#endif

