//======================================================================
/**
 * @file  fldmmdl.c
 * @brief  ���샂�f��
 * @author  kagaya
 * @date  05.07.13
 */
//======================================================================
#define MMDL_SYSTEM_FILE
#include "fldmmdl.h"
#include "fldmmdl_procdraw.h"

#include "arc_def.h"
#include "arc/fieldmap/fldmmdl_mdlparam.naix"

#include "fieldmap.h"
#include "field/eventwork.h"

//OBJCHRWORK0
#include "../../../resource/fldmapdata/flagwork/work_define.h"

#include "fldmmdl_work.h"

#include "system/main.h"

#ifdef PM_DEBUG
#ifdef _NITRO
//@note�@OBJCODE_PARAM ���̍\���̂̃T�C�Y��ύX����ƃR���o�[�^�[�̏C���܂ŕύX���y�т܂��B
//@note  �ύX����ۂ́A�֌W�҂ɒʒB�����肢���܂��B�@20100517 satio
SDK_COMPILER_ASSERT(sizeof(OBJCODE_PARAM) == 28);
#endif
#endif		//PM_DEBUG

#define DAT_HEADER_SIZE  (2)
#define DAT_FOOTER_SIZE (2)
#define REGULAR_MAX  (26)

#define SYS_MDL_PRM_WORK_SIZE ( (sizeof(u16) * REGULAR_MAX ) + DAT_HEADER_SIZE + DAT_FOOTER_SIZE + sizeof(OBJCODE_PARAM) * REGULAR_MAX )

//======================================================================
//  define
//======================================================================
//--------------------------------------------------------------
//  debug
//--------------------------------------------------------------

//--------------------------------------------------------------
/// ����XZ���W�ɔz�u�\�ȓ���I�u�W�F���C���[��
//--------------------------------------------------------------
#define MMDL_POST_LAYER_MAX (16)  //FLDMAPPER_GRIDINFO_MAX�Ɠ����l�ɂ��Ă܂�

//--------------------------------------------------------------
/// �����肫�₪�Q�ƁA�ۑ�������W�f�[�^��
//--------------------------------------------------------------
#define MMDL_ROCKPOS_MAX (30)
#define ROCKPOS_INIT (0xffffffff) ///<MMDL_ROCKPOS ���W�������l

//--------------------------------------------------------------
///  �G�C���A�X�V���{��
//--------------------------------------------------------------
enum
{
  RET_ALIES_NOT = 0,  //�G�C���A�X�ł͂Ȃ�
  RET_ALIES_EXIST,  //�G�C���A�X�Ƃ��Ċ��ɑ��݂��Ă���
  RET_ALIES_CHANGE,  //�G�C���A�X�ύX���K�v�ł���
};

//--------------------------------------------------------------
/// ���[�N�Q��OBJ�R�[�h
//--------------------------------------------------------------
#define WKOBJCODE_START (WKOBJCODE00)
#define WKOBJCODE_END (WKOBJCODE15)

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
///  MMDL�|�C���^�z��
//--------------------------------------------------------------
typedef struct{
  int count;  //�i�[���ꂽ�L���ȃ|�C���^��
  MMDL* mmdl_parray[MMDL_POST_LAYER_MAX];
}MMDL_PARRAY;

//--------------------------------------------------------------
/// MMDL_ROCKPOS
//--------------------------------------------------------------
struct _TAG_MMDL_ROCKPOS
{
  VecFx32 pos;
};

//======================================================================
//  proto
//======================================================================
//MMDLSYS �v���Z�X
static void mmdlsys_DeleteProc( MMDLSYS *mmdlsys );

//MMDL �ǉ��A�폜
static MMDL * mmdlsys_AddMMdlCore( MMDLSYS *mmdlsys,
    const MMDL_HEADER *header, int zone_id, const EVENTWORK *ev );
static void mmdl_SetHeaderMoveParam(
    MMDL * mmdl, const MMDL_HEADER *head );
static void mmdl_CallSetHeaderPos(
    MMDL * mmdl, const MMDL_HEADER *head );
static void mmdl_SetHeaderBefore( MMDL * mmdl, const MMDL_HEADER *head,
    const EVENTWORK *ev, const MMDLSYS *mmdlsys );
static void mmdl_SetHeaderAfter(
  MMDL * mmdl, const MMDL_HEADER *head, void *sys );
static void mmdl_SetHeaderPos(MMDL *mmdl,const MMDL_HEADER *head);
static void mmdl_InitWork( MMDL * mmdl, MMDLSYS *sys, int zone_id );
static void mmdl_InitDir( MMDL *mmdl );
static void mmdl_InitCallMoveProcWork( MMDL * mmdl );
static void mmdl_InitMoveWork( const MMDLSYS *mmdlsys, MMDL * mmdl );
static void mmdl_InitMoveProc( const MMDLSYS *mmdlsys, MMDL * mmdl );
static void mmdl_UpdateMove( MMDL * mmdl );
#if 0
static void mmdlsys_CheckSetInitMoveWork( MMDLSYS *mmdlsys );
static void mmdlsys_CheckSetInitDrawWork( MMDLSYS *mmdlsys );
#endif

//MMDL ����֐�
static void mmdl_TCB_MoveProc( GFL_TCB * tcb, void *work );
static void mmdl_TCB_DrawProc( MMDL * mmdl );

//MMDLSYS �ݒ�A�Q��
static void mmdlsys_OnStatusBit(
  MMDLSYS *mmdlsys, MMDLSYS_STABIT bit );
static void mmdlsys_OffStatusBit(
  MMDLSYS *mmdlsys, MMDLSYS_STABIT bit );
static void mmdlsys_IncrementOBJCount( MMDLSYS *mmdlsys );
static void mmdlsys_DecrementOBJCount( MMDLSYS *mmdlsys );

//MMDLSYS �c�[��
static MMDL * mmdlsys_SearchSpaceMMdl( const MMDLSYS *sys );
static MMDL * mmdlsys_SearchAlies(
  const MMDLSYS *mmdlsys, int obj_id, int zone_id );

//MMDL �c�[��
static void mmdl_AddTCB( MMDL *mmdl, const MMDLSYS *sys );
static void mmdl_DeleteTCB( MMDL *mmdl );
static void mmdl_InitDrawWork( MMDL *mmdl );
static void mmdl_InitCallDrawProcWork( MMDL * mmdl );
static void mmdl_InitDrawEffectFlag( MMDL * mmdl );
static void mmdl_ClearWork( MMDL *mmdl );
static int mmdl_CheckHeaderAlies(
    const MMDL *mmdl, int h_zone_id, int max,
    const MMDL_HEADER *head );
static MMDL * mmdl_SearchOBJIDZoneID(
    const MMDLSYS *mmdlsys, int obj_id, int zone_id );
static void mmdl_InitDrawStatus( MMDL * mmdl );
static void mmdl_SetDrawDeleteStatus( MMDL * mmdl );
static void mmdl_ChangeAliesOBJ(
  MMDL *mmdl, const MMDL_HEADER *head, int zone_id );
static void mmdl_ChangeOBJAlies(
  MMDL * mmdl, int zone_id, const MMDL_HEADER *head );

//OBJCODE_PARAM
static void mmdlsys_InitOBJCodeParam( MMDLSYS *mmdlsys, HEAPID heapID );
static void mmdlsys_DeleteOBJCodeParam( MMDLSYS *mmdlsys );
static void mmdlsys_LoadSetMMdlOBJCodeParam(
    const MMDLSYS *mmdlsys, u16 code, MMDL *mmdl );

//parts
static u16 WorkOBJCode_GetOBJCode( const EVENTWORK *ev, u16 code );
static const MMDL_MOVE_PROC_LIST * MoveProcList_GetList( u16 code );
static const MMDL_DRAW_PROC_LIST * DrawProcList_GetList( MMDL_DRAWPROCNO no );
static BOOL MMdlHeader_CheckAlies( const MMDL_HEADER *head );
static int MMdlHeader_GetAliesZoneID( const MMDL_HEADER *head );
static BOOL mmdlsys_CheckEventFlag( EVENTWORK *evwork, u16 flag_no );

//MMDL_ROCKPOS
static BOOL mmdl_rockpos_CheckSetPos( const MMDL_ROCKPOS *rockpos );
static int rockpos_GetPosNumber( const u16 zone_id, const u16 obj_id );
static int mmdl_rockpos_GetPosNumber( const MMDL *mmdl );
static BOOL mmdl_rockpos_CheckPos( const MMDL *mmdl );
static BOOL mmdl_rockpos_GetPos( const MMDL *mmdl, VecFx32 *pos );

//======================================================================
//  �t�B�[���h���샂�f���@�V�X�e��
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS �V�X�e���쐬
 * @param  heapID  HEAPID
 * @param  max  ���샂�f���ő吔
 * @param rockpos MMDL_ROCKPOS ���͊₪�Q�ƁA�ۑ�����f�[�^
 * @retval  MMDLSYS* �쐬���ꂽMMDLSYS*
 */
//--------------------------------------------------------------
MMDLSYS * MMDLSYS_CreateSystem(
    HEAPID heapID, u32 max, MMDL_ROCKPOS *rockpos )
{
  MMDLSYS *mmdlsys;
  mmdlsys = GFL_HEAP_AllocClearMemory( heapID, MMDLSYS_SIZE );
  mmdlsys->pMMdlBuf = GFL_HEAP_AllocClearMemory( heapID, max*MMDL_SIZE );
  mmdlsys->mmdl_max = max;
  mmdlsys->sysHeapID = heapID;
  mmdlsys->rockpos = rockpos;
  mmdlsys->tcb_pri = MMDL_TCBPRI_STANDARD;
  mmdlsys_InitOBJCodeParam( mmdlsys, heapID );

  {
    u32 size;
    size = SYS_MDL_PRM_WORK_SIZE;

    mmdlsys->pMdlPrmWork = GFL_HEAP_AllocClearMemory( heapID, size );
  }

  return( mmdlsys );
}

//--------------------------------------------------------------
/**
 * MMDLSYS �V�X�e���J��
 * @param  mmdlsys  MMDLSYS*
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_FreeSystem( MMDLSYS *mmdlsys )
{
  mmdlsys_DeleteOBJCodeParam( mmdlsys );
  GFL_HEAP_FreeMemory( mmdlsys->pMdlPrmWork );
  GFL_HEAP_FreeMemory( mmdlsys->pMMdlBuf );
  GFL_HEAP_FreeMemory( mmdlsys );
}

//--------------------------------------------------------------
/**
 * MMDLSYS �V�X�e�����������郂�f���p�����[�^���N���A
 * @param  mmdlsys  MMDLSYS*
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_ClearSysOBJCodeParam( MMDLSYS *mmdlsys )
{
  u32 size;
  size = SYS_MDL_PRM_WORK_SIZE;
  MI_CpuClear8(mmdlsys->pMdlPrmWork, size);
}

//======================================================================
//  �t�B�[���h���샂�f���@�V�X�e���@�v���Z�X
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS �V�X�e�� ����v���Z�X�Z�b�g�A�b�v
 * @param  mmdlsys  MMDLSYS*
 * @param  heapID  �v���Z�X�pHEAPID
 * @param  pG3DMapper FLDMAPPER
 * @param  pNOGRIDMapper FLDNOGRID_MAPPER
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_SetupProc( MMDLSYS *mmdlsys, HEAPID heapID,
    GAMEDATA *gdata, FIELDMAP_WORK *fieldmap,
    const FLDMAPPER *pG3DMapper, FLDNOGRID_MAPPER* pNOGRIDMapper )
{
  mmdlsys->heapID = heapID;
  mmdlsys->pG3DMapper = pG3DMapper;
  mmdlsys->gdata = gdata;
  mmdlsys->fieldMapWork = fieldmap;
  
  mmdlsys->pTCBSysWork = GFL_HEAP_AllocMemory(
    heapID, GFL_TCB_CalcSystemWorkSize(mmdlsys->mmdl_max) );
  mmdlsys->pTCBSys = GFL_TCB_Init( mmdlsys->mmdl_max, mmdlsys->pTCBSysWork );
  
  mmdlsys->arcH_res = GFL_ARC_OpenDataHandle( ARCID_MMDL_RES, heapID );
  
  // �m�[�O���b�h�ړ��ݒ�
  mmdlsys->pNOGRIDMapper = pNOGRIDMapper;
  mmdlsys_OnStatusBit( mmdlsys, MMDLSYS_STABIT_MOVE_INIT_COMP );
}

//--------------------------------------------------------------
/**
 * MMDLSYS �V�X�e���@����v���Z�X�폜
 * @param  mmdlsys    MMDLSYS *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdlsys_DeleteProc( MMDLSYS *mmdlsys )
{
  GFL_TCB_Exit( mmdlsys->pTCBSys );
  GFL_HEAP_FreeMemory( mmdlsys->pTCBSysWork );
  GFL_ARC_CloseDataHandle( mmdlsys->arcH_res );
  mmdlsys_OffStatusBit( mmdlsys, MMDLSYS_STABIT_MOVE_INIT_COMP );
}

//--------------------------------------------------------------
/**
 * MMDLSYS �S�Ẵv���Z�X�폜
 * @param  mmdlsys    MMDLSYS *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_DeleteProc( MMDLSYS *mmdlsys )
{
//  MMDLSYS_DeleteMMdl( mmdlsys );
  
  { //������fldmmdl.c���ŏ�������B
    MMDLSYS_DeleteDraw( mmdlsys );
    mmdlsys->targetCameraAngleYaw = NULL;
  }
  
  mmdlsys_DeleteProc( mmdlsys );
}

//--------------------------------------------------------------
/**
 * MMDLSYS ���샂�f���X�V
 * @param  mmdlsys  MMDLSYS
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_UpdateProc( MMDLSYS *mmdlsys )
{
  GFL_TCBSYS *tcbsys = mmdlsys->pTCBSys;
  GF_ASSERT( tcbsys != NULL );
  GFL_TCB_Main( tcbsys );
  MMDL_BLACTCONT_Update( mmdlsys );
}

//--------------------------------------------------------------
/**
 * MMDLSYS V�u�����N����
 * @param  mmdlsys  MMDLSYS
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_VBlankProc( MMDLSYS *mmdlsys )
{
  if( mmdlsys->pBlActCont != NULL ){
    MMDL_BLACTCONT_ProcVBlank( mmdlsys );
  }
}

//--------------------------------------------------------------
/**
 * MMDLSYS ����VBlank����
 * @param  mmdlsys  MMDLSYS
 * @retval  nothing
 * @attention ������V�u�����N�҂����s���܂��B
 * MMDL_BLACTCONT_IsThereReserve()�Œǉ��v�f�������Ȃ�܂Ŗ߂�܂���B
 * ���̊ԁA���C���������~�܂�܂��̂ŌĂԍۂ͒��ӂ��ĉ������B
 */
//--------------------------------------------------------------
void MMDLSYS_ForceWaitVBlankProc( MMDLSYS *mmdlsys )
{
  while( MMDL_BLACTCONT_IsThereReserve(mmdlsys) == TRUE ){
    MMDLSYS_UpdateProc( mmdlsys );
    OS_WaitInterrupt( TRUE, OS_IE_V_BLANK );
    MMDLSYS_VBlankProc( mmdlsys );
    D_MMDL_DPrintf( "MMDLSYS_ForceVBlankProc()\n" );
  }
}

//======================================================================
//  �t�B�[���h���샂�f���@�V�X�e���@�`��v���Z�X
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS �V�X�e���@�`��v���Z�X�Z�b�g�A�b�v
 * @param  mmdlsys  MMDLSYS*
 * @param  heapID  �v���Z�X�pHEAPID
 * @param  pG3DMapper FLDMAPPER
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_SetupDrawProc( MMDLSYS *mmdlsys, const u16 *angleYaw )
{
  mmdlsys->targetCameraAngleYaw = angleYaw;
  MMDLSYS_SetCompleteDrawInit( mmdlsys, TRUE );
}

//======================================================================
//  �t�B�[���h���샂�f���ǉ��A�폜
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS �t�B�[���h���샂�f����ǉ��@�R�A����
 * @param  mmdlsys      MMDLSYS *
 * @param  header    �ǉ��������Z�߂�MMDL_HEADER *
 * @param  zone_id    �]�[��ID
 * @param ev EVENTWORK*
 * @retval  MMDL  �ǉ����ꂽMMDL *
 * @attention ev==NULL��Ԃ̍ۂ�
 * WKOBJCODE00���̃��[�N�Q�ƌ^OBJ�R�[�h������ƃG���[�ƂȂ�B
 */
//--------------------------------------------------------------
static MMDL * mmdlsys_AddMMdlCore( MMDLSYS *mmdlsys,
    const MMDL_HEADER *header, int zone_id, const EVENTWORK *ev )
{
  MMDL *mmdl;
  MMDL_HEADER header_data = *header;
  const MMDL_HEADER *head = &header_data;
  mmdl = mmdlsys_SearchSpaceMMdl( mmdlsys );
  GF_ASSERT( mmdl != NULL );
  mmdl_InitWork( mmdl, mmdlsys, zone_id );
  mmdl_SetHeaderBefore( mmdl, head, ev, mmdlsys );
  mmdl_InitDir( mmdl );
  
  if( mmdl_rockpos_CheckPos(mmdl) == TRUE ){
    MMDL_OnStatusBit( mmdl,
        MMDL_STABIT_FELLOW_HIT_NON |
        MMDL_STABIT_HEIGHT_GET_OFF );
  }
  
  if( MMDLSYS_CheckStatusBit(mmdlsys,MMDLSYS_STABIT_MOVE_INIT_COMP) ){
    mmdl_InitMoveWork( mmdlsys, mmdl );
    mmdl_InitMoveProc( mmdlsys, mmdl );
  }
  
  if( MMDLSYS_CheckStatusBit(mmdlsys,MMDLSYS_STABIT_DRAW_INIT_COMP) ){
    mmdl_InitDrawWork( mmdl );
  }
  
  mmdlsys_IncrementOBJCount( (MMDLSYS*)MMDL_GetMMdlSys(mmdl) );
  
  mmdl_SetHeaderAfter( mmdl, head, NULL );
  return( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDLSYS �t�B�[���h���샂�f����ǉ�
 * @param  mmdlsys      MMDLSYS *
 * @param  header    �ǉ��������Z�߂�MMDL_HEADER *
 * @param  zone_id    �]�[��ID
 * @retval  MMDL  �ǉ����ꂽMMDL *
 */
//--------------------------------------------------------------
MMDL * MMDLSYS_AddMMdl(
  MMDLSYS *mmdlsys, const MMDL_HEADER *header, int zone_id )
{
  MMDL *mmdl;
  mmdl = mmdlsys_AddMMdlCore( mmdlsys, header, zone_id, NULL );
  return( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDLSYS ���샂�f����ǉ��B�K�v�Œ���ȏ�񂩂�w�b�_�[���쐬���Ēǉ��B
 * ����Ȃ�����0�ŃN���A����Ă���B�K�v����Βǉ���Ɋe���Őݒ肷��B
 * @param mmdlsys MMDLSYS
 * @param x ����X���W �O���b�h�P��
 * @param z ����Z���W �O���b�h�P��
 * @param dir ���������BDIR_UP��
 * @param id OBJ ID
 * @param code OBJ�R�[�h�BHERO��
 * @param move ����R�[�h�BMV_RND��
 * @param zone_id ZONE_ID
 * @retval MMDL �ǉ����ꂽMMDL*
 */
//--------------------------------------------------------------
MMDL * MMDLSYS_AddMMdlParam( MMDLSYS *mmdlsys,
    s16 gx, s16 gz, u16 dir,
    u16 id, u16 code, u16 move, int zone_id )
{
  MMDL *mmdl;
  MMDL_HEADER head;
  MMDL_HEADER_GRIDPOS *gridpos;
  MI_CpuClear8( &head, sizeof(MMDL_HEADER) );
  
  head.id = id;
  head.obj_code = code;
  head.move_code = move;
  head.dir = dir;
  head.pos_type = MMDL_HEADER_POSTYPE_GRID;
  gridpos = (MMDL_HEADER_GRIDPOS*)head.pos_buf;
  gridpos->gx = gx;
  gridpos->gz = gz;
  
  mmdl = MMDLSYS_AddMMdl( mmdlsys, &head, zone_id );
  return( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDLSYS �t�B�[���h���샂�f����ǉ��@����
 * @param  mmdlsys      MMDLSYS *
 * @param  header    �ǉ��������Z�߂�MMDL_HEADER *
 * @param  zone_id    �]�[��ID
 * @param  count    header�v�f��
 * @retval  nothing
 * @note �C�x���g�t���O�������Ă���w�b�_�[�͒ǉ����Ȃ��B
 */
//--------------------------------------------------------------
void MMDLSYS_SetMMdl( MMDLSYS *mmdlsys,
  const MMDL_HEADER *header, int zone_id, int count, EVENTWORK *eventWork )
{
  GF_ASSERT( count > 0 );
  GF_ASSERT( header != NULL );
  
  D_MMDL_DPrintf( "MMDLSYS_SetMMdl Count %d\n", count );
  do{
    if( MMdlHeader_CheckAlies(header) == TRUE ||
        mmdlsys_CheckEventFlag(eventWork,header->event_flag) == FALSE ){
      mmdlsys_AddMMdlCore( mmdlsys, header, zone_id, eventWork );
    }else{
      D_MMDL_DPrintf( "ADD STOP MMDL OBJID=%d,EVENT FLAG=%xH\n",
          header->id, header->event_flag );
    }
    
    header++;
    count--;
  }while( count );
}

//--------------------------------------------------------------
/**
 * MMDLSYS �t�B�[���h���샂�f����ǉ��@�w�b�_�[���̎w��OBJID
 * @param  mmdlsys      MMDLSYS *
 * @param  header    �ǉ��������Z�߂�MMDL_HEADER *
 * @param  zone_id    �]�[��ID
 * @param  count    header�v�f��
 * @param id �ǉ�����OBJ ID
 * @retval  nothing
 * @note �C�x���g�t���O�������Ă���w�b�_�[�͒ǉ����Ȃ��B
 */
//--------------------------------------------------------------
MMDL * MMDLSYS_AddMMdlHeaderID( MMDLSYS *mmdlsys,
  const MMDL_HEADER *header, int zone_id, int count, EVENTWORK *eventWork,
  u16 objID )
{
  MMDL *mmdl;

  GF_ASSERT( count > 0 );
  GF_ASSERT( header != NULL );
  
  D_MMDL_DPrintf( "MMDLSYS_SetMMdlHeaderID Count %d\n", count );
  
  do{
    if( MMdlHeader_CheckAlies(header) == FALSE )
    {
      if( header->id == objID )
      {
        if( mmdlsys_CheckEventFlag(
              eventWork,header->event_flag) == FALSE )
        {
          mmdl = mmdlsys_AddMMdlCore( mmdlsys, header, zone_id, eventWork );
          break;
        }
      }
    }
    
    header++;
    count--;
  }while( count );
  
  return( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f�����폜
 * @param  mmdl    �폜����MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_Delete( MMDL * mmdl )
{
  const MMDLSYS *mmdlsys;
  
  mmdlsys = MMDL_GetMMdlSys( mmdl );
  
  if( MMDLSYS_CheckCompleteDrawInit(mmdlsys) == TRUE ){
    MMDL_CallDrawDeleteProc( mmdl );
  }
  
  if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_MOVEPROC_INIT) ){
    MMDL_CallMoveDeleteProc( mmdl );
    mmdl_DeleteTCB( mmdl );
  }
  
  mmdlsys_DecrementOBJCount( (MMDLSYS*)(mmdl->pMMdlSys) );
  mmdl_ClearWork( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f�����폜�@�C�x���g�t���O OBJ��\���t���O��ON�ɁB
 * @param  mmdl    �폜����MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_DeleteEvent( MMDL * mmdl, EVENTWORK *evwork )
{
  EVENTWORK_SetEventFlag( evwork, MMDL_GetEventFlag(mmdl) );
  MMDL_Delete( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDLSYS ���ݔ������Ă���t�B�[���h���샂�f����S�č폜
 * @param  mmdlsys  MMDLSYS
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_DeleteMMdl( const MMDLSYS *mmdlsys )
{
  u32 no = 0;
  MMDL *mmdl;
  
  while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) ){
    if( MMDLSYS_CheckCompleteDrawInit(mmdlsys) == TRUE ){
      MMDL_CallDrawDeleteProc( mmdl );
    }
    
    if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_MOVEPROC_INIT) ){
      //MMDL_CallMoveDeleteProc( mmdl );
      //mmdl_DeleteTCB( mmdl );
    }
    
    mmdlsys_DecrementOBJCount( (MMDLSYS*)(mmdl->pMMdlSys) );
    mmdl_ClearWork( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���@�w�b�_�[��񔽉f�@����֘A
 * @param  mmdl    �ݒ肷��MMDL * 
 * @param  head    ���f�������Z�߂�MMDL_HEADER *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_SetHeaderMoveParam(
    MMDL * mmdl, const MMDL_HEADER *head )
{
  MMDL_SetOBJID( mmdl, head->id );
  MMDL_SetMoveCode( mmdl, head->move_code );
  MMDL_SetEventType( mmdl, head->event_type );
  MMDL_SetEventFlag( mmdl, head->event_flag );
  MMDL_SetEventID( mmdl, head->event_id );
  mmdl->dir_head = head->dir;
  mmdl->dir_move = head->dir;
  MMDL_SetParam( mmdl, head->param0, MMDL_PARAM_0 );
  MMDL_SetParam( mmdl, head->param1, MMDL_PARAM_1 );
  MMDL_SetParam( mmdl, head->param2, MMDL_PARAM_2 );
  MMDL_SetMoveLimitX( mmdl, head->move_limit_x );
  MMDL_SetMoveLimitZ( mmdl, head->move_limit_z );
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���@�w�b�_�[����
 * @param  mmdl    �ݒ肷��MMDL * 
 * @param  head    ���f�������Z�߂�MMDL_HEADER *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_CallSetHeaderPos(
    MMDL * mmdl, const MMDL_HEADER *head )
{
  // ���W�^�C�v�ɂ��A�ʒu�̏��������@��ύX
  if( head->pos_type == MMDL_HEADER_POSTYPE_GRID )
  {
    mmdl_SetHeaderPos( mmdl, head );
  }
  else
  {
    MMDL_SetRailHeaderBefore( mmdl, head );
  }
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���@�w�b�_�[��񔽉f
 * @param  mmdl    �ݒ肷��MMDL * 
 * @param  head    ���f�������Z�߂�MMDL_HEADER *
 * @param  ev      EVENTWORK*
 * @retval  nothing
 * @attention ev==NULL��Ԃ̍ۂ�
 * WKOBJCODE00���̃��[�N�Q�ƌ^OBJ�R�[�h������ƃG���[�ƂȂ�B
 */
//--------------------------------------------------------------
static void mmdl_SetHeaderBefore(
    MMDL * mmdl, const MMDL_HEADER *head,
    const EVENTWORK *ev, const MMDLSYS *mmdlsys )
{
  u16 obj_code;
  const OBJCODE_PARAM *param;
  
  mmdl_SetHeaderMoveParam( mmdl, head );
  
  obj_code = WorkOBJCode_GetOBJCode( ev, head->obj_code );
  MMDL_SetOBJCode( mmdl, obj_code );
  mmdlsys_LoadSetMMdlOBJCodeParam( mmdlsys, obj_code, mmdl );
  
  param = &mmdl->objcode_param;
  
  if( obj_code == STOPPER ){ //�T�C�Y�ݒ�BSTOPPER�̓T�C�Y�w��A��
    mmdl->gx_size = head->param0;
    mmdl->gz_size = head->param1;
    
    if( mmdl->gx_size == 0 ){
      mmdl->gx_size++;
    }
    
    if( mmdl->gz_size == 0 ){
      mmdl->gz_size++;
    }
  }else{
    mmdl->gx_size = param->size_width;
    mmdl->gz_size = param->size_depth;
  }
  
  mmdl->offset_x = param->offs_x; //�I�t�Z�b�g
  mmdl->offset_y = param->offs_y;
  mmdl->offset_z = param->offs_z;
  
  mmdl_CallSetHeaderPos( mmdl, head );
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���@�w�b�_�[��񔽉f  ���샂�f���̓o�^�����㏈��
 * @param  mmdl    �ݒ肷��MMDL * 
 * @param  head    ���f�������Z�߂�MMDL_HEADER *
 * @param  fsys    FIELDSYS_WORK *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_SetHeaderAfter(
  MMDL * mmdl, const MMDL_HEADER *head, void *sys )
{
  // ���W�^�C�v�ɂ��A�ʒu�̏��������@��ύX
  if( head->pos_type == MMDL_HEADER_POSTYPE_RAIL )
  {
    MMDL_SetRailHeaderAfter( mmdl, head );
  }
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���@���W�n������
 * @param  mmdl    MMDL * 
 * @param  head    ���f�������Z�߂�MMDL_HEADER *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_SetHeaderPos( MMDL *mmdl, const MMDL_HEADER *head )
{
  fx32 set_y;
  VecFx32 vec;
  int pos,set_gx,set_gz;
  const MMDL_HEADER_GRIDPOS *gridpos;
  
  GF_ASSERT( head->pos_type == MMDL_HEADER_POSTYPE_GRID );
  
  gridpos = (const MMDL_HEADER_GRIDPOS*)head->pos_buf;
  
  if( mmdl_rockpos_CheckPos(mmdl) == FALSE ){
    set_gx = gridpos->gx;
    set_gz = gridpos->gz;
    set_y = gridpos->y;
  }else{
    mmdl_rockpos_GetPos( mmdl, &vec );
    set_gx = SIZE_GRID_FX32( vec.x );
    set_gz = SIZE_GRID_FX32( vec.z );
    set_y = vec.y;
  }
  
  vec.x = GRID_SIZE_FX32( set_gx ) + MMDL_VEC_X_GRID_OFFS_FX32;
  MMDL_SetInitGridPosX( mmdl, gridpos->gx );
  MMDL_SetOldGridPosX( mmdl, set_gx );
  MMDL_SetGridPosX( mmdl, set_gx );
  
  pos = SIZE_GRID_FX32( gridpos->y );    //pos�ݒ��fx32�^�ŗ���B
  MMDL_SetInitGridPosY( mmdl, pos );
  
  vec.y = set_y;
  pos = SIZE_GRID_FX32( set_y );
  MMDL_SetOldGridPosY( mmdl, pos );
  MMDL_SetGridPosY( mmdl, pos );
  
  vec.z = GRID_SIZE_FX32( set_gz ) + MMDL_VEC_Z_GRID_OFFS_FX32;
  MMDL_SetInitGridPosZ( mmdl, gridpos->gz );
  MMDL_SetOldGridPosZ( mmdl, set_gz );
  MMDL_SetGridPosZ( mmdl, set_gz );
  
  MMDL_SetVectorPos( mmdl, &vec );
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���@���[�N������
 * @param  mmdl    MMDL * 
 * @param  sys      MMDLSYS *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_InitWork( MMDL * mmdl, MMDLSYS *sys, int zone_id )
{
  mmdl->pMMdlSys = sys;
  MMDL_SetZoneID( mmdl, zone_id );
  
  MMDL_OnStatusBit( mmdl, MMDL_STABIT_USE );
  MMDL_OnMoveBit( mmdl,
    MMDL_MOVEBIT_HEIGHT_GET_ERROR |  //�����擾���K�v
    MMDL_MOVEBIT_ATTR_GET_ERROR );  //�A�g���r���[�g�擾���K�v
  
  if( MMDL_CheckAliesEventID(mmdl) == TRUE ){
    MMDL_SetStatusBitAlies( mmdl, TRUE );
  }
  
  MMDL_FreeAcmd( mmdl );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���@����������
 * @param mmdl 
 * @retval
 */
//--------------------------------------------------------------
static void mmdl_InitDir( MMDL *mmdl )
{
  MMDL_SetForceDirDisp( mmdl, MMDL_GetDirHeader(mmdl) );
  MMDL_SetDirMove( mmdl, MMDL_GetDirHeader(mmdl) );
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f�� ����֐�������
 * @param  mmdl    MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_InitCallMoveProcWork( MMDL * mmdl )
{
  mmdl->move_proc_list =
    MoveProcList_GetList( MMDL_GetMoveCode(mmdl) );
}

//--------------------------------------------------------------
/**
 * MMDL ���쏉�����ɍs�������Z��
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_InitMoveWork( const MMDLSYS *mmdlsys, MMDL *mmdl )
{
  mmdl_InitCallMoveProcWork( mmdl );
  mmdl_AddTCB( mmdl, mmdlsys );
  MMDL_OnMoveBit( mmdl,
      MMDL_MOVEBIT_MOVE_START |
      MMDL_MOVEBIT_ATTR_GET_ERROR |
      MMDL_MOVEBIT_HEIGHT_GET_ERROR );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f�� ���쏉����
 *  @param  mmdlsys
 *  @param  mmdl 
 */
//--------------------------------------------------------------
static void mmdl_InitMoveProc( const MMDLSYS *mmdlsys, MMDL * mmdl )
{
  if( !MMDL_CheckStatusBit(mmdl,MMDL_STABIT_RAIL_MOVE) )
  {
    MMDL_InitMoveProc( mmdl );
  }
  else
  {
    MMDL_InitRailMoveProc( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f������
 *
 *  @param  mmdl 
 */
//--------------------------------------------------------------
static void mmdl_UpdateMove( MMDL * mmdl )
{
  GF_ASSERT( mmdl );
  if( !MMDL_CheckStatusBit( mmdl, MMDL_STABIT_RAIL_MOVE ) )
  {
    MMDL_UpdateMove( mmdl );
  }
  else
  {
    MMDL_UpdateRailMove( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * MMDLSYS ���쏉�������s���Ă��Ȃ����샂�f���ɑ΂��ď������������Z�b�g
 * @param  mmdlsys  MMDLSYS*
 * @retval  nothing
 */
//--------------------------------------------------------------
#if 0
static void mmdlsys_CheckSetInitMoveWork( MMDLSYS *mmdlsys )
{
  u32 i = 0;
  MMDL *mmdl;
  
  while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&i) == TRUE ){
    if( MMDL_CheckStatusBit(mmdl,  //�������֐��Ăяo���܂�
      MMDL_STABIT_MOVEPROC_INIT) == 0 ){
      mmdl_InitMoveProc( mmdl );
    }
  }
}
#endif

//--------------------------------------------------------------
/**
 * MMDLSYS �`�揉�������s���Ă��Ȃ����샂�f���ɑ΂��ď������������Z�b�g
 * @param  mmdlsys  MMDLSYS*
 * @retval  nothing
 */
//--------------------------------------------------------------
#if 0
static void mmdlsys_CheckSetInitDrawWork( MMDLSYS *mmdlsys )
{
  u32 i = 0;
  MMDL *mmdl;
  
  while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&i) == TRUE ){
    if( MMDL_CheckMoveBitCompletedDrawInit(mmdl) == FALSE ){
      mmdl_InitDrawWork( mmdl );
    }
  }
}
#endif

//======================================================================
//  MMDLSYS PUSH POP
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS ���샂�f�� �ޔ�
 * @param  mmdlsys  MMDLSYS
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_Push( MMDLSYS *mmdlsys )
{
  u32 no = 0;
  MMDL *mmdl;
  
  if( MMDLSYS_CheckCompleteDrawInit(mmdlsys) == FALSE ){
    D_MMDL_DPrintf( "WARNING!! ���샂�f�� �`�斢������\n" );
    GF_ASSERT( 0 );
    return;
  }
  
  while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) == TRUE ){
    {
      mmdl_DeleteTCB( mmdl );
      MMDL_CallDrawPushProc( mmdl );
      
      MMDL_OnMoveBit( mmdl,
        MMDL_MOVEBIT_DRAW_PUSH | //�`�揈����ޔ�����
        MMDL_MOVEBIT_NEED_MOVEPROC_RECOVER ); //���A�������K�v
    }
  }
}

//--------------------------------------------------------------
/**
 * MMDLSYS ���샂�f�����A
 * @param  mmdlsys  MMDLSYS
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_Pop( MMDLSYS *mmdlsys )
{
  u32 no = 0;
  MMDL *mmdl;
  
  while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) == TRUE ){
    {  //���쏈�����A
      mmdl_InitMoveWork( mmdlsys, mmdl ); //���[�N������
      
      //�������֐��Ăяo���܂�
      if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_MOVEPROC_INIT) == 0 ){
        mmdl_InitMoveProc( mmdlsys, mmdl );
      }
      
      //�����֐��Ăяo�����K�v
      if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_NEED_MOVEPROC_RECOVER) ){
        MMDL_CallMovePopProc( mmdl );
        MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_NEED_MOVEPROC_RECOVER );
      }
    }
    
    {  //�`�揈�����A
      if( MMDL_CheckMoveBitCompletedDrawInit(mmdl) == FALSE ){
        mmdl_InitDrawWork( mmdl );
      }
      
      if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_DRAW_PUSH) ){
        MMDL_CallDrawPopProc( mmdl );
        MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_DRAW_PUSH );
      }
    }

    { //���J�o���[
      mmdl_InitDrawEffectFlag( mmdl );
    }
  }
}

//======================================================================
//  MMDL ����֐�
//======================================================================
//--------------------------------------------------------------
/**
 * MMDL ���암�����s
 * @param  mmdl  MMDL*
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_UpdateMoveProc( MMDL *mmdl )
{
  mmdl_UpdateMove( mmdl );
  
  if( MMDL_CheckStatusBitUse(mmdl) == TRUE ){
    mmdl_TCB_DrawProc( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * MMDL TCB ����֐�
 * @param  tcb    GFL_TCB *
 * @param  work  tcb work
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_TCB_MoveProc( GFL_TCB * tcb, void *work )
{
  MMDL *mmdl = (MMDL *)work;
  MMDL_UpdateMoveProc( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDL TCB ����֐�����Ă΂��`��֐�
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_TCB_DrawProc( MMDL * mmdl )
{
  const MMDLSYS *mmdlsys = MMDL_GetMMdlSys(mmdl);
  
  if( MMDLSYS_CheckCompleteDrawInit(mmdlsys) == TRUE ){
    MMDL_UpdateDraw( mmdl );
  }
}

//======================================================================
//  MMDL �A�j���[�V�����R�}���h
//======================================================================
//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h���\���`�F�b�N
 * @param  mmdl    �ΏۂƂȂ�MMDL * 
 * @retval  int      TRUE=�\�BFALSE=����
 */
//--------------------------------------------------------------
BOOL MMDL_CheckPossibleAcmd( const MMDL * mmdl )
{
  if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_USE) == 0 ){
    return( FALSE );
  }
  
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_MOVE) ){
    return( FALSE );
  }
  
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_ACMD) ){ //�R�}���h��
    if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_ACMD_END) == 0 ){
      return( FALSE ); //�R�}���h�I�����Ă��Ȃ�
    }
  }
  
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h�Z�b�g
 * @param  mmdl    �ΏۂƂȂ�MMDL * 
 * @param  code    ���s����R�[�h�BAC_DIR_U��
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetAcmd( MMDL * mmdl, u16 code )
{
  GF_ASSERT( code < ACMD_MAX && "MMDL ACMD CODE ERROR" );
  MMDL_SetAcmdCode( mmdl, code );
  MMDL_SetAcmdSeq( mmdl, 0 );
  MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_ACMD );
  MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_ACMD_END );
}

//--------------------------------------------------------------
/**
 * �R�}���h�Z�b�g
 * @param  mmdl    �ΏۂƂȂ�MMDL * 
 * @param  code    ���s����R�[�h�BAC_DIR_U��
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetLocalAcmd( MMDL * mmdl, u16 code )
{
  MMDL_SetAcmdCode( mmdl, code );
  MMDL_SetAcmdSeq( mmdl, 0 );
  MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_ACMD_END );
}

//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h�I���`�F�b�N�B
 * @param  mmdl    �ΏۂƂȂ�MMDL * 
 * @retval  int      TRUE=�I��
 */
//--------------------------------------------------------------
BOOL MMDL_CheckEndAcmd( const MMDL * mmdl )
{
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_ACMD) == 0 ){
    return( TRUE );
  }
  
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_ACMD_END) == 0 ){
    return( FALSE );
  }
  
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h�I���`�F�b�N�ƊJ���B
 * �A�j���[�V�����R�}���h���I�����Ă��Ȃ��ꍇ�͊J������Ȃ��B
 * @param  mmdl    �ΏۂƂȂ�MMDL * 
 * @retval  BOOL  TRUE=�I�����Ă���B
 */
//--------------------------------------------------------------
BOOL MMDL_EndAcmd( MMDL * mmdl )
{
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_ACMD) == 0 ){
    return( TRUE );
  }
  
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_ACMD_END) == 0 ){
    return( FALSE );
  }
  
  MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_ACMD|MMDL_MOVEBIT_ACMD_END );
  
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h�J���B
 * �A�j���[�V�����R�}���h���I�����Ă��Ȃ��Ƃ������J���B
 * @param  mmdl    �ΏۂƂȂ�MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_FreeAcmd( MMDL * mmdl )
{
  MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_ACMD|MMDL_MOVEBIT_ACMD_END );
  MMDL_SetAcmdCode( mmdl, ACMD_NOT );
  MMDL_SetAcmdSeq( mmdl, 0 );
}

//======================================================================
//  MMDLSYS �p�����^�ݒ�A�Q��
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS �X�e�[�^�X�r�b�g�`�F�b�N
 * @param  mmdlsys  MMDLSYS*
 * @param  bit  MMDLSYS_STABIT
 * @retval  u32  (status bit & bit)
 */
//--------------------------------------------------------------
u32 MMDLSYS_CheckStatusBit(
  const MMDLSYS *mmdlsys, MMDLSYS_STABIT bit )
{
  return( (mmdlsys->status_bit&bit) );
}

//--------------------------------------------------------------
/**
 * MMDLSYS �X�e�[�^�X�r�b�g ON
 * @param  mmdlsys  MMDLSYS*
 * @param  bit  MMDLSYS_STABIT
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdlsys_OnStatusBit(
  MMDLSYS *mmdlsys, MMDLSYS_STABIT bit )
{
  mmdlsys->status_bit |= bit;
}

//--------------------------------------------------------------
/**
 * MMDLSYS �X�e�[�^�X�r�b�g OFF
 * @param  mmdlsys  MMDLSYS*
 * @param  bit  MMDLSYS_STABIT
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdlsys_OffStatusBit(
  MMDLSYS *mmdlsys, MMDLSYS_STABIT bit )
{
  mmdlsys->status_bit &= ~bit;
}

//--------------------------------------------------------------
/**
 * mmdlSYS ���샂�f���ő吔���擾
 * @param  mmdlsys  MMDLSYS*
 * @retval  u16  �ő吔
 */
//--------------------------------------------------------------
u16 MMDLSYS_GetMMdlMax( const MMDLSYS *mmdlsys )
{
  return( mmdlsys->mmdl_max );
}

//--------------------------------------------------------------
/**
 * mmdlSYS ���ݔ������Ă��铮�샂�f���̐����擾
 * @param  mmdlsys  MMDLSYS*
 * @retval  u16  ������
 */
//--------------------------------------------------------------
u16 MMDLSYS_GetMMdlCount( const MMDLSYS *mmdlsys )
{
  return( mmdlsys->mmdl_count );
}

//--------------------------------------------------------------
/**
 * MMDLSYS TCB�v���C�I���e�B���擾
 * @param  mmdlsys  MMDLSYS*
 * @retval  u16  TCB�v���C�I���e�B
 */
//--------------------------------------------------------------
u16 MMDLSYS_GetTCBPriority( const MMDLSYS *mmdlsys )
{
  return( mmdlsys->tcb_pri );
}

//--------------------------------------------------------------
/**
 * MMDLSYS HEAPID�擾
 * @param  mmdlsys  MMDLSYS*
 * @retval  HEAPID HEAPID
 */
//--------------------------------------------------------------
HEAPID MMDLSYS_GetHeapID( const MMDLSYS *mmdlsys )
{
  return( mmdlsys->heapID );
}

//--------------------------------------------------------------
/**
 * MMDLSYS ���ݔ������Ă���OBJ����1����
 * @param  mmdlsys  MMDLSYS*
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdlsys_IncrementOBJCount( MMDLSYS *mmdlsys )
{
  mmdlsys->mmdl_count++;
}

//--------------------------------------------------------------
/**
 * MMDLSYS ���ݔ������Ă���OBJ����1����
 * @param  mmdlsys  MMDLSYS*
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdlsys_DecrementOBJCount( MMDLSYS *mmdlsys )
{
  mmdlsys->mmdl_count--;
  GF_ASSERT( mmdlsys->mmdl_count >= 0 );
}

//--------------------------------------------------------------
/**
 * MMDLSYS GFL_TCBSYS*�擾
 * @param  mmdlsys  MMDLSYS*
 * @retval  GFL_TCBSYS*
 */
//--------------------------------------------------------------
GFL_TCBSYS * MMDLSYS_GetTCBSYS( MMDLSYS *mmdlsys )
{
  return( mmdlsys->pTCBSys );
}

//--------------------------------------------------------------
/**
 * MMDLSYS ���\�[�X�A�[�J�C�u�n���h���擾
 * @param mmdlsys MMDLSYS*
 * @retval ARCHANDLE*
 */
//--------------------------------------------------------------
ARCHANDLE * MMDLSYS_GetResArcHandle( MMDLSYS *mmdlsys )
{
  return( mmdlsys->arcH_res );
}

//--------------------------------------------------------------
/**
 * MMDLSYS MMDL_BLACTCONT�Z�b�g
 * @param  mmdlsys  MMDLSYS
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_SetBlActCont(
  MMDLSYS *mmdlsys, MMDL_BLACTCONT *pBlActCont )
{
  mmdlsys->pBlActCont = pBlActCont;
}

//--------------------------------------------------------------
/**
 * MMDLSYS MMDL_BLACTCONT�擾
 * @param  mmdlsys MMDLSYS
 * @retval  MMDL_BLACTCONT*
 */
//--------------------------------------------------------------
MMDL_BLACTCONT * MMDLSYS_GetBlActCont( MMDLSYS *mmdlsys )
{
  GF_ASSERT( mmdlsys->pBlActCont != NULL );
  return( mmdlsys->pBlActCont );
}

//--------------------------------------------------------------
/**
 * MMDLSYS MMDL_G3DOBJCONT�Z�b�g
 * @param  mmdlsys  MMDLSYS
 * @param objcont MMDL_G3DOBJCONT*
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_SetG3dObjCont(
  MMDLSYS *mmdlsys, MMDL_G3DOBJCONT *objcont )
{
  mmdlsys->pObjCont = objcont;
}

//--------------------------------------------------------------
/**
 * MMDLSYS MMDL_G3DOBJCONT�擾
 * @param  mmdlsys  MMDLSYS
 * @param objcont MMDL_G3DOBJCONT*
 * @retval  nothing
 */
//--------------------------------------------------------------
MMDL_G3DOBJCONT * MMDLSYS_GetG3dObjCont( MMDLSYS *mmdlsys )
{
  GF_ASSERT( mmdlsys->pObjCont != NULL );
  return( mmdlsys->pObjCont );
}

//--------------------------------------------------------------
/**
 * MMDLSYS FLDMAPPER�擾
 * @param  mmdlsys  MMDLSYS
 * @retval  FLDMAPPER* FLDMAPPER*
 */
//--------------------------------------------------------------
const FLDMAPPER * MMDLSYS_GetG3DMapper( const MMDLSYS *mmdlsys )
{
  GF_ASSERT( mmdlsys->pG3DMapper != NULL);
  return( mmdlsys->pG3DMapper );
}

//--------------------------------------------------------------
/**
 *  @brief  �m�[�O���b�h����}�b�p�[�̎擾
 *  @param  mmdlsys   ���샂�f���V�X�e��
 *  @return�@�}�b�p�[
 */
//--------------------------------------------------------------
FLDNOGRID_MAPPER * MMDLSYS_GetNOGRIDMapper( const MMDLSYS *mmdlsys )
{
  GF_ASSERT( mmdlsys );
  return mmdlsys->pNOGRIDMapper;
}

//--------------------------------------------------------------
/**
 * MMDLSYS FIELDMAP_WORK�擾
 * @param mmdlsys MMDLSYS
 * @retval fieldMapWork FIELDMAP_WORK
 */
//--------------------------------------------------------------
void * MMDLSYS_GetFieldMapWork( MMDLSYS *mmdlsys )
{
  GF_ASSERT( mmdlsys->fieldMapWork != NULL );
  return( mmdlsys->fieldMapWork );
}

//--------------------------------------------------------------
/**
 * MMDLSYS GAMEDATA�擾
 * @param mmdlsys MMDLSYS
 * @retval GAMEDATA*
 */
//--------------------------------------------------------------
GAMEDATA * MMDLSYS_GetGameData( MMDLSYS *mmdlsys )
{
  GF_ASSERT( mmdlsys->gdata != NULL );
  return( mmdlsys->gdata );
}

//--------------------------------------------------------------
/**
 * MMDLSYS �ڕW�ƂȂ�J�������������p�x���擾�B
 * �Z�b�g����Ă��Ȃ��ꍇ��0��Ԃ�
 * @param mmdlsys MMDLSYS
 * @retval u16
 */
//--------------------------------------------------------------
u16 MMDLSYS_GetTargetCameraAngleYaw( const MMDLSYS *mmdlsys )
{
  if( mmdlsys->targetCameraAngleYaw != NULL ){
    return( *mmdlsys->targetCameraAngleYaw );
  }
  return( 0 );
}

//======================================================================
//  MMDL�@�p�����^�Q�ƁA�ݒ�
//======================================================================
//--------------------------------------------------------------
/**
 * MMDL �X�e�[�^�X�r�b�gON
 * @param  mmdl  mmdl
 * @param  bit    MMDL_STABIT
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_OnStatusBit( MMDL *mmdl, MMDL_STABIT bit )
{
  mmdl->status_bit |= bit;
}

//--------------------------------------------------------------
/**
 * MMDL �X�e�[�^�X�r�b�gOFF
 * @param  mmdl  mmdl
 * @param  bit    MMDL_STABIT
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_OffStatusBit( MMDL *mmdl, MMDL_STABIT bit )
{
  mmdl->status_bit &= ~bit;
}

//--------------------------------------------------------------
/**
 * MMDL �X�e�[�^�X�r�b�g�擾
 * @param  mmdl    MMDL * 
 * @retval  MMDL_STABIT �X�e�[�^�X�r�b�g
 */
//--------------------------------------------------------------
MMDL_STABIT MMDL_GetStatusBit( const MMDL * mmdl )
{
  return( mmdl->status_bit );
}

//--------------------------------------------------------------
/**
 * MMDL �X�e�[�^�X�r�b�g�`�F�b�N
 * @param  mmdl  MMDL*
 * @param  bit  MMDL_STABIT
 * @retval  u32  (status bit & bit)
 */
//--------------------------------------------------------------
u32 MMDL_CheckStatusBit( const MMDL *mmdl, MMDL_STABIT bit )
{
  return( (mmdl->status_bit&bit) );
}

//--------------------------------------------------------------
/**
 * MMDL ����r�b�g�@�擾
 * @param  mmdl  MMDL *
 * @retval  u32    ����r�b�g
 */
//--------------------------------------------------------------
MMDL_MOVEBIT MMDL_GetMoveBit( const MMDL * mmdl )
{
  return( mmdl->move_bit );
}

//--------------------------------------------------------------
/**
 * MMDL ����r�b�g�@ON
 * @param  mmdl  MMDL *
 * @param  bit    ���Ă�r�b�g MMDL_MOVEBIT_NON��
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_OnMoveBit( MMDL * mmdl, MMDL_MOVEBIT bit )
{
  mmdl->move_bit |= bit;
}

//--------------------------------------------------------------
/**
 * MMDL ����r�b�g�@OFF
 * @param  mmdl  MMDL *
 * @param  bit    ���낷�r�b�g MMDL_MOVEBIT_NON��
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_OffMoveBit( MMDL * mmdl, MMDL_MOVEBIT bit )
{
  mmdl->move_bit &= ~bit;
}

//--------------------------------------------------------------
/**
 * MMDL ����r�b�g�@�`�F�b�N
 * @param  mmdl  MMDL *
 * @param  bit    �`�F�b�N����r�b�g MMDL_MOVEBIT_NON��
 * @retval  nothing
 */
//--------------------------------------------------------------
u32 MMDL_CheckMoveBit( const MMDL * mmdl, MMDL_MOVEBIT bit )
{
  return( (mmdl->move_bit & bit) );
}

//--------------------------------------------------------------
/**
 * MMDL OBJ ID�Z�b�g
 * @param  mmdl  MMDL * 
 * @param  id    obj id
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetOBJID( MMDL * mmdl, u16 obj_id )
{
  mmdl->obj_id = obj_id;
}

//--------------------------------------------------------------
/**
 * MMDL OBJ ID�擾
 * @param  mmdl  MMDL *
 * @retval  u16    OBJ ID
 */
//--------------------------------------------------------------
u16 MMDL_GetOBJID( const MMDL * mmdl )
{
  return( mmdl->obj_id );
}

//--------------------------------------------------------------
/**
 * MMDL ZONE ID�Z�b�g
 * @param  mmdl  MMDL *
 * @param  zone_id  �]�[��ID
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetZoneID( MMDL * mmdl, u16 zone_id )
{
  mmdl->zone_id = zone_id;
}

//--------------------------------------------------------------
/**
 * MMDL ZONE ID�擾
 * @param  mmdl  MMDL *
 * @retval  int    ZONE ID
 */
//--------------------------------------------------------------
u16 MMDL_GetZoneID( const MMDL * mmdl )
{
  return( mmdl->zone_id );
}

//--------------------------------------------------------------
/**
 * MMDL OBJ�R�[�h�Z�b�g
 * @param  mmdl      MMDL * 
 * @param  code      �Z�b�g����R�[�h
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetOBJCode( MMDL * mmdl, u16 code )
{
  mmdl->obj_code = code;
}

//--------------------------------------------------------------
/**
 * MMDL OBJ�R�[�h�擾
 * @param  mmdl      MMDL * 
 * @retval  u16        OBJ�R�[�h
 */
//--------------------------------------------------------------
u16 MMDL_GetOBJCode( const MMDL * mmdl )
{
  return( mmdl->obj_code );
}

//--------------------------------------------------------------
/**
 * MMDL ����R�[�h�Z�b�g
 * @param  mmdl      MMDL * 
 * @param  code      ����R�[�h
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetMoveCode( MMDL * mmdl, u16 code )
{
  mmdl->move_code = code;
}

//--------------------------------------------------------------
/**
 * MMDL ����R�[�h�擾
 * @param  mmdl      MMDL * 
 * @retval  u32        ����R�[�h
 */
//--------------------------------------------------------------
u16 MMDL_GetMoveCode( const MMDL * mmdl )
{
  return( mmdl->move_code );
}

//--------------------------------------------------------------
/**
 * MMDL �C�x���g�^�C�v�Z�b�g
 * @param  mmdl    MMDL * 
 * @param  type    Event Type
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetEventType( MMDL * mmdl, u16 type )
{
  mmdl->event_type = type;
}

//--------------------------------------------------------------
/**
 * MMDL �C�x���g�^�C�v�擾
 * @param  mmdl    MMDL * 
 * @retval  u32      Event Type
 */
//--------------------------------------------------------------
u16 MMDL_GetEventType( const MMDL * mmdl )
{
  return( mmdl->event_type );
}

//--------------------------------------------------------------
/**
 * MMDL �C�x���g�t���O�Z�b�g
 * @param  mmdl    MMDL * 
 * @param  flag    Event Flag
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetEventFlag( MMDL * mmdl, u16 flag )
{
  mmdl->event_flag = flag;
}

//--------------------------------------------------------------
/**
 * MMDL �C�x���g�t���O�擾
 * @param  mmdl    MMDL * 
 * @retval  u16      Event Flag
 */
//--------------------------------------------------------------
u16 MMDL_GetEventFlag( const MMDL * mmdl )
{
  return( mmdl->event_flag );
}

//--------------------------------------------------------------
/**
 * MMDL �C�x���gID�Z�b�g
 * @param  mmdl    MMDL *
 * @param  id      Event ID
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetEventID( MMDL * mmdl, u16 id )
{
  mmdl->event_id = id;
}

//--------------------------------------------------------------
/**
 * MMDL �C�x���gID�擾
 * @param  mmdl    MMDL * 
 * @retval  u16      Event ID
 */
//--------------------------------------------------------------
u16 MMDL_GetEventID( const MMDL * mmdl )
{
  return( mmdl->event_id );
}

//--------------------------------------------------------------
/**
 * MMDL �C�x���gID���G�C���A�X���ǂ����`�F�b�N
 * @param  mmdl    MMDL * 
 * @retval  int      TRUE=�G�C���A�XID�ł��� FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MMDL_CheckAliesEventID( const MMDL * mmdl )
{
  u16 id = (u16)MMDL_GetEventID( mmdl );
  
  if( id == SP_SCRID_ALIES ){
    return( TRUE );
  }

  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �w�b�_�[�w������擾
 * @param  mmdl    MMDL * 
 * @retval  u32      DIR_UP��
 */
//--------------------------------------------------------------
u32 MMDL_GetDirHeader( const MMDL * mmdl )
{
  return( mmdl->dir_head );
}

//--------------------------------------------------------------
/**
 * MMDL �\�������Z�b�g�@����
 * @param  mmdl      MMDL * 
 * @param  dir        DIR_UP��
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetForceDirDisp( MMDL * mmdl, u16 dir )
{
  mmdl->dir_disp_old = mmdl->dir_disp;
  mmdl->dir_disp = dir;
}

//--------------------------------------------------------------
/**
 * MMDL �\�������Z�b�g
 * @param  mmdl      MMDL * 
 * @param  dir        DIR_UP��
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetDirDisp( MMDL * mmdl, u16 dir )
{
  if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_PAUSE_DIR) == 0 ){
    mmdl->dir_disp_old = mmdl->dir_disp;
    mmdl->dir_disp = dir;
  }
}

//--------------------------------------------------------------
/**
 * MMDL �\�������擾
 * @param  mmdl  MMDL * 
 * @retval  u16   DIR_UP��
 */
//--------------------------------------------------------------
u16 MMDL_GetDirDisp( const MMDL * mmdl )
{
  return( mmdl->dir_disp );
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ��\�������擾
 * @param  mmdl      MMDL * 
 * @retval  dir        DIR_UP��
 */
//--------------------------------------------------------------
u16 MMDL_GetDirDispOld( const MMDL * mmdl )
{
  return( mmdl->dir_disp_old );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ������Z�b�g
 * @param  mmdl      MMDL * 
 * @param  dir        DIR_UP��
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetDirMove( MMDL * mmdl, u16 dir )
{
  mmdl->dir_move_old = mmdl->dir_move;
  mmdl->dir_move = dir;
}

//--------------------------------------------------------------
/**
 * MMDL �ړ������擾
 * @param  mmdl      MMDL * 
 * @retval  u16    DIR_UP��
 */
//--------------------------------------------------------------
u16 MMDL_GetDirMove( const MMDL * mmdl )
{
  return( mmdl->dir_move );
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ��ړ������擾
 * @param  mmdl      MMDL * 
 * @retval  u16  DIR_UP��
 */
//--------------------------------------------------------------
u16 MMDL_GetDirMoveOld( const MMDL * mmdl )
{
  return( mmdl->dir_move_old );
}

//--------------------------------------------------------------
/**
 * MMDL �\���A�ړ������Z�b�g
 * @param  mmdl      MMDL * 
 * @param  dir        DIR_UP��
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetDirAll( MMDL * mmdl, u16 dir )
{
  MMDL_SetDirDisp( mmdl, dir );
  MMDL_SetDirMove( mmdl, dir );
}

//--------------------------------------------------------------
/**
 * MMDL �w�b�_�[�w��p�����^�Z�b�g
 * @param  mmdl  MMDL * 
 * @param  param  �p�����^
 * @param  no    �Z�b�g����p�����^�ԍ��@MMDL_PARAM_0��
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetParam( MMDL *mmdl, u16 param, MMDL_H_PARAM no )
{
  switch( no ){
  case MMDL_PARAM_0: mmdl->param0 = param; break;
  case MMDL_PARAM_1: mmdl->param1 = param; break;
  case MMDL_PARAM_2: mmdl->param2 = param; break;
  default: GF_ASSERT( 0 );
  }
}

//--------------------------------------------------------------
/**
 * MMDL �w�b�_�[�w��p�����^�擾
 * @param  mmdl      MMDL *
 * @param  param      MMDL_PARAM_0��
 * @retval  u16 �p�����^
 */
//--------------------------------------------------------------
u16 MMDL_GetParam( const MMDL * mmdl, MMDL_H_PARAM param )
{
  switch( param ){
  case MMDL_PARAM_0: return( mmdl->param0 );
  case MMDL_PARAM_1: return( mmdl->param1 );
  case MMDL_PARAM_2: return( mmdl->param2 );
  }
  
  GF_ASSERT( 0 );
  return( 0 );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ�����X�Z�b�g
 * @param  mmdl      MMDL * 
 * @param  x        �ړ�����
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetMoveLimitX( MMDL * mmdl, s16 x )
{
  mmdl->move_limit_x = x;
}

//--------------------------------------------------------------
/**
 * MMDL �ړ�����X�擾
 * @param  mmdl    MMDL * 
 * @retval  s16      �ړ�����X
 */
//--------------------------------------------------------------
s16 MMDL_GetMoveLimitX( const MMDL * mmdl )
{
  return( mmdl->move_limit_x );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ�����Z�Z�b�g
 * @param  mmdl      MMDL * 
 * @param  z        �ړ�����
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetMoveLimitZ( MMDL * mmdl, s16 z )
{
  mmdl->move_limit_z = z;
}

//--------------------------------------------------------------
/**
 * MMDL �ړ�����Z�擾
 * @param  mmdl    MMDL * 
 * @retval  s16    �ړ�����z
 */
//--------------------------------------------------------------
s16 MMDL_GetMoveLimitZ( const MMDL * mmdl )
{
  return( mmdl->move_limit_z );
}

//--------------------------------------------------------------
/**
 * MMDL �`��X�e�[�^�X�Z�b�g
 * @param  mmdl    MMDL * 
 * @param  st      DRAW_STA_STOP��
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetDrawStatus( MMDL * mmdl, u16 st )
{
  mmdl->draw_status = st;
}

//--------------------------------------------------------------
/**
 * MMDL �`��X�e�[�^�X�擾
 * @param  mmdl    MMDL * 
 * @retval  u16      DRAW_STA_STOP��
 */
//--------------------------------------------------------------
u16 MMDL_GetDrawStatus( const MMDL * mmdl )
{
  return( mmdl->draw_status );
}

//--------------------------------------------------------------
/**
 * MMDL MMDLSYS *�擾
 * @param  mmdl      MMDL * 
 * @retval  MMDLSYS  MMDLSYS *
 */
//--------------------------------------------------------------
MMDLSYS * MMDL_GetMMdlSys( const MMDL *mmdl )
{
  return( mmdl->pMMdlSys );
}

//--------------------------------------------------------------
/**
 * MMDL ����֐��p���[�N�������B
 * size�����[�N�T�C�Y�𒴂��Ă����ꍇ�AASSERT�B
 * ����p���[�N��size���A0�ŏ������B
 * @param  mmdl  MMDL * 
 * @param  size  �K�v�ȃ��[�N�T�C�Y
 * @retval  void*  ���������ꂽ���[�N
 */
//--------------------------------------------------------------
void * MMDL_InitMoveProcWork( MMDL * mmdl, int size )
{
  void *work;
  GF_ASSERT( size <= MMDL_MOVE_WORK_SIZE );
  work = MMDL_GetMoveProcWork( mmdl );
  GFL_STD_MemClear( work, size );
  return( work );
}

//--------------------------------------------------------------
/**
 * MMDL ����֐��p���[�N�擾�B
 * @param  mmdl  MMDL * 
 * @retval  void*  ���[�N
 */
//--------------------------------------------------------------
void * MMDL_GetMoveProcWork( MMDL * mmdl )
{
  return( mmdl->move_proc_work );
}

//--------------------------------------------------------------
/**
 * MMDL ����⏕�֐��p���[�N�������B
 * size�����[�N�T�C�Y�𒴂��Ă����ꍇ�AASSERT�B
 * @param  mmdl  MMDL * 
 * @param  size  �K�v�ȃ��[�N�T�C�Y
 * @retval  void*  ���������ꂽ���[�N
 */
//--------------------------------------------------------------
void * MMDL_InitMoveSubProcWork( MMDL * mmdl, int size )
{
  u8 *work;
  
  GF_ASSERT( size <= MMDL_MOVE_SUB_WORK_SIZE );
  work = MMDL_GetMoveSubProcWork( mmdl );
  GFL_STD_MemClear( work, size );
  return( work );
}

//--------------------------------------------------------------
/**
 * MMDL ����⏕�֐��p���[�N�擾
 * @param  mmdl  MMDL * 
 * @retval  void*  ���[�N*
 */
//--------------------------------------------------------------
void * MMDL_GetMoveSubProcWork( MMDL * mmdl )
{
  return( mmdl->move_sub_proc_work );
}

//--------------------------------------------------------------
/**
 * MMDL ����R�}���h�p���[�N�������B
 * size�����[�N�T�C�Y�𒴂��Ă����ꍇ�AASSERT�B
 * @param  mmdl  MMDL * 
 * @param  size  �K�v�ȃ��[�N�T�C�Y
 * @retval  void*  ���������ꂽ���[�N
 */
//--------------------------------------------------------------
void * MMDL_InitMoveCmdWork( MMDL * mmdl, int size )
{
  u8 *work;
  
  GF_ASSERT( size <= MMDL_MOVE_CMD_WORK_SIZE );
  work = MMDL_GetMoveCmdWork( mmdl );
  GFL_STD_MemClear( work, size );
  return( work );
}

//--------------------------------------------------------------
/**
 * MMDL ����R�}���h�p���[�N�擾
 * @param  mmdl  MMDL *
 * @retval  void*  ���[�N*
 */
//--------------------------------------------------------------
void * MMDL_GetMoveCmdWork( MMDL * mmdl )
{
  return( mmdl->move_cmd_proc_work );
}

//--------------------------------------------------------------
/**
 * MMDL �`��֐��p���[�N�������B
 * size�����[�N�T�C�Y�𒴂��Ă����ꍇ�AASSERT�B
 * @param  mmdl  MMDL * 
 * @param  size  �K�v�ȃ��[�N�T�C�Y
 * @retval  void*  ���������ꂽ���[�N
 */
//--------------------------------------------------------------
void * MMDL_InitDrawProcWork( MMDL * mmdl, int size )
{
  u8 *work;
  
  GF_ASSERT( size <= MMDL_DRAW_WORK_SIZE );
  work = MMDL_GetDrawProcWork( mmdl );
  GFL_STD_MemClear( work, size );
  return( work );
}

//--------------------------------------------------------------
/**
 * MMDL �`��֐��p���[�N�擾
 * @param  mmdl  MMDL * 
 * @retval  void*  ���[�N
 */
//--------------------------------------------------------------
void * MMDL_GetDrawProcWork( MMDL * mmdl )
{
  return( mmdl->draw_proc_work );
}

//--------------------------------------------------------------
/**
 * MMDL ���쏉�����֐����s
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_CallMoveInitProc( MMDL * mmdl )
{
  GF_ASSERT( mmdl->move_proc_list );
  GF_ASSERT( mmdl->move_proc_list->init_proc );
  if( mmdl->move_proc_list->init_proc != NULL ){
    mmdl->move_proc_list->init_proc( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * MMDL ����֐����s
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_CallMoveProc( MMDL * mmdl )
{
  GF_ASSERT( mmdl->move_proc_list );
  GF_ASSERT( mmdl->move_proc_list->move_proc );
  if( mmdl->move_proc_list->move_proc != NULL ){
    mmdl->move_proc_list->move_proc( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * MMDL �폜�֐����s
 * @param  mmdl  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_CallMoveDeleteProc( MMDL * mmdl )
{
  GF_ASSERT( mmdl->move_proc_list );
  GF_ASSERT( mmdl->move_proc_list->delete_proc );
  if( mmdl->move_proc_list->delete_proc != NULL ){
    mmdl->move_proc_list->delete_proc( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * MMDL ���A�֐����s
 * @param  mmdl  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_CallMovePopProc( MMDL * mmdl )
{
  GF_ASSERT( mmdl->move_proc_list );
  GF_ASSERT( mmdl->move_proc_list->recover_proc );
  if( mmdl->move_proc_list->recover_proc != NULL ){
    mmdl->move_proc_list->recover_proc( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * MMDL �`�揉�����֐����s
 * @param  mmdl  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_CallDrawInitProc( MMDL * mmdl )
{
  GF_ASSERT( mmdl->draw_proc_list );
  GF_ASSERT( mmdl->draw_proc_list->init_proc );
  if( mmdl->draw_proc_list->init_proc != NULL ){
    mmdl->draw_proc_list->init_proc( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * MMDL �`��֐����s
 * @param  mmdl  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_CallDrawProc( MMDL * mmdl )
{
  GF_ASSERT( mmdl->draw_proc_list );
  GF_ASSERT( mmdl->draw_proc_list->draw_proc );
  if( mmdl->draw_proc_list->draw_proc != NULL ){
    mmdl->draw_proc_list->draw_proc( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * MMDL �`��폜�֐����s
 * @param  mmdl  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_CallDrawDeleteProc( MMDL * mmdl )
{
  GF_ASSERT( mmdl->draw_proc_list );
  GF_ASSERT( mmdl->draw_proc_list->delete_proc );
  if( mmdl->draw_proc_list->delete_proc != NULL ){
    mmdl->draw_proc_list->delete_proc( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * MMDL �`��ޔ��֐����s
 * @param  mmdl  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_CallDrawPushProc( MMDL * mmdl )
{
  GF_ASSERT( mmdl->draw_proc_list );
  GF_ASSERT( mmdl->draw_proc_list->push_proc );
  if( mmdl->draw_proc_list->push_proc != NULL ){
    mmdl->draw_proc_list->push_proc( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * MMDL �`�敜�A�֐����s
 * @param  mmdl  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_CallDrawPopProc( MMDL * mmdl )
{
  GF_ASSERT( mmdl->draw_proc_list );
  GF_ASSERT( mmdl->draw_proc_list->pop_proc );
  if( mmdl->draw_proc_list->pop_proc != NULL ){
    mmdl->draw_proc_list->pop_proc( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * MMDL �`��擾�֐����s
 * @param  mmdl  MMDL*
 * @param  state  �擾�֐��ɗ^������
 * @retval  nothing
 */
//--------------------------------------------------------------
u32 MMDL_CallDrawGetProc( MMDL *mmdl, u32 state )
{
  GF_ASSERT( mmdl->draw_proc_list );
  GF_ASSERT( mmdl->draw_proc_list->get_proc );
  if( mmdl->draw_proc_list->get_proc != NULL ){
    return( mmdl->draw_proc_list->get_proc(mmdl,state) );
  }
  return( 0 );
}

//--------------------------------------------------------------
/**
 * MMDL �A�j���[�V�����R�}���h�R�[�h�Z�b�g
 * @param  mmdl  MMDL * 
 * @param  code  AC_DIR_U��
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetAcmdCode( MMDL * mmdl, u16 code )
{
  mmdl->acmd_code = code;
}

//--------------------------------------------------------------
/**
 * MMDL �A�j���[�V�����R�}���h�R�[�h�擾
 * @param  mmdl  MMDL * 
 * @retval  u16  AC_DIR_U��
 */
//--------------------------------------------------------------
u16 MMDL_GetAcmdCode( const MMDL * mmdl )
{
  return( mmdl->acmd_code );
}

//--------------------------------------------------------------
/**
 * MMDL �A�j���[�V�����R�}���h�V�[�P���X�Z�b�g
 * @param  mmdl  MMDL * 
 * @param  no    �V�[�P���X
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetAcmdSeq( MMDL * mmdl, u16 no )
{
  mmdl->acmd_seq = no;
}

//--------------------------------------------------------------
/**
 * MMDL �A�j���[�V�����R�}���h�V�[�P���X����
 * @param  mmdl  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_IncAcmdSeq( MMDL * mmdl )
{
  mmdl->acmd_seq++;
}

//--------------------------------------------------------------
/**
 * MMDL �A�j���[�V�����R�}���h�V�[�P���X�擾
 * @param  mmdl  MMDL * 
 * @retval  u16 �V�[�P���X
 */
//--------------------------------------------------------------
u16 MMDL_GetAcmdSeq( const MMDL * mmdl )
{
  return( mmdl->acmd_seq );
}

//--------------------------------------------------------------
/**
 * MMDL ���݂̃}�b�v�A�g���r���[�g���Z�b�g
 * @param  mmdl  MMDL *
 * @param  attr  �Z�b�g����A�g���r���[�g
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetMapAttr( MMDL * mmdl, u32 attr )
{
  mmdl->now_attr = attr;
}

//--------------------------------------------------------------
/**
 * MMDL ���݂̃}�b�v�A�g���r���[�g���擾
 * @param  mmdl  MMDL *
 * @retval  u32    �}�b�v�A�g���r���[�g
 */
//--------------------------------------------------------------
u32 MMDL_GetMapAttr( const MMDL * mmdl )
{
  return( mmdl->now_attr );
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ��̃}�b�v�A�g���r���[�g���Z�b�g
 * @param  mmdl  MMDL *
 * @param  attr  �Z�b�g����A�g���r���[�g
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetMapAttrOld( MMDL * mmdl, u32 attr )
{
  mmdl->old_attr = attr;
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ��̃}�b�v�A�g���r���[�g���擾
 * @param  mmdl  MMDL *
 * @retval  u32    �}�b�v�A�g���r���[�g
 */
//--------------------------------------------------------------
u32 MMDL_GetMapAttrOld( const MMDL * mmdl )
{
  return( mmdl->old_attr );
}

//--------------------------------------------------------------
/**
 * MMDL �G�C���A�X���̃]�[��ID�擾�B
 * ���G�C���A�X���̓C�x���g�t���O���w��]�[��ID
 * @param  mmdl  MMDL *
 * @retval  u16 �]�[��ID
 */
//--------------------------------------------------------------
u16 MMDL_GetZoneIDAlies( const MMDL * mmdl )
{
  GF_ASSERT( MMDL_CheckStatusBitAlies(mmdl) == TRUE );
  return( MMDL_GetEventFlag(mmdl) );
}

//--------------------------------------------------------------
/**
 * MMDL �������W �O���b�hX���W�擾
 * @param  mmdl  MMDL * 
 * @retval  s16 X���W
 */
//--------------------------------------------------------------
s16 MMDL_GetInitGridPosX( const MMDL * mmdl )
{
  return( mmdl->gx_init );
}

//--------------------------------------------------------------
/**
 * MMDL �������W �O���b�hX���W�Z�b�g
 * @param  mmdl  MMDL * 
 * @param  x    �Z�b�g������W
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetInitGridPosX( MMDL * mmdl, s16 x )
{
  mmdl->gx_init = x;
}

//--------------------------------------------------------------
/**
 * MMDL �������W Y���W�擾
 * @param  mmdl  MMDL * 
 * @retval  s16    Y
 */
//--------------------------------------------------------------
s16 MMDL_GetInitGridPosY( const MMDL * mmdl )
{
  return( mmdl->gy_init );
}

//--------------------------------------------------------------
/**
 * MMDL �������W Y���W�Z�b�g
 * @param  mmdl  MMDL * 
 * @param  y    �Z�b�g������W
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetInitGridPosY( MMDL * mmdl, s16 y )
{
  mmdl->gy_init = y;
}

//--------------------------------------------------------------
/**
 * MMDL �������W z���W�擾
 * @param  mmdl  MMDL * 
 * @retval  s16    z���W
 */
//--------------------------------------------------------------
s16 MMDL_GetInitGridPosZ( const MMDL * mmdl )
{
  return( mmdl->gz_init );
}

//--------------------------------------------------------------
/**
 * MMDL �������W z���W�Z�b�g
 * @param  mmdl  MMDL * 
 * @param  z    �Z�b�g������W
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetInitGridPosZ( MMDL * mmdl, s16 z )
{
  mmdl->gz_init = z;
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ����W�@X���W�擾
 * @param  mmdl  MMDL * 
 * @retval  s16    X���W
 */
//--------------------------------------------------------------
s16 MMDL_GetOldGridPosX( const MMDL * mmdl )
{
  return( mmdl->gx_old );
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ����W X���W�Z�b�g
 * @param  mmdl  MMDL * 
 * @param  x    �Z�b�g������W
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetOldGridPosX( MMDL * mmdl, s16 x )
{
  mmdl->gx_old = x;
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ����W Y���W�擾
 * @param  mmdl  MMDL * 
 * @retval  s16    Y
 */
//--------------------------------------------------------------
s16 MMDL_GetOldGridPosY( const MMDL * mmdl )
{
  return( mmdl->gy_old );
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ����W Y���W�Z�b�g
 * @param  mmdl  MMDL * 
 * @param  y    �Z�b�g������W
 * @retval  s16    y���W
 */
//--------------------------------------------------------------
void MMDL_SetOldGridPosY( MMDL * mmdl, s16 y )
{
  mmdl->gy_old = y;
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ����W z���W�擾
 * @param  mmdl  MMDL * 
 * @retval  s16    z���W
 */
//--------------------------------------------------------------
s16 MMDL_GetOldGridPosZ( const MMDL * mmdl )
{
  return( mmdl->gz_old );
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ����W z���W�Z�b�g
 * @param  mmdl  MMDL * 
 * @param  z    �Z�b�g������W
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetOldGridPosZ( MMDL * mmdl, s16 z )
{
  mmdl->gz_old = z;
}

//--------------------------------------------------------------
/**
 * MMDL ���݃O���b�h���W
 * @param  mmdl  MMDL * 
 * @retval  MMDL_GRIDPOS
 */
//--------------------------------------------------------------
void MMDL_GetGridPos( const MMDL * mmdl, MMDL_GRIDPOS* pos )
{
  pos->gx = mmdl->gx_now;
  pos->gy = mmdl->gy_now;
  pos->gz = mmdl->gz_now;
}

//--------------------------------------------------------------
/**
 * MMDL ���ݍ��W X���W�擾
 * @param  mmdl  MMDL * 
 * @retval  s16    X���W
 */
//--------------------------------------------------------------
s16 MMDL_GetGridPosX( const MMDL * mmdl )
{
  return( mmdl->gx_now );
}

//--------------------------------------------------------------
/**
 * MMDL ���ݍ��W X���W�Z�b�g
 * @param  mmdl  MMDL * 
 * @param  x    �Z�b�g������W
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetGridPosX( MMDL * mmdl, s16 x )
{
  mmdl->gx_now = x;
}

//--------------------------------------------------------------
/**
 * MMDL ���ݍ��W X���W����
 * @param  mmdl  MMDL * 
 * @param  x    �����l
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_AddGridPosX( MMDL * mmdl, s16 x )
{
  mmdl->gx_now += x;
}

//--------------------------------------------------------------
/**
 * MMDL ���ݍ��W Y���W�擾
 * @param  mmdl  MMDL * 
 * @retval  s16    Y
 */
//--------------------------------------------------------------
s16 MMDL_GetGridPosY( const MMDL * mmdl )
{
  return( mmdl->gy_now );
}

//--------------------------------------------------------------
/**
 * MMDL ���ݍ��W Y���W�Z�b�g
 * @param  mmdl  MMDL * 
 * @param  y    �Z�b�g������W
 * @retval  s16    y���W
 */
//--------------------------------------------------------------
void MMDL_SetGridPosY( MMDL * mmdl, s16 y )
{
  mmdl->gy_now = y;
}

//--------------------------------------------------------------
/**
 * MMDL ���ݍ��W Y���W����
 * @param  mmdl  MMDL * 
 * @param  y    �����l
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_AddGridPosY( MMDL * mmdl, s16 y )
{
  mmdl->gy_now += y;
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ����W z���W�擾
 * @param  mmdl  MMDL * 
 * @retval  s16    z���W
 */
//--------------------------------------------------------------
s16 MMDL_GetGridPosZ( const MMDL * mmdl )
{
  return( mmdl->gz_now );
}

//--------------------------------------------------------------
/**
 * MMDL ���ݍ��W z���W�Z�b�g
 * @param  mmdl  MMDL * 
 * @param  z    �Z�b�g������W
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetGridPosZ( MMDL * mmdl, s16 z )
{
  mmdl->gz_now = z;
}

//--------------------------------------------------------------
/**
 * MMDL ���ݍ��W z���W����
 * @param  mmdl  MMDL * 
 * @param  z    �����l
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_AddGridPosZ( MMDL * mmdl, s16 z )
{
  mmdl->gz_now += z;
}

//--------------------------------------------------------------
/**
 * MMDL �����W�|�C���^�擾
 * @param  mmdl  MMDL * 
 * @retval VecFx32*
 */
//--------------------------------------------------------------
const VecFx32 * MMDL_GetVectorPosAddress( const MMDL * mmdl )
{
  return( &mmdl->vec_pos_now );
}

//--------------------------------------------------------------
/**
 * MMDL �����W�擾
 * @param  mmdl  MMDL * 
 * @param  vec    ���W�i�[��
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_GetVectorPos( const MMDL * mmdl, VecFx32 *vec )
{
  *vec = mmdl->vec_pos_now;
}

//--------------------------------------------------------------
/**
 * MMDL �����W�Z�b�g
 * @param  mmdl  MMDL * 
 * @param  vec    �Z�b�g���W
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetVectorPos( MMDL * mmdl, const VecFx32 *vec )
{
  mmdl->vec_pos_now = *vec;
}

//--------------------------------------------------------------
/**
 * MMDL �����WY�l�擾
 * @param  mmdl  MMDL * 
 * @retval  fx32  ����
 */
//--------------------------------------------------------------
fx32 MMDL_GetVectorPosY( const MMDL * mmdl )
{
  return( mmdl->vec_pos_now.y );
}

//--------------------------------------------------------------
/**
 * MMDL �\�����W�I�t�Z�b�g�擾
 * @param  mmdl  MMDL * 
 * @param  vec    �Z�b�g���W
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_GetVectorDrawOffsetPos( const MMDL * mmdl, VecFx32 *vec )
{
  *vec = mmdl->vec_draw_offs;
}

//--------------------------------------------------------------
/**
 * MMDL �\�����W�I�t�Z�b�g�Z�b�g
 * @param  mmdl  MMDL * 
 * @param  vec    �Z�b�g���W
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetVectorDrawOffsetPos( MMDL * mmdl, const VecFx32 *vec )
{
  mmdl->vec_draw_offs = *vec;
}

//--------------------------------------------------------------
/**
 * MMDL �O���w��\�����W�I�t�Z�b�g�擾
 * @param  mmdl  MMDL * 
 * @param  vec    �Z�b�g���W
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_GetVectorOuterDrawOffsetPos( const MMDL * mmdl, VecFx32 *vec )
{
  *vec = mmdl->vec_draw_offs_outside;
}

//--------------------------------------------------------------
/**
 * MMDL �O���w��\�����W�I�t�Z�b�g�Z�b�g
 * @param  mmdl  MMDL * 
 * @param  vec    �Z�b�g���W
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetVectorOuterDrawOffsetPos( MMDL * mmdl, const VecFx32 *vec )
{
  mmdl->vec_draw_offs_outside = *vec;
}

//--------------------------------------------------------------
/**
 * MMDL �A�g���r���[�g�ω����W�I�t�Z�b�g�擾
 * @param  mmdl  MMDL * 
 * @param  vec    �Z�b�g���W
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_GetVectorAttrDrawOffsetPos( const MMDL * mmdl, VecFx32 *vec )
{
  *vec = mmdl->vec_attr_offs;
}

//--------------------------------------------------------------
/**
 * MMDL �A�g���r���[�g�ω����W�I�t�Z�b�g�Z�b�g
 * @param  mmdl  MMDL * 
 * @param  vec    �Z�b�g���W
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetVectorAttrDrawOffsetPos( MMDL * mmdl, const VecFx32 *vec )
{
  mmdl->vec_attr_offs = *vec;
}

//--------------------------------------------------------------
/**
 * MMDL ����(�O���b�h�P��)���擾
 * @param  mmdl  MMDL *
 * @retval  s16    �����BH_GRID�P��
 * @attention �F�X�����Ĕp�~�\��B���s����Ɩ������ŃA�T�[�g�B
 * �O���b�hY���W��MMDL_GetGridPosY()���g�p���Ă��������B
 * �����W����O���b�h�l���~�����ꍇ��MMDL_GetVectorPosY()����ϊ����ĉ������B
 */
//--------------------------------------------------------------
#if 0
s16 MMDL_GetHeightGrid( const MMDL * mmdl )
{
  fx32 y = MMDL_GetVectorPosY( mmdl );
  s16 gy = SIZE_GRID_FX32( y );
  GF_ASSERT( 0 );
  return( gy );
}
#endif

//--------------------------------------------------------------
/**
 * MMDL MMDL_BLACTCONT���擾
 * @param  mmdl  MMDL *
 * @retval  MMDL_BLACTCONT*
 */
//--------------------------------------------------------------
MMDL_BLACTCONT * MMDL_GetBlActCont( MMDL *mmdl )
{
  return( MMDLSYS_GetBlActCont((MMDLSYS*)MMDL_GetMMdlSys(mmdl)) );
}

//--------------------------------------------------------------
/**
 * MMDL ���O���b�h�T�C�Y���擾
 * @param mmdl MMDL*
 * @retval u8 �O���b�h�T�C�Y
 */
//--------------------------------------------------------------
u8 MMDL_GetGridSizeX( const MMDL *mmdl )
{
  return( mmdl->gx_size );
}

//--------------------------------------------------------------
/**
 * MMDL ���O���b�h�T�C�Y���擾
 * @param mmdl MMDL*
 * @retval u8 �O���b�h�T�C�Y
 */
//--------------------------------------------------------------
u8 MMDL_GetGridSizeZ( const MMDL *mmdl )
{
  return( mmdl->gz_size );
}

//--------------------------------------------------------------
/**
 * MMDL �Ǘ��\�ݒ�I�t�Z�b�g���W���擾
 * @param mmdl MMDL*
 * @param pos ���W�i�[��
 * @retval
 */
//--------------------------------------------------------------
void MMDL_GetControlOffsetPos( const MMDL *mmdl, VecFx32 *pos )
{
  pos->x = NUM_FX32( mmdl->offset_x );
  pos->y = NUM_FX32( mmdl->offset_y );
  pos->z = NUM_FX32( mmdl->offset_z );
}

//======================================================================
//  MMDLSYS �X�e�[�^�X����
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS �`�揈��������������Ă��邩�ǂ����`�F�b�N
 * @param  mmdlsys  MMDLSYS *
 * @retval  BOOL  TRUE=����������Ă���
 */
//--------------------------------------------------------------
BOOL MMDLSYS_CheckCompleteDrawInit( const MMDLSYS *mmdlsys )
{
  if( MMDLSYS_CheckStatusBit(
      mmdlsys,MMDLSYS_STABIT_DRAW_INIT_COMP) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDLSYS �`�揈�������������Z�b�g
 * @param  mmdlsys  MMDLSYS*
 * @param  flag  TRUE=����������
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_SetCompleteDrawInit( MMDLSYS *mmdlsys, BOOL flag )
{
  if( flag == TRUE ){
    mmdlsys_OnStatusBit( mmdlsys, MMDLSYS_STABIT_DRAW_INIT_COMP );
  }else{
    mmdlsys_OffStatusBit( mmdlsys, MMDLSYS_STABIT_DRAW_INIT_COMP );
  }
}

//--------------------------------------------------------------
/**
 * MMDLSYS �e��t����A�t���Ȃ��̃Z�b�g
 * @param  mmdlsys MMDLSYS *
 * @param  flag  TRUE=�e��t���� FALSE=�e��t���Ȃ�
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_SetJoinShadow( MMDLSYS *mmdlsys, BOOL flag )
{
  if( flag == FALSE ){
    mmdlsys_OnStatusBit( mmdlsys, MMDLSYS_STABIT_SHADOW_JOIN_NOT );
  }else{
    mmdlsys_OffStatusBit( mmdlsys, MMDLSYS_STABIT_SHADOW_JOIN_NOT );
  }
}

//--------------------------------------------------------------
/**
 * MMDLSYS �e��t����A�t���Ȃ��̃`�F�b�N
 * @param  mmdlsys MMDLSYS *
 * @retval  BOOL TRUE=�t����
 */
//--------------------------------------------------------------
BOOL MMDLSYS_CheckJoinShadow( const MMDLSYS *mmdlsys )
{
  if( MMDLSYS_CheckStatusBit(mmdlsys,MMDLSYS_STABIT_SHADOW_JOIN_NOT) ){
    return( FALSE );
  }
  return( TRUE );
}

//======================================================================
//  MMDLSYS �v���Z�X����
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS �t�B�[���h���샂�f���S�̂̓�������S��~�B
 * ���쏈���A�`�揈�������S��~����B�A�j���[�V�����R�}���h�ɂ��������Ȃ��B
 * @param  mmdlsys  MMDLSYS*
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_StopProc( MMDLSYS *mmdlsys )
{
  mmdlsys_OnStatusBit( mmdlsys,
    MMDLSYS_STABIT_MOVE_PROC_STOP|MMDLSYS_STABIT_DRAW_PROC_STOP );
}

//--------------------------------------------------------------
/**
 * MMDLSYS MMDLSYS_StopProc()�̉����B
 * @param  mmdlsys  MMDLSYS*
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_PlayProc( MMDLSYS *mmdlsys )
{
  mmdlsys_OffStatusBit( mmdlsys,
    MMDLSYS_STABIT_MOVE_PROC_STOP|MMDLSYS_STABIT_DRAW_PROC_STOP );
}

//--------------------------------------------------------------
/**
 * MMDLSYS �t�B�[���h���샂�f���S�̂̓�����ꎞ��~
 * �ŗL�̓��쏈���i�����_���ړ����j���ꎞ��~����B
 * �A�j���[�V�����R�}���h�ɂ͔�������B
 * @param  mmdlsys  MMDLSYS *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_PauseMoveProc( MMDLSYS *mmdlsys )
{
  u32 no = 0;
  MMDL *mmdl;
  
  while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) == TRUE ){
    if( MMDL_CheckMoveBitRejectPauseMove(mmdl) == FALSE ){
      MMDL_OnMoveBitMoveProcPause( mmdl );
    }
  }
  
  mmdlsys_OnStatusBit( mmdlsys, MMDLSYS_STABIT_PAUSE_ALL );
}

//--------------------------------------------------------------
/**
 * MMDLSYS �t�B�[���h���샂�f���S�̂̈ꎞ��~������
 * @param  mmdlsys  MMDLSYS *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_ClearPauseMoveProc( MMDLSYS *mmdlsys )
{
  u32 no = 0;
  MMDL *mmdl;

  while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) == TRUE ){
    MMDL_OffMoveBitMoveProcPause( mmdl );
  }
  
  mmdlsys_OffStatusBit( mmdlsys, MMDLSYS_STABIT_PAUSE_ALL );
}

//======================================================================
//  MMDL �X�e�[�^�X�r�b�g�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f������
 * �t�B�[���h���샂�f���V�X�e���̃r�b�g�`�F�b�N
 * @param  mmdl    MMDL*
 * @param  bit      MMDLSYS_STABIT_DRAW_INIT_COMP��
 * @retval  u32      0�ȊO bit�q�b�g
 */
//--------------------------------------------------------------
u32 MMDL_CheckMMdlSysStatusBit(
  const MMDL *mmdl, MMDLSYS_STABIT bit )
{
  const MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
  return( MMDLSYS_CheckStatusBit(mmdlsys,bit) );
}

//--------------------------------------------------------------
/**
 * MMDL �g�p�`�F�b�N
 * @param  mmdl  MMDL *
 * @retval  BOOL  TRUE=�g�p��
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitUse( const MMDL *mmdl )
{
  if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_USE) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ����쒆�ɂ���
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_OnMoveBitMove( MMDL *mmdl )
{
  MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_MOVE );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ����쒆������
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_OffMoveBitMove( MMDL * mmdl )
{
  MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_MOVE );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ����쒆�`�F�b�N
 * @param  mmdl  MMDL *
 * @retval  BOOL TRUE=���쒆
 */
//--------------------------------------------------------------
BOOL MMDL_CheckMoveBitMove( const MMDL *mmdl )
{
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_MOVE) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ�����J�n�ɂ���
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_OnMoveBitMoveStart( MMDL * mmdl )
{
  MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_MOVE_START );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ�����J�n������
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_OffMoveBitMoveStart( MMDL * mmdl )
{
  MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_MOVE_START );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ�����J�n�`�F�b�N
 * @param  mmdl  MMDL *
 * @retval  BOOL TRUE=�ړ�����J�n
 */
//--------------------------------------------------------------
BOOL MMDL_CheckMoveBitMoveStart( const MMDL * mmdl )
{
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_MOVE_START) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ�����I���ɂ���
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_OnMoveBitMoveEnd( MMDL * mmdl )
{
  MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_MOVE_END );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ�����I��������
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_OffMoveBitMoveEnd( MMDL * mmdl )
{
  MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_MOVE_END );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ�����I���`�F�b�N
 * @param  mmdl  MMDL *
 * @retval  BOOL TRUE=�ړ��I��
 */
//--------------------------------------------------------------
BOOL MMDL_CheckMoveBitMoveEnd( const MMDL * mmdl )
{
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_MOVE_END) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �`�揉���������ɂ���
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_OnMoveBitCompletedDrawInit( MMDL * mmdl )
{
  MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_DRAW_PROC_INIT_COMP );
}

//--------------------------------------------------------------
/**
 * MMDL �`�揉��������������
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_OffMoveBitCompletedDrawInit( MMDL * mmdl )
{
  MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_DRAW_PROC_INIT_COMP );
}

//--------------------------------------------------------------
/**
 * MMDL �`�揉���������`�F�b�N
 * @param  mmdl  MMDL *
 * @retval  BOOL TRUE=�`�揉��������
 */
//--------------------------------------------------------------
BOOL MMDL_CheckMoveBitCompletedDrawInit( const MMDL * mmdl )
{
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_DRAW_PROC_INIT_COMP) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL ��\���t���O���`�F�b�N
 * @param  mmdl  MMDL *
 * @retval  BOOL TRUE=��\���@FALSE=�\��
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitVanish( const MMDL * mmdl )
{
  if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_VANISH) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL ��\���t���O��ݒ�
 * @param  mmdl  MMDL *
 * @param  flag  TRUE=��\���@FALSE=�\��
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitVanish( MMDL * mmdl, BOOL flag )
{
  if( flag == TRUE ){
    MMDL_OnStatusBit( mmdl, MMDL_STABIT_VANISH );
  }else{
    MMDL_OffStatusBit( mmdl, MMDL_STABIT_VANISH );
  }
}

//--------------------------------------------------------------
/**
 * MMDL OBJ���m�̓����蔻��t���O��ݒ�
 * @param  mmdl  MMDL *
 * @param  flag  TRUE=�q�b�g�A���@FALSE=�q�b�g�i�V
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitFellowHit( MMDL * mmdl, BOOL flag )
{
  if( flag == TRUE ){
    MMDL_OffStatusBit( mmdl, MMDL_STABIT_FELLOW_HIT_NON );
  }else{
    MMDL_OnStatusBit( mmdl, MMDL_STABIT_FELLOW_HIT_NON );
  }
}

//--------------------------------------------------------------
/**
 * MMDL ���쒆�t���O�̃Z�b�g
 * @param  mmdl  MMDL *
 * @param  flag  TRUE=���쒆�@FALSE=��~��
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetMoveBitMove( MMDL * mmdl, int flag )
{
  if( flag == TRUE ){
    MMDL_OnMoveBitMove( mmdl );
  }else{
    MMDL_OffMoveBitMove( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * MMDL �b�������\�`�F�b�N
 * @param  mmdl  MMDL *
 * @retval  BOOL TRUE=�\ FALSE=�s��
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitTalk( MMDL * mmdl )
{
  if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_TALK_OFF) ){
    return( FALSE );
  }
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * MMDL �b�������s�t���O���Z�b�g
 * @param  mmdl  MMDL *
 * @param  flag  TRUE=�s�� FALSE=�\
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitTalkOFF( MMDL * mmdl, BOOL flag )
{
  if( flag == TRUE ){
    MMDL_OnStatusBit( mmdl, MMDL_STABIT_TALK_OFF );
  }else{
    MMDL_OffStatusBit( mmdl, MMDL_STABIT_TALK_OFF );
  }
}

//--------------------------------------------------------------
/**
 * MMDL ����|�[�Y
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_OnMoveBitMoveProcPause( MMDL * mmdl )
{
  MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_PAUSE_MOVE );
}

//--------------------------------------------------------------
/**
 * MMDL ����|�[�Y����
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_OffMoveBitMoveProcPause( MMDL * mmdl )
{
  MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_PAUSE_MOVE );
}

//--------------------------------------------------------------
/**
 * MMDL ����|�[�Y�`�F�b�N
 * @param  mmdl  MMDL *
 * @retval  BOOL TRUE=����|�[�Y
 */
//--------------------------------------------------------------
BOOL MMDL_CheckMoveBitMoveProcPause( const MMDL * mmdl )
{
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_PAUSE_MOVE) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �X�e�[�^�X�r�b�g �`�揈�������������`�F�b�N
 * @param  mmdl    MMDL *
 * @retval  BOOL TRUE=�����BFALSE=�܂�
 */
//--------------------------------------------------------------
BOOL MMDL_CheckCompletedDrawInit( const MMDL * mmdl )
{
  const MMDLSYS *mmdlsys;
  
  mmdlsys = MMDL_GetMMdlSys( mmdl );
  
  if( MMDLSYS_CheckCompleteDrawInit(mmdlsys) == FALSE ){
    return( FALSE );
  }
  
  if( MMDL_CheckMoveBitCompletedDrawInit(mmdl) ){
    return( TRUE );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �����擾���֎~����
 * @param  mmdl  MMDL *
 * @param  int    TRUE=�����擾OFF FALSE=ON
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitHeightGetOFF( MMDL * mmdl, BOOL flag )
{
  if( flag == TRUE ){
    MMDL_OnStatusBit( mmdl, MMDL_STABIT_HEIGHT_GET_OFF );
  }else{
    MMDL_OffStatusBit( mmdl, MMDL_STABIT_HEIGHT_GET_OFF );
  }
}

//--------------------------------------------------------------
/**
 * MMDL �����擾���֎~����Ă��邩�`�F�b�N
 * @param  mmdl  MMDL *
 * @retval  BOOL TRUE=�֎~
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitHeightGetOFF( const MMDL * mmdl )
{
  if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_HEIGHT_GET_OFF) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �]�[���؂�ւ����̍폜�֎~
 * @param  mmdl  MMDL *
 * @param  flag  TRUE=�֎~ FALSE=�֎~���Ȃ�
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitNotZoneDelete( MMDL * mmdl, BOOL flag )
{
  if( flag == TRUE ){
    MMDL_OnStatusBit( mmdl, MMDL_STABIT_ZONE_DEL_NOT );
  }else{
    MMDL_OffStatusBit( mmdl, MMDL_STABIT_ZONE_DEL_NOT );
  }
}

//--------------------------------------------------------------
/**
 * MMDL �G�C���A�X�t���O���Z�b�g
 * @param  mmdl  MMDL *
 * @param  flag  TRUE=�G�C���A�X FALSE=�Ⴄ
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitAlies( MMDL * mmdl, BOOL flag )
{
  if( flag == TRUE ){
    MMDL_OnStatusBit( mmdl, MMDL_STABIT_ALIES );
  }else{
    MMDL_OffStatusBit( mmdl, MMDL_STABIT_ALIES );
  }
}

//--------------------------------------------------------------
/**
 * MMDL �G�C���A�X�t���O���`�F�b�N
 * @param  mmdl  MMDL *
 * @retval  BOOL TRUE=�G�C���A�X FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitAlies( const MMDL * mmdl )
{
  if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_ALIES) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �󐣃G�t�F�N�g�Z�b�g�t���O���Z�b�g
 * @param  mmdl  MMDL *
 * @param  flag  TRUE=�Z�b�g�@FALSE=�N���A
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetMoveBitShoalEffect( MMDL * mmdl, BOOL flag )
{
  if( flag == TRUE ){
    MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_EFFSET_SHOAL );
  }else{
    MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_EFFSET_SHOAL );
  }
}

//--------------------------------------------------------------
/**
 * MMDL �󐣃G�t�F�N�g�Z�b�g�t���O���`�F�b�N
 * @param  mmdl  MMDL *
 * @retval  BOOL TRUE=�Z�b�g�@FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MMDL_CheckMoveBitShoalEffect( const MMDL * mmdl )
{
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_EFFSET_SHOAL) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �A�g���r���[�g�I�t�Z�b�g�ݒ�OFF�Z�b�g
 * @param  mmdl  MMDL *
 * @param  flag  TRUE=�Z�b�g�@FALSE=�N���A
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitAttrOffsetOFF( MMDL * mmdl, BOOL flag )
{
  if( flag == TRUE ){
    MMDL_OnStatusBit( mmdl, MMDL_STABIT_ATTR_OFFS_OFF );
  }else{
    MMDL_OffStatusBit( mmdl, MMDL_STABIT_ATTR_OFFS_OFF );
  }
}

//--------------------------------------------------------------
/**
 * MMDL �A�g���r���[�g�I�t�Z�b�g�ݒ�OFF�`�F�b�N
 * @param  mmdl  MMDL *
 * @retval  BOOL TRUE=OFF�@FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitAttrOffsetOFF( const MMDL * mmdl )
{
  if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_ATTR_OFFS_OFF) ){
    return( TRUE );
  }
  return( FALSE );
}

#if 0 //old pl
//--------------------------------------------------------------
/**
 * MMDL ���ړ��t���O�Z�b�g
 * @param  mmdl  MMDL *
 * @param  flag  TRUE=�Z�b�g�@FALSE=�N���A
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitBridge( MMDL * mmdl, BOOL flag )
{
  if( flag == TRUE ){
    MMDL_OnStatusBit( mmdl, MMDL_STABIT_BRIDGE );
  }else{
    MMDL_OffStatusBit( mmdl, MMDL_STABIT_BRIDGE );
  }
}

//--------------------------------------------------------------
/**
 * MMDL ���ړ��t���O�`�F�b�N
 * @param  mmdl  MMDL *
 * @retval  BOOL TRUE=���@FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitBridge( const MMDL * mmdl )
{
  if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_BRIDGE) ){
    return( TRUE );
  }
  return( FALSE );
}
#endif

//--------------------------------------------------------------
/**
 * MMDL �f�肱�݃t���O�Z�b�g
 * @param  mmdl  MMDL *
 * @param  flag  TRUE=�Z�b�g�@FALSE=�N���A
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetMoveBitReflect( MMDL * mmdl, BOOL flag )
{
  if( flag == TRUE ){
    MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_REFLECT_SET );
  }else{
    MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_REFLECT_SET );
  }
}

//--------------------------------------------------------------
/**
 * MMDL �f�肱�݃t���O�`�F�b�N
 * @param  mmdl  MMDL *
 * @retval  BOOL TRUE=�Z�b�g�@FALSE=����
 */
//--------------------------------------------------------------
BOOL MMDL_CheckMoveBitReflect( const MMDL * mmdl )
{
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_REFLECT_SET) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �A�j���[�V�����R�}���h�`�F�b�N
 * @param  mmdl  MMDL *
 * @retval  BOOL TRUE=�R�}���h�A���@FALSE=����
 */
//--------------------------------------------------------------
BOOL MMDL_CheckMoveBitAcmd( const MMDL * mmdl )
{
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_ACMD) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL ����|�[�Y���ۃt���O�Z�b�g
 * @param  mmdl  MMDL *
 * @param  flag  TRUE=�Z�b�g�@FALSE=�N���A
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetMoveBitRejectPauseMove( MMDL *mmdl, BOOL flag )
{
  if( flag == TRUE ){
    MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_REJECT_PAUSE_MOVE );
  }else{
    MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_REJECT_PAUSE_MOVE );
  }
}

//--------------------------------------------------------------
/**
 * MMDL ����|�[�Y���ۃt���O�`�F�b�N
 * @param  mmdl  MMDL *
 * @retval BOOL TRUE=�Z�b�g�@FALSE=����
 */
//--------------------------------------------------------------
BOOL MMDL_CheckMoveBitRejectPauseMove( const MMDL *mmdl )
{
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_REJECT_PAUSE_MOVE) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �g�����������t���O���Z�b�g
 * @param  mmdl  MMDL *
 * @param  flag  TRUE=�Z�b�g�@FALSE=�N���A
 * @retval  nothing
 */
//--------------------------------------------------------------
#if 0 //wb null
void MMDL_SetStatusBitHeightExpand( MMDL * mmdl, BOOL flag )
{
  if( flag == TRUE ){
    MMDL_OnStatusBit( mmdl, MMDL_STABIT_HEIGHT_EXPAND );
  }else{
    MMDL_OffStatusBit( mmdl, MMDL_STABIT_HEIGHT_EXPAND );
  }
}

//--------------------------------------------------------------
/**
 * MMDL �g�����������t���O�`�F�b�N
 * @param  mmdl  MMDL *
 * @retval  BOOL TRUE=�g�������ɔ�������@FALSE=����
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitHeightExpand( const MMDL * mmdl )
{
  if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_HEIGHT_EXPAND) ){
    return( TRUE );
  }
  return( FALSE );
}
#endif

//--------------------------------------------------------------
/**
 * MMDL �A�g���r���[�g�擾���~
 * @param  mmdl  MMDL *
 * @param  flag  TRUE=��~
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitAttrGetOFF( MMDL * mmdl, BOOL flag )
{
  if( flag == TRUE ){
    MMDL_OnStatusBit( mmdl, MMDL_STABIT_ATTR_GET_OFF );
  }else{
    MMDL_OffStatusBit( mmdl, MMDL_STABIT_ATTR_GET_OFF );
  }
}

//--------------------------------------------------------------
/**
 * MMDL �A�g���r���[�g�擾���~�@�`�F�b�N
 * @param  mmdl  MMDL *
 * @retval  int  TRUE=��~
 */
//--------------------------------------------------------------
int MMDL_CheckStatusBitAttrGetOFF( const MMDL * mmdl )
{
  if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_ATTR_GET_OFF) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL ��Z�[�u�Ώۂɂ���
 * @param mmdl MMDL*
 * @param flag TRUE=��Z�[�u FALSE=�Z�[�u�����
 * @retval nothing
 */
//--------------------------------------------------------------
void MMDL_SetMoveBitNotSave( MMDL *mmdl, BOOL flag )
{
  if( flag == TRUE ){
    MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_NOT_SAVE );
  }else{
    MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_NOT_SAVE );
  }
}

//--------------------------------------------------------------
/**
 * MMDL �ړ��G�t�F�N�g���o���Ȃ�
 * @param mmdl MMDL*
 * @param flag TRUE=�o���Ȃ� FALSE=�o��
 * @retval nothing
 * 
 * �Г�BTS1625�Ώ��ׂ̈̒ǉ������B
 * ���̉e�����l���A����͓y���G�t�F�N�g�݂̂��Ώ�
 */
//--------------------------------------------------------------
void MMDL_SetMoveBitNonCreateMoveEffect( MMDL *mmdl, BOOL flag )
{
  if( flag == TRUE ){
    MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_NON_CREATE_MOVE_EFFECT );
  }else{
    MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_NON_CREATE_MOVE_EFFECT );
  }
}

//--------------------------------------------------------------
/**
 * MMDL �ړ��G�t�F�N�g���o���Ȃ��@�`�F�b�N
 * @param mmdl MMDL*
 * @retval BNOOL TRUE=�o���Ȃ� FALSE=�o��
 * 
 * �Г�BTS1625�Ώ��ׂ̈̒ǉ������B
 * ���̉e�����l���A����͓y���G�t�F�N�g�݂̂��Ώ�
 */
//--------------------------------------------------------------
BOOL MMDL_CheckMoveBitNonCreateMoveEffect( const MMDL *mmdl )
{
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_NON_CREATE_MOVE_EFFECT) ){
    return( TRUE );
  }
  
  return( FALSE );
}

//======================================================================
//  MMDLSYS �c�[��
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS �g�p���Ă���t�B�[���h���샂�f����T���B
 * @param  mmdlsys  MMDLSYS *
 * @param  mmdl  MMDL*�i�[��
 * @param  no  �����J�n���[�Nno�B�擪���猟������ۂ͏����l0���w��B
 * @retval BOOL TRUE=���샂�f���擾���� FALSE=no����I�[�܂Ō������擾����
 * @note ����no�͌Ăяo����A�擾�ʒu+1�̒l�ɂȂ�B
 * @note ����FOBJ ID 1�Ԃ̓��샂�f����T���B
 * u32 no=0;
 * MMDL *mmdl;
 * while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) == TRUE ){
 *     if( MMDL_GetOBJID(mmdl) == 1 ){
 *       break;
 *     }
 * }
 */
//--------------------------------------------------------------
BOOL MMDLSYS_SearchUseMMdl(
  const MMDLSYS *mmdlsys, MMDL **mmdl, u32 *no )
{
  u32 max = MMDLSYS_GetMMdlMax( mmdlsys );
  
  if( (*no) < max ){
    MMDL *check_obj = &(((MMDLSYS*)mmdlsys)->pMMdlBuf[*no]);
    
    do{
      (*no)++;
      if( MMDL_CheckStatusBit(check_obj,MMDL_STABIT_USE) ){
        *mmdl = check_obj;
        return( TRUE );
      }
      check_obj++;
    }while( (*no) < max );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDLSYS �w�肳�ꂽ�O���b�hX,Z���W�ɂ���OBJ�S�Ă�MMDL�|�C���^�z��Ɏ擾
 * @param  sys      MMDLSYS *
 * @param  x      �����O���b�hX
 * @param  z      �����O���b�hZ
 * @param  old_hit    TURE=�ߋ����W�����肷��
 * @param array   ������MMDL�|�C���^���ő�16���i�[����\���̌^
 * @return  �������z�� 
 */
//--------------------------------------------------------------
static int MMDLSYS_SearchGridPosArray(
  const MMDLSYS *sys, s16 x, s16 z, BOOL old_hit, MMDL_PARRAY *array )
{
  u32 no = 0;
  MMDL *mmdl;
  
  MI_CpuClear8( array, sizeof(MMDL_PARRAY) );
  
  while( MMDLSYS_SearchUseMMdl(sys,&mmdl,&no) == TRUE ){
    if( MMDL_HitCheckXZ(mmdl,x,z,old_hit) == TRUE ){
      array->mmdl_parray[array->count++] = mmdl;
    
      if( array->count >= MMDL_POST_LAYER_MAX ){
        GF_ASSERT_MSG( 0, "������WOBJ���I�[�o�[\n" ); 
        break;  //����ȏ�i�[�ł��Ȃ�
      }
    }
  }
  
  return array->count;
}

//--------------------------------------------------------------
/**
 * MMDLSYS �w�肳�ꂽ�O���b�hX,Z���W�ɂ���OBJ���擾(���̌������l���Ȃ���)
 * @param  sys      MMDLSYS *
 * @param  x      �����O���b�hX
 * @param  z      �����O���b�hZ
 * @param  old_hit    TURE=�ߋ����W�����肷��
 * @retval  MMDL  x,z�ʒu�ɂ���MMDL * �BNULL=���̍��W��OBJ�͂��Ȃ�
 */
//--------------------------------------------------------------
MMDL * MMDLSYS_SearchGridPos(
  const MMDLSYS *sys, s16 x, s16 z, BOOL old_hit )
{
  u32 num = 0;
  MMDL_PARRAY array;

  num = MMDLSYS_SearchGridPosArray(sys, x, z, old_hit , &array);
  
  if( num ){
    return array.mmdl_parray[0];
  }
  
  return( NULL );
}

//--------------------------------------------------------------
/**
 * MMDLSYS �w�肳�ꂽ�O���b�hX,Z,and Y���W�ɂ���OBJ���擾(���̌������l������)
 * @param  sys      MMDLSYS *
 * @param  x        �����O���b�hX
 * @param  z        �����O���b�hZ
 * @param height  ����Y���W�l fx32�^
 * @param h_diff  Y�l�ŋ��e����U�ꕝ(��Βl�����̎w��l������Y����������OBJ�݂̂�Ԃ�)
 *
 * @param  old_hit    TURE=�ߋ����W�����肷��
 * @retval  MMDL  x,z�ʒu�ɂ���MMDL * �BNULL=���̍��W��OBJ�͂��Ȃ�
 */
//--------------------------------------------------------------
MMDL * MMDLSYS_SearchGridPosEx(
  const MMDLSYS *sys, s16 x, s16 z, fx32 height, fx32 y_diff, BOOL old_hit )
{
  u32 i = 0,num = 0;
  MMDL_PARRAY array;
  
  num = MMDLSYS_SearchGridPosArray(sys, x, z, old_hit , &array);
  
  if( num == 0 ){
    return NULL;
  }
  
  for(i = 0;i < num;i++){
    fx32 y,diff;
    diff = MMDL_GetVectorPosY( array.mmdl_parray[i] ) - height;
    if(diff < 0){
      diff = FX_Mul(diff,FX32_CONST(-1));
    }
    if(diff < y_diff) {
      return array.mmdl_parray[i];
    }
  }
  
  return( NULL );
}

//--------------------------------------------------------------
/**
 * MMDLSYS ����R�[�h�Ɉ�v����MMDL *������
 * @param  mmdlsys    MMDLSYS *
 * @param  mv_code    �������铮��R�[�h
 * @retval  MMDL *  NULL=���݂��Ȃ�
 */
//--------------------------------------------------------------
MMDL * MMDLSYS_SearchMoveCode( const MMDLSYS *mmdlsys, u16 mv_code )
{
  u32 no = 0;
  MMDL *mmdl;
  
  while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) == TRUE ){
    if( MMDL_GetMoveCode(mmdl) == mv_code ){
      return( mmdl );
    }
  }
  
  return( NULL );
}

//--------------------------------------------------------------
/**
 * MMDLSYS OBJ ID�Ɉ�v����MMDL *������
 * @param  mmdlsys    MMDLSYS *
 * @param  id    ��������OBJ ID
 * @retval  MMDL *  NULL=���݂��Ȃ�
 */
//--------------------------------------------------------------
MMDL * MMDLSYS_SearchOBJID( const MMDLSYS *mmdlsys, u16 id )
{
  u32 no = 0;
  MMDL *mmdl;

  while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) == TRUE ){
    if( MMDL_CheckStatusBitAlies(mmdl) == FALSE ){
      if( MMDL_GetOBJID(mmdl) == id ){
        return( mmdl );
      }
    }
  }
  
  return( NULL );
}

//--------------------------------------------------------------
/**
 * MMDLSYS ���@OBJ��T��
 * @param mmdlsys MMDLSYS*
 * @retval MMDL* NULL=���݂��Ȃ�
 */
//--------------------------------------------------------------
MMDL * MMDLSYS_SearchMMdlPlayer( MMDLSYS *mmdlsys )
{
  MMDL *mmdl = MMDLSYS_SearchOBJID( mmdlsys, MMDL_ID_PLAYER );
  GF_ASSERT( mmdl != NULL );
  return( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDLSYS �t�B�[���h���샂�f���̋󂫂�T��
 * @param  sys      MMDLSYS *
 * @retval  MMDL  �󂫂�MMDL*�@�󂫂������ꍇ��NULL
 */
//--------------------------------------------------------------
static MMDL * mmdlsys_SearchSpaceMMdl( const MMDLSYS *sys )
{
  int i,max;
  MMDL *mmdl;
  
  i = 0;
  max = MMDLSYS_GetMMdlMax( sys );
  mmdl = ((MMDLSYS*)sys)->pMMdlBuf;
  
  do{
    if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_USE) == 0 ){
      return( mmdl );
    }
    
    mmdl++;
    i++;
  }while( i < max );
  
  return( NULL );
}

//--------------------------------------------------------------
/**
 * MMDLSYS �t�B�[���h���샂�f���@�G�C���A�X��T��
 * @param  mmdlsys      MMDLSYS *
 * @param  obj_id    ��v����OBJ ID
 * @param  zone_id    ��v����ZONE ID
 * @retval  MMDL  ��v����MMDL*�@��v����=NULL
 */
//--------------------------------------------------------------
static MMDL * mmdlsys_SearchAlies(
  const MMDLSYS *mmdlsys, int obj_id, int zone_id )
{
  u32 no = 0;
  MMDL * mmdl;
  
  while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) ){
    if( MMDL_CheckStatusBitAlies(mmdl) == TRUE ){
      if( MMDL_GetOBJID(mmdl) == obj_id ){
        if( MMDL_GetZoneIDAlies(mmdl) == zone_id ){
          return( mmdl );
        }
      }
    }
  }
  
  return( NULL );
}

//--------------------------------------------------------------
/**
 * MMDLSYS �t�B�[���h���샂�f�� �]�[���X�V���̓��샂�f���폜
 * @param  mmdlsys  MMDLSYS
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDLSYS_DeleteZoneUpdateMMdl( MMDLSYS *mmdlsys )
{
  u32 no = 0;
  MMDL *mmdl;
  
  while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) ){
    //�{���ł���΍X�ɃG�C���A�X�`�F�b�N������
    if( MMDL_CheckStatusBit(
        mmdl,MMDL_STABIT_ZONE_DEL_NOT) == 0 ){
      if( MMDL_GetOBJID(mmdl) == 0xff ){
        GF_ASSERT( 0 );
      }
      MMDL_Delete( mmdl );
    }
  }
}

//======================================================================
//  MMDL �c�[��
//======================================================================
//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���@TCB����֐��ǉ�
 * @param  mmdl  MMDL*
 * @param  sys    MMDLSYS*
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_AddTCB( MMDL *mmdl, const MMDLSYS *sys )
{
  int pri,code,offs;
  GFL_TCB * tcb;
  
  offs = 0;
  pri = MMDLSYS_GetTCBPriority( sys );
  code = MMDL_GetMoveCode( mmdl );
  
  if( code == MV_PAIR || code == MV_TR_PAIR ){
    offs = MMDL_TCBPRI_OFFS_PAIR;
  }else if( MMDL_GetOBJID(mmdl) == MMDL_ID_PLAYER ){
    offs = MMDL_TCBPRI_OFFS_PLAYER;
  }
  
  pri += offs;
  GF_ASSERT( pri >= 0 );

  tcb = GFL_TCB_AddTask( MMDLSYS_GetTCBSYS((MMDLSYS*)sys),
      mmdl_TCB_MoveProc, mmdl, (u32)pri );
  GF_ASSERT( tcb != NULL );
  
  mmdl->pTCB = tcb;
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���@TCB����֐��폜
 * @param  mmdl  MMDL*
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_DeleteTCB( MMDL *mmdl )
{
#if 0 //tcb�ǉ����ꂸ�폜����鎖������
  GF_ASSERT( mmdl->pTCB );
  GFL_TCB_DeleteTask( mmdl->pTCB );
  mmdl->pTCB = NULL;
#else
  if( mmdl->pTCB != NULL ){
    GFL_TCB_DeleteTask( mmdl->pTCB );
    mmdl->pTCB = NULL;
  }
#endif
}

//--------------------------------------------------------------
/**
 * MMDL ���ݔ������Ă���t�B�[���h���샂�f����OBJ�R�[�h���Q��
 * @param  mmdl  MMDL * 
 * @param  code  �`�F�b�N����R�[�h�BHERO��
 * @retval  BOOL  TRUE=mmdl�ȊO�ɂ�code�������Ă���z������
 */
//--------------------------------------------------------------
BOOL MMDL_SearchUseOBJCode( const MMDL *mmdl, u16 code )
{
  u32 no = 0;
  u16 check_code;
  MMDL *cmmdl;
  const MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
  
  while( MMDLSYS_SearchUseMMdl(mmdlsys,&cmmdl,&no) == TRUE ){
    if( cmmdl != mmdl ){
      check_code = MMDL_GetOBJCode( cmmdl );
      if( check_code != OBJCODEMAX && check_code == code ){
        return( TRUE );
      }
    }
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL ���W�A�������������B
 * @param  mmdl  MMDL *
 * @param  vec    ���������W
 * @param  dir    ���� DIR_UP��
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_InitPosition( MMDL * mmdl, const VecFx32 *vec, u16 dir )
{
  int grid = SIZE_GRID_FX32( vec->x );
  MMDL_SetGridPosX( mmdl, grid );
  
  grid = SIZE_GRID_FX32( vec->y );
  MMDL_SetGridPosY( mmdl, grid );
  
  grid = SIZE_GRID_FX32( vec->z );
  MMDL_SetGridPosZ( mmdl, grid );
  
  MMDL_SetVectorPos( mmdl, vec );
  MMDL_UpdateGridPosCurrent( mmdl );
  
  MMDL_SetForceDirDisp( mmdl, dir );
  
  MMDL_FreeAcmd( mmdl );
  MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_MOVE_START );
  MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_MOVE|MMDL_MOVEBIT_MOVE_END );
}

//--------------------------------------------------------------
/**
 * MMDL ���W�A�������������B�O���b�h��
 * @param  mmdl  MMDL *
 * @param  vec    ���������W
 * @param  dir    ���� DIR_UP��
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_InitGridPosition( MMDL * mmdl, s16 gx, s16 gy, s16 gz, u16 dir )
{
  VecFx32 pos;
  pos.x = GRID_SIZE_FX32( gx ) + MMDL_VEC_X_GRID_OFFS_FX32;
  pos.y = GRID_SIZE_FX32( gy );
  pos.z = GRID_SIZE_FX32( gz ) + MMDL_VEC_Z_GRID_OFFS_FX32;
  MMDL_InitPosition( mmdl, &pos, dir );
}

//--------------------------------------------------------------
/**
 * MMDL �w�b�_�[������W�^�C�v�A���W�A����R�[�h��ύX
 * @param  mmdl  MMDL *
 * @param  head MMDL_HEADAER
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_ChangeMoveParam( MMDL *mmdl, const MMDL_HEADER *head )
{
  const MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
  
  MMDL_CallMoveDeleteProc( mmdl );
  mmdl_SetHeaderMoveParam( mmdl, head );
  mmdl_CallSetHeaderPos( mmdl, head );
  mmdl_InitCallMoveProcWork( mmdl );
  mmdl_InitMoveProc( mmdlsys, mmdl );
  mmdl_SetHeaderAfter( mmdl, head, NULL );
}

//--------------------------------------------------------------
/**
 * MMDL ����R�[�h�w�b�_�[������W�^�C�v�A���W�A�R�[�h��ύX
 * @param  mmdl  MMDL *
 * @param  code MV_RND��
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_ChangeMoveCode( MMDL *mmdl, u16 code )
{
  const MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
  
  MMDL_CallMoveDeleteProc( mmdl );
  MMDL_SetMoveCode( mmdl, code );
  mmdl_InitCallMoveProcWork( mmdl );
  mmdl_InitMoveProc( mmdlsys, mmdl );
}

//--------------------------------------------------------------
/**
 * MMDL �`�揉�����ɍs�������Z��
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_InitDrawWork( MMDL *mmdl )
{
  const MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
  
  if( MMDLSYS_CheckCompleteDrawInit(mmdlsys) == FALSE ){
    return; //�`��V�X�e���������������Ă��Ȃ�
  }
  
  if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_HEIGHT_GET_ERROR) ){
    MMDL_UpdateCurrentHeight( mmdl );
  }
  
  MMDL_SetDrawStatus( mmdl, 0 );
  
  if( MMDL_CheckMoveBitCompletedDrawInit(mmdl) == FALSE ){
    mmdl_InitCallDrawProcWork( mmdl );
    MMDL_CallDrawInitProc( mmdl );
    MMDL_OnMoveBitCompletedDrawInit( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f�� �`��֐�������
 * @param  mmdl    MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_InitCallDrawProcWork( MMDL * mmdl )
{
  const MMDL_DRAW_PROC_LIST *list;
  const OBJCODE_PARAM *prm = MMDL_GetOBJCodeParam( mmdl );
  list = DrawProcList_GetList( prm->draw_proc_no );
  mmdl->draw_proc_list = list;
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���֘A�G�t�F�N�g�̃t���O�������B
 * �G�t�F�N�g�֘A�̃t���O���������܂Ƃ߂�B
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_InitDrawEffectFlag( MMDL * mmdl )
{
  MMDL_OffMoveBit( mmdl,
    MMDL_MOVEBIT_SHADOW_SET |
    MMDL_MOVEBIT_SHADOW_VANISH |
    MMDL_MOVEBIT_EFFSET_SHOAL |
    MMDL_MOVEBIT_REFLECT_SET );
}

//--------------------------------------------------------------
/**
 * MMDL OBJ ID��ύX
 * @param  mmdl  MMDL *
 * @param  id    OBJ ID
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_ChangeOBJID( MMDL * mmdl, u16 id )
{
  MMDL_SetOBJID( mmdl, id );
  MMDL_OnMoveBitMoveStart( mmdl );
  mmdl_InitDrawEffectFlag( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDL ���[�N����
 * @param  mmdl  MMDL
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_ClearWork( MMDL *mmdl )
{
  GFL_STD_MemClear( mmdl, MMDL_SIZE );
}

//--------------------------------------------------------------
/**
 * MMDL �w�肳�ꂽ�t�B�[���h���샂�f�����G�C���A�X�w�肩�ǂ����`�F�b�N
 * @param  mmdl    MMDL *
 * @param  h_zone_id  head��ǂݍ��ރ]�[��ID
 * @param  max      head�v�f��
 * @param  head    MMDL_H
 * @retval  int      RET_ALIES_NOT��
 */
//--------------------------------------------------------------
static int mmdl_CheckHeaderAlies(
    const MMDL *mmdl, int h_zone_id, int max,
    const MMDL_HEADER *head )
{
  u16 obj_id;
  int zone_id;
  
  while( max ){
    obj_id = head->id;
    
    if( MMDL_GetOBJID(mmdl) == obj_id ){
      //�G�C���A�X�w�b�_�[
      if( MMdlHeader_CheckAlies(head) == TRUE ){
        //�G�C���A�X�̐��K�]�[��ID
        zone_id = MMdlHeader_GetAliesZoneID( head );
        //�ΏۃG�C���A�X
        if( MMDL_CheckStatusBitAlies(mmdl) == TRUE ){
          if( MMDL_GetZoneIDAlies(mmdl) == zone_id ){
            return( RET_ALIES_EXIST );  //Alies�Ƃ��Ċ��ɑ���
          }
        }else if( MMDL_GetZoneID(mmdl) == zone_id ){
          return( RET_ALIES_CHANGE );    //Alies�Ώۂł���
        }
      }else{ //�ʏ�w�b�_�[
        if( MMDL_CheckStatusBitAlies(mmdl) == TRUE ){
          //�����G�C���A�X�ƈ�v
          if( MMDL_GetZoneIDAlies(mmdl) == h_zone_id ){
            return( RET_ALIES_CHANGE );
          }
        }
      }
    }
    
    max--;
    head++;
  }
  
  return( RET_ALIES_NOT );
}

//--------------------------------------------------------------
/**
 * MMDL �w�肳�ꂽ�]�[��ID��OBJ ID������MMDL *���擾�B
 * @param  mmdlsys    MMDLSYS *
 * @param  obj_id  OBJ ID
 * @param  zone_id  �]�[��ID
 * @retval  MMDL * MMDL *
 */
//--------------------------------------------------------------
static MMDL * mmdl_SearchOBJIDZoneID(
    const MMDLSYS *mmdlsys, int obj_id, int zone_id )
{
  u32 no = 0;
  MMDL *mmdl;
  
  while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) == TRUE ){
    if( MMDL_GetOBJID(mmdl) == obj_id &&
      MMDL_GetZoneID(mmdl) == zone_id ){
      return( mmdl );
    }
  }
  
  return( NULL );
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���`�揉�����ɍs������
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_InitDrawStatus( MMDL * mmdl )
{
  MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_MOVE_START );
  mmdl_InitDrawEffectFlag( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���`��폜���ɍs������
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_SetDrawDeleteStatus( MMDL * mmdl )
{
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���@�G�C���A�X���琳�KOBJ�ւ̕ύX
 * @param  mmdl    MMDL * 
 * @param  head    �Ώۂ�MMDL_H
 * @param  zone_id    ���K�̃]�[��ID
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_ChangeAliesOBJ(
  MMDL *mmdl, const MMDL_HEADER *head, int zone_id )
{
  GF_ASSERT( MMDL_CheckStatusBitAlies(mmdl) == TRUE );
  MMDL_SetStatusBitAlies( mmdl, FALSE );
  MMDL_SetZoneID( mmdl, zone_id );
  MMDL_SetEventID( mmdl, head->event_id );
  MMDL_SetEventFlag( mmdl, head->event_flag );
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���@���KOBJ����G�C���A�X�ւ̕ύX
 * @param  mmdl    MMDL * 
 * @param  head    �Ώۂ�MMDL_H
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdl_ChangeOBJAlies(
  MMDL * mmdl, int zone_id, const MMDL_HEADER *head )
{
  GF_ASSERT( MMdlHeader_CheckAlies(head) == TRUE );
  MMDL_SetStatusBitAlies( mmdl, TRUE );
  MMDL_SetEventID( mmdl, head->event_id );
  MMDL_SetEventFlag( mmdl, MMdlHeader_GetAliesZoneID(head) );
  MMDL_SetZoneID( mmdl, zone_id );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���̓���`�F�b�N�p��MMDL_CHECKSAME_DATA������
 * @param  mmdl  MMDL *
 * @param outData ����������mmdl�̏��i�[��
 * @retval nothing
 */
//--------------------------------------------------------------
void MMDL_InitCheckSameData( const MMDL *mmdl, MMDL_CHECKSAME_DATA *outData )
{
  outData->id = MMDL_GetOBJID( mmdl );
  outData->code = MMDL_GetOBJCode( mmdl );
  outData->zone_id = MMDL_GetZoneID( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDL_CHECKSAME_DATA���Q�Ƃ��t�B�[���h���샂�f���̓���`�F�b�N�B
 * ���S�A����ւ�肪�������Ă��邩�`�F�b�N����B
 * @param  mmdl  MMDL *
 * @param data MMDL_SAMEDATA*
 * @retval BOOL  TRUE=����BFALSE=����ł͂Ȃ�
 */
//--------------------------------------------------------------
BOOL MMDL_CheckSameData(
    const MMDL * mmdl, const MMDL_CHECKSAME_DATA *data )
{
  if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_USE) ){
    if( MMDL_GetOBJID(mmdl) == data->id &&
        MMDL_GetOBJCode(mmdl) == data->code &&
        MMDL_GetZoneID(mmdl) == data->zone_id ){
      return( TRUE );
    }
  }
   
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL_CHECKSAME_DATA���Q�Ƃ��t�B�[���h���샂�f���̓���`�F�b�N�B
 * ���S�A����ւ�肪�������Ă��邩�`�F�b�N����B
 * OBJ�R�[�h�̓���ւ��݂͓̂���Ƃ���B
 * @param  mmdl  MMDL *
 * @param data MMDL_SAMEDATA*
 * @retval BOOL  TRUE=����BFALSE=����ł͂Ȃ�
 */
//--------------------------------------------------------------
BOOL MMDL_CheckSameDataIDOnly(
    const MMDL * mmdl, const MMDL_CHECKSAME_DATA *data )
{
  if( MMDL_CheckSameData(mmdl,data) == FALSE ){
    if( MMDL_GetOBJCode(mmdl) == data->code ){
      return( TRUE );
    }
  }
   
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * OBJ�R�[�h��ύX����
 * @param mmdl MMDL*
 * @param code �ύX����R�[�h�BHERO��
 * @retval nothing
 */
//--------------------------------------------------------------
void MMDL_ChangeOBJCode( MMDL *mmdl, u16 code )
{
  const MMDLSYS *mmdlsys;
  mmdlsys = MMDL_GetMMdlSys( mmdl );
  
  if( MMDLSYS_CheckCompleteDrawInit(mmdlsys) == TRUE ){
    if( MMDL_CheckMoveBitCompletedDrawInit(mmdl) == TRUE ){
      MMDL_CallDrawDeleteProc( mmdl );
    }
  }
  
  MMDL_SetOBJCode( mmdl, code );
  mmdlsys_LoadSetMMdlOBJCodeParam( mmdlsys, code, mmdl );
  
  { //�V�KOBJCODE_PARAM
    const OBJCODE_PARAM *param = &mmdl->objcode_param;
    mmdl->gx_size = param->size_width;
    mmdl->gz_size = param->size_depth;
    mmdl->offset_x = param->offs_x; //�I�t�Z�b�g
    mmdl->offset_y = param->offs_y;
    mmdl->offset_z = param->offs_z;
  }
  
  MMDL_OffMoveBitCompletedDrawInit( mmdl );
  mmdl_InitDrawStatus( mmdl );
  mmdl_InitDrawWork( mmdl );
}

//======================================================================
//  OBJCODE_PARAM
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS OBJCODE_PARAM ������
 * @param  mmdlsys  MMDLSYS
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdlsys_InitOBJCodeParam( MMDLSYS *mmdlsys, HEAPID heapID )
{
#if 0
  u8 *p = GFL_ARC_LoadDataAlloc( ARCID_MMDL_PARAM, 
      NARC_fldmmdl_mdlparam_fldmmdl_mdlparam_bin, heapID );
  mmdlsys->pOBJCodeParamBuf = p;
  mmdlsys->pOBJCodeParamTbl = (const OBJCODE_PARAM*)(&p[OBJCODE_PARAM_TOTAL_NUMBER_SIZE]);
#endif
  
  mmdlsys->arcH_param = GFL_ARC_OpenDataHandle( ARCID_MMDL_PARAM, heapID );
}

//--------------------------------------------------------------
/**
 * MMDLSYS OBJCODE_PARAM �폜
 * @param  mmdlsys  MMDLSYS
 * @retval  nothing
 */
//--------------------------------------------------------------
static void mmdlsys_DeleteOBJCodeParam( MMDLSYS *mmdlsys )
{
#if 0
  GFL_HEAP_FreeMemory( mmdlsys->pOBJCodeParamBuf );
  mmdlsys->pOBJCodeParamBuf = NULL;
#endif
  
  GFL_ARC_CloseDataHandle( mmdlsys->arcH_param );
}

#if 0
//--------------------------------------------------------------
/**
 * MMDLSYS OBJCODE_PARAM �擾
 * @param  mmdlsys  MMDLSYS *
 * @param  code  �擾����OBJ�R�[�h
 * @retval  OBJCODE_PARAM*
 */
//--------------------------------------------------------------
const OBJCODE_PARAM * MMDLSYS_GetOBJCodeParam(
    const MMDLSYS *mmdlsys, u16 code )
{
  GF_ASSERT( mmdlsys->pOBJCodeParamTbl != NULL );
  code = MMDL_TOOL_OBJCodeToDataNumber( code );
  return( &(mmdlsys->pOBJCodeParamTbl[code]) );
}
#endif

//--------------------------------------------------------------
/**
 * �V�X�e����OBJCODE_PARAM�����[�h����
 * @note�@�@�풓�Ń������������ƂŃA�N�Z�X�X�s�[�h�����߂�
 *
 * @param   mmdlsys  MMDLSYS *
 * @param   list_id   �G���A�ʃ��X�g�h�c
 * 
 * @retval  none
 */
//--------------------------------------------------------------
void MMDLSYS_SetSysOBJCodeParam( MMDLSYS *mmdlsys, int list_id)
{
  NOZOMU_Printf("list_id %d\n",list_id);
	GFL_ARC_LoadData( mmdlsys->pMdlPrmWork, ARCID_MMDL_LIST, list_id );
  
  {
    u16 *data = (u16*)mmdlsys->pMdlPrmWork;
    u16 num = data[0];
    GF_ASSERT_MSG(num <= REGULAR_MAX, "%d �������ɓ���l���𒴂��܂���", num);
  }
}

//--------------------------------------------------------------
/**
 * �V�X�e������OBJCODE_PARAM���擾����
 * @note�@�@�풓�Ń������������ƂŃA�N�Z�X�X�s�[�h�����߂�
 *
 * @param   mmdlsys  MMDLSYS *
 * @param   idx           �V�X�e�����ێ�����p�����[�^�e�[�u���̃C���f�b�N�X
 * @param   outParam      �p�����[�^�i�[�o�b�t�@
 *
 * @retval  none
 */
//--------------------------------------------------------------
void MMDLSYS_GetSysOBJCodeParam(
    const MMDLSYS *mmdlsys, const u32 idx, OBJCODE_PARAM *outParam)
{
	u16 *data = (u16*)mmdlsys->pMdlPrmWork;
  u16 num = data[0];
  if (idx < num)
  {
    u16 ofs = ((num+1)/2) * 2;  //�A���C�����g�����������I�u�Z�b�g
    OBJCODE_PARAM *prm = (OBJCODE_PARAM *)&data[ofs+2];  //+2�̓w�b�_�ƃt�b�^�̕�
    *outParam = prm[idx];
  }
  else
  {
    GF_ASSERT_MSG(0,"ERROR idx=%d num = %",idx,num);
  }
}

//--------------------------------------------------------------
/**
 * �V�X�e������OBJCODE_PARAM���擾����
 * @note�@�@�풓�Ń������������ƂŃA�N�Z�X�X�s�[�h�����߂�
 * 
 * @param  mmdlsys  MMDLSYS *
 *
 * @retval  �o�^�������ڐ�
 */
//--------------------------------------------------------------
u16 MMDLSYS_GetSysOBJCodeParamNum( const MMDLSYS *mmdlsys)
{
	u16 *data = (u16*)mmdlsys->pMdlPrmWork;
  u16 num = data[0];
  NOZOMU_Printf("OBJ_NUM %d\n",num);
  return num;
}

//--------------------------------------------------------------
/**
 * MMDLSYS OBJCODE_PARAM ���[�h
 * @param  mmdlsys  MMDLSYS *
 * @param  code  �擾����OBJ�R�[�h
 * @param  outParam �i�[��
 * @retval nothing
 * @attention �A�[�J�C�u�f�[�^���烍�[�h���s��outParam�Ɋi�[���܂��B
 * �E�������d���ׁA�A���Ăт͍T���ĉ������B
 * �E�\�ł����MMDL_GetOBJCodeParam()���g�p���Ă��������B
 * @note �Ăяo�����x ��270ms�`300ms
 */
//--------------------------------------------------------------
void MMDLSYS_LoadOBJCodeParam(
    const MMDLSYS *mmdlsys, u16 code, OBJCODE_PARAM *outParam )
{
  {
    int i;
    u16 *data = (u16*)mmdlsys->pMdlPrmWork;
    u16 num = data[0];
    if (num < REGULAR_MAX)
    {
      u16 ofs = ((num+1)/2) * 2; //�A���C�����g�����������I�u�Z�b�g
      OBJCODE_PARAM *prm = (OBJCODE_PARAM *)&data[ofs+2]; //+2�̓w�b�_�ƃt�b�^�̕�
      for(i=0;i<num;i++)
      {
        if ( code == data[1+i] )
        {
//          NOZOMU_Printf("Find code:%d\n",code);
          *outParam = prm[i];
          return;
        }
      }
    }
  }

  { //���Ƀ��[�h�ς݂̓���R�[�h�������샂�f����T��
    u32 no = 0;
    MMDL *mmdl;
    
    while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) ){
      if( MMDL_GetOBJCode(mmdl) == code ){
        *outParam = mmdl->objcode_param;        
        D_MMDL_DPrintf(
            "MMDLSYS_LoadOBJCodeParam() CODE %xH �����f�[�^�A��\n", code );
        return;
      }
    }
  }

  {
    u16 size = sizeof( OBJCODE_PARAM );
    u16 offs = OBJCODE_PARAM_TOTAL_NUMBER_SIZE +
      (size * MMDL_TOOL_OBJCodeToDataNumber(code));
  
    GFL_ARC_LoadDataOfsByHandle( mmdlsys->arcH_param, 
        NARC_fldmmdl_mdlparam_fldmmdl_mdlparam_bin,
        offs, size, outParam );
  }
}

//--------------------------------------------------------------
/**
 * MMDLSYS OBJCODE_PARAM�𓮍샂�f���ɃZ�b�g
 * @param  mmdlsys  MMDLSYS *
 * @param  code  �擾����OBJ�R�[�h
 * @param  mmdl ���[�h��MMDL
 * @retval nothing
 */
//--------------------------------------------------------------
static void mmdlsys_LoadSetMMdlOBJCodeParam(
    const MMDLSYS *mmdlsys, u16 code, MMDL *mmdl )
{
  //�X�e�[�^�X�r�b�g���ꎞ�I�ɍ~�낵
  //MMDLSYS_LoadOBJCodeParam()���o�����̑Ώۂ���O��
  MMDL_OffStatusBit( mmdl, MMDL_STABIT_USE );
  
  //���[�h
  MMDLSYS_LoadOBJCodeParam( mmdlsys, code, &mmdl->objcode_param );
  
  //���ɖ߂�
  MMDL_OnStatusBit( mmdl, MMDL_STABIT_USE );
} 

//--------------------------------------------------------------
/**
 * MMDL MMDL�ɐݒ肳��Ă���OBJCODE_PARAM�擾
 * @param  mmdl  MMDL*
 * @param OBJCODE_PARAM*
 */
//--------------------------------------------------------------
const OBJCODE_PARAM * MMDL_GetOBJCodeParam( const MMDL *mmdl )
{
#if 0
  const MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
  return( MMDLSYS_GetOBJCodeParam(mmdlsys,code) );
#else  
  return( &(mmdl->objcode_param) );
#endif
}

//--------------------------------------------------------------
/**
 * MMDL OBJCODE_PARAM����OBJCODE_PARAM_BUF_BBD�擾
 * @param
 * @retval
 */
//--------------------------------------------------------------
const OBJCODE_PARAM_BUF_BBD * MMDL_TOOL_GetOBJCodeParamBufBBD(
    const OBJCODE_PARAM *param )
{
  return (const OBJCODE_PARAM_BUF_BBD*)param->buf;
}

//--------------------------------------------------------------
/**
 * MMDL OBJCODE_PARAM����OBJCODE_PARAM_BUF_MDL�擾
 * @param
 * @retval
 */
//--------------------------------------------------------------
const OBJCODE_PARAM_BUF_MDL * MMDL_TOOL_GetOBJCodeParamBufMDL(
    const OBJCODE_PARAM *param )
{
  return (const OBJCODE_PARAM_BUF_MDL*)param->buf;
}

//======================================================================
//  OBJCODE_PARAM �����A���s�A�I�t�Z�b�g
//======================================================================

//======================================================================
//  parts
//======================================================================
//--------------------------------------------------------------
/**
 * �w�肳�ꂽOBJ�R�[�h�����[�N�Q�ƌ^�Ȃ�΃��[�N��OBJ�R�[�h�ɂ��ĕύX
 * �Ⴄ�ꍇ�͂��̂܂܂ŕԂ��B
 * @param  fsys  FIELDSYS_WORK *
 * @param  code  OBJ�R�[�h�BHERO��
 * @retval  int    �`�F�b�N���ꂽOBJ�R�[�h
 */
//--------------------------------------------------------------
u16 MMDL_TOOL_GetWorkOBJCode( const EVENTWORK *ev, u16 code )
{
  return( WorkOBJCode_GetOBJCode(ev,code) );
}

//--------------------------------------------------------------
/**
 * �w�肳�ꂽOBJ�R�[�h�����[�N�Q�ƌ^�Ȃ�΃��[�N��OBJ�R�[�h�ɕύX�B
 * �Ⴄ�ꍇ�͂��̂܂܂ŕԂ��B
 * @param  fsys  FIELDSYS_WORK *
 * @param  code  OBJ�R�[�h�BHERO��
 * @retval  int    �`�F�b�N���ꂽOBJ�R�[�h
 */
//--------------------------------------------------------------
static u16 WorkOBJCode_GetOBJCode( const EVENTWORK *ev, u16 code )
{
  if( code >= WKOBJCODE_START && code <= WKOBJCODE_END ){
    if( ev == NULL ){
      GF_ASSERT( 0 );
      code = BOY1; //����ȃR�[�h�ɋ����ύX
    }else{
      u16 *work;
      code -= WKOBJCODE_START;
      code += OBJCHRWORK0;
      work = EVENTWORK_GetEventWorkAdrs( (EVENTWORK*)ev, code );
      code = *work;
    }
  }
  
  return( code );
}

//--------------------------------------------------------------
/**
 * �w�肳�ꂽ����R�[�h�̊֐����X�g���擾
 * @param  code  ����R�[�h
 * @retval MMDL_MOVE_PROC_LIST
 */
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST * MoveProcList_GetList( u16 code )
{
  GF_ASSERT( code < MV_CODE_MAX );
  return( DATA_FieldOBJMoveProcListTbl[code] );
}

//--------------------------------------------------------------
/**
 * �w�肳�ꂽOBJ�R�[�h�̕`��֐����X�g���擾
 * @param  code  OBJ�R�[�h
 * @retval  MMDL_DRAW_PROC_LIST*
 */
//--------------------------------------------------------------
static const MMDL_DRAW_PROC_LIST * DrawProcList_GetList(
    MMDL_DRAWPROCNO no )
{
  GF_ASSERT( no < MMDL_DRAWPROCNO_MAX );
  return( DATA_MMDL_DRAW_PROC_LIST_Tbl[no] );
}

//--------------------------------------------------------------
/**
 * MMDL_HEADER �G�C���A�X�`�F�b�N
 * @param  head  FIELD_OBJ_H
 * @retval  int    TRUE=�G�C���A�X�@FALSE=�Ⴄ
 */
//--------------------------------------------------------------
static BOOL MMdlHeader_CheckAlies( const MMDL_HEADER *head )
{
  u16 id = head->event_id;
  if( id == SP_SCRID_ALIES ){ return( TRUE ); }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �t�B�[���hOBJ�w�b�_�[�@�G�C���A�X���̃]�[��ID�擾�B
 * �G�C���A�X���̓C�x���g�t���O���]�[��ID�ɂȂ�
 * @param  head  FIELD_OBJ_H
 * @retval  int    �]�[��ID
 */
//--------------------------------------------------------------
static int MMdlHeader_GetAliesZoneID( const MMDL_HEADER *head )
{
  GF_ASSERT( MMdlHeader_CheckAlies(head) == TRUE );
  return( (int)head->event_flag );
}

//--------------------------------------------------------------
/**
 * �C�x���g�t���O�`�F�b�N
 * @param
 * @retval
 */
//--------------------------------------------------------------
static BOOL mmdlsys_CheckEventFlag( EVENTWORK *evwork, u16 flag_no )
{
#if 0 //
  FIELDMAP_WORK *fieldMap = mmdlsys->fieldMapWork;
  GF_ASSERT( fieldMap != NULL );
  
  if( fieldMap == NULL ){
    return( FALSE );
  }
  
  return( FALSE ); //FIELDMAP_WORK�̈������m�肵�Ă��Ȃ��B
  
  {
    GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( fieldMap );
    GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
    EVENTWORK *ev = GAMEDATA_GetEventWork( gdata );
    return( EVENTWORK_CheckEventFlag(ev,flag_no) );
  }
#else
  return( EVENTWORK_CheckEventFlag(evwork,flag_no) );
#endif
}

//--------------------------------------------------------------
/**
 * �w�肳�ꂽ�^�C�v���g���[�i�[�^�C�v�ł����
 * EV_TYPE_TRAINER��������EV_TYPE_TRAINER_EYEALL�ŕԂ��B
 * ����ȊO�̃^�C�v�͂��̂܂ܕԂ�
 * @param type EV_TYPE_TRAINER��
 * @retval u16 EV_TYPE_TRAINER��
 */
//--------------------------------------------------------------
u16 MMDL_TOOL_GetTrainerEventType( u16 type )
{
  switch( type ){
  case EV_TYPE_TRAINER_KYORO:
  case EV_TYPE_TRAINER_SPIN_STOP_L:
  case EV_TYPE_TRAINER_SPIN_STOP_R:
  case EV_TYPE_TRAINER_SPIN_MOVE_L:
  case EV_TYPE_TRAINER_SPIN_MOVE_R:
  case EV_TYPE_TRAINER_DASH_ALTER:
  case EV_TYPE_TRAINER_DASH_REACT:
  case EV_TYPE_TRAINER_DASH_ACCEL:
    type = EV_TYPE_TRAINER;
    break;
  }
  
  return( type );
}

//======================================================================
//  ����֐��_�~�[
//======================================================================
//--------------------------------------------------------------
/**
 * ���쏉�����֐��_�~�[
 * @param  MMDL  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MoveInitProcDummy( MMDL * mmdl )
{
}

//--------------------------------------------------------------
/**
 * ����֐��_�~�[
 * @param  MMDL  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MoveProcDummy( MMDL * mmdl )
{
}

//--------------------------------------------------------------
/**
 * ����폜�֐��_�~�[
 * @param  MMDL  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MoveDeleteProcDummy( MMDL * mmdl )
{
}

//--------------------------------------------------------------
/**
 * ���앜�A�֐��_�~�[
 * @param  MMDL *  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_MoveReturnProcDummy( MMDL * mmdl )
{
}

//======================================================================
//  �`��֐��_�~�[
//======================================================================
//--------------------------------------------------------------
/**
 * �`�揉�����֐��_�~�[
 * @param  MMDL  MMDL * 
 * @retval  int      TRUE=����������
 */
//--------------------------------------------------------------
void MMDL_DrawInitProcDummy( MMDL * mmdl )
{
}

//--------------------------------------------------------------
/**
 * �`��֐��_�~�[
 * @param  MMDL  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_DrawProcDummy( MMDL * mmdl )
{
}

//--------------------------------------------------------------
/**
 * �`��폜�֐��_�~�[
 * @param  MMDL  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_DrawDeleteProcDummy( MMDL * mmdl )
{
}

//--------------------------------------------------------------
/**
 * �`��ޔ��֐��_�~�[
 * @param  MMDL  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_DrawPushProcDummy( MMDL * mmdl )
{
}

//--------------------------------------------------------------
/**
 * �`�敜�A�֐��_�~�[
 * @param  MMDL  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_DrawPopProcDummy( MMDL * mmdl )
{
}

//======================================================================
//  MMDL_ROCKPOS,�����肫�₪�Q�ƁA�ۑ�������W�f�[�^
//======================================================================
//--------------------------------------------------------------
/// �Q�ƈʒu�f�[�^
//--------------------------------------------------------------
#include "../../../resource/fldmapdata/eventdata/define/total_header.h"
#include "../../../resource/fldmapdata/zonetable/zone_id.h"
#include "../../../resource/fldmapdata/pushrock/rockpos.cdat"

//--------------------------------------------------------------
/**
 * MMDL_ROCKPOS ���[�N�T�C�Y���擾
 * @param nothing
 * @retval u32 ���[�N�T�C�Y
 */
//--------------------------------------------------------------
u32 MMDL_ROCKPOS_GetWorkSize( void )
{
  return( sizeof(MMDL_ROCKPOS)*MMDL_ROCKPOS_MAX );
}

//--------------------------------------------------------------
/**
 * MMDL_ROCKPOS ���[�N������
 * @param p MMDL_ROCKPOS*
 * @retval nothing
 */
//--------------------------------------------------------------
void MMDL_ROCKPOS_Init( void *p )
{
  MI_CpuFill32( p, ROCKPOS_INIT, sizeof(MMDL_ROCKPOS)*MMDL_ROCKPOS_MAX );
}

//--------------------------------------------------------------
/**
 * MMDL_ROCKPOS ���W���Z�b�g����Ă��邩�ǂ���
 * @param rockpos MMDL_ROCKPOS
 * @retval  BOOL TRUE=�Z�b�g�ς�
 */
//--------------------------------------------------------------
static BOOL mmdl_rockpos_CheckSetPos( const MMDL_ROCKPOS *rockpos )
{
  if( (u32)rockpos->pos.x != ROCKPOS_INIT ||
      (u32)rockpos->pos.y != ROCKPOS_INIT ||
      (u32)rockpos->pos.z != ROCKPOS_INIT ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL_ROCKPOS �w��ID���g�p������W���[�N�ʒu�B
 * @param zone_id ZONE ID
 * @param obj_id OBJ ID
 * @retval int �g�p������W���[�N�̈ʒu�ԍ��BMMDL_ROCKPOS_MAX=�����B
 */
//--------------------------------------------------------------
static int rockpos_GetPosNumber( const u16 zone_id, const u16 obj_id )
{
  u16 z,o;
  int i = 0;
  
  do{
    z = DATA_MMDL_PushRockPosNum[i][0];
    o = DATA_MMDL_PushRockPosNum[i][1];
    
    D_MMDL_DPrintf( "���͊� ���W���[�N���� zone_id = %xH, obj_id = %xH,  data zone_id = %xH, data obj_id = %xH\n", zone_id, obj_id, z, o );

    if( z == zone_id && o == obj_id ){
      return( DATA_MMDL_PushRockPosNum[i][2] );
    }
    
    i++;
  }while( (z != ROCKPOS_DATA_END && o != ROCKPOS_DATA_END) );
  
  return( MMDL_ROCKPOS_MAX );
}

//--------------------------------------------------------------
/**
 * MMDL_ROCKPOS �w��̓��샂�f�������W��ێ����Ă��邩�ǂ���
 * @param mmdl  MMDL*
 * @retval int �ێ����Ă���ʒu�ԍ��BMMDL_ROCKPOS_MAX=�ێ������B
 */
//--------------------------------------------------------------
static int mmdl_rockpos_GetPosNumber( const MMDL *mmdl )
{
  int no = MMDL_ROCKPOS_MAX;
  
  if( MMDL_CheckOBJCodeKairiki(MMDL_GetOBJCode(mmdl)) == TRUE ){
    u16 zone_id = MMDL_GetZoneID( mmdl );
    u16 obj_id = MMDL_GetOBJID( mmdl );
    no = rockpos_GetPosNumber( zone_id, obj_id );
  }
  
  return( no );
}

//--------------------------------------------------------------
/**
 * �w��OBJ�̍��W����p���[�N�ɕۑ�����Ă��邩
 * @param mmdl MMDL*
 * @retval  BOOL TRUE=�ۑ�����BFALSE=�����B�������͏����l�̂܂܁B
 */
//--------------------------------------------------------------
static BOOL mmdl_rockpos_CheckPos( const MMDL *mmdl )
{
  int no = mmdl_rockpos_GetPosNumber( mmdl );
  
  if( no != MMDL_ROCKPOS_MAX ){
    const MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
    const MMDL_ROCKPOS *rockpos = &mmdlsys->rockpos[no];
    return( mmdl_rockpos_CheckSetPos(rockpos) );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �w��̓��샂�f�����ۑ����Ă�����W���擾
 * @param mmdl MMDL*
 * @param pos ���W�ۑ���
 * @retval BOOL TRUE=�擾 FALSE=�擾�ł��Ȃ��B
 * @note �����肫��OBJ�݂̂��ΏہB�Ⴄ�ꍇ�̓A�T�[�g�B
 */
//--------------------------------------------------------------
static BOOL mmdl_rockpos_GetPos( const MMDL *mmdl, VecFx32 *pos )
{
  int no = mmdl_rockpos_GetPosNumber( mmdl );
  
  if( no != MMDL_ROCKPOS_MAX ){
    const MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
    const MMDL_ROCKPOS *rockpos = &mmdlsys->rockpos[no];
    
    if( mmdl_rockpos_CheckSetPos(rockpos) ){
      *pos = rockpos->pos;
      return( TRUE );
    }
  }
  
  GF_ASSERT( 0 );
  return( FALSE );
}
 
//--------------------------------------------------------------
/**
 * �w��̓��샂�f���̍��W��MMDL_ROCKPOS�ɕۑ��B
 * @param mmdl MMDL*
 * @retval nothing
 * @note �����肫��OBJ�݂̂��ΏہB
 */
//--------------------------------------------------------------
void MMDL_ROCKPOS_SavePos( const MMDL *mmdl )
{
  int no = mmdl_rockpos_GetPosNumber( mmdl );
  
  if( no != MMDL_ROCKPOS_MAX ){
    VecFx32 pos;
    const MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
    MMDL_ROCKPOS *rockpos = &mmdlsys->rockpos[no];
    MMDL_GetVectorPos( mmdl, &pos );
    rockpos->pos = pos;
    return;
  }
  
  GF_ASSERT( 0 && "ERROR NOT ROCK OF SAVE" );
}

//--------------------------------------------------------------
/**
 * �w��ʒu�ɂ����肫��OBJ�������Ă��邩
 * @param mmdlsys MMDLSYS*
 * @param pos �`�F�b�N������W
 * @retval BOOL TRUE=�₪�����Ă���
 */
//--------------------------------------------------------------
BOOL MMDLSYS_ROCKPOS_CheckRockFalled(
    const MMDLSYS *mmdlsys, const VecFx32 *pos )
{
  s16 gx,gz;
  MMDL *mmdl;
  
  gx = SIZE_GRID_FX32( pos->x );
  gz = SIZE_GRID_FX32( pos->z );
  
  mmdl = MMDLSYS_SearchGridPosEx(
      mmdlsys, gx, gz, pos->y, GRID_FX32*2, FALSE );
  
  if( mmdl != NULL ){
    if( mmdl_rockpos_CheckPos(mmdl) == TRUE ){
      return( TRUE );
    }
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * OBJ�R�[�h�͉��͂ŉ����^�C�v���`�F�b�N
 * @param code OBJ�R�[�h
 * @retval BOOL TRUE=���͂ŉ�����
 */
//--------------------------------------------------------------
BOOL MMDL_CheckOBJCodeKairiki( const u16 code )
{
  if( code == ROCK || code == BIGROCK || code == JUNK || code == BIGROCK2 ){
    return( TRUE );
  }
  return( FALSE );
}

//======================================================================
//
//======================================================================
//--------------------------------------------------------------
/**
 *  @brief  �w�b�_�[�ɃO���b�h�}�b�v�|�W�V������ݒ�
 *  @param  head    �w�b�_�[
 *  @param  gx      ���O���b�h���W
 *  @param  gz      ���O���b�h���W
 *  @param  y       �����W
 */
//--------------------------------------------------------------
void MMDLHEADER_SetGridPos( MMDL_HEADER* head, u16 gx, u16 gz, int y )
{
  MMDL_HEADER_GRIDPOS* pos;
  
  GF_ASSERT( head );
  head->pos_type = MMDL_HEADER_POSTYPE_GRID;
  pos = (MMDL_HEADER_GRIDPOS*)head->pos_buf;

  pos->gx = gx;
  pos->gz = gz;
  pos->y  = y;
}

//--------------------------------------------------------------
/**
 *  @brief  �w�b�_�[�Ƀ��[���}�b�v�|�W�V������ݒ�
 *  @param  head    �w�b�_�[
 *  @param  index   �C���f�b�N�X
 *  @param  front   �t�����g�O���b�h���W
 *  @param  side    �T�C�h�O���b�h���W
 */
//--------------------------------------------------------------
void MMDLHEADER_SetRailPos( MMDL_HEADER* head, u16 index, u16 front, u16 side )
{
  MMDL_HEADER_RAILPOS* pos;
  
  GF_ASSERT( head );
  head->pos_type = MMDL_HEADER_POSTYPE_RAIL;
  pos = (MMDL_HEADER_RAILPOS*)head->pos_buf;

  pos->rail_index = index;
  pos->front_grid = front;
  pos->side_grid  = side;
}


//======================================================================
//  debug
//======================================================================
//----
#ifdef DEBUG_MMDL
//----
#include "arc/fieldmap/fldmmdl_objcodestr.naix"

//--------------------------------------------------------------
/**
 * �f�o�b�O�@���샂�f���@OBJ�R�[�h��������擾(ASCII)
 * @param  code OBJ�R�[�h
 * @param  heapID buf�̈�m�ۗpHEAPID
 * @retval  u8* �����񂪊i�[���ꂽu8*�B�g�p��GFL_HEAP_FreeMemory()���K�v�B
 * �����񒷂�DEBUG_OBJCODE_STR_LENGTH�B
 */
//--------------------------------------------------------------
u8 * DEBUG_MMDL_GetOBJCodeString( u16 code, HEAPID heapID )
{
  u8 *buf;
  
  code = MMDL_TOOL_OBJCodeToDataNumber( code );
  
  buf = GFL_HEAP_AllocClearMemoryLo(
      heapID, DEBUG_OBJCODE_STR_LENGTH );
  
  GFL_ARC_LoadDataOfs( buf, ARCID_DEBUG_MMDL_OBJCODESTR,
      NARC_fldmmdl_objcodestr_fldmmdl_objcodestr_bin,
      DEBUG_OBJCODE_STR_LENGTH * code, DEBUG_OBJCODE_STR_LENGTH );
  
  return( buf );
}

//--------------------------------------------------------------
/**
 * ���샂�f�������v�����g
 * @param mmdl �Ώۂ̓��샂�f��
 * @param f_str ���O�Ƀv�����g���镶���� NULL=�Ȃ�
 * @param e_str MMDL�v�����g��Ƀv�����g���镶���� NULL=�Ȃ�
 * @retval nothing
 */
//--------------------------------------------------------------
void DEBUG_MMDL_PrintState(
    const MMDL *mmdl, const char *f_str, const char *e_str )
{
  if( f_str != NULL ){
    OS_TPrintf( "%s\n", f_str );
  }
  
  D_MMDL_Printf( "MMDL ID %d : CODE %xH : MOVE CODE =%xH : DIR %d : GX = %d GY = %d GZ = %d : EV TYPE %d EV FLAG %d : PARAM 0(%d) 1(%d) 2(%d)\n",
      MMDL_GetOBJID(mmdl),
      MMDL_GetOBJCode(mmdl),
      MMDL_GetMoveCode(mmdl),
      MMDL_GetDirDisp(mmdl),
      MMDL_GetGridPosX(mmdl),
      MMDL_GetGridPosY(mmdl),
      MMDL_GetGridPosZ(mmdl),
      MMDL_GetEventType(mmdl),
      MMDL_GetEventFlag(mmdl),
      MMDL_GetParam(mmdl,MMDL_PARAM_0),
      MMDL_GetParam(mmdl,MMDL_PARAM_1),
      MMDL_GetParam(mmdl,MMDL_PARAM_2)
      );
      
  if( e_str != NULL ){
    OS_TPrintf( "%s\n", e_str );
  }
}

#ifdef DEBUG_ONLY_FOR_kagaya
#define DEBUG_SPEED_CHECK_ENABLE  //�f�o�b�O���x�v����L���ɂ���
#include "debug_speed_check.h"

#if 0
void DEBUG_MMDLSYS_CheckSpeed( MMDLSYS *mmdlsys )
{
  OBJCODE_PARAM param;
  MMDL *mmdl = MMDLSYS_SearchMMdlPlayer( mmdlsys );
  u16 code;

  code = MMDL_GetOBJCode( mmdl );
#if 0  
  if( code == HERO ){
    code = HEROINE;
  }else{
    code = HERO;
  }
#endif  
  INIT_CHECK();
  SET_CHECK( "MMDL CHECK PARAM_START" );
  SET_CHECK( "MMDL CHECK PARAM0" );
  
  MMDLSYS_LoadOBJCodeParam( mmdlsys, code, &param );
  
  SET_CHECK( "MMDL CHECK PARAM1" );
  SET_CHECK( "MMDL CHECK PARAM_END" );
  PUT_CHECK();

  {
    OSTick end_tick;
    TAIL_CHECK( &end_tick );
    OS_TPrintf("MMDL top_tick:%ld", end_tick);
    OS_TPrintf(" MMDL micro sec:%ld\n", OS_TicksToMicroSeconds( end_tick ) );
  }

  GF_ASSERT( 0 && "MMDL CHECK END" );
}
#endif
#endif

//----
#endif //DEBUG_MMDL
//----
