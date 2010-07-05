//======================================================================
/**
 * @file	fieldmap.c
 * @brief	�t�B�[���h�}�b�v����
 * @author
 *
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"					//GFL_HEAPID_APP
#include "system/rtc_tool.h"					//
#include "arc_def.h"

#include "net/network_define.h"
#include "system/g3d_tool.h"

#include "print/gf_font.h"

#include "field/field_const.h"
#include "fieldmap.h"
#include "field_bg_def.h"
#include "field_common.h"
#include "field_flagcontrol.h" //FIELD_FLAGCONT_INIT�`

#include "field_player.h"
#include "field_camera.h"
#include "field_data.h"
#include "field/field_msgbg.h"
#include "field_encount.h"
#include "effect_encount.h"
#include "move_pokemon.h"

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
#include "gamesystem/pm_weather.h"
#include "field/zonedata.h"
#include "field/areadata.h"
#include "warpdata.h"   //ARRIVEDATA_SetArriveFlag

#include "field_comm_actor.h"
#include "field_comm/intrude_field.h"
#include "field_comm/intrude_work.h"

#include "fldmmdl.h"

#include "field_debug.h"

#include "field/map_matrix.h"
#include "field_event_check.h"  

#include "eventdata_local.h"
#include "field/eventdata_sxy.h"

#include "field_effect.h"


#include "fieldskill_mapeff.h"

#include "fieldmap_func.h"
#include "field/fieldmap_call.h"

#include "arc/fieldmap/zone_id.h"
#include "field/weather_no.h"
#include "sound/pm_sndsys.h"
//#include "sound/wb_sound_data.sadl"
#include "net_app/union/union_main.h"

#include "debug/debugwin_sys.h" //�f�o�b�O���j���[

#if USE_DEBUGWIN_SYSTEM
#include "mapdatafunc/field_func_random_generate.h" //�f�o�b�O�o�^�̂���
#endif //USE_DEBUGWIN_SYSTEM

#include "field_place_name.h"
#include "field_palace_sys.h"

#include "field_sound.h"

#include "fieldmap_ctrl_grid.h"
#include "fieldmap_ctrl_nogrid_work.h"
#include "fieldmap_ctrl_hybrid.h"

#include "field_gimmick.h"
#include "gmk_tmp_wk.h"

#include "field_status_local.h" //FIELD_STATUS_�`
#include "script.h"   //SCRIPT_CallZoneChangeScript

#include "fld_particle.h"

#include "field_wfbc.h"
#include "fld_vreq.h"

#include "savedata/sodateya_work.h"
#include "sodateya.h"
#include "system/net_err.h"

#include "field_task.h"  
#include "field_task_manager.h"
#include "ev_time.h"  //EVTIME_Update
#include "field_bbd_color.h"

#include "field_crowd_people.h"  

#include "net_app/union_eff.h"

#include "calender.h"

#include "savedata/record.h"

#include "pleasure_boat.h"    //for PL_BOAT_

//======================================================================
//	DEBUG��`
//======================================================================

#define DEBUG_SPEED_CHECK_ENABLE  //�f�o�b�O���x�v����L���ɂ���
#include "debug_speed_check.h"  //�f�o�b�O���x�v���{��
#include "debug/debug_flg.h"
#ifdef PM_DEBUG

#include "field_camera_debug.h"
#include "test/performance.h"

//#define DEBUG_FIELDMAP_SETUP_SPEED_CHECK  //setup�ł̏������ׂ�\��
//#define DEBUG_FIELDMAP_INOUT_SPEED_CHECK  //FIELDMAP�o���ł̏������ׂ�\��
//#define DEBUG_FIELDMAP_DRAW_MICRO_SECOND_CHECK    // �t�B�[���h�}�b�v�`��ɂ����鏈�����Ԃ����߂�
//#define DEBUG_FIELDMAP_UPDATETOP_SPEED_CHECK  //update_top�ł̏������ׂ�����
//#define DEBUG_FIELDMAP_UPDATETAIL_SPEED_CHECK  //update_tail�ł̏������ׂ�����
//#define DEBUG_FIELDMAP_ZONE_CHANGE_SYNC    // �]�[���`�F���W�ɕK�v�ȃV���N�����Ď�


// �t�B�[���h�}�b�v�`��ɂ����鏈�����Ԃ����߂�
#ifdef DEBUG_FIELDMAP_DRAW_MICRO_SECOND_CHECK
#define DEBUG_FIELDMAP_DRAW_MICRO_SECOND_CHECK_DRAW_KEY ( PAD_BUTTON_L )
#endif //DEBUG_FIELDMAP_DRAW_MICRO_SECOND_CHECK

#ifdef DEBUG_MAIN_TIME_AVERAGE_MASTER_ON
extern u8 DEBUG_MAIN_UPDATE_TYPE;  ///<FIELDMAP TOP TAIL �t���[���`�F�b�N�p ���Ԃ́Amain.c
#endif


static void DbgVramDumpCallBack(u32 addr, u32 szByte, void* pUserData);
#endif  //PM_DEBUG

//======================================================================
//	define
//======================================================================

#ifdef PM_DEBUG
extern BOOL DebugBGInitEnd;    //BG�������Ď��t���O
#endif

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
  ///�e�N�X�`���g�p�T�C�Y�w��
  FIELD_3D_VRAM_SIZE    =   GFL_G3D_TEX384K,  // 256K����384K�ɕύX tomoya

  ///�p���b�g�g�p�T�C�Y�w��
  FIELD_3D_PLTT_SIZE    =   GFL_G3D_PLT16K,
  
  ///DTCM�G���A�̃T�C�Y
  FIELD_3D_DTCM_SIZE    =   0x1000, 
  
  ///billboardAct�Ŏg�p���郊�\�[�X�̍ő�ݒ�\��
  FIELD_G3D_BBDACT_RESMAX	    =   64,
  ///billboardAct�Ŏg�p����I�u�W�F�N�g�̍ő�ݒ�\��
  FIELD_G3D_BBDACT_ACTMAX	    =   128,

  FIELD_G3D_SUBBBDACT_RESMAX  =   58,
  FIELD_G3D_SUBBBDACT_ACTMAX  =   80,
  
  FIELD_G3D_EFFBBDACT_RESMAX  =   30,
  FIELD_G3D_EFFBBDACT_ACTMAX  =   40,
  
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

  ///���C���r���{�[�h�@�ڂ肱��T�t���b�v�@�e�N�X�`��T���W�@�␳�l
  FIELD_G3D_BBD_FLIP_TEX_OFFS_T = -(FX16_HALF+0x200),
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


///���샂�f���`�掞�̍����W��
enum {
  MMDL_BBD_OFFS_Y_VALUE = FX32_CONST(4),
  MMDL_BBD_OFFS_POS_Y   = FX32_CONST(-4),
};

//�v���W�F�N�V�����}�g���N�X�𑀍삷��ۂ̂y�I�t�Z�b�g
enum {
  ///�ʏ�̎ˉe�s��␳�l
  PROJ_MAT_Z_OFS        =	(310+0xb8),

  ///�����n�`�ł̎ˉe�s��␳�l
  PROJ_MAT_Z_OFS_DESERT = 310,
};

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
	UNION_EFF_SYSTEM *union_eff;    //���j�I���G�t�F�N�g����V�X�e��
  FIELD_PALACE_SYS *palace_sys;  //�N���n���[�N

	FIELDSKILL_MAPEFF * fieldskill_mapeff;
  

  GFL_G3D_CAMERA *g3Dcamera; //g3Dcamera Lib �n���h��
	GFL_G3D_LIGHTSET *g3Dlightset; //g3Dlight Lib �n���h��
	GFL_TCB *g3dVintr; //3D�pvIntrTask�n���h��
	GFL_BBDACT_SYS *mainBbdActSys; //�r���{�[�h�A�N�g�V�X�e���ݒ�n���h��
  GFL_BBDACT_SYS *subBbdActSys;
  GFL_BBDACT_SYS *effBbdActSys;
  
  FLDMAPFUNC_SYS * fldmapFuncSys;
	
	GAMEMODE gamemode;
	FLDMAPSEQ seq;
  u8 subseq;
  u8 seq_switch;
	int timer;
	u16 map_id;
	LOCATION location;
	
  const VecFx32 *target_now_pos_p;
	VecFx32 now_pos;
  
  fx32 bbdact_y_offs;      ///<���샂�f��BLACT�`�掞��Y�I�t�Z�b�g

  fx32 pro_mat_z_offs;    ///<�`�掞�̎ˉe�ɑ΂���␳�l

	const DEPEND_FUNCTIONS *func_tbl;
	void *mapCtrlWork;
	

  GFL_TCBSYS* fieldmapTCB;
  void* fieldmapTCBSysWork;
  GMK_TMP_WORK  GmkTmpWork;     //�M�~�b�N�e���|�������[�N�|�C���^

  FLD_PRTCL_SYS_PTR FldPrtclSys;
  FLD3D_CI_PTR Fld3dCiPtr;

  SODATEYA* sodateya;  // ��ĉ�

  DRAW3DMODE Draw3DMode;
  
  FLD_G3DOBJ_CTRL *fieldG3dObjCtrl;

  FIELD_TASK_MAN* taskManager;  // �^�X�N�}�l�[�W��

  BOOL MapFadeReq;     //�}�b�v�J�ڗp�t�F�[�h���N�G�X�g

  ENCEFF_CNT_PTR EncEffCntPtr;

  BOOL MainHookFlg;

  BOOL seasonDispFlag; // �G�߂�\�������ǂ���

  FACEUP_WK_PTR FaceUpWkPtr;

  FLD_VREQ* fldVReq;  ///<�t�B�[���hVBlank���f

  FIELD_CROWD_PEOPLE* crowdpeople; ///<�Q�W���[�N/�^�X�N�������Ƀ|�C���^��ݒ� field_event_check.c���ŁA�C�x���g�������ɑ���window���������߁B

#ifdef  PM_DEBUG
	FIELD_DEBUG_WORK *debugWork;
#endif
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
static void fldmap_G3D_Draw_top( FIELDMAP_WORK * fieldWork );
static void fldmap_G3D_Draw_tail( FIELDMAP_WORK * fieldWork );
static void fldmap_G3D_Unload( FIELDMAP_WORK * fieldWork );
static void	fldmap_G3D_VBlank( GFL_TCB* tcb, void* work );
static void	fldmap_G3D_BBDTrans(
		GFL_BBDACT_TRANSTYPE type, u32 dst, u32 src, u32 siz );
static void FIELD_EDGEMARK_Setup(const AREADATA * areadata, const u16 zone_id);
static void fieldmap_G3D_BBDSetColorParam( FIELDMAP_WORK * fieldWork );

//fldmmdl
static void fldmapMain_MMDL_Init( FIELDMAP_WORK *fieldWork );
static void fldmapMain_MMDL_Finish( FIELDMAP_WORK *fieldWork );
static void fldmap_MMDL_InitList(
		MMDL_LIST *mlist, int list_id, HEAPID heapID );
static fx32 calcBbdActYOffset( const FIELDMAP_WORK * fieldmap );

//zonechange
static BOOL fldmapMain_UpdateMoveZone( FIELDMAP_WORK *fieldWork );
static BOOL fldmap_CheckPlayerPosUpdate( FIELDMAP_WORK *fieldWork );
static BOOL fldmap_CheckMoveZoneChange( FIELDMAP_WORK *fieldWork );
static void fldmap_ZoneChange( FIELDMAP_WORK *fieldWork );

static void zoneChange_SetMMdl( GAMEDATA *gdata,
		MMDLSYS *fmmdlsys, EVENTDATA_SYSTEM *evdata, u32 zone_id );
static void zoneChange_SetBGM( FIELDMAP_WORK* fieldWork, u32 next_zone_id );
static void zoneChange_SetWeather( FIELDMAP_WORK *fieldWork, u32 zone_id );
static void zoneChange_SetZoneFogLight( FIELDMAP_WORK *fieldWork, u32 zone_id );
static void zoneChange_UpdatePlayerWork( GAMEDATA *gdata, u32 zone_id );
static void zoneChange_SetCameraArea( FIELDMAP_WORK* fieldWork, u32 zone_id );

static void zoneChangeScene( FIELDMAP_WORK *fieldWork, u32 zone_id );

//etc
static void fldmap_ClearMapCtrlWork( FIELDMAP_WORK *fieldWork );
static void setupCameraArea( FIELDMAP_WORK *fieldWork, u32 zone_id, HEAPID heapID );
static void setupWfbc( GAMEDATA* gdata, FIELDMAP_WORK *fieldWork, u32 zone_id );

static fx32 fldmap_getProjMatZOffsValue( u16 zone_id );

static BOOL zonedata_GetShadowUse( u16 zone_id );

static BOOL zonedata_IsEnvSE( u16 zone_id );


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
static void Draw3DNormalMode_top( FIELDMAP_WORK * fieldWork );
static void Draw3DNormalMode_tail( FIELDMAP_WORK * fieldWork );
static void Draw3DCutinMode(FIELDMAP_WORK * fieldWork);
//static void Draw3DScrnTexMode(FIELDMAP_WORK * fieldWork);
static void DrawEncEff(FIELDMAP_WORK * fieldWork);

static void DrawTop(FIELDMAP_WORK *fieldWork);

typedef void (*DRAW3DMODE_FUNC)(FIELDMAP_WORK * fieldWork);

//--------------------------------------------------------------
//  �I�[�o�[���CID
//--------------------------------------------------------------
FS_EXTERN_OVERLAY(union_room);
FS_EXTERN_OVERLAY(field_intrude);


//======================================================================
//======================================================================
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
  // �n�ʁA�����ڂɂ��������ɂ́A�i���̏����֌W�t����B
  {
    fieldWork->areadata = AREADATA_Create( heapID,
        ZONEDATA_GetAreaID(fieldWork->map_id), PMSEASON_GetConsiderCommSeason(gsys) );
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

  //�ʐM�A�C�R�������j��
  GFL_NET_WirelessIconEasyEnd();
  GFL_NET_WirelessSetForceXYPos(0,0); //���W�Œ胊�Z�b�g
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
	

	//VRAM�N���A
	//GFL_DISP_ClearVRAM( GX_VRAM_D );
	//VRAM�ݒ�
	GFL_DISP_SetBank( &fldmapdata_dispVram );

	//BG������
	fldmap_BG_Init( fieldWork );

	// CLACT������
	GFL_CLACT_SYS_Create(
			&fldmapdata_CLSYS_Init, &fldmapdata_dispVram, HEAPID_FIELD_PRBUF );
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

	//�R�c�f�[�^�̃��[�h
	fldmap_G3D_Load( fieldWork );
	
	//�z�u���ݒ�
	fieldWork->g3Dmapper = FLDMAPPER_Create( fieldWork->heapID, GAMEDATA_GetSeasonID(fieldWork->gamedata) );

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

  // VBLANK���f�Ǘ�
  fieldWork->fldVReq = FLD_VREQ_Create( fieldWork->heapID );

  //���j�I���ƐN���œǂݍ��ރI�[�o�[���C�𕪂���
  if( ZONEDATA_IsUnionRoom( fieldWork->map_id ) == TRUE
      || ZONEDATA_IsColosseum( fieldWork->map_id ) == TRUE){
    GFL_OVERLAY_Load( FS_OVERLAY_ID( union_room ) );
  }
  else{
    GFL_OVERLAY_Load( FS_OVERLAY_ID( field_intrude ) );
  }

	GFL_UI_StartFrameRateMode( GFL_UI_FRAMERATE_30 );

  return MAINSEQ_RESULT_NEXTSEQ;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static MAINSEQ_RESULT mainSeqFunc_setup(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  BOOL continue_flag = TRUE;

  INIT_CHECK();

  switch( fieldWork->subseq )
  {
  case 0:
    //�t���O����F�t�B�[���h�}�b�v�����^�C�~���O
    FIELD_FLAGCONT_INIT_FieldIn( gdata, fieldWork->map_id );

    fieldWork->fldMsgBG = FLDMSGBG_Create( fieldWork->heapID, fieldWork->g3Dcamera );
    fieldWork->goldMsgWin = NULL;

    SET_CHECK("setup: place name");  //�f�o�b�O�F�������׌v��
    // �n���\���V�X�e���쐬
    fieldWork->placeNameSys = FIELD_PLACE_NAME_Create( gsys, HEAPID_PLACE_NAME, fieldWork->fldMsgBG );

    SET_CHECK("setup: camera & scene");  //�f�o�b�O�F�������׌v��
    // �t���b�V���`�F�b�N
    {
      FIELD_STATUS * fldstatus = GAMEDATA_GetFieldStatus( fieldWork->gamedata );
      fieldWork->fieldskill_mapeff = FIELDSKILL_MAPEFF_Create( 
          FIELD_STATUS_GetFieldSkillMapEffectMsk(fldstatus), fieldWork->heapID ); 

      // �������`�F�b�N
#ifdef PM_DEBUG
      {
        u32 msk = FIELD_STATUS_GetFieldSkillMapEffectMsk(fldstatus);
        BOOL flash = FIELD_STATUS_IsFieldSkillFlash(fldstatus);
        msk = FIELDSKILL_MAPEFF_MSK_IS_ON( msk, FIELDSKILL_MAPEFF_MSK_FLASH_FAR );
        if( msk )
        {
          msk = TRUE;
        }
        GF_ASSERT( msk == flash );
      }
#endif
    }

    fieldWork->camera_control = FIELD_CAMERA_Create(
        ZONEDATA_GetCameraID(fieldWork->map_id),
        FIELD_CAMERA_MODE_CALC_CAMERA_POS,
        fieldWork->g3Dcamera,
        &fieldWork->now_pos,
        fieldWork->heapID );
    
    // �V�[���G���A
    fieldWork->sceneArea        = FLD_SCENEAREA_Create( fieldWork->heapID, fieldWork->camera_control, fieldWork );
    fieldWork->sceneAreaLoader  = FLD_SCENEAREA_LOADER_Create( fieldWork->heapID );

    break;

  case 1:
    SET_CHECK("setup: nogrid mapper");  //�f�o�b�O�F�������׌v��
    // NOGRID�}�b�p�[����
    fieldWork->nogridMapper = FLDNOGRID_MAPPER_Create( fieldWork->heapID, fieldWork->camera_control, fieldWork->sceneArea, fieldWork->sceneAreaLoader );
    
    //�i���p�O���[�X�P�[���Ǘ��쐬
    fieldWork->palace_sys = FIELD_PALACE_Create( fieldWork->heapID, gsys , fieldWork , fieldWork->map_id );
    
    SET_CHECK("setup: fldmapper");  //�f�o�b�O�F�������׌v��
    {
      FIELD_BMODEL_MAN * bmodel_man = FLDMAPPER_GetBuildModelManager( fieldWork->g3Dmapper );
      
      FIELDDATA_SetMapperData(fieldWork->map_id,
          fieldWork->areadata,
          &fieldWork->map_res,
          GAMEDATA_GetMapMatrix(fieldWork->gamedata) );

    SET_CHECK("setup: bmodel load");  //�f�o�b�O�F�������׌v��
      //�����Ŕz�u���f�����X�g���Z�b�g����
      {
        u8 *gray_scale;
        GAME_COMM_SYS_PTR game_comm;
        game_comm = GAMESYSTEM_GetGameCommSysPtr( fieldWork->gsys );
        gray_scale  = FIELD_PALACE_GetShadeTable( fieldWork->palace_sys );
        FIELD_BMODEL_MAN_Load(bmodel_man, fieldWork->map_id, fieldWork->areadata, gray_scale);

        //G3DOBJ�ɃO���[�X�P�[���K�p
        FLD_G3DOBJ_CTRL_SetGrayScale( fieldWork->fieldG3dObjCtrl, gray_scale );
      }
    }

    // WFBC�X����ݒ� �e�𑀍삷�邽�߂ɃM�~�b�N�̃Z�b�g�A�b�v�Ɉʒu��ύX
    setupWfbc( gdata, fieldWork, fieldWork->map_id );
    break;

  case 2:
    SET_CHECK("setup: resistdata");  //�f�o�b�O�F�������׌v��
    //�t�B�[���h�}�b�v�p���P�[�V�����쐬

    LOCATION_Set( &fieldWork->location, fieldWork->map_id, 0, 0, LOCATION_DEFAULT_EXIT_OFS,
        fieldWork->now_pos.x, fieldWork->now_pos.y, fieldWork->now_pos.z );

    
    //�}�b�v�f�[�^�o�^
    {
      u8 *gray_scale;
      GAME_COMM_SYS_PTR game_comm;
      game_comm = GAMESYSTEM_GetGameCommSysPtr( fieldWork->gsys );
      gray_scale  = FIELD_PALACE_GetShadeTable( fieldWork->palace_sys );
      FLDMAPPER_ResistData( fieldWork->g3Dmapper, &fieldWork->map_res, gray_scale );
    }

    //NOGRID�}�b�v�f�[�^�o�^
    {
      u32 raildata = ZONEDATA_GetRailDataID(fieldWork->map_id);
      if ( raildata != ZONEDATA_NO_RAILDATA_ID ){
        FLDNOGRID_MAPPER_ResistDataArc( fieldWork->nogridMapper, raildata, fieldWork->heapID );  
      }
    }
    //�`�掞�̎ˉe�ɑ΂���␳�l
    fieldWork->pro_mat_z_offs = fldmap_getProjMatZOffsValue( fieldWork->map_id );

    SET_CHECK("setup: cameraArea");  //�f�o�b�O�F�������׌v��

    //CAMERA_AREA�̔��f
    setupCameraArea( fieldWork, fieldWork->map_id, fieldWork->heapID );
    break;

  case 3:
    SET_CHECK("setup: mmdl init");  //�f�o�b�O�F�������׌v��
    //���샂�f��������
    fldmapMain_MMDL_Init(fieldWork);
    
    SET_CHECK("setup: fldeff sys");  //�f�o�b�O�F�������׌v��
    //�t�B�[���h�G�t�F�N�g������
    fieldWork->fldeff_ctrl = FLDEFF_CTRL_Create(
        fieldWork, FLDEFF_PROCID_MAX, fieldWork->heapID );
    
    //�t�B�[���h�G�t�F�N�g�@�p�����^�ݒ�@�^�X�N
    FLDEFF_CTRL_SetTaskParam( fieldWork->fldeff_ctrl, FLDEFF_TASK_MAX );
    
    SET_CHECK("setup: fldeff grayscale");  //�f�o�b�O�F�������׌v��
    //�t�B�[���h�G�t�F�N�g�@�p�����^�ݒ� �O���[�X�P�[��
    {
      u8 *gray_scale = FIELD_PALACE_GetShadeTable( fieldWork->palace_sys );
      FLDEFF_CTRL_SetGrayScaleParam( fieldWork->fldeff_ctrl, gray_scale );
    }
    
    SET_CHECK("setup: fldeff regist");  //�f�o�b�O�F�������׌v��
    //�t�B�[���h�G�t�F�N�g�@�o�^
    FLDEFF_CTRL_RegistEffect( fieldWork->fldeff_ctrl,
      DATA_FLDEFF_RegistEffectGroundTbl, DATA_FLDEFF_RegistEffectGroundTblNum );

    //���j�I�����[���Ȃ烆�j�I���G�t�F�N�g����V�X�e�����쐬
    if( ZONEDATA_IsUnionRoom( fieldWork->map_id ) == TRUE
        || ZONEDATA_IsColosseum( fieldWork->map_id ) == TRUE ){
      fieldWork->union_eff = UNION_EFF_SystemSetup(fieldWork->heapID);
    }
    
    break;

  case 4:
    SET_CHECK("setup: create_func");  //�f�o�b�O�F�������׌v��
    {
      PLAYER_WORK *pw = GAMESYSTEM_GetMyPlayerWork(gsys);
      const u16 dir = PLAYERWORK_getDirection_Type( pw );
      const VecFx32 *pos = &pw->position;
      
      //���@�쐬
      {
        //���]�ԋ֎~�ȏꏊ�ł���Ύ��O�Ɏ��]�Ԍ`�Ԃ�����
        if( PLAYERWORK_GetMoveForm(pw) == PLAYER_MOVE_FORM_CYCLE ){
          if( ZONEDATA_BicycleEnable(fieldWork->map_id) == FALSE ){
            PLAYERWORK_SetMoveForm(pw,PLAYER_MOVE_FORM_NORMAL);
          }
        }
        
        {
          MYSTATUS *mystatus = GAMEDATA_GetMyStatus( gdata );
          int sex = MyStatus_GetMySex( mystatus );
          fieldWork->field_player = FIELD_PLAYER_Create(
              pw, fieldWork, pos, sex, fieldWork->heapID );
        }
      }
      
      //�o�^�e�[�u�����ƂɌʂ̏������������Ăяo��
      fieldWork->now_pos = *pos;
      fieldWork->func_tbl->create_func(fieldWork, &fieldWork->now_pos, dir);

      FLDMAPPER_SetPos( fieldWork->g3Dmapper, &fieldWork->now_pos );
      
      TAMADA_Printf("start X,Y,Z=%d,%d,%d\n", FX_Whole(pos->x), FX_Whole(pos->y), FX_Whole(pos->z));
      TAMADA_Printf( "Start Dir = %04x\n", pw->direction );
    }
    SET_CHECK("setup: gimmick sys");  //�f�o�b�O�F�������׌v��

    //�M�~�b�N�e���|�������[�N������
    InitGmkTmpWork(&fieldWork->GmkTmpWork);

    //�g��3�c�I�u�W�F�N�g���W���[���쐬
    fieldWork->ExpObjCntPtr = FLD_EXP_OBJ_Create ( EXP_OBJ_RES_MAX, EXP_OBJ_MAX, fieldWork->heapID );
    
    //�G�b�W�}�[�L���O�ݒ�Z�b�g�A�b�v
    FIELD_EDGEMARK_Setup( fieldWork->areadata, FIELDMAP_GetZoneID( fieldWork ) );

    SET_CHECK("setup: fogs");  //�f�o�b�O�F�������׌v��
    // �t�H�O�V�X�e������
    fieldWork->fog	= FIELD_FOG_Create( fieldWork->heapID );
    
    // �]�[���t�H�O�V�X�e������
    fieldWork->zonefog = FIELD_ZONEFOGLIGHT_Create( fieldWork->heapID );
    FIELD_ZONEFOGLIGHT_LoadReq( fieldWork->zonefog, ZONEDATA_GetFog(fieldWork->map_id), ZONEDATA_GetLight(fieldWork->map_id), 1 );

    SET_CHECK("setup: light");  //�f�o�b�O�F�������׌v��
    // ���C�g�V�X�e������
    {
      TOMOYA_Printf( "�i�����܂񂾁@�V�[�Y�� %d �ʏ�̃V�[�Y�� %d\n", PMSEASON_GetConsiderCommSeason(gsys), GAMEDATA_GetSeasonID( fieldWork->gamedata ) );
      fieldWork->light = FIELD_LIGHT_Create( AREADATA_GetLightType( fieldWork->areadata ), 
          GFL_RTC_GetTimeBySecond(), 
          PMSEASON_GetConsiderCommSeason(gsys),
          fieldWork->fog, fieldWork->g3Dlightset, fieldWork->heapID );
    }

    SET_CHECK("setup: weather");  //�f�o�b�O�F�������׌v��
    // �V�C�V�X�e������
    {
      FIELD_SOUND* fsnd;
      fsnd = GAMEDATA_GetFieldSound( fieldWork->gamedata );
      
      fieldWork->weather_sys = FIELD_WEATHER_Init(
          fieldWork->camera_control,
          fieldWork->light,
          fieldWork->fog,
          fieldWork->zonefog, 
          fsnd,
          PMSEASON_GetConsiderCommSeason(gsys),
          HEAPID_WEATHER ); // �v���O�����q�[�v���g�p����B
    }
    
    // �V�C����
    FIELD_WEATHER_Set(
        fieldWork->weather_sys, GAMEDATA_GetWeatherNo( fieldWork->gamedata ), HEAPID_WEATHER );
    
    SET_CHECK("setup: gimmick setup");  //�f�o�b�O�F�������׌v��
    //�t�B�[���h�M�~�b�N�Z�b�g�A�b�v
    FLDGMK_SetUpFieldGimmick(fieldWork);
    break;

  case 5:
    
    SET_CHECK("setup: subscreen");  //�f�o�b�O�F�������׌v��
    //���o�[�̏�����
    {
      fieldWork->fieldSubscreenWork = FIELD_SUBSCREEN_Init(fieldWork->heapID, fieldWork, GAMEDATA_GetSubScreenMode(gdata));
    }
    break;
  case 6:
    
    SET_CHECK("setup: 3dsystem");  //�f�o�b�O�F�������׌v��
    //�t�B�[���h�G���J�E���g������
    fieldWork->encount = FIELD_ENCOUNT_Create( fieldWork );
    
    //�ʐM�������R�[���o�b�N�Ăяo��
    {
      GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
      GameCommSys_Callback_FieldCreate( game_comm, fieldWork );
    }

    // �t�B�[���h�}�b�v�p����^�X�N�V�X�e��
    fieldWork->fldmapFuncSys = FLDMAPFUNC_Sys_Create( fieldWork, fieldWork->heapID, FLDMAPFUNC_TASK_MAX );

#ifdef  PM_DEBUG
    fieldWork->debugWork = NULL;
#endif

    //�t�B�[���h�p�[�e�B�N��
    fieldWork->FldPrtclSys = FLD_PRTCL_Init(HEAPID_FLD3DCUTIN);
    //�t�B�[���h3D�J�b�g�C��
    fieldWork->Fld3dCiPtr = FLD3D_CI_Init(HEAPID_FLD3DCUTIN, fieldWork->FldPrtclSys);
    //�G���J�E���g�G�t�F�N�g
    fieldWork->EncEffCntPtr = ENCEFF_CreateCntPtr(fieldWork->heapID, fieldWork);

    // ��ĉ�
    {
      SAVE_CONTROL_WORK* scw = GAMEDATA_GetSaveControlWork( fieldWork->gamedata );
      SODATEYA_WORK* work = SODATEYA_WORK_GetSodateyaWork( scw );
      fieldWork->sodateya = SODATEYA_Create( fieldWork->heapID, fieldWork, work );
    }
    
    // scenearea
    {
      zoneChangeScene( fieldWork, fieldWork->map_id );
    }
    continue_flag = FALSE;  //setup�����̍Ō�I
    break;
  }
  fieldWork->subseq ++;

  SET_CHECK("setup: tail");  //�f�o�b�O�F�������׌v��
#ifdef  DEBUG_FIELDMAP_SETUP_SPEED_CHECK
  {
    OSTick end_tick;
    TAIL_CHECK(&end_tick);
    OS_Printf("mainSeqFunc_setup:total %ld\n", OS_TicksToMicroSeconds( end_tick ) );
    PUT_CHECK();
  }
#endif
  
  //3�c�`�惂�[�h�͒ʏ�ŃZ�b�g�A�b�v
  fieldWork->Draw3DMode = DRAW3DMODE_NORMAL;


  if ( continue_flag ) {
    return MAINSEQ_RESULT_CONTINUE;
  } else {
    return MAINSEQ_RESULT_NEXTSEQ;
  }
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static MAINSEQ_RESULT mainSeqFunc_ready(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{ 
#ifdef  DEBUG_FIELDMAP_INOUT_SPEED_CHECK
  OS_TPrintf("mainSeqFunc_ready\n");
#endif

  switch ( fieldWork->subseq )
  {
  case 0:
    //FLDMAPPER �ꊇ�ǂݍ���
    FLDMAPPER_AllSetUp( fieldWork->g3Dmapper );

    if( fieldWork->fldMMdlSys != NULL ){
      do {
        MMDLSYS_UpdateProc( fieldWork->fldMMdlSys );
        //VBlank���ԗp�֐������A�Ó]����VRAM�o�^�������邾���Ȃ̂ŕ���p���Ȃ�
        MMDL_BLACTCONT_ProcVBlank( fieldWork->fldMMdlSys );
      } while ( MMDL_BLACTCONT_IsThereReserve(fieldWork->fldMMdlSys) == TRUE );
    }

    // �V�C�҂�
    while( FIELD_WEATHER_IsLoading(fieldWork->weather_sys) ){
      FIELD_WEATHER_Main( fieldWork->weather_sys, HEAPID_WEATHER );
    }

    fldmap_G3D_Control( fieldWork );
    fldmap_G3D_Draw_top( fieldWork );
    fldmap_G3D_Draw_tail( fieldWork );
    GFL_CLACT_SYS_Main(); // CLSYS���C��
    
    if(fieldWork->fldMsgBG){ FLDMSGBG_PrintMain( fieldWork->fldMsgBG ); }
    
#ifdef  PM_DEBUG
    if(fieldWork->debugWork){ FIELD_DEBUG_UpdateProc( fieldWork->debugWork ); }
#endif

    FLDEFF_CTRL_Update( fieldWork->fldeff_ctrl );

    // �t�B�[���h�}�b�v�p����^�X�N�V�X�e��
    FLDMAPFUNC_Sys_Main( fieldWork->fldmapFuncSys );


    fieldWork->subseq ++;
    /* FALL THROUGH */

  case 1:
    //2010.06.20  tamada
    //�u�Â�����v�̏ꍇ�A�l�V�������ȂǂŊ����I��SE�̊J�n����
    //�ǂݍ��ݒ��ōĐ��ł��Ȃ��Ƃ�����肪����B
    //���̑Ώ��̂��߁A�����ŕ������[�h�̊�����҂�
    if ( FIELD_STATUS_GetContinueFlag( GAMEDATA_GetFieldStatus( fieldWork->gamedata ) ) == TRUE )
    {
      if (zonedata_IsEnvSE( fieldWork->map_id ) == TRUE )
      { //����SE��K�v�Ƃ���}�b�v�����A�����œǂݍ��ݑ҂����s��
        if ( PMSND_IsLoading() )
        {
          TAMADA_Printf( " BGM Load Wait!!\n" );
          return MAINSEQ_RESULT_CONTINUE;
        }
      }
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
    FIELD_STATUS_SetContinueFlag( GAMEDATA_GetFieldStatus( fieldWork->gamedata ), FALSE );
    FLDMAPPER_AllSetUp( fieldWork->g3Dmapper ); // �������X�N���v�g�Œǉ����ꂽ�}�b�v�u���b�N��ǂݍ���


    //�t�B�[���h�}�b�v�p�C�x���g�N���`�F�b�N���Z�b�g����
    if( ZONEDATA_IsUnionRoom( fieldWork->map_id ) == TRUE
        || ZONEDATA_IsColosseum( fieldWork->map_id ) == TRUE){
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
  }
  

  return MAINSEQ_RESULT_NEXTSEQ;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static MAINSEQ_RESULT mainSeqFunc_update_top(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
  // �`��ɂ����� micro second ��\��
#ifdef DEBUG_FIELDMAP_DRAW_MICRO_SECOND_CHECK
  OSTick debug_fieldmap_start_tick = OS_GetTick(); 
  OSTick debug_fieldmap_end_tick;
#endif

  INIT_CHECK();


#ifdef DEBUG_MAIN_TIME_AVERAGE_MASTER_ON
  DEBUG_MAIN_UPDATE_TYPE = 0;
#endif

  if ( GAMESYSTEM_GetEvent( fieldWork->gsys ) == NULL )
  { //�C�x���g���Ȃ���Ԃ̎�==�ʏ�t�B�[���h���
    FIELD_STATUS * fldstatus = GAMEDATA_GetFieldStatus( fieldWork->gamedata );
    FIELD_STATUS_SetProcAction( fldstatus, PROC_ACTION_FIELD );
  }

  if (GAMEDATA_GetIsSave( fieldWork->gamedata )) return MAINSEQ_RESULT_CONTINUE;

  if (fieldWork->MainHookFlg)
  {
    //�`�敔���͍s��
    DrawTop(fieldWork);
    GFL_CLACT_SYS_Main(); // CLSYS���C��
    //�� 2010.03.05�@CLSYS���C�����e�C������g�b�v�ֈړ��B���̈ړ��ŁA2�c�`���3�c�`��ɂP�V���N�̂��ꂪ�����܂��Bsaito
    return MAINSEQ_RESULT_CONTINUE;
  }

	//�L�[�̕����擾�J�E���^�����Z�b�g
	GFL_UI_ResetFrameRate();
  SET_CHECK("update_top:zone change check");
	//�]�[���X�V����
  fldmapMain_UpdateMoveZone( fieldWork );
  SET_CHECK("update_top:map main func");
  //�}�b�v�� �o�^����
  if( GAMESYSTEM_IsEventExists(gsys) == FALSE ) {
    //�o�^�e�[�u�����ƂɌʂ̃��C���������Ăяo��
    fieldWork->func_tbl->main_func( fieldWork, &fieldWork->now_pos ); 
    //���ԃC�x���g�̃A�b�v�f�[�g�������Ăяo��
    EVTIME_Update( fieldWork->gamedata );
  }
  SET_CHECK("update_top:placename & skill eff");
  // �n���\���V�X�e�����쏈��
  if(fieldWork->placeNameSys){ FIELD_PLACE_NAME_Process( fieldWork->placeNameSys ); }
  // �t���b�V������
  FIELDSKILL_MAPEFF_Main( fieldWork->fieldskill_mapeff );
  SET_CHECK("update_top:player update");
  //���@�X�V
  FIELD_PLAYER_Update( fieldWork->field_player );
  SET_CHECK("update_top:comm update");
  //�ʐM�p����(�v���C���[�̍��W�̐ݒ�Ƃ�
  if( ZONEDATA_IsUnionRoom( fieldWork->map_id ) == TRUE
      || ZONEDATA_IsColosseum( fieldWork->map_id ) == TRUE){
    Union_Main(GAMESYSTEM_GetGameCommSysPtr(gsys), fieldWork);
  }
  else{
    IntrudeField_UpdateCommSystem( fieldWork, fieldWork->gsys, fieldWork->field_player );
  }
  SET_CHECK("update_top:subscreen");
  FIELD_SUBSCREEN_Main(fieldWork->fieldSubscreenWork);
#ifdef  PM_DEBUG
  if(fieldWork->debugWork){ FIELD_DEBUG_UpdateProc( fieldWork->debugWork ); }
#endif
  SET_CHECK("update_top:mmdlsys");
  if( fieldWork->fldMMdlSys != NULL ){
    MI_SetMainMemoryPriority(MI_PROCESSOR_ARM9); //500 micro sec ���x�팸
    MMDLSYS_UpdateProc( fieldWork->fldMMdlSys );
    MI_SetMainMemoryPriority(MI_PROCESSOR_ARM7);
  }
  SET_CHECK("update_top:gimmick");
  //�M�~�b�N����
  FLDGMK_MoveFieldGimmick(fieldWork);
  SET_CHECK("update_top:fldeff");
  FLDEFF_CTRL_Update( fieldWork->fldeff_ctrl );
  if(fieldWork->union_eff != NULL){
    UNION_EFF_SystemUpdate(fieldWork->union_eff);
  }
  SET_CHECK("update_top:FLDMAPFUNC_Sys_Main");
	// �t�B�[���h�}�b�v�p����^�X�N�V�X�e��
	FLDMAPFUNC_Sys_Main( fieldWork->fldmapFuncSys );
  SET_CHECK("update_top:FLDMSGBG_PrintMain");
  if(fieldWork->fldMsgBG ){ FLDMSGBG_PrintMain( fieldWork->fldMsgBG ); }
  SET_CHECK("update_top:TASK");
  // TCB
  GFL_TCB_Main( fieldWork->fieldmapTCB );
  // �^�X�N�}�l�[�W��
  FIELD_TASK_MAN_Main( fieldWork->taskManager );
  //now_pos�X�V
  if( fieldWork->target_now_pos_p != NULL ){
    fieldWork->now_pos = *fieldWork->target_now_pos_p;
  }

  SET_CHECK("update_top:g3d_control");
  //Map�V�X�e���Ɉʒu��n���Ă���B
  //���ꂪ�Ȃ��ƃ}�b�v�ړ����Ȃ��̂Œ���
  FLDMAPPER_SetPos( fieldWork->g3Dmapper, &fieldWork->now_pos );
  // Visual�ɂ�����鏈��
	fldmap_G3D_Control( fieldWork );
  //�V���D�D�J�Ď��i�V���D�ł͂Ȃ��Ƃ��͏������X���[����j
  {
    PL_BOAT_WORK_PTR *wk_ptr = GAMEDATA_GetPlBoatWorkPtr(fieldWork->gamedata);
    PL_BOAT_Main(*wk_ptr);
  }
  SET_CHECK("update_top:CLACT");

  // ----------------------top��3D�`�揈��---------------------------------------
  // ����ȍ~�Ƀf�[�^�X�V���������Ȃ����ƁB
  GFL_CLACT_SYS_Main(); // CLSYS���C��
  //�� 2010.03.05�@CLSYS���C�����e�C������g�b�v�ֈړ��B���̈ړ��ŁA2�c�`���3�c�`��ɂP�V���N�̂��ꂪ�����܂��Bsaito
  SET_CHECK("update_top:DrawTop");
  DrawTop(fieldWork);
  SET_CHECK("update_top:tail");
#ifdef DEBUG_FIELDMAP_DRAW_MICRO_SECOND_CHECK
  debug_fieldmap_end_tick = OS_GetTick();
  debug_fieldmap_end_tick -= debug_fieldmap_start_tick;

  if( GFL_UI_KEY_GetCont() & DEBUG_FIELDMAP_DRAW_MICRO_SECOND_CHECK_DRAW_KEY )
  {
    OS_Printf( "top_tick:%d micro second:%d\n",
        debug_fieldmap_end_tick, OS_TicksToMicroSeconds( debug_fieldmap_end_tick ) );
  }
#endif
#ifdef  DEBUG_FIELDMAP_UPDATETOP_SPEED_CHECK
  {
    OSTick end_tick;
    TAIL_CHECK( &end_tick );
    if ( (GFL_UI_KEY_GetTrg() & PAD_BUTTON_L) ||
        ( (GFL_UI_KEY_GetCont() & (PAD_BUTTON_L+PAD_BUTTON_R)) == (PAD_BUTTON_L+PAD_BUTTON_R)) )
    {
      PUT_CHECK();
      OS_TPrintf("top_tick:%ld", end_tick);
      OS_TPrintf(" micro sec:%ld\n", OS_TicksToMicroSeconds( end_tick ) );
    }
  }
#endif

  return MAINSEQ_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�}�b�v�@�g�b�v�����`��
 * @param	fieldWork	FIELDMAP_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void DrawTop(FIELDMAP_WORK *fieldWork)
{
#ifdef PM_DEBUG
  FIELD_DEBUG_ClearDrawCallBackWork();
#endif  
  GFL_G3D_ClearG3dInfo();
  FIELD_CAMERA_Main( fieldWork->camera_control, GFL_UI_KEY_GetCont() );

  fieldWork->bbdact_y_offs = calcBbdActYOffset( fieldWork );

  MI_SetMainMemoryPriority(MI_PROCESSOR_ARM9);
  fldmap_G3D_Draw_top( fieldWork );
  MI_SetMainMemoryPriority(MI_PROCESSOR_ARM7);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static MAINSEQ_RESULT mainSeqFunc_update_tail(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
  
  // �`��ɂ����� micro second ��\��
#ifdef DEBUG_FIELDMAP_DRAW_MICRO_SECOND_CHECK
  OSTick debug_fieldmap_start_tick = OS_GetTick(); 
  OSTick debug_fieldmap_end_tick;
#endif
  INIT_CHECK();

#ifdef DEBUG_MAIN_TIME_AVERAGE_MASTER_ON
  DEBUG_MAIN_UPDATE_TYPE = 1;
#endif
  
  if(GAMEDATA_GetIsSave( fieldWork->gamedata )){
     GAMEDATA_ResetFrameSpritCount(GAMESYSTEM_GetGameData(gsys));
     return MAINSEQ_RESULT_CONTINUE;
  }

  //���[�h�㔼
  MI_SetMainMemoryPriority(MI_PROCESSOR_ARM9); //500 micro sec ���x�팸
  FLDMAPPER_MainTail( fieldWork->g3Dmapper );
  MI_SetMainMemoryPriority(MI_PROCESSOR_ARM7);
  
  SET_CHECK("update_tail:subscreen");
  FIELD_SUBSCREEN_Draw(fieldWork->fieldSubscreenWork);
  
  SET_CHECK("update_tail:fldmsgbg");
  if(fieldWork->fldMsgBG ){ FLDMSGBG_PrintMain( fieldWork->fldMsgBG ); }

  SET_CHECK("update_tail:place name");
  if(fieldWork->placeNameSys){ FIELD_PLACE_NAME_Draw( fieldWork->placeNameSys ); }
  
	MI_SetMainMemoryPriority(MI_PROCESSOR_ARM9);
	fldmap_G3D_Draw_tail( fieldWork );
	MI_SetMainMemoryPriority(MI_PROCESSOR_ARM7);

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
    OS_TPrintf( "draw_tick:%d ", debug_fieldmap_end_tick );
    OS_TPrintf( "micro second:%d\n", OS_TicksToMicroSeconds( debug_fieldmap_end_tick ) );
    OS_TPrintf( "poly_vtx = %d, %d\n",
        G3X_GetPolygonListRamCount(),
        G3X_GetVtxListRamCount()
        );
    OS_TPrintf( "total poly_vtx = %d, %d\n",
        info->TotalPolygonNum,
        info->TotalVertexNum
        );
  }
#endif
#ifdef  DEBUG_FIELDMAP_UPDATETAIL_SPEED_CHECK
  {
    OSTick end_tick;
    TAIL_CHECK( &end_tick );
    if ( (GFL_UI_KEY_GetTrg() & PAD_BUTTON_L) ||
        ( (GFL_UI_KEY_GetCont() & (PAD_BUTTON_L+PAD_BUTTON_R)) == (PAD_BUTTON_L+PAD_BUTTON_R)) )
    {
      PUT_CHECK();
      OS_TPrintf("tail_tick:%ld", end_tick);
      OS_TPrintf(" micro sec:%ld\n", OS_TicksToMicroSeconds( end_tick ) );
    }
  }
#endif

  return MAINSEQ_RESULT_CONTINUE;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static MAINSEQ_RESULT mainSeqFunc_free(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
  //�G���J�E���g�G�t�F�N�g�R���g���[���j��
  ENCEFF_DeleteCntPtr(fieldWork->EncEffCntPtr);
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

  if(fieldWork->union_eff != NULL){
    UNION_EFF_SystemExit(fieldWork->union_eff);
    fieldWork->union_eff = NULL;
  }
  
  // �V�C�V�X�e���j��
  FIELD_WEATHER_Exit( fieldWork->weather_sys );
  fieldWork->weather_sys = NULL;

  // ���C�g�V�X�e���j��
  FIELD_LIGHT_Delete( fieldWork->light );

	// �]�[���t�H�O�V�X�e���j��
	FIELD_ZONEFOGLIGHT_Delete( fieldWork->zonefog );
  
  // �t�H�O�V�X�e���j��
  FIELD_FOG_Delete( fieldWork->fog );

  //�N���n�J��
  FIELD_PALACE_Delete( fieldWork->palace_sys );
  
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

#ifdef  PM_DEBUG
  if(fieldWork->debugWork){ FIELD_DEBUG_Delete( fieldWork->debugWork ); }
#endif

	GAMEDATA_SetFrameSpritEnable(GAMESYSTEM_GetGameData(gsys), FALSE);
	GFL_UI_StartFrameRateMode( GFL_UI_FRAMERATE_60 );

  // ��ĉ�
  SODATEYA_Delete( fieldWork->sodateya );

	FIELDSKILL_MAPEFF_Delete( fieldWork->fieldskill_mapeff );

#if USE_DEBUGWIN_SYSTEM
  FIELD_FUNC_RANDOM_GENERATE_TermDebug();
  DEBUGWIN_ExitProc();
#endif  //USE_DEBUGWIN_SYSTEM


#ifdef PM_DEBUG
  //3�c�`��R�[���o�b�N���Z�b�g
  FIELD_DEBUG_SetDrawCallBackFunc(FALSE);
#endif  


#ifdef DEBUG_MAIN_TIME_AVERAGE_MASTER_ON
  DEBUG_MAIN_UPDATE_TYPE = 0;
#endif


  return MAINSEQ_RESULT_NEXTSEQ;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static MAINSEQ_RESULT mainSeqFunc_end(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{ 
  // TCB
  GFL_TCB_Exit( fieldWork->fieldmapTCB );
  GFL_HEAP_FreeMemory( fieldWork->fieldmapTCBSysWork );

  // VBLANK���f�Ǘ�
  FLD_VREQ_Delete( fieldWork->fldVReq );
  fieldWork->fldVReq = NULL;

  // �^�X�N�}�l�[�W��
  FIELD_TASK_MAN_Delete( fieldWork->taskManager );

	GFL_TCB_DeleteTask( fieldWork->g3dVintr );

  FLDMAPPER_Delete( fieldWork->g3Dmapper );

	fldmap_G3D_Unload( fieldWork );	//�R�c�f�[�^�j��
	
	GFL_CLACT_SYS_Delete(); // CLACT�j��
	
	GFL_BMPWIN_Exit();
	
	fldmap_BG_Exit( fieldWork );

  //���j�I���ƐN���œǂݍ��ރI�[�o�[���C�𕪂���
  if( ZONEDATA_IsUnionRoom( fieldWork->map_id ) == TRUE
      || ZONEDATA_IsColosseum( fieldWork->map_id ) == TRUE){
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( union_room ) );
  }
  else{
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( field_intrude ) );
  }

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
#ifdef  DEBUG_FIELDMAP_INOUT_SPEED_CHECK
  OSTick debug_tick = OS_GetTick(); 
#endif

  GF_ASSERT(fieldWork->seq < NELEMS(mainfuncTable));
	fieldWork->timer++;

  result = MAINSEQ_RESULT_CONTINUE;
  func = mainfuncTable[fieldWork->seq][fieldWork->seq_switch];
  if (func != NULL) 
  {
    result = func(gsys, fieldWork);
  }
#ifdef  DEBUG_FIELDMAP_INOUT_SPEED_CHECK
  if ( fieldWork->seq != FLDMAPSEQ_UPDATE )
  {
    debug_tick = OS_GetTick() - debug_tick;
    OS_Printf("FIELDMAP seq:%d tick:%ld\n", fieldWork->seq,  debug_tick );
    OS_Printf("FIELDMAP seq:%d micro:%ld\n", fieldWork->seq, OS_TicksToMicroSeconds( debug_tick ) );
  }
#endif

  switch (result) 
  { 
  case MAINSEQ_RESULT_CONTINUE:
#ifdef PM_DEBUG    
    DEBUG_PerformanceSetTopFlg(fieldWork->seq_switch);
#endif    
    fieldWork->seq_switch = !fieldWork->seq_switch;
    break;
  case MAINSEQ_RESULT_NEXTSEQ:
    fieldWork->seq ++;
    fieldWork->subseq = 0;
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
#ifdef PM_DEBUG
  DebugBGInitEnd = FALSE;    //BG�������Ď��t���O�N���A
#endif
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
  BOOL ret = FALSE;
  PLAYER_MOVE_FORM form;
    
  form = FIELD_PLAYER_GetMoveForm( fieldWork->field_player );

  if( form == PLAYER_MOVE_FORM_CYCLE )
  {
    ret = TRUE;
    FIELD_PLAYER_SetRequest( fieldWork->field_player, FIELD_PLAYER_REQBIT_NORMAL );
  }
  else if( form == PLAYER_MOVE_FORM_NORMAL )
  {
    ret = TRUE;
    PMSND_PlaySE( SEQ_SE_BICYCLE );
    FIELD_PLAYER_SetRequest( fieldWork->field_player, FIELD_PLAYER_REQBIT_CYCLE );

		{	// ���R�[�h�f�[�^�{
			RECORD * rec = GAMEDATA_GetRecordPtr( fieldWork->gamedata );
			RECORD_Inc( rec, RECID_RIDE_CYCLE );
		}
  }
  
  if( ret == TRUE ){
    FIELD_PLAYER_UpdateRequest( fieldWork->field_player );
  }
  
  return( ret );
}

//======================================================================
//	�t�B�[���h�}�b�v�@�Q�ƁA�ݒ�
//======================================================================
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
 * FIELDMAP_WORK �w��]�[���̌��݂̓V���Ԃ�
 * @param	fieldWork	FIELDMAP_WORK
 * @retval FIELD_WEATHER*
 */
//--------------------------------------------------------------
u16 FIELDMAP_GetZoneWeatherID( FIELDMAP_WORK *fieldWork, u16 zone_id )
{
	return PM_WEATHER_GetZoneWeatherNo( fieldWork->gsys, zone_id );
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

#ifdef  PM_DEBUG
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
#endif

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
 * FIELDMAP_WORK ���C��GFL_BBDACT_SYS�擾
 * @param fieldWork FIELDMAP_WORK
 * @retval GFL_BBDACT_SYS
 */
//--------------------------------------------------------------
GFL_BBDACT_SYS * FIELDMAP_GetBbdActSys( FIELDMAP_WORK *fieldWork )
{
	return fieldWork->mainBbdActSys;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK �T�uGFL_BBDACT_SYS�擾
 * @param fieldWork FIELDMAP_WORK
 * @retval GFL_BBDACT_SYS
 */
//--------------------------------------------------------------
GFL_BBDACT_SYS * FIELDMAP_GetSubBbdActSys( FIELDMAP_WORK *fieldWork )
{
  return fieldWork->subBbdActSys;
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK �G�t�F�N�gGFL_BBDACT_SYS�擾
 * @param fieldWork FIELDMAP_WORK
 * @retval GFL_BBDACT_SYS
 */
//--------------------------------------------------------------
GFL_BBDACT_SYS * FIELDMAP_GetEffBbdActSys( FIELDMAP_WORK *fieldWork )
{
  return fieldWork->effBbdActSys;
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
 * @brief  FLD_G3DOBJ_CTRL�擾
 * @param  fieldWork FIELDMAP_WORK
 * @retval FLD_G3DOBJ_CTRL*
 */
//--------------------------------------------------------------
FLD_G3DOBJ_CTRL * FIELDMAP_GetFldG3dOBJCtrl( FIELDMAP_WORK *fieldWork )
{
  GF_ASSERT( fieldWork->fieldG3dObjCtrl != NULL );
  return( fieldWork->fieldG3dObjCtrl );
}

//--------------------------------------------------------------
/**
 * FIELDMAP_WORK UNION_EFF_SYSTEM�擾
 * @param fieldWork FIELDMAP_WORK
 * @retval UNION_EFF_SYSTEM*
 */
//--------------------------------------------------------------
UNION_EFF_SYSTEM * FIELDMAP_GetUnionEffSystem( FIELDMAP_WORK *fieldWork )
{
  GF_ASSERT(fieldWork->union_eff != NULL);
  return fieldWork->union_eff;
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
	//�f�B�X�v���C�ʂ̑I��
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_MAIN );
	GFL_DISP_SetDispOn();
  
  //�g�p����E�B���h�E
  GX_SetVisibleWnd( GX_WNDMASK_NONE );
}

/*
 *  @brief  �t�B�[���hBG���[�h�f�t�H���g������
 */
void FIELDMAP_InitBGMode( void )
{
	//�a�f���[�h�ݒ�
	GFL_BG_SetBGMode( &fldmapdata_bgsysHeader );
}


/*
 *  @brief  �t�B�[���hBG Vram�f�t�H���g������
 */
void FIELDMAP_InitBG( FIELDMAP_WORK* fieldWork )
{
  /**
  //�A�t�B��������
  //@note 
  //�A�v���P�[�V�������ŁA�a�f�Q�ʂŃA�t�B�����g�p�����ꍇ�t�B�[���h�ɖ߂��Ă��Ă��p�����[�^���c���Ă���A
  //���������a�f�Q�ʂ��g�p����A�J�b�g�C����N���X�t�F�[�h�����ŕ\���̕s����������邽�߂����ŏ����������܂��B
  */
  {
    MtxFx22 mtx;
    GFL_CALC2D_GetAffineMtx22( &mtx, 0, FX32_ONE, FX32_ONE, GFL_CALC2D_AFFINE_MAX_NORMAL );
    G2_SetBG2Affine( &mtx, 0, 0, 0, 0 );
  }
	//�a�f�R���g���[���ݒ�
	GFL_BG_SetBGControl3D( FLDBG_MFRM_3D_PRI );

  // ��b�E�C���h�E���\�[�X�Z�b�g�A�b�v
	FLDMSGBG_SetupResource( fieldWork->fldMsgBG );
#if 0
  {
    GFL_BG_BGCNT_HEADER bgcntText = {
			0, 0, FLDBG_MFRM_EFF1_SCRSIZE, 0,
			GFL_BG_SCRSIZ_256x256, FLDBG_MFRM_EFF1_COLORMODE,
			FLDBG_MFRM_EFF1_SCRBASE, FLDBG_MFRM_EFF1_CHARBASE, FLDBG_MFRM_EFF1_CHARSIZE,
			GX_BG_EXTPLTT_01, FLDBG_MFRM_EFF1_PRI, 0, 0, FALSE
		};
		
		GFL_BG_SetBGControl( FLDBG_MFRM_EFF1, &bgcntText, GFL_BG_MODE_TEXT );
		GFL_BG_SetVisible( FLDBG_MFRM_EFF1, VISIBLE_ON );
		GFL_BG_FillCharacter( FLDBG_MFRM_EFF1, 0x00, 1, 0 );
		GFL_BG_FillScreen( FLDBG_MFRM_EFF1,0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
		GFL_BG_LoadScreenReq( FLDBG_MFRM_EFF1 );
	}
#endif
#ifdef  PM_DEBUG
  //�t�B�[���h�f�o�b�O������
  fieldWork->debugWork = FIELD_DEBUG_Init( fieldWork, FLDBG_MFRM_EFF1, fieldWork->heapID );
#endif

#if USE_DEBUGWIN_SYSTEM
	DEBUGWIN_InitProc( FLDBG_MFRM_MSG , FLDMSGBG_GetFontHandle(fieldWork->fldMsgBG) );
	DEBUGWIN_ChangeLetterColor( 31,31,31 );
	FIELD_FUNC_RANDOM_GENERATE_InitDebug
		( fieldWork->heapID, fieldWork->gamedata );
#endif  //USE_DEBUGWIN_SYSTEM

#ifdef PM_DEBUG
  //�a�f�������I��
  DebugBGInitEnd = TRUE;
#endif
}
/*
 *  @brief  �t�B�[���hBG Vram�f�t�H���g������
 */
void FIELDMAP_InitBgNoTrans( FIELDMAP_WORK* fieldWork )
{
  /**
  //�A�t�B��������
  //@note 
  //�A�v���P�[�V�������ŁA�a�f�Q�ʂŃA�t�B�����g�p�����ꍇ�t�B�[���h�ɖ߂��Ă��Ă��p�����[�^���c���Ă���A
  //���������a�f�Q�ʂ��g�p����A�J�b�g�C����N���X�t�F�[�h�����ŕ\���̕s����������邽�߂����ŏ����������܂��B
  */
  {
    MtxFx22 mtx;
    GFL_CALC2D_GetAffineMtx22( &mtx, 0, FX32_ONE, FX32_ONE, GFL_CALC2D_AFFINE_MAX_NORMAL );
    G2_SetBG2Affine( &mtx, 0, 0, 0, 0 );
  }
	//�a�f�R���g���[���ݒ�
	GFL_BG_SetBGControl3D( FLDBG_MFRM_3D_PRI );

  // ��b�E�C���h�E���\�[�X�Z�b�g�A�b�v
	FLDMSGBG_SetupResourceNoTrans( fieldWork->fldMsgBG );
#if USE_DEBUGWIN_SYSTEM
	DEBUGWIN_InitProc( FLDBG_MFRM_MSG , FLDMSGBG_GetFontHandle(fieldWork->fldMsgBG) );
	DEBUGWIN_ChangeLetterColor( 31,31,31 );
	FIELD_FUNC_RANDOM_GENERATE_InitDebug
		( fieldWork->heapID, fieldWork->gamedata );
#endif  //USE_DEBUGWIN_SYSTEM

#ifdef PM_DEBUG
  //�a�f�������I��
  DebugBGInitEnd = TRUE;
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief  VBLANK���ԃ��W�X�^���f�Ǘ��V�X�e���̎擾
 */
//-----------------------------------------------------------------------------
FLD_VREQ *FIELDMAP_GetFldVReq(FIELDMAP_WORK *fieldWork)
{
  return fieldWork->fldVReq;
}

#ifdef PM_DEBUG
//--------------------------------------------------------------
/**
 * �f�o�b�O�֐��f�o�b�O���[�N������
 * @param fieldWork FIELDMAP_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELDMAP_InitDebugWork( FIELDMAP_WORK* fieldWork )
{
  //�t�B�[���h�f�o�b�O������
  fieldWork->debugWork = FIELD_DEBUG_Init( fieldWork, FLDBG_MFRM_EFF1, fieldWork->heapID );
}
#endif

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

  // tail�t���[�������s������FreeSeq�ɑJ�ڂ����ꍇ�A
  // �`��r���̃R�}���h��NNS�ɂ��܂��Ă���\������̂ŁA
  // �O�̂���FlushBuffer
  NNS_G3dGeFlushBuffer();

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
	fieldWork->mainBbdActSys = GFL_BBDACT_CreateSys(
			FIELD_G3D_BBDACT_RESMAX, FIELD_G3D_BBDACT_ACTMAX,
			fldmap_G3D_BBDTrans, fieldWork->heapID );
  {
    GFL_BBD_SYS * bbdsys = GFL_BBDACT_GetBBDSystem(fieldWork->mainBbdActSys);
    GFL_BBD_SetOrigin(bbdsys, GFL_BBD_ORIGIN_BOTTOM);
    GFL_BBD_SetFlipTexOffs( bbdsys, 0, FIELD_G3D_BBD_FLIP_TEX_OFFS_T );
  }
  
  fieldWork->subBbdActSys = GFL_BBDACT_CreateSys(
      FIELD_G3D_SUBBBDACT_RESMAX, FIELD_G3D_SUBBBDACT_ACTMAX,
      fldmap_G3D_BBDTrans, fieldWork->heapID );
  
  fieldWork->effBbdActSys = GFL_BBDACT_CreateSys(
      FIELD_G3D_EFFBBDACT_RESMAX, FIELD_G3D_EFFBBDACT_ACTMAX,
      fldmap_G3D_BBDTrans, fieldWork->heapID );
  {
    #define BBD_EFF_SCALE ((FX32_ONE+0xc00)*4)
    GFL_BBD_SYS *bbdSys = GFL_BBDACT_GetBBDSystem( fieldWork->effBbdActSys );
    VecFx32 scale = {BBD_EFF_SCALE,BBD_EFF_SCALE,BBD_EFF_SCALE};
    GFL_BBD_SetScale( bbdSys, &scale );
    { //Poligon ID
      u8 id = 0;
      GFL_BBD_SetPolID( bbdSys, &id );
    }
  }
  
  fieldmap_G3D_BBDSetColorParam( fieldWork ); // �F�ݒ�
  
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
  BOOL map_load_start;

#ifdef PM_DEBUG
  //���׃Z�b�g
  DEBUG_PerformanceSetStress();
#endif

	GFL_BBDACT_Main( fieldWork->mainBbdActSys );
  GFL_BBDACT_Main( fieldWork->subBbdActSys );
  GFL_BBDACT_Main( fieldWork->effBbdActSys );

  // NOGRID���쐧��
  FLDNOGRID_MAPPER_Main( fieldWork->nogridMapper );

  // scenearea
  FLD_SCENEAREA_Update( fieldWork->sceneArea, &fieldWork->now_pos );

  //ZONEFOGLIGHT
  FIELD_ZONEFOGLIGHT_Update( fieldWork->zonefog, fieldWork->heapID ); 
	
	FIELD_WEATHER_Main( fieldWork->weather_sys, HEAPID_WEATHER );
	FIELD_FOG_Main( fieldWork->fog );
	FIELD_LIGHT_Main( fieldWork->light, GFL_RTC_GetTimeBySecond() );

  MI_SetMainMemoryPriority(MI_PROCESSOR_ARM9); //500 micro sec ���x�팸
	map_load_start = FLDMAPPER_Main( fieldWork->g3Dmapper );
  MI_SetMainMemoryPriority(MI_PROCESSOR_ARM7);
  if (map_load_start)
  {
    GFL_NET_ChangeoverChangeSpeed(GFL_NET_CROSS_SPEED_PAUSE);  //����Ⴂ�ꎞ��~
    NOZOMU_Printf("�}�b�v���[�h�J�n\n");
  }
#ifdef PM_DEBUG
  if( FLDMAPPER_CheckTrans( fieldWork->g3Dmapper ) == FALSE )
  {
    //���ׂ��I�t
    DEBUG_PerformanceStressON(FALSE, STRESS_ID_MAP);
  }
  else{
    //���ׂ��I��
    DEBUG_PerformanceStressON(TRUE, STRESS_ID_MAP);
  }
#endif
}

//--------------------------------------------------------------
/**
 * G3D �`��
 * @param fieldWork FIELMAP_WORK
 * @retval nothing
 */
//--------------------------------------------------------------

// top�t���[���ł̕`��
// �RD�`����̏�����
// field_g3d_mapper��top�t���[���`��
static void fldmap_G3D_Draw_top( FIELDMAP_WORK * fieldWork )
{
#ifdef PM_DEBUG
  if( (GFL_UI_KEY_GetCont() & PAD_BUTTON_DEBUG) &&
        (GFL_UI_KEY_GetCont() & PAD_BUTTON_SELECT) ){
    return;
  }
#endif
  
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
    static const DRAW3DMODE_FUNC func[DRAW3DMODE_MAX] = {
      Draw3DNormalMode_top,
      NULL,
      NULL,
    };

    if(func[ fieldWork->Draw3DMode ]){
      func[ fieldWork->Draw3DMode ](fieldWork);
    }
  }
}

// tail�t���[���ł̕`��
// 3D�E2D�̕`�揈��
// �RD�`��I���i�X���b�v�o�b�t�@�j
static void fldmap_G3D_Draw_tail( FIELDMAP_WORK * fieldWork )
{
#ifdef PM_DEBUG
  if( (GFL_UI_KEY_GetCont() & PAD_BUTTON_DEBUG) &&
        (GFL_UI_KEY_GetCont() & PAD_BUTTON_SELECT) ){
    return;
  }
#endif
  
  {
    static const DRAW3DMODE_FUNC func[DRAW3DMODE_MAX] = {
      Draw3DNormalMode_tail,
      Draw3DCutinMode,
//      Draw3DScrnTexMode,
      DrawEncEff,
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
	GFL_BBDACT_DeleteSys( fieldWork->mainBbdActSys );
  GFL_BBDACT_DeleteSys( fieldWork->subBbdActSys );
  GFL_BBDACT_DeleteSys( fieldWork->effBbdActSys );
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
  
  // 
  // Snd�ǂݍ��݂��X���b�h����ɂ��邱�ƂŁA
  // Snd���\�[�X�ǂݍ��ݒ��ɁAmain�X���b�h�����삵�Ă���B
  //
  // SND�����ǂݍ��݁@�ƕ����VBlank���������삵�Ă���ꍇ�ɂ́A
  // ���������ӂ��\�������邽�߂RD�e�N�X�`���̓]�����s��Ȃ��B
  // 
  if( PMSND_IsAccessCARD() == FALSE ){
    if( fieldWork->fldMMdlSys != NULL ){
      MMDLSYS_VBlankProc( fieldWork->fldMMdlSys );
    }
  
    if( fieldWork->fieldG3dObjCtrl != NULL ){
      FLD_G3DOBJ_CTRL_Trans( fieldWork->fieldG3dObjCtrl );
    }
  }

	GFL_CLACT_SYS_VBlankFunc();	//�Z���A�N�^�[VBlank

  //VREQ
  if(fieldWork->fldVReq){
    FLD_VREQ_VBlank( fieldWork->fldVReq );
  }
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
 * @param zone_id
 */
//--------------------------------------------------------------
static void FIELD_EDGEMARK_Setup(const AREADATA * areadata, const u16 zone_id)
{
  static GXRgb edgeTable[8];
  int edgemark_type = AREADATA_GetEdgeMarkingType(areadata);

  if (edgemark_type == 0xff)
  {
    G3X_EdgeMarking(FALSE);
    return;
  }
  
  //�r���S�}�b�v���������t�B�[���h�̂Ƃ��̓G�b�W�}�[�L���O�����Ȃ�
  if ( ZONEDATA_IsPalaceFieldNoBingo( zone_id) ) G3X_EdgeMarking(FALSE);
  else G3X_EdgeMarking(TRUE);

  GFL_ARC_LoadData((void *)edgeTable, ARCID_FIELD_EDGEMARK, edgemark_type);
  //�v���O�����p�G�b�W�J���[�ݒ�i�C���f�b�N�X7�Ԃ��g�p���܂��j
  edgeTable[7] = GX_RGB(16,16,16);
  G3X_SetEdgeColorTable( edgeTable );
}

//----------------------------------------------------------------------------
/**
 *	@brief  BBD�ɃJ���[�p�����[�^��ݒ肷��B
 *
 *	@param	fieldWork   �t�B�[���h���[�N
 *	@param	heapID      �q�[�vID
 */
//-----------------------------------------------------------------------------
static void fieldmap_G3D_BBDSetColorParam( FIELDMAP_WORK * fieldWork )
{
  FLD_BBD_COLOR* p_color;
  HEAPID heapID = GFL_HEAP_LOWID(fieldWork->heapID);
  u32 bbd_color_idx = AREADATA_GetBBDColor( fieldWork->areadata );

  // �r���{�[�h�J���[�̐ݒ�
  p_color = FLD_BBD_COLOR_Create( heapID );
  FLD_BBD_COLOR_Load( p_color, bbd_color_idx );
  FLD_BBD_COLOR_SetData(
      p_color, GFL_BBDACT_GetBBDSystem(fieldWork->mainBbdActSys) );
  FLD_BBD_COLOR_Delete( p_color );
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
  
	MMDLSYS_SetupProc( fmmdlsys,	//���샂�f���V�X�e���@�Z�b�g�A�b�v
		fieldWork->heapID, gdata, fieldWork,
    fieldWork->g3Dmapper, fieldWork->nogridMapper );
	
	MMDL_BLACTCONT_Setup(		//���샂�f���r���{�[�h�@�Z�b�g�A�b�v
		fieldWork->fldMMdlSys, fieldWork->mainBbdActSys, 32 );

	//�r���{�[�h���\�[�X�o�^
  MMDL_BLACTCONT_AddResourceTex( fieldWork->fldMMdlSys );
  
#ifdef ADDFIX100703_REGU_TEX_HERO
  { //���@���\�[�X�o�^
    int sex = 0;
    u16 hero_code;
    u16 zone_id = fieldWork->location.zone_id;
    
    if( ZONEDATA_IsUnionRoom(zone_id) == FALSE &&
        ZONEDATA_IsColosseum(zone_id) == FALSE ){
      GAME_COMM_SYS_PTR game_comm =
        GAMESYSTEM_GetGameCommSysPtr( fieldWork->gsys );
      sex = IntrudeField_GetPlayerSettingSex( game_comm );
    }else{
      const MYSTATUS *myst = GAMEDATA_GetMyStatus( gdata );
      sex = MyStatus_GetMySex( myst );
    }
    
    hero_code = FIELD_PLAYER_GetMoveFormToOBJCode(
        sex, PLAYER_DRAW_FORM_NORMAL );
    MMDL_BLACTCONT_AddResourceTexCode( fmmdlsys, hero_code );
    KAGAYA_Printf( "FIELD INIT ADD HERO TEX sex=%d\n", sex );
  }
#endif
  
  MMDL_G3DOBJCONT_Setup( //���샂�f���I�u�W�F�N�g�@�Z�b�g�A�b�v
      fieldWork->fldMMdlSys, fieldWork->fieldG3dObjCtrl );

	//���샂�f���`��@�Z�b�g�A�b�v
  {
    const u16 *pAngleYaw =
      FIELD_CAMERA_GetAngleYawAddress( fieldWork->camera_control );
	  MMDLSYS_SetupDrawProc( fieldWork->fldMMdlSys, pAngleYaw );
	}

	//���샂�f���@���A
	MMDLSYS_Pop( fieldWork->fldMMdlSys );
  
  //�e�̐�������t���O
  MMDLSYS_SetJoinShadow( fmmdlsys, zonedata_GetShadowUse( fieldWork->map_id ) );
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
	u16 i = 1;
  u16 num = 0;
  u16 count;
	u16 *pList;
	pList = GFL_ARC_LoadDataAlloc( ARCID_MMDL_LIST, list_id, heapID );
	mlist->count = 0;

  //�w�b�_�����Q�o�C�g�Ƃ΂�(i=1����)
  count = pList[0];
	while( pList[i] != OBJCODEMAX ){
		mlist->id_list[num] = pList[i];
		i++;
    num++;
		GF_ASSERT( num < MMDL_LIST_MAX );
	}
	
	NOZOMU_Printf( "���f�����X�g���� %d\n", num );
	mlist->count = num;
	mlist->id_list[num] = OBJCODEMAX;

  GF_ASSERT_MSG(count == mlist->count,"ERROR:%d_%d",count,mlist->count);
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
  { //�G���J�E���g�f�[�^�̓]�[���`�F���W��ɒx���ǂݍ��݂��s��
    EVENTDATA_SYSTEM *evdata = GAMEDATA_GetEventData( fieldWork->gamedata );
    if ( EVENTDATA_SYS_IsEncountLoaded( evdata ) == FALSE )
    {
      MI_SetMainMemoryPriority(MI_PROCESSOR_ARM9);
      EVENTDATA_SYS_LoadEncountData(
          evdata, fieldWork->location.zone_id, GAMEDATA_GetSeasonID(fieldWork->gamedata) );
      MI_SetMainMemoryPriority(MI_PROCESSOR_ARM7);
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

  GFL_NET_ChangeoverChangeSpeed(GFL_NET_CROSS_SPEED_PAUSE);  //����Ⴂ�ꎞ��~

  INIT_CHECK();   //�f�o�b�O�F�������׌v���p

  //�G�t�F�N�g�G���J�E���g�j��
  EFFECT_ENC_EffectDelete( fieldWork->encount );

  LOCATION_Get3DPos( lc, &lc_pos );

  new_zone_id = MAP_MATRIX_GetVectorPosZoneID(
			mat, lc_pos.x, lc_pos.z );

	
	GF_ASSERT( new_zone_id != MAP_MATRIX_ZONE_ID_NON );

  TOMOYA_Printf( "zone change start %d\n", new_zone_id );

  SET_CHECK("zonechange:delete mmdl");
	
  MI_SetMainMemoryPriority(MI_PROCESSOR_ARM9); // 1000 micro sec���x�팸
	//���]�[���z�u���샂�f���폜 
	MMDLSYS_DeleteZoneUpdateMMdl( fmmdlsys );

  SET_CHECK("zonechange:event load");
	
	//���̃C�x���g�f�[�^�����[�h
  EVENTDATA_SYS_Clear( evdata );
	EVENTDATA_SYS_LoadEventData( evdata, new_zone_id, GAMEDATA_GetSeasonID(gdata) );
  MI_SetMainMemoryPriority(MI_PROCESSOR_ARM7);

  //�����ă]�[�����ύX�����ꍇ�̃t���O������
  FIELD_FLAGCONT_INIT_WalkStepOver( gdata, fieldWork );
	
  SET_CHECK("zonechange:bgm");
	//BGM�؂�ւ�
	zoneChange_SetBGM( fieldWork, new_zone_id );

  SET_CHECK("zonechange: weather");
	// ZONE�t�H�O���C�g�ݒ�
	zoneChange_SetZoneFogLight( fieldWork, new_zone_id );
	
	//�V�󃊃N�G�X�g
	zoneChange_SetWeather( fieldWork, new_zone_id );

  SET_CHECK("zonechange: camera area");
  //�J�����G���A�̐ݒ�
  zoneChange_SetCameraArea( fieldWork, new_zone_id );
	
  SET_CHECK("zonechange: scene area");
	//PLAYER_WORK�X�V
	zoneChange_UpdatePlayerWork( gdata, new_zone_id );

  //SCENEAREA�X�V 
  zoneChangeScene( fieldWork, new_zone_id );

	// �n���\���V�X�e����, �]�[���̐؂�ւ���ʒB
  if( fieldWork->placeNameSys &&
      (GAMESYSTEM_IsEventExists(fieldWork->gsys) == FALSE) ){ 
    FIELD_PLACE_NAME_DisplayOnStanderdRule( fieldWork->placeNameSys, new_zone_id ); 
  }

  SET_CHECK("zonechange: script");
	//�]�[��ID�X�V
	lc->zone_id = new_zone_id;
	GAMEBEACON_Set_ZoneChange(new_zone_id, gdata);
	
  //�}�b�v�����t���O�Z�b�g
  ARRIVEDATA_SetArriveFlag( gdata, new_zone_id );

  //����X�N���v�g�Ăяo���F�]�[���؂�ւ�
  SCRIPT_CallZoneChangeScript( fieldWork->gsys, HEAPID_PROC );

  SET_CHECK("zonechange:set mmdl");
	//�V�K�]�[���ɔz�u���铮�샂�f���Z�b�g 1200 micro sec���x�팸
  MI_SetMainMemoryPriority(MI_PROCESSOR_ARM9);
	zoneChange_SetMMdl( gdata, fmmdlsys, evdata, new_zone_id );
  MI_SetMainMemoryPriority(MI_PROCESSOR_ARM7);

  //�N���F�ʐM�v���C���[�X�V��~�t���O���Z�b�g
  Intrude_ResetStopCommPlayerUpdateMapFlag(GAMESYSTEM_GetGameCommSysPtr(fieldWork->gsys));
	
  SET_CHECK("zonechange:tail");

#ifdef DEBUG_FIELDMAP_ZONE_CHANGE_SYNC
  {
  OSTick debug_fieldmap_end_tick;
  TAIL_CHECK( &debug_fieldmap_end_tick );
  debug_fieldmap_end_tick = OS_TicksToMicroSeconds (debug_fieldmap_end_tick);
  if( debug_fieldmap_end_tick > 10000 )
  {
    OS_TPrintf( "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" );
    OS_TPrintf( "!!!!!!zone_change TickOver  [%d] micro second !!!!!!\n", debug_fieldmap_end_tick );
    OS_TPrintf( "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" );
    PUT_CHECK();
  }
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
	NOZOMU_Printf("COUNT : %d\n",count);
	if( count ){
    EVENTWORK *evwork =  GAMEDATA_GetEventWork( gdata );
		const MMDL_HEADER *header = EVENTDATA_GetNpcData( evdata );
		MMDLSYS_SetMMdl( fmmdlsys, header, zone_id, count, evwork );
	}
}

//--------------------------------------------------------------
/**
 * �]�[���؂�ւ����̏����@BGM�؂�ւ�
 * @param	fieldWork FIELDMAP_WORK*
 * @param	next_zone_id ���̃]�[��ID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void zoneChange_SetBGM( FIELDMAP_WORK* fieldWork, u32 next_zone_id )
{
  GAMEDATA*        gdata  = GAMESYSTEM_GetGameData( fieldWork->gsys );
  ISS_SYS*         iss    = GAMESYSTEM_GetIssSystem( fieldWork->gsys );
  FIELD_SOUND*     fsnd   = GAMEDATA_GetFieldSound( gdata );
  PLAYER_WORK*     player = GAMEDATA_GetMyPlayerWork( gdata );
  PLAYER_MOVE_FORM form   = PLAYERWORK_GetMoveForm( player );

  // ISS�V�X�e���Ƀ]�[���̐؂�ւ���ʒm
  ISS_SYS_ZoneChange( iss, next_zone_id );

  // BGM ��ύX
  if( form != PLAYER_MOVE_FORM_SWIM ) { // �Ȃ݂̂莞�͕ύX���Ȃ�
    FSND_ChangeBGM_byZoneChange( fsnd, gdata, next_zone_id );
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
	u16 w_no;
	FIELD_WEATHER *we = FIELDMAP_GetFieldWeather( fieldWork );

  PM_WEATHER_UpdateZoneChangeWeatherNo( fieldWork->gsys, zone_id );
  
  w_no = GAMEDATA_GetWeatherNo( fieldWork->gamedata );

	if( w_no != WEATHER_NO_NUM ){
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
	
	FIELD_ZONEFOGLIGHT_LoadReq( fieldWork->zonefog, ZONEDATA_GetFog(zone_id), ZONEDATA_GetLight(zone_id), 1 );  // ���ۂ̓ǂݍ��݂̓��C���t���[��
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

//----------------------------------------------------------------------------
/**
 *	@brief  wfbc�̃Z�b�g�A�b�v
 *
 *  @param  gdata
 *	@param	fieldWork
 *	@param	zone_id 
 */
//-----------------------------------------------------------------------------
static void setupWfbc( GAMEDATA* gdata, FIELDMAP_WORK *fieldWork, u32 zone_id )
{
  FIELD_WFBC_CORE* p_core;
  FIELD_WFBC* p_wfbc;
  EVENTDATA_SYSTEM* p_evdata;
  FIELD_STATUS* p_fs =  GAMEDATA_GetFieldStatus( gdata );
  MAPMODE mapmode = 0;
  
  if( ZONEDATA_IsWfbc( fieldWork->map_id ) )
  {
    
    // WFBC�X����ݒ�
    mapmode = FIELD_STATUS_GetMapMode( p_fs );
    if( mapmode == MAPMODE_NORMAL )
    {
      p_core = GAMEDATA_GetMyWFBCCoreData( fieldWork->gamedata );
    }
    else
    {
      // �ʐM����̏����擾
      p_core = GAMEDATA_GetWFBCCoreData( fieldWork->gamedata, GAMEDATA_WFBC_ID_COMM );
    }
    FLDMAPPER_SetWfbcData( fieldWork->g3Dmapper, p_core, mapmode );

    // 
    p_wfbc = FLDMAPPER_GetWfbcWork( fieldWork->g3Dmapper );
    p_evdata = GAMEDATA_GetEventData( gdata );

    // �C�x���g�̐ݒ�
    FILED_WFBC_EventDataOverwrite( p_wfbc, p_evdata, mapmode, fieldWork->heapID );
    
    // �J�����̃Z�b�g�A�b�v
    FIELD_WFBC_SetUpCamera( p_wfbc, fieldWork->camera_control, fieldWork->heapID );

    // MMDLSYS�̃Z�b�g�A�b�v
    FIELD_WFBC_SetUpMmdlSys( p_wfbc, GAMEDATA_GetMMdlSys( fieldWork->gamedata ) );
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  �V�[���G���A�̃Z�b�g�A�b�v
 *
 *	@param	fieldWork ���[�N
 *	@param	zone_id   �]�[��ID
 */
//-----------------------------------------------------------------------------
static void zoneChangeScene( FIELDMAP_WORK *fieldWork, u32 zone_id )
{
  u32 id = ZONEDATA_GetSceneAreaID( zone_id );
  TOMOYA_Printf( "zone_id %d   id %d\n", zone_id, id );
  if( id != ZONEDATA_NO_SCENEAREA_ID ) 
  {
    TOMOYA_Printf( "seanarea load\n" );
    // �܂��N���A
    FLD_SCENEAREA_Release( fieldWork->sceneArea );
    FLD_SCENEAREA_LOADER_Clear( fieldWork->sceneAreaLoader );
    
    // �V�[���G���A��ǂݍ���Őݒ�
    FLD_SCENEAREA_LOADER_Load( fieldWork->sceneAreaLoader, FLD_SCENEAREA_LOAD_GRID, id, HEAPID_FIELD_SCENE );


    FLD_SCENEAREA_Load( fieldWork->sceneArea, 
        FLD_SCENEAREA_LOADER_GetData(fieldWork->sceneAreaLoader),
        FLD_SCENEAREA_LOADER_GetDataNum(fieldWork->sceneAreaLoader),
        FLD_SCENEAREA_LOADER_GetFunc(fieldWork->sceneAreaLoader) );
  }
  else if( !ZONEDATA_IsRailMap( zone_id ) )
  {
    TOMOYA_Printf( "seanarea clear\n" );
    FLD_SCENEAREA_Release( fieldWork->sceneArea );
    FLD_SCENEAREA_LOADER_Clear( fieldWork->sceneAreaLoader );
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
    if(NetErr_App_CheckError()){  //�G���[���������Ă���ꍇ�͍Ō�ɋN�����Ă����ʐM�ԍ���Ԃ�
      return GameCommSys_GetLastCommNo(game_comm);
    }
    return comm_no;
  case GAME_COMM_NO_FIELD_BEACON_SEARCH:   //�t�B�[���h��Ńr�[�R���T�[�`
  case GAME_COMM_NO_INVASION:              //�N��
  case GAME_COMM_NO_DEBUG_SCANONLY:        //�f�o�b�O�p�X�L�����̂�
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
	FIELD_CLSYS_RESOUCE_MAX+8,  //2010.06.18 ����Ȃ��󋵂ɑΉ����邽��CGR�̂ݒǉ�
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
 * @note  �g�p�̍ۂ�gmk_tmp_wk.h���Q�Ƃ̂���
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
 * @note  �g�p�̍ۂ�gmk_tmp_wk.h���Q�Ƃ̂���
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
//�@top�t���[���ł̕`�揈��
static void Draw3DNormalMode_top( FIELDMAP_WORK * fieldWork )
{
  FIELD_FOG_Reflect( fieldWork->fog );
	FIELD_LIGHT_Reflect( fieldWork->light, FALSE );

	GFL_G3D_CAMERA_Switching( fieldWork->g3Dcamera );
	GFL_G3D_LIGHT_Switching( fieldWork->g3Dlightset );

	FLDMAPPER_Draw(
      fieldWork->g3Dmapper, fieldWork->g3Dcamera, FLDMAPPER_DRAW_TOP );
}

//�@tail�t���[���ł̕`�揈��
static void Draw3DNormalMode_tail( FIELDMAP_WORK * fieldWork )
{
  if( fieldWork->fldMsgBG ){
    FLDMSGBG_PrintG3D( fieldWork->fldMsgBG );
  }
  
  SET_CHECK("update_tail:mapper draw");
  FLDMAPPER_Draw(
      fieldWork->g3Dmapper, fieldWork->g3Dcamera, FLDMAPPER_DRAW_TAIL );
  
  SET_CHECK("update_tail:wipe draw");
	FIELDSKILL_MAPEFF_Draw( fieldWork->fieldskill_mapeff );
  
#ifdef PM_DEBUG
  // �f�o�b�N�J�����\�� (WIPE�p)
  if( FIELD_CAMERA_DEBUG_IsWipeDraw( fieldWork->camera_control ) ){
    FIELD_CAMERA_DEBUG_Draw( fieldWork->camera_control );
  }
#endif
  
  SET_CHECK("update_tail:proj calc");

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
*/				
		org_pm = *m;
		pm = org_pm;

		pm._32 += FX_Mul( pm._22, fieldWork->pro_mat_z_offs );
    
		NNS_G3dGlbSetProjectionMtx(&pm);
		NNS_G3dGlbFlush();		  //�@�W�I���g���R�}���h��]��
    NNS_G3dGeFlushBuffer(); // �]���܂�

#ifdef PM_DEBUG
    if( (GFL_UI_KEY_GetCont() & PAD_BUTTON_DEBUG) &&
        (GFL_UI_KEY_GetCont() & PAD_BUTTON_A) )
    {
    }
    else
#endif
    
    {
      SET_CHECK("update_tail:fldeff draw");
      FLDEFF_CTRL_Draw( fieldWork->fldeff_ctrl );
    
      SET_CHECK("update_tail:g3dobj draw");
      FLD_G3DOBJ_CTRL_Draw( fieldWork->fieldG3dObjCtrl );
      
      SET_CHECK("update_tail:bbd draw");
      GFL_BBDACT_Draw( fieldWork->mainBbdActSys,
          fieldWork->g3Dcamera, fieldWork->g3Dlightset );
      
      SET_CHECK("update_tail:eff bbd draw");
      GFL_BBDACT_Draw( fieldWork->effBbdActSys,
          fieldWork->g3Dcamera, fieldWork->g3Dlightset );
    }
    
#ifdef  PM_DEBUG
  if(fieldWork->debugWork){ FIELD_DEBUG_Draw( fieldWork->debugWork ); }
#endif
    
    if(fieldWork->union_eff != NULL){
      UNION_EFF_SystemDraw( fieldWork->union_eff);
    }
    
    SET_CHECK("update_tail:bbd draw 2");

		pm = org_pm;
		pm._32 += FX_Mul( pm._22, fieldWork->pro_mat_z_offs / 2 );
		NNS_G3dGlbSetProjectionMtx(&pm);
		NNS_G3dGlbFlush();		  //�@�W�I���g���R�}���h��]��
    NNS_G3dGeFlushBuffer(); // �]���܂�

    GFL_BBDACT_Draw( fieldWork->subBbdActSys,
        fieldWork->g3Dcamera, fieldWork->g3Dlightset );
    
		NNS_G3dGlbSetProjectionMtx(&org_pm);
		NNS_G3dGlbFlush();		//�@�W�I���g���R�}���h��]��
  }
  
  // �t�B�[���h�}�b�v�p����^�X�N�V�X�e��
  SET_CHECK("update_tail:FLDMAPFUNC_Sys_Draw3D");
	FLDMAPFUNC_Sys_Draw3D( fieldWork->fldmapFuncSys );
	
  SET_CHECK("update_tail:weather 3d");
  FIELD_WEATHER_3DWrite( fieldWork->weather_sys );	// �V�C�`�揈��

  SET_CHECK("update_tail: exp obj draw");
  //�t�B�[���h�g��3�c�n�a�i�`��
  FLD_EXP_OBJ_Draw( fieldWork->ExpObjCntPtr );
  
#ifdef PM_DEBUG
  if (GFL_UI_KEY_GetCont() & PAD_BUTTON_DEBUG){
#if 0    
    if (GFL_UI_KEY_GetTrg() & PAD_BUTTON_L){
      u32 rest;
      rest = 0;
      NNS_GfdDumpLnkTexVramManagerEx( DbgVramDumpCallBack, DbgVramDumpCallBack, &rest );
      NOZOMU_Printf( "3DVram rest = 0x%x\n",rest);

//      DEBUG_CreateCamShakeEvt(fieldWork->gsys, 0, 10, 3, 10, 0,1,0,5);
//      FLD3D_CI_CallCutIn(fieldWork->gsys, fieldWork->Fld3dCiPtr, 0);
    }
/**    
    else if (GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT){
      FLD3D_CI_CallEncCutIn(fieldWork->gsys, fieldWork->Fld3dCiPtr);
    }else if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_R){
      FLD3D_CI_FlySkyCameraDebug(
          fieldWork->gsys, fieldWork->Fld3dCiPtr, fieldWork->camera_control,
          FIELDMAP_GetFieldPlayer(fieldWork),
          FIELDMAP_GetFldNoGridMapper( fieldWork ) );
    }
*/    
#endif    
  }
#endif  //PM_DEBUG  
  
  SET_CHECK("update_tail:prt & ci draw");
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
#if 0
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
#endif
//==================================================================
/**
 * �G���J�E���g�G�t�F�N�g�`��
 *
 * @param   fieldWork   �t�B�[���h���[�N�|�C���^
 *
 * @return none
 */
//==================================================================
static void DrawEncEff(FIELDMAP_WORK * fieldWork)
{
  G3X_SetClearColor(GX_RGB(16,16,16),31,0x7fff,0,FALSE);

	ENCEFF_Draw(fieldWork->EncEffCntPtr);
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

//==================================================================
/**
 * �G���J�E���g�G�t�F�N�g�Ǘ��|�C���^�擾
 *
 * @param   fieldWork   �t�B�[���h���[�N�|�C���^
 *
 * @return  ENCEFF_CNT_PTR    �G���J�E���g�G�t�F�N�g�Ǘ��|�C���^
 */
//==================================================================
ENCEFF_CNT_PTR FIELDMAP_GetEncEffCntPtr(FIELDMAP_WORK *fieldWork)
{
  return fieldWork->EncEffCntPtr;
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

//----------------------------------------------------------------------------
/**
 *	@brief  �t�B�[���hscenearea�擾
 *
 *	@param	fieldWork   ���[�N
 */
//-----------------------------------------------------------------------------
FLD_SCENEAREA* FIELDMAP_GetFldSceneArea( FIELDMAP_WORK * fieldWork )
{
  return fieldWork->sceneArea;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �}�b�v�t�F�[�h���N�G�X�g�Z�b�g
 *
 *	@param	fieldWork   ���[�N
 *	@param  BOOL
 */
//-----------------------------------------------------------------------------
void FIELDMAP_SetMapFadeReqFlg( FIELDMAP_WORK * fieldWork, const BOOL inReq )
{
  fieldWork->MapFadeReq = inReq;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �}�b�v�t�F�[�h���N�G�X�g�`�F�b�N
 *
 *	@param	fieldWork   ���[�N
 *	@return  BOOL     TRUE�F���N�G�X�g��
 */
//-----------------------------------------------------------------------------
BOOL FIELDMAP_CheckMapFadeReqFlg( FIELDMAP_WORK * fieldWork  )
{
  return fieldWork->MapFadeReq;
}



//----------------------------------------------------------------------------
/**
 *	@brief  SOUND�ABGM�̓ǂݍ��ݒ��`�F�b�N   BGM�̍Đ���SE�̍Đ����o���܂���B
 *
 *	@param	fieldWork 
 *
 *	@retval TRUE    �ǂݍ��ݒ�
 *	@retval FALSE   �ǂݍ���łȂ�
 */
//-----------------------------------------------------------------------------
BOOL FIELDMAP_CheckCanSoundPlay( const FIELDMAP_WORK* fieldWork )
{
  return PMSND_IsLoading();
}

//----------------------------------------------------------------------------
/**
 *	@brief  �C�x���g���쒆���`�F�b�N
 *
 *	@param	fieldWork   ���[�N
 *
 *	@retval TRUE    ���쒆
 *	@retval FALSE   ���삵�ĂȂ��ifieldmap_ctrl�̍X�V���s���Ă���j
 */
//-----------------------------------------------------------------------------
BOOL FIELDMAP_CheckDoEvent( const FIELDMAP_WORK* fieldWork )
{
  return GAMESYSTEM_IsEventExists(fieldWork->gsys);
}

//----------------------------------------------------------------------------
/**
 *	@brief  �G�߂��Ƃ̎��ԑт��擾
 *
 *	@param	fieldWork     �t�B�[���h���[�N
 *
 *	@return ���ԑ�
 */
//-----------------------------------------------------------------------------
u32 FIELDMAP_GetSeasonTimeZone( const FIELDMAP_WORK * fieldWork )
{
  return PM_RTC_GetTimeZone( GAMEDATA_GetSeasonID(fieldWork->gamedata) );
}

//-----------------------------------------------------------------------------
/**
 * @brief �G�߂�\�������ǂ������擾����
 *
 * @param fieldWork
 *
 * @return �G�߂�\�����Ȃ� TRUE
 *         �����łȂ���� FALSE
 */
//-----------------------------------------------------------------------------
BOOL FIELDMAP_CheckSeasonDispFlag( const FIELDMAP_WORK * fieldWork )
{
  return fieldWork->seasonDispFlag;
}

//-----------------------------------------------------------------------------
/**
 * @brief �G�ߕ\�����t���O��ݒ肷��
 *
 * @param fieldWork
 * @param flag
 */
//-----------------------------------------------------------------------------
void FIELDMAP_SetSeasonDispFlag( FIELDMAP_WORK * fieldWork, BOOL flag )
{
  fieldWork->seasonDispFlag = flag;
}


//==================================================================
/**
 * �X�V�����t�b�N�t���O�Z�b�g�֐�
 *
 * @param   fieldWork   �t�B�[���h���[�N�|�C���^
 * @param   inFlg       �t�b�N�t���O�@TRUE�ŏ������t�b�N
 *
 * @return none
 */
//==================================================================
void FIELDMAP_SetMainFuncHookFlg(FIELDMAP_WORK * fieldWork, const BOOL inFlg)
{
  fieldWork->MainHookFlg = inFlg;
}

//==================================================================
/**
 * ��A�b�v���[�N�|�C���^�ւ̃|�C���^�擾
 *
 * @param   fieldWork   �t�B�[���h���[�N�|�C���^
 *
 * @return  ENCEFF_CNT_PTR    �G���J�E���g�G�t�F�N�g�Ǘ��|�C���^
 */
//==================================================================
FACEUP_WK_PTR *FIELDMAP_GetFaceupWkPtrAdr(FIELDMAP_WORK *fieldWork)
{
  return &fieldWork->FaceUpWkPtr;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �Q�W���[�N�̐ݒ�
 *
 *	@param	fieldWork   �t�B�[���h���[�N
 *	@param	crowdpeople �Q�W���[�N
 */
//-----------------------------------------------------------------------------
void FIELDMAP_SetCrowdPeopleSys( FIELDMAP_WORK * fieldWork, FIELD_CROWD_PEOPLE * crowdpeople )
{
  fieldWork->crowdpeople = crowdpeople;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �Q�W���[�N�̎擾
 *
 *	@param	fieldWork   �t�B�[���h���[�N
 *
 *	@return �Q�W���[�N
 */
//-----------------------------------------------------------------------------
FIELD_CROWD_PEOPLE * FIELDMAP_GetCrowdPeopleSys( const FIELDMAP_WORK * fieldWork )
{
  return fieldWork->crowdpeople;
}


//==================================================================
/**
 * @brief ���샂�f���`�掞��Y�����I�t�Z�b�g�擾
 */
//==================================================================
fx32 FIELDMAP_GetBbdActYOffs( const FIELDMAP_WORK * fieldmap )
{
  return fieldmap->bbdact_y_offs;
}

//==================================================================
/**
 * @brief ���샂�f���`�掞��Y�����I�t�Z�b�g�v�Z
 */
//==================================================================
static fx32 calcBbdActYOffset( const FIELDMAP_WORK * fieldmap )
{
  u16 angle;
  fx32 offs;
  angle = *FIELD_CAMERA_GetAnglePitchAddress( fieldmap->camera_control );
  offs = MMDL_BBD_OFFS_POS_Y + FX_Mul( MMDL_BBD_OFFS_Y_VALUE, MATH_ABS(FX_CosIdx(angle)) );
  if ( offs < FX32_CONST(-2) ) {
    offs = FX32_CONST(-2);
  }
  return offs;
}

//==================================================================
//==================================================================
#ifdef PM_DEBUG
static void DbgVramDumpCallBack( u32 addr, u32 szByte, void* pUserData )
{
    // ���v�T�C�Y���v�Z�B
    (*((u32*)pUserData)) += szByte;
}

#endif


//==================================================================
/**
 * @brief �]�[�����Ƃ̎ˉe�␳�l�����o��
 * @date  2010.05.28
 * @author  tamada GAMEFREAK inc.
 *
 * ���]�Ԃ߂荞�ݖ��ւ̑Ώ��Ƃ��āA�l���r���{�[�h�Ȃǂ̎ˉe������
 * �����Ȃ������ʁA�����̕\���ɖ�肪�o�����߁A���܂ňꗥ�ɓK�p���Ă���
 * �ˉe�̒l���ꏊ���ƂɉςƂ����B
 * �{���͊O���������l���擾����d�g�݂����ׂ������A�����������Ȃ̂�
 * ���ڂ����ɍ쐬����B
 */
//==================================================================
static fx32 fldmap_getProjMatZOffsValue( u16 zone_id )
{
  if ( ZONEDATA_GetProjectionType( zone_id ) == TRUE )
  { //�ʏ�̎ˉe�I�t�Z�b�g�i2010.05.28�V�K�j
    return PROJ_MAT_Z_OFS;
  }
  else
  { //�w�肵���]�[���ɂ���Ă͈ȑO�̎ˉe�I�t�Z�b�g
    return PROJ_MAT_Z_OFS_DESERT;
  }
}

//==================================================================
/**
 * @brief �e���������ǂ����̔��菈��
 * @date  2010.06.15
 * @author  tamada
 * �{����zonedata.c�ɊO���������l���擾����d�g�݂����ׂ������A
 * �����������ł��邱�ƁA�풓���������Ȃ�ׂ���������Ȃ����ƁA
 * �̂Q�_���l�����Ē��ڂ����ɍ쐬����B
 */
//==================================================================
static BOOL zonedata_GetShadowUse( u16 zone_id )
{
  if ( zone_id == ZONE_ID_C04GYM0101 ) return FALSE;
  if ( ZONEDATA_IsColosseum(zone_id) == TRUE) return FALSE;
  return TRUE;
}

//==================================================================
/**
 * @brief ����SE���g�p���Ă��邩�ǂ����̔��菈��
 * @date  2010.06.20
 * @author  tamada
 * @return  BOOL FIELD_INIT�̒��Ń��[�vSE��炵�Ă���ӏ��̏ꍇ�ATRUE�ƂȂ�
 */
//==================================================================
static BOOL zonedata_IsEnvSE( u16 zone_id )
{
  if ( zone_id == ZONE_ID_C04GYM0101 ) return TRUE;
  if ( zone_id == ZONE_ID_C09R0101 ) return TRUE;
  if ( zone_id == ZONE_ID_C09R0201 ) return TRUE;
  if ( zone_id == ZONE_ID_C09R0301 ) return TRUE;
  return FALSE;
}

