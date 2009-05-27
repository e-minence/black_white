//======================================================================
/**
 * @file	fieldmap.c
 * @brief	�t�B�[���h�}�b�v����
 * @author
 *
 * @todo  1/30�œ��삵�Ă��邪�L�[��1/60�ŏE���Ă���̂Ŏ��Ȃ��^�C�~���O������
 * @todo  �O������̃t�B�[���h�}�b�v�I�����N�G�X�g������̃^�C�~���O���l�����Ă��Ȃ�
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"					//GFL_HEAPID_APP
#include "system/wipe.h"
#include "arc_def.h"

#include "net/network_define.h"
#include "system/g3d_tool.h"

#include "print/gf_font.h"

#include "fieldmap.h"
#include "field_common.h"
#include "field_player.h"
#include "field_camera.h"
#include "field_data.h"
#include "field/field_msgbg.h"
#include "field_encount.h"

#include "weather.h"
#include "field_fog.h"
#include "field_light.h"
#include "field_buildmodel.h"
#include "field_rail.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/playerwork.h"
#include "gamesystem/game_event.h"
#include "field/zonedata.h"
#include "field/areadata.h"

#include "event_mapchange.h"
#include "event_debug_menu.h"
#include "event_fieldmap_menu.h"
#include "event_battle.h"
#include "event_ircbattle.h"
#include "event_fieldtalk.h"

#include "field_comm_actor.h"
#include "field_comm/field_comm_main.h"
#include "field_comm/field_comm_event.h"

#include "fldmmdl.h"

#include "field_debug.h"

#include "map_matrix.h"

#include "field/eventdata_sxy.h"

#include "field_effect.h"

#include "arc/fieldmap/zone_id.h"
#include "field/weather_no.h"
#include "sound/pm_sndsys.h"
#include "sound/wb_sound_data.sadl"

#include "message.naix" //NARC_message_d_field_dat
#include "msg/msg_d_field.h"  //DEBUG_FIELD_STR00 DEBUG_FIELD_C_STR10

//======================================================================
//	define
//======================================================================
//--------------------------------------------------------------
///	GAMEMODE
//--------------------------------------------------------------
typedef enum{
	GAMEMODE_BOOT = 0,
	GAMEMODE_NORMAL,
	GAMEMODE_FINISH,
}GAMEMODE;

//--------------------------------------------------------------
///	FLDMAPSEQ
//--------------------------------------------------------------
typedef enum{
	FLDMAPSEQ_SETUP_SYSTEM = 0,
	FLDMAPSEQ_SETUP,
	FLDMAPSEQ_READY,
	FLDMAPSEQ_UPDATE,
	FLDMAPSEQ_FREE,
	FLDMAPSEQ_END,
	
	FLDMAPSEQ_MAX,
}FLDMAPSEQ;

//--------------------------------------------------------------
///	�萔
//--------------------------------------------------------------
enum { 
  ///3D�ʂ̕`��v���C�I���e�B�[
  FIELD_3D_FRAME_PRI    =   1,

  ///�e�N�X�`���g�p�T�C�Y�w��
  FIELD_3D_VRAM_SIZE    =   GFL_G3D_TEX256K,

  ///�p���b�g�g�p�T�C�Y�w��
  FIELD_3D_PLTT_SIZE    =   GFL_G3D_PLT64K,

  ///DTCM�G���A�̃T�C�Y
  FIELD_3D_DTCM_SIZE    =   0x1000, 

///billboardAct�Ŏg�p���郊�\�[�X�̍ő�ݒ�\��
  FIELD_G3D_BBDACT_RESMAX	    =   64,
///billboardAct�Ŏg�p����I�u�W�F�N�g�̍ő�ݒ�\��
  FIELD_G3D_BBDACT_ACTMAX	    =   256,

  ///<�Z���A�N�^�[���\�[�X�ő�
  FIELD_CLSYS_RESOUCE_MAX		  =   64,

  ///�t�H�O�F
  FIELD_DEFAULT_FOG_COLOR		  =   (GX_RGB(31,31,31)),
  ///�w�i�F
  FIELD_DEFAULT_BG_COLOR	    =   (GX_RGB(30,31,31)),
};

///g3Dutil�Ŏg�p���郊�\�[�X�̍ő�ݒ�\��
#define G3D_UTIL_RESCOUNT	(512)
///g3Dutil�Ŏg�p����I�u�W�F�N�g�̍ő�ݒ�\��
#define G3D_UTIL_OBJCOUNT	(128)

#define MMDL_LIST_MAX	(64) ///<���샂�f�����X�g�ő�

//--------------------------------------------------------------
/**
 * @brief �t�B�[���h�}�b�v�V�[�P���X����֐��̖߂�l
 */
//--------------------------------------------------------------
typedef enum {  
  MAINSEQ_RESULT_CONTINUE,      ///<����p����
  MAINSEQ_RESULT_NEXTSEQ,       ///<���̃V�[�P���X�֐؂�ւ�
  MAINSEQ_RESULT_FINISH,        ///<�I��

  MAINSEQ_RESULT_MAX
}MAINSEQ_RESULT;

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
/// �t�B�[���h�}�b�v�V�[�P���X����֐��̌^��`
//--------------------------------------------------------------
typedef MAINSEQ_RESULT (*FIELDMAP_MAIN_FUNC)(GAMESYS_WORK*,FIELDMAP_WORK*);

//--------------------------------------------------------------
///	FIELDMAP_WORK
//--------------------------------------------------------------
struct _FIELDMAP_WORK
{
	HEAPID heapID;
	GAMESYS_WORK *gsys;
	
	FIELD_CAMERA *camera_control;
	FIELD_LIGHT *light;
	FIELD_FOG_WORK *fog;
	
	FIELD_WEATHER *weather_sys;
  FIELD_SUBSCREEN_WORK* fieldSubscreenWork;
	
	FIELD_COMM_MAIN *commSys;
	
	FLDMSGBG *fldMsgBG;
	
	FLDMMDLSYS *fldMMdlSys;

	FIELD_RAIL_MAN * railMan;
	
	FLDMAPPER *g3Dmapper;
	MAP_MATRIX *pMapMatrix;
	FLDMAPPER_RESISTDATA map_res;
	FIELD_PLAYER *field_player;
	FIELD_ENCOUNT *encount;
	FLDEFF_CTRL *fldeff_ctrl;

  GFL_G3D_CAMERA *g3Dcamera; //g3Dcamera Lib �n���h��
	GFL_G3D_LIGHTSET *g3Dlightset; //g3Dlight Lib �n���h��
	GFL_TCB *g3dVintr; //3D�pvIntrTask�n���h��
	GFL_BBDACT_SYS *bbdActSys; //�r���{�[�h�A�N�g�V�X�e���ݒ�n���h��
	
	GAMEMODE gamemode;
	FLDMAPSEQ seq;
  u32 seq_switch;
	int timer;
	u16 map_id;
	VecFx32 now_pos;
	LOCATION location;
	
	const DEPEND_FUNCTIONS *func_tbl;
	void *mapCtrlWork;
	
	FIELD_DEBUG_WORK *debugWork;


  int firstConnectEventID;
};

//--------------------------------------------------------------
///	MMDL_LIST
//--------------------------------------------------------------
typedef struct
{
	int count;
	u16 id_list[MMDL_LIST_MAX];
}MMDL_LIST;

//======================================================================
//	proto
//======================================================================
//�t�B�[���h���C���V�[�P���X�֐��Q
static MAINSEQ_RESULT mainSeqFunc_setup_system(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );
static MAINSEQ_RESULT mainSeqFunc_setup(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );
static MAINSEQ_RESULT mainSeqFunc_ready(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );
static MAINSEQ_RESULT mainSeqFunc_update(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );
static MAINSEQ_RESULT mainSeqFunc_free(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );
static MAINSEQ_RESULT mainSeqFunc_end(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );

//bg
static void	fldmap_BG_Init( FIELDMAP_WORK *fieldWork );
static void	fldmap_BG_Exit( FIELDMAP_WORK *fieldWork );

//g3d
static void fldmap_G3D_CallBackSetUp( void );
static void fldmap_G3D_Load( FIELDMAP_WORK * fieldWork );
static void fldmap_G3D_Control( FIELDMAP_WORK * fieldWork );
static void fldmap_G3D_Draw( FIELDMAP_WORK * fieldWork );
static void fldmap_G3D_Unload( FIELDMAP_WORK * fieldWork );
static void	fldmap_G3D_VBlank( GFL_TCB* tcb, void* work );
static void	fldmap_G3D_BBDTrans(
		GFL_BBDACT_TRANSTYPE type, u32 dst, u32 src, u32 siz );

//fldmmdl
static void fldmapMain_FLDMMDL_Init( FIELDMAP_WORK *fieldWork );
static void fldmapMain_FLDMMDL_Finish( FIELDMAP_WORK *fieldWork );
static void fldmap_FLDMMDL_InitList(
		MMDL_LIST *mlist, int list_id, HEAPID heapID );

//event
static GMEVENT * fldmap_Event_CheckConnect(
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, const VecFx32 *now_pos );
static GMEVENT * fldmap_Event_CheckPushConnect(
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );
static GMEVENT * fldmapFunc_Event_CheckEvent(GAMESYS_WORK *gsys, void *work);

//zonechange
static BOOL fldmapMain_UpdateMoveZone( FIELDMAP_WORK *fieldWork );
static BOOL fldmap_CheckPlayerPosUpdate( FIELDMAP_WORK *fieldWork );
static BOOL fldmap_CheckMoveZoneChange( FIELDMAP_WORK *fieldWork );
static void fldmap_ZoneChange( FIELDMAP_WORK *fieldWork );

static void zoneChange_SetMMdl(
		FLDMMDLSYS *fmmdlsys, EVENTDATA_SYSTEM *evdata, u32 zone_id );
static void zoneChange_SetBGM( GAMEDATA *gdata, u32 zone_id );
static void zoneChange_SetWeather( FIELDMAP_WORK *fieldWork, u32 zone_id );
static void zoneChange_UpdatePlayerWork( GAMEDATA *gdata, u32 zone_id );

//etc
static void fldmap_ClearMapCtrlWork( FIELDMAP_WORK *fieldWork );

//data
static const GFL_DISP_VRAM fldmapdata_dispVram;
static const GFL_BG_SYS_HEADER fldmapdata_bgsysHeader;
static const GFL_CLSYS_INIT fldmapdata_CLSYS_Init;
static const VecFx32 fldmapdata_cameraTarget;
static const VecFx32 fldmapdata_cameraPos;
static const GFL_G3D_LIGHTSET_SETUP fldmapdata_light0Setup;
static const GXRgb fldmapdata_edgeColorTable[8];
static const u32 fldmapdata_fogColorTable[8];

static const u16 fldmapdata_bgColorTable[16];

//======================================================================
//	�t�B�[���h�}�b�v�@�����@�폜
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h�}�b�v�@����
 * @param	gsys	GAMESYS_WORK
 * @param	heapID HEAPID
 * @retval	FIELDMAP_WORK
 */
//--------------------------------------------------------------
FIELDMAP_WORK * FIELDMAP_Create( GAMESYS_WORK *gsys, HEAPID heapID )
{
	FIELDMAP_WORK *fieldWork;
  COMM_FIELD_SYS_PTR comm_field;
	
	fieldWork = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELDMAP_WORK) );
	fieldWork->heapID = heapID;
	fieldWork->gamemode = GAMEMODE_BOOT;
	fieldWork->gsys = gsys;
	
	{
		PLAYER_WORK *pw = GAMESYSTEM_GetMyPlayerWork(gsys);
		fieldWork->map_id = PLAYERWORK_getZoneID( pw );
	}
	
	//�}�b�v�R���g���[��
	fieldWork->func_tbl = FIELDDATA_GetFieldFunctions( fieldWork->map_id );

	//�}�b�v�}�g���N�X
	fieldWork->pMapMatrix = MAP_MATRIX_Create( heapID );
	
	//�ʐM�p���� 
  fieldWork->commSys = FIELD_COMM_MAIN_InitSystem( heapID, GFL_HEAPID_APP, GAMESYSTEM_GetGameCommSysPtr(gsys) );
  FIELD_COMM_MAIN_CommFieldMapInit(FIELD_COMM_MAIN_GetCommFieldSysPtr(fieldWork->commSys));
	FIELD_COMM_MAIN_SetCommActor(fieldWork->commSys,
      GAMEDATA_GetFldMMdlSys(GAMESYSTEM_GetGameData(gsys)));
  
#if 0   //��check�@���b��
  //�펞�ʐM���[�h
  {
    GAME_COMM_SYS_PTR gcsp = GAMESYSTEM_GetGameCommSysPtr(gsys);
    if(GAMESYSTEM_GetAlwaysNetFlag(gsys) == TRUE 
        && GameCommSys_BootCheck(gcsp) == GAME_COMM_NO_NULL){
      GameCommSys_Boot(gcsp, GAME_COMM_NO_FIELD_BEACON_SEARCH, gcsp);
    }
  }
#endif
	return fieldWork;
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�}�b�v�@�폜
 * @param	fieldWork	FIELDMAP_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
void FIELDMAP_Delete( FIELDMAP_WORK *fieldWork )
{
	//�}�b�v�}�g���N�X
	MAP_MATRIX_Delete( fieldWork->pMapMatrix );
	
	GFL_HEAP_FreeMemory( fieldWork );
}

//======================================================================
//	�t�B�[���h�}�b�v�@���C��
//======================================================================
//  �t�B�[���h���C���V�[�P���X�֐��Q
//--------------------------------------------------------------
//--------------------------------------------------------------
static MAINSEQ_RESULT mainSeqFunc_setup_system(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{ 
	HEAPID heapID = fieldWork->heapID;
	
	//����������
	GFL_STD_MtRandInit(0);

	//VRAM�N���A
	GFL_DISP_ClearVRAM( GX_VRAM_D );
	//VRAM�ݒ�
	GFL_DISP_SetBank( &fldmapdata_dispVram );

	//BG������
	fldmap_BG_Init( fieldWork );
	
	// CLACT������
	GFL_CLACT_SYS_Create(
			&fldmapdata_CLSYS_Init, &fldmapdata_dispVram, heapID );
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

	//BMP������
	GFL_BMPWIN_Init( heapID );
	
	//FONT������
	GFL_FONTSYS_Init();
	
	//�R�c�V�X�e���N��
	GFL_G3D_Init(
			GFL_G3D_VMANLNK, FIELD_3D_VRAM_SIZE,
      GFL_G3D_VMANLNK, FIELD_3D_PLTT_SIZE,
			FIELD_3D_DTCM_SIZE, fieldWork->heapID, fldmap_G3D_CallBackSetUp );
	DEBUG_GFL_G3D_SetVManSize( FIELD_3D_VRAM_SIZE, FIELD_3D_PLTT_SIZE );

	//�R�c�f�[�^�̃��[�h
	fldmap_G3D_Load( fieldWork );
	
	//�z�u���ݒ�
	fieldWork->g3Dmapper = FLDMAPPER_Create( fieldWork->heapID );

	TAMADA_Printf("TEX:%06x PLT:%04x\n",
			DEBUG_GFL_G3D_GetBlankTextureSize(),
			DEBUG_GFL_G3D_GetBlankPaletteSize());

	//VBlankTCB
	fieldWork->g3dVintr =
		GFUser_VIntr_CreateTCB( fldmap_G3D_VBlank, (void*)fieldWork, 0 );

	GFL_UI_StartFrameRateMode( GFL_UI_FRAMERATE_30 );
	
  return MAINSEQ_RESULT_NEXTSEQ;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static MAINSEQ_RESULT mainSeqFunc_setup(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{ 
  fieldWork->fldMsgBG = FLDMSGBG_Setup(
      fieldWork->heapID, fieldWork->g3Dcamera );

  fieldWork->camera_control = FIELD_CAMERA_Create(
      fieldWork,
      ZONEDATA_GetCameraID(fieldWork->map_id),
      fieldWork->g3Dcamera,
      &fieldWork->now_pos,
      fieldWork->heapID );

  // rail�V�X�e��������
  fieldWork->railMan = FIELD_RAIL_MAN_Create( fieldWork->heapID, fieldWork->camera_control );

  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
    FIELD_BMODEL_MAN * bmodel_man = FLDMAPPER_GetBuildModelManager( fieldWork->g3Dmapper );
    
    FIELDDATA_SetMapperData(fieldWork->map_id,
        GAMEDATA_GetSeasonID(gamedata),
        &fieldWork->map_res,
        fieldWork->pMapMatrix );
    { //�Ƃ肠�����A�d���f���p�������o�^
      FIELD_BMODEL_MAN_EntryELStringID(bmodel_man,
          FIELD_BMODEL_ELBOARD_ID1, NARC_message_d_field_dat, DEBUG_FIELD_C_STR10);
      FIELD_BMODEL_MAN_EntryELStringID(bmodel_man,
          FIELD_BMODEL_ELBOARD_ID2, NARC_message_d_field_dat, DEBUG_FIELD_STR00);
    }
    //�Ƃ肠���������Ŕz�u���f�����X�g���Z�b�g����
    FIELD_BMODEL_MAN_Load(bmodel_man, fieldWork->map_id);
  }
  
  //�t�B�[���h�}�b�v�p���P�[�V�����쐬
  fieldWork->location.zone_id = fieldWork->map_id;
  fieldWork->location.pos = fieldWork->now_pos;
  
  //�}�b�v�f�[�^�o�^
  FLDMAPPER_ResistData( fieldWork->g3Dmapper, &fieldWork->map_res );
  
  //���샂�f��������
  fldmapMain_FLDMMDL_Init(fieldWork);
  
  //�t�B�[���h�G�t�F�N�g������
  fieldWork->fldeff_ctrl = FLDEFF_CTRL_Create(
      fieldWork, FLDEFF_PROCID_MAX, fieldWork->heapID );
  
  //�t�B�[���h�G�t�F�N�g�@�p�����^������
  FLDEFF_CTRL_SetTaskParam( fieldWork->fldeff_ctrl, FLDEFF_TASK_MAX );
  
  //�t�B�[���h�G�t�F�N�g�@�o�^
  FLDEFF_CTRL_RegistEffect( fieldWork->fldeff_ctrl,
    DATA_FLDEFF_RegistEffectGroundTbl, DATA_FLDEFF_RegistEffectGroundTblNum );
  
  {
    const PLAYER_WORK *pw = GAMESYSTEM_GetMyPlayerWork(gsys);
    const u16 dir = pw->direction;
    const VecFx32 *pos = &pw->position;

    //���@�쐬
    fieldWork->field_player =
      FIELD_PLAYER_Create( fieldWork, pos, fieldWork->heapID );
    
    //�o�^�e�[�u�����ƂɌʂ̏������������Ăяo��
    fieldWork->now_pos = *pos;
    fieldWork->func_tbl->create_func(fieldWork, &fieldWork->now_pos, dir);

    //���[���}�b�v�̏ꍇ�́A����L�Ń��[���Ɋւ��鏉�������I��������
    //�ۑ������ʒu�𔽉f����
    if (ZONEDATA_DEBUG_IsRailMap(fieldWork->map_id) == TRUE)
    { 
      GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
      const RAIL_LOCATION * railLoc = GAMEDATA_GetRailLocation(gamedata);
      FIELD_RAIL_MAN_SetLocation(fieldWork->railMan, railLoc);
      FIELD_RAIL_MAN_GetPos( fieldWork->railMan, &fieldWork->now_pos );
    }

    FLDMAPPER_SetPos( fieldWork->g3Dmapper, &fieldWork->now_pos );
    
    TAMADA_Printf("start X,Y,Z=%d,%d,%d\n", FX_Whole(pos->x), FX_Whole(pos->y), FX_Whole(pos->z));
    TAMADA_Printf( "Start Dir = %04x\n", pw->direction );
  }
  
  // �t�H�O�V�X�e������
  fieldWork->fog	= FIELD_FOG_Create( fieldWork->heapID );

  // ���C�g�V�X�e������
  {
    GAMEDATA * gamedata = GAMESYSTEM_GetGameData(gsys);
    u16 area_id = ZONEDATA_GetAreaID(fieldWork->map_id);
    u16 area_season_id = area_id + ( AREADATA_HasSeason(area_id) ? GAMEDATA_GetSeasonID(gamedata) : 0 );

    fieldWork->light = FIELD_LIGHT_Create( AREADATA_GetLightType( area_season_id ), 
        14400, 
        fieldWork->fog, fieldWork->g3Dlightset, fieldWork->heapID );
  }

  // �V�C�V�X�e������
  fieldWork->weather_sys = FIELD_WEATHER_Init(
      fieldWork->camera_control,
      fieldWork->light,
      fieldWork->fog,
      fieldWork->heapID );
  
  // �V�C����
  FIELD_WEATHER_Set(
      fieldWork->weather_sys, WEATHER_NO_SUNNY, fieldWork->heapID );
  
  //���o�[�̏�����
	{
		GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
		fieldWork->fieldSubscreenWork = FIELD_SUBSCREEN_Init(fieldWork->heapID, fieldWork, GAMEDATA_GetSubScreenMode(gamedata));
	}
  
  //�t�B�[���h�G���J�E���g������
  fieldWork->encount = FIELD_ENCOUNT_Create( fieldWork );
  
   //�t�B�[���h�f�o�b�O������
  fieldWork->debugWork = FIELD_DEBUG_Init( fieldWork, fieldWork->heapID );

  return MAINSEQ_RESULT_NEXTSEQ;
}


//--------------------------------------------------------------
//--------------------------------------------------------------
static MAINSEQ_RESULT mainSeqFunc_ready(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{ 
	fldmap_G3D_Control( fieldWork );
	fldmap_G3D_Draw( fieldWork );
	GFL_CLACT_SYS_Main(); // CLSYS���C��
  
  FLDMSGBG_PrintMain( fieldWork->fldMsgBG );
  
  FIELD_DEBUG_UpdateProc( fieldWork->debugWork );
  
  if( fieldWork->fldMMdlSys != NULL ){
    FLDMMDLSYS_UpdateProc( fieldWork->fldMMdlSys );
  }
  
  if( FLDMAPPER_CheckTrans(fieldWork->g3Dmapper) == FALSE ){
    return MAINSEQ_RESULT_CONTINUE;
  }
  
  FLDEFF_CTRL_Update( fieldWork->fldeff_ctrl );

  if (ZONEDATA_DEBUG_IsRailMap(fieldWork->map_id) == TRUE)
  {
    GAMEDATA * gamedata = GAMESYSTEM_GetGameData(gsys);
    EVENTDATA_SYSTEM *evdata = GAMEDATA_GetEventData(gamedata);
    fieldWork->firstConnectEventID = 
      EVENTDATA_SearchConnectIDBySphere(evdata, &fieldWork->now_pos);
  }
  //�t�B�[���h�}�b�v�p�C�x���g�N���`�F�b�N���Z�b�g����
  GAMESYSTEM_EVENT_EntryCheckFunc(
      gsys, fldmapFunc_Event_CheckEvent, fieldWork );
  
  fieldWork->gamemode = GAMEMODE_NORMAL;
	GAMEDATA_SetFrameSpritEnable(GAMESYSTEM_GetGameData(gsys), TRUE);

  return MAINSEQ_RESULT_NEXTSEQ;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static MAINSEQ_RESULT mainSeqFunc_update_top(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
	//�L�[�̕����擾�J�E���^�����Z�b�g
	GFL_UI_ResetFrameRate();
	//�]�[���X�V����
  fldmapMain_UpdateMoveZone( fieldWork );

  //�}�b�v�� �o�^����

  if( GAMESYSTEM_GetEvent(gsys) == NULL) {

    // ���[���V�X�e�����C��
    FIELD_RAIL_MAN_Update(fieldWork->railMan, GFL_UI_KEY_GetCont() );
    FIELD_RAIL_MAN_UpdateCamera(fieldWork->railMan);
    
    //�o�^�e�[�u�����ƂɌʂ̃��C���������Ăяo��
    fieldWork->func_tbl->main_func( fieldWork, &fieldWork->now_pos );
    
    //Map�V�X�e���Ɉʒu��n���Ă���B
    //���ꂪ�Ȃ��ƃ}�b�v�ړ����Ȃ��̂Œ���
    FLDMAPPER_SetPos( fieldWork->g3Dmapper, &fieldWork->now_pos );
  }
  
  //���@�X�V
  FIELD_PLAYER_Update( fieldWork->field_player );

  //�ʐM�p����(�v���C���[�̍��W�̐ݒ�Ƃ�
  FIELD_COMM_MAIN_UpdateCommSystem( fieldWork,
      fieldWork->gsys, fieldWork->field_player, fieldWork->commSys );
  
  FIELD_SUBSCREEN_Main(fieldWork->fieldSubscreenWork);
  FIELD_DEBUG_UpdateProc( fieldWork->debugWork );

	fldmap_G3D_Control( fieldWork );
  
  if( fieldWork->fldMMdlSys != NULL ){
    FLDMMDLSYS_UpdateProc( fieldWork->fldMMdlSys );
  }
  
  FLDEFF_CTRL_Update( fieldWork->fldeff_ctrl );

  return MAINSEQ_RESULT_CONTINUE;
}

static MAINSEQ_RESULT mainSeqFunc_update_tail(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{ 
  FIELD_SUBSCREEN_Draw(fieldWork->fieldSubscreenWork);
  FIELD_CAMERA_Main( fieldWork->camera_control, GFL_UI_KEY_GetCont() );
  
  FLDMSGBG_PrintMain( fieldWork->fldMsgBG );
  
	fldmap_G3D_Draw( fieldWork );
	GFL_CLACT_SYS_Main(); // CLSYS���C��
  
	// �Q�[���f�[�^�̃t���[�������p�J�E���^�����Z�b�g
	GAMEDATA_ResetFrameSpritCount(GAMESYSTEM_GetGameData(gsys));
  return MAINSEQ_RESULT_CONTINUE;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static MAINSEQ_RESULT mainSeqFunc_free(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{ 
  //�C�x���g�N���`�F�b�N���~����
  GAMESYSTEM_EVENT_EntryCheckFunc(gsys, NULL, NULL);
  
  { //�A�N�^�[�����v���C���[���݈ʒu��PLAYER_WORK�ɔ��f����
    VecFx32 player_pos;
    PLAYER_WORK * pw = GAMESYSTEM_GetMyPlayerWork(gsys);
    FIELD_PLAYER_GetPos(fieldWork->field_player, &player_pos);
    PLAYERWORK_setPosition(pw, &player_pos);
  }
  if (ZONEDATA_DEBUG_IsRailMap(fieldWork->map_id) == TRUE)
  {
		GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
    RAIL_LOCATION railLoc;
    FIELD_RAIL_MAN_GetLocation(fieldWork->railMan, &railLoc);
    GAMEDATA_SetRailLocation(gamedata, &railLoc);
  }
  
  //�t�B�[���h�G���J�E���g�j��
  FIELD_ENCOUNT_Delete( fieldWork->encount );

  //���o�[�̊J��
	{
		GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
		GAMEDATA_SetSubScreenMode(gamedata, FIELD_SUBSCREEN_Exit(fieldWork->fieldSubscreenWork ));
	}
  
  // �t�B�[���h�G�t�F�N�g�j��
  FLDEFF_CTRL_Delete( fieldWork->fldeff_ctrl );
  
  // �V�C�V�X�e���j��
  FIELD_WEATHER_Exit( fieldWork->weather_sys );
  fieldWork->weather_sys = NULL;

  // ���C�g�V�X�e���j��
  FIELD_LIGHT_Delete( fieldWork->light );
  
  // �t�H�O�V�X�e���j��
  FIELD_FOG_Delete( fieldWork->fog );

  // ���[������j��
  FIELD_RAIL_MAN_Delete( fieldWork->railMan );
  
  FIELD_CAMERA_Delete( fieldWork->camera_control );

	//�t�B�[���h�ʐM�폜
	FIELD_COMM_MAIN_TermSystem( fieldWork, fieldWork->commSys );
  
  //�o�^�e�[�u�����ƂɌʂ̏I���������Ăяo��
  fieldWork->func_tbl->delete_func(fieldWork);
  fldmap_ClearMapCtrlWork( fieldWork );
  
  fldmapMain_FLDMMDL_Finish(fieldWork);
  
  //���@�j��
  FIELD_PLAYER_Delete( fieldWork->field_player );
  
  FLDMAPPER_ReleaseData( fieldWork->g3Dmapper );
  
  FLDMSGBG_Delete( fieldWork->fldMsgBG );
  
  FIELD_DEBUG_Delete( fieldWork->debugWork );

	GAMEDATA_SetFrameSpritEnable(GAMESYSTEM_GetGameData(gsys), FALSE);
	GFL_UI_StartFrameRateMode( GFL_UI_FRAMERATE_60 );

  return MAINSEQ_RESULT_NEXTSEQ;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static MAINSEQ_RESULT mainSeqFunc_end(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{ 

	GFL_TCB_DeleteTask( fieldWork->g3dVintr );

	FLDMAPPER_Delete( fieldWork->g3Dmapper );

	fldmap_G3D_Unload( fieldWork );	//�R�c�f�[�^�j��
	
	GFL_CLACT_SYS_Delete(); // CLACT�j��
	
	GFL_BMPWIN_Exit();
	
	fldmap_BG_Exit( fieldWork );
  return MAINSEQ_RESULT_FINISH;
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h�}�b�v�@���C��
 * @param	gsys	GAMESYS_WORK
 * @param	fieldWork FIELDMAP_WORK
 * @retval	TRUE    �t�B�[���h�}�b�v�I��
 * @retval  FALSE   ����p����
 */
//--------------------------------------------------------------
BOOL FIELDMAP_Main( GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
  static const FIELDMAP_MAIN_FUNC mainfuncTable[FLDMAPSEQ_MAX][2] = { 
    //FLDMAPSEQ_SETUP_SYSTEM: ��{�V�X�e���Z�b�g�A�b�v
    { 
      mainSeqFunc_setup_system,
      mainSeqFunc_setup_system,
    },
    //FLDMAPSEQ_SETUP: �Z�b�g�A�b�v
    { 
      mainSeqFunc_setup,
      mainSeqFunc_setup,
    },
    //FLDMAPSEQ_READY: �Z�b�g�A�b�v����
    { 
      mainSeqFunc_ready,
      mainSeqFunc_ready,
    },
    //FLDMAPSEQ_UPDATE: �t�B�[���h�}�b�v�X�V
    { 
      mainSeqFunc_update_top,
      mainSeqFunc_update_tail,
    },
    //FLDMAPSEQ_FREE: �t�B�[���h�}�b�v�J��
    { 
      mainSeqFunc_free,
      mainSeqFunc_free,
    },
    //FLDMAPSEQ_END: �I��
    { 
      mainSeqFunc_end,
      mainSeqFunc_end,
    },
  };

  MAINSEQ_RESULT result;
  FIELDMAP_MAIN_FUNC func;


  GF_ASSERT(fieldWork->seq < NELEMS(mainfuncTable));
	fieldWork->timer++;
	
  result = MAINSEQ_RESULT_CONTINUE;
  func = mainfuncTable[fieldWork->seq][fieldWork->seq_switch];
  if (func != NULL) 
  { 
    result = func(gsys, fieldWork);
  }

  switch (result) 
  { 
  case MAINSEQ_RESULT_CONTINUE:
    fieldWork->seq_switch = !fieldWork->seq_switch;
    break;
  case MAINSEQ_RESULT_NEXTSEQ:
    fieldWork->seq ++;
    fieldWork->seq_switch = 0;
    break;
  case MAINSEQ_RESULT_FINISH:
    return TRUE;
  }
  return FALSE;
}


//======================================================================
//	�t�B�[���h�}�b�v�@�c�[��
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h�}�b�v�@�I�����N�G�X�g�Z�b�g
 * @param fieldWork FIELDMAP_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELDMAP_Close( FIELDMAP_WORK *fieldWork )
{
	fieldWork->gamemode = GAMEMODE_FINISH;
	fieldWork->seq = FLDMAPSEQ_FREE;
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�}�b�v�@�X�V�\��ԃ`�F�b�N
 * @param	fieldWork FIELDMAP_WORK
 * @retval BOOL TRUE=�X�V�\
 */
//--------------------------------------------------------------
const BOOL FIELDMAP_IsReady( const FIELDMAP_WORK *fieldWork )
{
	return (fieldWork->gamemode == GAMEMODE_NORMAL);
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�}�b�v�@�t�B�[���h�����X�V
 * @param fieldWork FIELDMAP_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELDMAP_ForceUpdate( FIELDMAP_WORK *fieldWork )
{
  // ���[���V�X�e�����C��
  FIELD_RAIL_MAN_Update(fieldWork->railMan, GFL_UI_KEY_GetCont() );
  FIELD_RAIL_MAN_UpdateCamera(fieldWork->railMan);
  
	//�o�^�e�[�u�����ƂɌʂ̃��C���������Ăяo��
	fieldWork->func_tbl->main_func( fieldWork, &fieldWork->now_pos );
	
	//Map�V�X�e���Ɉʒu��n���Ă���B
	//���ꂪ�Ȃ��ƃ}�b�v�ړ����Ȃ��̂Œ���
	FLDMAPPER_SetPos( fieldWork->g3Dmapper, &fieldWork->now_pos );
}

//======================================================================
//	�t�B�[���h�}�b�v�@�Q�ƁA�ݒ�
//======================================================================
//--------------------------------------------------------------
/**
 * FIELDMAP_WORK FIELD_COMM_MAIN�擾
 * @param	fieldWork	FIELDMAP_WORK
 * @retval	void*
 */
//--------------------------------------------------------------
void * FIELDMAP_GetCommSys( const FIELDMAP_WORK *fieldWork )
{
	return (void*)fieldWork->commSys;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK FLDMSGBG�擾
 * @param	fieldWork	FIELDMAP_WORK
 * @retval FLDMSGBG*
 */
//--------------------------------------------------------------
FLDMSGBG * FIELDMAP_GetFldMsgBG( FIELDMAP_WORK *fieldWork )
{
	return fieldWork->fldMsgBG;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK FIELD_CAMERA�擾
 * @param	fieldWork	FIELDMAP_WORK
 * @retval FIELD_CAMERA*
 */
//--------------------------------------------------------------
FIELD_CAMERA * FIELDMAP_GetFieldCamera( FIELDMAP_WORK *fieldWork )
{
	return fieldWork->camera_control;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[������V�X�e���̎擾
 */
//-----------------------------------------------------------------------------
FIELD_RAIL_MAN * FIELDMAP_GetFieldRailMan( FIELDMAP_WORK *fieldWork )
{
	return fieldWork->railMan;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK FIELD_LIGHT�擾
 * @param	fieldWork	FIELDMAP_WORK
 * @retval FIELD_LIGHT*
 */
//--------------------------------------------------------------
FIELD_LIGHT * FIELDMAP_GetFieldLight( FIELDMAP_WORK *fieldWork )
{
	return fieldWork->light;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK FIELD_FOG_WORK�擾
 * @param	fieldWork	FIELDMAP_WORK
 * @retval FIELD_FOG_WORK
 */
//--------------------------------------------------------------
FIELD_FOG_WORK * FIELDMAP_GetFieldFog( FIELDMAP_WORK *fieldWork )
{
	return fieldWork->fog;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK FIELD_WEATHER*�擾
 * @param	fieldWork	FIELDMAP_WORK
 * @retval FIELD_WEATHER*
 */
//--------------------------------------------------------------
FIELD_WEATHER * FIELDMAP_GetFieldWeather( FIELDMAP_WORK *fieldWork )
{
	return fieldWork->weather_sys;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK FLDMMDLSYS�擾
 * @param	fieldWork	FIELDMAP_WORK
 * @retval FLDMMDLSYS*
 */
//--------------------------------------------------------------
FLDMMDLSYS * FIELDMAP_GetFldMMdlSys( FIELDMAP_WORK *fieldWork )
{
	return fieldWork->fldMMdlSys;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK FIELD_DEBUG_WORK�擾
 * @param	fieldWork FIELDMAP_WORK
 * @retval	FIELD_DEBUG_WORK*
 */
//--------------------------------------------------------------
FIELD_DEBUG_WORK * FIELDMAP_GetDebugWork( FIELDMAP_WORK *fieldWork )
{
	return fieldWork->debugWork;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK GAMESYS_WORK�擾
 * @param	fieldWork	FIELDMAP_WORK
 * @retval GAMESYS_WORK*
 */
//--------------------------------------------------------------
GAMESYS_WORK * FIELDMAP_GetGameSysWork( FIELDMAP_WORK *fieldWork )
{
	return fieldWork->gsys;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK MAP_MATRIX�擾
 * @param fieldWork	FIELDMAP_WORK
 * @retval MAP_MATRIX*
 */
//--------------------------------------------------------------
MAP_MATRIX * FIELDMAP_GetMapMatrix( FIELDMAP_WORK *fieldWork )
{
	return fieldWork->pMapMatrix;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK HEAPID�擾
 * @param fieldWork	FIELDMAP_WORK
 * @retval HEAPID
 */
//--------------------------------------------------------------
HEAPID FIELDMAP_GetHeapID( FIELDMAP_WORK *fieldWork )
{
	return( fieldWork->heapID );
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK GFL_BBDACT_SYS�擾
 * @param fieldWork FIELDMAP_WORK
 * @retval GFL_BBDACT_SYS
 */
//--------------------------------------------------------------
GFL_BBDACT_SYS * FIELDMAP_GetBbdActSys( FIELDMAP_WORK *fieldWork )
{
	return fieldWork->bbdActSys;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK FLDMAPPER*�擾
 * @param fieldWork FIELDMAP_WORK
 * @retval FLDMAPPER*
 */
//--------------------------------------------------------------
FLDMAPPER * FIELDMAP_GetFieldG3Dmapper( FIELDMAP_WORK *fieldWork )
{
	return fieldWork->g3Dmapper;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK �}�b�v�R���g���[���p���[�N�擾
 * @param fieldWork FIELDMAP_WORK
 * @retval void* �}�b�v�R���g���[���p���[�N
 */
//--------------------------------------------------------------
void * FIELDMAP_GetMapCtrlWork( FIELDMAP_WORK *fieldWork )
{
	GF_ASSERT( fieldWork->mapCtrlWork != NULL ); //���o�^�Q�Ƃ͋֎~
	return fieldWork->mapCtrlWork;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK �}�b�v�R���g���[���p���[�N�Z�b�g
 * @param fieldWork FIELDMAP_WORK
 * @param	ctrlWork �}�b�v�R���g���[���p���[�N
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELDMAP_SetMapCtrlWork( FIELDMAP_WORK *fieldWork, void *ctrlWork )
{
	GF_ASSERT( fieldWork->mapCtrlWork == NULL ); //��d�o�^�֎~
	fieldWork->mapCtrlWork = ctrlWork;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK FIELD_PLAYER�擾
 * @param fieldWork FIELDMAP_WORK
 * @retval FIELD_PLAYER*
 */
//--------------------------------------------------------------
FIELD_PLAYER * FIELDMAP_GetFieldPlayer( FIELDMAP_WORK *fieldWork )
{
	GF_ASSERT( fieldWork->field_player != NULL ); //���o�^�Q�Ƃ͋֎~
	return fieldWork->field_player;
}

//--------------------------------------------------------------
/**
 * @brief  FIELD_SUBSCREEN_WORK�𓾂�
 * @param fieldWork FIELDMAP_WORK
 * @retval FIELD_SUBSCREEN_WORK*
 */
//--------------------------------------------------------------
FIELD_SUBSCREEN_WORK * FIELDMAP_GetFieldSubscreenWork(
    FIELDMAP_WORK *fieldWork )
{
	return fieldWork->fieldSubscreenWork;
}

//--------------------------------------------------------------
/**
 * @brief  FIELD_SUBSCREEN_WORK��ݒ肷��
 * @param  fieldWork FIELDMAP_WORK
 * @retval FIELD_SUBSCREEN_WORK*
 */
//--------------------------------------------------------------
void FIELDMAP_SetFieldSubscreenWork(
    FIELDMAP_WORK *fieldWork, FIELD_SUBSCREEN_WORK *pWork )
{
  fieldWork->fieldSubscreenWork = pWork;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK FLDEFF_CTRL�擾
 * @param fieldWork FIELDMAP_WORK
 * @retval FLDEFF_CTRL*
 */
//--------------------------------------------------------------
FLDEFF_CTRL * FIELDMAP_GetFldEffCtrl( FIELDMAP_WORK *fieldWork )
{
  return fieldWork->fldeff_ctrl;
}

//======================================================================
//	�t�B�[���h�}�b�v�@�T�u�@BG�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * BG ������
 * @param fieldWork FIELDMAP_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static void	fldmap_BG_Init( FIELDMAP_WORK *fieldWork )
{
	//�a�f�V�X�e���N��
	GFL_BG_Init( fieldWork->heapID );
	
	//�w�i�F�p���b�g�]��
  GFL_BG_LoadPalette( GFL_BG_FRAME0_M, (void*)fldmapdata_bgColorTable, 16*2, 0 ); //���C����ʂ̔w�i�F
  GFL_BG_LoadPalette( GFL_BG_FRAME0_S, (void*)fldmapdata_bgColorTable, 16*2, 0 ); //�T�u��ʂ̔w�i�F

	//�a�f���[�h�ݒ�
	GFL_BG_SetBGMode( &fldmapdata_bgsysHeader );

	//�a�f�R���g���[���ݒ�
	G2S_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2, GX_BLEND_PLANEMASK_BG3, 16, 8 );
	
	GFL_BG_SetBGControl3D( FIELD_3D_FRAME_PRI );

	//�f�B�X�v���C�ʂ̑I��
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_MAIN );
	GFL_DISP_SetDispOn();
}

//--------------------------------------------------------------
/**
 * BG �폜
 * @param fieldWork FIELDMAP_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static void	fldmap_BG_Exit( FIELDMAP_WORK *fieldWork )
{
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_MAIN );
	GFL_G3D_Exit();
	GFL_BG_Exit();
}

//======================================================================
//	�t�B�[���h�}�b�v�@�T�u�@G3D�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * G3D �Z�b�g�A�b�v�R�[���o�b�N
 * @param nothing
 * @retval nothing
 */
//--------------------------------------------------------------
static void fldmap_G3D_CallBackSetUp( void )
{
	//�e��`�惂�[�h�̐ݒ�(�V�F�[�h���A���`�G�C���A�X��������)
	G3X_SetShading( GX_SHADING_TOON );
	G3X_AntiAlias( TRUE );
	G3X_AlphaTest( FALSE, 0 );	// �A���t�@�e�X�g�@�@�I�t
	G3X_AlphaBlend( TRUE );

	// �t�H�O�Z�b�g�A�b�v
  G3X_SetFog(TRUE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x0400, 0x00e00 );
  G3X_SetFogColor(FIELD_DEFAULT_FOG_COLOR, 0);
  G3X_SetFogTable(fldmapdata_fogColorTable);
	
	// �N���A�J���[�̐ݒ�
	//color,alpha,depth,polygonID,fog
	G3X_SetClearColor(GX_RGB(0,0,0),0,0x7fff,63,FALSE);
	
	G3X_SetEdgeColorTable( fldmapdata_edgeColorTable ); 
	G3X_EdgeMarking( FALSE );
	
	// �r���[�|�[�g�̐ݒ�
	G3_ViewPort(0, 0, 255, 191);
	GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_MANUAL, GX_BUFFERMODE_W );
}

//--------------------------------------------------------------
/**
 * G3D ���[�h
 * @param fieldWork FIELDMAP_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static void fldmap_G3D_Load( FIELDMAP_WORK *fieldWork )
{
	fieldWork->bbdActSys = GFL_BBDACT_CreateSys(
			FIELD_G3D_BBDACT_RESMAX, FIELD_G3D_BBDACT_ACTMAX,
			fldmap_G3D_BBDTrans, fieldWork->heapID );

	//�J�����쐬
	fieldWork->g3Dcamera = GFL_G3D_CAMERA_CreateDefault(
			&fldmapdata_cameraPos, &fldmapdata_cameraTarget, fieldWork->heapID );
	
	//���C�g�쐬
	fieldWork->g3Dlightset = GFL_G3D_LIGHT_Create(
			&fldmapdata_light0Setup, fieldWork->heapID );

	//�J�������C�g0���f
	GFL_G3D_CAMERA_Switching( fieldWork->g3Dcamera );
	GFL_G3D_LIGHT_Switching( fieldWork->g3Dlightset );
	
}

//--------------------------------------------------------------
/**
 * G3D ����
 * @param fieldWork FIELMAP_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static void fldmap_G3D_Control( FIELDMAP_WORK * fieldWork )
{
	FLDMAPPER_Main( fieldWork->g3Dmapper );
	GFL_BBDACT_Main( fieldWork->bbdActSys );
	
	FIELD_WEATHER_Main( fieldWork->weather_sys, fieldWork->heapID );
	FIELD_FOG_Main( fieldWork->fog );
  {
		static int time = 14400;
		time += 30;
		time %= 24*3600;
		FIELD_LIGHT_Main( fieldWork->light, time );
	}
}

//--------------------------------------------------------------
/**
 * G3D �`��
 * @param fieldWork FIELMAP_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static void fldmap_G3D_Draw( FIELDMAP_WORK * fieldWork )
{
	GFL_G3D_CAMERA_Switching( fieldWork->g3Dcamera );
	GFL_G3D_LIGHT_Switching( fieldWork->g3Dlightset );
  
  FLDMSGBG_PrintG3D( fieldWork->fldMsgBG );
  
  FLDEFF_CTRL_Draw( fieldWork->fldeff_ctrl );

	FLDMAPPER_Draw( fieldWork->g3Dmapper, fieldWork->g3Dcamera );
 
	GFL_BBDACT_Draw(
			fieldWork->bbdActSys, fieldWork->g3Dcamera, fieldWork->g3Dlightset );
	
  FIELD_WEATHER_3DWrite( fieldWork->weather_sys );	// �V�C�`�揈��
	
  GFL_G3D_DRAW_End(); //�`��I���i�o�b�t�@�X���b�v�j
}

//--------------------------------------------------------------
/**
 * G3D �j��
 * @param fieldWork FIELMAP_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static void fldmap_G3D_Unload( FIELDMAP_WORK * fieldWork )
{
	GFL_G3D_LIGHT_Delete( fieldWork->g3Dlightset );
	GFL_G3D_CAMERA_Delete( fieldWork->g3Dcamera );
	GFL_BBDACT_DeleteSys( fieldWork->bbdActSys );
}

//--------------------------------------------------------------
/**
 * G3D VBlank����
 * @param TCB GFL_TCB
 * @param work tcb work
 * @retval nothing
 */
//--------------------------------------------------------------
static void	fldmap_G3D_VBlank( GFL_TCB *tcb, void *work )
{
	FIELDMAP_WORK * fieldWork = (FIELDMAP_WORK*)work;
	GFL_CLACT_SYS_VBlankFunc();	//�Z���A�N�^�[VBlank
}

//--------------------------------------------------------------
/**
 * G3D �r���{�[�h�A�N�^�[�pVRAM�]������
 * @param type GFL_BBDACT_TRANSTYPE
 * @param dst �e�N�X�`���f�[�^�]����A�h���X
 * @param src �e�N�X�`���f�[�^�\�[�X�A�h���X
 * @param	siz �e�N�X�`���f�[�^�T�C�Y
 * @retval nothing
 */
//--------------------------------------------------------------
static void	fldmap_G3D_BBDTrans(
		GFL_BBDACT_TRANSTYPE type, u32 dst, u32 src, u32 siz )
{
	NNS_GFD_DST_TYPE transType;
	
	if( type == GFL_BBDACT_TRANSTYPE_DATA ){
		transType = NNS_GFD_DST_3D_TEX_VRAM;
	}else{
		transType = NNS_GFD_DST_3D_TEX_PLTT;
	}
	
	NNS_GfdRegisterNewVramTransferTask( transType, dst, (void*)src, siz );
}

//======================================================================
//	�t�B�[���h�}�b�v�@�T�u�@���샂�f���֘A
//======================================================================
//--------------------------------------------------------------
/**
 * ���샂�f���@������
 * @param fieldWork FIELDMAP_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static void fldmapMain_FLDMMDL_Init( FIELDMAP_WORK *fieldWork )
{
	GAMEDATA *gdata = GAMESYSTEM_GetGameData( fieldWork->gsys );
	FLDMMDLSYS *fmmdlsys = GAMEDATA_GetFldMMdlSys( gdata );
		
	fieldWork->fldMMdlSys = fmmdlsys;
	FLDMMDLSYS_SetFieldMapWork( fmmdlsys, fieldWork );

	FLDMMDLSYS_SetupProc( fmmdlsys,	//���샂�f���V�X�e���@�Z�b�g�A�b�v
		fieldWork->heapID, fieldWork->g3Dmapper );
		
	FLDMMDL_BLACTCONT_Setup(		//���샂�f���r���{�[�h�@�Z�b�g�A�b�v
		fieldWork->fldMMdlSys, fieldWork->bbdActSys, 32 );
	
	{ //�r���{�[�h���\�[�X�o�^
			int list_area_id = 0;
			MMDL_LIST mlist;
			fldmap_FLDMMDL_InitList( &mlist, list_area_id, fieldWork->heapID );
			FLDMMDL_BLACTCONT_AddOBJCodeRes( fieldWork->fldMMdlSys, HERO );
			FLDMMDL_BLACTCONT_AddResourceTex(
					fieldWork->fldMMdlSys, mlist.id_list, mlist.count );
	}

	//���샂�f���`��@�Z�b�g�A�b�v
	FLDMMDLSYS_SetupDrawProc( fieldWork->fldMMdlSys );
	
	//���샂�f���@���A
	FLDMMDLSYS_Pop( fieldWork->fldMMdlSys );
  
  if( fieldWork->func_tbl->type == FLDMAP_CTRLTYPE_GRID ){
    FLDMMDLSYS_SetJoinShadow( fmmdlsys, TRUE );
  }else{
    FLDMMDLSYS_SetJoinShadow( fmmdlsys, FALSE );
  }
}

//--------------------------------------------------------------
/**
 * ���샂�f���@�I��
 * @param fieldWork FIELDMAP_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static void fldmapMain_FLDMMDL_Finish( FIELDMAP_WORK *fieldWork )
{
	FLDMMDLSYS_Push( fieldWork->fldMMdlSys );
	FLDMMDLSYS_DeleteProc( fieldWork->fldMMdlSys );
	fieldWork->fldMMdlSys = NULL;
}

//--------------------------------------------------------------
/**
 * ���샂�f���@���X�g�쐬
 * @param mlist MMDL_LIST
 * @param list_id �쐬���郊�X�gID
 * @param	heapID �e���|�����pHEAPID
 * @retval nothing
 */
//--------------------------------------------------------------
static void fldmap_FLDMMDL_InitList(
		MMDL_LIST *mlist, int list_id, HEAPID heapID )
{
	int i = 0;
	u16 *pList;
	pList = GFL_ARC_LoadDataAlloc( ARCID_FLDMMDL_LIST, list_id, heapID );
	mlist->count = 0;
	
	while( pList[i] != OBJCODEMAX ){
		mlist->id_list[i] = pList[i];
		i++;
		GF_ASSERT( i < MMDL_LIST_MAX );
	}
	
	OS_Printf( "���f�����X�g���� %d\n", i );
	
	mlist->count = i;
	mlist->id_list[i] = OBJCODEMAX;
	GFL_HEAP_FreeMemory( pList );
}

//======================================================================
//	�t�B�[���h�}�b�v�@�T�u�@�C�x���g
//======================================================================
//--------------------------------------------------------------
/**
 * �C�x���g �}�b�v�ڑ��`�F�b�N
 * @param gsys GAMESYS_WORK
 * @param fieldWork FIELDMAP_WORK
 * @param now_pos �`�F�b�N������W
 * @retval GMEVENT NULL=�ڑ��Ȃ�
 */
//--------------------------------------------------------------
static GMEVENT * fldmap_Event_CheckConnect(
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, const VecFx32 *now_pos )
{
	const CONNECT_DATA * cnct;
	GAMEDATA * gamedata = GAMESYSTEM_GetGameData(gsys);
	EVENTDATA_SYSTEM *evdata = GAMEDATA_GetEventData(gamedata);
	int idx = EVENTDATA_SearchConnectIDByPos(evdata, now_pos);
	
	if( idx == EXIT_ID_NONE ){
		return NULL;
	}

	//�}�b�v�J�ڔ����̏ꍇ�A�o�������L�����Ă���
	{
		LOCATION ent_loc;
		LOCATION_Set(
				&ent_loc, fieldWork->map_id, idx, 0,
				now_pos->x, now_pos->y, now_pos->z);
		GAMEDATA_SetEntranceLocation(gamedata, &ent_loc);
	}

	cnct = EVENTDATA_GetConnectByID(evdata, idx);
	
	if (CONNECTDATA_IsSpecialExit(cnct)) {
		//����ڑ��悪�w�肳��Ă���ꍇ�A�L�����Ă������ꏊ�ɔ��
		const LOCATION * sp = GAMEDATA_GetSpecialLocation(gamedata);
		return EVENT_ChangeMap(gsys, fieldWork, sp);
	}else{
		LOCATION loc_req;
		CONNECTDATA_SetNextLocation(cnct, &loc_req);
		return EVENT_ChangeMap(gsys, fieldWork, &loc_req);
	}
}

//--------------------------------------------------------------
/**
 * �C�x���g �L�[���̓C�x���g�N���`�F�b�N
 * @param gsys GAMESYS_WORK
 * @param	fieldWork FIELDMAP_WORK
 * @retval GMEVENT NULL �C�x���g����
 */
//--------------------------------------------------------------
static GMEVENT * fldmap_Event_CheckPushConnect(
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
	VecFx32 now_pos = fieldWork->now_pos;
	
	switch( GFL_UI_KEY_GetCont() ) {
	case PAD_KEY_UP:		now_pos.z -= FX32_ONE * 16; break;
	case PAD_KEY_DOWN:	now_pos.z += FX32_ONE * 16; break;
	case PAD_KEY_LEFT:	now_pos.x -= FX32_ONE * 16; break;
	case PAD_KEY_RIGHT:	now_pos.x += FX32_ONE * 16; break;
	default: return NULL;
	}
	
	{	
		u32 attr = 0;
		FLDMAPPER_GRIDINFO gridInfo;
		FLDMAPPER *g3Dmapper = fieldWork->g3Dmapper;
		
		if( FLDMAPPER_GetGridInfo(g3Dmapper,&now_pos,&gridInfo) == TRUE ){
			attr = gridInfo.gridData[0].attr;
		}

		if( attr == 0 ){
			return NULL;
		}
	}
	
	return fldmap_Event_CheckConnect(gsys, fieldWork, &now_pos);
}

//--------------------------------------------------------------
/**
 * �T�u�X�N���[������̃C�x���g�N���`�F�b�N
 * @param gsys GAMESYS_WORK
 * @param	fieldWork FIELDMAP_WORK
 * @retval GMEVENT NULL �C�x���g����
 */
//--------------------------------------------------------------
static GMEVENT * fldmap_Event_Check_SubScreen(
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
  GMEVENT* event=NULL;
  
  switch(FIELD_SUBSCREEN_GetAction(FIELDMAP_GetFieldSubscreenWork(fieldWork))){
  case FIELD_SUBSCREEN_ACTION_DEBUGIRC:
		{
			GAME_COMM_SYS_PTR gcsp = GAMESYSTEM_GetGameCommSysPtr(gsys);
			GAME_COMM_NO no = GameCommSys_BootCheck(gcsp);
			if(GAME_COMM_NO_FIELD_BEACON_SEARCH == no){
				GameCommSys_ExitReq(gcsp);
			}
			if((GAME_COMM_NO_FIELD_BEACON_SEARCH == no) || (GAME_COMM_NO_NULL == no)){
				event = EVENT_IrcBattle(gsys, fieldWork, NULL, TRUE);
			}
		}
    break;
#if PM_DEBUG
  case FIELD_SUBSCREEN_ACTION_DEBUG_PALACEJUMP:
		event = DEBUG_PalaceJamp(fieldWork , gsys, fieldWork->field_player);
		break;
#endif
	}
	if(event)
	{
		FIELD_SUBSCREEN_GrantPermission(FIELDMAP_GetFieldSubscreenWork(fieldWork));
	}
	else
	{
		FIELD_SUBSCREEN_ResetAction(FIELDMAP_GetFieldSubscreenWork(fieldWork));
	}
  return event;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static GMEVENT * checkRailExit(GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldWork)
{
  int idx;
  VecFx32 pos;
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData(gsys);
  EVENTDATA_SYSTEM *evdata = GAMEDATA_GetEventData(gamedata);

  if (ZONEDATA_DEBUG_IsRailMap(fieldWork->map_id) == FALSE) return NULL;
  FIELD_RAIL_MAN_GetPos(fieldWork->railMan, &pos);
  idx = EVENTDATA_SearchConnectIDBySphere(evdata, &pos);
  if (fieldWork->firstConnectEventID == idx) return NULL;
  if (idx == EXIT_ID_NONE)
  {
    fieldWork->firstConnectEventID = idx;
    return NULL;
  }

  {
		LOCATION loc_req;
    const CONNECT_DATA * cnct;
    cnct = EVENTDATA_GetConnectByID(evdata, idx);
	
		CONNECTDATA_SetNextLocation(cnct, &loc_req);
		return EVENT_ChangeMap(gsys, fieldWork, &loc_req);
  }
#if 0
	//�}�b�v�J�ڔ����̏ꍇ�A�o�������L�����Ă���
	{
		LOCATION ent_loc;
		LOCATION_Set(
				&ent_loc, fieldWork->map_id, idx, 0,
				now_pos->x, now_pos->y, now_pos->z);
		GAMEDATA_SetEntranceLocation(gamedata, &ent_loc);
	}
#endif

#if 0
	if (CONNECTDATA_IsSpecialExit(cnct)) {
		//����ڑ��悪�w�肳��Ă���ꍇ�A�L�����Ă������ꏊ�ɔ��
		const LOCATION * sp = GAMEDATA_GetSpecialLocation(gamedata);
		return EVENT_ChangeMap(gsys, fieldWork, sp);
	}else{
		LOCATION loc_req;
		CONNECTDATA_SetNextLocation(cnct, &loc_req);
		return EVENT_ChangeMap(gsys, fieldWork, &loc_req);
	}
#endif

}

//--------------------------------------------------------------
/**
 * �C�x���g�@�C�x���g�N���`�F�b�N
 * @param	gsys GAMESYS_WORK
 * @param work FIELDMAP_WORK
 * @retval GMEVENT NULL=�C�x���g����
 */
//--------------------------------------------------------------
static GMEVENT * fldmapFunc_Event_CheckEvent( GAMESYS_WORK *gsys, void *work )
{
	enum{
		resetCont = PAD_BUTTON_L | PAD_BUTTON_R | PAD_BUTTON_START,
		chgCont = PAD_BUTTON_L | PAD_BUTTON_R | PAD_BUTTON_SELECT
	};
	
	GMEVENT *event;
	FIELDMAP_WORK *fieldWork = work;
	int	trg = GFL_UI_KEY_GetTrg();
	int cont = GFL_UI_KEY_GetCont();
	PLAYER_MOVE_STATE state;
  PLAYER_MOVE_VALUE value;

  { //���@����X�e�[�^�X�X�V
    FIELD_PLAYER_UpdateMoveStatus( fieldWork->field_player );
  }
  
  value = FIELD_PLAYER_GetMoveValue( fieldWork->field_player );
  state = FIELD_PLAYER_GetMoveState( fieldWork->field_player );
  
#ifdef  PM_DEBUG
	//�\�t�g���Z�b�g�`�F�b�N
	if( (cont&resetCont) == resetCont ){
		return DEBUG_EVENT_GameEnd(gsys, fieldWork);
	}
	
	//�}�b�v�ύX�`�F�b�N
	if( (cont&chgCont) == chgCont ){
		return DEBUG_EVENT_ChangeToNextMap(gsys, fieldWork);
	}
	
	//�f�o�b�O���j���[�N���`�F�b�N
	//if( trg == PAD_BUTTON_SELECT ){
  if( (trg & PAD_BUTTON_X) && (cont & PAD_BUTTON_R) )
  {
		return DEBUG_EVENT_DebugMenu(gsys, fieldWork, 
				fieldWork->heapID, ZONEDATA_GetMapRscID(fieldWork->map_id));
	}
#endif //debug
	
  //�퓬�ڍs�`�F�b�N
  if( fieldWork->func_tbl->type == FLDMAP_CTRLTYPE_GRID ){
    #ifdef PM_DEBUG
    if( (cont & PAD_BUTTON_R) == 0 ){
      if( state == PLAYER_MOVE_STATE_END ){
        if( FIELD_ENCOUNT_CheckEncount(fieldWork->encount) == TRUE ){
          return EVENT_Battle( gsys, fieldWork );
        }
      }
    }
    #else
    if( state == PLAYER_MOVE_STATE_END ){
			if(!GFL_NET_IsInit())
			{
				if( FIELD_ENCOUNT_CheckEncount(fieldWork->encount) == TRUE ){
					return EVENT_Battle( gsys, fieldWork );
				}
			}
    }
    #endif
  }
  
#if 0
	//���W�ڑ��`�F�b�N
	event = fldmap_Event_CheckConnect(gsys, fieldWork, &fieldWork->now_pos);
	if( event != NULL ){
		return event;
	}
#endif
	
  //���[���̏ꍇ�̃}�b�v�J�ڃ`�F�b�N
  event = checkRailExit(gsys, fieldWork);
  if( event != NULL ){
    return event;
  }

	//�L�[���͐ڑ��`�F�b�N
	event = fldmap_Event_CheckPushConnect(gsys, fieldWork);
	if( event != NULL ){
		return event;
	}
	
	//���j���[�N���`�F�b�N
	if( trg == PAD_BUTTON_X ){
		if(WIPE_SYS_EndCheck()){
  			return EVENT_FieldMapMenu( gsys, fieldWork, fieldWork->heapID );
		}
	}

	//�T�u�X�N���[������̃C�x���g�N���`�F�b�N
	event = fldmap_Event_Check_SubScreen(gsys, fieldWork);
	if( event != NULL )
  {
		return event;
	}
  
	///�ʐM�p��b����(��
	//�b�������鑤
	if( trg == PAD_BUTTON_A ){
		if( FIELD_COMM_MAIN_CanTalk( fieldWork->commSys ) == TRUE ){
			return FIELD_COMM_EVENT_StartTalk(
					gsys , fieldWork , fieldWork->commSys );
		}
	}

	//�b���������鑤(���ňꏏ�ɘb�����Ԃ��̃`�F�b�N�����Ă��܂�
	if( FIELD_COMM_MAIN_CheckReserveTalk( fieldWork->commSys ) == TRUE ){
		return FIELD_COMM_EVENT_StartTalkPartner(
				gsys , fieldWork , fieldWork->commSys );
	}
	
	//�t�B�[���h�b���|���`�F�b�N
	if(	fieldWork->func_tbl->type == FLDMAP_CTRLTYPE_GRID )
	{
		if( trg == PAD_BUTTON_A )
		{
			int gx,gy,gz;
			FLDMMDL *fmmdl_talk;
			FIELD_PLAYER_GetFrontGridPos( fieldWork->field_player, &gx, &gy, &gz );
			fmmdl_talk = FLDMMDLSYS_SearchGridPos(
					fieldWork->fldMMdlSys, gx, gz, FALSE );
			
			if( fmmdl_talk != NULL )
			{
				u32 scr_id = FLDMMDL_GetEventID( fmmdl_talk );
				FLDMMDL *fmmdl_player = FIELD_PLAYER_GetFldMMdl(
						fieldWork->field_player );
				return EVENT_FieldTalk( gsys, fieldWork,
					scr_id, fmmdl_player, fmmdl_talk, fieldWork->heapID );
			}
		}
	}
	
	//�f�o�b�O�F�p���X�Ŗ؂ɐG�ꂽ�烏�[�v
  {
    GMEVENT *ev;
    ev =  DEBUG_PalaceTreeMapWarp(fieldWork, gsys, fieldWork->field_player, fieldWork->commSys);
    if(ev != NULL){
      return ev;
    }
	}
	return NULL;
}

//======================================================================
//	�t�B�[���h�}�b�v�@�T�u�@�]�[���X�V
//======================================================================
//--------------------------------------------------------------
/**
 * ���@�ړ��ɂ��]�[���X�V
 * @param fieldWork FIELDMAP_WORK
 * @retval BOOL TRUE=�]�[���X�V����
 */
//--------------------------------------------------------------
static BOOL fldmapMain_UpdateMoveZone( FIELDMAP_WORK *fieldWork )
{
	if( fldmap_CheckPlayerPosUpdate(fieldWork) == TRUE ){
		if( fldmap_CheckMoveZoneChange(fieldWork) == TRUE ){
			fldmap_ZoneChange( fieldWork );
			return( TRUE );
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * ���@���W�X�V�`�F�b�N
 * @param	fieldWork	FIELDMAP_WORK
 * @retval	BOOL	TRUE=���W�X�V
 */
//--------------------------------------------------------------
static BOOL fldmap_CheckPlayerPosUpdate( FIELDMAP_WORK *fieldWork )
{
	LOCATION *lc = &fieldWork->location;
	GAMEDATA *gdata = GAMESYSTEM_GetGameData( fieldWork->gsys );
	PLAYER_WORK *player = GAMEDATA_GetMyPlayerWork( gdata );
	const VecFx32 *pos = PLAYERWORK_getPosition( player );
	
	if( pos->x != lc->pos.x || pos->z != lc->pos.z ){
		lc->pos = *pos;
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �]�[���؂�ւ��`�F�b�N 
 * @param	fieldWork	FIELDMAP_WORK
 * @retval	BOOL TRUE=�]�[���؂�ւ�����
 */
//--------------------------------------------------------------
static BOOL fldmap_CheckMoveZoneChange( FIELDMAP_WORK *fieldWork )
{
	LOCATION *lc = &fieldWork->location;
	MAP_MATRIX *mat = fieldWork->pMapMatrix;
	
	if( MAP_MATRIX_CheckVectorPosRange(mat,lc->pos.x,lc->pos.z) == TRUE ){
		u32 zone_id =
			MAP_MATRIX_GetVectorPosZoneID( mat, lc->pos.x, lc->pos.z );
		
		//���� ID�������ꍇ�͍X�V���Ȃ�
		if( zone_id != MAP_MATRIX_ZONE_ID_NON ){
			if( zone_id != (u32)lc->zone_id ){
				return( TRUE );
			}
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �]�[���؂�ւ����̏���
 * @param	fieldWork	FIELDMAP_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fldmap_ZoneChange( FIELDMAP_WORK *fieldWork )
{
	LOCATION *lc = &fieldWork->location;
	GAMEDATA *gdata = GAMESYSTEM_GetGameData( fieldWork->gsys );
	EVENTDATA_SYSTEM *evdata = GAMEDATA_GetEventData( gdata );
	FLDMMDLSYS *fmmdlsys = fieldWork->fldMMdlSys;
	
	MAP_MATRIX *mat = fieldWork->pMapMatrix;
	u32 new_zone_id = MAP_MATRIX_GetVectorPosZoneID(
			mat, lc->pos.x, lc->pos.z );
	u32 old_zone_id = lc->zone_id;
	
	GF_ASSERT( new_zone_id != MAP_MATRIX_ZONE_ID_NON );
	
	//���]�[���z�u���샂�f���폜
	FLDMMDLSYS_DeleteZoneUpdateFldMMdl( fmmdlsys );
	
	//���̃C�x���g�f�[�^�����[�h
	EVENTDATA_SYS_Load( evdata, new_zone_id );
	
	//�V�K�]�[���ɔz�u���铮�샂�f���Z�b�g
	zoneChange_SetMMdl( fmmdlsys, evdata, new_zone_id );
	
	//BGM�؂�ւ�
	zoneChange_SetBGM( gdata, new_zone_id );
	
	//�V�󃊃N�G�X�g
	zoneChange_SetWeather( fieldWork, new_zone_id );
	
	//PLAYER_WORK�X�V
	zoneChange_UpdatePlayerWork( gdata, new_zone_id );
	
	//�]�[��ID�X�V
	lc->zone_id = new_zone_id;
	
	KAGAYA_Printf( "�]�[���X�V���� %d -> %d\n", lc->zone_id, new_zone_id );
}

//--------------------------------------------------------------
/**
 * �]�[���؂�ւ����̏����@�V�K�]�[�����샂�f���Z�b�g
 * @param	fmmdlsys	FLDMMDLSYS
 * @param	evdata	EVENTDATA_SYSTEM
 * @param	zone_id	���̃]�[��ID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void zoneChange_SetMMdl(
		FLDMMDLSYS *fmmdlsys, EVENTDATA_SYSTEM *evdata, u32 zone_id )
{
	u16 count = EVENTDATA_GetNpcCount( evdata );
	
	if( count ){
		const FLDMMDL_HEADER *header = EVENTDATA_GetNpcData( evdata );
		FLDMMDLSYS_SetFldMMdl( fmmdlsys, header, zone_id, count );
	}
}

//--------------------------------------------------------------
/**
 * �]�[���؂�ւ����̏����@BGM�؂�ւ�
 * @param	gdata	GAMEDATA
 * @param	zone_id	���̃]�[��ID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void zoneChange_SetBGM( GAMEDATA *gdata, u32 zone_id )
{
	u16 trackBit = 0xfcff;	// track 9,10 OFF

	u16 nextBGM = ZONEDATA_GetBGMID(
			zone_id, GAMEDATA_GetSeasonID(gdata) );
	
	if( nextBGM != 0 ){
		if( PMSND_GetNextBGMsoundNo() != nextBGM ){
			PMSND_PlayNextBGM_EX( nextBGM, trackBit, 60, 0 );
		}
	}
}

//--------------------------------------------------------------
/**
 * �]�[���؂�ւ����̏����@�V�󃊃N�G�X�g
 * @param	fieldWork	FIELDMAP_WORK*
 * @param	zone_id	���̃]�[��ID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void zoneChange_SetWeather( FIELDMAP_WORK *fieldWork, u32 zone_id )
{
	u32 w_no = WEATHER_NO_NUM;
	FIELD_WEATHER *we = FIELDMAP_GetFieldWeather( fieldWork );
	
	switch( zone_id ){
	case ZONE_ID_T01:
		w_no = WEATHER_NO_SNOW;
		break;
	case ZONE_ID_R01:
		w_no = WEATHER_NO_RAIN;
		break;
	}
	
	if( w_no != WEATHER_NO_NUM && w_no != FIELD_WEATHER_GetWeatherNo(we) ){
		FIELD_WEATHER_Change( we, w_no );
	}
}

//--------------------------------------------------------------
/**
 * �]�[���؂�ւ����̏����@PLAYER_WORK�X�V
 * @param	gdata	GAMEDATA
 * @param	zone_id	���̃]�[��ID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void zoneChange_UpdatePlayerWork( GAMEDATA *gdata, u32 zone_id )
{
	PLAYER_WORK *player = GAMEDATA_GetMyPlayerWork( gdata );
	PLAYERWORK_setZoneID( player, zone_id );
}

//======================================================================
//	�t�B�[���h�}�b�v�@���̑�
//======================================================================
//--------------------------------------------------------------
/**
 * FIELDMAP_WORK �}�b�v�R���g���[���p���[�N�N���A
 * @param fieldWork FIELDMAP_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static void fldmap_ClearMapCtrlWork( FIELDMAP_WORK *fieldWork )
{
	fieldWork->mapCtrlWork = NULL;
}

//======================================================================
//	data
//======================================================================
//--------------------------------------------------------------
/// �f�B�X�v���C���f�[�^
//--------------------------------------------------------------
static const GFL_DISP_VRAM fldmapdata_dispVram =
{
	GX_VRAM_BG_128_C, //���C��2D�G���W����BG�Ɋ��蓖�� 
	GX_VRAM_BGEXTPLTT_NONE, //���C��2D�G���W����BG�g���p���b�g�Ɋ��蓖��
	GX_VRAM_SUB_BG_32_H, //�T�u2D�G���W����BG�Ɋ��蓖��
	GX_VRAM_SUB_BGEXTPLTT_NONE, //�T�u2D�G���W����BG�g���p���b�g�Ɋ��蓖��
	GX_VRAM_OBJ_64_E, //���C��2D�G���W����OBJ�Ɋ��蓖��
	GX_VRAM_OBJEXTPLTT_NONE, //���C��2D�G���W����OBJ�g���p���b�g�ɂɊ��蓖��
	GX_VRAM_SUB_OBJ_16_I, //�T�u2D�G���W����OBJ�Ɋ��蓖��
	GX_VRAM_SUB_OBJEXTPLTT_NONE, //�T�u2D�G���W����OBJ�g���p���b�g�ɂɊ��蓖��
	GX_VRAM_TEX_01_AB, //�e�N�X�`���C���[�W�X���b�g�Ɋ��蓖��
	GX_VRAM_TEXPLTT_0_G, //�e�N�X�`���p���b�g�X���b�g�Ɋ��蓖��
	GX_OBJVRAMMODE_CHAR_1D_64K, // ���C��OBJ�}�b�s���O���[�h
	GX_OBJVRAMMODE_CHAR_1D_32K, // �T�uOBJ�}�b�s���O���[�h
};

//--------------------------------------------------------------
///	BG�w�b�_�[
//--------------------------------------------------------------
static const GFL_BG_SYS_HEADER fldmapdata_bgsysHeader = {
	GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_3D
};

//--------------------------------------------------------------
///	�Z���A�N�^�[�@�������f�[�^
//--------------------------------------------------------------
static const GFL_CLSYS_INIT fldmapdata_CLSYS_Init =
{
	0, 0,
	0, 512,
	GFL_CLSYS_OAMMAN_INTERVAL, 128-GFL_CLSYS_OAMMAN_INTERVAL,
	GFL_CLSYS_OAMMAN_INTERVAL, 128-GFL_CLSYS_OAMMAN_INTERVAL, //�ʐM�A�C�R������
	0,
	FIELD_CLSYS_RESOUCE_MAX,
	FIELD_CLSYS_RESOUCE_MAX,
	FIELD_CLSYS_RESOUCE_MAX,
	FIELD_CLSYS_RESOUCE_MAX,
  16, 16,
};

//--------------------------------------------------------------
///	�J���������ݒ�f�[�^
//--------------------------------------------------------------
static const VecFx32 fldmapdata_cameraTarget	= { 0, 0, 0 };

static const VecFx32 fldmapdata_cameraPos	= {
	0, (FX32_ONE * 64), (FX32_ONE * 128)
};

//--------------------------------------------------------------
///	���C�g�����ݒ�f�[�^
//--------------------------------------------------------------
static const GFL_G3D_LIGHT_DATA light0Tbl[] =
{
	{ 0, {{-(FX16_ONE-1),-(FX16_ONE-1),-(FX16_ONE-1)},GX_RGB(31,31,31)} },
	{ 1, {{(FX16_ONE-1),-(FX16_ONE-1),-(FX16_ONE-1)},GX_RGB(31,31,31)} },
	{ 2, {{-(FX16_ONE-1),-(FX16_ONE-1),-(FX16_ONE-1)},GX_RGB(31,31,31)} },
	{ 3, {{-(FX16_ONE-1),-(FX16_ONE-1),-(FX16_ONE-1)},GX_RGB(31,31,31)} },
};

static const GFL_G3D_LIGHTSET_SETUP fldmapdata_light0Setup = {
	light0Tbl, NELEMS(light0Tbl)
};

//--------------------------------------------------------------
///	3D �G�b�W�J���[
//--------------------------------------------------------------
static const GXRgb fldmapdata_edgeColorTable[8] = {
	GX_RGB(10,10,10),GX_RGB(10,10,10),GX_RGB(10,10,10),GX_RGB(10,10,10),
	GX_RGB(10,10,10),GX_RGB(10,10,10),GX_RGB(10,10,10),GX_RGB(10,10,10),
};

//--------------------------------------------------------------
//  3D  �t�H�O�J���[
//--------------------------------------------------------------
#define FOG_VALUE(v) (u32)(((v*16)<<0) | ((v*16+4)<<8) | ((v*16+8)<<16) | ((v*16+12)<<24))
static const u32 fldmapdata_fogColorTable[8] = {  
  FOG_VALUE(0),
  FOG_VALUE(1),
  FOG_VALUE(2),
  FOG_VALUE(3),
  FOG_VALUE(4),
  FOG_VALUE(5),
  FOG_VALUE(6),
  FOG_VALUE(7),
};
//--------------------------------------------------------------
//  BG  �w�i�J���[
//--------------------------------------------------------------
static const u16 fldmapdata_bgColorTable[16] = { 
  FIELD_DEFAULT_BG_COLOR, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

