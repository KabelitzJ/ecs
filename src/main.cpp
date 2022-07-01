#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>

#include "types.hpp"
#include "registry.hpp"

struct vec3 {
  ecs::float32 x{};
  ecs::float32 y{};
  ecs::float32 z{};
};

int main(int argc, char** argv) {
  auto cli_args = std::vector<std::string>{argv, argv + argc};

  auto reg = ecs::registry{};

  auto e1 = reg.create_entity();

  return EXIT_SUCCESS;
}

