#ifndef PERMUTACION_SERIES_HPP
#define PERMUTACION_SERIES_HPP

#include <string>

namespace permutacion_series {

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

