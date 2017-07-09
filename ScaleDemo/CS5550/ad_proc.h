	//ad_proc.h
#ifndef _AD_PROC_H_
#define _AD_PROC_H_
#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif
	#include<includes.h>
	//--------------------------------------------------------------
	
	//INT8U b_ad_zero;
	//INT8U ad_sample_cnt;
	//--------------------------------------------------------------
	void ad_proc_init(void);
	void  ad_proc(void);
	INT8U ad_zero(void);
	INT8U ad_tare(void);
	INT8U zero_start(void);
	//--------------------------------------------------------------
	INT32S GetADInnerValue(void);   //Get Inner value of AD.
	void SetADInnerValue(INT32S);   //Set Inner value of AD,which is used to save current using.

	INT32S GetADTareValue(void);
	void SetADTareValue(INT32S);

	INT32S GetSrcWeightValue(void);	 //Get source value of weight.
	void SetSrcWeightValue(INT32S);  //Set source value of weight,which is used to save current weight.

	void  SetAD_Zero_Code(INT32S);
    INT32S GetAD_Zero_Code(void);
	
	INT32S GetAD_RAW_Value(void);
	void SetAD_RAW_Value(INT32S);
	
	BOOLEAN Getb_ad_zero(void);
#if defined(__cplusplus)
}
#endif 
#endif
