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
#define FanSetPoints_1_flow 3.1
#define FanSetPoints_2_flow 4.9
#define FanSetPoints_3_flow 4.9
#define _UpFlowThreshold -5;
#define _GainFlow 200
#define _MixMax 200
#define _IntegralFlow .20
#define _DutyStartingFlow .8
#define _MaxPercentChangePerSecondFlow .7   
#define _PIDIntegralUdateTimeValueFlow 1000
#define _BeanYflowX_avg_size 5
#define _BeanYflowX_avg_sizemax 5
#define _BeanYflow_avg_size 20
#define _BeanYflow_avg_sizemax 20
#define _FanCoolingBoostPercent 1.5
#define _TimeOpticalFlowDuration 20
#define _FlowSensorMode 1
//ALLPOSITIVE 0 LARGESTPOSTIVE 1 LARGESTAVG 2
#define _FlowSensorErrorUseSqrt 1
//False = 0 True = 1;

//current related
#define _CurrentsAvgRoll_size 3
#define _CurrentsAvgRoll_sizemax 3
#define _CurrentFan_MVPerAmp 208.0
#define _CurrentCoil_MVPerAmp 120.0
#define _AnalogReadRefVoltage 5300.0
#define _TimeReadCurrentsDuration 250
#define _Coil_0 45

//display related
#define _CoilTempOffSet -500
#define _OpDetailDisplay_x 550
#define _OpDetailDisplay_y 200
#define _OpDetailDisplay_rowmax 17
#define _ConfigDisplay_x 300
#define _ConfigDisplay_y 275
#define _ConfigDisplay_rowmax 13
#define _OpProgessDisplay_x 0
#define _OpProgessDisplay_y 80
#define _OpProgessDisplay_rowmax 13

#define _CompileTime_x 5
#define _CompileTime_y 480
#define _InRectBuffer 10
