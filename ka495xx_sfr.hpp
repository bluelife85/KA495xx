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
   inline RegPWR_CTRL& setADC_CONT(unsigned short value)
   {
      reg.b.ADC_CONT = value;
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
   inline RegPWR_CTRL& setINTM_TIM(unsigned short value)
   {
      reg.b.INTM_TIM = value;
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
   inline RegPWR_CTRL& setINTM_SEL(unsigned short value)
   {
      reg.b.INTM_SEL = value;
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
   inline RegPWR_CTRL& setLDM_SLP_EN(unsigned short value)
   {
      reg.b.LDM_SLP_EN = value;
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
   inline RegPWR_CTRL& setVPC_SLP_EN(unsigned short value)
   {
      reg.b.VPC_SLP_EN = value;
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
   inline RegPWR_CTRL& setLDM_STB_EN(unsigned short value)
   {
      reg.b.LDM_STB_EN = value;
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
   inline RegPWR_CTRL& setVPC_STB_EN(unsigned short value)
   {
      reg.b.VPC_STB_EN = value;
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
   inline RegPWR_CTRL& setAUTO_TIM(unsigned short value)
   {
      reg.b.AUTO_TIM = value;
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
   inline RegPWR_CTRL& setMSET_STB(unsigned short value)
   {
      reg.b.MSET_STB = value;
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
   inline RegPWR_CTRL& setMSET_LP(unsigned short value)
   {
      reg.b.MSET_LP = value;
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
   inline RegPWR_CTRL& setMSET_SLP(unsigned short value)
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
   inline RegPWR_CTRL& setMSET_SHDN(unsigned short value)
   {
      reg.b.MSET_SHDN = value;
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
   inline RegPWR_CTRL& setNPD_RST(unsigned short value)
   {
      reg.b.NPD_RST = value;
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
   inline RegINT_EN& setINT5_EN(unsigned short value)
   {
      reg.b.INT5_EN = value;
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
   inline RegINT_EN& setINT4_EN(unsigned short value)
   {
      reg.b.INT4_EN = value;
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
   inline RegINT_EN& setINT3_EN(unsigned short value)
   {
      reg.b.INT3_EN = value;
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
   inline RegINT_EN& setINT2_EN(unsigned short value)
   {
      reg.b.INT2_EN = value;
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
   inline RegINT_EN& setINT1_EN(unsigned short value)
   {
      reg.b.INT1_EN = value;
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

   inline RegINT_EN& setCHG_DIS_CLR(unsigned short value)
   {
      reg.b.CHG_DIS_CLR = value;
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
   inline RegINT_EN& setREG_EXT_EN(unsigned short value)
   {
      reg.b.REG_EXT_EN = value;
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
   inline RegINT_EN& setSTB_REG_EXT_LPEN(unsigned short value)
   {
      reg.b.STB_REG_EXT_LPEN = value;
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
   inline RegINT_EN& setSLP_REG_EXT_LPEN(unsigned short value)
   {
      reg.b.SLP_REG_EXT_LPEN = value;
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
   inline RegINT_EN& setINTM_REG_EXT_LPEN(unsigned short value)
   {
      reg.b.INTM_REG_EXT_LPEN = value;
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
   inline RegINT_EN& setSTB_VDD55_LPEN(unsigned short value)
   {
      reg.b.STB_VDD55_LPEN = value;
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
   inline RegINT_EN& setSLP_VDD55_LPEN(unsigned short value)
   {
      reg.b.SLP_VDD55_LPEN = value;
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
   inline RegINT_EN& setINTM_VDD55_LPEN(unsigned short value)
   {
      reg.b.INTM_VDD55_LPEN = value;
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
   inline RegSPIWD_CTL& setWDT_STB_EN(unsigned short value)
   {
      reg.b.WDT_STB_EN = value;
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
   inline RegSPIWD_CTL& setWDT_REGEXT_OFF(unsigned short value)
   {
      reg.b.WDT_REG_EXT_OFF = value;
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
   inline RegSPIWD_CTL& setCOMTIMON(unsigned short value)
   {
      reg.b.COMTIMON = value;
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
   inline RegFDRV_CTRL& setFDRV_ALM_SD(unsigned short value)
   {
      reg.b.FDRV_ALM_SD = value;
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
   inline RegFDRV_CTRL& setFDRV_ALM_RCV(unsigned short value)
   {
      reg.b.FDRV_ALM_RCV = value;
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
   inline RegFDRV_CTRL& setFDRV_ALM_CLR(unsigned short value)
   {
      reg.b.FDRV_ALM_CLR = value;
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
   inline RegFDRV_CTRL& setNPD_FDRV(unsigned short value)
   {
      reg.b.NPD_FDRV = value;
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
   inline RegFDRV_CTRL& setFDRV_SEL_CLK(unsigned short value)
   {
      reg.b.FDRV_SEL_CLK = value;
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
   inline RegFDRV_CTRL& setFDRV_CHG_FET(unsigned short value)
   {
      reg.b.FDRV_CHG_FET = value;
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
   inline RegFDRV_CTRL& setFDRV_DIS_FET(unsigned short value)
   {
      reg.b.FDRV_DIS_FET = value;
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
   inline RegFDRV_CTRL& setFDRV_STBY(unsigned short value)
   {
      reg.b.FDRV_STBY = value;
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
   inline RegFDRV_CTRL& setFDRV_LEVEL(unsigned short value)
   {
      reg.b.FDRV_LEVEL = value;
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
   inline RegFDRV_CTRL& setFDRV_OUV_CTL(unsigned short value)
   {
      reg.b.FDRV_OUV_CTL = value;
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

enum class bitdefCVSEL : unsigned long
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

bitdefCVSEL operator|(bitdefCVSEL a, bitdefCVSEL b)
{
   bitdefCVSEL ret;
   ret = static_cast<bitdefCVSEL>(static_cast<unsigned long>(a) | static_cast<unsigned long>(b));
   return ret;
}

bitdefCVSEL operator~(bitdefCVSEL b)
{
   bitdefCVSEL ret;
   unsigned long inverted = static_cast<unsigned long>(b);
   ret = static_cast<bitdefCVSEL>(~inverted);
   return ret;
}

bitdefCVSEL operator&(bitdefCVSEL a, bitdefCVSEL b)
{
   bitdefCVSEL ret;
   unsigned long hexA = static_cast<unsigned long>(a);
   unsigned long hexB = static_cast<unsigned long>(b);
   ret = static_cast<bitdefCVSEL>(hexA & hexB);
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

   RegCVSEL& select(bitdefCVSEL channel)
   {
      unsigned long lower = static_cast<unsigned long>(channel) & 0x00007FFF;
      unsigned long upper = (static_cast<unsigned long>(channel) & 0x003F8000) >> 15;
      CVSEL1.hfword = lower;
      CVSEL2.hfword = upper;
      return *this;
   }
   RegCVSEL& deselect(bitdefCVSEL channel)
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

enum class bitdefADI_LATCH : unsigned char
{
   not_effect = 0x00,
   conversion = 0x01
};

enum class bitdefGPIO1SEL : unsigned char
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

enum class bitdefGPIO1_CHDRV : unsigned char
{
   drive_2mA = 0x00,
   drive_4mA = 0x01
};

enum class bitdefGPIO1_OUT : unsigned char
{
   low = 0x00,
   high = 0x01
};

enum class bitdefGPIO1_MODE : unsigned char
{
   push_pull = 0x00,
   open_drain = 0x01
};

enum class bitdefGPIO1_PD : unsigned char
{
   no_pulldown = 0x00,
   pulldown = 0x01
};

enum class bitdefGPIO1_NOE : unsigned char
{
   enable = 0x00,
   disable = 0x01
};

enum class bitdefGPIO1_IE : unsigned char
{
   disable = 0x00,
   enable = 0x01
};

enum class bitdefGPIO2SEL : unsigned char
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

enum class bitdefGPIO2_CHDRV : unsigned char
{
   drive_2mA = 0x00,
   drive_4mA = 0x01
};

enum class bitdefGPIO2_OUT : unsigned char
{
   low = 0x00,
   high = 0x01
};

enum class bitdefGPIO2_MODE : unsigned char
{
   push_pull = 0x00,
   open_drain = 0x01
};

enum class bitdefGPIO2_PD : unsigned char
{
   no_pulldown = 0x00,
   pulldown = 0x01
};

enum class bitdefGPIO2_NOE : unsigned char
{
   enable = 0x00,
   disable = 0x01
};

enum class bitdefGPIO2_IE : unsigned char
{
   disable = 0x00,
   enable = 0x01
};

enum class bitdefGPIO3SEL : unsigned char
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

enum class bitdefGPIO3_CHDRV : unsigned char
{
   drive_2mA = 0x00,
   drive_4mA = 0x01
};

enum class bitdefGPIO3_OUT : unsigned char
{
   low = 0x00,
   high = 0x01
};

enum class bitdefGPIO3_MODE : unsigned char
{
   push_pull = 0x00,
   open_drain = 0x01
};

enum class bitdefGPIO3_PD : unsigned char
{
   no_pulldown = 0x00,
   pulldown = 0x01
};

enum class bitdefGPIO3_NOE : unsigned char
{
   enable = 0x00,
   disable = 0x01
};

enum class bitdefGPIO3_IE : unsigned char
{
   disable = 0x00,
   enable = 0x01
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

enum class bitdefGPOH1_CTL : unsigned char
{
   high = 0x00,
   low = 0x01
};

enum class bitdefOVP_F_SET : unsigned char
{
   shutdown = 0x00,
   no_change = 0x01
};

enum class bitdefTSD_F_SET : unsigned char
{
   shutdown = 0x00,
   no_change = 0x01
};

#endif /* __KA495XX_SFR_H__ */
