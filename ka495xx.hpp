/**
 * @file ka495xx.hpp
 * @author Kyungwoo Min (bluelife85@naver.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef __DRIVER_KA495XX_H__
#define __DRIVER_KA495XX_H__

#include "ka495xx_sfr.hpp"

class Ka495xx
{
public:
   RegPWR_CTRL PWR_CTRL;
   RegINT_EN INT_EN;
   RegSPIWD_CTL SPIWD;
   RegFDRV_CTRL FDRV_CTRL;
   RegCVSEL CVSEL;
   RegGVSEL GVSEL;
   RegFUSE FUSE;
   RegOUVCTL OUVCTL;
   RegOPMODE OPMODE;
   RegGPIO123 GPIO1;
   RegGPIO123 GPIO2;
   RegGPIO123 GPIO3;
   RegGPIOHV GPOH;
   RegGPIO4 GPIO4;
   RegALARM ALARM;
   RegCBSEL CBSEL;
   RegOTHCTL OTHCTL;
   RegADCTL ADCTL;
   RegINRCV INRCV;
   RegINR_CTL_DIAG_EN INR_CTL_DIAG_EN;
   RegSTAT STAT;
   RegCV_AD CV_AD[22];
   RegVPACK_AD VPACK_AD;
   RegTMONI_AD TMONI_AD[5];
   RegVDD55_AD VDD55_AD;
   RegGPIO12_AD GPIO12_AD[2];
   RegCVI_AD CVI_AD;
   RegVDD18_AD VDD18_AD;
   RegREGEXT_AD REGEXT_AD;
   RegVREF2_AD VREF2_AD;
   RegOVLSTAT OVLSTAT;
   RegUVLSTAT UVLSTAT;
   RegCBSTAT CBSTAT;
   RegAUTO_ITHL AUTOH_ITHL;
   RegVDD55_CTL VDD55_CTL;
   RegTMONRES TMON_Resistance;
   Ka495xx();
   void setHw(ka495xx_hardwares* hw);
   bool sleep();
   bool wakeup();
   bool update();
private:
   Ka495xx_interface interface;
   bool isInitialized;
   void setDefault();
};

#endif /* __DRIVER_KA495XX_H__ */
