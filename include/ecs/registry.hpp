#ifndef ECS_REGISTRY_HPP_
#define ECS_REGISTRY_HPP_

#include <vector>
#include <queue>
#include <memory>
#include <iostream>
#include <typeindex>

#include "entity.hpp"
#include "component_container.hpp"
#include "view.hpp"

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

  template<component Component, typename... Args>
  Component& add_component(const entity& entity, Args&&... args) {
    if (!is_valid_entity(entity)) {
      throw std::runtime_error{"Invalid entity"};
    }

    const auto component_id = _component_id<Component>();

    auto& container = *static_cast<component_container<Component>*>(_components[component_id].get());

    return container.add(entity, std::forward<Args>(args)...);
  }

  template<component Component>
  void remove_component(const entity& entity) {
    if (!is_valid_entity(entity)) {
      throw std::runtime_error{"Invalid entity"};
    }

    const auto component_id = _component_id<Component>();

    _components[component_id]->remove(entity);
  }

  template<component Component>
  bool has_component(const entity& entity) {
    if (!is_valid_entity(entity)) {
      throw std::runtime_error{"Invalid entity"};
    }

    const auto component_id = _component_id<Component>();

    auto& container = *static_cast<component_container<Component>*>(_components[component_id].get());

    return container.contains(entity);
  }

  template<component Component>
  Component& get_component(const entity& entity) {
    if (!is_valid_entity(entity)) {
      throw std::runtime_error{"Invalid entity"};
    }

    const auto component_id = _component_id<Component>();

    if constexpr (std::is_const_v<Component>) {
      const auto& container = *static_cast<component_container<Component>*>(_components[component_id].get());
      return container.get(entity);
    } else {
      auto& container = *static_cast<component_container<Component>*>(_components[component_id].get());
      return container.get(entity);
    }
  }

  template<component Component, typename Function>
  void patch_component(const entity& entity, Function&& function) {
    if (!is_valid_entity(entity)) {
      throw std::runtime_error{"Invalid entity"};
    }

    const auto component_id = _component_id<Component>();

    auto& container = *static_cast<component_container<Component>*>(_components[component_id].get());
    return container.patch(entity, std::forward<Function>(function));
  }

  template<component... Components>
  view<Components...> create_view() {
    if constexpr (sizeof...(Components) == size_type{0}) {
      return view<Components...>{};
    } else {
      using view_container_type = view<Components...>::container_type;

      const auto component_filter = [&](const auto& entity){
        const auto has_components = std::initializer_list{has_component<Components>(entity)...};
        return std::all_of(has_components.begin(), has_components.end(), std::identity{});
      };

      auto view_entries = view_container_type{};

      for (const auto& entity : _entities | std::views::filter(component_filter)) {
        view_entries.emplace_back(std::forward_as_tuple(entity, get_component<Components>(entity)...));
      }

      return view<Components...>{view_entries};
    }
  }

private:

  template<component Component>
  size_type _component_id() {
    // [NOTE] KAJ 2022-07-01 16:27 - Const and non-const types should be stored in the same container
    return _generate_next_component_id<std::remove_const_t<Component>>(); 
  }

  template<component Component>
  size_type _generate_next_component_id() {
    static const auto id = _register_component<Component>();
    return id;
  }

  template<component Component>
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
