/**************************************************************************************************
 * 
 * \file tlo.h
 * 
 * \brief Top-level object interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _APP_TLO_H
#define _APP_TLO_H

#include "inc/lib/tlo.h"
#include <stdint.h>

/**************************************************************************************************
 * 
 * Forward declarations
 * 
 *************************************************************************************************/

struct nfo;
struct net;
struct mal;
struct task;
struct adc;
struct ctl;
struct dlog;
struct logging;
struct supervisor;

struct adm_cs_fp_db;
struct adm_pc_bp25_db;
struct adm_pc_vg11_fm01_db;
struct adm_pc_vg11_fm02_db;

struct status_led;
struct protection;




/**************************************************************************************************
 * 
 * Top-level object definition
 * 
 *************************************************************************************************/
struct tlo {
    struct nfo *mod;
    struct nfo *boot;
    struct mal *mal;
    const struct net *can;
    const struct adm_cs_fp_db *db;
    const struct adm_pc_bp25_db *db_afe;
    const struct adm_pc_vg11_fm01_db *db_vg11_fm01;
    const struct adm_pc_vg11_fm02_db *db_vg11_fm02;
    const struct adc *adc;
    struct ctl *ctl;
    const struct task *task;
    //const struct wcs *wcs;
    struct dev_ctl *dev_ctl;
    const struct superset_ctl *superset_ctl;
    const struct dlog *dlog;
    const struct dlog_db *dlog_db;
    const struct logging *logging;
    const struct logging_db *logging_db;
    double *coding_wheel_value;
    double *blink_speed;

    struct keys *keys;
    struct state_machine *state_machine;


};

#endif /* _APP_TLO_H */
