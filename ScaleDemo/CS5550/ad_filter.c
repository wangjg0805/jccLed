//ad_filter.c
//--------------------------------------------------------------
#include "stdlib.h"
#include "global.h"
#include "ad_filter.h"

//--------------------------------------------------------------
uint32      ad_dat_idx;
uint32      ad_dat_avg;
uint32      ad_stable_cnt;
//	unsigned int     ad_filter_para;
//--------------------------------------------------------------
void ad_filter_init()
{
	ad_dat_idx=0;
	ad_dat_avg=0;
	ad_stable_cnt=0;
}

//--------------------------------------------------------------
static signed long ad_filter0(signed long ad_new_dat)
{
	 signed long  result;

        return result;
}   
               
//********************************************************
static signed long ad_filter1(signed long ad_new_dat1)
{
  signed long  result;

        return result;
}


static signed long ad_filter2(signed long ad_new_dat2)
{
    signed long  result;

        return result;     
}

//*****************************************************************
uint32  ad_filter(uint32 ad1230_raw_data)
{
        uint32   ad_dat_tmp;
		
        ad_dat_tmp = ad_filter0(ad1230_raw_data);
        ad_dat_tmp = ad_filter1(ad_dat_tmp);
        ad_dat_avg = ad_filter2(ad_dat_tmp);   
		return   ad_dat_avg;                     
}  
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

uint32 GetADStableCnt(void)
{
    return 	  ad_stable_cnt;
}

uint32 GetADDatIdx(void)
{
    return ad_dat_idx;
}

uint32 IsADStable(void)
{
    if(ad_stable_cnt > AD_SPEED_CNT)
	    return 1;
	else
	    return 0;
}
		
