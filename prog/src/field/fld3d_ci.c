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

#define OBJCOUNT_MAX  (2)
#define ANM_TYPE_MAX  (4)
#define RESCOUNT_MAX  (8)

#define UNIT_NONE   (0xffffffff)

#define NONDATA  (0xffff)

#define	PRO_MAT_Z_OFS	(20*FX32_ONE)   //�v���W�F�N�V�����}�g���b�N�X�ɂ��f�v�X�o�b�t�@����l

#ifdef PM_DEBUG

#include "fieldmap.h" //for FIELDMAP_�`
#include "event_mapchange.h"

#include "field/field_msgbg.h"    //for FLDMSGBG_RecoveryBG

#endif  //PM_DEBUG


typedef struct FLD3D_CI_tag
{
  FLD_PRTCL_SYS_PTR PrtclSys;
  GFL_G3D_CAMERA *g3Dcamera;
  HEAPID HeapID;
  GFL_G3D_UTIL* Util;
  int UnitIdx;
  GFL_G3D_OBJSTATUS Status;

  GFL_PTC_PTR PrtclPtr;

  u16 PrtclWait;
  u16 MdlAnm1Wait;
  u16 MdlAnm2Wait;
  u8 PartGene;
  u8 CutInNo;

  GXPlaneMask Mask;
  u8 BgPriority[4];
  BOOL CapEndFlg;
}FLD3D_CI;

//�o�C�i���f�[�^�t�H�[�}�b�g
typedef struct {
  u16 SpaIdx;
  u16 SpaWait;
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
  u16 SpaWait;
  u16 MdlAAnmWait;
  u16 MdlBAnmWait;
  u8 ResNum;
  u8 ObjNum;
  u8 AnmNum;
  u8 SpaIdx;
  u8 Dummy[2];
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
static void DeleteResource(FLD3D_CI_PTR ptr, RES_SETUP_DAT *outDat);
static BOOL PlayParticle(FLD3D_CI_PTR ptr);
static BOOL PlayMdlAnm1(FLD3D_CI_PTR ptr);
static BOOL PlayMdlAnm2(FLD3D_CI_PTR ptr);
static BOOL CheckAnmEnd(FLD3D_CI_PTR ptr);

static GMEVENT_RESULT CutInEvt( GMEVENT* event, int* seq, void* work );


static void ParticleCallBack(GFL_EMIT_PTR emit);    //@todo
static void Generate(FLD3D_CI_PTR ptr);    //@todo

static void CreateRes(RES_SETUP_DAT *outDat, const u8 inResArcIdx, const HEAPID inHeapID);
static void DeleteRes(RES_SETUP_DAT *outDat);

static GMEVENT *CreateCutInEvt(GAMESYS_WORK *gsys, FLD3D_CI_PTR ptr, const u8 inCutInNo);

static void PushPriority(FLD3D_CI_PTR ptr);
static void PushDisp(FLD3D_CI_PTR ptr);
static void PopPriority(FLD3D_CI_PTR ptr);
static void PopDisp(FLD3D_CI_PTR ptr);
static void ReqCapture(FLD3D_CI_PTR ptr);

static void Graphic_Tcb_Capture( GFL_TCB *p_tcb, void *p_work );

#ifdef PM_DEBUG
static GMEVENT_RESULT DebugFlySkyEffEvt( GMEVENT* event, int* seq, void* work );
#endif  //PM_DEBUG


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
 * @param   
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

void FLD3D_CI_Draw( FLD3D_CI_PTR ptr )
{
  u8 i,j;
  
#if 0
#ifdef PM_DEBUG
  static int test_ofs = 8;
  if (GFL_UI_KEY_GetCont() & PAD_BUTTON_DEBUG){
    if ( GFL_UI_KEY_GetTrg() & PAD_KEY_UP ){
      test_ofs++;
    }else if(GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN){
      test_ofs--;
    }
      OS_Printf("ofs=%d\n",test_ofs);
  }
#endif  //PM_DEBUG  
#endif
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


//�J�b�g�C���Ăяo��
void FLD3D_CI_CallCutIn( GAMESYS_WORK *gsys, FLD3D_CI_PTR ptr, const u8 inCutInNo )
{
  GMEVENT * event;
  event = CreateCutInEvt(gsys, ptr, inCutInNo);
  GAMESYSTEM_SetEvent(gsys, event);
}

//�J�b�g�C���C�x���g�쐬
static GMEVENT *CreateCutInEvt(GAMESYS_WORK *gsys, FLD3D_CI_PTR ptr, const u8 inCutInNo)
{
  GMEVENT * event;
  FLD3D_CI_EVENT_WORK *work;
  int size;

  size = sizeof(FLD3D_CI_EVENT_WORK);
  ptr->PartGene = 0;
  ptr->CutInNo = inCutInNo;
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

//�J�b�g�C���C�x���g
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
    //@todo �n�a�i�̃|�[�Y
    ;
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
      GFL_BG_SetPriority( 3, 3 );
      //�`�惂�[�h�ύX
      {
        const GFL_BG_SYS_HEADER bg_sys_header = 
          {
            GX_DISPMODE_GRAPHICS,GX_BGMODE_3,GX_BGMODE_0,GX_BG0_AS_3D
          };
        GFL_BG_SetBGMode( &bg_sys_header );
      }
      //�a�f�Z�b�g�A�b�v
      G2_SetBG3ControlDCBmp(
          GX_BG_SCRSIZE_DCBMP_256x256,
          GX_BG_AREAOVER_XLU,
          GX_BG_BMPSCRBASE_0x00000
      );
      GX_SetBankForBG(GX_VRAM_BG_128_D);
      //�A���t�@�u�����h
      G2_SetBlendAlpha(GX_BLEND_PLANEMASK_NONE,
				GX_BLEND_PLANEMASK_BG3, 0,0);
      //�����ł͂܂��L���v�`���ʂ̂ݕ\��(�p�V���h�~)
      GFL_BG_SetVisible( GFL_BG_FRAME3_M, VISIBLE_ON );
      (*seq)++;
    }
    break;
  case 3:
    //���\�[�X���[�h
    SetupResource(ptr, &evt_work->SetupDat, ptr->CutInNo);
    //�Z�b�g�A�b�v�I�������̂�3�c�ʂ��I��
    GFL_BG_SetVisible( GFL_BG_FRAME3_M, VISIBLE_ON );
    GFL_BG_SetVisible( GFL_BG_FRAME0_M, VISIBLE_ON );
    {
      int size = GFL_HEAP_GetHeapFreeSize(ptr->HeapID);
      OS_Printf("START::FLD3DCUTIN_HEAP_REST %x\n",size);
    }
    //�t�B�[���h�\�����[�h�ؑ�
    FIELDMAP_SetDraw3DMode(fieldmap, DRAW3DMODE_CUTIN);
    (*seq)++;
    break;
  case 4:
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

      if (rc1&&rc2&&rc3){
        (*seq)++;
      }
    }
    break;
  case 6:
    //�t�B�[���h���[�h�߂�
    FIELDMAP_SetDraw3DMode(fieldmap, DRAW3DMODE_NORMAL);
    //���\�[�X�������
    DeleteResource(ptr, &evt_work->SetupDat);
    (*seq)++;
    break;
  case 7:
    //�\����Ԃ̕��A
    PopDisp(ptr);
    {
      int size = GFL_HEAP_GetHeapFreeSize(ptr->HeapID);
      OS_Printf("END::FLD3DCUTIN_HEAP_REST %x\n",size);
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
      FLDMSGBG * fmb = FIELDMAP_GetFldMsgBG( fieldmap );
      FLDMSGBG_RecoveryBG( fmb );
    }
    //�v���C�I���e�B�̕��A
    PopPriority(ptr);

    //@todo �n�a�i�̃|�[�Y����
    ;
#if 1
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
        child = DEBUG_EVENT_ChangeMapPos(gsys, fieldmap,
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

//�p�[�e�B�N���Đ�
static BOOL PlayParticle(FLD3D_CI_PTR ptr)
{
  if (!ptr->PartGene){
    if (ptr->PrtclWait > 0){
      ptr->PrtclWait--;
    }else{
      //�p�[�e�B�N���W�F�l���[�g
      Generate(ptr);
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

//���f���A�j��1�Đ�
static BOOL PlayMdlAnm1(FLD3D_CI_PTR ptr)
{
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

//���f���A�j��2�Đ�
static BOOL PlayMdlAnm2(FLD3D_CI_PTR ptr)
{
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

static void SetupResource(FLD3D_CI_PTR ptr, RES_SETUP_DAT *outDat, const u8 inCutInNo)
{
  //�p�[�e�B�N������
  ptr->PrtclPtr = FLD_PRTCL_Create(ptr->PrtclSys);
  //�Z�b�g�A�b�v�f�[�^�쐬
  CreateRes(outDat, inCutInNo, ptr->HeapID);
  
  //3�c���f�����\�[�X���Z�b�g�A�b�v
  SetupResourceCore(ptr, &outDat->Setup);
  //�p�[�e�B�N�����\�[�X�Z�b�g�A�b�v
  {
    void *resource;
    resource = FLD_PRTCL_LoadResource(ptr->PrtclSys,
			ARCID_FLD3D_CI, outDat->SpaIdx);

    FLD_PRTCL_SetResource(ptr->PrtclSys, resource, TRUE, NULL);
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
  ptr->PrtclWait = outDat->SpaWait;
  ptr->MdlAnm1Wait = outDat->MdlAAnmWait;
  ptr->MdlAnm2Wait = outDat->MdlBAnmWait;
  
}

static void DeleteResource(FLD3D_CI_PTR ptr, RES_SETUP_DAT *outDat)
{
  DeleteRes(outDat);
  GFL_G3D_UTIL_DelUnit( ptr->Util, ptr->UnitIdx );
  ptr->UnitIdx = UNIT_NONE;
  FLD_PRTCL_Delete(ptr->PrtclSys);
}

static BOOL CheckAnmEnd(FLD3D_CI_PTR ptr)
{
  //�p�[�e�B�N���I������
  if ( !FLD_PRTCL_CheckEmitEnd( ptr->PrtclSys ) ){
    return FALSE;
  }

  return TRUE;
}



static void Generate(FLD3D_CI_PTR ptr)
{
///  GFL_PTC_SetCameraType(sys->PrtclPtr, GFL_G3D_PRJORTH);
	GFL_PTC_CreateEmitterCallback(ptr->PrtclPtr, 0,
									ParticleCallBack, NULL);
/**  
	GFL_PTC_CreateEmitterCallback(ptr->PrtclPtr, 1,
									ParticleCallBack, NULL);
	GFL_PTC_CreateEmitterCallback(ptr->PrtclPtr, 2,
									ParticleCallBack, NULL);
*/                  
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
 * @param	
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
  outDat->AnmNum = anm1_num+anm2_num;
  //�E�F�C�g�L��
  outDat->SpaWait = def_dat.SpaWait;
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
 * @param	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void DeleteRes(RES_SETUP_DAT *outDat)
{
  if (outDat->Anm1 != NULL){
    GFL_HEAP_FreeMemory( outDat->Anm1 );
  }
  if (outDat->Anm2 != NULL){
    GFL_HEAP_FreeMemory( outDat->Anm2 );
  }
  if (outDat->Obj != NULL){
    GFL_HEAP_FreeMemory( outDat->Obj );
  }
  if(outDat->Res != NULL){
    GFL_HEAP_FreeMemory( outDat->Res );
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
      child = CreateCutInEvt(wk->gsys, wk->CiPtr, 0);
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
      return GMEVENT_RES_FINISH;
    }
  }

  return GMEVENT_RES_CONTINUE;
}

//�L���v�`�����N�G�X�g
static void ReqCapture(FLD3D_CI_PTR ptr)
{
  ptr->CapEndFlg = FALSE;
  GX_SetBankForLCDC(GX_VRAM_LCDC_D);
  GFUser_VIntr_CreateTCB(Graphic_Tcb_Capture, &ptr->CapEndFlg, 0 );
}


#endif  //PM_DEBUG

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
  ptr->Mask = GX_GetVisiblePlane();
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
  GX_SetVisiblePlane( ptr->Mask );
}




