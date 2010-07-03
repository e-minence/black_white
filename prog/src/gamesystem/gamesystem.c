//============================================================================================
/**
 * @file  gamemsystem.c
 * @brief �|�P�����Q�[���V�X�e���i�������ʏ�Q�[���̃g�b�v���x���j
 * @author  tamada GAME FREAK Inc.
 * @date  08.10.20
 *
 */
//============================================================================================
#include <gflib.h>

#include "system/main.h"    //HEAPID�̎Q�Ɨp

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_data.h"
#include "gamesystem/playerwork.h"
#include "gamesystem/game_event.h"
#include "gamesystem/iss_sys.h"
#include "gamesystem/game_beacon.h"

#include "gamesystem/game_init.h"

#include "src/field/event_gamestart.h"
#include "src/field/field_sound_system.h"
#include "system/net_err.h"

#include "field/zonedata.h"

#include "system/playtime_ctrl.h"  //PLAYTIMECTRL_

#include "savedata/c_gear_data.h" //CGEAR_SV_GetCGearONOFF
#include "savedata/misc.h"
#include "field/field_comm/intrude_work.h"  //Intrude_Check_AlwaysBoot
#include "field/fieldmap_call.h"

#include "amprotect.h"


//============================================================================================
//============================================================================================
#ifdef PM_DEBUG

static void heapLeakCheck( void );

//�f�o�b�O�p�ϐ�
u32 DbgFldHeapRest = 0xffffffff;
u32 DbgFldHeapRestAlctbl = 0xffffffff;
u32 DbgVramRest = 0xffffffff;
u32 DbgFldHeapUseMaxZone = 0;
u32 DbgFldHeapUseMaxZoneAlctbl = 0;
u32 DbgVramUseMaxZone = 0;

#endif

//============================================================================================
//============================================================================================
enum {
  /**
   * @brief �i�����pHEAP�̈�
   */
  HEAPSIZE_WORLD = 0x10000 - 0x6000,

  /**
   * @brief �C�x���g�pHEAP�̈�
   *
   * �ʂ̃C�x���g���ƂɊm�ۂ��A�C�x���g�I���ŉ������郁������
   * ����HEAP����m�ۂ���
   */
  HEAPSIZE_PROC = 0x10000,

  /**
   * @brief �C�x���g�R���g���[���[�pHEAP�̈�
   * ��ɂł͂Ȃ����A�C�x���g�ƃC�x���g���܂����悤�Ȍ`��
   * ���݂��郁�����i���j�I�����[����o�g���^���[�̃R���g���[���j��
   * ����HEAP����擾����
   */
  HEAPSIZE_APP_CONTROL  =      (0x1400),

};

//------------------------------------------------------------------
/**
 * @brief �Q�[������V�X�e���p���[�N�ێ��ϐ�
 *
 * �����Ԃ�f�o�b�O�p�����莞�̃O���[�o���Q�Ɨp�ɂ����g��Ȃ��͂�
 */
//------------------------------------------------------------------
static GAMESYS_WORK * GameSysWork;

//------------------------------------------------------------------
/**
 * @brief �Q�[���������p�����[�^�ێ��ϐ�
 *
 */
//------------------------------------------------------------------
static GAME_INIT_WORK GameInitWork;

//============================================================================================
//============================================================================================

static void GameSystem_Init(GAMESYS_WORK * gsys, HEAPID heapID, GAME_INIT_WORK * init_param);
static BOOL GameSystem_Main(GAMESYS_WORK * gsys);
static void GameSystem_End(GAMESYS_WORK * gsys);
static u32 GAMESYS_WORK_GetSize(void);
static void GameSystem_SetBatt10SleepCallback( GAMESYS_WORK *gsys );
static void GameSystem_ResetBatt10SleepCallback( GAMESYS_WORK *gsys );
static void GameSystem_SetNotSleepCallback( GAMESYS_WORK *gsys );
static void GameSystem_ResetNotSleepCallback( GAMESYS_WORK *gsys );

static void GameSystem_UpdateDoEvent( GAMESYS_WORK * gsys );
static void GameSysmte_FieldAlwaysBootWatch(GAMESYS_WORK *gsys);


//============================================================================================
/**
 * @brief �Q�[���V�X�e���p�v���Z�X
 *
 * NitroMain->GFLUser_Main->GFL_PROC_Main()�ɓo�^�����B
 * ��{�I�ɂ̓Q�[���{�ҊJ�n��͂��̃v���Z�X�����삵������͂�
 */
//============================================================================================
static GFL_PROC_RESULT GameMainProcInit(GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT GameMainProcMain(GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT GameMainProcEnd(GFL_PROC * proc, int * seq, void * pwk, void * mywk);

FS_EXTERN_OVERLAY(amprotect);


// �}�W�R���΍􏈗����̂P
#ifdef AMPROTECT_FUNC
static void magicon1_failure_func(void);
static void magicon2_need_func(void);
static void magicon3_failure_func(void);
void magicon_dummy_task(GFL_TCB *tcb, void *pWork);

int debug_tcb_num=0;
void magicon_dummy_task( GFL_TCB *tcb, void *pWork )
{
  //�Ȃ񂩓K���ɁB
  u32 dummy1=GFL_STD_MtRand(GFL_STD_RAND_MAX),dummy2=GFL_STD_MtRand(GFL_STD_RAND_MAX);
  dummy1++;
  dummy2 += dummy1;
}  


// ���o�֐�A1��TRUE�̎��Ɏ��s����܂��i���j��H��j
static void magicon1_failure_func(void){
  GFL_TCB *dust_tcb;
   dust_tcb = GFUser_VIntr_CreateTCB( magicon_dummy_task, NULL, 127 );
   debug_tcb_num++;
}

// ���o�֐�A2��FALSE�̎��Ɏ��s����܂��i���K�v�ȏ����j
static void magicon2_need_func(void){
  u32 work_size;
  GFL_HEAP_CreateHeap( HEAPID_EXTRA, HEAPID_APP_CONTROL, HEAPSIZE_APP_CONTROL );
  work_size = GAMESYS_WORK_GetSize();
  GFL_HEAP_DeleteHeap( HEAPID_APP_CONTROL );

}

// ���o�֐�A3��TRUE�̎��Ɏ��s����܂��i��΂�TRUE�ɂȂ�Ȃ��̂ŉ��������Ă������j
static void magicon3_failure_func(void)
{
  // ���s����Ȃ��񂾂��ǉR�����������Ēu��
  GFL_TCB *dust_tcb;
  dust_tcb = GFUser_VIntr_CreateTCB( magicon_dummy_task, NULL, 127 );
  debug_tcb_num++;
}



#endif

//------------------------------------------------------------------
//------------------------------------------------------------------
static GFL_PROC_RESULT GameMainProcInit(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  GAMESYS_WORK * gsys;
  u32 work_size = GAMESYS_WORK_GetSize();
  GAME_INIT_WORK *game_init = pwk;
  u8 flag=0;
  u32 magicon = 2677*4783;  //�f�����f��
  
// �}�W�R���΍􏈗����̂P
#ifdef AMPROTECT_FUNC
  GFL_OVERLAY_Load( FS_OVERLAY_ID(amprotect) );  

  magicon+=(71*AM_IsMagiconA1(magicon1_failure_func));
  //�Q�[�����A�i���I�ɕێ�����K�v�̂���f�[�^�͂���HEAP��
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WORLD, HEAPSIZE_WORLD );

  magicon+=(401*(!AM_IsNotMagiconA2(magicon2_need_func)));
  //�Q�[���C�x���g���ɐ�������ׂ��������i�C�x���g�I���ŉ���j�̏ꍇ�͂���HEAP��
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_PROC, HEAPSIZE_PROC );
  flag=AM_IsMagiconA3(magicon3_failure_func);
  //�p���X���Ȃ���퓬���t�B�[���h������̂ł����Ŋm��
  GFL_HEAP_CreateHeap( HEAPID_EXTRA, HEAPID_APP_CONTROL, HEAPSIZE_APP_CONTROL );
  

  GFL_OVERLAY_Unload( FS_OVERLAY_ID(amprotect) );  

#else
  //�Q�[�����A�i���I�ɕێ�����K�v�̂���f�[�^�͂���HEAP��
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WORLD, HEAPSIZE_WORLD );
  //�Q�[���C�x���g���ɐ�������ׂ��������i�C�x���g�I���ŉ���j�̏ꍇ�͂���HEAP��
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_PROC, HEAPSIZE_PROC );
  //�p���X���Ȃ���퓬���t�B�[���h������̂ł����Ŋm��
  GFL_HEAP_CreateHeap( HEAPID_EXTRA, HEAPID_APP_CONTROL, HEAPSIZE_APP_CONTROL );
#endif
  gsys = GFL_PROC_AllocWork(proc, work_size, HEAPID_WORLD);
  GFL_STD_MemClear(gsys, work_size);
  GameSysWork = gsys;
  GameSystem_Init(gsys, HEAPID_WORLD, game_init);

  { //�Q�[���J�n�C�x���g�̐���
    GMEVENT * event = EVENT_GameStart(gsys, game_init);
    GAMESYSTEM_SetEvent(gsys, event);
  }
  
#ifdef AMPROTECT_FUNC
  if(magicon%4783){
    int i;
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
    MISC *misc         = GAMEDATA_GetMiscWork(gamedata);
    int badge          = MISC_GetBadgeCount(misc);
    for(i=0;i<badge;i++){
      magicon1_failure_func();
    }
  }
#endif
  return GFL_PROC_RES_FINISH;
}

#ifdef DEBUG_ONLY_FOR_hudson

#include "debug/debug_hudson.h"
#include "field/event_debug_menu.h" // for EVENT_DEBUG_AllMapCheck
#include "field/event_debug_all_connect_check.h" // for EVENT_DEBUG_AllConnectCheck
#include "field/event_debug_btl_all_waza_check.h" // for EVENT_DEBUG_BtlAllWazaCheck
#include "field/event_debug_btl_all_waza_check2.h" // for EVENT_DEBUG_BtlAllWazaCheck2
#include "field/event_debug_btl_all_poke_check.h" // for EVENT_DEBUG_BtlAllPokeCheck
#include "field/event_debug_btl_all_waza_cam_check.h" // for EVENT_DEBUG_BtlAllWazaCamCheck

#include "savedata/config.h"

static void HudsonMain( GAMESYS_WORK* gsys )
{
  static int debugcnt = 0;
  
#if 0
  if( OS_GetArgc() < 2 )
  {
    return;
  }
#endif

  if( debugcnt < 180 )
  {
    debugcnt++;
  }
  else if( debugcnt == 180 )
  {
    debugcnt++;

    //>>��������e�X�g���[�h�؂�ւ�
    
    // �S�}�b�v�`�F�b�N
    if ( HUDSON_IsTestCode( HUDSON_TESTCODE_MAP_JUMP ) )
    {
      GMEVENT * new_event;

      new_event = EVENT_DEBUG_AllMapCheck( gsys, FALSE );
      GAMESYSTEM_SetEvent( gsys, new_event );
    }
    // �S�ڑ��`�F�b�N
    else if ( HUDSON_IsTestCode( HUDSON_TESTCODE_ALL_CONNECT ) )
    {
      GMEVENT * new_event;

      new_event = GMEVENT_CreateOverlayEventCall( gsys, 
          FS_OVERLAY_ID( debug_connect_check ), EVENT_DEBUG_AllConnectCheck, NULL );
      GAMESYSTEM_SetEvent( gsys, new_event );
    }
    // �S�Z�`�F�b�N
    else if( HUDSON_IsTestCode( HUDSON_TESTCODE_ALL_WAZA ) )
    {
      GMEVENT * new_event;

#if 0      
      {
        // �Z�G�t�F�N�gOFF
        GAMEDATA* gdata = GAMESYSTEM_GetGameData( gsys );
        SAVE_CONTROL_WORK* sv = GAMEDATA_GetSaveControlWork( gdata );
        CONFIG* cfg = SaveData_GetConfig( sv ); 
        CONFIG_SetWazaEffectMode( cfg, WAZAEFF_MODE_OFF );
      }
#endif

      new_event = GMEVENT_CreateOverlayEventCall( gsys, 
          FS_OVERLAY_ID( debug_all_waza_check ), EVENT_DEBUG_BtlAllWazaCheck, NULL );
      GAMESYSTEM_SetEvent( gsys, new_event );
    }
    // �S�Z�`�F�b�N2(�g���v���o�g��)
    else if( HUDSON_IsTestCode( HUDSON_TESTCODE_ALL_WAZA2 ) )
    {
      GMEVENT * new_event;

      new_event = GMEVENT_CreateOverlayEventCall( gsys, 
          FS_OVERLAY_ID( debug_all_waza_check2 ), EVENT_DEBUG_BtlAllWazaCheck2, NULL );
      GAMESYSTEM_SetEvent( gsys, new_event );
    }
    // �S�|�P�����`�F�b�N
    else if( HUDSON_IsTestCode( HUDSON_TESTCODE_ALL_POKE ) )
    {
      GMEVENT * new_event;

      new_event = GMEVENT_CreateOverlayEventCall( gsys, 
          FS_OVERLAY_ID( debug_all_poke_check ), EVENT_DEBUG_BtlAllPokeCheck, NULL );
      GAMESYSTEM_SetEvent( gsys, new_event );
    }
    // �S�Z�J�����`�F�b�N
    else if( HUDSON_IsTestCode( HUDSON_TESTCODE_ALL_WAZA_CAM ) )
    {
      GMEVENT * new_event;

      new_event = GMEVENT_CreateOverlayEventCall( gsys, 
          FS_OVERLAY_ID( debug_all_waza_cam_check ), EVENT_DEBUG_BtlAllWazaCamCheck, NULL );
      GAMESYSTEM_SetEvent( gsys, new_event );
    }

  }

}
#endif // DEBUG_ONLY_FOR_hudson

//------------------------------------------------------------------
//------------------------------------------------------------------
static GFL_PROC_RESULT GameMainProcMain(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  GAMESYS_WORK * gsys = mywk;

#ifdef DEBUG_ONLY_FOR_hudson
  HudsonMain( gsys );
#endif // DEBUG_ONLY_FOR_hudson

  if (GameSystem_Main(gsys)) {
    return GFL_PROC_RES_FINISH;
  } else {
    return GFL_PROC_RES_CONTINUE;
  }
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static GFL_PROC_RESULT GameMainProcEnd(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  GAMESYS_WORK * gsys = mywk;
  GameSystem_End(gsys);
  GFL_PROC_FreeWork(proc);
  GFL_HEAP_DeleteHeap( HEAPID_WORLD );
  GFL_HEAP_DeleteHeap( HEAPID_PROC );
  GFL_HEAP_DeleteHeap( HEAPID_APP_CONTROL );
  OS_ResetSystem( 0 );
  return GFL_PROC_RES_FINISH;
}
//------------------------------------------------------------------
/**
 * @brief   �v���Z�X�֐��e�[�u��
 */
//------------------------------------------------------------------
const GFL_PROC_DATA GameMainProcData = {
  GameMainProcInit,
  GameMainProcMain,
  GameMainProcEnd,
};


//------------------------------------------------------------------
/**
 * @file  �Q�[���J�n�������p���[�N�擾
 */
//------------------------------------------------------------------
GAME_INIT_WORK * GAMEINIT_GetGameInitWork(GAMEINIT_MODE mode, u16 mapid, VecFx32 *pos, s16 dir )
{
  GFL_STD_MemClear(&GameInitWork, sizeof(GAME_INIT_WORK));
  GameInitWork.mode = mode;
  GameInitWork.mapid = mapid;
  GameInitWork.pos = *pos;
  GameInitWork.dir = dir;
  return &GameInitWork;
}


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief �Q�[������V�X�e���p���[�N��`
 */
//------------------------------------------------------------------
struct _GAMESYS_WORK {
  HEAPID heapID;      ///<�g�p����q�[�v�w��ID
  GAME_INIT_WORK * init_param;    ///<�e�i�Ăяo�����j��������p�������[�N�ւ̃|�C���^

  GFL_PROCSYS * procsys;  ///<�g�p���Ă���PROC�V�X�e���ւ̃|�C���^

  GFL_PROC_MAIN_STATUS proc_result;

  EVCHECK_FUNC evcheck_func;
  void * evcheck_context;
  GMEVENT * event;

  GAMEDATA * gamedata;
  FIELDMAP_WORK * fieldmap;
  GAME_COMM_SYS_PTR game_comm;    ///<�Q�[���ʐM�Ǘ����[�N�ւ̃|�C���^
  void * comm_infowin;    ///<INFOWIN�ʐM���[�N

  ISS_SYS * iss_sys;    // ISS�V�X�e��
  
  u8 field_comm_error_req;      ///<TRUE:�t�B�[���h�ł̒ʐM�G���[��ʕ\�����N�G�X�g
  u8 do_event;    ///< �C�x���g����`�F�b�N
  u8 comm_off_event_flag;     ///<�ʐM�s���C�x���g�t���O
  u8 batt10sleep:1;      ///<  �P�O���ȉ��̃o�b�e���[�ŃX���[�v�ɓ������ꍇ�̒ʐM���[�h�����t���O
  u8 dummy:7;
};

//------------------------------------------------------------------
//------------------------------------------------------------------
static u32 GAMESYS_WORK_GetSize(void)
{
  return sizeof(GAMESYS_WORK);
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void GAMESYS_WORK_Init(GAMESYS_WORK * gsys, HEAPID heapID, GAME_INIT_WORK * init_param)
{
  gsys->heapID = heapID;
  gsys->init_param = init_param;
  gsys->procsys = GFL_PROC_LOCAL_boot(gsys->heapID);
  gsys->proc_result = GFL_PROC_MAIN_NULL;
  gsys->evcheck_func = NULL;
  gsys->evcheck_context = NULL;
  gsys->event = NULL;

  gsys->gamedata = GAMEDATA_Create(gsys->heapID);
  gsys->fieldmap = NULL;
  gsys->game_comm = GameCommSys_Alloc(gsys->heapID, gsys->gamedata);
  gsys->comm_infowin = NULL;
  gsys->iss_sys = ISS_SYS_Create( gsys->gamedata, heapID );

  //�ʐM�s���C�x���g�t���OOFF�ŏ�����
  gsys->comm_off_event_flag = FALSE;
  gsys->batt10sleep = FALSE;
  GameSystem_SetBatt10SleepCallback( gsys );
  GameSystem_SetNotSleepCallback( gsys );
  
  ZONEDATA_Open( heapID );
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void GAMESYS_WORK_Delete(GAMESYS_WORK * gsys)
{
  GameSystem_ResetNotSleepCallback( gsys );
  GameSystem_ResetBatt10SleepCallback( gsys );
  ISS_SYS_Delete(gsys->iss_sys);
  GAMEDATA_Delete(gsys->gamedata);
  GameCommSys_Free(gsys->game_comm);
  ZONEDATA_Close();        // �]�[���f�[�^�̃A�[�J�C�u�n���h�������
}


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static void GameSystem_Init(GAMESYS_WORK * gsys, HEAPID heapID, GAME_INIT_WORK * init_param)
{
  GAMESYS_WORK_Init(gsys, heapID, init_param);
  GAMEBEACON_Setting(gsys->gamedata);
  PLAYTIMECTRL_Init(); //�v���C���ԃJ�E���g������������
}

//------------------------------------------------------------------
/**
 * @return BOOL TRUE�̂Ƃ��A�Q�[���I��
 */
//------------------------------------------------------------------
static BOOL GameSystem_Main(GAMESYS_WORK * gsys)
{
  //Game Server Proccess

  // ���t���[���̃C�x���g����t���O�X�V
  // �P�t���[���̓r���ŁA�C�x���g��Ԃ��ς��Ȃ��悤�ɂ��邽�߂̎d�g��
  GameSystem_UpdateDoEvent( gsys ); 

  if(GAMEDATA_IsFrameSpritMode(gsys->gamedata)) //�t���[��������Ԃɂ���ꍇ
  {
    //��������R�O�t���[���p�̃L�[��Ԃ��悤�ɂ��܂�
    GFL_UI_ChangeFrameRate(GFL_UI_FRAMERATE_30);
  }

  // ���̃t���[�����O�ȊO�̏ꍇ�A�����������s���Ă���
  // �����������ɂ͏��������̍ŏ��̃t���[�������A�����K�v������
  // �����łȂ��ꍇ�A�p���̃t���O�����Ԃ�
  if(!GAMEDATA_GetAndAddFrameSpritCount(gsys->gamedata))
  {
    //PlayerController/Event Trigger
    //�C�x���g�N���`�F�b�N�����i�V�`���G�[�V�����ɂ�蕪��j
    GAMESYSTEM_EVENT_CheckSet(gsys, gsys->evcheck_func, gsys->evcheck_context);
    //�펞�ʐM�̋N�����Ď�
    GameSysmte_FieldAlwaysBootWatch(gsys);
    //�C�x���g���s����
    GAMESYSTEM_EVENT_Main(gsys);

    //�ʐM�C�x���g���s����
    GameCommSys_Main(gsys->game_comm);
  }
  //���C���v���Z�X
  gsys->proc_result = GFL_PROC_LOCAL_Main(gsys->procsys);

  // FIELD_SOUND���C��
  {
    FIELD_SOUND* fsnd = GAMEDATA_GetFieldSound( gsys->gamedata );
    FIELD_SOUND_Main( fsnd );
  }
  // ISS�V�X�e�����C��
  ISS_SYS_Update( gsys->iss_sys );

  //�v���C���ԁ@�J�E���g�A�b�v
  PLAYTIMECTRL_Countup( gsys->gamedata );

  if(GAMEDATA_IsFrameSpritMode(gsys->gamedata)) //�t���[��������Ԃɂ���ꍇ
  {
    //��������U�O�t���[���p�̃L�[��Ԃ��悤�ɂ��܂�
    GFL_UI_ChangeFrameRate(GFL_UI_FRAMERATE_60);
  }

  //�ʐM�G���[��ʌĂяo���`�F�b�N
  if(gsys->proc_result == GFL_PROC_MAIN_CHANGE || gsys->proc_result == GFL_PROC_MAIN_NULL){
    NetErr_DispCall(FALSE);
  }
  
#ifdef  PM_DEBUG
  //PROC�����݂��Ă��āA�C�x���g�����݂��Ȃ����������[�N�`�F�b�N�𑖂点��
  if( gsys->event == NULL && gsys->proc_result != GFL_PROC_MAIN_NULL ){
    u16 zone_id = PLAYERWORK_getZoneID( GAMESYSTEM_GetMyPlayerWork(gsys) );
    
    //���[�N�`�F�b�N�����O����}�b�v
    if( ZONEDATA_IsUnionRoom(zone_id) == FALSE && //���j�I��
        ZONEDATA_IsColosseum(zone_id) == FALSE && //�R���V�A��
        ZONEDATA_IsBattleSubway(zone_id) == FALSE ) //�o�g���T�u�E�F�C
    {
      heapLeakCheck();
    }
  }
#endif
  if (gsys->proc_result == GFL_PROC_MAIN_NULL && gsys->event == NULL)
  {
    //�v���Z�X���C�x���g�����݂��Ȃ��Ƃ��A�Q�[���I��
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

//--------------------------------------------------------------
/**
 * �t�B�[���h��ŏ펞�ʐM���N���ł��邩�Ď����A�K�v�ł���΋N�����s��
 *
 * @param   gsys    
 */
//--------------------------------------------------------------
static void GameSysmte_FieldAlwaysBootWatch(GAMESYS_WORK *gsys)
{
  if(GAMESYSTEM_IsEventExists(gsys) == FALSE){
    if(GAMESYSTEM_CheckFieldMapWork(gsys) == TRUE){
      FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
      if(FIELDMAP_IsReady(fieldWork) == TRUE){
        GAMESYSTEM_CommBootAlways( gsys );
      }
    }
  }
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void GameSystem_End(GAMESYS_WORK * gsys)
{
  GFL_PROC_LOCAL_Exit(gsys->procsys);
  GAMESYS_WORK_Delete(gsys);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void GAMESYSTEM_EVENT_EntryCheckFunc(GAMESYS_WORK * gsys, EVCHECK_FUNC evcheck_func, void * context)
{
  gsys->evcheck_func = evcheck_func;
  gsys->evcheck_context = context;
}


//------------------------------------------------------------------
// �C�x���g�N���t���O�̍X�V
//------------------------------------------------------------------
static void GameSystem_UpdateDoEvent( GAMESYS_WORK * gsys )
{
  gsys->do_event = GAMESYSTEM_EVENT_IsExists(gsys);
}

//============================================================================================
//
//    �O���C���^�[�t�F�C�X�֐�
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
void GAMESYSTEM_SetNextProc(GAMESYS_WORK * gsys,
    FSOverlayID ov_id, const GFL_PROC_DATA *procdata, void * pwk)
{
  GFL_PROC_LOCAL_SetNextProc(gsys->procsys, ov_id, procdata, pwk);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void GAMESYSTEM_CallProc(GAMESYS_WORK * gsys,
    FSOverlayID ov_id, const GFL_PROC_DATA *procdata, void * pwk)
{
  GFL_PROC_LOCAL_CallProc(gsys->procsys, ov_id, procdata, pwk);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void GAMESYSTEM_CallFieldProc(GAMESYS_WORK * gsys)
{
  GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(fieldmap), &FieldProcData, gsys);
}


//------------------------------------------------------------------
/**
 * @brief �C�x���g�N�������ǂ�����Ԃ�
 * @param gsys  �Q�[�����䃏�[�N�ւ̃|�C���^
 * @retval  TRUE    �N����
 * @retval  FALSE   �Ȃ�
 */
//------------------------------------------------------------------
BOOL GAMESYSTEM_IsEventExists(const GAMESYS_WORK * gsys)
{
  // ���̃t���[���̎n�܂肩��I���̊ԂɂP��ł��C�x���g�̏�Ԃ������
  // TRUE��Ԃ��܂��B
  if( GAMESYSTEM_EVENT_IsExists(gsys) ){
    return TRUE;
  }
  return gsys->do_event;
}

//------------------------------------------------------------------
//  �v���Z�X���݃`�F�b�N
//------------------------------------------------------------------
GFL_PROC_MAIN_STATUS GAMESYSTEM_IsProcExists(const GAMESYS_WORK * gsys)
{
  return gsys->proc_result;
}
//------------------------------------------------------------------
//  �Q�[���f�[�^�擾
//------------------------------------------------------------------
GAMEDATA * GAMESYSTEM_GetGameData(GAMESYS_WORK * gsys)
{
  return gsys->gamedata;
}

//------------------------------------------------------------------
//  �����v���C���[���[�N�擾
//------------------------------------------------------------------
PLAYER_WORK * GAMESYSTEM_GetMyPlayerWork(GAMESYS_WORK * gsys)
{
  return GAMEDATA_GetMyPlayerWork(gsys->gamedata);
}
//------------------------------------------------------------------
//  �q�[�vID�擾
//------------------------------------------------------------------
HEAPID GAMESYSTEM_GetHeapID(GAMESYS_WORK * gsys)
{
  return gsys->heapID;
}

//------------------------------------------------------------------
//  �C�x���g�擾
//------------------------------------------------------------------
GMEVENT * GAMESYSTEM_GetEvent(const GAMESYS_WORK * gsys)
{
  return gsys->event;
}

//------------------------------------------------------------------
//  �C�x���g�ݒ�
//------------------------------------------------------------------
void GAMESYSTEM_SetEvent(GAMESYS_WORK * gsys, GMEVENT * event)
{
  gsys->event = event;
}

//------------------------------------------------------------------
//  �t�B�[���h�}�b�v���[�N�ւ̃|�C���^�擾
//------------------------------------------------------------------
FIELDMAP_WORK * GAMESYSTEM_GetFieldMapWork(GAMESYS_WORK * gsys)
{
  //GF_ASSERT(gsys->fieldmap != NULL);
  return gsys->fieldmap;
}

//------------------------------------------------------------------
//  �t�B�[���h�}�b�v���[�N�̃|�C���^�Z�b�g
//------------------------------------------------------------------
void GAMESYSTEM_SetFieldMapWork(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap)
{
  gsys->fieldmap = fieldmap;
}

//--------------------------------------------------------------
//  �t�B�[���h�}�b�v���[�N�|�C���^�L���`�F�b�N
//--------------------------------------------------------------
BOOL GAMESYSTEM_CheckFieldMapWork( const GAMESYS_WORK *gsys )
{
  if( gsys->fieldmap != NULL ){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * �t�B�[���h�ł̒ʐM�G���[��ʕ\�����N�G�X�g�ݒ�
 *
 * @param   fieldWork   FIELDMAP_WORK
 * @param   error_req   TRUE:�G���[��ʕ\���B�@FALSE:���N�G�X�g���N���A
 */
//==================================================================
void GAMESYSTEM_SetFieldCommErrorReq( GAMESYS_WORK *gsys, BOOL error_req )
{
  gsys->field_comm_error_req = error_req;
}

//==================================================================
/**
 * �t�B�[���h�ł̒ʐM�G���[��ʕ\�����N�G�X�g�擾
 *
 * @param   fieldWork   FIELDMAP_WORK
 * @retval  TRUE:�G���[��ʕ\���B�@FALSE:���N�G�X�g���N���A
 */
//==================================================================
BOOL GAMESYSTEM_GetFieldCommErrorReq( const GAMESYS_WORK *gsys )
{
  return gsys->field_comm_error_req;
}

//--------------------------------------------------------------
//  ISS�V�X�e���擾
//--------------------------------------------------------------
ISS_SYS * GAMESYSTEM_GetIssSystem( GAMESYS_WORK * gsys )
{
  return gsys->iss_sys;
}

//==================================================================
/**
 * �Q�[���ʐM�Ǘ����[�N�ւ̃|�C���^���擾����
 * @param   gsys    �Q�[������V�X�e���ւ̃|�C���^
 * @retval  GAME_COMM_SYS_PTR   �Q�[���ʐM�Ǘ����[�N�ւ̃|�C���^
 */
//==================================================================
GAME_COMM_SYS_PTR GAMESYSTEM_GetGameCommSysPtr(GAMESYS_WORK *gsys)
{
  return gsys->game_comm;
}

//----------------------------------------------------------------------------
/**
 * @brief �ʐM�s���C�x���g�t���O�̃Z�b�g
 * @param   gsys    �Q�[������V�X�e���ւ̃|�C���^
 * @param   flag    TRUE�̂Ƃ��A�ʐM�s���C�x���g�̊J�n���Ӗ�����
 */
//----------------------------------------------------------------------------
void GAMESYSTEM_SetNetOffEventFlag( GAMESYS_WORK * gsys, BOOL flag )
{
  gsys->comm_off_event_flag = flag;
}

//----------------------------------------------------------------------------
/**
 * @brief �ʐM�s���C�x���g�t���O�̎擾
 * @param   gsys    �Q�[������V�X�e���ւ̃|�C���^
 * @return  BOOL    TRUE�̎��A�ʐM�s���C�x���g��
 */
//----------------------------------------------------------------------------
BOOL GAMESYSTEM_GetNetOffEventFlag( const GAMESYS_WORK * gsys )
{
  return gsys->comm_off_event_flag;
}

//--------------------------------------------------------------
/**
 * @brief   �펞�ʐM�t���O�̎擾
 * @param   gsys    �Q�[������V�X�e���ւ̃|�C���^
 * @retval  TRUE:�펞�ʐMON�A�@FALSE:�펞�ʐMOFF
 */
//--------------------------------------------------------------
BOOL GAMESYSTEM_GetAlwaysNetFlag(const GAMESYS_WORK * gsys)
{
  return GAMEDATA_GetAlwaysNetFlag(gsys->gamedata);
}

//--------------------------------------------------------------
/**
 * @brief   �펞�ʐM�t���O�̐ݒ�
 * @param   gsys    �Q�[������V�X�e���ւ̃|�C���^
 * @param is_on     TRUE�Ȃ�Ώ펞�ʐM���[�hON FALSE�Ȃ�Ώ펞�ʐM���[�hOFF
 */
//--------------------------------------------------------------
void GAMESYSTEM_SetAlwaysNetFlag( GAMESYS_WORK * gsys, BOOL is_on )
{ 
  GAMEDATA_SetAlwaysNetFlag(gsys->gamedata, is_on);
}

//==================================================================
/**
 * �펞�ʐM���N�����Ă��悢��Ԃ����`�F�b�N����
 *
 * @param   gsys    
 *
 * @retval  BOOL    TRUE:�펞�ʐM���N�����Ă��悢
 * @retval  BOOL    FALSE:�N�����Ă͂����Ȃ�
 *
 * @note
 * �����_�ŁA�펞�ʐM���N���ł��邩�ǂ����̏����͉��L�̂悤�ɖ��Ă���
 * @li  �d�v�ȃC�x���g���łȂ�
 * @li  �r�[�R���T�[�`���֎~���Ă���}�b�v�łȂ�
 * @li  �ʐM�G���[���������Ă��Ȃ�
 * @li  �펞�ʐM�t���O�iC�M�A��ON��R���t�B�O�̒ʐM�ƘA�����Ă���t���O�j�������Ă���
 * @li  C�M�A����t���O�������Ă���(����O�Ȃ�펞�ʐM�t���O�����OFF�̂͂������O�̂��߂������)
 */
//==================================================================
BOOL GAMESYSTEM_CommBootAlways_Check(GAMESYS_WORK *gsys)
{
  GAME_COMM_SYS_PTR gcsp = GAMESYSTEM_GetGameCommSysPtr(gsys);
  u16 zone_id = PLAYERWORK_getZoneID( GAMESYSTEM_GetMyPlayerWork(gsys) );

  if( gsys->comm_off_event_flag == FALSE
      && ZONEDATA_IsFieldBeaconNG(zone_id) == FALSE
      && NetErr_App_CheckError() == NET_ERR_CHECK_NONE
      && GAMESYSTEM_GetAlwaysNetFlag( gsys ) == TRUE
      && Intrude_Check_AlwaysBoot( gsys ) == TRUE
      && (!(GFL_UI_CheckCoverOff() == TRUE && GFL_UI_CheckLowBatt() == TRUE))
      && CGEAR_SV_GetCGearONOFF(GAMEDATA_GetCGearSaveData(gsys->gamedata)) )
  {
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * �펞�ʐM�t���O���`�F�b�N������ŁA�펞�ʐM���N������
 *
 * @param   gsys    
 * @retval  BOOL    TRUE:�ʐM�N�������@FALSE:�ʐM���N�����Ȃ�����
 */
//==================================================================
BOOL GAMESYSTEM_CommBootAlways( GAMESYS_WORK *gsys )
{
  GAME_COMM_SYS_PTR gcsp = GAMESYSTEM_GetGameCommSysPtr(gsys);
  
  //���d�N���h�~
  if(GFL_NET_IsInit() == FALSE && GameCommSys_BootCheck(gcsp) == GAME_COMM_NO_NULL){
    if(GAMESYSTEM_CommBootAlways_Check(gsys) == TRUE){
      GameCommSys_Boot( gcsp, GAME_COMM_NO_FIELD_BEACON_SEARCH, gsys );
      return TRUE;
    }
  }
  return FALSE;
}

//==================================================================
/**
 * @brief   �o�b�e���[�P�O���ȉ��ŃX���[�v�ɓ������ꍇ��CGEARON�̏ꍇ�ɂ� TRUE
 * @param   gsys    
 * @retval  BOOL    TRUE:�o�b�e���[�P�O���ȉ��ŃX���[�v�ɓ������ꍇ��CGEARON�̏ꍇ
 */
//==================================================================

BOOL GAMESYSTEM_IsBatt10Sleep( GAMESYS_WORK *gsys )
{
  return gsys->batt10sleep;
}

//==================================================================
/**
 * @brief  �o�b�e���[�P�O���ȉ��ŃX���[�v�ɓ������ꍇ�ł̃t���O���Z�b�g
 * @param   gsys    
 * @retval  void
 */
//==================================================================

void GAMESYSTEM_ResetBatt10Sleep( GAMESYS_WORK *gsys )
{
  gsys->batt10sleep=FALSE;
}


//==================================================================
/**
 * �o�b�e���[�P�O���ȉ��ŃX���[�v�ɓ������ꍇ�ł̃R�[���o�b�N
 * @param   gsys    
 * @retval  void
 */
//==================================================================

static void GameSystem_CallbackBatt10Sleep( void *gsys_void )
{
  //�P�D�P�O�ȉ���CGEARON�ŃX���[�v�ɂȂ��Ă����Ȃ�΁A
  //    �ʐM�A�v���������[�h�t���O��ON  =>  gsys->batt10sleep == TRUE
  //    �����ɏ펞�ʐM�t���O��OFF 
  //�Q�D�펞�ʐM�͍ĊJ���Ȃ�
  //�R�Dgsys->batt10sleep�����Ȃ���e�A�v���͓���
  //�S�DCGEAR���̂͏펞�ʐMOFF������͂��߂�
  //�T�DCGEAR�ɍs���������i�K�Ńt���O�����Z�b�g ����
  GAMESYS_WORK *gsys = gsys_void;
  if(GAMESYSTEM_GetAlwaysNetFlag(gsys))
  {
    GAMESYSTEM_SetAlwaysNetFlag(gsys, FALSE);
    gsys->batt10sleep = TRUE;
  }
}

//==================================================================
/**
 * @brief  �o�b�e���[�P�O���ȉ��ŃX���[�v�ɓ������ꍇ�ł̃R�[���o�b�N�Z�b�g
 * @param   gsys    
 * @retval  void
 */
//==================================================================

static void GameSystem_SetBatt10SleepCallback( GAMESYS_WORK *gsys )
{
  GFL_UI_Batt10SleepReleaseSetFunc( GameSystem_CallbackBatt10Sleep, gsys );
}
static void GameSystem_ResetBatt10SleepCallback( GAMESYS_WORK *gsys )
{
  GFL_UI_Batt10SleepReleaseSetFunc( NULL, NULL );
}

//==================================================================
/**
 * @brief   �X���[�v�ɓ���Ȃ��������Q�[����Ŏw��ł���R�[���o�b�N
 * @param   gsys    
 * @retval  void
 */
//==================================================================

static BOOL GameSystem_CallbackNotSleep( void *gsys_void )
{
  GAMESYS_WORK *gsys = gsys_void;
  GAME_COMM_SYS_PTR gcsp = GAMESYSTEM_GetGameCommSysPtr(gsys);

  if(GameCommSys_BootCheck(gcsp) != GAME_COMM_NO_NULL){  //���j�I����
    return TRUE;  //�͂����Ăق����Ȃ�
  }
  if(GFL_NET_IW_SystemCheck()){  //IRC WIRLESS
    return TRUE;  //�͂����Ăق����Ȃ�
  }
  if(GFL_UI_CheckLowBatt()){  //�o�b�e���[��LOW
    return FALSE;  //�͂����Ăق���
  }
  if(GAMESYSTEM_GetAlwaysNetFlag(gsys)){  //�펞�ʐM
    return TRUE;  //�͂����Ăق����Ȃ�
  }
  return FALSE;  //�����Ă悢
}

//==================================================================
/**
 * @brief   �X���[�v�ɓ���Ȃ��������Q�[����Ŏw��ł���R�[���o�b�N�̓o�^����
 * @param   gsys    
 * @retval  void
 */
//==================================================================

static void GameSystem_SetNotSleepCallback( GAMESYS_WORK *gsys )
{
  GFL_UI_NotSleepSetFunc( GameSystem_CallbackNotSleep, gsys );
}
static void GameSystem_ResetNotSleepCallback( GAMESYS_WORK *gsys )
{
  GFL_UI_NotSleepSetFunc( NULL, NULL );
}



#ifdef PM_DEBUG

//============================================================================================
//============================================================================================
static u32 ProcHeapFreeSize = 0;
static u32 WorldHeapFreeSize = 0;

//------------------------------------------------------------------
/**
 * @brief �q�[�v�̃��[�N�`�F�b�N
 *
 * �Ȃ�̃C�x���g���N�����Ă��Ȃ���Ԃ̏����l��ێ����Ă����A
 * �ȍ~�C�x���g���N�����Ă��Ȃ��^�C�~���O�ŏ�ɊĎ����s��
 */
//------------------------------------------------------------------

static void heapLeakCheck( void )
{
  u32 temp_proc_size = GFL_HEAP_GetHeapFreeSize( HEAPID_PROC );
  u32 temp_world_size = GFL_HEAP_GetHeapFreeSize( HEAPID_WORLD );

  if ( ProcHeapFreeSize == 0 ) {
    ProcHeapFreeSize = temp_proc_size;
  } else if ( ProcHeapFreeSize != temp_proc_size ) {
    GFL_HEAP_DEBUG_PrintExistMemoryBlocks( HEAPID_PROC );
    GF_ASSERT_MSG( 0, "HEAPID_PROC LEAK now(%08x) < start(%08x)\n", temp_proc_size, ProcHeapFreeSize );
    ProcHeapFreeSize = temp_proc_size;
  }

  if ( WorldHeapFreeSize == 0 ) {
    WorldHeapFreeSize = temp_world_size;
  } else if ( WorldHeapFreeSize != temp_world_size ) {
    GFL_HEAP_DEBUG_PrintExistMemoryBlocks( HEAPID_WORLD );
    GF_ASSERT_MSG( 0, "HEAPID_WORLD LEAK now(%08x) < start(%08x)\n", temp_world_size, WorldHeapFreeSize );
    WorldHeapFreeSize = temp_world_size;
  }
}

#endif //PM_DEBUG




#if 0
  GFL_OVERLAY_Load( FS_OVERLAY_ID(amprotect) );  
  gsys = GFL_PROC_AllocWork(proc, work_size, HEAPID_WORLD);
  GFL_STD_MemClear(gsys, work_size);
  flag=AM_IsMagiconA1(magicon1_true_func);
  OS_Printf("MagiconCheck1=%d\n", flag);

  GameSysWork = gsys;
  GameSystem_Init(gsys, HEAPID_WORLD, game_init);

  flag=AM_IsMagiconA2(magicon2_true_func);
  OS_Printf("MagiconCheck2=%d\n", flag);

  { //�Q�[���J�n�C�x���g�̐���
    GMEVENT * event = EVENT_GameStart(gsys, game_init);
    GAMESYSTEM_SetEvent(gsys, event);
  }

  flag=AM_IsMagiconA3(magicon3_true_func);
  OS_Printf("MagiconCheck3=%d\n", flag);
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(amprotect) );  

#endif
