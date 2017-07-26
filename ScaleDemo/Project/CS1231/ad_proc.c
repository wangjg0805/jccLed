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
    static u32 lock_ad_tmp = 0; //��������������
    static u32 lock_ad_last = 0; //�ϴ�����������(����������)
    static u32 ad_zero_code_tmp;          //�������
    u32 ad_weigh_value;         //���β������������� 
    //�غɸ��ٵ� ��������
	ad_weigh_value = labs(MData.ad_dat_avg - MData.ad_zero_data);     //��ǰ������С��һ������ Ҳ�����и���
	if(ad_weigh_value < FilterData.ad_filter_para*50)                            
        return;
    ////////////////////////////////////////////��ʼ����
    if(0 == FilterData.load_track_cnt) { //�״ν��� �������ظ��Լ��
        ad_zero_code_tmp = MData.ad_zero_data; 
        lock_ad_tmp = ad_weigh_value;
        if(labs(ad_weigh_value-lock_ad_last) > FilterData.ad_filter_para*2) {//���´˴�����
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
        if(FilterData.load_track_cnt > 8) {//�غɸ���ʱ�䵽��
            FilterData.load_track_cnt = 1;    
            ad_tmp = labs(ad_weigh_value-lock_ad_last); //�Ƚ�����Ư���˶��� 
            if(ad_tmp < FilterData.ad_filter_para*2) { //�����ı仯�����С��3�����������仯���ŵ�0������ȥ 
                //�Ƿ������㣿������     �������Ƿ�Ҫ��һЩ������  
                //if(ad_weigh_value > lock_ad_tmp) 
                //  ad_zero_code = ad_zero_code + ad_tmp;  
                //else
                //  ad_zero_code = ad_zero_code - ad_tmp;     
                ///////////////////////////////////////////////////// 
                MData.ad_dat_avg = MData.ad_zero_data + lock_ad_last; 
            } else { //�����仯��̫���ˣ��غɸ�����ɱ �����ݲ�������
                FilterData.load_track_enable = 0;
                FilterData.load_track_cnt = 0;            
            }
        } else {    //�غɸ���ʱ��δ�� �����������仯
            MData.ad_dat_avg = MData.ad_zero_data + lock_ad_last;    //����
        }  
    }
}

/////////////////////////////////////////////////////////////////////////
void  do_tare_proc(void)
{	
	;  
}

//******************************************************************
void do_zero_proc()//�ֶ�����+-2%
{
    //�ֶ����� ����
   static u32 has_zero_code=0;
  
}


//********************************************************************


