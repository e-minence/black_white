//==============================================================================
/**
 * @file  game_start.c
 * @brief �u�������傩��vor�u�Â�����v��I����̏���
 * @author  matsuda
 * @date  2009.01.07(��)
 */
//==============================================================================
#include <gflib.h>
#include <procsys.h>
#include "system\main.h"
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "title/game_start.h"
#include "title/title.h"
#include "savedata/situation.h"
#include "savedata/c_gear_data.h"
#include "app/name_input.h"
#include "test/testmode.h"
#include "select_moji_mode.h"
#include "debug_message.naix"
#include "msg/debug/msg_debugname.h"
#include "net_app/union/union_beacon_tool.h"
#include "system/gfl_use.h"
#include "demo/demo3d.h"
#include "field/zonedata.h"

#include "arc_def.h"  //ARCID_MESSAGE

#include "../../resource/fldmapdata/zonetable/zone_id.h"  // for ZONE_ID_T01

#include "savedata/save_control.h" // for
#include "savedata/save_control_intr.h" // for
#include "savedata/config.h"

//BGM�������ɂ���
#include "sound/sound_manager.h"

#include "sound/pm_voice.h"   // for 
#include "savedata/save_outside.h"

//==============================================================================
//  
//==============================================================================
FS_EXTERN_OVERLAY(title);
FS_EXTERN_OVERLAY(outside_save);

typedef enum
{
  CONTINUE_MODE_SELECT,
  CONTINUE_MODE_COMM_OFF,
  CONTINUE_MODE_COMM_ON,
}CONTINUE_MODE_TYPE;

//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GFL_PROC_RESULT GameStart_FirstProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GameStart_FirstProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT GameStart_FirstProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GameStart_ContinueProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GameStart_ContinueProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GameStart_ContinueProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GameStart_DebugProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GameStart_DebugProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GameStart_DebugProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GameStart_DebugSelectNameProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GameStart_DebugSelectNameProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GameStart_DebugSelectNameProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static void _mystatus_info_set( MYSTATUS *my, STRBUF *name, int sex );


//==============================================================================
//  �f�[�^
//==============================================================================
///�ŏ�����n�߂�
static const GFL_PROC_DATA GameStart_FirstProcData = {
  GameStart_FirstProcInit,
  GameStart_FirstProcMain,
  GameStart_FirstProcEnd,
};

///��������n�߂�
static const GFL_PROC_DATA GameStart_ContinueProcData = {
  GameStart_ContinueProcInit,
  GameStart_ContinueProcMain,
  GameStart_ContinueProcEnd,
};

///�f�o�b�O�J�n
static const GFL_PROC_DATA GameStart_DebugProcData = {
  GameStart_DebugProcInit,
  GameStart_DebugProcMain,
  GameStart_DebugProcEnd,
};

///����߂����񂽂��J�n
static const GFL_PROC_DATA GameStart_DebugSelectNameProcData = {
  GameStart_DebugSelectNameProcInit,
  GameStart_DebugSelectNameProcMain,
  GameStart_DebugSelectNameProcEnd,
};

//==============================================================================
//  �萔��`
//==============================================================================
enum{
  DEBUG_COMM_SEARCH_ONLY,
  DEBUG_COMM_ON,
  DEBUG_COMM_OFF,
};

//==============================================================================
//
//  
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �u�ŏ�����n�߂�v��I��
 */
//--------------------------------------------------------------
void GameStart_Beginning(void)
{
  GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &GameStart_FirstProcData, NULL);
}

//--------------------------------------------------------------
/**
 * @brief   �u��������n�߂�v��I��
 */
//--------------------------------------------------------------
void GameStart_Continue(void)
{
  GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &GameStart_ContinueProcData, (void*)CONTINUE_MODE_SELECT);
}

//--------------------------------------------------------------
/**
 * @brief   �u��������n�߂�(�ʐM�Ȃ�)�v��I��
 */
//--------------------------------------------------------------
void GameStart_ContinueNetOff(void)
{
  GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &GameStart_ContinueProcData, (void*)CONTINUE_MODE_COMM_OFF);
}

//--------------------------------------------------------------
/**
 * @brief   �u��������n�߂�(�펞�ʐM)�v��I��
 */
//--------------------------------------------------------------
void GameStart_ContinueNet(void)
{
  GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &GameStart_ContinueProcData, (void*)CONTINUE_MODE_COMM_ON);
}

//--------------------------------------------------------------
/**
 * @brief   �u�f�o�b�O�J�n�v��I��
 */
//--------------------------------------------------------------
void GameStart_Debug(void)
{
#ifdef PM_DEBUG
  GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &GameStart_DebugProcData, (void*)DEBUG_COMM_SEARCH_ONLY);
#endif
}

//--------------------------------------------------------------
/**
 * @brief   �u�f�o�b�O�J�n(�펞�ʐM)�v��I��
 */
//--------------------------------------------------------------
void GameStart_DebugNet(void)
{
#ifdef PM_DEBUG
  GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &GameStart_DebugProcData, (void*)DEBUG_COMM_ON);
#endif
}

//--------------------------------------------------------------
/**
 * @brief   �u�f�o�b�O�J�n(�ʐMOFF)�v��I��
 */
//--------------------------------------------------------------
void GameStart_DebugNetOff(void)
{
#ifdef PM_DEBUG
  GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &GameStart_DebugProcData, (void*)DEBUG_COMM_OFF);
#endif
}

//--------------------------------------------------------------
/**
 * @brief   testmenu�́u����߂����񂽂��v��I��
 */
//--------------------------------------------------------------
void GameStart_Debug_SelectName(void)
{
#ifdef PM_DEBUG
  GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &GameStart_DebugSelectNameProcData, NULL);
#endif
}



//==============================================================================
//
//  �ŏ�����n�߂�
//
//==============================================================================
#include "demo/intro.h"
typedef struct
{
  SELECT_MODE_INIT_WORK selModeParam;
  INTRO_PARAM introParam;
  NAMEIN_PARAM *nameInParam;
  INTR_SAVE_CONTROL* intr_save;
  GFL_PROCSYS* procsys_up;
  SOUNDMAN_PRESET_HANDLE* bgm_handle;
  DEMO3D_PARAM demo3dParam;
  u32 voice_load_id;
  MYSTATUS *mystatus;
  CONFIG *config;
}GAMESTART_FIRST_WORK;

#define USE_INTRSAVE //INTRSAVE�L�������؂�ւ��t���O

//--------------------------------------------------------------
/**
 * @brief   
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT GameStart_FirstProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  GAMESTART_FIRST_WORK*   work;
  SAVE_CONTROL_WORK *sv_ctrl = SaveControl_GetPointer();
  
  // ���[�N �A���P�[�g
  work = GFL_PROC_AllocWork( proc , sizeof(GAMESTART_FIRST_WORK) , GFL_HEAPID_APP );
  
  // �Z�[�u�V�X�e���쐬
#ifdef USE_INTRSAVE
  //�����Z�[�u�̗L���𒲂ׂ�ׁA�Z�[�u�f�[�^�̃��[�N�G���A�̏����������������Ɏ��s
  work->intr_save = IntrSave_Init( GFL_HEAPID_APP, sv_ctrl );
#endif

  // �Z�[�u�f�[�^:���[�N�G���A�̂̏�����
  SaveControl_ClearData( sv_ctrl );
  
  work->mystatus = MyStatus_AllocWork(GFL_HEAPID_APP);
  work->config = CONFIG_AllocWork(GFL_HEAPID_APP);

  // SELMODE ������
  work->selModeParam.type       = SMT_START_GAME;
  work->selModeParam.configSave = work->config;
  work->selModeParam.mystatus   = work->mystatus;

  //��l���̐��ʂ́A���ʐݒ肪�I����Ă�������
  work->nameInParam = NAMEIN_AllocParam( GFL_HEAPID_APP , NAMEIN_MYNAME , 0, 0, NAMEIN_PERSON_LENGTH , NULL, NULL );

  work->procsys_up = GFL_PROC_LOCAL_boot( GFL_HEAPID_APP );

  //BGM�n���h��
  //�C���g���f�����ɖ炷���̂����A
  //�����Ń�������ɂ����Ă����A�����Ȃ��ƁA
  //�C���g���f�����̃A�v���I�����Ƀ��������J������Ă��܂��A���O���͒���BGM���Ȃ�Ȃ�
  { 
    u32 se_tbl[30]; //�K��
    u32 namein_se_num;
    u32 intro_se_num;
    u32 i = 0;

    GFL_OVERLAY_Load( FS_OVERLAY_ID(namein) );  
    namein_se_num = NAMEIN_SE_PresetNum;
    for( i = 0; i < namein_se_num; i++ )
    { 
      se_tbl[i] = NAMEIN_SE_PresetData[i];
    }
    GFL_OVERLAY_Unload( FS_OVERLAY_ID(namein) );
    
    GFL_OVERLAY_Load( FS_OVERLAY_ID(intro) ); 
    intro_se_num = Intro_Se_PresetNum;
    for( i = 0; i < intro_se_num; i++ )
    { 
      se_tbl[i+namein_se_num] = Intro_Se_PresetData[i];
    }
    GFL_OVERLAY_Unload( FS_OVERLAY_ID(intro) );

    work->bgm_handle  = SOUNDMAN_PresetSoundTbl( se_tbl, namein_se_num + intro_se_num );

    work->voice_load_id = PMVOICE_LoadOnly( MONSNO_TIRAAMHI, 0, 64, FALSE, 0, 0, FALSE, NULL );
  }

  // �C���g���f���̃p�����[�^������
  work->introParam.mystatus   = work->mystatus;
  work->introParam.config     = work->config;
  work->introParam.scene_id   = INTRO_SCENE_ID_00;
  work->introParam.intr_save  = work->intr_save;
  work->introParam.voice_load_id = work->voice_load_id;

  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT GameStart_FirstProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  enum 
  {
    SEQ_INIT,
    SEQ_INTRO_WAIT,
    SEQ_INPUT_NAME,
    SEQ_INPUT_NAME_WAIT,
    SEQ_INPUT_NAME_RETAKE_YESNO,
    SEQ_INPUT_NAME_RETAKE_CHECK,
    SEQ_INPUT_NAME_RETAKE_CHECK_WAIT,
    SEQ_END,
  };
  
  GAMESTART_FIRST_WORK *work = mywk;
  GFL_PROC_MAIN_STATUS up_status;

#ifdef USE_INTRSAVE
  IntrSave_Main( work->intr_save );
#endif

  up_status = GFL_PROC_LOCAL_Main( work->procsys_up );

  switch(*seq){
  case SEQ_INIT:
    //�����I��
//  GFL_PROC_SysCallProc(NO_OVERLAY_ID, &SelectModeProcData,&work->selModeParam);
    //�C���g���f��
    GFL_PROC_LOCAL_CallProc( work->procsys_up, FS_OVERLAY_ID(intro), &ProcData_Intro, &work->introParam );
//  GFL_PROC_SysCallProc(FS_OVERLAY_ID(intro), &ProcData_Intro, &work->introParam );
    (*seq) = SEQ_INTRO_WAIT;
    break;
  case SEQ_INTRO_WAIT :
    if( up_status != GFL_PROC_MAIN_VALID )
    {
      (*seq) = SEQ_INPUT_NAME;
    }
    break;
  case SEQ_INPUT_NAME:
    //���O����
    work->nameInParam->hero_sex   = MyStatus_GetMySex(work->selModeParam.mystatus);
    work->nameInParam->p_intr_sv  = work->intr_save;
    //GFL_PROC_SysCallProc(FS_OVERLAY_ID(namein), &NameInputProcData,(void*)work->nameInParam);
    GFL_PROC_LOCAL_CallProc( work->procsys_up, FS_OVERLAY_ID(namein), &NameInputProcData, work->nameInParam );
    (*seq) = SEQ_INPUT_NAME_WAIT;
    break;
  case SEQ_INPUT_NAME_WAIT :
    if( up_status != GFL_PROC_MAIN_VALID )
    {
      (*seq) = SEQ_INPUT_NAME_RETAKE_YESNO;
    }
    break;
  case SEQ_INPUT_NAME_RETAKE_YESNO:
    //�������̃Z�b�g
    _mystatus_info_set( work->mystatus, work->nameInParam->strbuf, MyStatus_GetMySex(work->mystatus) );

    //�C���g���f�� ���O���͂̔���
    work->introParam.scene_id = INTRO_SCENE_ID_05_RETAKE_YESNO;
    
//  GFL_PROC_SysCallProc(FS_OVERLAY_ID(intro), &ProcData_Intro, &work->introParam );
    GFL_PROC_LOCAL_CallProc( work->procsys_up, FS_OVERLAY_ID(intro), &ProcData_Intro, &work->introParam );
    (*seq) = SEQ_INPUT_NAME_RETAKE_CHECK_WAIT;
    break;
  case SEQ_INPUT_NAME_RETAKE_CHECK_WAIT :
    if( up_status != GFL_PROC_MAIN_VALID )
    {
      (*seq) = SEQ_INPUT_NAME_RETAKE_CHECK;
    }
    break;
  case SEQ_INPUT_NAME_RETAKE_CHECK:
    // ���O���͕��A����
    if( work->introParam.retcode == INTRO_RETCODE_NORMAL )
    {
      (*seq) = SEQ_END;
    }
    else
    {
      // ���O���͂ɕ��A
      (*seq) = SEQ_INPUT_NAME;
    }
    break;
  case SEQ_END:
    // �I��
    return GFL_PROC_RES_FINISH;
    break;
  }

  return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------------------
/**
 * PROC Quit
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT GameStart_FirstProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  SAVE_CONTROL_WORK *sv_ctrl = SaveControl_GetPointer();
  GAMESTART_FIRST_WORK *work = mywk;
  GAME_INIT_WORK * init_param;
  VecFx32 pos = {0,0,0};
  
  SOUNDMAN_ReleasePresetData( work->bgm_handle );

  // �Z�[�u�V�X�e���폜
#ifdef USE_INTRSAVE
  IntrSave_Exit( work->intr_save );
#endif
  // Intro�f���ŕύX���Ă����f�[�^���Z�[�u�f�[�^�֔��f
  {
    MyStatus_Copy(work->mystatus, SaveData_GetMyStatus(sv_ctrl));
    CONFIG_Copy(work->config, SaveData_GetConfig(sv_ctrl));
  }
  GFL_HEAP_FreeMemory(work->mystatus);
  GFL_HEAP_FreeMemory(work->config);
  
  GFL_PROC_LOCAL_Exit( work->procsys_up );

  //�Ǘ��O�Z�[�u�Ƀf�[�^�����݂��Ă���Βʏ�Z�[�u�̈�ֈڂ�
  if(SaveData_GetOutsideExistFlag(sv_ctrl) == TRUE){
    OUTSIDE_SAVE_CONTROL *outsv;

    GFL_OVERLAY_Load( FS_OVERLAY_ID(outside_save) );

    outsv = OutsideSave_SystemLoad(GFL_HEAPID_APP);
    if(OutsideSave_GetExistFlag(outsv) == TRUE){  //�ꉞ�g�p�ӏ����O�ł��`�F�b�N
      OutsideSave_MysteryData_Outside_to_Normal(outsv, sv_ctrl);
    }
    OutsideSave_SystemUnload(outsv);

    GFL_OVERLAY_Unload( FS_OVERLAY_ID(outside_save) );
  }

  init_param = GAMEINIT_GetGameInitWork(GAMEINIT_MODE_FIRST, 0, &pos, 0 );

  NAMEIN_FreeParam(work->nameInParam);
  GFL_PROC_FreeWork( proc );
  
  GFL_PROC_SysSetNextProc(NO_OVERLAY_ID, &GameMainProcData, init_param);

  return GFL_PROC_RES_FINISH;
}



//==============================================================================
//
//  ��������n�߂�
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT GameStart_ContinueProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  GAMESTART_FIRST_WORK *work = GFL_PROC_AllocWork( proc , sizeof(GAMESTART_FIRST_WORK) , GFL_HEAPID_APP );
  work->selModeParam.type = SMT_CONTINUE_GAME;
  work->selModeParam.configSave = SaveData_GetConfig( SaveControl_GetPointer() );
  work->selModeParam.miscSave = SaveData_GetMisc( SaveControl_GetPointer() );
  work->selModeParam.mystatus = NULL;
  
  
  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT GameStart_ContinueProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  GAMESTART_FIRST_WORK *work = mywk;
  CONTINUE_MODE_TYPE continueType = (CONTINUE_MODE_TYPE)pwk;
  switch(*seq){
  case 0:
    //�ʐM�I��
    if( continueType == CONTINUE_MODE_SELECT )
    {
      GFL_PROC_SysCallProc(NO_OVERLAY_ID, &SelectModeProcData,&work->selModeParam);
    }
    else
    if( continueType == CONTINUE_MODE_COMM_OFF )
    {
      CONFIG_SetNetworkSearchMode( work->selModeParam.configSave, NETWORK_SEARCH_OFF );
    }
    else
    if( continueType == CONTINUE_MODE_COMM_ON )
    {
      CONFIG_SetNetworkSearchMode( work->selModeParam.configSave, NETWORK_SEARCH_ON );
    }
    (*seq)++;
    break;
  case 1:
    return GFL_PROC_RES_FINISH;
    break;
  }
  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Quit
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT GameStart_ContinueProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  GAMESTART_FIRST_WORK *work = mywk;
  GAME_INIT_WORK * init_param;
  PLAYERWORK_SAVE plsv;
  SAVE_CONTROL_WORK *sv_ctrl = SaveControl_GetPointer();
  
  // ���O�̑I�����őI�񂾒ʐM���[�h���擾����
  int search_mode_temp = CONFIG_GetNetworkSearchMode( work->selModeParam.configSave );
  // �^�C�g�����j���[�̍��ڏ��擾
  u8  title_menu_flag = MISC_GetStartMenuFlagAll( work->selModeParam.miscSave );

  SaveControl_Load(sv_ctrl);
  CONFIG_ApplyMojiMode( SaveData_GetConfig( sv_ctrl ) );  //���[�h�����R���t�B�O�ɏ]���ĕ������[�h�ݒ�
  SaveData_SituationLoad_PlayerWorkSave(sv_ctrl, &plsv);

  init_param = GAMEINIT_GetGameInitWork(
  GAMEINIT_MODE_CONTINUE, plsv.zoneID, &plsv.position, plsv.direction );
  
  GFL_PROC_FreeWork( proc );
  
  GFL_PROC_SysSetNextProc(NO_OVERLAY_ID, &GameMainProcData, init_param);

  // �Z�[�u�f�[�^�ŒʐM���[�h���㏑������Ă��܂��Ă���̂łƂ��Ă������ʐM���[�h���ēx�i�[����
  CONFIG_SetNetworkSearchMode( work->selModeParam.configSave, search_mode_temp );
  // �Z�[�u�f�[�^�Ń^�C�g�����j���[�̍��ڏ�񂪏㏑������Ă��܂��Ă���̂ōĐݒ�
  MISC_SetStartMenuFlagAll( work->selModeParam.miscSave, title_menu_flag );

  return GFL_PROC_RES_FINISH;

}



//==============================================================================
//
//  �f�o�b�O�X�^�[�g
//
//==============================================================================
extern BOOL DebugScanOnly;
//--------------------------------------------------------------
/**
 * @brief   
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT GameStart_DebugProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT GameStart_DebugProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Quit
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT GameStart_DebugProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
#ifdef PM_DEBUG
  GAME_INIT_WORK * init_param;
  VecFx32 pos = {0,0,0};
  int always_net;
  int sex = 0;
  SAVE_CONTROL_WORK *sv_ctrl = SaveControl_GetPointer();
  
  always_net = (int)pwk;   //TRUE:�펞�ʐM�Łu��������v

  SaveControl_ClearData(sv_ctrl);  //�Z�[�u�f�[�^�N���A
  
  {//���O�̃Z�b�g
    MYSTATUS    *myStatus;
    GFL_MSGDATA *msgman;
    STRBUF *namebuf;
    u32 msg_id;
    
    msgman = GFL_MSG_Create( 
      GFL_MSG_LOAD_NORMAL, ARCID_DEBUG_MESSAGE, NARC_debug_message_debugname_dat, GFL_HEAPID_APP );
#if 0
  #if PM_VERSION == VERSION_BLACK
    namebuf = GFL_MSG_CreateString( msgman, DEBUG_NAME_BLACK );
  #else
    namebuf = GFL_MSG_CreateString( msgman, DEBUG_NAME_WHITE );
  #endif
#else //�N���p�ɖ��O�Œ�ł͂Ȃ��A�����_���Ō��肷��悤�ɂ���
    if(GFUser_GetPublicRand(2) & 1){
      namebuf = GFL_MSG_CreateString( msgman, DEBUG_NAME_RAND_M_000 + GFUser_GetPublicRand(8) );
      sex = PM_MALE;
    }
    else{
      namebuf = GFL_MSG_CreateString( msgman, DEBUG_NAME_RAND_W_000 + GFUser_GetPublicRand(8) );
      sex = PM_FEMALE;
    }
#endif
    myStatus = SaveData_GetMyStatus( sv_ctrl );
    _mystatus_info_set(myStatus, namebuf, sex);

    
    GFL_STR_DeleteBuffer(namebuf);
    GFL_MSG_Delete(msgman);
  }
  { 
    //�펞�ʐM���[�h�̃Z�b�g
    CONFIG *config;
    NETWORK_SEARCH_MODE mode;

    config  = SaveData_GetConfig( sv_ctrl );
    switch(always_net){
    case DEBUG_COMM_SEARCH_ONLY:
      mode = NETWORK_SEARCH_ON;
      DebugScanOnly = TRUE;
      break;
    case DEBUG_COMM_ON:
      mode = NETWORK_SEARCH_ON;
      DebugScanOnly = FALSE;
      break;
    case DEBUG_COMM_OFF:
    default:
      mode = NETWORK_SEARCH_OFF;
      DebugScanOnly = FALSE;
      break;
    }
    CONFIG_SetNetworkSearchMode( config, mode );
    //CGEARON
    CGEAR_SV_SetCGearONOFF(CGEAR_SV_GetCGearSaveData(sv_ctrl),TRUE);
  }
  
  init_param = GAMEINIT_GetGameInitWork(GAMEINIT_MODE_DEBUG, ZONE_ID_T01, &pos, 0 );
  GFL_PROC_SysSetNextProc(NO_OVERLAY_ID, &GameMainProcData, init_param);
#endif

  return GFL_PROC_RES_FINISH;
}


//==============================================================================
//
//  ����߂����񂽂��ŃX�^�[�g
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT GameStart_DebugSelectNameProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
#ifdef PM_DEBUG
  TESTMODE_PROC_WORK *work;
  work = GFL_PROC_AllocWork( proc, sizeof(TESTMODE_PROC_WORK), GFL_HEAPID_APP );
  work->startMode_ = TESTMODE_NAMESELECT;
  work->work_ = (void*)0;
  SaveControl_ClearData(SaveControl_GetPointer());  //�Z�[�u�f�[�^�N���A
#endif
  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT GameStart_DebugSelectNameProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
#ifdef PM_DEBUG
  FS_EXTERN_OVERLAY(testmode);

  switch(*seq){
  case 0:
    GFL_PROC_SysCallProc(FS_OVERLAY_ID(testmode), &TestMainProcData, (void*)mywk);
    (*seq)++;
    break;
  case 1:
    return GFL_PROC_RES_FINISH;
  }
#endif
  return GFL_PROC_RES_CONTINUE;

}

//--------------------------------------------------------------------------
/**
 * PROC Quit
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT GameStart_DebugSelectNameProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
#ifdef PM_DEBUG
  TESTMODE_PROC_WORK *work = mywk;
  SAVE_CONTROL_WORK *sv_ctrl = SaveControl_GetPointer();
  
  if( (int)work->work_ == 0 )
  {
    GAME_INIT_WORK * init_param;
    VecFx32 pos = {0,0,0};
    
    { 
      //�펞�ʐM���[�h�̃Z�b�g
      CONFIG *config;

      config  = SaveData_GetConfig( sv_ctrl );
      CONFIG_SetNetworkSearchMode( config, NETWORK_SEARCH_OFF );
      DebugScanOnly = TRUE;
    }
    //CGEARON
    CGEAR_SV_SetCGearONOFF(CGEAR_SV_GetCGearSaveData(sv_ctrl),TRUE);
    
    init_param = GAMEINIT_GetGameInitWork(GAMEINIT_MODE_DEBUG, 0, &pos, 0 );
    GFL_PROC_SysSetNextProc(NO_OVERLAY_ID, &GameMainProcData, init_param);
  }
  else
  {
    GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(testmode), &TestMainProcData, NULL );
  }
  GFL_PROC_FreeWork(proc);
#endif
  return GFL_PROC_RES_FINISH;
}


//----------------------------------------------------------------------------------
/**
 * @brief MYSTATUS�ɏ���o�^����
 *
 * @param   my    MYSTATUS
 * @param   name  ���͍ς̎����̖��O�̕�����
 * @param   sex   ����(PM_MALE, PM_FEMALE)
 */
//----------------------------------------------------------------------------------
static void _mystatus_info_set( MYSTATUS *my, STRBUF *name, int sex )
{
  // ���O
  MyStatus_SetMyNameFromString( my , name );
  // ����
  MyStatus_SetMySex(my, sex);
  // �g���[�i�[ID
  MyStatus_SetID(my, GFUser_GetPublicRand0(0xFFFFFFFF));
  // ���j�I�������ڐݒ�
  MyStatus_SetTrainerView( my, 
                           GFL_STD_MtRand(MYSTATUS_UNIONVIEW_MAX)+
                           MyStatus_GetMySex(my)*MYSTATUS_UNIONVIEW_MAX );
  OS_Printf("TrainerView = %d\n", MyStatus_GetTrainerView( my ));
  
}

#ifdef  PLAYABLE_VERSION
//----------------------------------------------------------------------------------
/**
 * @brief ���V��o�[�W�����p����������
 */
//----------------------------------------------------------------------------------
void GameStart_Playable( void )
{
  SAVE_CONTROL_WORK *sv_ctrl = SaveControl_GetPointer();

  SaveControl_ClearData(sv_ctrl);  //�Z�[�u�f�[�^�N���A
  {//���O�̃Z�b�g
    MYSTATUS    *myStatus;
    GFL_MSGDATA *msgman;
    STRBUF *namebuf;
    int sex = 0;
    
    msgman = GFL_MSG_Create( 
      GFL_MSG_LOAD_NORMAL, ARCID_DEBUG_MESSAGE, NARC_debug_message_debugname_dat, GFL_HEAPID_APP );
    if(GFUser_GetPublicRand(2) & 1){
      namebuf = GFL_MSG_CreateString( msgman, DEBUG_NAME_BLACK );
      sex = PM_MALE;
    } else{
      namebuf = GFL_MSG_CreateString( msgman, DEBUG_NAME_WHITE );
      sex = PM_FEMALE;
    }
    myStatus = SaveData_GetMyStatus( sv_ctrl );
    _mystatus_info_set(myStatus, namebuf, sex);

    
    GFL_STR_DeleteBuffer(namebuf);
    GFL_MSG_Delete(msgman);
  }
  {
    GAME_INIT_WORK * init_param;
    VecFx32 pos = {0,0,0};
    
    { 
      //�펞�ʐM���[�h�̃Z�b�g
      CONFIG *config;

      config  = SaveData_GetConfig( sv_ctrl );
      CONFIG_SetNetworkSearchMode( config, NETWORK_SEARCH_ON );
      //DebugScanOnly = TRUE;
    }
    //CGEARON
    CGEAR_SV_SetCGearONOFF(CGEAR_SV_GetCGearSaveData(sv_ctrl),TRUE);
    
    init_param = GAMEINIT_GetGameInitWork(GAMEINIT_MODE_FIRST, 0, &pos, 0 );
    GFL_PROC_SysSetNextProc(NO_OVERLAY_ID, &GameMainProcData, init_param);
  }
}
#endif

