#include <stdio.h>

#include "flecs/flecs.h"

typedef struct {
  double hp;
} Health;

typedef struct {
  double amount;
} Damage;

int main(void) {
  ecs_world_t* world = ecs_init();

  ECS_COMPONENT(world, Health);
  ECS_COMPONENT(world, Damage);

  const char* names[] = {"goblin", "orc", "dragon", "rock"};
  for (int i = 0; i < 4; i++) {
    ecs_entity_t e = ecs_entity(world, {.name = names[i]});
    ecs_set(world, e, Health, {10.0 * (i + 1)});
    if (i < 3) {
      ecs_set(world, e, Damage, {5.0 * (i + 1)});
    }
  }

  ecs_query_t* q = ecs_query(world, {
    .terms = {{.id = ecs_id(Health)}, {.id = ecs_id(Damage), .inout = EcsIn}},
    .cache_kind = EcsQueryCacheAuto
  });

  int matched = 0;
  ecs_iter_t it = ecs_query_iter(world, q);
  while (ecs_query_next(&it)) {
    Health* h = ecs_field(&it, Health, 0);
    const Damage* d = ecs_field(&it, Damage, 1);
    for (int i = 0; i < it.count; i++) {
      h[i].hp -= d[i].amount;
      printf("%s took %.1f damage, %.1f hp left\n",
             ecs_get_name(world, it.entities[i]), d[i].amount, h[i].hp);
      matched++;
    }
  }
  ecs_query_fini(q);

  printf("matched %d of 4 entities\n", matched);

  return ecs_fini(world);
}
