/**************************************************************************************************
 * 
 * \file ntc.h
 * 
 * \brief NTC temperature sensor interface
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _INC_LIB_NTC_H
#define _INC_LIB_NTC_H

#include "IQmathLib.h"

/**************************************************************************************************
 * 
 * NTC object definition
 * 
 *************************************************************************************************/
struct ntc {
    float T[3];             /* Parameters to calculate temperature from known resistance        */
    float R[4];             /* Parameters to calculate resistance from known temperature        */
    float R_25;             /* NTC resistance at 25 C (ohm)                                     */
    float R_PU;             /* Pull-up resistance in analog circuit (ohm)                       */
};

/**************************************************************************************************
 * 
 * \brief Creates new NTC object
 * 
 * \param None
 * 
 * \return NTC object handler
 * 
 *************************************************************************************************/
extern const struct ntc *
ntc_new(void);

/**************************************************************************************************
 * 
 * \brief Calculates physical temperature from normalized value
 * 
 * \param ntc NTC object handler
 * \param norm normalized value
 * 
 * \return Temperature in degrees Celsius
 * 
 *************************************************************************************************/
extern float
ntc_temperature(const struct ntc *ntc, _iq norm);

/**************************************************************************************************
 * 
 * \brief Calculates normalized value from physical temperature
 * 
 * \param ntc NTC object handler
 * \param T temperature in degrees Celsius
 * 
 * \return Normalized value
 * 
 *************************************************************************************************/
extern _iq
ntc_normalized(const struct ntc *ntc, float temp);

#endif /* _INC_LIB_NTC_H */
