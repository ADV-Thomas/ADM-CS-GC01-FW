/**************************************************************************************************
 * 
 * \file hapi.c
 * 
 * \brief Hardware application interface (rev. 1)
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/
#include "app/hapi.h"

int
hapi_resolve_rev1(struct _hapi *_hapi, unsigned variant)
{
    ASSERT(_hapi && _hapi->hapi);

    int ret;


    ret = hapi_resolve_rev0(_hapi, variant);
    
    /*struct hapi *hapi = _hapi->hapi;
    
    
    _HAPI_MAP_INIT(mcu_led_2, IO72_DOUT);
    _HAPI_MAP_INIT(mcu_led_1, IO84_DOUT);

    _HAPI_MAP_INIT(coding_a, IO10_DIN);
    _HAPI_MAP_INIT(coding_b, IO11_DIN);

    _HAPI_MAP_INIT(screen_rst_n, IO62_DOUT);
    _HAPI_MAP_INIT(screen_d_c, IO58_DOUT);


    _HAPI_MAP_INIT(spi_simo, IO58_SPIA_SIMO);
    _HAPI_MAP_INIT(spi_somi, IO59_SPIA_SOMI);
    _HAPI_MAP_INIT(spi_cs, IO61_SPIA_STE);
    _HAPI_MAP_INIT(spi_sck, IO60_SPIA_CLK);

    _HAPI_MAP_INIT(interlock_write, IO43_DOUT);
    _HAPI_MAP_INIT(interlock_read, IO69_DIN);
*/
   
    
    return ret;
}
