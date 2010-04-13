//==============================================================================
/**
 * @file	battle_rec_ov.c
 * @brief	�I�[�o�[���Cbattle_recorder�ł̂ݎg�p����^��֘A�c�[����
 * @author	matsuda
 * @date	2008.05.15(��)
 *
 * �{���Ȃ�battle_rec.c�Ɋ܂߂���̂����A�풓�������̊֌W��A�o�g�����R�[�_�[�ł���
 * �g�p���Ȃ��悤�Ȃ��̂́A�����Ɉړ�
 */
//==============================================================================
#include <gflib.h>
#include "savedata\save_control.h"
#include "savedata\mystatus_local.h"
#include "savedata\config.h"

#include "gds_local_common.h"
#include "savedata\battle_rec.h"
#include "waza_tool/wazano_def.h"
#include "net_app/gds/gds_profile_local.h"
#include "savedata/gds_profile.h"

#include "pm_define.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/poke_tool_def.h"
#include "savedata/gds_profile.h"
#include "net_app/gds/gds_profile_local.h"
#include "battle/btl_common.h"
#include "savedata\battle_rec.h"
#include "savedata\battle_rec_local.h"

#include "savedata/misc.h"



//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
//static void ErrorNameSet(STRBUF *dest_str, int heap_id);

//==============================================================================
//	
//==============================================================================
extern BATTLE_REC_SAVEDATA * brs;


//==============================================================================
//	
//==============================================================================

//--------------------------------------------------------------
/**
 * @brief   GDS�Ŏ�M�����f�[�^���Z�[�u����
 *
 * @param   sv				�Z�[�u�f�[�^�ւ̃|�C���^
 * @param   recv_data		BATTLE_REC_RECV�\����
 * @param   num				LOADDATA_MYREC�ALOADDATA_DOWNLOAD0�ALOADDATA_DOWNLOAD1�c
 * @param   secure		�����ς݃t���O(TRUE:�����ς݂ɂ��ăZ�[�u����B
 * 						FALSE:�t���O���삵�Ȃ�(�T�[�o�[��secure�𗧂ĂĂ���Ȃ痧�����܂܂ɂȂ�)
 * @param   work0		�Z�[�u�i�s�𐧌䂷�郏�[�N�ւ̃|�C���^(�ŏ���0�N���A������ԂŌĂ�ŉ�����)
 * @param   work1		�Z�[�u�i�s�𐧌䂷�郏�[�N�ւ̃|�C���^(�ŏ���0�N���A������ԂŌĂ�ŉ�����)
 *
 * @retval  SAVE_RESULT_CONTINUE	�Z�[�u�����p����
 * @retval  SAVE_RESULT_LAST		�Z�[�u�����p�����A�Ō�̈�O
 * @retval  SAVE_RESULT_OK			�Z�[�u����I��
 * @retval  SAVE_RESULT_NG			�Z�[�u���s�I��
 *
 * GDS�Ŏ�M�����f�[�^��brs�ɃR�s�[�������_(BattleRec_DataSet)�ŕ�������Ă���̂ŁA
 * �����ňÍ������Ă���Z�[�u���s��
 */
//--------------------------------------------------------------
SAVE_RESULT BattleRec_GDS_RecvData_Save(GAMEDATA *gamedata, int num, u8 secure, u16 *work0, u16 *work1, HEAPID heap_id)
{
	SAVE_RESULT result;
	
	switch(*work0){
	case 0:
		GF_ASSERT(brs);
		
		if(secure == TRUE){
			brs->head.secure = secure;
			//secure�t���O���X�V���ꂽ�̂ŁA�ēxCRC����蒼��
			brs->head.magic_key = REC_OCC_MAGIC_KEY;
			brs->head.crc.crc16ccitt_hash = GFL_STD_CrcCalc(&brs->head, 
				sizeof(BATTLE_REC_HEADER) - GDS_CRC_SIZE - DATANUMBER_SIZE);
		}
		
		//CRC���L�[�ɂ��ĈÍ���
		//��GDS�Ŏ�M�����f�[�^��brs�ɃR�s�[�������_(BattleRec_DataSet)�ŕ�������Ă���̂ŁB
		BattleRec_Coded(&brs->rec, sizeof(BATTLE_REC_WORK) - GDS_CRC_SIZE, 
			brs->rec.crc.crc16ccitt_hash + ((brs->rec.crc.crc16ccitt_hash ^ 0xffff) << 16));
		(*work0)++;
		break;
	case 1:
		result = Local_BattleRecSave(gamedata, brs, num, work1, heap_id);
		return result;
	}
	return SAVE_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   GDS�ő��M����ׂɁA�f�[�^�ɕϊ��������s��
 *
 * @param   sv		�Z�[�u�f�[�^�ւ̃|�C���^
 *
 * ���̊֐��g�p���brs�́A���̂܂܍Đ�������ۑ����Ă͂����Ȃ��`�ɂȂ��Ă���̂ŁA
 * GDS�ő��M���I�������j�����邱�ƁI
 * �A���ABattleRec_GDS_MySendData_DataNumberSetSave�֐����g�p����ꍇ��OK�B
 */
//--------------------------------------------------------------
void BattleRec_GDS_SendData_Conv(SAVE_CONTROL_WORK *sv)
{
	GF_ASSERT(brs);
	
	//��brs�ɓW�J����Ă���f�[�^�́A�{�̂�����������Ă���̂ő��M�O�ɍēx�Í������s��
	BattleRec_Coded(&brs->rec, sizeof(BATTLE_REC_WORK) - GDS_CRC_SIZE, 
		brs->rec.crc.crc16ccitt_hash + ((brs->rec.crc.crc16ccitt_hash ^ 0xffff) << 16));
}

//--------------------------------------------------------------
/**
 * @brief   GDS�ő��M����brs�ɑ΂��āA�f�[�^�i���o�[���Z�b�g���Z�[�u���s��
 *
 * @param   sv				�Z�[�u�f�[�^�ւ̃|�C���^
 * @param   data_number		�f�[�^�i���o�[
 * @param   work		�Z�[�u�i�s�𐧌䂷�郏�[�N�ւ̃|�C���^(�ŏ���0�N���A������ԂŌĂ�ŉ�����)
 * @param   work		�Z�[�u�i�s�𐧌䂷�郏�[�N�ւ̃|�C���^(�ŏ���0�N���A������ԂŌĂ�ŉ�����)
 * 
 * @retval	SAVE_RESULT_OK		�Z�[�u����
 * @retval	SAVE_RESULT_NG		�Z�[�u���s
 */
//--------------------------------------------------------------
SAVE_RESULT BattleRec_GDS_MySendData_DataNumberSetSave(GAMEDATA *gamedata, u64 data_number, u16 *work0, u16 *work1, HEAPID heap_id)
{
	SAVE_RESULT result;

	switch(*work0){
	case 0:
		GF_ASSERT(brs);
		
		//�f�[�^�i���o�[�͌��XCRC�̑Ώۂ���͂���Ă���̂ŁACRC���č쐬����K�v�͂Ȃ�
		brs->head.data_number = data_number;
		(*work0)++;
		break;
	case 1:
		//���M���ɍs���Ă���BattleRec_GDS_SendData_Conv�֐��Ŋ��ɈÍ������Ă���͂��Ȃ̂�
		//���̂܂܃Z�[�u���s��
		result = Local_BattleRecSave(gamedata, brs, LOADDATA_MYREC, work1, heap_id);
		return result;
	}
	return SAVE_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   �o�g�����R�[�_�[(�I�t���C��)�Ńr�f�I��������Abrs�ɓǂݍ���ł���o�g���r�f�I��
 * 			�����ς݃t���O���Z�b�g���ăZ�[�u����
 * 			���ʏ�Z�[�u����
 *
 * @param   sv		�Z�[�u�f�[�^�ւ̃|�C���^
 * @param	num		�Z�[�u����f�[�^�i���o�[�iLOADDATA_MYREC�ALOADDATA_DOWNLOAD0�ALOADDATA_DOWNLOAD1�c�j
 * @param   work		�Z�[�u�i�s�𐧌䂷�郏�[�N�ւ̃|�C���^(�ŏ���0�N���A������ԂŌĂ�ŉ�����)
 * @param   work		�Z�[�u�i�s�𐧌䂷�郏�[�N�ւ̃|�C���^(�ŏ���0�N���A������ԂŌĂ�ŉ�����)
 *
 * @retval  SAVE_RESULT_???
 *
 * �����Ŗ{�̂̈Í����������s���ׁA���̊֐��Ȍ��brs�͉�����Ă��������B
 */
//--------------------------------------------------------------
SAVE_RESULT BattleRec_SecureSetSave(GAMEDATA *gamedata, int num, u16 *work0, u16 *work1, HEAPID heap_id)
{
	SAVE_RESULT result;

  switch(*work0){
  case 0:
  	GF_ASSERT(brs != NULL);
  	
  	brs->head.secure = TRUE;
  	//secure�t���O���X�V���ꂽ�̂ŁA�ēxCRC����蒼��
  	brs->head.magic_key = REC_OCC_MAGIC_KEY;
  	brs->head.crc.crc16ccitt_hash = GFL_STD_CrcCalc(&brs->head, 
  		sizeof(BATTLE_REC_HEADER) - GDS_CRC_SIZE - DATANUMBER_SIZE);

  	//CRC���L�[�ɂ��ĈÍ���
  	BattleRec_Coded(&brs->rec, sizeof(BATTLE_REC_WORK) - GDS_CRC_SIZE, 
  		brs->rec.crc.crc16ccitt_hash + ((brs->rec.crc.crc16ccitt_hash ^ 0xffff) << 16));
    (*work0)++;
    break;
  case 1:
	  result = Local_BattleRecSave(gamedata, brs, num, work1, heap_id);
	  return result;
	}
	return SAVE_RESULT_CONTINUE;
}



#ifdef PM_DEBUG
//--------------------------------------------------------------
/**
 * @brief   �ǂݍ��ݍς݂�brs�ɑ΂���secure�t���O���Z�b�g����
 *
 * @param   sv		
 */
//--------------------------------------------------------------
void DEBUG_BattleRec_SecureFlagSet(SAVE_CONTROL_WORK *sv)
{
	GF_ASSERT(brs != NULL);
	
	brs->head.secure = TRUE;
	//secure�t���O���X�V���ꂽ�̂ŁA�ēxCRC����蒼��
	brs->head.magic_key = REC_OCC_MAGIC_KEY;
	brs->head.crc.crc16ccitt_hash = GFL_STD_CrcCalc(&brs->head, 
		sizeof(BATTLE_REC_HEADER) - GDS_CRC_SIZE - DATANUMBER_SIZE);
}
#endif	//PM_DEBUG
