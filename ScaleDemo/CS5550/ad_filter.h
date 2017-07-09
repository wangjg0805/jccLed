    //ad_filter.h
    #ifndef _AD_FILTER_H_
    #define _AD_FILTER_H_
    //--------------------------------------------------------------
	#define AD_SPEED_CNT  8
	#define AD_SPEED_CNT1 8
	#define AD_SPEED_CNT2 8
	#define AD_FILTER1_MAX_MIN_ON 1
	#define AD_FILTER2_MAX_MIN_ON 1
	//--------------------------------------------------------------
 	void    ad_filter_init(void);
 	uint32  ad_filter(uint32);
	uint32  GetADStableCnt(void);
	uint32  GetADDatIdx(void);
	uint32  IsADStable(void);
	//--------------------------------------------------------------
    #endif
