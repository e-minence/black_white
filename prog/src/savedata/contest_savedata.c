//==============================================================================
/**
 * @file	contest_savedata.c
 * @brief	�R���e�X�g�Z�[�u�f�[�^����
 * @author	matsuda
 * @date	2006.06.03(�y)
 */
//==============================================================================
#include <gflib.h>
#include "savedata/contest_savedata.h"



//==============================================================================
//	�萔��`
//==============================================================================
///�ΐ퐬�ыL�^�ő吔
#define CON_RECORD_COUNT_MAX		(9999)

#define CONTYPE_MAX					(5)
#define BREEDER_MAX					(4)

//==============================================================================
//	�\���̒�`
//==============================================================================
struct _CONTEST_DATA{
	u16	TuushinRecord[CONTYPE_MAX][BREEDER_MAX];	///<�R���e�X�g�ʐM�ΐ퐬��(CONTYPE, ����)
};



//--------------------------------------------------------------
/**
 * @brief	�J�X�^���{�[���̃T�C�Y���擾
 *
 * @param	none	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
u32	CONDATA_GetWorkSize(void)
{
	return sizeof(CONTEST_DATA);
}

//---------------------------------------------------------------------------
/**
 * @brief	�R���e�X�g�Z�[�u�f�[�^�̏���������
 * @param	condata		�R���e�X�g�Z�[�u�f�[�^�ւ̃|�C���^
 */
//---------------------------------------------------------------------------
void CONDATA_Init(void *work)
{
	CONTEST_DATA *condata = work;
	int ranking, type;
	
	for(type = 0; type < CONTYPE_MAX; type++){
		for(ranking = 0; ranking < BREEDER_MAX; ranking++){
			condata->TuushinRecord[type][ranking] = 0;
		}
	}
#if MATSU_MAKE_DEL
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_CONTEST)
	SVLD_SetCrc(GMDATA_ID_CONTEST);
#endif //CRC_LOADCHECK
#endif
}

//--------------------------------------------------------------
/**
 * @brief   �ΐ퐬�т��Z�b�g����
 *
 * @param   condata		�R���e�X�g�f�[�^�ւ̃|�C���^
 * @param   type		CONTYPE_???
 * @param   ranking		����(0origin)
 */
//--------------------------------------------------------------
void CONDATA_RecordAdd(CONTEST_DATA *condata, int type, int ranking)
{
	if(condata->TuushinRecord[type][ranking] < CON_RECORD_COUNT_MAX){
		condata->TuushinRecord[type][ranking]++;
	}
#if MATSU_MAKE_DEL
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_CONTEST)
	SVLD_SetCrc(GMDATA_ID_CONTEST);
#endif //CRC_LOADCHECK
#endif
}

//--------------------------------------------------------------
/**
 * @brief   �ΐ퐬�т��擾����
 *
 * @param   condata		�R���e�X�g�f�[�^�ւ̃|�C���^
 * @param   type		CONTYPE_???
 * @param   ranking		�擾���鏇��(0origin)
 *
 * @retval  �ΐ퐬��
 */
//--------------------------------------------------------------
u16 CONDATA_GetValue(CONTEST_DATA *condata, int type, int ranking)
{
#if MATSU_MAKE_DEL
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_CONTEST)
	SVLD_CheckCrc(GMDATA_ID_CONTEST);
#endif //CRC_LOADCHECK
#endif
	return condata->TuushinRecord[type][ranking];
}

