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
#include "arc_def.h"

#include "net/network_define.h"
#include "system/g3d_tool.h"

#include "print/gf_font.h"

#include "field/field_const.h"
#include "fieldmap.h"
#include "field_common.h"
#include "field_flagcontrol.h" //FIELD_FLAGCONT_INIT�`

#include "field_player.h"
#include "field_camera.h"
#include "field_data.h"
#include "field/field_msgbg.h"
#include "field_encount.h"
#include "effect_encount.h"

#include "weather.h"
#include "field_fog.h"
#include "field_light.h"
#include "field_buildmodel.h"
#include "field/field_nogrid_mapper.h"
#include "field/fld_scenearea.h"
#include "field/fld_scenearea_loader.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/playerwork.h"
#include "gamesystem/game_event.h"
#include "field/zonedata.h"
#include "field/areadata.h"
#include "warpdata.h"   //ARRIVEDATA_SetArriveFlag

#include "field_comm_actor.h"
#include "field_comm/field_comm_main.h"
#include "field_comm/field_comm_event.h"
#include "field_comm/intrude_field.h"

#include "fldmmdl.h"

#include "field_debug.h"

#include "field/map_matrix.h"
#include "field_event_check.h"  

#include "eventdata_local.h"
#include "field/eventdata_sxy.h"

#include "field_effect.h"

#include "field_player_grid.h"

#include "fieldskill_mapeff.h"

#include "fieldmap_func.h"
#include "field/fieldmap_call.h"

#include "arc/fieldmap/zone_id.h"
#include "field/weather_no.h"
#include "sound/pm_sndsys.h"
//#include "sound/wb_sound_data.sadl"
#include "net_app/union/union_main.h"

#include "message.naix" //NARC_message_d_field_dat
#include "msg/msg_d_field.h"  //DEBUG_FIELD_STR00 DEBUG_FIELD_C_STR10

#include "debug/debugwin_sys.h" //�f�o�b�O���j���[

#if USE_DEBUGWIN_SYSTEM
#include "mapdatafunc/field_func_random_generate.h" //�f�o�b�O�o�^�̂���
#endif //USE_DEBUGWIN_SYSTEM

#include "field_place_name.h"

#include "field_sound.h"

#include "fieldmap_ctrl.h"
#include "fieldmap_ctrl_grid.h"
#include "fieldmap_ctrl_nogrid_work.h"
#include "fieldmap_ctrl_hybrid.h"

#include "field_gimmick.h"
#include "gmk_tmp_wk.h"

#include "field_status_local.h" //FIELD_STATUS_�`
#include "script.h"   //SCRIPT_CallZoneChangeScript

#include "fld_particle.h"

#include "field_wfbc.h"

#include "savedata/sodateya_work.h"
#include "sodateya.h"
#include "system/net_err.h"

#include "field_task.h"
#include "field_task_manager.h"


//======================================================================
//	DEBUG��`
//======================================================================
#ifdef PM_DEBUG

#define DEBUG_FIELDMAP_DRAW_MICRO_SECOND_CHECK    // �t�B�[���h�}�b�v�`��ɂ����鏈�����Ԃ����߂�

#define DEBUG_FIELDMAP_ZONE_CHANGE_SYNC    // �]�[���`�F���W�ɕK�v�ȃV���N�����Ď�

#endif

// �t�B�[���h�}�b�v�`��ɂ����鏈�����Ԃ����߂�
#ifdef DEBUG_FIELDMAP_DRAW_MICRO_SECOND_CHECK
#define DEBUG_FIELDMAP_DRAW_MICRO_SECOND_CHECK_DRAW_KEY ( PAD_BUTTON_L )
#endif //DEBUG_FIELDMAP_DRAW_MICRO_SECOND_CHECK

#define FLD3DCUTIN_SIZE   (0xc000)   //�t�B�[���h3�c�J�b�g�C���̃q�[�v�T�C�Y

//======================================================================
//	define
//======================================================================
#define CROSSFADE_MODE

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
  
  ///FLD_G3DOBJ ���\�[�X�ő�
  FIELD_G3DOBJ_RES_MAX = 64,
  ///FLD_G3DOBJ �I�u�W�F�N�g�ő�
  FIELD_G3DOBJ_OBJ_MAX = 64,
};

///g3Dutil�Ŏg�p���郊�\�[�X�̍ő�ݒ�\��
#define G3D_UTIL_RESCOUNT	(512)
///g3Dutil�Ŏg�p����I�u�W�F�N�g�̍ő�ݒ�\��
#define G3D_UTIL_OBJCOUNT	(128)

#define MMDL_LIST_MAX	(64) ///<���샂�f�����X�g�ő�

/// FLDMAPFUNC�V�X�e���̃^�X�N�ő吔
#define FLDMAPFUNC_TASK_MAX	( 32 )

//�M�~�b�N�e���|�������[�N���A�T�C���h�c
#define GMK_TMP_NO_ASSIGN_ID  (0xffffffff)
//�g���n�a�i���\�[�X�ő�o�^��
#define EXP_OBJ_RES_MAX   (60)
//�g���n�a�i�o�^�ő吔
#define EXP_OBJ_MAX   (35)


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

//�t�B�[���h�p�M�~�b�N�e���|�������[�N
typedef struct GMK_TMP_WORK_tag
{
  u32 AssignID;
  void * Work;
}GMK_TMP_WORK;

//--------------------------------------------------------------
///	FIELDMAP_WORK
//--------------------------------------------------------------
struct _FIELDMAP_WORK
{
	HEAPID heapID;
	GAMESYS_WORK *gsys;
  GAMEDATA * gamedata;

  AREADATA * areadata;
	
	FIELD_CAMERA *camera_control;
	FIELD_LIGHT *light;
	FIELD_FOG_WORK *fog;
	FIELD_ZONEFOGLIGHT	 *zonefog;
	
	FIELD_WEATHER *weather_sys;
  FIELD_SUBSCREEN_WORK* fieldSubscreenWork;
	
	FIELD_COMM_MAIN *commSys;
	
	FLDMSGBG *fldMsgBG;
  FLDMSGWIN *goldMsgWin;  // �������\���p�E�B���h�E

	FIELD_PLACE_NAME* placeNameSys;	// �n���\���E�B���h�E
  FLD_EXP_OBJ_CNT_PTR ExpObjCntPtr;
	
	MMDLSYS *fldMMdlSys;

  FLDNOGRID_MAPPER* nogridMapper;
  FLD_SCENEAREA* sceneArea;
  FLD_SCENEAREA_LOADER* sceneAreaLoader;
	
	FLDMAPPER *g3Dmapper;
	MAP_MATRIX *pMapMatrix;
	FLDMAPPER_RESISTDATA map_res;
	FIELD_PLAYER *field_player;
	FIELD_ENCOUNT *encount;
	FLDEFF_CTRL *fldeff_ctrl;

	FIELDSKILL_MAPEFF * fieldskill_mapeff;
  

  GFL_G3D_CAMERA *g3Dcamera; //g3Dcamera Lib �n���h��
	GFL_G3D_LIGHTSET *g3Dlightset; //g3Dlight Lib �n���h��
	GFL_TCB *g3dVintr; //3D�pvIntrTask�n���h��
	GFL_BBDACT_SYS *bbdActSys; //�r���{�[�h�A�N�g�V�X�e���ݒ�n���h��

  FLDMAPFUNC_SYS * fldmapFuncSys;
	
	GAMEMODE gamemode;
	FLDMAPSEQ seq;
  u32 seq_switch;
	int timer;
	u16 map_id;
	LOCATION location;
	
  const VecFx32 *target_now_pos_p;
	VecFx32 now_pos;
  
	const DEPEND_FUNCTIONS *func_tbl;
	void *mapCtrlWork;
	
	FIELD_DEBUG_WORK *debugWork;


  int firstConnectEventID;

  GFL_TCBSYS* fieldmapTCB;
  void* fieldmapTCBSysWork;
  GMK_TMP_WORK  GmkTmpWork;     //�M�~�b�N�e���|�������[�N�|�C���^

  FLD_PRTCL_SYS_PTR FldPrtclSys;
  FLD3D_CI_PTR Fld3dCiPtr;

  SODATEYA* sodateya;  // ��ĉ�

  DRAW3DMODE Draw3DMode;
  
  FLD_G3DOBJ_CTRL *fieldG3dObjCtrl;

  FIELD_TASK_MAN* taskManager;  // �^�X�N�}�l�[�W��
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
static void FIELD_EDGEMARK_Setup(const AREADATA * areadata);

//fldmmdl
static void fldmapMain_MMDL_Init( FIELDMAP_WORK *fieldWork );
static void fldmapMain_MMDL_Finish( FIELDMAP_WORK *fieldWork );
static void fldmap_MMDL_InitList(
		MMDL_LIST *mlist, int list_id, HEAPID heapID );

//zonechange
static BOOL fldmapMain_UpdateMoveZone( FIELDMAP_WORK *fieldWork );
static BOOL fldmap_CheckPlayerPosUpdate( FIELDMAP_WORK *fieldWork );
static BOOL fldmap_CheckMoveZoneChange( FIELDMAP_WORK *fieldWork );
static void fldmap_ZoneChange( FIELDMAP_WORK *fieldWork );

static void zoneChange_SetMMdl( GAMEDATA *gdata,
		MMDLSYS *fmmdlsys, EVENTDATA_SYSTEM *evdata, u32 zone_id );
static void zoneChange_SetMMdlZoneWFBC( GAMEDATA *gdata,
		FIELDMAP_WORK* fieldWork, u32 zone_id );
static void zoneChange_SetBGM( GAMEDATA *gdata, u32 zone_id );
static void zoneChange_SetWeather( FIELDMAP_WORK *fieldWork, u32 zone_id );
static void zoneChange_SetZoneFogLight( FIELDMAP_WORK *fieldWork, u32 zone_id );
static void zoneChange_UpdatePlayerWork( GAMEDATA *gdata, u32 zone_id );
static void zoneChange_SetCameraArea( FIELDMAP_WORK* fieldWork, u32 zone_id );


//etc
static void fldmap_ClearMapCtrlWork( FIELDMAP_WORK *fieldWork );
static void setupCameraArea( FIELDMAP_WORK *fieldWork, u32 zone_id, HEAPID heapID );

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

static void InitGmkTmpWork(GMK_TMP_WORK *tmpWork);

//3�c�`�惂�[�h
static void Draw3DNormalMode( FIELDMAP_WORK * fieldWork );
static void Draw3DCutinMode(FIELDMAP_WORK * fieldWork);
static void Draw3DScrnTexMode(FIELDMAP_WORK * fieldWork);

typedef void (*DRAW3DMODE_FUNC)(FIELDMAP_WORK * fieldWork);

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

	fieldWork = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELDMAP_WORK) );
	fieldWork->heapID = heapID;
	fieldWork->gamemode = GAMEMODE_BOOT;
	fieldWork->gsys = gsys;
  fieldWork->gamedata = GAMESYSTEM_GetGameData(gsys);
	
	{
		PLAYER_WORK *pw = GAMESYSTEM_GetMyPlayerWork(gsys);
		fieldWork->map_id = PLAYERWORK_getZoneID( pw );
	}
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
    fieldWork->areadata = AREADATA_Create( heapID,
        ZONEDATA_GetAreaID(fieldWork->map_id), GAMEDATA_GetSeasonID(gamedata) );
  }
	
	//�}�b�v�R���g���[��
	fieldWork->func_tbl = FIELDDATA_GetFieldFunctions( fieldWork->map_id );

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
  GAMESYS_WORK *gsys = fieldWork->gsys;

  //�G���A�f�[�^
  AREADATA_Delete( fieldWork->areadata );

  //���[�N�J��
	GFL_HEAP_FreeMemory( fieldWork );
  
  //GAMESYSTEM���ŕێ����Ă��郏�[�N��NULL��
  GAMESYSTEM_SetFieldMapWork( gsys, NULL );
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
	//GFL_DISP_ClearVRAM( GX_VRAM_D );
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

  // TCB
  {
    u32 task_max = 32;
    u32     size = GFL_TCB_CalcSystemWorkSize( task_max );
    fieldWork->fieldmapTCBSysWork = GFL_HEAP_AllocMemory( heapID, size );
    fieldWork->fieldmapTCB = GFL_TCB_Init( task_max, fieldWork->fieldmapTCBSysWork );
  }

  // �^�X�N�}�l�[�W��
  fieldWork->taskManager = FIELD_TASK_MAN_Create( 10, fieldWork->heapID );

	GFL_UI_StartFrameRateMode( GFL_UI_FRAMERATE_30 );
	
  return MAINSEQ_RESULT_NEXTSEQ;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static MAINSEQ_RESULT mainSeqFunc_setup(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );

  //�t�B�[���h3�c�J�b�g�C���q�[�v�m��
  GFL_HEAP_CreateHeap( HEAPID_FIELDMAP, HEAPID_FLD3DCUTIN, FLD3DCUTIN_SIZE );

  fieldWork->fldMsgBG = FLDMSGBG_Create( fieldWork->heapID, fieldWork->g3Dcamera );
#ifndef CROSSFADE_MODE
	FLDMSGBG_SetupResource( fieldWork->fldMsgBG );
#endif
  fieldWork->goldMsgWin = NULL;

#ifndef CROSSFADE_MODE
  // �n���\���V�X�e���쐬
  fieldWork->placeNameSys = FIELD_PLACE_NAME_Create( fieldWork->heapID, fieldWork->fldMsgBG );
#else
  fieldWork->placeNameSys = NULL;
#endif
  // �t���b�V���`�F�b�N
  {
    FIELD_STATUS * fldstatus = GAMEDATA_GetFieldStatus( fieldWork->gamedata );
    fieldWork->fieldskill_mapeff = FIELDSKILL_MAPEFF_Create( 
        FIELD_STATUS_GetFieldSkillMapEffectMsk(fldstatus), fieldWork->heapID ); 
  }

  fieldWork->camera_control = FIELD_CAMERA_Create(
      ZONEDATA_GetCameraID(fieldWork->map_id),
			FIELD_CAMERA_MODE_CALC_CAMERA_POS,
      fieldWork->g3Dcamera,
      &fieldWork->now_pos,
      fieldWork->heapID );
  
  // �V�[���G���A
  fieldWork->sceneArea        = FLD_SCENEAREA_Create( fieldWork->heapID, fieldWork->camera_control );
  fieldWork->sceneAreaLoader  = FLD_SCENEAREA_LOADER_Create( fieldWork->heapID );

  // NOGRID�}�b�p�[����
  fieldWork->nogridMapper = FLDNOGRID_MAPPER_Create( fieldWork->heapID, fieldWork->camera_control, fieldWork->sceneArea, fieldWork->sceneAreaLoader );

  {
    FIELD_BMODEL_MAN * bmodel_man = FLDMAPPER_GetBuildModelManager( fieldWork->g3Dmapper );
    
    FIELDDATA_SetMapperData(fieldWork->map_id,
        fieldWork->areadata,
        &fieldWork->map_res,
        GAMEDATA_GetMapMatrix(fieldWork->gamedata) );

    //�����Ŕz�u���f�����X�g���Z�b�g����
    FIELD_BMODEL_MAN_Load(bmodel_man, fieldWork->map_id, fieldWork->areadata);
    // WFBC�X����ݒ�
    // @TODO�@��X�́A�p���X�ڑ���̐l�̊X����ݒ肷��
    FLDMAPPER_SetWfbcData( fieldWork->g3Dmapper, GAMEDATA_GetMyWFBCCoreData( fieldWork->gamedata ), MAPMODE_NORMAL );

    // WFBC�̐l��z�u
    zoneChange_SetMMdlZoneWFBC( gdata, fieldWork, fieldWork->map_id );
  }

  //�t�B�[���h�}�b�v�p���P�[�V�����쐬

  LOCATION_Set( &fieldWork->location, fieldWork->map_id, 0, 0, LOCATION_DEFAULT_EXIT_OFS,
      fieldWork->now_pos.x, fieldWork->now_pos.y, fieldWork->now_pos.z );
  
  //�}�b�v�f�[�^�o�^
  FLDMAPPER_ResistData( fieldWork->g3Dmapper, &fieldWork->map_res );

  //NOGRID�}�b�v�f�[�^�o�^
  {
    u32 raildata = ZONEDATA_GetRailDataID(fieldWork->map_id);
    if ( raildata != ZONEDATA_NO_RAILDATA_ID ){
      FLDNOGRID_MAPPER_ResistDataArc( fieldWork->nogridMapper, raildata, fieldWork->heapID );  
    }
  }

  //CAMERA_AREA�̔��f
  setupCameraArea( fieldWork, fieldWork->map_id, fieldWork->heapID );
  
  //���샂�f��������
  fldmapMain_MMDL_Init(fieldWork);
  
  //�t�B�[���h�G�t�F�N�g������
  fieldWork->fldeff_ctrl = FLDEFF_CTRL_Create(
      fieldWork, FLDEFF_PROCID_MAX, fieldWork->heapID );
  
  //�t�B�[���h�G�t�F�N�g�@�p�����^������
  FLDEFF_CTRL_SetTaskParam( fieldWork->fldeff_ctrl, FLDEFF_TASK_MAX );
  
  //�t�B�[���h�G�t�F�N�g�@�o�^
  FLDEFF_CTRL_RegistEffect( fieldWork->fldeff_ctrl,
    DATA_FLDEFF_RegistEffectGroundTbl, DATA_FLDEFF_RegistEffectGroundTblNum );
  
  {
    PLAYER_WORK *pw = GAMESYSTEM_GetMyPlayerWork(gsys);
    const u16 dir = PLAYERWORK_getDirection_Type( pw );
    const VecFx32 *pos = &pw->position;

    //���@�쐬
    {
      MYSTATUS *mystatus = GAMEDATA_GetMyStatus( gdata );
      int sex = MyStatus_GetMySex( mystatus );
      fieldWork->field_player = FIELD_PLAYER_Create(
          pw, fieldWork, pos, sex, fieldWork->heapID );
    }

    //�o�^�e�[�u�����ƂɌʂ̏������������Ăяo��
    fieldWork->now_pos = *pos;
    fieldWork->func_tbl->create_func(fieldWork, &fieldWork->now_pos, dir);

    FLDMAPPER_SetPos( fieldWork->g3Dmapper, &fieldWork->now_pos );
    
    TAMADA_Printf("start X,Y,Z=%d,%d,%d\n", FX_Whole(pos->x), FX_Whole(pos->y), FX_Whole(pos->z));
    TAMADA_Printf( "Start Dir = %04x\n", pw->direction );
  }

  //�M�~�b�N�e���|�������[�N������
  InitGmkTmpWork(&fieldWork->GmkTmpWork);

  //�g��3�c�I�u�W�F�N�g���W���[���쐬
  fieldWork->ExpObjCntPtr = FLD_EXP_OBJ_Create ( EXP_OBJ_RES_MAX, EXP_OBJ_MAX, fieldWork->heapID );
  
  //�G�b�W�}�[�L���O�ݒ�Z�b�g�A�b�v
  FIELD_EDGEMARK_Setup( fieldWork->areadata );

  // �t�H�O�V�X�e������
  fieldWork->fog	= FIELD_FOG_Create( fieldWork->heapID );
	
  // �]�[���t�H�O�V�X�e������
	fieldWork->zonefog = FIELD_ZONEFOGLIGHT_Create( fieldWork->heapID );
	FIELD_ZONEFOGLIGHT_Load( fieldWork->zonefog, ZONEDATA_GetFog(fieldWork->map_id), ZONEDATA_GetLight(fieldWork->map_id), fieldWork->heapID );

  // ���C�g�V�X�e������
  {
    fieldWork->light = FIELD_LIGHT_Create( AREADATA_GetLightType( fieldWork->areadata ), 
        GFL_RTC_GetTimeBySecond(), 
        fieldWork->fog, fieldWork->g3Dlightset, fieldWork->heapID );
  }

  // �V�C�V�X�e������
  fieldWork->weather_sys = FIELD_WEATHER_Init(
      fieldWork->camera_control,
      fieldWork->light,
      fieldWork->fog,
			fieldWork->zonefog, 
      fieldWork->heapID );
  
  // �V�C����
  FIELD_WEATHER_Set(
			fieldWork->weather_sys, ZONEDATA_GetWeatherID(fieldWork->map_id), fieldWork->heapID );

  //�t�B�[���h�M�~�b�N�Z�b�g�A�b�v
  FLDGMK_SetUpFieldGimmick(fieldWork);
  
  //���o�[�̏�����
	{
		fieldWork->fieldSubscreenWork = FIELD_SUBSCREEN_Init(fieldWork->heapID, fieldWork, GAMEDATA_GetSubScreenMode(gdata));
	}
  
  //�t�B�[���h�G���J�E���g������
  fieldWork->encount = FIELD_ENCOUNT_Create( fieldWork );
  
  //�ʐM�������R�[���o�b�N�Ăяo��
  {
    GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
    GameCommSys_Callback_FieldCreate( game_comm, fieldWork );
  }

	// �t�B�[���h�}�b�v�p����^�X�N�V�X�e��
	fieldWork->fldmapFuncSys = FLDMAPFUNC_Sys_Create( fieldWork, fieldWork->heapID, FLDMAPFUNC_TASK_MAX );

#ifndef CROSSFADE_MODE
  //�t�B�[���h�f�o�b�O������
  fieldWork->debugWork = FIELD_DEBUG_Init( fieldWork, fieldWork->heapID );

#if USE_DEBUGWIN_SYSTEM
	DEBUGWIN_InitProc( GFL_BG_FRAME1_M , FLDMSGBG_GetFontHandle(fieldWork->fldMsgBG) );
	DEBUGWIN_ChangeLetterColor( 31,31,31 );
	FIELD_FUNC_RANDOM_GENERATE_InitDebug
		( fieldWork->heapID, GAMEDATA_GetMyWFBCCoreData( fieldWork->gamedata ) );
#endif  //USE_DEBUGWIN_SYSTEM
#else
  fieldWork->debugWork = NULL;
#endif

  //�t�B�[���h�p�[�e�B�N��
  fieldWork->FldPrtclSys = FLD_PRTCL_Init(HEAPID_FLD3DCUTIN);
  //�t�B�[���h3D�J�b�g�C��
  fieldWork->Fld3dCiPtr = FLD3D_CI_Init(HEAPID_FLD3DCUTIN, fieldWork->FldPrtclSys);

  // ��ĉ�
  {
    SAVE_CONTROL_WORK* scw = GAMEDATA_GetSaveControlWork( fieldWork->gamedata );
    SODATEYA_WORK* work = SODATEYA_WORK_GetSodateyaWork( scw );
    fieldWork->sodateya = SODATEYA_Create( fieldWork->heapID, fieldWork, work );
  }

  //3�c�`�惂�[�h�͒ʏ�ŃZ�b�g�A�b�v
  fieldWork->Draw3DMode = DRAW3DMODE_NORMAL;

  return MAINSEQ_RESULT_NEXTSEQ;
}


//--------------------------------------------------------------
//--------------------------------------------------------------
static MAINSEQ_RESULT mainSeqFunc_ready(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{ 
	fldmap_G3D_Control( fieldWork );
	fldmap_G3D_Draw( fieldWork );
	GFL_CLACT_SYS_Main(); // CLSYS���C��
  
  if(fieldWork->fldMsgBG){ FLDMSGBG_PrintMain( fieldWork->fldMsgBG ); }
  
  if(fieldWork->debugWork){ FIELD_DEBUG_UpdateProc( fieldWork->debugWork ); }
  
  if( fieldWork->fldMMdlSys != NULL ){
    MMDLSYS_UpdateProc( fieldWork->fldMMdlSys );
  }
  
  if( FLDMAPPER_CheckTrans(fieldWork->g3Dmapper) == FALSE ){
    return MAINSEQ_RESULT_CONTINUE;
  }
  
  { //�t�B�[���h�������X�N���v�g�̌Ăяo��
    FIELD_STATUS * fldstatus = GAMEDATA_GetFieldStatus( fieldWork->gamedata );
    if ( FIELD_STATUS_GetFieldInitFlag( fldstatus ) ) 
    {
      SCRIPT_CallFieldInitScript( fieldWork->gsys, fieldWork->heapID );
      FIELD_STATUS_SetFieldInitFlag( GAMEDATA_GetFieldStatus( fieldWork->gamedata ), FALSE );
    } else {
      SCRIPT_CallFieldRecoverScript( fieldWork->gsys, fieldWork->heapID );
    }
  }

  FLDEFF_CTRL_Update( fieldWork->fldeff_ctrl );

	// �t�B�[���h�}�b�v�p����^�X�N�V�X�e��
	FLDMAPFUNC_Sys_Main( fieldWork->fldmapFuncSys );

  if (FIELDMAP_GetMapControlType(fieldWork) == FLDMAP_CTRLTYPE_NOGRID)
  {
    GAMEDATA * gamedata = GAMESYSTEM_GetGameData(gsys);
    EVENTDATA_SYSTEM *evdata = GAMEDATA_GetEventData(gamedata);
    // @TODO ����́A���[���}�b�v��ɂRDPOS�̃C�x���g��RAIL�̃C�x���g��
    // �������Ă���̂ŁB
    fieldWork->firstConnectEventID = 
      EVENTDATA_SearchConnectIDBySphere(evdata, &fieldWork->now_pos);
  }
  //�t�B�[���h�}�b�v�p�C�x���g�N���`�F�b�N���Z�b�g����
  if(fieldWork->map_id == ZONE_ID_UNION || fieldWork->map_id == ZONE_ID_CLOSSEUM || fieldWork->map_id == ZONE_ID_CLOSSEUM02){
    GAMESYSTEM_EVENT_EntryCheckFunc( gsys, FIELD_EVENT_CheckUnion, fieldWork );
  }
  else if( FIELDMAP_GetMapControlType( fieldWork ) == FLDMAP_CTRLTYPE_NOGRID ){
    GAMESYSTEM_EVENT_EntryCheckFunc( gsys, FIELD_EVENT_CheckNoGrid, fieldWork );
  }
  else if( FIELDMAP_GetMapControlType( fieldWork ) == FLDMAP_CTRLTYPE_HYBRID ){
    GAMESYSTEM_EVENT_EntryCheckFunc( gsys, FIELD_EVENT_CheckHybrid, fieldWork );
  }
  else{
    GAMESYSTEM_EVENT_EntryCheckFunc( gsys, FIELD_EVENT_CheckNormal_Wrap, fieldWork );
  }
  
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
    //�o�^�e�[�u�����ƂɌʂ̃��C���������Ăяo��
    fieldWork->func_tbl->main_func( fieldWork, &fieldWork->now_pos ); 
  }

  // �n���\���V�X�e�����쏈��
  if(fieldWork->placeNameSys){ FIELD_PLACE_NAME_Process( fieldWork->placeNameSys ); }

  // �t���b�V������
  FIELDSKILL_MAPEFF_Main( fieldWork->fieldskill_mapeff );

  //���@�X�V
  FIELD_PLAYER_Update( fieldWork->field_player );

  //�ʐM�p����(�v���C���[�̍��W�̐ݒ�Ƃ�
  IntrudeField_UpdateCommSystem( fieldWork, fieldWork->gsys, fieldWork->field_player ); 
  Union_Main(GAMESYSTEM_GetGameCommSysPtr(gsys), fieldWork);
  
  FIELD_SUBSCREEN_Main(fieldWork->fieldSubscreenWork);
  if(fieldWork->debugWork){ FIELD_DEBUG_UpdateProc( fieldWork->debugWork ); }
  
  if( fieldWork->fldMMdlSys != NULL ){
    MMDLSYS_UpdateProc( fieldWork->fldMMdlSys );
  }

  //�M�~�b�N����
  FLDGMK_MoveFieldGimmick(fieldWork);
  
  FLDEFF_CTRL_Update( fieldWork->fldeff_ctrl );

	// �t�B�[���h�}�b�v�p����^�X�N�V�X�e��
	FLDMAPFUNC_Sys_Main( fieldWork->fldmapFuncSys );

  if(fieldWork->fldMsgBG ){ FLDMSGBG_PrintMain( fieldWork->fldMsgBG ); }

  // TCB
  GFL_TCB_Main( fieldWork->fieldmapTCB );
  // �^�X�N�}�l�[�W��
  FIELD_TASK_MAN_Main( fieldWork->taskManager );

  //now_pos�X�V
  if( fieldWork->target_now_pos_p != NULL ){
    fieldWork->now_pos = *fieldWork->target_now_pos_p;
  }

  //Map�V�X�e���Ɉʒu��n���Ă���B
  //���ꂪ�Ȃ��ƃ}�b�v�ړ����Ȃ��̂Œ���
  FLDMAPPER_SetPos( fieldWork->g3Dmapper, &fieldWork->now_pos );

  // Visual�ɂ�����鏈��
	fldmap_G3D_Control( fieldWork );        

  return MAINSEQ_RESULT_CONTINUE;
}

static MAINSEQ_RESULT mainSeqFunc_update_tail(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{ 
  // �`��ɂ����� micro second ��\��
#ifdef DEBUG_FIELDMAP_DRAW_MICRO_SECOND_CHECK
  OSTick debug_fieldmap_start_tick = OS_GetTick(); 
  OSTick debug_fieldmap_end_tick;
#endif

  GFL_G3D_ClearG3dInfo();
  
  FIELD_SUBSCREEN_Draw(fieldWork->fieldSubscreenWork);
  FIELD_CAMERA_Main( fieldWork->camera_control, GFL_UI_KEY_GetCont() );
  
  if(fieldWork->fldMsgBG ){ FLDMSGBG_PrintMain( fieldWork->fldMsgBG ); }

  if(fieldWork->placeNameSys){ FIELD_PLACE_NAME_Draw( fieldWork->placeNameSys ); }
  
	MI_SetMainMemoryPriority(MI_PROCESSOR_ARM9);
	fldmap_G3D_Draw( fieldWork );
	MI_SetMainMemoryPriority(MI_PROCESSOR_ARM7);

	GFL_CLACT_SYS_Main(); // CLSYS���C��
  

	// �Q�[���f�[�^�̃t���[�������p�J�E���^�����Z�b�g
	GAMEDATA_ResetFrameSpritCount(GAMESYSTEM_GetGameData(gsys));


  // �`��ɂ����� micro second ��\��
  // 1�V���N�ɕ`��ȊO�̏��������Ă���̂ŁA
  // 10000 micro second�ȓ��ɂȂ��Ă���̂����z�ł��B
#ifdef DEBUG_FIELDMAP_DRAW_MICRO_SECOND_CHECK
  debug_fieldmap_end_tick = OS_GetTick();
  debug_fieldmap_end_tick -= debug_fieldmap_start_tick;

  if( GFL_UI_KEY_GetCont() & DEBUG_FIELDMAP_DRAW_MICRO_SECOND_CHECK_DRAW_KEY )
  {
    const GFL_D3D_INFO *info = GFL_G3D_GetG3dInfoPtr();
    OS_TPrintf( "draw_tick %d micro second\n", OS_TicksToMicroSeconds( debug_fieldmap_end_tick ) );
    OS_Printf( "poly_vtx = %d, %d\n",
        G3X_GetPolygonListRamCount(),
        G3X_GetVtxListRamCount()
        );
    OS_Printf( "total poly_vtx = %d, %d\n",
        info->TotalPolygonNum,
        info->TotalVertexNum
        );
  }
#endif

  return MAINSEQ_RESULT_CONTINUE;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static MAINSEQ_RESULT mainSeqFunc_free(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
  //�t�B�[���h3�c�J�b�g�C���R���g���[���j��
  FLD3D_CI_End(fieldWork->Fld3dCiPtr);
  //�t�B�[���h�p�[�e�B�N���j��
  FLD_PRTCL_End(fieldWork->FldPrtclSys);

  //�C�x���g�N���`�F�b�N���~����
  GAMESYSTEM_EVENT_EntryCheckFunc(gsys, NULL, NULL);
  
  //�A�N�^�[�����v���C���[���݈ʒu��PLAYER_WORK�ɔ��f����
  FIELD_PLAYER_Update( fieldWork->field_player );
#if 0
  { 
    VecFx32 player_pos;
    PLAYER_WORK * pw = GAMESYSTEM_GetMyPlayerWork(gsys);
    FIELD_PLAYER_GetPos(fieldWork->field_player, &player_pos);
    PLAYERWORK_setPosition(pw, &player_pos);
  }
  if (FIELDMAP_GetMapControlType(fieldWork) == FLDMAP_CTRLTYPE_NOGRID)
  {
    PLAYER_WORK * pw = GAMESYSTEM_GetMyPlayerWork(gsys);
    FIELDMAP_CTRL_NOGRID_WORK* p_mapctrl_work = FIELDMAP_GetMapCtrlWork( fieldWork );
    FIELD_PLAYER_NOGRID* p_nogrid_player = FIELDMAP_CTRL_NOGRID_WORK_GetNogridPlayerWork( p_mapctrl_work );
    RAIL_LOCATION railLoc;


    FIELD_PLAYER_NOGRID_GetLocation( p_nogrid_player, &railLoc );
    PLAYERWORK_setRailPosition(pw, &railLoc);
  }
#endif

	// �t�B�[���h�}�b�v�p����^�X�N�V�X�e��
	FLDMAPFUNC_Sys_Delete( fieldWork->fldmapFuncSys );
  
  //�ʐM�폜�R�[���o�b�N�Ăяo��
  {
    GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
    GameCommSys_Callback_FieldDelete( game_comm, fieldWork );
  }
  
  // �n���\���V�X�e���j��
  if(fieldWork->placeNameSys){ FIELD_PLACE_NAME_Delete( fieldWork->placeNameSys ); }

  //�M�~�b�N�I��
  FLDGMK_EndFieldGimmick(fieldWork);

  //�g��3�c�I�u�W�F�N�g���W���[���j��
  FLD_EXP_OBJ_Delete(fieldWork->ExpObjCntPtr);

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

	// �]�[���t�H�O�V�X�e���j��
	FIELD_ZONEFOGLIGHT_Delete( fieldWork->zonefog );
  
  // �t�H�O�V�X�e���j��
  FIELD_FOG_Delete( fieldWork->fog );

  // NOGRID�}�b�p�[�j��
  FLDNOGRID_MAPPER_Delete( fieldWork->nogridMapper );

  // �V�[���G���A�j��
  FLD_SCENEAREA_LOADER_Delete( fieldWork->sceneAreaLoader );
  FLD_SCENEAREA_Delete( fieldWork->sceneArea );


  FIELD_CAMERA_Delete( fieldWork->camera_control );

  //�o�^�e�[�u�����ƂɌʂ̏I���������Ăяo��
  fieldWork->func_tbl->delete_func(fieldWork);
  fldmap_ClearMapCtrlWork( fieldWork );
  
  fldmapMain_MMDL_Finish(fieldWork);
  
  //���@�j��
  FIELD_PLAYER_Delete( fieldWork->field_player );
  
  FLDMAPPER_ReleaseData( fieldWork->g3Dmapper );

  // �������\���E�B���h�E�j��
  if( fieldWork->goldMsgWin )
  {
    FLDMSGWIN_Delete( fieldWork->goldMsgWin );
  }
  
  if(fieldWork->fldMsgBG ){ FLDMSGBG_Delete( fieldWork->fldMsgBG ); }

  if(fieldWork->debugWork){ FIELD_DEBUG_Delete( fieldWork->debugWork ); }

	GAMEDATA_SetFrameSpritEnable(GAMESYSTEM_GetGameData(gsys), FALSE);
	GFL_UI_StartFrameRateMode( GFL_UI_FRAMERATE_60 );

  // ��ĉ�
  SODATEYA_Delete( fieldWork->sodateya );

	FIELDSKILL_MAPEFF_Delete( fieldWork->fieldskill_mapeff );

#if USE_DEBUGWIN_SYSTEM
  FIELD_FUNC_RANDOM_GENERATE_TermDebug();
  DEBUGWIN_ExitProc();
#endif  //USE_DEBUGWIN_SYSTEM

  //�t�B�[���h3�c�J�b�g�C���q�[�v���
  GFL_HEAP_DeleteHeap( HEAPID_FLD3DCUTIN );

  return MAINSEQ_RESULT_NEXTSEQ;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static MAINSEQ_RESULT mainSeqFunc_end(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{ 
  // TCB
  GFL_TCB_Exit( fieldWork->fieldmapTCB );
  GFL_HEAP_FreeMemory( fieldWork->fieldmapTCBSysWork );
  // �^�X�N�}�l�[�W��
  FIELD_TASK_MAN_Delete( fieldWork->taskManager );

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
 * �t�B�[���h�}�b�v�@���@�ɃA�C�e���A���]�Ԃ��g�p�B
 * ���s�`�Ԃ̏ꍇ�͎��]�ԂɁB���]�ԏ��̏ꍇ�͕��s�`�ԂɁB
 * @param fieldWork FIELDMAP_WORK
 * @retval BOOL TRUE=�g�p���� FALSE=�g�p�s��
 */
//--------------------------------------------------------------
BOOL FIELDMAP_SetPlayerItemCycle( FIELDMAP_WORK *fieldWork )
{
  FLDMAP_BASESYS_TYPE type;
  
  type = FIELDMAP_GetBaseSystemType( fieldWork );
  
  if( type == FLDMAP_BASESYS_GRID )
  {
    PLAYER_MOVE_FORM form;
    FIELD_PLAYER_GRID *gjiki;
    
    form = FIELD_PLAYER_GetMoveForm( fieldWork->field_player );
    gjiki = FIELDMAP_GetPlayerGrid( fieldWork );
    
    if( form == PLAYER_MOVE_FORM_CYCLE )
    {
      FIELD_PLAYER_GRID_SetRequest( gjiki, FIELD_PLAYER_REQBIT_NORMAL );
      return( TRUE );
    }
    else if( form == PLAYER_MOVE_FORM_NORMAL )
    {
      PMSND_PlaySE( SEQ_SE_BICYCLE );
      FIELD_PLAYER_GRID_SetRequest( gjiki, FIELD_PLAYER_REQBIT_CYCLE );
      return( TRUE );
    }
  }
  else
  {
    PLAYER_MOVE_FORM form;
    FIELD_PLAYER_NOGRID *gjiki;
    
    form = FIELD_PLAYER_GetMoveForm( fieldWork->field_player );
    gjiki = FIELDMAP_GetPlayerNoGrid( fieldWork );
    
    
    if( form == PLAYER_MOVE_FORM_CYCLE )
    {
      FIELD_PLAYER_NOGRID_ChangeForm( gjiki, PLAYER_MOVE_FORM_NORMAL );
      return( TRUE );
    }
    else if( form == PLAYER_MOVE_FORM_NORMAL )
    {
      FIELD_PLAYER_NOGRID_ChangeForm( gjiki, PLAYER_MOVE_FORM_CYCLE );
      return( TRUE );
    }
  }
  
  return( FALSE );
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

//--------------------------------------------------------------
/**
 *	@brief  �m�[�O���b�h����@�}�b�p�[�̎擾
 */
//--------------------------------------------------------------
FLDNOGRID_MAPPER* FIELDMAP_GetFldNoGridMapper( FIELDMAP_WORK *fieldWork )
{
  return fieldWork->nogridMapper;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
int * FIELDMAP_GetFirstConnectID( FIELDMAP_WORK * fieldWork )
{
  return &fieldWork->firstConnectEventID;
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

//----------------------------------------------------------------------------
/**
 * FIELDMAP_WORK FIELD_ZONEFOGLIGHT�擾
 * @param	fieldWork	FIELDMAP_WORK
 * @retval FIELD_ZONEFOGLIGHT
 */
//-----------------------------------------------------------------------------
FIELD_ZONEFOGLIGHT * FIELDMAP_GetFieldZoneFog( FIELDMAP_WORK *fieldWork )
{
	return fieldWork->zonefog;
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
 * FIELDMAP_WORK MMDLSYS�擾
 * @param	fieldWork	FIELDMAP_WORK
 * @retval MMDLSYS*
 */
//--------------------------------------------------------------
MMDLSYS * FIELDMAP_GetMMdlSys( FIELDMAP_WORK *fieldWork )
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
 * @param
 * @retval
 */
//--------------------------------------------------------------
const DEPEND_FUNCTIONS * FIELDMAP_GetDependFunctions( const FIELDMAP_WORK * fieldWork )
{
  return fieldWork->func_tbl;
}

//--------------------------------------------------------------
/**
 * @brief ���݂̃}�b�v�w��ID���擾
 * @param fieldWork
 * @return  u16   map�w��ID
 */
//--------------------------------------------------------------
u16 FIELDMAP_GetZoneID( const FIELDMAP_WORK * fieldWork )
{
#if 0
  return fieldWork->map_id;
#else
  return fieldWork->location.zone_id;
#endif
}

#if 0
//--------------------------------------------------------------
/**
 * @param
 * @retval
 */
//--------------------------------------------------------------
const VecFx32 * FIELDMAP_GetNowPos( const FIELDMAP_WORK * fieldWork )
{
  return &fieldWork->now_pos;
}
#endif

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

//--------------------------------------------------------------
/**
 * @param
 * @retval
 */
//--------------------------------------------------------------
FIELD_ENCOUNT * FIELDMAP_GetEncount( FIELDMAP_WORK * fieldWork )
{
  return fieldWork->encount;
}

//--------------------------------------------------------------
/**
 * ���݂̃}�b�v�R���g���[���^�C�v���擾 
 * @param fieldWork FIELDMAP_WORK
 * @retval FLDMAP_CTRLTYPE
 */
//--------------------------------------------------------------
FLDMAP_CTRLTYPE FIELDMAP_GetMapControlType( FIELDMAP_WORK *fieldWork )
{
  GF_ASSERT( fieldWork );
  GF_ASSERT( fieldWork->func_tbl );
  return fieldWork->func_tbl->type;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �O���b�h�v���C���[���[�N�̎擾
 *
 *	@param	fieldWork ���[�N
 *
 *	@retval �O���b�h�v���C���[���[�N
 *	@retval NULL  �Ȃ��΂���
 */
//-----------------------------------------------------------------------------
FIELD_PLAYER_GRID* FIELDMAP_GetPlayerGrid( const FIELDMAP_WORK *fieldWork )
{
  GF_ASSERT( fieldWork );
  GF_ASSERT( fieldWork->func_tbl );

  if( fieldWork->func_tbl->type == FLDMAP_CTRLTYPE_GRID )
  {
    FIELDMAP_CTRL_GRID* p_gridwk = fieldWork->mapCtrlWork;
    return FIELDMAP_CTRL_GRID_GetFieldPlayerGrid( p_gridwk );
  }
  else if( fieldWork->func_tbl->type == FLDMAP_CTRLTYPE_HYBRID )
  {
    FIELDMAP_CTRL_HYBRID* p_hybridwk = fieldWork->mapCtrlWork;
    return FIELDMAP_CTRL_HYBRID_GetFieldPlayerGrid( p_hybridwk );
  }
  else
  {
    GF_ASSERT( fieldWork->func_tbl->type == FLDMAP_CTRLTYPE_GRID );
  }
  return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �m���O���b�h�v���C���[���[�N�̎擾
 *
 *	@param	fieldWork   ���[�N
 *
 *	@retval �m���O���b�h�v���C���[���[�N
 *	@retval NULL  �Ȃ��ꍇ
 */
//-----------------------------------------------------------------------------
FIELD_PLAYER_NOGRID* FIELDMAP_GetPlayerNoGrid( const FIELDMAP_WORK *fieldWork )
{
  GF_ASSERT( fieldWork );
  GF_ASSERT( fieldWork->func_tbl );

  if( fieldWork->func_tbl->type == FLDMAP_CTRLTYPE_NOGRID )
  {
    FIELDMAP_CTRL_NOGRID_WORK* p_nogridwk = fieldWork->mapCtrlWork;
    return FIELDMAP_CTRL_NOGRID_WORK_GetNogridPlayerWork( p_nogridwk );
  }
  else if( fieldWork->func_tbl->type == FLDMAP_CTRLTYPE_HYBRID )
  {
    FIELDMAP_CTRL_HYBRID* p_hybridwk = fieldWork->mapCtrlWork;
    return FIELDMAP_CTRL_HYBRID_GetFieldPlayerNoGrid( p_hybridwk );
  }
  else
  {
    GF_ASSERT( fieldWork->func_tbl->type == FLDMAP_CTRLTYPE_NOGRID );
  }
  return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���݂̃x�[�X�V�X�e���^�C�v���擾
 *
 *	@param	fieldWork   �t�B�[���h���[�N
 *
 *	@retval FLDMAP_BASESYS_GRID = 0,  ///<�x�[�X�V�X�e���@�O���b�h�ړ�
 *	@retval FLDMAP_BASESYS_RAIL,      ///<�x�[�X�V�X�e���@���[���ړ�
 */
//-----------------------------------------------------------------------------
FLDMAP_BASESYS_TYPE FIELDMAP_GetBaseSystemType( const FIELDMAP_WORK *fieldWork )
{
  static const u8 MAPCTRL_BASESYS_TYPE[] = 
  {
    FLDMAP_BASESYS_GRID,
    FLDMAP_BASESYS_RAIL,
    FLDMAP_BASESYS_MAX,   ///<HYBRID�́A���[�N����擾����
  };

  GF_ASSERT( fieldWork );
  GF_ASSERT( fieldWork->func_tbl );
  
  if( MAPCTRL_BASESYS_TYPE[ fieldWork->func_tbl->type ] == FLDMAP_BASESYS_MAX )
  {
    return FIELDMAP_CTRL_HYBRID_GetBaseSystemType( fieldWork->mapCtrlWork );
  }
  return MAPCTRL_BASESYS_TYPE[ fieldWork->func_tbl->type ];
}

//--------------------------------------------------------------
/**
 */
//--------------------------------------------------------------
FIELD_PLACE_NAME * FIELDMAP_GetPlaceNameSys( FIELDMAP_WORK * fieldWork )
{
  return fieldWork->placeNameSys;
}

//--------------------------------------------------------------
/**
 *	@brief	�t�B�[���h�}�b�v��œ��삷�鐧��^�X�N�V�X�e��	�擾
 */
//--------------------------------------------------------------
FLDMAPFUNC_SYS * FIELDMAP_GetFldmapFuncSys( FIELDMAP_WORK * fieldWork )
{
	return fieldWork->fldmapFuncSys;
}

//--------------------------------------------------------------
/**
 *  @brief	�g��3�c�I�u�W�F�N�g�R���g���[���擾
 * @param	fieldWork	FIELDMAP_WORK
 * @retval FLD_EXP_OBJ_CNT_PTR
 */
//--------------------------------------------------------------
FLD_EXP_OBJ_CNT_PTR FIELDMAP_GetExpObjCntPtr( FIELDMAP_WORK *fieldWork )
{
	return fieldWork->ExpObjCntPtr;
}

//--------------------------------------------------------------
/**
 *	@brief	TCB�V�X�e���@�擾
 */
//--------------------------------------------------------------
extern GFL_TCBSYS* FIELDMAP_GetFieldmapTCBSys( FIELDMAP_WORK * fieldWork )
{
  return fieldWork->fieldmapTCB;
}

//--------------------------------------------------------------
/**
 * @brief �t�B�[���h�^�X�N�}�l�[�W�� �擾
 */
//--------------------------------------------------------------
FIELD_TASK_MAN* FIELDMAP_GetTaskManager( FIELDMAP_WORK* fieldWork )
{
  return fieldWork->taskManager;
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�J�����̖ڕW���W�ƂȂ�|�C���^���Z�b�g
 * @param fieldWork FIELDMAP_WORK
 * @retval  nothing
 */
//--------------------------------------------------------------
void FIELDMAP_SetNowPosTarget( FIELDMAP_WORK *fieldWork, const VecFx32 *pos )
{
  fieldWork->target_now_pos_p = pos;
}

//--------------------------------------------------------------
/**
 * @brief �������\���p�E�B���h�E���擾
 * @param fieldWork FIELDMAP_WORK
 * @retval �������\���p�E�B���h�E
 */
//--------------------------------------------------------------
FLDMSGWIN * FIELDMAP_GetGoldMsgWin( FIELDMAP_WORK *fieldWork )
{
  return fieldWork->goldMsgWin;
}

//--------------------------------------------------------------
/**
 * @brief �������\���p�E�B���h�E���Z�b�g
 * @param fieldWork FIELDMAP_WORK
 * @param msgWin    FLDMSGWIN
 * @retval �������\���p�E�B���h�E
 */
//--------------------------------------------------------------
void FIELDMAP_SetGoldMsgWin( FIELDMAP_WORK *fieldWork, FLDMSGWIN *msgWin )
{
  fieldWork->goldMsgWin = msgWin;
}

//--------------------------------------------------------------
/**
 * @brief ��ĉ����擾
 * @param fieldWork FIELDMAP_WORK
 * @retval ��ĉ�
 */
//--------------------------------------------------------------
SODATEYA* FIELDMAP_GetSodateya( FIELDMAP_WORK* fieldWork )
{
  return fieldWork->sodateya;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK AREADATA*�擾
 * @param fieldWork FIELDMAP_WORK
 * @retval AREADATA*
 */
//--------------------------------------------------------------
AREADATA* FIELDMAP_GetAreaData( FIELDMAP_WORK *fieldWork )
{
	return fieldWork->areadata;
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
	//���C����ʂ̔w�i�F
  GFL_BG_LoadPalette( GFL_BG_FRAME0_M, (void*)fldmapdata_bgColorTable, 16*2, 0 );
	//�T�u��ʂ̔w�i�F
  GFL_BG_LoadPalette( GFL_BG_FRAME0_S, (void*)fldmapdata_bgColorTable, 16*2, 0 );
#ifndef CROSSFADE_MODE
	//�a�f���[�h�ݒ�
	GFL_BG_SetBGMode( &fldmapdata_bgsysHeader );
	//�a�f�R���g���[���ݒ�
	G2S_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2, GX_BLEND_PLANEMASK_BG3, 16, 8 );

	GFL_BG_SetBGControl3D( FIELD_3D_FRAME_PRI );
#endif
	//�f�B�X�v���C�ʂ̑I��
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_MAIN );
	GFL_DISP_SetDispOn();
  
  //�g�p����E�B���h�E
  GX_SetVisibleWnd( GX_WNDMASK_NONE );
}

void FIELDMAP_InitBG( FIELDMAP_WORK* fieldWork );
void FIELDMAP_InitBG( FIELDMAP_WORK* fieldWork )
{
	//�a�f���[�h�ݒ�
	GFL_BG_SetBGMode( &fldmapdata_bgsysHeader );
	//�a�f�R���g���[���ݒ�
	G2S_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2, GX_BLEND_PLANEMASK_BG3, 16, 8 );
	
	GFL_BG_SetBGControl3D( FIELD_3D_FRAME_PRI );

  // ��b�E�C���h�E���\�[�X�Z�b�g�A�b�v
	FLDMSGBG_SetupResource( fieldWork->fldMsgBG );
  // �n���\���V�X�e���쐬
  fieldWork->placeNameSys = FIELD_PLACE_NAME_Create( fieldWork->heapID, fieldWork->fldMsgBG );

  //�t�B�[���h�f�o�b�O������
  fieldWork->debugWork = FIELD_DEBUG_Init( fieldWork, fieldWork->heapID );

#if USE_DEBUGWIN_SYSTEM
	DEBUGWIN_InitProc( GFL_BG_FRAME1_M , FLDMSGBG_GetFontHandle(fieldWork->fldMsgBG) );
	DEBUGWIN_ChangeLetterColor( 31,31,31 );
	FIELD_FUNC_RANDOM_GENERATE_InitDebug
		( fieldWork->heapID, GAMEDATA_GetMyWFBCCoreData( fieldWork->gamedata ) );
#endif  //USE_DEBUGWIN_SYSTEM
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
  G3X_SetFog(FALSE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x0400, 0x00e00 );
  G3X_SetFogColor(FIELD_DEFAULT_FOG_COLOR, 0);
  G3X_SetFogTable(fldmapdata_fogColorTable);
	
	// �N���A�J���[�̐ݒ�
	//color,alpha,depth,polygonID,fog
	G3X_SetClearColor(GX_RGB(0,0,0),31,0x7fff,0,FALSE);
	
	G3X_SetEdgeColorTable( fldmapdata_edgeColorTable ); 
	G3X_EdgeMarking( FALSE );
	
	// �r���[�|�[�g�̐ݒ�
	G3_ViewPort(0, 0, 255, 191);
	GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_MANUAL, GX_BUFFERMODE_Z );
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
  {
    GFL_BBD_SYS * bbdsys = GFL_BBDACT_GetBBDSystem(fieldWork->bbdActSys);
    GFL_BBD_SetOrigin(bbdsys, GFL_BBD_ORIGIN_BOTTOM);
  }
  
  //G3dObj�쐬
  fieldWork->fieldG3dObjCtrl = FLD_G3DOBJ_CTRL_Create(
      fieldWork->heapID, FIELD_G3DOBJ_RES_MAX, FIELD_G3DOBJ_OBJ_MAX );
  
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
  
  // NOGRID���쐧��
  FLDNOGRID_MAPPER_Main( fieldWork->nogridMapper );

  // scenearea
  FLD_SCENEAREA_Update( fieldWork->sceneArea, &fieldWork->now_pos );
	
	FIELD_WEATHER_Main( fieldWork->weather_sys, fieldWork->heapID );
	FIELD_FOG_Main( fieldWork->fog );
	FIELD_LIGHT_Main( fieldWork->light, GFL_RTC_GetTimeBySecond() );
}

//--------------------------------------------------------------
/**
 * G3D �`��
 * @param fieldWork FIELMAP_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
//�v���W�F�N�V�����}�g���N�X�𑀍삷��ۂ̂y�I�t�Z�b�g
#define	PRO_MAT_Z_OFS	(310)
static void fldmap_G3D_Draw( FIELDMAP_WORK * fieldWork )
{
  GFL_G3D_DRAW_Start();
  GFL_G3D_DRAW_SetLookAt();	//�J�����O���[�o���X�e�[�g�ݒ�

#ifdef PM_DEBUG  
  {
    s32 check;
    //���C���Y�I�[�o�[�`�F�b�N
    check = G3X_IsLineBufferUnderflow();
//    GF_ASSERT_MSG( check == 0, "LINES OVER" );
    if (check) OS_Printf("LINES OVER\n");
    //�|���S�����X�gRAM, ���_RAM�̃I�[�o�[�t���[�`�F�b�N
    check = G3X_IsListRamOverflow();
//    GF_ASSERT_MSG( check == 0, "POLY_VTX_OVER" );
    if (check) OS_Printf("POLY_VTX_OVER\n");
  }
#endif  //PM_DEBUG

  {
    static const DRAW3DMODE_FUNC func[] = {
      Draw3DNormalMode,
      Draw3DCutinMode,
      Draw3DScrnTexMode
    };

    func[ fieldWork->Draw3DMode ](fieldWork);
  }

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
  FLD_G3DOBJ_CTRL_Delete( fieldWork->fieldG3dObjCtrl );
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
  
  if( fieldWork->fldMMdlSys != NULL ){
    MMDLSYS_VBlankProc( fieldWork->fldMMdlSys );
  }
  
  if( fieldWork->fieldG3dObjCtrl != NULL ){
    FLD_G3DOBJ_CTRL_Trans( fieldWork->fieldG3dObjCtrl );
  }

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

//--------------------------------------------------------------
/**
 * @brief �G�b�W�}�[�L���O�ݒ蔽�f
 * @param areadata
 */
//--------------------------------------------------------------
static void FIELD_EDGEMARK_Setup(const AREADATA * areadata)
{
  static GXRgb edgeTable[8];
  int edgemark_type = AREADATA_GetEdgeMarkingType(areadata);

  if (edgemark_type == 0xff)
  {
    G3X_EdgeMarking(FALSE);
    return;
  }
  G3X_EdgeMarking(TRUE);
  GFL_ARC_LoadData((void *)edgeTable, ARCID_FIELD_EDGEMARK, edgemark_type);
  G3X_SetEdgeColorTable( edgeTable );
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
static void fldmapMain_MMDL_Init( FIELDMAP_WORK *fieldWork )
{
	GAMEDATA *gdata = GAMESYSTEM_GetGameData( fieldWork->gsys );
	MMDLSYS *fmmdlsys = GAMEDATA_GetMMdlSys( gdata );
		
	fieldWork->fldMMdlSys = fmmdlsys;
	MMDLSYS_SetFieldMapWork( fmmdlsys, fieldWork );

	MMDLSYS_SetupProc( fmmdlsys,	//���샂�f���V�X�e���@�Z�b�g�A�b�v
		fieldWork->heapID, fieldWork->g3Dmapper, fieldWork->nogridMapper );
	
	MMDL_BLACTCONT_Setup(		//���샂�f���r���{�[�h�@�Z�b�g�A�b�v
		fieldWork->fldMMdlSys, fieldWork->bbdActSys, 32 );

	{ //�r���{�[�h���\�[�X�o�^
	  MMDL_LIST mlist;
	  int list_area_id = 0; //��
	  fldmap_MMDL_InitList( &mlist, list_area_id, fieldWork->heapID );
	  MMDL_BLACTCONT_AddResourceTex(
	    fieldWork->fldMMdlSys, mlist.id_list, mlist.count );
	}
	
  MMDL_G3DOBJCONT_Setup( //���샂�f���I�u�W�F�N�g�@�Z�b�g�A�b�v
      fieldWork->fldMMdlSys, fieldWork->fieldG3dObjCtrl );

	//���샂�f���`��@�Z�b�g�A�b�v
  {
    const u16 *pAngle = FIELD_CAMERA_GetAngleYawAddress( fieldWork->camera_control );
	  MMDLSYS_SetupDrawProc( fieldWork->fldMMdlSys, pAngle );
	}

	//���샂�f���@���A
	MMDLSYS_Pop( fieldWork->fldMMdlSys );

/*
  if( fieldWork->func_tbl->type == FLDMAP_CTRLTYPE_GRID ){
    MMDLSYS_SetJoinShadow( fmmdlsys, TRUE );
  }else{
    MMDLSYS_SetJoinShadow( fmmdlsys, FALSE );
  }
//*/
  MMDLSYS_SetJoinShadow( fmmdlsys, TRUE );
  
#ifdef DEBUG_ONLY_FOR_kagaya
  /*
  #define OBJID_TEST_TRAINER (128)
  #include "../../../resource/fldmapdata/script/trainer_def.h"
  
  if( fieldWork->location.zone_id == ZONE_ID_T01 )
  {
    MMDLSYS *mmdlsys = fieldWork->fldMMdlSys;
    
    if( MMDLSYS_SearchOBJID(mmdlsys,OBJID_TEST_TRAINER) == NULL )
    {
      MMDL_HEADER head =
      {
        OBJID_TEST_TRAINER,
        BOY1,
        MV_DOWN,
        EV_TYPE_TRAINER, 0, SCRID_R01_TRAINER_02,
        DIR_DOWN,
        4, 0, 0,
        0, 0,
        785,748,0,
      };
      
      MMDLSYS_AddMMdl( mmdlsys, &head, ZONE_ID_T01 );
    }
  }
  */
#endif
}

//--------------------------------------------------------------
/**
 * ���샂�f���@�I��
 * @param fieldWork FIELDMAP_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static void fldmapMain_MMDL_Finish( FIELDMAP_WORK *fieldWork )
{
	MMDLSYS_Push( fieldWork->fldMMdlSys );
	MMDLSYS_DeleteProc( fieldWork->fldMMdlSys );
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
static void fldmap_MMDL_InitList(
		MMDL_LIST *mlist, int list_id, HEAPID heapID )
{
	int i = 0;
	u16 *pList;
	pList = GFL_ARC_LoadDataAlloc( ARCID_MMDL_LIST, list_id, heapID );
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
  VecFx32 loc_pos;

  LOCATION_Get3DPos( lc, &loc_pos ); 
	
	if( pos->x != loc_pos.x || pos->z != loc_pos.z ){
    LOCATION_Set3DPos( lc, pos );
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
  VecFx32 pos;
	MAP_MATRIX *mat = GAMEDATA_GetMapMatrix(fieldWork->gamedata);

  LOCATION_Get3DPos( lc, &pos );
	
	if( MAP_MATRIX_CheckVectorPosRange(mat,pos.x,pos.z) == TRUE ){
		u32 zone_id =
			MAP_MATRIX_GetVectorPosZoneID( mat, pos.x, pos.z );
		
		//���� ID�������ꍇ�͍X�V���Ȃ�
		if( zone_id != MAP_MATRIX_ZONE_ID_NON ){
			if( zone_id != (u32)lc->zone_id ){
				return( TRUE );
			}
		}
	}
	
	return( FALSE );
}

#ifdef DEBUG_FIELDMAP_ZONE_CHANGE_SYNC
static u64 checks[10];
static u32 check_count;
static void init_checks(void){check_count = 0;}
static void SET_CHECK(void){
  if (check_count >= NELEMS(checks)) return;
  checks[check_count] = OS_GetTick();
  check_count ++;
}
static void put_checks(OSTick start)
{
  int i;
  u64 value;
  for (i = 1; i < check_count; i++) {
    OS_Printf("%6ld:", checks[i] );
    value = checks[i] - checks[i-1];
    OS_Printf("%6ld\n", value);
  }
}

#else

#define SET_CHECK()   /* DO NOTHING */

#endif
 
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
  VecFx32 lc_pos;
	GAMEDATA *gdata = GAMESYSTEM_GetGameData( fieldWork->gsys );
	EVENTDATA_SYSTEM *evdata = GAMEDATA_GetEventData( gdata );
	MMDLSYS *fmmdlsys = fieldWork->fldMMdlSys;
	
	MAP_MATRIX *mat = GAMEDATA_GetMapMatrix(fieldWork->gamedata);
	u32 new_zone_id;
	u32 old_zone_id = lc->zone_id;

#ifdef DEBUG_FIELDMAP_ZONE_CHANGE_SYNC
  OSTick debug_fieldmap_start_tick = OS_GetTick(); 
  OSTick debug_fieldmap_end_tick;
  init_checks();
  SET_CHECK();
#endif

  //�G�t�F�N�g�G���J�E���g�j��
  EFFECT_ENC_EffectDelete( fieldWork->encount );

  LOCATION_Get3DPos( lc, &lc_pos );

  new_zone_id = MAP_MATRIX_GetVectorPosZoneID(
			mat, lc_pos.x, lc_pos.z );
	
	GF_ASSERT( new_zone_id != MAP_MATRIX_ZONE_ID_NON );
	
	//���]�[���z�u���샂�f���폜
	MMDLSYS_DeleteZoneUpdateMMdl( fmmdlsys );

  SET_CHECK();
	
	//���̃C�x���g�f�[�^�����[�h
	EVENTDATA_SYS_Load( evdata, new_zone_id, GAMEDATA_GetSeasonID(gdata) );

  //�����ă]�[�����ύX�����ꍇ�̃t���O������
  FIELD_FLAGCONT_INIT_WalkStepOver( gdata, fieldWork );
	
  SET_CHECK();
	//�V�K�]�[���ɔz�u���铮�샂�f���Z�b�g
	zoneChange_SetMMdl( gdata, fmmdlsys, evdata, new_zone_id );
  zoneChange_SetMMdlZoneWFBC( gdata, fieldWork, new_zone_id );
	
  SET_CHECK();
	//BGM�؂�ւ�
	zoneChange_SetBGM( gdata, new_zone_id );

  SET_CHECK();
	// ZONE�t�H�O���C�g�ݒ�
	zoneChange_SetZoneFogLight( fieldWork, new_zone_id );
	
	//�V�󃊃N�G�X�g
	zoneChange_SetWeather( fieldWork, new_zone_id );

  SET_CHECK();
  //�J�����G���A�̐ݒ�
  zoneChange_SetCameraArea( fieldWork, new_zone_id );
	
  SET_CHECK();
	//PLAYER_WORK�X�V
	zoneChange_UpdatePlayerWork( gdata, new_zone_id );

	// �n���\���V�X�e����, �]�[���̐؂�ւ���ʒB
  if(fieldWork->placeNameSys){ FIELD_PLACE_NAME_Display( fieldWork->placeNameSys, new_zone_id ); }

  SET_CHECK();
	//�]�[��ID�X�V
	lc->zone_id = new_zone_id;
	
  //�}�b�v�����t���O�Z�b�g
  ARRIVEDATA_SetArriveFlag( gdata, new_zone_id );

  //����X�N���v�g�Ăяo���F�]�[���؂�ւ�
  SCRIPT_CallZoneChangeScript( fieldWork->gsys, HEAPID_PROC );

  SET_CHECK();
#ifdef DEBUG_FIELDMAP_ZONE_CHANGE_SYNC
  debug_fieldmap_end_tick = OS_GetTick();
  debug_fieldmap_end_tick -= debug_fieldmap_start_tick;

  debug_fieldmap_end_tick = OS_TicksToMicroSeconds( debug_fieldmap_end_tick );

  if( debug_fieldmap_end_tick > 10000 )
  {
    OS_TPrintf( "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" );
    OS_TPrintf( "!!!!!!zone_change TickOver  [%d] micro second !!!!!!\n", debug_fieldmap_end_tick );
    OS_TPrintf( "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" );
    put_checks(debug_fieldmap_start_tick);
  }
#endif

	KAGAYA_Printf( "�]�[���X�V���� %d -> %d\n", lc->zone_id, new_zone_id );
}

//--------------------------------------------------------------
/**
 * �]�[���؂�ւ����̏����@�V�K�]�[�����샂�f���Z�b�g
 * @param	fmmdlsys	MMDLSYS
 * @param	evdata	EVENTDATA_SYSTEM
 * @param	zone_id	���̃]�[��ID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void zoneChange_SetMMdl( GAMEDATA *gdata,
		MMDLSYS *fmmdlsys, EVENTDATA_SYSTEM *evdata, u32 zone_id )
{
	u16 count = EVENTDATA_GetNpcCount( evdata );
	
	if( count ){
    EVENTWORK *evwork =  GAMEDATA_GetEventWork( gdata );
		const MMDL_HEADER *header = EVENTDATA_GetNpcData( evdata );
		MMDLSYS_SetMMdl( fmmdlsys, header, zone_id, count, evwork );
	}
}

// @TODO WFBC�p�̐l�z�u
static void zoneChange_SetMMdlZoneWFBC( GAMEDATA *gdata,
		FIELDMAP_WORK* fieldWork, u32 zone_id )
{
  if( zone_id != ZONE_ID_BCWFTEST )
  {
    return ;
  }
  
  {
    FIELD_WFBC* p_wfbc;
    MMDL_HEADER* p_header;
    u32 count;
    EVENTWORK *evwork =  GAMEDATA_GetEventWork( gdata );
    MMDLSYS *fmmdlsys = GAMEDATA_GetMMdlSys( gdata );

    p_wfbc    = FLDMAPPER_GetWfbcWork( fieldWork->g3Dmapper );
    p_header  = FIELD_WFBC_MMDLHeaderCreateHeapLo( p_wfbc, fieldWork->heapID );
    count     = FIELD_WFBC_GetPeopleNum( p_wfbc );
    
    if( p_header && (count > 0) )
    {
      TOMOYA_Printf( "WFBC MMDL SetUp\n" );
		  MMDLSYS_SetMMdl( fmmdlsys, p_header, zone_id, count, evwork );


      GFL_HEAP_FreeMemory( p_header );
    }
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
  FIELD_SOUND* fsnd = GAMEDATA_GetFieldSound( gdata );
  PLAYER_WORK *player = GAMEDATA_GetPlayerWork( gdata, 0 );
  PLAYER_MOVE_FORM form = PLAYERWORK_GetMoveForm( player );
  FIELD_SOUND_ChangePlayZoneBGM( fsnd, gdata, form, zone_id );
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
	u32 w_no = ZONEDATA_GetWeatherID( zone_id );
	FIELD_WEATHER *we = FIELDMAP_GetFieldWeather( fieldWork );
	
	if( w_no != WEATHER_NO_NUM && w_no != FIELD_WEATHER_GetWeatherNo(we) ){
		FIELD_WEATHER_Change( we, w_no );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�]�[���؂�ւ����̏���	�]�[���p�㏑��FOG�A���C�g���
 *
 *	@param	fieldWork	
 *	@param	zone_id 
 */
//-----------------------------------------------------------------------------
static void zoneChange_SetZoneFogLight( FIELDMAP_WORK *fieldWork, u32 zone_id )
{
	FIELD_ZONEFOGLIGHT* p_zonefog = FIELDMAP_GetFieldZoneFog( fieldWork );

	FIELD_ZONEFOGLIGHT_Clear( p_zonefog );
	
	FIELD_ZONEFOGLIGHT_Load( fieldWork->zonefog, ZONEDATA_GetFog(zone_id), ZONEDATA_GetLight(zone_id), fieldWork->heapID );
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

//----------------------------------------------------------------------------
/**
 *	@brief  �]�[���؂�ւ����̏����@�J�����G���A�X�V
 *
 *	@param	fieldWork   FIELDMAP_WORK
 *	@param	zone_id     ���̃]�[��
 */
//-----------------------------------------------------------------------------
static void zoneChange_SetCameraArea( FIELDMAP_WORK* fieldWork, u32 zone_id )
{
  setupCameraArea( fieldWork, zone_id, fieldWork->heapID );
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

//----------------------------------------------------------------------------
/**
 *	@brief  �J�����G���A�̐ݒ�
 *
 *	@param	fieldWork
 *	@param	heapID 
 */
//-----------------------------------------------------------------------------
static void setupCameraArea( FIELDMAP_WORK *fieldWork, u32 zone_id, HEAPID heapID )
{
  u32 cameraarea = ZONEDATA_GetCameraAreaID(zone_id);

  if ( cameraarea != ZONEDATA_NO_CAMERA_AREA_ID )
  {
    FIELD_CAMERA_LoadCameraArea( fieldWork->camera_control, cameraarea, heapID );
  }
  else
  {
    FIELD_CAMERA_ClearCameraArea( fieldWork->camera_control );
  }
}


//==================================================================
/**
 * �t�B�[���h�Ŏ��s����Ă���ʐMNo���擾����
 *
 * @param   game_comm		    
 *
 * @retval  GAME_COMM_NO		���s���Ă���ʐM�Q�[���ԍ�
 */
//==================================================================
GAME_COMM_NO FIELDCOMM_CheckCommNo(GAME_COMM_SYS_PTR game_comm)
{
  GAME_COMM_NO comm_no = GameCommSys_BootCheck(game_comm);
  switch(comm_no){
  case GAME_COMM_NO_NULL:                  //�����N�����Ă��Ȃ����
  case GAME_COMM_NO_FIELD_BEACON_SEARCH:   //�t�B�[���h��Ńr�[�R���T�[�`
  case GAME_COMM_NO_INVASION:              //�N��
    return comm_no;
  }
  GF_ASSERT_MSG(0, "comm_no = %d\n", comm_no); //�t�B�[���h�ŏ�L�ʐM�ȊO�͂��肦�Ȃ�
  return GAME_COMM_NO_INVASION;   //�ʐM���Ƃ����Ӗ��ŐN��No��Ԃ��悤�ɂ���
}

//==================================================================
/**
 * �t�B�[���h�ʐM�I������
 *
 * @param   game_comm		
 *
 * @retval  FIELDCOMM_EXIT		�ʐM�I�����
 *
 * FIELDCOMM_EXIT_END     �F�ʐM������ɏI��
 * FIELDCOMM_EXIT_CONTINUE�F�ʐM�I���҂��ł��B��̃V�[�P���X�ւ͐i�܂���END�ɂȂ�܂ő҂��Ă�������
 * FIELDCOMM_EXIT_ERROR   �F�G���[���������Ă��܂��B�G���[��ʂ�\�����Ă��������B
 *                          �G���[��ʕ\����͂��̊֐����ĂԕK�v�͂���܂���
 */
//==================================================================
FIELDCOMM_EXIT FIELDCOMM_ExitWait(GAME_COMM_SYS_PTR game_comm)
{
  if(NetErr_App_CheckError() == TRUE){
    return FIELDCOMM_EXIT_ERROR;
  }
  if(GameCommSys_BootCheck(game_comm) == GAME_COMM_NO_NULL){
    return FIELDCOMM_EXIT_END;
  }
  else if(GameCommSys_CheckSystemWaiting(game_comm) == FALSE){
    GameCommSys_ExitReq(game_comm);
  }
  return FIELDCOMM_EXIT_CONTINUE;
}

//======================================================================
//	data
//======================================================================
//--------------------------------------------------------------
/// �f�B�X�v���C���f�[�^
//--------------------------------------------------------------
static const GFL_DISP_VRAM fldmapdata_dispVram =
{
	GX_VRAM_BG_128_D, //���C��2D�G���W����BG�Ɋ��蓖�� 
	GX_VRAM_BGEXTPLTT_NONE, //���C��2D�G���W����BG�g���p���b�g�Ɋ��蓖��
	GX_VRAM_SUB_BG_32_H, //�T�u2D�G���W����BG�Ɋ��蓖��
	GX_VRAM_SUB_BGEXTPLTT_NONE, //�T�u2D�G���W����BG�g���p���b�g�Ɋ��蓖��
	GX_VRAM_OBJ_64_E, //���C��2D�G���W����OBJ�Ɋ��蓖��
	GX_VRAM_OBJEXTPLTT_NONE, //���C��2D�G���W����OBJ�g���p���b�g�ɂɊ��蓖��
	GX_VRAM_SUB_OBJ_16_I, //�T�u2D�G���W����OBJ�Ɋ��蓖��
	GX_VRAM_SUB_OBJEXTPLTT_NONE, //�T�u2D�G���W����OBJ�g���p���b�g�ɂɊ��蓖��
	GX_VRAM_TEX_012_ABC, //�e�N�X�`���C���[�W�X���b�g�Ɋ��蓖��
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

//--�t�B�[���h�p�M�~�b�N�e���|�������[�N����֐��Q--//
//==================================================================
/**
 * �M�~�b�N�e���|�������[�N�������֐�
 *
 * @param   tmpWork     �e���|�������[�N�|�C���^
 */
//==================================================================
static void InitGmkTmpWork(GMK_TMP_WORK *tmpWork)
{
  tmpWork->AssignID = GMK_TMP_NO_ASSIGN_ID;
  tmpWork->Work = NULL;
}

//==================================================================
/**
 * �M�~�b�N�e���|�������[�N�������A���b�N�֐�
 *
 * @note  �g�p�̍ۂ�gmk_tmp_wk.h���Q�Ƃ̂���
 *
 * @param   fieldWork   �t�B�[���h���[�N�|�C���^
 * @param   inAssinID   �A�T�C���h�c
 * @param   inHeapID    �q�[�v�h�c
 * @param   inSize      �m�ۃ�����
 *
 * @return �A���P�[�V�����������[�N�|�C���^
 */
//==================================================================
void *GMK_TMP_WK_AllocWork
      (FIELDMAP_WORK *fieldWork, const u32 inAssignID, const HEAPID inHeapID, const u32 inSize)
{
  GMK_TMP_WORK *tmpWork;
  tmpWork = &fieldWork->GmkTmpWork;
  if (inAssignID == GMK_TMP_NO_ASSIGN_ID){
    GF_ASSERT(0);
    return NULL;
  }
  if (tmpWork->AssignID != GMK_TMP_NO_ASSIGN_ID){
    GF_ASSERT_MSG(0,"Alrady Assigned %d\n",tmpWork->AssignID);
    return NULL;
  }
  
  tmpWork->AssignID = inAssignID;
  tmpWork->Work = GFL_HEAP_AllocClearMemory( inHeapID, inSize );
  return tmpWork->Work;
}

//==================================================================
/**
 * �M�~�b�N�e���|�������[�N����������֐�
 *
 * @todo  �g�p�̍ۂ�gmk_tmp_wk.h���Q�Ƃ̂���
 *
 * @param   fieldWork   �t�B�[���h���[�N�|�C���^
 * @param   inAssinID   �A�T�C���h�c
 * @param   inHeapID    �q�[�v�h�c
 * @param   inSize      �m�ۃ�����
 */
//==================================================================
void GMK_TMP_WK_FreeWork
      (FIELDMAP_WORK *fieldWork, const u32 inAssignID)
{
  GMK_TMP_WORK *tmpWork;
  tmpWork = &fieldWork->GmkTmpWork;
  if (inAssignID == GMK_TMP_NO_ASSIGN_ID){
    GF_ASSERT(0);
    return;
  }
  if (tmpWork->AssignID != inAssignID){
    GF_ASSERT_MSG(0,"AssignID Diff %d_%d\n",tmpWork->AssignID, inAssignID);
  }

  GFL_HEAP_FreeMemory( tmpWork->Work );
  tmpWork->AssignID = GMK_TMP_NO_ASSIGN_ID;
}


//==================================================================
/**
 * �M�~�b�N�e���|�������[�N�擾�֐�
 *
 * @todo  �g�p�̍ۂ�gmk_tmp_wk.h���Q�Ƃ̂���
 *
 * @param   fieldWork   �t�B�[���h���[�N�|�C���^
 * @param   inAssinID   �A�T�C���h�c
 *
 * @return void*    �������|�C���^
 */
//==================================================================
void *GMK_TMP_WK_GetWork(FIELDMAP_WORK *fieldWork, const u32 inAssignID)
{
  GMK_TMP_WORK *tmpWork;
  tmpWork = &fieldWork->GmkTmpWork;
  if (inAssignID == GMK_TMP_NO_ASSIGN_ID){
    GF_ASSERT(0);
    return NULL;
  }
  if (inAssignID != tmpWork->AssignID){
    GF_ASSERT(0);
    return NULL;
  }
  return tmpWork->Work;
}

//==================================================================
/**
 * �ʏ�`��
 *
 * @param   fieldWork   �t�B�[���h���[�N�|�C���^
 *
 * @return none
 */
//==================================================================
static void Draw3DNormalMode( FIELDMAP_WORK * fieldWork )
{
  FIELD_FOG_Reflect( fieldWork->fog );
	FIELD_LIGHT_Reflect( fieldWork->light );

	GFL_G3D_CAMERA_Switching( fieldWork->g3Dcamera );
	GFL_G3D_LIGHT_Switching( fieldWork->g3Dlightset );
  
  if(fieldWork->fldMsgBG ){ FLDMSGBG_PrintG3D( fieldWork->fldMsgBG ); }

	FLDMAPPER_Draw( fieldWork->g3Dmapper, fieldWork->g3Dcamera );
	FIELDSKILL_MAPEFF_Draw( fieldWork->fieldskill_mapeff );


#ifdef PM_DEBUG
  // �f�o�b�N�J�����\��
  // (WIPE�p)
  FIELD_CAMERA_DEBUG_Draw( fieldWork->camera_control );
#endif

  {
	  MtxFx44 org_pm,pm;
		const MtxFx44 *m;
		m = NNS_G3dGlbGetProjectionMtx();
/**		
		OS_Printf("%x,%x,%x,%x\n%x,%x,%x,%x\n%x,%x,%x,%x\n%x,%x,%x,%x\n",
				m->_00,m->_01,m->_02,m->_03,
				m->_10,m->_11,m->_12,m->_13,
				m->_20,m->_21,m->_22,m->_23,
				m->_30,m->_31,m->_32,m->_33);
//*/				
    
		org_pm = *m;
		pm = org_pm;
		pm._32 += FX_Mul( pm._22, PRO_MAT_Z_OFS );
		NNS_G3dGlbSetProjectionMtx(&pm);
		NNS_G3dGlbFlush();		  //�@�W�I���g���R�}���h��]��
    NNS_G3dGeFlushBuffer(); // �]���܂�

    FLDEFF_CTRL_Draw( fieldWork->fldeff_ctrl );
    
    FLD_G3DOBJ_CTRL_Draw( fieldWork->fieldG3dObjCtrl );
    
    GFL_BBDACT_Draw(
        fieldWork->bbdActSys, fieldWork->g3Dcamera, fieldWork->g3Dlightset );

		NNS_G3dGlbSetProjectionMtx(&org_pm);
		NNS_G3dGlbFlush();		//�@�W�I���g���R�}���h��]��
  }

	// �t�B�[���h�}�b�v�p����^�X�N�V�X�e��
	FLDMAPFUNC_Sys_Draw3D( fieldWork->fldmapFuncSys );

	
  FIELD_WEATHER_3DWrite( fieldWork->weather_sys );	// �V�C�`�揈��

  //�t�B�[���h�g��3�c�n�a�i�`��
  FLD_EXP_OBJ_Draw( fieldWork->ExpObjCntPtr );
#ifdef PM_DEBUG
  if (GFL_UI_KEY_GetCont() & PAD_BUTTON_DEBUG){
    if (/*GFL_UI_KEY_GetTrg() & PAD_BUTTON_L*/0){
      FLD3D_CI_CallCutIn(fieldWork->gsys, fieldWork->Fld3dCiPtr, 0);
    }else if (GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT){
      FLD3D_CI_CallPokeCutIn(fieldWork->gsys, fieldWork->Fld3dCiPtr);
    }else if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_R){
      FLD3D_CI_FlySkyCameraDebug(
          fieldWork->gsys, fieldWork->Fld3dCiPtr, fieldWork->camera_control,
          FIELDMAP_GetFieldPlayer(fieldWork),
          FIELDMAP_GetFldNoGridMapper( fieldWork ) );
    }else if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_B){
      static const GFL_BG_SYS_HEADER sc_bg_sys_header = 
      {
				GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_3D
      };
      GFL_BG_SetBGMode( &sc_bg_sys_header );
      GX_SetBankForBG(GX_VRAM_BG_128_D);
    }
  }
#endif  //PM_DEBUG  

  FLD_PRTCL_Main();
  FLD3D_CI_Draw( fieldWork->Fld3dCiPtr );


}

//==================================================================
/**
 * �t�B�[���h3�c�J�b�g�C�����`��
 *
 * @param   fieldWork   �t�B�[���h���[�N�|�C���^
 *
 * @return none
 */
//==================================================================
static void Draw3DCutinMode(FIELDMAP_WORK * fieldWork)
{
  G3X_SetClearColor(GX_RGB(0,0,0),0,0x7fff,0,FALSE);

  FLD_PRTCL_Main();
  FLD3D_CI_Draw( fieldWork->Fld3dCiPtr );
}

//==================================================================
/**
 * �t�B�[���h�X�N���[���L���v�`���`��
 *
 * @param   fieldWork   �t�B�[���h���[�N�|�C���^
 *
 * @return none
 */
extern void ENCEFF_Draw3D(void);
//==================================================================
static void Draw3DScrnTexMode(FIELDMAP_WORK * fieldWork)
{
  G3X_SetClearColor(GX_RGB(31,31,31),31,0x7fff,0,FALSE);

	ENCEFF_Draw3D();
}

//==================================================================
/**
 * 3D�`�惂�[�h�擾
 *
 * @param   fieldWork   �t�B�[���h���[�N�|�C���^
 *
 * @return DRAW3DMODE   �`�惂�[�h
 */
//==================================================================
DRAW3DMODE FIELDMAP_GetDraw3DMode(FIELDMAP_WORK *fieldWork)
{
  return fieldWork->Draw3DMode;
}

//==================================================================
/**
 * 3D�`�惂�[�h�Z�b�g
 *
 * @param   fieldWork   �t�B�[���h���[�N�|�C���^
 * @param   DRAW3DMODE  �`�惂�[�h
 *
 * @return  none
 */
//==================================================================
void FIELDMAP_SetDraw3DMode(FIELDMAP_WORK *fieldWork, DRAW3DMODE mode)
{
  fieldWork->Draw3DMode = mode;
}

//==================================================================
/**
 * �J�b�g�C���Ǘ��|�C���^�擾
 *
 * @param   fieldWork   �t�B�[���h���[�N�|�C���^
 *
 * @return  FLD3D_CI_PTR    �J�b�g�C���Ǘ��|�C���^
 */
//==================================================================
FLD3D_CI_PTR FIELDMAP_GetFld3dCiPtr(FIELDMAP_WORK *fieldWork)
{
  return fieldWork->Fld3dCiPtr;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t�B�[���h�Z�@�}�b�v���ʃ��[�N�擾
 *
 *	@param	fieldWork   ���[�N
 */
//-----------------------------------------------------------------------------
FIELDSKILL_MAPEFF * FIELDMAP_GetFieldSkillMapEffect( FIELDMAP_WORK * fieldWork )
{
  return fieldWork->fieldskill_mapeff;
}


