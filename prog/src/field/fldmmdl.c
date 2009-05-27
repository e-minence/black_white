//======================================================================
/**
 * @file	fldmmdl.c
 * @brief	�t�B�[���h���샂�f��
 * @author	kagaya
 * @data	05.07.13
 */
//======================================================================
#include "fldmmdl.h"
#include "fldmmdl_procdraw.h"

#include "arc_def.h"
#include "arc/fieldmap/fldmmdl_mdlparam.naix"

//======================================================================
//	define
//======================================================================
//--------------------------------------------------------------
//	debug
//--------------------------------------------------------------

//--------------------------------------------------------------
//	FLDMMDL ����A�`��֐����[�N�T�C�Y (byte size)
//--------------------------------------------------------------
#define FLDMMDL_MOVE_WORK_SIZE		(16)	///<����֐��p���[�N�T�C�Y
#define FLDMMDL_MOVE_SUB_WORK_SIZE	(16)	///<����T�u�֐��p���[�N�T�C�Y
#define FLDMMDL_MOVE_CMD_WORK_SIZE	(16)	///<����R�}���h�p���[�N�T�C�Y
#define FLDMMDL_DRAW_WORK_SIZE		(32)	///<�`��֐��p���[�N�T�C�Y

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
///	FLDMMDLSYS�\����
//--------------------------------------------------------------
struct _TAG_FLDMMDLSYS
{
	u32 status_bit;					///<�X�e�[�^�X�r�b�g
	u16 fmmdl_max;					///<FLDMMDL�ő吔
	HEAPID sysHeapID;				///<�V�X�e���p �q�[�vID
	FLDMMDL *pFldMMdlBuf;			///<FLDMMDL���[�N *
	
	HEAPID heapID;					///<�q�[�vID
	s16 fmmdl_count;				///<�t�B�[���h���샂�f�����ݐ�
	u16 tcb_pri;					///<TCB�v���C�I���e�B
	u16 dummy;						///<�]��
	const FLDMAPPER *pG3DMapper;	///<FLDMAPPER
	
	void *pTCBSysWork;				///<TCB���[�N
	GFL_TCBSYS *pTCBSys;			///<TCBSYS*
	
	FLDMMDL_BLACTCONT *pBlActCont;	///<FLDMMDL_BLACTCONT
	
	u8 *pOBJCodeParamBuf;			///<OBJCODE_PARAM�o�b�t�@
	const OBJCODE_PARAM *pOBJCodeParamTbl; ///<OBJCODE_PARAM
};

#define FLDMMDLSYS_SIZE (sizeof(FLDMMDLSYS)) ///<FLDMMDLSYS�T�C�Y

//--------------------------------------------------------------
///	FLDMMDL�\����
//--------------------------------------------------------------
struct _TAG_FLDMMDL
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
	u16 dir_head;				///<FLDMMDL_H�w�����
	u16 dir_disp;				///<���݌����Ă������
	u16 dir_move;				///<���ݓ����Ă������
	u16 dir_disp_old;			///<�ߋ��̓����Ă�������
	u16 dir_move_old;			///<�ߋ��̓����Ă�������
	
	int param0;					///<�w�b�_�w��p�����^
	int param1;					///<�w�b�_�w��p�����^
	int param2;					///<�w�b�_�w��p�����^
	
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
	const FLDMMDLSYS *pFldMMdlSys;///<FLDMMDLSYS*
	
	const FLDMMDL_MOVE_PROC_LIST *move_proc_list; ///<����֐����X�g
	const FLDMMDL_DRAW_PROC_LIST *draw_proc_list; ///<�`��֐����X�g
	
	u8 move_proc_work[FLDMMDL_MOVE_WORK_SIZE];///����֐��p���[�N
	u8 move_sub_proc_work[FLDMMDL_MOVE_SUB_WORK_SIZE];///����T�u�֐��p���[�N
	u8 move_cmd_proc_work[FLDMMDL_MOVE_CMD_WORK_SIZE];///����R�}���h�p���[�N
	u8 draw_proc_work[FLDMMDL_DRAW_WORK_SIZE];///�`��֐��p���[�N
};

#define FLDMMDL_SIZE (sizeof(FLDMMDL)) ///<FLDMMDL�T�C�Y 224

//--------------------------------------------------------------
///	FLDMMDL_SAVEWORK�\���� size 80
//--------------------------------------------------------------
typedef struct
{
	u32 status_bit;			///<�X�e�[�^�X�r�b�g
	u32 move_bit;			///<����r�b�g
	
	u8 obj_id;				///<OBJ ID
	u8 move_code;			///<����R�[�h
	s8 move_limit_x;		///<X�����ړ�����
	s8 move_limit_z;		///<Z�����ړ�����
	
	s8 dir_head;			///<FLDMMDL_H�w�����
	s8 dir_disp;			///<���݌����Ă������
	s8 dir_move;			///<���ݓ����Ă������
	u8 dummy;				///<�_�~�[
	
	u16 zone_id;			///<�]�[�� ID
	u16 obj_code;			///<OBJ�R�[�h
	u16 event_type;			///<�C�x���g�^�C�v
	u16 event_flag;			///<�C�x���g�t���O
	u16 event_id;			///<�C�x���gID
	s16 param0;				///<�w�b�_�w��p�����^
	s16 param1;				///<�w�b�_�w��p�����^
	s16 param2;				///<�w�b�_�w��p�����^
	s16 gx_init;			///<�����O���b�hX
	s16 gy_init;			///<�����O���b�hY
	s16 gz_init;			///<�����O���b�hZ
	s16 gx_now;				///<���݃O���b�hX
	s16 gy_now;				///<���݃O���b�hY
	s16 gz_now;				///<���݃O���b�hZ
	fx32 fx32_y;			///<fx32�^�̍����l
	u8 move_proc_work[FLDMMDL_MOVE_WORK_SIZE];///<����֐��p���[�N
	u8 move_sub_proc_work[FLDMMDL_MOVE_SUB_WORK_SIZE];///<����T�u�֐��p���[�N
}FLDMMDL_SAVEWORK;

//--------------------------------------------------------------
///	FLDMMDL_SAVEDATA�\����
//--------------------------------------------------------------
struct _TAG_FLDMMDL_SAVEDATA
{
	FLDMMDL_SAVEWORK SaveWorkBuf[FLDMMDL_SAVEMMDL_MAX];
};

//======================================================================
//	proto
//======================================================================
//FLDMMDLSYS �v���Z�X
static void FldMMdlSys_DeleteProc( FLDMMDLSYS *fos );

//FLDMMDL �ǉ��A�폜
static void FldMMdl_SetHeader(
	FLDMMDL * fmmdl, const FLDMMDL_HEADER *head, void *sys );
static void FldMMdl_SetHeaderPos(FLDMMDL *fmmdl,const FLDMMDL_HEADER *head);
static void FldMMdl_InitWork( FLDMMDL * fmmdl, const FLDMMDLSYS *sys );
static void FldMMdl_InitCallMoveProcWork( FLDMMDL * fmmdl );
static void FldMMdl_InitMoveWork( const FLDMMDLSYS *fos, FLDMMDL * fmmdl );
#if 0
static void FldMMdlSys_CheckSetInitMoveWork( FLDMMDLSYS *fos );
static void FldMMdlSys_CheckSetInitDrawWork( FLDMMDLSYS *fos );
#endif

//FLDMMDL ����֐�
static void FldMMdl_TCB_MoveProc( GFL_TCB * tcb, void *work );
static void FldMMdl_TCB_DrawProc( FLDMMDL * fmmdl );

//FLDMMDL_SAVEDATA
static void FldMMdl_SaveData_SaveFldMMdl(
	const FLDMMDL *fmmdl, FLDMMDL_SAVEWORK *save );
static void FldMMdl_SaveData_LoadFldMMdl(
	FLDMMDL *fmmdl, const FLDMMDL_SAVEWORK *save, const FLDMMDLSYS *fos );

//FLDMMDLSYS �ݒ�A�Q��
static void FldMMdlSys_OnStatusBit(
	FLDMMDLSYS *fmmdlsys, FLDMMDLSYS_STABIT bit );
static void FldMMdlSys_OffStatusBit(
	FLDMMDLSYS *fmmdlsys, FLDMMDLSYS_STABIT bit );
static void FldMMdlSys_IncrementOBJCount( FLDMMDLSYS *fmmdlsys );
static void FldMMdlSys_DecrementOBJCount( FLDMMDLSYS *fmmdlsys );

//FLDMMDLSYS �c�[��
static FLDMMDL * FldMMdlSys_SearchSpaceFldMMdl( const FLDMMDLSYS *sys );
static FLDMMDL * FldMMdlSys_SearchAlies(
	const FLDMMDLSYS *fos, int obj_id, int zone_id );

//FLDMMDL �c�[��
static void FldMMdl_AddTCB( FLDMMDL *fmmdl, const FLDMMDLSYS *sys );
static void FldMMdl_DeleteTCB( FLDMMDL *fmmdl );
static void FldMMdl_InitDrawWork( FLDMMDL *fmmdl );
static void FldMMdl_InitCallDrawProcWork( FLDMMDL * fmmdl );
static void FldMMdl_InitDrawEffectFlag( FLDMMDL * fmmdl );
static void FldMMdl_ClearWork( FLDMMDL *fmmdl );
static int FldMMdl_CheckHeaderAlies(
		const FLDMMDL *fmmdl, int h_zone_id, int max,
		const FLDMMDL_HEADER *head );
static FLDMMDL * FldMMdl_SearchOBJIDZoneID(
		const FLDMMDLSYS *fos, int obj_id, int zone_id );
static void FldMMdl_InitDrawStatus( FLDMMDL * fmmdl );
static void FldMMdl_SetDrawDeleteStatus( FLDMMDL * fmmdl );
static void FldMMdl_ChangeAliesOBJ(
	FLDMMDL *fmmdl, const FLDMMDL_HEADER *head, int zone_id );
static void FldMMdl_ChangeOBJAlies(
	FLDMMDL * fmmdl, int zone_id, const FLDMMDL_HEADER *head );

//OBJCODE_PARAM
static void FldMMdlSys_InitOBJCodeParam( FLDMMDLSYS *fmmdlsys );
static void FldMMdlSys_DeleteOBJCodeParam( FLDMMDLSYS *fmmdlsys );

//parts
static u16 WorkOBJCode_GetOBJCode( void *fsys, int code );
static const FLDMMDL_MOVE_PROC_LIST * MoveProcList_GetList( u16 code );
static const FLDMMDL_DRAW_PROC_LIST * DrawProcList_GetList(
		FLDMMDL_DRAWPROCNO no );
static BOOL FldMMdlHeader_CheckAlies( const FLDMMDL_HEADER *head );
static int FldMMdlHeader_GetAliesZoneID( const FLDMMDL_HEADER *head );

//======================================================================
//	�t�B�[���h���샂�f���@�V�X�e��
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMMDLSYS �V�X�e���쐬
 * @param	heapID	HEAPID
 * @param	max	���샂�f���ő吔
 * @retval	FLDMMDLSYS* �쐬���ꂽFLDMMDLSYS*
 */
//--------------------------------------------------------------
FLDMMDLSYS * FLDMMDLSYS_CreateSystem( HEAPID heapID, u32 max )
{
	FLDMMDLSYS *fos;
	fos = GFL_HEAP_AllocClearMemory( heapID, FLDMMDLSYS_SIZE );
	fos->pFldMMdlBuf = GFL_HEAP_AllocClearMemory( heapID, max*FLDMMDL_SIZE );
	fos->fmmdl_max = max;
	fos->sysHeapID = heapID;
	return( fos );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS �V�X�e���J��
 * @param	fos	FLDMMDLSYS*
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_FreeSystem( FLDMMDLSYS *fos )
{
	GFL_HEAP_FreeMemory( fos->pFldMMdlBuf );
	GFL_HEAP_FreeMemory( fos->pFldMMdlBuf );
}

//======================================================================
//	�t�B�[���h���샂�f���@�V�X�e���@�v���Z�X
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMMDLSYS �V�X�e�� ����v���Z�X�Z�b�g�A�b�v
 * @param	fos	FLDMMDLSYS*
 * @param	heapID	�v���Z�X�pHEAPID
 * @param	pG3DMapper FLDMAPPER
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_SetupProc(
	FLDMMDLSYS *fos, HEAPID heapID, const FLDMAPPER *pG3DMapper )
{
	fos->heapID = heapID;
	fos->pG3DMapper = pG3DMapper;
	
	FldMMdlSys_InitOBJCodeParam( fos );
	
	fos->pTCBSysWork = GFL_HEAP_AllocMemory(
		heapID, GFL_TCB_CalcSystemWorkSize(fos->fmmdl_max) );
	fos->pTCBSys = GFL_TCB_Init( fos->fmmdl_max, fos->pTCBSysWork );
	
	FldMMdlSys_OnStatusBit( fos, FLDMMDLSYS_STABIT_MOVE_INIT_COMP );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS �V�X�e���@����v���Z�X�폜
 * @param	fos		FLDMMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdlSys_DeleteProc( FLDMMDLSYS *fos )
{
	FldMMdlSys_DeleteOBJCodeParam( fos );
	GFL_TCB_Exit( fos->pTCBSys );
	GFL_HEAP_FreeMemory( fos->pTCBSysWork );
	FldMMdlSys_OffStatusBit( fos, FLDMMDLSYS_STABIT_MOVE_INIT_COMP );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS �S�Ẵv���Z�X�폜
 * @param	fos		FLDMMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_DeleteProc( FLDMMDLSYS *fos )
{
//	FLDMMDLSYS_DeleteMMdl( fos );
	FLDMMDLSYS_DeleteDraw( fos );
	FldMMdlSys_DeleteProc( fos );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS ���샂�f���X�V
 * @param	fos	FLDMMDLSYS
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_UpdateProc( FLDMMDLSYS *fos )
{
	GFL_TCBSYS *tcbsys = fos->pTCBSys;
	GF_ASSERT( tcbsys != NULL );
	GFL_TCB_Main( tcbsys );
}

//======================================================================
//	�t�B�[���h���샂�f���@�V�X�e���@�`��v���Z�X
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMMDLSYS �V�X�e���@�`��v���Z�X�Z�b�g�A�b�v
 * @param	fos	FLDMMDLSYS*
 * @param	heapID	�v���Z�X�pHEAPID
 * @param	pG3DMapper FLDMAPPER
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_SetupDrawProc( FLDMMDLSYS *fos )
{
	FLDMMDLSYS_SetCompleteDrawInit( fos, TRUE );
}

//======================================================================
//	�t�B�[���h���샂�f���ǉ��A�폜
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMMDLSYS �t�B�[���h���샂�f����ǉ�
 * @param	fos			FLDMMDLSYS *
 * @param	header		�ǉ��������Z�߂�FLDMMDL_HEADER *
 * @param	zone_id		�]�[��ID
 * @retval	FLDMMDL	�ǉ����ꂽFLDMMDL *
 */
//--------------------------------------------------------------
FLDMMDL * FLDMMDLSYS_AddFldMMdl(
	const FLDMMDLSYS *fos, const FLDMMDL_HEADER *header, int zone_id )
{
	FLDMMDL *fmmdl;
	FLDMMDL_HEADER header_data = *header;
	const FLDMMDL_HEADER *head = &header_data;
	
	fmmdl = FldMMdlSys_SearchSpaceFldMMdl( fos );
	GF_ASSERT( fmmdl != NULL );
	
	FldMMdl_SetHeader( fmmdl, head, NULL );
	FldMMdl_InitWork( fmmdl, fos );
	FLDMMDL_SetZoneID( fmmdl, zone_id );
	
	if( FLDMMDLSYS_CheckStatusBit(fos,FLDMMDLSYS_STABIT_MOVE_INIT_COMP) ){
		FldMMdl_InitMoveWork( fos, fmmdl );
		FLDMMDL_InitMoveProc( fmmdl );
	}
	
	if( FLDMMDLSYS_CheckStatusBit(fos,FLDMMDLSYS_STABIT_DRAW_INIT_COMP) ){
		FldMMdl_InitDrawWork( fmmdl );
	}
	
	FldMMdlSys_IncrementOBJCount( (FLDMMDLSYS*)FLDMMDL_GetFldMMdlSys(fmmdl) );
	return( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS �t�B�[���h���샂�f����ǉ��@����
 * @param	fos			FLDMMDLSYS *
 * @param	header		�ǉ��������Z�߂�FLDMMDL_HEADER *
 * @param	zone_id		�]�[��ID
 * @param	count		header�v�f��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_SetFldMMdl( const FLDMMDLSYS *fos,
	const FLDMMDL_HEADER *header, int zone_id, int count )
{
	GF_ASSERT( count > 0 );
	GF_ASSERT( header != NULL );
	
	KAGAYA_Printf( "FLDMMDLSYS_SetFldMMdl Count %d\n", count );
	
	do{
		FLDMMDLSYS_AddFldMMdl( fos, header, zone_id );
		header++;
		count--;
	}while( count );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �t�B�[���h���샂�f�����폜
 * @param	fmmdl		�폜����FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_Delete( FLDMMDL * fmmdl )
{
	const FLDMMDLSYS *fos;
	
	fos = FLDMMDL_GetFldMMdlSys( fmmdl );
	
	if( FLDMMDLSYS_CheckCompleteDrawInit(fos) == TRUE ){
		FLDMMDL_CallDrawDeleteProc( fmmdl );
	}
	
	if( FLDMMDL_CheckMoveBit(fmmdl,FLDMMDL_MOVEBIT_MOVEPROC_INIT) ){
		FLDMMDL_CallMoveDeleteProc( fmmdl );
		FldMMdl_DeleteTCB( fmmdl );
	}
	
	FldMMdlSys_DecrementOBJCount( (FLDMMDLSYS*)(fmmdl->pFldMMdlSys) );
	FldMMdl_ClearWork( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS ���ݔ������Ă���t�B�[���h���샂�f����S�č폜
 * @param	fos	FLDMMDLSYS
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_DeleteMMdl( const FLDMMDLSYS *fos )
{
	u32 no = 0;
	FLDMMDL *fmmdl;
	
	while( FLDMMDLSYS_SearchUseFldMMdl(fos,&fmmdl,&no) ){
		FLDMMDL_Delete( fmmdl );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL �t�B�[���h���샂�f���@�w�b�_�[��񔽉f
 * @param	fmmdl		�ݒ肷��FLDMMDL * 
 * @param	head		���f�������Z�߂�FLDMMDL_HEADER *
 * @param	fsys		FIELDSYS_WORK *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_SetHeader(
	FLDMMDL * fmmdl, const FLDMMDL_HEADER *head, void *sys )
{
	FLDMMDL_SetOBJID( fmmdl, head->id );
	FLDMMDL_SetOBJCode( fmmdl, WorkOBJCode_GetOBJCode(sys,head->obj_code) );
	FLDMMDL_SetMoveCode( fmmdl, head->move_code );
	FLDMMDL_SetEventType( fmmdl, head->event_type );
	FLDMMDL_SetEventFlag( fmmdl, head->event_flag );
	FLDMMDL_SetEventID( fmmdl, head->event_id );
	fmmdl->dir_head = head->dir;
	FLDMMDL_SetParam( fmmdl, head->param0, FLDMMDL_PARAM_0 );
	FLDMMDL_SetParam( fmmdl, head->param1, FLDMMDL_PARAM_1 );
	FLDMMDL_SetParam( fmmdl, head->param2, FLDMMDL_PARAM_2 );
	FLDMMDL_SetMoveLimitX( fmmdl, head->move_limit_x );
	FLDMMDL_SetMoveLimitZ( fmmdl, head->move_limit_z );
	
	FldMMdl_SetHeaderPos( fmmdl, head );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �t�B�[���h���샂�f���@���W�n������
 * @param	fmmdl		FLDMMDL * 
 * @param	head		���f�������Z�߂�FLDMMDL_HEADER *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_SetHeaderPos( FLDMMDL *fmmdl, const FLDMMDL_HEADER *head )
{
	int pos;
	VecFx32 vec;
	
	pos = head->gx;
	vec.x = GRID_SIZE_FX32( pos ) + FLDMMDL_VEC_X_GRID_OFFS_FX32;
	FLDMMDL_SetInitGridPosX( fmmdl, pos );
	FLDMMDL_SetOldGridPosX( fmmdl, pos );
	FLDMMDL_SetGridPosX( fmmdl, pos );
	
	pos = head->y;		//pos�ݒ��fx32�^�ŗ���B
	vec.y = (fx32)pos;
	pos = SIZE_GRID_FX32( pos );
	FLDMMDL_SetInitGridPosY( fmmdl, pos );
	FLDMMDL_SetOldGridPosY( fmmdl, pos );
	FLDMMDL_SetGridPosY( fmmdl, pos );
	
	pos = head->gz;
	vec.z = GRID_SIZE_FX32( pos ) + FLDMMDL_VEC_Z_GRID_OFFS_FX32;
	FLDMMDL_SetInitGridPosZ( fmmdl, pos );
	FLDMMDL_SetOldGridPosZ( fmmdl, pos );
	FLDMMDL_SetGridPosZ( fmmdl, pos );
	
	FLDMMDL_SetVectorPos( fmmdl, &vec );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �t�B�[���h���샂�f���@���[�N������
 * @param	fmmdl		FLDMMDL * 
 * @param	sys			FLDMMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_InitWork( FLDMMDL * fmmdl, const FLDMMDLSYS *sys )
{
	FLDMMDL_OnStatusBit( fmmdl,
		FLDMMDL_STABIT_USE |				//�g�p��
		FLDMMDL_STABIT_HEIGHT_GET_ERROR |	//�����擾���K�v
		FLDMMDL_STABIT_ATTR_GET_ERROR );	//�A�g���r���[�g�擾���K�v
	
	if( FLDMMDL_CheckAliesEventID(fmmdl) == TRUE ){
		FLDMMDL_SetStatusBitAlies( fmmdl, TRUE );
	}
	
	fmmdl->pFldMMdlSys = sys;
	FLDMMDL_SetForceDirDisp( fmmdl, FLDMMDL_GetDirHeader(fmmdl) );
	FLDMMDL_SetDirMove( fmmdl, FLDMMDL_GetDirHeader(fmmdl) );
	FLDMMDL_FreeAcmd( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �t�B�[���h���샂�f�� ����֐�������
 * @param	fmmdl		FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_InitCallMoveProcWork( FLDMMDL * fmmdl )
{
	fmmdl->move_proc_list =
		MoveProcList_GetList( FLDMMDL_GetMoveCode(fmmdl) );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ���쏉�����ɍs�������Z��
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_InitMoveWork( const FLDMMDLSYS *fos, FLDMMDL *fmmdl )
{
	FldMMdl_InitCallMoveProcWork( fmmdl );
	FldMMdl_AddTCB( fmmdl, fos );
	FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_MOVE_START );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS ���쏉�������s���Ă��Ȃ����샂�f���ɑ΂��ď������������Z�b�g
 * @param	fos	FLDMMDLSYS*
 * @retval	nothing
 */
//--------------------------------------------------------------
#if 0
static void FldMMdlSys_CheckSetInitMoveWork( FLDMMDLSYS *fos )
{
	u32 i = 0;
	FLDMMDL *fmmdl;
	
	while( FLDMMDLSYS_SearchUseFldMMdl(fos,&fmmdl,&i) == TRUE ){
		if( FLDMMDL_CheckMoveBit(fmmdl,	//�������֐��Ăяo���܂�
			FLDMMDL_MOVEBIT_MOVEPROC_INIT) == 0 ){
			FLDMMDL_InitMoveProc( fmmdl );
		}
	}
}
#endif

//--------------------------------------------------------------
/**
 * FLDMMDLSYS �`�揉�������s���Ă��Ȃ����샂�f���ɑ΂��ď������������Z�b�g
 * @param	fos	FLDMMDLSYS*
 * @retval	nothing
 */
//--------------------------------------------------------------
#if 0
static void FldMMdlSys_CheckSetInitDrawWork( FLDMMDLSYS *fos )
{
	u32 i = 0;
	FLDMMDL *fmmdl;
	
	while( FLDMMDLSYS_SearchUseFldMMdl(fos,&fmmdl,&i) == TRUE ){
		if( FLDMMDL_CheckStatusBitCompletedDrawInit(fmmdl) == FALSE ){
			FldMMdl_InitDrawWork( fmmdl );
		}
	}
}
#endif

//======================================================================
//	FLDMMDLSYS PUSH POP
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMMDLSYS ���샂�f�� �ޔ�
 * @param	fmmdlsys	FLDMMDLSYS
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_Push( FLDMMDLSYS *fmmdlsys )
{
	u32 no = 0;
	FLDMMDL *fmmdl;
	
	#ifdef DEBUG_FLDMMDL_PRINT
	if( FLDMMDLSYS_CheckCompleteDrawInit(fmmdlsys) == FALSE ){
		GF_ASSERT( 0 && "WARNING!! ���샂�f�� �`�斢������\n" );
	}
	#endif
	
	while( FLDMMDLSYS_SearchUseFldMMdl(fmmdlsys,&fmmdl,&no) == TRUE ){
		{ //���쏈���̑ޔ�
			FldMMdl_DeleteTCB( fmmdl );
			FLDMMDL_OnMoveBit( fmmdl,
				FLDMMDL_MOVEBIT_NEED_MOVEPROC_RECOVER );
		}
		
		{ //�`�揈���̑ޔ�
			FLDMMDL_CallDrawPushProc( fmmdl );
			FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_DRAW_PUSH );
		}
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS ���샂�f�����A
 * @param	fmmdlsys	FLDMMDLSYS
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_Pop( FLDMMDLSYS *fmmdlsys )
{
	u32 no = 0;
	FLDMMDL *fmmdl;
	
	while( FLDMMDLSYS_SearchUseFldMMdl(fmmdlsys,&fmmdl,&no) == TRUE ){
		{	//���쏈�����A
			FldMMdl_InitMoveWork( fmmdlsys, fmmdl ); //���[�N������
			
			if( FLDMMDL_CheckMoveBit(fmmdl,	//�������֐��Ăяo���܂�
				FLDMMDL_MOVEBIT_MOVEPROC_INIT) == 0 ){
				FLDMMDL_InitMoveProc( fmmdl );
			}
			
			if( FLDMMDL_CheckMoveBit(fmmdl, //�����֐��Ăяo�����K�v
				FLDMMDL_MOVEBIT_NEED_MOVEPROC_RECOVER) ){
				FLDMMDL_CallMovePopProc( fmmdl );
				FLDMMDL_OffMoveBit( fmmdl,
					FLDMMDL_MOVEBIT_NEED_MOVEPROC_RECOVER );
			}
		}
		
		{	//�`�揈�����A
			if( FLDMMDL_CheckStatusBitCompletedDrawInit(fmmdl) == FALSE ){
				FldMMdl_InitDrawWork( fmmdl );
			}
			
			if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_DRAW_PUSH) ){
				FLDMMDL_CallDrawPopProc( fmmdl );
				FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_DRAW_PUSH );
			}
		}
	}
}

//======================================================================
//	FLDMMDL_SAVEDATA
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMMDL_SAVEDATA �Z�[�u�f�[�^ �o�b�t�@�T�C�Y�擾
 * @param	nothing
 * @retval	u32	�T�C�Y
 */
//--------------------------------------------------------------
u32 FLDMMDL_SAVEDATA_GetWorkSize( void )
{
	return( sizeof(FLDMMDL_SAVEDATA) );
}

//--------------------------------------------------------------
/**
 * FLDMMDL_SAVEDATA �Z�[�u�f�[�^ �o�b�t�@������
 * @param	p	FLDMMDL_SAVEDATA
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SAVEDATA_Init( void *p )
{
	MI_CpuClear8( p, FLDMMDL_SAVEDATA_GetWorkSize() );
}

//--------------------------------------------------------------
/**
 * FLDMMDL_SAVEDATA ���샂�f���Z�[�u
 * @param	fmmdlsys �Z�[�u����FLDMMDLSYS
 * @param	savedata LDMMDL_SAVEDATA
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SAVEDATA_Save(
	FLDMMDLSYS *fmmdlsys, FLDMMDL_SAVEDATA *savedata )
{
	u32 no = 0;
	FLDMMDL *fmmdl;
	FLDMMDL_SAVEWORK *save = savedata->SaveWorkBuf;
	
	while( FLDMMDLSYS_SearchUseFldMMdl(fmmdlsys,&fmmdl,&no) == TRUE ){
		FldMMdl_SaveData_SaveFldMMdl( fmmdl, save );
		save++;
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL_SAVEDATA ���샂�f�����[�h
 * @param	fmmdlsys	FLDMMDLSYS
 * @param	save	���[�h����FLDMMDL_SAVEWORK
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SAVEDATA_Load(
	FLDMMDLSYS *fmmdlsys, FLDMMDL_SAVEDATA *savedata )
{
	u32 no = 0;
	FLDMMDL *fmmdl;
	FLDMMDL_SAVEWORK *save = savedata->SaveWorkBuf;
	
	while( no < FLDMMDL_SAVEMMDL_MAX ){
		if( (save->status_bit&FLDMMDL_STABIT_USE) ){
			fmmdl = FldMMdlSys_SearchSpaceFldMMdl( fmmdlsys );
			FldMMdl_SaveData_LoadFldMMdl( fmmdl, save, fmmdlsys );
		}
		save++;
		no++;
	}

  fmmdlsys->fmmdl_count = no;
}

//--------------------------------------------------------------
/**
 * FLDMMDL_SAVEDATA ���샂�f���@�Z�[�u
 * @param	fldmmdl		�Z�[�u����FLDMMDL*
 * @param	save FLDMMDL_SAVEWORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_SaveData_SaveFldMMdl(
	const FLDMMDL *fmmdl, FLDMMDL_SAVEWORK *save )
{
	save->status_bit = FLDMMDL_GetStatusBit( fmmdl );
	save->move_bit = FLDMMDL_GetMoveBit( fmmdl );
	save->obj_id = FLDMMDL_GetOBJID( fmmdl );
	save->zone_id = FLDMMDL_GetZoneID( fmmdl );
	save->obj_code = FLDMMDL_GetOBJCode( fmmdl );
	save->move_code = FLDMMDL_GetMoveCode( fmmdl );
	save->event_type = FLDMMDL_GetEventType( fmmdl );
	save->event_flag = FLDMMDL_GetEventFlag( fmmdl );
	save->event_id = FLDMMDL_GetEventID( fmmdl );
	save->dir_head = FLDMMDL_GetDirHeader( fmmdl );
	save->dir_disp = FLDMMDL_GetDirDisp( fmmdl );
	save->dir_move = FLDMMDL_GetDirMove( fmmdl );
	save->param0 = FLDMMDL_GetParam( fmmdl, FLDMMDL_PARAM_0 );
	save->param1 = FLDMMDL_GetParam( fmmdl, FLDMMDL_PARAM_1 );
	save->param2 = FLDMMDL_GetParam( fmmdl, FLDMMDL_PARAM_2 );
	save->move_limit_x = FLDMMDL_GetMoveLimitX( fmmdl );
	save->move_limit_z = FLDMMDL_GetMoveLimitZ( fmmdl );
	save->gx_init = FLDMMDL_GetInitGridPosX( fmmdl );
	save->gy_init = FLDMMDL_GetInitGridPosY( fmmdl );
	save->gz_init = FLDMMDL_GetInitGridPosZ( fmmdl );
	save->gx_now = FLDMMDL_GetGridPosX( fmmdl );
	save->gy_now = FLDMMDL_GetGridPosY( fmmdl );
	save->gz_now = FLDMMDL_GetGridPosZ( fmmdl );
	save->fx32_y = FLDMMDL_GetVectorPosY( fmmdl );
	
	MI_CpuCopy8( FLDMMDL_GetMoveProcWork((FLDMMDL*)fmmdl),
		save->move_proc_work, FLDMMDL_MOVE_WORK_SIZE );
	MI_CpuCopy8( FLDMMDL_GetMoveSubProcWork((FLDMMDL*)fmmdl),
		save->move_sub_proc_work, FLDMMDL_MOVE_SUB_WORK_SIZE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL_SAVEDATA ���샂�f���@���[�h
 * @param	fldmmdl		�Z�[�u����FLDMMDL*
 * @param	save FLDMMDL_SAVEWORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_SaveData_LoadFldMMdl(
	FLDMMDL *fmmdl, const FLDMMDL_SAVEWORK *save, const FLDMMDLSYS *fos )
{
	FldMMdl_ClearWork( fmmdl );

	fmmdl->status_bit = save->status_bit;
	fmmdl->move_bit = save->move_bit;
	FLDMMDL_SetOBJID( fmmdl, save->obj_id );
	FLDMMDL_SetZoneID( fmmdl, save->zone_id );
	FLDMMDL_SetOBJCode( fmmdl, save->obj_code ); 
	FLDMMDL_SetMoveCode( fmmdl, save->move_code );
	FLDMMDL_SetEventType( fmmdl, save->event_type );
	FLDMMDL_SetEventFlag( fmmdl, save->event_flag );
	FLDMMDL_SetEventID( fmmdl, save->event_id );
	fmmdl->dir_head = save->dir_head;
	FLDMMDL_SetForceDirDisp( fmmdl, save->dir_disp );
	FLDMMDL_SetDirMove( fmmdl, save->dir_move );
	FLDMMDL_SetParam( fmmdl, save->param0, FLDMMDL_PARAM_0 );
	FLDMMDL_SetParam( fmmdl, save->param1, FLDMMDL_PARAM_1 );
	FLDMMDL_SetParam( fmmdl, save->param2, FLDMMDL_PARAM_2 );
	FLDMMDL_SetMoveLimitX( fmmdl, save->move_limit_x );
	FLDMMDL_SetMoveLimitZ( fmmdl, save->move_limit_z );
	FLDMMDL_SetInitGridPosX( fmmdl, save->gx_init );
	FLDMMDL_SetInitGridPosY( fmmdl, save->gy_init );
	FLDMMDL_SetInitGridPosZ( fmmdl, save->gz_init );
	FLDMMDL_SetGridPosX( fmmdl, save->gx_now );
	FLDMMDL_SetGridPosY( fmmdl, save->gy_now );
	FLDMMDL_SetGridPosZ( fmmdl, save->gz_now );
	
	MI_CpuCopy8( save->move_proc_work,
		FLDMMDL_GetMoveProcWork((FLDMMDL*)fmmdl), FLDMMDL_MOVE_WORK_SIZE );
	MI_CpuCopy8( save->move_sub_proc_work,
		FLDMMDL_GetMoveSubProcWork((FLDMMDL*)fmmdl), FLDMMDL_MOVE_SUB_WORK_SIZE );
	
	fmmdl->pFldMMdlSys = fos;
	
	{ //���W���A
		s16 grid;
		VecFx32 pos = {0,0,0};
		
		grid = FLDMMDL_GetGridPosX( fmmdl );
		FLDMMDL_SetOldGridPosX( fmmdl, grid );
		pos.x = GRID_SIZE_FX32( grid ) + FLDMMDL_VEC_X_GRID_OFFS_FX32;
		
		grid = FLDMMDL_GetGridPosY( fmmdl );
		FLDMMDL_SetOldGridPosY( fmmdl, grid );
		pos.y = save->fx32_y; //�Z�[�u���̍�����M�p����
	
		grid = FLDMMDL_GetGridPosZ( fmmdl );
		FLDMMDL_SetOldGridPosZ( fmmdl, grid );
		pos.z = GRID_SIZE_FX32( grid ) + FLDMMDL_VEC_Z_GRID_OFFS_FX32;
	
		FLDMMDL_SetVectorPos( fmmdl, &pos );
	}

	{ //�X�e�[�^�X�r�b�g���A
		FLDMMDL_OnStatusBit( fmmdl,
			FLDMMDL_STABIT_USE |
//			FLDMMDL_STABIT_HEIGHT_GET_NEED | //�Z�[�u���̍�����M�p����
			FLDMMDL_STABIT_MOVE_START );
		
		FLDMMDL_OffStatusBit( fmmdl,
			FLDMMDL_STABIT_PAUSE_MOVE |
			FLDMMDL_STABIT_VANISH |
			FLDMMDL_STABIT_DRAW_PROC_INIT_COMP |
			FLDMMDL_STABIT_JUMP_START |
			FLDMMDL_STABIT_JUMP_END |
			FLDMMDL_STABIT_MOVE_END |
			FLDMMDL_STABIT_FELLOW_HIT_NON |
			FLDMMDL_STABIT_TALK_OFF |
			FLDMMDL_STABIT_DRAW_PUSH |
			FLDMMDL_STABIT_BLACT_ADD_PRAC |
			FLDMMDL_STABIT_HEIGHT_GET_OFF );
		
		FLDMMDL_OffStatusBit( fmmdl,
			FLDMMDL_STABIT_SHADOW_SET |
			FLDMMDL_STABIT_SHADOW_VANISH |
			FLDMMDL_STABIT_EFFSET_SHOAL	 |
			FLDMMDL_STABIT_REFLECT_SET );
		
		if( FLDMMDL_CheckMoveBit(fmmdl,FLDMMDL_MOVEBIT_MOVEPROC_INIT) == 0 ){
			FLDMMDL_OnMoveBit( fmmdl, FLDMMDL_MOVEBIT_NEED_MOVEPROC_RECOVER );
		}
	}
}


//======================================================================
//	FLDMMDL ����֐�
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMMDL ���암�����s
 * @param	fmmdl	FLDMMDL*
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_UpdateMoveProc( FLDMMDL *fmmdl )
{
	FLDMMDL_UpdateMove( fmmdl );
	
	if( FLDMMDL_CheckStatusBitUse(fmmdl) == TRUE ){
		FldMMdl_TCB_DrawProc( fmmdl );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL TCB ����֐�
 * @param	tcb		GFL_TCB *
 * @param	work	tcb work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_TCB_MoveProc( GFL_TCB * tcb, void *work )
{
	FLDMMDL *fmmdl = (FLDMMDL *)work;
	FLDMMDL_UpdateMoveProc( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDL TCB ����֐�����Ă΂��`��֐�
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_TCB_DrawProc( FLDMMDL * fmmdl )
{
	const FLDMMDLSYS *fos = FLDMMDL_GetFldMMdlSys(fmmdl);
	
	if( FLDMMDLSYS_CheckCompleteDrawInit(fos) == TRUE ){
		FLDMMDL_UpdateDraw( fmmdl );
	}
}

//======================================================================
//	FLDMMDL �A�j���[�V�����R�}���h
//======================================================================
//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h���\���`�F�b�N
 * @param	fmmdl		�ΏۂƂȂ�FLDMMDL * 
 * @retval	int			TRUE=�\�BFALSE=����
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckPossibleAcmd( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_USE) == 0 ){
		return( FALSE );
	}
	
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_MOVE) ){
		return( FALSE );
	}
	
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_ACMD) &&
		FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_ACMD_END) == 0 ){
		return( FALSE );
	}
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h�Z�b�g
 * @param	fmmdl		�ΏۂƂȂ�FLDMMDL * 
 * @param	code		���s����R�[�h�BAC_DIR_U��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetAcmd( FLDMMDL * fmmdl, u16 code )
{
	GF_ASSERT( code < ACMD_MAX );
	FLDMMDL_SetAcmdCode( fmmdl, code );
	FLDMMDL_SetAcmdSeq( fmmdl, 0 );
	FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_ACMD );
	FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_ACMD_END );
}

//--------------------------------------------------------------
/**
 * �R�}���h�Z�b�g
 * @param	fmmdl		�ΏۂƂȂ�FLDMMDL * 
 * @param	code		���s����R�[�h�BAC_DIR_U��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetLocalAcmd( FLDMMDL * fmmdl, u16 code )
{
	FLDMMDL_SetAcmdCode( fmmdl, code );
	FLDMMDL_SetAcmdSeq( fmmdl, 0 );
	FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_ACMD_END );
}

//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h�I���`�F�b�N�B
 * @param	fmmdl		�ΏۂƂȂ�FLDMMDL * 
 * @retval	int			TRUE=�I��
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckEndAcmd( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_ACMD) == 0 ){
		return( TRUE );
	}
	
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_ACMD_END) == 0 ){
		return( FALSE );
	}
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h�I���`�F�b�N�ƊJ���B
 * �A�j���[�V�����R�}���h���I�����Ă��Ȃ��ꍇ�͊J������Ȃ��B
 * @param	fmmdl		�ΏۂƂȂ�FLDMMDL * 
 * @retval	BOOL	TRUE=�I�����Ă���B
 */
//--------------------------------------------------------------
BOOL FLDMMDL_EndAcmd( FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_ACMD) == 0 ){
		return( TRUE );
	}
	
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_ACMD_END) == 0 ){
		return( FALSE );
	}
	
	FLDMMDL_OffStatusBit(
		fmmdl, FLDMMDL_STABIT_ACMD|FLDMMDL_STABIT_ACMD_END );
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h�J���B
 * �A�j���[�V�����R�}���h���I�����Ă��Ȃ��Ƃ������J���B
 * @param	fmmdl		�ΏۂƂȂ�FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_FreeAcmd( FLDMMDL * fmmdl )
{
	FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_ACMD );
	FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_ACMD_END ); //���[�J���R�}���h�t���O
	FLDMMDL_SetAcmdCode( fmmdl, ACMD_NOT );
	FLDMMDL_SetAcmdSeq( fmmdl, 0 );
}

//======================================================================
//	FLDMMDLSYS �p�����^�ݒ�A�Q��
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMMDLSYS �X�e�[�^�X�r�b�g�`�F�b�N
 * @param	fmmdlsys	FLDMMDLSYS*
 * @param	bit	FLDMMDLSYS_STABIT
 * @retval	u32	(status bit & bit)
 */
//--------------------------------------------------------------
u32 FLDMMDLSYS_CheckStatusBit(
	const FLDMMDLSYS *fmmdlsys, FLDMMDLSYS_STABIT bit )
{
	return( (fmmdlsys->status_bit&bit) );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS �X�e�[�^�X�r�b�g ON
 * @param	fmmdlsys	FLDMMDLSYS*
 * @param	bit	FLDMMDLSYS_STABIT
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdlSys_OnStatusBit(
	FLDMMDLSYS *fmmdlsys, FLDMMDLSYS_STABIT bit )
{
	fmmdlsys->status_bit |= bit;
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS �X�e�[�^�X�r�b�g OFF
 * @param	fmmdlsys	FLDMMDLSYS*
 * @param	bit	FLDMMDLSYS_STABIT
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdlSys_OffStatusBit(
	FLDMMDLSYS *fmmdlsys, FLDMMDLSYS_STABIT bit )
{
	fmmdlsys->status_bit &= ~bit;
}

//--------------------------------------------------------------
/**
 * FMMDLSYS ���샂�f���ő吔���擾
 * @param	fmmdlsys	FLDMMDLSYS*
 * @retval	u16	�ő吔
 */
//--------------------------------------------------------------
u16 FLDMMDLSYS_GetFldMMdlMax( const FLDMMDLSYS *fmmdlsys )
{
	return( fmmdlsys->fmmdl_max );
}

//--------------------------------------------------------------
/**
 * FMMDLSYS ���ݔ������Ă��铮�샂�f���̐����擾
 * @param	fmmdlsys	FLDMMDLSYS*
 * @retval	u16	������
 */
//--------------------------------------------------------------
u16 FLDMMDLSYS_GetFldMMdlCount( const FLDMMDLSYS *fmmdlsys )
{
	return( fmmdlsys->fmmdl_count );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS TCB�v���C�I���e�B���擾
 * @param	fmmdlsys	FLDMMDLSYS*
 * @retval	u16	TCB�v���C�I���e�B
 */
//--------------------------------------------------------------
u16 FLDMMDLSYS_GetTCBPriority( const FLDMMDLSYS *fmmdlsys )
{
	return( fmmdlsys->tcb_pri );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS HEAPID�擾
 * @param	fmmdlsys	FLDMMDLSYS*
 * @retval	HEAPID HEAPID
 */
//--------------------------------------------------------------
HEAPID FLDMMDLSYS_GetHeapID( const FLDMMDLSYS *fmmdlsys )
{
	return( fmmdlsys->heapID );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS ���ݔ������Ă���OBJ����1����
 * @param	fmmdlsys	FLDMMDLSYS*
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdlSys_IncrementOBJCount( FLDMMDLSYS *fmmdlsys )
{
	fmmdlsys->fmmdl_count++;
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS ���ݔ������Ă���OBJ����1����
 * @param	fmmdlsys	FLDMMDLSYS*
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdlSys_DecrementOBJCount( FLDMMDLSYS *fmmdlsys )
{
	fmmdlsys->fmmdl_count--;
	GF_ASSERT( fmmdlsys->fmmdl_count >= 0 );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS GFL_TCBSYS*�擾
 * @param	fos	FLDMMDLSYS*
 * @retval	GFL_TCBSYS*
 */
//--------------------------------------------------------------
GFL_TCBSYS * FLDMMDLSYS_GetTCBSYS( FLDMMDLSYS *fos )
{
	return( fos->pTCBSys );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS FLDMMDL_BLACTCONT�Z�b�g
 * @param	fmmdlsys	FLDMMDLSYS
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_SetBlActCont(
	FLDMMDLSYS *fmmdlsys, FLDMMDL_BLACTCONT *pBlActCont )
{
	fmmdlsys->pBlActCont = pBlActCont;
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS FLDMMDL_BLACTCONT�擾
 * @param	fmmdlsys FLDMMDLSYS
 * @retval	FLDMMDL_BLACTCONT*
 */
//--------------------------------------------------------------
FLDMMDL_BLACTCONT * FLDMMDLSYS_GetBlActCont( FLDMMDLSYS *fmmdlsys )
{
	GF_ASSERT( fmmdlsys->pBlActCont != NULL );
	return( fmmdlsys->pBlActCont );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS FLDMAPPER�擾
 * @param	fmmdlsys	FLDMMDLSYS
 * @retval	FLDMAPPER* FLDMAPPER*
 */
//--------------------------------------------------------------
const FLDMAPPER * FLDMMDLSYS_GetG3DMapper( const FLDMMDLSYS *fos )
{
	GF_ASSERT( fos->pG3DMapper != NULL);
	return( fos->pG3DMapper );
}

//======================================================================
//	FLDMMDL�@�p�����^�Q�ƁA�ݒ�
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMMDL �X�e�[�^�X�r�b�gON
 * @param	fmmdl	fmmdl
 * @param	bit		FLDMMDL_STABIT
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OnStatusBit( FLDMMDL *fmmdl, FLDMMDL_STABIT bit )
{
	fmmdl->status_bit |= bit;
}

//--------------------------------------------------------------
/**
 * FLDMMDL �X�e�[�^�X�r�b�gOFF
 * @param	fmmdl	fmmdl
 * @param	bit		FLDMMDL_STABIT
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OffStatusBit( FLDMMDL *fmmdl, FLDMMDL_STABIT bit )
{
	fmmdl->status_bit &= ~bit;
}

//--------------------------------------------------------------
/**
 * FLDMMDL �X�e�[�^�X�r�b�g�擾
 * @param	fmmdl		FLDMMDL * 
 * @retval	FLDMMDL_STABIT �X�e�[�^�X�r�b�g
 */
//--------------------------------------------------------------
FLDMMDL_STABIT FLDMMDL_GetStatusBit( const FLDMMDL * fmmdl )
{
	return( fmmdl->status_bit );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �X�e�[�^�X�r�b�g�`�F�b�N
 * @param	fmmdl	FLDMMDL*
 * @param	bit	FLDMMDL_STABIT
 * @retval	u32	(status bit & bit)
 */
//--------------------------------------------------------------
u32 FLDMMDL_CheckStatusBit( const FLDMMDL *fmmdl, FLDMMDL_STABIT bit )
{
	return( (fmmdl->status_bit&bit) );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ����r�b�g�@�擾
 * @param	fmmdl	FLDMMDL *
 * @retval	u32		����r�b�g
 */
//--------------------------------------------------------------
FLDMMDL_MOVEBIT FLDMMDL_GetMoveBit( const FLDMMDL * fmmdl )
{
	return( fmmdl->move_bit );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ����r�b�g�@ON
 * @param	fmmdl	FLDMMDL *
 * @param	bit		���Ă�r�b�g FLDMMDL_MOVEBIT_NON��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OnMoveBit( FLDMMDL * fmmdl, FLDMMDL_MOVEBIT bit )
{
	fmmdl->move_bit |= bit;
}

//--------------------------------------------------------------
/**
 * FLDMMDL ����r�b�g�@OFF
 * @param	fmmdl	FLDMMDL *
 * @param	bit		���낷�r�b�g FLDMMDL_MOVEBIT_NON��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OffMoveBit( FLDMMDL * fmmdl, FLDMMDL_MOVEBIT bit )
{
	fmmdl->move_bit &= ~bit;
}

//--------------------------------------------------------------
/**
 * FLDMMDL ����r�b�g�@�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @param	bit		�`�F�b�N����r�b�g FLDMMDL_MOVEBIT_NON��
 * @retval	nothing
 */
//--------------------------------------------------------------
u32 FLDMMDL_CheckMoveBit( const FLDMMDL * fmmdl, FLDMMDL_MOVEBIT bit )
{
	return( (fmmdl->move_bit & bit) );
}

//--------------------------------------------------------------
/**
 * FLDMMDL OBJ ID�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	id		obj id
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetOBJID( FLDMMDL * fmmdl, u16 obj_id )
{
	fmmdl->obj_id = obj_id;
}

//--------------------------------------------------------------
/**
 * FLDMMDL OBJ ID�擾
 * @param	fmmdl	FLDMMDL *
 * @retval	u16		OBJ ID
 */
//--------------------------------------------------------------
u16 FLDMMDL_GetOBJID( const FLDMMDL * fmmdl )
{
	return( fmmdl->obj_id );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ZONE ID�Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @param	zone_id	�]�[��ID
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetZoneID( FLDMMDL * fmmdl, u16 zone_id )
{
	fmmdl->zone_id = zone_id;
}

//--------------------------------------------------------------
/**
 * FLDMMDL ZONE ID�擾
 * @param	fmmdl	FLDMMDL *
 * @retval	int		ZONE ID
 */
//--------------------------------------------------------------
u16 FLDMMDL_GetZoneID( const FLDMMDL * fmmdl )
{
	return( fmmdl->zone_id );
}

//--------------------------------------------------------------
/**
 * FLDMMDL OBJ�R�[�h�Z�b�g
 * @param	fmmdl			FLDMMDL * 
 * @param	code			�Z�b�g����R�[�h
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetOBJCode( FLDMMDL * fmmdl, u16 code )
{
	fmmdl->obj_code = code;
}

//--------------------------------------------------------------
/**
 * FLDMMDL OBJ�R�[�h�擾
 * @param	fmmdl			FLDMMDL * 
 * @retval	u16				OBJ�R�[�h
 */
//--------------------------------------------------------------
u16 FLDMMDL_GetOBJCode( const FLDMMDL * fmmdl )
{
	return( fmmdl->obj_code );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ����R�[�h�Z�b�g
 * @param	fmmdl			FLDMMDL * 
 * @param	code			����R�[�h
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetMoveCode( FLDMMDL * fmmdl, u16 code )
{
	fmmdl->move_code = code;
}

//--------------------------------------------------------------
/**
 * FLDMMDL ����R�[�h�擾
 * @param	fmmdl			FLDMMDL * 
 * @retval	u32				����R�[�h
 */
//--------------------------------------------------------------
u16 FLDMMDL_GetMoveCode( const FLDMMDL * fmmdl )
{
	return( fmmdl->move_code );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �C�x���g�^�C�v�Z�b�g
 * @param	fmmdl		FLDMMDL * 
 * @param	type		Event Type
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetEventType( FLDMMDL * fmmdl, u16 type )
{
	fmmdl->event_type = type;
}

//--------------------------------------------------------------
/**
 * FLDMMDL �C�x���g�^�C�v�擾
 * @param	fmmdl		FLDMMDL * 
 * @retval	u32			Event Type
 */
//--------------------------------------------------------------
u16 FLDMMDL_GetEventType( const FLDMMDL * fmmdl )
{
	return( fmmdl->event_type );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �C�x���g�t���O�Z�b�g
 * @param	fmmdl		FLDMMDL * 
 * @param	flag		Event Flag
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetEventFlag( FLDMMDL * fmmdl, u16 flag )
{
	fmmdl->event_flag = flag;
}

//--------------------------------------------------------------
/**
 * FLDMMDL �C�x���g�t���O�擾
 * @param	fmmdl		FLDMMDL * 
 * @retval	u16			Event Flag
 */
//--------------------------------------------------------------
u16 FLDMMDL_GetEventFlag( const FLDMMDL * fmmdl )
{
	return( fmmdl->event_flag );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �C�x���gID�Z�b�g
 * @param	fmmdl		FLDMMDL *
 * @param	id			Event ID
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetEventID( FLDMMDL * fmmdl, u16 id )
{
	fmmdl->event_id = id;
}

//--------------------------------------------------------------
/**
 * FLDMMDL �C�x���gID�擾
 * @param	fmmdl		FLDMMDL * 
 * @retval	u16			Event ID
 */
//--------------------------------------------------------------
u16 FLDMMDL_GetEventID( const FLDMMDL * fmmdl )
{
	return( fmmdl->event_id );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �C�x���gID���G�C���A�X���ǂ����`�F�b�N
 * @param	fmmdl		FLDMMDL * 
 * @retval	int			TRUE=�G�C���A�XID�ł��� FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckAliesEventID( const FLDMMDL * fmmdl )
{
	u16 id = (u16)FLDMMDL_GetEventID( fmmdl );
	
	if( id == SP_SCRID_ALIES ){
		return( TRUE );
	}

	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �w�b�_�[�w������擾
 * @param	fmmdl		FLDMMDL * 
 * @retval	u32			DIR_UP��
 */
//--------------------------------------------------------------
u32 FLDMMDL_GetDirHeader( const FLDMMDL * fmmdl )
{
	return( fmmdl->dir_head );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �\�������Z�b�g�@����
 * @param	fmmdl			FLDMMDL * 
 * @param	dir				DIR_UP��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetForceDirDisp( FLDMMDL * fmmdl, u16 dir )
{
	fmmdl->dir_disp_old = fmmdl->dir_disp;
	fmmdl->dir_disp = dir;
}

//--------------------------------------------------------------
/**
 * FLDMMDL �\�������Z�b�g
 * @param	fmmdl			FLDMMDL * 
 * @param	dir				DIR_UP��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetDirDisp( FLDMMDL * fmmdl, u16 dir )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_PAUSE_DIR) == 0 ){
		fmmdl->dir_disp_old = fmmdl->dir_disp;
		fmmdl->dir_disp = dir;
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL �\�������擾
 * @param	fmmdl	FLDMMDL * 
 * @retval	u16 	DIR_UP��
 */
//--------------------------------------------------------------
u16 FLDMMDL_GetDirDisp( const FLDMMDL * fmmdl )
{
	return( fmmdl->dir_disp );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �ߋ��\�������擾
 * @param	fmmdl			FLDMMDL * 
 * @retval	dir				DIR_UP��
 */
//--------------------------------------------------------------
u16 FLDMMDL_GetDirDispOld( const FLDMMDL * fmmdl )
{
	return( fmmdl->dir_disp_old );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �ړ������Z�b�g
 * @param	fmmdl			FLDMMDL * 
 * @param	dir				DIR_UP��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetDirMove( FLDMMDL * fmmdl, u16 dir )
{
	fmmdl->dir_move_old = fmmdl->dir_move;
	fmmdl->dir_move = dir;
}

//--------------------------------------------------------------
/**
 * FLDMMDL �ړ������擾
 * @param	fmmdl			FLDMMDL * 
 * @retval	u16		DIR_UP��
 */
//--------------------------------------------------------------
u16 FLDMMDL_GetDirMove( const FLDMMDL * fmmdl )
{
	return( fmmdl->dir_move );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �ߋ��ړ������擾
 * @param	fmmdl			FLDMMDL * 
 * @retval	u16	DIR_UP��
 */
//--------------------------------------------------------------
u16 FLDMMDL_GetDirMoveOld( const FLDMMDL * fmmdl )
{
	return( fmmdl->dir_move_old );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �\���A�ړ������Z�b�g
 * @param	fmmdl			FLDMMDL * 
 * @param	dir				DIR_UP��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetDirAll( FLDMMDL * fmmdl, u16 dir )
{
	FLDMMDL_SetDirDisp( fmmdl, dir );
	FLDMMDL_SetDirMove( fmmdl, dir );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �w�b�_�[�w��p�����^�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	param	�p�����^
 * @param	no		�Z�b�g����p�����^�ԍ��@FLDMMDL_PARAM_0��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetParam( FLDMMDL *fmmdl, int param, FLDMMDL_H_PARAM no )
{
	switch( no ){
	case FLDMMDL_PARAM_0: fmmdl->param0 = param; break;
	case FLDMMDL_PARAM_1: fmmdl->param1 = param; break;
	case FLDMMDL_PARAM_2: fmmdl->param2 = param; break;
	default: GF_ASSERT( 0 );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL �w�b�_�[�w��p�����^�擾
 * @param	fmmdl			FLDMMDL *
 * @param	param			FLDMMDL_PARAM_0��
 * @retval	int				�p�����^
 */
//--------------------------------------------------------------
int FLDMMDL_GetParam( const FLDMMDL * fmmdl, FLDMMDL_H_PARAM param )
{
	switch( param ){
	case FLDMMDL_PARAM_0: return( fmmdl->param0 );
	case FLDMMDL_PARAM_1: return( fmmdl->param1 );
	case FLDMMDL_PARAM_2: return( fmmdl->param2 );
	}
	
	GF_ASSERT( 0 );
	return( 0 );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �ړ�����X�Z�b�g
 * @param	fmmdl			FLDMMDL * 
 * @param	x				�ړ�����
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetMoveLimitX( FLDMMDL * fmmdl, s16 x )
{
	fmmdl->move_limit_x = x;
}

//--------------------------------------------------------------
/**
 * FLDMMDL �ړ�����X�擾
 * @param	fmmdl		FLDMMDL * 
 * @retval	s16			�ړ�����X
 */
//--------------------------------------------------------------
s16 FLDMMDL_GetMoveLimitX( const FLDMMDL * fmmdl )
{
	return( fmmdl->move_limit_x );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �ړ�����Z�Z�b�g
 * @param	fmmdl			FLDMMDL * 
 * @param	z				�ړ�����
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetMoveLimitZ( FLDMMDL * fmmdl, s16 z )
{
	fmmdl->move_limit_z = z;
}

//--------------------------------------------------------------
/**
 * FLDMMDL �ړ�����Z�擾
 * @param	fmmdl		FLDMMDL * 
 * @retval	s16		�ړ�����z
 */
//--------------------------------------------------------------
s16 FLDMMDL_GetMoveLimitZ( const FLDMMDL * fmmdl )
{
	return( fmmdl->move_limit_z );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �`��X�e�[�^�X�Z�b�g
 * @param	fmmdl		FLDMMDL * 
 * @param	st			DRAW_STA_STOP��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetDrawStatus( FLDMMDL * fmmdl, u16 st )
{
	fmmdl->draw_status = st;
}

//--------------------------------------------------------------
/**
 * FLDMMDL �`��X�e�[�^�X�擾
 * @param	fmmdl		FLDMMDL * 
 * @retval	u16			DRAW_STA_STOP��
 */
//--------------------------------------------------------------
u16 FLDMMDL_GetDrawStatus( const FLDMMDL * fmmdl )
{
	return( fmmdl->draw_status );
}

//--------------------------------------------------------------
/**
 * FLDMMDL FLDMMDLSYS *�擾
 * @param	fmmdl			FLDMMDL * 
 * @retval	FLDMMDLSYS	FLDMMDLSYS *
 */
//--------------------------------------------------------------
const FLDMMDLSYS * FLDMMDL_GetFldMMdlSys( const FLDMMDL *fmmdl )
{
	return( fmmdl->pFldMMdlSys );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ����֐��p���[�N�������B
 * size�����[�N�T�C�Y�𒴂��Ă����ꍇ�AASSERT�B
 * ����p���[�N��size���A0�ŏ������B
 * @param	fmmdl	FLDMMDL * 
 * @param	size	�K�v�ȃ��[�N�T�C�Y
 * @retval	void*	���������ꂽ���[�N
 */
//--------------------------------------------------------------
void * FLDMMDL_InitMoveProcWork( FLDMMDL * fmmdl, int size )
{
	void *work;
	GF_ASSERT( size <= FLDMMDL_MOVE_WORK_SIZE );
	work = FLDMMDL_GetMoveProcWork( fmmdl );
	GFL_STD_MemClear( work, size );
	return( work );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ����֐��p���[�N�擾�B
 * @param	fmmdl	FLDMMDL * 
 * @retval	void*	���[�N
 */
//--------------------------------------------------------------
void * FLDMMDL_GetMoveProcWork( FLDMMDL * fmmdl )
{
	return( fmmdl->move_proc_work );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ����⏕�֐��p���[�N�������B
 * size�����[�N�T�C�Y�𒴂��Ă����ꍇ�AASSERT�B
 * @param	fmmdl	FLDMMDL * 
 * @param	size	�K�v�ȃ��[�N�T�C�Y
 * @retval	void*	���������ꂽ���[�N
 */
//--------------------------------------------------------------
void * FLDMMDL_InitMoveSubProcWork( FLDMMDL * fmmdl, int size )
{
	u8 *work;
	
	GF_ASSERT( size <= FLDMMDL_MOVE_SUB_WORK_SIZE );
	work = FLDMMDL_GetMoveSubProcWork( fmmdl );
	GFL_STD_MemClear( work, size );
	return( work );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ����⏕�֐��p���[�N�擾
 * @param	fmmdl	FLDMMDL * 
 * @retval	void*	���[�N*
 */
//--------------------------------------------------------------
void * FLDMMDL_GetMoveSubProcWork( FLDMMDL * fmmdl )
{
	return( fmmdl->move_sub_proc_work );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ����R�}���h�p���[�N�������B
 * size�����[�N�T�C�Y�𒴂��Ă����ꍇ�AASSERT�B
 * @param	fmmdl	FLDMMDL * 
 * @param	size	�K�v�ȃ��[�N�T�C�Y
 * @retval	void*	���������ꂽ���[�N
 */
//--------------------------------------------------------------
void * FLDMMDL_InitMoveCmdWork( FLDMMDL * fmmdl, int size )
{
	u8 *work;
	
	GF_ASSERT( size <= FLDMMDL_MOVE_CMD_WORK_SIZE );
	work = FLDMMDL_GetMoveCmdWork( fmmdl );
	GFL_STD_MemClear( work, size );
	return( work );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ����R�}���h�p���[�N�擾
 * @param	fmmdl	FLDMMDL *
 * @retval	void*	���[�N*
 */
//--------------------------------------------------------------
void * FLDMMDL_GetMoveCmdWork( FLDMMDL * fmmdl )
{
	return( fmmdl->move_cmd_proc_work );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �`��֐��p���[�N�������B
 * size�����[�N�T�C�Y�𒴂��Ă����ꍇ�AASSERT�B
 * @param	fmmdl	FLDMMDL * 
 * @param	size	�K�v�ȃ��[�N�T�C�Y
 * @retval	void*	���������ꂽ���[�N
 */
//--------------------------------------------------------------
void * FLDMMDL_InitDrawProcWork( FLDMMDL * fmmdl, int size )
{
	u8 *work;
	
	GF_ASSERT( size <= FLDMMDL_DRAW_WORK_SIZE );
	work = FLDMMDL_GetDrawProcWork( fmmdl );
	GFL_STD_MemClear( work, size );
	return( work );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �`��֐��p���[�N�擾
 * @param	fmmdl	FLDMMDL * 
 * @retval	void*	���[�N
 */
//--------------------------------------------------------------
void * FLDMMDL_GetDrawProcWork( FLDMMDL * fmmdl )
{
	return( fmmdl->draw_proc_work );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ���쏉�����֐����s
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_CallMoveInitProc( FLDMMDL * fmmdl )
{
	GF_ASSERT( fmmdl->move_proc_list );
	GF_ASSERT( fmmdl->move_proc_list->init_proc );
	fmmdl->move_proc_list->init_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ����֐����s
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_CallMoveProc( FLDMMDL * fmmdl )
{
	GF_ASSERT( fmmdl->move_proc_list );
	GF_ASSERT( fmmdl->move_proc_list->move_proc );
	fmmdl->move_proc_list->move_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �폜�֐����s
 * @param	fmmdl	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_CallMoveDeleteProc( FLDMMDL * fmmdl )
{
	GF_ASSERT( fmmdl->move_proc_list );
	GF_ASSERT( fmmdl->move_proc_list->delete_proc );
	fmmdl->move_proc_list->delete_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ���A�֐����s
 * @param	fmmdl	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_CallMovePopProc( FLDMMDL * fmmdl )
{
	GF_ASSERT( fmmdl->move_proc_list );
	GF_ASSERT( fmmdl->move_proc_list->recover_proc );
	fmmdl->move_proc_list->recover_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �`�揉�����֐����s
 * @param	fmmdl	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_CallDrawInitProc( FLDMMDL * fmmdl )
{
	GF_ASSERT( fmmdl->draw_proc_list );
	GF_ASSERT( fmmdl->draw_proc_list->init_proc );
	fmmdl->draw_proc_list->init_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �`��֐����s
 * @param	fmmdl	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_CallDrawProc( FLDMMDL * fmmdl )
{
	GF_ASSERT( fmmdl->draw_proc_list );
	GF_ASSERT( fmmdl->draw_proc_list->draw_proc );
	fmmdl->draw_proc_list->draw_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �`��폜�֐����s
 * @param	fmmdl	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_CallDrawDeleteProc( FLDMMDL * fmmdl )
{
	GF_ASSERT( fmmdl->draw_proc_list );
	GF_ASSERT( fmmdl->draw_proc_list->delete_proc );
	fmmdl->draw_proc_list->delete_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �`��ޔ��֐����s
 * @param	fmmdl	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_CallDrawPushProc( FLDMMDL * fmmdl )
{
	GF_ASSERT( fmmdl->draw_proc_list );
	GF_ASSERT( fmmdl->draw_proc_list->push_proc );
	fmmdl->draw_proc_list->push_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �`�敜�A�֐����s
 * @param	fmmdl	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_CallDrawPopProc( FLDMMDL * fmmdl )
{
	GF_ASSERT( fmmdl->draw_proc_list );
	GF_ASSERT( fmmdl->draw_proc_list->pop_proc );
	fmmdl->draw_proc_list->pop_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �`��擾�֐����s
 * @param	fmmdl	FLDMMDL*
 * @param	state	�擾�֐��ɗ^������
 * @retval	nothing
 */
//--------------------------------------------------------------
u32 FLDMMDL_CallDrawGetProc( FLDMMDL *fmmdl, u32 state )
{
	GF_ASSERT( fmmdl->draw_proc_list );
	GF_ASSERT( fmmdl->draw_proc_list->get_proc );
	return( fmmdl->draw_proc_list->get_proc(fmmdl,state) );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �A�j���[�V�����R�}���h�R�[�h�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	code	AC_DIR_U��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetAcmdCode( FLDMMDL * fmmdl, u16 code )
{
	fmmdl->acmd_code = code;
}

//--------------------------------------------------------------
/**
 * FLDMMDL �A�j���[�V�����R�}���h�R�[�h�擾
 * @param	fmmdl	FLDMMDL * 
 * @retval	u16	AC_DIR_U��
 */
//--------------------------------------------------------------
u16 FLDMMDL_GetAcmdCode( const FLDMMDL * fmmdl )
{
	return( fmmdl->acmd_code );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �A�j���[�V�����R�}���h�V�[�P���X�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	no		�V�[�P���X
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetAcmdSeq( FLDMMDL * fmmdl, u16 no )
{
	fmmdl->acmd_seq = no;
}

//--------------------------------------------------------------
/**
 * FLDMMDL �A�j���[�V�����R�}���h�V�[�P���X����
 * @param	fmmdl	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_IncAcmdSeq( FLDMMDL * fmmdl )
{
	fmmdl->acmd_seq++;
}

//--------------------------------------------------------------
/**
 * FLDMMDL �A�j���[�V�����R�}���h�V�[�P���X�擾
 * @param	fmmdl	FLDMMDL * 
 * @retval	u16 �V�[�P���X
 */
//--------------------------------------------------------------
u16 FLDMMDL_GetAcmdSeq( const FLDMMDL * fmmdl )
{
	return( fmmdl->acmd_seq );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ���݂̃}�b�v�A�g���r���[�g���Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @param	attr	�Z�b�g����A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetMapAttr( FLDMMDL * fmmdl, u32 attr )
{
	fmmdl->now_attr = attr;
}

//--------------------------------------------------------------
/**
 * FLDMMDL ���݂̃}�b�v�A�g���r���[�g���擾
 * @param	fmmdl	FLDMMDL *
 * @retval	u32		�}�b�v�A�g���r���[�g
 */
//--------------------------------------------------------------
u32 FLDMMDL_GetMapAttr( const FLDMMDL * fmmdl )
{
	return( fmmdl->now_attr );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �ߋ��̃}�b�v�A�g���r���[�g���Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @param	attr	�Z�b�g����A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetMapAttrOld( FLDMMDL * fmmdl, u32 attr )
{
	fmmdl->old_attr = attr;
}

//--------------------------------------------------------------
/**
 * FLDMMDL �ߋ��̃}�b�v�A�g���r���[�g���擾
 * @param	fmmdl	FLDMMDL *
 * @retval	u32		�}�b�v�A�g���r���[�g
 */
//--------------------------------------------------------------
u32 FLDMMDL_GetMapAttrOld( const FLDMMDL * fmmdl )
{
	return( fmmdl->old_attr );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �G�C���A�X���̃]�[��ID�擾�B
 * ���G�C���A�X���̓C�x���g�t���O���w��]�[��ID
 * @param	fmmdl	FLDMMDL *
 * @retval	u16 �]�[��ID
 */
//--------------------------------------------------------------
u16 FLDMMDL_GetZoneIDAlies( const FLDMMDL * fmmdl )
{
	GF_ASSERT( FLDMMDL_CheckStatusBitAlies(fmmdl) == TRUE );
	return( FLDMMDL_GetEventFlag(fmmdl) );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �������W �O���b�hX���W�擾
 * @param	fmmdl	FLDMMDL * 
 * @retval	s16 X���W
 */
//--------------------------------------------------------------
s16 FLDMMDL_GetInitGridPosX( const FLDMMDL * fmmdl )
{
	return( fmmdl->gx_init );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �������W �O���b�hX���W�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	x		�Z�b�g������W
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetInitGridPosX( FLDMMDL * fmmdl, s16 x )
{
	fmmdl->gx_init = x;
}

//--------------------------------------------------------------
/**
 * FLDMMDL �������W Y���W�擾
 * @param	fmmdl	FLDMMDL * 
 * @retval	s16		Y
 */
//--------------------------------------------------------------
s16 FLDMMDL_GetInitGridPosY( const FLDMMDL * fmmdl )
{
	return( fmmdl->gy_init );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �������W Y���W�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	y		�Z�b�g������W
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetInitGridPosY( FLDMMDL * fmmdl, s16 y )
{
	fmmdl->gy_init = y;
}

//--------------------------------------------------------------
/**
 * FLDMMDL �������W z���W�擾
 * @param	fmmdl	FLDMMDL * 
 * @retval	s16		z���W
 */
//--------------------------------------------------------------
s16 FLDMMDL_GetInitGridPosZ( const FLDMMDL * fmmdl )
{
	return( fmmdl->gz_init );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �������W z���W�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	z		�Z�b�g������W
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetInitGridPosZ( FLDMMDL * fmmdl, s16 z )
{
	fmmdl->gz_init = z;
}

//--------------------------------------------------------------
/**
 * FLDMMDL �ߋ����W�@X���W�擾
 * @param	fmmdl	FLDMMDL * 
 * @retval	s16		X���W
 */
//--------------------------------------------------------------
s16 FLDMMDL_GetOldGridPosX( const FLDMMDL * fmmdl )
{
	return( fmmdl->gx_old );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �ߋ����W X���W�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	x		�Z�b�g������W
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetOldGridPosX( FLDMMDL * fmmdl, s16 x )
{
	fmmdl->gx_old = x;
}

//--------------------------------------------------------------
/**
 * FLDMMDL �ߋ����W Y���W�擾
 * @param	fmmdl	FLDMMDL * 
 * @retval	s16		Y
 */
//--------------------------------------------------------------
s16 FLDMMDL_GetOldGridPosY( const FLDMMDL * fmmdl )
{
	return( fmmdl->gy_old );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �ߋ����W Y���W�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	y		�Z�b�g������W
 * @retval	s16		y���W
 */
//--------------------------------------------------------------
void FLDMMDL_SetOldGridPosY( FLDMMDL * fmmdl, s16 y )
{
	fmmdl->gy_old = y;
}

//--------------------------------------------------------------
/**
 * FLDMMDL �ߋ����W z���W�擾
 * @param	fmmdl	FLDMMDL * 
 * @retval	s16		z���W
 */
//--------------------------------------------------------------
s16 FLDMMDL_GetOldGridPosZ( const FLDMMDL * fmmdl )
{
	return( fmmdl->gz_old );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �ߋ����W z���W�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	z		�Z�b�g������W
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetOldGridPosZ( FLDMMDL * fmmdl, s16 z )
{
	fmmdl->gz_old = z;
}

//--------------------------------------------------------------
/**
 * FLDMMDL ���ݍ��W X���W�擾
 * @param	fmmdl	FLDMMDL * 
 * @retval	s16		X���W
 */
//--------------------------------------------------------------
s16 FLDMMDL_GetGridPosX( const FLDMMDL * fmmdl )
{
	return( fmmdl->gx_now );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ���ݍ��W X���W�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	x		�Z�b�g������W
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetGridPosX( FLDMMDL * fmmdl, s16 x )
{
	fmmdl->gx_now = x;
}

//--------------------------------------------------------------
/**
 * FLDMMDL ���ݍ��W X���W����
 * @param	fmmdl	FLDMMDL * 
 * @param	x		�����l
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_AddGridPosX( FLDMMDL * fmmdl, s16 x )
{
	fmmdl->gx_now += x;
}

//--------------------------------------------------------------
/**
 * FLDMMDL ���ݍ��W Y���W�擾
 * @param	fmmdl	FLDMMDL * 
 * @retval	s16		Y
 */
//--------------------------------------------------------------
s16 FLDMMDL_GetGridPosY( const FLDMMDL * fmmdl )
{
	return( fmmdl->gy_now );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ���ݍ��W Y���W�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	y		�Z�b�g������W
 * @retval	s16		y���W
 */
//--------------------------------------------------------------
void FLDMMDL_SetGridPosY( FLDMMDL * fmmdl, s16 y )
{
	fmmdl->gy_now = y;
}

//--------------------------------------------------------------
/**
 * FLDMMDL ���ݍ��W Y���W����
 * @param	fmmdl	FLDMMDL * 
 * @param	y		�����l
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_AddGridPosY( FLDMMDL * fmmdl, s16 y )
{
	fmmdl->gy_now += y;
}

//--------------------------------------------------------------
/**
 * FLDMMDL �ߋ����W z���W�擾
 * @param	fmmdl	FLDMMDL * 
 * @retval	s16		z���W
 */
//--------------------------------------------------------------
s16 FLDMMDL_GetGridPosZ( const FLDMMDL * fmmdl )
{
	return( fmmdl->gz_now );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ���ݍ��W z���W�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	z		�Z�b�g������W
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetGridPosZ( FLDMMDL * fmmdl, s16 z )
{
	fmmdl->gz_now = z;
}

//--------------------------------------------------------------
/**
 * FLDMMDL ���ݍ��W z���W����
 * @param	fmmdl	FLDMMDL * 
 * @param	z		�����l
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_AddGridPosZ( FLDMMDL * fmmdl, s16 z )
{
	fmmdl->gz_now += z;
}

//--------------------------------------------------------------
/**
 * FLDMMDL �����W�|�C���^�擾
 * @param	fmmdl	FLDMMDL * 
 * @retval VecFx32*
 */
//--------------------------------------------------------------
const VecFx32 * FLDMMDL_GetVectorPosAddress( const FLDMMDL * fmmdl )
{
  return( &fmmdl->vec_pos_now );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �����W�擾
 * @param	fmmdl	FLDMMDL * 
 * @param	vec		���W�i�[��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_GetVectorPos( const FLDMMDL * fmmdl, VecFx32 *vec )
{
	*vec = fmmdl->vec_pos_now;
}

//--------------------------------------------------------------
/**
 * FLDMMDL �����W�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	vec		�Z�b�g���W
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetVectorPos( FLDMMDL * fmmdl, const VecFx32 *vec )
{
	fmmdl->vec_pos_now = *vec;
}

//--------------------------------------------------------------
/**
 * FLDMMDL �����WY�l�擾
 * @param	fmmdl	FLDMMDL * 
 * @retval	fx32	����
 */
//--------------------------------------------------------------
fx32 FLDMMDL_GetVectorPosY( const FLDMMDL * fmmdl )
{
	return( fmmdl->vec_pos_now.y );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �\�����W�I�t�Z�b�g�擾
 * @param	fmmdl	FLDMMDL * 
 * @param	vec		�Z�b�g���W
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_GetVectorDrawOffsetPos( const FLDMMDL * fmmdl, VecFx32 *vec )
{
	*vec = fmmdl->vec_draw_offs;
}

//--------------------------------------------------------------
/**
 * FLDMMDL �\�����W�I�t�Z�b�g�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	vec		�Z�b�g���W
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetVectorDrawOffsetPos( FLDMMDL * fmmdl, const VecFx32 *vec )
{
	fmmdl->vec_draw_offs = *vec;
}

//--------------------------------------------------------------
/**
 * FLDMMDL �O���w��\�����W�I�t�Z�b�g�擾
 * @param	fmmdl	FLDMMDL * 
 * @param	vec		�Z�b�g���W
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_GetVectorOuterDrawOffsetPos( const FLDMMDL * fmmdl, VecFx32 *vec )
{
	*vec = fmmdl->vec_draw_offs_outside;
}

//--------------------------------------------------------------
/**
 * FLDMMDL �O���w��\�����W�I�t�Z�b�g�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	vec		�Z�b�g���W
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetVectorOuterDrawOffsetPos( FLDMMDL * fmmdl, const VecFx32 *vec )
{
	fmmdl->vec_draw_offs_outside = *vec;
}

//--------------------------------------------------------------
/**
 * FLDMMDL �A�g���r���[�g�ω����W�I�t�Z�b�g�擾
 * @param	fmmdl	FLDMMDL * 
 * @param	vec		�Z�b�g���W
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_GetVectorAttrDrawOffsetPos( const FLDMMDL * fmmdl, VecFx32 *vec )
{
	*vec = fmmdl->vec_attr_offs;
}

//--------------------------------------------------------------
/**
 * FLDMMDL �A�g���r���[�g�ω����W�I�t�Z�b�g�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	vec		�Z�b�g���W
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetVectorAttrDrawOffsetPos( FLDMMDL * fmmdl, const VecFx32 *vec )
{
	fmmdl->vec_attr_offs = *vec;
}

//--------------------------------------------------------------
/**
 * FLDMMDL ����(�O���b�h�P��)���擾
 * @param	fmmdl	FLDMMDL *
 * @retval	s16		�����BH_GRID�P��
 */
//--------------------------------------------------------------
s16 FLDMMDL_GetHeightGrid( const FLDMMDL * fmmdl )
{
	fx32 y = FLDMMDL_GetVectorPosY( fmmdl );
	s16 gy = SIZE_GRID_FX32( y );
	return( gy );
}

//--------------------------------------------------------------
/**
 * FLDMMDL FLDMMDL_BLACTCONT���擾
 * @param	fmmdl	FLDMMDL *
 * @retval	FLDMMDL_BLACTCONT*
 */
//--------------------------------------------------------------
FLDMMDL_BLACTCONT * FLDMMDL_GetBlActCont( FLDMMDL *fmmdl )
{
	return( FLDMMDLSYS_GetBlActCont((FLDMMDLSYS*)FLDMMDL_GetFldMMdlSys(fmmdl)) );
}

//======================================================================
//	FLDMMDLSYS �X�e�[�^�X����
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMMDLSYS �`�揈��������������Ă��邩�ǂ����`�F�b�N
 * @param	fmmdlsys	FLDMMDLSYS *
 * @retval	BOOL	TRUE=����������Ă���
 */
//--------------------------------------------------------------
BOOL FLDMMDLSYS_CheckCompleteDrawInit( const FLDMMDLSYS *fmmdlsys )
{
	if( FLDMMDLSYS_CheckStatusBit(
			fmmdlsys,FLDMMDLSYS_STABIT_DRAW_INIT_COMP) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS �`�揈�������������Z�b�g
 * @param	fmmdlsys	FLDMMDLSYS*
 * @param	flag	TRUE=����������
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_SetCompleteDrawInit( FLDMMDLSYS *fmmdlsys, BOOL flag )
{
	if( flag == TRUE ){
		FldMMdlSys_OnStatusBit( fmmdlsys, FLDMMDLSYS_STABIT_DRAW_INIT_COMP );
	}else{
		FldMMdlSys_OffStatusBit( fmmdlsys, FLDMMDLSYS_STABIT_DRAW_INIT_COMP );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS �e��t����A�t���Ȃ��̃Z�b�g
 * @param	fmmdlsys FLDMMDLSYS *
 * @param	flag	TRUE=�e��t���� FALSE=�e��t���Ȃ�
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_SetJoinShadow( FLDMMDLSYS *fos, BOOL flag )
{
	if( flag == FALSE ){
		FldMMdlSys_OnStatusBit( fos, FLDMMDLSYS_STABIT_SHADOW_JOIN_NOT );
	}else{
		FldMMdlSys_OffStatusBit( fos, FLDMMDLSYS_STABIT_SHADOW_JOIN_NOT );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS �e��t����A�t���Ȃ��̃`�F�b�N
 * @param	fmmdlsys FLDMMDLSYS *
 * @retval	BOOL TRUE=�t����
 */
//--------------------------------------------------------------
BOOL FLDMMDLSYS_CheckJoinShadow( const FLDMMDLSYS *fos )
{
	if( FLDMMDLSYS_CheckStatusBit(fos,FLDMMDLSYS_STABIT_SHADOW_JOIN_NOT) ){
		return( FALSE );
	}
	return( TRUE );
}

//======================================================================
//	FLDMMDLSYS �v���Z�X����
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMMDLSYS �t�B�[���h���샂�f���S�̂̓�������S��~�B
 * ���쏈���A�`�揈�������S��~����B�A�j���[�V�����R�}���h�ɂ��������Ȃ��B
 * @param	fmmdlsys	FLDMMDLSYS*
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_StopProc( FLDMMDLSYS *fmmdlsys )
{
	FldMMdlSys_OnStatusBit( fmmdlsys,
		FLDMMDLSYS_STABIT_MOVE_PROC_STOP|FLDMMDLSYS_STABIT_DRAW_PROC_STOP );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS FLDMMDLSYS_StopProc()�̉����B
 * @param	fmmdlsys	FLDMMDLSYS*
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_PlayProc( FLDMMDLSYS *fmmdlsys )
{
	FldMMdlSys_OffStatusBit( fmmdlsys,
		FLDMMDLSYS_STABIT_MOVE_PROC_STOP|FLDMMDLSYS_STABIT_DRAW_PROC_STOP );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS �t�B�[���h���샂�f���S�̂̓�����ꎞ��~
 * �ŗL�̓��쏈���i�����_���ړ����j���ꎞ��~����B
 * �A�j���[�V�����R�}���h�ɂ͔�������B
 * @param	fmmdlsys	FLDMMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_PauseMoveProc( FLDMMDLSYS *fmmdlsys )
{
	u32 no = 0;
	FLDMMDL *fmmdl;
	
	while( FLDMMDLSYS_SearchUseFldMMdl(fmmdlsys,&fmmdl,&no) == TRUE ){
		FLDMMDL_OnStatusBitMoveProcPause( fmmdl );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS �t�B�[���h���샂�f���S�̂̈ꎞ��~������
 * @param	fmmdlsys	FLDMMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_ClearPauseMoveProc( FLDMMDLSYS *fmmdlsys )
{
	u32 no = 0;
	FLDMMDL *fmmdl;

	while( FLDMMDLSYS_SearchUseFldMMdl(fmmdlsys,&fmmdl,&no) == TRUE ){
		FLDMMDL_OffStatusBitMoveProcPause( fmmdl );
	}
}

//======================================================================
//	FLDMMDL �X�e�[�^�X�r�b�g�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMMDL �t�B�[���h���샂�f������
 * �t�B�[���h���샂�f���V�X�e���̃r�b�g�`�F�b�N
 * @param	fmmdl		FLDMMDL*
 * @param	bit			FLDMMDLSYS_STABIT_DRAW_INIT_COMP��
 * @retval	u32			0�ȊO bit�q�b�g
 */
//--------------------------------------------------------------
u32 FLDMMDL_CheckFldMMdlSysStatusBit(
	const FLDMMDL *fmmdl, FLDMMDLSYS_STABIT bit )
{
	const FLDMMDLSYS *fos = FLDMMDL_GetFldMMdlSys( fmmdl );
	return( FLDMMDLSYS_CheckStatusBit(fos,bit) );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �g�p�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL	TRUE=�g�p��
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitUse( const FLDMMDL *fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_USE) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �ړ����쒆�ɂ���
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OnStatusBitMove( FLDMMDL *fmmdl )
{
	FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_MOVE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �ړ����쒆������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OffStatusBitMove( FLDMMDL * fmmdl )
{
	FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_MOVE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �ړ����쒆�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL TRUE=���쒆
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitMove( const FLDMMDL *fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_MOVE) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �ړ�����J�n�ɂ���
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OnStatusBitMoveStart( FLDMMDL * fmmdl )
{
	FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_MOVE_START );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �ړ�����J�n������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OffStatusBitMoveStart( FLDMMDL * fmmdl )
{
	FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_MOVE_START );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �ړ�����J�n�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL TRUE=�ړ�����J�n
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitMoveStart( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_MOVE_START) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �ړ�����I���ɂ���
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OnStatusBitMoveEnd( FLDMMDL * fmmdl )
{
	FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_MOVE_END );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �ړ�����I��������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OffStatusBitMoveEnd( FLDMMDL * fmmdl )
{
	FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_MOVE_END );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �ړ�����I���`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL TRUE=�ړ��I��
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitMoveEnd( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_MOVE_END) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �`�揉���������ɂ���
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OnStatusBitCompletedDrawInit( FLDMMDL * fmmdl )
{
	FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_DRAW_PROC_INIT_COMP );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �`�揉��������������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OffStatusBitCompletedDrawInit( FLDMMDL * fmmdl )
{
	FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_DRAW_PROC_INIT_COMP );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �`�揉���������`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL TRUE=�`�揉��������
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitCompletedDrawInit( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(
			fmmdl,FLDMMDL_STABIT_DRAW_PROC_INIT_COMP) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ��\���t���O���`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL TRUE=��\���@FALSE=�\��
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitVanish( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_VANISH) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ��\���t���O��ݒ�
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=��\���@FALSE=�\��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetStatusBitVanish( FLDMMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_VANISH );
	}else{
		FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_VANISH );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL OBJ���m�̓����蔻��t���O��ݒ�
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=�q�b�g�A���@FALSE=�q�b�g�i�V
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetStatusBitFellowHit( FLDMMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_FELLOW_HIT_NON );
	}else{
		FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_FELLOW_HIT_NON );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL ���쒆�t���O�̃Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=���쒆�@FALSE=��~��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetStatusBitMove( FLDMMDL * fmmdl, int flag )
{
	if( flag == TRUE ){
		FLDMMDL_OnStatusBitMove( fmmdl );
	}else{
		FLDMMDL_OffStatusBitMove( fmmdl );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL �b�������\�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL TRUE=�\ FALSE=�s��
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitTalk( FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_TALK_OFF) ){
		return( FALSE );
	}
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �b�������s�t���O���Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=�s�� FALSE=�\
 */
//--------------------------------------------------------------
void FLDMMDL_SetStatusBitTalkOFF( FLDMMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_TALK_OFF );
	}else{
		FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_TALK_OFF );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL ����|�[�Y
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OnStatusBitMoveProcPause( FLDMMDL * fmmdl )
{
	FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_PAUSE_MOVE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ����|�[�Y����
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OffStatusBitMoveProcPause( FLDMMDL * fmmdl )
{
	FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_PAUSE_MOVE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ����|�[�Y�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL TRUE=����|�[�Y
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitMoveProcPause( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_PAUSE_MOVE) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �X�e�[�^�X�r�b�g �`�揈�������������`�F�b�N
 * @param	fmmdl		FLDMMDL *
 * @retval	BOOL TRUE=�����BFALSE=�܂�
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckCompletedDrawInit( const FLDMMDL * fmmdl )
{
	const FLDMMDLSYS *fos;
	
	fos = FLDMMDL_GetFldMMdlSys( fmmdl );
	
	if( FLDMMDLSYS_CheckCompleteDrawInit(fos) == FALSE ){
		return( FALSE );
	}
	
	if( FLDMMDL_CheckStatusBitCompletedDrawInit(fmmdl) ){
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �����擾���֎~����
 * @param	fmmdl	FLDMMDL *
 * @param	int		TRUE=�����擾OFF FALSE=ON
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetStatusBitHeightGetOFF( FLDMMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_HEIGHT_GET_OFF );
	}else{
		FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_HEIGHT_GET_OFF );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL �����擾���֎~����Ă��邩�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL TRUE=�֎~
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitHeightGetOFF( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_HEIGHT_GET_OFF) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �]�[���؂�ւ����̍폜�֎~
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=�֎~ FALSE=�֎~���Ȃ�
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetStatusBitNotZoneDelete( FLDMMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_ZONE_DEL_NOT );
	}else{
		FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_ZONE_DEL_NOT );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL �G�C���A�X�t���O���Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=�G�C���A�X FALSE=�Ⴄ
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetStatusBitAlies( FLDMMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_ALIES );
	}else{
		FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_ALIES );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL �G�C���A�X�t���O���`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL TRUE=�G�C���A�X FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitAlies( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_ALIES) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �󐣃G�t�F�N�g�Z�b�g�t���O���Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=�Z�b�g�@FALSE=�N���A
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetStatusBitShoalEffect( FLDMMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_EFFSET_SHOAL );
	}else{
		FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_EFFSET_SHOAL );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL �󐣃G�t�F�N�g�Z�b�g�t���O���`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL TRUE=�Z�b�g�@FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitShoalEffect( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_EFFSET_SHOAL) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �A�g���r���[�g�I�t�Z�b�g�ݒ�OFF�Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=�Z�b�g�@FALSE=�N���A
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetStatusBitAttrOffsetOFF( FLDMMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_ATTR_OFFS_OFF );
	}else{
		FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_ATTR_OFFS_OFF );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL �A�g���r���[�g�I�t�Z�b�g�ݒ�OFF�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL TRUE=OFF�@FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitAttrOffsetOFF( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_ATTR_OFFS_OFF) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ���ړ��t���O�Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=�Z�b�g�@FALSE=�N���A
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetStatusBitBridge( FLDMMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_BRIDGE );
	}else{
		FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_BRIDGE );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL ���ړ��t���O�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL TRUE=���@FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitBridge( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_BRIDGE) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �f�肱�݃t���O�Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=�Z�b�g�@FALSE=�N���A
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetStatusBitReflect( FLDMMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_REFLECT_SET );
	}else{
		FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_REFLECT_SET );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL �f�肱�݃t���O�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL TRUE=�Z�b�g�@FALSE=����
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitReflect( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_REFLECT_SET) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �A�j���[�V�����R�}���h�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL TRUE=�R�}���h�A���@FALSE=����
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitAcmd( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_ACMD) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �g�����������t���O���Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=�Z�b�g�@FALSE=�N���A
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetStatusBitHeightExpand( FLDMMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_HEIGHT_EXPAND );
	}else{
		FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_HEIGHT_EXPAND );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL �g�����������t���O�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	BOOL TRUE=�g�������ɔ�������@FALSE=����
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckStatusBitHeightExpand( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_HEIGHT_EXPAND) ){
		return( TRUE );
	}
	return( FALSE );
}

//======================================================================
//	FLDMMDL ����r�b�g�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMMDL �A�g���r���[�g�擾���~
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=��~
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_SetMoveBitAttrGetOFF( FLDMMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		FLDMMDL_OnMoveBit( fmmdl, FLDMMDL_MOVEBIT_ATTR_GET_OFF );
	}else{
		FLDMMDL_OffMoveBit( fmmdl, FLDMMDL_MOVEBIT_ATTR_GET_OFF );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL �A�g���r���[�g�擾���~�@�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	int	TRUE=��~
 */
//--------------------------------------------------------------
int FLDMMDL_CheckMoveBitAttrGetOFF( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckMoveBit(fmmdl,FLDMMDL_MOVEBIT_ATTR_GET_OFF) ){
		return( TRUE );
	}
	return( FALSE );
}

//======================================================================
//	FLDMMDLSYS �c�[��
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMMDLSYS �g�p���Ă���t�B�[���h���샂�f����T���B
 * @param	fos	FLDMMDLSYS *
 * @param	fmmdl	FLDMMDL*�i�[��
 * @param	no	�����J�n���[�Nno�B�擪���猟������ۂ͏����l0���w��B
 * @retval	BOOL TRUE=���샂�f���擾���� FALSE=no����I�[�܂Ō������擾�����B
 * ����no�͌Ăяo����A�擾�ʒu+1�̒l�ɂȂ�B
 *
 * ����FOBJ ID 1�Ԃ̓��샂�f����T���B
 * u32 no=0;
 * FLDMMDL *fmmdl;
 * while( FLDMMDLSYS_SearchUseFldMMdl(fos,&fmmdl,&no) == TRUE ){
 * 		if( FLDMMDL_GetOBJID(fmmdl) == 1 ){
 * 			break;
 * 		}
 * }
 */
//--------------------------------------------------------------
BOOL FLDMMDLSYS_SearchUseFldMMdl(
	const FLDMMDLSYS *fos, FLDMMDL **fmmdl, u32 *no )
{
	u32 max = FLDMMDLSYS_GetFldMMdlMax( fos );
	
	if( (*no) < max ){
		FLDMMDL *check_obj = &(((FLDMMDLSYS*)fos)->pFldMMdlBuf[*no]);
		
		do{
			(*no)++;
			if( FLDMMDL_CheckStatusBit(check_obj,FLDMMDL_STABIT_USE) ){
				*fmmdl = check_obj;
				return( TRUE );
			}
			check_obj++;
		}while( (*no) < max );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS �w�肳�ꂽ�O���b�hX,Z���W�ɂ���OBJ���擾
 * @param	sys			FLDMMDLSYS *
 * @param	x			�����O���b�hX
 * @param	z			�����O���b�hZ
 * @param	old_hit		TURE=�ߋ����W�����肷��
 * @retval	FLDMMDL	x,z�ʒu�ɂ���FLDMMDL * �BNULL=���̍��W��OBJ�͂��Ȃ�
 */
//--------------------------------------------------------------
FLDMMDL * FLDMMDLSYS_SearchGridPos(
	const FLDMMDLSYS *sys, s16 x, s16 z, BOOL old_hit )
{
	u32 no = 0;
	FLDMMDL *fmmdl;
	
	while( FLDMMDLSYS_SearchUseFldMMdl(sys,&fmmdl,&no) == TRUE ){
		if( old_hit ){
			if( FLDMMDL_GetOldGridPosX(fmmdl) == x &&
				FLDMMDL_GetOldGridPosZ(fmmdl) == z ){
				return( fmmdl );
			}
		}
		
		if( FLDMMDL_GetGridPosX(fmmdl) == x &&
			FLDMMDL_GetGridPosZ(fmmdl) == z ){
			return( fmmdl );
		}
	}
	
	return( NULL );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS ����R�[�h�Ɉ�v����FLDMMDL *������
 * @param	fos		FLDMMDLSYS *
 * @param	mv_code		�������铮��R�[�h
 * @retval	FLDMMDL *	NULL=���݂��Ȃ�
 */
//--------------------------------------------------------------
FLDMMDL * FLDMMDLSYS_SearchMoveCode( const FLDMMDLSYS *fos, u16 mv_code )
{
	u32 no = 0;
	FLDMMDL *fmmdl;
	
	while( FLDMMDLSYS_SearchUseFldMMdl(fos,&fmmdl,&no) == TRUE ){
		if( FLDMMDL_GetMoveCode(fmmdl) == mv_code ){
			return( fmmdl );
		}
	}
	
	return( NULL );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS OBJ ID�Ɉ�v����FLDMMDL *������
 * @param	fos		FLDMMDLSYS *
 * @param	id		��������OBJ ID
 * @retval	FLDMMDL *	NULL=���݂��Ȃ�
 */
//--------------------------------------------------------------
FLDMMDL * FLDMMDLSYS_SearchOBJID( const FLDMMDLSYS *fos, u16 id )
{
	u32 no = 0;
	FLDMMDL *fmmdl;

	while( FLDMMDLSYS_SearchUseFldMMdl(fos,&fmmdl,&no) == TRUE ){
		if( FLDMMDL_CheckStatusBitAlies(fmmdl) == FALSE ){
			if( FLDMMDL_GetOBJID(fmmdl) == id ){
				return( fmmdl );
			}
		}
	}
	
	return( NULL );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS �t�B�[���h���샂�f���̋󂫂�T��
 * @param	sys			FLDMMDLSYS *
 * @retval	FLDMMDL	�󂫂�FLDMMDL*�@�󂫂������ꍇ��NULL
 */
//--------------------------------------------------------------
static FLDMMDL * FldMMdlSys_SearchSpaceFldMMdl( const FLDMMDLSYS *sys )
{
	int i,max;
	FLDMMDL *fmmdl;
	
	i = 0;
	max = FLDMMDLSYS_GetFldMMdlMax( sys );
	fmmdl = ((FLDMMDLSYS*)sys)->pFldMMdlBuf;
	
	do{
		if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_USE) == 0 ){
			return( fmmdl );
		}
		
		fmmdl++;
		i++;
	}while( i < max );
	
	return( NULL );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS �t�B�[���h���샂�f���@�G�C���A�X��T��
 * @param	fos			FLDMMDLSYS *
 * @param	obj_id		��v����OBJ ID
 * @param	zone_id		��v����ZONE ID
 * @retval	FLDMMDL	��v����FLDMMDL*�@��v����=NULL
 */
//--------------------------------------------------------------
static FLDMMDL * FldMMdlSys_SearchAlies(
	const FLDMMDLSYS *fos, int obj_id, int zone_id )
{
	u32 no = 0;
	FLDMMDL * fmmdl;
	
	while( FLDMMDLSYS_SearchUseFldMMdl(fos,&fmmdl,&no) ){
		if( FLDMMDL_CheckStatusBitAlies(fmmdl) == TRUE ){
			if( FLDMMDL_GetOBJID(fmmdl) == obj_id ){
				if( FLDMMDL_GetZoneIDAlies(fmmdl) == zone_id ){
					return( fmmdl );
				}
			}
		}
	}
	
	return( NULL );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS �t�B�[���h���샂�f�� �]�[���X�V���̓��샂�f���폜
 * @param	fos	FLDMMDLSYS
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_DeleteZoneUpdateFldMMdl( FLDMMDLSYS *fos )
{
	u32 no = 0;
	FLDMMDL *fmmdl;
	
	while( FLDMMDLSYS_SearchUseFldMMdl(fos,&fmmdl,&no) ){
		//�{���ł���΍X�ɃG�C���A�X�`�F�b�N������
		if( FLDMMDL_CheckStatusBit(
				fmmdl,FLDMMDL_STABIT_ZONE_DEL_NOT) == 0 ){
			if( FLDMMDL_GetOBJID(fmmdl) == 0xff ){
				GF_ASSERT( 0 );
			}
			FLDMMDL_Delete( fmmdl );
		}
	}
}

//======================================================================
//	FLDMMDL �c�[��
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMMDL �t�B�[���h���샂�f���@TCB����֐��ǉ�
 * @param	fmmdl	FLDMMDL*
 * @param	sys		FLDMMDLSYS*
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_AddTCB( FLDMMDL *fmmdl, const FLDMMDLSYS *sys )
{
	int pri,code;
	GFL_TCB * tcb;
	
	pri = FLDMMDLSYS_GetTCBPriority( sys );
	code = FLDMMDL_GetMoveCode( fmmdl );
	
	if( code == MV_PAIR || code == MV_TR_PAIR ){
		pri += FLDMMDL_TCBPRI_OFFS_AFTER;
	}
	
	tcb = GFL_TCB_AddTask( FLDMMDLSYS_GetTCBSYS((FLDMMDLSYS*)sys),
			FldMMdl_TCB_MoveProc, fmmdl, pri );
	GF_ASSERT( tcb != NULL );
	
	fmmdl->pTCB = tcb;
}

//--------------------------------------------------------------
/**
 * FLDMMDL �t�B�[���h���샂�f���@TCB����֐��폜
 * @param	fmmdl	FLDMMDL*
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_DeleteTCB( FLDMMDL *fmmdl )
{
	GF_ASSERT( fmmdl->pTCB );
	GFL_TCB_DeleteTask( fmmdl->pTCB );
	fmmdl->pTCB = NULL;
}

//--------------------------------------------------------------
/**
 * FLDMMDL ���ݔ������Ă���t�B�[���h���샂�f����OBJ�R�[�h���Q��
 * @param	fmmdl	FLDMMDL * 
 * @param	code	�`�F�b�N����R�[�h�BHERO��
 * @retval	BOOL	TRUE=fmmdl�ȊO�ɂ�code�������Ă���z������
 */
//--------------------------------------------------------------
BOOL FLDMMDL_SearchUseOBJCode( const FLDMMDL *fmmdl, u16 code )
{
	u32 no = 0;
	u16 check_code;
	FLDMMDL *cmmdl;
	const FLDMMDLSYS *fos = FLDMMDL_GetFldMMdlSys( fmmdl );
	
	while( FLDMMDLSYS_SearchUseFldMMdl(fos,&cmmdl,&no) == TRUE ){
		if( cmmdl != fmmdl ){
			check_code = FLDMMDL_GetOBJCode( cmmdl );
			if( check_code != OBJCODEMAX && check_code == code ){
				return( TRUE );
			}
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ���W�A�������������B
 * @param	fmmdl	FLDMMDL *
 * @param	vec		���������W
 * @param	dir		���� DIR_UP��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_InitPosition( FLDMMDL * fmmdl, const VecFx32 *vec, u16 dir )
{
	int grid = SIZE_GRID_FX32( vec->x );
	FLDMMDL_SetGridPosX( fmmdl, grid );
	
	grid = SIZE_GRID_FX32( vec->y );
	FLDMMDL_SetGridPosY( fmmdl, grid );
	
	grid = SIZE_GRID_FX32( vec->z );
	FLDMMDL_SetGridPosZ( fmmdl, grid );
	
	FLDMMDL_SetVectorPos( fmmdl, vec );
	FLDMMDL_UpdateGridPosCurrent( fmmdl );
	
	FLDMMDL_SetForceDirDisp( fmmdl, dir );
	
	FLDMMDL_FreeAcmd( fmmdl );
	FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_MOVE_START );
	FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_MOVE|FLDMMDL_STABIT_MOVE_END );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ����R�[�h�ύX
 * @param	fmmdl	FLDMMDL *
 * @param	code	MV_RND��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_ChangeMoveCode( FLDMMDL *fmmdl, u16 code )
{
	FLDMMDL_CallMoveDeleteProc( fmmdl );
	FLDMMDL_SetMoveCode( fmmdl, code );
	FldMMdl_InitCallMoveProcWork( fmmdl );
	FLDMMDL_InitMoveProc( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �`�揉�����ɍs�������Z��
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_InitDrawWork( FLDMMDL *fmmdl )
{
	const FLDMMDLSYS *fos = FLDMMDL_GetFldMMdlSys( fmmdl );
	
	if( FLDMMDLSYS_CheckCompleteDrawInit(fos) == FALSE ){
		return; //�`��V�X�e���������������Ă��Ȃ�
	}
	
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_HEIGHT_GET_ERROR) ){
		FLDMMDL_UpdateCurrentHeight( fmmdl );
	}
	
	FLDMMDL_SetDrawStatus( fmmdl, 0 );
	#ifndef FLDMMDL_PL_NULL
	FLDMMDL_BlActAddPracFlagSet( fmmdl, FALSE );
	#endif
	
	if( FLDMMDL_CheckStatusBitCompletedDrawInit(fmmdl) == FALSE ){
		FldMMdl_InitCallDrawProcWork( fmmdl );
		FLDMMDL_CallDrawInitProc( fmmdl );
		FLDMMDL_OnStatusBitCompletedDrawInit( fmmdl );
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL �t�B�[���h���샂�f�� �`��֐�������
 * @param	fmmdl		FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_InitCallDrawProcWork( FLDMMDL * fmmdl )
{
	const FLDMMDL_DRAW_PROC_LIST *list;
	u16 code = FLDMMDL_GetOBJCode( fmmdl );
	const OBJCODE_PARAM *prm = FLDMMDL_GetOBJCodeParam( fmmdl, code );
	list = DrawProcList_GetList( prm->draw_proc_no );
	fmmdl->draw_proc_list = list;
}

//--------------------------------------------------------------
/**
 * FLDMMDL �t�B�[���h���샂�f���֘A�G�t�F�N�g�̃t���O�������B
 * �G�t�F�N�g�֘A�̃t���O���������܂Ƃ߂�B
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_InitDrawEffectFlag( FLDMMDL * fmmdl )
{
	FLDMMDL_OffStatusBit( fmmdl,
		FLDMMDL_STABIT_SHADOW_SET		|
		FLDMMDL_STABIT_SHADOW_VANISH	|
		FLDMMDL_STABIT_EFFSET_SHOAL		|
		FLDMMDL_STABIT_REFLECT_SET );
}

//--------------------------------------------------------------
/**
 * FLDMMDL OBJ ID��ύX
 * @param	fmmdl	FLDMMDL *
 * @param	id		OBJ ID
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_ChangeOBJID( FLDMMDL * fmmdl, u16 id )
{
	FLDMMDL_SetOBJID( fmmdl, id );
	FLDMMDL_OnStatusBitMoveStart( fmmdl );
	FldMMdl_InitDrawEffectFlag( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDL ���[�N����
 * @param	fmmdl	FLDMMDL
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_ClearWork( FLDMMDL *fmmdl )
{
	GFL_STD_MemClear( fmmdl, FLDMMDL_SIZE );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �w�肳�ꂽ�t�B�[���h���샂�f�����G�C���A�X�w�肩�ǂ����`�F�b�N
 * @param	fmmdl		FLDMMDL *
 * @param	h_zone_id	head��ǂݍ��ރ]�[��ID
 * @param	max			head�v�f��
 * @param	head		FLDMMDL_H
 * @retval	int			RET_ALIES_NOT��
 */
//--------------------------------------------------------------
static int FldMMdl_CheckHeaderAlies(
		const FLDMMDL *fmmdl, int h_zone_id, int max,
		const FLDMMDL_HEADER *head )
{
	u16 obj_id;
	int zone_id;
	
	while( max ){
		obj_id = head->id;
		
		if( FLDMMDL_GetOBJID(fmmdl) == obj_id ){
			//�G�C���A�X�w�b�_�[
			if( FldMMdlHeader_CheckAlies(head) == TRUE ){
				//�G�C���A�X�̐��K�]�[��ID
				zone_id = FldMMdlHeader_GetAliesZoneID( head );
				//�ΏۃG�C���A�X
				if( FLDMMDL_CheckStatusBitAlies(fmmdl) == TRUE ){
					if( FLDMMDL_GetZoneIDAlies(fmmdl) == zone_id ){
						return( RET_ALIES_EXIST );	//Alies�Ƃ��Ċ��ɑ���
					}
				}else if( FLDMMDL_GetZoneID(fmmdl) == zone_id ){
					return( RET_ALIES_CHANGE );		//Alies�Ώۂł���
				}
			}else{ //�ʏ�w�b�_�[
				if( FLDMMDL_CheckStatusBitAlies(fmmdl) == TRUE ){
					//�����G�C���A�X�ƈ�v
					if( FLDMMDL_GetZoneIDAlies(fmmdl) == h_zone_id ){
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
 * FLDMMDL �w�肳�ꂽ�]�[��ID��OBJ ID������FLDMMDL *���擾�B
 * @param	fos		FLDMMDLSYS *
 * @param	obj_id	OBJ ID
 * @param	zone_id	�]�[��ID
 * @retval	FLDMMDL * FLDMMDL *
 */
//--------------------------------------------------------------
static FLDMMDL * FldMMdl_SearchOBJIDZoneID(
		const FLDMMDLSYS *fos, int obj_id, int zone_id )
{
	u32 no = 0;
	FLDMMDL *fmmdl;
	
	while( FLDMMDLSYS_SearchUseFldMMdl(fos,&fmmdl,&no) == TRUE ){
		if( FLDMMDL_GetOBJID(fmmdl) == obj_id &&
			FLDMMDL_GetZoneID(fmmdl) == zone_id ){
			return( fmmdl );
		}
	}
	
	return( NULL );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �t�B�[���h���샂�f���`�揉�����ɍs������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_InitDrawStatus( FLDMMDL * fmmdl )
{
	FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_MOVE_START );
	FldMMdl_InitDrawEffectFlag( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �t�B�[���h���샂�f���`��폜���ɍs������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_SetDrawDeleteStatus( FLDMMDL * fmmdl )
{
}

//--------------------------------------------------------------
/**
 * FLDMMDL �t�B�[���h���샂�f���@�G�C���A�X���琳�KOBJ�ւ̕ύX
 * @param	fmmdl		FLDMMDL * 
 * @param	head		�Ώۂ�FLDMMDL_H
 * @param	zone_id		���K�̃]�[��ID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_ChangeAliesOBJ(
	FLDMMDL *fmmdl, const FLDMMDL_HEADER *head, int zone_id )
{
	GF_ASSERT( FLDMMDL_CheckStatusBitAlies(fmmdl) == TRUE );
	FLDMMDL_SetStatusBitAlies( fmmdl, FALSE );
	FLDMMDL_SetZoneID( fmmdl, zone_id );
	FLDMMDL_SetEventID( fmmdl, head->event_id );
	FLDMMDL_SetEventFlag( fmmdl, head->event_flag );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �t�B�[���h���샂�f���@���KOBJ����G�C���A�X�ւ̕ύX
 * @param	fmmdl		FLDMMDL * 
 * @param	head		�Ώۂ�FLDMMDL_H
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_ChangeOBJAlies(
	FLDMMDL * fmmdl, int zone_id, const FLDMMDL_HEADER *head )
{
	GF_ASSERT( FldMMdlHeader_CheckAlies(head) == TRUE );
	FLDMMDL_SetStatusBitAlies( fmmdl, TRUE );
	FLDMMDL_SetEventID( fmmdl, head->event_id );
	FLDMMDL_SetEventFlag( fmmdl, FldMMdlHeader_GetAliesZoneID(head) );
	FLDMMDL_SetZoneID( fmmdl, zone_id );
}

//--------------------------------------------------------------
/**
 * FLDMMDL �t�B�[���h���샂�f���̓���`�F�b�N�B
 * ���S�A����ւ�肪�������Ă��邩�`�F�b�N����B
 * @param	fmmdl	FLDMMDL *
 * @param	obj_id	����Ƃ݂Ȃ�OBJ ID
 * @param	zone_id	����Ƃ݂Ȃ�ZONE ID
 * @retval	int		TRUE=����BFALSE=����ł͂Ȃ�
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckSameID( const FLDMMDL * fmmdl, u16 obj_id, u16 zone_id )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_USE) == 0 ){
		return( FALSE );
	}
	
	if( FLDMMDL_GetOBJID(fmmdl) != obj_id ){
		return( FALSE );
	}
	
	if( FLDMMDL_GetZoneID(fmmdl) != zone_id ){
		if( FLDMMDL_CheckStatusBitAlies(fmmdl) == FALSE ){
			return( FALSE );
		}
		
		if( FLDMMDL_GetZoneIDAlies(fmmdl) != zone_id ){
			return( FALSE );
		}
	}
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���̓���`�F�b�N�BOBJ�R�[�h�܂�
 * ���S�A����ւ�肪�������Ă��邩�`�F�b�N����B
 * @param	fmmdl	FLDMMDL *
 * @param	code	����Ƃ݂Ȃ�OBJ�R�[�h
 * @param	obj_id	����Ƃ݂Ȃ�OBJ ID
 * @param	zone_id	����Ƃ݂Ȃ�ZONE ID
 * @retval	int		TRUE=����BFALSE=����ł͂Ȃ�
 */
//--------------------------------------------------------------
BOOL FLDMMDL_CheckSameIDCode(
		const FLDMMDL * fmmdl, u16 code, u16 obj_id, u16 zone_id )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_USE) ){
		return( FALSE );
	}
	
	{
		u16 ret = FLDMMDL_GetOBJCode( fmmdl );
		if( ret != code ){
			return( FALSE );
		}
	}
	
	return( FLDMMDL_CheckSameID(fmmdl,obj_id,zone_id) );
}

//======================================================================
//	OBJCODE_PARAM
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMMDLSYS OBJCODE_PARAM ������
 * @param	fmmdlsys	FLDMMDLSYS
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdlSys_InitOBJCodeParam( FLDMMDLSYS *fmmdlsys )
{
	u8 *p = GFL_ARC_LoadDataAlloc( ARCID_FLDMMDL_PARAM, 
			NARC_fldmmdl_mdlparam_fldmmdl_mdlparam_bin,
			fmmdlsys->heapID );
	fmmdlsys->pOBJCodeParamBuf = p;
	fmmdlsys->pOBJCodeParamTbl = (const OBJCODE_PARAM*)(&p[OBJCODE_PARAM_TOTAL_NUMBER_SIZE]);
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS OBJCODE_PARAM �폜
 * @param	fmmdlsys	FLDMMDLSYS
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdlSys_DeleteOBJCodeParam( FLDMMDLSYS *fmmdlsys )
{
	GFL_HEAP_FreeMemory( fmmdlsys->pOBJCodeParamBuf );
	fmmdlsys->pOBJCodeParamBuf = NULL;
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS OBJCODE_PARAM �擾
 * @param	fmmdlsys	FLDMMDLSYS *
 * @param	code	�擾����OBJ�R�[�h
 * @retval	OBJCODE_PARAM*
 */
//--------------------------------------------------------------
const OBJCODE_PARAM * FLDMMDLSYS_GetOBJCodeParam(
		const FLDMMDLSYS *fmmdlsys, u16 code )
{
	GF_ASSERT( code < OBJCODEMAX );
	return( &(fmmdlsys->pOBJCodeParamTbl[code]) );
}

//--------------------------------------------------------------
/**
 * FLDMMDL OBJCODE_PARAM �擾
 * @param	fmmdl	FLDMMDL*
 * @param	code	�擾����OBJ�R�[�h
 * @retval	OBJCODE_PARAM*
 */
//--------------------------------------------------------------
const OBJCODE_PARAM * FLDMMDL_GetOBJCodeParam(
		const FLDMMDL *fmmdl, u16 code )
{
	const FLDMMDLSYS *fmmdlsys = FLDMMDL_GetFldMMdlSys( fmmdl );
	return( FLDMMDLSYS_GetOBJCodeParam(fmmdlsys,code) );
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
	#ifndef FLDMMDL_PL_NULL
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
 * �w�肳�ꂽ����R�[�h�̊֐����X�g���擾
 * @param	code	����R�[�h
 * @retval FLDMMDL_MOVE_PROC_LIST
 */
//--------------------------------------------------------------
static const FLDMMDL_MOVE_PROC_LIST * MoveProcList_GetList( u16 code )
{
	GF_ASSERT( code < MV_CODE_MAX );
	return( DATA_FieldOBJMoveProcListTbl[code] );
}

//--------------------------------------------------------------
/**
 * �w�肳�ꂽOBJ�R�[�h�̕`��֐����X�g���擾
 * @param	code	OBJ�R�[�h
 * @retval	FLDMMDL_DRAW_PROC_LIST*
 */
//--------------------------------------------------------------
static const FLDMMDL_DRAW_PROC_LIST * DrawProcList_GetList(
		FLDMMDL_DRAWPROCNO no )
{
	GF_ASSERT( no < FLDMMDL_DRAWPROCNO_MAX );
	return( DATA_FLDMMDL_DRAW_PROC_LIST_Tbl[no] );
}

//--------------------------------------------------------------
/**
 * FLDMMDL_HEADER �G�C���A�X�`�F�b�N
 * @param	head	FIELD_OBJ_H
 * @retval	int		TRUE=�G�C���A�X�@FALSE=�Ⴄ
 */
//--------------------------------------------------------------
static BOOL FldMMdlHeader_CheckAlies( const FLDMMDL_HEADER *head )
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
static int FldMMdlHeader_GetAliesZoneID( const FLDMMDL_HEADER *head )
{
	GF_ASSERT( FldMMdlHeader_CheckAlies(head) == TRUE );
	return( (int)head->event_flag );
}

//======================================================================
//	����֐��_�~�[
//======================================================================
//--------------------------------------------------------------
/**
 * ���쏉�����֐��_�~�[
 * @param	FLDMMDL	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveInitProcDummy( FLDMMDL * fmmdl )
{
}

//--------------------------------------------------------------
/**
 * ����֐��_�~�[
 * @param	FLDMMDL	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveProcDummy( FLDMMDL * fmmdl )
{
}

//--------------------------------------------------------------
/**
 * ����폜�֐��_�~�[
 * @param	FLDMMDL	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveDeleteProcDummy( FLDMMDL * fmmdl )
{
}

//--------------------------------------------------------------
/**
 * ���앜�A�֐��_�~�[
 * @param	FLDMMDL *	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveReturnProcDummy( FLDMMDL * fmmdl )
{
}

//======================================================================
//	�`��֐��_�~�[
//======================================================================
//--------------------------------------------------------------
/**
 * �`�揉�����֐��_�~�[
 * @param	FLDMMDL	FLDMMDL * 
 * @retval	int			TRUE=����������
 */
//--------------------------------------------------------------
void FLDMMDL_DrawInitProcDummy( FLDMMDL * fmmdl )
{
}

//--------------------------------------------------------------
/**
 * �`��֐��_�~�[
 * @param	FLDMMDL	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_DrawProcDummy( FLDMMDL * fmmdl )
{
}

//--------------------------------------------------------------
/**
 * �`��폜�֐��_�~�[
 * @param	FLDMMDL	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_DrawDeleteProcDummy( FLDMMDL * fmmdl )
{
}

//--------------------------------------------------------------
/**
 * �`��ޔ��֐��_�~�[
 * @param	FLDMMDL	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_DrawPushProcDummy( FLDMMDL * fmmdl )
{
}

//--------------------------------------------------------------
/**
 * �`�敜�A�֐��_�~�[
 * @param	FLDMMDL	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_DrawPopProcDummy( FLDMMDL * fmmdl )
{
}

//======================================================================
//	debug
//======================================================================
#ifdef DEBUG_FLDMMDL
//--------------------------------------------------------------
/**
 * �f�o�b�O�@���샂�f���@OBJ�R�[�h��������擾(ASCII)
 * @param	code OBJ�R�[�h
 * @param	heapID buf�̈�m�ۗpHEAPID
 * @retval	u8* �����񂪊i�[���ꂽu8*�B�g�p��GFL_HEAP_FreeMemory()���K�v�B
 * �����񒷂�DEBUG_OBJCODE_STR_LENGTH�B
 */
//--------------------------------------------------------------
u8 * DEBUG_FLDMMDL_GetOBJCodeString( u16 code, HEAPID heapID )
{
	u8 *buf;
	GF_ASSERT( code < OBJCODEMAX );
	buf = GFL_HEAP_AllocClearMemoryLo(
			heapID, DEBUG_OBJCODE_STR_LENGTH );
	GFL_ARC_LoadDataOfs( buf, ARCID_FLDMMDL_PARAM,
			NARC_fldmmdl_mdlparam_fldmmdl_objcodestr_bin,
			DEBUG_OBJCODE_STR_LENGTH * code, DEBUG_OBJCODE_STR_LENGTH );
	return( buf );
}
#endif //DEBUG_FLDMMDL

//======================================================================
//
//======================================================================
#if 0
//--------------------------------------------------------------
/**
 * FLDMMDL_BUFFER �t�B�[���h���샂�f���o�b�t�@���烍�[�h
 * @param
 * @retval
 */
//--------------------------------------------------------------
FLDMMDL * FLDMMDL_BUFFER_LoadFldMMdl(
	FLDMMDL_BUFFER *buf, FLDMMDLSYS *fos, int no )
{
	FLDMMDL *fmmdl;
	
	fmmdl = FldMMdlSys_SearchSpaceFldMMdl( fos );
	GF_ASSERT( fmmdl != NULL );
	
	OS_Printf( "FLDMMDL LoadNo %d\n", no );
	
	*fmmdl = buf->fldMMdlBuf[no];
	
	FldMMdl_InitWork( fmmdl, fos );
	
	if( FLDMMDL_CheckMoveBit(fmmdl,FLDMMDL_MOVEBIT_MOVEPROC_INIT) == 0 ){
		FldMMdl_InitMoveWork( fmmdl );
	}else{
		FldMMdl_InitCallMoveProcWork( fmmdl );
	}
	
	FLDMMDL_OffStatusBitCompletedDrawInit( fmmdl );
	FldMMdl_InitDrawWork( fmmdl );
	
	FldMMdlSys_AddFldMMdlTCB( fos, fmmdl );
	FldMMdlSys_IncrementOBJCount( (FLDMMDLSYS*)FLDMMDL_GetFldMMdlSys(fmmdl) );
	
	return( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDL_BUFFER �t�B�[���h���샂�f���o�b�t�@���烍�[�h
 * @param
 * @retval
 */
//--------------------------------------------------------------
void FLDMMDL_BUFFER_LoadBuffer( FLDMMDL_BUFFER *buf, FLDMMDLSYS *fos )
{
	int i;
	FLDMMDL *fmmdl;

	for( i = 0; i < FLDMMDL_BUFFER_MAX; i++ ){
		fmmdl = &buf->fldMMdlBuf[i];
		if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_USE) ){
			FLDMMDL_BUFFER_LoadFldMMdl( buf, fos, i );
		}
	}
}

//--------------------------------------------------------------
/**
 * FLDMMDL_BUFFER �t�B�[���h���샂�f���o�b�t�@�փZ�[�u
 * @param
 * @retval
 */
//--------------------------------------------------------------
void FLDMMDL_BUFFER_SaveBuffer( FLDMMDL_BUFFER *buf, FLDMMDLSYS *fos )
{
	FLDMMDL *fmmdl;
	u32 i = 0, no = 0;
	
	FLDMMDL_BUFFER_InitBuffer( buf );
	while( FLDMMDLSYS_SearchUseFldMMdl(fos,&fmmdl,&i) == TRUE ){
		OS_Printf( "FLDMMDL BUFFER WorkNo %d, BufferNo %d Save\n", i-1, no );
		buf->fldMMdlBuf[no] = *fmmdl;
		no++;
	}
}
#endif
