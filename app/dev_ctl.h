#ifndef _APP_DEV_CTL_H
#define _APP_DEV_CTL_H

#include <stdint.h>
#include <stdbool.h>


#include "app/tlo.h"
#include "inc/net/net.h"
#include "inc/lib/nfo.h"



#include "app/dev/ctl/bp25_ctl.h"
#include "app/dev/ctl/vg11_fm01_ctl.h"
#include "app/dev/ctl/vg11_fm02_ctl.h"





#define N_NODES 4
#define N_DEVICES 10

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



struct mode{
    bool supported;
    unsigned char *icon;
    unsigned char *name;
};


enum VIEW_PARAM {
    VIEW_PARAM_LINE,
    VIEW_PARAM_COLUMN,
    VIEW_PARAM_PAGE,
} ;


enum STRING_PARAM {
    NAME,
    UNIT,
};


enum SETABLE_PARAM {
    SET_MIN,
    SET_MAX,
};




struct can_dev{
    bool present;
    uint16_t alive_count; 
    bool compatible;

    enum nfo_id id;

    uint8_t stack; 
    bool duplicated_stack;

    uint8_t hw_rev; 
    uint8_t hw_var;
    uint32_t serial_number;

  

    //status
    bool trip_internal;
    bool trip_external;
    bool ready;
    bool running; 
    int  mode_ctrl;
    bool faults[32]; //cast to uint_32t to get fault


    //control
    bool setpoint_changed;
    bool request_on;
    int  request_mode;
    bool clear_interlock;

 
    //HW specific
    double mesurables[32]; 
    double setpoints[16];
    
    //custom view
    int custom_mesurables[4];


    const struct can_dev * paired;  //pointer to paired
    bool paired_slave;

    bool part_of_ss;

};





struct dev_ctl{
    struct can_dev can_dev[N_DEVICES];
    uint16_t last_dev_id;
    uint16_t send_message_to;
    uint16_t timestamp;

};

struct dev_ctl * dev_ctl_new(const struct tlo *tlo);
bool dev_ctl_update_devices(const struct tlo *tlo, const struct can_f *f);
int dev_ctl_find_last_devices(const struct tlo  *tlo, enum nfo_id  exp_id );
bool device_is_supported(enum nfo_id  id);
const char* device_id_to_str(enum nfo_id id);

int change_device_stack( const struct net *net, const struct dev_ctl * self ,  int selected_dev, int new_stack);
void dev_ctl_update_timestamp(struct dev_ctl *dev_ctl);
void dev_ctl_check_alive(struct dev_ctl *dev_ctl);


//Generic function
const char* DEV_fault_to_str(const struct can_dev *can_dev,int fault);

bool DEV_mode_is_supported(const struct can_dev *can_dev,int mode);
const char* DEV_mode_to_str(const struct can_dev *can_dev,int mode);
const unsigned char* DEV_mode_to_icon(const struct can_dev *can_dev,int mode);
int DEV_mesurables_view_param(const struct can_dev *can_dev,int  mesurable, enum VIEW_PARAM  param );
const char* DEV_mesurables_to_str(const struct can_dev *can_dev,int mesurable,  enum STRING_PARAM param );
const char* DEV_setpoints_to_str(const struct can_dev *can_dev,int setpoints, enum STRING_PARAM  param );
double DEV_setables_param(const struct can_dev *can_dev,int setpoints, enum SETABLE_PARAM  param );
void DEV_setpoints_check(const struct can_dev *can_dev,int setpoints, double* value);
int DEV_mode_main_view(const struct can_dev *can_dev,int mode, int param);


int DEV_mode_enum_end(const struct can_dev *can_dev);
int DEV_setpoints_enum_end(const struct can_dev *can_dev);
int DEV_mesurables_enum_end(const struct can_dev *can_dev);
int DEV_fault_enum_end(const struct can_dev *can_dev);
double DEV_get_temp(const struct can_dev *can_dev);

double DEV_get_mesurables(const struct can_dev *can_dev,int  mesurable);


int  VG11_mode_main_view( int mode, int param);



#endif


