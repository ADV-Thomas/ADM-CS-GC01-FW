// Advantics FW_LIB
//
// lib/sfra/sfra.h
//
// Copyright (C) 2019 Advantics sarl.
//
// Note: Wrapper for TI SFRA

#ifndef LIB_SFRA_SFRA_H
#define LIB_SFRA_SFRA_H

// #include "net/net_can.h"
// #include "lib/module/module.h"
#include <stdbool.h>
#include "inc/net/net.h"
#include "inc/api/obj.h"

#if defined(F2803X)
#include "SFRA_IQ_Include.h"
#elif defined(F28004X)
#include "sfra_f32.h"
#endif

struct sfra_priv;
struct tlo;

struct sfra {
    bool enabled;
    // priv
    struct sfra_priv *priv;
};


/* USER CODE END (section: User_Section) */


// // Initialise SFRA module
struct sfra *sfra_init();

void sfra_periodic(const struct sfra *sfra);

void sfra_strat_sweep(const struct sfra *sfra);


#endif // LIB_SFRA_SFRA_H
