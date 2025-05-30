#pragma once

#include <ecs/ComponentManager.hpp>
#include <ecs/Entity.hpp>
#include <memory>
#include <set>

class System {
public:
  std::set<Entity> registered_entities;
  std::shared_ptr<ComponentManager> component_manager;

  void register_entity(Entity id) { registered_entities.insert(id); }

  void remove_entity(Entity id) { registered_entities.erase(id); }

  void set_component_manager(std::shared_ptr<ComponentManager> manager) {
    component_manager = manager;
  }
};
