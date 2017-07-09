	//ad_proc.c
	//--------------------------------------------------------------
    #include "filter.h"
    #include "stdlib.h"
    #include "global.h"
	//****************************************************************
    #define AD_INN_REV 300000L
    //****************************************************************
	//--------------------------------------------------------------
	static INT32S src_weight_value;	//the source value of weight.
	static INT32S ad_raw_code;
	static INT32S ad_zero_code;
	static INT32S ad_inn_code;     
	static INT32S ad_tare_code;
	static INT32S aNetw;
	static INT32S grossw;
    static INT32S zero_trace_code;
	//----------------------
	static BOOLEAN b_ad_zero;
	static INT8U ad_sample_cnt;

void ad_proc_init(void)
{

}


//--------------------------------------------------------------
INT32S compute_ad_value(INT32S ad_value,INT32U n,INT32U span)
{
	if(ad_value>=0)
		return (((FP32)ad_value/span)*n+0.5);
	else
		return (((FP32)ad_value/span)*n-0.5);
} 
//--------------------------------------------------------------
void ad_proc(void)
{
	static BOOLEAN b_zero_start=OS_TRUE;
	static INT8U zero_start_time=100; //开机置零时间，防止开机后微小重量引起数值的变动
//	INT32S ad_inn_code;
//	INT32S aNetw=0; 
	INT32S ad_dat_avg_loc=0; 
	demarcate_config ad_demarcate_cfg;
	INT32S aAdValue;
	if(ad_sample(&aAdValue))
	{
		ad_demarcate_cfg = GetDemarcateCfg();
		ad_dat_avg_loc   = ad_filter(aAdValue);	 //filter() rewrite

		ad_inn_code=compute_ad_value(ad_dat_avg_loc-GetAD_Zero_Code(),AD_INN_REV,ad_demarcate_cfg.full_range_ad);
	   	//////////////////////////////////////////////
	   	SetAD_RAW_Value(ad_dat_avg_loc);
		SetADInnerValue(ad_inn_code);
		ad_inn_code= ad_inn_code+zero_trace_code;
		if(b_zero_start==OS_TRUE)
		{ 
		   if(--zero_start_time == 0)
		      b_zero_start=OS_FALSE;
		     else
			  zero_start();
		}
		else
	    ad_zero_track(ad_inn_code);   //自动跟踪功能
	    grossw=compute_ad_value(ad_inn_code,ad_demarcate_cfg.division_degree,AD_INN_REV)*ad_demarcate_cfg.division_degree_value;
		aNetw=compute_ad_value(ad_inn_code-GetADTareValue(),ad_demarcate_cfg.division_degree,AD_INN_REV)*ad_demarcate_cfg.division_degree_value;
	
	//	disp_weigh_trace(ad_inn_code,ad_demarcate_cfg.division_degree);	   //稳定后变化多少个D引起重量变化
	
		ad_sample_cnt++;
		if(aNetw==0)
		    mbAddUpVaild = OS_TRUE;
		SetSrcWeightValue(aNetw);
	}
}

BOOLEAN Getb_ad_zero(void)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif
    BOOLEAN  value;
    OS_ENTER_CRITICAL();
	value = b_ad_zero;
	OS_EXIT_CRITICAL();
	return 	 value;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
INT32S GetAD_RAW_Value(void)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif
    INT32S value;
    OS_ENTER_CRITICAL();
	value = ad_raw_code;
	OS_EXIT_CRITICAL();
	return 	 value;
}

void SetAD_RAW_Value(INT32S value)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif
    OS_ENTER_CRITICAL();
	ad_raw_code = value;
	OS_EXIT_CRITICAL();
}																										  
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
INT32S GetADInnerValue(void)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif
    INT32S value;
    OS_ENTER_CRITICAL();
	value = ad_inn_code;
	OS_EXIT_CRITICAL();
	return 	 value;
}

void SetADInnerValue(INT32S value)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif
    OS_ENTER_CRITICAL();
	ad_inn_code = value;
	OS_EXIT_CRITICAL();
}																										  
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
     Function: GetADTareValue()   
  Describtion: Get Inner value of AD.
       return: Inner AD Value,which from rasor.
	     note:
*/
INT32S GetADTareValue(void)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif
    INT32S value;
    OS_ENTER_CRITICAL();
	value = ad_tare_code;
	OS_EXIT_CRITICAL();
	return 	 value;
}

/*
     Function: SetADTareValue(INT32S)
  Describtion: Set Inner value of AD,which is used to save current using.
        notes: 
*/
void SetADTareValue(INT32S value)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif
    OS_ENTER_CRITICAL();
	ad_tare_code = value;
	OS_EXIT_CRITICAL();
}


///////////////////////////////////////////////////////////////////////
void  SetAD_Zero_Code(INT32S value)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif
    OS_ENTER_CRITICAL();
	ad_zero_code = value;
	OS_EXIT_CRITICAL();
}
INT32S GetAD_Zero_Code(void)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif
INT32S value;
    OS_ENTER_CRITICAL();
	value=ad_zero_code;
	OS_EXIT_CRITICAL();
	return value;
}
/////////////////////////////////////////////////////////////////////////////////////

/*
       Function: GetSrcWeightValue()
	Describtion: Get source value of weight.
	     return: the source value of weight.
		   note:
*/
INT32S GetSrcWeightValue(void)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif
    INT32S value;
    OS_ENTER_CRITICAL();
	value = src_weight_value;
	OS_EXIT_CRITICAL();
	return 	 value;
}
/*
    Function: SetSrcWeightValue(INT32S)
 Describtion: Set source value of weight,which is used to save current weight.
        note:
*/
void SetSrcWeightValue(INT32S value)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif
    OS_ENTER_CRITICAL();
	src_weight_value = value;
	OS_EXIT_CRITICAL();
}  

//--------------------------------------------------------------
INT8U ad_zero()
{
//	static INT32S has_zero_code=0;
//	INT32S aAdInnCode = GetADInner_Tare_Value();		 //置零条件的设定
	if( (GetADStableCnt()>AD_SPEED_CNT) && (ABS(GetADInnerValue()) <=(AD_INN_REV)/10))
	{
	   // has_zero_code=GetADInnerValue();  //只是用于作为一种置零条件的出现
		SetAD_Zero_Code(GetAD_RAW_Value());
		zero_trace_code = 0 ;
		return 1;
	}
	else
	return 0;
}

//--------------------------------------------------------------
INT8U ad_tare()		 //去皮处理，把当前的重量数值内码作为去皮量
{
	demarcate_config ad_demarcate_cfg;
	if(GetADStableCnt()<AD_SPEED_CNT)
		return 0;
	ad_demarcate_cfg = GetDemarcateCfg();
	if( (grossw > 0) && (grossw<=ad_demarcate_cfg.division_degree*ad_demarcate_cfg.division_degree_value))
	  {
		SetADTareValue(GetADInnerValue());
		return 1;
	   }
	if(grossw==0)
	  {
		SetADTareValue(0);	
		return 1;
	  }
}
//--------------------------------------------------------------
INT8U zero_start()							  //开机允许有一定的重量在，做置零处理
{
	const INT8U ZERO_START_RANGE=10;

	if(ABS(GetADInnerValue())<=(AD_INN_REV*ZERO_START_RANGE)/100)
	{
		if(GetADStableCnt()>AD_SPEED_CNT)
		{
	    	SetAD_Zero_Code(GetAD_RAW_Value());
			zero_trace_code = 0 ;
			return 1;
		}
	}
	return 0;
}

//--------------------------------------------------------------
 
BOOLEAN ad_sample(INT32S* value)
{
	BOOLEAN bRet = OS_FALSE;	
	static unsigned int no_ad_dat_count = 0;

//	P2M1 &= 0xdf;         //set pin DOUT input mode	

	IO_ADS1230_SET &= ADS1230_DOUT ; //ADS1230_DOUT = 1;     // ADS1230_DOUT_MODE_SET_INPUT;

    //if (ADS1230_DOUT == 0)
	if (( IO_ADS1230_PIN & ADS1230_DOUT ) == 0 )
    {	
		no_ad_dat_count = 0;

		(*value) = ad_sample_simple();
/*		if (ad_sample_dat == 8388607 || ad_sample_dat == -8388593)//
		{
			ads1230_init();
			while (b_HasGetNewADDat == 0) 
			{
        		ad_sample_simple();
			}
			ADS1230_SCLK = 0;
			_nop_();_nop_();
			ADS1230_SCLK = 1;
			_nop_();_nop_();
			ADS1230_SCLK = 0; 
			b_HasGetNewADDat = 0;		
		}	*/ //  抗静电特殊处理程序段
	 return(1);
	}
	else
	{
		no_ad_dat_count++;
		if (no_ad_dat_count > 10000) 
		{
			ads1230_init();
			while (b_HasGetNewADDat == 0) 
			{
	        	ad_sample_simple();
			}
			no_ad_dat_count = 0;
			b_HasGetNewADDat = 0;
		}
	return(0);
	}           ////  抗静电特殊处理程序段
		
}
#if 0
//******************************************************************
void netw_kela_compute()
{
	demarcate_config ad_demarcate_cfg;
	ad_demarcate_cfg = GetDemarcateCfg();
	
	switch(ad_demarcate_cfg.weight_dot_pos) {
	case 0:
	    netw_kela = netw * 5;
	    break;
	case 1:
	    netw_kela = (netw * 5+5)/10;
	    break;
	case 2:
	    netw_kela = (netw * 5 + 5)/10;
	    break; 
	case 3:
	    netw_kela = (netw * 5 + 5)/10;
	    break; 
	case 4:
	    netw_kela = (netw * 5 + 5)/10;
	    break;
	}
}
//****************************************
void netw_angsi_compute()
{
	demarcate_config ad_demarcate_cfg;
	ad_demarcate_cfg = GetDemarcateCfg();
	
	switch(ad_demarcate_cfg.weight_dot_pos) {
	case 0:
		netw_angsi = (netw * 3527 + 5000)/10000;
		break;
	case 1:
		netw_angsi = (netw * 3527 + 5000)/10000;
		break;
	case 2:
		netw_angsi = (netw * 3527 + 5000)/10000;
		break; 
	case 3:
		netw_angsi = (netw * 3527 + 5000)/10000;
		break; 
	case 4:
		netw_angsi = 0;
		break;
	}
}
void netw_bang_compute()
{
	FP32 netw_buf;
	
	demarcate_config ad_demarcate_cfg;
	ad_demarcate_cfg = GetDemarcateCfg();
	
	switch(ad_demarcate_cfg.weight_dot_pos) {
	case 0:
	    netw_buf = netw * 2.2046;
	    break;
	case 1:
	    netw_buf = (netw * 2.2046 )/10;
	    break;
	case 2:
	    netw_buf = (netw * 2.2046 )/100;
	    break; 
	case 3:
	    netw_buf = (netw * 2.2046)/1000;
	    break; 
	case 4:
	    netw_buf = (netw * 2.2046)/10000;
	    break;	
	}
	netw_bang = (INT32S)netw_buf;
	netw_buf  = (netw_buf - netw_bang * 1.0) *160.0;
	netw_bang = netw_bang * 1000 + (INT32S)netw_buf; 
}
#endif
