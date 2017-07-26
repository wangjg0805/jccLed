//ad_proc.c
//--------------------------------------------------------------
#include  "global.h"
#include  "ad_filter.h"

#include  "i2c.h"
#include  "math.h"


void auto_zero_proc(void)
{ 
    if(labs(MData.ad_dat_avg-MData.ad_zero_data) < FilterData.ad_filter_para*5) {
	    FilterData.zero_track_cnt++;
	    if(FilterData.zero_track_cnt > 20) {
	        FilterData.zero_track_cnt = 0;
	        MData.ad_zero_data = MData.ad_dat_avg;		 
	    }
	} else  
	    FilterData.zero_track_cnt = 0;
}

/////////////////////////////////////////////////////////////////////////////////
void load_track_proc(void)
{ 
    u32   ad_tmp;
    static u32 lock_ad_tmp = 0; //本次锁定的重量
    static u32 lock_ad_last = 0; //上次锁定的重量(纯重量内码)
    static u32 ad_zero_code_tmp;          //记忆零点
    u32 ad_weigh_value;         //本次产生的重量内码 
    //载荷跟踪的 重量条件
	ad_weigh_value = labs(MData.ad_dat_avg - MData.ad_zero_data);     //当前的重量小于一定重量 也不进行跟踪
	if(ad_weigh_value < FilterData.ad_filter_para*50)                            
        return;
    ////////////////////////////////////////////开始跟踪
    if(0 == FilterData.load_track_cnt) { //首次进入 首先是重复性检查
        ad_zero_code_tmp = MData.ad_zero_data; 
        lock_ad_tmp = ad_weigh_value;
        if(labs(ad_weigh_value-lock_ad_last) > FilterData.ad_filter_para*2) {//更新此次重量
            lock_ad_last = ad_weigh_value;
        } else {
            lock_ad_last = (lock_ad_last+ad_weigh_value)/2;
        }
		 //get new
         MData.ad_dat_avg = MData.ad_zero_data + lock_ad_last;
         FilterData.load_track_cnt++;
    } else {   
        ///////////////////////////////////////////////       
        FilterData.load_track_cnt++;
        if(FilterData.load_track_cnt > 8) {//载荷跟踪时间到了
            FilterData.load_track_cnt = 1;    
            ad_tmp = labs(ad_weigh_value-lock_ad_last); //比较重量漂移了多少 
            if(ad_tmp < FilterData.ad_filter_para*2) { //重量的变化量如果小于3个，则把这个变化量放到0点上面去 
                //是否更新零点？？？？     不更新是否要好一些？？？  
                //if(ad_weigh_value > lock_ad_tmp) 
                //  ad_zero_code = ad_zero_code + ad_tmp;  
                //else
                //  ad_zero_code = ad_zero_code - ad_tmp;     
                ///////////////////////////////////////////////////// 
                MData.ad_dat_avg = MData.ad_zero_data + lock_ad_last; 
            } else { //重量变化量太大了，载荷跟踪自杀 对数据不予修正
                FilterData.load_track_enable = 0;
                FilterData.load_track_cnt = 0;            
            }
        } else {    //载荷跟踪时间未到 重量不发生变化
            MData.ad_dat_avg = MData.ad_zero_data + lock_ad_last;    //锁定
        }  
    }
}

/////////////////////////////////////////////////////////////////////////
void  do_tare_proc(void)
{	
	;  
}

//******************************************************************
void do_zero_proc()//手动置零+-2%
{
    //手动置零 条件
   static u32 has_zero_code=0;
  
}


//********************************************************************


