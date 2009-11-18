//======================================================================
/**
 * @file	fldmmdl_blact.c
 * @brief	���샂�f�� �r���{�[�h�A�N�^�[�Ǘ�
 * @author	kagaya
 * @date	05.07.13
 */
//======================================================================
#include "fldmmdl.h"


//--------------------------------------------------------------
//	debug
//--------------------------------------------------------------

//DEBUG ���\�[�X�������g�p�ʂ̌���
//�t�B�[���h�}�b�v�������g�p�ʒ����̂��ߍ쐬
//�e�N�X�`�����\�[�X�̊m�ہA�j�����ɁA�������m�ۃT�C�Y���v�Z����B
//091117 tomoya takahashi
//
//kagaya����ց@�\�[�X��ҏW���ɂ����Ȃǂ�������A�����Ă��������B
#ifdef DEBUG_MMDL_RESOURCE_MEMORY_SIZE

static u32 DEBUG_MMDL_RESOURCE_MemorySize = 0;  // ���\�[�X
// ���\�[�X�������[�T�C�Y�ǉ�
#define DEBUG_MMDL_RESOURCE_MEMORY_SIZE_Plus( x )    DEBUG_MMDL_RESOURCE_MemorySize += GFL_HEAP_DEBUG_GetMemoryBlockSize((x))
// ���\�[�X�������[�T�C�Y�폜
#define DEBUG_MMDL_RESOURCE_MEMORY_SIZE_Minus( x )    DEBUG_MMDL_RESOURCE_MemorySize -= GFL_HEAP_DEBUG_GetMemoryBlockSize((x))
// ���\�[�X�������[�T�C�Y �N���[���`�F�b�N
#define DEBUG_MMDL_RESOURCE_MEMORY_SIZE_IsAllDelete    GF_ASSERT( DEBUG_MMDL_RESOURCE_MemorySize == 0 )

#else   // DEBUG_MMDL_RESOURCE_MEMORY_SIZE

// ���\�[�X�������[�T�C�Y�ǉ�
#define DEBUG_MMDL_RESOURCE_MEMORY_SIZE_Plus( x )   /**/ 
// ���\�[�X�������[�T�C�Y�폜
#define DEBUG_MMDL_RESOURCE_MEMORY_SIZE_Minus( x )    /**/
// ���\�[�X�������[�T�C�Y �N���[���`�F�b�N
#define DEBUG_MMDL_RESOURCE_MEMORY_SIZE_IsAllDelete   /**/ 

#endif  // DEBUG_MMDL_RESOURCE_MEMORY_SIZE



//======================================================================
//	define
//======================================================================
#define REGIDCODE_MAX (0xffff) ///<BBDRESID�ő�
#define BLACT_RESID_NULL (0xffff) ///<�r���{�[�h���\�[�X ����ID
//#define RES_DIGEST_FRAME_MAX (4) ///<1�t���[���ɏ����ł��郊�\�[�X��
#define RES_DIGEST_FRAME_MAX (1) ///<1�t���[���ɏ����ł��郊�\�[�X��

typedef enum
{
  BBDRESBIT_GUEST = (1<<0), ///<���\�[�X���� �Q�X�g�o�^ OFF��=���M�����[
  BBDRESBIT_TRANS = (1<<1), ///<���\�[�X���� ON=VRAM�]���p���\�[�X
  
  ///���\�[�X�@VRAM�풓�@���M�����[
  BBDRES_VRAM_REGULAR =(0),
  ///���\�[�X�@VRAM�]���@���M�����[
  BBDRES_TRANS_REGULAR =(BBDRESBIT_TRANS),
  ///���\�[�X�@VRAM�풓�@�Q�X�g
  BBDRES_VRAM_GUEST=(BBDRESBIT_GUEST),
  ///���\�[�X�@VRAM�]���@�Q�X�g
  BBDRES_TRANS_GUEST=(BBDRESBIT_TRANS|BBDRESBIT_GUEST),
}BBDRESBIT;

///�r���{�[�h�`��X�P�[���B(x2 = �`��T�C�Y1/2)
#define MMDL_BBD_GLOBAL_SCALE ((FX32_ONE+0xc00)*2)
///�r���{�[�h�`��X�P�[�����{�B(x2 = �`��T�C�Y1/2)
#define MMDL_BBD_GLOBAL_SCALE_ONE (FX32_ONE*2)
///�r���{�[�h�`��T�C�Y (FX16_ONE*4 = 1/2)
//#define MMDL_BBD_DRAW_SIZE (FX16_ONE*8-1)
#define MMDL_BBD_DRAW_SIZE (FX16_ONE*4)

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
///	IDCODE
//--------------------------------------------------------------
typedef struct
{
	u16 code; ///<�o�^�R�[�h
	u16 id; ///<�o�^�R�[�h����Ԃ�ID
  u16 flag; ///<�o�^���e��\���t���O
  u16 dmy;
}IDCODE;

//--------------------------------------------------------------
///	IDCODEIDX
//--------------------------------------------------------------
typedef struct
{
	int max;
	IDCODE *pIDCodeBuf;
}IDCODEIDX;

//--------------------------------------------------------------
/// ADDRES_RESERVE_PARAM
//--------------------------------------------------------------
typedef struct
{
  u16 code;
  BBDRESBIT flag;
}ADDRES_RESERVE_PARAM;

//--------------------------------------------------------------
/// ADDRES_RESERVE
//--------------------------------------------------------------
typedef struct
{
  BOOL compFlag; //�f�[�^�̐ݒ肪���������t���O
  
  u16 code; //�o�^�p�R�[�h
  BBDRESBIT flag; //�o�^�p�t���O
  GFL_G3D_RES *pG3dRes; //�o�^���\�[�X
}ADDRES_RESERVE;

//--------------------------------------------------------------
/// ADDACT_RESERVE
//--------------------------------------------------------------
typedef struct
{
  BOOL compFlag; //�f�[�^�̐ݒ肪���������t���O
  
  MMDL *mmdl;
  u16 code;
  u16 dummy;
  u16 *outID;
  GFL_G3D_RES *pTransActRes; ///<�]���p�A�N�^�[���\�[�X
  
  ///���[�U�[�����w�肷��A�N�^�[�ǉ��������ɌĂяo���֐�
  MMDL_BLACTCONT_ADDACT_USERPROC user_init_proc;
  ///���[�U�[�����w�肷��user_init_proc�Őݒ肷�郏�[�N
  void *user_init_work;
  ///���[�U�[�����w�肷��A�N�^�[�ǉ����̍��W
  VecFx32 user_add_pos;
}ADDACT_RESERVE;

//--------------------------------------------------------------
/// BLACT_RESERVE
//--------------------------------------------------------------
typedef struct
{
  BOOL funcFlag; //TRUE=�\�񏈗������s����
  u16 resMax;
  u16 resDigestFrameMax;
  u16 actMax;
  u16 dmy;
  ADDRES_RESERVE_PARAM *pReserveResParam;
  ADDRES_RESERVE *pReserveRes;
  ADDACT_RESERVE *pReserveAct;
}BLACT_RESERVE;

//--------------------------------------------------------------
///	MMDL_BLACTCONT
//--------------------------------------------------------------
struct _TAG_MMDL_BLACTCONT
{
  int resourceMax;
  
	GFL_BBDACT_SYS *pBbdActSys; //���[�U�[����
	GFL_BBDACT_RESUNIT_ID bbdActResUnitID;
	GFL_BBDACT_ACTUNIT_ID bbdActActUnitID;
	u16 bbdActResCount;
	u16 bbdActActCount;
  
	MMDLSYS *mmdlsys;
  
	IDCODEIDX BBDResUnitIdx;
  BLACT_RESERVE *pReserve;
};

//======================================================================
//	proto
//======================================================================
static void BlActFunc( GFL_BBDACT_SYS *bbdActSys, int actIdx, void *work );

static void IDCodeIndex_Init( IDCODEIDX *idx, int max, HEAPID heapID );
static void IDCodeIndex_RegistCode(
    IDCODEIDX *idx, u16 code, u16 id, u16 flag );
static void IDCodeIndex_DeleteCode( IDCODEIDX *idx, u16 code );
static BOOL IDCodeIndex_SearchCode(
    IDCODEIDX *idx, u16 code, u16 *outID, u16 *outFlag );
static void BBDResUnitIndex_Init( MMDL_BLACTCONT *pBlActCont, int max );
static void BBDResUnitIndex_Delete( MMDL_BLACTCONT *pBlActCont );
static void BBDResUnitIndex_AddResource( MMDL_BLACTCONT *pBlActCont,
    GFL_G3D_RES *g3dres, u16 obj_code, BBDRESBIT flag );
static void BBDResUnitIndex_AddResUnit(
    MMDL_BLACTCONT *pBlActCont, u16 obj_code, BBDRESBIT flag );
static void BBDResUnitIndex_RemoveResUnit(
    MMDL_BLACTCONT *pBlActCont, u16 obj_code );
static BOOL BBDResUnitIndex_SearchResID(
  MMDL_BLACTCONT *pBlActCont, u16 obj_code, u16 *outID, u16 *outFlag );

static void BlActAddReserve_Init( MMDL_BLACTCONT *pBlActCont );
static void BlActAddReserve_Delete( MMDL_BLACTCONT *pBlActCont );
static void BlActAddReserve_RegistResourceParam(
    MMDL_BLACTCONT *pBlActCont, u16 code, BBDRESBIT flag );
static void BlActAddReserve_DigestResourceParam(
    MMDL_BLACTCONT *pBlActCont );
static BOOL BlActAddReserve_RegistResource(
    MMDL_BLACTCONT *pBlActCont, u16 code, BBDRESBIT flag );
static void BlActAddReserve_DigestResource( MMDL_BLACTCONT *pBlActCont );
static BOOL BlActAddReserve_SearchResource(
    MMDL_BLACTCONT *pBlActCont, u16 code, u16 *outFlag );
static BOOL BlActAddReserve_CancelResource(
    MMDL_BLACTCONT *pBlActCont, u16 code );
static void BlActAddReserve_RegistActor(
    MMDL_BLACTCONT *pBlActCont, MMDL *mmdl, u16 code, u16 *outID,
    MMDL_BLACTCONT_ADDACT_USERPROC init_proc,
    void *init_work, const VecFx32 *user_pos );
static void BlActAddReserve_DigestActorCore(
    MMDL_BLACTCONT *pBlActCont, ADDACT_RESERVE *pRes );
static void BlActAddReserve_DigestActor( MMDL_BLACTCONT *pBlActCont );
static BOOL BlActAddReserve_SearchActorOBJCode(
    MMDL_BLACTCONT *pBlActCont, u16 code, MMDL *mmdl );
static BOOL BlActAddReserve_SearchActorOBJCodeOutID(
    MMDL_BLACTCONT *pBlActCont, u16 code, const u16 *outID );
static void BlActAddReserve_CancelActor(
    MMDL_BLACTCONT *pBlActCont, MMDL *mmdl );
static void BlActAddReserve_ClearWork( ADDACT_RESERVE *pRes );

static const MMDL_BBDACT_ANMTBL * BlActAnm_GetAnmTbl( u32 no );
static GFL_BBDACT_RESUNIT_ID BlActRes_AddRes(
    MMDL_BLACTCONT *pBlActCont, u16 code,
    GFL_G3D_RES *g3dres, GFL_BBDACT_RESTYPE type );

//test data
/*
const GFL_BBDACT_RESDATA testResTable[];
const u32 testResTableMax;
*/

//======================================================================
//	�t�B�[���h���샂�f���@�r���{�[�h�A�N�^�[�Ǘ�
//======================================================================
//--------------------------------------------------------------
/**
 * MMDL_BLACTCONT �r���{�[�h�A�N�^�[�Ǘ��@�Z�b�g�A�b�v
 * @param	mmdlsys	�\�z�ς݂�MMDLSYS
 * @param	pBbdActSys	�\�z�ς݂�GFL_BBDACT_SYS
 * @param res_max ���\�[�X���ő�
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_BLACTCONT_Setup( MMDLSYS *mmdlsys,
	GFL_BBDACT_SYS *pBbdActSys, int res_max )
{
	HEAPID heapID;
	GFL_BBDACT_ACTDATA *actData;
	GFL_BBDACT_ACTUNIT_ID actUnitID;
	MMDL_BLACTCONT *pBlActCont;
	
	heapID = MMDLSYS_GetHeapID( mmdlsys );
	pBlActCont = GFL_HEAP_AllocClearMemory( heapID, sizeof(MMDL_BLACTCONT) );
	pBlActCont->mmdlsys = mmdlsys;
	pBlActCont->pBbdActSys = pBbdActSys;
  pBlActCont->resourceMax = res_max;
  
	MMDLSYS_SetBlActCont( mmdlsys, pBlActCont );
	
	BBDResUnitIndex_Init( pBlActCont, res_max );
  BlActAddReserve_Init( pBlActCont );
  
  { //Parameter
    GFL_BBD_SYS *bbdSys = GFL_BBDACT_GetBBDSystem( pBbdActSys );

    { //Scale
      VecFx32 scale =
      {MMDL_BBD_GLOBAL_SCALE,MMDL_BBD_GLOBAL_SCALE,MMDL_BBD_GLOBAL_SCALE};
      GFL_BBD_SetScale( bbdSys, &scale );
    }
    
    { //Poligon ID
      u8 id = 0;
      GFL_BBD_SetPolID( bbdSys, &id );
    }

    { // Normal ����50�x
      VecFx16 normal = { 0,FX16_CONST(1.19175f),FX16_CONST(1.0f) };
      
      VEC_Fx16Normalize( &normal, &normal );
      
      GFL_BBD_SetUseCustomVecN( bbdSys, &normal );
    }
  }
}

//--------------------------------------------------------------
/**
 * MMDL_BLACTCONT �r���{�[�h�A�N�^�[�Ǘ��@�폜
 * @param	mmdlsys	MMDLSYS
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_BLACTCONT_Release( MMDLSYS *mmdlsys )
{
	MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( mmdlsys );
  BlActAddReserve_Delete( pBlActCont );
	BBDResUnitIndex_Delete( pBlActCont );
  
	GFL_HEAP_FreeMemory( pBlActCont );
	MMDLSYS_SetBlActCont( mmdlsys, NULL );


  // �S�J���`�F�b�N
//  DEBUG_MMDL_RESOURCE_MEMORY_SIZE_IsAllDelete;
}

//--------------------------------------------------------------
/**
 * MMDL_BLACTCONT �X�V����
 * @param mmdlsys MMDLSYS
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_BLACTCONT_Update( MMDLSYS *mmdlsys )
{
	MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( mmdlsys );
  
  if( pBlActCont != NULL ){
    if( pBlActCont->pReserve != NULL ){ //�\�����
      //�A�N�^�[�\�����
      BlActAddReserve_DigestActor( pBlActCont );
      //���\�[�X�p�����^�\�����
      BlActAddReserve_DigestResourceParam( pBlActCont );
    }
  }
}

//--------------------------------------------------------------
/**
 * MMDL_BLACTCONT V�u�����N����
 * @param	mmdlsys	MMDLSYS
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_BLACTCONT_ProcVBlank( MMDLSYS *mmdlsys )
{
  MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( mmdlsys );
  BlActAddReserve_DigestResource( pBlActCont );
}

#if 0
//--------------------------------------------------------------
/**
 * MMDL_BLACTCONT GFL_BBDACT_SYS�擾
 * @param mmdlsys MMDLSYS*
 * @retval GFL_BBDACT_SYS*
 */
//--------------------------------------------------------------
GFL_BBDACT_SYS * MMDL_BLACTCONT_GetBlActSys( MMDLSYS *mmdlsys )
{
	MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( mmdlsys );
  GF_ASSERT( pBlActCont != NULL );
  return( pBlActCont->pBbdActSys );
}
#endif

//--------------------------------------------------------------
/**
 * MMDLSYS �r���{�[�h�A�N�^�[�X�P�[���𓙔{��
 * @param mmdlsys MMDLSYS
 * @retval nothing
 */
//--------------------------------------------------------------
void MMDL_BLACTCONT_SetGlobalScaleOne( MMDLSYS *mmdlsys )
{
	MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( mmdlsys );
  VecFx32 scale = {MMDL_BBD_GLOBAL_SCALE_ONE,
    MMDL_BBD_GLOBAL_SCALE_ONE,MMDL_BBD_GLOBAL_SCALE_ONE};
  GFL_BBD_SYS *bbdSys = GFL_BBDACT_GetBBDSystem( pBlActCont->pBbdActSys );
  GFL_BBD_SetScale( bbdSys, &scale );
}

//======================================================================
//	�t�B�[���h���샂�f���@�r���{�[�h���\�[�X
//======================================================================
//--------------------------------------------------------------
/**
 * MMDL_BLACTCONT ���\�[�X�ǉ� ���M�����[�AVRAM�풓�^�œo�^
 * @param	mmdlsys	MMDLSYS
 * @param	code �\���R�[�h�z��
 * @param	max code�v�f��
 * @retval	nothing
 * @note �Ă΂ꂽ���̏�œǂݍ��݂���������B
 */
//--------------------------------------------------------------
void MMDL_BLACTCONT_AddResourceTex(
	MMDLSYS *mmdlsys, const u16 *code, int max )
{
  MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( mmdlsys );
	while( max ){
		BBDResUnitIndex_AddResUnit( pBlActCont, *code, BBDRES_VRAM_REGULAR );
		code++;
		max--;
	}
}

//======================================================================
//	�t�B�[���h���샂�f�� �r���{�[�h�A�N�^�[�ǉ�
//======================================================================
//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[�@�A�N�^�[�ǉ�
 * @param mmdlsys MMDLSYS
 * @retval GFL_BBDACT_ACYUNIT_ID
 */
//--------------------------------------------------------------
static GFL_BBDACT_ACTUNIT_ID blact_AddActorCore(
  MMDLSYS *mmdlsys , const VecFx32 *pos, u16 code, u16 resID )
{
	GFL_BBDACT_ACTDATA actData;
	GFL_BBDACT_ACTUNIT_ID actID;
  
	MMDL_BLACTCONT *pBlActCont =
    MMDLSYS_GetBlActCont( mmdlsys );

  actData.resID = resID;
	actData.sizX = MMDL_BBD_DRAW_SIZE;
	actData.sizY = MMDL_BBD_DRAW_SIZE;
  
	actData.trans = *pos;
	actData.alpha = 31;
	actData.drawEnable = TRUE;
	actData.lightMask = GFL_BBD_LIGHTMASK_01;
	actData.work = NULL;
	actData.func = BlActFunc;
	
	actID = GFL_BBDACT_AddAct(
		pBlActCont->pBbdActSys, pBlActCont->bbdActResUnitID, &actData, 1 );
   
  {
	  const OBJCODE_PARAM *prm =
      MMDLSYS_GetOBJCodeParam( mmdlsys, code );
	  const MMDL_BBDACT_ANMTBL *anmTbl =
      BlActAnm_GetAnmTbl( prm->anm_id );
    
	  if( anmTbl->pAnmTbl != NULL ){
		  GFL_BBDACT_SetAnimeTable( pBlActCont->pBbdActSys,
        actID, (GFL_BBDACT_ANMTBL)anmTbl->pAnmTbl, anmTbl->anm_max );
	    GFL_BBDACT_SetAnimeIdxOn( pBlActCont->pBbdActSys, actID, 0 );
    }
	}
	
	return( actID );
}

//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[ �A�N�^�[�ǉ��@���샂�f����
 * @param	mmdl	MMDL
 * @param	resID �ǉ�����A�N�^�[�Ŏg�p���郊�\�[�X�C���f�b�N�X
 * @retval GFL_BBDACT_ACTUNIT_ID
 */
//--------------------------------------------------------------
static GFL_BBDACT_ACTUNIT_ID blact_AddActor(
    MMDL *mmdl, u16 code, u16 resID )
{
	VecFx32 pos;
  MMDLSYS *mmdlsys = (MMDLSYS*)MMDL_GetMMdlSys( mmdl );
  MMDL_GetDrawVectorPos( mmdl, &pos );
  return( blact_AddActorCore(mmdlsys,&pos,code,resID) );
}

//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[�@���\�[�X����A�N�^�[�Z�b�g
 * @param pos �\�����W
 * @param code �\���R�[�h
 * @param resID �A�N�^�[���\�[�XID
 * @param transResID �]���p���\�[�X
 * @retval GFL_BBDACT_ACTUNIT_ID
 */
//--------------------------------------------------------------
static GFL_BBDACT_ACTUNIT_ID blact_SetResActorCore(
    MMDLSYS *mmdlsys,
    const VecFx32 *pos, u16 code, u16 resID, u16 transResID )
{
	GFL_BBDACT_ACTUNIT_ID actID;
  
  actID = blact_AddActorCore( mmdlsys, pos, code, resID );
  
  if( transResID != BLACT_RESID_NULL ){ //�]���p���\�[�X�ƌ���
	  MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( mmdlsys );
    GFL_BBDACT_BindActTexRes( pBlActCont->pBbdActSys, actID, transResID );
  }
  
  return( actID );
}

//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[�@���\�[�X����A�N�^�[�Z�b�g�@���샂�f����
 * @param mmdl MMDL
 * @param code �\���R�[�h
 * @param resID �A�N�^�[���\�[�XID
 * @param transResID �]���p���\�[�X
 * @retval GFL_BBDACT_ACTUNIT_ID
 */
//--------------------------------------------------------------
static GFL_BBDACT_ACTUNIT_ID blact_SetResActor(
    MMDL *mmdl, u16 code, u16 resID, u16 transResID )
{
  VecFx32 pos;
	MMDLSYS *mmdlsys = (MMDLSYS*)MMDL_GetMMdlSys( mmdl );
  MMDL_GetDrawVectorPos( mmdl, &pos );
  return( blact_SetResActorCore(mmdlsys,&pos,code,resID,transResID) );
}

#if 0
//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[�@�]���^�A�N�^�[�Z�b�g
 * @param mmdl MMDL
 * @param code �\���R�[�h
 * @param transResID �]���p���\�[�XID
 * @param pTransActRes �]���p�r���{�[�h���\�[�XID
 * @retval GFL_BBDACT_ACTUNIT_ID
 */
//--------------------------------------------------------------
static GFL_BBDACT_ACTUNIT_ID blact_SetTransActor(
    MMDL *mmdl, u16 code, u16 transResID, GFL_G3D_RES pTransActRes )
{
  u16 resID;
	GFL_BBDACT_ACTUNIT_ID actID;
	MMDLSYS *mmdlsys = (MMDLSYS*)MMDL_GetMMdlSys( mmdl );
	MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( mmdlsys );
	
  {
  	const OBJCODE_PARAM *prm;
  	GFL_BBDACT_G3DRESDATA data;
	  prm = MMDLSYS_GetOBJCodeParam( pBlActCont->mmdlsys, obj_code );
    
    data.g3dres = g3dres;
	  data.texFmt = GFL_BBD_TEXFMT_PAL16;
	  data.texSiz = prm->tex_size;
	  data.celSizX = 32;				//������mdl_size����
	  data.celSizY = 32;
  }

    actID = blact_AddActor( mmdl, code, resID );
  
  GFL_BBDACT_ACTUNIT_ID blact_SetResActor(
    MMDL *mmdl, u16 code, u16 resID, resID );
  u16 transResID )
  if( transResID != BLACT_RESID_NULL ){ //�]���p���\�[�X�ƌ���
    GFL_BBDACT_BindActTexRes( pBlActCont->pBbdActSys, actID, transResID );
  }
   
	return( actID );
}
#endif

//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[�@�A�N�^�[�Z�b�g
 * @param mmdl MMDL
 * @param code �\���R�[�h
 * @retval nothing
 */
//--------------------------------------------------------------
#if 0
static GFL_BBDACT_ACTUNIT_ID blact_SetActor( MMDL *mmdl, u16 code )
{
  u16 resID,flag;
	GFL_BBDACT_ACTUNIT_ID actID;
	MMDLSYS *mmdlsys = (MMDLSYS*)MMDL_GetMMdlSys( mmdl );
	MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( mmdlsys );
	
	BBDResUnitIndex_SearchResID( pBlActCont, code, &resID, &flag );
	
  if( resID == REGIDCODE_MAX ){ //��o�^���\�[�X
    MMDL_BLACTCONT_AddOBJCodeRes( mmdlsys, code,  FALSE, TRUE );
	  BBDResUnitIndex_SearchResID( pBlActCont, code, &resID, &flag );
    GF_ASSERT( resID != REGIDCODE_MAX );
    
    #ifdef DEBUG_MMDL
    KAGAYA_Printf( "MMDL ADD GUEST RESOURCE %xH\n", code );
    #endif
  }
  
  if( (flag&BBDRESBIT_TRANS) ){ //�]���^ �]���惊�\�[�X�쐬
    //�]���p���\�[�X���擾���ǉ��B
    //resID = �A�N�^�[�p�̃��\�[�X
    GF_ASSERT( 0 ); //���󖢑Ή�
  }
  
  actID = blact_AddActor( mmdl, code, resID );
  
  if( (flag&BBDRESBIT_TRANS) ){ //�]���^ ���\�[�X�ƌ���
	  BBDResUnitIndex_SearchResID( pBlActCont, code, &resID, &flag );
    GFL_BBDACT_BindActTexRes( pBlActCont->pBbdActSys, actID, resID );
  }
  
	return( actID );
}
#endif

static GFL_BBDACT_ACTUNIT_ID blact_SetActor( MMDL *mmdl, u16 code )
{
  u16 resID,flag;
	GFL_BBDACT_ACTUNIT_ID actID;
	MMDLSYS *mmdlsys = (MMDLSYS*)MMDL_GetMMdlSys( mmdl );
	MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( mmdlsys );
	
	BBDResUnitIndex_SearchResID( pBlActCont, code, &resID, &flag );
	GF_ASSERT( resID != REGIDCODE_MAX ); //���\�[�X����
  GF_ASSERT( !(flag&BBDRESBIT_TRANS) ); //�]���^�C�v
  
  actID = blact_AddActor( mmdl, code, resID );
	return( actID );
}

#if 0
//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[ �ǉ�
 * @param	mmdl	MMDL
 * @param	code �g�p����OBJ�R�[�h
 * @param outID �ǉ��A�N�^�[ID�i�[�� MMDL_BBDACT_ACTID_NULL=�ǉ���
 * @retval BOOL TRUE=�ǉ��BFALSE=�ǉ����B
 */
//--------------------------------------------------------------
GFL_BBDACT_ACTUNIT_ID MMDL_BLACTCONT_AddActor( MMDL *mmdl, u32 code )
{
  u16 flag;
	VecFx32 pos;
	GFL_BBDACT_ACTDATA actData;
	GFL_BBDACT_ACTUNIT_ID actID;
	const MMDL_BBDACT_ANMTBL *anmTbl;
	const OBJCODE_PARAM *prm;
	MMDLSYS *mmdlsys = (MMDLSYS*)MMDL_GetMMdlSys( mmdl );
	MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( mmdlsys );
	
	prm = MMDLSYS_GetOBJCodeParam( mmdlsys, code );
	MMDL_GetDrawVectorPos( mmdl, &pos );
	
	BBDResUnitIndex_SearchResID( mmdlsys, code, &actData.resID, &flag );
	
  if( actData.resID == REGIDCODE_MAX ){ //��o�^���\�[�X
    MMDL_BLACTCONT_AddOBJCodeRes( mmdlsys, code,  FALSE, TRUE );
	  BBDResUnitIndex_SearchResID( mmdlsys, code, &actData.resID, &flag );
    GF_ASSERT( actData.resID != REGIDCODE_MAX );
    
    #ifdef DEBUG_MMDL
    KAGAYA_Printf( "MMDL ADD GUEST RESOURCE %xH\n", code );
    #endif
  }
  
	actData.sizX = FX16_ONE*8-1;
	actData.sizY = FX16_ONE*8-1;
	actData.trans = pos;
	actData.alpha = 31;
	actData.drawEnable = TRUE;
	actData.lightMask = GFL_BBD_LIGHTMASK_01;
	actData.work = mmdl;
	actData.func = BlActFunc;
	
	actID = GFL_BBDACT_AddAct(
		pBlActCont->pBbdActSys, pBlActCont->bbdActResUnitID, &actData, 1 );
  
#if 0 //�]���p���\�[�X�ƃo�C���h
  if( flag & == HERO ){
//    GFL_BBDACT_BindActTexRes( pBlActCont->pBbdActSys, actID, actData.resID );
  }
#endif

	anmTbl = BlActAnm_GetAnmTbl( prm->anm_id );
	
	if( anmTbl->pAnmTbl != NULL ){
		GFL_BBDACT_SetAnimeTable( pBlActCont->pBbdActSys,
			actID, (GFL_BBDACT_ANMTBL)anmTbl->pAnmTbl, anmTbl->anm_max );
	}
	
	GFL_BBDACT_SetAnimeIdxOn(
			pBlActCont->pBbdActSys, actID, 0 );
	return( actID );
}
#endif

//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[ �ǉ�
 * @param	mmdl	MMDL
 * @param	code �g�p����OBJ�R�[�h
 * @param outID �ǉ��A�N�^�[ID�i�[�� MMDL_BLACTID_NULL=�ǉ���
 * @retval BOOL TRUE=�ǉ��BFALSE=�ǉ����B
 */
//--------------------------------------------------------------
BOOL MMDL_BLACTCONT_AddActor(
    MMDL *mmdl, u16 code, GFL_BBDACT_ACTUNIT_ID *outID )
{
  u16 resID,flag;
	MMDLSYS *mmdlsys = (MMDLSYS*)MMDL_GetMMdlSys( mmdl );
	MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( mmdlsys );
  
	BBDResUnitIndex_SearchResID( pBlActCont, code, &resID, &flag );
	
  if( resID != REGIDCODE_MAX && (flag&BBDRESBIT_TRANS) == 0 ){
    (*outID) = blact_SetActor( mmdl, code );
    return( TRUE );
  }
  
  *outID = MMDL_BLACTID_NULL; //�\��o�^
  BlActAddReserve_RegistActor(
      pBlActCont, mmdl, code, outID, NULL, NULL, NULL );
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[ �폜
 * @param	mmdl	MMDL*
 * @param	actID GFL_BBDACT_ACTUNIT_ID
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_BLACTCONT_DeleteActor( MMDL *mmdl, u32 actID )
{
  u16 id,flag;
  u16 code = MMDL_GetOBJCode( mmdl );
	MMDLSYS *pMMdlSys = (MMDLSYS*)MMDL_GetMMdlSys( mmdl );
	MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( pMMdlSys );
  
  if( actID == MMDL_BLACTID_NULL ){ //�܂��ǉ�����Ă��Ȃ��B
    BlActAddReserve_CancelActor( pBlActCont, mmdl ); //�\�񂠂�΃L�����Z��
    return;
  }
  
  if( BBDResUnitIndex_SearchResID(pBlActCont,code,&id,&flag) == TRUE )
  {
    if( (flag&BBDRESBIT_TRANS) ) //�]���^ �A�N�^�[�̓]���p���\�[�X�폜
    {
      u16 idx = GFL_BBDACT_GetResIdx( pBlActCont->pBbdActSys, actID );
	    GFL_BBDACT_RemoveResourceUnit( pBlActCont->pBbdActSys, idx, 1 );
    }
    
    if( (flag&BBDRESBIT_GUEST) ) //�Q�X�g�o�^ ���ɗ��p������΍폜
    {
      if( MMDL_SearchUseOBJCode(mmdl,code) == FALSE )
      {
        BBDResUnitIndex_RemoveResUnit( pBlActCont, code );
        
        KAGAYA_Printf(
          "MMDL DEL GUEST RESOURCE CODE=%d, RESID=%d\n", code, id );
      }
    }
  }
  
	GFL_BBDACT_RemoveAct( pBlActCont->pBbdActSys, actID, 1 );
}

//======================================================================
//  �t�B�[���h���샂�f�� �r���{�[�h�A�N�^�[�ǉ� ���[�U�[�p
//======================================================================
//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[�@�A�N�^�[�ǉ��@���[�U�[�p
 * @param mmdlsys MMDLSYS*
 * @param code �g�p����OBJ�R�[�h
 * @param outID �ǉ��A�N�^�[ID�i�[�� MMDL_BLACTID_NULL=�ǉ���
 * @param pos �\�����W
 * @param init_proc �A�N�^�[�ǉ��������ɌĂяo���֐� NULL=�Ăяo������
 * @param init_work init_proc�Ɏw�肷��������[�N
 * @retval BOOL TRUE=�ǉ��BFALSE=�ǉ����B
 * @note �K�v�ȃ��\�[�X�͗\�ߒǉ�����Ă��鎖���O��B
 */
//--------------------------------------------------------------
BOOL MMDL_BLACTCONT_USER_AddActor( MMDLSYS *mmdlsys,
    u16 code, MMDL_BLACTWORK_USER *userAct, const VecFx32 *pos,
    MMDL_BLACTCONT_ADDACT_USERPROC init_proc, void *init_work )
{
  u16 resID,flag;
	MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( mmdlsys );
  
	BBDResUnitIndex_SearchResID( pBlActCont, code, &resID, &userAct->flag );
  GF_ASSERT( resID != REGIDCODE_MAX ); //���\�[�X����
  
  if( (userAct->flag&BBDRESBIT_TRANS) == 0 ){
    userAct->actID = blact_AddActorCore( mmdlsys, pos, code, resID );
    return( TRUE );
  }
  
  userAct->actID = MMDL_BLACTID_NULL; //���\�[�X�]���^
  
  BlActAddReserve_RegistActor(
      pBlActCont, NULL, code, &userAct->actID, init_proc, init_work, pos );
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[�@�A�N�^�[�폜�@���[�U�[�p
 * @param mmdlsys MMDLSYS*
 * @param actID MMLD_BLACTCONT_USER_AddActor()�w�肵��ID�i�[�|�C���^
 * @retval nothing
 * @note actID�͎g�p���Ă���A�N�^�[ID���i�[����Ă��鎖�B
 */
//--------------------------------------------------------------
void MMDL_BLACTCONT_USER_DeleteActor(
    MMDLSYS *mmdlsys, MMDL_BLACTWORK_USER *userAct )
{
	MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( mmdlsys );
  
  if( userAct->actID == MMDL_BLACTID_NULL ){ //�܂��ǉ�����Ă��Ȃ�
//BlActAddReserve_CancelActor( pBlActCont, mmdl ); //�\�񂠂�΃L�����Z��
  }else{
    if( (userAct->flag&BBDRESBIT_TRANS) ){ //�]���^ �]���p���\�[�X�폜
      u16 idx = GFL_BBDACT_GetResIdx(
         pBlActCont->pBbdActSys, userAct->actID );
	    GFL_BBDACT_RemoveResourceUnit( pBlActCont->pBbdActSys, idx, 1 );
    }
  	GFL_BBDACT_RemoveAct( pBlActCont->pBbdActSys, userAct->actID, 1 );
  }
  
  userAct->flag = 0;
  userAct->actID = MMDL_BLACTID_NULL;
}

//======================================================================
//	�t�B�[���h���샂�f���@�r���{�[�h�A�N�^�[����
//======================================================================
//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[����֐�
 * @param	bbdActSys	GFL_BBDACT_SYS
 * @param	actIdx		int
 * @param	work		void*
 * @retval	nothing
 */
//--------------------------------------------------------------
static void BlActFunc( GFL_BBDACT_SYS *bbdActSys, int actIdx, void *work )
{
}

//======================================================================
//	�t�B�[���h���샂�f���@�r���{�[�h�A�N�^�[�Ǘ��@�Q��
//======================================================================
//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[�Ǘ��@GFL_BBDACT_SYS�擾
 * @param	pBlActCont MMDL_BLACTCONT
 * @retval	GFL_BBDACT_SYS
 */
//--------------------------------------------------------------
GFL_BBDACT_SYS * MMDL_BLACTCONT_GetBbdActSys(
		MMDL_BLACTCONT *pBlActCont )
{
	return( pBlActCont->pBbdActSys );
}

//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[�Ǘ� GFL_BBDACT_RESUNIT_ID�擾
 * @param	pBlActCont MMDL_BLACTCONT
 * @retval	GFL_BBDACT_RESUNIT_ID
 */
//--------------------------------------------------------------
GFL_BBDACT_RESUNIT_ID MMDL_BLACTCONT_GetResUnitID(
		MMDL_BLACTCONT *pBlActCont )
{
	return( pBlActCont->bbdActResUnitID );
}

//======================================================================
//	IDCODEIDX
//======================================================================
//--------------------------------------------------------------
/**
 * IDCODEIDX ������
 * @param	idx IDCODEIDX*
 * @param	max �o�^�ő吔
 * @param	heapID HEAPID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void IDCodeIndex_Init( IDCODEIDX *idx, int max, HEAPID heapID )
{
	IDCODE *tbl;
	GF_ASSERT( max );
	tbl = GFL_HEAP_AllocClearMemory( heapID, sizeof(IDCODE)*max );
	idx->max = max;
	idx->pIDCodeBuf = tbl;
	do{
		tbl->code = REGIDCODE_MAX;
		tbl->id = REGIDCODE_MAX;
		tbl++;
		max--;
	}while( max );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX �폜
 * @param	idx	IDCODEIDX*
 * @retval	nothing
 */
//--------------------------------------------------------------
static void IDCodeIndex_Delete( IDCODEIDX *idx )
{
	GF_ASSERT( idx->pIDCodeBuf );
	GFL_HEAP_FreeMemory( idx->pIDCodeBuf );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX �w�肳�ꂽ�R�[�h��ID�A�t���O��o�^
 * @param	idx IDCODEIDX
 * @param	code �R�[�h
 * @param	id ID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void IDCodeIndex_RegistCode(
    IDCODEIDX *idx, u16 code, u16 id, u16 flag )
{
	int i = 0;
	IDCODE *tbl = idx->pIDCodeBuf;
	do{
		if( tbl->code == REGIDCODE_MAX ){
			tbl->code = code;
			tbl->id = id;
      tbl->flag = flag;
			return;
		}
		tbl++;
		i++;
	}while( i < idx->max );
	GF_ASSERT( 0 );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX �w�肳�ꂽ�R�[�h���폜
 * @param	idx IDCODEIDX
 * @param	code OBJ�R�[�h
 * @retval	nothing
 */
//--------------------------------------------------------------
static void IDCodeIndex_DeleteCode( IDCODEIDX *idx, u16 code )
{
	int i = 0;
	IDCODE *tbl = idx->pIDCodeBuf;
	do{
		if( tbl->code == code ){
			tbl->code = REGIDCODE_MAX;
			tbl->id = REGIDCODE_MAX;
      tbl->flag = 0;
			return;
		}
		tbl++;
		i++;
	}while( i < idx->max );
	GF_ASSERT( 0 );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX �w�肳�ꂽ�R�[�h�Ɉ�v����ID�A�t���O��Ԃ�
 * @param	idx IDCODEIDX
 * @param	code OBJ�R�[�h
 * @param outID ID�i�[�� NULL=�i�[���Ȃ�
 * @param outFlag �t���O�i�[�� NULL=�i�[���Ȃ�
 * @retval BOOL TRUE=��v�L��AFALSE=�R�[�h�����݂��Ȃ�
 */
//--------------------------------------------------------------
static BOOL IDCodeIndex_SearchCode(
    IDCODEIDX *idx, u16 code, u16 *outID, u16 *outFlag )
{
	int i = 0;
	IDCODE *tbl = idx->pIDCodeBuf;
  
  if( outID != NULL ){
    *outID = REGIDCODE_MAX;
  }
  
	do{
		if( tbl->code == code ){
      if( outID != NULL ){
        *outID = tbl->id;
      }
      if( outFlag != NULL ){
        *outFlag = tbl->flag;
      }
			return( TRUE );
		}
		tbl++;
		i++;
	}while( i < idx->max );
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX BBDACT ������
 * @param	pBlActCont MMDL_BLACTCONT
 * @param	max	�o�^�ő吔
 * @retval	nothing
 */
//--------------------------------------------------------------
static void BBDResUnitIndex_Init( MMDL_BLACTCONT *pBlActCont, int max )
{
	HEAPID heapID = MMDLSYS_GetHeapID( pBlActCont->mmdlsys );
	IDCodeIndex_Init( &pBlActCont->BBDResUnitIdx, max, heapID );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX BBDACT �폜
 * @param	pBlActCont MMDL_BLACTCONT
 * @param	max	�o�^�ő吔
 * @retval	nothing
 */
//--------------------------------------------------------------
static void BBDResUnitIndex_Delete( MMDL_BLACTCONT *pBlActCont )
{
	int i = 0;
	IDCODE *tbl = pBlActCont->BBDResUnitIdx.pIDCodeBuf;
	
	do{
		if( tbl->code != REGIDCODE_MAX ){
			BBDResUnitIndex_RemoveResUnit( pBlActCont, tbl->code );
		}
		tbl++;
		i++;
	}while( i < pBlActCont->BBDResUnitIdx.max );
	
	IDCodeIndex_Delete( &pBlActCont->BBDResUnitIdx );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX BBDACT ���\�[�X�ǉ�
 * @param	pBlActCont MMDL_BLACTCONT
 * @param	code	OBJ�R�[�h
 * @param flag �o�^�t���O BBDRESBIT_VRAM��
 * @retval	nothing
 */
//--------------------------------------------------------------
static void BBDResUnitIndex_AddResource( MMDL_BLACTCONT *pBlActCont,
    GFL_G3D_RES *g3dres, u16 obj_code, BBDRESBIT flag )
{
	GFL_BBDACT_RESUNIT_ID id;
  GFL_BBDACT_RESTYPE type;
  
	type = GFL_BBDACT_RESTYPE_DATACUT; //��{�̓��\�[�X�j���^
  
  if( (flag&BBDRESBIT_TRANS) ){ //�]���p���\�[�X�w��
    type = GFL_BBDACT_RESTYPE_TRANSSRC;
  }
  
  id = BlActRes_AddRes( pBlActCont, obj_code, g3dres, type );
	IDCodeIndex_RegistCode( &pBlActCont->BBDResUnitIdx, obj_code, id, flag );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX BBDACT ���\�[�X���j�b�g�ǉ�
 * @param	pBlActCont MMDL_BLACTCONT
 * @param	code	OBJ�R�[�h
 * @param flag �o�^�t���O BBDRESBIT_VRAM��
 * @retval	nothing
 */
//--------------------------------------------------------------
static void BBDResUnitIndex_AddResUnit(
    MMDL_BLACTCONT *pBlActCont, u16 obj_code, BBDRESBIT flag )
{
  GFL_G3D_RES *g3dres;
	const OBJCODE_PARAM *prm;
  const OBJCODE_PARAM_BUF_BBD *prm_bbd;
  
	prm = MMDLSYS_GetOBJCodeParam( pBlActCont->mmdlsys, obj_code );
  prm_bbd = MMDL_GetOBJCodeParamBufBBD( prm );
  
  g3dres = GFL_G3D_CreateResourceHandle(
      MMDLSYS_GetResArcHandle(pBlActCont->mmdlsys), prm_bbd->res_idx );
  BBDResUnitIndex_AddResource( pBlActCont, g3dres, obj_code, flag );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX BBDACT �w��OBJ�R�[�h�̃��\�[�X���j�b�g�폜
 * @param	pBlActCont MMDL_BLACTCONT
 * @param	code	OBJ�R�[�h
 * @retval	nothing
 */
//--------------------------------------------------------------
static void BBDResUnitIndex_RemoveResUnit(
		MMDL_BLACTCONT *pBlActCont, u16 obj_code )
{
	GFL_BBDACT_RESUNIT_ID id;
  BOOL ret = IDCodeIndex_SearchCode(
      &pBlActCont->BBDResUnitIdx, obj_code, &id, NULL );
	GF_ASSERT( ret == TRUE );
	GFL_BBDACT_RemoveResourceUnit( pBlActCont->pBbdActSys, id, 1 );
	IDCodeIndex_DeleteCode( &pBlActCont->BBDResUnitIdx, obj_code );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX BBDACT�@�w��R�[�h�̃��\�[�XID���擾
 * @param	pBlActCont MMDL_BLACTCONT
 * @param	code	OBJ�R�[�h
 * @retval	BOOL TRUE=�w��R�[�h�o�^�L��
 */
//--------------------------------------------------------------
static BOOL BBDResUnitIndex_SearchResID(
    MMDL_BLACTCONT *pBlActCont, u16 obj_code, u16 *outID, u16 *outFlag )
{
  BOOL ret = IDCodeIndex_SearchCode(
    &pBlActCont->BBDResUnitIdx, obj_code, outID, outFlag );
  return( ret );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX BBDACT �w��R�[�h�̃��\�[�XID���ǉ��ς݂��`�F�b�N
 * @param	mmdlsys	MMDLSYS
 * @param	code OBJ�R�[�h
 * @retval	BOOL	TRUE=�ǉ��ς� FALSE=�ǉ��܂�
 */
//--------------------------------------------------------------
BOOL MMDL_BLACTCONT_CheckOBJCodeRes( MMDLSYS *mmdlsys, u16 code )
{
	u16 res;
	MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( mmdlsys );
	
	if( IDCodeIndex_SearchCode(&pBlActCont->BBDResUnitIdx,code,NULL,NULL) ){
    return( TRUE );
  }
  
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX BBDACT �w��R�[�h�̃��\�[�X��ǉ�
 * @param	mmdlssy	MMDLSYS
 * @param	code	OBJ�R�[�h
 * @param trans TRUE=�e�N�X�`���]���^�œo�^
 * @param guest TRUE=�g�p����Ȃ��Ȃ�Ɣj�������Q�X�g�^�C�v�œo�^
 * @retval	BOOL	TRUE=�ǉ������BFALSE=���ɒǉ��ς�
 * @note �Ă΂ꂽ���̏�œǂݍ��݂���������B
 */
//--------------------------------------------------------------
BOOL MMDL_BLACTCONT_AddOBJCodeRes(
    MMDLSYS *mmdlsys, u16 code, BOOL trans, BOOL guest )
{
	if( MMDL_BLACTCONT_CheckOBJCodeRes(mmdlsys,code) == FALSE ){
    MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( mmdlsys );
    BBDRESBIT flag = 0;
    if( trans ){ flag |= BBDRESBIT_TRANS; }
    if( guest ){ flag |= BBDRESBIT_GUEST; }
		BBDResUnitIndex_AddResUnit( pBlActCont, code, flag );
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX BBDACT �w��R�[�h�̃��\�[�X���폜
 * @param	mmdlssy	MMDLSYS
 * @param	code	OBJ�R�[�h
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_BLACTCONT_DeleteOBJCodeRes( MMDLSYS *mmdlsys, u16 code )
{
  MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( mmdlsys );
	BBDResUnitIndex_RemoveResUnit( pBlActCont, code );
}

//======================================================================
//  �A�N�^�[�ǉ��\�񏈗�
//======================================================================
//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[�ǉ��\�񏈗��@������
 * @param pBlActCont MMDL_BLACTCONT*
 * @retval nothing
 */
//--------------------------------------------------------------
static void BlActAddReserve_Init( MMDL_BLACTCONT *pBlActCont )
{
  HEAPID heapID = MMDLSYS_GetHeapID( pBlActCont->mmdlsys );
  BLACT_RESERVE *pReserve = 
    GFL_HEAP_AllocClearMemory( heapID, sizeof(BLACT_RESERVE) );
  
  pReserve->resMax = pBlActCont->resourceMax;
  pReserve->actMax = MMDLSYS_GetMMdlMax( pBlActCont->mmdlsys );
  pReserve->resDigestFrameMax = RES_DIGEST_FRAME_MAX;
  
  pReserve->pReserveResParam = GFL_HEAP_AllocClearMemory(
      heapID, sizeof(ADDRES_RESERVE_PARAM)*pReserve->resMax );
  DEBUG_MMDL_RESOURCE_MEMORY_SIZE_Plus( pReserve->pReserveResParam );
  {
    int i = 0;
    ADDRES_RESERVE_PARAM *pResParam = pReserve->pReserveResParam;
    for( ; i < pReserve->resMax; i++, pResParam++ ){
      pResParam->code = REGIDCODE_MAX;
    }
  }
  
  pReserve->pReserveRes = GFL_HEAP_AllocClearMemory(
      heapID, sizeof(ADDRES_RESERVE)*pReserve->resDigestFrameMax );
  pReserve->pReserveAct = GFL_HEAP_AllocClearMemory(
      heapID, sizeof(ADDACT_RESERVE)*pReserve->actMax );
  
  pReserve->funcFlag = TRUE;
  pBlActCont->pReserve = pReserve;
}

//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[�ǉ��\�񏈗��@�폜
 * @param pBlActCont MMDL_BLACTCONT*
 * @retval nothing
 */
//--------------------------------------------------------------
static void BlActAddReserve_Delete( MMDL_BLACTCONT *pBlActCont )
{
  BLACT_RESERVE *pReserve = pBlActCont->pReserve;
  
  if( pReserve != NULL ){
    u32 i;
    pReserve->funcFlag = FALSE;
    
    { //���\�[�X�\��p�����^
      DEBUG_MMDL_RESOURCE_MEMORY_SIZE_Minus( pReserve->pReserveResParam );
      GFL_HEAP_FreeMemory( pReserve->pReserveResParam );
    }

    { //���\�[�X
      ADDRES_RESERVE *pRes = pReserve->pReserveRes;
      for( i = 0; i < pReserve->resDigestFrameMax; i++, pRes++ ){
        if( pRes->pG3dRes != NULL ){
          DEBUG_MMDL_RESOURCE_MEMORY_SIZE_Minus( GFL_G3D_GetResourceFileHeader( pRes->pG3dRes ) );
          DEBUG_MMDL_RESOURCE_MEMORY_SIZE_Minus( pRes->pG3dRes );
          GFL_G3D_DeleteResource( pRes->pG3dRes );
        }
      }
      GFL_HEAP_FreeMemory( pReserve->pReserveRes );
    }
    
    { //�A�N�^�[
      ADDACT_RESERVE *pRes = pReserve->pReserveAct;
      for( i = 0; i < pReserve->actMax; i++, pRes++ ){
        if( pRes->outID != NULL && pRes->pTransActRes != NULL ){
          DEBUG_MMDL_RESOURCE_MEMORY_SIZE_Minus( GFL_G3D_GetResourceFileHeader( pRes->pTransActRes ) );
          DEBUG_MMDL_RESOURCE_MEMORY_SIZE_Minus( pRes->pTransActRes );
          GFL_G3D_DeleteResource( pRes->pTransActRes );
        }
      }
      GFL_HEAP_FreeMemory( pReserve->pReserveAct );
    }
    
    GFL_HEAP_FreeMemory( pReserve );
    pBlActCont->pReserve = NULL;
  }
}

//--------------------------------------------------------------
/**
 * ���\�[�X�ǉ��\�񏈗��@�\��p�����^�o�^
 * @param pBlActCont MMDL_BLACTCONT
 * @param code �\���R�[�h
 * @param flag BBDRESBIT
 * @retval nothing
 */
//--------------------------------------------------------------
static void BlActAddReserve_RegistResourceParam(
    MMDL_BLACTCONT *pBlActCont, u16 code, BBDRESBIT flag )
{
  u32 i = 0;
  BLACT_RESERVE *pReserve = pBlActCont->pReserve;
  ADDRES_RESERVE_PARAM *pResParam = pReserve->pReserveResParam;
  
  for( ; i < pReserve->resMax; i++, pResParam++ ){
    if( pResParam->code == REGIDCODE_MAX ){
      pResParam->code = code;
      pResParam->flag = flag;
      KAGAYA_Printf( "MMDL BLACT RESERVE REG RESPRM CODE=%d\n", code );
      return;
    }
  }
  
  GF_ASSERT( 0 );
}

//--------------------------------------------------------------
/**
 * ���\�[�X�ǉ��\�񏈗��@�\��p�����^����
 * @param pBlActCont MMDL_BLACTCONT
 * @retval nothing
 */
//--------------------------------------------------------------
static void BlActAddReserve_DigestResourceParam(
    MMDL_BLACTCONT *pBlActCont )
{
  u32 i = 0,j = 0;
  BLACT_RESERVE *pReserve = pBlActCont->pReserve;
  ADDRES_RESERVE_PARAM *pResParam = pReserve->pReserveResParam;
  
  for( ; i < pReserve->resMax; i++, pResParam++ ){
    if( pResParam->code != REGIDCODE_MAX ){
      if( BlActAddReserve_RegistResource(
          pBlActCont,pResParam->code,pResParam->flag) == FALSE ){
        KAGAYA_Printf( "MMDL DIG RESPRM RESERVE OVER\n" );
        break; //�\���t
      }
      
      pResParam->code = REGIDCODE_MAX;

      j++;
      if( j >= pReserve->resDigestFrameMax ){
        KAGAYA_Printf( "MMDL DIG RESPRM FRAME OVER\n" );
        break;
      }
    }
  }
  
  if( j ){  //������̑O�l��
    pResParam = pReserve->pReserveResParam;
    
    for( i = 0; i < (pReserve->resMax-1); i++ ){
      if( pResParam[i].code == REGIDCODE_MAX ){
        for( j = i+1; j < pReserve->resMax; j++ ){
          if( pResParam[j].code != REGIDCODE_MAX ){
            pResParam[i] = pResParam[j];
            pResParam[j].code = REGIDCODE_MAX;
            break;
          }
        }
      }
    }
  }
}

//--------------------------------------------------------------
/**
 * ���\�[�X�ǉ��\�񏈗��@�\��o�^
 * @param pBlActCont MMDL_BLACTCONT
 * @param code �\���R�[�h
 * @param flag BBDRESBIT
 * @retval BOOL TRUE=�o�^
 */
//--------------------------------------------------------------
static BOOL BlActAddReserve_RegistResource(
    MMDL_BLACTCONT *pBlActCont, u16 code, BBDRESBIT flag )
{
  u32 i = 0;
  BLACT_RESERVE *pReserve = pBlActCont->pReserve;
  ADDRES_RESERVE *pRes = pReserve->pReserveRes;
  
  for( ; i < pReserve->resDigestFrameMax; i++, pRes++ ){
    if( pRes->pG3dRes == NULL ){
	    const OBJCODE_PARAM *prm;
      const OBJCODE_PARAM_BUF_BBD  *prm_bbd;
      prm = MMDLSYS_GetOBJCodeParam( pBlActCont->mmdlsys, code );
      prm_bbd = MMDL_GetOBJCodeParamBufBBD( prm );
      
      pRes->compFlag = FALSE;
      pRes->code = code;
      pRes->flag = flag;
      pRes->pG3dRes = GFL_G3D_CreateResourceHandle(
          MMDLSYS_GetResArcHandle(pBlActCont->mmdlsys), prm_bbd->res_idx );
      pRes->compFlag = TRUE;

      DEBUG_MMDL_RESOURCE_MEMORY_SIZE_Plus( GFL_G3D_GetResourceFileHeader( pRes->pG3dRes ) );
      DEBUG_MMDL_RESOURCE_MEMORY_SIZE_Plus( pRes->pG3dRes );
      
      KAGAYA_Printf(
        "MMDL BLACT RESERVE ADD RESOURCE CODE=%d,ARCIDX=%d\n",
        pRes->code, prm_bbd->res_idx );
      return( TRUE );
    }
  }
  
  GF_ASSERT( 0 ); //�\���t
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �A�N�^�[�ǉ��\�񏈗��@�\�����
 * @param MMDLSYS *mmdlsys
 * @retval nothing
 */
//--------------------------------------------------------------
static void BlActAddReserve_DigestResource( MMDL_BLACTCONT *pBlActCont )
{
  BLACT_RESERVE *pReserve = pBlActCont->pReserve;
  
  if( pReserve != NULL )
  {
    if( pReserve->funcFlag == TRUE )
    {
      u32 i = 0;
      BLACT_RESERVE *pReserve = pBlActCont->pReserve;
      ADDRES_RESERVE *pRes = pReserve->pReserveRes;
      
      for( ; i < pReserve->resDigestFrameMax; i++, pRes++ ){
        if( pRes->pG3dRes != NULL && pRes->compFlag == TRUE ){
          pRes->compFlag = FALSE;
          BBDResUnitIndex_AddResource(
              pBlActCont, pRes->pG3dRes, pRes->code, pRes->flag );
          pRes->pG3dRes = NULL;
        
          #ifdef DEBUG_MMDL_DEVELOP
          {
            u16 id,flag;
            BBDResUnitIndex_SearchResID( pBlActCont, pRes->code, &id, &flag );
            KAGAYA_Printf(
                "MMDL BLACT RESERVE DIG RESOURCE CODE=%d, RESID=%d\n",
                pRes->code, id );
          }
          #endif
        }
      }
    }
  }
}

//--------------------------------------------------------------
/**
 * ���\�[�X�ǉ��\�񏈗��@�\�����
 * @param pBlActCont MMDL_BLACTCONT
 * @retval nothing
 */
//--------------------------------------------------------------
#if 0
static void BlActAddReserve_DigestResource( MMDL_BLACTCONT *pBlActCont )
{
  u32 i = 0;
  BLACT_RESERVE *pReserve = pBlActCont->pReserve;
  ADDRES_RESERVE *pRes = pReserve->pReserveRes;
  
  for( ; i < pReserve->resMax; i++, pRes++ ){
    if( pRes->pG3dRes != NULL && pRes->compFlag == TRUE ){
      pRes->compFlag = FALSE;
      BBDResUnitIndex_AddResource(
          pBlActCont, pRes->pG3dRes, pRes->code, pRes->flag );
      pRes->pG3dRes = NULL;
    
#ifdef DEBUG_MMDL_DEVELOP
      {
        u16 id,flag;
        BBDResUnitIndex_SearchResID( pBlActCont, pRes->code, &id, &flag );
        KAGAYA_Printf(
            "MMDL BLACT RESERVE DIG RESOURCE CODE=%d, RESID=%d\n",
            pRes->code, id );
      }
#endif
    }
  }
}
#endif

//--------------------------------------------------------------
/**
 * ���\�[�X�ǉ��\�񏈗��@�\�񌟍�
 * @param pBlActCont MMDL_BLACTCONT
 * @param code ��������\���R�[�h
 * @param outFlag *�\��̍ۂɎw�肳�ꂽBBDRESBIT�i�[��
 * @retval BOOL TRUE=�\��ς� FALSE=�\��Ȃ�
 */
//--------------------------------------------------------------
static BOOL BlActAddReserve_SearchResource(
    MMDL_BLACTCONT *pBlActCont, u16 code, u16 *outFlag )
{
  { //�p�����^�\��
    u32 i = 0;
    BLACT_RESERVE *pReserve = pBlActCont->pReserve;
    ADDRES_RESERVE_PARAM *pResParam = pReserve->pReserveResParam;
    
    for( ; i < pReserve->resMax; i++, pResParam++ ){
      if( pResParam->code == code ){
        *outFlag = pResParam->flag;
        return( TRUE );
      }
    }
  }
  
  { //���\�[�X�\��
    u32 i = 0;
    BLACT_RESERVE *pReserve = pBlActCont->pReserve;
    ADDRES_RESERVE *pRes = pReserve->pReserveRes;
  
    for( ; i < pReserve->resDigestFrameMax; i++, pRes++ ){
      if( pRes->code == code && pRes->pG3dRes != NULL ){
        *outFlag = pRes->flag;
        return( TRUE );
      }
    }
  }

  return( FALSE );
}

//--------------------------------------------------------------
/**
 * ���\�[�X�ǉ��\�񏈗��@�\��L�����Z��
 * @param pBlActCont MMDL_BLACTCONT
 * @param code �L�����Z������\���R�[�h
 * @retval BOOL TRUE=�L�����Z������ FALSE=�\��Ȃ�
 */
//--------------------------------------------------------------
static BOOL BlActAddReserve_CancelResource(
    MMDL_BLACTCONT *pBlActCont, u16 code )
{
  { //�p�����^�\��
    u32 i = 0;
    BLACT_RESERVE *pReserve = pBlActCont->pReserve;
    ADDRES_RESERVE_PARAM *pResParam = pReserve->pReserveResParam;
    
    for( ; i < pReserve->resMax; i++, pResParam++ ){
      if( pResParam->code == code ){
        pResParam->code = REGIDCODE_MAX;
        KAGAYA_Printf( "MMDL BLACT RESERVE CANCEL CODE=%d\n", code );
        return( TRUE );
      }
    }
  }
  
  { //���\�[�X�\��
    u32 i = 0;
    BLACT_RESERVE *pReserve = pBlActCont->pReserve;
    ADDRES_RESERVE *pRes = pReserve->pReserveRes;
  
    for( ; i < pReserve->resDigestFrameMax; i++, pRes++ ){
      if( pRes->pG3dRes != NULL && pRes->code == code ){
        pRes->compFlag = FALSE;
        DEBUG_MMDL_RESOURCE_MEMORY_SIZE_Minus( GFL_G3D_GetResourceFileHeader( pRes->pG3dRes ) );
        DEBUG_MMDL_RESOURCE_MEMORY_SIZE_Minus( pRes->pG3dRes );
        GFL_G3D_DeleteResource( pRes->pG3dRes );
        pRes->pG3dRes = NULL;
        KAGAYA_Printf( "MMDL BLACT RESERVE CANCEL CODE=%d\n", code );
        return( TRUE );
      }
    }
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �A�N�^�[�ǉ��\�񏈗��@�\��o�^
 * @param pBlActCont MMDL_BLACTCONT
 * @param mmdl MMDL*
 * @param code �\���R�[�h
 * @param outID �A�N�^�[ID�i�[��
 * @retval nothing
 */
//--------------------------------------------------------------
static void BlActAddReserve_RegistActor(
    MMDL_BLACTCONT *pBlActCont, MMDL *mmdl, u16 code, u16 *outID,
    MMDL_BLACTCONT_ADDACT_USERPROC init_proc,
    void *init_work, const VecFx32 *user_pos )
{
  u32 i = 0;
  u32 max = MMDLSYS_GetMMdlMax( pBlActCont->mmdlsys );
  ADDACT_RESERVE *pRes = pBlActCont->pReserve->pReserveAct;
  
  for( ; i < max; i++, pRes++ )
  {
    if( pRes->outID == NULL )
    {
      u16 resID,flag;
	    const OBJCODE_PARAM *prm;
      
      pRes->compFlag = FALSE;
      pRes->mmdl = mmdl;
      pRes->code = code;
      pRes->outID = outID;
      
	    prm = MMDLSYS_GetOBJCodeParam( pBlActCont->mmdlsys, pRes->code );
	    BBDResUnitIndex_SearchResID( pBlActCont, code, &resID, &flag );
      
      if( resID == REGIDCODE_MAX ) //��o�^���\�[�X
      {
        if( BlActAddReserve_SearchResource( //���\�[�X�\�񌟍�
              pBlActCont,code,&flag) == FALSE )
        {
          flag = BBDRES_VRAM_GUEST; //��{VRAM�풓�Q�X�g�^�œo�^
          BlActAddReserve_RegistResourceParam( pBlActCont, code, flag );
        }
      }
      
      if( (flag&BBDRESBIT_TRANS) ){ //�]���^ �]���惊�\�[�X�쐬
        #if 0
        //�]���p���\�[�X�f�[�^���擾
        pRes->pTransActRes = GFL_G3D_CreateResourceHandle(
              pBlActCont->arcH_res, prm->res_idx );
        #else
        GF_ASSERT( 0 ); //���󖢑Ή�
        #endif
      }
      
      pRes->user_init_proc = init_proc;
      pRes->user_init_work = init_work;
      
      if( user_pos != NULL ){
        pRes->user_add_pos = *user_pos;
      }

      pRes->compFlag = TRUE;
      
#ifdef DEBUG_ONLY_FOR_kagaya
      if( mmdl != NULL ){
        KAGAYA_Printf( "MMDL BLACT RESERVE ADD ACTOR OBJID=%d, CODE=%d\n", 
          MMDL_GetOBJID(mmdl), pRes->code );
      }else{
        KAGAYA_Printf( "MMDL BLACT RESERVE ADD ACTOR OBJID=NON, CODE=%d\n", 
          pRes->code );
      }
#endif
      return;
    }
  }
  
  GF_ASSERT( 0 && "MMDL BLACT RESERVE ADD MAX" );
}

//--------------------------------------------------------------
/**
 * �A�N�^�[�ǉ��\�񏈗��@�\������R�A����
 * @param MMDLSYS *mmdlsys
 * @retval nothing
 */
//--------------------------------------------------------------
static void BlActAddReserve_DigestActorCore(
    MMDL_BLACTCONT *pBlActCont, ADDACT_RESERVE *pRes )
{
  BOOL ret;
  u16 resID,flag;
  u16 transID = BLACT_RESID_NULL;
  
  pRes->compFlag = FALSE;
   
  //�ꉞ�`�F�b�N
  if( pRes->pTransActRes != NULL ) //VRAM�]���^
  {
    resID = BlActRes_AddRes( pBlActCont, pRes->code,
        pRes->pTransActRes, GFL_BBDACT_RESTYPE_DATACUT );
    pRes->pTransActRes = NULL;
        
    ret = BBDResUnitIndex_SearchResID(
          pBlActCont, pRes->code, &transID, &flag );
        
    GF_ASSERT( ret == TRUE &&
        "BLACT ADD RESERVE RESOURCE NONE" ); //�]���p���\�[�X����
    GF_ASSERT( (flag&BBDRESBIT_TRANS) &&
        "BLACT ADD RESERVE RESOURCE ERROR" ); //�]���p���\�[�X�ł͖���
  }
  else //VRAM�풓�^
  {
    ret = BBDResUnitIndex_SearchResID(
          pBlActCont, pRes->code, &resID, &flag );
        
    if( ret != TRUE ){
      OS_Printf( "BLACT ADD RESOURCE ERROR CODE=%xH\n", pRes->code );
    }
        
    //���\�[�X����
    GF_ASSERT( ret == TRUE && "BLACT ADD RESERVE RESOURCE NONE" );
  }
      
  if( pRes->mmdl != NULL ){ //�A�N�^�[�ǉ� ���샂�f��
    *(pRes->outID) = blact_SetResActor( //�A�N�^�[�ǉ�
          pRes->mmdl, pRes->code, resID, transID );
  }else{ //�A�N�^�[�ǉ� ���[�U�[�w��
    *(pRes->outID) = blact_SetResActorCore( pBlActCont->mmdlsys,
          &pRes->user_add_pos, pRes->code, resID, transID );
  }
      
  if( pRes->user_init_proc != NULL ){ //���[�U�[�w�菉���������Ăяo��
    pRes->user_init_proc( *pRes->outID, pRes->user_init_work );
  }
      
  if( pRes->mmdl != NULL ){
    MMDL_CallDrawProc( pRes->mmdl ); //�`�揈���Ăяo��
  }
      
  #ifdef DEBUG_ONLY_FOR_kagaya
  KAGAYA_Printf( "MMDL BLACT RESERVE DIG ACTOR " );
  if( pRes->mmdl != NULL ){
    KAGAYA_Printf( "OBJID=%d, CODE=%d, RESID=%d,TRANSID=%d\n",
      MMDL_GetOBJID(pRes->mmdl), pRes->code, resID, transID );
  }else{
    KAGAYA_Printf( "OBJID=NONE, CODE=%d, RESID=%d,TRANSID=%d\n",
      MMDL_GetOBJID(pRes->mmdl), pRes->code, resID, transID );
  }
  #endif
}

//--------------------------------------------------------------
/**
 * �A�N�^�[�ǉ��\�񏈗��@�\�����
 * @param MMDLSYS *mmdlsys
 * @retval nothing
 */
//--------------------------------------------------------------
static void BlActAddReserve_DigestActor( MMDL_BLACTCONT *pBlActCont )
{
  BOOL ret;
  u16 flag,transID;
  u32 i = 0;
  u32 max = MMDLSYS_GetMMdlMax( pBlActCont->mmdlsys );
  ADDACT_RESERVE *pRes = pBlActCont->pReserve->pReserveAct;
  
  for( ; i < max; i++, pRes++ )
  {
    if( pRes->outID != NULL && pRes->compFlag == TRUE )
    {
      ret = BBDResUnitIndex_SearchResID(
          pBlActCont, pRes->code, &transID, &flag );
      
      if( ret == TRUE ){ //���\�[�X���o�^���ꂽ
        BlActAddReserve_DigestActorCore( pBlActCont, pRes );
        BlActAddReserve_ClearWork( pRes );
      }
    }
  }
}

//--------------------------------------------------------------
/**
 * �A�N�^�[�ǉ��\�񏈗��@�\��ς݂̕\���R�[�h������
 * @param pBlActCont MMDL_BLACTCONT
 * @param code ��������\���R�[�h
 * @param mmdl �����ΏۂƂ��Ȃ�MMDL
 * @retval BOOL TRUE=��v�L�� FALSE=��v����
 */
//--------------------------------------------------------------
static BOOL BlActAddReserve_SearchActorOBJCode(
    MMDL_BLACTCONT *pBlActCont, u16 code, MMDL *mmdl )
{
  u32 i = 0;
  u32 max = MMDLSYS_GetMMdlMax( pBlActCont->mmdlsys );
  ADDACT_RESERVE *pRes = pBlActCont->pReserve->pReserveAct;
  
  for( ; i < max; i++, pRes++ ){
    if( pRes->outID != NULL && pRes->mmdl != NULL && pRes->mmdl != mmdl ){
      if( pRes->code == code ){
        return( TRUE );
      }
    }
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �A�N�^�[�ǉ��\�񏈗��@�\��ς݂̕\���R�[�h������ ID�i�[�A�h���X��
 * @param pBlActCont MMDL_BLACTCONT
 * @param code ��������\���R�[�h
 * @param outID �����ΏۂƂ��Ȃ�outID
 * @retval BOOL TRUE=��v�L�� FALSE=��v����
 */
//--------------------------------------------------------------
static BOOL BlActAddReserve_SearchActorOBJCodeOutID(
    MMDL_BLACTCONT *pBlActCont, u16 code, const u16 *outID )
{
  u32 i = 0;
  u32 max = MMDLSYS_GetMMdlMax( pBlActCont->mmdlsys );
  ADDACT_RESERVE *pRes = pBlActCont->pReserve->pReserveAct;
  
  for( ; i < max; i++, pRes++ ){
    if( pRes->outID != NULL && pRes->outID != outID ){
      if( pRes->code == code ){
        return( TRUE );
      }
    }
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �A�N�^�[�ǉ��\�񏈗��@�\��L�����Z��
 * @param pBlActCont MMDL_BLACTCONT
 * @param mmdl �\�񂵂�MMDL*
 * @retval nothing
 * @note �ǉ����Ɏw�肳�ꂽ�R�[�h���Q�X�g���\�[�X�Ƃ���
 * �ǉ��\��ɓ����Ă����ۂ̓��\�[�X�\��L�����Z�������킹�čs���B
 */
//--------------------------------------------------------------
static void BlActAddReserve_CancelActorOutID(
    MMDL_BLACTCONT *pBlActCont, const u16 *outID )
{
  u32 i = 0;
  u32 max = MMDLSYS_GetMMdlMax( pBlActCont->mmdlsys );
  ADDACT_RESERVE *pRes = pBlActCont->pReserve->pReserveAct;
  
  for( ; i < max; i++, pRes++ )
  {
    if( pRes->outID != NULL && pRes->outID == outID )
    {
      pRes->compFlag = FALSE;
      
      if( pRes->pTransActRes != NULL ) //�]���p���\�[�X�L��
      {
        DEBUG_MMDL_RESOURCE_MEMORY_SIZE_Minus( GFL_G3D_GetResourceFileHeader( pRes->pTransActRes ) );
        DEBUG_MMDL_RESOURCE_MEMORY_SIZE_Minus( pRes->pTransActRes );
        GFL_G3D_DeleteResource( pRes->pTransActRes );
        pRes->pTransActRes = NULL;
      }
      
      KAGAYA_Printf(
          "MMDL BLACT RESERVE CANCEL OBJID=NON, CODE=%d\n",  pRes->code );
      
      BlActAddReserve_ClearWork( pRes );
      return;
    }
  }
}

//--------------------------------------------------------------
/**
 * �A�N�^�[�ǉ��\�񏈗��@�\��L�����Z�� ID�|�C���^
 * @param pBlActCont MMDL_BLACTCONT
 * @param outID �\�񎞂Ɏw�肵��ID�i�[��|�C���^
 * @retval nothing
 * @note ���\�[�X�֘A�̃L�����Z���͍s��Ȃ��B
 */
//--------------------------------------------------------------
static void BlActAddReserve_CancelActor(
    MMDL_BLACTCONT *pBlActCont, MMDL *mmdl )
{
  u32 i = 0;
  u32 max = MMDLSYS_GetMMdlMax( pBlActCont->mmdlsys );
  ADDACT_RESERVE *pRes = pBlActCont->pReserve->pReserveAct;
  
  for( ; i < max; i++, pRes++ )
  {
    if( pRes->outID != NULL && pRes->mmdl == mmdl )
    {
      u16 flag;
      pRes->compFlag = FALSE;
      
      if( BlActAddReserve_SearchResource( //���\�[�X�\��L��
            pBlActCont,pRes->code,&flag) == TRUE )
      {
        if( (flag&BBDRESBIT_GUEST) ){ //�Q�X�g���\�[�X
          if( BlActAddReserve_SearchActorOBJCode( //���̗\��Ŏg�p����
                pBlActCont,pRes->code,pRes->mmdl) == FALSE )
          {
            BlActAddReserve_CancelResource( pBlActCont, pRes->code );
          }
        }
      }
      
      if( pRes->pTransActRes != NULL ) //�]���p���\�[�X�L��
      {
        DEBUG_MMDL_RESOURCE_MEMORY_SIZE_Minus( GFL_G3D_GetResourceFileHeader( pRes->pTransActRes ) );
        DEBUG_MMDL_RESOURCE_MEMORY_SIZE_Minus( pRes->pTransActRes );
        GFL_G3D_DeleteResource( pRes->pTransActRes );
        pRes->pTransActRes = NULL;
      }
      
      KAGAYA_Printf( "MMDL BLACT RESERVE CANCEL OBJID=%d, CODE=%d\n", 
          MMDL_GetOBJID(mmdl), pRes->code );
      
      BlActAddReserve_ClearWork( pRes );
      return;
    }
  }
}

//--------------------------------------------------------------
/**
 * ADDACT_RESERVE�N���A
 * @param pRes ADDACT_RESERVE
 * @retval nothing
 */
//--------------------------------------------------------------
static void BlActAddReserve_ClearWork( ADDACT_RESERVE *pRes )
{
  pRes->compFlag = FALSE;
  MI_CpuClear8( pRes, sizeof(ADDACT_RESERVE) );
}

//======================================================================
//	parts
//======================================================================
//--------------------------------------------------------------
/**
 * �e�[�u���ԍ�����A�N�^�[�A�j���e�[�u���擾
 * @param	no	MMDL_BLACT_ANMTBLNO_MAX
 * @retval	MMDL_BBDACT_ANMTBL*
 */
//--------------------------------------------------------------
static const MMDL_BBDACT_ANMTBL * BlActAnm_GetAnmTbl( u32 no )
{
	GF_ASSERT( no < MMDL_BLACT_ANMTBLNO_MAX );
	return( &DATA_MMDL_BBDACT_ANM_ListTable[no] );
}

#if 0
//--------------------------------------------------------------
/**
 * �r���{�[�h�A�j�� �`��X�e�[�^�X����e�[�u���v�f�����擾�B��{�^�C�v
 * @param sta DRAW_STA_STOP��
 * @retval u32 �Ή�����A�j���[�V�����e�[�u���v�f��
 */
//--------------------------------------------------------------
u32 FLDMMDL_BLACTCONT_GetAnmTblNo_Status( u32 sta )
{
  GF_ASSERT( sta < DRAW_STA_MAX );
  //�㉺���E�̕��тŊe�S����
  return( sta * DIR_MAX4 );
}

//--------------------------------------------------------------
/**
 * �r���{�[�h�A�j�� �`��X�e�[�^�X����e�[�u���v�f�����擾�B���@��p
 * @param sta DRAW_STA_STOP��
 * @retval u32 �Ή�����A�j���[�V�����e�[�u���v�f��
 */
//--------------------------------------------------------------
u32 FLDMMDL_BLACTCONT_GetAnmTblNo_StatusHero( u32 sta )
{
  GF_ASSERT( sta < DRAW_STA_MAX_HERO );
  //�㉺���E�̕��тŊe�S����
  return( sta * DIR_MAX4 );
}
#endif

//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[���\�[�X�ǉ�
 * @param pBlActCont MMDL_BLACTCONT
 * @param code OBJ�R�[�h
 * g3dres �ǉ����郊�\�[�X*
 * @param type ���\�[�X�^�C�v
 * @retval GFL_BBDACT_RESUNIT_ID
 */
//--------------------------------------------------------------
static GFL_BBDACT_RESUNIT_ID BlActRes_AddRes(
    MMDL_BLACTCONT *pBlActCont, u16 code,
    GFL_G3D_RES *g3dres, GFL_BBDACT_RESTYPE type )
{
	GFL_BBDACT_RESUNIT_ID id;
	const OBJCODE_PARAM *prm;
	GFL_BBDACT_G3DRESDATA data;
  
	prm = MMDLSYS_GetOBJCodeParam( pBlActCont->mmdlsys, code );
  
  data.g3dres = g3dres;
	data.texFmt = GFL_BBD_TEXFMT_PAL16;
	data.texSiz = prm->tex_size;
  
#if 0
	data.celSizX = 32;				//������mdl_size����
	data.celSizY = 32;
#else 
  {
    const u16 *size = DATA_MMDL_BLACT_MdlSize[prm->mdl_size];
    data.celSizX = size[0];
    data.celSizY = size[1];
  }
#endif

  data.dataCut = type;
  
	id = GFL_BBDACT_AddResourceG3DResUnit( pBlActCont->pBbdActSys, &data, 1 );
  
  KAGAYA_Printf( "MMDL ADD RESOURCE CODE=%d,RESID=%d\n", code, id );
  return( id );
}

//======================================================================
//	debug
//======================================================================
#ifdef DEBUG_MMDL_RESOURCE_MEMORY_SIZE
//----------------------------------------------------------------------------
/**
 *	@brief  ���\�[�X�������T�C�Y��Ԃ�
 */
//-----------------------------------------------------------------------------
u32 DEBUG_MMDL_GetUseResourceMemorySize( void )
{
  return DEBUG_MMDL_RESOURCE_MemorySize;
}
#endif

//======================================================================
//	test data
//======================================================================
/*
static const GFL_BBDACT_RESDATA testResTable[] = {
#ifdef MMDL_BLACT_HEAD3_TEST
	{ ARCID_MMDL_BTX, NARC_fldmmdl_btx_obj_kabi32_nsbtx, 
		GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32, GFL_BBDACT_RESTYPE_DATACUT },
#else
	{ ARCID_FLDMAP_ACTOR, NARC_fld_act_hero_nsbtx, 
		GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x1024, 32, 32, GFL_BBDACT_RESTYPE_DATACUT },
#endif
	{ ARCID_FLDMAP_ACTOR, NARC_fld_act_achamo_nsbtx, 
		GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32, GFL_BBDACT_RESTYPE_DATACUT },
	{ ARCID_FLDMAP_ACTOR, NARC_fld_act_badman_nsbtx, 
		GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32, GFL_BBDACT_RESTYPE_DATACUT },
	{ ARCID_FLDMAP_ACTOR, NARC_fld_act_beachgirl_nsbtx,
		GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32, GFL_BBDACT_RESTYPE_DATACUT },
	{ ARCID_FLDMAP_ACTOR, NARC_fld_act_idol_nsbtx,
		GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32, GFL_BBDACT_RESTYPE_DATACUT },
	{ ARCID_FLDMAP_ACTOR, NARC_fld_act_lady_nsbtx,
		GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32, GFL_BBDACT_RESTYPE_DATACUT },
	{ ARCID_FLDMAP_ACTOR, NARC_fld_act_oldman1_nsbtx,
		GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32, GFL_BBDACT_RESTYPE_DATACUT },
	{ ARCID_FLDMAP_ACTOR, NARC_fld_act_policeman_nsbtx,
		GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32, GFL_BBDACT_RESTYPE_DATACUT },
};
const u32 testResTableMax = NELEMS( testResTable );
*/

//======================================================================
//	���샂�f���Ǘ����Œ�`�����e�N�X�`���T�C�Y�w�肪
//	�r���{�[�h�A�N�^�[�Œ�`����Ă�����̂Ɠ��ꂩ�ǂ���
//	�v���v���Z�b�T���߂Ń`�F�b�N
//======================================================================
#define MMDL_CHECK_GFL_BBD_TEXSIZDEF ///<��`�Ń`�F�b�N�L��

#ifdef MMDL_CHECK_GFL_BBD_TEXSIZDEF
#include "field/fldmmdl_list_symbol.h"

#if (GFL_BBD_TEXSIZDEF_8x8-TEXSIZE_8x8)
#error (GFL_BBD_TEXSIZDEF_8x8-TEXSIZE_8x8)
#endif
#if (GFL_BBD_TEXSIZDEF_8x16-TEXSIZE_8x16)
#error (GFL_BBD_TEXSIZDEF_8x16-TEXSIZE_8x16)
#endif
#if (GFL_BBD_TEXSIZDEF_8x32-TEXSIZE_8x32)
#error (GFL_BBD_TEXSIZDEF_8x32-TEXSIZE_8x32)
#endif
#if (GFL_BBD_TEXSIZDEF_8x64-TEXSIZE_8x64)
#error (GFL_BBD_TEXSIZDEF_8x64-TEXSIZE_8x64)
#endif
#if (GFL_BBD_TEXSIZDEF_8x128-TEXSIZE_8x128)
#error (GFL_BBD_TEXSIZDEF_8x128-TEXSIZE_8x128)
#endif
#if (GFL_BBD_TEXSIZDEF_8x256-TEXSIZE_8x256)
#error (GFL_BBD_TEXSIZDEF_8x256-TEXSIZE_8x256)
#endif
#if (GFL_BBD_TEXSIZDEF_8x512-TEXSIZE_8x512)
#error (GFL_BBD_TEXSIZDEF_8x512-TEXSIZE_8x512)
#endif
#if (GFL_BBD_TEXSIZDEF_8x1024-TEXSIZE_8x1024)
#error (GFL_BBD_TEXSIZDEF_8x1024-TEXSIZE_8x1024)
#endif
#if (GFL_BBD_TEXSIZDEF_16x8-TEXSIZE_16x8)
#error (GFL_BBD_TEXSIZDEF_16x8-TEXSIZE_16x8)
#endif
#if (GFL_BBD_TEXSIZDEF_16x16-TEXSIZE_16x16)
#error (GFL_BBD_TEXSIZDEF_16x16-TEXSIZE_16x16)
#endif
#if (GFL_BBD_TEXSIZDEF_16x32-TEXSIZE_16x32)
#error (GFL_BBD_TEXSIZDEF_16x32-TEXSIZE_16x32)
#endif
#if (GFL_BBD_TEXSIZDEF_16x64-TEXSIZE_16x64)
#error (GFL_BBD_TEXSIZDEF_16x64-TEXSIZE_16x64)
#endif
#if (GFL_BBD_TEXSIZDEF_16x128-TEXSIZE_16x128)
#error (GFL_BBD_TEXSIZDEF_16x128-TEXSIZE_16x128)
#endif
#if (GFL_BBD_TEXSIZDEF_16x256-TEXSIZE_16x256)
#error (GFL_BBD_TEXSIZDEF_16x256-TEXSIZE_16x256)
#endif
#if (GFL_BBD_TEXSIZDEF_16x512-TEXSIZE_16x512)
#error (GFL_BBD_TEXSIZDEF_16x512-TEXSIZE_16x512)
#endif
#if (GFL_BBD_TEXSIZDEF_16x1024-TEXSIZE_16x1024)
#error (GFL_BBD_TEXSIZDEF_16x1024-TEXSIZE_16x1024)
#endif
#if (GFL_BBD_TEXSIZDEF_32x8-TEXSIZE_32x8)
#error (GFL_BBD_TEXSIZDEF_32x8-TEXSIZE_32x8)
#endif
#if (GFL_BBD_TEXSIZDEF_32x16-TEXSIZE_32x16)
#error (GFL_BBD_TEXSIZDEF_32x16-TEXSIZE_32x16)
#endif
#if (GFL_BBD_TEXSIZDEF_32x32-TEXSIZE_32x32)
#error (GFL_BBD_TEXSIZDEF_32x32-TEXSIZE_32x32)
#endif
#if (GFL_BBD_TEXSIZDEF_32x64-TEXSIZE_32x64)
#error (GFL_BBD_TEXSIZDEF_32x64-TEXSIZE_32x64)
#endif
#if (GFL_BBD_TEXSIZDEF_32x128-TEXSIZE_32x128)
#error (GFL_BBD_TEXSIZDEF_32x128-TEXSIZE_32x128)
#endif
#if (GFL_BBD_TEXSIZDEF_32x256-TEXSIZE_32x256)
#error (GFL_BBD_TEXSIZDEF_32x256-TEXSIZE_32x256)
#endif
#if (GFL_BBD_TEXSIZDEF_32x512-TEXSIZE_32x512)
#error (GFL_BBD_TEXSIZDEF_32x512-TEXSIZE_32x512)
#endif
#if (GFL_BBD_TEXSIZDEF_32x1024-TEXSIZE_32x1024)
#error (GFL_BBD_TEXSIZDEF_32x1024-TEXSIZE_32x1024)
#endif
#if (GFL_BBD_TEXSIZDEF_64x8-TEXSIZE_64x8)
#error (GFL_BBD_TEXSIZDEF_64x8-TEXSIZE_64x8)
#endif
#if (GFL_BBD_TEXSIZDEF_64x16-TEXSIZE_64x16)
#error (GFL_BBD_TEXSIZDEF_64x16-TEXSIZE_64x16)
#endif
#if (GFL_BBD_TEXSIZDEF_64x32-TEXSIZE_64x32)
#error (GFL_BBD_TEXSIZDEF_64x32-TEXSIZE_64x32)
#endif
#if (GFL_BBD_TEXSIZDEF_64x64-TEXSIZE_64x64)
#error (GFL_BBD_TEXSIZDEF_64x64-TEXSIZE_64x64)
#endif
#if (GFL_BBD_TEXSIZDEF_64x128-TEXSIZE_64x128)
#error (GFL_BBD_TEXSIZDEF_64x128-TEXSIZE_64x128)
#endif
#if (GFL_BBD_TEXSIZDEF_64x256-TEXSIZE_64x256)
#error (GFL_BBD_TEXSIZDEF_64x256-TEXSIZE_64x256)
#endif
#if (GFL_BBD_TEXSIZDEF_64x512-TEXSIZE_64x512)
#error (GFL_BBD_TEXSIZDEF_64x512-TEXSIZE_64x512)
#endif
#if (GFL_BBD_TEXSIZDEF_64x1024-TEXSIZE_64x1024)
#error (GFL_BBD_TEXSIZDEF_64x1024-TEXSIZE_64x1024)
#endif
#if (GFL_BBD_TEXSIZDEF_128x8-TEXSIZE_128x8)
#error (GFL_BBD_TEXSIZDEF_128x8-TEXSIZE_128x8)
#endif
#if (GFL_BBD_TEXSIZDEF_128x16-TEXSIZE_128x16)
#error (GFL_BBD_TEXSIZDEF_128x16-TEXSIZE_128x16)
#endif
#if (GFL_BBD_TEXSIZDEF_128x32-TEXSIZE_128x32)
#error (GFL_BBD_TEXSIZDEF_128x32-TEXSIZE_128x32)
#endif
#if (GFL_BBD_TEXSIZDEF_128x64-TEXSIZE_128x64)
#error (GFL_BBD_TEXSIZDEF_128x64-TEXSIZE_128x64)
#endif
#if (GFL_BBD_TEXSIZDEF_128x128-TEXSIZE_128x128)
#error (GFL_BBD_TEXSIZDEF_128x128-TEXSIZE_128x128)
#endif
#if (GFL_BBD_TEXSIZDEF_128x256-TEXSIZE_128x256)
#error (GFL_BBD_TEXSIZDEF_128x256-TEXSIZE_128x256)
#endif
#if (GFL_BBD_TEXSIZDEF_128x512-TEXSIZE_128x512)
#error (GFL_BBD_TEXSIZDEF_128x512-TEXSIZE_128x512)
#endif
#if (GFL_BBD_TEXSIZDEF_128x1024-TEXSIZE_128x1024)
#error (GFL_BBD_TEXSIZDEF_128x1024-TEXSIZE_128x1024)
#endif
#if (GFL_BBD_TEXSIZDEF_256x8-TEXSIZE_256x8)
#error (GFL_BBD_TEXSIZDEF_256x8-TEXSIZE_256x8)
#endif
#if (GFL_BBD_TEXSIZDEF_256x16-TEXSIZE_256x16)
#error (GFL_BBD_TEXSIZDEF_256x16-TEXSIZE_256x16)
#endif
#if (GFL_BBD_TEXSIZDEF_256x32-TEXSIZE_256x32)
#error (GFL_BBD_TEXSIZDEF_256x32-TEXSIZE_256x32)
#endif
#if (GFL_BBD_TEXSIZDEF_256x64-TEXSIZE_256x64)
#error (GFL_BBD_TEXSIZDEF_256x64-TEXSIZE_256x64)
#endif
#if (GFL_BBD_TEXSIZDEF_256x128-TEXSIZE_256x128)
#error (GFL_BBD_TEXSIZDEF_256x128-TEXSIZE_256x128)
#endif
#if (GFL_BBD_TEXSIZDEF_256x256-TEXSIZE_256x256)
#error (GFL_BBD_TEXSIZDEF_256x256-TEXSIZE_256x256)
#endif
#if (GFL_BBD_TEXSIZDEF_256x512-TEXSIZE_256x512)
#error (GFL_BBD_TEXSIZDEF_256x512-TEXSIZE_256x512)
#endif
#if (GFL_BBD_TEXSIZDEF_256x1024-TEXSIZE_256x1024)
#error (GFL_BBD_TEXSIZDEF_256x1024-TEXSIZE_256x1024)
#endif
#if (GFL_BBD_TEXSIZDEF_512x8-TEXSIZE_512x8)
#error (GFL_BBD_TEXSIZDEF_512x8-TEXSIZE_512x8)
#endif
#if (GFL_BBD_TEXSIZDEF_512x16-TEXSIZE_512x16)
#error (GFL_BBD_TEXSIZDEF_512x16-TEXSIZE_512x16)
#endif
#if (GFL_BBD_TEXSIZDEF_512x32-TEXSIZE_512x32)
#error (GFL_BBD_TEXSIZDEF_512x32-TEXSIZE_512x32)
#endif
#if (GFL_BBD_TEXSIZDEF_512x64-TEXSIZE_512x64)
#error (GFL_BBD_TEXSIZDEF_512x64-TEXSIZE_512x64)
#endif
#if (GFL_BBD_TEXSIZDEF_512x128-TEXSIZE_512x128)
#error (GFL_BBD_TEXSIZDEF_512x128-TEXSIZE_512x128)
#endif
#if (GFL_BBD_TEXSIZDEF_512x256-TEXSIZE_512x256)
#error (GFL_BBD_TEXSIZDEF_512x256-TEXSIZE_512x256)
#endif
#if (GFL_BBD_TEXSIZDEF_512x512-TEXSIZE_512x512)
#error (GFL_BBD_TEXSIZDEF_512x512-TEXSIZE_512x512)
#endif
#if (GFL_BBD_TEXSIZDEF_512x1024-TEXSIZE_512x1024)
#error (GFL_BBD_TEXSIZDEF_512x1024-TEXSIZE_512x1024)
#endif
#if (GFL_BBD_TEXSIZDEF_1024x8-TEXSIZE_1024x8)
#error (GFL_BBD_TEXSIZDEF_1024x8-TEXSIZE_1024x8)
#endif
#if (GFL_BBD_TEXSIZDEF_1024x16-TEXSIZE_1024x16)
#error (GFL_BBD_TEXSIZDEF_1024x16-TEXSIZE_1024x16)
#endif
#if (GFL_BBD_TEXSIZDEF_1024x32-TEXSIZE_1024x32)
#error (GFL_BBD_TEXSIZDEF_1024x32-TEXSIZE_1024x32)
#endif
#if (GFL_BBD_TEXSIZDEF_1024x64-TEXSIZE_1024x64)
#error (GFL_BBD_TEXSIZDEF_1024x64-TEXSIZE_1024x64)
#endif
#if (GFL_BBD_TEXSIZDEF_1024x128-TEXSIZE_1024x128)
#error (GFL_BBD_TEXSIZDEF_1024x128-TEXSIZE_1024x128)
#endif
#if (GFL_BBD_TEXSIZDEF_1024x256-TEXSIZE_1024x256)
#error (GFL_BBD_TEXSIZDEF_1024x256-TEXSIZE_1024x256)
#endif
#if (GFL_BBD_TEXSIZDEF_1024x512-TEXSIZE_1024x512)
#error (GFL_BBD_TEXSIZDEF_1024x512-TEXSIZE_1024x512)
#endif
#if (GFL_BBD_TEXSIZDEF_1024x1024-TEXSIZE_1024x1024)
#error (GFL_BBD_TEXSIZDEF_1024x1024-TEXSIZE_1024x1024)
#endif
#endif //MMDL_CHECK_GFL_BBD_TEXSIZDEF
