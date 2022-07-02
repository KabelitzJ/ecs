#include <cstdlib>
#include <vector>
#include <string>
#include <chrono>
#include <iostream>

#include "types.hpp"
#include "registry.hpp"

struct vec3 {
  ecs::float32 x{};
  ecs::float32 y{};
  ecs::float32 z{};

  vec3& operator+=(const vec3& other) {
    x += other.x;
    y += other.y;
    z += other.z;

    return *this;
  }
};

vec3 operator/(const vec3& lhs, const ecs::float32 rhs) {
  return vec3{lhs.x / rhs, lhs.y / rhs, lhs.z / rhs};
}

vec3 operator*(const vec3& lhs, const ecs::float32 rhs) {
  return vec3{lhs.x * rhs, lhs.y * rhs, lhs.z * rhs};
}

struct transform {
  vec3 position{};
  vec3 rotation{};
  vec3 scale{};
};

struct rigidbody {
  vec3 velocity{};
  ecs::float32 mass{};
};

inline constexpr auto gravity = vec3{0.0f, -9.81f, 0.0f};

std::ostream& operator<<(std::ostream& output_stream, const vec3& vector) {
  return output_stream << vector.x << ", " << vector.y << ", " << vector.z;
}

void simulate(ecs::registry& registry, ecs::float32 delta_time) {
  auto view = registry.create_view<transform, rigidbody>();

  for (auto& [entity, transform, rigidbody] : view) {
    const auto acceleration = gravity / rigidbody.mass;
    rigidbody.velocity += acceleration * delta_time;
    transform.position += rigidbody.velocity * delta_time;
  }
}

void print(ecs::registry& registry) {
  auto view = registry.create_view<transform, rigidbody>();

  for (auto& [entity, transform, rigidbody] : view) {
    std::cout << transform.position << '\n';
  }
}

int main(int argc, char** argv) {
  auto cli_args = std::vector<std::string>{argv, argv + argc};

  auto registry = ecs::registry{};

  auto e = registry.create_entity();
  registry.add_component<transform>(e, vec3{0.0f, 40.0f, 0.0f}, vec3{}, vec3{});
  registry.add_component<rigidbody>(e, vec3{}, 1.0f);

  using clock = std::chrono::steady_clock;
  using duration = std::chrono::duration<ecs::float32>;

  auto last = clock::now();

  while (true) {
    const auto now = clock::now();
    const auto delta_time = std::chrono::duration_cast<duration>(now - last).count();
    last = now;

    simulate(registry, delta_time);
    print(registry);
  }

  registry.remove_component<transform>(e);
  registry.remove_component<rigidbody>(e);

  registry.destroy_entity(e);

  return EXIT_SUCCESS;
}

