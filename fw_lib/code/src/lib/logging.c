/**************************************************************************************************
 * 
 * \file logging.c
 * 
 * \brief Logging module implementation. It makes use of a lightweight implementation of the
 * standard printf (and others) functions which is optimized for space (microcontrollers). The
 * lightweight printf module is authored by Marco Paland (info@paland.com)
 * 
 * 
 * \author Jorge Sola
 * 
 *************************************************************************************************/

#include "inc/lib/logging.h"

#include "inc/lib/printf.h"
#include "inc/api/db.h"

#include "logging_db.h"

#include <stdbool.h>



/** Set the default buffer size to 512 words */
#ifndef C_LOGGING_BUFFER_LENGTH
#define C_LOGGING_BUFFER_LENGTH (512)
#endif


struct logging {
    const struct logging_db *log_db;
    char strbuf[C_LOGGING_BUFFER_LENGTH];
    int head;
    int tail;
    enum logging_level level;
    int enable;
};

static struct logging log;

void logging_init(const struct logging_db *log_db)
{
    log.log_db = log_db;
    log.enable = false;
    log.head = 0;
    log.tail = 0;
    log.level = LOGGING_WARNING;

}

void _logging(enum logging_level level, const char *fmt, ...)
{

    va_list args;
    if (log.level > level || !log.enable) {
        return;
    }
  
    va_start(args,fmt);
    vprintf_(fmt, args);
    va_end(args);
}


void _putchar(char character)
{
    if (log.tail == (log.head + 1))
        return; // full, advancing head would wrap ring around

    log.strbuf[log.head] = character;
    log.head = (log.head + 1) % C_LOGGING_BUFFER_LENGTH;

    if(log.log_db == NULL){
        return;
    }

    log.log_db->logging_logging_string_stream->enable = true;

#ifdef VIRTUAL
    printf("%c", character);
#endif
}

/**************************************************************************************************
 * 
 * logging_db_init()
 * 
 *************************************************************************************************/
DB_PROTOTYPE_INIT(logging_db)
{
    DB_SET_PERIODIC_CALLBACK(logging_logging_string_stream);
    return 0;
}

/**************************************************************************************************
 * 
 * logging_logging_set_options_callback()
 * 
 *************************************************************************************************/
DB_PROTOTYPE_CALLBACK(logging_logging_set_options)
{
    log.level = (enum logging_level)msg->level;
    log.enable = (msg->enable > 0)? true:false;

}

/**************************************************************************************************
 * 
 * logging_logging_string_stream_callback()
 * 
 *************************************************************************************************/
DB_PROTOTYPE_CALLBACK(logging_logging_string_stream)
{
    int i;
    uint8_t *pMsgOut;

    if(log.log_db == NULL){
        return;
    }

    pMsgOut = &msg->ch_0;

    for(i=0; i<8; i++){
        if (log.tail != log.head) { // ring is not empty
            pMsgOut[i] = log.strbuf[log.tail];
            log.tail = (log.tail + 1) % C_LOGGING_BUFFER_LENGTH;
        } else { // we consider empty when head == tail
            pMsgOut[i] = 0;
            log.log_db->logging_logging_string_stream->enable = false;
            // No break here to clear all unused characters
        }
    }

}
