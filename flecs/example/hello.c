#include <stdio.h>

#include "flecs/flecs.h"

typedef struct {
  double x;
  double y;
} Position, Velocity;

static void Move(ecs_iter_t* it) {
  Position* p = ecs_field(it, Position, 0);
  const Velocity* v = ecs_field(it, Velocity, 1);

  char* table = ecs_table_str(it->world, it->table);
  printf("Move over [%s]\n", table);
  ecs_os_free(table);

  for (int i = 0; i < it->count; i++) {
    p[i].x += v[i].x;
    p[i].y += v[i].y;
  }
}

int main(void) {
  ecs_world_t* world = ecs_init();

  ECS_COMPONENT(world, Position);
  ECS_COMPONENT(world, Velocity);
  ECS_SYSTEM(world, Move, EcsOnUpdate, Position, [in] Velocity);
  ECS_TAG(world, Eats);
  ECS_TAG(world, Apples);

  ecs_entity_t bob = ecs_entity(world, {.name = "Bob"});
  ecs_set(world, bob, Position, {0, 0});
  ecs_set(world, bob, Velocity, {1, 2});
  ecs_add_pair(world, bob, Eats, Apples);

  ecs_entity_t alice = ecs_entity(world, {.name = "Alice"});
  ecs_set(world, alice, Position, {10, 10});
  ecs_set(world, alice, Velocity, {-1, 0.5});

  for (int frame = 0; frame < 3; frame++) {
    ecs_progress(world, 1.0f);
  }

  const Position* bp = ecs_get(world, bob, Position);
  const Position* ap = ecs_get(world, alice, Position);
  printf("Bob is at {%.1f, %.1f}\n", bp->x, bp->y);
  printf("Alice is at {%.1f, %.1f}\n", ap->x, ap->y);
  printf("Bob eats apples: %d\n", ecs_has_pair(world, bob, Eats, Apples));

  return ecs_fini(world);
}
