/*---------------------------------------------------------------------------*
  Project:  TwlSDK - PM - include - common
  File:     pm_common.h

  Copyright 2003-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-04#$
  $Rev: 10698 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_PM_COMMON_H_
#define NITRO_PM_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SDK_TWL
//---------------- NITRO
#include <nitro/misc.h>
#include <nitro/types.h>
#ifdef SDK_ARM9
#include <nitro/hw/ARM9/ioreg_PAD.h>
#else  // SDK_ARM9
#include <nitro/hw/ARM7/ioreg_PAD.h>
#endif // SDK_ARM9

#else
//---------------- TWL
#include <twl/misc.h>
#include <twl/types.h>
#ifdef SDK_ARM9
#include <twl/hw/ARM9/ioreg_PAD.h>
#else  // SDK_ARM9
#include <twl/hw/ARM7/ioreg_PAD.h>
#endif // SDK_ARM9
#include <twl/spi/common/pm_common.h>
#endif


//================================================================
//    PMIC register parameter
//================================================================
//---------------- address
#define REG_PMIC_CTL_ADDR        0     // R/W
#define REG_PMIC_STAT_ADDR       1     // R
#define REG_PMIC_OP_CTL_ADDR     2     // R/W
#define REG_PMIC_PGA_GAIN_ADDR   3     // R/W
#define REG_PMIC_BL_CTL_ADDR     4     // R/W(bit0-3), R(bit4,5,7)
#define PMIC_REG_NUMS            5     // nums of register

//---------------- each register spec
//---- PMIC_CTL
#define PMIC_CTL_SND_PWR        (1<< 0)
#define PMIC_CTL_SND_VOLCTRL    (1<< 1)
#define PMIC_CTL_BKLT1          (1<< 2)
#define PMIC_CTL_BKLT2          (1<< 3)
#define PMIC_CTL_LED_SW         (1<< 4)
#define PMIC_CTL_LED_SP         (1<< 5)
#define PMIC_CTL_PWR_OFF        (1<< 6)

//---- PMIC_STAT
#define PMIC_STAT_VDET          (1<< 0)

//---- PMIC_OP_CTL
#define PMIC_OP_CTL             (1<< 0)

//---- PMIC_PGA_GAIN
#define PMIC_PGA_GAIN_SHIFT     0
#define PMIC_PGA_GAIN_SCALE     4
#define PMIC_PGA_GAIN_MASK      ((PMIC_PGA_GAIN_SCALE-1)<< PMIC_PGA_GAIN_SHIFT)

//---- PMIC_BL_CTL
#define PMIC_BL_CTL_BL_SHIFT    0
#define PMIC_BL_CTL_BL_MASK     (3<< PMIC_BL_CTL_BL_SHIFT)
#define PMIC_BL_CTL_ADPT_SW     (1<< 2)
#define PMIC_BL_CTL_ADPT_DETECT (1<< 3)
#define PMIC_BL_CTL_VERSION     (1<< 6)


//---------------- PMIC register access spec
#define PMIC_REG_READ           1
#define PMIC_REG_WRITE          0
#define PMIC_REG_OP_SHIFT       7
#define PMIC_REG_OP_MASK        1

//================================================================
//    UTILITY command to access PMIC 
//================================================================
//---- pm ulitity proc number
enum
{
    PM_UTIL_DUMMY = 0,
    PM_UTIL_LED_ON,
    PM_UTIL_LED_BLINK_HIGH_SPEED,
    PM_UTIL_LED_BLINK_LOW_SPEED,
    PM_UTIL_LCD1_BACKLIGHT_ON,
    PM_UTIL_LCD1_BACKLIGHT_OFF,
    PM_UTIL_LCD2_BACKLIGHT_ON,
    PM_UTIL_LCD2_BACKLIGHT_OFF,
    PM_UTIL_LCD12_BACKLIGHT_ON,
    PM_UTIL_LCD12_BACKLIGHT_OFF,
    PM_UTIL_SOUND_POWER_ON,
    PM_UTIL_SOUND_POWER_OFF,
    PM_UTIL_SOUND_VOL_CTRL_ON,
    PM_UTIL_SOUND_VOL_CTRL_OFF,
    PM_UTIL_FORCE_POWER_OFF,

	//get status
	PM_UTIL_GET_STATUS,

	//set status
	PM_UTIL_SET_AMP,
	PM_UTIL_SET_AMPGAIN,
	PM_UTIL_SET_BLINK,

#ifdef SDK_TWL
	//reset & shutdown
	PM_UTIL_FORCE_RESET_HARDWARE,
	PM_UTIL_FORCE_EXIT,

	//wireless LED
	PM_UTIL_WIRELESS_LED,

	//amp gain
	PM_UTIL_SET_AMPGAIN_LEVEL,
#endif

#ifdef SDK_TWL
	// set backlight brightness (for system. don't use)
	PMi_UTIL_SET_BACKLIGHT_BRIGHTNESS,
#endif

	//direct register op. (for debug. don't use)
	PMi_UTIL_READREG,
	PMi_UTIL_WRITEREG,
	PMi_UTIL_PMIC_10,

	//dummy end
	PM_UTIL_DUMMYEND
};

// ( parameter for PM_UTIL_GET_STATUS )
enum
{
	PM_UTIL_PARAM_BATTERY,
	PM_UTIL_PARAM_BATTERY_LEVEL,
	PM_UTIL_PARAM_AC_ADAPTER,
	PM_UTIL_PARAM_BACKLIGHT,
	PM_UTIL_PARAM_SOUND_POWER,
	PM_UTIL_PARAM_SOUND_VOLUME,
	PM_UTIL_PARAM_AMP,
	PM_UTIL_PARAM_AMPGAIN,
	PM_UTIL_PARAM_BLINK,

#ifdef SDK_TWL
	PM_UTIL_PARAM_AMPGAIN_LEVEL,
#endif
#ifdef SDK_TWL
	// get backlight brightness (for system. don't use)
	PMi_UTIL_GET_BACKLIGHT_BRIGHTNESS,
#endif

	PM_UTIL_PARAM_DUMMYEND
};

//================================================================
//    NOTIFY command from ARM7 to ARM9
//================================================================
#ifdef SDK_TWL
#define PM_NOTIFY_POWER_SWITCH    0
#define PM_NOTIFY_RESET_HARDWARE  1
#define PM_NOTIFY_SHUTDOWN        2
#define PM_NOTIFY_BATTERY_CHANGED 3
#define PM_NOTIFY_BATTERY_LOW     4
#define PM_NOTIFY_BATTERY_EMPTY   5
#endif

//================================================================
//    BAUDRATE parameter
//================================================================
#define PM_BAUDRATE_4MHZ          0
#define PM_BAUDRATE_2MHZ          1
#define PM_BAUDRATE_1MHZ          2
#define PM_BAUDRATE_512KHZ        3

//---- default
#define PM_BAUDRATE_PMIC_DEFAULT  PM_BAUDRATE_1MHZ

//================================================================
//    SLEEP parameter
//================================================================
//---- factors to wake up from sleep mode
#define PM_TRIGGER_KEY              (1 << 0)
#define PM_TRIGGER_RTC_ALARM        (1 << 1)
#define PM_TRIGGER_COVER_OPEN       (1 << 2)
#define PM_TRIGGER_CARD             (1 << 3)
#define PM_TRIGGER_CARTRIDGE        (1 << 4)
#ifdef SDK_TWL
#define PM_TRIGGER_SDIO             (1 << 5)
#define PM_TRIGGER_MASK             (PM_TRIGGER_KEY|PM_TRIGGER_RTC_ALARM|PM_TRIGGER_COVER_OPEN|PM_TRIGGER_CARD|PM_TRIGGER_CARTRIDGE|PM_TRIGGER_SDIO)
#else
#define PM_TRIGGER_MASK             (PM_TRIGGER_KEY|PM_TRIGGER_RTC_ALARM|PM_TRIGGER_COVER_OPEN|PM_TRIGGER_CARD|PM_TRIGGER_CARTRIDGE)
#endif
typedef u32 PMWakeUpTrigger;

//---- logic for key interrupt
#define PM_PAD_LOGIC_OR    (0 << REG_PAD_KEYCNT_LOGIC_SHIFT)
#define PM_PAD_LOGIC_AND   (1 << REG_PAD_KEYCNT_LOGIC_SHIFT)
typedef u32 PMLogic;

//---- backlight to recover from sleep  (uses in library)
#define PM_BACKLIGHT_RECOVER_TOP_SHIFT      6
#define PM_BACKLIGHT_RECOVER_BOTTOM_SHIFT   7
#define PM_BACKLIGHT_RECOVER_TOP_MASK       (1<<PM_BACKLIGHT_RECOVER_TOP_SHIFT)
#define PM_BACKLIGHT_RECOVER_TOP_ON         PM_BACKLIGHT_RECOVER_TOP_MASK
#define PM_BACKLIGHT_RECOVER_TOP_OFF        0
#define PM_BACKLIGHT_RECOVER_BOTTOM_MASK    (1<<PM_BACKLIGHT_RECOVER_BOTTOM_SHIFT)
#define PM_BACKLIGHT_RECOVER_BOTTOM_ON      PM_BACKLIGHT_RECOVER_BOTTOM_MASK
#define PM_BACKLIGHT_RECOVER_BOTTOM_OFF     0
#define PM_BACKLIGHT_RECOVER_MASK           (PM_BACKLIGHT_RECOVER_TOP_MASK | PM_BACKLIGHT_RECOVER_BOTTOM_MASK)

//================================================================
//     LED pattern parameter
//================================================================
//---- LED blink pattern
typedef enum
{
    PM_LED_PATTERN_NONE = 0,           // don't care
    PM_LED_PATTERN_ON = 1,
    PM_LED_PATTERN_BLINK_LOW = 2,
    PM_LED_PATTERN_BLINK_HIGH = 3,
    PM_LED_PATTERN_BLINK1 = 4,
    PM_LED_PATTERN_BLINK2 = 5,
    PM_LED_PATTERN_BLINK3 = 6,
    PM_LED_PATTERN_BLINK4 = 7,
    PM_LED_PATTERN_BLINK5 = 8,
    PM_LED_PATTERN_BLINK6 = 9,
    PM_LED_PATTERN_BLINK8 = 10,
    PM_LED_PATTERN_BLINK10 = 11,
    PM_LED_PATTERN_PATTERN1 = 12,
    PM_LED_PATTERN_PATTERN2 = 13,
    PM_LED_PATTERN_PATTERN3 = 14,
    PM_LED_PATTERN_WIRELESS = 15
}
PMLEDPattern;

#define PM_LED_PATTERN_MAX  PM_LED_PATTERN_WIRELESS


//---- LED status
typedef enum
{
    PM_LED_NONE = 0,                   // dummy
    PM_LED_ON = 1,
    PM_LED_BLINK_LOW = 2,
    PM_LED_BLINK_HIGH = 3
}
PMLEDStatus;

#ifdef SDK_TWL
//---- Wireless LED status
typedef enum
{
    PM_WIRELESS_LED_OFF = FALSE,
    PM_WIRELESS_LED_ON  = TRUE
}
PMWirelessLEDStatus;
#endif

//================================================================
//     BATTERY
//================================================================
//---- battery level status ( empty: 0, 1,..., 5 : full )
typedef u16 PMBatteryLevel;
#define PM_BATTERY_LEVEL_MIN      0
#define PM_BATTERY_LEVEL_MAX      5


//================================================================
#ifdef SDK_TWL
//---- pm reset flag ID
#define   PM_RESET_FLAG_NONE       0
#define   PM_RESET_FLAG_FORCED     1

//---- amp gain level
#define   PM_AMPGAIN_LEVEL_SCALE   120
#define   PM_AMPGAIN_LEVEL_MIN     0
#define   PM_AMPGAIN_LEVEL_MAX     (PM_AMPGAIN_LEVEL_SCALE - 1)
#endif
#define   PM_AMPGAIN_LEVEL_DS0     31
#define   PM_AMPGAIN_LEVEL_DS1     43
#define   PM_AMPGAIN_LEVEL_DS2     55
#define   PM_AMPGAIN_LEVEL_DS3     67

//---- status of folding
#define   PM_READING               -1

#define   PM_SUCCESS               0
#define   PM_BUSY                  1
#define   PM_INVALID_COMMAND       0xffff

#define   PM_RESULT_NONE           0xff
#define   PM_RESULT_SUCCESS        (PM_SUCCESS)
#define   PM_RESULT_BUSY           (PM_BUSY)
#define   PM_RESULT_ERROR          2
#define   PM_RESULT_NA             3

//----status of PXI
#define   PM_PXI_RESULT_EXCLUSIVE  0xffff

//================================================================


#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_PM_COMMON_H_ */
#endif

