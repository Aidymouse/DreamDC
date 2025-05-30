#pragma once

#include <ecs/ComponentArray.hpp>
#include <engine/Components.hpp>
#include <memory>
#include <typeindex>
#include <unordered_map>

#include <ProjectConfig.h>

class ComponentManager {
  std::shared_ptr<ComponentArrayInterface> component_arrays[MAX_COMPONENTS] = {
      NULL};
  std::unordered_map<std::type_index, int> type_to_component_array;
  // std::unordered_map<int, std::type_index> signature_index_to_component_type;

public:
  template <typename T> void add_component(COMP_SIG signature_index) {
    if (signature_index > MAX_COMPONENTS) {
      std::cout << "Trying to add component with sig (" << signature_index
                << ") above max (" << MAX_COMPONENTS << "). Not adding."
                << std::endl;
      return;
    }

    if (component_arrays[signature_index] != NULL) {
      std::cout << "Trying to add component with sig (" << signature_index
                << ") but one already exists. Not adding." << std::endl;
      return;
    } else {
      component_arrays[signature_index] = std::make_shared<ComponentArray<T>>();
      type_to_component_array[typeid(T)] = signature_index;
      // signature_index_to_component_type[i] = typeid(T);
      std::cout << "Added component " << typeid(T).name() << " at index "
                << signature_index << std::endl;
    }
  }

  template <typename T> T *get_component_data(Entity id) {
    return get_component_array<T>()->get_data(id);
  }

  template <typename T> T *get_editable_component_data(Entity id) {
    return get_component_array<T>()->get_editable_data(id);
  }

  template <typename T>
  std::shared_ptr<ComponentArray<T>> get_component_array() {
    return std::static_pointer_cast<ComponentArray<T>>(
        component_arrays[type_to_component_array[typeid(T)]]);
  }

  /*std::type_index get_type_for_signature(int sig_index) {*/
  /*	return signature_index_to_component_type[sig_index];*/
  /*}*/

  template <typename T> int get_signature_index_for_type() {
    return type_to_component_array[typeid(T)];
  }

  void entity_removed(Entity id) {
    for (int i = 0; i < MAX_COMPONENTS; i++) {
      if (component_arrays[i] != NULL)
        component_arrays[i]->entity_destroyed(id);
    }
  }
};
