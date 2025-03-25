
#include "app/dev_ctl.h"

#include <stddef.h>
#include <string.h>

#include "inc/net/can.h"
#include "app/user.h"
#include "inc/lib/data.h"



#include "app/display/state_machine.h"
#include "app/SSD1322_OLED_lib/Icons/icons.h"







#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))


struct dev_ctl * dev_ctl_new(const struct tlo *tlo)
{

    if (!(tlo)) {
        return NULL;
    }

    static struct dev_ctl dev_ctl; // Allocate memory for dev_ctl
    memset(&dev_ctl, 0u, sizeof(struct dev_ctl));


    return &dev_ctl;
}



void dev_ctl_check_alive(struct dev_ctl *dev_ctl){

    int i;
    for(i=0;i< N_DEVICES ;i++){
        if(dev_ctl->can_dev[i].present == true){
            if( abs(dev_ctl->can_dev[i].alive_count - dev_ctl->timestamp) > 3000){ //3sec
                    dev_ctl->can_dev[i].present = false;
            } 
        }
           
    }


}

void dev_ctl_update_timestamp(struct dev_ctl *dev_ctl){
    dev_ctl->timestamp ++;
}

int dev_ctl_find_last_devices(const struct tlo  *tlo, enum nfo_id  exp_id ){
   
    if( ! device_is_supported(exp_id)){
        return -1;
    }
    
    if(tlo->dev_ctl == NULL){
        return -1;
    }

    struct dev_ctl *self = (struct dev_ctl *)tlo->dev_ctl;
    uint16_t device_id = self->last_dev_id;
 

    int i;
    for(i=0;i<N_DEVICES;i++){
        //devices already there 
        //if (  self->can_dev[i].present == true){
                volatile uint8_t _id = self->can_dev[i].id ;
                if(_id  == exp_id){
                    volatile uint8_t _stack = self->can_dev[i].stack;
                    volatile uint16_t _device_id =   ((uint16_t)  _stack  << 8  )    |   _id;
                    if(device_id == _device_id){
                        return i;
                    }
                }
        //}
    }
    return -1;

}


bool dev_ctl_update_devices(const struct tlo  *tlo, const struct can_f *f){


    if(tlo->dev_ctl == NULL){
        return false;
    }


    struct dev_ctl *self = (struct dev_ctl *)tlo->dev_ctl;
    uint16_t  last_dev_id =  (f->id & 0xFFFF0000) >> 16;
    self->last_dev_id = last_dev_id;
    //save this but not clean 


    //check if still present
    //alive is bof


    if( tlo->state_machine->currentState != state_sniffer_stack &&
        tlo->state_machine->currentState != state_sniffer_version &&
        tlo->state_machine->currentState != state_sniffer_interlock &&
        tlo->state_machine->currentState != state_select_superset &&
        tlo->state_machine->currentState != state_welcome &&
        tlo->state_machine->currentState != state_set_module){
    
        return false;
    }


    #define mask_device_type 0x8000 //should received every sec

    //id for Device type is 0x8000 offset 0 lenght = 8


    uint8_t id = 0;
    uint8_t stack =0;
    uint8_t rev = 0;
    uint8_t var = 0;
    uint32_t serial_number =  0x00000000;
    if( ((f->id) &  0xFFFF) ==  0x8000 ){
        //this is a device type message 
        id = (f->id & 0xFF0000) >> 16;
        stack = (f->id & 0xFF000000) >> 24; 

        rev =  f->data[1];
        var =  f->data[2];

        serial_number |=  ((uint32_t) f->data[4 + 0]) << (8*3) ;
        serial_number |=  ((uint32_t) f->data[4 + 1]) << (8*2) ;
        serial_number |=  ((uint32_t) f->data[4 + 2]) << (8*1) ;
        serial_number |=  ((uint32_t) f->data[4 + 3] )<< (8*0) ;

    }
    else{
        return false;
    }

    if(id == 0){
        return false;
    }

    int i;

    bool devices_was_register = false;
    for(i=0;i<N_DEVICES;i++){
        //devices already there 
        if ( self->can_dev[i].serial_number  == serial_number ){
            //update this already present device
            self->can_dev[i].id = (enum nfo_id) id;
            self->can_dev[i].hw_rev = rev;
            self->can_dev[i].hw_var = var;
            self->can_dev[i].stack =  stack;
            self->can_dev[i].present = true;
            self->can_dev[i].compatible = device_is_supported((enum nfo_id) id);
            devices_was_register = true;
            self->can_dev[i].alive_count = tlo->dev_ctl->timestamp;
            self->can_dev[i].serial_number = serial_number ;
            self->can_dev[i].duplicated_stack = false;

            self->can_dev[i].paired = NULL;
    
            if(id == NFO_VG11_FM01 || id == NFO_VG11_FM02 ){
                //check is have a pair
                int j = 0;
                for(j=0;j<N_DEVICES;j++){ 
                        if( self->can_dev[j].present == true){
                            if(self->can_dev[i].stack ==  self->can_dev[j].stack){
                                if( (self->can_dev[j].id != id) && ( self->can_dev[j].id  == NFO_VG11_FM01 ||  self->can_dev[j].id  == NFO_VG11_FM02  ) ){
                                            self->can_dev[i].paired = &self->can_dev[j];
                                            self->can_dev[j].paired = &self->can_dev[i];
                                            break;
                                }
                            }
                        }
                }
            }

            if(id == NFO_VG11_FM02){
                if(self->can_dev[i].paired != NULL){
                        self->can_dev[i].paired_slave = true;
                }
            }
           


            break;
        }
    }

    if( ! devices_was_register){
        //add a  devices
        for(i=0;i<N_DEVICES;i++){
            //find a free sport
            if (self->can_dev[i].id  == 0){
            
                self->can_dev[i].custom_mesurables[0] = 0;
                self->can_dev[i].custom_mesurables[1] = 1;
                self->can_dev[i].custom_mesurables[2] = 2;
                self->can_dev[i].custom_mesurables[3] = 3;
          
                self->can_dev[i].id = (enum nfo_id) id;
                self->can_dev[i].hw_rev = rev;
                self->can_dev[i].hw_var = var;
                self->can_dev[i].stack =  stack;
                self->can_dev[i].present = true;
                self->can_dev[i].compatible = device_is_supported((enum nfo_id) id);  //&  no_malloc_error;
                self->can_dev[i].alive_count = tlo->dev_ctl->timestamp;
                self->can_dev[i].serial_number = serial_number ;
                self->can_dev[i].duplicated_stack = false;
                self->can_dev[i].paired = NULL;
                self->can_dev[i].paired_slave = false;
                self->can_dev[i].part_of_ss = false;
                
            
                break;
            }
        }
    }


    //check for colision
    int j = 0;
    for(i=0;i< N_DEVICES -1 ;i++){
        for (j = i + 1; j < N_DEVICES ; ++j) {

            if (  self->can_dev[i].id  == self->can_dev[j].id) {
                if (  self->can_dev[i].stack  == self->can_dev[j].stack) {
                    self->can_dev[i].duplicated_stack = true;
                    self->can_dev[j].duplicated_stack = true;

                }
                
            }

                
         }
    }


    if(self->can_dev[i].present == false){
        //delete only in some mode
         memset( &self->can_dev[i], 0u, sizeof(struct can_dev));
    }
    
    return true;
}



bool device_is_supported(enum nfo_id  id) {
        switch (id) {
            case NFO_BP25:
            case NFO_VG11_FM01:
            case NFO_VG11_FM02:
                return true;
            default: 
                return false;
        }        
}

 



const char* device_id_to_str(enum nfo_id  id) {
    switch (id) {
        case NFO_NONE : 
            return "NONE";
        case NFO_LF45:
            return "LF45";
        case NFO_UP25:
            return "UP25";
        case NFO_LL25:
            return "LL25";
        case NFO_BC25:
            return "BC25";
        case NFO_BP25:
            return "BP25";
        case NFO_BI25:
            return "BI25";
        case NFO_FAN1:
            return "FAN1";
        case NFO_DLOG:
            return "DLOG";
        case NFO_LOGGING:
            return "LOG";
        case NFO_VG11_FM01:
            return "VG111";
        case NFO_VG11_FM02:
            return "VG112";
        case NFO_CB01 :
            return "CB01";
        case NFO_TBT:
            return "TBT";
        case NFO_TBTE:
            return "TBTE";
        case NFO_FP:
            return "FP";
        default:
            return "UNK";
    }
}



int change_device_stack( const struct net *net, const struct dev_ctl * self ,  int selected_dev, int new_stack){
    struct can_f f;

    uint8_t id = self->can_dev[selected_dev].id;
    uint8_t stack =  self->can_dev[selected_dev].stack;
    uint16_t msg_id = 0x0045;

    f.id = msg_id |  ( ((uint32_t)id) << 16) |  (  ((uint32_t) stack) << 24);
    f.length  =  6;

    uint8_t new_stack_pos = (uint8_t) new_stack; 
    uint8_t stack_size  = 0x00; //0x01;
    uint32_t serial_number =  self->can_dev[selected_dev].serial_number;
    f.data[0] = new_stack_pos;
    f.data[1] = stack_size;

    f.data[2+0] =  (serial_number >>  (8*3)) & 0xFF;
    f.data[2+1] =  (serial_number >>  (8*2)) & 0xFF;
    f.data[2+2] =  (serial_number >>  (8*1)) & 0xFF;
    f.data[2+3] =  (serial_number >>  (8*0)) & 0xFF;
  
    int ret = can_write(net, &f);
    return ret;
}






const char* DEV_fault_to_str(const struct can_dev *can_dev,int fault){
    
    if( fault > DEV_fault_enum_end(can_dev) ){ return false;}
    switch (can_dev->id)
    {
    case NFO_BP25 :
        return BP25_fault_to_str( (enum BP25_fault) fault);
    case NFO_VG11_FM01 :
        if(can_dev->paired != NULL){
            const char * _fault = VG11_FM02_fault_to_str( (enum VG11_FM02_fault) fault);
            if( _fault[0] == '!'){
                return VG11_FM01_fault_to_str( (enum VG11_FM01_fault) fault);
            }
            return _fault;    
       
        }
    case NFO_VG11_FM02 :
        return VG11_FM02_fault_to_str( (enum VG11_FM02_fault) fault);

    
    default:
        return false;

    }
}


bool DEV_mode_is_supported(const struct can_dev *can_dev,int mode){

    if( mode > DEV_mode_enum_end(can_dev) ){ return false;}

    switch (can_dev->id)
    {
    case NFO_BP25 :
        return BP25_mode_is_supported( (enum BP25_mode) mode);
    case NFO_VG11_FM01 :
        if(can_dev->paired != NULL){
            return VG11_FM02_mode_is_supported( (enum VG11_FM02_mode) mode);
        }
        return VG11_FM01_mode_is_supported( (enum VG11_FM01_mode) mode);
    case NFO_VG11_FM02 :
        return VG11_FM02_mode_is_supported( (enum VG11_FM02_mode) mode);
    default:
        return false;

    }

}


const char* DEV_mode_to_str(const struct can_dev *can_dev,int mode){

    
    if( mode > DEV_mode_enum_end(can_dev) ){   return "NAN";}

    switch (can_dev->id)
    {
    case NFO_BP25 :
        return BP25_mode_to_str( (enum BP25_mode) mode);
    case NFO_VG11_FM01 :
        if(can_dev->paired != NULL){
            return VG11_FM02_mode_to_str( (enum VG11_FM02_mode) mode);
        }
        return VG11_FM01_mode_to_str( (enum VG11_FM01_mode) mode);
    case NFO_VG11_FM02 :
        return VG11_FM02_mode_to_str( (enum VG11_FM02_mode) mode);
    default:
        return "NAN";

    }

}



const unsigned char*  DEV_mode_to_icon(const struct can_dev *can_dev,int mode){

    if( mode > DEV_mode_enum_end(can_dev) ){   return icon_none;}
    switch (can_dev->id)
    {
    case NFO_BP25 :
        return BP25_mode_to_icon( (enum BP25_mode) mode) ;
    case NFO_VG11_FM01 :
        if(can_dev->paired != NULL){
            return VG11_FM02_mode_to_icon( (enum VG11_FM02_mode) mode);
        }
        return VG11_FM01_mode_to_icon( (enum VG11_FM01_mode) mode);
    case NFO_VG11_FM02 :
        return VG11_FM02_mode_to_icon( (enum VG11_FM02_mode) mode);
    default:
        return icon_none;
    }

}


int DEV_mesurables_view_param(const struct can_dev *can_dev,int  mesurable, enum VIEW_PARAM param){
    
    if( mesurable > DEV_mesurables_enum_end(can_dev) ){ return 0;}

    switch (can_dev->id)
    {
    case NFO_BP25 :
        return BP25_mesurables_view_param( (enum BP25_mesurables) mesurable , (int) param) ;
    case NFO_VG11_FM01 :
        if(can_dev->paired != NULL  && mesurable > (enum_VG11_FM01_mesurables_end - 1)){
            int res =  VG11_FM02_mesurables_view_param( (enum VG11_FM02_mesurables) (mesurable - enum_VG11_FM01_mesurables_end), (int) param) ;
            if(param == VIEW_PARAM_PAGE ){
                 res += 6; //find max page
            }
            return res ;
        }
        return VG11_FM01_mesurables_view_param( (enum VG11_FM01_mesurables) (mesurable ), (int) param) ;
    case NFO_VG11_FM02 :
        return VG11_FM02_mesurables_view_param( (enum VG11_FM02_mesurables) mesurable , (int) param) ;
    default:
        return 0;
    }

}




double DEV_get_mesurables(const struct can_dev *can_dev,int  mesurable){

    if( mesurable > DEV_mesurables_enum_end(can_dev) ){   return 0;}

    switch (can_dev->id)
    {

    case NFO_BP25 :
        return can_dev->mesurables[mesurable];
    case NFO_VG11_FM01 :
        if(can_dev->paired != NULL && mesurable > (enum_VG11_FM01_mesurables_end-1)){
            const struct can_dev* can_dev_p = can_dev->paired;
            if(mesurable ==  (VG11_FM02_energized + enum_VG11_FM01_mesurables_end) ){
                return MAX(  can_dev->mesurables[VG11_FM01_energized] ,  can_dev_p->mesurables[VG11_FM02_energized]  );
            }
            return can_dev_p->mesurables[mesurable - enum_VG11_FM01_mesurables_end];
        }
        return can_dev->mesurables[mesurable];

    case NFO_VG11_FM02 :
        return can_dev->mesurables[mesurable];
    default:
        return 0;
    }


}



const char*  DEV_mesurables_to_str(const struct can_dev *can_dev,int  mesurable, enum STRING_PARAM param){

    if( mesurable > DEV_mesurables_enum_end(can_dev) ){   return "NAN";}

    switch (can_dev->id)
    {
    case NFO_BP25 :
        return BP25_mesurables_to_str( (enum BP25_mesurables) mesurable ,(int) param) ;
    case NFO_VG11_FM01 :
        if(can_dev->paired != NULL && mesurable > (enum_VG11_FM01_mesurables_end -1)){
            VG11_FM02_mesurables_to_str( (enum VG11_FM02_mesurables) (mesurable - enum_VG11_FM01_mesurables_end) , (int) param);
        }
        return VG11_FM01_mesurables_to_str( (enum VG11_FM01_mesurables) mesurable , (int) param) ;

    case NFO_VG11_FM02 :
        return VG11_FM02_mesurables_to_str( (enum VG11_FM02_mesurables) mesurable , (int) param);
    default:
        return "NAN";
    }


}


const char* DEV_setpoints_to_str(const struct can_dev *can_dev,int setpoints,  enum STRING_PARAM param ){
    if( setpoints > DEV_setpoints_enum_end(can_dev) ){   return "NAN";}
    switch (can_dev->id)
    {
    case NFO_BP25 :
        return BP25_setpoints_to_str( (enum BP25_setpoints) setpoints , (int) param) ;
    case NFO_VG11_FM01 :
        if(can_dev->paired != NULL){
            return VG11_FM02_setpoints_to_str( (enum VG11_FM02_setpoints) setpoints , (int) param) ;
        }
        return VG11_FM01_setpoints_to_str( (enum VG11_FM01_setpoints) setpoints , (int) param) ;
    case NFO_VG11_FM02 :
        return VG11_FM02_setpoints_to_str( (enum VG11_FM02_setpoints) setpoints , (int) param) ;
    default:
        return "NAN";
    }


}


double DEV_setables_param(const struct can_dev *can_dev,int setpoints, enum SETABLE_PARAM param ){
    if( setpoints > DEV_setpoints_enum_end(can_dev) ){   return 0;}
    switch (can_dev->id)
    {
    case NFO_BP25 :
        return BP25_setables_param( (enum BP25_setpoints) setpoints , (int) param) ;
    case NFO_VG11_FM01 :
        if(can_dev->paired != NULL){
            return VG11_FM02_setables_param( (enum VG11_FM02_setpoints) setpoints , (int) param) ;
        }
        return VG11_FM01_setables_param( (enum VG11_FM01_setpoints) setpoints , (int) param) ;
    case NFO_VG11_FM02 :
        return VG11_FM02_setables_param( (enum VG11_FM02_setpoints) setpoints , (int) param) ;
    default:
        return 0;
    }


}


void DEV_setpoints_check(const struct can_dev *can_dev, int setpoints, double* value){
    if( setpoints > DEV_setpoints_enum_end(can_dev) ){return ;}
    switch (can_dev->id)
    {
    case NFO_BP25 :
        return BP25_setpoints_check( (enum BP25_setpoints) setpoints , value) ;
    case NFO_VG11_FM01 :
        if(can_dev->paired != NULL){
             return VG11_FM02_setpoints_check( (enum VG11_FM02_setpoints) setpoints , value);
        }
        return VG11_FM01_setpoints_check( (enum VG11_FM01_setpoints) setpoints , value) ;
    case NFO_VG11_FM02 :
        return VG11_FM02_setpoints_check( (enum VG11_FM02_setpoints) setpoints , value) ;
    default : 
        return;

    }
  

}


int DEV_mode_main_view(const struct can_dev *can_dev,int mode,  int param ){
     
    if( mode > DEV_mode_enum_end(can_dev) ){return 0;}
    switch (can_dev->id)
    {
    case NFO_BP25 :
        return BP25_mode_main_view( (enum BP25_mode) mode , param) ;
    case NFO_VG11_FM01 :
        if(can_dev->paired != NULL){
             return VG11_mode_main_view( mode , param);
        }
        return VG11_FM01_mode_main_view( (enum VG11_FM01_mode) mode , param);
    case NFO_VG11_FM02 : 
        return VG11_FM02_mode_main_view( (enum VG11_FM02_mode) mode , param);
    default:
        return 0;
    }

}



int DEV_mode_enum_end(const struct can_dev *can_dev){
    switch (can_dev->id)
    {
    case NFO_BP25 :
        return enum_BP25_mode_end ;
    case NFO_VG11_FM01 :
       if(can_dev->paired != NULL){
            return( enum_VG11_FM02_mode_end + enum_VG11_FM01_mode_end);
        }
        return enum_VG11_FM01_mode_end ;
    case NFO_VG11_FM02 :
        return enum_VG11_FM02_mode_end ;
    default:
        return 0;
    }

}



int DEV_setpoints_enum_end(const struct can_dev *can_dev){
    switch (can_dev->id)
    {
    case NFO_BP25 :
        return enum_BP25_setpoints_end;
     case NFO_VG11_FM01 :
        if(can_dev->paired != NULL){
            return( enum_VG11_FM01_setpoints_end + enum_VG11_FM02_setpoints_end);
        }

        return enum_VG11_FM01_setpoints_end ;
     case NFO_VG11_FM02 :
        return enum_VG11_FM02_setpoints_end ;
    default:
        return 0;
    }

}


int DEV_mesurables_enum_end(const struct can_dev *can_dev){
    switch (can_dev->id)
    {
    case NFO_BP25 :
        return enum_BP25_mesurables_end ;
    case NFO_VG11_FM01 :
        if(can_dev->paired != NULL){
            return( enum_VG11_FM01_mesurables_end + enum_VG11_FM02_mesurables_end);
        }
        return enum_VG11_FM01_mesurables_end ;
    case NFO_VG11_FM02 :
        return enum_VG11_FM02_mesurables_end ;
    default:
        return 0;
    }

}



int DEV_fault_enum_end(const struct can_dev *can_dev){
    switch (can_dev->id)
    {
    case NFO_BP25 :
        return enum_BP25_fault_end ;
    case NFO_VG11_FM01 :
        if(can_dev->paired != NULL){
                return MAX(enum_VG11_FM01_fault_end,enum_VG11_FM02_fault_end);
        }
        return enum_VG11_FM01_fault_end ;
    case NFO_VG11_FM02 :
        return enum_VG11_FM02_fault_end ;
    default:
        return 0;
    }

}





double DEV_get_temp(const struct can_dev *can_dev){
    switch (can_dev->id)
    {
    case NFO_BP25 :
        return can_dev->mesurables[BP25_temp_bridge] ;
    case NFO_VG11_FM01 :
        return can_dev->mesurables[VG11_FM01_temp_bridge];
    case NFO_VG11_FM02 :
        return  can_dev->mesurables[VG11_FM02_temp_bridge];
    default:
        return 0;
    }

}






int  VG11_mode_main_view(int mode, int param) {   
    int enum_VG11_mesurables_end  =  enum_VG11_FM01_mesurables_end + enum_VG11_FM02_mesurables_end ;
   int enum_FM01_offset =  enum_VG11_FM01_mesurables_end;
    int _P1 = enum_VG11_mesurables_end ;
    int _P2 = enum_VG11_mesurables_end;
    int  _P3 = enum_VG11_mesurables_end ;
   
    switch(mode){
        case VG11_FM02_mode_none :            _P1 = VG11_FM02_energized + enum_FM01_offset;        _P2 = enum_VG11_mesurables_end  ;       _P3 = enum_VG11_mesurables_end;      break;
        case VG11_FM02_mode_grid_following :  _P1 = VG11_FM02_energized  + enum_FM01_offset;        _P2 = enum_VG11_mesurables_end  ;       _P3 = enum_VG11_mesurables_end;      break;
        case VG11_FM02_mode_pwm :            _P1 = VG11_FM02_energized  + enum_FM01_offset;         _P2 = enum_VG11_mesurables_end  ;       _P3 = enum_VG11_mesurables_end;      break;

        default : 
            return enum_VG11_mesurables_end;
    }

    switch(param){
        case 1 :
            return _P1;
        case 2 :
            return _P2;
        case 3 :
            return _P3;

        default :
            return enum_VG11_mesurables_end;
    }


}
