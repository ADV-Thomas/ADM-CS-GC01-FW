/**************************************************************************************************
 *
 * \file mppt.c
 *
 * \brief Maximum Power Point Tracking module
 *
 * \author Jorge Sola
 *
 *************************************************************************************************/

#include "inc/lib/mppt.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

//#define FUZZY_LOGIC 0
#ifdef FUZZY_LOGIC
enum shape { TRIANGLE = 0, TRAPEZOID, TRAPEZOID_LEFT, TRAPEZOID_RIGHT };

struct mem_func {
    float value;
    float p_left;
    float slope_left;
    float p_right;
    float slope_right;
};

struct rule {
    struct mem_func *in1_mf;
    struct mem_func *in2_mf;
    struct mem_func *out_mf;
};

#define SLOPE_IN1 2.5
struct mem_func in1_nb = {0, -1e9, SLOPE_IN1, -0.11, SLOPE_IN1};
struct mem_func in1_ns = {0, -0.15, SLOPE_IN1, -0.01, SLOPE_IN1}; // [-0.15,-0.04]
struct mem_func in1_z  = {0, -0.02, SLOPE_IN1, 0.02, SLOPE_IN1};  // [-0.08,0.08] or [-0.04,0.04]
struct mem_func in1_ps = {0, 0.01, SLOPE_IN1, 0.15, SLOPE_IN1};
struct mem_func in1_pb = {0, 0.11, SLOPE_IN1, 1e9, SLOPE_IN1};

#define SLOPE_IN2 10
struct mem_func in2_nb = {0, -1e9, SLOPE_IN2, -0.04, SLOPE_IN2};
struct mem_func in2_ns = {0, -0.05, SLOPE_IN2, 0, SLOPE_IN2};
struct mem_func in2_z  = {0, -0.01, SLOPE_IN2, 0.01, SLOPE_IN2};
struct mem_func in2_ps = {0, 0, SLOPE_IN2, 0.05, SLOPE_IN2};
struct mem_func in2_pb = {0, 0.04, SLOPE_IN2, 1e9, SLOPE_IN2};

#define SLOPE_OUT 0.4
struct mem_func out_nb = {0, -1, SLOPE_OUT, -0.5, SLOPE_OUT};
struct mem_func out_ns = {0, -0.75, SLOPE_OUT, -0.05, SLOPE_OUT};
struct mem_func out_z  = {0, -0.25, SLOPE_OUT, 0.25, SLOPE_OUT};
struct mem_func out_ps = {0, 0.05, SLOPE_OUT, 0.75, SLOPE_OUT};
struct mem_func out_pb = {0, 0.5, SLOPE_OUT, 1, SLOPE_OUT};

struct rule r11 = {&in1_nb, &in2_nb, &out_z};
struct rule r12 = {&in1_nb, &in2_ns, &out_z};
struct rule r13 = {&in1_nb, &in2_z, &out_nb};
struct rule r14 = {&in1_nb, &in2_ps, &out_nb};
struct rule r15 = {&in1_nb, &in2_pb, &out_nb};

struct rule r21 = {&in1_ns, &in2_nb, &out_z};
struct rule r22 = {&in1_ns, &in2_ns, &out_z};
struct rule r23 = {&in1_ns, &in2_z, &out_ns};
struct rule r24 = {&in1_ns, &in2_ps, &out_ns};
struct rule r25 = {&in1_ns, &in2_pb, &out_ns};

struct rule r31 = {&in1_z, &in2_nb, &out_ns};
struct rule r32 = {&in1_z, &in2_ns, &out_z}; // out_z
struct rule r33 = {&in1_z, &in2_z, &out_z};
struct rule r34 = {&in1_z, &in2_ps, &out_z}; //_out_z
struct rule r35 = {&in1_z, &in2_pb, &out_ps};

struct rule r41 = {&in1_ps, &in2_nb, &out_ps};
struct rule r42 = {&in1_ps, &in2_ns, &out_ps};
struct rule r43 = {&in1_ps, &in2_z, &out_ps};
struct rule r44 = {&in1_ps, &in2_ps, &out_z};
struct rule r45 = {&in1_ps, &in2_pb, &out_z};

struct rule r51 = {&in1_pb, &in2_nb, &out_pb};
struct rule r52 = {&in1_pb, &in2_ns, &out_pb};
struct rule r53 = {&in1_pb, &in2_z, &out_pb};
struct rule r54 = {&in1_pb, &in2_ps, &out_z};
struct rule r55 = {&in1_pb, &in2_pb, &out_z};

struct mem_func *in1_funcs[5] = {&in1_nb, &in1_ns, &in1_z, &in1_ps, &in1_pb};
struct mem_func *in2_funcs[5] = {&in2_nb, &in2_ns, &in2_z, &in2_ps, &in2_pb};
struct mem_func *out_funcs[5] = {&out_nb, &out_ns, &out_z, &out_ps, &out_pb};

struct rule *all_rules[25] = {&r11, &r12, &r13, &r14, &r15, &r21, &r22, &r23, &r24,
                              &r25, &r31, &r32, &r33, &r34, &r35, &r41, &r42, &r43,
                              &r44, &r45, &r51, &r52, &r53, &r54, &r55};

void compute_degree_of_membership(struct mem_func *mf, float input);
float compute_area_of_trapezoid(struct mem_func *mf);
void fuzzification(float dPdV, float dE);
void rule_evaluation(void);
float defuzzification(void);
float min(float a, float b);
float max(float a, float b);

#endif

/**************************************************************************************************
 *
 *  mppt_init()
 *
 *************************************************************************************************/
void mppt_init(struct mppt *mppt, float v_init)
{
    mppt->priv.v_old      = v_init;
    mppt->priv.v_oc       = v_init;
    mppt->priv.v_max_step = v_init * mppt->usr.max_v_step * 0.01;
    mppt->priv.v_min_step = v_init * mppt->usr.min_v_step * 0.01; // 0.4/893.0362;//
    mppt->priv.p_old      = 0;
    mppt->priv.v_ref      = 0.85 * v_init;
    mppt->out.v_ref       = &mppt->priv.v_ref;
    mppt->priv.p_mppt     = 0;

    mppt->priv.debug_dPdV_prev = 0;
    mppt->priv.debug_dE        = 0;
}

/**************************************************************************************************
 *
 *  mppt_run()
 *
 *************************************************************************************************/
float mppt_run(struct mppt *mppt, float pv_voltage, float power)
{
    float dP, dV, dPdV;
    static float V_step = 0;
    float C, dC, dCdV;
    float S;
    static float C_old = 0;

    dP                  = power - mppt->priv.p_old;
    dV                  = pv_voltage - mppt->priv.v_old;
    mppt->priv.debug_dP = dP;
    mppt->priv.debug_dV = dV;

    /** Method provided by PLECS **/
    /*if (dV >= 0.)
    {
            if (dV < mppt->usr.min_v_step)
        {
            dV = mppt->usr.min_v_step;
        }
    }
    else
    {
        if (dV > -mppt->usr.min_v_step)
        {
            dV = -mppt->usr.min_v_step;
        }
    }

    dPdV = dP/dV;
    v_step = dPdV * mppt->usr.gain * mppt->usr.period;

    if (v_step > mppt->usr.max_v_step)
    {
            v_step = mppt->usr.max_v_step;
    }
    if (v_step < -mppt->usr.max_v_step)
    {
            v_step = -mppt->usr.max_v_step;
    }


    mppt->priv.v_ref += v_step;
    mppt->priv.dPdV = dPdV;
    mppt->priv.p_old = power;
    mppt->priv.v_old = pv_voltage;
    v_step_prev = v_step;*/

    /** Method based on this paper: https://sci-hub.se/https://ieeexplore.ieee.org/document/5559432
     * **/

    if (fabsf(dV) < mppt->priv.v_min_step && fabsf(dP) < 0.01 * mppt->priv.p_mppt) {
        // Do nothing if the voltage or the power does not change much, to prevent
        // oscillations at the expense of a little bit of MPPT tracking accuracy
    } else {
        /*if(dV >= 0 && dV < mppt->priv.v_min_step){
            //dV = mppt->priv.v_min_step;
        }
        else if(dV < 0 && dV > -mppt->priv.v_min_step){
            //dV = -mppt->priv.v_min_step;
        }*/

        dPdV                  = dP / dV;
        C                     = sqrt(power) * fabsf(dPdV);
        dC                    = C - C_old;
        dCdV                  = dC / dV;
        mppt->priv.debug_dCdV = dCdV;
        S                     = fabsf(dPdV) / sqrtf(1.0 + (dPdV * dPdV)) * mppt->priv.v_max_step;
        if (S < 2.0 * mppt->priv.v_min_step) {
            S                 = 2.0 * mppt->priv.v_min_step;
            mppt->priv.p_mppt = power;
        }

        if (dPdV == 0) {
            V_step = 0;
        } else if (dPdV > 0) {
            if (dCdV > 0) {
                V_step = mppt->priv.v_max_step;
            } else {
                V_step = S;
            }
        } else {
            if (dCdV >= 0) {
                V_step = -S;
            } else {
                V_step = -mppt->priv.v_max_step;
            }
        }
        mppt->priv.debug_step      = V_step;
        mppt->priv.v_ref           = mppt->priv.v_ref + V_step;
        mppt->priv.p_old           = power;
        mppt->priv.dPdV            = dPdV;
        mppt->priv.debug_dE        = dPdV - mppt->priv.debug_dPdV_prev;
        mppt->priv.debug_dPdV_prev = dPdV;
        mppt->priv.v_old           = pv_voltage;
        C_old                      = C;
    }

/** P&O Method **/
/*if(dV == 0){
    S = V_step;
}
else{
    dPdV = dP/dV;
    C = power* fabs(dPdV);
    dC = C - C_old;
    dCdV = dC/dV;
    S = fabs(dPdV)/sqrt(1.0+(dPdV*dPdV))*mppt->usr.max_v_step;
}

if(dP > 0){
    V_step = mppt->usr.max_v_step;//S;
}
else if(dP < 0){
    V_step = -mppt->usr.max_v_step;//-fabs(S);

}
else{
    V_step = 0;
}
mppt->priv.v_ref = mppt->priv.v_ref+V_step;
mppt->priv.p_old = power;
mppt->priv.v_old = pv_voltage;
C_old = C;*/

/* Fuzzy logic method */
#ifdef FUZZY_LOGIC
    float output;
    if (dV >= 0 && dV < mppt->priv.v_min_step) {
        dV = mppt->priv.v_min_step;
    } else if (dV < 0 && dV > -mppt->priv.v_min_step) {
        dV = -mppt->priv.v_min_step;
    }
    dPdV                = dP / dV;
    mppt->priv.debug_dE = dPdV - mppt->priv.debug_dPdV_prev;
    fuzzification(dPdV, mppt->priv.debug_dE);
    rule_evaluation();
    output = defuzzification();
    V_step = output * mppt->priv.v_max_step;

    if (V_step > 0 && V_step < 1 * mppt->priv.v_max_step) {
        V_step = 1 * mppt->priv.v_max_step;
    } else if (V_step < 0 && V_step > -1 * mppt->priv.v_max_step) {
        V_step = -1 * mppt->priv.v_max_step;
    }
    mppt->priv.v_ref           = mppt->priv.v_ref + V_step;
    mppt->priv.dPdV            = dPdV;
    mppt->priv.debug_step      = output;
    mppt->priv.debug_dPdV_prev = dPdV;
    mppt->priv.p_old           = power;
    mppt->priv.v_old           = pv_voltage;
#endif

    return mppt->priv.v_ref;
}

#ifdef FUZZY_LOGIC
void compute_degree_of_membership(struct mem_func *mf, float input)
{
    float delta_left;
    float delta_right;

    delta_left  = input - mf->p_left;
    delta_right = mf->p_right - input;

    if ((delta_left <= 0) || (delta_right <= 0)) /* input outside mem. function ?  */
        mf->value = 0;                           /* then degree of membership is 0 */
    else
        mf->value = min((mf->slope_left * delta_left), (mf->slope_right * delta_right));

    mf->value = min(mf->value, 0.1); /* enforce upper limit */
}

float compute_area_of_trapezoid(struct mem_func *mf)
{
    float run_1;
    float run_2;
    float base;
    float top;
    float area;

    base  = mf->p_right - mf->p_left;
    run_1 = mf->value / mf->slope_left;
    run_2 = mf->value / mf->slope_right;
    top   = base - run_1 - run_2;
    area  = mf->value * (base + top) / 2;

    return (area);
}

void fuzzification(float dPdV, float dE)
{
    int i;

    for (i = 0; i < 5; i++) {
        compute_degree_of_membership(in1_funcs[i], dPdV);
        compute_degree_of_membership(in2_funcs[i], dE);
    }
}

void rule_evaluation(void)
{
    int i;
    float strength; /* strength of  rule currently being evaluated */

    for (i = 0; i < 25; i++) {
        all_rules[i]->out_mf->value = 0;
    }

    for (i = 0; i < 25; i++) {
        strength = 0.1; /* max rule strength allowed */
        strength = min(strength, (all_rules[i]->in1_mf->value));
        strength = min(strength, (all_rules[i]->in2_mf->value));
        /* process then-side of rule to apply strength */
        all_rules[i]->out_mf->value = max(strength, all_rules[i]->out_mf->value);
    }
}

float defuzzification(void)
{
    float sum_of_products; /* sum of products of area & centroid */
    float sum_of_areas;    /* sum of shortend trapezoid area          */
    float area;
    float centroid;
    int i;
    float output_value;

    sum_of_products = 0;
    sum_of_areas    = 0;
    for (i = 0; i < 5; i++) {
        area     = compute_area_of_trapezoid(out_funcs[i]);
        centroid = out_funcs[i]->p_left + (out_funcs[i]->p_right - out_funcs[i]->p_left) / 2;
        sum_of_products += area * centroid;
        sum_of_areas += area;
    }

    output_value = sum_of_products / sum_of_areas; /* weighted average */
    return output_value;
}

float min(float a, float b)
{
    if (a < b)
        return a;
    else
        return b;
}

float max(float a, float b)
{
    if (a < b)
        return b;
    else
        return a;
}
#endif
