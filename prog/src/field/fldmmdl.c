//======================================================================
/**
 * @file	fldmmdl.c
 * @brief	�t�B�[���h���샂�f��
 * @author	kagaya
 * @data	05.07.13
 */
//======================================================================
#include "fldmmdl.h"

//======================================================================
//	define
//======================================================================
//--------------------------------------------------------------
//	debug
//--------------------------------------------------------------

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
typedef struct _TAG_FLDMMDLSYS
{
	u32 status_bit;					///<�X�e�[�^�X�r�b�g
	u16 fmmdl_max;					///<FLDMMDL�ő吔
	s16 fmmdl_count;				///<�t�B�[���h���샂�f�����ݐ�
	u16 tcb_pri;					///<TCB�v���C�I���e�B
	HEAPID heapID;					///<�q�[�vID
	FLDMMDL *pFldMMdlBuf;			///<FLDMMDL���[�N *
	void *pTCBSysWork;				///<TCB���[�N
	GFL_TCBSYS *pTCBSys;			///<TCBSYS*
	FLDMMDL_BLACTCONT *pBlActCont;	///<FLDMMDL_BLACTCONT
	ARCHANDLE *pArcHandle;			///<�A�[�J�C�u�n���h��
	
	const FLDMAPPER *pG3DMapper;	///<FLDMAPPER
//	FIELD_MAIN_WORK *pFldMainWork;	///<FIELD_MAIN_WORK
}FLDMMDLSYS;

#define FLDMMDLSYS_SIZE (sizeof(FLDMMDLSYS)) ///<FLDMMDLSYS�T�C�Y

//--------------------------------------------------------------
///	FLDMMDL�\����
//--------------------------------------------------------------
typedef struct _TAG_FLDMMDL
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
	
	FLDMMDL_MOVE_PROC_INIT move_init_proc;	///<�������֐�
	FLDMMDL_MOVE_PROC move_proc;			///<����֐�
	FLDMMDL_MOVE_PROC_DEL move_delete_proc;	///<�폜�֐�
	FLDMMDL_DRAW_PROC_INIT draw_init_proc;	///<�`�揉�����֐�
	FLDMMDL_DRAW_PROC draw_proc;			///<�`��֐�
	FLDMMDL_DRAW_PROC_DEL draw_delete_proc;	///<�`��폜�֐�
	FLDMMDL_DRAW_PROC_PUSH draw_push_proc;	///<�`��ޔ��֐�
	FLDMMDL_DRAW_PROC_POP draw_pop_proc;	///<�`�敜�A�֐�
	
	///����֐��p���[�N
	u8 move_proc_work[FLDMMDL_MOVE_WORK_SIZE];
	///����T�u�֐��p���[�N
	u8 move_sub_proc_work[FLDMMDL_MOVE_SUB_WORK_SIZE];
	///����R�}���h�p���[�N
	u8 move_cmd_proc_work[FLDMMDL_MOVE_CMD_WORK_SIZE];
	///�`��֐��p���[�N
	u8 draw_proc_work[FLDMMDL_DRAW_WORK_SIZE];
	
	GFL_BBDACT_ACTUNIT_ID blActID;
}FLDMMDL;

#define FLDMMDL_SIZE (sizeof(FLDMMDL)) ///<FLDMMDL�T�C�Y

//--------------------------------------------------------------
///	FLDMMDL_HEADER_LOAD_FILE�\����
//--------------------------------------------------------------
typedef struct
{
	int zone;
	int add_max;			//�o�^�ő吔
	int add_count;			//�ǉ���
	const FLDMMDLSYS *sys;	//FLDMMDLSYS *
	FLDMMDL_HEADER *head;		//�w�b�_�[
}FLDMMDL_HEADER_LOAD_FILE;

///FLDMMDL_HEADER_LOAD_FILE�T�C�Y
#define FLDMMDL_HEADER_LOAD_FILE_SIZE (sizeof(FLDMMDL_HEADER_LOAD_FILE))

//======================================================================
//	proto
//======================================================================
//FLDMMDL �ǉ��A�폜
static void FldMMdl_SetHeader(
	FLDMMDL * fmmdl, const FLDMMDL_HEADER *head, void *sys );
static void FldMMdl_SetHeaderPos( FLDMMDL *fmmdl, const FLDMMDL_HEADER *head );
static void FldMMdl_InitWork( FLDMMDL * fmmdl, const FLDMMDLSYS *sys );
static void FldMMdl_InitMoveProcWork( FLDMMDL * fmmdl );
static void FldMMdl_InitMoveWork( FLDMMDL * fmmdl );

//FLDMMDL ����֐�
static void FldMMdl_TCB_MoveProc( GFL_TCB * tcb, void *work );
static void FldMMdl_TCB_DrawProc( FLDMMDL * fmmdl );

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
static void FldMMdlSys_AddFldMMdlTCB(
	const FLDMMDLSYS *sys, FLDMMDL * fmmdl );

//FLDMMDL �c�[��
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

//parts
static u16 WorkOBJCode_GetOBJCode( void *fsys, int code );
static const FLDMMDL_MOVE_PROC_LIST * MoveProcList_GetList( u16 code );
static const FLDMMDL_DRAW_PROC_LIST * DrawProcList_GetList( u16 code );
static BOOL FldMMdlHeader_CheckAlies( const FLDMMDL_HEADER *head );
static int FldMMdlHeader_GetAliesZoneID( const FLDMMDL_HEADER *head );

//======================================================================
//	�t�B�[���h���샂�f���@�V�X�e��
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMMDLSYS �V�X�e���쐬
 * @param	fsys	FIELDSYS_WORK *
 * @param	max	��������OBJ�ő吔
 * @retval	FLDMMDLSYS*	�ǉ����ꂽ�t�B�[���h���샂�f���V�X�e��*
 */
//--------------------------------------------------------------
FLDMMDLSYS * FLDMMDLSYS_Create(
	int max, HEAPID heapID, const FLDMAPPER *pG3DMapper )
{
	FLDMMDLSYS *fos;
	
	fos = GFL_HEAP_AllocClearMemory( heapID, FLDMMDLSYS_SIZE );
	fos->pFldMMdlBuf = GFL_HEAP_AllocClearMemory( heapID, max*FLDMMDL_SIZE );
	fos->fmmdl_max = max;
	fos->heapID = heapID;
//	fos->pFldMainWork = pFldMainWork;
	fos->pG3DMapper = pG3DMapper;
	
	fos->pTCBSysWork = GFL_HEAP_AllocMemory(
		heapID, GFL_TCB_CalcSystemWorkSize(max) );
	fos->pTCBSys = GFL_TCB_Init( max, fos->pTCBSysWork );
	return( fos );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS �V�X�e���폜
 * @param	fos		FLDMMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_Delete( FLDMMDLSYS *fos )
{
	GFL_TCB_Exit( fos->pTCBSys );
	GFL_HEAP_FreeMemory( fos->pTCBSysWork );
	GFL_HEAP_FreeMemory( fos->pFldMMdlBuf );
	GFL_HEAP_FreeMemory( fos );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS �S�Ă��폜
 * @param	fos		FLDMMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_DeleteAll( FLDMMDLSYS *fos )
{
	FLDMMDLSYS_DeleteMMdl( fos );
	FLDMMDLSYS_DeleteDraw( fos );
	FLDMMDLSYS_Delete( fos );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS ���샂�f���X�V
 * @param	fos	FLDMMDLSYS
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_UpdateMove( FLDMMDLSYS *fos )
{
	GFL_TCBSYS *tcbsys = fos->pTCBSys;
	if( tcbsys != NULL ){
		GFL_TCB_Main( tcbsys );
	}
}

//======================================================================
//	�t�B�[���h���샂�f���ǉ��A�폜
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMMDLSYS �t�B�[���h���샂�f����ǉ�
 * @param	fos			FLDMMDLSYS *
 * @param	head		�ǉ��������Z�߂�FLDMMDL_HEADER *
 * @param	zone_id		�]�[��ID
 * @retval	FLDMMDL	�ǉ����ꂽFLDMMDL *
 */
//--------------------------------------------------------------
FLDMMDL * FLDMMDLSYS_AddFldMMdl(
	const FLDMMDLSYS *fos, const FLDMMDL_HEADER *header, int zone_id )
{
	FLDMMDL *fmmdl;
	FLDMMDL_HEADER headdata = *header;
	FLDMMDL_HEADER *head = &headdata;
	
	fmmdl = FldMMdlSys_SearchSpaceFldMMdl( fos );
	GF_ASSERT( fmmdl != NULL );
	
	FldMMdl_SetHeader( fmmdl, head, NULL );
	FldMMdl_InitWork( fmmdl, fos );
	FLDMMDL_SetZoneID( fmmdl, zone_id );
	
	FldMMdl_InitMoveWork( fmmdl );
	FldMMdl_InitDrawWork( fmmdl );
	
	FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_MOVE_START );
	
	FldMMdlSys_AddFldMMdlTCB( fos, fmmdl );
	FldMMdlSys_IncrementOBJCount( (FLDMMDLSYS*)FLDMMDL_GetFldMMdlSys(fmmdl) );
	
	return( fmmdl );
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
	
	FLDMMDL_CallMoveDeleteProc( fmmdl );
	GFL_TCB_DeleteTask( fmmdl->pTCB );
	FldMMdlSys_DecrementOBJCount( (FLDMMDLSYS*)(fmmdl->pFldMMdlSys) );
	FldMMdl_ClearWork( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS ���ݔ������Ă���t�B�[���h���샂�f����S�č폜
 * @param	fmmdl		�폜����FLDMMDL * 
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
	
	pos = head->gy;		//pos�ݒ��fx32�^�ŗ���B
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
static void FldMMdl_InitMoveProcWork( FLDMMDL * fmmdl )
{
	const FLDMMDL_MOVE_PROC_LIST *list;
	list = MoveProcList_GetList( FLDMMDL_GetMoveCode(fmmdl) );
	fmmdl->move_init_proc = list->init_proc;
	fmmdl->move_proc = list->move_proc;
	fmmdl->move_delete_proc = list->delete_proc;
}

//--------------------------------------------------------------
/**
 * FLDMMDL ���쏉�����ɍs�������Z��
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_InitMoveWork( FLDMMDL * fmmdl )
{
	FldMMdl_InitMoveProcWork( fmmdl );
	FLDMMDL_InitMoveProc( fmmdl );
}

//======================================================================
//	FLDMMDL ����֐�
//======================================================================
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
	FLDMMDL_UpdateMove( fmmdl );
	
	if( FLDMMDL_CheckStatusBitUse(fmmdl) == TRUE ){
		FldMMdl_TCB_DrawProc( fmmdl );
	}
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
		FLDMMDLSYS_UpdateDraw( fmmdl );
	}
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
 * FLDMMDLSYS �A�[�J�C�u�n���h���Z�b�g
 * @param	fmmdlsys	FLDMMDLSYS *
 * @param	handle	ARCHANDLE *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_SetArcHandle( FLDMMDLSYS *fmmdlsys, ARCHANDLE *handle )
{
	fmmdlsys->pArcHandle = handle;
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS �A�[�J�C�u�n���h���擾
 * @param	fmmdlsys FLDMMDLSYS *
 * @retval	ARCHANDLE*�@ARCHANDLE *
 */
//--------------------------------------------------------------
ARCHANDLE * FLDMMDLSYS_GetArcHandle( FLDMMDLSYS *fmmdlsys )
{
	GF_ASSERT( fmmdlsys->pArcHandle != NULL );
	return( fmmdlsys->pArcHandle );
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
 * @retval	u32			�X�e�[�^�X�r�b�g
 */
//--------------------------------------------------------------
u32 FLDMMDL_GetStatusBit( const FLDMMDL * fmmdl )
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
u32 FLDMMDL_GetMoveBit( const FLDMMDL * fmmdl )
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
void FLDMMDL_OnMoveBit( FLDMMDL * fmmdl, u32 bit )
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
void FLDMMDL_OffMoveBit( FLDMMDL * fmmdl, u32 bit )
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
u32 FLDMMDL_CheckMoveBit( const FLDMMDL * fmmdl, u32 bit )
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
	GF_ASSERT( fmmdl->move_init_proc );
	fmmdl->move_init_proc( fmmdl );
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
	GF_ASSERT( fmmdl->move_proc );
	fmmdl->move_proc( fmmdl );
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
	GF_ASSERT( fmmdl->move_delete_proc );
	fmmdl->move_delete_proc( fmmdl );
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
	const FLDMMDL_MOVE_PROC_LIST *list =
		MoveProcList_GetList( FLDMMDL_GetMoveCode(fmmdl) );
	list->return_proc( fmmdl );
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
	fmmdl->draw_init_proc( fmmdl );
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
#ifndef FLDMMDL_PL_NULL
	fmmdl->draw_proc( fmmdl );
#endif
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
	fmmdl->draw_delete_proc( fmmdl );
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
	fmmdl->draw_push_proc( fmmdl );
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
	fmmdl->draw_pop_proc( fmmdl );
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
 * ����no�͌Ăяo����A�����ʒu+1�̒l�ɂȂ�B
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
 * FLDMMDLSYS �t�B�[���h���샂�f���@TCB����֐��ǉ�
 * @param	sys		FLDMMDLSYS*
 * @param	fmmdl	FLDMMDL*
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdlSys_AddFldMMdlTCB(
	const FLDMMDLSYS *sys, FLDMMDL * fmmdl )
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

//======================================================================
//	FLDMMDL �c�[��
//======================================================================
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
	FldMMdl_InitMoveProcWork( fmmdl );
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
	u32 code = FLDMMDL_GetOBJCode( fmmdl );
	
	if( code == NONDRAW ){
		list = &DATA_FieldOBJDraw_Non;
	}else{
		#ifndef FLDMMDL_PL_NULL
		list = DrawProcList_GetList( code );
		#else
		list = &DATA_FieldOBJDraw_Non;
		#endif
	}
	
	fmmdl->draw_init_proc = list->init_proc;
	fmmdl->draw_proc = list->draw_proc;
	fmmdl->draw_delete_proc = list->delete_proc;
	fmmdl->draw_push_proc = list->push_proc;
	fmmdl->draw_pop_proc = list->pop_proc;
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
BOOL FLDMMDL_CheckSameID( const FLDMMDL * fmmdl, u16 obj_id, int zone_id )
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
		const FLDMMDL * fmmdl, u16 code, u16 obj_id, int zone_id )
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
	if( code >= WKOBJCODE_ORG && code <= WKOBJCODE_END ){
		#ifndef FLDMMDL_PL_NULL
		code -= WKOBJCODE_ORG;
		code = GetEvDefineObjCode( fsys, code );
		#else
		GF_ASSERT( 0 );
		#endif
	}
	
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
static const FLDMMDL_DRAW_PROC_LIST * DrawProcList_GetList( u16 code )
{
#ifndef FLDMMDL_PL_NULL
	const FIELD_OBJ_DRAW_PROC_LIST_REG *tbl;
	tbl = DATA_FieldOBJDrawProcListRegTbl;
	
	do{
		if( tbl->code == code ){
			return( tbl->list );
		}
		tbl++;
	}while( tbl->code != OBJCODEMAX );
	
	GF_ASSERT( 0 );
	return( NULL );
#else
	return( NULL );
#endif
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

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//	��
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#if 0
FIELD_MAIN_WORK * FLDMMDLSYS_GetFieldMainWork( FLDMMDLSYS *fmmdlsys )
{
	return( fmmdlsys->pFldMainWork );
}
#endif

void FLDMMDL_SetBlActID( FLDMMDL *fmmdl, GFL_BBDACT_ACTUNIT_ID blActID )
{
	fmmdl->blActID = blActID;
}

GFL_BBDACT_ACTUNIT_ID FLDMMDL_GetBlActID( FLDMMDL *fmmdl )
{
	return( fmmdl->blActID );
}

const FLDMAPPER * FLDMMDLSYS_GetG3DMapper( const FLDMMDLSYS *fos )
{
	return( fos->pG3DMapper );
}
