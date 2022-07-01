#include "component_container.hpp"

namespace ecs {

template<typename Type>
component_container<Type>::component_container() noexcept
: _entity_to_index{},
  _index_to_entity{},
  _components{} {}

template<typename Type>
component_container<Type>::component_container(component_container&& other) noexcept
: _entity_to_index{std::move(other._entity_to_index)},
  _index_to_entity{std::move(other._index_to_entity)},
  _components{std::move(other._components)} {}

template<typename Type>
component_container<Type>& component_container<Type>::operator=(component_container&& other) noexcept {
  if (this != &other) {
    _entity_to_index = std::move(other._entity_to_index);
    _index_to_entity = std::move(other._index_to_entity);
    _components = std::move(other._components);
  }

  return *this;
}

template<typename Type>
void component_container<Type>::remove(const entity& entity) {
  if (const auto entry = _entity_to_index.find(entity); entry != _entity_to_index.cend()) {
    const auto index = entry->second;

    auto& back = _components.back();
    auto& current = _components[index];

    using std::swap;

    swap(current, back);

    _components.pop_back();

    const auto& back_entity = _index_to_entity.at(_components.size());
    _entity_to_index.at(back_entity) = index;

    _index_to_entity.erase(index);
    _entity_to_index.erase(entity);

    std::cout << "_components.size(): " << _components.size() << '\n';
  }
}

template<typename Type>
template<typename... Args>
component_container<Type>::reference component_container<Type>::add(const entity& entity, Args&&... args) {
  const auto index = _components.size();

  _entity_to_index.emplace(std::make_pair(entity, index));
  _index_to_entity.emplace(std::make_pair(index, entity));

  _components.push_back(std::make_unique<value_type>(std::forward<Args>(args)...));

  std::cout << "_components.size(): " << _components.size() << '\n';

  return *_components.back();
}

template<typename Type>
bool component_container<Type>::contains(const entity& entity) const {
  return _entity_to_index.contains(entity) && _index_to_entity.at(_entity_to_index.at(entity)) == entity;
}

} // namespace ecs
