//these are the setting you get after you do reset or use a new arduino
//temp and temp pid related
#define _MySetPoints_0_Temperature 390 
#define _MySetPoints_1_Temperature 390 
#define _MySetPoints_2_Temperature 410 
#define _MySetPoints_3_Temperature 430 
#define _MySetPoints_4_Temperature 440
#define _RoastLength 14
#define _CoolingTimeCompression 5.0
#define _GainTemp 30
#define _IntegralTemp .03
#define _PIDIntegralUdateTimeValueTemp 5000
#define _TBeanAvgRoll_size 10
#define _TBeanAvgRoll_sizemax 10
#define _TCoilAvgRoll_size 30
#define _TCoilAvgRoll_sizemax 30
#define _TimeReadThermoDuration 500
#define _TempCoilTooHot 1000


//bean flow and flow pid related
#define FanSetPoints_0_flow 2.1
#define FanSetPoints_1_flow 2.1
#define FanSetPoints_2_flow 2.9
#define FanSetPoints_3_flow 3.3
#define _UpFlowThreshold -5;
#define _GainFlow 200
#define _IntegralFlow .20
#define _DutyStartingFlow .8
#define _MaxPercentChangePerSecondFlow .7   
#define _PIDIntegralUdateTimeValueFlow 1000
#define _BeanYflow_avg_size 30
#define _BeanYflow_avg_sizemax 30
#define _FanCoolingBoostPercent 1.5
#define _TimeOpticalFlowDuration 20

//current related
#define _CurrentsAvgRoll_size 30
#define _CurrentsAvgRoll_sizemax 30


//display related
#define _CoilTempOffSet -500
#define _OpDetailDisplay_x 600
#define _OpDetailDisplay_y 200
#define _ConfigDisplay_x 300
#define _ConfigDisplay_y 300
#define _OpProgessDisplay_x 0
#define _OpProgessDisplay_y 80
