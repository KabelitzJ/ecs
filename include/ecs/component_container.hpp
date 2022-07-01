#ifndef ECS_COMPONENT_CONTAINER_HPP_
#define ECS_COMPONENT_CONTAINER_HPP_

#include <memory>
#include <unordered_map>
#include <vector>

#include "entity.hpp"

namespace ecs {

class component_container_base {

public:

  virtual ~component_container_base() = default;

  virtual void remove(const entity& entity) = 0;

}; // class component_container_base

template<typename Type>
class component_container : public component_container_base {

public:

  using value_type = Type;
  using size_type = std::size_t;
  using reference = value_type&;
  using const_reference = const value_type&;

  component_container() noexcept;

  component_container(const component_container& other) = delete;

  component_container(component_container&& other) noexcept;

  ~component_container() override = default;

  component_container& operator=(const component_container& other) = delete;

  component_container& operator=(component_container&& other) noexcept;

  void remove(const entity& entity) override;

  template<typename... Args>
  reference add(const entity& entity, Args&&... args);

  bool contains(const entity& entity) const;

private:

  std::unordered_map<entity, size_type> _entity_to_index{};
  std::unordered_map<size_type, entity> _index_to_entity{};
  std::vector<std::unique_ptr<value_type>> _components{};

}; // class component_container

} // namespace ecs

#include "component_container.inl"

#endif // ECS_COMPONENT_CONTAINER_HPP_
