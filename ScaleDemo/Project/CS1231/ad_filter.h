//ad_filter.h

#define AD_FILTER_LENGTH 8

extern  void   ad_filter_init(void);
extern  void   ad_filter(u32 ad_data);

extern  void   ad_battery_temp_init(void);
extern  u16 battery_filter(u16 ad_data);
extern  void   temp_filter(u16 ad_data);



	
