/*---------------------------------------------------------------------------*
  Project:  TwlSDK - SPI - include
  File:     type.h

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-01-11#$
  $Rev: 3502 $
  $Author: yada $
 *---------------------------------------------------------------------------*/
#ifndef NITRO_SPI_COMMON_TYPE_H_
#define NITRO_SPI_COMMON_TYPE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================*/

/*---------------------------------------------------------------------------*
    �萔��`
 *---------------------------------------------------------------------------*/
// PXI�ł̒ʐM�v���g�R���֘A��`
#define SPI_PXI_START_BIT                   0x02000000  // �A���p�P�b�g�J�n�r�b�g
#define SPI_PXI_END_BIT                     0x01000000  // �A���p�P�b�g�I���r�b�g
#define SPI_PXI_INDEX_MASK                  0x000f0000  // �A���p�P�b�g�̃C���f�b�N�X��
#define SPI_PXI_INDEX_SHIFT                 16  //  �������r�b�g�̔z�u���`
#define SPI_PXI_DATA_MASK                   0x0000ffff  // �A���p�P�b�g�̃f�[�^��
#define SPI_PXI_DATA_SHIFT                  0   //  �������r�b�g�̔z�u���`
#define SPI_PXI_RESULT_COMMAND_BIT          0x00008000  // �����������r�b�g
#define SPI_PXI_RESULT_COMMAND_MASK         0x00007f00  // �����R�}���h�̎�ʕ���
#define SPI_PXI_RESULT_COMMAND_SHIFT        8   //  �r�b�g�z�u���`
#define SPI_PXI_RESULT_DATA_MASK            0x000000ff  // �����R�}���h�̌��ʕ���
#define SPI_PXI_RESULT_DATA_SHIFT           0   //  �r�b�g�z�u���`
#define SPI_PXI_CONTINUOUS_PACKET_MAX       16  // �A���p�P�b�g�̍ő�A����

// PXI�o�R��ARM9��蔭�s����閽�ߌQ(0x10�̃r�b�g��1�̂��̂�ARM7����̒ʒB�R�}���h)
#define SPI_PXI_COMMAND_TP_SAMPLING         0x0000
#define SPI_PXI_COMMAND_TP_AUTO_ON          0x0001
#define SPI_PXI_COMMAND_TP_AUTO_OFF         0x0002
#define SPI_PXI_COMMAND_TP_SETUP_STABILITY  0x0003
#define SPI_PXI_COMMAND_TP_AUTO_SAMPLING    0x0010      // �����T���v�����O�̓r���o�ߒʒB�p

#define SPI_PXI_COMMAND_NVRAM_WREN          0x0020
#define SPI_PXI_COMMAND_NVRAM_WRDI          0x0021
#define SPI_PXI_COMMAND_NVRAM_RDSR          0x0022
#define SPI_PXI_COMMAND_NVRAM_READ          0x0023
#define SPI_PXI_COMMAND_NVRAM_FAST_READ     0x0024
#define SPI_PXI_COMMAND_NVRAM_PW            0x0025
#define SPI_PXI_COMMAND_NVRAM_PP            0x0026
#define SPI_PXI_COMMAND_NVRAM_PE            0x0027
#define SPI_PXI_COMMAND_NVRAM_SE            0x0028
#define SPI_PXI_COMMAND_NVRAM_DP            0x0029
#define SPI_PXI_COMMAND_NVRAM_RDP           0x002a
#define SPI_PXI_COMMAND_NVRAM_CE            0x002b
#define SPI_PXI_COMMAND_NVRAM_RSI           0x002c
#define SPI_PXI_COMMAND_NVRAM_SR            0x002d

#define SPI_PXI_COMMAND_MIC_SAMPLING        0x0040
#define SPI_PXI_COMMAND_MIC_AUTO_ON         0x0041
#define SPI_PXI_COMMAND_MIC_AUTO_OFF        0x0042
#define SPI_PXI_COMMAND_MIC_AUTO_ADJUST     0x0043
#ifdef  SDK_TWL
#define SPI_PXI_COMMAND_MIC_LTDAUTO_ON      0x0044
#define SPI_PXI_COMMAND_MIC_LTDAUTO_OFF     0x0045
#define SPI_PXI_COMMAND_MIC_LTDAUTO_ADJUST  0x0046
#endif
#define SPI_PXI_COMMAND_MIC_BUFFER_FULL     0x0051

// for PM
#define SPI_PXI_COMMAND_PM_SYNC             0x0060
#define SPI_PXI_COMMAND_PM_UTILITY          0x0061
#define SPI_PXI_COMMAND_PM_SLEEP_START      0x0062
#define SPI_PXI_COMMAND_PM_SLEEP_END        0x0063
#define SPI_PXI_COMMAND_PM_SET_LED          0x0064
#ifdef SDK_TWL
#define SPI_PXI_COMMAND_PM_NOTIFY           0x0070
#endif


// PXI�o�R��ARM7���Ԃ���鉞���Q
#define SPI_PXI_RESULT_SUCCESS              0x0000      // ����
#define SPI_PXI_RESULT_INVALID_COMMAND      0x0001      // ���߂��ُ�
#define SPI_PXI_RESULT_INVALID_PARAMETER    0x0002      // �p�����[�^���ُ�
#define SPI_PXI_RESULT_ILLEGAL_STATUS       0x0003      // ���߂��󂯕t�����Ȃ����
#define SPI_PXI_RESULT_EXCLUSIVE            0x0004      // SPI�f�o�C�X���r������Ă���


// �^�b�`�p�l���֘A��`
#define SPI_TP_SAMPLING_FREQUENCY_MAX       4   // �����T���v�����O�p�x�̍ő�l
#define SPI_TP_DEFAULT_STABILITY_RANGE      20  // ���蔻�莞�ɋ��e�����o�͔͈͍�(�����l)
#define SPI_TP_VALARM_TAG                   0x54505641  // "TPVA" - TP�pV�A���[���̃^�O

// NVRAM(�t���b�V��ROM)�֘A��`
#define SPI_NVRAM_PAGE_SIZE                 256
#define SPI_NVRAM_SECTOR_SIZE               ( SPI_NVRAM_PAGE_SIZE * 256 )
#define SPI_NVRAM_ALL_SIZE                  ( SPI_NVRAM_SECTOR_SIZE * 8 )

// �}�C�N�֘A��`
#define SPI_MIC_SAMPLING_TYPE_8BIT          0x0000
#define SPI_MIC_SAMPLING_TYPE_12BIT         0x0001
#define SPI_MIC_SAMPLING_TYPE_S8BIT         0x0002
#define SPI_MIC_SAMPLING_TYPE_S12BIT        0x0003
#define SPI_MIC_SAMPLING_TYPE_FILTER_ON     0x0000
#define SPI_MIC_SAMPLING_TYPE_FILTER_OFF    0x0004
#define SPI_MIC_SAMPLING_TYPE_ADMODE_MASK   0x0007
#define SPI_MIC_SAMPLING_TYPE_BIT_MASK      0x0001
#define SPI_MIC_SAMPLING_TYPE_SIGNED_MASK   0x0002
#define SPI_MIC_SAMPLING_TYPE_FILTER_MASK   0x0004

#define SPI_MIC_SAMPLING_TYPE_LOOP_OFF      0x0000
#define SPI_MIC_SAMPLING_TYPE_LOOP_ON       0x0010
#define SPI_MIC_SAMPLING_TYPE_LOOP_MASK     0x0010

#define SPI_MIC_SAMPLING_TYPE_CORRECT_OFF   0x0000
#define SPI_MIC_SAMPLING_TYPE_CORRECT_ON    0x0020
#define SPI_MIC_SAMPLING_TYPE_CORRECT_MASK  0x0020

#define SPI_MIC_SAMPLING_TYPE_MIX_TP_OFF    0x0000
#define SPI_MIC_SAMPLING_TYPE_MIX_TP_ON     0x0040
#define SPI_MIC_SAMPLING_TYPE_MIX_TP_MASK   0x0040


/*---------------------------------------------------------------------------*
    �\���̒�`
 *---------------------------------------------------------------------------*/
// SPI�֘A�f�o�C�X���
typedef enum SPIDeviceType
{
    SPI_DEVICE_TYPE_TP = 0,            // �^�b�`�p�l��
    SPI_DEVICE_TYPE_NVRAM,             // NVRAM(�t���b�V��ROM)
    SPI_DEVICE_TYPE_MIC,               // �}�C�N
    SPI_DEVICE_TYPE_PM,                // �p���[�}�l�W�����gIC
    SPI_DEVICE_TYPE_ARM7,
    SPI_DEVICE_TYPE_MAX
}
SPIDeviceType;

// �^�b�`�p�l���̐ڐG����
typedef enum SPITpTouch
{
    SPI_TP_TOUCH_OFF = 0,
    SPI_TP_TOUCH_ON
}
SPITpTouch;

// �^�b�`�p�l���̃f�[�^�L��������
typedef enum SPITpValidity
{
    SPI_TP_VALIDITY_VALID = 0,
    SPI_TP_VALIDITY_INVALID_X,
    SPI_TP_VALIDITY_INVALID_Y,
    SPI_TP_VALIDITY_INVALID_XY
}
SPITpValidity;

// �^�b�`�p�l�����͍\����
typedef union SPITpData
{
    struct
    {
        u32     x:12;
        u32     y:12;
        u32     touch:1;
        u32     validity:2;
        u32     dummy:5;

    }
    e;
    u32     raw;
    u8      bytes[4];
    u16     halfs[2];

}
SPITpData;


/*===========================================================================*/

#ifdef  __cplusplus
}       /* extern "C" */
#endif

#endif /* NITRO_SPI_COMMON_TYPE_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
