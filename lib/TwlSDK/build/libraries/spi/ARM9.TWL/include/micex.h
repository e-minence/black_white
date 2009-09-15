/*---------------------------------------------------------------------------*
  Project:  TwlSDK - libraries - mic
  File:     micex.h

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 8573 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/
#ifndef TWL_LIBRARIES_SPI_ARM9_MICEX_H_
#define TWL_LIBRARIES_SPI_ARM9_MICEX_H_
#ifdef  __cplusplus
extern  "C" {
#endif
/*---------------------------------------------------------------------------*/

#include    <nitro/spi/ARM9/mic.h>

/*---------------------------------------------------------------------------*
    �\���̒�`
 *---------------------------------------------------------------------------*/
/* �񓯊��֐��r�������p���b�N��` */
typedef enum MICLock
{
    MIC_LOCK_OFF = 0,                  // ���b�N�J�����
    MIC_LOCK_ON,                       // ���b�N�{�����
    MIC_LOCK_MAX
}
MICLock;

/* ���[�N�p�\���� */
typedef struct MICWork
{
    MICLock lock;                      // �r�����b�N
    MICCallback callback;              // �񓯊��֐��R�[���o�b�N�ޔ�p
    void   *callbackArg;               // �R�[���o�b�N�֐��̈����ۑ��p
    MICResult commonResult;            // �񓯊��֐��̏������ʑޔ�p
    MICCallback full;                  // �T���v�����O�����R�[���o�b�N�ޔ�p
    void   *fullArg;                   // �����R�[���o�b�N�֐��̈����ۑ��p
    void   *dst_buf;                   // �P�̃T���v�����O���ʊi�[�G���A�ޔ�p

}
MICWork;

/*---------------------------------------------------------------------------*
    �֐���`
 *---------------------------------------------------------------------------*/
MICWork*    MICi_GetSysWork(void);

MICResult   MICEXi_StartLimitedSampling(const MICAutoParam* param);
MICResult   MICEXi_StartLimitedSamplingAsync(const MICAutoParam* param, MICCallback callback, void* arg);
MICResult   MICEXi_StopLimitedSampling(void);
MICResult   MICEXi_StopLimitedSamplingAsync(MICCallback callback, void* arg);
MICResult   MICEXi_AdjustLimitedSampling(u32 rate);
MICResult   MICEXi_AdjustLimitedSamplingAsync(u32 rate, MICCallback callback, void* arg);


/*---------------------------------------------------------------------------*/
#ifdef __cplusplus
}   /* extern "C" */
#endif
#endif  /* TWL_LIBRARIES_SPI_ARM9_MICEX_H_ */
