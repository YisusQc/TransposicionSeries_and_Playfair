#include "../include/interfaz.hpp"
#include "../include/permutacion_series.hpp"

#include <exception>
#include <iostream>

int main() {
  try {
    interfaz::ejecutar(
      "Cifrado Permutacion de Series",
      permutacion_series::cifrar,
      permutacion_series::descifrar
    );
  } catch (const std::exception& e) {
    std::cerr << "Error al iniciar la interfaz: "
              << e.what() << '\n';
    return 1;
  }

  return 0;
}

