/**
 * @file ka495xx_sfr.hpp
 * @author Kyungwoo Min (bluelife85@naver.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef __KA495XX_SFR_H__
#define __KA495XX_SFR_H__

typedef bool (*hw_io3)(bool state);
typedef bool (*hw_io2)(bool state);
typedef bool (*hw_io1)(bool state);
typedef bool (*hw_nrst)(void);
typedef bool (*hw_miso)(void);
typedef void (*hw_vpc)(bool state);
typedef void (*hw_sen)(bool state);
typedef void (*hw_sdn)(bool state);
typedef void (*hw_fet_off)(bool state);
typedef unsigned char (*spi_xfer8)(unsigned char data);
typedef void (*hw_delay_ms)(unsigned long ms);

struct ka495xx_hardwares
{
   hw_vpc vpc;
   hw_sen sen;
   hw_nrst nrst;
   hw_miso miso;
   hw_sdn sdn;
   hw_fet_off fet_off;
   hw_io3 io3;
   hw_io2 io2;
   hw_io1 io1;
   spi_xfer8 spi;
   hw_delay_ms delay;
};

enum class Ka495xx_error
{
   success,
   crc,
   invalid
};

template <typename T> class bitprops
{
public:
   T& operator=(const T d) { return value = d;}
   operator T const &() const { return value; }
private:
   T value;
};

enum class Ka495xx_addr
{
   PWR_CTRL          = (0x01),
   INTEN             = (0x02),
   SPIWD             = (0x03),
   FDRV              = (0x04),
   CVSEL1            = (0x05),
   CVSEL2            = (0x06),
   GVSEL             = (0x07),
   FUSE1             = (0x08),
   FUSE2             = (0x09),
   OUVCTL1           = (0x0A),
   OUVCTL2           = (0x0B),
   OPMODE            = (0x0C),
   GPIO1             = (0x0D),
   GPIO2             = (0x0E),
   GPIO3             = (0x0F),
   GPIOH             = (0x10),
   GPIO4             = (0x11),
   ALARM1            = (0x12),
   ALARM2            = (0x13),
   ALARM3            = (0x14),
   CBSEL1            = (0x15),
   CBSEL2            = (0x16),
   OTH               = (0x17),
   ADC               = (0x18),
   INRCV1            = (0x19),
   INRCV2            = (0x1A),
   INR_DIAG_EN       = (0x1B),
   STAT1             = (0x1C),
   STAT2             = (0x1D),
   STAT3             = (0x1E),
   STAT4             = (0x1F),
   ANA               = (0x20),
   OTHSTAT           = (0x21),
   OVSTAT1           = (0x22),
   OVSTAT2           = (0x23),
   UVSTAT1           = (0x24),
   UVSTAT2           = (0x25),
   BIASSTAT          = (0x26),
   STAT5             = (0x27),
   CV01AD            = (0x28),
   CV02AD            = (0x29),
   CV03AD            = (0x2A),
   CV04AD            = (0x2B),
   CV05AD            = (0x2C),
   CV06AD            = (0x2D),
   CV07AD            = (0x2E),
   CV08AD            = (0x2F),

   CV09AD            = (0x30),
   CV10AD            = (0x31),
   CV11AD            = (0x32),
   CV12AD            = (0x33),
   CV13AD            = (0x34),
   CV14AD            = (0x35),
   CV15AD            = (0x36),
   CV16AD            = (0x37),
   CV17AD            = (0x38),
   CV18AD            = (0x39),
   CV19AD            = (0x3A),
   CV20AD            = (0x3B),
   CV21AD            = (0x3C),
   CV22AD            = (0x3D),
   VPACKAD           = (0x3E),
   TMONI1AD          = (0x3F),

   TMONI2AD          = (0x40),
   TMONI3AD          = (0x41),
   TMONI4AD          = (0x42),
   TMONI5AD          = (0x43),
   VDD55AD           = (0x44),
   GPIO1AD           = (0x45),
   GPIO2AD           = (0x46),
   CVIHAD            = (0x47),
   CVILAD            = (0x48),
   VDD18AD           = (0x49),
   EXTAD             = (0x4A),
   VREF2AD           = (0x4B),
   OVLSTAT1          = (0x4D),
   OVLSTAT2          = (0x4E),
   UVLSTAT1          = (0x4F),
   UVLSTAT2          = (0x50),
   CBSTAT1           = (0x51),
   CBSTAT2           = (0x52),
   FUSE_BLOW         = (0x53),

   AUTO_ITHL         = (0x55),
   VDD55             = (0x56),
   TMONI1            = (0x57),
   TMONI23           = (0x58),
   TMONI45           = (0x59)
};

class Ka495xx_interface
{
public:
   hw_vpc vpc;
   hw_delay_ms delay;
   hw_nrst nrst;
   hw_fet_off fet_off;
   hw_io3 io3;
   hw_io2 io2;
   hw_io1 io1;
   hw_sdn sdn;
   Ka495xx_interface() {}
   void regist(struct ka495xx_hardwares* hw)
   {
      vpc = hw->vpc;
      sen = hw->sen;
      nrst = hw->nrst;
      miso = hw->miso;
      fet_off = hw->fet_off;
      sdn = hw->sdn;
      io3 = hw->io3;
      io2 = hw->io2;
      io1 = hw->io1;
      spi = hw->spi;
      delay = hw->delay;
   }

   inline unsigned char swapbit(unsigned char data)
   {
      data = (data & 0xF0) >> 4 | (data & 0x0F) << 4;
      data = (data & 0xCC) >> 2 | (data & 0x33) << 2;
      data = (data & 0xAA) >> 1 | (data & 0x55) << 1;
      return data;
   }

   void wakeup()
   {
      sdn(false);
      vpc(true);
      fet_off(false);
      delay(5);
      vpc(false);

      while(!nrst());
   }

   Ka495xx_error write(Ka495xx_addr addr, unsigned char* data)
   {
      unsigned char bytes[5];
      unsigned long i;

      bytes[0] = 0xE0;
      bytes[1] = static_cast<unsigned char>(addr) << 1;
      bytes[2] = data[1];
      bytes[3] = data[0];
      bytes[4] = crc(&bytes[0], 4);

      sen(true);
      if(!miso())
         return Ka495xx_error::crc;

      for(i = 0;i < 5; i++) 
         spi(bytes[i]);
      sen(false);
      if(!miso())
         return Ka495xx_error::crc;

      return Ka495xx_error::success;
   }

   Ka495xx_error read(Ka495xx_addr addr, unsigned char* data)
   {
      unsigned char bytes[3];
      unsigned char crcValue;
      unsigned long i;

      bytes[0] = 0xE1;
      bytes[1] = static_cast<unsigned char>(addr) << 1;
      bytes[2] = crc(&bytes[0], 4);

      sen(true);
      if(!miso())
         return Ka495xx_error::crc;

      for(i = 0;i < 3; i++) 
         spi(bytes[i]);

      for(i = 0;i < 3; i++) 
         data[i] = spi(0x00);
      
      sen(false);
      if(!miso())
         return Ka495xx_error::crc;
      crcValue = crc(&bytes[0], 2);
      if(data[2] != crcValue)
         return Ka495xx_error::invalid;
      return Ka495xx_error::success;
   }
private:
   spi_xfer8 spi;
   hw_sen sen;
   hw_miso miso;

   const unsigned char crcTable[256] = 
   {
      0x00, 0xD5, 0x7F, 0xAA, 0xFE, 0x2B, 0x81, 0x54, 0x29, 0xFC, 0x56, 0x83, 0xD7, 0x02, 0xA8, 0x7D,
      0x52, 0x87, 0x2D, 0xF8, 0xAC, 0x79, 0xD3, 0x06, 0x7B, 0xAE, 0x04, 0xD1, 0x85, 0x50, 0xFA, 0x2F,
      0xA4, 0x71, 0xDB, 0x0E, 0x5A, 0x8F, 0x25, 0xF0, 0x8D, 0x58, 0xF2, 0x27, 0x73, 0xA6, 0x0C, 0xD9,
      0xF6, 0x23, 0x89, 0x5C, 0x08, 0xDD, 0x77, 0xA2, 0xDF, 0x0A, 0xA0, 0x75, 0x21, 0xF4, 0x5E, 0x8B,
      0x9D, 0x48, 0xE2, 0x37, 0x63, 0xB6, 0x1C, 0xC9, 0xB4, 0x61, 0xCB, 0x1E, 0x4A, 0x9F, 0x35, 0xE0,
      0xCF, 0x1A, 0xB0, 0x65, 0x31, 0xE4, 0x4E, 0x9B, 0xE6, 0x33, 0x99, 0x4C, 0x18, 0xCD, 0x67, 0xB2,
      0x39, 0xEC, 0x46, 0x93, 0xC7, 0x12, 0xB8, 0x6D, 0x10, 0xC5, 0x6F, 0xBA, 0xEE, 0x3B, 0x91, 0x44,
      0x6B, 0xBE, 0x14, 0xC1, 0x95, 0x40, 0xEA, 0x3F, 0x42, 0x97, 0x3D, 0xE8, 0xBC, 0x69, 0xC3, 0x16,
      0xEF, 0x3A, 0x90, 0x45, 0x11, 0xC4, 0x6E, 0xBB, 0xC6, 0x13, 0xB9, 0x6C, 0x38, 0xED, 0x47, 0x92,
      0xBD, 0x68, 0xC2, 0x17, 0x43, 0x96, 0x3C, 0xE9, 0x94, 0x41, 0xEB, 0x3E, 0x6A, 0xBF, 0x15, 0xC0,
      0x4B, 0x9E, 0x34, 0xE1, 0xB5, 0x60, 0xCA, 0x1F, 0x62, 0xB7, 0x1D, 0xC8, 0x9C, 0x49, 0xE3, 0x36,
      0x19, 0xCC, 0x66, 0xB3, 0xE7, 0x32, 0x98, 0x4D, 0x30, 0xE5, 0x4F, 0x9A, 0xCE, 0x1B, 0xB1, 0x64,
      0x72, 0xA7, 0x0D, 0xD8, 0x8C, 0x59, 0xF3, 0x26, 0x5B, 0x8E, 0x24, 0xF1, 0xA5, 0x70, 0xDA, 0x0F,
      0x20, 0xF5, 0x5F, 0x8A, 0xDE, 0x0B, 0xA1, 0x74, 0x09, 0xDC, 0x76, 0xA3, 0xF7, 0x22, 0x88, 0x5D,
      0xD6, 0x03, 0xA9, 0x7C, 0x28, 0xFD, 0x57, 0x82, 0xFF, 0x2A, 0x80, 0x55, 0x01, 0xD4, 0x7E, 0xAB,
      0x84, 0x51, 0xFB, 0x2E, 0x7A, 0xAF, 0x05, 0xD0, 0xAD, 0x78, 0xD2, 0x07, 0x53, 0x86, 0x2C, 0xF9
   };

   unsigned char crc(unsigned char* data, unsigned long size)
   {
      unsigned long i;
      unsigned char crc = 0xd5;

      for(i = 0;i < size;i++)
      {
         crc = crcTable[crc ^ data[i]];
      }

      return crc;
   }
};

enum class bitdefADC_CONT : unsigned char { once = 0x00, repeat = 0x01 };

enum class bitdefINTM_TIM : unsigned char
{
   delay_20ms = 0x00,
   delay_40ms = 0x01,
   delay_80ms = 0x02,
   delay_160ms = 0x03
};

enum class bitdefINTM_SEL : unsigned char
{
   no_int = 0x00,
   no_spi = 0x01,
   tinm_tim = 0x02,
   int_auto = 0x03
};

enum class bitdefLDM_SLP_EN : unsigned char { disable = 0x00, enable = 0x01 };

enum class bitdefVPC_SLP_EN : unsigned char { disable = 0x00, enable = 0x01 };

enum class bitdefLDM_STB_EN : unsigned char { disable = 0x00, enable = 0x01 };

enum class bitdefVPC_STB_EN : unsigned char { disable = 0x00, enable = 0x01 };

enum class bitdefAUTO_TIM : unsigned char
{
   delay_10ms = 0x00,
   delay_20ms = 0x01,
   delay_40ms = 0x02,
   delay_80ms = 0x03
};

enum class bitdefMSET_STB : unsigned char { normal = 0x00, standby = 0x01 };

enum class bitdefMSET_LP : unsigned char { normal = 0x00, low_power = 0x01 };

enum class bitdefMSET_SLP : unsigned char { normal = 0x00, sleep = 0x01 };

enum class bitdefMSET_SHDN : unsigned char { normal = 0x00, shutdown = 0x01 };

enum class bitdefNPD_RST : unsigned char { normal = 0x00, reset = 0x01 };

class RegPWR_CTRL
{
public:
   void hal(Ka495xx_interface* ifc) { this->ifc = ifc; }
   RegPWR_CTRL& write()
   {
      if(ifc == nullptr) return;
      ifc->write(Ka495xx_addr::PWR_CTRL, &reg.bytes[0]);
      return *this;
   }
   RegPWR_CTRL& update()
   {
      if(ifc == nullptr) return;
      ifc->read(Ka495xx_addr::PWR_CTRL, &reg.bytes[0]);
      return *this;
   }

   // bit ADC_CONT
   inline RegPWR_CTRL& setADC_CONT(bitdefADC_CONT value)
   {
      reg.b.ADC_CONT = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefADC_CONT getADC_CONT() const
   {
      return static_cast<bitdefADC_CONT>(reg.b.ADC_CONT);
   }

   // bit INTM_TIM
   inline RegPWR_CTRL& setINTM_TIM(bitdefINTM_TIM value)
   {
      reg.b.INTM_TIM = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefINTM_TIM getINTM_TIM() const
   {
      return static_cast<bitdefINTM_TIM>(reg.b.INTM_TIM);
   }

   // bit INTM_SEL
   inline RegPWR_CTRL& setINTM_SEL(bitdefINTM_SEL value)
   {
      reg.b.INTM_SEL = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefINTM_SEL getINTM_SEL() const
   {
      return static_cast<bitdefINTM_SEL>(reg.b.INTM_SEL);
   }

   // bit LDM_SLP_EN
   inline RegPWR_CTRL& setLDM_SLP_EN(bitdefLDM_SLP_EN value)
   {
      reg.b.LDM_SLP_EN = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefLDM_SLP_EN getLDM_SLP_EN() const
   {
      return static_cast<bitdefLDM_SLP_EN>(reg.b.LDM_SLP_EN);
   }

   // bit VPC_SLP_EN
   inline RegPWR_CTRL& setVPC_SLP_EN(bitdefVPC_SLP_EN value)
   {
      reg.b.LDM_SLP_EN = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefVPC_SLP_EN getVPC_SLP_EN() const
   {
      return static_cast<bitdefVPC_SLP_EN>(reg.b.VPC_SLP_EN);
   }

   // bit LDM_STB_EN
   inline RegPWR_CTRL& setLDM_STB_EN(bitdefLDM_STB_EN value)
   {
      reg.b.LDM_STB_EN = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefLDM_STB_EN getLDM_STB_EN() const
   {
      return static_cast<bitdefLDM_STB_EN>(reg.b.LDM_STB_EN);
   }

   // bit VPC_STB_EN
   inline RegPWR_CTRL& setVPC_STB_EN(bitdefVPC_STB_EN value)
   {
      reg.b.VPC_STB_EN = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefVPC_STB_EN getVPC_STB_EN() const
   {
      return static_cast<bitdefVPC_STB_EN>(reg.b.VPC_STB_EN);
   }
   // bit AUTO_TIM
   inline RegPWR_CTRL& setAUTO_TIM(bitdefAUTO_TIM value)
   {
      reg.b.AUTO_TIM = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefAUTO_TIM getAUTO_TIM() const
   {
      return static_cast<bitdefAUTO_TIM>(reg.b.AUTO_TIM);
   }

   //bit MSET_STB
   inline RegPWR_CTRL& setMSET_STB(bitdefMSET_STB value)
   {
      reg.b.MSET_STB = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefMSET_STB getMSET_STB() const
   {
      return static_cast<bitdefMSET_STB>(reg.b.MSET_STB);
   }

   // bit MSET_LP
   inline RegPWR_CTRL& setMSET_LP(bitdefMSET_LP value)
   {
      reg.b.MSET_STB = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefMSET_LP getMSET_LP() const
   {
      return static_cast<bitdefMSET_LP>(reg.b.MSET_LP);
   }

   // bit MSET_SLP
   inline RegPWR_CTRL& setMSET_SLP(bitdefMSET_SLP value)
   {
      reg.b.MSET_SLP = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefMSET_SLP getMSET_SLP() const
   {
      return static_cast<bitdefMSET_SLP>(reg.b.MSET_SLP);
   }

   // bit MSET_SHDN
   inline RegPWR_CTRL& setMSET_SHDN(bitdefMSET_SHDN value)
   {
      reg.b.MSET_SHDN = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefMSET_SHDN getMSET_SHDN() const
   {
      return static_cast<bitdefMSET_SHDN>(reg.b.MSET_SHDN);
   }

   // bit NPD_RST
   inline RegPWR_CTRL& setNPD_RST(bitdefNPD_RST value)
   {
      reg.b.MSET_SHDN = static_cast<unsigned short>(value);
      return *this;
   }
private:
   Ka495xx_interface* ifc;
   union
   {
      unsigned short hfword;
      unsigned char bytes[2];
      struct
      {
         unsigned short NPD_RST : 1;
         unsigned short MSET_SHDN : 1;
         unsigned short MSET_SLP : 1;
         unsigned short MSET_LP : 1;
         unsigned short MSET_STB : 1;
         unsigned short AUTO_TIM : 2;
         unsigned short VPC_STB_EN : 1;
         unsigned short LDM_STB_EN : 1;
         unsigned short VPC_SLP_EN : 1;
         unsigned short LDM_SLP_EN : 1;
         unsigned short INTM_SEL : 2;
         unsigned short INTM_TIM : 2;
         unsigned short ADC_CONT : 1;
      } b;
   } reg;
};

enum class bitdefINT5_EN : unsigned char { wdt_int_disable = 0x00, wdt_int_enable = 0x01 };

enum class bitdefINT4_EN : unsigned char
{
   cur_detect_int_disable = 0x00,
   cur_detect_int_enable = 0x01
};

enum class bitdefINT3_EN : unsigned char
{
   cur_release_int_disable = 0x00,
   cur_release_int_enable = 0x01
};

enum class bitdefINT2_EN : unsigned char
{
   ldm_int_disable = 0x00,
   ldm_int_enable = 0x01
};

enum class bitdefINT1_EN : unsigned char
{
   vpc_int_disable = 0x00,
   vpc_int_enable = 0x01
};

enum class bitdefCHG_DIS_CLR : unsigned char { no_change = 0x00, clear = 0x01 };

enum class bitdefREGEXT_EN : unsigned char { off = 0x00, on = 0x01 };

enum class bitdefSTB_REGEXT_LPEN : unsigned char { hp_mode = 0x00, lp_mode = 0x01 };

enum class bitdefSLP_REGEXT_LPEN : unsigned char { hp_mode = 0x00, lp_mode = 0x01 };

enum class bitdefINTM_REGEXT_LPEN : unsigned char { hp_mode = 0x00, lp_mode = 0x01 };

enum class bitdefSTB_VDD55_LPEN : unsigned char { hp_mode = 0x00, lp_mode = 0x01 };

enum class bitdefSLP_VDD55_LPEN : unsigned char { hp_mode = 0x00, lp_mode = 0x01 };

enum class bitdefINTM_VDD55_LPEN : unsigned char { hp_mode = 0x00, lp_mode = 0x01 };

class RegINT_EN
{
public:
   void hal(Ka495xx_interface* ifc) { this->ifc = ifc; }
   RegINT_EN& write()
   {
      if(ifc == nullptr) return;
      ifc->write(Ka495xx_addr::INTEN, &reg.bytes[0]);
      return *this;
   }
   RegINT_EN& update()
   {
      if(ifc == nullptr) return;
      ifc->read(Ka495xx_addr::INTEN, &reg.bytes[0]);
      return *this;
   }

   inline RegINT_EN& setINT5_EN(bitdefINT5_EN value)
   {
      reg.b.INT5_EN = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefINT4_EN getINT4_EN() const
   {
      return static_cast<bitdefINT4_EN>(reg.b.INT4_EN);
   }

   inline RegINT_EN& setINT4_EN(bitdefINT4_EN value)
   {
      reg.b.INT4_EN = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefINT4_EN getINT4_EN() const
   {
      return static_cast<bitdefINT4_EN>(reg.b.INT4_EN);
   }

   inline RegINT_EN& setINT3_EN(bitdefINT3_EN value)
   {
      reg.b.INT3_EN = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefINT3_EN getINT3_EN() const
   {
      return static_cast<bitdefINT3_EN>(reg.b.INT3_EN);
   }

   inline RegINT_EN& setINT2_EN(bitdefINT2_EN value)
   {
      reg.b.INT2_EN = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefINT2_EN getINT2_EN() const
   {
      return static_cast<bitdefINT2_EN>(reg.b.INT2_EN);
   }

   inline RegINT_EN& setINT1_EN(bitdefINT1_EN value)
   {
      reg.b.INT1_EN = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefINT1_EN getINT1_EN() const
   {
      return static_cast<bitdefINT1_EN>(reg.b.INT1_EN);
   }

   inline RegINT_EN& setCHG_DIS_CLR(bitdefCHG_DIS_CLR value)
   {
      reg.b.CHG_DIS_CLR = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefCHG_DIS_CLR getCHG_DIS_CLR() const
   {
      return static_cast<bitdefCHG_DIS_CLR>(reg.b.CHG_DIS_CLR);
   }

   inline RegINT_EN& setREG_EXT_EN(bitdefREGEXT_EN value)
   {
      reg.b.REG_EXT_EN = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefREGEXT_EN getREG_EXT_EN() const
   {
      return static_cast<bitdefREGEXT_EN>(reg.b.REG_EXT_EN);
   }

   inline RegINT_EN& setSTB_REG_EXT_LPEN(bitdefSTB_REGEXT_LPEN value)
   {
      reg.b.STB_REG_EXT_LPEN = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefSTB_REGEXT_LPEN getSTB_REG_EXT_LPEN() const
   {
      return static_cast<bitdefSTB_REGEXT_LPEN>(reg.b.STB_REG_EXT_LPEN);
   }

   inline RegINT_EN& setSLP_REG_EXT_LPEN(bitdefSLP_REGEXT_LPEN value)
   {
      reg.b.SLP_REG_EXT_LPEN = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefSLP_REGEXT_LPEN getSLP_REG_EXT_LPEN() const
   {
      return static_cast<bitdefSLP_REGEXT_LPEN>(reg.b.SLP_REG_EXT_LPEN);
   }

   inline RegINT_EN& setINTM_REG_EXT_LPEN(bitdefINTM_REGEXT_LPEN value)
   {
      reg.b.INTM_REG_EXT_LPEN = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefINTM_REGEXT_LPEN getINTM_REG_EXT_LPEN() const
   {
      return static_cast<bitdefINTM_REGEXT_LPEN>(reg.b.INTM_REG_EXT_LPEN);
   }

   inline RegINT_EN& setSTB_VDD55_LPEN(bitdefSTB_VDD55_LPEN value)
   {
      reg.b.STB_VDD55_LPEN = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefSTB_VDD55_LPEN getSTB_VDD55_LPEN() const
   {
      return static_cast<bitdefSTB_VDD55_LPEN>(reg.b.STB_VDD55_LPEN);
   }

   inline RegINT_EN& setSLP_VDD55_LPEN(bitdefSLP_VDD55_LPEN value)
   {
      reg.b.SLP_VDD55_LPEN = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefSLP_VDD55_LPEN getSLP_VDD55_LPEN() const
   {
      return static_cast<bitdefSLP_VDD55_LPEN>(reg.b.SLP_VDD55_LPEN);
   }

   inline RegINT_EN& setINTM_VDD55_LPEN(bitdefINTM_VDD55_LPEN value)
   {
      reg.b.INTM_VDD55_LPEN = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefINTM_VDD55_LPEN getINTM_VDD55_LPEN() const
   {
      return static_cast<bitdefINTM_VDD55_LPEN>(reg.b.INTM_VDD55_LPEN);
   }
private:
   Ka495xx_interface* ifc;
   union
   {
      unsigned short hfword;
      unsigned char bytes[2];
      struct
      {
         unsigned short : 1;
         unsigned short INTM_VDD55_LPEN : 1;
         unsigned short SLP_VDD55_LPEN : 1;
         unsigned short STB_VDD55_LPEN : 1;
         unsigned short INTM_REG_EXT_LPEN : 1;
         unsigned short SLP_REG_EXT_LPEN : 1;
         unsigned short STB_REG_EXT_LPEN : 1;
         unsigned short : 1;
         unsigned short REG_EXT_EN : 1;
         unsigned short : 1;
         unsigned short CHG_DIS_CLR : 1;
         unsigned short INT1_EN : 1;
         unsigned short INT2_EN : 1;
         unsigned short INT3_EN : 1;
         unsigned short INT4_EN : 1;
         unsigned short INT5_EN : 1;
      } b;
   } reg;
};

enum class bitdefWDT_STB_EN : unsigned char { disable = 0x00, enable = 0x01 };

enum class bitdefWDT_REGEXT_OFF : unsigned char { off_immediatly = 0x00, retry = 0x01 };

enum class bitdefCOMTIMON : unsigned char { off = 0x00, on = 0x01 };

class RegSPIWD_CTL
{
public:
   void hal(Ka495xx_interface* ifc) { this->ifc = ifc; }
   RegSPIWD_CTL& write()
   {
      if(ifc == nullptr) return;
      ifc->write(Ka495xx_addr::SPIWD, &reg.bytes[0]);
      return *this;
   }
   RegSPIWD_CTL& update()
   {
      if(ifc == nullptr) return;
      ifc->read(Ka495xx_addr::SPIWD, &reg.bytes[0]);
      return *this;
   }

   inline RegSPIWD_CTL& setWDT_STB_EN(bitdefWDT_STB_EN value)
   {
      reg.b.WDT_STB_EN = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefWDT_STB_EN getWDT_STB_EN() const
   {
      return static_cast<bitdefWDT_STB_EN>(reg.b.WDT_STB_EN);
   }

   inline RegSPIWD_CTL& setWDT_REGEXT_OFF(bitdefWDT_REGEXT_OFF value)
   {
      reg.b.WDT_REG_EXT_OFF = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefWDT_REGEXT_OFF getWDT_REGEXT_OFF() const
   {
      return static_cast<bitdefWDT_REGEXT_OFF>(reg.b.WDT_REG_EXT_OFF);
   }

   inline RegSPIWD_CTL& setCOMTIMON(bitdefCOMTIMON value)
   {
      reg.b.COMTIMON = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefCOMTIMON getCOMTIMON() const
   {
      return static_cast<bitdefCOMTIMON>(reg.b.COMTIMON);
   }
private:
   Ka495xx_interface* ifc;
   union
   {
      unsigned short hfword;
      unsigned char bytes[2];
      struct
      {
         unsigned short WDTCOUNT : 12;
         unsigned short COMTIMON : 1;
         unsigned short WDT_REG_EXT_OFF : 1;
         unsigned short WDT_STB_EN : 1;
         unsigned short : 1;
      } b;
   } reg;
};

enum class bitdefFDRV_ALM_SD : unsigned char { no_response = 0x00, auto_off = 0x01 };

enum class bitdefFDRV_ALM_RCV : unsigned char { auto_recover = 0x00, manual_recover = 0x01 };

enum class bitdefFDRV_ALM_CLR : unsigned char { no_change = 0x00, clear = 0x01 };

enum class bitdefNPD_FDRV : unsigned char { power_down = 0x00, normal = 0x01 };

enum class bitdefFDRV_SEL_CLK : unsigned char { div32 = 0x00, div128 = 0x01 };

enum class bitdefFDRV_CHG_FET : unsigned char { charge_off = 0x00, charge_on = 0x01 };

enum class bitdefFDRV_DIS_FET : unsigned char { discharge_off = 0x00, discharge_on = 0x01 };

enum class bitdefFDRV_STBY : unsigned char { normal = 0x00, standby = 0x01 };

enum class bitdefFDRV_LEVEL : unsigned char
{
   zero = 0x00,
   minus_1 = 0x01,
   minus_2 = 0x02,
   minus_3 = 0x03,
   minus_4 = 0x04,
   minus_5 = 0x05,
   minus_6 = 0x06,
   minus_7 = 0x07
};

enum class bitdefFDRV_OUVCTL : unsigned char
{
   cross_off_on_event = 0x00,
   both_off_on_event = 0x01
};

class RegFDRV_CTRL
{
public:
   void hal(Ka495xx_interface* ifc) { this->ifc = ifc; }
   RegFDRV_CTRL& write()
   {
      if(ifc == nullptr) return;
      ifc->write(Ka495xx_addr::FDRV, &reg.bytes[0]);
      return *this;
   }
   RegFDRV_CTRL& update()
   {
      if(ifc == nullptr) return;
      ifc->read(Ka495xx_addr::FDRV, &reg.bytes[0]);
      return *this;
   }

   inline RegFDRV_CTRL& setFDRV_ALM_SD(bitdefFDRV_ALM_SD value)
   {
      reg.b.FDRV_ALM_SD = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefFDRV_ALM_SD getFDRV_ALM_SD() const
   {
      return static_cast<bitdefFDRV_ALM_SD>(reg.b.FDRV_ALM_SD);
   }

   inline RegFDRV_CTRL& setFDRV_ALM_RCV(bitdefFDRV_ALM_RCV value)
   {
      reg.b.FDRV_ALM_RCV = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefFDRV_ALM_RCV getFDRV_ALM_RCV() const
   {
      return static_cast<bitdefFDRV_ALM_RCV>(reg.b.FDRV_ALM_RCV);
   }

   inline RegFDRV_CTRL& setFDRV_ALM_CLR(bitdefFDRV_ALM_CLR value)
   {
      reg.b.FDRV_ALM_CLR = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefFDRV_ALM_CLR getFDRV_ALM_CLR() const
   {
      return static_cast<bitdefFDRV_ALM_CLR>(reg.b.FDRV_ALM_CLR);
   }

   inline RegFDRV_CTRL& setNPD_FDRV(bitdefNPD_FDRV value)
   {
      reg.b.NPD_FDRV = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefNPD_FDRV getNPD_FDRV() const
   {
      return static_cast<bitdefNPD_FDRV>(reg.b.NPD_FDRV);
   }

   inline RegFDRV_CTRL& setFDRV_SEL_CLK(bitdefFDRV_SEL_CLK value)
   {
      reg.b.FDRV_SEL_CLK = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefFDRV_SEL_CLK getFDRV_SEL_CLK() const
   {
      return static_cast<bitdefFDRV_SEL_CLK>(reg.b.FDRV_SEL_CLK);
   }

   inline RegFDRV_CTRL& setFDRV_CHG_FET(bitdefFDRV_CHG_FET value)
   {
      reg.b.FDRV_CHG_FET = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefFDRV_CHG_FET getFDRV_CHG_FET() const
   {
      return static_cast<bitdefFDRV_CHG_FET>(reg.b.FDRV_CHG_FET);
   }

   inline RegFDRV_CTRL& setFDRV_DIS_FET(bitdefFDRV_DIS_FET value)
   {
      reg.b.FDRV_DIS_FET = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefFDRV_DIS_FET getFDRV_DIS_FET() const
   {
      return static_cast<bitdefFDRV_DIS_FET>(reg.b.FDRV_DIS_FET);
   }

   inline RegFDRV_CTRL& setFDRV_STBY(bitdefFDRV_STBY value)
   {
      reg.b.FDRV_STBY = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefFDRV_STBY getFDRV_STBY() const
   {
      return static_cast<bitdefFDRV_STBY>(reg.b.FDRV_STBY);
   }

   inline RegFDRV_CTRL& setFDRV_LEVEL(bitdefFDRV_LEVEL value)
   {
      reg.b.FDRV_LEVEL = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefFDRV_LEVEL getFDRV_LEVEL() const
   {
      return static_cast<bitdefFDRV_LEVEL>(reg.b.FDRV_LEVEL);
   }

   inline RegFDRV_CTRL& setFDRV_OUV_CTL(bitdefFDRV_OUVCTL value)
   {
      reg.b.FDRV_OUV_CTL = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefFDRV_OUVCTL getFDRV_OUV_CTL() const
   {
      return static_cast<bitdefFDRV_OUVCTL>(reg.b.FDRV_OUV_CTL);
   }
private:
   Ka495xx_interface* ifc;
   union
   {
      unsigned short hfword;
      unsigned char bytes[2];
      struct
      {
         unsigned short  : 1;
         unsigned short FDRV_OUV_CTL : 1;
         unsigned short FDRV_LEVEL : 3;
         unsigned short  : 3;
         unsigned short FDRV_STBY : 1;
         unsigned short FDRV_DIS_FET : 1;
         unsigned short FDRV_CHG_FET : 1;
         unsigned short FDRV_SEL_CLK : 1;
         unsigned short NPD_FDRV : 1;
         unsigned short FDRV_ALM_CLR : 1;
         unsigned short FDRV_ALM_RCV : 1;
         unsigned short FDRV_ALM_SD : 1;
      } b;
   } reg;
};

enum class CELLS : unsigned long
{
   CELL1  = 0x00000001,
   CELL2  = 0x00000002,
   CELL3  = 0x00000004,
   CELL4  = 0x00000008,
   CELL5  = 0x00000010,
   CELL6  = 0x00000020,
   CELL7  = 0x00000040,
   CELL8  = 0x00000080,
   CELL9  = 0x00000100,
   CELL10 = 0x00000200,
   CELL11 = 0x00000400,
   CELL12 = 0x00000800,
   CELL13 = 0x00001000,
   CELL14 = 0x00002000,
   CELL15 = 0x00004000,
   CELL16 = 0x00008000,
   CELL17 = 0x00010000,
   CELL18 = 0x00020000,
   CELL19 = 0x00040000,
   CELL20 = 0x00080000,
   CELL21 = 0x00100000,
   CELL22 = 0x00200000
};

CELLS operator|(CELLS a, CELLS b)
{
   CELLS ret;
   ret = static_cast<CELLS>(static_cast<unsigned long>(a) | static_cast<unsigned long>(b));
   return ret;
}

CELLS operator~(CELLS b)
{
   CELLS ret;
   unsigned long inverted = static_cast<unsigned long>(b);
   ret = static_cast<CELLS>(~inverted);
   return ret;
}

CELLS operator&(CELLS a, CELLS b)
{
   CELLS ret;
   unsigned long hexA = static_cast<unsigned long>(a);
   unsigned long hexB = static_cast<unsigned long>(b);
   ret = static_cast<CELLS>(hexA & hexB);
   return ret;
}

class RegCVSEL
{
public:
   void hal(Ka495xx_interface* ifc) { this->ifc = ifc; }
   RegCVSEL& write()
   {
      if(ifc == nullptr) return;
      ifc->write(Ka495xx_addr::CVSEL1, &CVSEL1.bytes[0]);
      ifc->write(Ka495xx_addr::CVSEL2, &CVSEL2.bytes[0]);
      return *this;
   }
   RegCVSEL& update()
   {
      if(ifc == nullptr) return;
      ifc->read(Ka495xx_addr::CVSEL1, &CVSEL1.bytes[0]);
      ifc->read(Ka495xx_addr::CVSEL2, &CVSEL2.bytes[0]);
      return *this;
   }

   inline RegCVSEL& select(CELLS channel)
   {
      unsigned long lower = static_cast<unsigned long>(channel) & 0x00007FFF;
      unsigned long upper = (static_cast<unsigned long>(channel) & 0x003F8000) >> 15;
      CVSEL1.hfword = lower;
      CVSEL2.hfword = upper;
      return *this;
   }
   inline RegCVSEL& deselect(CELLS channel)
   {
      unsigned long lower = static_cast<unsigned long>(channel) & 0x00007FFF;
      unsigned long upper = (static_cast<unsigned long>(channel) & 0x003F8000) >> 15;
      CVSEL1.hfword &= ~lower;
      CVSEL2.hfword &= ~upper;
      return *this;
   }
private:
   Ka495xx_interface* ifc;

   union
   {
      unsigned short hfword;
      unsigned char bytes[2];
      struct
      {
         unsigned short CELL1 : 1;
         unsigned short CELL2 : 1;
         unsigned short CELL3 : 1;
         unsigned short CELL4 : 1;
         unsigned short CELL5 : 1;
         unsigned short CELL6 : 1;
         unsigned short CELL7 : 1;
         unsigned short CELL8 : 1;
         unsigned short CELL9 : 1;
         unsigned short CELL10 : 1;
         unsigned short CELL11 : 1;
         unsigned short CELL12 : 1;
         unsigned short CELL13 : 1;
         unsigned short CELL14 : 1;
         unsigned short CELL15 : 1;
         unsigned short CELL16 : 1;
      } b;
   } CVSEL1;

   union
   {
      unsigned short hfword;
      unsigned char bytes[2];
      struct
      {
         unsigned short CELL17 : 1;
         unsigned short CELL18 : 1;
         unsigned short CELL19 : 1;
         unsigned short CELL20 : 1;
         unsigned short CELL21 : 1;
         unsigned short CELL22 : 1;
         unsigned short : 10;
      } b;
   } CVSEL2;
};

enum class bitdefGVSEL : unsigned short
{
   VPACK = 0x0001,
   TMON1 = 0x0002,
   TMON2 = 0x0004,
   TMON3 = 0x0008,
   TMON4 = 0x0010,
   TMON5 = 0x0020,
   VDD55 = 0x0040,
   GPAD1 = 0x0080,
   GPAD2 = 0x0100,
   VDD18 = 0x1000,
   REG_EXT = 0x2000,
   VREF2 = 0x4000,
};

bitdefGVSEL operator|(bitdefGVSEL a, bitdefGVSEL b)
{
   bitdefGVSEL ret;
   ret = static_cast<bitdefGVSEL>(static_cast<unsigned long>(a) | static_cast<unsigned long>(b));
   return ret;
}

bitdefGVSEL operator~(bitdefGVSEL b)
{
   bitdefGVSEL ret;
   unsigned long inverted = static_cast<unsigned long>(b);
   ret = static_cast<bitdefGVSEL>(~inverted);
   return ret;
}

bitdefGVSEL operator&(bitdefGVSEL a, bitdefGVSEL b)
{
   bitdefGVSEL ret;
   unsigned long hexA = static_cast<unsigned long>(a);
   unsigned long hexB = static_cast<unsigned long>(b);
   ret = static_cast<bitdefGVSEL>(hexA & hexB);
   return ret;
}

class RegGVSEL
{
public:
   void hal(Ka495xx_interface* ifc) { this->ifc = ifc; }
   RegGVSEL& write()
   {
      if(ifc == nullptr) return;
      ifc->write(Ka495xx_addr::GVSEL, &reg.bytes[0]);
      return *this;
   }
   RegGVSEL& update()
   {
      if(ifc == nullptr) return;
      ifc->read(Ka495xx_addr::GVSEL, &reg.bytes[0]);
      return *this;
   }
   inline RegGVSEL& select(bitdefGVSEL channel)
   {
      reg.hfword = static_cast<unsigned short>(channel);
      return *this;
   }
   inline RegGVSEL& deselect(bitdefGVSEL channel)
   {
      reg.hfword &= ~static_cast<unsigned short>(channel);
      return *this;
   }
private:
   Ka495xx_interface* ifc;
   union
   {
      unsigned short hfword;
      unsigned char bytes[2];
      struct
      {
         unsigned short VPACKSEL : 1;
         unsigned short TMONI1SEL : 1;
         unsigned short TMONI2SEL : 1;
         unsigned short TMONI3SEL : 1;
         unsigned short TMONI4SEL : 1;
         unsigned short TMONI5SEL : 1;
         unsigned short VDD55SEL : 1;
         unsigned short GPAD1SEL : 1;
         unsigned short GPAD2SEL : 1;
         unsigned short : 3;
         unsigned short VDD18SEL : 1;
         unsigned short REG_EXT_SEL : 1;
         unsigned short VREF2SEL : 1;
         unsigned short : 1;
      } b;
   } reg;
};

enum class bitdefDLY_FUSE_2V : unsigned char
{
   delay_1000ms = 0x00,
   delay_2000ms = 0x01,
   delay_3000ms = 0x02,
   delay_4000ms = 0x03,
   delay_5000ms = 0x04,
   delay_6000ms = 0x05,
   delay_10000ms = 0x06,
   delay_20000ms = 0x07,
   delay_30000ms = 0x08,
   delay_50000ms = 0x09,
   delay_75000ms = 0x0A,
   delay_100000ms = 0x0B,
   delay_120000ms = 0x0C,
   delay_140000ms = 0x0D,
   delay_160000ms = 0x0E,
   delay_180000ms = 0x0F
};

enum class bitdefDLY_FUSE_1V : unsigned char
{
   delay_200ms = 0x00,
   delay_400ms = 0x01,
   delay_600ms = 0x02,
   delay_800ms = 0x03,
   delay_1000ms = 0x04,
   delay_1200ms = 0x05,
   delay_1400ms = 0x06,
   delay_1600ms = 0x07
};

enum class bitdefFUSEB_ENV : unsigned char
{
   disable = 0x00,
   enable = 0x01
};

enum class bitdefDLY_FUSE_2C : unsigned char
{
   delay_500ms = 0x00,
   delay_800ms = 0x01,
   delay_1100ms = 0x02,
   delay_1400ms = 0x03,
   delay_1700ms = 0x04,
   delay_2000ms = 0x05,
   delay_2300ms = 0x06,
   delay_2600ms = 0x07,
   delay_2900ms = 0x08,
   delay_3200ms = 0x09,
   delay_3500ms = 0x0A,
   delay_3800ms = 0x0B,
   delay_4100ms = 0x0C,
   delay_4400ms = 0x0D,
   delay_4700ms = 0x0E,
   delay_5000ms = 0x0F
};

enum class bitdefDLY_FUSE_1C : unsigned char
{
   delay_200ms = 0x00,
   delay_400ms = 0x01,
   delay_600ms = 0x02,
   delay_800ms = 0x03,
   delay_1000ms = 0x04,
   delay_1200ms = 0x05,
   delay_1400ms = 0x06,
   delay_1600ms = 0x07
};

enum class bitdefFUSEB_ENC : unsigned char
{
   disable = 0x00,
   enable = 0x01
};

enum class bitdefOVTH2_SEL : unsigned char
{
   threshold_200mV = 0x00,
   threshold_150mV = 0x01,
   threshold_100mV = 0x02,
   threshold_50mV = 0x03
};

enum class bitdefFUSEB_ENV2 : unsigned char
{
   disable = 0x00,
   enable = 0x01
};

enum class bitdefFUSEB_ENC2 : unsigned char
{
   disable = 0x00,
   enable = 0x01
};

enum class bitdefFUSE_BLOW : unsigned char;

enum class bitdefDLY_FUSECUT : unsigned char
{
   hold_30000ms = 0x00,
   hold_45000ms = 0x01,
   hold_60000ms = 0x02,
   hold_75000ms = 0x03,
   hold_90000ms = 0x04,
   hold_105000ms = 0x05,
   hold_120000ms = 0x06,
   hold_135000ms = 0x07,
   hold_150000ms = 0x08,
   hold_165000ms = 0x09,
   hold_180000ms = 0x0A,
   hold_195000ms = 0x0B,
   hold_210000ms = 0x0C,
   hold_225000ms = 0x0D,
   hold_240000ms = 0x0E,
   hold_300000ms = 0x0F
};

class RegFUSE
{
public:
   void hal(Ka495xx_interface* ifc) { this->ifc = ifc; }
   RegFUSE& write()
   {
      if(ifc == nullptr) return;
      ifc->write(Ka495xx_addr::FUSE1, &FUSE1.bytes[0]);
      ifc->write(Ka495xx_addr::FUSE2, &FUSE2.bytes[0]);
      ifc->write(Ka495xx_addr::FUSE_BLOW, &FUSE_BLOW.bytes[0]);
      return *this;
   }
   RegFUSE& update()
   {
      if(ifc == nullptr) return;
      ifc->read(Ka495xx_addr::FUSE1, &FUSE1.bytes[0]);
      ifc->read(Ka495xx_addr::FUSE2, &FUSE2.bytes[0]);
      ifc->read(Ka495xx_addr::FUSE_BLOW, &FUSE_BLOW.bytes[0]);
      return *this;
   }

   inline RegFUSE& setDLY_FUSE_2V(bitdefDLY_FUSE_2V value)
   {
      FUSE1.b.DLY_FUSE_2V = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefDLY_FUSE_2V getDLY_FUSE_2V() const
   {
      return static_cast<bitdefDLY_FUSE_2V>(FUSE1.b.DLY_FUSE_2V);
   }

   inline RegFUSE& setDLY_FUSE_1V(bitdefDLY_FUSE_1V value)
   {
      FUSE1.b.DLY_FUSE_1V = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefDLY_FUSE_1V getDLY_FUSE_1V() const
   {
      return static_cast<bitdefDLY_FUSE_1V>(FUSE1.b.DLY_FUSE_1C);
   }

   inline RegFUSE& setFUSEB_ENV(bitdefFUSEB_ENV value)
   {
      FUSE1.b.FUSEB_ENV = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefFUSEB_ENV getFUSEB_ENV() const
   {
      return static_cast<bitdefFUSEB_ENV>(FUSE1.b.FUSEB_ENV);
   }

   inline RegFUSE& setDLY_FUSE_2C(bitdefDLY_FUSE_2C value)
   {
      FUSE1.b.DLY_FUSE_2C = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefDLY_FUSE_2C getDLY_FUSE_2C() const
   {
      return static_cast<bitdefDLY_FUSE_2C>(FUSE1.b.DLY_FUSE_2C);
   }

   inline RegFUSE& setDLY_FUSE_1C(bitdefDLY_FUSE_1C value)
   {
      FUSE1.b.DLY_FUSE_1C = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefDLY_FUSE_1C getDLY_FUSE_1C() const
   {
      return static_cast<bitdefDLY_FUSE_1C>(FUSE1.b.DLY_FUSE_1C);
   }

   inline RegFUSE& setFUSEB_ENC(bitdefFUSEB_ENC value)
   {
      FUSE1.b.FUSEB_ENC = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefFUSEB_ENC getFUSEB_ENC() const
   {
      return static_cast<bitdefFUSEB_ENC>(FUSE1.b.FUSEB_ENC);
   }

   inline RegFUSE& setOVTH2_SEL(bitdefOVTH2_SEL value)
   {
      FUSE2.b.OVTH2_SEL = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefOVTH2_SEL getOVTH2_SEL() const
   {
      return static_cast<bitdefOVTH2_SEL>(FUSE2.b.OVTH2_SEL);
   }
   
   inline RegFUSE& setFUSEB_ENV2(bitdefFUSEB_ENV2 value)
   {
      FUSE2.b.FUSEB_ENV2 = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefFUSEB_ENV2 getFUSEB_ENV2() const
   {
      return static_cast<bitdefFUSEB_ENV2>(FUSE2.b.FUSEB_ENV2);
   }

   inline RegFUSE& setFUSEB_ENC2(bitdefFUSEB_ENC2 value)
   {
      FUSE2.b.FUSEB_ENC2 = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefFUSEB_ENC2 getFUSEB_ENC2() const
   {
      return static_cast<bitdefFUSEB_ENC2>(FUSE2.b.FUSEB_ENC2);
   }

   inline RegFUSE& setDLY_FUSECUT(bitdefDLY_FUSECUT value)
   {
      FUSE2.b.DLY_FUSECUT = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefDLY_FUSECUT getDLY_FUSECUT() const
   {
      return static_cast<bitdefDLY_FUSECUT>(FUSE2.b.DLY_FUSECUT);
   }

   inline RegFUSE& setFUSE_BLOW(bitdefFUSE_BLOW value)
   {
      FUSE_BLOW.b.FUSE_BLOW = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefFUSE_BLOW getFUSE_BLOW() const
   {
      return static_cast<bitdefFUSE_BLOW>(FUSE_BLOW.b.FUSE_BLOW);
   }
private:
   Ka495xx_interface* ifc;

   union
   {
      unsigned short hfword;
      unsigned char bytes[2];
      struct
      {
         unsigned short FUSEB_ENC : 1;
         unsigned short DLY_FUSE_1C : 3;
         unsigned short DLY_FUSE_2C : 4;
         unsigned short FUSEB_ENV : 1;
         unsigned short DLY_FUSE_1V : 3;
         unsigned short DLY_FUSE_2V : 4;
      } b;
   } FUSE1;

   union
   {
      unsigned short hfword;
      unsigned char bytes[2];
      struct
      {
         unsigned short DLY_FUSECUT : 4;
         unsigned short FUSEB_ENC2 : 1;
         unsigned short FUSEB_ENV2 : 1;
         unsigned short OVTH2_SEL : 2;
         unsigned short : 8;
      } b;
   } FUSE2;

   union
   {
      unsigned short hfword;
      unsigned char bytes[2];
      struct
      {
         unsigned short FUSE_BLOW : 8;
         unsigned short : 8;
      } b;
   } FUSE_BLOW;
};

enum class bitdefOCTH_SEL : unsigned char
{
   threshold_0mV1 = 0x00,
   threshold_0mV3 = 0x01,
   threshold_0mV6 = 0x02,
   threshold_1mV0 = 0x03,
};

enum class bitdefOVTH : unsigned char
{
   threshold_2V00 = 0x02,
   threshold_2V05 = 0x03,
   threshold_2V10 = 0x04,
   threshold_2V15 = 0x05,
   threshold_2V20 = 0x06,
   threshold_2V25 = 0x07,
   threshold_2V30 = 0x08,
   threshold_2V35 = 0x09,
   threshold_2V40 = 0x0A,
   threshold_2V45 = 0x0B,
   threshold_2V50 = 0x0C,
   threshold_2V55 = 0x0D,
   threshold_2V60 = 0x0E,
   threshold_2V65 = 0x0F,
   threshold_2V70 = 0x10,
   threshold_2V75 = 0x11,
   threshold_2V80 = 0x12,
   threshold_2V85 = 0x13,
   threshold_2V90 = 0x14,
   threshold_2V95 = 0x15,
   threshold_3V00 = 0x16,
   threshold_3V05 = 0x17,
   threshold_3V10 = 0x18,
   threshold_3V15 = 0x19,
   threshold_3V20 = 0x1A,
   threshold_3V25 = 0x1B,
   threshold_3V30 = 0x1C,
   threshold_3V35 = 0x1D,
   threshold_3V40 = 0x1E,
   threshold_3V45 = 0x1F,
   threshold_3V50 = 0x20,
   threshold_3V55 = 0x21,
   threshold_3V60 = 0x22,
   threshold_3V65 = 0x23,
   threshold_3V70 = 0x24,
   threshold_3V75 = 0x25,
   threshold_3V80 = 0x26,
   threshold_3V85 = 0x27,
   threshold_3V90 = 0x28,
   threshold_3V95 = 0x29,
   threshold_4V00 = 0x2A,
   threshold_4V05 = 0x2B,
   threshold_4V10 = 0x2C,
   threshold_4V15 = 0x2D,
   threshold_4V20 = 0x2E,
   threshold_4V25 = 0x2F,
   threshold_4V30 = 0x30,
   threshold_4V35 = 0x31,
   threshold_4V40 = 0x32,
   threshold_4V45 = 0x33,
   threshold_4V50 = 0x34
};

enum class bitdefUVTH : unsigned char
{
   threshold_0V50 = 0x00,
   threshold_0V55 = 0x01,
   threshold_0V60 = 0x02,
   threshold_0V65 = 0x03,
   threshold_0V70 = 0x04,
   threshold_0V75 = 0x05,
   threshold_0V80 = 0x06,
   threshold_0V85 = 0x07,
   threshold_0V90 = 0x08,
   threshold_0V95 = 0x09,
   threshold_1V00 = 0x0A,
   threshold_1V05 = 0x0B,
   threshold_1V10 = 0x0C,
   threshold_1V15 = 0x0D,
   threshold_1V20 = 0x0E,
   threshold_1V25 = 0x0F,
   threshold_1V30 = 0x10,
   threshold_1V35 = 0x11,
   threshold_1V40 = 0x12,
   threshold_1V45 = 0x13,
   threshold_1V50 = 0x14,
   threshold_1V55 = 0x15,
   threshold_1V60 = 0x16,
   threshold_1V65 = 0x17,
   threshold_1V70 = 0x18,
   threshold_1V75 = 0x19,
   threshold_1V80 = 0x1A,
   threshold_1V85 = 0x1B,
   threshold_1V90 = 0x1C,
   threshold_1V95 = 0x1D,
   threshold_2V00 = 0x1E,
   threshold_2V05 = 0x1F,
   threshold_2V10 = 0x20,
   threshold_2V15 = 0x21,
   threshold_2V20 = 0x22,
   threshold_2V25 = 0x23,
   threshold_2V30 = 0x24,
   threshold_2V35 = 0x25,
   threshold_2V40 = 0x26,
   threshold_2V45 = 0x27,
   threshold_2V50 = 0x28,
   threshold_2V55 = 0x29,
   threshold_2V60 = 0x2A,
   threshold_2V65 = 0x2B,
   threshold_2V70 = 0x2C,
   threshold_2V75 = 0x2D,
   threshold_2V80 = 0x2E,
   threshold_2V85 = 0x2F,
   threshold_2V90 = 0x30,
   threshold_2V95 = 0x31,
   threshold_3V00 = 0x32
};

enum class bitdefOV_HYS : unsigned char
{
   level_25mV = 0x00,
   level_50mV = 0x01,
   level_75mV = 0x02,
   level_100mV = 0x03,
   level_125mV = 0x04,
   level_150mV = 0x05,
   level_175mV = 0x06,
   level_200mV = 0x07
};

enum class bitdefUV_HYS : unsigned char
{
   level_25mV = 0x00,
   level_50mV = 0x01,
   level_75mV = 0x02,
   level_100mV = 0x03,
   level_125mV = 0x04,
   level_150mV = 0x05,
   level_175mV = 0x06,
   level_200mV = 0x07
};

enum class bitdefOV_DLY : unsigned char
{
   delay_200ms = 0x00,
   delay_400ms = 0x01,
   delay_600ms = 0x02,
   delay_800ms = 0x03,
   delay_1500ms = 0x04,
   delay_3000ms = 0x05,
   delay_4500ms = 0x06,
   delay_6000ms = 0x07
};

enum class bitdefUV_DLY : unsigned char
{
   delay_200ms = 0x00,
   delay_400ms = 0x01,
   delay_600ms = 0x02,
   delay_800ms = 0x03,
   delay_1500ms = 0x04,
   delay_3000ms = 0x05,
   delay_4500ms = 0x06,
   delay_6000ms = 0x07
};

class OUVCTL
{
public:
   void hal(Ka495xx_interface* ifc) { this->ifc = ifc; }
   OUVCTL& write()
   {
      if(ifc == nullptr) return;
      ifc->write(Ka495xx_addr::OUVCTL1, &OUVCTL1.bytes[0]);
      ifc->write(Ka495xx_addr::OUVCTL2, &OUVCTL2.bytes[0]);
      return *this;
   }
   OUVCTL& update()
   {
      if(ifc == nullptr) return;
      ifc->read(Ka495xx_addr::OUVCTL1, &OUVCTL1.bytes[0]);
      ifc->read(Ka495xx_addr::OUVCTL2, &OUVCTL2.bytes[0]);
      return *this;
   }

   inline OUVCTL& setOCTH_SEL(bitdefOCTH_SEL value)
   {
      OUVCTL1.b.OCTH_SEL = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefOCTH_SEL getOCTH_SEL() const
   {
      return static_cast<bitdefOCTH_SEL>(OUVCTL1.b.OCTH_SEL);
   }

   inline OUVCTL& setOVTH(bitdefOVTH value)
   {
      OUVCTL1.b.OVTH = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefOVTH getOVTH() const
   {
      return static_cast<bitdefOVTH>(OUVCTL1.b.OVTH);
   }

   inline OUVCTL& setUVTH(bitdefUVTH value)
   {
      OUVCTL1.b.UVTH = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefUVTH getUVTH() const
   {
      return static_cast<bitdefUVTH>(OUVCTL1.b.UVTH);
   }

   inline OUVCTL& setOV_HYS(bitdefOV_HYS value)
   {
      OUVCTL2.b.OV_HYS = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefOV_HYS getOV_HYS() const
   {
      return static_cast<bitdefOV_HYS>(OUVCTL2.b.OV_HYS);
   }

   inline OUVCTL& setUV_HYS(bitdefUV_HYS value)
   {
      OUVCTL2.b.UV_HYS = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefUV_HYS getUV_HYS() const
   {
      return static_cast<bitdefUV_HYS>(OUVCTL2.b.UV_HYS);
   }

   inline OUVCTL& setOV_DLY(bitdefOV_DLY value)
   {
      OUVCTL2.b.OV_DLY = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefOV_DLY getOV_DLY() const
   {
      return static_cast<bitdefOV_DLY>(OUVCTL2.b.OV_DLY);
   }

   inline OUVCTL& setUV_DLY(bitdefUV_DLY value)
   {
      OUVCTL2.b.UV_DLY = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefUV_DLY getUV_DLY() const
   {
      return static_cast<bitdefUV_DLY>(OUVCTL2.b.UV_DLY);
   }
private:
   Ka495xx_interface* ifc;
   union
   {
      unsigned short hfword;
      unsigned char bytes[2];
      struct
      {
         unsigned short UVTH : 6;
         unsigned short OVTH : 6;
         unsigned short OCTH_SEL : 2;
         unsigned short : 2;
      } b;
   } OUVCTL1;

   union
   {
      unsigned short hfword;
      unsigned char bytes[2];
      struct
      {
         unsigned short UV_DLY : 3;
         unsigned short : 1;
         unsigned short OV_DLY : 3;
         unsigned short : 1;
         unsigned short UV_HYS : 3;
         unsigned short : 1;
         unsigned short OV_HYS : 3;
         unsigned short : 1;
      } b;
   } OUVCTL2;
};

enum class bitdefCB_SET : unsigned char
{
   off = 0x00,
   on = 0x01
};

enum class bitdefUVMSK : unsigned char
{
   on = 0x00,
   off = 0x01
};

enum class bitdefOVMSK : unsigned char
{
   on = 0x00,
   off = 0x01
};

enum class bitdefADC_TRG : unsigned char
{
   stop = 0x00,
   conversion = 0x01
};

enum class bitdefADIL_LATCH : unsigned char
{
   not_effect = 0x00,
   conversion = 0x01
};

enum class bitdefADIH_LATCH : unsigned char
{
   not_effect = 0x00,
   conversion = 0x01
};

enum class bitdefADV_LATCH : unsigned char
{
   not_effect = 0x00,
   conversion = 0x01
};

class RegOPMODE
{
public:
   void hal(Ka495xx_interface* ifc) { this->ifc = ifc; }
   RegOPMODE& write()
   {
      if(ifc == nullptr) return;
      ifc->write(Ka495xx_addr::OPMODE, &OPMODE.bytes[0]);
      return *this;
   }
   RegOPMODE& update()
   {
      if(ifc == nullptr) return;
      ifc->read(Ka495xx_addr::OPMODE, &OPMODE.bytes[0]);
      return *this;
   }

   inline RegOPMODE& setCB_SET(bitdefCB_SET value)
   {
      OPMODE.b.CB_SET = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefCB_SET getCB_SET() const
   {
      return static_cast<bitdefCB_SET>(OPMODE.b.CB_SET);
   }

   inline RegOPMODE& setUVMSK(bitdefUVMSK value)
   {
      OPMODE.b.UV_MSK = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefUVMSK getUVMSK() const
   {
      return static_cast<bitdefUVMSK>(OPMODE.b.UV_MSK);
   }

   inline RegOPMODE& setOVMSK(bitdefOVMSK value)
   {
      OPMODE.b.OV_MSK = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefOVMSK getOVMSK() const
   {
      return static_cast<bitdefOVMSK>(OPMODE.b.OV_MSK);
   }

   inline RegOPMODE& setADC_TRG(bitdefADC_TRG value)
   {
      OPMODE.b.ADC_TRG = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefADC_TRG getADC_TRG() const
   {
      return static_cast<bitdefADC_TRG>(OPMODE.b.ADC_TRG);
   }

   inline RegOPMODE& setADIL_LATCH(bitdefADIL_LATCH value)
   {
      OPMODE.b.ADIL_LATCH = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefADIL_LATCH getADIL_LATCH() const
   {
      return static_cast<bitdefADIL_LATCH>(OPMODE.b.ADIL_LATCH);
   }

   inline RegOPMODE& setADIH_LATCH(bitdefADIH_LATCH value)
   {
      OPMODE.b.ADIH_LATCH = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefADIH_LATCH getADIH_LATCH() const
   {
      return static_cast<bitdefADIH_LATCH>(OPMODE.b.ADIH_LATCH);
   }

   inline RegOPMODE& setADV_LATCH(bitdefADV_LATCH value)
   {
      OPMODE.b.ADV_LATCH = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefADV_LATCH getADV_LATCH() const
   {
      return static_cast<bitdefADV_LATCH>(OPMODE.b.ADV_LATCH);
   }
private:
   Ka495xx_interface* ifc;
   union
   {
      unsigned short hfword;
      unsigned char bytes[2];
      struct
      {
         unsigned short ADV_LATCH : 1;
         unsigned short ADIH_LATCH : 1;
         unsigned short ADIL_LATCH : 1;
         unsigned short : 1;
         unsigned short ADC_TRG : 1;
         unsigned short  : 1;
         unsigned short OV_MSK : 1;
         unsigned short UV_MSK : 1;
         unsigned short CB_SET : 1;
         unsigned short : 7;
      } b;
   } OPMODE;
};

enum class bitdefGPIOSEL : unsigned short
{
   GPIO = 0x00,
   GPIOH1 = 0x01,
   GPIOH2 = 0x02,
   ADIRQ1 = 0x03,
   ADIRQ2 = 0x04,
   HSOSC = 0x05,
   LSOSC = 0x06,
   active_state = 0x07,
   standby_state = 0x08,
   lp_state = 0x09,
   fuse_fet = 0x0A,
   alarm2 = 0x0B,
   mcu_int = 0x0C
};

enum class bitdefGPIO_CHDRV : unsigned short
{
   drive_2mA = 0x00,
   drive_4mA = 0x01
};

enum class bitdefGPIO_OUT : unsigned short
{
   low = 0x00,
   high = 0x01
};

enum class bitdefGPIO_MODE : unsigned short
{
   push_pull = 0x00,
   open_drain = 0x01
};

enum class bitdefGPIO_PD : unsigned short
{
   no_pulldown = 0x00,
   pulldown = 0x01
};

enum class bitdefGPIO_NOE : unsigned short
{
   enable = 0x00,
   disable = 0x01
};

enum class bitdefGPIO_IE : unsigned short
{
   disable = 0x00,
   enable = 0x01
};


enum class GPIOId
{
   GPIO1,
   GPIO2,
   GPIO3
};

class RegGPIO123
{
public:
   RegGPIO123(GPIOId id)
   {
      this->id = id;
      switch(id)
      {
         case GPIOId::GPIO1:
         address = Ka495xx_addr::GPIO1;
         break;
         case GPIOId::GPIO2:
         address = Ka495xx_addr::GPIO2;
         break;
         case GPIOId::GPIO3:
         address = Ka495xx_addr::GPIO3;
         break;
      }
   }
   void hal(Ka495xx_interface* ifc) { this->ifc = ifc; }
   RegGPIO123& write()
   {
      if(ifc == nullptr) return;
      ifc->write(address, &GPIO.bytes[0]);
      return *this;
   }
   RegGPIO123& update()
   {
      if(ifc == nullptr) return;
      ifc->read(address, &GPIO.bytes[0]);
      return *this;
   }
   inline RegGPIO123& setGPIOSEL(bitdefGPIOSEL value)
   {
      GPIO.b.GPIOSEL = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefGPIOSEL getGPIOSEL() const
   {
      return static_cast<bitdefGPIOSEL>(GPIO.b.GPIOSEL);
   }

   inline RegGPIO123& setCHDRV(bitdefGPIO_CHDRV value)
   {
      GPIO.b.GPIO_CHDRV = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefGPIO_CHDRV getCHDRV() const
   {
      return static_cast<bitdefGPIO_CHDRV>(GPIO.b.GPIO_CHDRV);
   }

   inline RegGPIO123& setOutput(bitdefGPIO_OUT value)
   {
      GPIO.b.GPIO_OUT = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefGPIO_OUT getOutState() const
   {
      return static_cast<bitdefGPIO_OUT>(GPIO.b.GPIO_OUT);
   }

   inline RegGPIO123& setOutMode(bitdefGPIO_MODE value)
   {
      GPIO.b.GPIO_OD = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefGPIO_MODE getOutMode() const
   {
      return static_cast<bitdefGPIO_MODE>(GPIO.b.GPIO_OD);
   }

   inline RegGPIO123& setPulldown(bitdefGPIO_PD value)
   {
      GPIO.b.GPIO_PD = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefGPIO_PD isPulldown() const
   {
      return static_cast<bitdefGPIO_PD>(GPIO.b.GPIO_PD);
   }

   inline RegGPIO123& enableOutput(bitdefGPIO_NOE value)
   {
      GPIO.b.GPIO_NOE = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefGPIO_NOE isOutputEnable() const
   {
      return static_cast<bitdefGPIO_NOE>(GPIO.b.GPIO_NOE);
   }

   inline RegGPIO123& enableInput(bitdefGPIO_IE value)
   {
      GPIO.b.GPIO_IE = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefGPIO_IE isInputEnable() const
   {
      return static_cast<bitdefGPIO_IE>(GPIO.b.GPIO_IE);
   }
private:
   GPIOId id;
   Ka495xx_addr address;
   Ka495xx_interface* ifc;
   union
   {
      unsigned short hfword;
      unsigned char bytes[2];
      struct
      {
         unsigned short GPIO_IE : 1;
         unsigned short GPIO_NOE : 1;
         unsigned short GPIO_PD : 1;
         unsigned short GPIO_OD : 1;
         unsigned short GPIO_OUT : 1;
         unsigned short GPIO_CHDRV : 1;
         unsigned short : 2;
         unsigned short GPIOSEL : 4;
         unsigned short : 4;
      } b;
   } GPIO;
};

enum class bitdefGPOH2_ALM_ST : unsigned char
{
   hi_z = 0x00,
   low = 0x01
};

enum class bitdefGPOH1_ALM_ST : unsigned char
{
   hi_z = 0x00,
   low = 0x01
};

enum class bitdefGPOH_FET : unsigned char
{
   disable = 0x00,
   enable = 0x01
};

enum class bitdefGPOH2_CTRL : unsigned char
{
   high = 0x00,
   low = 0x01
};

enum class bitdefGPOH1_CTRL : unsigned char
{
   high = 0x00,
   low = 0x01
};

class RegGPIOHV
{
public:
   void hal(Ka495xx_interface* ifc) { this->ifc = ifc; }
   RegGPIOHV& write()
   {
      if(ifc == nullptr) return;
      ifc->write(Ka495xx_addr::GPIOH, &GPOH.bytes[0]);
      return *this;
   }
   RegGPIOHV& update()
   {
      if(ifc == nullptr) return;
      ifc->read(Ka495xx_addr::GPIOH, &GPOH.bytes[0]);
      return *this;
   }

   inline RegGPIOHV& setGPOH2_ALM_ST(bitdefGPOH2_ALM_ST value)
   {
      GPOH.b.GPIOH2_ALM_ST = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefGPOH2_ALM_ST getGPOH2_ALM_ST() const
   {
      return static_cast<bitdefGPOH2_ALM_ST>(GPOH.b.GPIOH2_ALM_ST);
   }

   inline RegGPIOHV& setGPOH1_ALM_ST(bitdefGPOH1_ALM_ST value)
   {
      GPOH.b.GPIOH1_ALM_ST = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefGPOH1_ALM_ST getGPOH_ALM_ST() const
   {
      return static_cast<bitdefGPOH1_ALM_ST>(GPOH.b.GPIOH1_ALM_ST);
   }

   inline RegGPIOHV& setGPOH_FET(bitdefGPOH_FET value)
   {
      GPOH.b.GPIOH_FET = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefGPOH_FET getGPOH_FET() const
   {
      return static_cast<bitdefGPOH_FET>(GPOH.b.GPIOH_FET);
   }

   inline RegGPIOHV& enableGPOH2(bitdefGPOH2_CTRL value)
   {
      GPOH.b.GPIOH2_EN = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefGPOH2_CTRL isEnabledGPIOH2() const
   {
      return static_cast<bitdefGPOH2_CTRL>(GPOH.b.GPIOH2_EN);
   }

   inline RegGPIOHV& enableGPOH1(bitdefGPOH1_CTRL value)
   {
      GPOH.b.GPIOH1_EN = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefGPOH1_CTRL isEnabledGPIOH1() const
   {
      return static_cast<bitdefGPOH1_CTRL>(GPOH.b.GPIOH1_EN);
   }
private:
   Ka495xx_interface* ifc;
   union
   {
      unsigned short hfword;
      unsigned char bytes[2];
      struct
      {
         unsigned short GPIOH1_EN : 1;
         unsigned short GPIOH2_EN : 1;
         unsigned short GPIOH_FET : 1;
         unsigned short : 1;
         unsigned short GPIOH1_ALM_ST : 1;
         unsigned short GPIOH2_ALM_ST : 1;
         unsigned short : 10;
      } b;
   } GPOH;
};

enum class bitdefOVP_F_SET : unsigned short
{
   shutdown = 0x00,
   no_change = 0x01
};

enum class bitdefTSD_F_SET : unsigned short
{
   shutdown = 0x00,
   no_change = 0x01
};

enum class bitdefOSCH_DIV : unsigned short
{
   osch_160KHz = 0x00,
   osch_40KHz = 0x01
};

enum class bitdefOSCL_DIV : unsigned short
{
   oscl_262K144Hz = 0x00,
   oscl_2k096Hz = 0x01
};

enum class bitdefPULLUP_SEL : unsigned short
{
   off = 0x00,
   on = 0x01
};

enum class bitdefACTV_DLY : unsigned short
{
   delay_1cycle = 0x00,
   delay_2cycle = 0x01,
   delay_3cycle = 0x02,
   delay_4cycle = 0x03
};

class RegGPIO4
{
public:
   void hal(Ka495xx_interface* ifc) { this->ifc = ifc; }
   RegGPIO4& write()
   {
      if(ifc == nullptr) return;
      ifc->write(Ka495xx_addr::GPIO4, &GPIO4.bytes[0]);
      return *this;
   }
   RegGPIO4& update()
   {
      if(ifc == nullptr) return;
      ifc->read(Ka495xx_addr::GPIO4, &GPIO4.bytes[0]);
      return *this;
   }

   inline RegGPIO4& setOVP_F_SET(bitdefOVP_F_SET value)
   {
      GPIO4.b.OVP_F_SET = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefOVP_F_SET getOVP_F_SET() const
   {
      return static_cast<bitdefOVP_F_SET>(GPIO4.b.OVP_F_SET);
   }

   inline RegGPIO4& setTSD_F_SET(bitdefTSD_F_SET value)
   {
      GPIO4.b.TSD_F_SET = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefTSD_F_SET getTSD_F_SET() const
   {
      return static_cast<bitdefTSD_F_SET>(GPIO4.b.TSD_F_SET);
   }

   inline RegGPIO4& setOSCH_DIV(bitdefOSCH_DIV value)
   {
      GPIO4.b.OSCH_DIV = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefOSCH_DIV getOSCH_DIV() const
   {
      return static_cast<bitdefOSCH_DIV>(GPIO4.b.OSCH_DIV);
   }

   inline RegGPIO4& setOSCL_DIV(bitdefOSCL_DIV value)
   {
      GPIO4.b.OSCL_DIV = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefOSCL_DIV getOSCL_DIV() const
   {
      return static_cast<bitdefOSCL_DIV>(GPIO4.b.OSCL_DIV);
   }

   inline RegGPIO4& setTMON1PullUp(bitdefPULLUP_SEL value)
   {
      GPIO4.b.PULLUP_SEL1 = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefPULLUP_SEL getTMON1PullUp() const
   {
      return static_cast<bitdefPULLUP_SEL>(GPIO4.b.PULLUP_SEL1);
   }

   inline RegGPIO4& setTMON2PullUp(bitdefPULLUP_SEL value)
   {
      GPIO4.b.PULLUP_SEL2 = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefPULLUP_SEL getTMON2PullUp() const
   {
      return static_cast<bitdefPULLUP_SEL>(GPIO4.b.PULLUP_SEL2);
   }

   inline RegGPIO4& setTMON3PullUp(bitdefPULLUP_SEL value)
   {
      GPIO4.b.PULLUP_SEL3 = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefPULLUP_SEL getTMON3PullUp() const
   {
      return static_cast<bitdefPULLUP_SEL>(GPIO4.b.PULLUP_SEL3);
   }

   inline RegGPIO4& setTMON4PullUp(bitdefPULLUP_SEL value)
   {
      GPIO4.b.PULLUP_SEL4 = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefPULLUP_SEL getTMON4PullUp() const
   {
      return static_cast<bitdefPULLUP_SEL>(GPIO4.b.PULLUP_SEL4);
   }

   inline RegGPIO4& setTMON5PullUp(bitdefPULLUP_SEL value)
   {
      GPIO4.b.PULLUP_SEL5 = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefPULLUP_SEL getTMON5PullUp() const
   {
      return static_cast<bitdefPULLUP_SEL>(GPIO4.b.PULLUP_SEL5);
   }

   inline RegGPIO4& setACTV_DLY(bitdefACTV_DLY value)
   {
      GPIO4.b.ACTV_DLY = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefACTV_DLY getACTV_DLY() const
   {
      return static_cast<bitdefACTV_DLY>(GPIO4.b.ACTV_DLY);
   }
private:
   Ka495xx_interface* ifc;
   union
   {
      unsigned short hfword;
      unsigned char bytes[2];
      struct
      {
         unsigned short ACTV_DLY : 2;
         unsigned short : 1;
         unsigned short PULLUP_SEL1 : 1;
         unsigned short PULLUP_SEL2 : 1;
         unsigned short PULLUP_SEL3 : 1;
         unsigned short PULLUP_SEL4 : 1;
         unsigned short PULLUP_SEL5 : 1;
         unsigned short OSCL_DIV : 1;
         unsigned short OSCH_DIV : 1;
         unsigned short : 3;
         unsigned short TSD_F_SET : 1;
         unsigned short OVP_F_SET : 1;
         unsigned short : 1;
      } b;
   } GPIO4;
};

enum class bitdefALARMSEL : unsigned short
{
   all = 0x00,
   scd_only = 0x01
};

enum class bitdefEN_SCD : unsigned short
{
   disable = 0x00,
   enable = 0x01
};

enum class bitdefEN_OCD : unsigned short
{
   disable = 0x00,
   enable = 0x01
};

enum class bitdefEN_OCC : unsigned short
{
   disable = 0x00,
   enable = 0x01
};

enum class bitdefEN_CP : unsigned short
{
   disable = 0x00,
   enable = 0x01
};

enum class bitdefSCD_D : unsigned short
{
   threshold_20mV = 0x00,
   threshold_40mV = 0x01,
   threshold_60mV = 0x02,
   threshold_80mV = 0x03,
   threshold_100mV = 0x04,
   threshold_120mV = 0x05,
   threshold_140mV = 0x06,
   threshold_160mV = 0x07,
   threshold_180mV = 0x08,
   threshold_200mV = 0x09,
   threshold_220mV = 0x0A,
   threshold_240mV = 0x0B,
   threshold_260mV = 0x0C,
   threshold_280mV = 0x0D,
   threshold_300mV = 0x0E,
   threshold_320mV = 0x0F,
   threshold_340mV = 0x10,
   threshold_360mV = 0x11,
   threshold_380mV = 0x12,
   threshold_400mV = 0x13,
   threshold_420mV = 0x14,
   threshold_440mV = 0x15,
   threshold_460mV = 0x16,
   threshold_480mV = 0x17,
   threshold_500mV = 0x18,
   threshold_520mV = 0x19,
   threshold_540mV = 0x1A,
   threshold_560mV = 0x1B,
   threshold_580mV = 0x1C,
   threshold_600mV = 0x1D,
   threshold_620mV = 0x1E,
   threshold_640mV = 0x1F
};

enum class bitdefOCD_D : unsigned short
{
   threshold_10mV = 0x00,
   threshold_20mV = 0x01,
   threshold_30mV = 0x02,
   threshold_40mV = 0x03,
   threshold_50mV = 0x04,
   threshold_60mV = 0x05,
   threshold_70mV = 0x06,
   threshold_80mV = 0x07,
   threshold_90mV = 0x08,
   threshold_100mV = 0x09,
   threshold_110mV = 0x0A,
   threshold_120mV = 0x0B,
   threshold_130mV = 0x0C,
   threshold_140mV = 0x0D,
   threshold_150mV = 0x0E,
   threshold_160mV = 0x0F,
   threshold_170mV = 0x10,
   threshold_180mV = 0x11,
   threshold_190mV = 0x12,
   threshold_200mV = 0x13,
   threshold_210mV = 0x14,
   threshold_220mV = 0x15,
   threshold_230mV = 0x16,
   threshold_240mV = 0x17,
   threshold_250mV = 0x18,
   threshold_260mV = 0x19,
   threshold_270mV = 0x1A,
   threshold_280mV = 0x1B,
   threshold_290mV = 0x1C,
   threshold_200mV = 0x1D,
   threshold_310mV = 0x1E,
   threshold_320mV = 0x1F
};

enum class bitdefOCC_D : unsigned short
{
   threshold_5mV = 0x00,
   threshold_10mV = 0x01,
   threshold_15mV = 0x02,
   threshold_20mV = 0x03,
   threshold_25mV = 0x04,
   threshold_30mV = 0x05,
   threshold_35mV = 0x06,
   threshold_40mV = 0x07,
   threshold_45mV = 0x08,
   threshold_50mV = 0x09,
   threshold_55mV = 0x0A,
   threshold_60mV = 0x0B,
   threshold_65mV = 0x0C,
   threshold_70mV = 0x0D,
   threshold_75mV = 0x0E,
   threshold_80mV = 0x0F,
   threshold_85mV = 0x10,
   threshold_90mV = 0x11,
   threshold_95mV = 0x12,
   threshold_100mV = 0x13,
   threshold_105mV = 0x14,
   threshold_110mV = 0x15,
   threshold_115mV = 0x16,
   threshold_120mV = 0x17
};

enum class bitdefSCD_DLY : unsigned short
{
   delay_00us00 = 0x00,
   delay_31us25 = 0x01,
   delay_62us50 = 0x02,
   delay_93us75 = 0x03,
   delay_125us00 = 0x04,
   delay_156us25 = 0x05,
   delay_187us50 = 0x06,
   delay_218us75 = 0x07,
   delay_250us00 = 0x08,
   delay_281us25 = 0x09,
   delay_312us50 = 0x0A,
   delay_343us75 = 0x0B,
   delay_375us00 = 0x0C,
   delay_406us25 = 0x0D,
   delay_437us50 = 0x0E,
   delay_468us75 = 0x0F,
   delay_500us00 = 0x10,
   delay_531us25 = 0x11,
   delay_562us50 = 0x12,
   delay_593us75 = 0x13,
   delay_625us00 = 0x14,
   delay_656us25 = 0x15,
   delay_687us50 = 0x16,
   delay_718us75 = 0x17,
   delay_750us00 = 0x18,
   delay_781us25 = 0x19,
   delay_812us50 = 0x1A,
   delay_843us75 = 0x1B,
   delay_875us00 = 0x1C,
   delay_906us25 = 0x1D,
   delay_937us50 = 0x1E,
   delay_968us75 = 0x1F
};

enum class bitdefOCD_DLY : unsigned short
{
   delay_10ms = 0x00,
   delay_20ms = 0x01,
   delay_30ms = 0x02,
   delay_40ms = 0x03,
   delay_50ms = 0x04,
   delay_60ms = 0x05,
   delay_70ms = 0x06,
   delay_80ms = 0x07,
   delay_90ms = 0x08,
   delay_100ms = 0x09,
   delay_110ms = 0x0A,
   delay_120ms = 0x0B,
   delay_130ms = 0x0C,
   delay_140ms = 0x0D,
   delay_150ms = 0x0E,
   delay_160ms = 0x0F,
   delay_170ms = 0x10,
   delay_180ms = 0x11,
   delay_190ms = 0x12,
   delay_200ms = 0x13,
   delay_210ms = 0x14,
   delay_220ms = 0x15,
   delay_230ms = 0x16,
   delay_240ms = 0x17,
   delay_250ms = 0x18,
   delay_260ms = 0x19,
   delay_270ms = 0x1A,
   delay_280ms = 0x1B,
   delay_290ms = 0x1C,
   delay_300ms = 0x1D,
   delay_310ms = 0x1E,
   delay_320ms = 0x1F
};

enum class bitdefOCC_DLY : unsigned short
{
   delay_10ms = 0x00,
   delay_20ms = 0x01,
   delay_30ms = 0x02,
   delay_40ms = 0x03,
   delay_50ms = 0x04,
   delay_60ms = 0x05,
   delay_70ms = 0x06,
   delay_80ms = 0x07,
   delay_90ms = 0x08,
   delay_100ms = 0x09,
   delay_110ms = 0x0A,
   delay_120ms = 0x0B,
   delay_130ms = 0x0C,
   delay_140ms = 0x0D,
   delay_150ms = 0x0E,
   delay_160ms = 0x0F,
   delay_170ms = 0x10,
   delay_180ms = 0x11,
   delay_190ms = 0x12,
   delay_200ms = 0x13,
   delay_210ms = 0x14,
   delay_220ms = 0x15,
   delay_230ms = 0x16,
   delay_240ms = 0x17,
   delay_250ms = 0x18,
   delay_260ms = 0x19,
   delay_270ms = 0x1A,
   delay_280ms = 0x1B,
   delay_290ms = 0x1C,
   delay_300ms = 0x1D,
   delay_310ms = 0x1E,
   delay_320ms = 0x1F
};

class RegALARM
{
public:
   void hal(Ka495xx_interface* ifc) { this->ifc = ifc; }
   RegALARM& write()
   {
      if(ifc == nullptr) return;
      ifc->write(Ka495xx_addr::ALARM1, &ALARM1.bytes[0]);
      ifc->write(Ka495xx_addr::ALARM2, &ALARM2.bytes[0]);
      ifc->write(Ka495xx_addr::ALARM3, &ALARM3.bytes[0]);
      return *this;
   }
   RegALARM& update()
   {
      if(ifc == nullptr) return;
      ifc->read(Ka495xx_addr::ALARM1, &ALARM1.bytes[0]);
      ifc->read(Ka495xx_addr::ALARM2, &ALARM2.bytes[0]);
      ifc->read(Ka495xx_addr::ALARM3, &ALARM3.bytes[0]);
      return *this;
   }

   inline RegALARM& setALARMSEL(bitdefALARMSEL value)
   {
      ALARM1.b.ALARM_SEL = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefALARMSEL getALARMSEL() const
   {
      return static_cast<bitdefALARMSEL>(ALARM1.b.ALARM_SEL);
   }

   inline RegALARM& setEN_SCD(bitdefEN_SCD value)
   {
      ALARM1.b.EN_SCD = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefEN_SCD getEN_SCD() const
   {
      return static_cast<bitdefEN_SCD>(ALARM1.b.EN_SCD);
   }

   inline RegALARM& setEN_OCD(bitdefEN_OCD value)
   {
      ALARM1.b.EN_OCD = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefEN_OCD getEN_OCD() const
   {
      return static_cast<bitdefEN_OCD>(ALARM1.b.EN_OCD);
   }

   inline RegALARM& setSCD_D(bitdefSCD_D value)
   {
      ALARM2.b.SCD_D = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefSCD_D getSCD_D() const
   {
      return static_cast<bitdefSCD_D>(ALARM2.b.SCD_D);
   }

   inline RegALARM& setOCD_D(bitdefOCD_D value)
   {
      ALARM2.b.OCD_D = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefOCD_D getOCD_D() const
   {
      return static_cast<bitdefOCD_D>(ALARM2.b.OCD_D);
   }

   inline RegALARM& setOCC_D(bitdefOCC_D value)
   {
      ALARM2.b.OCC_D = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefOCC_D getOCC_D() const
   {
      return static_cast<bitdefOCC_D>(ALARM2.b.OCD_D);
   }

   inline RegALARM& setSCD_DLY(bitdefSCD_DLY value)
   {
      ALARM3.b.SCD_DLY = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefSCD_DLY getSCD_DLY() const
   {
      return static_cast<bitdefSCD_DLY>(ALARM3.b.SCD_DLY);
   }

   inline RegALARM& setOCD_DLY(bitdefOCD_DLY value)
   {
      ALARM3.b.OCD_DLY = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefOCD_DLY getOCD_DLY() const
   {
      return static_cast<bitdefOCD_DLY>(ALARM3.b.OCD_DLY);
   }
   
   inline RegALARM& setOCC_DLY(bitdefOCC_DLY value)
   {
      ALARM3.b.OCC_DLY = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefOCC_DLY getOCC_DLY() const
   {
      return static_cast<bitdefOCC_DLY>(ALARM3.b.OCC_DLY);
   }
private:
   Ka495xx_interface* ifc;
   union
   {
      unsigned short hfword;
      unsigned char bytes[2];
      struct
      {
         unsigned short EN_CP : 1;
         unsigned short EN_OCC : 1;
         unsigned short EN_OCD : 1;
         unsigned short EN_SCD : 1;
         unsigned short : 11;
         unsigned short ALARM_SEL : 1;
      } b;
   } ALARM1;

   union
   {
      unsigned short hfword;
      unsigned char bytes[2];
      struct
      {
         unsigned short OCC_D : 5;
         unsigned short OCD_D : 5;
         unsigned short SCD_D : 5;
         unsigned short : 1;
      } b;
   } ALARM2;

   union
   {
      unsigned short hfword;
      unsigned char bytes[2];
      struct
      {
         unsigned short OCC_DLY : 5;
         unsigned short OCD_DLY : 5;
         unsigned short SCD_DLY : 5;
         unsigned short : 1;
      } b;
   } ALARM3;
};

class RegCBSEL
{
public:
   void hal(Ka495xx_interface* ifc) { this->ifc = ifc; }
   RegCBSEL& write()
   {
      if(ifc == nullptr) return;
      ifc->write(Ka495xx_addr::CBSEL1, &CBSEL1.bytes[0]);
      ifc->write(Ka495xx_addr::CBSEL2, &CBSEL2.bytes[0]);
      return *this;
   }
   RegCBSEL& update()
   {
      if(ifc == nullptr) return;
      ifc->read(Ka495xx_addr::CBSEL1, &CBSEL1.bytes[0]);
      ifc->read(Ka495xx_addr::CBSEL2, &CBSEL2.bytes[0]);
      return *this;
   }

   inline RegCBSEL& select(CELLS channel)
   {
      unsigned long lower = static_cast<unsigned long>(channel) & 0x00007FFF;
      unsigned long upper = (static_cast<unsigned long>(channel) & 0x003F8000) >> 15;
      CBSEL1.hfword = lower;
      CBSEL2.hfword = upper;
      return *this;
   }
   inline RegCBSEL& deselect(CELLS channel)
   {
      unsigned long lower = static_cast<unsigned long>(channel) & 0x00007FFF;
      unsigned long upper = (static_cast<unsigned long>(channel) & 0x003F8000) >> 15;
      CBSEL1.hfword &= ~lower;
      CBSEL2.hfword &= ~upper;
      return *this;
   }
private:
   Ka495xx_interface* ifc;

   union
   {
      unsigned short hfword;
      unsigned char bytes[2];
      struct
      {
         unsigned short CELL1 : 1;
         unsigned short CELL2 : 1;
         unsigned short CELL3 : 1;
         unsigned short CELL4 : 1;
         unsigned short CELL5 : 1;
         unsigned short CELL6 : 1;
         unsigned short CELL7 : 1;
         unsigned short CELL8 : 1;
         unsigned short CELL9 : 1;
         unsigned short CELL10 : 1;
         unsigned short CELL11 : 1;
         unsigned short CELL12 : 1;
         unsigned short CELL13 : 1;
         unsigned short CELL14 : 1;
         unsigned short CELL15 : 1;
         unsigned short CELL16 : 1;
      } b;
   } CBSEL1;

   union
   {
      unsigned short hfword;
      unsigned char bytes[2];
      struct
      {
         unsigned short CELL17 : 1;
         unsigned short CELL18 : 1;
         unsigned short CELL19 : 1;
         unsigned short CELL20 : 1;
         unsigned short CELL21 : 1;
         unsigned short CELL22 : 1;
         unsigned short : 10;
      } b;
   } CBSEL2;
};

enum class bitdefDIS_OSC_OFF : unsigned short
{
   off = 0x00,
   on = 0x01
};

enum class bitdefSDI_PLDW : unsigned short
{
   off = 0x00,
   on = 0x01
};

enum class bitdefSCL_PLDW : unsigned short
{
   off = 0x00,
   on = 0x01
};

enum class bitdefSEN_PLDW : unsigned short
{
   off = 0x00,
   on = 0x01
};

enum class bitdefPD_REG55 : unsigned short
{
   normal = 0x00,
   power_down = 0x01
};

enum class bitdefNPD_CB : unsigned short
{
   power_down = 0x00,
   normal = 0x01
};

class RegOTHCTL
{
public:
   void hal(Ka495xx_interface* ifc) { this->ifc = ifc; }
   RegOTHCTL& write()
   {
      if(ifc == nullptr) return;
      ifc->write(Ka495xx_addr::OTH, &OTH.bytes[0]);
      ifc->write(Ka495xx_addr::OTH, &OTH.bytes[0]);
      return *this;
   }
   RegOTHCTL& update()
   {
      if(ifc == nullptr) return;
      ifc->read(Ka495xx_addr::CBSEL1, &OTH.bytes[0]);
      ifc->read(Ka495xx_addr::CBSEL2, &OTH.bytes[0]);
      return *this;
   }

   inline RegOTHCTL& setDIS_OSC_OFF(bitdefDIS_OSC_OFF value)
   {
      OTH.b.DIS_OSC_OFF = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefDIS_OSC_OFF getDIS_OSC_OFF() const
   {
      return static_cast<bitdefDIS_OSC_OFF>(OTH.b.DIS_OSC_OFF);
   }

   inline RegOTHCTL& setSDI_PLDW(bitdefSDI_PLDW value)
   {
      OTH.b.SDI_PLDW = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefSDI_PLDW getSDI_PLDW() const
   {
      return static_cast<bitdefSDI_PLDW>(OTH.b.SDI_PLDW);
   }

   inline RegOTHCTL& setSCL_PLDW(bitdefSCL_PLDW value)
   {
      OTH.b.SCL_PLDW = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefSCL_PLDW getSCL_PLDW() const
   {
      return static_cast<bitdefSCL_PLDW>(OTH.b.SCL_PLDW);
   }

   inline RegOTHCTL& setSEN_PLDW(bitdefSEN_PLDW value)
   {
      OTH.b.SEN_PLDW = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefSEN_PLDW getSEN_PLDW() const
   {
      return static_cast<bitdefSEN_PLDW>(OTH.b.SEN_PLDW);
   }

   inline RegOTHCTL& setPD_REG55(bitdefPD_REG55 value)
   {
      OTH.b.PD_REG55 = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefPD_REG55 getPD_REG55() const
   {
      return static_cast<bitdefPD_REG55>(OTH.b.PD_REG55);
   }

   inline RegOTHCTL& setNPD_CB(bitdefNPD_CB value)
   {
      OTH.b.NPD_CB = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefNPD_CB getNPD_CB() const
   {
      return static_cast<bitdefNPD_CB>(OTH.b.NPD_CB);
   }
private:
   Ka495xx_interface* ifc;
   union
   {
      unsigned short hfword;
      unsigned char bytes[2];
      struct
      {
         unsigned short NPD_CB : 1;
         unsigned short : 5;
         unsigned short PD_REG55 : 1;
         unsigned short : 1;
         unsigned short SEN_PLDW : 1;
         unsigned short SCL_PLDW : 1;
         unsigned short SDI_PLDW : 1;
         unsigned short : 3;
         unsigned short DIS_OSC_OFF : 1;
         unsigned short : 1;
      } b;
   } OTH;
};

enum class bitdefADSWHY_EN : unsigned short
{
   off = 0x00,
   on = 0x01
};

enum class bitdefADSWSD_EN : unsigned short
{
   off = 0x00,
   on = 0x01
};

enum class bitdefADIH_CSYNC : unsigned short
{
   disable = 0x00,
   enable = 0x01
};

enum class bitdefISD_STOPEN : unsigned short
{
   enable = 0x00,
   disable = 0x01
};

enum class bitdefADI_LATCH_SET : unsigned short
{
   instant = 0x00,
   sync = 0x01
};

enum class bitdefADV_LATCH_SET : unsigned short
{
   instant = 0x00,
   sync = 0x01
};

enum class bitdefADIL_ON  : unsigned short
{
   disable = 0x00,
   enable = 0x01
};

enum class bitdefADIH_ON : unsigned short
{
   disable = 0x00,
   enable = 0x01
};

class RegADCTL
{
public:
   void hal(Ka495xx_interface* ifc) { this->ifc = ifc; }
   RegADCTL& write()
   {
      if(ifc == nullptr) return;
      ifc->write(Ka495xx_addr::ADC, &ADCTL.bytes[0]);
      return *this;
   }
   RegADCTL& update()
   {
      if(ifc == nullptr) return;
      ifc->read(Ka495xx_addr::ADC, &ADCTL.bytes[0]);
      return *this;
   }

   inline RegADCTL& setADSWHY_EN(bitdefADSWHY_EN value)
   {
      ADCTL.b.ADSWHY_EN = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefADSWHY_EN getADSWHY_EN() const
   {
      return static_cast<bitdefADSWHY_EN>(ADCTL.b.ADSWHY_EN);
   }

   inline RegADCTL& setADSWSD_EN(bitdefADSWSD_EN value)
   {
      ADCTL.b.ADSWSD_EN = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefADSWSD_EN getADSWSD_EN() const
   {
      return static_cast<bitdefADSWSD_EN>(ADCTL.b.ADSWSD_EN);
   }

   inline RegADCTL& setADIH_CSYNC(bitdefADIH_CSYNC value)
   {
      ADCTL.b.ADIH_CSYNC = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefADIH_CSYNC getADIH_CSYNC() const
   {
      return static_cast<bitdefADIH_CSYNC>(ADCTL.b.ADIH_CSYNC);
   }

   inline RegADCTL& setISD_STOPEN(bitdefISD_STOPEN value)
   {
      ADCTL.b.ISD_STOP_EN = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefISD_STOPEN getISD_STOPEN() const
   {
      return static_cast<bitdefISD_STOPEN>(ADCTL.b.ISD_STOP_EN);
   }

   inline RegADCTL& setADI_LATCH_SET(bitdefADI_LATCH_SET value)
   {
      ADCTL.b.ADI_LATCH_SET = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefADI_LATCH_SET getADI_LATCH_SET() const
   {
      return static_cast<bitdefADI_LATCH_SET>(ADCTL.b.ADI_LATCH_SET);
   }

   inline RegADCTL& setADV_LATCH_SET(bitdefADV_LATCH_SET value)
   {
      ADCTL.b.ADV_LATCH_SET = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefADV_LATCH_SET getADV_LATCH_SET() const
   {
      return static_cast<bitdefADV_LATCH_SET>(ADCTL.b.ADV_LATCH_SET);
   }

   inline RegADCTL& setADIL_ON(bitdefADIL_ON value)
   {
      ADCTL.b.ADIL_ON = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefADIL_ON getADIL_ON() const
   {
      return static_cast<bitdefADIL_ON>(ADCTL.b.ADIL_ON);
   }

   inline RegADCTL& setADIH_ON(bitdefADIH_ON value)
   {
      ADCTL.b.ADIH_ON = static_cast<unsigned short>(value);
      return *this;
   }
   inline bitdefADIH_ON getADIH_ON() const
   {
      return static_cast<bitdefADIH_ON>(ADCTL.b.ADIH_ON);
   }
private:
   Ka495xx_interface* ifc;
   union
   {
      unsigned short hfword;
      unsigned char bytes[2];
      struct
      {
         unsigned short ADIH_ON : 1;
         unsigned short ADIL_ON : 1;
         unsigned short ADV_LATCH_SET : 1;
         unsigned short ADI_LATCH_SET : 1;
         unsigned short ISD_STOP_EN : 1;
         unsigned short ADIH_CSYNC : 1;
         unsigned short : 6;
         unsigned short ADSWSD_EN : 1;
         unsigned short ADSWHY_EN : 1;
         unsigned short : 2;
      } b;
   } ADCTL;
};

class RegINRCV
{
public:
   void hal(Ka495xx_interface* ifc) { this->ifc = ifc; }
   RegINRCV& write()
   {
      if(ifc == nullptr) return;
      ifc->write(Ka495xx_addr::INRCV1, &INRCV1.bytes[0]);
      ifc->write(Ka495xx_addr::INRCV2, &INRCV2.bytes[0]);
      return *this;
   }
   RegINRCV& update()
   {
      if(ifc == nullptr) return;
      ifc->read(Ka495xx_addr::INRCV1, &INRCV1.bytes[0]);
      ifc->read(Ka495xx_addr::INRCV2, &INRCV2.bytes[0]);
      return *this;
   }

   inline RegINRCV& select(CELLS channel)
   {
      unsigned long lower = static_cast<unsigned long>(channel) & 0x00007FFF;
      unsigned long upper = (static_cast<unsigned long>(channel) & 0x003F8000) >> 15;
      INRCV1.hfword = lower;
      INRCV2.hfword = upper;
      return *this;
   }
   inline RegINRCV& deselect(CELLS channel)
   {
      unsigned long lower = static_cast<unsigned long>(channel) & 0x00007FFF;
      unsigned long upper = (static_cast<unsigned long>(channel) & 0x003F8000) >> 15;
      INRCV1.hfword &= ~lower;
      INRCV2.hfword &= ~upper;
      return *this;
   }
private:
   Ka495xx_interface* ifc;

   union
   {
      unsigned short hfword;
      unsigned char bytes[2];
      struct
      {
         unsigned short CELL1 : 1;
         unsigned short CELL2 : 1;
         unsigned short CELL3 : 1;
         unsigned short CELL4 : 1;
         unsigned short CELL5 : 1;
         unsigned short CELL6 : 1;
         unsigned short CELL7 : 1;
         unsigned short CELL8 : 1;
         unsigned short CELL9 : 1;
         unsigned short CELL10 : 1;
         unsigned short CELL11 : 1;
         unsigned short CELL12 : 1;
         unsigned short CELL13 : 1;
         unsigned short CELL14 : 1;
         unsigned short CELL15 : 1;
         unsigned short CELL16 : 1;
      } b;
   } INRCV1;

   union
   {
      unsigned short hfword;
      unsigned char bytes[2];
      struct
      {
         unsigned short CELL17 : 1;
         unsigned short CELL18 : 1;
         unsigned short CELL19 : 1;
         unsigned short CELL20 : 1;
         unsigned short CELL21 : 1;
         unsigned short CELL22 : 1;
         unsigned short : 10;
      } b;
   } INRCV2;
};

#endif /* __KA495XX_SFR_H__ */
