//==============================================================================
/**
 * @file	gds_rap.c
 * @brief	GDS���C�u���������b�p�[��������
 * @author	matsuda
 * @date	2008.01.09(��)
 */
//==============================================================================
#include <gflib.h>
#include <dwc.h>
#include "net/dwc_rap.h"
#include "savedata\save_control.h"

#include <arc_tool.h>
#include "print/wordset.h"
#include "message.naix"

#include "gds_rap.h"
#include "gds_rap_response.h"
#include "savedata/gds_profile.h"

#include "msg/msg_wifi_system.h"
#include "savedata/playtime.h"

#include "message.naix"
#include "msg/msg_battle_rec.h"
#include "arc_def.h"

#include "battle/btl_net.h"
#include "gamesystem/game_data.h"


//==============================================================================
//	�萔��`
//==============================================================================
///DWC�ɓn�����[�N�̃T�C�Y
#define MYDWC_HEAPSIZE		0x20000

///�G���[���b�Z�[�W�W�J�p�o�b�t�@�̃T�C�Y
#define DWC_ERROR_BUF_NUM		(16*8*2)

///���M�O�̃E�F�C�g(���M���Ă��郁�b�Z�[�W��������ׂɃE�F�C�g�����Ă���)
#define GDS_SEND_BEFORE_WAIT	(60)

///�|�P�����s���`�F�b�N�ɓo�^���鐔
#define GDS_VIDEO_EVIL_CHECK_NUM  (12)

//--------------------------------------------------------------
//	�T�u�V�[�P���X�̖߂�l
//--------------------------------------------------------------
enum{
	SUBSEQ_CONTINUE,		///<�T�u�V�[�P���X�F�ێ�
	SUBSEQ_NEXT,			///<�T�u�V�[�P���X�F���̃T�u�V�[�P���X��
	SUBSEQ_PROCCESS_CHANGE,	///<�T�u�V�[�P���X�F�T�u�V�[�P���X�e�[�u���ύX
};

//==============================================================================
//	�\���̒�`
//==============================================================================
///�T�u�v���Z�X�V�[�P���X�̊֐��^��`
typedef int (*SUBPROCESS_FUNC)(GDS_RAP_WORK *, GDS_RAP_SUB_PROCCESS_WORK *sub_work);


//==============================================================================
//  �f�[�^
//==============================================================================
#include "gds_video_bit.cdat"

//--------------------------------------------------------------
//  
//--------------------------------------------------------------
FS_EXTERN_OVERLAY(dpw_common);


//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
static int GDSRAP_MAIN_Send(GDS_RAP_WORK *gdsrap);
static int GDSRAP_MAIN_Recv(GDS_RAP_WORK *gdsrap);

static int Local_GetResponse(GDS_RAP_WORK *gdsrap);

static void LIB_Heap_Init(int heap_id);
static void LIB_Heap_Exit(void);

static BOOL RecvSubProccess_Normal(void *work_gdsrap, void *work_recv_sub_work);
static BOOL RecvSubProccess_DataNumberSetSave(void *work_gdsrap, void *work_recv_sub_work);
static BOOL RecvSubProccess_SystemError(void *work_gdsrap, void *work_recv_sub_work);

static void* mydwc_AllocFunc( DWCAllocType name, u32   size, int align );
static void mydwc_FreeFunc( DWCAllocType name, void* ptr,  u32 size  );




//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   GDS���C�u��������������
 *
 * @param   gdsrap				GDS���b�p�[���[�N�ւ̃|�C���^
 * @param   init_data			�������f�[�^
 *
 * @retval  TRUE
 */
//--------------------------------------------------------------
int GDSRAP_Init(GDS_RAP_WORK *gdsrap, const GDSRAP_INIT_DATA *init_data)
{
	int ret;
	
  GFL_OVERLAY_Load( FS_OVERLAY_ID(dpw_common));

	GFL_STD_MemClear(gdsrap, sizeof(GDS_RAP_WORK));
	gdsrap->heap_id = init_data->heap_id;
	gdsrap->gamedata = init_data->gamedata;
	gdsrap->pSvl = init_data->pSvl;
	gdsrap->response_callback = init_data->response_callback;
	gdsrap->laststat = -1;
	gdsrap->send_req = POKE_NET_GDS_REQCODE_LAST;
	gdsrap->recv_wait_req = POKE_NET_GDS_REQCODE_LAST;
	
	gdsrap->response = GFL_HEAP_AllocMemory(init_data->heap_id, POKE_NET_GDS_GetResponseMaxSize(-1));
	GFL_STD_MemClear(gdsrap->response, POKE_NET_GDS_GetResponseMaxSize(-1));
	OS_TPrintf("���X�|���X�o�b�t�@�̃T�C�Y��%x\n", POKE_NET_GDS_GetResponseMaxSize(-1));

	//���b�Z�[�W�}�l�[�W���쐬
	gdsrap->msgman_wifisys = GFL_MSG_Create(GFL_MSG_LOAD_NORMAL, 
		ARCID_MESSAGE, NARC_message_wifi_system_dat, init_data->heap_id);
	gdsrap->wordset = WORDSET_Create(init_data->heap_id);	//�P��o�b�t�@�쐬
	gdsrap->ErrorString = GFL_STR_CreateBuffer(DWC_ERROR_BUF_NUM, init_data->heap_id);
	
	//��check�@�b��q�[�v�쐬
//	gdsrap->areanaLo = LIB_Heap_Init(init_data->heap_id);
	LIB_Heap_Init(init_data->heap_id);

	gdsrap->pokenet_auth.PID = init_data->gs_profile_id;
	gdsrap->pokenet_auth.ROMCode = PM_VERSION;
	gdsrap->pokenet_auth.LangCode = PM_LANG;
	ret = POKE_NET_GDS_Initialize(&gdsrap->pokenet_auth, SYACHI_SERVER_URL, SYACHI_SERVER_PORT);
	GF_ASSERT(ret == TRUE);		//�������Ɏ��s���鎖�͂��肦�Ȃ��͂�
	gdsrap->gdslib_initialize = TRUE;

	gdsrap->comm_initialize_ok = TRUE;
	
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   GDS���C�u�����j��
 *
 * @param   gdsrap		
 */
//--------------------------------------------------------------
void GDSRAP_Exit(GDS_RAP_WORK *gdsrap)
{
  if(gdsrap->p_nhttp != NULL){
    NHTTP_RAP_PokemonEvilCheckDelete( gdsrap->p_nhttp );
    NHTTP_RAP_End( gdsrap->p_nhttp );
  }

	//GDS���C�u�������
	POKE_NET_GDS_Release();
	gdsrap->gdslib_initialize = FALSE;

	//���b�Z�[�W�}�l�[�W���폜
	GFL_STR_DeleteBuffer(gdsrap->ErrorString);
	WORDSET_Delete(gdsrap->wordset);
	GFL_MSG_Delete(gdsrap->msgman_wifisys);

	GFL_HEAP_FreeMemory(gdsrap->response);

	//��check�@�b��q�[�v���
	LIB_Heap_Exit();
	//GFL_HEAP_FreeMemory(gdsrap->areanaLo);

  GFL_OVERLAY_Unload( FS_OVERLAY_ID(dpw_common));
}



//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   ���M���N�G�X�g�F�~���[�W�J���V���b�g�o�^
 *
 * @param   gdsrap			
 * @param   profile		
 * @param   dress		
 *
 * @retval  TRUE�F���N�G�X�g���󂯕t����
 * @retval  FALSE�F�󂯕t�����Ȃ�����
 */
//--------------------------------------------------------------
int GDSRAP_Tool_Send_MusicalShotUpload(GDS_RAP_WORK *gdsrap, GDS_PROFILE_PTR gpp, const MUSICAL_SHOT_DATA *musshot)
{
	if(GDSRAP_MoveStatusAllCheck(gdsrap) == FALSE){
		return FALSE;
	}
  
  gdsrap->send_buf.mus_send.profile = *gpp;
  gdsrap->send_buf.mus_send.mus_shot = *musshot;
	
	gdsrap->send_before_wait = GDS_SEND_BEFORE_WAIT;
	gdsrap->send_req = POKE_NET_GDS_REQCODE_MUSICALSHOT_REGIST;
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   ���M���N�G�X�g�F�~���[�W�J���V���b�g �_�E�����[�h
 *
 * @param   gdsrap			
 * @param   monsno			��M����|�P�����ԍ�
 *
 * @retval  TRUE�F���N�G�X�g���󂯕t����
 * @retval  FALSE�F�󂯕t�����Ȃ�����
 */
//--------------------------------------------------------------
int GDSRAP_Tool_Send_MusicalShotDownload(GDS_RAP_WORK *gdsrap, int monsno)
{
	if(GDSRAP_MoveStatusAllCheck(gdsrap) == FALSE){
		return FALSE;
	}

	gdsrap->send_buf.sub_para.musical.recv_monsno = monsno;
	
	gdsrap->send_req = POKE_NET_GDS_REQCODE_MUSICALSHOT_GET;
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   ���M���N�G�X�g�F�o�g���f�[�^�o�^
 *
 * @param   gdsrap		
 * @param   gpp						GDS�v���t�B�[���ւ̃|�C���^
 *
 * @retval  TRUE�F���N�G�X�g���󂯕t����
 * @retval  FALSE�F�󂯕t�����Ȃ�����
 * 
 * �^��f�[�^�̓T�C�Y������ȈׁA�R�s�[�����ɁAbrs�œǂݍ���ł���f�[�^�����̂܂ܑ��M���܂��B
 * brs�Ƀf�[�^��W�J���Ă��炱�̊֐����g�p����悤�ɂ��Ă��������B
 *
 * �I�I���̊֐��g�p��Abrs�ɓǂݍ���ł���v���t�B�[���f�[�^�̏��������A
 * 	   �^��f�[�^�{�̂̈Í������s���̂ł��̂܂܃Z�[�u������Đ����Ȃ��悤�ɒ��ӁI�I
 */
//--------------------------------------------------------------
int GDSRAP_Tool_Send_BattleVideoUpload(GDS_RAP_WORK *gdsrap, GDS_PROFILE_PTR gpp)
{
	GDS_PROFILE_PTR profile_ptr;
	BATTLE_REC_SEND *br_send = BattleRec_RecWorkAdrsGet();
	
	if(GDSRAP_MoveStatusAllCheck(gdsrap) == FALSE){
		return FALSE;
	}

  //�Í�������O�Ƀ|�P�����s���`�F�b�N�p�̃p�����[�^���쐬
  {
    int client_max, temoti_max, i;
    BOOL ret;
    
    GF_ASSERT(gdsrap->p_nhttp == NULL);
    gdsrap->p_nhttp = NHTTP_RAP_Init( gdsrap->heap_id, 
      MyStatus_GetProfileID( GAMEDATA_GetMyStatus(gdsrap->gamedata) ), gdsrap->pSvl );
    
    //�s�������̈�m��
    NHTTP_RAP_PokemonEvilCheckCreate( gdsrap->p_nhttp, gdsrap->heap_id, 
      sizeof(REC_POKEPARA) * GDS_VIDEO_EVIL_CHECK_NUM, NHTTP_POKECHK_VIDIO );

    //�|�P�����o�^
    BattleRec_ClientTemotiGet(br_send->head.mode, &client_max, &temoti_max);
    gdsrap->evil_check_count = 0;
    for(i = 0; i < client_max; i++){
      NHTTP_RAP_PokemonEvilCheckAdd(gdsrap->p_nhttp, 
        br_send->rec.rec_party[i].member, 
        sizeof(REC_POKEPARA) * br_send->rec.rec_party[i].PokeCount);//temoti_max);
      gdsrap->evil_check_count += br_send->rec.rec_party[i].PokeCount;
    }

    //�s������ �R�l�N�V�����쐬
    ret = NHTTP_RAP_PokemonEvilCheckConectionCreate(gdsrap->p_nhttp);
    GF_ASSERT(ret);

    ret = NHTTP_RAP_StartConnect( gdsrap->p_nhttp );  
    GF_ASSERT( ret );
  }
  
	//�^��f�[�^�͋���ȈׁA�R�s�[�����ɁA���̂܂�brs�̃f�[�^�𑗐M����
	gdsrap->send_buf.battle_rec_send_ptr = br_send;
	//brs�ɓW�J����Ă���^��f�[�^�{�͕̂���������Ă���̂ŁA���M����O�ɍēx�Í�������
	BattleRec_GDS_SendData_Conv(GAMEDATA_GetSaveControlWork(gdsrap->gamedata));
	
	//GDS�v���t�B�[���̂݁A�ŐV�̂�K�p����ׁA�㏑������
	profile_ptr = BattleRec_GDSProfilePtrGet();
	GFL_STD_MemCopy(gpp, profile_ptr, sizeof(GDS_PROFILE));
	
	gdsrap->send_before_wait = GDS_SEND_BEFORE_WAIT;
	gdsrap->send_req = POKE_NET_GDS_REQCODE_BATTLEDATA_REGIST;
	return TRUE;
}

///�����I�Ƀt���[���[�h�̎��ȏЉ�b�Z�[�W���Z�b�g����
void DEBUG_GDSRAP_SendVideoProfileFreeWordSet(GDS_RAP_WORK *gdsrap, u16 *set_code)
{
#ifdef PM_DEBUG
	GDS_PROFILE_PTR profile_ptr;

	profile_ptr = BattleRec_GDSProfilePtrGet();
	GFL_STD_MemCopy16(set_code, profile_ptr->event_self_introduction, EVENT_SELF_INTRO*2);
	profile_ptr->message_flag = 1;
	DEBUG_BattleRec_SecureFlagSet(GAMEDATA_GetSaveControlWork(gdsrap->gamedata));
#endif
}

//--------------------------------------------------------------
/**
 * @brief   ���M���N�G�X�g�F�o�g���r�f�I�_�E�����[�h(�ڍ׌���)
 *
 * @param   gdsrap		
 * @param   monsno			�|�P�����ԍ�(�w��Ȃ��FBATTLE_REC_SEARCH_MONSNO_NONE)
 * @param   search_mode_no	�����{��(BATTLEMODE_SEARCH_NO)
 * @param   country_code	���R�[�h(�w��Ȃ��FBATTLE_REC_SEARCH_COUNTRY_CODE_NONE)
 * @param   local_code		�n���R�[�h(�w��Ȃ��FBATTLE_REC_SEARCH_LOCAL_CODE_NONE)
 *
 * @retval  TRUE�F���N�G�X�g���󂯕t����
 * @retval  FALSE�F�󂯕t�����Ȃ�����
 */
//--------------------------------------------------------------
int GDSRAP_Tool_Send_BattleVideoSearchDownload(GDS_RAP_WORK *gdsrap, u16 monsno, BATTLEMODE_SEARCH_NO search_mode_no, u8 country_code, u8 local_code)
{
	if(GDSRAP_MoveStatusAllCheck(gdsrap) == FALSE){
		return FALSE;
	}
  
  GF_ASSERT(search_mode_no < NELEMS(BattleModeBitTbl));
  
	GFL_STD_MemClear(&gdsrap->send_buf.battle_rec_search, sizeof(BATTLE_REC_SEARCH_SEND));
	gdsrap->send_buf.battle_rec_search.monsno = monsno;
	gdsrap->send_buf.battle_rec_search.battle_mode = BattleModeBitTbl[search_mode_no].mode;
	gdsrap->send_buf.battle_rec_search.battle_mode_bit_mask = BattleModeBitTbl[search_mode_no].bit_mask;
	gdsrap->send_buf.battle_rec_search.country_code = country_code;
	gdsrap->send_buf.battle_rec_search.local_code = local_code;
	gdsrap->send_buf.battle_rec_search.server_version = BTL_NET_SERVER_VERSION;

	gdsrap->send_req_option = POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_CONDITION;
	gdsrap->send_req = POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH;
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   ���M���N�G�X�g�F�o�g���r�f�I�_�E�����[�h(�ŐV30��)
 *
 * @param   gdsrap		
 *
 * @retval  TRUE�F���N�G�X�g���󂯕t����
 * @retval  FALSE�F�󂯕t�����Ȃ�����
 */
//--------------------------------------------------------------
int GDSRAP_Tool_Send_BattleVideoNewDownload(GDS_RAP_WORK *gdsrap)
{
	if(GDSRAP_MoveStatusAllCheck(gdsrap) == FALSE){
		return FALSE;
	}

	gdsrap->send_req_option = POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_RANKING_LATEST;
	gdsrap->send_req = POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH;
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   ���M���N�G�X�g�F�o�g���r�f�I�_�E�����[�h(�o�g���T�u�E�F�C�����L���O)
 *
 * @param   gdsrap		
 *
 * @retval  TRUE�F���N�G�X�g���󂯕t����
 * @retval  FALSE�F�󂯕t�����Ȃ�����
 */
//--------------------------------------------------------------
int GDSRAP_Tool_Send_BattleVideoSubwayDownload(GDS_RAP_WORK *gdsrap)
{
	if(GDSRAP_MoveStatusAllCheck(gdsrap) == FALSE){
		return FALSE;
	}

	gdsrap->send_req_option = POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_RANKING_SUBWAY;
	gdsrap->send_req = POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH;
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   ���M���N�G�X�g�F�o�g���r�f�I�_�E�����[�h(�ʐM�ΐ탉���L���O)
 *
 * @param   gdsrap		
 *
 * @retval  TRUE�F���N�G�X�g���󂯕t����
 * @retval  FALSE�F�󂯕t�����Ȃ�����
 */
//--------------------------------------------------------------
int GDSRAP_Tool_Send_BattleVideoCommDownload(GDS_RAP_WORK *gdsrap)
{
	if(GDSRAP_MoveStatusAllCheck(gdsrap) == FALSE){
		return FALSE;
	}

	gdsrap->send_req_option = POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_RANKING_COMM;
	gdsrap->send_req = POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH;
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   ���M���N�G�X�g�F�o�g���r�f�I�擾
 *
 * @param   gdsrap		
 * @param   data_number	�f�[�^�i���o�[
 *
 * @retval  TRUE�F���N�G�X�g���󂯕t����
 * @retval  FALSE�F�󂯕t�����Ȃ�����
 */
//--------------------------------------------------------------
int GDSRAP_Tool_Send_BattleVideo_DataDownload(GDS_RAP_WORK *gdsrap, u64 data_number)
{
	if(GDSRAP_MoveStatusAllCheck(gdsrap) == FALSE){
		return FALSE;
	}

	gdsrap->send_buf.sub_para.rec.data_number = data_number;

	gdsrap->send_req = POKE_NET_GDS_REQCODE_BATTLEDATA_GET;
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   ���M���N�G�X�g�F�o�g���r�f�I���C�ɓ���o�^
 *
 * @param   gdsrap		
 * @param   data_number	�f�[�^�i���o�[
 *
 * @retval  TRUE�F���N�G�X�g���󂯕t����
 * @retval  FALSE�F�󂯕t�����Ȃ�����
 */
//--------------------------------------------------------------
int GDSRAP_Tool_Send_BattleVideo_FavoriteUpload(GDS_RAP_WORK *gdsrap, u64 data_number)
{
	if(GDSRAP_MoveStatusAllCheck(gdsrap) == FALSE){
		return FALSE;
	}

	gdsrap->send_buf.sub_para.rec.data_number = data_number;

	gdsrap->send_req = POKE_NET_GDS_REQCODE_BATTLEDATA_FAVORITE;
	return TRUE;
}




//--------------------------------------------------------------
/**
 * @brief   ���C������
 *
 * @param   gdsrap		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
int GDSRAP_Main(GDS_RAP_WORK *gdsrap)
{
	int ret;

#if 0	//gds_main.c �ŌĂ΂��悤�ɂȂ���
	if(gdsrap->comm_initialize_ok){
		// ��M���x�����N�𔽉f������
		DWC_UpdateConnection();

		// Dpw_Tr_Main() �����͗�O�I�ɂ��ł��Ăׂ�
	//	Dpw_Tr_Main();

		// �ʐM��Ԃ��m�F���ăA�C�R���̕\����ς���
		WirelessIconEasy_SetLevel( WM_LINK_LEVEL_3 - DWC_GetLinkLevel() );
	}
#endif

	//�l�b�g�ڑ����݂̂̏���
	if(gdsrap->gdslib_initialize == TRUE){
//		DWC_ProcessInet();
		
		//���M�f�[�^�m�F
		if(gdsrap->send_req != POKE_NET_GDS_REQCODE_LAST){
			if(gdsrap->send_before_wait > 0){
				gdsrap->send_before_wait--;
			}
			else{
				switch(POKE_NET_GDS_GetStatus()){
				case POKE_NET_GDS_STATUS_INITIALIZED:	//�������ς�
				case POKE_NET_GDS_STATUS_ABORTED:		//!< ���f�I��
				case POKE_NET_GDS_STATUS_FINISHED:		//!< ����I��
				case POKE_NET_GDS_STATUS_ERROR:			//!< �G���[�I��
					ret = GDSRAP_MAIN_Send(gdsrap);
					if(ret == TRUE){
						OS_TPrintf("data send! req_code = %d\n", gdsrap->send_req);
						gdsrap->send_req = POKE_NET_GDS_REQCODE_LAST;
						gdsrap->local_seq = 0;
					}
					break;
				default:	//���͑��M�o���Ȃ�
					OS_TPrintf("send wait ... status = %d\n", POKE_NET_GDS_GetStatus());
					break;
				}
			}
		}
		
		//��M�f�[�^�m�F
		GDSRAP_MAIN_Recv(gdsrap);
		
		//��M�T�u�v���Z�X���s(�f�[�^��M��A������t���[�ɖ߂��O�Ɏ��s���鏈��)
		if(gdsrap->recv_sub_work.recv_sub_proccess != NULL){
			if(gdsrap->recv_sub_work.recv_sub_proccess(gdsrap, &gdsrap->recv_sub_work) == TRUE){
				gdsrap->recv_sub_work.recv_sub_proccess = NULL;
				//��M�f�[�^�̃R�[���o�b�N�Ăяo��
				if(gdsrap->recv_sub_work.user_callback_func != NULL){
					gdsrap->recv_sub_work.user_callback_func(
						gdsrap->response_callback.callback_work, &gdsrap->error_info);
					gdsrap->recv_sub_work.user_callback_func = NULL;
				}
				GFL_STD_MemClear(&gdsrap->recv_sub_work, sizeof(GDS_RECV_SUB_PROCCESS_WORK));
				gdsrap->recv_wait_req = POKE_NET_GDS_REQCODE_LAST;
			}
		}
	}
	
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   ���M����
 *
 * @param   gdsrap		
 *
 * @retval  TRUE:���M����
 * @retval  FALSE:���M���s
 */
//--------------------------------------------------------------
static int GDSRAP_MAIN_Send(GDS_RAP_WORK *gdsrap)
{
	int ret = FALSE;
	
	//���N�G�X�g�Ƀf�[�^�����܂��Ă���Α��M
	switch(gdsrap->send_req){
	case POKE_NET_GDS_REQCODE_MUSICALSHOT_REGIST:	// �~���[�W�J���V���b�g�o�^
		ret = POKE_NET_GDS_MusicalShotRegist(&gdsrap->send_buf.mus_send, gdsrap->response);
		break;
	case POKE_NET_GDS_REQCODE_MUSICALSHOT_GET:		// �~���[�W�J���V���b�g�擾
		ret = POKE_NET_GDS_MusicalShotGet(gdsrap->send_buf.sub_para.musical.recv_monsno, 
			gdsrap->response);
		break;
	case POKE_NET_GDS_REQCODE_BATTLEDATA_REGIST:	// �o�g���r�f�I�o�^
	  switch(gdsrap->local_seq){
	  case 0:
	    {
        NHTTPError error;
        error = NHTTP_RAP_Process( gdsrap->p_nhttp );
        if( NHTTP_ERROR_NONE == error ){
          void *p_data;
          int i;
          int poke_result;
          
          p_data  = NHTTP_RAP_GetRecvBuffer( gdsrap->p_nhttp );
          //�����Ă�����Ԃ͐��킩
          if( NHTTP_RAP_EVILCHECK_GetStatusCode( p_data ) == 0 )    // ����
          {
            gdsrap->error_nhttp = FALSE;
            // �������擾
            { 
              const s8 *cp_sign  = NHTTP_RAP_EVILCHECK_GetSign( p_data, gdsrap->evil_check_count );
              GFL_STD_MemCopy( cp_sign, gdsrap->sign, NHTTP_RAP_EVILCHECK_RESPONSE_SIGN_LEN );
            }

            OS_TPrintf("�s����������\n");
            for( i = 0; i < gdsrap->evil_check_count; i++ ){
              poke_result  = NHTTP_RAP_EVILCHECK_GetPokeResult( p_data, i );
              gdsrap->nhttp_last_error = poke_result;
              if( poke_result != NHTTP_RAP_EVILCHECK_RESULT_OK ){
                OS_TPrintf("�|�P�����F�؃G���[ %d�� error=%d\n", i, poke_result);
              }
            }
          }
          else
          {
            OS_TPrintf("�s���������s\n");
            gdsrap->error_nhttp = TRUE;
            for( i = 0; i < gdsrap->evil_check_count; i++ ){
              poke_result  = NHTTP_RAP_EVILCHECK_GetPokeResult( p_data, i );
              gdsrap->nhttp_last_error = poke_result;
              if( poke_result != NHTTP_RAP_EVILCHECK_RESULT_OK ){
                OS_TPrintf("�|�P�����F�؃G���[ %d�� error=%d\n", i, poke_result);
              }
            }
          }

          NHTTP_RAP_PokemonEvilCheckDelete( gdsrap->p_nhttp );
          NHTTP_RAP_End( gdsrap->p_nhttp );
          gdsrap->p_nhttp = NULL;
          
          gdsrap->local_seq++;
        }
        else{
          OS_TPrintf("�s��������...\n");
          return FALSE;
        }
      }
      break;
	  default:
  		ret = POKE_NET_GDS_BattleDataRegist(gdsrap->send_buf.battle_rec_send_ptr, 
  			gdsrap->sign, NHTTP_RAP_EVILCHECK_RESPONSE_SIGN_LEN, gdsrap->response);
  		if(ret == TRUE){
  			OS_TPrintf("�o�g���r�f�I�o�^���N�G�X�g����\n");
  		}
  		break;
  	}
  	break;
	case POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH:	// �o�g���r�f�I����
		switch(gdsrap->send_req_option){
		case POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_CONDITION:	//�ڍ׌���
			ret = POKE_NET_GDS_BattleDataSearchCondition(&gdsrap->send_buf.battle_rec_search,
				gdsrap->response);
			break;
		case POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_RANKING_LATEST:	//�ŐV30��
		  ret = POKE_NET_GDS_BattleDataSearchRankingLatest(BTL_NET_SERVER_VERSION, gdsrap->response);
			break;
		case POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_RANKING_SUBWAY:	//�n���S�����L���O
			ret = POKE_NET_GDS_BattleDataSearchRankingSubway(BTL_NET_SERVER_VERSION, gdsrap->response);
			break;
	  case POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_RANKING_COMM:   //�ʐM�ΐ탉���L���O
	    ret = POKE_NET_GDS_BattleDataSearchRankingComm(BTL_NET_SERVER_VERSION, gdsrap->response);
	    break;
		}
		if(ret == TRUE){
			OS_TPrintf("�o�g���r�f�I�������N�G�X�g����\n");
		}
		break;
	case POKE_NET_GDS_REQCODE_BATTLEDATA_GET:	// �o�g���r�f�I�擾
		ret = POKE_NET_GDS_BattleDataGet(gdsrap->send_buf.sub_para.rec.data_number, 
			BTL_NET_SERVER_VERSION, gdsrap->response);
		if(ret == TRUE){
			OS_TPrintf("�o�g���r�f�I�擾���N�G�X�g����\n");
		}
		break;
	case POKE_NET_GDS_REQCODE_BATTLEDATA_FAVORITE:	// �o�g���r�f�I���C�ɓ���o�^
		ret = POKE_NET_GDS_BattleDataFavorite(gdsrap->send_buf.sub_para.rec.data_number, 
			gdsrap->response);
		if(ret == TRUE){
			OS_TPrintf("�o�g���r�f�I���C�ɓ���o�^���N�G�X�g����\n");
		}
		break;
	default:
		OS_Printf("-not supported\n");
		GF_ASSERT(0);
		break;
	}
	
	if(ret == TRUE){
		OS_TPrintf("���M���� send_req = %d\n", gdsrap->send_req);
		gdsrap->recv_wait_req = gdsrap->send_req;
	}
	else{
		//��check ���M���s���A����͐�������܂Ŗ��t���[�����킵�����Ă��邪�A
		//        ����ł����̂��A�G���[���b�Z�[�W���o���K�v������̂��m�F
		OS_TPrintf("���M���s�Bsend_req = %d\n", gdsrap->send_req);
	}
	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   ��M�X�e�[�^�X�m�F
 *
 * @param   gdsrap		
 *
 * @retval  
 */
//--------------------------------------------------------------
static int GDSRAP_MAIN_Recv(GDS_RAP_WORK *gdsrap)
{
	if(gdsrap->recv_wait_req == POKE_NET_GDS_REQCODE_LAST){
		return TRUE;
	}
	
	// �X�e�[�^�X�擾
	gdsrap->stat = POKE_NET_GDS_GetStatus();

	if(	gdsrap->stat != gdsrap->laststat ){
		// �X�e�[�^�X�ɕω���������
		switch(gdsrap->stat){
		case POKE_NET_GDS_STATUS_INACTIVE:					// ��ғ���
			OS_Printf("-inactive\n");
			break;
		case POKE_NET_GDS_STATUS_INITIALIZED:				// �������ς�
			OS_Printf("-initialized\n");
			break;
		case POKE_NET_GDS_STATUS_REQUEST:					// ���N�G�X�g���s
			OS_Printf("-request\n");
			break;
		case POKE_NET_GDS_STATUS_NETSETTING:				// �l�b�g���[�N�ݒ蒆
			OS_Printf("-netsetting\n");
			break;
		case POKE_NET_GDS_STATUS_CONNECTING:				// �ڑ���
			OS_Printf("-connecting\n");
			break;
		case POKE_NET_GDS_STATUS_SENDING:					// ���M��
			OS_Printf("-sending\n");
			break;
		case POKE_NET_GDS_STATUS_RECEIVING:					// ��M��
			OS_Printf("-receiving\n");
			break;
		case POKE_NET_GDS_STATUS_ABORTED:					// ���f
			OS_Printf("-aborted\n");
			gdsrap->error_info.type = GDS_ERROR_TYPE_STATUS;
			gdsrap->error_info.req_code = 0;
			gdsrap->error_info.result = POKE_NET_GDS_STATUS_ABORTED;
			gdsrap->error_info.occ = TRUE;
		//	gdsrap->recv_wait_req = POKE_NET_GDS_REQCODE_LAST;
			gdsrap->recv_sub_work.recv_sub_proccess = RecvSubProccess_SystemError;
			break;
		case POKE_NET_GDS_STATUS_FINISHED:					// ����I��
			OS_Printf("-finished\n");
			gdsrap->recv_sub_work.recv_sub_proccess = RecvSubProccess_Normal;
			// ���X�|���X����
			Local_GetResponse(gdsrap);
			break;
		case POKE_NET_GDS_STATUS_ERROR:						// �G���[�I��
			OS_Printf("-error lasterrorcode:%d\n" ,POKE_NET_GDS_GetLastErrorCode());
			gdsrap->error_info.type = GDS_ERROR_TYPE_LIB;
			gdsrap->error_info.req_code = 0;
			gdsrap->error_info.result = POKE_NET_GDS_GetLastErrorCode();
			gdsrap->error_info.occ = TRUE;
		//	gdsrap->recv_wait_req = POKE_NET_GDS_REQCODE_LAST;
			gdsrap->recv_sub_work.recv_sub_proccess = RecvSubProccess_SystemError;
			break;
		}
		gdsrap->laststat = gdsrap->stat;
	}
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   �f�[�^��M��̃T�u�v���Z�X�F���ɉ����������Ȃ��ꍇ
 *
 * @param   gdsrap		
 *
 * @retval  TRUE:���������B�@FALSE:�����p����
 */
//--------------------------------------------------------------
static BOOL RecvSubProccess_Normal(void *work_gdsrap, void *work_recv_sub_work)
{
	GDS_RAP_WORK *gdsrap = work_gdsrap;
	GDS_RECV_SUB_PROCCESS_WORK *recv_sub_work = work_recv_sub_work;

	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   �f�[�^��M��̃T�u�v���Z�X�F�^��f�[�^�Ƀf�[�^�i���o�[���Z�b�g���ăZ�[�u����
 *
 * @param   gdsrap		
 * @param   recv_sub_work		
 *
 * @retval  
 */
//--------------------------------------------------------------
static BOOL RecvSubProccess_DataNumberSetSave(void *work_gdsrap, void *work_recv_sub_work)
{
	GDS_RAP_WORK *gdsrap = work_gdsrap;
	GDS_RECV_SUB_PROCCESS_WORK *recv_sub_work = work_recv_sub_work;
	POKE_NET_RESPONSE *res;
	POKE_NET_GDS_RESPONSE_BATTLEDATA_REGIST *param;
	SAVE_RESULT ret;
	
	res = POKE_NET_GDS_GetResponse();
	param = (POKE_NET_GDS_RESPONSE_BATTLEDATA_REGIST *)(res->Param);
	ret = BattleRec_GDS_MySendData_DataNumberSetSave(
		GAMEDATA_GetSaveControlWork(gdsrap->gamedata), param->Code, 
		&recv_sub_work->recv_save_seq0, &recv_sub_work->recv_save_seq1, gdsrap->heap_id);
	if(ret == SAVE_RESULT_OK || ret == SAVE_RESULT_NG){
		OS_TPrintf("�O���Z�[�u����\n");
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   �f�[�^��M��̃T�u�v���Z�X�F�V�X�e���G���[�����������ꍇ�̃P�A����
 *
 * @param   gdsrap		
 *
 * @retval  
 */
//--------------------------------------------------------------
static BOOL RecvSubProccess_SystemError(void *work_gdsrap, void *work_recv_sub_work)
{
	GDS_RAP_WORK *gdsrap = work_gdsrap;
	GDS_RECV_SUB_PROCCESS_WORK *recv_sub_work = work_recv_sub_work;
	
	OS_TPrintf("�V�X�e���G���[�����������ׁA�P�A������ laststat=%d, recv_wait_req=%d\n", gdsrap->laststat, gdsrap->recv_wait_req);
	
	//���M�O�̃Z�[�u�͖����Ȃ������A���[�J���ł̑��M�ς݃`�F�b�N�������Ȃ����̂�
	//�P�A�����͖����Ȃ���
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   ��M�f�[�^����
 *
 * @param   gdsrap		
 *
 * @retval  TRUE:����I���B�@FALSE:�G���[����(�G���[����error_info�Q��)
 */
//--------------------------------------------------------------
static int Local_GetResponse(GDS_RAP_WORK *gdsrap)
{
	POKE_NET_RESPONSE *res;
	int size;
	int result = TRUE;
	
	res = POKE_NET_GDS_GetResponse();
	size = POKE_NET_GDS_GetResponseSize();
	
	GF_ASSERT(gdsrap->recv_wait_req == res->ReqCode);
	
	gdsrap->recv_sub_work.user_callback_func = NULL;
	
	// == �����ł��ꂼ��̃��X�|���X�ɑ΂��鏈�����s�� ==
	switch(res->ReqCode){
	case POKE_NET_GDS_REQCODE_MUSICALSHOT_REGIST:		// �~���[�W�J���V���b�g�o�^
		OS_TPrintf("��M�F�~���[�W�J���V���b�g�A�b�v���[�h\n");
		result = GDS_RAP_RESPONSE_MusicalShot_Upload(gdsrap, res);
		gdsrap->recv_sub_work.user_callback_func = gdsrap->response_callback.func_musicalshot_regist;
		break;
	case POKE_NET_GDS_REQCODE_MUSICALSHOT_GET:			// �~���[�W�J���V���b�g�擾
		OS_TPrintf("��M�F�~���[�W�J���V���b�g�擾\n");
		result = GDS_RAP_RESPONSE_MusicalShot_Download(gdsrap, res);
		gdsrap->recv_sub_work.user_callback_func = gdsrap->response_callback.func_musicalshot_get;
		break;
	case POKE_NET_GDS_REQCODE_BATTLEDATA_REGIST:		// �o�g���r�f�I�o�^
		OS_TPrintf("��M�F�o�g���r�f�I�o�^\n");
		result = GDS_RAP_RESPONSE_BattleVideo_Upload(gdsrap, res);
		gdsrap->recv_sub_work.user_callback_func = gdsrap->response_callback.func_battle_video_regist;
		if(result == TRUE){
			//�o�^�R�[�h�������̃f�[�^�ɓ���āA�O���Z�[�u���s
			gdsrap->recv_sub_work.recv_sub_proccess = RecvSubProccess_DataNumberSetSave;
		}
		break;
	case POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH:		// �o�g���r�f�I����
		OS_TPrintf("��M�F�o�g���r�f�I����\n");
		result = GDS_RAP_RESPONSE_BattleVideo_Search_Download(gdsrap, res);
		gdsrap->recv_sub_work.user_callback_func = gdsrap->response_callback.func_battle_video_search_get;
		break;
	case POKE_NET_GDS_REQCODE_BATTLEDATA_GET:		// �o�g���r�f�I�擾
		OS_TPrintf("��M�F�o�g���r�f�I�擾\n");
		result = GDS_RAP_RESPONSE_BattleVideo_Data_Download(gdsrap, res);
		gdsrap->recv_sub_work.user_callback_func = gdsrap->response_callback.func_battle_video_data_get;
		break;
	case POKE_NET_GDS_REQCODE_BATTLEDATA_FAVORITE:		// �o�g���r�f�I���C�ɓ���o�^
		OS_TPrintf("��M�F�o�g���r�f�I���C�ɓ���o�^\n");
		result = GDS_RAP_RESPONSE_BattleVideo_Favorite_Upload(gdsrap, res);
		gdsrap->recv_sub_work.user_callback_func = gdsrap->response_callback.func_battle_video_favorite_regist;
		break;
		
	case POKE_NET_GDS_REQCODE_DEBUG_MESSAGE:			// �f�o�b�O�p���N�G�X�g
	default:
		OS_TPrintf("��M�F����`�Ȃ���\n");
		break;
	}
	
	//�G���[�����Z�b�g
	if(result == FALSE){
		gdsrap->error_info.type = GDS_ERROR_TYPE_APP;
		gdsrap->error_info.req_code = res->ReqCode;
		gdsrap->error_info.result = res->Result;
		gdsrap->error_info.occ = TRUE;
	}
	else{
		gdsrap->error_info.occ = FALSE;
	}
	
	return result;
}

//--------------------------------------------------------------
/**
 * @brief   �G���[�����擾����
 *
 * @param   gdsrap			
 * @param   error_info		�G���[�������
 *
 * @retval  TRUE:�G���[���������Ă���
 * @retval  FALSE:�G���[����
 */
//--------------------------------------------------------------
BOOL GDSRAP_ErrorInfoGet(GDS_RAP_WORK *gdsrap, GDS_RAP_ERROR_INFO **error_info)
{
	*error_info = &gdsrap->error_info;
	return gdsrap->error_info.occ;
}

//--------------------------------------------------------------
/**
 * @brief   �G���[�����N���A����
 *
 * @param   gdsrap		
 */
//--------------------------------------------------------------
void GDSRAP_ErrorInfoClear(GDS_RAP_WORK *gdsrap)
{
	GFL_STD_MemClear(&gdsrap->error_info, sizeof(GDS_RAP_ERROR_INFO));
}

//--------------------------------------------------------------
/**
 * @brief   GDSRAP�����炩�̓�������s�A���͎�t�A�҂����A�Ȃǂ����Ă��邩�m�F����
 *
 * @param   gdsrap		
 *
 * @retval  TRUE:���̏��������s�A��t���Ă��Ȃ�
 * @retval  FALSE:���炩�̏��������s�A��t�A�҂��Ă���
 */
//--------------------------------------------------------------
BOOL GDSRAP_MoveStatusAllCheck(GDS_RAP_WORK *gdsrap)
{
	if(gdsrap->send_req == POKE_NET_GDS_REQCODE_LAST
			&& gdsrap->recv_wait_req == POKE_NET_GDS_REQCODE_LAST){
		return TRUE;
	}
	return FALSE;
}



//==============================================================================
//	�f�o�b�O�p�c�[��
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �f�o�b�O�c�[���F�Z�[�u���s�t���O��OFF����
 *
 * @param   gdsrap		
 */
//--------------------------------------------------------------
void DEBUG_GDSRAP_SaveFlagReset(GDS_RAP_WORK *gdsrap)
{
#ifdef PM_DEBUG
//	gdsrap->send_before_div_save = 0;
#endif
}

//==============================================================================
//
//	��check�@�b��@GDS���C�u������OS_AllocFromMain�֐����g�p���Ă���̂�
//				�ꎞ�I�Ƀq�[�v���쐬����
//
//==============================================================================
#if 0 //WB�ŕύX 2010.03.20(�y)
#define ROUND(n, a)		(((u32)(n)+(a)-1) & ~((a)-1))
static OSHeapHandle sHandle;
#else
static HEAPID GdsHeapID;
#endif

static void LIB_Heap_Init(int heap_id)
{
#if 0 //WB�ŕύX 2010.03.20(�y)
	void*    arenaLo;
	void*    arenaHi;
	void*	alloc_ptr;
	
	int heap_size = 0x2000;
	
	arenaLo = sys_AllocMemory(heap_id, heap_size);
	alloc_ptr = arenaLo;
	arenaHi = (void*)((u32)arenaLo + heap_size);
	
    arenaLo = OS_InitAlloc(OS_ARENA_MAIN, arenaLo, arenaHi, 1);
    OS_SetArenaLo(OS_ARENA_MAIN, arenaLo);

    // [nakata] �|�C���^��32bit���E�ɃA���C������
    arenaLo = (void*)ROUND(arenaLo, 32);
    arenaHi = (void*)ROUND(arenaHi, 32);

    // [nakata] �q�[�v�̈�̍쐬
    sHandle = OS_CreateHeap(OS_ARENA_MAIN, arenaLo, arenaHi);
    OS_SetCurrentHeap(OS_ARENA_MAIN, sHandle );

    // From here on out, OS_Alloc and OS_Free behave like malloc and free respectively
//    OS_SetArenaLo(OS_ARENA_MAIN, arenaHi);

	return alloc_ptr;
#else
  GdsHeapID = heap_id;
//  DWC_SetMemFunc( mydwc_AllocFunc, mydwc_FreeFunc );
#endif
}

static void LIB_Heap_Exit(void)
{
	;
}

/*---------------------------------------------------------------------------*
  �������m�ۊ֐�
 *---------------------------------------------------------------------------*/
static void* mydwc_AllocFunc( DWCAllocType name, u32   size, int align )
{
#pragma unused( name )
  void * ptr;
  OSIntrMode old;

  GF_ASSERT(align <= 32);  // �������������đΉ�
  old = OS_DisableInterrupts();
  ptr = GFL_NET_Align32Alloc(GdsHeapID, size);
  OS_RestoreInterrupts( old );

  return ptr;
}

/*---------------------------------------------------------------------------*
  �������J���֐�
 *---------------------------------------------------------------------------*/
static void mydwc_FreeFunc( DWCAllocType name, void* ptr,  u32 size  )
{
#pragma unused( name, size )
  OSIntrMode old;

  if ( !ptr ){
    return;  //NULL�J����F�߂�
  }
  old = OS_DisableInterrupts();
  GFL_NET_Align32Free(ptr);
  OS_RestoreInterrupts( old );
}
