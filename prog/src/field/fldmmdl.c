//======================================================================
/**
 * @file	fldmmdl.c
 * @brief	���샂�f��
 * @author	kagaya
 * @date	05.07.13
 */
//======================================================================
#include "fldmmdl.h"
#include "fldmmdl_procdraw.h"

#include "arc_def.h"
#include "arc/fieldmap/fldmmdl_mdlparam.naix"

#include "fieldmap.h"
#include "eventwork.h"

//======================================================================
//	define
//======================================================================
//--------------------------------------------------------------
//	debug
//--------------------------------------------------------------

//--------------------------------------------------------------
//	MMDL ����A�`��֐����[�N�T�C�Y (byte size)
//--------------------------------------------------------------
#define MMDL_MOVE_WORK_SIZE		(16)	///<����֐��p���[�N�T�C�Y
#define MMDL_MOVE_SUB_WORK_SIZE	(16)	///<����T�u�֐��p���[�N�T�C�Y
#define MMDL_MOVE_CMD_WORK_SIZE	(16)	///<����R�}���h�p���[�N�T�C�Y
#define MMDL_DRAW_WORK_SIZE		(32)	///<�`��֐��p���[�N�T�C�Y

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
///	�G�C���A�X�V���{��
//--------------------------------------------------------------
enum
{
	RET_ALIES_NOT = 0,	//�G�C���A�X�ł͂Ȃ�
	RET_ALIES_EXIST,	//�G�C���A�X�Ƃ��Ċ��ɑ��݂��Ă���
	RET_ALIES_CHANGE,	//�G�C���A�X�ύX���K�v�ł���
};

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
///	MMDLSYS�\����
//--------------------------------------------------------------
struct _TAG_MMDLSYS
{
	u32 status_bit;					///<�X�e�[�^�X�r�b�g
	u16 mmdl_max;					///<MMDL�ő吔
	HEAPID sysHeapID;				///<�V�X�e���p �q�[�vID
	MMDL *pMMdlBuf;			///<MMDL���[�N *
	
	HEAPID heapID;					///<�q�[�vID
	s16 mmdl_count;				///<�t�B�[���h���샂�f�����ݐ�
	u16 tcb_pri;					///<TCB�v���C�I���e�B
	u16 dummy;						///<�]��
	const FLDMAPPER *pG3DMapper;	///<FLDMAPPER
	FLDNOGRID_MAPPER *pNOGRIDMapper;	///<FLDNOGRID_MAPPER
  
	void *pTCBSysWork;				///<TCB���[�N
	GFL_TCBSYS *pTCBSys;			///<TCBSYS*
	
	MMDL_BLACTCONT *pBlActCont;	///<MMDL_BLACTCONT
  MMDL_G3DOBJCONT *pObjCont; ///<MMDL_G3DOBJCONT

	u8 *pOBJCodeParamBuf;			///<OBJCODE_PARAM�o�b�t�@
	const OBJCODE_PARAM *pOBJCodeParamTbl; ///<OBJCODE_PARAM
  
  void *fieldMapWork; ///<FIELDMAP_WORK <-�e���[�N�P�ʂł̐ڑ����ǂ��B
  
  MMDL_ROCKPOS *rockpos; ///<�����肫����W �Z�[�u�f�[�^�|�C���^
  
  const u16 *targetCameraAngleYaw; //�O���[�o���ŎQ�Ƃ���J����
  
  ARCHANDLE *arcH_res; ///<���샂�f�����\�[�X�A�[�J�C�u�n���h��
};

#define MMDLSYS_SIZE (sizeof(MMDLSYS)) ///<MMDLSYS�T�C�Y

//--------------------------------------------------------------
///	MMDL�\����
//--------------------------------------------------------------
struct _TAG_MMDL
{
	u32 status_bit;				///<�X�e�[�^�X�r�b�g
	u32 move_bit;				///<����r�b�g
	
	u16 obj_id;					///<OBJ ID
	u16 zone_id;				///<�]�[�� ID
	u16 obj_code;				///<OBJ�R�[�h
	u16 move_code;				///<����R�[�h
	u16 event_type;				///<�C�x���g�^�C�v
	u16 event_flag;				///<�C�x���g�t���O
	u16 event_id;				///<�C�x���gID
	u16 dir_head;				///<MMDL_H�w�����
	u16 dir_disp;				///<���݌����Ă������
	u16 dir_move;				///<���ݓ����Ă������
	u16 dir_disp_old;			///<�ߋ��̓����Ă�������
	u16 dir_move_old;			///<�ߋ��̓����Ă�������
	
	u16 param0;					///<�w�b�_�w��p�����^
	u16 param1;					///<�w�b�_�w��p�����^
	u16 param2;					///<�w�b�_�w��p�����^
	
	u16 acmd_code;				///<�A�j���[�V�����R�}���h�R�[�h
	u16 acmd_seq;				///<�A�j���[�V�����R�}���h�V�[�P���X
	u16 draw_status;			///<�`��X�e�[�^�X
	s16 move_limit_x;			///<X�����ړ�����
	s16 move_limit_z;			///<Z�����ړ�����
	s16 gx_init;				///<�����O���b�hX
	s16 gy_init;				///<�����O���b�hY
	s16 gz_init;				///<�����O���b�hZ
	s16 gx_old;					///<�ߋ��O���b�hX
	s16 gy_old;					///<�ߋ��O���b�hY
	s16 gz_old;					///<�ߋ��O���b�hZ
	s16 gx_now;					///<���݃O���b�hX
	s16 gy_now;					///<���݃O���b�hY
	s16 gz_now;					///<���݃O���b�hZ
	VecFx32 vec_pos_now;		///<���ݎ������W
	VecFx32 vec_draw_offs;		///<�\�����W�I�t�Z�b�g
	VecFx32 vec_draw_offs_outside;	///<�O���w��\�����W�I�t�Z�b�g
	VecFx32 vec_attr_offs;		///<�A�g���r���[�g�ɂ����W�I�t�Z�b�g
	u32 now_attr;				///<���݂̃}�b�v�A�g���r���[�g
	u32 old_attr;				///<�ߋ��̃}�b�v�A�g���r���[�g
	
	GFL_TCB *pTCB;				///<����֐�TCB*
	MMDLSYS *pMMdlSys;///<MMDLSYS*
	
	const MMDL_MOVE_PROC_LIST *move_proc_list; ///<����֐����X�g
	const MMDL_DRAW_PROC_LIST *draw_proc_list; ///<�`��֐����X�g
	
	u8 move_proc_work[MMDL_MOVE_WORK_SIZE];///����֐��p���[�N
	u8 move_sub_proc_work[MMDL_MOVE_SUB_WORK_SIZE];///����T�u�֐��p���[�N
	u8 move_cmd_proc_work[MMDL_MOVE_CMD_WORK_SIZE];///����R�}���h�p���[�N
	u8 draw_proc_work[MMDL_DRAW_WORK_SIZE];///�`��֐��p���[�N
};

#define MMDL_SIZE (sizeof(MMDL)) ///<MMDL�T�C�Y 224

//--------------------------------------------------------------
///	MMDL�|�C���^�z��
//--------------------------------------------------------------
typedef struct{
  int count;  //�i�[���ꂽ�L���ȃ|�C���^��
  MMDL* mmdl_parray[MMDL_POST_LAYER_MAX];
}MMDL_PARRAY;

//--------------------------------------------------------------
///	MMDL_SAVEWORK�\���� size 80
//--------------------------------------------------------------
typedef struct
{
	u32 status_bit;			///<�X�e�[�^�X�r�b�g
	u32 move_bit;			///<����r�b�g
	
	u8 obj_id;        ///<OBJ ID
	u8 move_code;       ///<����R�[�h
	s8 move_limit_x;		///<X�����ړ�����
	s8 move_limit_z;		///<Z�����ړ�����
	
	s8 dir_head;			///<MMDL_H�w�����
	s8 dir_disp;			///<���݌����Ă������
	s8 dir_move;			///<���ݓ����Ă������
	u8 dummy;				///<�_�~�[
	
	u16 zone_id;			///<�]�[�� ID
	u16 obj_code;			///<OBJ�R�[�h
	u16 event_type;			///<�C�x���g�^�C�v
	u16 event_flag;			///<�C�x���g�t���O
	u16 event_id;			///<�C�x���gID
  
	u16 param0;				///<�w�b�_�w��p�����^
	u16 param1;				///<�w�b�_�w��p�����^
	u16 param2;				///<�w�b�_�w��p�����^
  
	s16 gx_init;			///<�����O���b�hX
	s16 gy_init;			///<�����O���b�hY
	s16 gz_init;			///<�����O���b�hZ
	s16 gx_now;				///<���݃O���b�hX
	s16 gy_now;				///<���݃O���b�hY
	s16 gz_now;				///<���݃O���b�hZ
  
	fx32 fx32_y;			///<fx32�^�̍����l
  
	u8 move_proc_work[MMDL_MOVE_WORK_SIZE];///<����֐��p���[�N
	u8 move_sub_proc_work[MMDL_MOVE_SUB_WORK_SIZE];///<����T�u�֐��p���[�N
}MMDL_SAVEWORK;

//--------------------------------------------------------------
///	MMDL_SAVEDATA�\����
//--------------------------------------------------------------
struct _TAG_MMDL_SAVEDATA
{
	MMDL_SAVEWORK SaveWorkBuf[MMDL_SAVEMMDL_MAX];
};

//--------------------------------------------------------------
/// MMDL_ROCKPOS
//--------------------------------------------------------------
struct _TAG_MMDL_ROCKPOS
{
  VecFx32 pos;
};

//======================================================================
//	proto
//======================================================================
//MMDLSYS �v���Z�X
static void MMdlSys_DeleteProc( MMDLSYS *fos );

//MMDL �ǉ��A�폜
static void MMdl_SetHeaderBefore(
	MMDL * mmdl, const MMDL_HEADER *head, void *sys );
static void MMdl_SetHeaderAfter(
	MMDL * mmdl, const MMDL_HEADER *head, void *sys );
static void MMdl_SetHeaderPos(MMDL *mmdl,const MMDL_HEADER *head);
static void MMdl_InitWork( MMDL * mmdl, MMDLSYS *sys );
static void MMdl_InitCallMoveProcWork( MMDL * mmdl );
static void MMdl_InitMoveWork( const MMDLSYS *fos, MMDL * mmdl );
static void MMdl_InitMoveProc( const MMDLSYS *fos, MMDL * mmdl );
static void MMdl_UpdateMove( MMDL * mmdl );
#if 0
static void MMdlSys_CheckSetInitMoveWork( MMDLSYS *fos );
static void MMdlSys_CheckSetInitDrawWork( MMDLSYS *fos );
#endif

//MMDL ����֐�
static void MMdl_TCB_MoveProc( GFL_TCB * tcb, void *work );
static void MMdl_TCB_DrawProc( MMDL * mmdl );

//MMDL_SAVEDATA
static void MMdl_SaveData_SaveMMdl(
	const MMDL *mmdl, MMDL_SAVEWORK *save );
static void MMdl_SaveData_LoadMMdl(
	MMDL *mmdl, const MMDL_SAVEWORK *save, MMDLSYS *fos );

//MMDLSYS �ݒ�A�Q��
static void MMdlSys_OnStatusBit(
	MMDLSYS *mmdlsys, MMDLSYS_STABIT bit );
static void MMdlSys_OffStatusBit(
	MMDLSYS *mmdlsys, MMDLSYS_STABIT bit );
static void MMdlSys_IncrementOBJCount( MMDLSYS *mmdlsys );
static void MMdlSys_DecrementOBJCount( MMDLSYS *mmdlsys );

//MMDLSYS �c�[��
static MMDL * MMdlSys_SearchSpaceMMdl( const MMDLSYS *sys );
static MMDL * MMdlSys_SearchAlies(
	const MMDLSYS *fos, int obj_id, int zone_id );

//MMDL �c�[��
static void MMdl_AddTCB( MMDL *mmdl, const MMDLSYS *sys );
static void MMdl_DeleteTCB( MMDL *mmdl );
static void MMdl_InitDrawWork( MMDL *mmdl );
static void MMdl_InitCallDrawProcWork( MMDL * mmdl );
static void MMdl_InitDrawEffectFlag( MMDL * mmdl );
static void MMdl_ClearWork( MMDL *mmdl );
static int MMdl_CheckHeaderAlies(
		const MMDL *mmdl, int h_zone_id, int max,
		const MMDL_HEADER *head );
static MMDL * MMdl_SearchOBJIDZoneID(
		const MMDLSYS *fos, int obj_id, int zone_id );
static void MMdl_InitDrawStatus( MMDL * mmdl );
static void MMdl_SetDrawDeleteStatus( MMDL * mmdl );
static void MMdl_ChangeAliesOBJ(
	MMDL *mmdl, const MMDL_HEADER *head, int zone_id );
static void MMdl_ChangeOBJAlies(
	MMDL * mmdl, int zone_id, const MMDL_HEADER *head );

//OBJCODE_PARAM
static void MMdlSys_InitOBJCodeParam( MMDLSYS *mmdlsys );
static void MMdlSys_DeleteOBJCodeParam( MMDLSYS *mmdlsys );

//parts
static u16 WorkOBJCode_GetOBJCode( void *fsys, int code );
static u16 OBJCode_GetDataNumber( u16 code );
static const MMDL_MOVE_PROC_LIST * MoveProcList_GetList( u16 code );
static const MMDL_DRAW_PROC_LIST * DrawProcList_GetList(
		MMDL_DRAWPROCNO no );
static BOOL MMdlHeader_CheckAlies( const MMDL_HEADER *head );
static int MMdlHeader_GetAliesZoneID( const MMDL_HEADER *head );
static BOOL MMdlSys_CheckEventFlag( EVENTWORK *evwork, u16 flag_no );

//MMDL_ROCKPOS
static BOOL mmdl_rockpos_CheckSetPos( const MMDL_ROCKPOS *rockpos );
static int rockpos_GetPosNumber( const u16 zone_id, const u16 obj_id );
static int mmdl_rockpos_GetPosNumber( const MMDL *mmdl );
static BOOL mmdl_rockpos_CheckPos( const MMDL *mmdl );
static BOOL mmdl_rockpos_GetPos( const MMDL *mmdl, VecFx32 *pos );

//======================================================================
//	�t�B�[���h���샂�f���@�V�X�e��
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS �V�X�e���쐬
 * @param	heapID	HEAPID
 * @param	max	���샂�f���ő吔
 * @param rockpos MMDL_ROCKPOS ���͊₪�Q�ƁA�ۑ�����f�[�^
 * @retval	MMDLSYS* �쐬���ꂽMMDLSYS*
 */
//--------------------------------------------------------------
MMDLSYS * MMDLSYS_CreateSystem(
    HEAPID heapID, u32 max, MMDL_ROCKPOS *rockpos )
{
	MMDLSYS *fos;
	fos = GFL_HEAP_AllocClearMemory( heapID, MMDLSYS_SIZE );
	fos->pMMdlBuf = GFL_HEAP_AllocClearMemory( heapID, max*MMDL_SIZE );
	fos->mmdl_max = max;
	fos->sysHeapID = heapID;
  fos->rockpos = rockpos;
	return( fos );
}

//--------------------------------------------------------------
/**
 * MMDLSYS �V�X�e���J��
 * @param	fos	MMDLSYS*
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDLSYS_FreeSystem( MMDLSYS *fos )
{
	GFL_HEAP_FreeMemory( fos->pMMdlBuf );
	GFL_HEAP_FreeMemory( fos );
}

//======================================================================
//	�t�B�[���h���샂�f���@�V�X�e���@�v���Z�X
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS �V�X�e�� ����v���Z�X�Z�b�g�A�b�v
 * @param	fos	MMDLSYS*
 * @param	heapID	�v���Z�X�pHEAPID
 * @param	pG3DMapper FLDMAPPER
 * @param	pNOGRIDMapper FLDNOGRID_MAPPER
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDLSYS_SetupProc(
	MMDLSYS *fos, HEAPID heapID, const FLDMAPPER *pG3DMapper, FLDNOGRID_MAPPER* pNOGRIDMapper )
{
	fos->heapID = heapID;
	fos->pG3DMapper = pG3DMapper;
	
	MMdlSys_InitOBJCodeParam( fos );
	
	fos->pTCBSysWork = GFL_HEAP_AllocMemory(
		heapID, GFL_TCB_CalcSystemWorkSize(fos->mmdl_max) );
	fos->pTCBSys = GFL_TCB_Init( fos->mmdl_max, fos->pTCBSysWork );

  fos->arcH_res = GFL_ARC_OpenDataHandle( ARCID_MMDL_RES, heapID );
  
  // �m�[�O���b�h�ړ��ݒ�
	fos->pNOGRIDMapper = pNOGRIDMapper;
	MMdlSys_OnStatusBit( fos, MMDLSYS_STABIT_MOVE_INIT_COMP );
}

//--------------------------------------------------------------
/**
 * MMDLSYS �V�X�e���@����v���Z�X�폜
 * @param	fos		MMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdlSys_DeleteProc( MMDLSYS *fos )
{
	MMdlSys_DeleteOBJCodeParam( fos );
	GFL_TCB_Exit( fos->pTCBSys );
	GFL_HEAP_FreeMemory( fos->pTCBSysWork );
  GFL_ARC_CloseDataHandle( fos->arcH_res );
	MMdlSys_OffStatusBit( fos, MMDLSYS_STABIT_MOVE_INIT_COMP );
}

//--------------------------------------------------------------
/**
 * MMDLSYS �S�Ẵv���Z�X�폜
 * @param	fos		MMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDLSYS_DeleteProc( MMDLSYS *fos )
{
//	MMDLSYS_DeleteMMdl( fos );
  
  { //������fldmmdl.c���ŏ�������B
    MMDLSYS_DeleteDraw( fos );
    fos->targetCameraAngleYaw = NULL;
  }
  
	MMdlSys_DeleteProc( fos );
}

//--------------------------------------------------------------
/**
 * MMDLSYS ���샂�f���X�V
 * @param	fos	MMDLSYS
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDLSYS_UpdateProc( MMDLSYS *fos )
{
	GFL_TCBSYS *tcbsys = fos->pTCBSys;
	GF_ASSERT( tcbsys != NULL );
	GFL_TCB_Main( tcbsys );
  MMDL_BLACTCONT_Update( fos );
}

//--------------------------------------------------------------
/**
 * MMDLSYS V�u�����N����
 * @param	fos	MMDLSYS
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDLSYS_VBlankProc( MMDLSYS *fos )
{
  if( fos->pBlActCont != NULL ){
    MMDL_BLACTCONT_ProcVBlank( fos );
  }
}

//======================================================================
//	�t�B�[���h���샂�f���@�V�X�e���@�`��v���Z�X
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS �V�X�e���@�`��v���Z�X�Z�b�g�A�b�v
 * @param	fos	MMDLSYS*
 * @param	heapID	�v���Z�X�pHEAPID
 * @param	pG3DMapper FLDMAPPER
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDLSYS_SetupDrawProc( MMDLSYS *fos, const u16 *angleYaw )
{
  fos->targetCameraAngleYaw = angleYaw;
	MMDLSYS_SetCompleteDrawInit( fos, TRUE );
}

//======================================================================
//	�t�B�[���h���샂�f���ǉ��A�폜
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS �t�B�[���h���샂�f����ǉ�
 * @param	fos			MMDLSYS *
 * @param	header		�ǉ��������Z�߂�MMDL_HEADER *
 * @param	zone_id		�]�[��ID
 * @retval	MMDL	�ǉ����ꂽMMDL *
 */
//--------------------------------------------------------------
MMDL * MMDLSYS_AddMMdl(
	MMDLSYS *fos, const MMDL_HEADER *header, int zone_id )
{
	MMDL *mmdl;
	MMDL_HEADER header_data = *header;
	const MMDL_HEADER *head = &header_data;
	
	mmdl = MMdlSys_SearchSpaceMMdl( fos );
	GF_ASSERT( mmdl != NULL );
	
	MMdl_SetHeaderBefore( mmdl, head, NULL );
	MMdl_InitWork( mmdl, fos );
	MMDL_SetZoneID( mmdl, zone_id );
  
  if( mmdl_rockpos_CheckPos(mmdl) == TRUE ){
    MMDL_OnStatusBit( mmdl,
        MMDL_STABIT_FELLOW_HIT_NON |
        MMDL_STABIT_HEIGHT_GET_OFF );
  }
  
	if( MMDLSYS_CheckStatusBit(fos,MMDLSYS_STABIT_MOVE_INIT_COMP) ){
		MMdl_InitMoveWork( fos, mmdl );
		MMdl_InitMoveProc( fos, mmdl );
	}
	
	if( MMDLSYS_CheckStatusBit(fos,MMDLSYS_STABIT_DRAW_INIT_COMP) ){
		MMdl_InitDrawWork( mmdl );
	}
	
	MMdlSys_IncrementOBJCount( (MMDLSYS*)MMDL_GetMMdlSys(mmdl) );

#ifdef DEBUG_ONLY_FOR_kagaya
  if( MMDL_GetOBJID(mmdl) == 1 && MMDL_GetOBJCode(mmdl) == WOMAN2 &&
      MMDL_GetMoveCode(mmdl) == MV_DMY ){
    KAGAYA_Printf( "�ǉ�����܂���\n" );
  }
#endif

	MMdl_SetHeaderAfter( mmdl, head, NULL );
	return( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDLSYS ���샂�f����ǉ��B�K�v�Œ���ȏ�񂩂�w�b�_�[���쐬���Ēǉ��B
 * ����Ȃ�����0�ŃN���A����Ă���B�K�v����Βǉ���Ɋe���Őݒ肷��B
 * @param fos MMDLSYS
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
MMDL * MMDLSYS_AddMMdlParam( MMDLSYS *fos,
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
  
  mmdl = MMDLSYS_AddMMdl( fos, &head, zone_id );
  return( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDLSYS �t�B�[���h���샂�f����ǉ��@����
 * @param	fos			MMDLSYS *
 * @param	header		�ǉ��������Z�߂�MMDL_HEADER *
 * @param	zone_id		�]�[��ID
 * @param	count		header�v�f��
 * @retval	nothing
 * @note �C�x���g�t���O�������Ă���w�b�_�[�͒ǉ����Ȃ��B
 */
//--------------------------------------------------------------
void MMDLSYS_SetMMdl( MMDLSYS *fos,
	const MMDL_HEADER *header, int zone_id, int count, EVENTWORK *eventWork )
{
	GF_ASSERT( count > 0 );
	GF_ASSERT( header != NULL );
	
	KAGAYA_Printf( "MMDLSYS_SetMMdl Count %d\n", count );
	
	do{
    if( MMdlHeader_CheckAlies(header) == TRUE ||
        MMdlSys_CheckEventFlag(eventWork,header->event_flag) == FALSE ){
		  MMDLSYS_AddMMdl( fos, header, zone_id );
    }
#ifdef PM_DEBUG
    else{
      OS_Printf( "ADD STOP MMDL OBJID=%d,EVENT FLAG=%xH\n",
          header->id, header->event_flag );
    }
#endif
		header++;
		count--;
	}while( count );
}

//--------------------------------------------------------------
/**
 * MMDLSYS �t�B�[���h���샂�f����ǉ��@�w�b�_�[���̎w��OBJID
 * @param	fos			MMDLSYS *
 * @param	header		�ǉ��������Z�߂�MMDL_HEADER *
 * @param	zone_id		�]�[��ID
 * @param	count		header�v�f��
 * @param id �ǉ�����OBJ ID
 * @retval	nothing
 * @note �C�x���g�t���O�������Ă���w�b�_�[�͒ǉ����Ȃ��B
 */
//--------------------------------------------------------------
MMDL * MMDLSYS_AddMMdlHeaderID( MMDLSYS *fos,
	const MMDL_HEADER *header, int zone_id, int count, EVENTWORK *eventWork,
  u16 objID )
{
  MMDL *mmdl;

	GF_ASSERT( count > 0 );
	GF_ASSERT( header != NULL );
	
	KAGAYA_Printf( "MMDLSYS_SetMMdlHeaderID Count %d\n", count );
	
	do{
    if( MMdlHeader_CheckAlies(header) == FALSE )
    {
      if( header->id == objID )
      {
        if( MMdlSys_CheckEventFlag(
              eventWork,header->event_flag) == FALSE )
        {
		      mmdl = MMDLSYS_AddMMdl( fos, header, zone_id );
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
 * @param	mmdl		�폜����MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_Delete( MMDL * mmdl )
{
	const MMDLSYS *fos;
	
	fos = MMDL_GetMMdlSys( mmdl );
	
	if( MMDLSYS_CheckCompleteDrawInit(fos) == TRUE ){
		MMDL_CallDrawDeleteProc( mmdl );
	}
	
	if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_MOVEPROC_INIT) ){
		MMDL_CallMoveDeleteProc( mmdl );
		MMdl_DeleteTCB( mmdl );
	}
	
	MMdlSys_DecrementOBJCount( (MMDLSYS*)(mmdl->pMMdlSys) );
	MMdl_ClearWork( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f�����폜�@�C�x���g�t���O OBJ��\���t���O��ON�ɁB
 * @param	mmdl		�폜����MMDL * 
 * @retval	nothing
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
 * @param	fos	MMDLSYS
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDLSYS_DeleteMMdl( const MMDLSYS *fos )
{
	u32 no = 0;
	MMDL *mmdl;
	
	while( MMDLSYS_SearchUseMMdl(fos,&mmdl,&no) ){
#if 0 //�}�b�v�J�ڂ���̌Ăяo���Ńn���O�A�b�v���邽�߂Ƃ肠�����Ώ�
		MMDL_Delete( mmdl );
#else
    if( MMDLSYS_CheckCompleteDrawInit(fos) == TRUE ){
      MMDL_CallDrawDeleteProc( mmdl );
    }
    
    if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_MOVEPROC_INIT) ){
      //MMDL_CallMoveDeleteProc( mmdl );
      //MMdl_DeleteTCB( mmdl );
    }
    
    MMdlSys_DecrementOBJCount( (MMDLSYS*)(mmdl->pMMdlSys) );
    MMdl_ClearWork( mmdl );
#endif
	}
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���@�w�b�_�[��񔽉f
 * @param	mmdl		�ݒ肷��MMDL * 
 * @param	head		���f�������Z�߂�MMDL_HEADER *
 * @param	fsys		FIELDSYS_WORK *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_SetHeaderBefore(
	MMDL * mmdl, const MMDL_HEADER *head, void *sys )
{
	MMDL_SetOBJID( mmdl, head->id );
	MMDL_SetOBJCode( mmdl, WorkOBJCode_GetOBJCode(sys,head->obj_code) );
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
	
  // ���W�^�C�v�ɂ��A�ʒu�̏��������@��ύX
  if( head->pos_type == MMDL_HEADER_POSTYPE_GRID )
  {
  	MMdl_SetHeaderPos( mmdl, head );
  }
  else
  {
    MMDL_SetRailHeaderBefore( mmdl, head );
  }
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���@�w�b�_�[��񔽉f  ���샂�f���̓o�^�����㏈��
 * @param	mmdl		�ݒ肷��MMDL * 
 * @param	head		���f�������Z�߂�MMDL_HEADER *
 * @param	fsys		FIELDSYS_WORK *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_SetHeaderAfter(
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
 * @param	mmdl		MMDL * 
 * @param	head		���f�������Z�߂�MMDL_HEADER *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_SetHeaderPos( MMDL *mmdl, const MMDL_HEADER *head )
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
	
	pos = SIZE_GRID_FX32( gridpos->y );		//pos�ݒ��fx32�^�ŗ���B
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
  
  IWASAWA_Printf( "Mmdl (%d,%d) h = %d\n",
      FX_Whole(vec.x), FX_Whole(vec.z), FX_Whole(vec.y) );
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���@���[�N������
 * @param	mmdl		MMDL * 
 * @param	sys			MMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_InitWork( MMDL * mmdl, MMDLSYS *sys )
{
	MMDL_OnStatusBit( mmdl,
		MMDL_STABIT_USE |				//�g�p��
		MMDL_STABIT_HEIGHT_GET_ERROR |	//�����擾���K�v
		MMDL_STABIT_ATTR_GET_ERROR );	//�A�g���r���[�g�擾���K�v
	
	if( MMDL_CheckAliesEventID(mmdl) == TRUE ){
		MMDL_SetStatusBitAlies( mmdl, TRUE );
	}
	
	mmdl->pMMdlSys = sys;
	MMDL_SetForceDirDisp( mmdl, MMDL_GetDirHeader(mmdl) );
	MMDL_SetDirMove( mmdl, MMDL_GetDirHeader(mmdl) );
	MMDL_FreeAcmd( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f�� ����֐�������
 * @param	mmdl		MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_InitCallMoveProcWork( MMDL * mmdl )
{
	mmdl->move_proc_list =
		MoveProcList_GetList( MMDL_GetMoveCode(mmdl) );
}

//--------------------------------------------------------------
/**
 * MMDL ���쏉�����ɍs�������Z��
 * @param	mmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_InitMoveWork( const MMDLSYS *fos, MMDL *mmdl )
{
	MMdl_InitCallMoveProcWork( mmdl );
	MMdl_AddTCB( mmdl, fos );
	MMDL_OnStatusBit( mmdl, MMDL_STABIT_MOVE_START );
}

//----------------------------------------------------------------------------
/**
 * �t�B�[���h���샂�f�� ���쏉����
 *	@param	fos
 *	@param	mmdl 
 */
//-----------------------------------------------------------------------------
static void MMdl_InitMoveProc( const MMDLSYS *fos, MMDL * mmdl )
{
  if( !MMDL_CheckStatusBit( mmdl, MMDL_STABIT_RAIL_MOVE ) )
  {
    MMDL_InitMoveProc( mmdl );
  }
  else
  {
    MMDL_InitRailMoveProc( mmdl );
  }
}

//----------------------------------------------------------------------------
/**
 * �t�B�[���h���샂�f������
 *
 *	@param	mmdl 
 */
//-----------------------------------------------------------------------------
static void MMdl_UpdateMove( MMDL * mmdl )
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
 * @param	fos	MMDLSYS*
 * @retval	nothing
 */
//--------------------------------------------------------------
#if 0
static void MMdlSys_CheckSetInitMoveWork( MMDLSYS *fos )
{
	u32 i = 0;
	MMDL *mmdl;
	
	while( MMDLSYS_SearchUseMMdl(fos,&mmdl,&i) == TRUE ){
		if( MMDL_CheckMoveBit(mmdl,	//�������֐��Ăяo���܂�
			MMDL_MOVEBIT_MOVEPROC_INIT) == 0 ){
			MMdl_InitMoveProc( mmdl );
		}
	}
}
#endif

//--------------------------------------------------------------
/**
 * MMDLSYS �`�揉�������s���Ă��Ȃ����샂�f���ɑ΂��ď������������Z�b�g
 * @param	fos	MMDLSYS*
 * @retval	nothing
 */
//--------------------------------------------------------------
#if 0
static void MMdlSys_CheckSetInitDrawWork( MMDLSYS *fos )
{
	u32 i = 0;
	MMDL *mmdl;
	
	while( MMDLSYS_SearchUseMMdl(fos,&mmdl,&i) == TRUE ){
		if( MMDL_CheckStatusBitCompletedDrawInit(mmdl) == FALSE ){
			MMdl_InitDrawWork( mmdl );
		}
	}
}
#endif

//======================================================================
//	MMDLSYS PUSH POP
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS ���샂�f�� �ޔ�
 * @param	mmdlsys	MMDLSYS
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDLSYS_Push( MMDLSYS *mmdlsys )
{
	u32 no = 0;
	MMDL *mmdl;
	
	#ifdef DEBUG_MMDL_PRINT
	if( MMDLSYS_CheckCompleteDrawInit(mmdlsys) == FALSE ){
		GF_ASSERT( 0 && "WARNING!! ���샂�f�� �`�斢������\n" );
	}
	#endif
	
	while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) == TRUE ){
		{ //���쏈���̑ޔ�
			MMdl_DeleteTCB( mmdl );
			MMDL_OnMoveBit( mmdl,
				MMDL_MOVEBIT_NEED_MOVEPROC_RECOVER );
		}
		
		{ //�`�揈���̑ޔ�
			MMDL_CallDrawPushProc( mmdl );
			MMDL_OnStatusBit( mmdl, MMDL_STABIT_DRAW_PUSH );
		}
	}
}

//--------------------------------------------------------------
/**
 * MMDLSYS ���샂�f�����A
 * @param	mmdlsys	MMDLSYS
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDLSYS_Pop( MMDLSYS *mmdlsys )
{
	u32 no = 0;
	MMDL *mmdl;
	
	while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) == TRUE ){
		{	//���쏈�����A
			MMdl_InitMoveWork( mmdlsys, mmdl ); //���[�N������
			
			if( MMDL_CheckMoveBit(mmdl,	//�������֐��Ăяo���܂�
				MMDL_MOVEBIT_MOVEPROC_INIT) == 0 ){
				MMdl_InitMoveProc( mmdlsys, mmdl );
			}
			
			if( MMDL_CheckMoveBit(mmdl, //�����֐��Ăяo�����K�v
				MMDL_MOVEBIT_NEED_MOVEPROC_RECOVER) ){
				MMDL_CallMovePopProc( mmdl );
				MMDL_OffMoveBit( mmdl,
					MMDL_MOVEBIT_NEED_MOVEPROC_RECOVER );
			}
		}
		
		{	//�`�揈�����A
			if( MMDL_CheckStatusBitCompletedDrawInit(mmdl) == FALSE ){
				MMdl_InitDrawWork( mmdl );
			}
			
			if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_DRAW_PUSH) ){
				MMDL_CallDrawPopProc( mmdl );
				MMDL_OffStatusBit( mmdl, MMDL_STABIT_DRAW_PUSH );
			}
		}

    { //���J�o���[
      MMdl_InitDrawEffectFlag( mmdl );
    }
	}
}

//======================================================================
//	MMDL_SAVEDATA
//======================================================================
//--------------------------------------------------------------
/**
 * MMDL_SAVEDATA �Z�[�u�f�[�^ �o�b�t�@�T�C�Y�擾
 * @param	nothing
 * @retval	u32	�T�C�Y
 */
//--------------------------------------------------------------
u32 MMDL_SAVEDATA_GetWorkSize( void )
{
	return( sizeof(MMDL_SAVEDATA) );
}

//--------------------------------------------------------------
/**
 * MMDL_SAVEDATA �Z�[�u�f�[�^ �o�b�t�@������
 * @param	p	MMDL_SAVEDATA
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SAVEDATA_Init( void *p )
{
	MI_CpuClear8( p, MMDL_SAVEDATA_GetWorkSize() );
}

//--------------------------------------------------------------
/**
 * MMDL_SAVEDATA ���샂�f���Z�[�u
 * @param	mmdlsys �Z�[�u����MMDLSYS
 * @param	savedata LDMMDL_SAVEDATA
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SAVEDATA_Save(
	MMDLSYS *mmdlsys, MMDL_SAVEDATA *savedata )
{
	u32 no = 0;
	MMDL *mmdl;
	MMDL_SAVEWORK *save;
  //�Z�[�u�O�Ƀo�b�t�@���N���A
  MMDL_SAVEDATA_Init(savedata);
  save = savedata->SaveWorkBuf;
	
	while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) == TRUE ){
		MMdl_SaveData_SaveMMdl( mmdl, save );
		save++;
	}
}

//--------------------------------------------------------------
/**
 * MMDL_SAVEDATA ���샂�f�����[�h
 * @param	mmdlsys	MMDLSYS
 * @param	save	���[�h����MMDL_SAVEWORK
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SAVEDATA_Load(
	MMDLSYS *mmdlsys, MMDL_SAVEDATA *savedata )
{
	u32 no = 0;
	MMDL *mmdl;
	MMDL_SAVEWORK *save = savedata->SaveWorkBuf;
	
	while( no < MMDL_SAVEMMDL_MAX ){
		if( (save->status_bit&MMDL_STABIT_USE) ){
			mmdl = MMdlSys_SearchSpaceMMdl( mmdlsys );
			MMdl_SaveData_LoadMMdl( mmdl, save, mmdlsys );
		}
		save++;
		no++;
	}

  mmdlsys->mmdl_count = no;
}

//--------------------------------------------------------------
/**
 * MMDL_SAVEDATA ���샂�f���@�Z�[�u
 * @param	fldmmdl		�Z�[�u����MMDL*
 * @param	save MMDL_SAVEWORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_SaveData_SaveMMdl(
	const MMDL *mmdl, MMDL_SAVEWORK *save )
{
	save->status_bit = MMDL_GetStatusBit( mmdl );
	save->move_bit = MMDL_GetMoveBit( mmdl );
	save->obj_id = MMDL_GetOBJID( mmdl );
	save->zone_id = MMDL_GetZoneID( mmdl );
	save->obj_code = MMDL_GetOBJCode( mmdl );
	save->move_code = MMDL_GetMoveCode( mmdl );
	save->event_type = MMDL_GetEventType( mmdl );
	save->event_flag = MMDL_GetEventFlag( mmdl );
	save->event_id = MMDL_GetEventID( mmdl );
	save->dir_head = MMDL_GetDirHeader( mmdl );
	save->dir_disp = MMDL_GetDirDisp( mmdl );
	save->dir_move = MMDL_GetDirMove( mmdl );
	save->param0 = MMDL_GetParam( mmdl, MMDL_PARAM_0 );
	save->param1 = MMDL_GetParam( mmdl, MMDL_PARAM_1 );
	save->param2 = MMDL_GetParam( mmdl, MMDL_PARAM_2 );
	save->move_limit_x = MMDL_GetMoveLimitX( mmdl );
	save->move_limit_z = MMDL_GetMoveLimitZ( mmdl );
	save->gx_init = MMDL_GetInitGridPosX( mmdl );
	save->gy_init = MMDL_GetInitGridPosY( mmdl );
	save->gz_init = MMDL_GetInitGridPosZ( mmdl );
	save->gx_now = MMDL_GetGridPosX( mmdl );
	save->gy_now = MMDL_GetGridPosY( mmdl );
	save->gz_now = MMDL_GetGridPosZ( mmdl );
	save->fx32_y = MMDL_GetVectorPosY( mmdl );
	
	MI_CpuCopy8( MMDL_GetMoveProcWork((MMDL*)mmdl),
		save->move_proc_work, MMDL_MOVE_WORK_SIZE );
	MI_CpuCopy8( MMDL_GetMoveSubProcWork((MMDL*)mmdl),
		save->move_sub_proc_work, MMDL_MOVE_SUB_WORK_SIZE );
}

//--------------------------------------------------------------
/**
 * MMDL_SAVEDATA ���샂�f���@���[�h
 * @param	fldmmdl		�Z�[�u����MMDL*
 * @param	save MMDL_SAVEWORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_SaveData_LoadMMdl(
	MMDL *mmdl, const MMDL_SAVEWORK *save, MMDLSYS *fos )
{
	MMdl_ClearWork( mmdl );

	mmdl->status_bit = save->status_bit;
	mmdl->move_bit = save->move_bit;
	MMDL_SetOBJID( mmdl, save->obj_id );
	MMDL_SetZoneID( mmdl, save->zone_id );
	MMDL_SetOBJCode( mmdl, save->obj_code ); 
	MMDL_SetMoveCode( mmdl, save->move_code );
	MMDL_SetEventType( mmdl, save->event_type );
	MMDL_SetEventFlag( mmdl, save->event_flag );
	MMDL_SetEventID( mmdl, save->event_id );
	mmdl->dir_head = save->dir_head;
	MMDL_SetForceDirDisp( mmdl, save->dir_disp );
	MMDL_SetDirMove( mmdl, save->dir_move );
	MMDL_SetParam( mmdl, save->param0, MMDL_PARAM_0 );
	MMDL_SetParam( mmdl, save->param1, MMDL_PARAM_1 );
	MMDL_SetParam( mmdl, save->param2, MMDL_PARAM_2 );
	MMDL_SetMoveLimitX( mmdl, save->move_limit_x );
	MMDL_SetMoveLimitZ( mmdl, save->move_limit_z );
	MMDL_SetInitGridPosX( mmdl, save->gx_init );
	MMDL_SetInitGridPosY( mmdl, save->gy_init );
	MMDL_SetInitGridPosZ( mmdl, save->gz_init );
	MMDL_SetGridPosX( mmdl, save->gx_now );
	MMDL_SetGridPosY( mmdl, save->gy_now );
	MMDL_SetGridPosZ( mmdl, save->gz_now );
	
	MI_CpuCopy8( save->move_proc_work,
		MMDL_GetMoveProcWork((MMDL*)mmdl), MMDL_MOVE_WORK_SIZE );
	MI_CpuCopy8( save->move_sub_proc_work,
		MMDL_GetMoveSubProcWork((MMDL*)mmdl), MMDL_MOVE_SUB_WORK_SIZE );
	
	mmdl->pMMdlSys = fos;
	
	{ //���W���A
		s16 grid;
		VecFx32 pos = {0,0,0};
		
		grid = MMDL_GetGridPosX( mmdl );
		MMDL_SetOldGridPosX( mmdl, grid );
		pos.x = GRID_SIZE_FX32( grid ) + MMDL_VEC_X_GRID_OFFS_FX32;
		
		grid = MMDL_GetGridPosY( mmdl );
		MMDL_SetOldGridPosY( mmdl, grid );
		pos.y = save->fx32_y; //�Z�[�u���̍�����M�p����
	
		grid = MMDL_GetGridPosZ( mmdl );
		MMDL_SetOldGridPosZ( mmdl, grid );
		pos.z = GRID_SIZE_FX32( grid ) + MMDL_VEC_Z_GRID_OFFS_FX32;
	
		MMDL_SetVectorPos( mmdl, &pos );
	}
  
	{ //�X�e�[�^�X�r�b�g���A
		MMDL_OnStatusBit( mmdl,
			MMDL_STABIT_USE |
//			MMDL_STABIT_HEIGHT_GET_NEED | //�Z�[�u���̍�����M�p����
			MMDL_STABIT_MOVE_START );
		
		MMDL_OffStatusBit( mmdl,
			MMDL_STABIT_PAUSE_MOVE |
			MMDL_STABIT_VANISH |
			MMDL_STABIT_DRAW_PROC_INIT_COMP |
			MMDL_STABIT_JUMP_START |
			MMDL_STABIT_JUMP_END |
			MMDL_STABIT_MOVE_END |
//			MMDL_STABIT_FELLOW_HIT_NON | //change wb
			MMDL_STABIT_TALK_OFF |
			MMDL_STABIT_DRAW_PUSH |
			MMDL_STABIT_BLACT_ADD_PRAC );
//			MMDL_STABIT_HEIGHT_GET_OFF ); //change wb
		
		MMDL_OffStatusBit( mmdl,
			MMDL_STABIT_SHADOW_SET |
			MMDL_STABIT_SHADOW_VANISH |
			MMDL_STABIT_EFFSET_SHOAL	 |
			MMDL_STABIT_REFLECT_SET );
		
		if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_MOVEPROC_INIT) ){
			MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_NEED_MOVEPROC_RECOVER );
		}
	}
}


//======================================================================
//	MMDL ����֐�
//======================================================================
//--------------------------------------------------------------
/**
 * MMDL ���암�����s
 * @param	mmdl	MMDL*
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_UpdateMoveProc( MMDL *mmdl )
{
	MMdl_UpdateMove( mmdl );
	
	if( MMDL_CheckStatusBitUse(mmdl) == TRUE ){
		MMdl_TCB_DrawProc( mmdl );
	}
}

//--------------------------------------------------------------
/**
 * MMDL TCB ����֐�
 * @param	tcb		GFL_TCB *
 * @param	work	tcb work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_TCB_MoveProc( GFL_TCB * tcb, void *work )
{
	MMDL *mmdl = (MMDL *)work;
	MMDL_UpdateMoveProc( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDL TCB ����֐�����Ă΂��`��֐�
 * @param	mmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_TCB_DrawProc( MMDL * mmdl )
{
	const MMDLSYS *fos = MMDL_GetMMdlSys(mmdl);
	
	if( MMDLSYS_CheckCompleteDrawInit(fos) == TRUE ){
		MMDL_UpdateDraw( mmdl );
	}
}

//======================================================================
//	MMDL �A�j���[�V�����R�}���h
//======================================================================
//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h���\���`�F�b�N
 * @param	mmdl		�ΏۂƂȂ�MMDL * 
 * @retval	int			TRUE=�\�BFALSE=����
 */
//--------------------------------------------------------------
BOOL MMDL_CheckPossibleAcmd( const MMDL * mmdl )
{
	if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_USE) == 0 ){
		return( FALSE );
	}
	
	if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_MOVE) ){
		return( FALSE );
	}
	
	if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_ACMD) &&
		MMDL_CheckStatusBit(mmdl,MMDL_STABIT_ACMD_END) == 0 ){
		return( FALSE );
	}
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h�Z�b�g
 * @param	mmdl		�ΏۂƂȂ�MMDL * 
 * @param	code		���s����R�[�h�BAC_DIR_U��
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetAcmd( MMDL * mmdl, u16 code )
{
	GF_ASSERT( code < ACMD_MAX );
	MMDL_SetAcmdCode( mmdl, code );
	MMDL_SetAcmdSeq( mmdl, 0 );
	MMDL_OnStatusBit( mmdl, MMDL_STABIT_ACMD );
	MMDL_OffStatusBit( mmdl, MMDL_STABIT_ACMD_END );
}

//--------------------------------------------------------------
/**
 * �R�}���h�Z�b�g
 * @param	mmdl		�ΏۂƂȂ�MMDL * 
 * @param	code		���s����R�[�h�BAC_DIR_U��
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetLocalAcmd( MMDL * mmdl, u16 code )
{
	MMDL_SetAcmdCode( mmdl, code );
	MMDL_SetAcmdSeq( mmdl, 0 );
	MMDL_OffStatusBit( mmdl, MMDL_STABIT_ACMD_END );
}

//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h�I���`�F�b�N�B
 * @param	mmdl		�ΏۂƂȂ�MMDL * 
 * @retval	int			TRUE=�I��
 */
//--------------------------------------------------------------
BOOL MMDL_CheckEndAcmd( const MMDL * mmdl )
{
	if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_ACMD) == 0 ){
		return( TRUE );
	}
	
	if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_ACMD_END) == 0 ){
		return( FALSE );
	}
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h�I���`�F�b�N�ƊJ���B
 * �A�j���[�V�����R�}���h���I�����Ă��Ȃ��ꍇ�͊J������Ȃ��B
 * @param	mmdl		�ΏۂƂȂ�MMDL * 
 * @retval	BOOL	TRUE=�I�����Ă���B
 */
//--------------------------------------------------------------
BOOL MMDL_EndAcmd( MMDL * mmdl )
{
	if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_ACMD) == 0 ){
		return( TRUE );
	}
	
	if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_ACMD_END) == 0 ){
		return( FALSE );
	}
	
	MMDL_OffStatusBit(
		mmdl, MMDL_STABIT_ACMD|MMDL_STABIT_ACMD_END );
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h�J���B
 * �A�j���[�V�����R�}���h���I�����Ă��Ȃ��Ƃ������J���B
 * @param	mmdl		�ΏۂƂȂ�MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_FreeAcmd( MMDL * mmdl )
{
	MMDL_OffStatusBit( mmdl, MMDL_STABIT_ACMD );
	MMDL_OnStatusBit( mmdl, MMDL_STABIT_ACMD_END ); //���[�J���R�}���h�t���O
	MMDL_SetAcmdCode( mmdl, ACMD_NOT );
	MMDL_SetAcmdSeq( mmdl, 0 );
}

//======================================================================
//	MMDLSYS �p�����^�ݒ�A�Q��
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS �X�e�[�^�X�r�b�g�`�F�b�N
 * @param	mmdlsys	MMDLSYS*
 * @param	bit	MMDLSYS_STABIT
 * @retval	u32	(status bit & bit)
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
 * @param	mmdlsys	MMDLSYS*
 * @param	bit	MMDLSYS_STABIT
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdlSys_OnStatusBit(
	MMDLSYS *mmdlsys, MMDLSYS_STABIT bit )
{
	mmdlsys->status_bit |= bit;
}

//--------------------------------------------------------------
/**
 * MMDLSYS �X�e�[�^�X�r�b�g OFF
 * @param	mmdlsys	MMDLSYS*
 * @param	bit	MMDLSYS_STABIT
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdlSys_OffStatusBit(
	MMDLSYS *mmdlsys, MMDLSYS_STABIT bit )
{
	mmdlsys->status_bit &= ~bit;
}

//--------------------------------------------------------------
/**
 * mmdlSYS ���샂�f���ő吔���擾
 * @param	mmdlsys	MMDLSYS*
 * @retval	u16	�ő吔
 */
//--------------------------------------------------------------
u16 MMDLSYS_GetMMdlMax( const MMDLSYS *mmdlsys )
{
	return( mmdlsys->mmdl_max );
}

//--------------------------------------------------------------
/**
 * mmdlSYS ���ݔ������Ă��铮�샂�f���̐����擾
 * @param	mmdlsys	MMDLSYS*
 * @retval	u16	������
 */
//--------------------------------------------------------------
u16 MMDLSYS_GetMMdlCount( const MMDLSYS *mmdlsys )
{
	return( mmdlsys->mmdl_count );
}

//--------------------------------------------------------------
/**
 * MMDLSYS TCB�v���C�I���e�B���擾
 * @param	mmdlsys	MMDLSYS*
 * @retval	u16	TCB�v���C�I���e�B
 */
//--------------------------------------------------------------
u16 MMDLSYS_GetTCBPriority( const MMDLSYS *mmdlsys )
{
	return( mmdlsys->tcb_pri );
}

//--------------------------------------------------------------
/**
 * MMDLSYS HEAPID�擾
 * @param	mmdlsys	MMDLSYS*
 * @retval	HEAPID HEAPID
 */
//--------------------------------------------------------------
HEAPID MMDLSYS_GetHeapID( const MMDLSYS *mmdlsys )
{
	return( mmdlsys->heapID );
}

//--------------------------------------------------------------
/**
 * MMDLSYS ���ݔ������Ă���OBJ����1����
 * @param	mmdlsys	MMDLSYS*
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdlSys_IncrementOBJCount( MMDLSYS *mmdlsys )
{
	mmdlsys->mmdl_count++;
}

//--------------------------------------------------------------
/**
 * MMDLSYS ���ݔ������Ă���OBJ����1����
 * @param	mmdlsys	MMDLSYS*
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdlSys_DecrementOBJCount( MMDLSYS *mmdlsys )
{
	mmdlsys->mmdl_count--;
	GF_ASSERT( mmdlsys->mmdl_count >= 0 );
}

//--------------------------------------------------------------
/**
 * MMDLSYS GFL_TCBSYS*�擾
 * @param	fos	MMDLSYS*
 * @retval	GFL_TCBSYS*
 */
//--------------------------------------------------------------
GFL_TCBSYS * MMDLSYS_GetTCBSYS( MMDLSYS *fos )
{
	return( fos->pTCBSys );
}

//--------------------------------------------------------------
/**
 * MMDLSYS ���\�[�X�A�[�J�C�u�n���h���擾
 * @param fos MMDLSYS*
 * @retval ARCHANDLE*
 */
//--------------------------------------------------------------
ARCHANDLE * MMDLSYS_GetResArcHandle( MMDLSYS *fos )
{
  return( fos->arcH_res );
}

//--------------------------------------------------------------
/**
 * MMDLSYS MMDL_BLACTCONT�Z�b�g
 * @param	mmdlsys	MMDLSYS
 * @retval	nothing
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
 * @param	mmdlsys MMDLSYS
 * @retval	MMDL_BLACTCONT*
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
 * @param	mmdlsys	MMDLSYS
 * @param objcont MMDL_G3DOBJCONT*
 * @retval	nothing
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
 * @param	mmdlsys	MMDLSYS
 * @param objcont MMDL_G3DOBJCONT*
 * @retval	nothing
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
 * @param	mmdlsys	MMDLSYS
 * @retval	FLDMAPPER* FLDMAPPER*
 */
//--------------------------------------------------------------
const FLDMAPPER * MMDLSYS_GetG3DMapper( const MMDLSYS *fos )
{
	GF_ASSERT( fos->pG3DMapper != NULL);
	return( fos->pG3DMapper );
}

//--------------------------------------------------------------
/**
 * MMDLSYS FIELDMAP_WORK�Z�b�g
 * @param mmdlsys MMDLSYS
 * @param fieldMapWork FIELDMAP_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
void MMDLSYS_SetFieldMapWork(
    MMDLSYS *fos, void *fieldMapWork )
{
  fos->fieldMapWork = fieldMapWork;
}

//--------------------------------------------------------------
/**
 * MMDLSYS FIELDMAP_WORK�擾
 * @param mmdlsys MMDLSYS
 * @retval fieldMapWork FIELDMAP_WORK
 */
//--------------------------------------------------------------
void * MMDLSYS_GetFieldMapWork( MMDLSYS *fos )
{
  GF_ASSERT( fos->fieldMapWork != NULL );
  return( fos->fieldMapWork );
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
//	MMDL�@�p�����^�Q�ƁA�ݒ�
//======================================================================
//--------------------------------------------------------------
/**
 * MMDL �X�e�[�^�X�r�b�gON
 * @param	mmdl	mmdl
 * @param	bit		MMDL_STABIT
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_OnStatusBit( MMDL *mmdl, MMDL_STABIT bit )
{
	mmdl->status_bit |= bit;
}

//--------------------------------------------------------------
/**
 * MMDL �X�e�[�^�X�r�b�gOFF
 * @param	mmdl	mmdl
 * @param	bit		MMDL_STABIT
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_OffStatusBit( MMDL *mmdl, MMDL_STABIT bit )
{
	mmdl->status_bit &= ~bit;
}

//--------------------------------------------------------------
/**
 * MMDL �X�e�[�^�X�r�b�g�擾
 * @param	mmdl		MMDL * 
 * @retval	MMDL_STABIT �X�e�[�^�X�r�b�g
 */
//--------------------------------------------------------------
MMDL_STABIT MMDL_GetStatusBit( const MMDL * mmdl )
{
	return( mmdl->status_bit );
}

//--------------------------------------------------------------
/**
 * MMDL �X�e�[�^�X�r�b�g�`�F�b�N
 * @param	mmdl	MMDL*
 * @param	bit	MMDL_STABIT
 * @retval	u32	(status bit & bit)
 */
//--------------------------------------------------------------
u32 MMDL_CheckStatusBit( const MMDL *mmdl, MMDL_STABIT bit )
{
	return( (mmdl->status_bit&bit) );
}

//--------------------------------------------------------------
/**
 * MMDL ����r�b�g�@�擾
 * @param	mmdl	MMDL *
 * @retval	u32		����r�b�g
 */
//--------------------------------------------------------------
MMDL_MOVEBIT MMDL_GetMoveBit( const MMDL * mmdl )
{
	return( mmdl->move_bit );
}

//--------------------------------------------------------------
/**
 * MMDL ����r�b�g�@ON
 * @param	mmdl	MMDL *
 * @param	bit		���Ă�r�b�g MMDL_MOVEBIT_NON��
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_OnMoveBit( MMDL * mmdl, MMDL_MOVEBIT bit )
{
	mmdl->move_bit |= bit;
}

//--------------------------------------------------------------
/**
 * MMDL ����r�b�g�@OFF
 * @param	mmdl	MMDL *
 * @param	bit		���낷�r�b�g MMDL_MOVEBIT_NON��
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_OffMoveBit( MMDL * mmdl, MMDL_MOVEBIT bit )
{
	mmdl->move_bit &= ~bit;
}

//--------------------------------------------------------------
/**
 * MMDL ����r�b�g�@�`�F�b�N
 * @param	mmdl	MMDL *
 * @param	bit		�`�F�b�N����r�b�g MMDL_MOVEBIT_NON��
 * @retval	nothing
 */
//--------------------------------------------------------------
u32 MMDL_CheckMoveBit( const MMDL * mmdl, MMDL_MOVEBIT bit )
{
	return( (mmdl->move_bit & bit) );
}

//--------------------------------------------------------------
/**
 * MMDL OBJ ID�Z�b�g
 * @param	mmdl	MMDL * 
 * @param	id		obj id
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetOBJID( MMDL * mmdl, u16 obj_id )
{
	mmdl->obj_id = obj_id;
}

//--------------------------------------------------------------
/**
 * MMDL OBJ ID�擾
 * @param	mmdl	MMDL *
 * @retval	u16		OBJ ID
 */
//--------------------------------------------------------------
u16 MMDL_GetOBJID( const MMDL * mmdl )
{
	return( mmdl->obj_id );
}

//--------------------------------------------------------------
/**
 * MMDL ZONE ID�Z�b�g
 * @param	mmdl	MMDL *
 * @param	zone_id	�]�[��ID
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetZoneID( MMDL * mmdl, u16 zone_id )
{
	mmdl->zone_id = zone_id;
}

//--------------------------------------------------------------
/**
 * MMDL ZONE ID�擾
 * @param	mmdl	MMDL *
 * @retval	int		ZONE ID
 */
//--------------------------------------------------------------
u16 MMDL_GetZoneID( const MMDL * mmdl )
{
	return( mmdl->zone_id );
}

//--------------------------------------------------------------
/**
 * MMDL OBJ�R�[�h�Z�b�g
 * @param	mmdl			MMDL * 
 * @param	code			�Z�b�g����R�[�h
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetOBJCode( MMDL * mmdl, u16 code )
{
	mmdl->obj_code = code;
}

//--------------------------------------------------------------
/**
 * MMDL OBJ�R�[�h�擾
 * @param	mmdl			MMDL * 
 * @retval	u16				OBJ�R�[�h
 */
//--------------------------------------------------------------
u16 MMDL_GetOBJCode( const MMDL * mmdl )
{
	return( mmdl->obj_code );
}

//--------------------------------------------------------------
/**
 * MMDL ����R�[�h�Z�b�g
 * @param	mmdl			MMDL * 
 * @param	code			����R�[�h
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetMoveCode( MMDL * mmdl, u16 code )
{
	mmdl->move_code = code;
}

//--------------------------------------------------------------
/**
 * MMDL ����R�[�h�擾
 * @param	mmdl			MMDL * 
 * @retval	u32				����R�[�h
 */
//--------------------------------------------------------------
u16 MMDL_GetMoveCode( const MMDL * mmdl )
{
	return( mmdl->move_code );
}

//--------------------------------------------------------------
/**
 * MMDL �C�x���g�^�C�v�Z�b�g
 * @param	mmdl		MMDL * 
 * @param	type		Event Type
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetEventType( MMDL * mmdl, u16 type )
{
	mmdl->event_type = type;
}

//--------------------------------------------------------------
/**
 * MMDL �C�x���g�^�C�v�擾
 * @param	mmdl		MMDL * 
 * @retval	u32			Event Type
 */
//--------------------------------------------------------------
u16 MMDL_GetEventType( const MMDL * mmdl )
{
	return( mmdl->event_type );
}

//--------------------------------------------------------------
/**
 * MMDL �C�x���g�t���O�Z�b�g
 * @param	mmdl		MMDL * 
 * @param	flag		Event Flag
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetEventFlag( MMDL * mmdl, u16 flag )
{
	mmdl->event_flag = flag;
}

//--------------------------------------------------------------
/**
 * MMDL �C�x���g�t���O�擾
 * @param	mmdl		MMDL * 
 * @retval	u16			Event Flag
 */
//--------------------------------------------------------------
u16 MMDL_GetEventFlag( const MMDL * mmdl )
{
	return( mmdl->event_flag );
}

//--------------------------------------------------------------
/**
 * MMDL �C�x���gID�Z�b�g
 * @param	mmdl		MMDL *
 * @param	id			Event ID
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetEventID( MMDL * mmdl, u16 id )
{
	mmdl->event_id = id;
}

//--------------------------------------------------------------
/**
 * MMDL �C�x���gID�擾
 * @param	mmdl		MMDL * 
 * @retval	u16			Event ID
 */
//--------------------------------------------------------------
u16 MMDL_GetEventID( const MMDL * mmdl )
{
	return( mmdl->event_id );
}

//--------------------------------------------------------------
/**
 * MMDL �C�x���gID���G�C���A�X���ǂ����`�F�b�N
 * @param	mmdl		MMDL * 
 * @retval	int			TRUE=�G�C���A�XID�ł��� FALSE=�Ⴄ
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
 * @param	mmdl		MMDL * 
 * @retval	u32			DIR_UP��
 */
//--------------------------------------------------------------
u32 MMDL_GetDirHeader( const MMDL * mmdl )
{
	return( mmdl->dir_head );
}

//--------------------------------------------------------------
/**
 * MMDL �\�������Z�b�g�@����
 * @param	mmdl			MMDL * 
 * @param	dir				DIR_UP��
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetForceDirDisp( MMDL * mmdl, u16 dir )
{
#ifdef DEBUG_ONLY_FOR_kagaya
  if( MMDL_GetOBJID(mmdl) != MMDL_ID_PLAYER ){
    if( MMDL_GetMoveCode(mmdl) == MV_DMY ){
      OS_Printf( "���샂�f���@�����Z�b�g ID %d DIR %d\n",
      MMDL_GetOBJID(mmdl), dir );
    }
  }
#endif

	mmdl->dir_disp_old = mmdl->dir_disp;
	mmdl->dir_disp = dir;
}

//--------------------------------------------------------------
/**
 * MMDL �\�������Z�b�g
 * @param	mmdl			MMDL * 
 * @param	dir				DIR_UP��
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetDirDisp( MMDL * mmdl, u16 dir )
{
#ifdef DEBUG_ONLY_FOR_kagaya
  if( MMDL_GetOBJID(mmdl) != MMDL_ID_PLAYER ){
    if( MMDL_GetMoveCode(mmdl) == MV_DMY ){
      OS_Printf( "���샂�f���@�����Z�b�g ID %d DIR %d\n",
      MMDL_GetOBJID(mmdl), dir );
    }
  }
#endif

	if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_PAUSE_DIR) == 0 ){
		mmdl->dir_disp_old = mmdl->dir_disp;
		mmdl->dir_disp = dir;
	}
}

//--------------------------------------------------------------
/**
 * MMDL �\�������擾
 * @param	mmdl	MMDL * 
 * @retval	u16 	DIR_UP��
 */
//--------------------------------------------------------------
u16 MMDL_GetDirDisp( const MMDL * mmdl )
{
	return( mmdl->dir_disp );
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ��\�������擾
 * @param	mmdl			MMDL * 
 * @retval	dir				DIR_UP��
 */
//--------------------------------------------------------------
u16 MMDL_GetDirDispOld( const MMDL * mmdl )
{
	return( mmdl->dir_disp_old );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ������Z�b�g
 * @param	mmdl			MMDL * 
 * @param	dir				DIR_UP��
 * @retval	nothing
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
 * @param	mmdl			MMDL * 
 * @retval	u16		DIR_UP��
 */
//--------------------------------------------------------------
u16 MMDL_GetDirMove( const MMDL * mmdl )
{
	return( mmdl->dir_move );
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ��ړ������擾
 * @param	mmdl			MMDL * 
 * @retval	u16	DIR_UP��
 */
//--------------------------------------------------------------
u16 MMDL_GetDirMoveOld( const MMDL * mmdl )
{
	return( mmdl->dir_move_old );
}

//--------------------------------------------------------------
/**
 * MMDL �\���A�ړ������Z�b�g
 * @param	mmdl			MMDL * 
 * @param	dir				DIR_UP��
 * @retval	nothing
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
 * @param	mmdl	MMDL * 
 * @param	param	�p�����^
 * @param	no		�Z�b�g����p�����^�ԍ��@MMDL_PARAM_0��
 * @retval	nothing
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
 * @param	mmdl			MMDL *
 * @param	param			MMDL_PARAM_0��
 * @retval	u16 �p�����^
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
 * @param	mmdl			MMDL * 
 * @param	x				�ړ�����
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetMoveLimitX( MMDL * mmdl, s16 x )
{
	mmdl->move_limit_x = x;
}

//--------------------------------------------------------------
/**
 * MMDL �ړ�����X�擾
 * @param	mmdl		MMDL * 
 * @retval	s16			�ړ�����X
 */
//--------------------------------------------------------------
s16 MMDL_GetMoveLimitX( const MMDL * mmdl )
{
	return( mmdl->move_limit_x );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ�����Z�Z�b�g
 * @param	mmdl			MMDL * 
 * @param	z				�ړ�����
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetMoveLimitZ( MMDL * mmdl, s16 z )
{
	mmdl->move_limit_z = z;
}

//--------------------------------------------------------------
/**
 * MMDL �ړ�����Z�擾
 * @param	mmdl		MMDL * 
 * @retval	s16		�ړ�����z
 */
//--------------------------------------------------------------
s16 MMDL_GetMoveLimitZ( const MMDL * mmdl )
{
	return( mmdl->move_limit_z );
}

//--------------------------------------------------------------
/**
 * MMDL �`��X�e�[�^�X�Z�b�g
 * @param	mmdl		MMDL * 
 * @param	st			DRAW_STA_STOP��
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetDrawStatus( MMDL * mmdl, u16 st )
{
	mmdl->draw_status = st;
}

//--------------------------------------------------------------
/**
 * MMDL �`��X�e�[�^�X�擾
 * @param	mmdl		MMDL * 
 * @retval	u16			DRAW_STA_STOP��
 */
//--------------------------------------------------------------
u16 MMDL_GetDrawStatus( const MMDL * mmdl )
{
	return( mmdl->draw_status );
}

//--------------------------------------------------------------
/**
 * MMDL MMDLSYS *�擾
 * @param	mmdl			MMDL * 
 * @retval	MMDLSYS	MMDLSYS *
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
 * @param	mmdl	MMDL * 
 * @param	size	�K�v�ȃ��[�N�T�C�Y
 * @retval	void*	���������ꂽ���[�N
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
 * @param	mmdl	MMDL * 
 * @retval	void*	���[�N
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
 * @param	mmdl	MMDL * 
 * @param	size	�K�v�ȃ��[�N�T�C�Y
 * @retval	void*	���������ꂽ���[�N
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
 * @param	mmdl	MMDL * 
 * @retval	void*	���[�N*
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
 * @param	mmdl	MMDL * 
 * @param	size	�K�v�ȃ��[�N�T�C�Y
 * @retval	void*	���������ꂽ���[�N
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
 * @param	mmdl	MMDL *
 * @retval	void*	���[�N*
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
 * @param	mmdl	MMDL * 
 * @param	size	�K�v�ȃ��[�N�T�C�Y
 * @retval	void*	���������ꂽ���[�N
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
 * @param	mmdl	MMDL * 
 * @retval	void*	���[�N
 */
//--------------------------------------------------------------
void * MMDL_GetDrawProcWork( MMDL * mmdl )
{
	return( mmdl->draw_proc_work );
}

//--------------------------------------------------------------
/**
 * MMDL ���쏉�����֐����s
 * @param	mmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_CallMoveInitProc( MMDL * mmdl )
{
	GF_ASSERT( mmdl->move_proc_list );
	GF_ASSERT( mmdl->move_proc_list->init_proc );
	mmdl->move_proc_list->init_proc( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDL ����֐����s
 * @param	mmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_CallMoveProc( MMDL * mmdl )
{
	GF_ASSERT( mmdl->move_proc_list );
	GF_ASSERT( mmdl->move_proc_list->move_proc );
	mmdl->move_proc_list->move_proc( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDL �폜�֐����s
 * @param	mmdl	MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_CallMoveDeleteProc( MMDL * mmdl )
{
	GF_ASSERT( mmdl->move_proc_list );
	GF_ASSERT( mmdl->move_proc_list->delete_proc );
	mmdl->move_proc_list->delete_proc( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDL ���A�֐����s
 * @param	mmdl	MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_CallMovePopProc( MMDL * mmdl )
{
	GF_ASSERT( mmdl->move_proc_list );
	GF_ASSERT( mmdl->move_proc_list->recover_proc );
	mmdl->move_proc_list->recover_proc( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDL �`�揉�����֐����s
 * @param	mmdl	MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_CallDrawInitProc( MMDL * mmdl )
{
	GF_ASSERT( mmdl->draw_proc_list );
	GF_ASSERT( mmdl->draw_proc_list->init_proc );
	mmdl->draw_proc_list->init_proc( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDL �`��֐����s
 * @param	mmdl	MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_CallDrawProc( MMDL * mmdl )
{
	GF_ASSERT( mmdl->draw_proc_list );
	GF_ASSERT( mmdl->draw_proc_list->draw_proc );
	mmdl->draw_proc_list->draw_proc( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDL �`��폜�֐����s
 * @param	mmdl	MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_CallDrawDeleteProc( MMDL * mmdl )
{
	GF_ASSERT( mmdl->draw_proc_list );
	GF_ASSERT( mmdl->draw_proc_list->delete_proc );
	mmdl->draw_proc_list->delete_proc( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDL �`��ޔ��֐����s
 * @param	mmdl	MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_CallDrawPushProc( MMDL * mmdl )
{
	GF_ASSERT( mmdl->draw_proc_list );
	GF_ASSERT( mmdl->draw_proc_list->push_proc );
	mmdl->draw_proc_list->push_proc( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDL �`�敜�A�֐����s
 * @param	mmdl	MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_CallDrawPopProc( MMDL * mmdl )
{
	GF_ASSERT( mmdl->draw_proc_list );
	GF_ASSERT( mmdl->draw_proc_list->pop_proc );
	mmdl->draw_proc_list->pop_proc( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDL �`��擾�֐����s
 * @param	mmdl	MMDL*
 * @param	state	�擾�֐��ɗ^������
 * @retval	nothing
 */
//--------------------------------------------------------------
u32 MMDL_CallDrawGetProc( MMDL *mmdl, u32 state )
{
	GF_ASSERT( mmdl->draw_proc_list );
	GF_ASSERT( mmdl->draw_proc_list->get_proc );
	return( mmdl->draw_proc_list->get_proc(mmdl,state) );
}

//--------------------------------------------------------------
/**
 * MMDL �A�j���[�V�����R�}���h�R�[�h�Z�b�g
 * @param	mmdl	MMDL * 
 * @param	code	AC_DIR_U��
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetAcmdCode( MMDL * mmdl, u16 code )
{
	mmdl->acmd_code = code;
}

//--------------------------------------------------------------
/**
 * MMDL �A�j���[�V�����R�}���h�R�[�h�擾
 * @param	mmdl	MMDL * 
 * @retval	u16	AC_DIR_U��
 */
//--------------------------------------------------------------
u16 MMDL_GetAcmdCode( const MMDL * mmdl )
{
	return( mmdl->acmd_code );
}

//--------------------------------------------------------------
/**
 * MMDL �A�j���[�V�����R�}���h�V�[�P���X�Z�b�g
 * @param	mmdl	MMDL * 
 * @param	no		�V�[�P���X
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetAcmdSeq( MMDL * mmdl, u16 no )
{
	mmdl->acmd_seq = no;
}

//--------------------------------------------------------------
/**
 * MMDL �A�j���[�V�����R�}���h�V�[�P���X����
 * @param	mmdl	MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_IncAcmdSeq( MMDL * mmdl )
{
	mmdl->acmd_seq++;
}

//--------------------------------------------------------------
/**
 * MMDL �A�j���[�V�����R�}���h�V�[�P���X�擾
 * @param	mmdl	MMDL * 
 * @retval	u16 �V�[�P���X
 */
//--------------------------------------------------------------
u16 MMDL_GetAcmdSeq( const MMDL * mmdl )
{
	return( mmdl->acmd_seq );
}

//--------------------------------------------------------------
/**
 * MMDL ���݂̃}�b�v�A�g���r���[�g���Z�b�g
 * @param	mmdl	MMDL *
 * @param	attr	�Z�b�g����A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetMapAttr( MMDL * mmdl, u32 attr )
{
	mmdl->now_attr = attr;
}

//--------------------------------------------------------------
/**
 * MMDL ���݂̃}�b�v�A�g���r���[�g���擾
 * @param	mmdl	MMDL *
 * @retval	u32		�}�b�v�A�g���r���[�g
 */
//--------------------------------------------------------------
u32 MMDL_GetMapAttr( const MMDL * mmdl )
{
	return( mmdl->now_attr );
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ��̃}�b�v�A�g���r���[�g���Z�b�g
 * @param	mmdl	MMDL *
 * @param	attr	�Z�b�g����A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetMapAttrOld( MMDL * mmdl, u32 attr )
{
	mmdl->old_attr = attr;
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ��̃}�b�v�A�g���r���[�g���擾
 * @param	mmdl	MMDL *
 * @retval	u32		�}�b�v�A�g���r���[�g
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
 * @param	mmdl	MMDL *
 * @retval	u16 �]�[��ID
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
 * @param	mmdl	MMDL * 
 * @retval	s16 X���W
 */
//--------------------------------------------------------------
s16 MMDL_GetInitGridPosX( const MMDL * mmdl )
{
	return( mmdl->gx_init );
}

//--------------------------------------------------------------
/**
 * MMDL �������W �O���b�hX���W�Z�b�g
 * @param	mmdl	MMDL * 
 * @param	x		�Z�b�g������W
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetInitGridPosX( MMDL * mmdl, s16 x )
{
	mmdl->gx_init = x;
}

//--------------------------------------------------------------
/**
 * MMDL �������W Y���W�擾
 * @param	mmdl	MMDL * 
 * @retval	s16		Y
 */
//--------------------------------------------------------------
s16 MMDL_GetInitGridPosY( const MMDL * mmdl )
{
	return( mmdl->gy_init );
}

//--------------------------------------------------------------
/**
 * MMDL �������W Y���W�Z�b�g
 * @param	mmdl	MMDL * 
 * @param	y		�Z�b�g������W
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetInitGridPosY( MMDL * mmdl, s16 y )
{
	mmdl->gy_init = y;
}

//--------------------------------------------------------------
/**
 * MMDL �������W z���W�擾
 * @param	mmdl	MMDL * 
 * @retval	s16		z���W
 */
//--------------------------------------------------------------
s16 MMDL_GetInitGridPosZ( const MMDL * mmdl )
{
	return( mmdl->gz_init );
}

//--------------------------------------------------------------
/**
 * MMDL �������W z���W�Z�b�g
 * @param	mmdl	MMDL * 
 * @param	z		�Z�b�g������W
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetInitGridPosZ( MMDL * mmdl, s16 z )
{
	mmdl->gz_init = z;
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ����W�@X���W�擾
 * @param	mmdl	MMDL * 
 * @retval	s16		X���W
 */
//--------------------------------------------------------------
s16 MMDL_GetOldGridPosX( const MMDL * mmdl )
{
	return( mmdl->gx_old );
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ����W X���W�Z�b�g
 * @param	mmdl	MMDL * 
 * @param	x		�Z�b�g������W
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetOldGridPosX( MMDL * mmdl, s16 x )
{
	mmdl->gx_old = x;
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ����W Y���W�擾
 * @param	mmdl	MMDL * 
 * @retval	s16		Y
 */
//--------------------------------------------------------------
s16 MMDL_GetOldGridPosY( const MMDL * mmdl )
{
	return( mmdl->gy_old );
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ����W Y���W�Z�b�g
 * @param	mmdl	MMDL * 
 * @param	y		�Z�b�g������W
 * @retval	s16		y���W
 */
//--------------------------------------------------------------
void MMDL_SetOldGridPosY( MMDL * mmdl, s16 y )
{
	mmdl->gy_old = y;
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ����W z���W�擾
 * @param	mmdl	MMDL * 
 * @retval	s16		z���W
 */
//--------------------------------------------------------------
s16 MMDL_GetOldGridPosZ( const MMDL * mmdl )
{
	return( mmdl->gz_old );
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ����W z���W�Z�b�g
 * @param	mmdl	MMDL * 
 * @param	z		�Z�b�g������W
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetOldGridPosZ( MMDL * mmdl, s16 z )
{
	mmdl->gz_old = z;
}

//--------------------------------------------------------------
/**
 * MMDL ���ݍ��W X���W�擾
 * @param	mmdl	MMDL * 
 * @retval	s16		X���W
 */
//--------------------------------------------------------------
s16 MMDL_GetGridPosX( const MMDL * mmdl )
{
	return( mmdl->gx_now );
}

//--------------------------------------------------------------
/**
 * MMDL ���ݍ��W X���W�Z�b�g
 * @param	mmdl	MMDL * 
 * @param	x		�Z�b�g������W
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetGridPosX( MMDL * mmdl, s16 x )
{
	mmdl->gx_now = x;
}

//--------------------------------------------------------------
/**
 * MMDL ���ݍ��W X���W����
 * @param	mmdl	MMDL * 
 * @param	x		�����l
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_AddGridPosX( MMDL * mmdl, s16 x )
{
	mmdl->gx_now += x;
}

//--------------------------------------------------------------
/**
 * MMDL ���ݍ��W Y���W�擾
 * @param	mmdl	MMDL * 
 * @retval	s16		Y
 */
//--------------------------------------------------------------
s16 MMDL_GetGridPosY( const MMDL * mmdl )
{
	return( mmdl->gy_now );
}

//--------------------------------------------------------------
/**
 * MMDL ���ݍ��W Y���W�Z�b�g
 * @param	mmdl	MMDL * 
 * @param	y		�Z�b�g������W
 * @retval	s16		y���W
 */
//--------------------------------------------------------------
void MMDL_SetGridPosY( MMDL * mmdl, s16 y )
{
	mmdl->gy_now = y;
}

//--------------------------------------------------------------
/**
 * MMDL ���ݍ��W Y���W����
 * @param	mmdl	MMDL * 
 * @param	y		�����l
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_AddGridPosY( MMDL * mmdl, s16 y )
{
	mmdl->gy_now += y;
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ����W z���W�擾
 * @param	mmdl	MMDL * 
 * @retval	s16		z���W
 */
//--------------------------------------------------------------
s16 MMDL_GetGridPosZ( const MMDL * mmdl )
{
	return( mmdl->gz_now );
}

//--------------------------------------------------------------
/**
 * MMDL ���ݍ��W z���W�Z�b�g
 * @param	mmdl	MMDL * 
 * @param	z		�Z�b�g������W
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetGridPosZ( MMDL * mmdl, s16 z )
{
	mmdl->gz_now = z;
}

//--------------------------------------------------------------
/**
 * MMDL ���ݍ��W z���W����
 * @param	mmdl	MMDL * 
 * @param	z		�����l
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_AddGridPosZ( MMDL * mmdl, s16 z )
{
	mmdl->gz_now += z;
}

//--------------------------------------------------------------
/**
 * MMDL �����W�|�C���^�擾
 * @param	mmdl	MMDL * 
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
 * @param	mmdl	MMDL * 
 * @param	vec		���W�i�[��
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_GetVectorPos( const MMDL * mmdl, VecFx32 *vec )
{
	*vec = mmdl->vec_pos_now;
}

//--------------------------------------------------------------
/**
 * MMDL �����W�Z�b�g
 * @param	mmdl	MMDL * 
 * @param	vec		�Z�b�g���W
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetVectorPos( MMDL * mmdl, const VecFx32 *vec )
{
	mmdl->vec_pos_now = *vec;
}

//--------------------------------------------------------------
/**
 * MMDL �����WY�l�擾
 * @param	mmdl	MMDL * 
 * @retval	fx32	����
 */
//--------------------------------------------------------------
fx32 MMDL_GetVectorPosY( const MMDL * mmdl )
{
	return( mmdl->vec_pos_now.y );
}

//--------------------------------------------------------------
/**
 * MMDL �\�����W�I�t�Z�b�g�擾
 * @param	mmdl	MMDL * 
 * @param	vec		�Z�b�g���W
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_GetVectorDrawOffsetPos( const MMDL * mmdl, VecFx32 *vec )
{
	*vec = mmdl->vec_draw_offs;
}

//--------------------------------------------------------------
/**
 * MMDL �\�����W�I�t�Z�b�g�Z�b�g
 * @param	mmdl	MMDL * 
 * @param	vec		�Z�b�g���W
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetVectorDrawOffsetPos( MMDL * mmdl, const VecFx32 *vec )
{
	mmdl->vec_draw_offs = *vec;
}

//--------------------------------------------------------------
/**
 * MMDL �O���w��\�����W�I�t�Z�b�g�擾
 * @param	mmdl	MMDL * 
 * @param	vec		�Z�b�g���W
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_GetVectorOuterDrawOffsetPos( const MMDL * mmdl, VecFx32 *vec )
{
	*vec = mmdl->vec_draw_offs_outside;
}

//--------------------------------------------------------------
/**
 * MMDL �O���w��\�����W�I�t�Z�b�g�Z�b�g
 * @param	mmdl	MMDL * 
 * @param	vec		�Z�b�g���W
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetVectorOuterDrawOffsetPos( MMDL * mmdl, const VecFx32 *vec )
{
	mmdl->vec_draw_offs_outside = *vec;
}

//--------------------------------------------------------------
/**
 * MMDL �A�g���r���[�g�ω����W�I�t�Z�b�g�擾
 * @param	mmdl	MMDL * 
 * @param	vec		�Z�b�g���W
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_GetVectorAttrDrawOffsetPos( const MMDL * mmdl, VecFx32 *vec )
{
	*vec = mmdl->vec_attr_offs;
}

//--------------------------------------------------------------
/**
 * MMDL �A�g���r���[�g�ω����W�I�t�Z�b�g�Z�b�g
 * @param	mmdl	MMDL * 
 * @param	vec		�Z�b�g���W
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetVectorAttrDrawOffsetPos( MMDL * mmdl, const VecFx32 *vec )
{
	mmdl->vec_attr_offs = *vec;
}

//--------------------------------------------------------------
/**
 * MMDL ����(�O���b�h�P��)���擾
 * @param	mmdl	MMDL *
 * @retval	s16		�����BH_GRID�P��
 */
//--------------------------------------------------------------
s16 MMDL_GetHeightGrid( const MMDL * mmdl )
{
	fx32 y = MMDL_GetVectorPosY( mmdl );
	s16 gy = SIZE_GRID_FX32( y );
	return( gy );
}

//--------------------------------------------------------------
/**
 * MMDL MMDL_BLACTCONT���擾
 * @param	mmdl	MMDL *
 * @retval	MMDL_BLACTCONT*
 */
//--------------------------------------------------------------
MMDL_BLACTCONT * MMDL_GetBlActCont( MMDL *mmdl )
{
	return( MMDLSYS_GetBlActCont((MMDLSYS*)MMDL_GetMMdlSys(mmdl)) );
}

//======================================================================
//	MMDLSYS �X�e�[�^�X����
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS �`�揈��������������Ă��邩�ǂ����`�F�b�N
 * @param	mmdlsys	MMDLSYS *
 * @retval	BOOL	TRUE=����������Ă���
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
 * @param	mmdlsys	MMDLSYS*
 * @param	flag	TRUE=����������
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDLSYS_SetCompleteDrawInit( MMDLSYS *mmdlsys, BOOL flag )
{
	if( flag == TRUE ){
		MMdlSys_OnStatusBit( mmdlsys, MMDLSYS_STABIT_DRAW_INIT_COMP );
	}else{
		MMdlSys_OffStatusBit( mmdlsys, MMDLSYS_STABIT_DRAW_INIT_COMP );
	}
}

//--------------------------------------------------------------
/**
 * MMDLSYS �e��t����A�t���Ȃ��̃Z�b�g
 * @param	mmdlsys MMDLSYS *
 * @param	flag	TRUE=�e��t���� FALSE=�e��t���Ȃ�
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDLSYS_SetJoinShadow( MMDLSYS *fos, BOOL flag )
{
	if( flag == FALSE ){
		MMdlSys_OnStatusBit( fos, MMDLSYS_STABIT_SHADOW_JOIN_NOT );
	}else{
		MMdlSys_OffStatusBit( fos, MMDLSYS_STABIT_SHADOW_JOIN_NOT );
	}
}

//--------------------------------------------------------------
/**
 * MMDLSYS �e��t����A�t���Ȃ��̃`�F�b�N
 * @param	mmdlsys MMDLSYS *
 * @retval	BOOL TRUE=�t����
 */
//--------------------------------------------------------------
BOOL MMDLSYS_CheckJoinShadow( const MMDLSYS *fos )
{
	if( MMDLSYS_CheckStatusBit(fos,MMDLSYS_STABIT_SHADOW_JOIN_NOT) ){
		return( FALSE );
	}
	return( TRUE );
}

//======================================================================
//	MMDLSYS �v���Z�X����
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS �t�B�[���h���샂�f���S�̂̓�������S��~�B
 * ���쏈���A�`�揈�������S��~����B�A�j���[�V�����R�}���h�ɂ��������Ȃ��B
 * @param	mmdlsys	MMDLSYS*
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDLSYS_StopProc( MMDLSYS *mmdlsys )
{
	MMdlSys_OnStatusBit( mmdlsys,
		MMDLSYS_STABIT_MOVE_PROC_STOP|MMDLSYS_STABIT_DRAW_PROC_STOP );
}

//--------------------------------------------------------------
/**
 * MMDLSYS MMDLSYS_StopProc()�̉����B
 * @param	mmdlsys	MMDLSYS*
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDLSYS_PlayProc( MMDLSYS *mmdlsys )
{
	MMdlSys_OffStatusBit( mmdlsys,
		MMDLSYS_STABIT_MOVE_PROC_STOP|MMDLSYS_STABIT_DRAW_PROC_STOP );
}

//--------------------------------------------------------------
/**
 * MMDLSYS �t�B�[���h���샂�f���S�̂̓�����ꎞ��~
 * �ŗL�̓��쏈���i�����_���ړ����j���ꎞ��~����B
 * �A�j���[�V�����R�}���h�ɂ͔�������B
 * @param	mmdlsys	MMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
#include "field/field_comm/intrude_types.h"
#include "field/field_comm/intrude_main.h"  //Intrude_ObjPauseCheck
void MMDLSYS_PauseMoveProc( MMDLSYS *mmdlsys )
{
	u32 no = 0;
	MMDL *mmdl;

#if 1
  FIELDMAP_WORK *fieldMap = mmdlsys->fieldMapWork;
  if( fieldMap != NULL ){
    GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( fieldMap );
    if(Intrude_ObjPauseCheck(gsys) == FALSE){
      return;
    }
  }
#endif
	
	while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) == TRUE ){
		MMDL_OnStatusBitMoveProcPause( mmdl );
	}
}

//--------------------------------------------------------------
/**
 * MMDLSYS �t�B�[���h���샂�f���S�̂̈ꎞ��~������
 * @param	mmdlsys	MMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDLSYS_ClearPauseMoveProc( MMDLSYS *mmdlsys )
{
	u32 no = 0;
	MMDL *mmdl;

	while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) == TRUE ){
		MMDL_OffStatusBitMoveProcPause( mmdl );
	}
}

//======================================================================
//	MMDL �X�e�[�^�X�r�b�g�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f������
 * �t�B�[���h���샂�f���V�X�e���̃r�b�g�`�F�b�N
 * @param	mmdl		MMDL*
 * @param	bit			MMDLSYS_STABIT_DRAW_INIT_COMP��
 * @retval	u32			0�ȊO bit�q�b�g
 */
//--------------------------------------------------------------
u32 MMDL_CheckMMdlSysStatusBit(
	const MMDL *mmdl, MMDLSYS_STABIT bit )
{
	const MMDLSYS *fos = MMDL_GetMMdlSys( mmdl );
	return( MMDLSYS_CheckStatusBit(fos,bit) );
}

//--------------------------------------------------------------
/**
 * MMDL �g�p�`�F�b�N
 * @param	mmdl	MMDL *
 * @retval	BOOL	TRUE=�g�p��
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
 * @param	mmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_OnStatusBitMove( MMDL *mmdl )
{
	MMDL_OnStatusBit( mmdl, MMDL_STABIT_MOVE );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ����쒆������
 * @param	mmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_OffStatusBitMove( MMDL * mmdl )
{
	MMDL_OffStatusBit( mmdl, MMDL_STABIT_MOVE );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ����쒆�`�F�b�N
 * @param	mmdl	MMDL *
 * @retval	BOOL TRUE=���쒆
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitMove( const MMDL *mmdl )
{
	if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_MOVE) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ�����J�n�ɂ���
 * @param	mmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_OnStatusBitMoveStart( MMDL * mmdl )
{
	MMDL_OnStatusBit( mmdl, MMDL_STABIT_MOVE_START );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ�����J�n������
 * @param	mmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_OffStatusBitMoveStart( MMDL * mmdl )
{
	MMDL_OffStatusBit( mmdl, MMDL_STABIT_MOVE_START );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ�����J�n�`�F�b�N
 * @param	mmdl	MMDL *
 * @retval	BOOL TRUE=�ړ�����J�n
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitMoveStart( const MMDL * mmdl )
{
	if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_MOVE_START) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ�����I���ɂ���
 * @param	mmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_OnStatusBitMoveEnd( MMDL * mmdl )
{
	MMDL_OnStatusBit( mmdl, MMDL_STABIT_MOVE_END );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ�����I��������
 * @param	mmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_OffStatusBitMoveEnd( MMDL * mmdl )
{
	MMDL_OffStatusBit( mmdl, MMDL_STABIT_MOVE_END );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ�����I���`�F�b�N
 * @param	mmdl	MMDL *
 * @retval	BOOL TRUE=�ړ��I��
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitMoveEnd( const MMDL * mmdl )
{
	if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_MOVE_END) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �`�揉���������ɂ���
 * @param	mmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_OnStatusBitCompletedDrawInit( MMDL * mmdl )
{
	MMDL_OnStatusBit( mmdl, MMDL_STABIT_DRAW_PROC_INIT_COMP );
}

//--------------------------------------------------------------
/**
 * MMDL �`�揉��������������
 * @param	mmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_OffStatusBitCompletedDrawInit( MMDL * mmdl )
{
	MMDL_OffStatusBit( mmdl, MMDL_STABIT_DRAW_PROC_INIT_COMP );
}

//--------------------------------------------------------------
/**
 * MMDL �`�揉���������`�F�b�N
 * @param	mmdl	MMDL *
 * @retval	BOOL TRUE=�`�揉��������
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitCompletedDrawInit( const MMDL * mmdl )
{
	if( MMDL_CheckStatusBit(
			mmdl,MMDL_STABIT_DRAW_PROC_INIT_COMP) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL ��\���t���O���`�F�b�N
 * @param	mmdl	MMDL *
 * @retval	BOOL TRUE=��\���@FALSE=�\��
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
 * @param	mmdl	MMDL *
 * @param	flag	TRUE=��\���@FALSE=�\��
 * @retval	nothing
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
 * @param	mmdl	MMDL *
 * @param	flag	TRUE=�q�b�g�A���@FALSE=�q�b�g�i�V
 * @retval	nothing
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
 * @param	mmdl	MMDL *
 * @param	flag	TRUE=���쒆�@FALSE=��~��
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitMove( MMDL * mmdl, int flag )
{
	if( flag == TRUE ){
		MMDL_OnStatusBitMove( mmdl );
	}else{
		MMDL_OffStatusBitMove( mmdl );
	}
}

//--------------------------------------------------------------
/**
 * MMDL �b�������\�`�F�b�N
 * @param	mmdl	MMDL *
 * @retval	BOOL TRUE=�\ FALSE=�s��
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
 * @param	mmdl	MMDL *
 * @param	flag	TRUE=�s�� FALSE=�\
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
 * @param	mmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_OnStatusBitMoveProcPause( MMDL * mmdl )
{
	MMDL_OnStatusBit( mmdl, MMDL_STABIT_PAUSE_MOVE );
}

//--------------------------------------------------------------
/**
 * MMDL ����|�[�Y����
 * @param	mmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_OffStatusBitMoveProcPause( MMDL * mmdl )
{
	MMDL_OffStatusBit( mmdl, MMDL_STABIT_PAUSE_MOVE );
}

//--------------------------------------------------------------
/**
 * MMDL ����|�[�Y�`�F�b�N
 * @param	mmdl	MMDL *
 * @retval	BOOL TRUE=����|�[�Y
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitMoveProcPause( const MMDL * mmdl )
{
	if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_PAUSE_MOVE) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �X�e�[�^�X�r�b�g �`�揈�������������`�F�b�N
 * @param	mmdl		MMDL *
 * @retval	BOOL TRUE=�����BFALSE=�܂�
 */
//--------------------------------------------------------------
BOOL MMDL_CheckCompletedDrawInit( const MMDL * mmdl )
{
	const MMDLSYS *fos;
	
	fos = MMDL_GetMMdlSys( mmdl );
	
	if( MMDLSYS_CheckCompleteDrawInit(fos) == FALSE ){
		return( FALSE );
	}
	
	if( MMDL_CheckStatusBitCompletedDrawInit(mmdl) ){
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �����擾���֎~����
 * @param	mmdl	MMDL *
 * @param	int		TRUE=�����擾OFF FALSE=ON
 * @retval	nothing
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
 * @param	mmdl	MMDL *
 * @retval	BOOL TRUE=�֎~
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
 * @param	mmdl	MMDL *
 * @param	flag	TRUE=�֎~ FALSE=�֎~���Ȃ�
 * @retval	nothing
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
 * @param	mmdl	MMDL *
 * @param	flag	TRUE=�G�C���A�X FALSE=�Ⴄ
 * @retval	nothing
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
 * @param	mmdl	MMDL *
 * @retval	BOOL TRUE=�G�C���A�X FALSE=�Ⴄ
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
 * @param	mmdl	MMDL *
 * @param	flag	TRUE=�Z�b�g�@FALSE=�N���A
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitShoalEffect( MMDL * mmdl, BOOL flag )
{
	if( flag == TRUE ){
		MMDL_OnStatusBit( mmdl, MMDL_STABIT_EFFSET_SHOAL );
	}else{
		MMDL_OffStatusBit( mmdl, MMDL_STABIT_EFFSET_SHOAL );
	}
}

//--------------------------------------------------------------
/**
 * MMDL �󐣃G�t�F�N�g�Z�b�g�t���O���`�F�b�N
 * @param	mmdl	MMDL *
 * @retval	BOOL TRUE=�Z�b�g�@FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitShoalEffect( const MMDL * mmdl )
{
	if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_EFFSET_SHOAL) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �A�g���r���[�g�I�t�Z�b�g�ݒ�OFF�Z�b�g
 * @param	mmdl	MMDL *
 * @param	flag	TRUE=�Z�b�g�@FALSE=�N���A
 * @retval	nothing
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
 * @param	mmdl	MMDL *
 * @retval	BOOL TRUE=OFF�@FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitAttrOffsetOFF( const MMDL * mmdl )
{
	if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_ATTR_OFFS_OFF) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL ���ړ��t���O�Z�b�g
 * @param	mmdl	MMDL *
 * @param	flag	TRUE=�Z�b�g�@FALSE=�N���A
 * @retval	nothing
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
 * @param	mmdl	MMDL *
 * @retval	BOOL TRUE=���@FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitBridge( const MMDL * mmdl )
{
	if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_BRIDGE) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �f�肱�݃t���O�Z�b�g
 * @param	mmdl	MMDL *
 * @param	flag	TRUE=�Z�b�g�@FALSE=�N���A
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitReflect( MMDL * mmdl, BOOL flag )
{
	if( flag == TRUE ){
		MMDL_OnStatusBit( mmdl, MMDL_STABIT_REFLECT_SET );
	}else{
		MMDL_OffStatusBit( mmdl, MMDL_STABIT_REFLECT_SET );
	}
}

//--------------------------------------------------------------
/**
 * MMDL �f�肱�݃t���O�`�F�b�N
 * @param	mmdl	MMDL *
 * @retval	BOOL TRUE=�Z�b�g�@FALSE=����
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitReflect( const MMDL * mmdl )
{
	if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_REFLECT_SET) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �A�j���[�V�����R�}���h�`�F�b�N
 * @param	mmdl	MMDL *
 * @retval	BOOL TRUE=�R�}���h�A���@FALSE=����
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitAcmd( const MMDL * mmdl )
{
	if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_ACMD) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �g�����������t���O���Z�b�g
 * @param	mmdl	MMDL *
 * @param	flag	TRUE=�Z�b�g�@FALSE=�N���A
 * @retval	nothing
 */
//--------------------------------------------------------------
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
 * @param	mmdl	MMDL *
 * @retval	BOOL TRUE=�g�������ɔ�������@FALSE=����
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitHeightExpand( const MMDL * mmdl )
{
	if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_HEIGHT_EXPAND) ){
		return( TRUE );
	}
	return( FALSE );
}

//======================================================================
//	MMDL ����r�b�g�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * MMDL �A�g���r���[�g�擾���~
 * @param	mmdl	MMDL *
 * @param	flag	TRUE=��~
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetMoveBitAttrGetOFF( MMDL * mmdl, BOOL flag )
{
	if( flag == TRUE ){
		MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_ATTR_GET_OFF );
	}else{
		MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_ATTR_GET_OFF );
	}
}

//--------------------------------------------------------------
/**
 * MMDL �A�g���r���[�g�擾���~�@�`�F�b�N
 * @param	mmdl	MMDL *
 * @retval	int	TRUE=��~
 */
//--------------------------------------------------------------
int MMDL_CheckMoveBitAttrGetOFF( const MMDL * mmdl )
{
	if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_ATTR_GET_OFF) ){
		return( TRUE );
	}
	return( FALSE );
}

//======================================================================
//	MMDLSYS �c�[��
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS �g�p���Ă���t�B�[���h���샂�f����T���B
 * @param	fos	MMDLSYS *
 * @param	mmdl	MMDL*�i�[��
 * @param	no	�����J�n���[�Nno�B�擪���猟������ۂ͏����l0���w��B
 * @retval BOOL TRUE=���샂�f���擾���� FALSE=no����I�[�܂Ō������擾����
 * @note ����no�͌Ăяo����A�擾�ʒu+1�̒l�ɂȂ�B
 * @note ����FOBJ ID 1�Ԃ̓��샂�f����T���B
 * u32 no=0;
 * MMDL *mmdl;
 * while( MMDLSYS_SearchUseMMdl(fos,&mmdl,&no) == TRUE ){
 * 		if( MMDL_GetOBJID(mmdl) == 1 ){
 * 			break;
 * 		}
 * }
 */
//--------------------------------------------------------------
BOOL MMDLSYS_SearchUseMMdl(
	const MMDLSYS *fos, MMDL **mmdl, u32 *no )
{
	u32 max = MMDLSYS_GetMMdlMax( fos );
	
	if( (*no) < max ){
		MMDL *check_obj = &(((MMDLSYS*)fos)->pMMdlBuf[*no]);
		
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
 * @param	sys			MMDLSYS *
 * @param	x			�����O���b�hX
 * @param	z			�����O���b�hZ
 * @param	old_hit		TURE=�ߋ����W�����肷��
 * @param array   ������MMDL�|�C���^���ő�16���i�[����\���̌^
 *
 * @return  �������z�� 
 */
//--------------------------------------------------------------
static int MMDLSYS_SearchGridPosArray(
	const MMDLSYS *sys, s16 x, s16 z, BOOL old_hit ,MMDL_PARRAY* array)
{
	u32 no = 0;
	MMDL *mmdl;

  MI_CpuClear8(array,sizeof(MMDL_PARRAY));

	while( MMDLSYS_SearchUseMMdl(sys,&mmdl,&no) == TRUE ){
    if(array->count >= MMDL_POST_LAYER_MAX){
      GF_ASSERT_MSG(0,"������WOBJ���I�[�o�[\n"); 
      break;  //����ȏ�i�[�ł��Ȃ�
    }
		if( old_hit ){
			if( MMDL_GetOldGridPosX(mmdl) == x &&
				  MMDL_GetOldGridPosZ(mmdl) == z ){
				array->mmdl_parray[array->count++] = mmdl;
        continue;
			}
		}
		
		if( MMDL_GetGridPosX(mmdl) == x &&
		  	MMDL_GetGridPosZ(mmdl) == z ){
      array->mmdl_parray[array->count++] = mmdl;
      continue;
		}
	}
	
	return array->count;
}

//--------------------------------------------------------------
/**
 * MMDLSYS �w�肳�ꂽ�O���b�hX,Z���W�ɂ���OBJ���擾(���̌������l���Ȃ���)
 * @param	sys			MMDLSYS *
 * @param	x			�����O���b�hX
 * @param	z			�����O���b�hZ
 * @param	old_hit		TURE=�ߋ����W�����肷��
 * @retval	MMDL	x,z�ʒu�ɂ���MMDL * �BNULL=���̍��W��OBJ�͂��Ȃ�
 */
//--------------------------------------------------------------
MMDL * MMDLSYS_SearchGridPos(
	const MMDLSYS *sys, s16 x, s16 z, BOOL old_hit )
{
	u32 num = 0;
	MMDL_PARRAY array;

  num = MMDLSYS_SearchGridPosArray(sys, x, z, old_hit , &array);
	
  if(num){
    return array.mmdl_parray[0];
  }
	return( NULL );
}

//--------------------------------------------------------------
/**
 * MMDLSYS �w�肳�ꂽ�O���b�hX,Z,and Y���W�ɂ���OBJ���擾(���̌������l������)
 * @param	sys			MMDLSYS *
 * @param	x		  	�����O���b�hX
 * @param	z			  �����O���b�hZ
 * @param height  ����Y���W�l fx32�^
 * @param h_diff  Y�l�ŋ��e����U�ꕝ(��Βl�����̎w��l������Y����������OBJ�݂̂�Ԃ�)
 *
 * @param	old_hit		TURE=�ߋ����W�����肷��
 * @retval	MMDL	x,z�ʒu�ɂ���MMDL * �BNULL=���̍��W��OBJ�͂��Ȃ�
 */
//--------------------------------------------------------------
MMDL * MMDLSYS_SearchGridPosEx(
	const MMDLSYS *sys, s16 x, s16 z, fx32 height, fx32 y_diff, BOOL old_hit )
{
	u32 i = 0,num = 0;
	MMDL_PARRAY array;

  num = MMDLSYS_SearchGridPosArray(sys, x, z, old_hit , &array);

  if(num == 0){
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
 * @param	fos		MMDLSYS *
 * @param	mv_code		�������铮��R�[�h
 * @retval	MMDL *	NULL=���݂��Ȃ�
 */
//--------------------------------------------------------------
MMDL * MMDLSYS_SearchMoveCode( const MMDLSYS *fos, u16 mv_code )
{
	u32 no = 0;
	MMDL *mmdl;
	
	while( MMDLSYS_SearchUseMMdl(fos,&mmdl,&no) == TRUE ){
		if( MMDL_GetMoveCode(mmdl) == mv_code ){
			return( mmdl );
		}
	}
	
	return( NULL );
}

//--------------------------------------------------------------
/**
 * MMDLSYS OBJ ID�Ɉ�v����MMDL *������
 * @param	fos		MMDLSYS *
 * @param	id		��������OBJ ID
 * @retval	MMDL *	NULL=���݂��Ȃ�
 */
//--------------------------------------------------------------
MMDL * MMDLSYS_SearchOBJID( const MMDLSYS *fos, u16 id )
{
	u32 no = 0;
	MMDL *mmdl;

	while( MMDLSYS_SearchUseMMdl(fos,&mmdl,&no) == TRUE ){
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
 * MMDLSYS �t�B�[���h���샂�f���̋󂫂�T��
 * @param	sys			MMDLSYS *
 * @retval	MMDL	�󂫂�MMDL*�@�󂫂������ꍇ��NULL
 */
//--------------------------------------------------------------
static MMDL * MMdlSys_SearchSpaceMMdl( const MMDLSYS *sys )
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
 * @param	fos			MMDLSYS *
 * @param	obj_id		��v����OBJ ID
 * @param	zone_id		��v����ZONE ID
 * @retval	MMDL	��v����MMDL*�@��v����=NULL
 */
//--------------------------------------------------------------
static MMDL * MMdlSys_SearchAlies(
	const MMDLSYS *fos, int obj_id, int zone_id )
{
	u32 no = 0;
	MMDL * mmdl;
	
	while( MMDLSYS_SearchUseMMdl(fos,&mmdl,&no) ){
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
 * @param	fos	MMDLSYS
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDLSYS_DeleteZoneUpdateMMdl( MMDLSYS *fos )
{
	u32 no = 0;
	MMDL *mmdl;
	
	while( MMDLSYS_SearchUseMMdl(fos,&mmdl,&no) ){
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
//	MMDL �c�[��
//======================================================================
//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���@TCB����֐��ǉ�
 * @param	mmdl	MMDL*
 * @param	sys		MMDLSYS*
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_AddTCB( MMDL *mmdl, const MMDLSYS *sys )
{
	int pri,code;
	GFL_TCB * tcb;
	
	pri = MMDLSYS_GetTCBPriority( sys );
	code = MMDL_GetMoveCode( mmdl );
	
	if( code == MV_PAIR || code == MV_TR_PAIR ){
		pri += MMDL_TCBPRI_OFFS_AFTER;
	}
	
	tcb = GFL_TCB_AddTask( MMDLSYS_GetTCBSYS((MMDLSYS*)sys),
			MMdl_TCB_MoveProc, mmdl, pri );
	GF_ASSERT( tcb != NULL );
	
	mmdl->pTCB = tcb;
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���@TCB����֐��폜
 * @param	mmdl	MMDL*
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_DeleteTCB( MMDL *mmdl )
{
	GF_ASSERT( mmdl->pTCB );
	GFL_TCB_DeleteTask( mmdl->pTCB );
	mmdl->pTCB = NULL;
}

//--------------------------------------------------------------
/**
 * MMDL ���ݔ������Ă���t�B�[���h���샂�f����OBJ�R�[�h���Q��
 * @param	mmdl	MMDL * 
 * @param	code	�`�F�b�N����R�[�h�BHERO��
 * @retval	BOOL	TRUE=mmdl�ȊO�ɂ�code�������Ă���z������
 */
//--------------------------------------------------------------
BOOL MMDL_SearchUseOBJCode( const MMDL *mmdl, u16 code )
{
	u32 no = 0;
	u16 check_code;
	MMDL *cmmdl;
	const MMDLSYS *fos = MMDL_GetMMdlSys( mmdl );
	
	while( MMDLSYS_SearchUseMMdl(fos,&cmmdl,&no) == TRUE ){
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
 * @param	mmdl	MMDL *
 * @param	vec		���������W
 * @param	dir		���� DIR_UP��
 * @retval	nothing
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
	MMDL_OnStatusBit( mmdl, MMDL_STABIT_MOVE_START );
	MMDL_OffStatusBit( mmdl, MMDL_STABIT_MOVE|MMDL_STABIT_MOVE_END );
}

//--------------------------------------------------------------
/**
 * MMDL ���W�A�������������B�O���b�h��
 * @param	mmdl	MMDL *
 * @param	vec		���������W
 * @param	dir		���� DIR_UP��
 * @retval	nothing
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
 * MMDL �w�b�_�[������W�^�C�v�A���W�A�R�[�h��ύX
 * @param	mmdl	MMDL *
 * @param	head MMDL_HEADAER
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_ChangeMoveParam( MMDL *mmdl, const MMDL_HEADER *head )
{
	const MMDLSYS *fos = MMDL_GetMMdlSys( mmdl );
  
	MMDL_CallMoveDeleteProc( mmdl );
  MMdl_SetHeaderBefore( mmdl, head, NULL );
	MMdl_InitCallMoveProcWork( mmdl );
	MMdl_InitMoveProc( fos, mmdl );
  MMdl_SetHeaderAfter( mmdl, head, NULL );
}

//--------------------------------------------------------------
/**
 * MMDL �`�揉�����ɍs�������Z��
 * @param	mmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_InitDrawWork( MMDL *mmdl )
{
	const MMDLSYS *fos = MMDL_GetMMdlSys( mmdl );
	
	if( MMDLSYS_CheckCompleteDrawInit(fos) == FALSE ){
		return; //�`��V�X�e���������������Ă��Ȃ�
	}
	
	if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_HEIGHT_GET_ERROR) ){
		MMDL_UpdateCurrentHeight( mmdl );
	}
	
	MMDL_SetDrawStatus( mmdl, 0 );
	#ifndef MMDL_PL_NULL
	MMDL_BlActAddPracFlagSet( mmdl, FALSE );
	#endif
	
	if( MMDL_CheckStatusBitCompletedDrawInit(mmdl) == FALSE ){
		MMdl_InitCallDrawProcWork( mmdl );
		MMDL_CallDrawInitProc( mmdl );
		MMDL_OnStatusBitCompletedDrawInit( mmdl );
	}
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f�� �`��֐�������
 * @param	mmdl		MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_InitCallDrawProcWork( MMDL * mmdl )
{
	const MMDL_DRAW_PROC_LIST *list;
	u16 code = MMDL_GetOBJCode( mmdl );
	const OBJCODE_PARAM *prm = MMDL_GetOBJCodeParam( mmdl, code );
	list = DrawProcList_GetList( prm->draw_proc_no );
	mmdl->draw_proc_list = list;
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���֘A�G�t�F�N�g�̃t���O�������B
 * �G�t�F�N�g�֘A�̃t���O���������܂Ƃ߂�B
 * @param	mmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_InitDrawEffectFlag( MMDL * mmdl )
{
	MMDL_OffStatusBit( mmdl,
		MMDL_STABIT_SHADOW_SET		|
		MMDL_STABIT_SHADOW_VANISH	|
		MMDL_STABIT_EFFSET_SHOAL		|
		MMDL_STABIT_REFLECT_SET );
}

//--------------------------------------------------------------
/**
 * MMDL OBJ ID��ύX
 * @param	mmdl	MMDL *
 * @param	id		OBJ ID
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_ChangeOBJID( MMDL * mmdl, u16 id )
{
	MMDL_SetOBJID( mmdl, id );
	MMDL_OnStatusBitMoveStart( mmdl );
	MMdl_InitDrawEffectFlag( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDL ���[�N����
 * @param	mmdl	MMDL
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_ClearWork( MMDL *mmdl )
{
	GFL_STD_MemClear( mmdl, MMDL_SIZE );
}

//--------------------------------------------------------------
/**
 * MMDL �w�肳�ꂽ�t�B�[���h���샂�f�����G�C���A�X�w�肩�ǂ����`�F�b�N
 * @param	mmdl		MMDL *
 * @param	h_zone_id	head��ǂݍ��ރ]�[��ID
 * @param	max			head�v�f��
 * @param	head		MMDL_H
 * @retval	int			RET_ALIES_NOT��
 */
//--------------------------------------------------------------
static int MMdl_CheckHeaderAlies(
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
						return( RET_ALIES_EXIST );	//Alies�Ƃ��Ċ��ɑ���
					}
				}else if( MMDL_GetZoneID(mmdl) == zone_id ){
					return( RET_ALIES_CHANGE );		//Alies�Ώۂł���
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
 * @param	fos		MMDLSYS *
 * @param	obj_id	OBJ ID
 * @param	zone_id	�]�[��ID
 * @retval	MMDL * MMDL *
 */
//--------------------------------------------------------------
static MMDL * MMdl_SearchOBJIDZoneID(
		const MMDLSYS *fos, int obj_id, int zone_id )
{
	u32 no = 0;
	MMDL *mmdl;
	
	while( MMDLSYS_SearchUseMMdl(fos,&mmdl,&no) == TRUE ){
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
 * @param	mmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_InitDrawStatus( MMDL * mmdl )
{
	MMDL_OnStatusBit( mmdl, MMDL_STABIT_MOVE_START );
	MMdl_InitDrawEffectFlag( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���`��폜���ɍs������
 * @param	mmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_SetDrawDeleteStatus( MMDL * mmdl )
{
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���@�G�C���A�X���琳�KOBJ�ւ̕ύX
 * @param	mmdl		MMDL * 
 * @param	head		�Ώۂ�MMDL_H
 * @param	zone_id		���K�̃]�[��ID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_ChangeAliesOBJ(
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
 * @param	mmdl		MMDL * 
 * @param	head		�Ώۂ�MMDL_H
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_ChangeOBJAlies(
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
 * @param	mmdl	MMDL *
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
 * @param	mmdl	MMDL *
 * @param data MMDL_SAMEDATA*
 * @retval BOOL	TRUE=����BFALSE=����ł͂Ȃ�
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
 * @param	mmdl	MMDL *
 * @param data MMDL_SAMEDATA*
 * @retval BOOL	TRUE=����BFALSE=����ł͂Ȃ�
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

//======================================================================
//	OBJCODE_PARAM
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS OBJCODE_PARAM ������
 * @param	mmdlsys	MMDLSYS
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdlSys_InitOBJCodeParam( MMDLSYS *mmdlsys )
{
	u8 *p = GFL_ARC_LoadDataAlloc( ARCID_MMDL_PARAM, 
			NARC_fldmmdl_mdlparam_fldmmdl_mdlparam_bin,
			mmdlsys->heapID );
	mmdlsys->pOBJCodeParamBuf = p;
	mmdlsys->pOBJCodeParamTbl = (const OBJCODE_PARAM*)(&p[OBJCODE_PARAM_TOTAL_NUMBER_SIZE]);
}

//--------------------------------------------------------------
/**
 * MMDLSYS OBJCODE_PARAM �폜
 * @param	mmdlsys	MMDLSYS
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdlSys_DeleteOBJCodeParam( MMDLSYS *mmdlsys )
{
	GFL_HEAP_FreeMemory( mmdlsys->pOBJCodeParamBuf );
	mmdlsys->pOBJCodeParamBuf = NULL;
}

//--------------------------------------------------------------
/**
 * MMDLSYS OBJCODE_PARAM �擾
 * @param	mmdlsys	MMDLSYS *
 * @param	code	�擾����OBJ�R�[�h
 * @retval	OBJCODE_PARAM*
 */
//--------------------------------------------------------------
const OBJCODE_PARAM * MMDLSYS_GetOBJCodeParam(
		const MMDLSYS *mmdlsys, u16 code )
{
  GF_ASSERT( mmdlsys->pOBJCodeParamTbl != NULL );
  code = OBJCode_GetDataNumber( code );
	return( &(mmdlsys->pOBJCodeParamTbl[code]) );
}

//--------------------------------------------------------------
/**
 * MMDL OBJCODE_PARAM �擾
 * @param	mmdl	MMDL*
 * @param	code	�擾����OBJ�R�[�h
 * @retval	OBJCODE_PARAM*
 */
//--------------------------------------------------------------
const OBJCODE_PARAM * MMDL_GetOBJCodeParam(
		const MMDL *mmdl, u16 code )
{
	const MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
	return( MMDLSYS_GetOBJCodeParam(mmdlsys,code) );
}

//--------------------------------------------------------------
/**
 * MMDL OBJCODE_PARAM����OBJCODE_PARAM_BUF_BBD�擾
 * @param
 * @retval
 */
//--------------------------------------------------------------
const OBJCODE_PARAM_BUF_BBD * MMDL_GetOBJCodeParamBufBBD(
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
const OBJCODE_PARAM_BUF_MDL * MMDL_GetOBJCodeParamBufMDL(
    const OBJCODE_PARAM *param )
{
  return (const OBJCODE_PARAM_BUF_MDL*)param->buf;
}

//======================================================================
//	parts
//======================================================================
//--------------------------------------------------------------
/**
 * �w�肳�ꂽOBJ�R�[�h�����[�N�Q�ƌ^�Ȃ�΃��[�N��OBJ�R�[�h�ɕύX�B
 * �Ⴄ�ꍇ�͂��̂܂܂ŕԂ��B
 * @param	fsys	FIELDSYS_WORK *
 * @param	code	OBJ�R�[�h�BHERO��
 * @retval	int		�`�F�b�N���ꂽOBJ�R�[�h
 */
//--------------------------------------------------------------
static u16 WorkOBJCode_GetOBJCode( void *fsys, int code )
{
	#ifndef MMDL_PL_NULL
	if( code >= WKOBJCODE_ORG && code <= WKOBJCODE_END ){
		code -= WKOBJCODE_ORG;
		code = GetEvDefineObjCode( fsys, code );
	}
	#else
//	GF_ASSERT( 0 );
	#endif
	
	return( code );
}

//--------------------------------------------------------------
/**
 * �w�肳�ꂽOBJ�R�[�h���f�[�^�z��v�f���ɕϊ�
 * @param code OBJ CODE
 * @retval u16 code���f�[�^�̕��тɂ��킹�����l
 */
//--------------------------------------------------------------
static u16 OBJCode_GetDataNumber( u16 code )
{
  if( code < OBJCODEEND_BBD ){
    return( code );
  }
  
  if( code >= OBJCODESTART_TPOKE && code < OBJCODEEND_TPOKE ){
    code = (code - OBJCODESTART_TPOKE) + OBJCODEEND_BBD;
    return( code );
  }
  
  GF_ASSERT( 0 );
  return( BOY1 ); //�G���[���p�Ƃ��Ė���ȃR�[�h��Ԃ�
}

//--------------------------------------------------------------
/**
 * �w�肳�ꂽ����R�[�h�̊֐����X�g���擾
 * @param	code	����R�[�h
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
 * @param	code	OBJ�R�[�h
 * @retval	MMDL_DRAW_PROC_LIST*
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
 * @param	head	FIELD_OBJ_H
 * @retval	int		TRUE=�G�C���A�X�@FALSE=�Ⴄ
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
 * @param	head	FIELD_OBJ_H
 * @retval	int		�]�[��ID
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
static BOOL MMdlSys_CheckEventFlag( EVENTWORK *evwork, u16 flag_no )
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

//======================================================================
//	����֐��_�~�[
//======================================================================
//--------------------------------------------------------------
/**
 * ���쏉�����֐��_�~�[
 * @param	MMDL	MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_MoveInitProcDummy( MMDL * mmdl )
{
}

//--------------------------------------------------------------
/**
 * ����֐��_�~�[
 * @param	MMDL	MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_MoveProcDummy( MMDL * mmdl )
{
#ifdef  DEBUG_ONLY_FOR_kagaya
  if( MMDL_GetOBJID(mmdl) != MMDL_ID_PLAYER ){
    if( MMDL_GetDirDisp(mmdl) != DIR_UP ){
      OS_Printf( "���샂�f�� ID %d DIR %d\n",
          MMDL_GetOBJID(mmdl), MMDL_GetDirDisp(mmdl) );
    }
  }
#endif
}

//--------------------------------------------------------------
/**
 * ����폜�֐��_�~�[
 * @param	MMDL	MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_MoveDeleteProcDummy( MMDL * mmdl )
{
}

//--------------------------------------------------------------
/**
 * ���앜�A�֐��_�~�[
 * @param	MMDL *	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_MoveReturnProcDummy( MMDL * mmdl )
{
}

//======================================================================
//	�`��֐��_�~�[
//======================================================================
//--------------------------------------------------------------
/**
 * �`�揉�����֐��_�~�[
 * @param	MMDL	MMDL * 
 * @retval	int			TRUE=����������
 */
//--------------------------------------------------------------
void MMDL_DrawInitProcDummy( MMDL * mmdl )
{
}

//--------------------------------------------------------------
/**
 * �`��֐��_�~�[
 * @param	MMDL	MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_DrawProcDummy( MMDL * mmdl )
{
}

//--------------------------------------------------------------
/**
 * �`��폜�֐��_�~�[
 * @param	MMDL	MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_DrawDeleteProcDummy( MMDL * mmdl )
{
}

//--------------------------------------------------------------
/**
 * �`��ޔ��֐��_�~�[
 * @param	MMDL	MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_DrawPushProcDummy( MMDL * mmdl )
{
}

//--------------------------------------------------------------
/**
 * �`�敜�A�֐��_�~�[
 * @param	MMDL	MMDL * 
 * @retval	nothing
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
  
  if( MMDL_GetOBJCode(mmdl) == ROCK ){
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
      mmdlsys, gx, gz, pos->y, GRID_FX32, FALSE );
  
  if( mmdl != NULL ){
    if( mmdl_rockpos_CheckPos(mmdl) == TRUE ){
      return( TRUE );
    }
  }
  
  return( FALSE );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �w�b�_�[�ɃO���b�h�}�b�v�|�W�V������ݒ�
 *	@param	head    �w�b�_�[
 *	@param	gx      ���O���b�h���W
 *	@param	gz      ���O���b�h���W
 *	@param	y       �����W
 */
//-----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
/**
 *	@brief  �w�b�_�[�Ƀ��[���}�b�v�|�W�V������ݒ�
 *
 *	@param	head    �w�b�_�[
 *	@param	index   �C���f�b�N�X
 *	@param	front   �t�����g�O���b�h���W
 *	@param	side    �T�C�h�O���b�h���W
 */
//-----------------------------------------------------------------------------
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
//	debug
//======================================================================
#ifdef DEBUG_MMDL
//--------------------------------------------------------------
/**
 * �f�o�b�O�@���샂�f���@OBJ�R�[�h��������擾(ASCII)
 * @param	code OBJ�R�[�h
 * @param	heapID buf�̈�m�ۗpHEAPID
 * @retval	u8* �����񂪊i�[���ꂽu8*�B�g�p��GFL_HEAP_FreeMemory()���K�v�B
 * �����񒷂�DEBUG_OBJCODE_STR_LENGTH�B
 */
//--------------------------------------------------------------
u8 * DEBUG_MMDL_GetOBJCodeString( u16 code, HEAPID heapID )
{
	u8 *buf;
  
  code = OBJCode_GetDataNumber( code );
	
  buf = GFL_HEAP_AllocClearMemoryLo(
			heapID, DEBUG_OBJCODE_STR_LENGTH );
	
  GFL_ARC_LoadDataOfs( buf, ARCID_MMDL_PARAM,
			NARC_fldmmdl_mdlparam_fldmmdl_objcodestr_bin,
			DEBUG_OBJCODE_STR_LENGTH * code, DEBUG_OBJCODE_STR_LENGTH );
	
  return( buf );
}
#endif //DEBUG_MMDL

//--------------------------------------------------------------
/**
 * OBJ�R�[�h��ύX����
 * @param
 * @retval
 */
//--------------------------------------------------------------
void MMDL_ChangeOBJCode( MMDL *mmdl, u16 code )
{
	const MMDLSYS *fos;
	fos = MMDL_GetMMdlSys( mmdl );
  
	if( MMDLSYS_CheckCompleteDrawInit(fos) == TRUE ){
    if( MMDL_CheckStatusBitCompletedDrawInit(mmdl) == TRUE ){
		  MMDL_CallDrawDeleteProc( mmdl );
    }
  }
  
  MMDL_SetOBJCode( mmdl, code );
  MMDL_OffStatusBitCompletedDrawInit( mmdl );
  MMdl_InitDrawStatus( mmdl );
  MMdl_InitDrawWork( mmdl );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �m�[�O���b�h����}�b�p�[�̎擾
 *
 *	@param	fos   ���샂�f���V�X�e��
 *
 *	@return�@�}�b�p�[
 */
//-----------------------------------------------------------------------------
FLDNOGRID_MAPPER * MMDLSYS_GetNOGRIDMapper( const MMDLSYS *fos )
{
  GF_ASSERT( fos );
  return fos->pNOGRIDMapper;
}

//======================================================================
//
//======================================================================
#if 0
//--------------------------------------------------------------
/**
 * MMDL_BUFFER �t�B�[���h���샂�f���o�b�t�@���烍�[�h
 * @param
 * @retval
 */
//--------------------------------------------------------------
MMDL * MMDL_BUFFER_LoadMMdl(
	MMDL_BUFFER *buf, MMDLSYS *fos, int no )
{
	MMDL *mmdl;
	
	mmdl = MMdlSys_SearchSpaceMMdl( fos );
	GF_ASSERT( mmdl != NULL );
	
	OS_Printf( "MMDL LoadNo %d\n", no );
	
	*mmdl = buf->fldMMdlBuf[no];
	
	MMdl_InitWork( mmdl, fos );
	
	if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_MOVEPROC_INIT) == 0 ){
		MMdl_InitMoveWork( mmdl );
	}else{
		MMdl_InitCallMoveProcWork( mmdl );
	}
	
	MMDL_OffStatusBitCompletedDrawInit( mmdl );
	MMdl_InitDrawWork( mmdl );
	
	MMdlSys_AddMMdlTCB( fos, mmdl );
	MMdlSys_IncrementOBJCount( (MMDLSYS*)MMDL_GetMMdlSys(mmdl) );
	
	return( mmdl );
}

//--------------------------------------------------------------
/**
 * MMDL_BUFFER �t�B�[���h���샂�f���o�b�t�@���烍�[�h
 * @param
 * @retval
 */
//--------------------------------------------------------------
void MMDL_BUFFER_LoadBuffer( MMDL_BUFFER *buf, MMDLSYS *fos )
{
	int i;
	MMDL *mmdl;

	for( i = 0; i < MMDL_BUFFER_MAX; i++ ){
		mmdl = &buf->fldMMdlBuf[i];
		if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_USE) ){
			MMDL_BUFFER_LoadMMdl( buf, fos, i );
		}
	}
}

//--------------------------------------------------------------
/**
 * MMDL_BUFFER �t�B�[���h���샂�f���o�b�t�@�փZ�[�u
 * @param
 * @retval
 */
//--------------------------------------------------------------
void MMDL_BUFFER_SaveBuffer( MMDL_BUFFER *buf, MMDLSYS *fos )
{
	MMDL *mmdl;
	u32 i = 0, no = 0;
	
	MMDL_BUFFER_InitBuffer( buf );
	while( MMDLSYS_SearchUseMMdl(fos,&mmdl,&i) == TRUE ){
		OS_Printf( "MMDL BUFFER WorkNo %d, BufferNo %d Save\n", i-1, no );
		buf->fldMMdlBuf[no] = *mmdl;
		no++;
	}
}
#endif
