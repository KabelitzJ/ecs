#ifndef ECS_REGISTRY_HPP_
#define ECS_REGISTRY_HPP_

#include <vector>
#include <queue>
#include <memory>

#include "entity.hpp"
#include "component_container.hpp"

namespace ecs {

class registry {

  using component_container_type = std::vector<std::unique_ptr<component_container_base>>;

public:

  using size_type = std::size_t;

  registry() = default;

  registry(const registry&) = delete;

  registry(registry&&) noexcept;

  ~registry() = default;

  registry& operator=(const registry&) = delete;

  registry& operator=(registry&&) noexcept;

  [[nodiscard]] entity create_entity();

  void destroy_entity(const entity& entity);

  [[nodiscard]] bool is_valid_entity(const entity& entity) const noexcept;

  template<typename Component, typename... Args>
  Component& add_component(const entity& entity, Args&&... args) {
    if (!is_valid_entity(entity)) {
      throw std::runtime_error{"Invalid entity"};
    }

    const auto component_id = _component_id<Component>();

    auto& container = *static_cast<component_container<Component>*>(_components[component_id].get());

    return container.add(entity, std::forward<Args>(args)...);
  }

  template<typename Component>
  void remove_component(const entity& entity) {
    if (!is_valid_entity(entity)) {
      throw std::runtime_error{"Invalid entity"};
    }

    const auto component_id = _component_id<Component>();

    _components[component_id]->remove(entity);
  }

  template<typename Component>
  bool has_component(const entity& entity) {
    if (!is_valid_entity(entity)) {
      throw std::runtime_error{"Invalid entity"};
    }

    const auto component_id = _component_id<Component>();

    auto& container = *static_cast<component_container<Component>*>(_components[component_id].get());

    return container.contains(entity);
  }

private:

  template<typename Component>
  size_type _component_id() {
    static const auto id = _register_component<Component>();
    return id; 
  }

  template<typename Component>
  size_type _register_component() {
    _components.push_back(std::make_unique<component_container<Component>>());
    return _current_component_id++;
  }

  std::vector<entity> _entities{};
  std::vector<size_type> _free_entities{};
  component_container_type _components{};
  size_type _current_component_id{};

}; // class registry;

} // namespace ecs

#endif // ECS_REGISTRY_HPP_
