#ifndef PLAYFAIR_HPP
#define PLAYFAIR_HPP

#include <string>

namespace playfair {

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

