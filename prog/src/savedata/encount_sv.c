//============================================================================================
/**
 * @file	encount.c
 * @brief	�G���J�E���g�֘A�A�N�Z�X�p�\�[�X
 * @author	iwasawa GAME FREAK inc.
 * @date	2009.10.27
 */
//============================================================================================

#include "gflib.h"
#include "savedata/save_tbl.h"
#include "savedata/encount_sv.h"
#include "system/gfl_use.h"
#include "field/move_pokemon_def.h"
#include "field/generate_enc_def.h"

//���@�̃]�[������
typedef struct PLAYER_ZONE_HIST_tag
{
	u16 beforeZone;	//�O��]�[��
	u16 oldZone;	  //�O�X��]�[��
}PLAYER_ZONE_HIST;

//�ړ��|�P�����f�[�^(20)
typedef struct MV_POKE_DATA_tag{

	u16 zoneID;			//�]�[���h�c
  u8  seikaku;    //���i
  u8  dummy;

	u32	pow_rnd;		// �p���[����
	u32	personal_rnd;	// ������

	u16	mons_no;				// �|�P�����i���o�[
	u16	hp;				// HP

	u8	level;				// level
	u8	cond;			// ��Ԉُ�

	u8	encount_f;		// �G���J�E���g�t���O
	u8	move_type;    //�ړ����[�`���^�C�v

}MV_POKE_DATA;

typedef struct ENC_SV_DATA_tag
{
	PLAYER_ZONE_HIST	PlayerZoneHist;	//�]�[������
	MV_POKE_DATA	MovePokeData[MOVE_POKE_MAX];	//�ړ��|�P�����f�[�^
	u8 MovePokeZoneIdx[MOVE_POKE_MAX];	//�ړ��|�P�������݃]�[���C���f�b�N�X
	u8 MovePokeStatus[MOVE_POKE_MAX];	//�ړ��|�P�����ߊl��
	u8 GenerateZoneIdx;					//��ʔ��������t���O
	u8 SprayCount;						//�X�v���[�L������(�ő�250��)
	
}ENC_SV_DATA;

//----------------------------------------------------------
/**
 * @brief	�G���J�E���g�֘A�Z�[�u�f�[�^�T�C�Y�擾
 * 
 * @return	int		ENC_SV_DATA�̃T�C�Y
 */
//----------------------------------------------------------
int EncDataSave_GetWorkSize(void)
{
	return sizeof(ENC_SV_DATA);
}

//==============================================================================
/**
 * �Z�[�u�f�[�^�擪�|�C���^�擾
 *
 * @param	sv		�Z�[�u�|�C���^
 *
 * @return	ENC_SV_PTR		�g���[�i�[�J�[�h�f�[�^�|�C���^
 */
//==============================================================================
ENC_SV_PTR EncDataSave_GetSaveDataPtr(SAVE_CONTROL_WORK * sv)
{
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_ENCOUNT)
	SVLD_SetCrc(GMDATA_ID_ENCOUNT);
#endif //CRC_LOADCHECK

	return SaveControl_DataPtrGet(sv, GMDATA_ID_ENCOUNT);
}

//----------------------------------------------------------
/**
 * @brief		�G���J�E���g�֘A�Z�[�u�f�[�^�̏�����
 * 
 * @param	outEncData		�G���J�E���g�֘A�Z�[�u�f�[�^�ւ̃|�C���^
 *
 * @return	none
 */
//----------------------------------------------------------
void EncDataSave_Init(ENC_SV_PTR outEncData)
{
	MI_CpuClear8( outEncData, sizeof(ENC_SV_DATA) );

	//��ʔ��������t���O�𗎂Ƃ�
	outEncData->GenerateZoneIdx = 0;
	//�X�v���[
	outEncData->SprayCount = 0;

#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_ENCOUNT)
	SVLD_SetCrc(GMDATA_ID_ENCOUNT);
#endif //CRC_LOADCHECK

}

//==============================================================================
/**
 * ��ʔ��������_���X�V
 *
 * @param	sv				�Z�[�u�|�C���^
 *
 * @return	none
 */
//==============================================================================
void EncDataSave_UpdateGenerate( SAVE_CONTROL_WORK * sv )
{
	ENC_SV_PTR enc_data;
	enc_data = EncDataSave_GetSaveDataPtr(sv);
	enc_data->GenerateZoneIdx = GFUser_GetPublicRand0(GENERATE_ENC_POKE_MAX);

#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_ENCOUNT)
	SVLD_SetCrc(GMDATA_ID_ENCOUNT);
#endif //CRC_LOADCHECK

}

//==============================================================================
/**
 * ��ʔ����]�[��index��Ԃ�
 *
 * @param	inEncData			�G���J�E���g�֘A�Z�[�u�|�C���^
 *
 * @return	u8 0�ȊO�F��ʔ�����
 */
//==============================================================================
u8 EncDataSave_GetGenerateZoneIdx( ENC_SV_PTR inEncData )
{
	return inEncData->GenerateZoneIdx;
}

//==============================================================================
/**
 * ��l���̃]�[�������̍X�V
 *
 * @param	outEncData			�G���J�E���g�֘A�Z�[�u�|�C���^
 * @param	inzoneID			�]�[���h�c
 */
//==============================================================================
void EncDataSave_UpdatePlayerZoneHist(ENC_SV_PTR ioEncData, const int inzoneID)
{
	if (ioEncData->PlayerZoneHist.beforeZone != inzoneID){
		ioEncData->PlayerZoneHist.oldZone = ioEncData->PlayerZoneHist.beforeZone;
		ioEncData->PlayerZoneHist.beforeZone = inzoneID;
	}
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_ENCOUNT)
	SVLD_SetCrc(GMDATA_ID_ENCOUNT);
#endif //CRC_LOADCHECK
}

//==============================================================================
/**
 * ��l���̑O�X�񂢂��]�[�����擾
 *
 * @param	inEncData		�G���J�E���g�֘A�Z�[�u�|�C���^
 *
 * @return	int				�]�[���h�c
 */
//==============================================================================
int EncDataSave_GetPlayerOldZone(ENC_SV_PTR inEncData)
{
	return inEncData->PlayerZoneHist.oldZone;
}

//==============================================================================
/**
 * �w��ړ��|�P�����̕ߊl�󋵂��擾����
 *
 * @param	inEncData		�G���J�E���g�֘A�Z�[�u�|�C���^
 * @param	inTargetPoke	�ړ��|�P����(0�`2)
 *
 * @return	u8	�ߊl�X�e�[�^�X field/move_pokemon_def.h MVPOKE_STATE_XXX	
 */
//==============================================================================
u8 EncDataSave_GetMovePokeState(ENC_SV_PTR inEncData, const u8 inTargetPoke)
{
	GF_ASSERT(inTargetPoke<MOVE_POKE_MAX);
	
	return inEncData->MovePokeStatus[inTargetPoke];
}

//==============================================================================
/**
 * �w��ړ��|�P�����̕ߊl�󋵂��Z�b�g����
 *
 * @param	inEncData		�G���J�E���g�֘A�Z�[�u�|�C���^
 * @param	inTargetPoke	�ړ��|�P����(0�`2)
 * @param	status �ߊl�X�e�[�^�X field/move_pokemon_def.h MVPOKE_STATE_XXX	
 */
//==============================================================================
void EncDataSave_SetMovePokeState(ENC_SV_PTR inEncData, const u8 inTargetPoke,u8 status)
{
	GF_ASSERT(inTargetPoke<MOVE_POKE_MAX);
	
	inEncData->MovePokeStatus[inTargetPoke] = status;
}

//==============================================================================
/**
 * �w��ړ��|�P�����̃]�[���C���f�b�N�X���擾
 *
 * @param	inEncData		�G���J�E���g�֘A�Z�[�u�|�C���^
 * @param	inTargetPoke	�ړ��|�P����(0�`2)
 *
 * @return	u8				�]�[���C���f�b�N�X
 */
//==============================================================================
u8 EncDataSave_GetMovePokeZoneIdx(ENC_SV_PTR inEncData, const u8 inTargetPoke)
{
	GF_ASSERT(inTargetPoke<MOVE_POKE_MAX);
	
	return inEncData->MovePokeZoneIdx[inTargetPoke];
}

//==============================================================================
/**
 * �w��ړ��|�P�����̃]�[���C���f�b�N�X���Z�b�g
 *
 * @param	outEncData		�G���J�E���g�֘A�Z�[�u�|�C���^
 * @param	inTargetPoke	�ړ��|�P����(0�`2)
 * @param	inZoneIndex		�]�[���C���f�b�N�X
 *
 * @return	none
 */
//==============================================================================
void EncDataSave_SetMovePokeZoneIdx(ENC_SV_PTR outEncData, const u8 inTargetPoke, const u8 inZoneIndex)
{
	GF_ASSERT(inTargetPoke<MOVE_POKE_MAX);
	outEncData->MovePokeZoneIdx[inTargetPoke] = inZoneIndex;

#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_ENCOUNT)
	SVLD_SetCrc(GMDATA_ID_ENCOUNT);
#endif //CRC_LOADCHECK

}

//==============================================================================
/**
 * �w��ړ��|�P�����̃G���J�E���g�t���O�݂āA�ړ����Ă��邩�ǂ������`�F�b�N
 *
 * @param	inEncData		�G���J�E���g�֘A�Z�[�u�|�C���^
 * @param	inTargetPoke	�ړ��|�P����(0�`2)
 *
 * @return	u8			0�ȊO:�ړ����Ă�	0:�ړ����ĂȂ�
 */
//==============================================================================
u8 EncDataSave_IsMovePokeValid(ENC_SV_PTR inEncData, const u8 inTargetPoke)
{
	GF_ASSERT(inTargetPoke<MOVE_POKE_MAX);
	return inEncData->MovePokeData[inTargetPoke].encount_f;
}

//==============================================================================
/**
 * �w��ړ��|�P�����f�[�^�̃N���A
 *
 * @param	inEncData		�G���J�E���g�֘A�Z�[�u�|�C���^
 * @param	inTargetPoke	�ړ��|�P����(0�`2)
 *
 * @return	none
 */
//==============================================================================
void EncDataSave_ClearMovePokeData(MPD_PTR *outMPData)
{
	MI_CpuClear8((*outMPData), sizeof(MV_POKE_DATA));

#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_ENCOUNT)
	SVLD_SetCrc(GMDATA_ID_ENCOUNT);
#endif //CRC_LOADCHECK

}

//==============================================================================
/**
 * �w��ړ��|�P�����f�[�^�|�C���^�̎擾
 *
 * @param	inEncData		�G���J�E���g�֘A�Z�[�u�|�C���^
 * @param	inTargetPoke	�ړ��|�P����(0�`2)
 *
 * @return	MPD_PTR			�ړ��|�P�����f�[�^
 */
//==============================================================================
MPD_PTR	EncDataSave_GetMovePokeDataPtr(ENC_SV_PTR inEncData, const u8 inTargetPoke)
{
	GF_ASSERT(inTargetPoke<MOVE_POKE_MAX);
	return &(inEncData->MovePokeData[inTargetPoke]);
}

//==============================================================================
/**
 * �ړ��|�P�����f�[�^�p�����[�^�擾
 *
 * @param	inMPData		�ړ����|�P�����f�[�^�|�C���^
 * @param	inParamID		�p�����[�^�h�c
 *
 * @return	u32
 */
//==============================================================================
u32 EncDataSave_GetMovePokeDataParam(const MPD_PTR inMPData, const u8 inParamID)
{
	u32 val;
	switch(inParamID){
	case MP_PARAM_ZONE_ID:			//�o���]�[��
		val = inMPData->zoneID;
		break;
	case MP_PARAM_POW_RND:			//�p���[����
		val = inMPData->pow_rnd;
		break;
	case MP_PARAM_PER_RND:			//������
		val = inMPData->personal_rnd;
		break;
	case MP_PARAM_SEIKAKU:			//���i
		val = inMPData->seikaku;
		break;
	case MP_PARAM_MONSNO:			//�����X�^�[�i���o�[
		val = inMPData->mons_no;
		break;
	case MP_PARAM_HP:				//�g�o
		val = inMPData->hp;
		break;
	case MP_PARAM_LV:				//�k�u
		val = inMPData->level;
		break;
	case MP_PARAM_COND:				//��Ԉُ�
		val = inMPData->cond;
		break;
	case MP_PARAM_ENC:				//�G���J�E���g�t���O
		val = inMPData->encount_f;
		break;
  case MP_PARAM_MV_TYPE:		//�ړ��^�C�v
		val = inMPData->move_type;
		break;
	default:
		GF_ASSERT(0);
		return 0;
	}
	return val;
}

//==============================================================================
/**
 * �ړ��|�P�����f�[�^�p�����[�^�Z�b�g
 *
 * @param	EncData		�G���J�E���g�֘A�Z�[�u�|�C���^
 * @param	inParamID	�p�����[�^�h�c
 * @param	inVal		�Z�b�g����l
 *
 * @return	none
 */
//==============================================================================
void EncDataSave_SetMovePokeDataParam(MPD_PTR outMPData, const u8 inParamID, const u32 inVal)
{
	u32 val;
	switch(inParamID){
	case MP_PARAM_ZONE_ID:			//�o���]�[��
		outMPData->zoneID = (u16)inVal;
		break;
	case MP_PARAM_POW_RND:			//�p���[����
		outMPData->pow_rnd = inVal;
		break;
	case MP_PARAM_PER_RND:			//������
		outMPData->personal_rnd = inVal;
		break;
	case MP_PARAM_SEIKAKU:			//���i
		outMPData->seikaku = inVal;
		break;
	case MP_PARAM_MONSNO:			//�����X�^�[�i���o�[
		outMPData->mons_no = inVal;
		break;
	case MP_PARAM_HP:				//�g�o
		outMPData->hp = inVal;
		break;
	case MP_PARAM_LV:				//�k�u
		outMPData->level = inVal;
		break;
	case MP_PARAM_COND:				//��Ԉُ�
		outMPData->cond = inVal;
		break;
	case MP_PARAM_ENC:				//�G���J�E���g�t���O
		outMPData->encount_f = inVal;
		break;
  case MP_PARAM_MV_TYPE:		//�ړ��^�C�v
		outMPData->move_type = inVal;
		break;
	default:
		GF_ASSERT(0);
	}

#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_ENCOUNT)
	SVLD_SetCrc(GMDATA_ID_ENCOUNT);
#endif //CRC_LOADCHECK

}


//==============================================================================
/**
 * �X�v���[�L���������Z�b�g����
 *
 * @param	inEncData		�G���J�E���g�֘A�Z�[�u�|�C���^
 */
//==============================================================================
void EncDataSave_SetSprayCnt(ENC_SV_PTR inEncData,u8 count)
{
	inEncData->SprayCount = count;
}

//==============================================================================
/**
 * �X�v���[�L���������P�����炷
 *
 * @param	inEncData		�G���J�E���g�֘A�Z�[�u�|�C���^
 *
 * @return	BOOL				�X�v���[���ʂ��؂ꂽ���ǂ���
 */
//==============================================================================
BOOL EncDataSave_DecSprayCnt(ENC_SV_PTR inEncData)
{
  if(inEncData->SprayCount > 0){
    inEncData->SprayCount--;
    if(inEncData->SprayCount == 0){
      return TRUE;
    }
  }
	return FALSE;
}

//==============================================================================
/**
 * �X�v���[���g�p�ł��邩�ǂ����̃`�F�b�N
 * 
 * @param	inEncData		�G���J�E���g�֘A�Z�[�u�|�C���^
 *
 * @return	BOOL	TRUE:�g�p��		FALSE:�g�p�s��
 */
//==============================================================================
BOOL EncDataSave_CanUseSpray(ENC_SV_PTR inEncData)
{
	if (!inEncData->SprayCount){
		return TRUE;
	}else{
		return FALSE;
	}
}

