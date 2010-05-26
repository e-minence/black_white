//==============================================================================
/**
 * @file	gds_rap_response.c
 * @brief	gds_rap.c�����M�f�[�^�̔����擾������Ɨ�����������
 * @author	matsuda
 * @date	2008.01.13(��)
 */
//==============================================================================
#include <gflib.h>
#include <dwc.h>
#include "net/dwc_rap.h"
#include "savedata\save_control.h"
#include "savedata/gds_profile.h"
#include "net_app/gds/gds_profile_local.h"
#include "battle/btl_common.h"

#include "savedata/gds_profile.h"

#include <arc_tool.h>
#include "print/wordset.h"
#include "message.naix"

#include "gds_rap.h"
#include "gds_rap_response.h"

#include "msg/msg_wifi_system.h"



//--------------------------------------------------------------
/**
 * @brief   ���X�|���X�f�[�^���߁F�h���X�A�b�v�V���b�g �A�b�v���[�h
 *
 * @param   gdsrap		
 * @param   res		
 *
 * @retval  TRUE:����
 * @retval  FALSE:���炩�̃G���[������(���e��res->Result�Q��)
 */
//--------------------------------------------------------------
int GDS_RAP_RESPONSE_MusicalShot_Upload(GDS_RAP_WORK *gdsrap, POKE_NET_RESPONSE *res)
{
	POKE_NET_GDS_RESPONSE_MUSICALSHOT_REGIST *param;
	int ret = FALSE;
	
	switch(res->Result){
	//!< �o�^����
	case POKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_REGIST_SUCCESS:
		param = (POKE_NET_GDS_RESPONSE_MUSICALSHOT_REGIST *)(res->Param);
		OS_TPrintf("�~���[�W�J���V���b�g�A�b�v���[�h�����B�o�^�R�[�h = %d\n", param->Code);
		ret = TRUE;
		break;
	//!< ���[�U�[�F�؃G���[
	case POKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_REGIST_ERROR_AUTH:
		OS_TPrintf("�~���[�W�J���V���b�g�o�^�G���[�I:���[�U�[�F�؃G���[\n");
		break;
	//!< ���łɓo�^����Ă���
	case POKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_REGIST_ERROR_ALREADY:
		OS_TPrintf("�~���[�W�J���V���b�g�o�^�G���[�I:���ɓo�^����Ă���\n");
		break;
	//!< �s���ȃf�[�^
	case POKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_REGIST_ERROR_ILLEGAL:
		OS_TPrintf("�~���[�W�J���V���b�g�o�^�G���[�I:�s���f�[�^\n");
		break;
	//!< �s���ȃ|�P�����ԍ�
  case POKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_REGIST_ERROR_ILLEGALPOKEMON:
		OS_TPrintf("�~���[�W�J���V���b�g�o�^�G���[�I:�s���ȃ|�P�����ԍ�\n");
		break;
	//!< �s���ȃ��[�U�[�v���t�B�[��
	case POKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_REGIST_ERROR_ILLEGALPROFILE:
		OS_TPrintf("�~���[�W�J���V���b�g�o�^�G���[�I:�s���v���t�B�[��\n");
		break;
	//!< ���̑��G���[
	case POKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_REGIST_ERROR_UNKNOWN:
	default:
		OS_TPrintf("�~���[�W�J���V���b�g�o�^�G���[�I:���̑��̃G���[\n");
		break;
	}
	
	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   ���X�|���X�f�[�^���߁F�~���[�W�J���V���b�g �_�E�����[�h
 *
 * @param   gdsrap		
 * @param   res		
 *
 * @retval  TRUE:����
 * @retval  FALSE:���炩�̃G���[������(���e��res->Result�Q��)
 */
//--------------------------------------------------------------
int GDS_RAP_RESPONSE_MusicalShot_Download(GDS_RAP_WORK *gdsrap, POKE_NET_RESPONSE *res)
{
	POKE_NET_GDS_RESPONSE_MUSICALSHOT_GET *param;
	int ret = FALSE;
	
	switch(res->Result){
	//!< �擾����
	case POKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_GET_SUCCESS:
		param = (POKE_NET_GDS_RESPONSE_MUSICALSHOT_GET *)(res->Param);
		OS_TPrintf("�~���[�W�J���V���b�g��M�����B�q�b�g���� = %d\n", param->HitCount);
		ret = TRUE;
		break;
	//!< ���[�U�[�F�؃G���[
	case POKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_GET_ERROR_AUTH:
		OS_TPrintf("�~���[�W�J���V���b�g��M�G���[�I:���[�U�[�F�؃G���[\n");
		break;
	//!< �|�P�����ԍ��G���[
	case POKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_GET_ERROR_ILLEGALPOKEMON:
		OS_TPrintf("�~���[�W�J���V���b�g��M�G���[�I:�|�P�����ԍ��G���[\n");
		break;
	//!< ���̑��G���[
	case POKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_GET_ERROR_UNKNOWN:
	default:
		OS_TPrintf("�~���[�W�J���V���b�g��M�G���[�I:���̑��̃G���[\n");
		break;
	}
	
	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   ��M�o�b�t�@����f�[�^�������Ă��镔���̃|�C���^���擾�F�~���[�W�J���V���b�g
 *
 * @param   gdsrap		
 * @param   mus_array		�|�C���^���Z�b�g����z��f�[�^
 * @param   array_max		mus_array�̔z��v�f��
 *
 * @retval  ��M����
 *
 * mus_array[0] �` mus_array[x] �܂Ŏ�M�f�[�^�ւ̃|�C���^���Z�b�g����܂�
 */
//--------------------------------------------------------------
int GDS_RAP_RESPONSE_MusicalShot_Download_RecvPtr_Set(GDS_RAP_WORK *gdsrap, MUSICAL_SHOT_RECV **mus_array, int array_max)
{
	POKE_NET_RESPONSE *res;
	POKE_NET_GDS_RESPONSE_MUSICALSHOT_GET *param;
	POKE_NET_GDS_RESPONSE_MUSICALSHOT_RECVDATA *mus_recv;
	int i, hit_count;
	
	res = POKE_NET_GDS_GetResponse();
	param = (POKE_NET_GDS_RESPONSE_MUSICALSHOT_GET *)(res->Param);
	
	hit_count = param->HitCount;
	if(hit_count > array_max){
		hit_count = array_max;
	//	GF_ASSERT(0);	//�p�ӂ��Ă���o�b�t�@������M����������
	}

	mus_recv = (POKE_NET_GDS_RESPONSE_MUSICALSHOT_RECVDATA *)(param->Data);
	for(i = 0; i < hit_count; i++){
		mus_array[i] = &mus_recv[i].Data;
	}
	for( ; i < array_max; i++){
		mus_array[i] = NULL;
	}

	return hit_count;
}

//--------------------------------------------------------------
/**
 * @brief   ���X�|���X�f�[�^���߁F�o�g���r�f�I�o�^
 *
 * @param   gdsrap		
 * @param   res		
 *
 * @retval  TRUE:����
 * @retval  FALSE:���炩�̃G���[������(���e��res->Result�Q��)
 */
//--------------------------------------------------------------
int GDS_RAP_RESPONSE_BattleVideo_Upload(GDS_RAP_WORK *gdsrap, POKE_NET_RESPONSE *res)
{
	POKE_NET_GDS_RESPONSE_BATTLEDATA_REGIST *param;
	int ret = FALSE;
	
	switch(res->Result){
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_SUCCESS:		//!< �o�^����
		param = (POKE_NET_GDS_RESPONSE_BATTLEDATA_REGIST *)(res->Param);
		OS_TPrintf("�o�g���r�f�I�o�^��M�����F�o�^�R�[�h%d\n", param->Code);
		ret = TRUE;
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_AUTH:		//!< ���[�U�[�F�؃G���[
		OS_TPrintf("�o�g���r�f�I�o�^��M�G���[�I:���[�U�[�F�؃G���[\n");
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ALREADY:	//!< ���łɓo�^����Ă���
		OS_TPrintf("�o�g���r�f�I�o�^��M�G���[�I:���ɓo�^����Ă���\n");
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ILLEGAL:	//!< �s���ȃf�[�^
		OS_TPrintf("�o�g���r�f�I�o�^��M�G���[�I:�s���f�[�^\n");
		break;
  case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ILLEGAL_RANKINGTYPE:  //!< �s���ȃ����L���O���
    OS_TPrintf("�o�g���r�f�I�o�^��M�G���[�I:�s���ȃ����L���O���\n");
    break;
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ILLEGALPROFILE:	//!< �s���ȃ��[�U�[�v���t�B�[��
		OS_TPrintf("�o�g���r�f�I�o�^��M�G���[�I:�s���ȃ��[�U�[\n");
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ILLEGALPOKEMON_VERIFY: //!< �|�P���������ŃG���[
		OS_TPrintf("�o�g���r�f�I�o�^��M�G���[�I:�|�P���������ŃG���[\n");
	  break;
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_UNKNOWN:	//!< ���̑��G���[
	default:
		OS_TPrintf("�o�g���r�f�I�o�^��M�G���[�I:���̑��̃G���[ %d\n", res->Result);
		break;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   ��M�o�b�t�@����f�[�^�擾�F�o�g���r�f�I�o�^
 *
 * @param   gdsrap		
 * 
 * @retval	�o�^�R�[�h
 */
//--------------------------------------------------------------
u64 GDS_RAP_RESPONSE_BattleVideo_Upload_DataGet(GDS_RAP_WORK *gdsrap)
{
	POKE_NET_RESPONSE *res;
	POKE_NET_GDS_RESPONSE_BATTLEDATA_REGIST *param;
	
	res = POKE_NET_GDS_GetResponse();
	param = (POKE_NET_GDS_RESPONSE_BATTLEDATA_REGIST *)(res->Param);
	return param->Code;
}
//--------------------------------------------------------------
/**
 * @brief   ���X�|���X�f�[�^���߁F�o�g���r�f�I����
 *
 * @param   gdsrap		
 * @param   res		
 *
 * @retval  TRUE:����
 * @retval  FALSE:���炩�̃G���[������(���e��res->Result�Q��)
 */
//--------------------------------------------------------------
int GDS_RAP_RESPONSE_BattleVideo_Search_Download(GDS_RAP_WORK *gdsrap, POKE_NET_RESPONSE *res)
{
	POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH *param;
	int ret = FALSE;
	
	switch(res->Result){
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_SUCCESS:		//!< �擾����
		param = (POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH *)(res->Param);
		OS_TPrintf("�o�g���r�f�I������M�����F�q�b�g����=%d\n", param->HitCount);
		ret = TRUE;
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_ERROR_AUTH:		//!< ���[�U�[�F�؃G���[
		OS_TPrintf("�o�g���r�f�I������M�G���[�I:���[�U�[�F�؃G���[\n");
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_ERROR_ILLEGALPARAM:	//!< �����p�����[�^�G���[
		OS_TPrintf("�o�g���r�f�I������M�G���[�I:�����p�����[�^�G���[\n");
		break;
  case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_ERROR_ILLEGALRANKINGTYPE: //!< �����p�����[�^�G���[(�����L���O��ʂ��s��)
		OS_TPrintf("�o�g���r�f�I������M�G���[�I:�����L���O��ʃG���[\n");
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_ERROR_UNKNOWN:	//!< ���̑��G���[
	default:
		OS_TPrintf("�o�g���r�f�I������M�G���[�I:���̑��̃G���[\n");
		break;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   ��M�o�b�t�@����f�[�^�������Ă��镔���̃|�C���^���擾�F�o�g���r�f�I����
 *
 * @param   gdsrap		
 * @param   outline_array	�|�C���^���Z�b�g����z��f�[�^
 * @param   array_max		outline_array�̔z��v�f��
 *
 * @retval  ��M����
 *
 * outline_array[0] �` outline_array[x] �܂Ŏ�M�f�[�^�ւ̃|�C���^���Z�b�g����܂�
 */
//--------------------------------------------------------------
int GDS_RAP_RESPONSE_BattleVideoSearch_Download_RecvPtr_Set(GDS_RAP_WORK *gdsrap, BATTLE_REC_OUTLINE_RECV **outline_array, int array_max)
{
	POKE_NET_RESPONSE *res;
	POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH *param;
	POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH_RECVDATA *outline_recv;
	int i, hit_count;
	
	res = POKE_NET_GDS_GetResponse();
	param = (POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH *)(res->Param);
	
	hit_count = param->HitCount;
	if(hit_count > array_max){
		hit_count = array_max;
		GF_ASSERT(0);	//�p�ӂ��Ă���o�b�t�@������M����������
	}

	outline_recv = (POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH_RECVDATA *)(param->Data);
	for(i = 0; i < hit_count; i++){
		outline_array[i] = &outline_recv[i].Data;
		if(outline_recv[i].Code != outline_array[i]->head.data_number){
			//�w�b�_�ƃf�[�^�i���o�[���Ⴄ�ꍇ�̓w�b�_�Ƀi���o�[�������ĂȂ��̂ł������œ����
			outline_array[i]->head.data_number = outline_recv[i].Code;
			//CRC�쐬
			outline_array[i]->head.crc.crc16ccitt_hash = GFL_STD_CrcCalc(
				&outline_array[i]->head, 
				sizeof(BATTLE_REC_HEADER) - GDS_CRC_SIZE - sizeof(outline_recv[i].Code));
		}
	}
	for( ; i < array_max; i++){
		outline_array[i] = NULL;
	}
	
	return hit_count;
}

//--------------------------------------------------------------
/**
 * @brief   ��M�o�b�t�@����f�[�^���R�s�[���܂��F�o�g���r�f�I����
 *
 * @param   gdsrap		
 * @param   outline_array	�|�C���^���Z�b�g����z��f�[�^
 * @param   array_max		outline_array�̔z��v�f��
 *
 * @retval  ��M����
 *
 * outline_array[0] �` outline_array[x] �܂Ŏ�M�f�[�^�ւ̃|�C���^���Z�b�g����܂�
 */
//--------------------------------------------------------------
int GDS_RAP_RESPONSE_BattleVideoSearch_Download_DataCopy(GDS_RAP_WORK *gdsrap, BATTLE_REC_OUTLINE_RECV *outline_array, int array_max)
{
	POKE_NET_RESPONSE *res;
	POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH *param;
	POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH_RECVDATA *outline_recv;
	int i, hit_count;
	
	res = POKE_NET_GDS_GetResponse();
	param = (POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH *)(res->Param);
	
	hit_count = param->HitCount;
	if(hit_count > array_max){
		hit_count = array_max;
		GF_ASSERT(0);	//�p�ӂ��Ă���o�b�t�@������M����������
	}

	GFL_STD_MemClear(outline_array, sizeof(BATTLE_REC_OUTLINE_RECV) * array_max);
	outline_recv = (POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH_RECVDATA *)(param->Data);
	for(i = 0; i < hit_count; i++){
		if(outline_recv[i].Code != outline_recv[i].Data.head.data_number){
			//�w�b�_�ƃf�[�^�i���o�[���Ⴄ�ꍇ�̓w�b�_�Ƀi���o�[�������ĂȂ��̂ł������œ����
			outline_recv[i].Data.head.data_number = outline_recv[i].Code;
			//CRC�쐬
			outline_recv[i].Data.head.crc.crc16ccitt_hash = GFL_STD_CrcCalc(
				&outline_recv[i].Data.head, 
				sizeof(BATTLE_REC_HEADER) - GDS_CRC_SIZE - sizeof(outline_recv[i].Code));
		}
		outline_array[i] = outline_recv[i].Data;
	}
	
	return hit_count;
}

//--------------------------------------------------------------
/**
 * @brief   ���X�|���X�f�[�^���߁F�o�g���r�f�I�f�[�^�擾
 *
 * @param   gdsrap		
 * @param   res		
 *
 * @retval  TRUE:����
 * @retval  FALSE:���炩�̃G���[������(���e��res->Result�Q��)
 */
//--------------------------------------------------------------
int GDS_RAP_RESPONSE_BattleVideo_Data_Download(GDS_RAP_WORK *gdsrap, POKE_NET_RESPONSE *res)
{
	POKE_NET_GDS_RESPONSE_BATTLEDATA_GET *param;
	int ret = FALSE;
	
	switch(res->Result){
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_SUCCESS:			//!< �擾����
		param = (POKE_NET_GDS_RESPONSE_BATTLEDATA_GET *)(res->Param);
		OS_TPrintf("�o�g���r�f�I�f�[�^�擾��M����\n");
		ret = TRUE;
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_ERROR_AUTH:		//!< ���[�U�[�F�؃G���[
		OS_TPrintf("�o�g���r�f�I�f�[�^�擾��M�G���[�I:���[�U�[�F�؃G���[\n");
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_ERROR_ILLEGALCODE:	//!< �R�[�h�G���[
		OS_TPrintf("�o�g���r�f�I�f�[�^�擾��M�G���[�I:�R�[�h�G���[\n");
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_ERROR_UNKNOWN:		//!< ���̑��G���[
	default:
		OS_TPrintf("�o�g���r�f�I�f�[�^�擾��M�G���[�I:���̑��̃G���[\n");
		break;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   ��M�o�b�t�@����f�[�^�������Ă��镔���̃|�C���^���擾�F�o�g���r�f�I�f�[�^�擾
 *
 * @param   gdsrap		
 * @param   rec			�^��f�[�^�������Ă���f�[�^�ւ̃|�C���^�����
 *
 * @retval  �o�^�R�[�h
 */
//--------------------------------------------------------------
int GDS_RAP_RESPONSE_BattleVideoData_Download_RecvPtr_Set(GDS_RAP_WORK *gdsrap, BATTLE_REC_RECV **rec)
{
	POKE_NET_RESPONSE *res;
	POKE_NET_GDS_RESPONSE_BATTLEDATA_GET *param;
	
	res = POKE_NET_GDS_GetResponse();
	param = (POKE_NET_GDS_RESPONSE_BATTLEDATA_GET *)(res->Param);
	
	*rec = &param->Data;
	if(param->Code != param->Data.head.data_number){
		//�w�b�_�ƃf�[�^�i���o�[���Ⴄ�ꍇ�̓w�b�_�Ƀi���o�[�������ĂȂ��̂ł������œ����
		param->Data.head.data_number = param->Code;
		//CRC�쐬
		(*rec)->head.crc.crc16ccitt_hash = GFL_STD_CrcCalc(&(*rec)->head, 
			sizeof(BATTLE_REC_HEADER) - GDS_CRC_SIZE - sizeof(param->Code));
	}
	
	return param->Code;
}

//--------------------------------------------------------------
/**
 * @brief   ���X�|���X�f�[�^���߁F�o�g���r�f�I���C�ɓ���o�^
 *
 * @param   gdsrap		
 * @param   res		
 *
 * @retval  TRUE:����
 * @retval  FALSE:���炩�̃G���[������(���e��res->Result�Q��)
 */
//--------------------------------------------------------------
int GDS_RAP_RESPONSE_BattleVideo_Favorite_Upload(GDS_RAP_WORK *gdsrap, POKE_NET_RESPONSE *res)
{
//	POKE_NET_GDS_RESPONSE_BATTLEDATA_FAVORITE *param;
	int ret = FALSE;
	
	switch(res->Result){
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_FAVORITE_SUCCESS:			//!< ���C�ɓ���o�^����
//		param = (POKE_NET_GDS_RESPONSE_BATTLEDATA_FAVORITE *)(res->Param);
		OS_TPrintf("�o�g���r�f�I�f�[�^���C�ɓ���o�^��M����\n");
		ret = TRUE;
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_FAVORITE_ERROR_AUTH:		//!< ���[�U�[�F�؃G���[
		OS_TPrintf("�o�g���r�f�I���C�ɓ���o�^��M�G���[�I:���[�U�[�F�؃G���[\n");
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_FAVORITE_ERROR_ILLEGALCODE:	//!< �R�[�h�G���[
		OS_TPrintf("�o�g���r�f�I���C�ɓ���o�^��M�G���[�I:�R�[�h�G���[\n");
		break;
	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_FAVORITE_ERROR_UNKNOWN:	//!< ���̑��G���[
	default:
		OS_TPrintf("�o�g���r�f�I���C�ɓ���o�^��M�G���[�I:���̑��̃G���[\n");
		break;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   ��M�o�b�t�@����f�[�^�������Ă��镔���̃|�C���^���擾�F�o�g���r�f�I���C�ɓ���o�^
 *
 * @param   gdsrap		
 *
 * @retval  �o�^�R�[�h
 */
//--------------------------------------------------------------
void GDS_RAP_RESPONSE_BattleVideoFavorite_Upload_RecvPtr_Set(GDS_RAP_WORK *gdsrap)
{
	//����A�擾������͉̂�������
	return;
}
