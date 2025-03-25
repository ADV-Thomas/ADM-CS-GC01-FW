
#include "inc/lib/fan_ctl.h"

#include "inc/net/can.h"
#include "app/user.h"
#include "inc/lib/data.h"

#include <stddef.h>
#include <string.h>

struct fan_ctl_priv{
    uint16_t next_node;
    struct node node[N_NODES];
};

float calculate_fan_speed(struct node *node)
{
    float speed = (float)node->max_speed/(float)(node->max_temp - node->min_temp)*(float)(node->temperature - node->min_temp);

    if(speed > node->max_speed){
        speed = node->max_speed;
    }
    else if(speed < 0.0f){
        speed = 0.0f;
    }

    // Speed is multiplied by 0.01 to convert from 0-100 to 0-1 range
    return 0.01 * speed;
}

struct fan_ctl * fan_ctl_new(struct mal *mal)
{
    static struct fan_ctl_priv priv;

    if (!(mal)){
        return NULL;
    }
    
    memset(&priv,0u,sizeof(priv));

    static struct fan_ctl fan_ctl = {
        .priv = &priv,
    };

    fan_ctl.node = &priv.node[0];

    /** Read previous Fan Control config stored in EEPROM
     * If it fails, the fan_ctl structure will simply be empty
     */
    data_fan_ctl(mal,&fan_ctl,true);

    return &fan_ctl;
}

bool fan_ctl_update_node(const struct fan_ctl *fan_ctl, const struct can_f *f)
{
    int i;
    bool ret = false;
    uint16_t byte_offset;

    struct fan_ctl *self = (struct fan_ctl *)fan_ctl;
    struct fan_ctl_priv *priv = self->priv;
    for(i=0; i<N_NODES; i++){
        if(priv->node[i].suscribed && priv->node[i].id == f->id){
            ret = true;
            byte_offset = self->node[i].offset;
            priv->node[i].temperature = 0.01 * ((f->data[byte_offset + 1] << 8) | f->data[byte_offset]);
            priv->node[i].timeout_cnt = 0;
            priv->node[i].active = true;
            priv->node[i].fan_speed = calculate_fan_speed(&priv->node[i]);
        }
    }

    return ret;
}

int fan_ctl_add_node(const struct fan_ctl *fan_ctl, struct node *node)
{
    struct fan_ctl_priv *priv = fan_ctl->priv;
    int i;

    // Iterate through the array of nodes to find an available spot
    for(i=0;i<N_NODES;i++){
        if(priv->node[priv->next_node].suscribed){
            priv->next_node++;
            if(priv->next_node >= N_NODES){
                priv->next_node = 0;
            }
        }
        else{
            priv->node[priv->next_node] = *node;
            return 0;
        }
    }

    return -1;
}

int fan_ctl_remove_node(const struct fan_ctl *fan_ctl, struct node *node)
{
    struct fan_ctl_priv *priv = fan_ctl->priv;
    int i;

    // Iterate through node array and unsuscribe the node (or nodes) that match
    for(i=0;i<N_NODES;i++){
        if(priv->node[i].id == node->id && priv->node[i].offset == node->offset){
            priv->node[i].suscribed = false;
            priv->node[i].active = false;
            return 0;
        }
    }

    return -1;
}

void fan_ctl_timeout_node(const struct fan_ctl *fan_ctl)
{
    struct fan_ctl_priv *priv = fan_ctl->priv;
    int i;

    /** Iterate through node array and update timeout counter. Set node as inactive
    * if timeout counter is bigger than threshold
    * */
    for(i=0;i<N_NODES;i++){
        if(priv->node[i].suscribed){
            priv->node[i].timeout_cnt++;
            if(priv->node[i].timeout_cnt >= 5*C_TASK_FREQ_FB){
                priv->node[i].active = false;
                priv->node[i].timeout_cnt = 5*C_TASK_FREQ_FB;
            }
        }
    }
}

float fan_ctl_get_speed(const struct fan_ctl *fan_ctl)
{
    struct fan_ctl_priv *priv = fan_ctl->priv;

    int i;
    float speed = 0;

    for(i=0;i<N_NODES;i++){
        if(priv->node[i].suscribed && priv->node[i].active){
            if(priv->node[i].fan_speed > speed){
                speed = priv->node[i].fan_speed;
            }
        }
    }
    return speed;
}
