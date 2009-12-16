//======================================================================
/**
 * @file	fld3d_ci.c
 * @brief	�t�B�[���h3D�J�b�g�C��
 * @author	Saito
 *
 */
//======================================================================
#include "fld3d_ci.h"
#include "arc_def.h"
#include "gamesystem/game_event.h"
#include "field/field_msgbg.h"    //for FLDMSGBG_RecoveryBG
#include "system/pokegra.h"

#include "../../../resource/fld3d_ci/fldci_id_def.h"

#include "sound/pm_wb_voice.h"   //for PMV_PlayVoice

#define EMITCOUNT_MAX  (2)
#define OBJCOUNT_MAX  (2)
#define ANM_TYPE_MAX  (4)
#define RESCOUNT_MAX  (8)

#define UNIT_NONE   (0xffffffff)

#define NONDATA  (0xffff)

#define	PRO_MAT_Z_OFS	(20*FX32_ONE)   //�v���W�F�N�V�����}�g���b�N�X�ɂ��f�v�X�o�b�t�@����l

#define TEXSIZ_S  (128)
#define TEXSIZ_T  (128)
#define TEXVRAMSIZ		(TEXSIZ_S/8 * TEXSIZ_T/8 * 0x20)	// chrNum_x * chrNum_y * chrSiz


#define POKE_VOICE_WAIT (5)

#ifdef PM_DEBUG

#include "fieldmap.h" //for FIELDMAP_�`
#include "event_mapchange.h"

#endif  //PM_DEBUG

typedef void (*SETUP_CALLBACK)(FLD3D_CI_PTR ptr);


typedef struct FLD3D_CI_tag
{
  FLD_PRTCL_SYS_PTR PrtclSys;
  GFL_G3D_CAMERA *g3Dcamera;
  HEAPID HeapID;
  GFL_G3D_UTIL* Util;
  int UnitIdx;
  GFL_G3D_OBJSTATUS Status;

  GFL_PTC_PTR PrtclPtr;

  u16 PrtclWait[EMITCOUNT_MAX];
  u16 MdlAnm1Wait;
  u16 MdlAnm2Wait;
  u16 PartGene;
  u16 CutInNo;

  GXPlaneMask Mask;
  u8 BgPriority[4];
  BOOL CapEndFlg;
  BOOL Emit1;
  BOOL Emit2;
  BOOL Anm1Flg;
  BOOL Anm2Flg;

  //�|�P�����W�J�p
  NNSG2dCharacterData * chr;
  NNSG2dPaletteData* plt;
  GFL_BMP_DATA* pokeImgBitmap;
  void *chr_buf;
  void *pal_buf;
  int MonsNo;
  int FormNo;
  int Sex;
  int Rare;
  BOOL Egg;

  int SePlayWait;

  SETUP_CALLBACK SetupCallBack;
}FLD3D_CI;

//�o�C�i���f�[�^�t�H�[�}�b�g
typedef struct {
  u32 SpaIdx;
  u16 SpaWait[EMITCOUNT_MAX];
  u16 MdlObjIdx[OBJCOUNT_MAX]; //0�F�`�@1�F�a
  u16 MdlAAnmWait;
  u16 MdlBAnmWait;
  u16 MdlAAnmIdx[ANM_TYPE_MAX];  //4�ڂ̓f�[�^�Ȃ�
  u16 MdlBAnmIdx[ANM_TYPE_MAX];  //4�ڂ̓f�[�^�Ȃ�
}RES_DEF_DAT;

typedef struct {
  GFL_G3D_UTIL_RES *Res;
  GFL_G3D_UTIL_OBJ *Obj;
  GFL_G3D_UTIL_ANM *Anm1;
  GFL_G3D_UTIL_ANM *Anm2;
  GFL_G3D_UTIL_SETUP Setup;
  u16 SpaWait[EMITCOUNT_MAX];
  u16 MdlAAnmWait;
  u16 MdlBAnmWait;
  u32 SpaIdx;
  u8 ResNum;
  u8 ObjNum;
  u8 Anm1Num;
  u8 Anm2Num;
}RES_SETUP_DAT;

typedef struct {
  GAMESYS_WORK * gsys;
  FLD3D_CI_PTR CiPtr;
  RES_SETUP_DAT SetupDat;
}FLD3D_CI_EVENT_WORK;

#ifdef PM_DEBUG
typedef struct {
  GAMESYS_WORK *gsys;
  FLD3D_CI_PTR CiPtr;
  FIELD_CAMERA * camera;
  FIELD_PLAYER * player;
  FLDNOGRID_MAPPER* mapper;
  const VecFx32 *Watch;
}FLYSKY_EFF_WORK;
#endif

static void SetupResource(FLD3D_CI_PTR ptr, RES_SETUP_DAT *outDat, const u8 inCutInNo);
static void SetupResourceCore(FLD3D_CI_PTR ptr, const GFL_G3D_UTIL_SETUP *inSetup);
static void DeleteResource(FLD3D_CI_PTR ptr, RES_SETUP_DAT *ioDat);
static BOOL PlayParticle(FLD3D_CI_PTR ptr);
static BOOL PlayMdlAnm1(FLD3D_CI_PTR ptr);
static BOOL PlayMdlAnm2(FLD3D_CI_PTR ptr);

static GMEVENT_RESULT CutInEvt( GMEVENT* event, int* seq, void* work );


static void ParticleCallBack(GFL_EMIT_PTR emit);
static void Generate(FLD3D_CI_PTR ptr, const u32 inResNo);

static void CreateRes(RES_SETUP_DAT *outDat, const u8 inResArcIdx, const HEAPID inHeapID);
static void DeleteRes(RES_SETUP_DAT *ioDat);

static void PushPriority(FLD3D_CI_PTR ptr);
static void PushDisp(FLD3D_CI_PTR ptr);
static void PopPriority(FLD3D_CI_PTR ptr);
static void PopDisp(FLD3D_CI_PTR ptr);
static void ReqCapture(FLD3D_CI_PTR ptr);

static void Graphic_Tcb_Capture( GFL_TCB *p_tcb, void *p_work );

#ifdef PM_DEBUG
static GMEVENT_RESULT DebugFlySkyEffEvt( GMEVENT* event, int* seq, void* work );
#endif  //PM_DEBUG

static void ReTransToPokeGra(FLD3D_CI_PTR ptr);

//--------------------------------------------------------------------------------------------
/**
 * ���W���[��������
 *
 * @param   inHeapID      �q�[�v�h�c
 * @param   inPrtclSysptr �p�[�e�B�N���V�X�e���|�C���^
 *
 * @return	ptr         ���W���[���|�C���^
 */
//--------------------------------------------------------------------------------------------
FLD3D_CI_PTR FLD3D_CI_Init(const HEAPID inHeapID, FLD_PRTCL_SYS_PTR inPrtclSysPtr)
{
  FLD3D_CI_PTR ptr;
  
  GF_ASSERT(inPrtclSysPtr != NULL);

  ptr = GFL_HEAP_AllocClearMemory(inHeapID, sizeof(FLD3D_CI));
  ptr->Util = GFL_G3D_UTIL_Create( RESCOUNT_MAX, OBJCOUNT_MAX, inHeapID );
  ptr->HeapID = inHeapID;
  ptr->PrtclSys = inPrtclSysPtr;
  
  ptr->UnitIdx = UNIT_NONE;

  {
    VecFx32 pos = {0, 0, FX32_ONE*(128)};
    VecFx32 target = {0,0,0};
    //�J�����쐬
    ptr->g3Dcamera = GFL_G3D_CAMERA_CreateDefault(
        &pos, &target, inHeapID );
  }
  return ptr;
}

//--------------------------------------------------------------------------------------------
/**
 * ���W���[���j��
 *
 * @param   ptr     ���W���[���|�C���^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FLD3D_CI_End(FLD3D_CI_PTR ptr)
{
  u16 i;
/**  
  //����Y�ꂪ���邩������Ȃ��̂ŁA���j�b�g�p�������Ă���
  for (i=0;i<FLD_EXP_OBJ_UNIT_MAX;i++){
    if ( ptr->Unit[i].Valid){
      FLD_EXP_OBJ_DelUnit( ptr, i );
    }
  }
*/
  //�J�������
  GFL_G3D_CAMERA_Delete(ptr->g3Dcamera);
  GFL_G3D_UTIL_Delete( ptr->Util );
  GFL_HEAP_FreeMemory( ptr );
}

//--------------------------------------------------------------------------------------------
/**
 * ���\�[�X�Z�b�g�A�b�v
 *
 * @param   ptr       �J�b�g�C���Ǘ��|�C���^
 * @param   inSetup   �Z�b�g�A�b�v�f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void SetupResourceCore(FLD3D_CI_PTR ptr, const GFL_G3D_UTIL_SETUP *inSetup)
{
  u16 i;
  u16 unitIdx;
  GFL_G3D_OBJSTATUS *status;
  u16 obj_num;

  const GFL_G3D_UTIL_SETUP *setup;
  
  setup = inSetup;

  unitIdx = GFL_G3D_UTIL_AddUnit( ptr->Util, setup );
  NOZOMU_Printf("unit_idx=%d\n",unitIdx);
  obj_num = setup->objCount;
  if (obj_num == 0){
    GF_ASSERT_MSG(0,"OBJ NOTHING\n");
  }

  ptr->UnitIdx = unitIdx;

  {
    VEC_Set( &ptr->Status.scale, FX32_ONE, FX32_ONE, FX32_ONE );
	  MTX_Identity33( &ptr->Status.rotate );
    VEC_Set( &ptr->Status.trans, 0, 0, 0 );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �`��
 *
 * @param   ptr     �J�b�g�C���Ǘ��|�C���^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FLD3D_CI_Draw( FLD3D_CI_PTR ptr )
{
  u8 i,j;
  
  GFL_G3D_CAMERA_Switching( ptr->g3Dcamera );
  {
    fx32 ofs;
	  MtxFx44 org_pm,pm;
		const MtxFx44 *m;
		m = NNS_G3dGlbGetProjectionMtx();
		org_pm = *m;
		pm = org_pm;
		pm._32 += FX_Mul( pm._22, PRO_MAT_Z_OFS );
		NNS_G3dGlbSetProjectionMtx(&pm);
		NNS_G3dGlbFlush();		  //�@�W�I���g���R�}���h��]��
    NNS_G3dGeFlushBuffer(); // �]���܂�

    //���j�b�g�������[�v
    if (ptr->UnitIdx != UNIT_NONE){
      u16 obj_idx = GFL_G3D_UTIL_GetUnitObjIdx( ptr->Util, ptr->UnitIdx );
      u16 obj_count = GFL_G3D_UTIL_GetUnitObjCount( ptr->Util, ptr->UnitIdx );
      for (j=0;j<obj_count;j++){
        GFL_G3D_OBJ* pObj;
        pObj = GFL_G3D_UTIL_GetObjHandle(ptr->Util, obj_idx+j);
        GFL_G3D_DRAW_DrawObject( pObj, &ptr->Status );
      }
    }
    NNS_G3dGlbSetProjectionMtx(&org_pm);
		NNS_G3dGlbFlush();		//�@�W�I���g���R�}���h��]��
  }
}

#ifdef PM_DEBUG
//�J�b�g�C���Ăяo��
void FLD3D_CI_CallCutIn( GAMESYS_WORK *gsys, FLD3D_CI_PTR ptr, const u8 inCutInNo )
{
  GMEVENT * event;
  event = FLD3D_CI_CreateCutInEvt(gsys, ptr, inCutInNo);
  GAMESYSTEM_SetEvent(gsys, event);
}

//�|�P�����J�b�g�C���Ăяo��
void FLD3D_CI_CallPokeCutIn( GAMESYS_WORK *gsys, FLD3D_CI_PTR ptr )
{
  GMEVENT * event;
  int no = FLDCIID_POKE;
  event = FLD3D_CI_CreateCutInEvt(gsys, ptr, no);
  //�Z�b�g�A�b�v��R�[���o�b�N�ݒ�
  ptr->SetupCallBack = ReTransToPokeGra;
  //�|�P�����J�b�g�C���p�|�P�����w��ϐ��Z�b�g
  ptr->MonsNo = 3;
  ptr->FormNo = 0;
  ptr->Sex = 0;
  ptr->Rare = 0;
  ptr->SePlayWait = POKE_VOICE_WAIT;

  GAMESYSTEM_SetEvent(gsys, event);
}
#endif

//--------------------------------------------------------------------------------------------
/**
 * �J�b�g�C���C�x���g�쐬
 *
 * @param   gsys        �Q�[���V�X�e���|�C���^
 * @param   ptr         �J�b�g�C���Ǘ��|�C���^
 * @param   inCutInNo   �J�b�g�C���i���o�[
 *
 * @return	event       �C�x���g�|�C���^
 */
//--------------------------------------------------------------------------------------------
GMEVENT *FLD3D_CI_CreateCutInEvt(GAMESYS_WORK *gsys, FLD3D_CI_PTR ptr, const u8 inCutInNo)
{
  GMEVENT * event;
  FLD3D_CI_EVENT_WORK *work;
  int size;

  size = sizeof(FLD3D_CI_EVENT_WORK);
  ptr->PartGene = 0;
  ptr->CutInNo = inCutInNo;
  //�Z�b�g�A�b�v��R�[���o�b�N�Ȃ��ŃZ�b�g����
  ptr->SetupCallBack = NULL;
  //�r�d�����ŃZ�b�g
  ptr->SePlayWait = 0;
  //�C�x���g�쐬
  {
    event = GMEVENT_Create( gsys, NULL, CutInEvt, size );

    work = GMEVENT_GetEventWork(event);
    MI_CpuClear8( work, size );
    work->gsys = gsys;
    work->CiPtr = ptr;
  }
  return event;
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����J�b�g�C���C�x���g�쐬
 *
 * @param   gsys        �Q�[���V�X�e���|�C���^
 * @param   ptr         �J�b�g�C���Ǘ��|�C���^
 * @apram   inMonsNo    �����X�^�[�i���o�[
 * @param   inFormNo    �t�H�����i���o�[
 * @param   inSex       ����
 * @param   inRare      ���A���H
 * @param   inEgg       ����
 *
 * @return	event       �C�x���g�|�C���^
 */
//--------------------------------------------------------------------------------------------
GMEVENT *FLD3D_CI_CreatePokeCutInEvt( GAMESYS_WORK *gsys, FLD3D_CI_PTR ptr,
                                      const int inMonsNo, const int inFormNo,
                                      const int inSex, const int inRare, const BOOL inEgg )
{
  GMEVENT * event;
  int no = FLDCIID_POKE;
  event = FLD3D_CI_CreateCutInEvt(gsys, ptr, no);
  //�Z�b�g�A�b�v��R�[���o�b�N�ݒ�
  ptr->SetupCallBack = ReTransToPokeGra;
  //�|�P�����J�b�g�C���p�|�P�����w��ϐ��Z�b�g
  ptr->MonsNo = inMonsNo;
  ptr->FormNo = inFormNo;
  ptr->Sex = inSex;
  ptr->Rare = inRare;
  ptr->Egg  = inEgg;
  ptr->SePlayWait = POKE_VOICE_WAIT;

  return event;
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����J�b�g�C���C�x���g�쐬(�莝��POKEPARTY����)
 *
 * @param   gsys        �Q�[���V�X�e���|�C���^
 * @param   ptr         �J�b�g�C���Ǘ��|�C���^
 * @param   pos         �J�b�g�C���̃^�[�Q�b�g�ɂ���莝���|�P����index
 *
 * @return	event       �C�x���g�|�C���^
 */
//--------------------------------------------------------------------------------------------
GMEVENT *FLD3D_CI_CreatePokeCutInEvtTemoti( GAMESYS_WORK *gsys, FLD3D_CI_PTR ptr, u8 pos )
{
  GMEVENT * event;
  GAMEDATA *gdata =  GAMESYSTEM_GetGameData( gsys );

  {
    POKEPARTY*     party = GAMEDATA_GetMyPokemon( gdata );
    POKEMON_PARAM* pp = PokeParty_GetMemberPointer( party, pos );

    int monsno = PP_Get( pp, ID_PARA_monsno, NULL );
    int formno = PP_Get( pp, ID_PARA_form_no, NULL );
    int sex = PP_Get( pp, ID_PARA_sex, NULL );
    int rare = PP_CheckRare( pp );
    int egg = PP_Get( pp, ID_PARA_tamago_flag, NULL );

    event = FLD3D_CI_CreatePokeCutInEvt(gsys, ptr, monsno,formno,sex,rare,egg);
  }
  return event;
}

//--------------------------------------------------------------------------------------------
/**
 * �J�b�g�C���C�x���g
 *
 * @param   event       �C�x���g�|�C���^
 * @param   *seq        �V�[�P���T
 * @param   work        ���[�N�|�C���^
 *
 * @return	GMEVENT_RESULT    �C�x���g����
 */
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT CutInEvt( GMEVENT* event, int* seq, void* work )
{
  FLD3D_CI_PTR ptr;
  FLD3D_CI_EVENT_WORK *evt_work = work;
  GAMESYS_WORK * gsys;
  FIELDMAP_WORK * fieldmap;
  
  ptr = evt_work->CiPtr;
  gsys = GMEVENT_GetGameSysWork(event);
  fieldmap = GAMESYSTEM_GetFieldMapWork(gsys);

  switch(*seq){
  case 0:
    //�n�a�i�̃|�[�Y
    {
      MMDLSYS *mmdlsys = FIELDMAP_GetMMdlSys( fieldmap );
      MMDLSYS_PauseMoveProc(mmdlsys);
    }
    //�v���C�I���e�B�̕ۑ�
    PushPriority(ptr);
    //�\����Ԃ̕ۑ�
    PushDisp(ptr);
    //�J�b�g�C�����ʃz���C�g�A�E�g����
    (*seq)++;
    break;
  case 1:
    //�z���C�g�A�E�g�҂�
    if (1){
      //�L���v�`�����N�G�X�g
      ReqCapture(ptr);
      (*seq)++;
    }
    break;
  case 2:
    //�L���v�`���I���҂�
    if (ptr->CapEndFlg){
      //�L���v�`���I�������A�v���C�I���e�B�ύX�ƕ\���ύX
      GFL_BG_SetPriority( 0, 0 );
      GFL_BG_SetPriority( 2, 3 );
      //�`�惂�[�h�ύX
      {
        const GFL_BG_SYS_HEADER bg_sys_header = 
          {
            GX_DISPMODE_GRAPHICS,GX_BGMODE_5,GX_BGMODE_0,GX_BG0_AS_3D
          };
        GFL_BG_SetBGMode( &bg_sys_header );
      }
      //�a�f�Z�b�g�A�b�v
      G2_SetBG2ControlDCBmp(
          GX_BG_SCRSIZE_DCBMP_256x256,
          GX_BG_AREAOVER_XLU,
          GX_BG_BMPSCRBASE_0x00000
      );
      GX_SetBankForBG(GX_VRAM_BG_128_D);
      //�A���t�@�u�����h
      G2_SetBlendAlpha(GX_BLEND_PLANEMASK_NONE,
				GX_BLEND_PLANEMASK_BG2, 0,0);
      //�����ł͂܂��L���v�`���ʂ̂ݕ\��(�p�V���h�~)
      GFL_BG_SetVisible( GFL_BG_FRAME2_M, VISIBLE_ON );
      (*seq)++;
    }
    break;
  case 3:
    //���\�[�X���[�h
    SetupResource(ptr, &evt_work->SetupDat, ptr->CutInNo);
    //�Z�b�g�A�b�v�I�������̂�3�c�ʂ��I��
    GFL_BG_SetVisible( GFL_BG_FRAME2_M, VISIBLE_ON );
    GFL_BG_SetVisible( GFL_BG_FRAME0_M, VISIBLE_ON );
    {
      int size = GFL_HEAP_GetHeapFreeSize(ptr->HeapID);
      NOZOMU_Printf("START::FLD3DCUTIN_HEAP_REST %x\n",size);
    }
    //�t�B�[���h�\�����[�h�ؑ�
    FIELDMAP_SetDraw3DMode(fieldmap, DRAW3DMODE_CUTIN);
    (*seq)++;
    break;
  case 4:
    //1�V���N�҂����̂ŁA�|�P�����O���t�B�b�N��W�J���Ă����ꍇ�͉������
    if (ptr->pokeImgBitmap != NULL)
    {
      GFL_BMP_Delete(ptr->pokeImgBitmap);
      ptr->pokeImgBitmap = NULL;
      GFL_HEAP_FreeMemory( ptr->chr_buf );
      GFL_HEAP_FreeMemory( ptr->pal_buf );
    }
    //��ʕ��A
    if (1){
      (*seq)++;
    }
    break;
  case 5:
    {
      BOOL rc1,rc2,rc3;
      //�p�[�e�B�N���Đ�
      rc1 = PlayParticle(ptr);
      //3�c���f��1�A�j���Đ�
      rc2 = PlayMdlAnm1(ptr);
      //3�c���f��2�A�j���Đ�
      rc3 = PlayMdlAnm2(ptr);

      //�����Đ�    @todo
      {
        if (ptr->SePlayWait)
        {
          ptr->SePlayWait--;
          if ( ptr->SePlayWait == 0 )
          {
            PMV_PlayVoice( ptr->MonsNo, ptr->FormNo );
          }
        }
      }

      if (rc1&&rc2&&rc3){
        PMV_StopVoice();    //�����Ȃ��Ă���Ȃ�X�g�b�v  @todo
        (*seq)++;
      }
    }
    break;
  case 6:
    //�t�B�[���h���[�h�߂�
    FIELDMAP_SetDraw3DMode(fieldmap, DRAW3DMODE_NORMAL);
    //���\�[�X�������
    DeleteResource(ptr, &evt_work->SetupDat);
    //3D�ʂ��I�t(�L���v�`���ʂ������Ă���)
    GFL_BG_SetVisible( GFL_BG_FRAME0_M, VISIBLE_OFF );
    G3X_SetClearColor(GX_RGB(0,0,0),31,0x7fff,0,FALSE);
    (*seq)++;
    break;
  case 7:
    //�o�b�N�J���[�ݒ�
    {
      FIELD_LIGHT *light = FIELDMAP_GetFieldLight( fieldmap );
      FIELD_LIGHT_Reflect( light, TRUE );
    }
    //3D�ʂ��I��
    GFL_BG_SetVisible( GFL_BG_FRAME0_M, VISIBLE_ON );

    {
      int size = GFL_HEAP_GetHeapFreeSize(ptr->HeapID);
      NOZOMU_Printf("END::FLD3DCUTIN_HEAP_REST %x\n",size);
    }

    //�`�惂�[�h�߂�
    {
      const GFL_BG_SYS_HEADER bg_sys_header = 
        {
          GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_3D
        };
      GFL_BG_SetBGMode( &bg_sys_header );
    }

    {
      FLDMSGBG *fmb = FIELDMAP_GetFldMsgBG( fieldmap );
      FIELD_PLACE_NAME *place_name_sys = FIELDMAP_GetPlaceNameSys( fieldmap );
      FIELD_DEBUG_WORK *debug = FIELDMAP_GetDebugWork( fieldmap );
      FLDMSGBG_RecoveryBG( fmb );
      FIELD_PLACE_NAME_RecoverBG( place_name_sys );
      FIELD_DEBUG_RecoverBgCont( debug );
      //�a�f2�ʃN���A
      GFL_BG_ClearFrame( GFL_BG_FRAME2_M );
    }
    
    //�v���C�I���e�B�̕��A
    PopPriority(ptr);
    //�\����Ԃ̕��A
    PopDisp(ptr);

    //�n�a�i�̃|�[�Y����
    {
      MMDLSYS *mmdlsys = FIELDMAP_GetMMdlSys( fieldmap );
      MMDLSYS_ClearPauseMoveProc(mmdlsys);
    }
#if 0
    //�}�b�v�J�ڂ����āA�a�f�̃Z�b�g�A�b�v������
    {
      GMEVENT * child;
      int zone_id;
      FIELD_PLAYER *fld_player;
      PLAYER_WORK *player_work;
      FLDMAP_CTRLTYPE map_type;

      fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
      {
        GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
        player_work = GAMEDATA_GetMyPlayerWork(gamedata);
      }
      zone_id = player_work->zoneID;

      map_type = FIELDMAP_GetMapControlType( fieldmap );
      
      if (map_type == FLDMAP_CTRLTYPE_GRID){
        VecFx32 pos;
        //���@�̍��W
        FIELD_PLAYER_GetPos( fld_player, &pos );
        child = EVENT_ChangeMapPos(gsys, fieldmap,
            zone_id, &pos, DIR_DOWN );
      }
      else{
        RAIL_LOCATION rail_loc;
        MMDL_GetRailLocation( FIELD_PLAYER_GetMMdl( fld_player ), &rail_loc );
        child = DEBUG_EVENT_ChangeMapRailLocation(gsys, fieldmap,
            zone_id, &rail_loc, DIR_DOWN );
      }
      //�C�x���g�R�[��
      GMEVENT_CallEvent(event, child);
    }
#endif    
    (*seq)++;
    break;
  case 8:
    //�I��
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * �p�[�e�B�N���Đ�
 *
 * @param   ptr         �J�b�g�C���Ǘ��|�C���^
 *
 * @return	BOOL        TRUE�ōĐ��I��
 */
//--------------------------------------------------------------------------------------------
static BOOL PlayParticle(FLD3D_CI_PTR ptr)
{
  if (!ptr->PartGene){
    //�G�~�b�^�[1��
    if ( ptr->Emit1==FALSE )
    {
      if ( ptr->PrtclWait[0] > 0 ){
        ptr->PrtclWait[0]--;
      }else{
        //�p�[�e�B�N���W�F�l���[�g
        Generate(ptr, 0);
        ptr->Emit1 = TRUE;
      }
    }

    //�G�~�b�^�[2��
    if ( ptr->Emit2==FALSE )
    {
      if ( ptr->PrtclWait[1] > 0 ){
        ptr->PrtclWait[1]--;
      }else{
        //�p�[�e�B�N���W�F�l���[�g
        Generate(ptr, 1);
        ptr->Emit2 = TRUE;
      }
    }

    if ( ptr->Emit1 && ptr->Emit2 ){
      ptr->PartGene = 1;
    }
  }else{
    //�p�[�e�B�N���I������
    if ( FLD_PRTCL_CheckEmitEnd( ptr->PrtclSys ) ){
      return TRUE;
    }
  }
  return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * ���f���A�j��1�Đ�
 *
 * @param   ptr         �J�b�g�C���Ǘ��|�C���^
 *
 * @return	BOOL        TRUE�ōĐ��I��
 */
//--------------------------------------------------------------------------------------------
static BOOL PlayMdlAnm1(FLD3D_CI_PTR ptr)
{
  //�A�j�����Ȃ��ꍇ�͏I���Ƃ݂Ȃ�
  if (!ptr->Anm1Flg) return TRUE;

  if (ptr->MdlAnm1Wait > 0){
    ptr->MdlAnm1Wait--;
  }else{
    BOOL rc;
    u8 i;
    u16 num;
    GFL_G3D_OBJ* obj;
    u16 obj_idx = GFL_G3D_UTIL_GetUnitObjIdx( ptr->Util, ptr->UnitIdx );
    obj = GFL_G3D_UTIL_GetObjHandle( ptr->Util, obj_idx );
    num = GFL_G3D_OBJECT_GetAnimeCount( obj );
    //�A�j���Đ�
    for (i=0;i<num;i++){
      rc = GFL_G3D_OBJECT_IncAnimeFrame( obj, i, FX32_ONE );
    }
    if (rc == FALSE){
      return TRUE;
    }
  }
  return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * ���f���A�j��2�Đ�
 *
 * @param   ptr         �J�b�g�C���Ǘ��|�C���^
 *
 * @return	BOOL        TRUE�ōĐ��I��
 */
//--------------------------------------------------------------------------------------------
static BOOL PlayMdlAnm2(FLD3D_CI_PTR ptr)
{
  //�A�j�����Ȃ��ꍇ�͏I���Ƃ݂Ȃ�
  if (!ptr->Anm2Flg) return TRUE;

  if (ptr->MdlAnm2Wait > 0){
    ptr->MdlAnm2Wait--;
  }else{
    //�A�j���Đ�
    BOOL rc;
    u8 i;
    u16 num;
    GFL_G3D_OBJ* obj;
    u16 obj_idx = GFL_G3D_UTIL_GetUnitObjIdx( ptr->Util, ptr->UnitIdx );
    obj = GFL_G3D_UTIL_GetObjHandle( ptr->Util, obj_idx+1 );
    num = GFL_G3D_OBJECT_GetAnimeCount( obj );
    //�A�j���Đ�
    for (i=0;i<num;i++){
      rc = GFL_G3D_OBJECT_IncAnimeFrame( obj, i, FX32_ONE );
    }
    if (rc == FALSE){
      return TRUE;
    }
  }
  return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * ���\�[�X�Z�b�g�A�b�v
 *
 * @param   ptr         �J�b�g�C���Ǘ��|�C���^
 * @param   outDat      �Z�b�g�A�b�v�f�[�^�i�[�o�b�t�@
 * @param   inCutInNo   �J�b�g�C���i���o�[
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void SetupResource(FLD3D_CI_PTR ptr, RES_SETUP_DAT *outDat, const u8 inCutInNo)
{
  void *resource;
  //�p�[�e�B�N������
  ptr->PrtclPtr = FLD_PRTCL_Create(ptr->PrtclSys);
  //�Z�b�g�A�b�v�f�[�^�쐬
  CreateRes(outDat, inCutInNo, ptr->HeapID);
  
  //3�c���f�����\�[�X���Z�b�g�A�b�v
  SetupResourceCore(ptr, &outDat->Setup);
  //�p�[�e�B�N�����\�[�X�Z�b�g�A�b�v
  if ( outDat->SpaIdx != NONDATA )
  {
    resource = FLD_PRTCL_LoadResource(ptr->PrtclSys,
			ARCID_FLD3D_CI, outDat->SpaIdx);

    FLD_PRTCL_SetResource(ptr->PrtclSys, resource, TRUE, NULL);
  }else
  {
    resource = NULL;
  }

  //�A�j����L���ɂ���
  {
    u8 i,j;
    u16 obj_num,anm_num;
    GFL_G3D_OBJ* obj;
    u16 obj_idx = GFL_G3D_UTIL_GetUnitObjIdx( ptr->Util, ptr->UnitIdx );
    obj_num = GFL_G3D_UTIL_GetUnitObjCount( ptr->Util, ptr->UnitIdx );
    for(i=0;i<obj_num;i++){
      obj = GFL_G3D_UTIL_GetObjHandle( ptr->Util, obj_idx+i );
      anm_num = GFL_G3D_OBJECT_GetAnimeCount( obj );
      //�A�j���L��
      for (j=0;j<anm_num;j++){
        GFL_G3D_OBJECT_EnableAnime( obj, j );
      }
    }
  }
  //�E�F�C�g�ݒ�
  ptr->PrtclWait[0] = outDat->SpaWait[0];
  ptr->PrtclWait[1] = outDat->SpaWait[1];
  ptr->MdlAnm1Wait = outDat->MdlAAnmWait;
  ptr->MdlAnm2Wait = outDat->MdlBAnmWait;
  //���\�[�X�̒��̂������̐��ŕ���
  {
    u16 res_num;
    if (resource != NULL) res_num = GFL_PTC_GetResNum( resource );
    else res_num = 0;

    NOZOMU_Printf("resnum=%d\n",res_num);
    if ( res_num == EMITCOUNT_MAX )
    {
      ptr->Emit1 = FALSE;
      ptr->Emit2 = FALSE;
    }
    else if( res_num == 1 )
    {
      ptr->Emit1 = FALSE;
      ptr->Emit2 = TRUE;
    }
    else
    {
      ptr->Emit1 = TRUE;
      ptr->Emit2 = TRUE;
    }
  }
  //�n�a�i�̃A�j�����ōĐ����������邩�𔻒f����
  {
    ptr->Anm1Flg = FALSE;
    ptr->Anm2Flg = FALSE;
    if (outDat->Anm1Num != 0) ptr->Anm1Flg = TRUE;
    if (outDat->Anm2Num != 0) ptr->Anm2Flg = TRUE;
  }

  if (ptr->SetupCallBack != NULL )
  {
    ptr->SetupCallBack(ptr);
  }
}

//--------------------------------------------------------------------------------------------
/**
 * ���\�[�X�폜
 *
 * @param   ptr         �J�b�g�C���Ǘ��|�C���^
 * @param   ioDat      �Z�b�g�A�b�v�f�[�^�i�[�o�b�t�@
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void DeleteResource(FLD3D_CI_PTR ptr, RES_SETUP_DAT *ioDat)
{
  DeleteRes(ioDat);
  GFL_G3D_UTIL_DelUnit( ptr->Util, ptr->UnitIdx );
  ptr->UnitIdx = UNIT_NONE;
  FLD_PRTCL_Delete(ptr->PrtclSys);
}

//--------------------------------------------------------------------------------------------
/**
 * �p�[�e�B�N���W�F�l���[�g
 *
 * @param   ptr         �J�b�g�C���Ǘ��|�C���^
 * @param   inResNo     ���\�[�X�i���o�[
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void Generate(FLD3D_CI_PTR ptr, const u32 inResNo)
{
///  GFL_PTC_SetCameraType(sys->PrtclPtr, GFL_G3D_PRJORTH);
	GFL_PTC_CreateEmitterCallback(ptr->PrtclPtr, inResNo,
									ParticleCallBack, NULL);                 
}

//--------------------------------------------------------------
/**
 * @brief	�R�[���o�b�N
 *
 * @param	emit	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void ParticleCallBack(GFL_EMIT_PTR emit)
{
///	SOFT_SPRITE *ss = Particle_GetTempPtr();
	VecFx32 pos = { 0, 0, 0 };
	
	pos.z = 0x40;
	SPL_SetEmitterPosition(emit, &pos);
}

//--------------------------------------------------------------
/**
 * @brief	���\�[�X�쐬�֐�
 *
 * @param     outDat          �Z�b�g�A�b�v�f�[�^�i�[�o�b�t�@
 * @param     inResArcIdx     �A�[�J�C�u�C���f�b�N�X
 * @param     inHeapID        �q�[�v�h�c
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void CreateRes(RES_SETUP_DAT *outDat, const u8 inResArcIdx, const HEAPID inHeapID)
{
  u8 i;
  RES_DEF_DAT def_dat;
  u8 res_num,obj_num,anm1_num,anm2_num;
  u8 obj_res_start,anm1_res_start,anm2_res_start;

  //�A�[�J�C�u���烊�\�[�X��`�����[�h
  GFL_ARC_LoadData(&def_dat, ARCID_FLD3D_CI_SETUP, inResArcIdx);

  //OBJ���𒲂ׂ�
  obj_num = 0;
  for (i=0;i<OBJCOUNT_MAX;i++){
    if (def_dat.MdlObjIdx[i] != NONDATA){
      obj_num++;
    }
  }
  //�A�j�����𒲂ׂ�
  anm1_num = 0;
  anm2_num = 0;
  for(i=0;i<ANM_TYPE_MAX;i++){
    if (def_dat.MdlAAnmIdx[i] != NONDATA){
      anm1_num++;
    }
  }
  for(i=0;i<ANM_TYPE_MAX;i++){
    if (def_dat.MdlBAnmIdx[i] != NONDATA){
      anm2_num++;
    }
  }

  obj_res_start = 0;
  anm1_res_start = obj_res_start + obj_num;
  anm2_res_start = anm1_res_start + anm1_num;

  //���\�[�X���𒲂ׂ�
  res_num = obj_num+anm1_num+anm2_num;
#if 0
  OS_Printf("obj_anm_res = %d %d %d %d\n",obj_num, anm1_num, anm2_num, res_num);
  OS_Printf("res_start = %d %d %d \n",obj_res_start,anm1_res_start,anm2_res_start );
#endif
  //���\�[�X�e�[�u���A���P�[�g
  if (res_num){
    outDat->Res = GFL_HEAP_AllocClearMemory(inHeapID, sizeof(GFL_G3D_UTIL_RES)*res_num);
  }
  //�n�a�i�e�[�u���A���P�[�g
  if(obj_num){
    outDat->Obj = GFL_HEAP_AllocClearMemory(inHeapID, sizeof(GFL_G3D_UTIL_OBJ)*obj_num);
  }
  //�A�j���e�[�u���A���P�[�g
  if(anm1_num){
    outDat->Anm1 = GFL_HEAP_AllocClearMemory(inHeapID, sizeof(GFL_G3D_UTIL_ANM)*anm1_num);
  }
  //�A�j���e�[�u���A���P�[�g
  if(anm2_num){
    outDat->Anm2 = GFL_HEAP_AllocClearMemory(inHeapID, sizeof(GFL_G3D_UTIL_ANM)*anm2_num);
  }

  //���\�[�X���L��
  outDat->ResNum = res_num;
  outDat->ObjNum = obj_num;
  outDat->Anm1Num = anm1_num;
  outDat->Anm2Num = anm2_num;
  //�E�F�C�g�L��
  outDat->SpaWait[0] = def_dat.SpaWait[0];
  outDat->SpaWait[1] = def_dat.SpaWait[1];
  outDat->MdlAAnmWait = def_dat.MdlAAnmWait;
  outDat->MdlBAnmWait = def_dat.MdlBAnmWait;

  {
    u8 i;
    u8 count = 0;
    u16 arc_id[RESCOUNT_MAX];
    arc_id[0] = def_dat.MdlObjIdx[0];
    arc_id[1] = def_dat.MdlObjIdx[1];
    arc_id[2] = def_dat.MdlAAnmIdx[0];
    arc_id[3] = def_dat.MdlAAnmIdx[1];
    arc_id[4] = def_dat.MdlAAnmIdx[2];
    arc_id[5] = def_dat.MdlBAnmIdx[0];
    arc_id[6] = def_dat.MdlBAnmIdx[1];
    arc_id[7] = def_dat.MdlBAnmIdx[2];


    for (i=0;i<RESCOUNT_MAX;i++){
      if ( arc_id[i] != NONDATA ){
        outDat->Res[count].arcive = ARCID_FLD3D_CI;
        outDat->Res[count].datID = arc_id[i];
        outDat->Res[count].arcType = GFL_G3D_UTIL_RESARC;
        count++;
      }
    }
  }

  {
    u8 i;
    for (i=0;i<anm1_num;i++){
      outDat->Anm1[i].anmresID = anm1_res_start+i;
      outDat->Anm1[i].anmdatID = 0;
    }
    for (i=0;i<anm2_num;i++){
      outDat->Anm2[i].anmresID = anm2_res_start+i;
      outDat->Anm2[i].anmdatID = 0;
    }
  }

  {
    u8 i;
    for (i=0;i<obj_num;i++){
      outDat->Obj[i].mdlresID = obj_res_start+i;  
      outDat->Obj[i].mdldatID = 0;
      outDat->Obj[i].texresID = obj_res_start+i;
    }
    if (obj_num>=1){
      outDat->Obj[0].anmTbl = outDat->Anm1;
      outDat->Obj[0].anmCount = anm1_num;
    }
    if (obj_num>=2){
      outDat->Obj[1].anmTbl = outDat->Anm2;
      outDat->Obj[1].anmCount = anm2_num;
    }
  }


  outDat->Setup.resTbl = outDat->Res;
  outDat->Setup.resCount = res_num;
  outDat->Setup.objTbl = outDat->Obj;
  outDat->Setup.objCount = obj_num;

  outDat->SpaIdx = def_dat.SpaIdx;
}

//--------------------------------------------------------------
/**
 * @brief	���\�[�X�j���֐�
 *
 * @param	  ioDat   �Z�b�g�A�b�v�f�[�^�i�[�o�b�t�@
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void DeleteRes(RES_SETUP_DAT *ioDat)
{
  if (ioDat->Anm1 != NULL){
    GFL_HEAP_FreeMemory( ioDat->Anm1 );
  }
  if (ioDat->Anm2 != NULL){
    GFL_HEAP_FreeMemory( ioDat->Anm2 );
  }
  if (ioDat->Obj != NULL){
    GFL_HEAP_FreeMemory( ioDat->Obj );
  }
  if(ioDat->Res != NULL){
    GFL_HEAP_FreeMemory( ioDat->Res );
  }
}

#ifdef PM_DEBUG
#define FLYSKY_CAM_MOVE_FRAME (10)
//--------------------------------------------------------------
/**
 * @brief	����ԃJ�����A���O���ɂȂ�C�x���g���쐬����
 * @todo�@���̋@�\�͂��̃t�@�C������؂藣��
 *
 * @param	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void FLD3D_CI_FlySkyCameraDebug(
    GAMESYS_WORK *gsys, FLD3D_CI_PTR ptr, FIELD_CAMERA *camera, FIELD_PLAYER * player, FLDNOGRID_MAPPER* mapper )
{
  FLYSKY_EFF_WORK *work;
  int size;
  size = sizeof(FLYSKY_EFF_WORK);
  //�C�x���g�쐬
  {
    GMEVENT * event = GMEVENT_Create( gsys, NULL, DebugFlySkyEffEvt, size );

    work = GMEVENT_GetEventWork(event);
    MI_CpuClear8( work, size );
    work->gsys = gsys;
    work->camera = camera;
    work->CiPtr = ptr;
    work->player = player;
    work->mapper = mapper;

    GAMESYSTEM_SetEvent(gsys, event);
  }
}

static GMEVENT_RESULT DebugFlySkyEffEvt( GMEVENT* event, int* seq, void* work )
{
  FLYSKY_EFF_WORK *wk;
  wk = work;

  switch(*seq){
  case 0:
    //�J�����G���A������t�b�N
    FIELD_CAMERA_SetCameraAreaActive( wk->camera, FALSE );
    //���݃E�H�b�`�^�[�Q�b�g��ۑ�
    wk->Watch = FIELD_CAMERA_GetWatchTarget(wk->camera);
    FIELD_CAMERA_StopTraceRequest(wk->camera);
    (*seq)++;
    break;
  case 1:
    if ( (wk->Watch != NULL) && FIELD_CAMERA_CheckTrace(wk->camera) ){
      break;
    }
    //���[���V�X�e���X�V�X�g�b�v
    FLDNOGRID_MAPPER_SetRailCameraActive( wk->mapper, FALSE );
    //�J�����p�[�W
    FIELD_CAMERA_FreeTarget(wk->camera);
    //���݂̃J�����ݒ��ۑ�
    FIELD_CAMERA_SetRecvCamParam(wk->camera);
    //�J���������уJ�����֐��`�ړ����N�G�X�g
    {
      FLD_CAM_MV_PARAM param;
      VecFx32 player_vec;
      //��l����3�c���W���擾
      FIELD_PLAYER_GetPos( wk->player, &player_vec );
      param.Core.AnglePitch = 9688;
      param.Core.AngleYaw = 0;
      param.Core.Distance = 970752;
      param.Core.Shift.x = 0;
      param.Core.Shift.y = 16384;
      param.Core.Shift.z = 0;
      param.Core.TrgtPos = player_vec;
      param.Core.Fovy = 3640;
      param.Chk.Shift = TRUE;
      param.Chk.Pitch = TRUE;
      param.Chk.Yaw = FALSE;
      param.Chk.Dist = TRUE;
      param.Chk.Fovy = TRUE;
      param.Chk.Pos = TRUE;
      NOZOMU_Printf("plyer_pos = %d,%d,%d\n",player_vec.x, player_vec.y, player_vec.z);
      FIELD_CAMERA_SetLinerParam(wk->camera, &param, FLYSKY_CAM_MOVE_FRAME);
    }
    (*seq)++;
    /*none break*/
  case 2:
    //�J�����ړ��҂�
    if ( !FIELD_CAMERA_CheckMvFunc(wk->camera) )
    {
      GMEVENT *child;
#if 0      
      {
        u16 pitch, yaw;
        fx32 len;
        u16 fovy;
        VecFx32 shift, target;
        OS_Printf("--DUMP_FIELD_CAMERA_PARAM--\n");
        pitch = FIELD_CAMERA_GetAnglePitch(wk->camera);
        yaw = FIELD_CAMERA_GetAngleYaw(wk->camera);
        len = FIELD_CAMERA_GetAngleLen(wk->camera);
        fovy = FIELD_CAMERA_GetFovy(wk->camera);
        FIELD_CAMERA_GetTargetOffset( wk->camera, &shift );
        FIELD_CAMERA_GetTargetPos( wk->camera, &target );
      
        OS_Printf("%d,%d,%d,%d,%d,%d\n", pitch, yaw, len, target.x, target.y, target.z );
        OS_Printf("%d,%d,%d,%d\n", fovy, shift.x, shift.y, shift.z );
      }
#endif      
      //�J�b�g�C�����o�J�n
      child = FLD3D_CI_CreateCutInEvt(wk->gsys, wk->CiPtr, 0);
      GMEVENT_CallEvent(event, child);
      (*seq)++;
    }
    break;
  case 3:
    {
      FLD_CAM_MV_PARAM_CHK chk;
      chk.Shift = TRUE;
      chk.Pitch = TRUE;
      chk.Yaw = FALSE;
      chk.Dist = TRUE;
      chk.Fovy = TRUE;
      chk.Pos = TRUE;
      //�J�����߂�
      FIELD_CAMERA_RecvLinerParam(wk->camera, &chk, FLYSKY_CAM_MOVE_FRAME);
    }
    (*seq)++;
    /*none break*/
  case 4:
    //�J�����߂��҂�
    if ( !FIELD_CAMERA_CheckMvFunc(wk->camera) ){
      //�J�����o�C���h
      if (wk->Watch != NULL){
        FIELD_CAMERA_BindTarget(wk->camera, wk->Watch);
      }
      FIELD_CAMERA_RestartTrace(wk->camera);
      //���[���V�X�e���X�V�J�n
      FLDNOGRID_MAPPER_SetRailCameraActive( wk->mapper, TRUE );
      //���A�f�[�^�̃N���A
      FIELD_CAMERA_ClearRecvCamParam(wk->camera);
      //�J�����G���A������t�b�N����
      FIELD_CAMERA_SetCameraAreaActive( wk->camera, TRUE );
      return GMEVENT_RES_FINISH;
    }
  }

  return GMEVENT_RES_CONTINUE;
}

#endif  //PM_DEBUG

//----------------------------------------------------------------------------
/**
 *	@brief	�L���v�`�����N�G�X�g
 *
 *	@param	�J�b�g�C���Ǘ��|�C���^
 *
 *	@return none
 */
//-----------------------------------------------------------------------------
static void ReqCapture(FLD3D_CI_PTR ptr)
{
  ptr->CapEndFlg = FALSE;
  GX_SetBankForLCDC(GX_VRAM_LCDC_D);
  GFUser_VIntr_CreateTCB(Graphic_Tcb_Capture, &ptr->CapEndFlg, 0 );
}

//----------------------------------------------------------------------------
/**
 *	@brief	V�u�����N�^�X�N
 *
 *	@param	GFL_TCB *p_tcb
 *	@param	*p_work 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void Graphic_Tcb_Capture( GFL_TCB *p_tcb, void *p_work )
{
  BOOL *cap_end_flg = p_work;
  
	GX_SetCapture(GX_CAPTURE_SIZE_256x192,  // Capture size
                      GX_CAPTURE_MODE_A,			   // Capture mode
                      GX_CAPTURE_SRCA_3D,						 // Blend src A
                      GX_CAPTURE_SRCB_VRAM_0x00000,     // Blend src B
                      GX_CAPTURE_DEST_VRAM_D_0x00000,   // Output VRAM
                      16,             // Blend parameter for src A
                      0);            // Blend parameter for src B

  GFL_TCB_DeleteTask( p_tcb );
  if (cap_end_flg != NULL){
    *cap_end_flg = TRUE;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�v���C�I���e�B�v�b�V��
 *
 *	@param	ptr       �J�b�g�C���Ǘ��|�C���^
 *	@return none
 */
//-----------------------------------------------------------------------------
static void PushPriority(FLD3D_CI_PTR ptr)
{
  int i;
  for(i=0;i<4;i++){
    ptr->BgPriority[i] = GFL_BG_GetPriority( i );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�\����ԃv�b�V��
 *
 *	@param	ptr       �J�b�g�C���Ǘ��|�C���^
 *	@return none
 */
//-----------------------------------------------------------------------------
static void PushDisp(FLD3D_CI_PTR ptr)
{
  ptr->Mask = GFL_DISP_GetMainVisible();
}

//----------------------------------------------------------------------------
/**
 *	@brief	�v���C�I���e�B�|�b�v
 *
 *	@param	ptr       �J�b�g�C���Ǘ��|�C���^
 *	@return none
 */
//-----------------------------------------------------------------------------
static void PopPriority(FLD3D_CI_PTR ptr)
{
  int i;
  for(i=0;i<4;i++){
    GFL_BG_SetPriority( i, ptr->BgPriority[i] );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�\����ԃ|�b�v
 *
 *	@param	ptr       �J�b�g�C���Ǘ��|�C���^
 *	@return none
 */
//-----------------------------------------------------------------------------
static void PopDisp(FLD3D_CI_PTR ptr)
{
  GFL_DISP_GX_SetVisibleControlDirect(ptr->Mask);
}

//----------------------------------------------------------------------------
/**
 *	@brief	�|�P�����O���t�B�b�N�ɏ�������
 *
 *	@param	ptr       �J�b�g�C���Ǘ��|�C���^
 *	@return none
 */
//-----------------------------------------------------------------------------
static void ReTransToPokeGra(FLD3D_CI_PTR ptr)
{
  u32 tex_adr;
  u32 pal_adr;
  HEAPID heapID = ptr->HeapID;
  ptr->pokeImgBitmap = GFL_BMP_Create(TEXSIZ_S/8, TEXSIZ_T/8, GFL_BMP_16_COLOR, heapID);
  //���\�[�X�̂u�q�`�l�A�h���X�擾
  {
    GFL_G3D_RES* res = GFL_G3D_UTIL_GetResHandle( ptr->Util, 0 );
    NNSG3dTexKey texkey = GFL_G3D_GetTextureDataVramKey( res );
    NNSG3dPlttKey palkey = GFL_G3D_GetTexturePlttVramKey( res );
    tex_adr = NNS_GfdGetTexKeyAddr(texkey);
    pal_adr = NNS_GfdGetPlttKeyAddr(palkey);
    NOZOMU_Printf("key = %d %d\n",texkey, palkey);
    NOZOMU_Printf("adr = %x %x\n",tex_adr, pal_adr);
    NOZOMU_Printf("resid = %d\n",GFL_G3D_UTIL_GetUnitResIdx( ptr->Util,0 ) );
    
  }
  //�|�P�����O���t�B�b�N�擾
  {
    u32 cgr, pal;
    //���\�[�X�󂯎��
    cgr	= POKEGRA_GetCgrArcIndex( ptr->MonsNo,ptr->FormNo,ptr->Sex,ptr->Rare,POKEGRA_DIR_FRONT, ptr->Egg );
    pal = POKEGRA_GetPalArcIndex( ptr->MonsNo,ptr->FormNo,ptr->Sex,ptr->Rare,POKEGRA_DIR_FRONT, ptr->Egg );
    ptr->chr = NULL;
    ptr->plt = NULL;
    //���\�[�X��OBJ�Ƃ��č���Ă���̂ŁALoadOBJ����Ȃ��Ɠǂݍ��߂Ȃ�
    ptr->chr_buf = GFL_ARC_UTIL_LoadOBJCharacter( POKEGRA_GetArcID(), cgr, FALSE, &ptr->chr, heapID );
    ptr->pal_buf = GFL_ARC_UTIL_LoadPalette( POKEGRA_GetArcID(), pal, &ptr->plt, heapID );
  }
  //12x12chr�|�P�O����16x16chr�̐^�񒆂ɓ\��t����
  {
    u8	*src, *dst;
    int x, y;
    int	chrNum = 0;

    src = ptr->chr->pRawData;
		dst = GFL_BMP_GetCharacterAdrs(ptr->pokeImgBitmap);
    GFL_STD_MemClear32((void*)dst, 16*16*0x20);

    dst += ( (2*16+2) * 0x20 );
    //�L�����f�[�^��8x8�A4x8�A8x4�A4x4�̏��Ԃ�1D�}�b�s���O����Ă���
    for (y=0; y<8; y++){
      for(x=0; x<8; x++){
        GFL_STD_MemCopy32((void*)(&src[chrNum*0x20]), (void*)(&dst[(y*16+x)*0x20]), 0x20);
        chrNum++;
      }
    }
    for (y=0; y<8; y++){
      for(x=8; x<12; x++){
        GFL_STD_MemCopy32((void*)(&src[chrNum*0x20]), (void*)(&dst[(y*16+x)*0x20]), 0x20);
        chrNum++;
      }
    }
    for (y=8; y<12; y++){
      for(x=0; x<8; x++){
        GFL_STD_MemCopy32((void*)(&src[chrNum*0x20]), (void*)(&dst[(y*16+x)*0x20]), 0x20);
        chrNum++;
      }
    }
    for (y=8; y<12; y++){
      for(x=8; x<12; x++){
        GFL_STD_MemCopy32((void*)(&src[chrNum*0x20]), (void*)(&dst[(y*16+x)*0x20]), 0x20);
        chrNum++;
      }
    }
  }
  //16x16chr�t�H�[�}�b�g�f�[�^���a�l�o�f�[�^�ɕϊ�
  GFL_BMP_DataConv_to_BMPformat(ptr->pokeImgBitmap, FALSE, heapID);
  //�]��
  {
    BOOL rc;
    void*	src;
		u32		dst;
    src = (void*)GFL_BMP_GetCharacterAdrs(ptr->pokeImgBitmap);
		dst = tex_adr;
    rc = NNS_GfdRegisterNewVramTransferTask(NNS_GFD_DST_3D_TEX_VRAM, dst, src, TEXVRAMSIZ);
    GF_ASSERT(rc);
    src = ptr->plt->pRawData;
		dst = pal_adr;
    rc = NNS_GfdRegisterNewVramTransferTask(NNS_GFD_DST_3D_TEX_PLTT, dst, src, 32);
    GF_ASSERT(rc);
  }

  {
    int size = GFL_HEAP_GetHeapFreeSize(ptr->HeapID);
    NOZOMU_Printf("::FLD3DCUTIN_HEAP_REST %x\n",size);
  }
}

