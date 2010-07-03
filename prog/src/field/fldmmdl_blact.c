//======================================================================
/**
 * @file  fldmmdl_blact.c
 * @brief  ���샂�f�� �r���{�[�h�A�N�^�[�Ǘ�
 * @author  kagaya
 * @date  05.07.13
 */
//======================================================================
#include "fldmmdl.h"

#include "field/fldmmdl_list_symbol.h"

//======================================================================
//  debug
//======================================================================
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
//  define
//======================================================================
#define REGIDCODE_MAX (0xffff) ///<BBDRESID�ő�
#define BLACT_RESID_NULL (0xffff) ///<�r���{�[�h���\�[�X ����ID
//#define RES_DIGEST_FRAME_MAX (4) ///<1�t���[���ɏ����ł��郊�\�[�X��
#define RES_DIGEST_FRAME_MAX (1) ///<1�t���[���ɏ����ł��郊�\�[�X��
#define DMYACT_MAX (4) ///<�_�~�[�A�N�^�[����

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

///�r���{�[�h�`��X�P�[���B0x1000=x1:0x2000=x2
#define MMDL_BBD_GLOBAL_SCALE ((FX32_ONE+0xc00)*4)

///�r���{�[�h�`��X�P�[�����{ 0x2000=x2
#define MMDL_BBD_GLOBAL_SCALE_ONE (FX32_ONE*4)

///�r���{�[�h�`��T�C�Y 0x8000=1 0x4000=1/2
//#define MMDL_BBD_DRAW_SIZE (FX16_ONE*8-1)
#define MMDL_BBD_DRAW_SIZE (FX16_ONE*4)

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
///  IDCODE
//--------------------------------------------------------------
typedef struct
{
  u16 code; ///<�o�^�R�[�h
  u16 id; ///<�o�^�R�[�h����Ԃ�ID
  u16 flag; ///<�o�^���e��\���t���O
  u16 dmy;
}IDCODE;

//--------------------------------------------------------------
///  IDCODEIDX
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
  u16 res_idx;
  u8 mdl_size;
  u8 tex_size;
  BBDRESBIT flag;
}ADDRES_RESERVE_PARAM;

//--------------------------------------------------------------
/// ADDRES_RESERVE
//--------------------------------------------------------------
typedef struct
{
  u8 compFlag; //�f�[�^�̐ݒ肪���������t���O
  BBDRESBIT flag; //�o�^�p�t���O
  
  u16 code; //�o�^�p�R�[�h
  u8 mdl_size; //���f���T�C�Y
  u8 tex_size; //�e�N�X�`���T�C�Y
  
  MMDL *mmdl; //�֘A�̓��샂�f�� �����ꍇ��NULL
  GFL_G3D_RES *pG3dRes; //�o�^���\�[�X
}ADDRES_RESERVE;

//--------------------------------------------------------------
/// ADDACT_RESERVE
//--------------------------------------------------------------
typedef struct
{
  u16 *outID;
  MMDL *mmdl;
  
  u16 code; //OBJ�R�[�h
  u8 mdl_size;
  u8 tex_size;
  
  u8 anm_id; //�A�j��ID
  u8 compFlag;    //�f�[�^�̐ݒ肪���������t���O
  u8 padding[2];
  
  GFL_G3D_RES *pTransActRes; ///<�]���p�A�N�^�[���\�[�X
  
  ///���[�U�[�����w�肷��A�N�^�[�ǉ��������ɌĂяo���֐�
  MMDL_BLACTCONT_ADDACT_USERPROC user_init_proc;
  ///���[�U�[�����w�肷��user_init_proc�Őݒ肷�郏�[�N
  void *user_init_work;
  ///���[�U�[�����w�肷��A�N�^�[�ǉ����̍��W
  VecFx32 user_add_pos;
}ADDACT_RESERVE;

//--------------------------------------------------------------
/// DELRES_RESERVE
//--------------------------------------------------------------
typedef struct
{
  u16 compFlag; //�f�[�^�ݒu�����ӂ炮
  u16 res_idx;  //�폜���郊�\�[�X�C���f�b�N�X
}DELRES_RESERVE;

//--------------------------------------------------------------
/// DMYACT_RESCHG
//--------------------------------------------------------------
typedef struct
{
  MMDL *mmdl;
  u16 dmy_act_id;
  u16 dmy_obj_code;
  u16 next_code;
  u8 padding[2];
}DMYACT_RESCHG;

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
  DELRES_RESERVE *pReserveDelRes;
}BLACT_RESERVE;

//--------------------------------------------------------------
///  MMDL_BLACTCONT
//--------------------------------------------------------------
struct _TAG_MMDL_BLACTCONT
{
  u8 bbdUpdateFlag;
  u8 padding;
  u16 resourceMax;
  
  GFL_BBDACT_SYS *pBbdActSys; //���[�U�[����
  GFL_BBDACT_RESUNIT_ID bbdActResUnitID;
  GFL_BBDACT_ACTUNIT_ID bbdActActUnitID;
  u16 bbdActResCount;
  u16 bbdActActCount;
  
  MMDLSYS *mmdlsys;
  
  IDCODEIDX BBDResUnitIdx;
  BLACT_RESERVE *pReserve;
  DMYACT_RESCHG *pDmyActTbl;
};

//======================================================================
//  proto
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
static void BBDResUnitIndex_AddResource(
    MMDL_BLACTCONT *pBlActCont, GFL_G3D_RES *g3dres,
    u16 obj_code, u8 mdl_size, u8 tex_size, BBDRESBIT flag );
static void BBDResUnitIndex_AddResUnit(
    MMDL_BLACTCONT *pBlActCont, const OBJCODE_PARAM *pParam, BBDRESBIT flag );
static void BBDResUnitIndex_RemoveResUnit(
    MMDL_BLACTCONT *pBlActCont, u16 obj_code );
static void BBDResUnitIndex_RegistRemoveResUnit(
    MMDL_BLACTCONT *pBlActCont, u16 act_id, u16 obj_code );
static BOOL BBDResUnitIndex_SearchResID(
  MMDL_BLACTCONT *pBlActCont, u16 obj_code, u16 *outID, u16 *outFlag );

static void BlActAddReserve_Init( MMDL_BLACTCONT *pBlActCont );
static void BlActAddReserve_Delete( MMDL_BLACTCONT *pBlActCont );
static u32 BlActAddReserve_CheckReserve( const MMDL_BLACTCONT *pBlActCont );
static void BlActAddReserve_RegistResourceParam(MMDL_BLACTCONT *pBlActCont,
    u16 code, u8 mdl_size, u8 tex_size, u16 res_idx, BBDRESBIT flag );
static void BlActAddReserve_DigestResourceParam(
    MMDL_BLACTCONT *pBlActCont );
static BOOL BlActAddReserve_RegistResource( MMDL_BLACTCONT *pBlActCont,
    u16 code, u8 mdl_size, u8 tex_size, u16 res_idx, BBDRESBIT flag );
static void BlActAddReserve_DigestResource( MMDL_BLACTCONT *pBlActCont );
static BOOL BlActAddReserve_SearchResource(
    MMDL_BLACTCONT *pBlActCont, u16 code, u16 *outFlag );
static BOOL BlActAddReserve_CancelResource(
    MMDL_BLACTCONT *pBlActCont, u16 code );
static void BlActAddReserve_RegistActor(
    MMDL_BLACTCONT *pBlActCont,
    MMDL *mmdl,
    const OBJCODE_PARAM *pParam,
    u16 *outID,
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
static BOOL BlActDelReserve_RegistResource(
    MMDL_BLACTCONT *pBlActCont, u16 res_idx );

static void DummyAct_Init( MMDL_BLACTCONT *pBlActCont );
static void DummyAct_Delete( MMDL_BLACTCONT *pBlActCont );
static void DummyAct_Update( MMDL_BLACTCONT *pBlActCont );
static BOOL DummyAct_CheckMMdl(
    MMDL_BLACTCONT *pBlActCont, const MMDL *mmdl, u16 actID );

static const MMDL_BBDACT_ANMTBL * BlActAnm_GetAnmTbl( u32 no );
static GFL_BBDACT_RESUNIT_ID BlActRes_AddResCore(
    MMDL_BLACTCONT *pBlActCont,
    u16 code, u8 mdl_size, u8 tex_size,
    GFL_G3D_RES *g3dres, GFL_BBDACT_RESTYPE type );
#if 0
static GFL_BBDACT_RESUNIT_ID BlActRes_AddRes(
    MMDL_BLACTCONT *pBlActCont, u16 code,
    GFL_G3D_RES *g3dres, GFL_BBDACT_RESTYPE type );
static GFL_BBDACT_RESUNIT_ID BlActRes_AddResMMdl(
    MMDL_BLACTCONT *pBlActCont, const MMDL *mmdl, 
    GFL_G3D_RES *g3dres, GFL_BBDACT_RESTYPE type );
#endif

static const u16 data_BBDTexSizeTbl[TEXSIZE_MAX];

//test data
/*
const GFL_BBDACT_RESDATA testResTable[];
const u32 testResTableMax;
*/

//======================================================================
//  �t�B�[���h���샂�f���@�r���{�[�h�A�N�^�[�Ǘ�
//======================================================================
//--------------------------------------------------------------
/**
 * MMDL_BLACTCONT �r���{�[�h�A�N�^�[�Ǘ��@�Z�b�g�A�b�v
 * @param  mmdlsys  �\�z�ς݂�MMDLSYS
 * @param  pBbdActSys  �\�z�ς݂�GFL_BBDACT_SYS
 * @param res_max ���\�[�X���ő�
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_BLACTCONT_Setup( MMDLSYS *mmdlsys,
  GFL_BBDACT_SYS *pBbdActSys, int res_max )
{
  HEAPID heapID;
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
  DummyAct_Init( pBlActCont );

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
  }
}

//--------------------------------------------------------------
/**
 * MMDL_BLACTCONT �r���{�[�h�A�N�^�[�Ǘ��@�폜
 * @param  mmdlsys  MMDLSYS
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_BLACTCONT_Release( MMDLSYS *mmdlsys )
{
  MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( mmdlsys );
  DummyAct_Delete( pBlActCont );
  BlActAddReserve_Delete( pBlActCont );
  BBDResUnitIndex_Delete( pBlActCont );
  
  GFL_HEAP_FreeMemory( pBlActCont );
  MMDLSYS_SetBlActCont( mmdlsys, NULL );
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
    
    DummyAct_Update( pBlActCont ); //�_�~�[�A�N�^�[�X�V����
    
    pBlActCont->bbdUpdateFlag = 0;
  }
}

//--------------------------------------------------------------
/**
 * MMDL_BLACTCONT V�u�����N����
 * @param  mmdlsys  MMDLSYS
 * @retval  nothing
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

//--------------------------------------------------------------
/**
 * MMDL_BLACTCONT �\�񃊃\�[�X�A�A�N�^�[�`�F�b�N
 * @param
 * @retval
 */
//--------------------------------------------------------------
BOOL MMDL_BLACTCONT_IsThereReserve( const MMDLSYS *mmdlsys )
{
  const MMDL_BLACTCONT *pBlActCont =
    MMDLSYS_GetBlActCont( (MMDLSYS*)mmdlsys );

  if( BlActAddReserve_CheckReserve(pBlActCont) == TRUE ){
    return( TRUE );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL_BLACTCONT �r���{�[�h�A�N�^�[�����삵�Ă��邩�`�F�b�N
 * @param mmdl MMDL* ���֐�����MMDL�w��
 * @retval BOOL TRUE=���삵�Ă��� FALSE=���Ă��Ȃ�
 */
//--------------------------------------------------------------
BOOL MMDL_BLACTCONT_CheckUpdateBBD( const MMDL *mmdl )
{
  const MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
  const MMDL_BLACTCONT *pBlActCont =
    MMDLSYS_GetBlActCont( (MMDLSYS*)mmdlsys );
  return( pBlActCont->bbdUpdateFlag );
}

//======================================================================
//  �t�B�[���h���샂�f���@�r���{�[�h���\�[�X
//======================================================================
#if 0
//--------------------------------------------------------------
/**
 * MMDL_BLACTCONT ���\�[�X�ǉ� ���M�����[�AVRAM�풓�^�œo�^
 * @param  mmdlsys  MMDLSYS
 * @param  code �\���R�[�h�z��
 * @param  max code�v�f��
 * @retval  nothing
 * @note �Ă΂ꂽ���̏�œǂݍ��݂���������B
 * @attention �A�[�J�C�u�f�[�^��A���Ń��[�h����ׁA�������x�͏d���B
 */
//--------------------------------------------------------------
void MMDL_BLACTCONT_AddResourceTex(
  MMDLSYS *mmdlsys, const u16 *code, int max )
{
  OBJCODE_PARAM param;
  const OBJCODE_PARAM_BUF_BBD *prm_bbd;
  MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( mmdlsys );
  
  while( max ){
    MMDLSYS_LoadOBJCodeParam( mmdlsys, *code, &param );
    BBDResUnitIndex_AddResUnit( pBlActCont, &param, BBDRES_VRAM_REGULAR );
    code++;
    max--;
  }
}
#endif

//--------------------------------------------------------------
/**
 * MMDL_BLACTCONT ���\�[�X�ǉ� ���M�����[�AVRAM�풓�^�œo�^
 * @param  mmdlsys  MMDLSYS
 * @param  code �\���R�[�h�z��
 * @param  max code�v�f��
 * @retval  nothing
 * @note �Ă΂ꂽ���̏�œǂݍ��݂���������B
 * @attention �A�[�J�C�u�f�[�^��A���Ń��[�h����ׁA�������x�͏d���B
 */
//--------------------------------------------------------------
void MMDL_BLACTCONT_AddResourceTexCode( MMDLSYS *mmdlsys, const u16 code )
{
  OBJCODE_PARAM param;
  const OBJCODE_PARAM_BUF_BBD *prm_bbd;
  MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( mmdlsys );
  
  MMDLSYS_LoadOBJCodeParam( mmdlsys, code, &param );
  BBDResUnitIndex_AddResUnit( pBlActCont, &param, BBDRES_VRAM_REGULAR );
}

//--------------------------------------------------------------
/**
 * MMDL_BLACTCONT ���\�[�X�ǉ� ���M�����[�AVRAM�풓�^�œo�^
 * @param  mmdlsys  MMDLSYS
 * @param  code �\���R�[�h�z��
 * @param  max code�v�f��
 * @retval  nothing
 * @note �Ă΂ꂽ���̏�œǂݍ��݂���������B
 * @attention �A�[�J�C�u�f�[�^��A���Ń��[�h����ׁA�������x�͏d���B
 */
//--------------------------------------------------------------
void MMDL_BLACTCONT_AddResourceTex( MMDLSYS *mmdlsys )
{
  int i;
  OBJCODE_PARAM param;
  const OBJCODE_PARAM_BUF_BBD *prm_bbd;
  MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( mmdlsys );
  int max = MMDLSYS_GetSysOBJCodeParamNum( mmdlsys );
  
  for(i=0;i<max;i++){
    MMDLSYS_GetSysOBJCodeParam( mmdlsys, i, &param );
    BBDResUnitIndex_AddResUnit( pBlActCont, &param, BBDRES_VRAM_REGULAR );
  }
}

//======================================================================
//  �t�B�[���h���샂�f�� �r���{�[�h�A�N�^�[�ǉ�
//======================================================================
//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[�@�A�N�^�[�ǉ�
 * @param mmdlsys MMDLSYS
 * @retval GFL_BBDACT_ACYUNIT_ID
 */
//--------------------------------------------------------------
static GFL_BBDACT_ACTUNIT_ID blact_AddActorCore(
  MMDLSYS *mmdlsys , const VecFx32 *pos,
  MMDL_BLACT_MDLSIZE mdl_size, MMDL_BLACT_ANMTBLNO anm_id, u16 resID )
{
  GFL_BBDACT_ACTDATA actData;
  GFL_BBDACT_ACTUNIT_ID actID;
  MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( mmdlsys );
  
  actData.resID = resID;
  
  {
    const u16 *size = DATA_MMDL_BLACT_MdlSizeDrawSize[mdl_size];
    actData.sizX = size[0];
    actData.sizY = size[1];
  }
  
  actData.trans = *pos;
  actData.alpha = 31;
  actData.drawEnable = TRUE;
  actData.lightMask = GFL_BBD_LIGHTMASK_0;
  actData.work = pBlActCont;
  actData.func = BlActFunc;
  
  actID = GFL_BBDACT_AddAct(
    pBlActCont->pBbdActSys, pBlActCont->bbdActResUnitID, &actData, 1 );
   
  {
    const MMDL_BBDACT_ANMTBL *anmTbl = BlActAnm_GetAnmTbl( anm_id );
    
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
 * @param  mmdl  MMDL
 * @param  resID �ǉ�����A�N�^�[�Ŏg�p���郊�\�[�X�C���f�b�N�X
 * @retval GFL_BBDACT_ACTUNIT_ID
 */
//--------------------------------------------------------------
static GFL_BBDACT_ACTUNIT_ID blact_AddActor( MMDL *mmdl, u16 resID )
{
  VecFx32 pos;
  MMDLSYS *mmdlsys = (MMDLSYS*)MMDL_GetMMdlSys( mmdl );
  const OBJCODE_PARAM *pParam = MMDL_GetOBJCodeParam( mmdl );
  MMDL_GetDrawVectorPos( mmdl, &pos );
  
  return( blact_AddActorCore(
        mmdlsys,&pos,pParam->mdl_size,pParam->anm_id,resID) );
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
    MMDLSYS *mmdlsys, const VecFx32 *pos,
    MMDL_BLACT_MDLSIZE mdl_size, MMDL_BLACT_ANMTBLNO anm_id,
    u16 resID, u16 transResID )
{
  GFL_BBDACT_ACTUNIT_ID actID;
  
  actID = blact_AddActorCore( mmdlsys, pos, mdl_size, anm_id, resID );
  
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
    MMDL *mmdl, u16 resID, u16 transResID )
{
  VecFx32 pos;
  MMDLSYS *mmdlsys = (MMDLSYS*)MMDL_GetMMdlSys( mmdl );
  const OBJCODE_PARAM *param = MMDL_GetOBJCodeParam( mmdl );
  MMDL_GetDrawVectorPos( mmdl, &pos );
  
  return( blact_SetResActorCore(
        mmdlsys,&pos,param->mdl_size,param->anm_id,resID,transResID) );
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
    data.celSizX = 32;        //������mdl_size����
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
    D_MMDL_DPrintf( "MMDL ADD GUEST RESOURCE %xH\n", code );
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
  
  actID = blact_AddActor( mmdl, resID );
  return( actID );
}

#if 0
//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[ �ǉ�
 * @param  mmdl  MMDL
 * @param  code �g�p����OBJ�R�[�h
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
    D_MMDL_DPrintf( "MMDL ADD GUEST RESOURCE %xH\n", code );
    #endif
  }
  
  actData.sizX = FX16_ONE*8-1;
  actData.sizY = FX16_ONE*8-1;
  actData.trans = pos;
  actData.alpha = 31;
  actData.drawEnable = TRUE;
  actData.lightMask = GFL_BBD_LIGHTMASK_0;
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
 * @param  mmdl  MMDL
 * @param  code �g�p����OBJ�R�[�h
 * @param outID �ǉ��A�N�^�[ID�i�[�� MMDL_BLACTID_NULL=�ǉ���
 * @retval BOOL TRUE=�ǉ��BFALSE=�ǉ����B
 */
//--------------------------------------------------------------
BOOL MMDL_BLACTCONT_AddActor( MMDL *mmdl, GFL_BBDACT_ACTUNIT_ID *outID )
{
  u16 resID,flag;
  MMDLSYS *mmdlsys = (MMDLSYS*)MMDL_GetMMdlSys( mmdl );
  MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( mmdlsys );
  const OBJCODE_PARAM *pParam = MMDL_GetOBJCodeParam( mmdl );
  
  BBDResUnitIndex_SearchResID( pBlActCont, pParam->code, &resID, &flag );
  
  if( resID != REGIDCODE_MAX && (flag&BBDRESBIT_TRANS) == 0 ){
    (*outID) = blact_SetActor( mmdl, pParam->code );
    return( TRUE );
  }
  
  *outID = MMDL_BLACTID_NULL; //�\��o�^
  BlActAddReserve_RegistActor(
      pBlActCont, mmdl, pParam, outID, NULL, NULL, NULL );
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[�@���\�[�X�폜
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void blact_DeleteResource(
    MMDL_BLACTCONT *pBlActCont, MMDL *mmdl, u16 actID, u16 code )
{
  u16 id,flag,res_idx;
  
  if( BBDResUnitIndex_SearchResID(pBlActCont,code,&id,&flag) == TRUE )
  {
    if( (flag&BBDRESBIT_TRANS) ) //�]���^ �A�N�^�[�̓]���p���\�[�X�폜
    {
      u16 res_idx = GFL_BBDACT_GetResIdx( pBlActCont->pBbdActSys, actID );
      GFL_BBDACT_RemoveResourceUnit( pBlActCont->pBbdActSys, res_idx, 1 );
    }
    
    if( (flag&BBDRESBIT_GUEST) ) //�Q�X�g�o�^ ���ɗ��p������΍폜
    {
      if( MMDL_SearchUseOBJCode(mmdl,code) == FALSE )
      {
        #if 1 //���̏�ō폜����
        BBDResUnitIndex_RemoveResUnit( pBlActCont, code );
        #else //���̏�ō폜�����B���\�[�X�폜��\�񂷂�
        BBDResUnitIndex_RegistRemoveResUnit( pBlActCont, actID, code );
        #endif
        D_MMDL_DPrintf(
          "MMDL DEL GUEST RESOURCE CODE=%d, RESID=%d\n", code, id );
        return;
      }
    }
  }
}

//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[ �폜
 * @param  mmdl  MMDL*
 * @param  actID GFL_BBDACT_ACTUNIT_ID
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_BLACTCONT_DeleteActor( MMDL *mmdl, u32 actID )
{
  u16 id,flag,res_idx;
  u16 code = MMDL_GetOBJCode( mmdl );
  MMDLSYS *pMMdlSys = (MMDLSYS*)MMDL_GetMMdlSys( mmdl );
  MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( pMMdlSys );
  
  if( actID == MMDL_BLACTID_NULL ){ //�܂��ǉ�����Ă��Ȃ��B
    BlActAddReserve_CancelActor( pBlActCont, mmdl ); //�\�񂠂�΃L�����Z��
    return;
  }
  
  if( DummyAct_CheckMMdl(pBlActCont,mmdl,actID) == TRUE ){
    return; //�_�~�[�o�^����Ă���
  }
  
  blact_DeleteResource( pBlActCont, mmdl, actID, code );
  GFL_BBDACT_RemoveAct( pBlActCont->pBbdActSys, actID, 1 );
}

#if 0 //old
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
        
        D_MMDL_DPrintf(
          "MMDL DEL GUEST RESOURCE CODE=%d, RESID=%d\n", code, id );
      }
    }
  }
  
  GFL_BBDACT_RemoveAct( pBlActCont->pBbdActSys, actID, 1 );
}
#endif

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
    const OBJCODE_PARAM *pParam,
    MMDL_BLACTWORK_USER *userAct, const VecFx32 *pos,
    MMDL_BLACTCONT_ADDACT_USERPROC init_proc, void *init_work )
{
  u16 resID,flag;
  MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( mmdlsys );
  
  BBDResUnitIndex_SearchResID(
      pBlActCont, pParam->code, &resID, &userAct->flag );
  GF_ASSERT( resID != REGIDCODE_MAX ); //���\�[�X����
  
  if( (userAct->flag&BBDRESBIT_TRANS) == 0 ){
    userAct->actID = blact_AddActorCore(
        mmdlsys, pos, pParam->mdl_size, pParam->anm_id, resID );
    return( TRUE );
  }
  
  userAct->actID = MMDL_BLACTID_NULL; //���\�[�X�]���^
  
  BlActAddReserve_RegistActor( pBlActCont,
      NULL, pParam,
      &userAct->actID,
      init_proc, init_work, pos );
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
//  �t�B�[���h���샂�f���@�r���{�[�h�A�N�^�[����
//======================================================================
//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[����֐�
 * @param  bbdActSys  GFL_BBDACT_SYS
 * @param  actIdx    int
 * @param  work    void*
 * @retval  nothing
 */
//--------------------------------------------------------------
static void BlActFunc( GFL_BBDACT_SYS *bbdActSys, int actIdx, void *work )
{
  //�r���{�[�h�A�N�^�[�����삵����������
  ((MMDL_BLACTCONT*)work)->bbdUpdateFlag |= 1;
}

//======================================================================
//  �t�B�[���h���샂�f���@�r���{�[�h�A�N�^�[�Ǘ��@�Q��
//======================================================================
//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[�Ǘ��@GFL_BBDACT_SYS�擾
 * @param  pBlActCont MMDL_BLACTCONT
 * @retval  GFL_BBDACT_SYS
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
 * @param  pBlActCont MMDL_BLACTCONT
 * @retval  GFL_BBDACT_RESUNIT_ID
 */
//--------------------------------------------------------------
GFL_BBDACT_RESUNIT_ID MMDL_BLACTCONT_GetResUnitID(
    MMDL_BLACTCONT *pBlActCont )
{
  return( pBlActCont->bbdActResUnitID );
}

//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[�@�w�肳�ꂽ���샂�f���̃A�N�^�[�`��T�C�Y���擾����
 * @param
 * @retval
 */
//--------------------------------------------------------------
void MMDL_BLACTCONT_GetMMdlDrawSize(
    const MMDL *mmdl, u16 *pSizeX, u16 *pSizeY )
{
  const OBJCODE_PARAM *prm = MMDL_GetOBJCodeParam( mmdl );
  const u16 *size = DATA_MMDL_BLACT_MdlSizeDrawSize[prm->mdl_size];
  *pSizeX = size[0];
  *pSizeY = size[1];
}

//======================================================================
//  IDCODEIDX
//======================================================================
//--------------------------------------------------------------
/**
 * IDCODEIDX ������
 * @param  idx IDCODEIDX*
 * @param  max �o�^�ő吔
 * @param  heapID HEAPID
 * @retval  nothing
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
 * @param  idx  IDCODEIDX*
 * @retval  nothing
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
 * @param  idx IDCODEIDX
 * @param  code �R�[�h
 * @param  id ID
 * @retval  nothing
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
  
  GF_ASSERT_MSG( 0,
      "MMDL BLACT RESOURCE MAX(%d) OBJCODE %d RES %d", idx->max, code, id );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX �w�肳�ꂽ�R�[�h���폜
 * @param  idx IDCODEIDX
 * @param  code OBJ�R�[�h
 * @retval  nothing
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
  
  GF_ASSERT_MSG( 0,
      "MMDL BLACT NONE RESOURCE OBJCODE %d", code );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX �w�肳�ꂽ�R�[�h�Ɉ�v����ID�A�t���O��Ԃ�
 * @param  idx IDCODEIDX
 * @param  code OBJ�R�[�h
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
 * @param  pBlActCont MMDL_BLACTCONT
 * @param  max  �o�^�ő吔
 * @retval  nothing
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
 * @param  pBlActCont MMDL_BLACTCONT
 * @param  max  �o�^�ő吔
 * @retval  nothing
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
 * @param  pBlActCont MMDL_BLACTCONT
 * @param  code  OBJ�R�[�h
 * @param flag �o�^�t���O BBDRESBIT_VRAM��
 * @retval  nothing
 */
//--------------------------------------------------------------
static void BBDResUnitIndex_AddResource(
    MMDL_BLACTCONT *pBlActCont, GFL_G3D_RES *g3dres,
    u16 obj_code, u8 mdl_size, u8 tex_size, BBDRESBIT flag )
{
  GFL_BBDACT_RESUNIT_ID id;
  GFL_BBDACT_RESTYPE type;
  
  type = GFL_BBDACT_RESTYPE_DATACUT; //��{�̓��\�[�X�j���^
  
  if( (flag&BBDRESBIT_TRANS) ){ //�]���p���\�[�X�w��
    type = GFL_BBDACT_RESTYPE_TRANSSRC;
  }
  
  id = BlActRes_AddResCore(
      pBlActCont, obj_code, mdl_size, tex_size, g3dres, type );
  IDCodeIndex_RegistCode( &pBlActCont->BBDResUnitIdx, obj_code, id, flag );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX BBDACT ���\�[�X���j�b�g�ǉ�
 * @param  pBlActCont MMDL_BLACTCONT
 * @param  code  OBJ�R�[�h
 * @param flag �o�^�t���O BBDRESBIT_VRAM��
 * @retval  nothing
 */
//--------------------------------------------------------------
static void BBDResUnitIndex_AddResUnit(
    MMDL_BLACTCONT *pBlActCont, const OBJCODE_PARAM *pParam, BBDRESBIT flag )
{
  GFL_G3D_RES *g3dres;
  const OBJCODE_PARAM_BUF_BBD *prm_bbd;
  
  prm_bbd = MMDL_TOOL_GetOBJCodeParamBufBBD( pParam );
  
  g3dres = GFL_G3D_CreateResourceHandle(
      MMDLSYS_GetResArcHandle(pBlActCont->mmdlsys), prm_bbd->res_idx );
  BBDResUnitIndex_AddResource( pBlActCont, g3dres,
      pParam->code, pParam->mdl_size, pParam->tex_size, flag );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX BBDACT �w��OBJ�R�[�h�̃��\�[�X���j�b�g�폜
 * @param  pBlActCont MMDL_BLACTCONT
 * @param  code  OBJ�R�[�h
 * @retval  nothing
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
 * IDCODEIDX BBDACT �w��OBJ�R�[�h�̃��\�[�X���j�b�g�폜�B
 * �o�^�f�[�^�͍폜���A���\�[�X�폜�\����s���B
 * @param  pBlActCont MMDL_BLACTCONT
 * @param  code  OBJ�R�[�h
 * @retval  nothing
 */
//--------------------------------------------------------------
static void BBDResUnitIndex_RegistRemoveResUnit(
    MMDL_BLACTCONT *pBlActCont, u16 act_id, u16 obj_code )
{
  GFL_BBDACT_RESUNIT_ID id;
  BOOL ret = IDCodeIndex_SearchCode(
      &pBlActCont->BBDResUnitIdx, obj_code, &id, NULL );
  GF_ASSERT( ret == TRUE );
  
  if( BlActDelReserve_RegistResource(pBlActCont,id) == FALSE ){
    //�\���t�Ȃ炻�̏�ō폜
    GF_ASSERT( 0 && "MMDL RESERVE DEL RES MAX" );
    GFL_BBDACT_RemoveResourceUnit( pBlActCont->pBbdActSys, id, 1 );
  }
  
  IDCodeIndex_DeleteCode( &pBlActCont->BBDResUnitIdx, obj_code );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX BBDACT�@�w��R�[�h�̃��\�[�XID���擾
 * @param  pBlActCont MMDL_BLACTCONT
 * @param  code  OBJ�R�[�h
 * @retval  BOOL TRUE=�w��R�[�h�o�^�L��
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
 * @param  mmdlsys  MMDLSYS
 * @param  code OBJ�R�[�h
 * @retval  BOOL  TRUE=�ǉ��ς� FALSE=�ǉ��܂�
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
 * @param  mmdlssy  MMDLSYS
 * @param  code  OBJ�R�[�h
 * @param trans TRUE=�e�N�X�`���]���^�œo�^
 * @param guest TRUE=�g�p����Ȃ��Ȃ�Ɣj�������Q�X�g�^�C�v�œo�^
 * @retval  BOOL  TRUE=�ǉ������BFALSE=���ɒǉ��ς�
 * @note �Ă΂ꂽ���̏�œǂݍ��݂���������B
 */
//--------------------------------------------------------------
BOOL MMDL_BLACTCONT_AddOBJCodeRes(
    MMDLSYS *mmdlsys, u16 code, BOOL trans, BOOL guest )
{
  if( MMDL_BLACTCONT_CheckOBJCodeRes(mmdlsys,code) == FALSE ){
    OBJCODE_PARAM param;
    MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( mmdlsys );
    BBDRESBIT flag = 0;
    if( trans ){ flag |= BBDRESBIT_TRANS; }
    if( guest ){ flag |= BBDRESBIT_GUEST; }
    MMDLSYS_LoadOBJCodeParam( mmdlsys, code, &param );
    BBDResUnitIndex_AddResUnit( pBlActCont, &param, flag );
    return( TRUE );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * IDCODEIDX BBDACT �w��R�[�h�̃��\�[�X���폜
 * @param  mmdlssy  MMDLSYS
 * @param  code  OBJ�R�[�h
 * @retval  nothing
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
  pReserve->pReserveDelRes = GFL_HEAP_AllocClearMemory(
      heapID, sizeof(DELRES_RESERVE)*pReserve->resMax );
  
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
          DEBUG_MMDL_RESOURCE_MEMORY_SIZE_Minus(
              GFL_G3D_GetResourceFileHeader( pRes->pG3dRes ) );
          DEBUG_MMDL_RESOURCE_MEMORY_SIZE_Minus( pRes->pG3dRes );
          GFL_G3D_DeleteResource( pRes->pG3dRes );
        }
      }
      GFL_HEAP_FreeMemory( pReserve->pReserveRes );
    }
    
    { //�폜���\�[�X
      DELRES_RESERVE *pDelRes = pReserve->pReserveDelRes;
      for( i = 0; i < pReserve->resMax; i++, pDelRes++ ){
        if( pDelRes->compFlag == TRUE ){
          pDelRes->compFlag = FALSE;
          GFL_BBDACT_RemoveResourceUnit(
              pBlActCont->pBbdActSys, pDelRes->res_idx, 1 );
        }
      }
      GFL_HEAP_FreeMemory( pReserve->pReserveDelRes );
    }
    
    { //�A�N�^�[
      ADDACT_RESERVE *pRes = pReserve->pReserveAct;
      for( i = 0; i < pReserve->actMax; i++, pRes++ ){
        if( pRes->outID != NULL && pRes->pTransActRes != NULL ){
          DEBUG_MMDL_RESOURCE_MEMORY_SIZE_Minus(
              GFL_G3D_GetResourceFileHeader( pRes->pTransActRes ) );
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
 * �r���{�[�h�A�N�^�[�ǉ��\�񏈗��@�\�񂠂邩�`�F�b�N
 * @param MMDL_BLACTCONT *pBlActCont
 * @retval BOOL TRUE=�\�񂠂�
 */
//--------------------------------------------------------------
static u32 BlActAddReserve_CheckReserve( const MMDL_BLACTCONT *pBlActCont )
{
  u32 i;
  u32 count = 0;
  const BLACT_RESERVE *pReserve = pBlActCont->pReserve;
  
  { //�\��p�����^
    const ADDRES_RESERVE_PARAM *pResParam = pReserve->pReserveResParam;
    
    for( i = 0; i < pReserve->resMax; i++, pResParam++ ){
      if( pResParam->code != REGIDCODE_MAX ){
        return( TRUE );
      }
    }
  }
  
  { //���\�[�X
    const ADDRES_RESERVE *pRes = pReserve->pReserveRes;
    
    for( i = 0; i < pReserve->resDigestFrameMax; i++, pRes++ ){
      if( pRes->pG3dRes != NULL ){
        return( TRUE );
      }
    }
  }

  { //�A�N�^�[
    const ADDACT_RESERVE *pRes = pReserve->pReserveAct;
    
    for( i = 0; i < pReserve->actMax; i++, pRes++ ){
      if( pRes->outID != NULL && pRes->pTransActRes != NULL ){
        return( TRUE );
      }
    }
  }
  
  return( FALSE );
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
static void BlActAddReserve_RegistResourceParam(MMDL_BLACTCONT *pBlActCont,
    u16 code, u8 mdl_size, u8 tex_size, u16 res_idx, BBDRESBIT flag )
{
  u32 i = 0;
  BLACT_RESERVE *pReserve = pBlActCont->pReserve;
  ADDRES_RESERVE_PARAM *pResParam = pReserve->pReserveResParam;
  
  for( ; i < pReserve->resMax; i++, pResParam++ ){
    if( pResParam->code == REGIDCODE_MAX ){
      pResParam->code = code;
      pResParam->mdl_size = mdl_size;
      pResParam->tex_size = tex_size;
      pResParam->res_idx = res_idx;
      pResParam->flag = flag;
      D_MMDL_DPrintf( "MMDL BLACT RESERVE REG RESPRM CODE=%d\n", code );
      return;
    }
  }
  
  GF_ASSERT_MSG( 0,
      "MMDL BLACT REG RESPRM MAX(%d) OBJCODE %d", pReserve->resMax, code );
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
      if( BlActAddReserve_RegistResource(pBlActCont,
          pResParam->code,pResParam->mdl_size,pResParam->tex_size,
          pResParam->res_idx,pResParam->flag) == FALSE ){
        D_MMDL_DPrintf( "MMDL DIG RESPRM RESERVE OVER\n" );
        break; //�\���t
      }
      
      pResParam->code = REGIDCODE_MAX;

      j++;
      if( j >= pReserve->resDigestFrameMax ){
        D_MMDL_DPrintf( "MMDL DIG RESPRM FRAME OVER\n" );
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
static BOOL BlActAddReserve_RegistResource( MMDL_BLACTCONT *pBlActCont,
    u16 code, u8 mdl_size, u8 tex_size, u16 res_idx, BBDRESBIT flag )
{
  u32 i = 0;
  BLACT_RESERVE *pReserve = pBlActCont->pReserve;
  ADDRES_RESERVE *pRes = pReserve->pReserveRes;
  
  for( ; i < pReserve->resDigestFrameMax; i++, pRes++ ){
    if( pRes->pG3dRes == NULL ){
      pRes->compFlag = FALSE;
      pRes->code = code;
      pRes->mdl_size = mdl_size;
      pRes->tex_size = tex_size;
      pRes->flag = flag;
      pRes->pG3dRes = GFL_G3D_CreateResourceHandle(
          MMDLSYS_GetResArcHandle(pBlActCont->mmdlsys), res_idx );
      
      pRes->compFlag = TRUE;
      
      DEBUG_MMDL_RESOURCE_MEMORY_SIZE_Plus(
          GFL_G3D_GetResourceFileHeader( pRes->pG3dRes ) );
      DEBUG_MMDL_RESOURCE_MEMORY_SIZE_Plus( pRes->pG3dRes );
      
      D_MMDL_DPrintf(
          "MMDL BLACT RESERVE ADD RESOURCE CODE=%d,ARCIDX=%d\n",
          pRes->code, res_idx );
      return( TRUE );
    }
  }
  
#if 0
  //�\��������o���Ă��Ȃ�����m�点��ASSERT
  GF_ASSERT_MSG( 0,
      "MMDL BLACT RESERVE REG RES MAX:CODE %d", code );
#else
  D_MMDL_Printf( 0,
      "MMDL BLACT RESERVE REG RES MAX:CODE %d", code );
#endif
  
  return( FALSE );
}


#if 0 //new
static BOOL BlActAddReserve_RegistResource( MMDL_BLACTCONT *pBlActCont,
    u16 code, u8 mdl_size, u8 tex_size, u16 res_idx, BBDRESBIT flag )
{
  u32 i = 0;
  BLACT_RESERVE *pReserve = pBlActCont->pReserve;
  ADDRES_RESERVE *pRes = pReserve->pReserveRes;
  
  for( ; i < pReserve->resDigestFrameMax; i++, pRes++ ){
    if( pRes->pG3dRes == NULL ){
      pRes->compFlag = FALSE;
      pRes->code = code;
      pRes->mdl_size = mdl_size;
      pRes->tex_size = tex_size;
      pRes->flag = flag;
      pRes->pG3dRes = GFL_G3D_CreateResourceHandle(
          MMDLSYS_GetResArcHandle(pBlActCont->mmdlsys), res_idx );
      
      pRes->compFlag = TRUE;
      
      DEBUG_MMDL_RESOURCE_MEMORY_SIZE_Plus(
          GFL_G3D_GetResourceFileHeader( pRes->pG3dRes ) );
      DEBUG_MMDL_RESOURCE_MEMORY_SIZE_Plus( pRes->pG3dRes );
      
      D_MMDL_DPrintf(
          "MMDL BLACT RESERVE ADD RESOURCE CODE=%d,ARCIDX=%d\n",
          pRes->code, res_idx );
      return( TRUE );
    }
  }
  
#if 0
  //�\��������o���Ă��Ȃ�����m�点��ASSERT
  GF_ASSERT_MSG( 0,
      "MMDL BLACT RESERVE REG RES MAX:CODE %d", code );
#else
  D_MMDL_Printf( 0,
      "MMDL BLACT RESERVE REG RES MAX:CODE %d", code );
#endif
  
  return( FALSE );
}
#endif

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
          
          BBDResUnitIndex_AddResource( pBlActCont, pRes->pG3dRes,
              pRes->code, pRes->mdl_size, pRes->tex_size, pRes->flag );
          
          pRes->pG3dRes = NULL;
          
          #ifdef DEBUG_MMDL_DEVELOP
          {
            u16 id,flag;
            BBDResUnitIndex_SearchResID(
                pBlActCont, pRes->code, &id, &flag );
            D_MMDL_DPrintf(
                "MMDL BLACT RESERVE DIG RESOURCE CODE=%d, RESID=%d\n",
                pRes->code, id );
          }
          #endif
        }
      }
      
      if( pBlActCont->bbdUpdateFlag ){ //���\�[�X�폜
        DELRES_RESERVE *pDelRes = pReserve->pReserveDelRes;
        
        for( i = 0; i < pReserve->resMax; i++, pDelRes++ ){
          if( pDelRes->compFlag == TRUE ){
            pDelRes->compFlag = FALSE;
            GFL_BBDACT_RemoveResourceUnit(
              pBlActCont->pBbdActSys, pDelRes->res_idx, 1 );
          }
        }
      }
    }
  }
}

/*
36704
  | 434
362d0
  | 274
3605c
  | 434
35c28
  | 274
359b4
  |
35580
3530c
34ed8
34c64
34830
345bc
34188
*/

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
        D_MMDL_DPrintf(
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
        D_MMDL_DPrintf( "MMDL BLACT RESERVE CANCEL CODE=%d\n", code );
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
        
        DEBUG_MMDL_RESOURCE_MEMORY_SIZE_Minus(
            GFL_G3D_GetResourceFileHeader( pRes->pG3dRes ) );
        DEBUG_MMDL_RESOURCE_MEMORY_SIZE_Minus( pRes->pG3dRes );
        
        GFL_G3D_DeleteResource( pRes->pG3dRes );
        pRes->pG3dRes = NULL;
        D_MMDL_DPrintf( "MMDL BLACT RESERVE CANCEL CODE=%d\n", code );
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
    MMDL_BLACTCONT *pBlActCont,
    MMDL *mmdl,
    const OBJCODE_PARAM *pParam,
    u16 *outID,
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
      
      pRes->compFlag = FALSE;
      pRes->mmdl = mmdl;
      pRes->outID = outID;
      pRes->code = pParam->code;
      pRes->mdl_size = pParam->mdl_size;
      pRes->tex_size = pParam->tex_size;
      pRes->anm_id = pParam->anm_id;
      
      BBDResUnitIndex_SearchResID( pBlActCont, pRes->code, &resID, &flag );
      
      if( resID == REGIDCODE_MAX ) //��o�^���\�[�X
      {
        if( BlActAddReserve_SearchResource( //���\�[�X�\�񌟍�
              pBlActCont,pRes->code,&flag) == FALSE )
        {
          const OBJCODE_PARAM_BUF_BBD *prm_bbd;
          prm_bbd = MMDL_TOOL_GetOBJCodeParamBufBBD( pParam );
          
          flag = BBDRES_VRAM_GUEST; //��{VRAM�풓�Q�X�g�^�œo�^
          BlActAddReserve_RegistResourceParam( pBlActCont,
              pRes->code, pRes->mdl_size, pRes->tex_size,
              prm_bbd->res_idx, flag );
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
      
      #ifdef DEBUG_MMDL_DEVELOP
      if( mmdl != NULL ){
        D_MMDL_DPrintf(
            "MMDL BLACT RESERVE ADD ACTOR OBJID=%d, CODE=%d\n", 
            MMDL_GetOBJID(mmdl), pRes->code );
      }else{
        D_MMDL_DPrintf(
            "MMDL BLACT RESERVE ADD ACTOR OBJID=NON, CODE=%d\n", 
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
    resID = BlActRes_AddResCore( pBlActCont,
        pRes->code, pRes->mdl_size, pRes->tex_size,
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
      D_MMDL_Printf( "BLACT ADD RESOURCE ERROR CODE=%xH\n", pRes->code );
    }
        
    //���\�[�X����
    GF_ASSERT( ret == TRUE && "BLACT ADD RESERVE RESOURCE NONE" );
  }
      
  if( pRes->mmdl != NULL ){ //�A�N�^�[�ǉ� ���샂�f��
    *(pRes->outID) = blact_SetResActor( pRes->mmdl, resID, transID );
  }else{ //�A�N�^�[�ǉ� ���[�U�[�w��
    *(pRes->outID) = blact_SetResActorCore( pBlActCont->mmdlsys,
        &pRes->user_add_pos, pRes->mdl_size, pRes->anm_id, resID, transID );
  }
      
  if( pRes->user_init_proc != NULL ){ //���[�U�[�w�菉���������Ăяo��
    pRes->user_init_proc( *pRes->outID, pRes->user_init_work );
  }
      
  if( pRes->mmdl != NULL ){
    MMDL_CallDrawProc( pRes->mmdl ); //�`�揈���Ăяo��
  }
      
  #ifdef DEBUG_MMDL_DEVELOP
  D_MMDL_DPrintf( "MMDL BLACT RESERVE DIG ACTOR " );
  if( pRes->mmdl != NULL ){
    D_MMDL_DPrintf( "OBJID=%d, CODE=%d, RESID=%d,TRANSID=%d\n",
      MMDL_GetOBJID(pRes->mmdl), pRes->code, resID, transID );
  }else{
    D_MMDL_DPrintf( "OBJID=NONE, CODE=%d, RESID=%d,TRANSID=%d\n",
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
      
      D_MMDL_DPrintf(
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
        DEBUG_MMDL_RESOURCE_MEMORY_SIZE_Minus(
            GFL_G3D_GetResourceFileHeader( pRes->pTransActRes ) );
        DEBUG_MMDL_RESOURCE_MEMORY_SIZE_Minus( pRes->pTransActRes );
        
        GFL_G3D_DeleteResource( pRes->pTransActRes );
        pRes->pTransActRes = NULL;
      }
      
      D_MMDL_DPrintf( "MMDL BLACT RESERVE CANCEL OBJID=%d, CODE=%d\n", 
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

//--------------------------------------------------------------
/**
 * ���\�[�X�폜�\��@�o�^
 * @param
 * @retval
 */
//--------------------------------------------------------------
static BOOL BlActDelReserve_RegistResource(
    MMDL_BLACTCONT *pBlActCont, u16 res_idx )
{
  u32 i = 0;
  u32 max = pBlActCont->pReserve->resMax;
  DELRES_RESERVE *pDelRes = pBlActCont->pReserve->pReserveDelRes;

  for( i = 0; i < max; i++, pDelRes++ ){
    if( pDelRes->compFlag == FALSE ){
      pDelRes->res_idx = res_idx;
      pDelRes->compFlag = TRUE;
      return( TRUE );
    }
  }
  
  return( FALSE );
}

//======================================================================
//  �_�~�[�A�N�^�[�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * �_�~�[�A�N�^�[�𗘗p��OBJ�R�[�h��ύX����
 * @param mmdl MMDL*
 * @param next_code �ύX����OBJ�R�[�h
 * @retval nothing
 * @note ����g�p���Ă���r���{�[�h�A�N�^�[���_�~�[�A�N�^�[�Ƃ��Ĉ���
 * ���̗���OBJ�R�[�h�̕ύX���s���B�ύX��Ƀ_�~�[�A�N�^�[�͍폜�����B
 */
//--------------------------------------------------------------
void MMDL_BLACTCONT_ChangeOBJCodeWithDummy( MMDL *mmdl, u16 next_code )
{
  int i;
  u16 actID,resID,flag;
  MMDLSYS *mmdlsys = (MMDLSYS*)MMDL_GetMMdlSys( mmdl );
  MMDL_BLACTCONT *pBlActCont = MMDLSYS_GetBlActCont( mmdlsys );
  DMYACT_RESCHG *pDmyAct = pBlActCont->pDmyActTbl;
  
  BBDResUnitIndex_SearchResID( pBlActCont, next_code, &resID, &flag );
  
  //���\�[�X�����ɗL���ԂȂ�_�~�[�o�^�̕K�v�Ȃ�
  if( resID != REGIDCODE_MAX && (flag&BBDRESBIT_TRANS) == 0 ){
    MMDL_ChangeOBJCode( mmdl, next_code );
    return;
  }
  
  //�A�N�^�[ID�����݂��Ȃ��ꍇ�̓_�~�[�͒ǉ������B
  actID = MMDL_CallDrawGetProc( mmdl, 0 );
  
  if( actID == MMDL_BLACTID_NULL ){
    MMDL_ChangeOBJCode( mmdl, next_code );
    return;
  }
  
  i = 0;
  while( i < DMYACT_MAX ){ 
    if( pDmyAct->mmdl == NULL ){
      break;
    }
    i++;
  }
    
  if( i >= DMYACT_MAX ){ //�_�~�[���t�B�A�T�[�g���o���o�^�͂����I���
    GF_ASSERT( 0 && "MMDL BLACT DUMMY MAX" );
    MMDL_ChangeOBJCode( mmdl, next_code );
    return;
  }
  
  //�_�~�[�o�^
  pDmyAct->mmdl = mmdl;
  pDmyAct->dmy_act_id = actID;
  pDmyAct->dmy_obj_code = MMDL_GetOBJCode( mmdl );
  pDmyAct->next_code = next_code;
  
  //next_code���w�肵���샂�f��OBJ�R�[�h�ύX�Ăяo��
  //�`��폜�������ĂуA�N�^�[���폜���邪
  //�_�~�[�A�N�^�[�ɓo�^����Ă���̂�
  //�A�N�^�[�폜�͍s��Ȃ��B
  //�����ĕ`�揉���������Ăяo����
  //������next_code��add_actor()���s��ꃊ�\�[�X�ǉ������N���锤
  MMDL_ChangeOBJCode( mmdl, next_code );
  
  //�O�̂��߂����ŃA�N�^�[�ǉ������`�F�b�N�B
  //�����ς݂Ȃ瑦�I��
  if( MMDL_CallDrawGetProc(mmdl,0) != MMDL_BLACTID_NULL ){
    GF_ASSERT( 0 ); //�{���݂蓾�Ȃ��B�A�T�[�g
    blact_DeleteResource(
        pBlActCont, mmdl, pDmyAct->dmy_act_id, pDmyAct->dmy_obj_code );
    GFL_BBDACT_RemoveAct( pBlActCont->pBbdActSys, pDmyAct->dmy_act_id, 1 );
    pDmyAct->mmdl = NULL;
  }
}

//--------------------------------------------------------------
/**
 * �_�~�[�A�N�^�[�@�A�N�^�[�e�[�u��������
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void DummyAct_Init( MMDL_BLACTCONT *pBlActCont )
{
  HEAPID heapID = MMDLSYS_GetHeapID( pBlActCont->mmdlsys );
  pBlActCont->pDmyActTbl = GFL_HEAP_AllocClearMemory(
      heapID, sizeof(DMYACT_RESCHG) * DMYACT_MAX );
}

//--------------------------------------------------------------
/**
 * �_�~�[�A�N�^�[�@�폜
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void DummyAct_Delete( MMDL_BLACTCONT *pBlActCont )
{
  DMYACT_RESCHG *pDmyAct = pBlActCont->pDmyActTbl;
  
  if( pDmyAct != NULL ){
    int i = 0;
    for( ; i < DMYACT_MAX; i++, pDmyAct++ ){
      if( pDmyAct->mmdl != NULL ){
        GFL_BBDACT_RemoveAct(
            pBlActCont->pBbdActSys, pDmyAct->dmy_act_id, 1 );
        pDmyAct->mmdl = NULL;
      }
    }
    
    GFL_HEAP_FreeMemory( pBlActCont->pDmyActTbl );
  }
}

//--------------------------------------------------------------
/**
 * �_�~�[�A�N�^�[�@�X�V
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void DummyAct_UpdateAct(
    MMDL_BLACTCONT *pBlActCont, DMYACT_RESCHG *pDmyAct )
{
  MMDL *mmdl = pDmyAct->mmdl;
  
  if( MMDL_CheckStatusBitUse(mmdl) == FALSE ){ //���S
    blact_DeleteResource(
        pBlActCont, mmdl, pDmyAct->dmy_act_id, pDmyAct->dmy_obj_code );
    GFL_BBDACT_RemoveAct( pBlActCont->pBbdActSys, pDmyAct->dmy_act_id, 1 );
    pDmyAct->mmdl = NULL;
  }else{
    u16 actID = MMDL_CallDrawGetProc( mmdl, 0 );

    if( actID != MMDL_BLACTID_NULL ){ //�o�^�����ꂽ
      if( MMDL_GetOBJCode(mmdl) != pDmyAct->dmy_obj_code ){ 
        //����ƈႤ�R�[�h�œo�^���ꂽ�Ȃ�_�~�[�A�N�^�[�p���\�[�X�͍폜
        blact_DeleteResource(
            pBlActCont, mmdl, pDmyAct->dmy_act_id, pDmyAct->dmy_obj_code );
      }
    
      GFL_BBDACT_RemoveAct(
          pBlActCont->pBbdActSys, pDmyAct->dmy_act_id, 1 );
      pDmyAct->mmdl = NULL;
    }
  }
}

//--------------------------------------------------------------
/**
 * �_�~�[�A�N�^�[�@�X�V����
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void DummyAct_Update( MMDL_BLACTCONT *pBlActCont )
{
  DMYACT_RESCHG *pDmyAct = pBlActCont->pDmyActTbl;
  
  if( pDmyAct != NULL ){
    int i = 0;
    for( ; i < DMYACT_MAX; i++, pDmyAct++ ){
      if( pDmyAct->mmdl != NULL ){
        DummyAct_UpdateAct( pBlActCont, pDmyAct );
      }
    }
  }
}

//--------------------------------------------------------------
/**
 * �_�~�[�A�N�^�[�@���샂�f���o�^�`�F�b�N
 * @param
 * @retval
 */
//--------------------------------------------------------------
static BOOL DummyAct_CheckMMdl(
    MMDL_BLACTCONT *pBlActCont, const MMDL *mmdl, u16 actID )
{
  DMYACT_RESCHG *pDmyAct = pBlActCont->pDmyActTbl;
  
  if( pDmyAct != NULL ){
    int i = 0;
    for( ; i < DMYACT_MAX; i++, pDmyAct++ ){
      if( pDmyAct->mmdl != NULL ){
        if( pDmyAct->mmdl == mmdl && pDmyAct->dmy_act_id == actID ){
          return( TRUE );
        }
      }
    }
  }
  
  return( FALSE );
}

//======================================================================
//  parts
//======================================================================
//--------------------------------------------------------------
/**
 * �e�[�u���ԍ�����A�N�^�[�A�j���e�[�u���擾
 * @param  no  MMDL_BLACT_ANMTBLNO_MAX
 * @retval  MMDL_BBDACT_ANMTBL*
 */
//--------------------------------------------------------------
static const MMDL_BBDACT_ANMTBL * BlActAnm_GetAnmTbl( u32 no )
{
  GF_ASSERT( no < MMDL_BLACT_ANMTBLNO_MAX );
  return( &DATA_MMDL_BBDACT_ANM_ListTable[no] );
}

//--------------------------------------------------------------
/**
 * OBJ�R�[�h�Ŏg�p����A�N�^�[�A�j���e�[�u���擾
 * @param
 * @retval
 */
//--------------------------------------------------------------
#if 0
const MMDL_BBDACT_ANMTBL * MMDL_BLACTCONT_GetObjAnimeTable(
  const MMDLSYS *mmdlsys, u16 code )
{
  u16 id;
  const MMDL_BBDACT_ANMTBL *anmTbl;
  const OBJCODE_PARAM *prm = MMDLSYS_GetOBJCodeParam( mmdlsys, code );
  
  id = prm->anm_id;
  
  if( prm->draw_type != MMDL_DRAWTYPE_BLACT ){
    GF_ASSERT( 0 );
    id = MMDL_BLACT_ANMTBLNO_BLACT;
  }
  
  anmTbl = BlActAnm_GetAnmTbl( id );
  return( anmTbl );
}
#endif

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
  GF_ASSERT( sta < DRAW_STA_HERO_MAX );
  //�㉺���E�̕��тŊe�S����
  return( sta * DIR_MAX4 );
}
#endif

//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[���\�[�X�ǉ��@�R�A����
 * @param pBlActCont MMDL_BLACTCONT
 * @param code OBJ�R�[�h
 * g3dres �ǉ����郊�\�[�X*
 * @param type ���\�[�X�^�C�v
 * @retval GFL_BBDACT_RESUNIT_ID
 */
//--------------------------------------------------------------
static GFL_BBDACT_RESUNIT_ID BlActRes_AddResCore(
    MMDL_BLACTCONT *pBlActCont,
    u16 code, u8 mdl_size, u8 tex_size,
    GFL_G3D_RES *g3dres, GFL_BBDACT_RESTYPE type )
{
  GFL_BBDACT_RESUNIT_ID id;
  GFL_BBDACT_G3DRESDATA data;
  
  data.g3dres = g3dres;
  data.texFmt = GFL_BBD_TEXFMT_PAL16;
  data.texSiz = data_BBDTexSizeTbl[tex_size];
  
  {
    const u16 *size = DATA_MMDL_BLACT_MdlSize[mdl_size];
    data.celSizX = size[0];
    data.celSizY = size[1];
    #if 0   
    D_MMDL_DPrintf( "�e�N�X�`�����\�[�X�ǉ� �Z���T�C�Y X=%d,Y=%d\n",
        data.celSizX, data.celSizY );
    #endif
  }
  
  data.dataCut = type;
  
  id = GFL_BBDACT_AddResourceG3DResUnit( pBlActCont->pBbdActSys, &data, 1 );
  
  D_MMDL_DPrintf( "MMDL ADD RESOURCE CODE=%d,RESID=%d\n", code, id );
  return( id );
}

//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[���\�[�X�ǉ� OBJ�R�[�h�w��
 * @param pBlActCont MMDL_BLACTCONT
 * @param code OBJ�R�[�h
 * g3dres �ǉ����郊�\�[�X*
 * @param type ���\�[�X�^�C�v
 * @retval GFL_BBDACT_RESUNIT_ID
 */
//--------------------------------------------------------------
#if 0 //�s�v�ƂȂ���
static GFL_BBDACT_RESUNIT_ID BlActRes_AddRes(
    MMDL_BLACTCONT *pBlActCont, u16 code, u16 res_idx,
    GFL_G3D_RES *g3dres, GFL_BBDACT_RESTYPE type )
{
  GFL_BBDACT_RESUNIT_ID id;
  OBJCODE_PARAM param;
  GFL_BBDACT_G3DRESDATA data;
  
  MMDLSYS_LoadOBJCodeParam( pBlActCont->mmdlsys, code, &param );
  return( BlActRes_AddResCore(pBlActCont,&param,g3dres,type) );
}
#endif

//--------------------------------------------------------------
/**
 * �r���{�[�h�A�N�^�[���\�[�X�ǉ� ���샂�f���w��
 * @param pBlActCont MMDL_BLACTCONT
 * @param code OBJ�R�[�h
 * g3dres �ǉ����郊�\�[�X*
 * @param type ���\�[�X�^�C�v
 * @retval GFL_BBDACT_RESUNIT_ID
 */
//--------------------------------------------------------------
#if 0 //����s�v
static GFL_BBDACT_RESUNIT_ID BlActRes_AddResMMdl(
    MMDL_BLACTCONT *pBlActCont, const MMDL *mmdl, 
    GFL_G3D_RES *g3dres, GFL_BBDACT_RESTYPE type )
{
  GFL_BBDACT_RESUNIT_ID id;
  const OBJCODE_PARAM *param;
  GFL_BBDACT_G3DRESDATA data;
  
  param = MMDL_GetOBJCodeParam( mmdl );
  return( BlActRes_AddResCore(pBlActCont,param,g3dres,type) );
}
#endif

//======================================================================
//  data
//======================================================================
static const u16 data_BBDTexSizeTbl[TEXSIZE_MAX] =
{
  GFL_BBD_TEXSIZ_8x8,
  GFL_BBD_TEXSIZ_8x16,
  GFL_BBD_TEXSIZ_8x32,
  GFL_BBD_TEXSIZ_8x64,
  GFL_BBD_TEXSIZ_8x128,
  GFL_BBD_TEXSIZ_8x256,
  GFL_BBD_TEXSIZ_8x512,
  GFL_BBD_TEXSIZ_8x1024,
  GFL_BBD_TEXSIZ_16x8,
  GFL_BBD_TEXSIZ_16x16,
  GFL_BBD_TEXSIZ_16x32,
  GFL_BBD_TEXSIZ_16x64,
  GFL_BBD_TEXSIZ_16x128,
  GFL_BBD_TEXSIZ_16x256,
  GFL_BBD_TEXSIZ_16x512,
  GFL_BBD_TEXSIZ_16x1024,
  GFL_BBD_TEXSIZ_32x8,
  GFL_BBD_TEXSIZ_32x16,
  GFL_BBD_TEXSIZ_32x32,
  GFL_BBD_TEXSIZ_32x64,
  GFL_BBD_TEXSIZ_32x128,
  GFL_BBD_TEXSIZ_32x256,
  GFL_BBD_TEXSIZ_32x512,
  GFL_BBD_TEXSIZ_32x1024,
  GFL_BBD_TEXSIZ_64x8,
  GFL_BBD_TEXSIZ_64x16,
  GFL_BBD_TEXSIZ_64x32,
  GFL_BBD_TEXSIZ_64x64,
  GFL_BBD_TEXSIZ_64x128,
  GFL_BBD_TEXSIZ_64x256,
  GFL_BBD_TEXSIZ_64x512,
  GFL_BBD_TEXSIZ_64x1024,
  GFL_BBD_TEXSIZ_128x8,
  GFL_BBD_TEXSIZ_128x16,
  GFL_BBD_TEXSIZ_128x32,
  GFL_BBD_TEXSIZ_128x64,
  GFL_BBD_TEXSIZ_128x128,
  GFL_BBD_TEXSIZ_128x256,
  GFL_BBD_TEXSIZ_128x512,
  GFL_BBD_TEXSIZ_128x1024,
  GFL_BBD_TEXSIZ_256x8,
  GFL_BBD_TEXSIZ_256x16,
  GFL_BBD_TEXSIZ_256x32,
  GFL_BBD_TEXSIZ_256x64,
  GFL_BBD_TEXSIZ_256x128,
  GFL_BBD_TEXSIZ_256x256,
  GFL_BBD_TEXSIZ_256x512,
  GFL_BBD_TEXSIZ_256x1024,
  GFL_BBD_TEXSIZ_512x8,
  GFL_BBD_TEXSIZ_512x16,
  GFL_BBD_TEXSIZ_512x32,
  GFL_BBD_TEXSIZ_512x64,
  GFL_BBD_TEXSIZ_512x128,
  GFL_BBD_TEXSIZ_512x256,
  GFL_BBD_TEXSIZ_512x512,
  GFL_BBD_TEXSIZ_512x1024,
  GFL_BBD_TEXSIZ_1024x8,
  GFL_BBD_TEXSIZ_1024x16,
  GFL_BBD_TEXSIZ_1024x32,
  GFL_BBD_TEXSIZ_1024x64,
  GFL_BBD_TEXSIZ_1024x128,
  GFL_BBD_TEXSIZ_1024x256,
  GFL_BBD_TEXSIZ_1024x512,
  GFL_BBD_TEXSIZ_1024x1024,
};

//======================================================================
//  debug
//======================================================================
#ifdef DEBUG_MMDL_RESOURCE_MEMORY_SIZE
//--------------------------------------------------------------
//  @brief  ���\�[�X�������T�C�Y��Ԃ�
//--------------------------------------------------------------
u32 DEBUG_MMDL_GetUseResourceMemorySize( void )
{
  return DEBUG_MMDL_RESOURCE_MemorySize;
}
#endif

//======================================================================
//  test data
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
//  ���샂�f���Ǘ����Œ�`�����e�N�X�`���T�C�Y�w�肪
//  �r���{�[�h�A�N�^�[�Œ�`����Ă�����̂Ɠ��ꂩ�ǂ���
//  �v���v���Z�b�T���߂Ń`�F�b�N
//======================================================================
//#define MMDL_CHECK_GFL_BBD_TEXSIZDEF ///<��`�Ń`�F�b�N�L��

#ifdef MMDL_CHECK_GFL_BBD_TEXSIZDEF

//-------------------------------------
///	�r���{�[�h�e�N�X�`���T�C�Y define��`��
//-------------------------------------
#define GFL_BBD_TEXSIZDEF_8x8 0
#define GFL_BBD_TEXSIZDEF_8x16 1
#define GFL_BBD_TEXSIZDEF_8x32 2
#define GFL_BBD_TEXSIZDEF_8x64 3
#define GFL_BBD_TEXSIZDEF_8x128 4
#define GFL_BBD_TEXSIZDEF_8x256 5
#define GFL_BBD_TEXSIZDEF_8x512 6
#define GFL_BBD_TEXSIZDEF_8x1024 7
#define GFL_BBD_TEXSIZDEF_16x8 8
#define GFL_BBD_TEXSIZDEF_16x16 9
#define GFL_BBD_TEXSIZDEF_16x32 10
#define GFL_BBD_TEXSIZDEF_16x64 11
#define GFL_BBD_TEXSIZDEF_16x128 12
#define GFL_BBD_TEXSIZDEF_16x256 13
#define GFL_BBD_TEXSIZDEF_16x512 14
#define GFL_BBD_TEXSIZDEF_16x1024 15
#define GFL_BBD_TEXSIZDEF_32x8 16
#define GFL_BBD_TEXSIZDEF_32x16 17
#define GFL_BBD_TEXSIZDEF_32x32 18
#define GFL_BBD_TEXSIZDEF_32x64 19
#define GFL_BBD_TEXSIZDEF_32x128 20
#define GFL_BBD_TEXSIZDEF_32x256 21
#define GFL_BBD_TEXSIZDEF_32x512 22
#define GFL_BBD_TEXSIZDEF_32x1024 23
#define GFL_BBD_TEXSIZDEF_64x8 24 
#define GFL_BBD_TEXSIZDEF_64x16 25
#define GFL_BBD_TEXSIZDEF_64x32 26
#define GFL_BBD_TEXSIZDEF_64x64 27
#define GFL_BBD_TEXSIZDEF_64x128 28
#define GFL_BBD_TEXSIZDEF_64x256 29
#define GFL_BBD_TEXSIZDEF_64x512 30
#define GFL_BBD_TEXSIZDEF_64x1024 31
#define GFL_BBD_TEXSIZDEF_128x8 32
#define GFL_BBD_TEXSIZDEF_128x16 33
#define GFL_BBD_TEXSIZDEF_128x32 34
#define GFL_BBD_TEXSIZDEF_128x64 35
#define GFL_BBD_TEXSIZDEF_128x128 36
#define GFL_BBD_TEXSIZDEF_128x256 37
#define GFL_BBD_TEXSIZDEF_128x512 38
#define GFL_BBD_TEXSIZDEF_128x1024 39
#define GFL_BBD_TEXSIZDEF_256x8 40
#define GFL_BBD_TEXSIZDEF_256x16 41
#define GFL_BBD_TEXSIZDEF_256x32 42
#define GFL_BBD_TEXSIZDEF_256x64 43
#define GFL_BBD_TEXSIZDEF_256x128 44
#define GFL_BBD_TEXSIZDEF_256x256 45
#define GFL_BBD_TEXSIZDEF_256x512 46
#define GFL_BBD_TEXSIZDEF_256x1024 47
#define GFL_BBD_TEXSIZDEF_512x8 48
#define GFL_BBD_TEXSIZDEF_512x16 49
#define GFL_BBD_TEXSIZDEF_512x32 50
#define GFL_BBD_TEXSIZDEF_512x64 51
#define GFL_BBD_TEXSIZDEF_512x128 52
#define GFL_BBD_TEXSIZDEF_512x256 53
#define GFL_BBD_TEXSIZDEF_512x512 54
#define GFL_BBD_TEXSIZDEF_512x1024 55
#define GFL_BBD_TEXSIZDEF_1024x8 56
#define GFL_BBD_TEXSIZDEF_1024x16 57
#define GFL_BBD_TEXSIZDEF_1024x32 58
#define GFL_BBD_TEXSIZDEF_1024x64 59
#define GFL_BBD_TEXSIZDEF_1024x128 60
#define GFL_BBD_TEXSIZDEF_1024x256 61
#define GFL_BBD_TEXSIZDEF_1024x512 62
#define GFL_BBD_TEXSIZDEF_1024x1024 63

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
