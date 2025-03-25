/**************************************************************************************************
 *
 * \file lco.c
 *
 * \brief Load Current Observer implementation
 *
 * \author Jorge Sola
 *
 *************************************************************************************************/

#include "inc/lib/lco.h"

#include "inc/lib/debug.h"
#include "inc/lib/utils.h"

void lco_setup(struct lco *lco,  struct lco_usr *usr)
{
    lco->b = usr->b;
    lco->b_inv = 1.0f/lco->b;
    lco->ts = usr->ts;
    lco->beta[0] = usr->ts * 2 * usr->f_bw * C_2PI;
    lco->beta[1] = usr->ts * usr->f_bw * C_2PI * usr->f_bw * C_2PI;
}

void lco_force(struct lco *lco, float i_ind, float v_cap)
{
    lco->zeta[0] = v_cap;
    lco->zeta[1] = -lco->b * i_ind;
}
