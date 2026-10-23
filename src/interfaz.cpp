#include "../include/interfaz.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <algorithm>
#include <array>
#include <exception>
#include <string>
#include <optional>
#include <stdexcept>
#include <cstddef>

namespace interfaz {

namespace {

constexpr unsigned int ANCHO = 1100;
constexpr unsigned int ALTO = 660;

const sf::Color FONDO(20, 22, 28);
const sf::Color PANEL(29, 32, 40);
const sf::Color CAMPO(37, 41, 51);
const sf::Color BORDE(62, 68, 82);
const sf::Color TEXTO(235, 237, 243);
const sf::Color SECUNDARIO(160, 167, 182);
const sf::Color ACENTO(104, 125, 230);
const sf::Color ERROR(245, 115, 115);


struct Campo {
  std::string etiqueta;
  std::string valor;
  sf::FloatRect limites;
  bool editable;
  bool enfocado = false;
  std::size_t cursor = 0;
  std::size_t ancla = 0;
  bool seleccionando = false;
  bool seleccionable = true;
};

struct Boton {
  std::string texto;
  sf::FloatRect limites;
};

bool contiene(const sf::FloatRect& r, sf::Vector2f punto) {
  return r.contains(punto);
}

sf::String a_sf_string(const std::string& texto) {
  return sf::String::fromUtf8(texto.begin(), texto.end());
}

void agregar_utf8(std::string& destino, char32_t c) {
  if (c <= 0x7F) {
    destino += static_cast<char>(c);
  } else if (c <= 0x7FF) {
    destino += static_cast<char>(0xC0 | (c >> 6));
    destino += static_cast<char>(0x80 | (c & 0x3F));
  } else if (c <= 0xFFFF) {
    destino += static_cast<char>(0xE0 | (c >> 12));
    destino += static_cast<char>(0x80 | ((c >> 6) & 0x3F));
    destino += static_cast<char>(0x80 | (c & 0x3F));
  } else if (c <= 0x10FFFF) {
    destino += static_cast<char>(0xF0 | (c >> 18));
    destino += static_cast<char>(0x80 | ((c >> 12) & 0x3F));
    destino += static_cast<char>(0x80 | ((c >> 6) & 0x3F));
    destino += static_cast<char>(0x80 | (c & 0x3F));
  }
}

bool cargar_fuente(sf::Font& fuente) {
  const std::array<std::string, 3> rutas = {
    "/usr/share/fonts/TTF/DejaVuSans.ttf",
    "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
    "/usr/share/fonts/TTF/LiberationSans-Regular.ttf"
  };

  for (const auto& ruta : rutas) {
    if (fuente.openFromFile(ruta)) {
      return true;
    }
  }

  return false;
}

std::size_t anterior_utf8(
    const std::string& texto,
    std::size_t posicion
) {
  if (posicion == 0) return 0;

  --posicion;

  while (posicion > 0 &&
       (static_cast<unsigned char>(texto[posicion]) & 0xC0) == 0x80) {
    --posicion;
  }

  return posicion;
}

std::size_t siguiente_utf8(
  const std::string& texto,
  std::size_t posicion
) {
  if (posicion >= texto.size()) return texto.size();

  ++posicion;

  while (posicion < texto.size() &&
       (static_cast<unsigned char>(texto[posicion]) & 0xC0) == 0x80) {
    ++posicion;
  }

  return posicion;
}

std::pair<std::size_t, std::size_t> rango_seleccion(
  const Campo& campo
) {
  return {
    std::min(campo.cursor, campo.ancla),
    std::max(campo.cursor, campo.ancla)
  };
}

void borrar_seleccion(Campo& campo) {
  const auto [inicio, fin] = rango_seleccion(campo);

  if (inicio != fin) {
    campo.valor.erase(inicio, fin - inicio);
    campo.cursor = inicio;
    campo.ancla = inicio;
  }
}

void insertar_texto(Campo& campo, const std::string& texto) {
  if (!campo.editable) return;

  borrar_seleccion(campo);

  campo.valor.insert(campo.cursor, texto);
  campo.cursor += texto.size();
  campo.ancla = campo.cursor;
}

void seleccionar_todo(Campo& campo) {
  campo.ancla = 0;
  campo.cursor = campo.valor.size();
}

void copiar_seleccion(const Campo& campo) {
  const auto [inicio, fin] = rango_seleccion(campo);

  if (inicio != fin) {
    sf::Clipboard::setString(
      a_sf_string(campo.valor.substr(inicio, fin - inicio))
    );
  }
}

void desactivar_campos(
  std::array<Campo, 3>& a,
  std::array<Campo, 3>& b
) {
  for (auto& campo : a) campo.enfocado = false;
  for (auto& campo : b) campo.enfocado = false;
}

void dibujar_texto(
  sf::RenderWindow& ventana,
  const sf::Font& fuente,
  const std::string& contenido,
  unsigned int tamano,
  sf::Color color,
  sf::Vector2f posicion
) {
  sf::Text texto(fuente, a_sf_string(contenido), tamano);
  texto.setFillColor(color);
  texto.setPosition(posicion);
  ventana.draw(texto);
}

void dibujar_campo(
  sf::RenderWindow& ventana,
  const sf::Font& fuente,
  Campo& campo
) {
  sf::RectangleShape caja;
  caja.setPosition(campo.limites.position);
  caja.setSize(campo.limites.size);
  caja.setFillColor(CAMPO);
  caja.setOutlineThickness(campo.enfocado ? 2.f : 1.f);
  caja.setOutlineColor(campo.enfocado ? ACENTO : BORDE);
  ventana.draw(caja);

  sf::String contenido = a_sf_string(campo.valor);

  const float x = campo.limites.position.x + 12.f;
  const float y = campo.limites.position.y
    + (campo.limites.size.y - 20.f) / 2.f;

  // Para mostrar la selección, se trabaja con posiciones Unicode.
  const auto contar_caracteres = [](const std::string& s,
                                  std::size_t bytes) {
    std::size_t cuenta = 0;
    for (std::size_t i = 0; i < std::min(bytes, s.size()); ++cuenta) {
      i = siguiente_utf8(s, i);
    }
    return cuenta;
  };

  if (campo.valor.empty() && campo.editable) {
    dibujar_texto(
      ventana, fuente, "Escribe aqui...", 16, SECUNDARIO, {x, y}
    );
    return;
  }

  sf::Text texto(fuente, contenido, 16);
  texto.setFillColor(TEXTO);
  texto.setPosition({x, y});

  const auto [inicio, fin] = rango_seleccion(campo);
  const std::size_t iniChar = contar_caracteres(campo.valor, inicio);
  const std::size_t finChar = contar_caracteres(campo.valor, fin);

  // Resaltar selección cuando todo el texto es visible.
  if (inicio != fin &&
    texto.getLocalBounds().size.x <= campo.limites.size.x - 24.f) {
    sf::Text antes(fuente, contenido.substring(0, iniChar), 16);
    sf::Text seleccionado(
      fuente,
      contenido.substring(iniChar, finChar - iniChar),
      16
    );

    const float xInicio = x + antes.getLocalBounds().size.x;
    const float ancho = seleccionado.getLocalBounds().size.x;

    sf::RectangleShape resaltado;
    resaltado.setPosition({xInicio, y});
    resaltado.setSize({ancho, 23.f});
    resaltado.setFillColor(ACENTO);
    ventana.draw(resaltado);
  }

  // Si la cadena es larga, muestra su parte inicial en esta versión.
  while (!texto.getString().isEmpty() &&
     texto.getLocalBounds().size.x > campo.limites.size.x - 24.f) {
    texto.setString(texto.getString().substring(
      0, texto.getString().getSize() - 1
    ));
  }

  ventana.draw(texto);
}

void dibujar_boton(
  sf::RenderWindow& ventana,
  const sf::Font& fuente,
  const Boton& boton
) {
  sf::RectangleShape forma;
  forma.setPosition(boton.limites.position);
  forma.setSize(boton.limites.size);
  forma.setFillColor(ACENTO);
  forma.setOutlineThickness(0.f);
  ventana.draw(forma);

  sf::Text texto(fuente, a_sf_string(boton.texto), 17);
  texto.setFillColor(sf::Color::White);

  const auto bounds = texto.getLocalBounds();
  texto.setPosition({
    boton.limites.position.x
      + (boton.limites.size.x - bounds.size.x) / 2.f
      - bounds.position.x,
    boton.limites.position.y
      + (boton.limites.size.y - bounds.size.y) / 2.f
      - bounds.position.y
  });

  ventana.draw(texto);
}

void dibujar_panel(
  sf::RenderWindow& ventana,
  const sf::Font& fuente,
  const std::string& titulo,
  float x,
  std::array<Campo, 3>& campos,
  const Boton& boton
) {
  sf::RectangleShape panel;
  panel.setPosition({x, 100.f});
  panel.setSize({510.f, 510.f});
  panel.setFillColor(PANEL);
  panel.setOutlineThickness(1.f);
  panel.setOutlineColor(BORDE);
  ventana.draw(panel);

  dibujar_texto(
    ventana, fuente, titulo, 23, TEXTO, {x + 24.f, 120.f}
  );

  for (auto& campo : campos) {
    dibujar_texto(
      ventana,
      fuente,
      campo.etiqueta,
      15,
      SECUNDARIO,
      {campo.limites.position.x, campo.limites.position.y - 27.f}
    );

    dibujar_campo(ventana, fuente, campo);
  }

  dibujar_boton(ventana, fuente, boton);
}

} // namespace

void ejecutar(
  const std::string& titulo,
  Transformador cifrar,
  Transformador descifrar
) {
  // Una sola instancia de ventana dentro de esta ejecución.
  sf::RenderWindow ventana(
    sf::VideoMode({ANCHO, ALTO}),
    titulo
  );

  ventana.setFramerateLimit(60);

  sf::Font fuente;
  if (!cargar_fuente(fuente)) {
    throw std::runtime_error(
      "No se encontro una fuente compatible. "
      "Instala ttf-dejavu o ajusta las rutas en interfaz.cpp."
    );
  }

  const float xIzquierda = 30.f;
  const float xDerecha = 560.f;

  std::array<Campo, 3> cifrado = {{
    {"Texto claro", "", {{xIzquierda + 24.f, 180.f}, {462.f, 65.f}}, true},
    {"Clave", "", {{xIzquierda + 24.f, 290.f}, {462.f, 55.f}}, true},
    {"Texto cifrado", "", {{xIzquierda + 24.f, 410.f}, {462.f, 100.f}}, false}
  }};

  std::array<Campo, 3> descifrado = {{
    {"Texto cifrado", "", {{xDerecha + 24.f, 180.f}, {462.f, 65.f}}, true},
    {"Clave", "", {{xDerecha + 24.f, 290.f}, {462.f, 55.f}}, true},
    {"Texto descifrado", "", {{xDerecha + 24.f, 410.f}, {462.f, 100.f}}, false}
  }};

  const Boton botonCifrar = {
    "Cifrar", {{xIzquierda + 24.f, 540.f}, {462.f, 48.f}}
  };

  const Boton botonDescifrar = {
    "Descifrar", {{xDerecha + 24.f, 540.f}, {462.f, 48.f}}
  };

  std::string estado;
  sf::Color colorEstado = SECUNDARIO;

  while (ventana.isOpen()) {
    while (const std::optional evento = ventana.pollEvent()) {
      if (evento->is<sf::Event::Closed>()) {
        ventana.close();
      }

      if (const auto* raton =
            evento->getIf<sf::Event::MouseButtonPressed>()) {
        if (raton->button == sf::Mouse::Button::Left) {
          const sf::Vector2f punto(
            static_cast<float>(raton->position.x),
            static_cast<float>(raton->position.y)
          );

          desactivar_campos(cifrado, descifrado);

          for (auto* grupo : {&cifrado, &descifrado}) {
            for (auto& campo : *grupo) {
              if (campo.seleccionable && contiene(campo.limites, punto)) {
                campo.enfocado = true;
                campo.cursor = campo.valor.size();
                campo.ancla = campo.cursor;
              }
            }
          }

          try {
            if (contiene(botonCifrar.limites, punto)) {
              cifrado[2].valor =
                cifrar(cifrado[0].valor, cifrado[1].valor);

              estado = "Cifrado completado.";
              colorEstado = sf::Color(130, 220, 160);
            } else if (
              contiene(botonDescifrar.limites, punto)
            ) {
              descifrado[2].valor =
                descifrar(
                  descifrado[0].valor,
                  descifrado[1].valor
                );

              estado = "Descifrado completado.";
              colorEstado = sf::Color(130, 220, 160);
            }
          } catch (const std::exception& e) {
            estado = e.what();
            colorEstado = ERROR;
          }
        }
      }

      if (const auto* entrada =
            evento->getIf<sf::Event::TextEntered>()) {
        if (entrada->unicode >= 32 && entrada->unicode != 127) {
          for (auto* grupo : {&cifrado, &descifrado}) {
            for (auto& campo : *grupo) {
              if (campo.editable && campo.enfocado) {
                std::string caracter;
                agregar_utf8(caracter, entrada->unicode);
                insertar_texto(campo, caracter);
              }
            }
          }
        }
      }

      if (const auto* tecla =
            evento->getIf<sf::Event::KeyPressed>()) {
        if (tecla->code == sf::Keyboard::Key::Escape) {
          ventana.close();
        }

        for (auto* grupo : {&cifrado, &descifrado}) {
          for (auto& campo : *grupo) {
            if (!campo.enfocado) continue;

            const bool ctrl = tecla->control;
            const bool shift = tecla->shift;

            if (ctrl && tecla->code == sf::Keyboard::Key::A) {
              seleccionar_todo(campo);
              continue;
            }

            if (ctrl && tecla->code == sf::Keyboard::Key::C) {
              copiar_seleccion(campo);
              continue;
            }

            if (ctrl && tecla->code == sf::Keyboard::Key::X) {
              copiar_seleccion(campo);
              if (campo.editable) borrar_seleccion(campo);
              continue;
            }

            if (ctrl && tecla->code == sf::Keyboard::Key::V) {
              if (campo.editable) {
                const sf::String pegado = sf::Clipboard::getString();
                auto textoPegado = pegado.toUtf8();
                insertar_texto(campo, std::string(textoPegado.begin(), textoPegado.end()));
              }
              continue;
            }

            if (tecla->code == sf::Keyboard::Key::Left) {
              campo.cursor = anterior_utf8(campo.valor, campo.cursor);
              if (!shift) campo.ancla = campo.cursor;
              continue;
            }

            if (tecla->code == sf::Keyboard::Key::Right) {
              campo.cursor = siguiente_utf8(campo.valor, campo.cursor);
              if (!shift) campo.ancla = campo.cursor;
              continue;
            }

            if (tecla->code == sf::Keyboard::Key::Home) {
              campo.cursor = 0;
              if (!shift) campo.ancla = campo.cursor;
              continue;
            }

            if (tecla->code == sf::Keyboard::Key::End) {
              campo.cursor = campo.valor.size();
              if (!shift) campo.ancla = campo.cursor;
              continue;
            }

            if (tecla->code == sf::Keyboard::Key::Backspace &&
              campo.editable) {
              const auto [inicio, fin] = rango_seleccion(campo);

              if (inicio != fin) {
                borrar_seleccion(campo);
              } else if (campo.cursor > 0) {
                const auto anterior =
                  anterior_utf8(campo.valor, campo.cursor);
                campo.valor.erase(
                  anterior, campo.cursor - anterior
                );
                campo.cursor = anterior;
                campo.ancla = anterior;
              }
              continue;
            }

            if (tecla->code == sf::Keyboard::Key::Delete &&
              campo.editable) {
              const auto [inicio, fin] = rango_seleccion(campo);

              if (inicio != fin) {
                borrar_seleccion(campo);
              } else if (campo.cursor < campo.valor.size()) {
                const auto siguiente =
                  siguiente_utf8(campo.valor, campo.cursor);
                campo.valor.erase(
                  campo.cursor, siguiente - campo.cursor
                );
                campo.ancla = campo.cursor;
              }
              continue;
            }
          }
        }
      }
    }

    ventana.clear(FONDO);

    dibujar_texto(
      ventana,
      fuente,
      titulo,
      30,
      TEXTO,
      {30.f, 30.f}
    );

    dibujar_panel(
      ventana, fuente, "Cifrado",
      xIzquierda, cifrado, botonCifrar
    );

    dibujar_panel(
      ventana, fuente, "Descifrado",
      xDerecha, descifrado, botonDescifrar
    );

    dibujar_texto(
      ventana,
      fuente,
      estado,
      14,
      colorEstado,
      {30.f, 720.f}
    );

    ventana.display();
  }
}

} // namespace interfaz

