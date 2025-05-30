#pragma once

#include <ecs/Entity.hpp>
#include <ecs/System.hpp>

#include <iostream>
#include <memory>

#include <ProjectConfig.h>

class ECS;

class SystemManager {
  std::shared_ptr<System> systems[MAX_SYSTEMS] = {NULL};
  Signature system_signatures[MAX_SYSTEMS] = {0};
  std::shared_ptr<ComponentManager> component_manager;

public:
  SystemManager(std::shared_ptr<ComponentManager> manager) {
    component_manager = manager;
  }

  template <typename T> std::shared_ptr<T> add_system(Signature signature) {
    std::cout << "Registering system '" << typeid(T).name()
              << "' with signature " << signature << std::endl;
    std::shared_ptr<System> sys = std::make_shared<T>();
    for (int i = 0; i < MAX_SYSTEMS; i++) {
      if (systems[i] == NULL) {
        systems[i] = sys;
        system_signatures[i] = signature;
        break;
      }
    }
    sys->set_component_manager(component_manager);
    return std::static_pointer_cast<T>(sys);
  }

  void entity_changed(Entity id, Signature sig) {
    // std::cout << "Entity " << id << " has changed " << sig << std::endl;
    for (int i = 0; i < MAX_SYSTEMS; i++) {
      if (systems[i] != NULL) {
        if ((system_signatures[i] & sig) == system_signatures[i]) {
          systems[i]->register_entity(id);
        } else {
          systems[i]->remove_entity(id);
        }
      }
    }
  }

  void entity_removed(Entity id) {
    for (int i = 0; i < MAX_SYSTEMS; i++) {
      if (systems[i] != NULL)
        systems[i]->remove_entity(id);
    }
  }
};
