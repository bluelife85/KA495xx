/**
 * @file ka495xx.cpp
 * @author Kyungwoo Min (bluelife85@naver.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "ka495xx.hpp"

Ka495xx::Ka495xx()
{
   unsigned long i;

   PWR_CTRL.hal(&this->interface);
   INT_EN.hal(&this->interface);
   SPIWD.hal(&this->interface);
   FDRV_CTRL.hal(&this->interface);
   CVSEL.hal(&this->interface);
   GVSEL.hal(&this->interface);
   FUSE.hal(&this->interface);
   OUVCTL.hal(&this->interface);
   OPMODE.hal(&this->interface);
   GPIO1.hal(&this->interface, GPIOId::GPIO1);
   GPIO2.hal(&this->interface, GPIOId::GPIO2);
   GPIO3.hal(&this->interface, GPIOId::GPIO3);
   GPOH.hal(&this->interface);
   GPIO4.hal(&this->interface);
   ALARM.hal(&this->interface);
   CBSEL.hal(&this->interface);
   OTHCTL.hal(&this->interface);
   ADCTL.hal(&this->interface);
   INRCV.hal(&this->interface);
   INR_CTL_DIAG_EN.hal(&this->interface);
   STAT.hal(&this->interface);
   for(i = 0;i < 22; i++)
   {
      CV_AD[i].config(&this->interface, static_cast<Ka495xx_addr>(i + 0x28));
   }
   
   VPACK_AD.config(&this->interface);
   for(i = 0;i < 5; i++)
   {
      TMONI_AD[i].config(&this->interface, static_cast<Ka495xx_addr>(i + 0x3F));
   }
   
   VDD55_AD.config(&this->interface);
   GPIO12_AD[0].config(&this->interface, Ka495xx_addr::GPIO1AD);
   GPIO12_AD[1].config(&this->interface, Ka495xx_addr::GPIO2AD);
   CVI_AD.config(&this->interface);
   VDD18_AD.config(&this->interface);
   REGEXT_AD.config(&this->interface);
   VREF2_AD.config(&this->interface);
   OVLSTAT.hal(&this->interface);
   UVLSTAT.hal(&this->interface);
   CBSTAT.hal(&this->interface);
   AUTOH_ITHL.hal(&this->interface);
   VDD55_CTL.hal(&this->interface);
   TMON_Resistance.hal(&this->interface);
}

void Ka495xx::setHw(ka495xx_hardwares *hw)
{
   unsigned long i;

   interface.regist(hw);
   isInitialized = true;
   
   setDefault();

   PWR_CTRL.update();
   INT_EN.update();
   SPIWD.update();
   FDRV_CTRL.update();
   CVSEL.update();
   GVSEL.update();
   FUSE.update();
   OUVCTL.update();
   OPMODE.update();
   GPIO1.update();
   GPIO2.update();
   GPIO3.update();
   GPOH.update();
   GPIO4.update();
   ALARM.update();
   CBSEL.update();
   OTHCTL.update();
   ADCTL.update();
   INRCV.update();
   INR_CTL_DIAG_EN.update();
   STAT.update();
   for(i = 0;i < 22; i++)
   {
      CV_AD[i].update();
   }
   
   VPACK_AD.config(&this->interface);
   for(i = 0;i < 5; i++)
   {
      TMONI_AD[i].update();
   }
   
   VDD55_AD.update();
   GPIO12_AD[0].update();
   GPIO12_AD[1].update();
   CVI_AD.update();
   VDD18_AD.update();
   REGEXT_AD.update();
   VREF2_AD.update();
   OVLSTAT.update();
   UVLSTAT.update();
   CBSTAT.update();
   AUTOH_ITHL.update();
   VDD55_CTL.update();
   TMON_Resistance.update();
}

bool Ka495xx::sleep()
{
   if(!isInitialized) return false;
   PWR_CTRL.setMSET_SLP(bitdefMSET_SLP::sleep).write();
   PWR_CTRL.setMSET_SLP(bitdefMSET_SLP::normal);
   return true;
}

bool Ka495xx::wakeup()
{
   if(!isInitialized) return false;
   interface.wakeup();
   return true;
}

bool Ka495xx::update()
{
   unsigned long i;

   if(!isInitialized) return false;
   STAT.update();
   for(i = 0;i < 22; i++)
   {
      CV_AD[i].update();
   }
   
   VPACK_AD.config(&this->interface);
   for(i = 0;i < 5; i++)
   {
      TMONI_AD[i].update();
   }
   
   VDD55_AD.update();
   GPIO12_AD[0].update();
   GPIO12_AD[1].update();
   CVI_AD.update();
   VDD18_AD.update();
   REGEXT_AD.update();
   VREF2_AD.update();
   OVLSTAT.update();
   UVLSTAT.update();
   CBSTAT.update();
   return true;
}

void Ka495xx::setDefault()
{
   const unsigned short KA49522_DefaultValue[31] = 
   {
      0x1D00, 0x0045, 0x8781, 0xD000,
      0xFFFF, 0x003F, 0x707F, 0x0000,
      0x000A, 0x0B9A, 0x3333, 0x0000,
      0x0D00, 0x0B00, 0x0400, 0x0000,
      0x639F, 0x800F, 0x0821, 0x0400,
      0x0000, 0x0000, 0x0700, 0x3203,
      0x0000, 0x0000, 0x0000, 0x8006,
      0x0000, 0x0000, 0x0000
   };
   const Ka495xx_addr matchAddresses[31] = 
   {
      Ka495xx_addr::INTEN, Ka495xx_addr::SPIWD, Ka495xx_addr::PWR_CTRL, Ka495xx_addr::FDRV,
      Ka495xx_addr::CVSEL1, Ka495xx_addr::CVSEL2, Ka495xx_addr::GVSEL, Ka495xx_addr::FUSE1,
      Ka495xx_addr::FUSE2, Ka495xx_addr::OUVCTL1, Ka495xx_addr::OUVCTL2, Ka495xx_addr::OPMODE,
      Ka495xx_addr::GPIO1, Ka495xx_addr::GPIO2, Ka495xx_addr::GPIO3, Ka495xx_addr::GPIOH,
      Ka495xx_addr::GPIO4, Ka495xx_addr::ALARM1, Ka495xx_addr::ALARM2, Ka495xx_addr::ALARM3,
      Ka495xx_addr::CBSEL1, Ka495xx_addr::CBSEL2, Ka495xx_addr::OTH, Ka495xx_addr::ADC,
      Ka495xx_addr::INRCV1, Ka495xx_addr::INRCV2, Ka495xx_addr::INR_DIAG_EN, Ka495xx_addr::ANA,
      Ka495xx_addr::FUSE_BLOW, Ka495xx_addr::AUTO_ITHL, Ka495xx_addr::VDD55
   };
   union
   {
      unsigned short hfword;
      unsigned char bytes[2];
   } data;
   unsigned long i;

   data.hfword = 0x0380;
   interface.write(Ka495xx_addr::PWR_CTRL, &data.bytes[0]);
   interface.delay(1);
   for(i = 0;i < 31; i++)
   {
      data.hfword = KA49522_DefaultValue[i];
      interface.write(matchAddresses[i], &data.bytes[0]);
   }
}
