//=============================================================================
/**
 * @file  trainerbadge_data.h
 * @bfief �g���[�i�[�J�[�h�p�Z�[�u�f�[�^�A�N�Z�X�֘A
 * @author  Akito Mori
 *
 *
 */
//=============================================================================
#ifndef __TRAINERCARD_DATA_H__
#define __TRAINERCARD_DATA_H__

// �g���[�i�[�J�[�h�Őݒ�ł���u���i�v�̍ő�l
#define TRCARD_PERSONARITY_MAX    ( 8 )

#include "savedata/save_control.h"
#include "system/pms_data.h"

typedef struct TR_CARD_SV_DATA_tag * TR_CARD_SV_PTR;

extern int TRCSave_GetSaveDataSize(void);
extern void TRCSave_InitSaveData(TR_CARD_SV_PTR outTrCard);

// �g���[�i�J�[�h�f�[�^�擾
extern TR_CARD_SV_PTR TRCSave_GetSaveDataPtr(SAVE_CONTROL_WORK * sv);

// �T�C���f�[�^����
extern u8 *TRCSave_GetSignDataPtr(TR_CARD_SV_PTR inTrCard);
extern int TRCSave_GetSigned( TR_CARD_SV_PTR inTrCard );
// ���j�I�������ڂ�ύX���Ă��邩�H
extern void TRCSave_SetTrainerViewChange( TR_CARD_SV_PTR inTrCard );
extern  int TRCSave_GetTrainerViewChange( TR_CARD_SV_PTR outTrCard );
// ���i�i�g���[�i�[�J�[�h�ŕ\�������j
extern void TRCSave_SetPersonarity( TR_CARD_SV_PTR inTrCard, int personarity );
extern  int TRCSave_GetPersonarity( TR_CARD_SV_PTR outTrCard );
// �T�C���A�j��
extern void TRCSave_SetSignAnime( TR_CARD_SV_PTR inTrCard, int flag );
extern  int TRCSave_GetSignAnime( TR_CARD_SV_PTR outTrCard );



#endif //__TRAINERCARD_DATA_H__

