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
typedef void (*hw_fet_off)(bool state);
typedef unsigned char (*spi_xfer8)(unsigned char data);
typedef void (*hw_delay_ms)(unsigned long ms);

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

enum class bitADC_CONT : unsigned char { once = 0x00, repeat = 0x01 };

enum class bitINTM_TIM : unsigned char
{
   delay_20ms = 0x00,
   delay_40ms = 0x01,
   delay_80ms = 0x02,
   delay_160ms = 0x03
};

enum class bitINTM_SEL : unsigned char
{
   no_int = 0x00,
   no_spi = 0x01,
   tinm_tim = 0x02,
   int_auto = 0x03
};

enum class bitLDM_SLP_EN : unsigned char { disable = 0x00, enable = 0x01 };

enum class bitVPC_SLP_EN : unsigned char { disable = 0x00, enable = 0x01 };

enum class bitLDM_STB_EN : unsigned char { disable = 0x00, enable = 0x01 };

enum class bitVPC_STB_EN : unsigned char { disable = 0x00, enable = 0x01 };

enum class bitAUTO_TIM : unsigned char
{
   delay_10ms = 0x00,
   delay_20ms = 0x01,
   delay_40ms = 0x02,
   delay_80ms = 0x03
};

enum class bitMSET_STB : unsigned char { normal = 0x00, standby = 0x01 };

enum class bitMSET_LP : unsigned char { normal = 0x00, low_power = 0x01 };

enum class bitMSET_SLP : unsigned char { normal = 0x00, sleep = 0x01 };

enum class bitMSET_SHDN : unsigned char { normal = 0x00, shutdown = 0x01 };

enum class bitNPD_RST : unsigned char { normal = 0x00, reset = 0x01 };

enum class bitINT5_EN : unsigned char { wdt_int_disable = 0x00, wdt_int_enable = 0x01 };

enum class bitINT4_EN : unsigned char
{
   cur_detect_int_disable = 0x00,
   cur_detect_int_enable = 0x01
};

enum class bitINT3_EN : unsigned char
{
   cur_release_int_disable = 0x00,
   cur_release_int_enable = 0x01
};

enum class bitINT2_EN : unsigned char
{
   ldm_int_disable = 0x00,
   ldm_int_enable = 0x01
};

enum class bitINT1_EN : unsigned char
{
   vpc_int_disable = 0x00,
   vpc_int_enable = 0x01
};

enum class bitCHG_DIS_CLR : unsigned char { no_change = 0x00, clear = 0x01 };

enum class bitREGEXT_EN : unsigned char { off = 0x00, on = 0x01 };

enum class bitSTB_REGEXT_LPEN : unsigned char { hp_mode = 0x00, lp_mode = 0x01 };

enum class bitSLP_REGEXT_LPEN : unsigned char { hp_mode = 0x00, lp_mode = 0x01 };

enum class bitINTM_REGEXT_LPEN : unsigned char { hp_mode = 0x00, lp_mode = 0x01 };

enum class bitSTB_VDD55_LPEN : unsigned char { hp_mode = 0x00, lp_mode = 0x01 };

enum class bitSLP_VDD55_LPEN : unsigned char { hp_mode = 0x00, lp_mode = 0x01 };

enum class bitINTM_VDD55_LPEN : unsigned char { hp_mode = 0x00, lp_mode = 0x01 };

enum class bitWDT_STB_EN : unsigned char { disable = 0x00, enable = 0x01 };

enum class bitWDT_REGEXT_OFF : unsigned char { off_immediatly = 0x00, retry = 0x01 };

enum class bitCOMTIMON : unsigned char { off = 0x00, on = 0x01 };

enum class bitSPI_WDTCount : unsigned char;

enum class bitFDRV_ALM_SD : unsigned char { no_response = 0x00, auto_off = 0x01 };

enum class bitFDRV_ALM_RCV : unsigned char { auto_recover = 0x00, manual_recover = 0x01 };

enum class bitFDRV_ALM_CLR : unsigned char { no_change = 0x00, clear = 0x01 };

enum class bitNPD_FDRV : unsigned char { power_down = 0x00, normal = 0x01 };

enum class bitFDRVG_SEL_CLK : unsigned char { div32 = 0x00, div128 = 0x01 };

enum class bitFDRV_CHG_FET : unsigned char { charge_off = 0x00, charge_on = 0x01 };

enum class bitFDRV_DIS_FET : unsigned char { discharge_off = 0x00, discharge_on = 0x01 };

enum class bitFDRV_STBY : unsigned char { normal = 0x00, standby = 0x01 };

enum class bitFDRV_LEVEL : unsigned char
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

enum class bitFDRV_OUVCTL : unsigned char
{
   cross_off_on_event = 0x00,
   both_off_on_event = 0x01
};

enum class bitCVSEL : unsigned long
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

bitCVSEL operator <(bitCVSEL a, bitCVSEL b)
{
   bitCVSEL ret;
   ret = static_cast<bitCVSEL>(static_cast<unsigned long>(a) | static_cast<unsigned long>(b));
   return ret;
}

bool operator &(bitCVSEL data, bitCVSEL b)
{
   unsigned long hexData = static_cast<unsigned long>(data);
   unsigned long hexB = static_cast<unsigned long>(b);

   return ((hexData & hexB) == hexB);
}

bitCVSEL operator >(bitCVSEL data, bitCVSEL b)
{
   bitCVSEL ret;
   unsigned long hexData = static_cast<unsigned long>(data);
   unsigned long hexB = static_cast<unsigned long>(b);
   ret = static_cast<bitCVSEL>((hexData & (~hexB)) & 0x003FFFFF);
   bitCVSEL ret;
}

enum class bitGVSEL : unsigned short
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

bitGVSEL operator <(bitGVSEL data, bitGVSEL add)
{
   bitGVSEL ret;
   ret = static_cast<bitGVSEL>(static_cast<unsigned long>(data) | static_cast<unsigned long>(add));
   return ret;
}

bitGVSEL operator >(bitGVSEL data, bitGVSEL sub)
{
   bitGVSEL ret;
   unsigned long hexData = static_cast<unsigned long>(data);
   unsigned long hexB = static_cast<unsigned long>(sub);
   ret = static_cast<bitGVSEL>((hexData & (~hexB)) & 0x003FFFFF);
   bitGVSEL ret;
}
#endif /* __KA495XX_SFR_H__ */
