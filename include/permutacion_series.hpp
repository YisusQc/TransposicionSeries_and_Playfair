#ifndef TRANSPOSICION_HPP
#define TRANSPOSICION_HPP

#include <string>

namespace transposicion {

  std::string cifrar(
    const std::string& texto,
    const std::string& clave
  );

  std::string descifrar(
    const std::string& texto,
    const std::string& clave
  );

}

#endif

