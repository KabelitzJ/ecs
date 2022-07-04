#ifndef ECS_ENTITY_HPP_
#define ECS_ENTITY_HPP_

#include <iostream>
#include <bitset>

#include "types.hpp"

namespace ecs {

class entity final {

  friend struct std::hash<entity>;

  friend class registry;

  friend bool operator==(const entity& lhs, const entity& rhs) noexcept;
  friend std::ostream& operator<<(std::ostream& output_stream, const entity& entity);

  using id_type = uint32;
  using version_type = uint16;
  using value_type = uint32;

  inline static constexpr auto id_mask = value_type{0xFFFFF};
  inline static constexpr auto version_mask = value_type{0xFFF};
  inline static constexpr auto version_shift = std::size_t{12}; 

public:

  static const entity null;

  entity(const entity& other) noexcept = default;

  entity(entity&& other) noexcept;

  ~entity() noexcept = default;

  entity& operator=(const entity& other) noexcept = default;

  entity& operator=(entity&& other) noexcept;

private:

  entity() noexcept;

  entity(const id_type id, const version_type version) noexcept;

  id_type _id() const noexcept;

  version_type _version() const noexcept;

  void _increment_version() noexcept;

  value_type _value{};

}; // class entity

bool operator==(const entity& lhs, const entity& rhs) noexcept;

std::ostream& operator<<(std::ostream& output_stream, const entity& entity);

} // namespace ecs

template<>
struct std::hash<ecs::entity> {

  inline std::size_t operator()(const ecs::entity& entity) const noexcept {
    return std::hash<ecs::entity::value_type>{}(entity._value);
  }

}; // struct std::hash

#endif // ECS_ENTITY_HPP_
