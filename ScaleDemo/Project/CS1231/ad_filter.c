
//ad_filter.c
//--------------------------------------------------------------
#include  "global.h"
#include  "ad_filter.h"
#include  "math.h"														  
 
#define  RAW_DATA_MAG              8		 //ԭʼ������С��������������
//--------------------------------------------------------------

static u32 ad_dat[AD_FILTER_LENGTH];
static u8  flag_ad_buf_full = 0;
static u8  ad_dat_idx,ad_stable_cnt;
static u8  fast_filter_flag = 0;

//--------------------------------------------------------------
void ad_filter_init(void)
{
	u8   i;
	for(i=0;i<AD_FILTER_LENGTH;i++)
		ad_dat[i]=0;
	ad_dat_idx = 0;
 	ad_stable_cnt = 0;
}

//--------------------------------------------------------------
//*************************************************************
u32 ad_filter0(u32 ad_new_dat)
{
    static u8 direct_add_cnt     = 0;
    static u8 direct_sub_cnt     = 0;
    static u8 filter0_stable_cnt = 0;    
    static u32 last_ad_dat  = 0;  
	if(labs(ad_new_dat-last_ad_dat) > FilterData.ad_filter_para*3) {  //�����������ݲ��ܴ� 	         
        if(ad_new_dat >= last_ad_dat) {
            direct_add_cnt++;
	        if(direct_add_cnt>200)
	            direct_add_cnt = 50;
	            direct_sub_cnt = 0;
	    } else {
	        direct_sub_cnt++;
	        if(direct_sub_cnt>200)
	            direct_sub_cnt = 50;
	        direct_add_cnt = 0;
        }
	    last_ad_dat = ad_new_dat; 
	    if((direct_add_cnt>3)||(direct_sub_cnt>3)) { //�������ݴ���2D �����仯3�ξͽ�������˲�ģʽ
	     fast_filter_flag = 1;
	     filter0_stable_cnt = 0;
	     flag_ad_buf_full = 0;  //�ƻ��������ݣ����¿�ʼ���� 
 	     ad_dat_idx       = 0;
	    }
	    //�������α仯����2D �Ͳ������Զ�����
        FilterData.zero_track_enable  = 0;
        FilterData.zero_track_cnt     = 0;
        FilterData.load_track_enable  = 0;
        FilterData.load_track_cnt     = 0;
     } else {
         filter0_stable_cnt++;
         if(filter0_stable_cnt > 200)
             filter0_stable_cnt =  30;
         //���ȶ���ʹ����������ֵ��Ȩ��Ϊ��������  
         if(filter0_stable_cnt < 6) {   //�Ӵ�С����  
             last_ad_dat = ad_new_dat;
         } else if(filter0_stable_cnt < 10) {
			 last_ad_dat = (ad_new_dat*9+last_ad_dat)/10; 
         } else if(filter0_stable_cnt < 15) {
             last_ad_dat = (ad_new_dat*8+last_ad_dat*2)/10;    //����
         } else {
             last_ad_dat = (ad_new_dat*7+last_ad_dat*3)/10;    //����
         }     
         ////////////////////////   
         if(filter0_stable_cnt > 2) { //��������������2�� С��2D ʱ�򣬿����˲�ģʽ��ֹ
             direct_add_cnt = 0;
	         direct_sub_cnt = 0;    
             fast_filter_flag = 0;  //
        } 
        ///////////////////////////////��������������8�� С��2D ʱ�򣬴��Զ�����        
        if(filter0_stable_cnt > 8) {
			FilterData.zero_track_enable = 1;
			FilterData.zero_track_cnt    = 0;
	    }
    }   	  
	return last_ad_dat;
}    
//********************************************************
u32 get_buf_data(void)
{   //��ǰ������������ȡ���������Сֵ���ƽ��ֵ
    //���û����  ����ȡ �������ݵ�ƽ��ֵ
    u32 sum;
    u8 pass,i;
    if(1 == flag_ad_buf_full) {
        for(pass = 0;pass < AD_FILTER_LENGTH - 1 ;pass++) {
            for( i = 0;i < (AD_FILTER_LENGTH - 1 - pass);i++) {
                if(ad_dat[i] > ad_dat[i+1]) {
                    sum = ad_dat[i];
		   	        ad_dat[i] = ad_dat[i+1];
			        ad_dat[i+1] = sum ;
			    }
		    }
	    }
	    sum = 0;  
	    for(i=2;i<6;i++)   
	        sum += ad_dat[i]; //+ad_dat[3]+ad_dat[4]+ad_dat[5]+2;
	        sum += 2;
	        return(sum/4);
    } else {
        sum = 0;
        for(i=0;i<ad_dat_idx;i++)
            sum += ad_dat[i];         
 	    sum += ad_dat_idx/2;
 	    return(sum/ad_dat_idx); 
    }
}     
///////////////////////////////////////////////////////////////////////////   
           
void ad_filter(u32 ad_data) 
{
    u32 tmp;
    tmp = ad_filter0(ad_data); 
    //////////////////////////////////////////
    if(1==fast_filter_flag) {
        ad_stable_cnt  = 0;
        RunData.stable_flag    = 0;
		//auto_off_cnt   = 0;
 	    MData.ad_dat_avg = tmp/RAW_DATA_MAG;   //���ٱ仯�׶�
    } else {
        /////////////////////////////////////////////////
 	    if(0==flag_ad_buf_full)
 	        ad_dat[ad_dat_idx++] = tmp;    //save 
        else {// ����������ٽ����ȶ���ʱ ������ʱ����ȡ�����ݵ�Ȩ
            ad_stable_cnt++;
 	        if(ad_stable_cnt  > 250)
 	            ad_stable_cnt  = 50; 
 	        ///////////////////////////�ȶ���ʱ
			if(ad_stable_cnt > 10)
			    RunData.stable_flag = 1;
			else {//�������ȶ�
				//auto_off_cnt = 0;
		    }
			///////////////////////////////////////////////////    
            if(ad_stable_cnt < 5) {
                if(0 == ad_dat_idx)
 	                ad_dat[ad_dat_idx++] = (tmp*9+ad_dat[AD_FILTER_LENGTH-1])/10;
 	            else
 	                ad_dat[ad_dat_idx++] = (tmp*9+ad_dat[ad_dat_idx-1])/10;     
            } else if(ad_stable_cnt<10){
                if(0 == ad_dat_idx)
                    ad_dat[ad_dat_idx++] = (tmp*8+ad_dat[AD_FILTER_LENGTH-1]*2)/10;
 	            else
 	                ad_dat[ad_dat_idx++] = (tmp*8+ad_dat[ad_dat_idx-1]*2)/10;  
            } else {
                if(0 == ad_dat_idx)
 	                ad_dat[ad_dat_idx++] = (tmp*6+ad_dat[AD_FILTER_LENGTH-1]*4)/10;
 	            else
 	                ad_dat[ad_dat_idx++] = (tmp*6+ad_dat[ad_dat_idx-1]*4)/10;  
            }       
 	    }
 	    //�õ�������  
 	    MData.ad_dat_avg = get_buf_data()/RAW_DATA_MAG;  
 	    if(AD_FILTER_LENGTH == ad_dat_idx) {
            flag_ad_buf_full = 1; 
            ad_dat_idx = 0;
        }     
    }
  
    if(ad_stable_cnt>2) {//������˲������  �������غɸ���
	 	FilterData.load_track_enable = 1;
        FilterData.load_track_cnt    = 0;
    }       
}
