#ifndef PROCESAMIENTO_HPP
#define PROCESAMIENTO_HPP

#include <string>

namespace procesamiento {

  // Normaliza una entrada española y conserva únicamente A-Z.
  std::string normalizar(const std::string& texto);

}

#endif

