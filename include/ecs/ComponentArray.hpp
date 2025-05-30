#pragma once

#include <ecs/Entity.hpp>
#include <functional>
#include <iostream>
#include <unordered_map>
// #include <memory>

class ECS;

class ComponentArrayInterface {
public:
  virtual ~ComponentArrayInterface() = default;
  virtual void entity_destroyed(Entity id) = 0;
};

template <typename T> class ComponentArray : public ComponentArrayInterface {
  T components[MAX_ENTITIES];
  int num_components = 0;
  std::unordered_map<Entity, int> entity_to_component_idx;
  std::unordered_map<int, Entity> component_idx_to_entity;

public:
  void add_entity(Entity id, T data) {
    // std::cout << "Adding entity (" << id << ") data to spot " <<
    // num_components << std::endl;

    entity_to_component_idx[id] = num_components;
    component_idx_to_entity[num_components] = id;

    components[num_components] = data;

    num_components++;
  };

  void remove_entity(Entity id) {
    int old_component_idx = entity_to_component_idx[id];
    // std::cout << "Removing entity (" << id << ") data from spot " <<
    // old_component_idx << std::endl;

    components[old_component_idx] = components[num_components - 1];

    // Entity swap_entity = component_idx_to_entity[num_components-1]; // When
    // an entity is destroyed, the latest entity becomes the one that was just
    // destroyed
    entity_to_component_idx[id] = old_component_idx;
    component_idx_to_entity[old_component_idx] = id;

    num_components--;
  };

  T *get_data(Entity id) {
    // std::cout << "Getting Entity (" << id << ") data from spot " <<
    // entity_to_component_idx[id] << std::endl;
    return &components[entity_to_component_idx[id]];
  }

  T *get_editable_data(Entity id) {
    // std::cout << "Getting Entity (" << id << ") data from spot " <<
    // entity_to_component_idx[id] << std::endl;
    return &components[entity_to_component_idx[id]];
  }

  T get_data_from_idx(int idx) { return components[idx]; }

  int get_num_components() { return num_components; }

  int get_entity_from_idx(int i) { return component_idx_to_entity[i]; }

  void print_carray() {
    std::cout << "Num of components: " << num_components << std::endl;
    for (int i = 0; i < num_components; i++) {
      std::cout << "Component " << i << " (entity "
                << component_idx_to_entity[i] << ")" << std::endl;
    }
  }

  void entity_destroyed(Entity id) override { remove_entity(id); }

  /** Quick Sort */
  // Eugh... can we make this faster ?
  void swap_components(int idx_1, int idx_2) {
    // std::cout << "Swapping" << std::endl;
    T comp_1 = components[idx_1];
    T comp_2 = components[idx_2];
    Entity holder_1 = component_idx_to_entity[idx_1];
    Entity holder_2 = component_idx_to_entity[idx_2];

    components[idx_1] = components[idx_2];
    components[idx_2] = comp_1;
    component_idx_to_entity[idx_1] = holder_2;
    component_idx_to_entity[idx_2] = holder_1;
    entity_to_component_idx[holder_1] = idx_2;
    entity_to_component_idx[holder_2] = idx_1;
  }

  void
  insertion_sort(std::function<int(T, T, Entity, Entity, ECS *ecs)> compare,
                 ECS *ecs) {
    for (int i = 1; i < num_components; i++) {
      Entity e2 = component_idx_to_entity[i];

      for (int k = i - 1; k > 0; k--) {
        if (compare(components[k - 1], components[k],
                    component_idx_to_entity[k - 1], e2, ecs) == 1) {
          swap_components(k - 1, k);
        } else {
          break;
        }
      }
    }
  }

  // void sort(int start, int end, std::function<int(T *, T *)> compare_fn) {
  //   for (int i = 1; i < num_components; i++) {
  //     int elems_index = i;
  //     for (int k = i - 1; k >= 0; k--) {
  //       if (compare_fn(&components[elems_index], &components[k]) == -1) {
  //         swap_components(elems_index, k);
  //         elems_index--;
  //       } else {
  //         break;
  //       }
  //     }
  //   }
  // }

  // void sort(int start, int end, std::function<int(T, T, ECS *)> compare_fn,
  //           ECS *ecs) {
  //
  //   if (start >= end || start < 0)
  //     return;
  //   int len = (end - start) + 1;
  //   if (len < 2) {
  //     return;
  //   }
  //
  //   // Partition
  //   int pivot_idx = start + len / 2;
  //   T pivot_value = components[pivot_idx];
  //   int pivot_will_go = pivot_idx;
  //
  //   for (int idx = start; idx <= end; idx++) {
  //
  //     if (idx == pivot_idx || idx == pivot_will_go) {
  //       continue;
  //     }
  //
  //     if (compare_fn(components[idx], pivot_value, ecs) > 0 &&
  //         idx < pivot_will_go) {
  //       swap_components(idx, pivot_will_go);
  //       if (pivot_will_go == pivot_idx) {
  //         pivot_idx = idx;
  //       }
  //       pivot_will_go--;
  //       idx--;
  //
  //     } else if (compare_fn(components[idx], pivot_value, ecs) <= 0 &&
  //                idx > pivot_will_go) {
  //       swap_components(idx, pivot_will_go);
  //       if (pivot_will_go == pivot_idx) {
  //         pivot_idx = idx;
  //       }
  //       pivot_will_go++;
  //       idx--;
  //     }
  //   }
  //
  //   if (compare_fn(pivot_value, components[pivot_will_go], ecs) == -1 &&
  //       pivot_idx < pivot_will_go) {
  //     pivot_will_go--;
  //   }
  //
  //   swap_components(pivot_will_go, pivot_idx);
  //
  //   // Do the rest of the sort
  //   sort(start, pivot_will_go - 1, compare_fn, ecs);
  //   sort(pivot_will_go + 1, end, compare_fn, ecs);
  //   return;
  // }
};
