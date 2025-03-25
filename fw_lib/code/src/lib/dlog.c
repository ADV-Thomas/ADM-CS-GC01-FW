/**************************************************************************************************
 * 
 * \file dlog.c
 * 
 * \brief DLOG implementation
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#include "inc/lib/dlog.h"
#include "inc/api/db.h"
#include "dlog_db.h"

#include "app/tlo.h"

#define RAM_DLOG_SIZE_INT (28672u) /* Ram allocated for DLOG data in 16-bit words */
#define RAM_DLOG_SIZE_FLOAT (RAM_DLOG_SIZE_INT/2) 

static float dlog_buffer[RAM_DLOG_SIZE_FLOAT];
#pragma DATA_SECTION(dlog_buffer,"dlog_data")


enum dlog_status {
    DLOG_WAIT = 0,
    DLOG_RECORD,
    DLOG_CAN,
};

struct dlog_priv {
    enum dlog_status status;
    uint16_t dec_ctr;
    uint16_t ch_ctr;
    uint16_t buffer_ctr; // this counter is used for post trigger half buffer count
    _iq prev_val;
    struct dlog_db *db;
    uint16_t ch_index;
    uint16_t data_index;
    uint16_t trigger_pos; // where in the buffer we have encountered a trigger
    bool force_trigger;
};

struct dlog_channel{
    const _iq *input; /* pointer to input of the channel */
    _iq *buffer; /* pointer to data buffer (you have to provide and _allocate_ the buffer) */
};

struct dlog_channel_group{
    uint16_t num; /* Number of channels */
    uint16_t len; /* Depth of each channel */
    struct dlog_channel ch[DLOG_MAX_CHANNEL_NUMBER]; /* Array of channels */
};

struct dlog {
    uint16_t decimation;        /* Decimation ratio */
    uint16_t trigger_channel;   /* Channel on which the dlog module is triggering (if >= ch_num, trigger is _disabled_) */
    _iq trigger_value;          /* Trigger value for given trigger_channel */
    uint16_t trigger_position;  /* Position of trigger in buffer (you can change portion of pre/post trigger data in buffer)*/
    bool enable;                /* Enable/disable dlog */
    struct dlog_priv *priv;     /* Private data of the dlog module */
    struct dlog_channel_group *ch_group; /* Pointer to channel group */
};

static float *dlog_alloc_channel(uint16_t size);

__attribute__((ramfunc)) static inline bool dlog_trigger(struct dlog *self)
{
    bool trigd                = self->priv->force_trigger;
    self->priv->force_trigger = false;
    if (self->trigger_channel >= self->ch_group->num) {
        return trigd;
    }
    struct dlog_channel *ch = &self->ch_group->ch[self->trigger_channel];
    if (*ch->input > self->trigger_value && self->priv->prev_val < self->trigger_value) {
        trigd = true;
    }
    self->priv->prev_val = *self->ch_group->ch[self->trigger_channel].input;
    return trigd;
}

__attribute__((ramfunc)) static void record(struct dlog *self)
{
    uint16_t i;
    self->priv->dec_ctr = (self->priv->dec_ctr + 1) % self->decimation;
    if (self->priv->dec_ctr == 0) {
        for (i = 0; i < self->ch_group->num; ++i) {
            self->ch_group->ch[i].buffer[self->priv->ch_ctr] = *self->ch_group->ch[i].input;
        }
        self->priv->ch_ctr = (self->priv->ch_ctr + 1) % self->ch_group->len;
        if (self->priv->buffer_ctr > 0) {
            self->priv->buffer_ctr--;
        }
    }
}

__attribute__((ramfunc)) void dlog_run(struct dlog *self, bool ext_trigger)
{
    bool trigd = false;
    if (self->enable) {
        trigd |= dlog_trigger(self);
        trigd |= ext_trigger;
        switch (self->priv->status) {
        case DLOG_WAIT:
            record(self);
            if (trigd && self->priv->buffer_ctr == 0) {
                self->priv->status      = DLOG_RECORD;
                self->priv->buffer_ctr  = self->ch_group->len - self->trigger_position;
                self->priv->trigger_pos = self->priv->ch_ctr;
            }
            break;
        case DLOG_RECORD:
            record(self);
            if (self->priv->buffer_ctr == 0) {
                self->priv->status                             = DLOG_CAN;
                self->priv->db->dlog_dlog_data_stream->enable = true;
                self->priv->buffer_ctr                         = self->ch_group->len;
            }
            break;
        default:
            // Hold-off trigger/recording while can is transferring buffer
            break;
        };
    }
}

struct dlog *dlog_init(struct dlog_usr *dlog_usr, struct dlog_db *db)
{
    static struct dlog_priv _priv = {
        .status        = DLOG_WAIT,
        .dec_ctr       = 0,
        .ch_ctr        = 0,
        .prev_val      = _IQ24(0.0f),
        .ch_index      = 0,
        .data_index    = 0,
        .force_trigger = false,
    };
    _priv.db = db;

    static struct dlog dlog = {
        .decimation      = 2,
        .trigger_channel = 0,
        .trigger_value   = _IQ24(0.0f),
        // Last 1/4 of the buffer is post trigger
        .enable = false,
        .priv   = &_priv,
    };

    if(!db){
        return NULL;
    }


    int i;
    static struct dlog_channel_group ch_group;
    
    ch_group.num = dlog_usr->num_channels;
    ch_group.len = RAM_DLOG_SIZE_FLOAT/dlog_usr->num_channels;

    for(i=0; i < ch_group.num; i++){
        ch_group.ch[i].input = dlog_usr->pInput[i];
        ch_group.ch[i].buffer = dlog_alloc_channel(ch_group.len);
    }

    dlog.ch_group          = &ch_group;
    dlog.trigger_position  = ch_group.len - ch_group.len / 4;

    _priv.buffer_ctr = ch_group.len;
    return &dlog;
}

/**************************************************************************************************
 * 
 * adm_pc_bp25_db_init()
 * 
 *************************************************************************************************/
DB_PROTOTYPE_INIT(dlog_db)
{
    DB_SET_PERIODIC_CALLBACK(dlog_dlog_status);
    DB_SET_PERIODIC_CALLBACK(dlog_dlog_data_stream);

    DB_MSG_DISABLE(dlog_dlog_data_stream);
    return 0;
}

/**************************************************************************************************
 * 
 * adm_pc_bp25__afe_adc_calibration_mode_callback()
 * 
 *************************************************************************************************/
DB_PROTOTYPE_CALLBACK(dlog_dlog_status)
{
    struct dlog *self = (struct dlog *)tlo->dlog;

    msg->channel_number = self->priv->ch_index;
    msg->status = self->priv->status;
    msg->vector_length = self->ch_group->len;
}

/**************************************************************************************************
 * 
 * adm_pc_bp25__afe_adc_calibration_mode_callback()
 * 
 *************************************************************************************************/
DB_PROTOTYPE_CALLBACK(dlog_dlog_data_stream)
{
    int32_t data_index;
    struct dlog *self = (struct dlog *)tlo->dlog;
    if (self->priv->status == DLOG_CAN) {
        msg->channel_index = self->priv->ch_index;
        // We want to display at half buffer length before trigger point
        // Such that trigger point ends up in middle of screen
        data_index = (self->priv->data_index + 2 * self->ch_group->len - self->priv->trigger_pos
                      + self->trigger_position)
                     % self->ch_group->len;
        msg->vector_index = (uint16_t)data_index;
        // self->priv->seq->seq_dlog_data_stream.vector_index = self->priv->data_index;
        msg->value = _IQ24toF(self->ch_group->ch[self->priv->ch_index].buffer[self->priv->data_index]);

        self->priv->data_index = (self->priv->data_index + 1) % self->ch_group->len;
        if (self->priv->data_index == 0) {
            self->priv->ch_index = (self->priv->ch_index + 1) % self->ch_group->num;
        }
    }
}

/**************************************************************************************************
 * 
 * adm_pc_bp25__afe_adc_calibration_mode_callback()
 * 
 *************************************************************************************************/
DB_PROTOTYPE_CALLBACK(dlog_dlog_set_options)
{
    struct dlog *self = (struct dlog *)tlo->dlog;

    self->decimation      = msg->decimation;
    self->trigger_channel = msg->trigger_channel;
    self->trigger_value   = _IQ24(msg->trigger_value);
}

/**************************************************************************************************
 * 
 * adm_pc_bp25__afe_adc_calibration_mode_callback()
 * 
 *************************************************************************************************/
DB_PROTOTYPE_CALLBACK(dlog_dlog_command)
{
    struct dlog *self = (struct dlog *)tlo->dlog;

    if (msg->enable) {
        self->enable = true;
    } else if (msg->disable) {
        self->enable = false;
    }

    if ((msg->disable || msg->trigger_arm) && self->priv->status == DLOG_CAN) {
        self->priv->status                             = DLOG_WAIT;
        self->priv->db->dlog_dlog_data_stream->enable = false;
    }

    self->priv->force_trigger = msg->trigger_force;
}

float *dlog_alloc_channel(uint16_t size)
{
    float static *pBuffer = &dlog_buffer[0];
    static uint16_t alloc_counter = 0;

    if((alloc_counter + size) <= (RAM_DLOG_SIZE_FLOAT)){
        pBuffer = &dlog_buffer[alloc_counter];
        alloc_counter = alloc_counter + size;
        return pBuffer;
    }
    else{
        return NULL;
    }
}
