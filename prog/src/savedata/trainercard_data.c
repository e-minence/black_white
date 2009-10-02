//=============================================================================
/**
 * @file	trainercard_data.c
 * @bfief	�g���[�i�[�J�[�h�p�Z�[�u�f�[�^�A�N�Z�X�֘A
 * @author	Nozomu Saito
 *
 *
 */
//=============================================================================
#include <gflib.h>
#include "savedata/save_tbl.h"
#include "savedata/trainercard_data.h"

#define BADGE_MAX	(8)
#define SIGHN_W	(24)
#define SIGHN_H	(8)

#define DEFAULT_BADGE_SCRUCH	(140)

typedef struct TR_CARD_SV_DATA_tag
{
	u8 SignData[SIGHN_W*SIGHN_H*64/8];	//�T�C���ʃf�[�^
}TR_CARD_SV_DATA;

//==============================================================================
/**
 * �Z�[�u�f�[�^�T�C�Y�擾
 *
 * @param	none
 *
 * @return	int		�T�C�Y
 */
//==============================================================================
int TRCSave_GetSaveDataSize(void)
{
	return sizeof(TR_CARD_SV_DATA);
}

//==============================================================================
/**
 * �Z�[�u�f�[�^������
 *
 * @param	outTrCard	�g���[�i�[�J�[�h�f�[�^�|�C���^
 *
 * @return	none
 */
//==============================================================================
void TRCSave_InitSaveData(TR_CARD_SV_PTR outTrCard)
{
	GFL_STD_MemFill( outTrCard, 0, sizeof(TR_CARD_SV_DATA) );

#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TRCARD)
	SVLD_SetCrc(GMDATA_ID_TRCARD);
#endif //CRC_LOADCHECK
}

//==============================================================================
/**
 * �Z�[�u�f�[�^�擪�|�C���^�擾
 *
 * @param	sv		�Z�[�u�|�C���^
 *
 * @return	TR_CARD_SV_PTR		�g���[�i�[�J�[�h�f�[�^�|�C���^
 */
//==============================================================================
TR_CARD_SV_PTR TRCSave_GetSaveDataPtr(SAVE_CONTROL_WORK * sv)
{
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TRCARD)
	SVLD_CheckCrc(GMDATA_ID_TRCARD);
#endif //CRC_LOADCHECK

	return SaveControl_DataPtrGet(sv, GMDATA_ID_TRCARD);
}



//==============================================================================
/**
 * �T�C���f�[�^�擪�|�C���^�擾
 *
 * @param	inTrCard		�g���[�i�[�J�[�h�f�[�^�|�C���^
 *
 * @return	u8*				�T�C���f�[�^�|�C���^
 */
//==============================================================================
u8 *TRCSave_GetSignDataPtr(TR_CARD_SV_PTR inTrCard)
{
	return inTrCard->SignData;
}

//==============================================================================
/**
 * @brief   �T�C�����������܂�Ă��邩�m�F����
 *
 * @param   inTrCard		
 *
 * @retval  int		TRUE�Ȃ珑�����܂�Ă���BFALSE�Ȃ�S�ċ�
 */
//==============================================================================
int TRCSave_GetSigned( TR_CARD_SV_PTR inTrCard )
{
	int search=0;
	int i;
	u32 *ptr = 	(u32*)inTrCard->SignData;	//�T�C���ʃf�[�^

	
	for(i=0;i<SIGHN_W*SIGHN_H*64/(8*4);i++){
		search += ptr[i];
	}

	if(search){
		return TRUE;
	}
	
	return FALSE;
}
