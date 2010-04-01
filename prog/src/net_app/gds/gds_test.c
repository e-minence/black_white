//==============================================================================
/**
 * @file  gds_test.c
 * @brief GDS���C�u�����e�X�g�p�\�[�X
 * @author  matsuda
 * @date  2008.01.09(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "gds_rap.h"
#include "gds_rap_response.h"
#include "savedata/musical_save.h"
#include "musical\musical_define.h"
#include "gamesystem/game_data.h"
#include "musical/musical_debug.h"
#include "gds_test.h"


#ifdef PM_DEBUG //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//==============================================================================
//  �萔��`
//==============================================================================
#define HEAPID_GDS_TEST     (GFL_HEAPID_APP)


//==============================================================================
//  �\���̒�`
//==============================================================================
typedef struct{
//  IMC_TELEVISION_SAVEDATA* dummy_imc;
//  POKEMON_PARAM *pp;
  int monsno;
  int count;
}MUSICAL_ALL_UP;

typedef struct _GDS_TEST_SYS{
  //�V�X�e���n
  SAVE_CONTROL_WORK *sv;

  //�ʐM�p�����[�^
  GDS_PROFILE_PTR gds_profile_ptr;

  //�ʐM
  GDS_RAP_WORK gdsrap;  ///<GDS���C�u�����ANitroDWC�֘A�̃��[�N�\����

  //���[�J�����[�N
  int seq;

  //��M�f�[�^
  u64 data_number;

  MUSICAL_SHOT_DATA musshot;
  
  //�~���[�W�J���S�A�b�v�p���[�N
  MUSICAL_ALL_UP dau;
}GDS_TEST_SYS;

//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
GFL_PROC_RESULT GdsTestProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
GFL_PROC_RESULT GdsTestProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
GFL_PROC_RESULT GdsTestMusicalAllProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
GFL_PROC_RESULT GdsTestProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static int GdsTest_Main(GDS_TEST_SYS *testsys);
static int GdsTest_DressUpload(GDS_TEST_SYS *testsys);
static void Response_MusicalRegist(void *work, const GDS_RAP_ERROR_INFO *error_info);
static void Response_MusicalGet(void *work, const GDS_RAP_ERROR_INFO *error_info);
static void Response_BattleVideoRegist(void *work, const GDS_RAP_ERROR_INFO *error_info);
static void Response_BattleVideoSearch(void *work, const GDS_RAP_ERROR_INFO *error_info);
static void Response_BattleVideoDataGet(void *work, const GDS_RAP_ERROR_INFO *error_info);
static void Response_BattleVideoFavorite(void *work, const GDS_RAP_ERROR_INFO *error_info);


//==============================================================================
//  �f�[�^
//==============================================================================
///GDS�e�X�g�v���Z�X��`�f�[�^
const GFL_PROC_DATA GdsTestProcData = {
	GdsTestProc_Init,
	GdsTestProc_Main,
	GdsTestProc_End,
};

///GDS�~���[�W�J���V���b�g�S���M�v���Z�X��`�f�[�^
const GFL_PROC_DATA GdsTestMusicalAllProcData = {
	GdsTestProc_Init,
	GdsTestMusicalAllProc_Main,
	GdsTestProc_End,
};

//--------------------------------------------------------------
//  �~���[�W�J���f�[�^
//--------------------------------------------------------------
#include "musical/musical_entry.cdat"



//--------------------------------------------------------------
/**
 * @brief   �v���Z�X�֐��F������
 *
 * @param   proc    �v���Z�X�f�[�^
 * @param   seq     �V�[�P���X
 *
 * @retval  ������
 */
//--------------------------------------------------------------
GFL_PROC_RESULT GdsTestProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  GDS_TEST_PARENT *gtp = pwk;
  GDS_TEST_SYS *testsys;
  
  testsys = GFL_PROC_AllocWork(proc, sizeof(GDS_TEST_SYS), HEAPID_GDS_TEST );
  GFL_STD_MemClear(testsys, sizeof(GDS_TEST_SYS));
  
  testsys->sv = GAMEDATA_GetSaveControlWork(gtp->gamedata);
  
  testsys->gds_profile_ptr = GDS_Profile_AllocMemory(HEAPID_GDS_TEST);
  GDS_Profile_MyDataSet(testsys->gds_profile_ptr, gtp->gamedata);

  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
/**
 * @brief   �v���Z�X�֐��F���C��
 *
 * @param   proc    �v���Z�X�f�[�^
 * @param   seq     �V�[�P���X
 *
 * @retval  ������
 */
//--------------------------------------------------------------
GFL_PROC_RESULT GdsTestProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  GDS_TEST_SYS *testsys = mywk;
  GDS_TEST_PARENT *gtp = pwk;
  int ret;
  enum{
    SEQ_COMM_INIT,
    SEQ_COMM_MAIN,
    SEQ_COMM_END,

    SEQ_END,
  };

  switch( *seq ){
  case SEQ_COMM_INIT:
    {
      GDSRAP_INIT_DATA init_data;

      init_data.gs_profile_id = MyStatus_GetProfileID(GAMEDATA_GetMyStatus(gtp->gamedata));
      init_data.heap_id = HEAPID_GDS_TEST;
      init_data.gamedata = gtp->gamedata;
      init_data.pSvl = gtp->pSvl;

      init_data.response_callback.callback_work = testsys;
      init_data.response_callback.func_musicalshot_regist = Response_MusicalRegist;
      init_data.response_callback.func_musicalshot_get = Response_MusicalGet;
      init_data.response_callback.func_battle_video_regist = Response_BattleVideoRegist;
      init_data.response_callback.func_battle_video_search_get = Response_BattleVideoSearch;
      init_data.response_callback.func_battle_video_data_get = Response_BattleVideoDataGet;
      init_data.response_callback.func_battle_video_favorite_regist = Response_BattleVideoFavorite;

      GDSRAP_Init(&testsys->gdsrap, &init_data);  //�ʐM���C�u����������
    }
    (*seq)++;
    break;

  case SEQ_COMM_MAIN:
    if(GdsTest_Main(testsys) == FALSE){
      (*seq)++;
    }
    break;
  case SEQ_COMM_END:
    GDSRAP_Exit(&testsys->gdsrap);
    (*seq)++;
    break;

  case SEQ_END:
    return GFL_PROC_RES_FINISH;
  }

  return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   �v���Z�X�֐��F���C�� ���~���[�W�J���S�|�P�����A�b�v���[�h
 *
 * @param   proc    �v���Z�X�f�[�^
 * @param   seq     �V�[�P���X
 *
 * @retval  ������
 */
//--------------------------------------------------------------
GFL_PROC_RESULT GdsTestMusicalAllProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  GDS_TEST_SYS * testsys  = mywk;
  GDS_TEST_PARENT *gtp = pwk;
  int ret;
  enum{
    SEQ_COMM_INIT,
    SEQ_COMM_MAIN,
    SEQ_COMM_END,

    SEQ_END,
  };

  switch( *seq ){
  case SEQ_COMM_INIT:
    {
      GDSRAP_INIT_DATA init_data;

      init_data.gs_profile_id = MyStatus_GetProfileID(GAMEDATA_GetMyStatus(gtp->gamedata));
      init_data.heap_id = HEAPID_GDS_TEST;
      init_data.gamedata = gtp->gamedata;
      init_data.pSvl = gtp->pSvl;

      init_data.response_callback.callback_work = testsys;
      init_data.response_callback.func_musicalshot_regist = Response_MusicalRegist;
      init_data.response_callback.func_musicalshot_get = Response_MusicalGet;
      init_data.response_callback.func_battle_video_regist = Response_BattleVideoRegist;
      init_data.response_callback.func_battle_video_search_get = Response_BattleVideoSearch;
      init_data.response_callback.func_battle_video_data_get = Response_BattleVideoDataGet;
      init_data.response_callback.func_battle_video_favorite_regist = Response_BattleVideoFavorite;

      GDSRAP_Init(&testsys->gdsrap, &init_data);  //�ʐM���C�u����������
    }
    (*seq)++;
    break;

  case SEQ_COMM_MAIN:
    if(GdsTest_DressUpload(testsys) == FALSE){
      (*seq)++;
    }
    break;
  case SEQ_COMM_END:
    GDSRAP_Exit(&testsys->gdsrap);
    (*seq)++;
    break;

  case SEQ_END:
    return GFL_PROC_RES_FINISH;
  }

  return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   �v���Z�X�֐��F�I��
 *
 * @param   proc    �v���Z�X�f�[�^
 * @param   seq     �V�[�P���X
 *
 * @retval  ������
 */
//--------------------------------------------------------------
GFL_PROC_RESULT GdsTestProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  GDS_TEST_SYS * testsys = mywk;
  int i;

  GDS_Profile_FreeMemory(testsys->gds_profile_ptr);
  GFL_PROC_FreeWork(proc);        // ���[�N�J��

  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
/**
 * @brief   ���C��
 *
 * @param   testsys
 *
 * @retval
 *
 *
 */
//--------------------------------------------------------------
static int GdsTest_Main(GDS_TEST_SYS *testsys)
{
  GDS_RAP_ERROR_INFO *error_info;
  enum{
    SEQ_WIFI_CONNECT,

    SEQ_MUSICAL_UPLOAD,
    SEQ_MUSICAL_UPLOAD_ERROR_CHECK,

    SEQ_MUSICAL_DOWNLOAD,
    SEQ_MUSICAL_DOWNLOAD_ERROR_CHECK,

    SEQ_VIDEO_UPLOAD,
    SEQ_VIDEO_UPLOAD_ERROR_CHECK,

    SEQ_VIDEO_SEARCH,
    SEQ_VIDEO_SEARCH_ERROR_CHECK,

    SEQ_VIDEO_DATA_GET,
    SEQ_VIDEO_DATA_GET_ERROR_CHECK,

    SEQ_VIDEO_FAVORITE,
    SEQ_VIDEO_FAVORITE_ERROR_CHECK,

    SEQ_WIFI_CLEANUP,

    SEQ_EXIT,
  };

  if(GDSRAP_MoveStatusAllCheck(&testsys->gdsrap) == TRUE){
    switch(testsys->seq){
    case SEQ_WIFI_CONNECT:  //WIFI�ڑ�
      //GDSRAP_ProccessReq(&testsys->gdsrap, GDSRAP_PROCESS_REQ_INTERNET_CONNECT);
      testsys->seq++;
      break;

    case SEQ_MUSICAL_UPLOAD:  //�~���[�W�J�����M
      {
        MUSICAL_DEBUG_CreateDummyData( &testsys->musshot, MUSICAL_ENTRY_ARR[0], HEAPID_GDS_TEST );
        if(GDSRAP_Tool_Send_MusicalShotUpload(
            &testsys->gdsrap, testsys->gds_profile_ptr, &testsys->musshot) == TRUE){
          OS_TPrintf("�����̃~���[�W�J���V���b�g���M\n");
          testsys->seq++;
        }
      }
      break;
    case SEQ_MUSICAL_UPLOAD_ERROR_CHECK:
      if(GDSRAP_ErrorInfoGet(&testsys->gdsrap, &error_info) == TRUE){
        //�G���[�������̏���
        //�G���[���b�Z�[�W�̕\�����̓R�[���o�b�N�ōs����̂ŁA
        //�����ł͕\����̏����B(�A�v���I���Ƃ�����̃��j���[�ɖ߂��Ƃ�)
        OS_TPrintf("�~���[�W�J���V���b�g���M�F�G���[����\n");
        testsys->seq++;
      }
      else{
        OS_TPrintf("�~���[�W�J���V���b�g���M�F����\n");
        testsys->seq++;
      }
      break;

    case SEQ_MUSICAL_DOWNLOAD:  //�~���[�W�J���_�E�����[�h
      if(GDSRAP_Tool_Send_MusicalShotDownload(&testsys->gdsrap, MUSICAL_ENTRY_ARR[0]) == TRUE){
        OS_TPrintf("�~���[�W�J���V���b�g�_�E�����[�h���N�G�X�g\n");
        testsys->seq++;
      }
      break;
    case SEQ_MUSICAL_DOWNLOAD_ERROR_CHECK:
      if(GDSRAP_ErrorInfoGet(&testsys->gdsrap, &error_info) == TRUE){
        //�G���[�������̏���
        //�G���[���b�Z�[�W�̕\�����̓R�[���o�b�N�ōs����̂ŁA
        //�����ł͕\����̏����B(�A�v���I���Ƃ�����̃��j���[�ɖ߂��Ƃ�)
        OS_TPrintf("�~���[�W�J���V���b�g�_�E�����[�h���N�G�X�g�F�G���[����\n");
        testsys->seq++;
      }
      else{
        OS_TPrintf("�~���[�W�J���V���b�g�_�E�����[�h���N�G�X�g�F����\n");
        testsys->seq++;
      }
      break;

    case SEQ_VIDEO_UPLOAD:  //
      {
        LOAD_RESULT result;

        BattleRec_Load(testsys->sv, HEAPID_GDS_TEST, &result, LOADDATA_MYREC);

        if(GDSRAP_Tool_Send_BattleVideoUpload(
            &testsys->gdsrap, testsys->gds_profile_ptr) == TRUE){
          OS_TPrintf("�����̃o�g���r�f�I���A�b�v���[�h\n");
          testsys->seq++;
        }
      }
      break;
    case SEQ_VIDEO_UPLOAD_ERROR_CHECK:
      BattleRec_Exit();
      if(GDSRAP_ErrorInfoGet(&testsys->gdsrap, &error_info) == TRUE){
        //�G���[�������̏���
        //�G���[���b�Z�[�W�̕\�����̓R�[���o�b�N�ōs����̂ŁA
        //�����ł͕\����̏����B(�A�v���I���Ƃ�����̃��j���[�ɖ߂��Ƃ�)
        OS_TPrintf("�����̃o�g���r�f�I���A�b�v���[�h�F�G���[����\n");
        testsys->seq++;
      }
      else{
        OS_TPrintf("�����̃o�g���r�f�I���A�b�v���[�h�F����\n");
        testsys->seq++;
      }
      break;

    case SEQ_VIDEO_SEARCH:  //
      //�ŐV30��
      if(GDSRAP_Tool_Send_BattleVideoNewDownload(&testsys->gdsrap) == TRUE){
        OS_TPrintf("�ŐV30���v��\n");
        testsys->seq++;
      }
      break;
    case SEQ_VIDEO_SEARCH_ERROR_CHECK:
      if(GDSRAP_ErrorInfoGet(&testsys->gdsrap, &error_info) == TRUE){
        //�G���[�������̏���
        //�G���[���b�Z�[�W�̕\�����̓R�[���o�b�N�ōs����̂ŁA
        //�����ł͕\����̏����B(�A�v���I���Ƃ�����̃��j���[�ɖ߂��Ƃ�)
        OS_TPrintf("�ŐV30���v���F�G���[����\n");
        testsys->seq++;
      }
      else{
        OS_TPrintf("�ŐV30���v���F����\n");
        testsys->seq++;
      }
      break;

    case SEQ_VIDEO_DATA_GET:  //
      if(GDSRAP_Tool_Send_BattleVideo_DataDownload(
          &testsys->gdsrap, testsys->data_number) == TRUE){
        OS_TPrintf("�f�[�^�i���o�[�w��Ń_�E�����[�h dataNumber=%d\n", testsys->data_number);
        testsys->seq++;
      }
      break;
    case SEQ_VIDEO_DATA_GET_ERROR_CHECK:
      if(GDSRAP_ErrorInfoGet(&testsys->gdsrap, &error_info) == TRUE){
        //�G���[�������̏���
        //�G���[���b�Z�[�W�̕\�����̓R�[���o�b�N�ōs����̂ŁA
        //�����ł͕\����̏����B(�A�v���I���Ƃ�����̃��j���[�ɖ߂��Ƃ�)
        OS_TPrintf("�f�[�^�i���o�[�w��Ń_�E�����[�h�F�G���[����\n");
        testsys->seq++;
      }
      else{
        OS_TPrintf("�f�[�^�i���o�[�w��Ń_�E�����[�h�F����\n");
        testsys->seq++;
      }
      break;

    case SEQ_VIDEO_FAVORITE:  //
      if(GDSRAP_Tool_Send_BattleVideo_DataDownload(
          &testsys->gdsrap, testsys->data_number) == TRUE){
        OS_TPrintf("�f�[�^�i���o�[�w��ł��C�ɓ��著�M\n");
        testsys->seq++;
      }
      break;
    case SEQ_VIDEO_FAVORITE_ERROR_CHECK:
      if(GDSRAP_ErrorInfoGet(&testsys->gdsrap, &error_info) == TRUE){
        //�G���[�������̏���
        //�G���[���b�Z�[�W�̕\�����̓R�[���o�b�N�ōs����̂ŁA
        //�����ł͕\����̏����B(�A�v���I���Ƃ�����̃��j���[�ɖ߂��Ƃ�)
        OS_TPrintf("�f�[�^�i���o�[�w��ł��C�ɓ��著�M�F�G���[����\n");
        testsys->seq++;
      }
      else{
        OS_TPrintf("�f�[�^�i���o�[�w��ł��C�ɓ��著�M�F����\n");
        testsys->seq++;
      }
      break;

    case SEQ_WIFI_CLEANUP:  //WIFI�ؒf
      //GDSRAP_ProccessReq(&testsys->gdsrap, GDSRAP_PROCESS_REQ_INTERNET_CLEANUP);
      testsys->seq++;
      break;

    case SEQ_EXIT:
      return FALSE;
    }
  }

  GDSRAP_Main(&testsys->gdsrap);
  return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   ���C��(�~���[�W�J���ɑS�|�P�����A�b�v���[�h
 *
 * @param   testsys
 *
 * @retval
 *
 *
 */
//--------------------------------------------------------------
static int GdsTest_DressUpload(GDS_TEST_SYS *testsys)
{
#if 0
  MUSICAL_ALL_UP *dau = &testsys->dau;
  GDS_RAP_ERROR_INFO *error_info;
  enum{
    SEQ_INIT,

    SEQ_POKE_CREATE,
    SEQ_MUSICAL_UPLOAD,
    SEQ_MUSICAL_UPLOAD_ERROR_CHECK,

    SEQ_EXIT,
    SEQ_TRG_WAIT,
  };
  // ������擾
  STRBUF *tempbuf, *destbuf;

  if(GDSRAP_MoveStatusAllCheck(&testsys->gdsrap) == TRUE){
    switch(testsys->seq){
    case SEQ_INIT:
      dau->monsno = 1;
      dau->pp = PokemonParam_AllocWork(testsys->heap_id);
      dau->dummy_imc = ImcSaveData_TelevisionAllocWork(testsys->heap_id);
      testsys->seq++;
      //break;

    case SEQ_POKE_CREATE:
      PokeParaSetPowRnd(dau->pp, dau->monsno, 20, 1234, gf_rand());//9876);
      PokeParaPut(dau->pp, ID_PARA_oyaname, MyStatus_GetMyName(SaveData_GetMyStatus(testsys->sv)));
      Debug_ImcSaveData_MakeTelevisionDummyData(dau->dummy_imc, dau->pp, 5);
      testsys->seq++;
      //break;

    case SEQ_MUSICAL_UPLOAD:  //�~���[�W�J�����M
      if(GDSRAP_Tool_Send_MusicalUpload(&testsys->gdsrap, testsys->gds_profile_ptr,
          dau->dummy_imc) == TRUE){
        DEBUG_GDSRAP_SaveFlagReset(&testsys->gdsrap);
        OS_TPrintf("monsno = %d�� ���M�� %d����\n", dau->monsno, dau->count+1);
        // �E�C���h�E�g�`�恕���b�Z�[�W�̈�N���A
        GF_BGL_BmpWinDataFill(&testsys->win[TEST_BMPWIN_TITLE], 15 );
        // ������`��J�n
        WORDSET_RegisterPokeMonsName(testsys->wordset, 0, PPPPointerGet(dau->pp));
        WORDSET_RegisterNumber(testsys->wordset, 1, dau->count+1, 1,
          NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);
        destbuf = STRBUF_Create(256, testsys->heap_id);
        tempbuf = MSGMAN_AllocString(testsys->msgman, DMMSG_GDS_MUSICAL_UPLOAD);
        WORDSET_ExpandStr( testsys->wordset, destbuf, tempbuf);
        GF_STR_PrintSimple( &testsys->win[TEST_BMPWIN_TITLE],
          FONT_TALK, destbuf, 0, 0, MSG_ALLPUT, NULL);
        STRBUF_Delete(destbuf);
        STRBUF_Delete(tempbuf);
        testsys->seq++;
      }
      break;
    case SEQ_MUSICAL_UPLOAD_ERROR_CHECK:
      if(GDSRAP_ErrorInfoGet(&testsys->gdsrap, &error_info) == TRUE){
        //�G���[�������̏���
        //�G���[���b�Z�[�W�̕\�����̓R�[���o�b�N�ōs����̂ŁA
        //�����ł͕\����̏����B(�A�v���I���Ƃ�����̃��j���[�ɖ߂��Ƃ�)
        OS_TPrintf("�G���[���������܂���\n");
        tempbuf = MSGMAN_AllocString(testsys->msgman, DMMSG_GDS_MUSICAL_UPLOAD_ERROR);
        GF_STR_PrintSimple( &testsys->win[TEST_BMPWIN_TITLE],
          FONT_TALK, tempbuf, 0, 0, MSG_ALLPUT, NULL);
        STRBUF_Delete(tempbuf);
        testsys->seq = SEQ_TRG_WAIT;
      }
      else{
        dau->count++;
        if(dau->count >= BR_MUSICAL_VIEW_MAX){
          dau->monsno++;
          dau->count = 0;
          if(dau->monsno > MONSNO_END-1){ //-1=�A���Z�E�X������(�s�����������̂�)
            testsys->seq++;
          }
          else{
            testsys->seq = SEQ_POKE_CREATE;
          }
        }
        else{
          testsys->seq = SEQ_POKE_CREATE;
        }
      }
      break;

    case SEQ_EXIT:
      sys_FreeMemoryEz(dau->dummy_imc);
      sys_FreeMemoryEz(dau->pp);
      return FALSE;

    case SEQ_TRG_WAIT:
      if(sys.trg & PAD_BUTTON_A){
        testsys->seq = SEQ_EXIT;
      }
      break;
    }
  }

  GDSRAP_Main(&testsys->gdsrap);
#endif
  return TRUE;
}

//==============================================================================
//
//  �ʐM�f�[�^���X�|���X�R�[���o�b�N�֐�
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �~���[�W�J���A�b�v���[�h���̃��X�|���X�R�[���o�b�N
 *
 * @param   work
 * @param   error_info    �G���[���
 */
//--------------------------------------------------------------
static void Response_MusicalRegist(void *work, const GDS_RAP_ERROR_INFO *error_info)
{
  GDS_TEST_SYS *testsys = work;

  OS_TPrintf("�~���[�W�J���V���b�g�̃A�b�v���[�h���X�|���X�擾\n");
  if(error_info->occ == TRUE){
    //TRUE�Ȃ�΃G���[�������Ă���̂ŁA�����Ń��j���[��߂��Ƃ��A�v���I�����[�h�ֈڍs�Ƃ�����
  }
  else{
    //���펞�Ȃ�Ύ�M�o�b�t�@����f�[�^�擾�Ȃǂ��s��
    //�A�b�v���[�h�̏ꍇ�͓��ɕK�v�Ȃ�
  }
}

//--------------------------------------------------------------
/**
 * @brief   �~���[�W�J���_�E�����[�h���̃��X�|���X�R�[���o�b�N
 *
 * @param   work
 * @param   error_info    �G���[���
 */
//--------------------------------------------------------------
static void Response_MusicalGet(void *work, const GDS_RAP_ERROR_INFO *error_info)
{
  GDS_TEST_SYS *testsys = work;

  OS_TPrintf("�~���[�W�J���V���b�g�̃_�E�����[�h���X�|���X�擾\n");
  if(error_info->occ == TRUE){
    //TRUE�Ȃ�΃G���[�������Ă���̂ŁA�����Ń��j���[��߂��Ƃ��A�v���I�����[�h�ֈڍs�Ƃ�����
  }
  else{
    //���펞�Ȃ�Ύ�M�o�b�t�@����f�[�^�擾�Ȃǂ��s��
    //�A�b�v���[�h�̏ꍇ�͓��ɕK�v�Ȃ�

  //  GDS_RAP_RESPONSE_MusicalShot_Download_RecvPtr_Set(GDS_RAP_WORK *gdsrap, GDS_MUSICAL_RECV **dress_array, int array_max);
  }
}

//--------------------------------------------------------------
/**
 * @brief   �o�g���r�f�I�o�^���̃��X�|���X�R�[���o�b�N
 *
 * @param   work
 * @param   error_info    �G���[���
 */
//--------------------------------------------------------------
static void Response_BattleVideoRegist(void *work, const GDS_RAP_ERROR_INFO *error_info)
{
  GDS_TEST_SYS *testsys = work;

  OS_TPrintf("�o�g���r�f�I�o�^���̃_�E�����[�h���X�|���X�擾\n");
  if(error_info->occ == TRUE){
    //TRUE�Ȃ�΃G���[�������Ă���̂ŁA�����Ń��j���[��߂��Ƃ��A�v���I�����[�h�ֈڍs�Ƃ�����
    switch(error_info->result){
    case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_ERROR_AUTH:    //!< ���[�U�[�F�؃G���[
      OS_TPrintf("�o�g���r�f�I�f�[�^�擾��M�G���[�I:���[�U�[�F�؃G���[\n");
      break;
    case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_ERROR_ILLEGALCODE: //!< �R�[�h�G���[
      OS_TPrintf("�o�g���r�f�I�f�[�^�擾��M�G���[�I:�R�[�h�G���[\n");
      break;
  	case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ILLEGALPOKEMON_VERIFY: //!< �|�P���������ŃG���[
  		OS_TPrintf("�o�g���r�f�I�o�^��M�G���[�I:�|�P���������ŃG���[\n");
  		break;
    case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_ERROR_UNKNOWN:   //!< ���̑��G���[
    default:
      OS_TPrintf("�o�g���r�f�I�f�[�^�擾��M�G���[�I:���̑��̃G���[\n");
      break;
    }
  }
  else{
    //���펞�Ȃ�Ύ�M�o�b�t�@����f�[�^�擾�Ȃǂ��s��
    //�A�b�v���[�h�̏ꍇ�͓��ɕK�v�Ȃ�
    u64 data_number;

    data_number = GDS_RAP_RESPONSE_BattleVideo_Upload_DataGet(&testsys->gdsrap);
    testsys->data_number = data_number;
    OS_TPrintf("�o�^�R�[�h��%d\n", data_number);
  }
}

//--------------------------------------------------------------
/**
 * @brief   �o�g���r�f�I�������̃��X�|���X�R�[���o�b�N
 *
 * @param   work
 * @param   error_info    �G���[���
 */
//--------------------------------------------------------------
static void Response_BattleVideoSearch(void *work, const GDS_RAP_ERROR_INFO *error_info)
{
  GDS_TEST_SYS *testsys = work;

  OS_TPrintf("�o�g���r�f�I�����̃_�E�����[�h���X�|���X�擾\n");
  if(error_info->occ == TRUE){
    //TRUE�Ȃ�΃G���[�������Ă���̂ŁA�����Ń��j���[��߂��Ƃ��A�v���I�����[�h�ֈڍs�Ƃ�����
  }
  else{
    //���펞�Ȃ�Ύ�M�o�b�t�@����f�[�^�擾�Ȃǂ��s��
    //�A�b�v���[�h�̏ꍇ�͓��ɕK�v�Ȃ�

  //  int GDS_RAP_RESPONSE_Boxshot_Download_RecvPtr_Set(GDS_RAP_WORK *gdsrap, BOX_SHOT_RECV **box_array, int array_max);
  }
}

//--------------------------------------------------------------
/**
 * @brief   �o�g���r�f�I�f�[�^�_�E�����[�h���̃��X�|���X�R�[���o�b�N
 *
 * @param   work
 * @param   error_info    �G���[���
 */
//--------------------------------------------------------------
static void Response_BattleVideoDataGet(void *work, const GDS_RAP_ERROR_INFO *error_info)
{
  GDS_TEST_SYS *testsys = work;

  OS_TPrintf("�o�g���r�f�I�f�[�^�擾�̃_�E�����[�h���X�|���X�擾\n");
  if(error_info->occ == TRUE){
    //TRUE�Ȃ�΃G���[�������Ă���̂ŁA�����Ń��j���[��߂��Ƃ��A�v���I�����[�h�ֈڍs�Ƃ�����
  }
  else{
    //���펞�Ȃ�Ύ�M�o�b�t�@����f�[�^�擾�Ȃǂ��s��
    //�A�b�v���[�h�̏ꍇ�͓��ɕK�v�Ȃ�

  //  int GDS_RAP_RESPONSE_Boxshot_Download_RecvPtr_Set(GDS_RAP_WORK *gdsrap, BOX_SHOT_RECV **box_array, int array_max);
  }
}

//--------------------------------------------------------------
/**
 * @brief   �o�g���r�f�I���C�ɓ���o�^�̃��X�|���X�R�[���o�b�N
 *
 * @param   work
 * @param   error_info    �G���[���
 */
//--------------------------------------------------------------
static void Response_BattleVideoFavorite(void *work, const GDS_RAP_ERROR_INFO *error_info)
{
  GDS_TEST_SYS *testsys = work;

  OS_TPrintf("�o�g���r�f�I���C�ɓ���o�^�̃_�E�����[�h���X�|���X�擾\n");
  if(error_info->occ == TRUE){
    //TRUE�Ȃ�΃G���[�������Ă���̂ŁA�����Ń��j���[��߂��Ƃ��A�v���I�����[�h�ֈڍs�Ƃ�����
  }
  else{
    //���펞�Ȃ�Ύ�M�o�b�t�@����f�[�^�擾�Ȃǂ��s��
    //�A�b�v���[�h�̏ꍇ�͓��ɕK�v�Ȃ�

  //  int GDS_RAP_RESPONSE_Boxshot_Download_RecvPtr_Set(GDS_RAP_WORK *gdsrap, BOX_SHOT_RECV **box_array, int array_max);
  }
}

#endif //PM_DEBUG +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
