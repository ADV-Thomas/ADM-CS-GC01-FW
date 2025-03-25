

#ifndef _APP_SUPERVISOR_H
#define _APP_SUPERVISOR_H

#include <stdint.h>
#include <stdbool.h>

#define N_NODES 4

struct can_f;
struct mal;

struct node{
    bool suscribed;
    bool active;
    uint32_t id; 
    uint16_t offset;
    uint16_t min_temp;
    uint16_t max_temp;
    uint16_t max_speed;
    uint16_t timeout_cnt;
    float temperature;
    float fan_speed;
};

struct fan_ctl{
    struct fan_ctl_priv * const priv;
    const struct node *node;
};

struct fan_ctl * fan_ctl_new(struct mal *mal);

int fan_ctl_add_node(const struct fan_ctl *fan_ctl, struct node *node);
int fan_ctl_remove_node(const struct fan_ctl *fan_ctl, struct node *node);
bool fan_ctl_update_node(const struct fan_ctl *fan_ctl, const struct can_f *f);
void fan_ctl_timeout_node(const struct fan_ctl *fan_ctl);
float fan_ctl_get_speed(const struct fan_ctl *fan_ctl);


#endif
