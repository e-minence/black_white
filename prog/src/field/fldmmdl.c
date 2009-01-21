//******************************************************************************
/**
 * 
 * @file	fldmmdl.c
 * @brief	�t�B�[���h���샂�f��
 * @author	kagaya
 * @data	05.07.13
 *
 */
//******************************************************************************
#include "fldmmdl.h"

//==============================================================================
//	define
//==============================================================================
//--------------------------------------------------------------
//	debug
//--------------------------------------------------------------
#if 0
#ifdef PM_DEBUG
#define DEBUG_OBJID_SAM_CHECK		//��`��OBJ ID����`�F�b�N
#define DEBUG_PRINT_FLDMMDL_COUNT	//��`��OBJ���ݐ��o��

#ifdef DEBUG_ONLY_FOR_kagaya		//kagaya��p
//#define DEBUG_CHECK_NEWOBJ		//�V�KOBJ�`�F�b�N
#define DEBUG_FLDMMDL_PRINTF			//OS_Printf()�L��
#endif

#ifdef DEBUG_CHECK_NEWOBJ
#define DEBUG_NEWOBJCODE_0 ROTOMWALL
#define DEBUG_NEWOBJCODE_1 ROTOMWALL
#endif

#endif //PM_DEBUG
#endif

//--------------------------------------------------------------
///	�G�C���A�X�V���{��
//--------------------------------------------------------------
enum
{
	RET_ALIES_NOT = 0,	//�G�C���A�X�ł͂Ȃ�
	RET_ALIES_EXIST,	//�G�C���A�X�Ƃ��Ċ��ɑ��݂��Ă���
	RET_ALIES_CHANGE,	//�G�C���A�X�ύX���K�v�ł���
};

//==============================================================================
//	struct
//==============================================================================
//--------------------------------------------------------------
///	FLDMMDLSYS�\����
//--------------------------------------------------------------
typedef struct _TAG_FLDMMDLSYS
{
	u32 status_bit;					///<�X�e�[�^�X�r�b�g
	int fmmdl_max;					///<FLDMMDL�ő吔
	int fmmdl_count;				///<�t�B�[���h���샂�f�����ݐ�
	int tcb_pri;					///<TCB�v���C�I���e�B
	int blact_idx;					///<�r���{�[�h�A�N�^�[�C���f�b�N�X
	ARCHANDLE *archandle;			///<�A�[�J�C�u�n���h��
	FLDMMDL *fmmdl_work;			///<FLDMMDL���[�N *
	
#if 0
	FLDMMDL_BLACT_CONT blact_cont;			///<FLDMMDL_BLACT_CONT
	FLDMMDL_RENDER_CONT_PTR render_cont;	///<FLDMMDL_RENDER_CONT
	FIELDSYS_WORK *fieldsys;				///<FIELDSYS_WORK *
#endif
	
	HEAPID heapID;
	void *pTCBSysWork;
	GFL_TCBSYS *pTCBSys;
	FIELD_MAIN_WORK *pFldMainWork;
	
	const FLDMAPPER *pG3DMapper;

	FLDMMDL_BLACTCONT *pBlActCont;
}FLDMMDLSYS;

#define FLDMMDLSYS_SIZE (sizeof(FLDMMDLSYS)) ///<FLDMMDLSYS�T�C�Y

//--------------------------------------------------------------
///	FLDMMDL�\����
//--------------------------------------------------------------
typedef struct _TAG_FLDMMDL
{
	u32 status_bit;				///<�X�e�[�^�X�r�b�g
	u32 move_bit;				///<����r�b�g
	u32 obj_id;					///<OBJ ID
	u32 zone_id;				///<�]�[�� ID
	u32 obj_code;				///<OBJ�R�[�h
	u32 move_code;				///<����R�[�h
	u32 event_type;				///<�C�x���g�^�C�v
	u32 event_flag;				///<�C�x���g�t���O
	u32 event_id;				///<�C�x���gID
	int dir_head;				///<FLDMMDL_H�w�����
	int dir_disp;				///<���݌����Ă������
	int dir_move;				///<���ݓ����Ă������
	int dir_disp_old;			///<�ߋ��̓����Ă�������
	int dir_move_old;			///<�ߋ��̓����Ă�������
	int param0;					///<�w�b�_�w��p�����^
	int param1;					///<�w�b�_�w��p�����^
	int param2;					///<�w�b�_�w��p�����^
	int move_limit_x;			///<X�����ړ�����
	int move_limit_z;			///<Z�����ړ�����
	int gx_init;				///<�����O���b�hX
	int gy_init;				///<�����O���b�hY
	int gz_init;				///<�����O���b�hZ
	int gx_old;					///<�ߋ��O���b�hX
	int gy_old;					///<�ߋ��O���b�hY
	int gz_old;					///<�ߋ��O���b�hZ
	int gx_now;					///<���݃O���b�hX
	int gy_now;					///<���݃O���b�hY
	int gz_now;					///<���݃O���b�hZ
	VecFx32 vec_pos_now;		///<���ݎ������W
	VecFx32 vec_draw_offs;		///<�\�����W�I�t�Z�b�g
	VecFx32 vec_draw_offs_outside;		///<�O���w��\�����W�I�t�Z�b�g
	VecFx32 vec_attr_offs;				///<�A�g���r���[�g�ɂ����W�I�t�Z�b�g
	u32 draw_status;					///<�`��X�e�[�^�X
	int acmd_code;						///<�A�j���[�V�����R�}���h�R�[�h
	int acmd_seq;						///<�A�j���[�V�����R�}���h�V�[�P���X
	u16 now_attr;						///<���݂̃}�b�v�A�g���r���[�g
	u16 old_attr;						///<�ߋ��̃}�b�v�A�g���r���[�g
	
	GFL_TCB * tcb;						///<����֐�TCB*
	const FLDMMDLSYS *fmmdlsys;			///<FLDMMDLSYS�ւ� *
	
	FLDMMDL_MOVE_PROC_INIT move_init_proc;	///<�������֐�
	FLDMMDL_MOVE_PROC move_proc;			///<����֐�
	FLDMMDL_MOVE_PROC_DEL move_delete_proc;	///<�폜�֐�
	FLDMMDL_DRAW_PROC_INIT draw_init_proc;	///<�`�揉�����֐�
	FLDMMDL_DRAW_PROC draw_proc;			///<�`��֐�
	FLDMMDL_DRAW_PROC_DEL draw_delete_proc;	///<�`��폜�֐�
	FLDMMDL_DRAW_PROC_PUSH draw_push_proc;	///<�`��ޔ��֐�
	FLDMMDL_DRAW_PROC_POP draw_pop_proc;	///<�`�敜�A�֐�
	
	u8 move_proc_work[FLDMMDL_MOVE_WORK_SIZE];///<����֐��p���[�N
	u8 move_sub_proc_work[FLDMMDL_MOVE_SUB_WORK_SIZE];///<����T�u�֐��p���[�N
	u8 move_cmd_proc_work[FLDMMDL_MOVE_CMD_WORK_SIZE];///<����R�}���h�p���[�N
	u8 draw_proc_work[FLDMMDL_DRAW_WORK_SIZE];///<�`��֐��p���[�N
	
	GFL_BBDACT_ACTUNIT_ID blActID;
}FLDMMDL;

#define FLDMMDL_SIZE (sizeof(FLDMMDL)) ///<FLDMMDL�T�C�Y

HEAPID FLDMMDLSYS_GetHeapID( const FLDMMDLSYS *fos )
{
	return( fos->heapID );
}

HEAPID FLDMMDL_GetHeapID( const FLDMMDL *fmmdl )
{
	return( FLDMMDLSYS_GetHeapID(fmmdl->fmmdlsys) );
}

GFL_TCBSYS * FLDMMDLSYS_GetTCBSYS( const FLDMMDLSYS *fos )
{
	return( ((FLDMMDLSYS*)fos)->pTCBSys );
}

GFL_TCBSYS * FLDMMDL_GetTCBSYS( FLDMMDL *fmmdl )
{
	return( FLDMMDLSYS_GetTCBSYS(fmmdl->fmmdlsys) );
}

FIELD_MAIN_WORK * FLDMMDLSYS_GetFieldMainWork( FLDMMDLSYS *fldmmdlsys )
{
	return( fldmmdlsys->pFldMainWork );
}

void FLDMMDLSYS_SetBlActCont( FLDMMDLSYS *fldmmdlsys, FLDMMDL_BLACTCONT *pBlActCont )
{
	fldmmdlsys->pBlActCont = pBlActCont;
}

FLDMMDL_BLACTCONT * FLDMMDLSYS_GetBlActCont( FLDMMDLSYS *fldmmdlsys )
{
	GF_ASSERT( fldmmdlsys->pBlActCont != NULL );
	return( fldmmdlsys->pBlActCont );
}

FLDMMDLSYS * FLDMMDL_GetFldMMdlSys( const FLDMMDL *fmmdl )
{
	return( (FLDMMDLSYS*)(fmmdl->fmmdlsys) );
}

void FLDMMDL_SetBlActID( FLDMMDL *fldmmdl, GFL_BBDACT_ACTUNIT_ID blActID )
{
	fldmmdl->blActID = blActID;
}

GFL_BBDACT_ACTUNIT_ID FLDMMDL_GetBlActID( FLDMMDL *fldmmdl )
{
	return( fldmmdl->blActID );
}

const FLDMAPPER * FLDMMDLSYS_GetG3DMapper( const FLDMMDLSYS *fos )
{
	return( fos->pG3DMapper );
}

//--------------------------------------------------------------
///	FLDMMDL_H_LOAD_FILE�\����
//--------------------------------------------------------------
typedef struct
{
	int zone;
	int add_max;			//�o�^�ő吔
	int add_count;			//�ǉ���
	const FLDMMDLSYS *sys;	//FLDMMDLSYS *
	FLDMMDL_H *head;		//�w�b�_�[
}FLDMMDL_H_LOAD_FILE;

///FLDMMDL_H_LOAD_FILE�T�C�Y
#define FLDMMDL_H_LOAD_FILE_SIZE (sizeof(FLDMMDL_H_LOAD_FILE))

//==============================================================================
//	�v���g�^�C�v
//==============================================================================
static void fmmdl_SaveDataSave( void *fsys, FLDMMDL * fmmdl, FLDMMDL_SAVE_DATA_PTR save );
static void fmmdl_SaveDataLoad( FLDMMDL * fmmdl, FLDMMDL_SAVE_DATA_PTR save );
static void fmmdl_DataLoadRecover( const FLDMMDLSYS *fos, FLDMMDL * fmmdl );
static void fmmdl_DataLoadStatusBitRecover( FLDMMDL * fmmdl );
static void fmmdl_DataLoadPosRecover( FLDMMDL * fmmdl );

static void fmmdl_AddFileProc( FLDMMDL_H_LOAD_FILE *work );

static FLDMMDL * fmmdl_SpaceSearch( const FLDMMDLSYS *sys );
static FLDMMDL * fmmdl_AliesSearch( const FLDMMDLSYS *fos, int obj_id,int zone_id );
static void fmmdl_MoveTcbAdd( const FLDMMDLSYS *sys, FLDMMDL * fmmdl );
static void fmmdl_AddHeaderSet(
		FLDMMDL * fmmdl, const FLDMMDL_H *head, void *fsys );
static void fmmdl_AddHeaderPosInit( FLDMMDL * fmmdl, const FLDMMDL_H *head );
static void fmmdl_WorkInit( FLDMMDL * fmmdl, const FLDMMDLSYS *sys );
static void fmmdl_WorkInit_MoveProcInit( FLDMMDL * fmmdl );
static void fmmdl_WorkInit_DrawProcInit( FLDMMDL * fmmdl );
static void fmmdl_WorkClear( FLDMMDL * fmmdl );
static int fmmdl_HeaderAliesCheck(
		const FLDMMDL * fmmdl, int h_zone_id, int max, const FLDMMDL_H *head );
static FLDMMDL * fmmdl_OBJIDZoneIDSearch(
		const FLDMMDLSYS *fos, int obj_id, int zone_id );
static void fmmdl_DeleteAll_OldZoneID( FLDMMDLSYS *fos, u32 zone_id );
static void fmmdl_DrawInitStatusSet( FLDMMDL * fmmdl );
static void fmmdl_DrawEffectFlagInit( FLDMMDL * fmmdl );
static void fmmdl_DrawDeleteStatusSet( FLDMMDL * fmmdl );
static int fmmdl_OBJCodeWkOBJCodeConv( void *fsys, int code );
static void fmmdl_HeightNeedCheckInit( FLDMMDL * fmmdl );
static void fmmdl_MoveWorkInit( FLDMMDL * fmmdl );
static void fmmdl_DrawWorkInit( FLDMMDL * fmmdl );
static void fmmdl_AliesOBJChange( FLDMMDL * fmmdl, const FLDMMDL_H *head, int zone_id );
static void fmmdl_OBJAliesChange( FLDMMDL * fmmdl, int zone_id, const FLDMMDL_H *head );

static void fmmdl_TCB_MoveProc( GFL_TCB * tcb, void *work );
static void fmmdl_TCB_DrawProc( FLDMMDL * fmmdl );

static FLDMMDLSYS * fmmdl_ConstSysLocalGet( const FLDMMDLSYS *sys );
static void fmmdlsys_OBJCountInc( FLDMMDLSYS *sys );
static void fmmdlsys_OBJCountDec( FLDMMDLSYS *sys );
static void fmmdlsys_OBJCountSet( FLDMMDLSYS *sys, int count );
static FLDMMDL * fmmdlsys_FieldOBJWorkGet( const FLDMMDLSYS *sys );

static FLDMMDLSYS * fmmdl_FieldOBJSysGet( const FLDMMDL * fmmdl );

static const FLDMMDL_H * fmmdl_H_OBJIDSearch( int id, int max, const FLDMMDL_H *head );
static int fmmdl_H_AliesCheck( const FLDMMDL_H *head );
static int fmmdl_H_AliesZoneIDGet( const FLDMMDL_H *head );

static const FLDMMDL_MOVE_PROC_LIST * fmmdl_MoveProcListGet( u32 code );
static FLDMMDL_MOVE_PROC_INIT fmmdl_MoveProcList_InitGet(
		const FLDMMDL_MOVE_PROC_LIST *list );
static FLDMMDL_MOVE_PROC fmmdl_MoveProcList_MoveGet(
		const FLDMMDL_MOVE_PROC_LIST *list );
static FLDMMDL_MOVE_PROC_DEL fmmdl_MoveProcList_DeleteGet(
		const FLDMMDL_MOVE_PROC_LIST *list );
static FLDMMDL_DRAW_PROC_PUSH fmmdl_DrawProcList_PushGet(
		const FLDMMDL_DRAW_PROC_LIST *list );
static FLDMMDL_DRAW_PROC_POP fmmdl_DrawProcList_PopGet(
		const FLDMMDL_DRAW_PROC_LIST *list );

static FLDMMDL_DRAW_PROC_INIT fmmdl_DrawProcList_InitGet(
	const FLDMMDL_DRAW_PROC_LIST *list );
static FLDMMDL_DRAW_PROC fmmdl_DrawProcList_DrawGet( const FLDMMDL_DRAW_PROC_LIST *list );
static FLDMMDL_DRAW_PROC_DEL fmmdl_DrawProcList_DeleteGet(
		const FLDMMDL_DRAW_PROC_LIST *list );
static const FLDMMDL_DRAW_PROC_LIST * fmmdl_DrawProcListGet( u32 code );

//==============================================================================
//	�t�B�[���h���샂�f��	�V�X�e��
//==============================================================================
//--------------------------------------------------------------
/**
 * FIELD OBJ �V�X�e��������
 * @param		fsys			FIELDSYS_WORK *
 * @param		max 			��������OBJ�ő吔
 * @retval		FLDMMDLSYS*	�ǉ����ꂽ�t�B�[���h���샂�f��*
 */
//--------------------------------------------------------------
FLDMMDLSYS * FLDMMDLSYS_Init(
	FIELD_MAIN_WORK *pFldMainWork,
	const FLDMAPPER *pG3DMapper, HEAPID heapID, int max )
{
	FLDMMDLSYS *fos;
	
	fos = GFL_HEAP_AllocClearMemory( heapID, FLDMMDLSYS_SIZE );
	fos->fmmdl_work = GFL_HEAP_AllocClearMemory( heapID, max*FLDMMDL_SIZE );
	fos->fmmdl_max = max;
	fos->heapID = heapID;
	fos->pFldMainWork = pFldMainWork;
	fos->pG3DMapper = pG3DMapper;

	fos->pTCBSysWork = GFL_HEAP_AllocMemory(
		heapID, GFL_TCB_CalcSystemWorkSize(max) );
	fos->pTCBSys = GFL_TCB_Init( max, fos->pTCBSysWork );
	return( fos );
}

//--------------------------------------------------------------
/**
 * FIELD OBJ �V�X�e���폜
 * @param	fos		FLDMMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_Delete( FLDMMDLSYS *fos )
{
	GFL_TCB_Exit( fos->pTCBSys );
	GFL_HEAP_FreeMemory( fos->pTCBSys );
	GFL_HEAP_FreeMemory( fos->fmmdl_work );
	GFL_HEAP_FreeMemory( fos );
}

//--------------------------------------------------------------
/**
 * FIELD OBJ�@�S�Ă��폜
 * @param	fos		FLDMMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_DeleteAll( FLDMMDLSYS *fos )
{
	FLDMMDLSYS_DeleteMMdl( fos );
	FLDMMDLSYS_DrawDelete( fos );
	FLDMMDLSYS_Delete( fos );
}

//--------------------------------------------------------------
/**
 *
 * @param
 * @retval
 *
 */
//--------------------------------------------------------------
void FLDMMDLSYS_UpdateMove( FLDMMDLSYS *fos )
{
	GFL_TCBSYS *tcbsys = fos->pTCBSys;
	if( tcbsys != NULL ){
		GFL_TCB_Main( tcbsys );
	}
}

//--------------------------------------------------------------
/**
 * FIELD OBJ�@�]�[���X�V���̍폜����
 * @param	sys		FLDMMDLSYS *
 * @param	old_zone	�Â��]�[��ID
 * @param	new_zone	�V�����]�[��ID
 * @param	head_max	head�v�f��
 * @param	head	�V�����̃]�[���ɔz�u����FLDMMDL_H *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_ZoneUpdateDelete( FLDMMDLSYS *fos,
		int old_zone, int new_zone, int head_max, const FLDMMDL_H *head )
{
#if 0
	int ret,max = FLDMMDLSYS_OBJMaxGet( fos );
	FLDMMDL * fmmdl = FLDMMDLSYS_fmmdlWorkGet( fos );
	
	#ifdef DEBUG_FLDMMDL_PRINTF
	OS_Printf( "�Â�Zone�ɔz�u���ꂽ�t�B�[���h���샂�f���@�폜�J�n\n" );
	#endif
	
	while( max ){
		if( FLDMMDL_StatusBitCheck_Use(fmmdl) == TRUE ){
			ret = fmmdl_HeaderAliesCheck( fmmdl, new_zone, head_max, head );
			
			switch( ret ){
			case RET_ALIES_NOT:
				if( FLDMMDL_ZoneIDGet(fmmdl) != new_zone ){
					if( FLDMMDL_StatusBit_CheckEasy(
							fmmdl,FLDMMDL_STABIT_ZONE_DEL_NOT) == FALSE ){
						FLDMMDL_Delete( fmmdl );
					}
				}
				
				break;
			case RET_ALIES_CHANGE:
				#ifdef DEBUG_FLDMMDL_PRINTF
				OS_Printf(
					"�G�C���A�X�Ώۂ�OBJ���� OBJ_ID=%d ZONE_ID=%d GRID X=%d,GRID Z=%d \n",
					FLDMMDL_OBJIDGet(fmmdl),
					new_zone,
					FLDMMDL_NowPosGX_Get(fmmdl), FLDMMDL_NowPosGZ_Get(fmmdl) );
				#endif
				break;
			case RET_ALIES_EXIST:
				#ifdef DEBUG_FLDMMDL_PRINTF
				OS_Printf( "�G�C���A�X�Ώۂ�OBJ�����ɃG�C���A�X�����Ă��܂� " );
				OS_Printf( "OBJ_ID=%d ", FLDMMDL_OBJIDGet(fmmdl) );
				OS_Printf( "ZONE_ID=%d ", FLDMMDL_ZoneIDGetAlies(fmmdl) );
				OS_Printf( "�ǉ����̃]�[��ID=%d ", new_zone );
				OS_Printf( "GRID X=%d, ", FLDMMDL_NowPosGX_Get(fmmdl) );
				OS_Printf( "GRID Z=%d\n", FLDMMDL_NowPosGZ_Get(fmmdl) );
				#endif
				break;
			}
		}
		
		fmmdl++;
		max--;
	}
	
	FLDMMDL_BlActCont_ResmGuestDeleteAll( fos, FLDMMDLSYS_BlActContGet(fos) );
	#ifdef DEBUG_FLDMMDL_PRINTF
	OS_Printf( "�Â�Zone�ɔz�u���ꂽ�t�B�[���h���샂�f���@�폜����\n" );
	#endif
#endif
}

//==============================================================================
//	�t�B�[���h���샂�f���V�X�e���@�p�[�c
//==============================================================================
//--------------------------------------------------------------
/**
 * FIELD OBJ ��Ɨ̈�m��
 * @param	max				��������OBJ�ő吔
 * @retval	FLDMMDLSYS	�m�ۂ����̈�*
 */
//--------------------------------------------------------------
#if 0
static FLDMMDLSYS *fmmdlsys_AllocMemory( HEAPID heapID, int max )
{
	int size;
	FLDMMDL * fmmdl;
	FLDMMDLSYS *fos;
	
	fos = GFL_HEAP_AllocClearMemory( heapID, FLDMMDLSYS_SIZE );
	
	size = FLDMMDL_SIZE * max;
	fmmdl = GFL_HEAP_AllocClearMemory( heapID, size );
	
	FLDMMDLSYS_FieldOBJWorkSet( fos, fmmdl );
	return( fos );
}
#endif

//==============================================================================
//	�t�B�[���h���샂�f��
//==============================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f����ǉ� �����w�b�_�[
 * @param	fos			FLDMMDLSYS *
 * @param	head		�ǉ��������Z�߂�FLDMMDL_H *
 * @param	zone_id		�]�[��ID
 * @retval	FLDMMDL	�ǉ����ꂽFLDMMDL * �BNULL=�ǉ��s��
 */
//--------------------------------------------------------------
FLDMMDL * FLDMMDL_AddH(
	const FLDMMDLSYS *fos, const FLDMMDL_H *header, int zone_id )
{
	FLDMMDL * fmmdl;
	FLDMMDL_H headdata = *header;
	FLDMMDL_H *head = &headdata;
	
	fmmdl = fmmdl_SpaceSearch( fos );
	GF_ASSERT( fmmdl != NULL );
	
	fmmdl_AddHeaderSet( fmmdl, head, NULL );
	fmmdl_WorkInit( fmmdl, fos );
	FLDMMDL_ZoneIDSet( fmmdl, zone_id );
	
	fmmdl_MoveWorkInit( fmmdl );
	fmmdl_DrawWorkInit( fmmdl );
	
	FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_MOVE_START );
	
	fmmdl_MoveTcbAdd( fos, fmmdl );
	fmmdlsys_OBJCountInc( fmmdl_ConstSysLocalGet(fos) );
	
	return( fmmdl );
}

#if 0 //pl
FLDMMDL * FLDMMDL_AddH( const FLDMMDLSYS *fos, const FLDMMDL_H *headtest, int zone_id )
{
	FLDMMDL * fmmdl;
	FLDMMDL_H headdata = *headtest;
	FLDMMDL_H *head = &headdata;
	
#if 0	
	{
		int obj_id = FLDMMDL_H_IDGet( head );
		
		if( fmmdl_H_AliesCheck(head) == FALSE ){	//�G�C���A�X�ł͂Ȃ�
			fmmdl = fmmdl_AliesSearch( fos, obj_id, zone_id );
			
			if( fmmdl != NULL ){		//�G�C���A�X�Ŋ��ɑ��݂��Ă���
				//�G�C���A�X����t�B�[���h���샂�f����
				fmmdl_AliesOBJChange( fmmdl, head, zone_id );
				return( fmmdl );
			}
		}else{		//�G�C���A�X�ł���
			fmmdl = fmmdl_OBJIDZoneIDSearch(
				fos, obj_id, fmmdl_H_AliesZoneIDGet(head) );
			
			if( fmmdl != NULL ){	//���Ƀt�B�[���h���샂�f�������݂���
				//�t�B�[���h���샂�f������G�C���A�X��
				fmmdl_OBJAliesChange( fmmdl, zone_id, head );
				return( fmmdl );
			}
		}
	}
#endif
	
	fmmdl = fmmdl_SpaceSearch( fos );
	
	if( fmmdl == NULL ){
		return( fmmdl );
	}
	
//	fmmdl_AddHeaderSet( fmmdl, head, FLDMMDLSYS_FieldSysWorkGet(fos) );
	fmmdl_WorkInit( fmmdl, fos );
	FLDMMDL_ZoneIDSet( fmmdl, zone_id );
	
	fmmdl_MoveWorkInit( fmmdl );
	fmmdl_DrawWorkInit( fmmdl );
	
	FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_MOVE_START );
	
	fmmdl_MoveTcbAdd( fos, fmmdl );
	fmmdlsys_OBJCountInc( fmmdl_ConstSysLocalGet(fos) );
	
	return( fmmdl );
}
#endif

#if 0
FLDMMDL * FLDMMDL_AddH( const FLDMMDLSYS *fos, const FLDMMDL_H *head, int zone_id )
{
	FLDMMDL * fmmdl;
	
	{
		int obj_id = FLDMMDL_H_IDGet( head );
		
		if( fmmdl_H_AliesCheck(head) == FALSE ){				//�G�C���A�X�ł͂Ȃ�
			fmmdl = fmmdl_AliesSearch( fos, obj_id, zone_id );
			
			if( fmmdl != NULL ){								//�G�C���A�X�Ŋ��ɑ��݂��Ă���
				fmmdl_AliesOBJChange( fmmdl, head, zone_id );	//�G�C���A�X����t�B�[���h���샂�f����
				OS_Printf( "�G�C���A�X -> �t�B�[���h���샂�f�� OBJ_ID %d ", obj_id );
				OS_Printf( "GIRD X=%d ", FLDMMDL_H_PosXGet(head) );
				OS_Printf( "GIRD Z=%d\n", FLDMMDL_H_PosZGet(head) );
				return( fmmdl );
			}
		}else{													//�G�C���A�X�ł���
			fmmdl = fmmdl_OBJIDZoneIDSearch( fos, obj_id, fmmdl_H_AliesZoneIDGet(head) );
			
			if( fmmdl != NULL ){								//���Ƀt�B�[���h���샂�f�������݂���
				fmmdl_OBJAliesChange( fmmdl, zone_id, head );	//�t�B�[���h���샂�f������G�C���A�X��
				OS_Printf( "�t�B�[���h���샂�f�� -> �G�C���A�X OBJ_ID %d ", obj_id );
				OS_Printf( "GIRD X=%d ", FLDMMDL_H_PosXGet(head) );
				OS_Printf( "GIRD Z=%d\n", FLDMMDL_H_PosZGet(head) );
				return( fmmdl );
			}
			
			OS_Printf( "�G�C���A�X���z�u����܂� " );
			OS_Printf( "ZONE_ID %d, OBJ_ID %d ", obj_id, fmmdl_H_AliesZoneIDGet(head) );
			OS_Printf( "GIRD X=%d ", FLDMMDL_H_PosXGet(head) );
			OS_Printf( "GIRD Z=%d\n", FLDMMDL_H_PosZGet(head) );
		}
	}
	
	fmmdl = fmmdl_SpaceSearch( fos );
	
	if( fmmdl == NULL ){
		return( fmmdl );
	}
	
	fmmdl_AddHeaderSet( fmmdl, head, FLDMMDLSYS_FieldSysWorkGet(fos) );
	fmmdl_WorkInit( fmmdl, fos );
	FLDMMDL_ZoneIDSet( fmmdl, zone_id );
	
	fmmdl_MoveWorkInit( fmmdl );
	fmmdl_DrawWorkInit( fmmdl );
	
	FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_MOVE_START );
	
	fmmdl_MoveTcbAdd( fos, fmmdl );
	fmmdlsys_OBJCountInc( fmmdl_ConstSysLocalGet(fos) );
	
	return( fmmdl );
}
#endif

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f����ǉ��B�Œ���K�v�ȏ�񂩂�w�b�_�[�쐬���Ēǉ��B
 * ����Ȃ�����0�ŃN���A����Ă���B�K�v����Βǉ���Ɋe���Ŏ��R�ɐݒ�B
 * @param	fos			FLDMMDLSYS *
 * @param	x			�����O���b�h���WX
 * @param	z			�����O���b�h���WZ
 * @param	dir			���������BDIR_UP��
 * @param	obj			OBJ�R�[�h�BHERO��
 * @param	move		����R�[�h�BMV_RND��
 * @param	zone_id		�]�[��ID ZONE_ID_NOTHING��
 * @retval	FLDMMDL	�ǉ����ꂽFLDMMDL * �BNULL=�ǉ��s��
 */
//--------------------------------------------------------------
FLDMMDL * FLDMMDL_AddHMake(
		const FLDMMDLSYS *fos, int x, int z, int dir, int obj, int move, int zone_id )
{
	FLDMMDL_H head;
	FLDMMDL * fmmdl;
	
	FLDMMDL_H_IDSet( &head, 0 );
	FLDMMDL_H_OBJCodeSet( &head, obj );
	FLDMMDL_H_MoveCodeSet( &head, move );
	FLDMMDL_H_EventTypeSet( &head, 0 );
	FLDMMDL_H_EventFlagSet( &head, 0 );
	FLDMMDL_H_EventIDSet( &head, 0 );
	FLDMMDL_H_DirSet( &head, dir );
	FLDMMDL_H_ParamSet( &head, 0, FLDMMDL_PARAM_0 );
	FLDMMDL_H_ParamSet( &head, 0, FLDMMDL_PARAM_1 );
	FLDMMDL_H_ParamSet( &head, 0, FLDMMDL_PARAM_2 );
	FLDMMDL_H_MoveLimitXSet( &head, 0 );
	FLDMMDL_H_MoveLimitZSet( &head, 0 );
	FLDMMDL_H_PosXSet( &head, x );
	FLDMMDL_H_PosZSet( &head, z );
	FLDMMDL_H_PosYSet( &head, 0 );
	
	fmmdl = FLDMMDL_AddH( fos, &head, zone_id );
	
	return( fmmdl );
}

//--------------------------------------------------------------
/**
 * FLDMMDL_H�e�[�u�����w���ID��������OBJ��ǉ�
 * @param	fos		FLDMMDLSYS *
 * @param	id		�ǉ�����head����ID
 * @param	zone	�ǉ��̍ۂɎw�肷��]�[��ID
 * @param	max		head�v�f��
 * @param	head	FLDMMDL_H *
 * @retval	FLDMMDL *	�ǉ����ꂽFLDMMDL * NULL=id�Y������
 */
//--------------------------------------------------------------
FLDMMDL * FLDMMDL_AddHEvent(
		const FLDMMDLSYS *fos, int id, int max, int zone, const FLDMMDL_H *head )
{
	FLDMMDL * fmmdl = NULL;
	const FLDMMDL_H *hit_head = fmmdl_H_OBJIDSearch( id, max, head );
	
	if( hit_head != NULL ){
		int flag = FLDMMDL_H_EventFlagGet( hit_head );
//		void *fsys = FLDMMDLSYS_FieldSysWorkGet( fos );
#if 0	
		if( CheckEventFlag(fsys,flag) == FALSE ){
			fmmdl = FLDMMDL_AddH( fos, hit_head, zone );
		}
#else
		fmmdl = FLDMMDL_AddH( fos, hit_head, zone );
#endif
	}
	
	return( fmmdl );
}

//--------------------------------------------------------------
/**
 * FIELD OBJ�@�\���n�̂ݍĐݒ�@�폜�͍s��Ȃ�
 * @param	fmmdl	FLDMMDL *
 * @param	code	OBJ�R�[�h HERO��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_DrawResetAlone( FLDMMDL * fmmdl, int code )
{
	FLDMMDL_OBJCodeSet( fmmdl, code );
	fmmdl_DrawInitStatusSet( fmmdl );
	FLDMMDL_StatusBit_OFF( fmmdl, FLDMMDL_STABIT_DRAW_PROC_INIT_COMP );
	fmmdl_DrawWorkInit( fmmdl );
}

//--------------------------------------------------------------
/**
 * FIELD OBJ�@���݂̕`��֐����폜���čĐݒ�
 * @param	fmmdl	FLDMMDL *
 * @param	code	OBJ�R�[�h HERO��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_DrawReset( FLDMMDL * fmmdl, int code )
{
	if( FLDMMDL_StatusBit_DrawInitCompCheck(fmmdl) == TRUE ){
		FLDMMDL_DrawDelete( fmmdl );
	}
	
	FLDMMDL_DrawResetAlone( fmmdl, code );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f�����폜
 * @param	fmmdl		�폜����FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_Delete( FLDMMDL * fmmdl )
{
	const FLDMMDLSYS *fos;
	
	fos = FLDMMDL_FieldOBJSysGet( fmmdl );
	
	if( FLDMMDLSYS_DrawInitCompCheck(fos) == TRUE ){
		FLDMMDL_DrawDeleteProcCall( fmmdl );
	}
	
	FLDMMDL_MoveDeleteProcCall( fmmdl );
	FLDMMDL_MoveTcbDelete( fmmdl );
	fmmdlsys_OBJCountDec( fmmdl_FieldOBJSysGet(fmmdl) );
	fmmdl_WorkClear( fmmdl );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f�����폜�@�X�N���v�g��OBJ�폜�t���O��ON�ɂ���
 * @param	fmmdl		�폜����FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_DeleteEvent( FLDMMDL * fmmdl )
{
	int ev;

	ev = FLDMMDL_EventFlagGet( fmmdl );
//	SetEventFlag( FLDMMDL_FieldSysWorkGet(fmmdl), ev );
	FLDMMDL_Delete( fmmdl );
}

//--------------------------------------------------------------
/**
 * FIELD OBJ�@�\���n�̂ݍ폜
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_DrawDelete( FLDMMDL * fmmdl )
{
	const FLDMMDLSYS *fos;
	
	fos = FLDMMDL_FieldOBJSysGet( fmmdl );
	
	if( FLDMMDLSYS_DrawInitCompCheck(fos) == TRUE ){
		if( FLDMMDL_StatusBit_Check(fmmdl,FLDMMDL_STABIT_DRAW_PROC_INIT_COMP) ){
			FLDMMDL_DrawDeleteProcCall( fmmdl );
		}
		
		FLDMMDL_StatusBit_OFF( fmmdl, FLDMMDL_STABIT_DRAW_PROC_INIT_COMP );
	}
	
	FLDMMDL_OBJCodeSet( fmmdl, OBJCODEMAX );
	
	FLDMMDL_DrawInitProcSet( fmmdl, FLDMMDL_DrawInitProcDummy );
	FLDMMDL_DrawProcSet( fmmdl, FLDMMDL_DrawProcDummy );
	FLDMMDL_DrawDeleteProcSet( fmmdl, FLDMMDL_DrawProcDummy );
	FLDMMDL_DrawPushProcSet( fmmdl, FLDMMDL_DrawPushProcDummy );
	FLDMMDL_DrawPopProcSet( fmmdl, FLDMMDL_DrawPopProcDummy );
}

//--------------------------------------------------------------
/**
 * ���ݔ������Ă���t�B�[���h���샂�f����S�č폜
 * @param	fmmdl		�폜����FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_DeleteMMdl( FLDMMDLSYS *fos )
{
	int i,max;
	FLDMMDL * fmmdl;
	
	i = 0;
	max = FLDMMDLSYS_OBJMaxGet( fos );
	fmmdl = FLDMMDLSYS_fmmdlWorkGet( fos );
	
	do{
		if( FLDMMDL_StatusBit_Check(fmmdl,FLDMMDL_STABIT_USE) ){
			FLDMMDL_Delete( fmmdl );
		}
		
		fmmdl++;
		i++;
	}while( i < max );
}

//--------------------------------------------------------------
/**
 * ���ݔ������Ă���t�B�[���h���샂�f���S�Ă�ޔ�
 * @param	fmmdl		�폜����FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_PushAll( FLDMMDLSYS *fos )
{
	int i,max;
	FLDMMDL * fmmdl;
	
	i = 0;
	max = FLDMMDLSYS_OBJMaxGet( fos );
	fmmdl = FLDMMDLSYS_fmmdlWorkGet( fos );
	
	do{
		if( FLDMMDL_StatusBit_Check(fmmdl,FLDMMDL_STABIT_USE) ){
			if( FLDMMDL_StatusBitCheck_DrawProcInitComp(fmmdl) == TRUE ){
				FLDMMDL_DrawPushProcCall( fmmdl );
			}
		}
		
		fmmdl++;
		i++;
	}while( i < max );
}

//--------------------------------------------------------------
/**
 * ���ݔ������Ă���t�B�[���h���샂�f���S�Ăɂ������ĕ`�揈���̑ޔ����s��
 * @param	fos		FLDMMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_DrawProcPushAll( FLDMMDLSYS *fos )
{
	#ifdef DEBUG_FLDMMDL_PRINTF
	if( FLDMMDLSYS_DrawInitCompCheck(fos) != TRUE ){
		OS_Printf( "FLDMMDL �`�揈�������݂��Ă��Ȃ��̂�" );
		OS_Printf( "FLDMMDL_DrawProcPushAll()���Ă΂�Ă��܂�" );
	}
	#endif
	
	GF_ASSERT( FLDMMDLSYS_DrawInitCompCheck(fos) == TRUE );
	
	{
		int i,max;
		FLDMMDL * fmmdl;
	
		i = 0;
		max = FLDMMDLSYS_OBJMaxGet( fos );
		fmmdl = FLDMMDLSYS_fmmdlWorkGet( fos );
	
		do{
			if( FLDMMDL_StatusBit_Check(fmmdl,FLDMMDL_STABIT_USE) ){
				if( FLDMMDL_StatusBit_Check(fmmdl,FLDMMDL_STABIT_DRAW_PROC_INIT_COMP) ){
					FLDMMDL_DrawPushProcCall( fmmdl );
					fmmdl_DrawDeleteStatusSet( fmmdl );
				}
			}
		
			fmmdl++;
			i++;
		}while( i < max );
	}
	
	#ifdef DEBUG_FLDMMDL_PRINTF
	OS_Printf( "�t�B�[���h���샂�f�� �`�����ޔ����܂���\n" );
	#endif
}

//--------------------------------------------------------------
/**
 * ���ݔ������Ă���t�B�[���h���샂�f���S�Ă𕜋A
 * @param	fmmdl		�폜����FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_PopAll( FLDMMDLSYS *fos )
{
	int i,max;
	FLDMMDL * fmmdl;
	
	i = 0;
	max = FLDMMDLSYS_OBJMaxGet( fos );
	fmmdl = FLDMMDLSYS_fmmdlWorkGet( fos );
	
	do{
		if( FLDMMDL_StatusBitCheck_Use(fmmdl) == TRUE ){
			if( FLDMMDL_StatusBitCheck_DrawProcInitComp(fmmdl) == TRUE ){
				FLDMMDL_DrawPopProcCall( fmmdl );
			}
		}
	
		fmmdl++;
		i++;
	}while( i < max );
}

//--------------------------------------------------------------
/**
 * ���ݔ������Ă���t�B�[���h���샂�f���S�Ăɑ΂��ĕ`�揈�����A
 * @param	fmmdl		�폜����FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_DrawProcPopAll( FLDMMDLSYS *fos )
{
	#ifdef DEBUG_FLDMMDL_PRINTF
	if( FLDMMDLSYS_DrawInitCompCheck(fos) != TRUE ){
		OS_Printf("FLDMMDL �`�揈�����������ŕ`�敜�A�������Ă΂�Ă��܂�\n");
	}
	#endif
	GF_ASSERT( FLDMMDLSYS_DrawInitCompCheck(fos) == TRUE );
	
	{
		int i = 0;
		int max = FLDMMDLSYS_OBJMaxGet( fos );
		FLDMMDL * fmmdl = FLDMMDLSYS_fmmdlWorkGet( fos );
	
		do{
			if( FLDMMDL_StatusBitCheck_Use(fmmdl) == TRUE ){
				if( FLDMMDL_StatusBitCheck_DrawProcInitComp(fmmdl) == TRUE ){
					FLDMMDL_DrawPopProcCall( fmmdl );
				}else{
					fmmdl_DrawWorkInit( fmmdl );
				}
				
				fmmdl_DrawInitStatusSet( fmmdl );
				FLDMMDL_DrawInitAfterMoveProcCall( fmmdl );
			}
		
			fmmdl++;
			i++;
		}while( i < max );
	}
	
	#ifdef DEBUG_FLDMMDL_PRINTF
	OS_Printf( "�t�B�[���h���샂�f�� �`����𕜋A���܂���\n" );
	#endif
}

//==============================================================================
//	�t�B�[���h���샂�f�� �Z�[�u
//==============================================================================
//--------------------------------------------------------------
/**
 * ���ݔ������Ă���t�B�[���h���샂�f���S�Ă��Z�[�u�f�[�^�ɕۑ�
 * @param	fos		FLDMMDLSYS *
 * @param	save	�ۑ���FLDMMDL_SAVE_DATA_PTR
 * @param	max		save�̗v�f��
 * @retval	nothing
 */
//--------------------------------------------------------------
#if 0
void FLDMMDLSYS_DataSaveAll(
		void *fsys, const FLDMMDLSYS *fos, FLDMMDL_SAVE_DATA_PTR save, int max )
{
	int no = 0;
	FLDMMDL * fmmdl;
	
	while( FLDMMDLSYS_FieldOBJSearch(fos,&fmmdl,&no,FLDMMDL_STABIT_USE) ){
		fmmdl_SaveDataSave( fsys, fmmdl, save );
		save++; max--;
		GF_ASSERT( max > 0 && "FLDMMDLSYS_DataSaveAll()�ُ�" );
	}
	
	if( max ){
		GFL_STD_MemClear( save, max * FLDMMDL_SAVE_DATA_SIZE );
	}
}
#endif

//--------------------------------------------------------------
/**
 * �Z�[�u�����f�[�^�����[�h
 * @param	fos		FLDMMDLSYS *
 * @param	save	�ۑ������f�[�^���i�[���ꂽFLDMMDL_SAVE_DATA_PTR
 * @param	max		save�̗v�f��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_DataLoadAll(
		const FLDMMDLSYS *fos, FLDMMDL_SAVE_DATA_PTR save, int max )
{
	int i = 0;
	FLDMMDL * fmmdl;
	
	while( max ){
		if( (save->status_bit & FLDMMDL_STABIT_USE) ){
			fmmdl = fmmdl_SpaceSearch( fos );
			GF_ASSERT( fmmdl != NULL && "FLDMMDLSYS_DataLoadAll()�ُ�" );
		
			fmmdl_SaveDataLoad( fmmdl, save );
			fmmdl_DataLoadRecover( fos, fmmdl );
		}
		
		save++;
		max--;
	}
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���̏����Z�[�u�f�[�^�ɃZ�b�g
 * @param	fmmdl		FLDMMDL *
 * @param	save		FLDMMDL_SAVE_DATA_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_SaveDataSave( void *fsys, FLDMMDL * fmmdl, FLDMMDL_SAVE_DATA_PTR save )
{
	save->status_bit = FLDMMDL_StatusBit_Get( fmmdl );
	save->move_bit = FLDMMDL_MoveBit_Get( fmmdl );
	save->obj_id = FLDMMDL_OBJIDGet( fmmdl );
	save->zone_id = FLDMMDL_ZoneIDGet( fmmdl );
	save->obj_code = FLDMMDL_OBJCodeGet( fmmdl );
	save->move_code = FLDMMDL_MoveCodeGet( fmmdl );
	save->event_type = FLDMMDL_EventTypeGet( fmmdl );
	save->event_flag = FLDMMDL_EventFlagGet( fmmdl );
	save->event_id = FLDMMDL_EventIDGet( fmmdl );
	save->dir_head = FLDMMDL_DirHeaderGet( fmmdl );
	save->dir_disp = FLDMMDL_DirDispGet( fmmdl );
	save->dir_move = FLDMMDL_DirMoveGet( fmmdl );
	save->param0 = FLDMMDL_ParamGet( fmmdl, FLDMMDL_PARAM_0 );
	save->param1 = FLDMMDL_ParamGet( fmmdl, FLDMMDL_PARAM_1 );
	save->param2 = FLDMMDL_ParamGet( fmmdl, FLDMMDL_PARAM_2 );
	save->move_limit_x = FLDMMDL_MoveLimitXGet( fmmdl );
	save->move_limit_z = FLDMMDL_MoveLimitZGet( fmmdl );
	save->gx_init = FLDMMDL_InitPosGX_Get( fmmdl );
	save->gy_init = FLDMMDL_InitPosGY_Get( fmmdl );
	save->gz_init = FLDMMDL_InitPosGZ_Get( fmmdl );
	save->gx_now = FLDMMDL_NowPosGX_Get( fmmdl );
	save->gy_now = FLDMMDL_NowPosGY_Get( fmmdl );
	save->gz_now = FLDMMDL_NowPosGZ_Get( fmmdl );

	//�����̕ۑ�
#if 0
	{
		HEIGHT_TYPE flag;
		fx32 x,y,z;
		VecFx32 vec_pos;
		//�O���b�h��FX32�^�ɂ���(�O���b�h�̒��S���W)
		x = save->gx_now * 16 * FX32_ONE+(8*FX32_ONE);
		z = save->gz_now * 16 * FX32_ONE+(8*FX32_ONE);
		FLDMMDL_VecPosGet( fmmdl, &vec_pos );

		y = GetHeightPack( fsys, vec_pos.y, x, z, &flag );
		
		if( flag == HEIGHT_FAIL ){
			save->fx32_y = save->gy_now * 8 * FX32_ONE;
//			OS_Printf("height_FAIL:%x\n",save->fx32_y);
		}else{
			save->fx32_y = y;
//			OS_Printf("height_SAC:%x\n",save->fx32_y);
		}
	}
#else
	{
		VecFx32 vec;
		int flag,eflag;
		
		FieldOBJTool_GridCenterPosGet( save->gx_now, save->gz_now, &vec );
		vec.y = FLDMMDL_VecPosYGet( fmmdl );
		
		eflag = FLDMMDL_StatusBitCheck_HeightExpand( fmmdl );
		#if 0
		flag = FieldOBJTool_GetHeightExpand( fsys, &vec, eflag );
		#else
		flag = FALSE;
		#endif
		
		if( flag == FALSE ){
			#if 0
			//���ꂾ�ƍ����擾���s���A
			//����܂Ő���Ɏ擾���Ă��������𖳎����ăO���b�h�P�ʂ���
			//�����W�ɕϊ����Ă��܂��A�΂ߓ��̒[���������ƂȂ��Ă��܂��B
			save->fx32_y = H_GRID_SIZE_FX32( save->gy_now );
			#else
			save->fx32_y = FLDMMDL_VecPosYGet( fmmdl );	
			#endif
		}else{
			#if 1 //PL �����擾�֎~���͎����W���̂܂ܕۑ�
			if( FLDMMDL_HeightOFFCheck(fmmdl) == TRUE ){
				vec.y = FLDMMDL_VecPosYGet( fmmdl );
			}
			#endif
			
			save->fx32_y = vec.y;
		}
	}
#endif
	
	memcpy( save->move_proc_work,
			FLDMMDL_MoveProcWorkGet(fmmdl), FLDMMDL_MOVE_WORK_SIZE );
	memcpy( save->move_sub_proc_work,
			FLDMMDL_MoveSubProcWorkGet(fmmdl), FLDMMDL_MOVE_SUB_WORK_SIZE );
}

//--------------------------------------------------------------
/**
 * �Z�[�u�f�[�^�̏����t�B�[���h���샂�f���ɃZ�b�g
 * @param	fmmdl		FLDMMDL *
 * @param	save		FLDMMDL_SAVE_DATA_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_SaveDataLoad( FLDMMDL * fmmdl, FLDMMDL_SAVE_DATA_PTR save )
{
	FLDMMDL_StatusBit_Init( fmmdl, save->status_bit );
	FLDMMDL_MoveBit_Init( fmmdl, save->move_bit );
	FLDMMDL_OBJIDSet( fmmdl,  save->obj_id );
	FLDMMDL_ZoneIDSet( fmmdl, save->zone_id );
	FLDMMDL_OBJCodeSet( fmmdl, save->obj_code );
	FLDMMDL_MoveCodeSet( fmmdl, save->move_code );
	FLDMMDL_EventTypeSet( fmmdl, save->event_type );
	FLDMMDL_EventFlagSet( fmmdl, save->event_flag );
	FLDMMDL_EventIDSet( fmmdl, save->event_id );
	FLDMMDL_DirHeaderSet( fmmdl, save->dir_head );
	FLDMMDL_DirDispSetForce( fmmdl, save->dir_disp );
	FLDMMDL_DirMoveSet( fmmdl, save->dir_move );
	FLDMMDL_ParamSet( fmmdl, save->param0, FLDMMDL_PARAM_0 );
	FLDMMDL_ParamSet( fmmdl, save->param1, FLDMMDL_PARAM_1 );
	FLDMMDL_ParamSet( fmmdl, save->param2, FLDMMDL_PARAM_2 );
	FLDMMDL_MoveLimitXSet( fmmdl, save->move_limit_x );
	FLDMMDL_MoveLimitZSet( fmmdl, save->move_limit_z );
	FLDMMDL_InitPosGX_Set( fmmdl, save->gx_init );
	FLDMMDL_InitPosGY_Set( fmmdl, save->gy_init );
	FLDMMDL_InitPosGZ_Set( fmmdl, save->gz_init );
	FLDMMDL_NowPosGX_Set( fmmdl, save->gx_now );
	FLDMMDL_NowPosGY_Set( fmmdl, save->gy_now );
	FLDMMDL_NowPosGZ_Set( fmmdl, save->gz_now );

	//�������A
	{
		VecFx32 vec = {0,0,0};
		vec.y = save->fx32_y;
		FLDMMDL_VecPosSet( fmmdl, &vec );
	}
	
	memcpy( FLDMMDL_MoveProcWorkGet(fmmdl),
			save->move_proc_work, FLDMMDL_MOVE_WORK_SIZE );
	memcpy( FLDMMDL_MoveSubProcWorkGet(fmmdl),
			save->move_sub_proc_work, FLDMMDL_MOVE_SUB_WORK_SIZE );
}

//--------------------------------------------------------------
/**
 * �f�[�^���[�h��̕��A
 * @param	fos		FLDMMDLSYS *
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_DataLoadRecover( const FLDMMDLSYS *fos, FLDMMDL * fmmdl )
{
	fmmdl_DataLoadStatusBitRecover( fmmdl );
	fmmdl_DataLoadPosRecover( fmmdl );
	
	FLDMMDL_FieldOBJSysSet( fmmdl, fos );
	
	fmmdl_WorkInit_MoveProcInit( fmmdl );						//����֐��̏������̂�
	FLDMMDL_AcmdFree( fmmdl );
	fmmdl_DrawWorkInit( fmmdl );
	
	fmmdl_MoveTcbAdd( fos, fmmdl );
	FLDMMDL_MoveReturnProcCall( fmmdl );
	fmmdlsys_OBJCountInc( fmmdl_ConstSysLocalGet(fos) );
}

//--------------------------------------------------------------
/**
 * �f�[�^���[�h��̃X�e�[�^�X�r�b�g���A
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_DataLoadStatusBitRecover( FLDMMDL * fmmdl )
{
	FLDMMDL_StatusBit_ON( fmmdl,
			FLDMMDL_STABIT_USE |
//			FLDMMDL_STABIT_HEIGHT_GET_NEED |	//�Z�[�u���̍�����M�p����
			FLDMMDL_STABIT_MOVE_START );
	
	FLDMMDL_StatusBit_OFF( fmmdl,
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
	
	fmmdl_DrawEffectFlagInit( fmmdl );
}

//--------------------------------------------------------------
/**
 * �f�[�^���[�h��̍��W���A
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_DataLoadPosRecover( FLDMMDL * fmmdl )
{
	int pos;
	VecFx32 vec;
	
	FLDMMDL_VecPosGet( fmmdl, &vec );		//Y�l�̂݃Z�b�g�ς�

//	OS_Printf("recover_y:%x\n",vec.y);
	
	pos = FLDMMDL_NowPosGX_Get( fmmdl );
	vec.x = GRID_SIZE_FX32( pos ) + FLDMMDL_VEC_X_GRID_OFFS_FX32;
	FLDMMDL_OldPosGX_Set( fmmdl, pos );

	pos = FLDMMDL_NowPosGY_Get( fmmdl );
//	vec.y = H_GRID_SIZE_FX32( pos ) + FLDMMDL_VEC_Y_GRID_OFFS_FX32;	<<���[�h���ɒl���Z�b�g����Ă���͂�
	FLDMMDL_OldPosGY_Set( fmmdl, pos );
	
	pos = FLDMMDL_NowPosGZ_Get( fmmdl );
	vec.z = GRID_SIZE_FX32( pos ) + FLDMMDL_VEC_Z_GRID_OFFS_FX32;
	FLDMMDL_OldPosGZ_Set( fmmdl, pos );
	
	FLDMMDL_VecPosSet( fmmdl, &vec );
}

//==============================================================================
//	�t�B�[���h���샂�f�� �����o�^
//==============================================================================
#if 0	//�p����

//--------------------------------------------------------------
///	FLDMMDL_AddFile()�Ńt�@�C�����ǂݍ��݂Ŏg�p����o�b�t�@�T�C�Y
//--------------------------------------------------------------
#define FLDMMDL_FILE_1LINE_BUF (96)

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���@�t�@�C�����w��ɂ�镡���o�^
 * @param	sys			FLDMMDLSYS *
 * @param	zone		�ǉ������]�[��ID
 * @param	name		FLDMMDL_H���L�q���ꂽ�t�@�C����
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_AddFile( const FLDMMDLSYS *sys, int zone, const char *name )
{
	int i,max;
	char *file,*f_pos;
	char buf[FLDMMDL_FILE_1LINE_BUF];
	FLDMMDL_H *head;
	FLDMMDL_H_LOAD_FILE *load;
	
	if( name == NULL ){
		return;
	}
	
	file = sys_LoadFile( HEAPID_WORLD, name );
	GF_ASSERT( file != NULL && "fieldobj add file alloc error" );
	
	StrTok( file, buf, ',' );								//�o�^��
	max = AtoI( buf );
	
	OS_Printf( "fieldobj add file load %d\n", max );
	
	head = GFL_HEAP_AllocClearMemoryLo( HEAPID_WORLD, FLDMMDL_H_SIZE * max );
	GF_ASSERT( head != NULL && "fieldobj add file head alloc error" );
	
	f_pos = StrTok( file, buf, RETURN_CODE );				//1�s��΂�
	f_pos = StrTok( f_pos, buf, RETURN_CODE );				//�i����΂�
	
	for( i = 0; i < max; i++ ){
		f_pos = StrTok( f_pos, buf, ',' );					//ID�擾
		FLDMMDL_H_IDSet( &head[i], AtoI(buf) );
		f_pos = StrTok( f_pos, buf, RETURN_CODE );			//1�s��΂�
		
		f_pos = StrTok( f_pos, buf, ',' );					//OBJ�R�[�h�擾
		FLDMMDL_H_OBJCodeSet( &head[i], AtoI(buf) );
		f_pos = StrTok( f_pos, buf, RETURN_CODE );			//1�s��΂�
		
		f_pos = StrTok( f_pos, buf, ',' );					//����R�[�h�擾
		FLDMMDL_H_MoveCodeSet( &head[i], AtoI(buf) );
		f_pos = StrTok( f_pos, buf, RETURN_CODE );			//1�s��΂�
		
		f_pos = StrTok( f_pos, buf, ',' );					//�C�x���g�^�C�v�擾
		FLDMMDL_H_EventTypeSet( &head[i], AtoI(buf) );
		f_pos = StrTok( f_pos, buf, RETURN_CODE );			//1�s��΂�
		
		f_pos = StrTok( f_pos, buf, ',' );					//�C�x���g�t���O�擾
		FLDMMDL_H_EventFlagSet( &head[i], AtoI(buf) );
		f_pos = StrTok( f_pos, buf, RETURN_CODE );			//1�s��΂�
		
		f_pos = StrTok( f_pos, buf, ',' );					//�C�x���gID�擾
		FLDMMDL_H_EventIDSet( &head[i], AtoI(buf) );
		f_pos = StrTok( f_pos, buf, RETURN_CODE );			//1�s��΂�
		
		f_pos = StrTok( f_pos, buf, ',' );					//�����擾
		FLDMMDL_H_DirSet( &head[i], AtoI(buf) );
		f_pos = StrTok( f_pos, buf, RETURN_CODE );			//1�s��΂�
		
		f_pos = StrTok( f_pos, buf, ',' );					//�p�����^�擾
		FLDMMDL_H_ParamSet( &head[i], AtoI(buf) );
		f_pos = StrTok( f_pos, buf, RETURN_CODE );			//1�s��΂�
		
		f_pos = StrTok( f_pos, buf, ',' );					//�ړ�����X�擾
		FLDMMDL_H_MoveLimitXSet( &head[i], AtoI(buf) );
		
		f_pos = StrTok( f_pos, buf, ',' );					//�ړ�����Z�擾
		FLDMMDL_H_MoveLimitZSet( &head[i], AtoI(buf) );
		f_pos = StrTok( f_pos, buf, RETURN_CODE );			//1�s��΂�
		
		f_pos = StrTok( f_pos, buf, ',' );					//X���W�擾
		FLDMMDL_H_PosXSet( &head[i], AtoI(buf) );
		
		f_pos = StrTok( f_pos, buf, ',' );					//Z���W�擾
		FLDMMDL_H_PosZSet( &head[i], AtoI(buf) );
		f_pos = StrTok( f_pos, buf, RETURN_CODE );			//1�s��΂�
		
		f_pos = StrTok( f_pos, buf, ',' );					//Y���W�擾
		FLDMMDL_H_PosYSet( &head[i], AtoI(buf) );
		f_pos = StrTok( f_pos, buf, RETURN_CODE );			//��s��΂�
		
		f_pos = StrTok( f_pos, buf, RETURN_CODE );			//�i����΂�
	}
	
	GFL_HEAP_FreeMemory( HEAPID_WORLD, file );
	
	load = GFL_HEAP_AllocClearMemoryLo( HEAPID_WORLD, FLDMMDL_H_LOAD_FILE_SIZE );
	GF_ASSERT( load != NULL && "fieldobj add file work alloc error" );
	
	load->zone = zone;
	load->add_max = max;
	load->add_count = 0;
	load->sys = sys;
	load->head = head;
	
	fmmdl_AddFileProc( load );
	
	OS_Printf( "fieldobj add file load OK\n" );
}
#endif

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���@�o�C�i���w��ɂ�镡���o�^
 * @param	sys			FLDMMDLSYS *
 * @param	zone		�ǉ������]�[��ID
 * @param	max			�ǉ�����ő吔
 * @param	bin			�ǉ�����FLDMMDL_H���z�u����Ă���o�C�i��*
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_AddBin( const FLDMMDLSYS *sys, int zone, int max, const FLDMMDL_H *bin )
{
	int size;
	FLDMMDL_H *head;
	FLDMMDL_H_LOAD_FILE *load;
	
	#ifdef DEBUG_FLDMMDL_PRINTF
	OS_Printf( "�t�B�[���h���샂�f�� �ꊇ�o�^�J�n �z�u�� %d\n", max );
	if( max == 0 ){
		OS_Printf( "FLDMMDL_AddBin()�@�w��ő吔�ُ�" );
	}
	#endif
	GF_ASSERT( max );
	
	size = FLDMMDL_H_SIZE *max;
	head = GFL_HEAP_AllocClearMemoryLo( FLDMMDLSYS_GetHeapID(sys), size );
	GF_ASSERT( head != NULL && "fieldobj add bin head alloc error" );
	memcpy( head, bin, size );
	
	load = GFL_HEAP_AllocClearMemoryLo( FLDMMDLSYS_GetHeapID(sys),
			FLDMMDL_H_LOAD_FILE_SIZE );
	GF_ASSERT( load != NULL && "fieldobj add file work alloc error" );
	
	load->zone = zone;
	load->add_max = max;
	load->add_count = 0;
	load->sys = sys;
	load->head = head;
	
	fmmdl_AddFileProc( load );
	
	#ifdef DEBUG_FLDMMDL_PRINTF
	OS_Printf( "�t�B�[���h���샂�f�� �ꊇ�o�^�I��\n" );
	#endif
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���ꊇ�o�^
 * @param	work	FLDMMDL_H_LOAD_FILE
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_AddFileProc( FLDMMDL_H_LOAD_FILE *work )
{
#if 0
	FLDMMDL * fmmdl;
	void *fsys;
	const FLDMMDL_H *head;
	
	fsys = FLDMMDLSYS_FieldSysWorkGet( work->sys );
	head = work->head;

	do{
		#if 0	
		if( fmmdl_H_AliesCheck(head) == TRUE ||
			CheckEventFlag(fsys,head->event_flag) == FALSE ){
			fmmdl = FLDMMDL_AddH( work->sys, head, work->zone );
			GF_ASSERT( fmmdl != NULL );
		}
		#else
		fmmdl = FLDMMDL_AddH( work->sys, head, work->zone );
		GF_ASSERT( fmmdl != NULL );
		#endif

		head++;
		work->add_count++;
	}while( work->add_count < work->add_max );
	
	GFL_HEAP_FreeMemory( work->head );
	GFL_HEAP_FreeMemory( work );
#endif
}

#if 0	//���Ł@�G�C���A�X�ł̃o�O����
static void fmmdl_AddFileProc( FLDMMDL_H_LOAD_FILE *work )
{
	FLDMMDL * fmmdl;
	FIELDSYS_WORK *fsys;
	const FLDMMDL_H *head;
	
	fsys = FLDMMDLSYS_FieldSysWorkGet( work->sys );
	head = work->head;
	
	do{
		if( CheckEventFlag(fsys,head->event_flag) == FALSE ){
			fmmdl = FLDMMDL_AddH( work->sys, head, work->zone );
			GF_ASSERT( fmmdl != NULL && "�t�B�[���h���샂�f���̈ꊇ�o�^�Ɏ��s���܂���\n" );
		}else{
			OS_Printf( "�t�B�[���h���샂�f���ꊇ�o�^���c�C�x���g�t���O��FALSE�ׁ̈A" );
			OS_Printf( "OBJ ID=%d,",FLDMMDL_H_IDGet(head) );
			OS_Printf( "GIRD X=%d,",FLDMMDL_H_PosXGet(head) );
			OS_Printf( "GIRD Z=%d",FLDMMDL_H_PosZGet(head) );
			OS_Printf( "�ɔz�u����OBJ�̒ǉ��𖳎����܂���\n" );
		}
	
		head++;
		work->add_count++;
	}while( work->add_count < work->add_max );
	
	GFL_HEAP_FreeMemory( HEAPID_WORLD, work->head );
	GFL_HEAP_FreeMemory( HEAPID_WORLD, work );
}
#endif

//==============================================================================
//	�t�B�[���h���샂�f���@�p�[�c
//==============================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���̋󂫂�T��
 * @param	sys			FLDMMDLSYS *
 * @retval	FLDMMDL	�󂫂�FLDMMDL*�@�󂫂������ꍇ��NULL
 */
//--------------------------------------------------------------
static FLDMMDL * fmmdl_SpaceSearch( const FLDMMDLSYS *sys )
{
	int i,max;
	FLDMMDL * fmmdl;
	
	i = 0;
	max = FLDMMDLSYS_OBJMaxGet( sys );
	fmmdl = FLDMMDLSYS_fmmdlWorkGet( sys );
	
	do{
		if( FLDMMDL_StatusBit_Check(fmmdl,FLDMMDL_STABIT_USE) == 0 ){
			return( fmmdl );
		}
		
		fmmdl++;
		i++;
	}while( i < max );
	
	return( NULL );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���@�G�C���A�X��T��
 * @param	fos			FLDMMDLSYS *
 * @param	obj_id		��v����OBJ ID
 * @param	zone_id		��v����ZONE ID
 * @retval	FLDMMDL	��v����FLDMMDL*�@��v����=NULL
 */
//--------------------------------------------------------------
static FLDMMDL * fmmdl_AliesSearch( const FLDMMDLSYS *fos, int obj_id, int zone_id )
{
	int no = 0;
	FLDMMDL * fmmdl;
	
	while( FLDMMDLSYS_FieldOBJSearch(fos,&fmmdl,&no,FLDMMDL_STABIT_USE) == TRUE ){
		if( FLDMMDL_StatusBitCheck_Alies(fmmdl) == TRUE ){
			if( FLDMMDL_OBJIDGet(fmmdl) == obj_id ){
				if( FLDMMDL_ZoneIDGetAlies(fmmdl) == zone_id ){
					return( fmmdl );
				}
			}
		}
	}
	
	return( NULL );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���@TCB����֐��ǉ�
 * @param	sys		FLDMMDLSYS*
 * @param	fmmdl	FLDMMDL*
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_MoveTcbAdd( const FLDMMDLSYS *sys, FLDMMDL * fmmdl )
{
	int pri,code;
	GFL_TCB * tcb;
	
	pri = FLDMMDLSYS_TCBStandardPriorityGet( sys );
	code = FLDMMDL_MoveCodeGet( fmmdl );
	
	if( code == MV_PAIR || code == MV_TR_PAIR ){
		pri += FLDMMDL_TCBPRI_OFFS_AFTER;
	}
	
	tcb = GFL_TCB_AddTask(
		FLDMMDLSYS_GetTCBSYS(sys), fmmdl_TCB_MoveProc, fmmdl, pri );
	GF_ASSERT( tcb != NULL && "fmmdl tcb not add error" );
	
	FLDMMDL_MoveTcbPtrSet( fmmdl, tcb );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���@�w�b�_�[��񔽉f
 * @param	fmmdl		�ݒ肷��FLDMMDL * 
 * @param	head		���f�������Z�߂�FLDMMDL_H *
 * @param	fsys		FIELDSYS_WORK *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_AddHeaderSet(
		FLDMMDL * fmmdl, const FLDMMDL_H *head, void *fsys )
{
	FLDMMDL_OBJIDSet( fmmdl, FLDMMDL_H_IDGet(head) );
	FLDMMDL_OBJCodeSet( fmmdl,fmmdl_OBJCodeWkOBJCodeConv(fsys,FLDMMDL_H_OBJCodeGet(head)) );
	FLDMMDL_MoveCodeSet( fmmdl, FLDMMDL_H_MoveCodeGet(head) );
	FLDMMDL_EventTypeSet( fmmdl, FLDMMDL_H_EventTypeGet(head) );
	FLDMMDL_EventFlagSet( fmmdl, FLDMMDL_H_EventFlagGet(head) );
	FLDMMDL_EventIDSet( fmmdl, FLDMMDL_H_EventIDGet(head) );
	FLDMMDL_DirHeaderSet( fmmdl, FLDMMDL_H_DirGet(head) );
	FLDMMDL_ParamSet( fmmdl, FLDMMDL_H_ParamGet(head,FLDMMDL_PARAM_0), FLDMMDL_PARAM_0 );
	FLDMMDL_ParamSet( fmmdl, FLDMMDL_H_ParamGet(head,FLDMMDL_PARAM_1), FLDMMDL_PARAM_1 );
	FLDMMDL_ParamSet( fmmdl, FLDMMDL_H_ParamGet(head,FLDMMDL_PARAM_2), FLDMMDL_PARAM_2 );
	FLDMMDL_MoveLimitXSet( fmmdl, FLDMMDL_H_MoveLimitXGet(head) );
	FLDMMDL_MoveLimitZSet( fmmdl, FLDMMDL_H_MoveLimitZGet(head) );
	
	fmmdl_AddHeaderPosInit( fmmdl, head );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���@���W�n������
 * @param	fmmdl		FLDMMDL * 
 * @param	head		���f�������Z�߂�FLDMMDL_H *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_AddHeaderPosInit( FLDMMDL * fmmdl, const FLDMMDL_H *head )
{
	int pos;
	VecFx32 vec;
	
	pos = FLDMMDL_H_PosXGet( head );
	vec.x = GRID_SIZE_FX32( pos ) + FLDMMDL_VEC_X_GRID_OFFS_FX32;
	FLDMMDL_InitPosGX_Set( fmmdl, pos );
	FLDMMDL_OldPosGX_Set( fmmdl, pos );
	FLDMMDL_NowPosGX_Set( fmmdl, pos );
	
	pos = FLDMMDL_H_PosYGet( head );							//pos�ݒ��fx32�^�ŗ���B
	vec.y = (fx32)pos;
	pos = SIZE_H_GRID_FX32( pos );
	FLDMMDL_InitPosGY_Set( fmmdl, pos );
	FLDMMDL_OldPosGY_Set( fmmdl, pos );
	FLDMMDL_NowPosGY_Set( fmmdl, pos );
	
	pos = FLDMMDL_H_PosZGet( head );
	vec.z = GRID_SIZE_FX32( pos ) + FLDMMDL_VEC_Z_GRID_OFFS_FX32;
	FLDMMDL_InitPosGZ_Set( fmmdl, pos );
	FLDMMDL_OldPosGZ_Set( fmmdl, pos );
	FLDMMDL_NowPosGZ_Set( fmmdl, pos );
	
	FLDMMDL_VecPosSet( fmmdl, &vec );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���@���[�N������
 * @param	fmmdl		FLDMMDL * 
 * @param	sys			FLDMMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_WorkInit( FLDMMDL * fmmdl, const FLDMMDLSYS *sys )
{
	FLDMMDL_StatusBit_ON( fmmdl,
			FLDMMDL_STABIT_USE |							//�g�p��
			FLDMMDL_STABIT_HEIGHT_GET_ERROR |				//�����擾���K�v�ł���
			FLDMMDL_STABIT_ATTR_GET_ERROR );				//�A�g���r���[�g�擾���K�v�ł���

#if 0	
	if( FLDMMDL_EventIDAliesCheck(fmmdl) == TRUE ){
		FLDMMDL_StatusBitSet_Alies( fmmdl, TRUE );
	}
#endif	

	FLDMMDL_FieldOBJSysSet( fmmdl, sys );
	FLDMMDL_DirDispSetForce( fmmdl, FLDMMDL_DirHeaderGet(fmmdl) );
	FLDMMDL_DirMoveSet( fmmdl, FLDMMDL_DirHeaderGet(fmmdl) );
	FLDMMDL_AcmdFree( fmmdl );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f�� ����֐�������
 * @param	fmmdl		FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_WorkInit_MoveProcInit( FLDMMDL * fmmdl )
{
	const FLDMMDL_MOVE_PROC_LIST *list;
	
	list = fmmdl_MoveProcListGet( FLDMMDL_MoveCodeGet(fmmdl) );
	FLDMMDL_MoveInitProcSet( fmmdl, fmmdl_MoveProcList_InitGet(list) );
	FLDMMDL_MoveProcSet( fmmdl, fmmdl_MoveProcList_MoveGet(list) );
	FLDMMDL_MoveDeleteProcSet( fmmdl, fmmdl_MoveProcList_DeleteGet(list) );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f�� �`��֐�������
 * @param	fmmdl		FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_WorkInit_DrawProcInit( FLDMMDL * fmmdl )
{
	const FLDMMDL_DRAW_PROC_LIST *list;
	u32 code = FLDMMDL_OBJCodeGet( fmmdl );
	
	if( code == NONDRAW ){
		list = &DATA_FieldOBJDraw_Non;
	}else{
		list = &DATA_FieldOBJDraw_Non;
#if 0
		list = fmmdl_DrawProcListGet( code );
#else
		list = &DATA_FieldOBJDraw_Non;
#endif
	}
	
	FLDMMDL_DrawInitProcSet( fmmdl, fmmdl_DrawProcList_InitGet(list) );
	FLDMMDL_DrawProcSet( fmmdl, fmmdl_DrawProcList_DrawGet(list) );
	FLDMMDL_DrawDeleteProcSet( fmmdl, fmmdl_DrawProcList_DeleteGet(list) );
	FLDMMDL_DrawPushProcSet( fmmdl, fmmdl_DrawProcList_PushGet(list) );
	FLDMMDL_DrawPopProcSet( fmmdl, fmmdl_DrawProcList_PopGet(list) );
}

//--------------------------------------------------------------
/**
 * ���[�N����
 * @param	fmmdl	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_WorkClear( FLDMMDL * fmmdl )
{
	GFL_STD_MemClear( fmmdl, FLDMMDL_SIZE );
}

//--------------------------------------------------------------
/**
 * �w�肳�ꂽ�t�B�[���h���샂�f�����G�C���A�X�w�肩�ǂ����`�F�b�N
 * @param	fmmdl		FLDMMDL *
 * @param	h_zone_id	head��ǂݍ��ރ]�[��ID
 * @param	max			head�v�f��
 * @param	head		FLDMMDL_H
 * @retval	int			RET_ALIES_NOT��
 */
//--------------------------------------------------------------
static int fmmdl_HeaderAliesCheck(
		const FLDMMDL * fmmdl, int h_zone_id, int max, const FLDMMDL_H *head )
{
	int obj_id;
	int zone_id;
	
	while( max ){
		obj_id = FLDMMDL_H_IDGet( head );
		
		if( FLDMMDL_OBJIDGet(fmmdl) == obj_id ){					//ID��v
			if( fmmdl_H_AliesCheck(head) == TRUE ){				//�G�C���A�X�w�b�_�[
				zone_id = fmmdl_H_AliesZoneIDGet( head );			//�G�C���A�X�̐��K�]�[��ID
				
				if( FLDMMDL_StatusBitCheck_Alies(fmmdl) == TRUE ){	//�ΏۃG�C���A�X
					if( FLDMMDL_ZoneIDGetAlies(fmmdl) == zone_id ){
						return( RET_ALIES_EXIST );						//Alies�Ƃ��Ċ��ɑ���
					}
				}else if( FLDMMDL_ZoneIDGet(fmmdl) == zone_id ){
					return( RET_ALIES_CHANGE );							//Alies�Ώۂł���
				}
			}else{														//�ʏ�w�b�_�[
				if( FLDMMDL_StatusBitCheck_Alies(fmmdl) == TRUE ){
					if( FLDMMDL_ZoneIDGetAlies(fmmdl) == h_zone_id ){	//�����G�C���A�X�ƈ�v
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
 * �w�肳�ꂽ�]�[��ID��OBJ ID������FLDMMDL *���擾�B
 * @param	fos		FLDMMDLSYS *
 * @param	obj_id	OBJ ID
 * @param	zone_id	�]�[��ID
 * @retval	FLDMMDL * FLDMMDL *
 */
//--------------------------------------------------------------
static FLDMMDL * fmmdl_OBJIDZoneIDSearch(
		const FLDMMDLSYS *fos, int obj_id, int zone_id )
{
	int no = 0;
	FLDMMDL * fmmdl;
	
	while( FLDMMDLSYS_FieldOBJSearch(fos,&fmmdl,&no,FLDMMDL_STABIT_USE) == TRUE ){
		if( FLDMMDL_OBJIDGet(fmmdl) == obj_id && FLDMMDL_ZoneIDGet(fmmdl) == zone_id ){
			return( fmmdl );
		}
	}
	
	return( NULL );
}

//--------------------------------------------------------------
/**
 * �w�肳�ꂽ�]�[��ID�������̈ȊO��S�č폜
 * @param	fos			FLDMMDLSYS *
 * @param	zone_id		�폜���Ȃ��]�[��ID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_DeleteAll_OldZoneID( FLDMMDLSYS *fos, u32 zone_id )
{
	u32 ret;
	int max = FLDMMDLSYS_OBJMaxGet( fos );
	FLDMMDL * fmmdl = FLDMMDLSYS_fmmdlWorkGet( fos );
	
	do{
		if( FLDMMDL_StatusBitCheck_Use(fmmdl) == TRUE ){
			if( FLDMMDL_ZoneIDGet(fmmdl) != zone_id ){
				if( FLDMMDL_StatusBit_CheckEasy(fmmdl,FLDMMDL_STABIT_ZONE_DEL_NOT) == FALSE ){
					FLDMMDL_Delete( fmmdl );
				}
			}
		}
		
		fmmdl++;
		max--;
	}while( max );
}

//--------------------------------------------------------------
/**
 * OBJ ID�Ɉ�v����FLDMMDL *������
 * @param	fos		FLDMMDLSYS *
 * @param	id		��������OBJ ID
 * @retval	FLDMMDL *	NULL=���݂��Ȃ�
 */
//--------------------------------------------------------------
FLDMMDL * FLDMMDLSYS_OBJIDSearch( const FLDMMDLSYS *fos, int id )
{
	int max;
	FLDMMDL * fmmdl;
	
	GF_ASSERT( fos != NULL && "FLDMMDLSYS_OBJIDSearch() NULL�w��ł�" );
	
	max = FLDMMDLSYS_OBJMaxGet( fos );
	fmmdl = fmmdlsys_FieldOBJWorkGet( fos );
	
	do{
		if( FLDMMDL_StatusBit_CheckEasy(fmmdl,FLDMMDL_STABIT_USE) == TRUE ){
			if( FLDMMDL_StatusBitCheck_Alies(fmmdl) == FALSE ){
				if( FLDMMDL_OBJIDGet(fmmdl) == id ){
					return( fmmdl );
				}
			}
		}
		
		fmmdl++;
		max--;
	}while( max > 0 );
	
	return( NULL );
}

//--------------------------------------------------------------
/**
 * ����R�[�h�Ɉ�v����FLDMMDL *������
 * @param	fos		FLDMMDLSYS *
 * @param	mv_code		�������铮��R�[�h
 * @retval	FLDMMDL *	NULL=���݂��Ȃ�
 */
//--------------------------------------------------------------
FLDMMDL * FLDMMDLSYS_MoveCodeSearch( const FLDMMDLSYS *fos, int mv_code )
{
	int max;
	FLDMMDL * fmmdl;
	
	max = FLDMMDLSYS_OBJMaxGet( fos );
	fmmdl = fmmdlsys_FieldOBJWorkGet( fos );
	
	do{
		if( FLDMMDL_StatusBit_CheckEasy(fmmdl,FLDMMDL_STABIT_USE) == TRUE ){
			if( FLDMMDL_MoveCodeGet(fmmdl) == mv_code ){
				return( fmmdl );
			}
		}
		
		fmmdl++;
		max--;
	}while( max > 0 );
	
	return( NULL );
}

FLDMMDL * FLDMMDLSYS_OBJIDMoveCodeSearch( const FLDMMDLSYS *fos, int o, int m )
{
	return( NULL );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f�����[�N����
 * @param	fos			FLDMMDLSYS *
 * @param	fmmdl		�Y������FLDMMDL *�i�[��
 * @param	no			�����J�n���[�Nno�B���̊֐�����߂�ہA�����ʒu+1�̒l�ɂȂ�
 * @param	check_bit	���������X�e�[�^�X�r�b�g�BFLDMMDL_STABIT_USE��
 * @retval	int			TRUE=��v���� FALSE=��v�Ȃ�
 */
//--------------------------------------------------------------
int FLDMMDLSYS_FieldOBJSearch(
	const FLDMMDLSYS *fos, FLDMMDL ** fmmdl, int *no, u32 check_bit )
{
	int max;
	FLDMMDL * check_obj;
	
	max = FLDMMDLSYS_OBJMaxGet( fos );
	
	if( (*no) >= max ){
		return( FALSE );
	}
	
	check_obj = fmmdlsys_FieldOBJWorkGet( fos );
	check_obj = &check_obj[(*no)];
	
	do{
		(*no)++;
		
		if( FLDMMDL_StatusBit_Check(check_obj,check_bit) == check_bit ){
			*fmmdl = check_obj;
			return( TRUE );
		}
		
		check_obj++;
	}while( (*no) < max );
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���`�揉�����ɍs������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_DrawInitStatusSet( FLDMMDL * fmmdl )
{
	FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_MOVE_START );
	fmmdl_DrawEffectFlagInit( fmmdl );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���֘A�G�t�F�N�g�̃t���O�������B
 * �G�t�F�N�g�֘A�̃t���O���������܂Ƃ߂�
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_DrawEffectFlagInit( FLDMMDL * fmmdl )
{
	FLDMMDL_StatusBit_OFF( fmmdl,
		FLDMMDL_STABIT_SHADOW_SET		|
		FLDMMDL_STABIT_SHADOW_VANISH	|
		FLDMMDL_STABIT_EFFSET_SHOAL		|
		FLDMMDL_STABIT_REFLECT_SET );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���`��폜���ɍs������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_DrawDeleteStatusSet( FLDMMDL * fmmdl )
{
}

//--------------------------------------------------------------
/**
 * �w�肳�ꂽOBJ�R�[�h�����[�N�Q�ƌ^�Ȃ�΃��[�N��OBJ�R�[�h�ɕύX�B
 * �Ⴄ�ꍇ�͂��̂܂܂ŕԂ��B
 * @param	fsys	FIELDSYS_WORK *
 * @param	code	OBJ�R�[�h�BHERO��
 * @retval	int		�`�F�b�N���ꂽOBJ�R�[�h
 */
//--------------------------------------------------------------
static int fmmdl_OBJCodeWkOBJCodeConv( void *fsys, int code )
{
	if( code >= WKOBJCODE_ORG && code <= WKOBJCODE_END ){
		#if 0
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
 * ���ݔ������Ă���t�B�[���h���샂�f����OBJ�R�[�h���Q��
 * @param	fmmdl		FLDMMDL * 
 * @param	code		�`�F�b�N����R�[�h�BHERO��
 * @retval	int			TRUE=fmmdl�ȊO�ɂ�code�������Ă���z������
 */
//--------------------------------------------------------------
int FLDMMDL_OBJCodeUseSearch( const FLDMMDL * fmmdl, int code )
{
	const FLDMMDLSYS *fos = FLDMMDL_FieldOBJSysGet( fmmdl );
	int check_code,i = FLDMMDLSYS_OBJMaxGet( fos );
	const FLDMMDL * fieldobj = FLDMMDLSYS_FieldOBJWorkGet( fos );
	
	do{
		if( fieldobj != fmmdl ){
			if( FLDMMDL_StatusBitCheck_Use(fieldobj) == TRUE ){
				check_code = FLDMMDL_OBJCodeGet( fieldobj );
				#if 0		
				if( FLDMMDL_OBJCodeSeedCheck(check_code) == TRUE ){
					check_code = FLDMMDL_OBJCodeSeedGet( fieldobj );
				}
				#endif
				if( check_code != OBJCODEMAX && check_code == code ){
					return( TRUE );
				}
			}
		}
		
		FLDMMDLSYS_FieldOBJWorkInc( &fieldobj );
		i--;
	}while( i );
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * ���������� FLDMMDL_STABIT_HEIGHT_GET_ERROR��OFF�̏ꍇ�͏��������Ȃ�
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_HeightNeedCheckInit( FLDMMDL * fmmdl )
{
	if( FLDMMDL_StatusBit_Check(fmmdl,FLDMMDL_STABIT_HEIGHT_GET_ERROR) ){
		FLDMMDL_VecPosNowHeightGetSet( fmmdl );
	}
}

//--------------------------------------------------------------
/**
 * ���쏉�����ɍs�������Z��
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_MoveWorkInit( FLDMMDL * fmmdl )
{
	fmmdl_WorkInit_MoveProcInit( fmmdl );
	FLDMMDL_MoveInit( fmmdl );
}

//--------------------------------------------------------------
/**
 * �`�揉�����ɍs�������Z��
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_DrawWorkInit( FLDMMDL * fmmdl )
{
	const FLDMMDLSYS *fos = FLDMMDL_FieldOBJSysGet( fmmdl );
	
	if( FLDMMDLSYS_DrawInitCompCheck(fos) == FALSE ){
		return; //�`��V�X�e���������������Ă��Ȃ�
	}
	
	fmmdl_HeightNeedCheckInit( fmmdl );
	FLDMMDL_DrawStatusSet( fmmdl, 0 );
#if 0
	FLDMMDL_BlActAddPracFlagSet( fmmdl, FALSE );
#endif	
	if( FLDMMDL_StatusBitCheck_DrawProcInitComp(fmmdl) == FALSE ){
		fmmdl_WorkInit_DrawProcInit( fmmdl );
		FLDMMDL_DrawInitProcCall( fmmdl );
		FLDMMDL_StatusBitON_DrawProcInitComp( fmmdl );
	}
}

//--------------------------------------------------------------
/**
 * �C�x���gID���G�C���A�X���ǂ����`�F�b�N
 * @param	fmmdl		FLDMMDL * 
 * @retval	int			TRUE=�G�C���A�XID�ł��� FALSE=�Ⴄ
 */
//--------------------------------------------------------------
int FLDMMDL_EventIDAliesCheck( const FLDMMDL * fmmdl )
{
#if 0
	u16 id = (u16)FLDMMDL_EventIDGet( fmmdl );
	
	if( id == SP_SCRID_ALIES ){
		return( TRUE );
	}
#else
	GF_ASSERT( 0 );
#endif
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���@�G�C���A�X���琳�KOBJ�ւ̕ύX
 * @param	fmmdl		FLDMMDL * 
 * @param	head		�Ώۂ�FLDMMDL_H
 * @param	zone_id		���K�̃]�[��ID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_AliesOBJChange( FLDMMDL * fmmdl, const FLDMMDL_H *head, int zone_id )
{
	GF_ASSERT( FLDMMDL_StatusBitCheck_Alies(fmmdl) == TRUE &&
			"fmmdl_AliesOBJChange()alies�ł͂Ȃ�" );
	
	FLDMMDL_StatusBitSet_Alies( fmmdl, FALSE );
	FLDMMDL_ZoneIDSet( fmmdl, zone_id );
	FLDMMDL_EventIDSet( fmmdl, FLDMMDL_H_EventIDGet(head) );
	FLDMMDL_EventFlagSet( fmmdl, FLDMMDL_H_EventFlagGet(head) );
}	

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���@���KOBJ����G�C���A�X�ւ̕ύX
 * @param	fmmdl		FLDMMDL * 
 * @param	head		�Ώۂ�FLDMMDL_H
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_OBJAliesChange( FLDMMDL * fmmdl, int zone_id, const FLDMMDL_H *head )
{
	GF_ASSERT( fmmdl_H_AliesCheck(head) == TRUE && "fmmdl_OBJAliesChange()alies�ł͂Ȃ�" );
	
	FLDMMDL_StatusBitSet_Alies( fmmdl, TRUE );
	FLDMMDL_EventIDSet( fmmdl, FLDMMDL_H_EventIDGet(head) );
	FLDMMDL_EventFlagSet( fmmdl, fmmdl_H_AliesZoneIDGet(head) );
	FLDMMDL_ZoneIDSet( fmmdl, zone_id );
}

//--------------------------------------------------------------
/**
 * FLDMMDL *��TCB�X�^���_�[�h�v���C�I���e�B�𒲐����Ď擾
 * @param	fmmdl	FLDMMDL *
 * @param	offset	�X�^���_�[�h�v���C�I���e�B�ɉ��Z����l
 * @retval	int		���߂��ꂽ�v���C�I���e�B
 */
//--------------------------------------------------------------
int FLDMMDL_TCBPriGet( const FLDMMDL * fmmdl, int offset )
{
	int pri;
	
	pri = FLDMMDL_TCBStandardPriorityGet( fmmdl );
	pri += offset;
	
	return( pri );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���̓���`�F�b�N�B
 * ���S�A����ւ�肪�������Ă��邩�`�F�b�N����B
 * @param	fmmdl	FLDMMDL *
 * @param	obj_id	����Ƃ݂Ȃ�OBJ ID
 * @param	zone_id	����Ƃ݂Ȃ�ZONE ID
 * @retval	int		TRUE=����BFALSE=����ł͂Ȃ�
 */
//--------------------------------------------------------------
int FLDMMDL_CheckSameID( const FLDMMDL * fmmdl, int obj_id, int zone_id )
{
	if( FLDMMDL_StatusBit_CheckEasy(fmmdl,FLDMMDL_STABIT_USE) == FALSE ){
		return( FALSE );
	}
	
	if( FLDMMDL_OBJIDGet(fmmdl) != obj_id ){
		return( FALSE );
	}
	
	if( FLDMMDL_ZoneIDGet(fmmdl) != zone_id ){
		if( FLDMMDL_StatusBitCheck_Alies(fmmdl) == FALSE ){
			return( FALSE );
		}
		
		if( FLDMMDL_ZoneIDGetAlies(fmmdl) != zone_id ){
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
int FLDMMDL_CheckSameIDOBJCodeIn(
		const FLDMMDL * fmmdl, int code, int obj_id, int zone_id )
{
	if( FLDMMDL_StatusBit_CheckEasy(fmmdl,FLDMMDL_STABIT_USE) == FALSE ){
		return( FALSE );
	}
	
	{
		int ret = FLDMMDL_OBJCodeGetSeedIn( fmmdl );
		
		if( ret != code ){
			return( FALSE );
		}
	}
	
	return( FLDMMDL_CheckSameID(fmmdl,obj_id,zone_id) );
}

//==============================================================================
//	����֐�
//==============================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f�� TCB ����֐�
 * @param	tcb		GFL_TCB *
 * @param	work	tcb work
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_TCB_MoveProc( GFL_TCB * tcb, void *work )
{
	FLDMMDL * fmmdl = (FLDMMDL *)work;
	
	FLDMMDL_Move( fmmdl );
	
	if( FLDMMDL_StatusBitCheck_Use(fmmdl) == FALSE ){
		return;
	}
	
//	fmmdl_TCB_DrawProc( fmmdl );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f�� TCB ����֐�����Ă΂��`��֐�
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_TCB_DrawProc( FLDMMDL * fmmdl )
{
	const FLDMMDLSYS *fos = FLDMMDL_FieldOBJSysGet(fmmdl);
	
	if( FLDMMDLSYS_DrawInitCompCheck(fos) == TRUE ){
		FLDMMDL_Draw( fmmdl );
	}
}

//==============================================================================
//	�t�B�[���h���샂�f���V�X�e���@�Q��
//==============================================================================
//--------------------------------------------------------------
/**
 * const FLDMMDLSYS��FLDMMDLSYS��
 * @param	sys		FLDMMDLSYS *
 * @retval	FLDMMDLSYS	FLDMMDLSYS *
 */
//--------------------------------------------------------------
static FLDMMDLSYS * fmmdl_ConstSysLocalGet( const FLDMMDLSYS *sys )
{
	return( (FLDMMDLSYS *)sys );
}

//--------------------------------------------------------------
/**
 * OBJ�o�^�\�ő吔���Z�b�g
 * @param	sys		FLDMMDLSYS *
 * @param	max		�ő吔
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_OBJMaxSet( FLDMMDLSYS *sys, int max )
{
	sys->fmmdl_max = max;
}

//--------------------------------------------------------------
/**
 * OBJ�o�^�\�ő吔���擾
 * @param	sys		FLDMMDLSYS *
 * @retval	int		OBJ�ő吔
 */
//--------------------------------------------------------------
int FLDMMDLSYS_OBJMaxGet( const FLDMMDLSYS *sys )
{
	return( sys->fmmdl_max );
}

//--------------------------------------------------------------
/**
 * ���ݑ��݂��Ă���OBJ�̐���1����
 * @param	sys		FLDMMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdlsys_OBJCountInc( FLDMMDLSYS *sys )
{
	sys->fmmdl_count++;
#ifdef DEBUG_PRINT_FLDMMDL_COUNT
	OS_Printf( "FIELD OBJ ADD, COUNT=0x%x\n", sys->fmmdl_count );
#endif
}

//--------------------------------------------------------------
/**
 * ���ݑ��݂��Ă���OBJ�̐���1���炷
 * @param	sys		FLDMMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdlsys_OBJCountDec( FLDMMDLSYS *sys )
{
	sys->fmmdl_count--;
#ifdef DEBUG_PRINT_FLDMMDL_COUNT
	OS_Printf( "FIELD OBJ DEL, COUNT=0x%x\n", sys->fmmdl_count );
#endif
}

//--------------------------------------------------------------
/**
 * ���ݑ��݂��Ă���OBJ�̐����Z�b�g
 * @param	sys		FLDMMDLSYS *
 * @param	count	�Z�b�g���鐔
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdlsys_OBJCountSet( FLDMMDLSYS *sys, int count )
{
	sys->fmmdl_count = count;
}

//--------------------------------------------------------------
/**
 * ���ݑ��݂��Ă���OBJ�̐����擾
 * @param	fos		FLDMMDLSYS *
 * @retval	int		���ݑ��݂��Ă���OBJ�̐�
 */
//--------------------------------------------------------------
int FLDMMDLSYS_OBJCountGet( const FLDMMDLSYS *fos )
{
	return( fos->fmmdl_count );
}

//--------------------------------------------------------------
/**
 * �V�X�e���@�X�e�[�^�X�r�b�g������
 * @param	sys		FLDMMDLSYS *
 * @param	bit		�������l�BFLDMMDLSYS_STABIT_NON��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_StatusBit_Init( FLDMMDLSYS *sys, u32 bit )
{
	sys->status_bit = bit;
}

//--------------------------------------------------------------
/**
 * �V�X�e���@�X�e�[�^�X�r�b�g ON
 * @param	sys		FLDMMDLSYS *
 * @param	bit		ON�ɂ���r�b�g�BFLDMMDLSYS_STABIT_NON��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_StatusBit_ON( FLDMMDLSYS *sys, u32 bit )
{
	sys->status_bit |= bit;
}

//--------------------------------------------------------------
/**
 * �V�X�e���@�X�e�[�^�X�r�b�g OFF
 * @param	sys		FLDMMDLSYS *
 * @param	bit		OFF�ɂ���r�b�g�BFLDMMDLSYS_STABIT_NON��
 * @retval	int		�ǉ�����Ă���OBJ��
 */
//--------------------------------------------------------------
void FLDMMDLSYS_StatusBit_OFF( FLDMMDLSYS *sys, u32 bit )
{
	sys->status_bit &= ~bit;
}

//--------------------------------------------------------------
/**
 * �V�X�e���@�X�e�[�^�X�r�b�g �`�F�b�N
 * �߂�l�̓X�e�[�^�X�r�b�g�ƃ`�F�b�N����r�b�g��and�����l���Ԃ�B
 * ��F�Ώ� FIDOBJSYS_STR_BIT_DRAW_INIT_COMP�ȊO��OFF�B
 * �`�F�b�N����r�b�g��FLDMMDLSYS_STABIT_DRAW_INIT_COMP|FLDMMDLSYS_STABIT_MOVE_PROC_STOP
 * �߂�l��FLDMMDLSYS_STABIT_DRAW_PROC_INIT���Ԃ�B
 * @param	sys				FLDMMDLSYS *
 * @param	bit				�`�F�b�N����r�b�g�BFLDMMDL_STABIT_USE��
 * @retval	u32				�`�F�b�N��̃r�b�g
 */
//--------------------------------------------------------------
u32 FLDMMDLSYS_StatusBit_Check( const FLDMMDLSYS *sys, u32 bit )
{
	return( (sys->status_bit&bit) );
}

//--------------------------------------------------------------
/**
 * TCB��{�v���C�I���e�B���Z�b�g
 * @param	sys				FLDMMDLSYS *
 * @param	pri				TCB��{�v���C�I���e�B
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_TCBStandardPrioritySet( FLDMMDLSYS *sys, int pri )
{
	sys->tcb_pri = pri;
}

//--------------------------------------------------------------
/**
 * TCB��{�v���C�I���e�B�擾
 * @param	sys			FLDMMDLSYS *
 * @retval	u32			TCB�v���C�I���e�B
 */
//--------------------------------------------------------------
int FLDMMDLSYS_TCBStandardPriorityGet( const FLDMMDLSYS *sys )
{
	return( sys->tcb_pri );
}

//--------------------------------------------------------------
/**
 * FLDMMDL_BLACT_CONT *�擾
 * @param	sys		FLDMMDLSYS *
 * @retval	FLDMMDL_BLACT_CONT	FLDMMDL_BLACT_CONT *
 */
//--------------------------------------------------------------
FLDMMDL_BLACT_CONT * FLDMMDLSYS_BlActContGet( const FLDMMDLSYS *sys )
{
#if 0
	return( &(((FLDMMDLSYS *)sys)->blact_cont) );
#else
	GF_ASSERT( 0 );
	return( NULL );
#endif
}

//--------------------------------------------------------------
/**
 * FLDMMDL_RENDER_CONT_PTR�Z�b�g
 * @param	sys		FLDMMDLSYS *
 * @param	render	FLDMMDL_RENDER_CONT_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_RenderContSet( FLDMMDLSYS *sys, FLDMMDL_RENDER_CONT_PTR render )
{
#if 0
	sys->render_cont = render;
#else
	GF_ASSERT( 0 );
#endif
}

//--------------------------------------------------------------
/**
 * FLDMMDL_RENDER_CONT_PTR �擾
 * @param	sys		FLDMMDLSYS *
 * @retval	FLDMMDL_RENDER_CONT_PTR FLDMMDL_RENDER_CONT_PTR
 */
//--------------------------------------------------------------
FLDMMDL_RENDER_CONT_PTR FLDMMDLSYS_RenderContGet( const FLDMMDLSYS *sys )
{
#if 0
	return( sys->render_cont );
#else
	GF_ASSERT( 0 );
	return( NULL );
#endif
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f�����[�N���Z�b�g
 * @param	sys		FLDMMDLSYS *
 * @param	ptr		FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_FieldOBJWorkSet( FLDMMDLSYS *sys, FLDMMDL * ptr )
{
	sys->fmmdl_work = ptr;
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f�����[�N�擾
 * @param	sys			FLDMMDLSYS *
 * @retval	FLDMMDL	FLDMMDL * 
 */
//--------------------------------------------------------------
const FLDMMDL * FLDMMDLSYS_FieldOBJWorkGet( const FLDMMDLSYS *sys )
{
	return( sys->fmmdl_work );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f�����[�N�擾�@���[�J��
 * @param	sys			FLDMMDLSYS *
 * @retval	FLDMMDL	FLDMMDL * 
 */
//--------------------------------------------------------------
static FLDMMDL * fmmdlsys_FieldOBJWorkGet( const FLDMMDLSYS *sys )
{
	return( sys->fmmdl_work );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f�����[�N�擾�@FIELD OBJ���[�J���p
 * const�����B�t�B�[���h���샂�f���ȊO�͎g�p�֎~
 * @param	sys			FLDMMDLSYS *
 * @retval	FLDMMDL	FLDMMDL * 
 */
//--------------------------------------------------------------
FLDMMDL * FLDMMDLSYS_fmmdlWorkGet( const FLDMMDLSYS *sys )
{
	return( sys->fmmdl_work );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f�����[�N�i�s�B
 * FLDMMDLSYS_FieldOBJWorkGet()�Ŏ擾�����|�C���^���P�i�߂�B
 * @param	FLDMMDL	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_FieldOBJWorkInc( const FLDMMDL ** fmmdl )
{
	(*fmmdl)++;
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f�����[�N�i�s�B��const
 * FLDMMDLSYS_FieldOBJWorkGet()�Ŏ擾�����|�C���^���P�i�߂�B
 * @param	FLDMMDL	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_fmmdlWorkInc( FLDMMDL ** fmmdl )
{
	(*fmmdl)++;
}

//--------------------------------------------------------------
/**
 * FIELDSYS_WORK *�Z�b�g
 * @param	fos		FLDMMDLSYS *
 * @param	fsys	FIELDSYS_WORK *
 * @retval	nothing
 */
//--------------------------------------------------------------
#if 0
void FLDMMDLSYS_FieldSysWorkSet( FLDMMDLSYS *fos, void *fsys )
{
	fos->fieldsys = fsys;
}
#endif

//--------------------------------------------------------------
/**
 * FIELDSYS_WORK *�擾
 * @param	fos		FLDMMDLSYS *
 * @retval	FIELDSYS_WORK	FIELDSYS_WORK *
 */
//--------------------------------------------------------------
#if 0
void * FLDMMDLSYS_FieldSysWorkGet( const FLDMMDLSYS *fos )
{
	return( fos->fieldsys );
}
#endif

//--------------------------------------------------------------
/**
 * �A�[�J�C�u�n���h���Z�b�g
 * @param	fos		FLDMMDLSYS *
 * @param	handle	ARCHANDLE *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_ArcHandleSet( FLDMMDLSYS *fos, ARCHANDLE *handle )
{
	fos->archandle = handle;
}

//--------------------------------------------------------------
/**
 * �A�[�J�C�u�n���h���擾
 * @param	fos		FLDMMDLSYS *
 * @retval	ARCHANDLE*�@ARCHANDLE *
 */
//--------------------------------------------------------------
ARCHANDLE * FLDMMDLSYS_ArcHandleGet( const FLDMMDLSYS *fos )
{
	GF_ASSERT( fos->archandle != NULL && "FLDMMDLSYS_ArcHandleGet()�n���h������" );
	return( ((FLDMMDLSYS *)fos)->archandle );
}

//==============================================================================
//	�t�B�[���h���샂�f���@�Q��
//==============================================================================
//--------------------------------------------------------------
/**
 * �X�e�[�^�X�r�b�g������
 * @param	fmmdl			FLDMMDL * 
 * @param	bit				�������l�BFLDMMDL_STABIT_USE��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_StatusBit_Init( FLDMMDL * fmmdl, u32 bit )
{
	fmmdl->status_bit = bit;
}

//--------------------------------------------------------------
/**
 * �X�e�[�^�X�r�b�g�擾
 * @param	fmmdl			FLDMMDL * 
 * @retval	u32				�X�e�[�^�X�r�b�g
 */
//--------------------------------------------------------------
u32 FLDMMDL_StatusBit_Get( const FLDMMDL * fmmdl )
{
	return( fmmdl->status_bit );
}

//--------------------------------------------------------------
/**
 * �X�e�[�^�X�r�b�g ON
 * @param	fmmdl			FLDMMDL * 
 * @param	bit				ON�ɂ������r�b�g�BFLDMMDL_STABIT_USE��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_StatusBit_ON( FLDMMDL * fmmdl, u32 bit )
{
	fmmdl->status_bit |= bit;
}

//--------------------------------------------------------------
/**
 * �X�e�[�^�X�r�b�g OFF
 * @param	fmmdl			FLDMMDL * 
 * @param	bit				OFF�ɂ������r�b�g�BFLDMMDL_STABIT_USE��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_StatusBit_OFF( FLDMMDL * fmmdl, u32 bit )
{
	fmmdl->status_bit &= ~bit;
}

//--------------------------------------------------------------
/**
 * �X�e�[�^�X�r�b�g �`�F�b�N
 * �߂�l�̓X�e�[�^�X�r�b�g�ƃ`�F�b�N����r�b�g��and�����l���Ԃ�B
 * 
 * ��F�Ώ�OBJ�̃X�e�[�^�X�r�b�g��FIDOBJ_STR_BIT_USE�ȊO��OFF�B
 * �`�F�b�N����r�b�g��FLDMMDL_STABIT_USE|FLDMMDL_STABIT_MOVE_START�B
 * �߂�l��FLDMMDL_STABIT_USE���Ԃ�B
 * 
 * @param	fmmdl			FLDMMDL * 
 * @param	bit				�`�F�b�N����r�b�g�BFLDMMDL_STABIT_USE��
 * @retval	u32				�`�F�b�N��̃r�b�g
 */
//--------------------------------------------------------------
u32 FLDMMDL_StatusBit_Check( const FLDMMDL * fmmdl, u32 bit )
{
	return( fmmdl->status_bit & bit );
}

//--------------------------------------------------------------
/**
 * �X�e�[�^�X�r�b�g �ȈՃ`�F�b�N
 * @param	fmmdl			FLDMMDL * 
 * @param	bit				�`�F�b�N����r�b�g�BFLDMMDL_STABIT_USE��
 * @retval	u32				TRUE=bit�̂ǂꂩ���Y���BFALSE=�Y������
 */
//--------------------------------------------------------------
u32 FLDMMDL_StatusBit_CheckEasy( const FLDMMDL * fmmdl, u32 bit )
{
	if( (fmmdl->status_bit&bit) ){
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * ����r�b�g�@������
 * @param	fmmdl	FLDMMDL *
 * @param	bit		����������r�b�g FLDMMDL_MOVEBIT_NON��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveBit_Init( FLDMMDL * fmmdl, u32 bit )
{
	fmmdl->move_bit = bit;
}

//--------------------------------------------------------------
/**
 * ����r�b�g�@�擾
 * @param	fmmdl	FLDMMDL *
 * @retval	u32		����r�b�g
 */
//--------------------------------------------------------------
u32 FLDMMDL_MoveBit_Get( const FLDMMDL * fmmdl )
{
	return( fmmdl->move_bit );
}

//--------------------------------------------------------------
/**
 * ����r�b�g�@ON
 * @param	fmmdl	FLDMMDL *
 * @param	bit		���Ă�r�b�g FLDMMDL_MOVEBIT_NON��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveBit_ON( FLDMMDL * fmmdl, u32 bit )
{
	fmmdl->move_bit |= bit;
}

//--------------------------------------------------------------
/**
 * ����r�b�g�@OFF
 * @param	fmmdl	FLDMMDL *
 * @param	bit		���낷�r�b�g FLDMMDL_MOVEBIT_NON��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveBit_OFF( FLDMMDL * fmmdl, u32 bit )
{
	fmmdl->move_bit &= ~bit;
}

//--------------------------------------------------------------
/**
 * ����r�b�g�@�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @param	bit		�`�F�b�N����r�b�g FLDMMDL_MOVEBIT_NON��
 * @retval	nothing
 */
//--------------------------------------------------------------
u32 FLDMMDL_MoveBit_Check( const FLDMMDL * fmmdl, u32 bit )
{
	return( (fmmdl->move_bit & bit) );
}

//--------------------------------------------------------------
/**
 * OBJ ID�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	id		obj id
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OBJIDSet( FLDMMDL * fmmdl, u32 obj_id )
{
	fmmdl->obj_id = obj_id;
}

//--------------------------------------------------------------
/**
 * OBJ ID�擾
 * @param	fmmdl	FLDMMDL *
 * @retval	u32		OBJ ID
 */
//--------------------------------------------------------------
u32 FLDMMDL_OBJIDGet( const FLDMMDL * fmmdl )
{
	return( fmmdl->obj_id );
}

//--------------------------------------------------------------
/**
 * ZONE ID�Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @param	zone_id	�]�[��ID
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_ZoneIDSet( FLDMMDL * fmmdl, int zone_id )
{
	fmmdl->zone_id = zone_id;
}

//--------------------------------------------------------------
/**
 * ZONE ID�擾
 * @param	fmmdl	FLDMMDL *
 * @retval	int		ZONE ID
 */
//--------------------------------------------------------------
int FLDMMDL_ZoneIDGet( const FLDMMDL * fmmdl )
{
	return( fmmdl->zone_id );
}

//--------------------------------------------------------------
/**
 * OBJ�R�[�h�Z�b�g
 * @param	fmmdl			FLDMMDL * 
 * @param	code			�Z�b�g����R�[�h
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OBJCodeSet( FLDMMDL * fmmdl, u32 code )
{
	fmmdl->obj_code = code;
}

//--------------------------------------------------------------
/**
 * OBJ�R�[�h�擾
 * @param	fmmdl			FLDMMDL * 
 * @retval	u32				OBJ�R�[�h
 */
//--------------------------------------------------------------
u32 FLDMMDL_OBJCodeGet( const FLDMMDL * fmmdl )
{
	return( fmmdl->obj_code );
}

//--------------------------------------------------------------
/**
 * OBJ�R�[�h�擾�@��`�F�b�N����
 * @param	fmmdl			FLDMMDL * 
 * @retval	u32				OBJ�R�[�h
 */
//--------------------------------------------------------------
u32 FLDMMDL_OBJCodeGetSeedIn( const FLDMMDL * fmmdl )
{
	u32 code = FLDMMDL_OBJCodeGet( fmmdl );
#if 0	
	if( FLDMMDL_OBJCodeSeedCheck(code) == TRUE ){
		code = FLDMMDL_OBJCodeSeedGet( fmmdl );
	}
#endif
	return( code );
}

//--------------------------------------------------------------
/**
 * ����R�[�h�Z�b�g
 * @param	fmmdl			FLDMMDL * 
 * @param	code			����R�[�h
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveCodeSet( FLDMMDL * fmmdl, u32 code )
{
	fmmdl->move_code = code;
}

//--------------------------------------------------------------
/**
 * ����R�[�h�擾
 * @param	fmmdl			FLDMMDL * 
 * @retval	u32				����R�[�h
 */
//--------------------------------------------------------------
u32 FLDMMDL_MoveCodeGet( const FLDMMDL * fmmdl )
{
	return( fmmdl->move_code );
}

//--------------------------------------------------------------
/**
 * �C�x���g�^�C�v�Z�b�g
 * @param	fmmdl		FLDMMDL * 
 * @param	type		Event Type
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_EventTypeSet( FLDMMDL * fmmdl, u32 type )
{
	fmmdl->event_type = type;
}

//--------------------------------------------------------------
/**
 * �C�x���g�^�C�v�擾
 * @param	fmmdl		FLDMMDL * 
 * @retval	u32			Event Type
 */
//--------------------------------------------------------------
u32 FLDMMDL_EventTypeGet( const FLDMMDL * fmmdl )
{
	return( fmmdl->event_type );
}

//--------------------------------------------------------------
/**
 * �C�x���g�t���O�Z�b�g
 * @param	fmmdl		FLDMMDL * 
 * @param	flag		Event Flag
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_EventFlagSet( FLDMMDL * fmmdl, u32 flag )
{
	fmmdl->event_flag = flag;
}

//--------------------------------------------------------------
/**
 * �C�x���g�t���O�擾
 * @param	fmmdl		FLDMMDL * 
 * @retval	u32			Event Flag
 */
//--------------------------------------------------------------
u32 FLDMMDL_EventFlagGet( const FLDMMDL * fmmdl )
{
	return( fmmdl->event_flag );
}

//--------------------------------------------------------------
/**
 * �C�x���gID�Z�b�g
 * @param	fmmdl		FLDMMDL * 
 * @param	id			Event ID
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_EventIDSet( FLDMMDL * fmmdl, u32 id )
{
	fmmdl->event_id = id;
}

//--------------------------------------------------------------
/**
 * �C�x���gID�擾
 * @param	fmmdl		FLDMMDL * 
 * @retval	u32			Event ID
 */
//--------------------------------------------------------------
u32 FLDMMDL_EventIDGet( const FLDMMDL * fmmdl )
{
	return( fmmdl->event_id );
}

//--------------------------------------------------------------
/**
 * �w�b�_�[�w������Z�b�g
 * @param	fmmdl		FLDMMDL * 
 * @param	dir			DIR_UP��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_DirHeaderSet( FLDMMDL * fmmdl, int dir )
{
	fmmdl->dir_head = dir;
}

//--------------------------------------------------------------
/**
 * �w�b�_�[�w������擾
 * @param	fmmdl		FLDMMDL * 
 * @retval	u32			DIR_UP��
 */
//--------------------------------------------------------------
u32 FLDMMDL_DirHeaderGet( const FLDMMDL * fmmdl )
{
	return( fmmdl->dir_head );
}

//--------------------------------------------------------------
/**
 * �\�������Z�b�g�@����
 * @param	fmmdl			FLDMMDL * 
 * @param	dir				DIR_UP��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_DirDispSetForce( FLDMMDL * fmmdl, int dir )
{
	fmmdl->dir_disp_old = fmmdl->dir_disp;
	fmmdl->dir_disp = dir;
}

//--------------------------------------------------------------
/**
 * �\�������Z�b�g�@FLDMMDL_STABIT_PAUSE_DIR�`�F�b�N����
 * @param	fmmdl			FLDMMDL * 
 * @param	dir				DIR_UP��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_DirDispCheckSet( FLDMMDL * fmmdl, int dir )
{
	if( FLDMMDL_StatusBit_Check(fmmdl,FLDMMDL_STABIT_PAUSE_DIR) == 0 ){
		fmmdl->dir_disp_old = fmmdl->dir_disp;
		fmmdl->dir_disp = dir;
	}
}

//--------------------------------------------------------------
/**
 * �\�������擾
 * @param	fmmdl			FLDMMDL * 
 * @param	dir				DIR_UP��
 * @retval	nothing
 */
//--------------------------------------------------------------
int FLDMMDL_DirDispGet( const FLDMMDL * fmmdl )
{
	return( fmmdl->dir_disp );
}

//--------------------------------------------------------------
/**
 * �ߋ��\�������擾
 * @param	fmmdl			FLDMMDL * 
 * @param	dir				DIR_UP��
 * @retval	nothing
 */
//--------------------------------------------------------------
int FLDMMDL_DirDispOldGet( const FLDMMDL * fmmdl )
{
	return( fmmdl->dir_disp_old );
}

//--------------------------------------------------------------
/**
 * �ړ������Z�b�g
 * @param	fmmdl			FLDMMDL * 
 * @param	dir				DIR_UP��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_DirMoveSet( FLDMMDL * fmmdl, int dir )
{
	fmmdl->dir_move_old = fmmdl->dir_move;
	fmmdl->dir_move = dir;
}

//--------------------------------------------------------------
/**
 * �ړ������擾
 * @param	fmmdl			FLDMMDL * 
 * @retval	dir				DIR_UP��
 */
//--------------------------------------------------------------
int FLDMMDL_DirMoveGet( const FLDMMDL * fmmdl )
{
	return( fmmdl->dir_move );
}

//--------------------------------------------------------------
/**
 * �ߋ��ړ������擾
 * @param	fmmdl			FLDMMDL * 
 * @retval	dir				DIR_UP��
 */
//--------------------------------------------------------------
int FLDMMDL_DirMoveOldGet( const FLDMMDL * fmmdl )
{
	return( fmmdl->dir_move_old );
}

//--------------------------------------------------------------
/**
 * �\���A�ړ������Z�b�g�@FLDMMDL_STABIT_PAUSE_DIR�`�F�b�N����
 * @param	fmmdl			FLDMMDL * 
 * @param	dir				DIR_UP��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_DirMoveDispCheckSet( FLDMMDL * fmmdl, int dir )
{
	FLDMMDL_DirDispCheckSet( fmmdl, dir );
	FLDMMDL_DirMoveSet( fmmdl, dir );
}

//--------------------------------------------------------------
/**
 * �w�b�_�[�w��p�����^�Z�b�g
 * @param	fmmdl			FLDMMDL * 
 * @param	param			�p�����^
 * @param	no				�Z�b�g����p�����^�ԍ��@FLDMMDL_PARAM_0��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_ParamSet( FLDMMDL * fmmdl, int param, FLDMMDL_H_PARAM no )
{
	switch( no ){
	case FLDMMDL_PARAM_0: fmmdl->param0 = param; break;
	case FLDMMDL_PARAM_1: fmmdl->param1 = param; break;
	case FLDMMDL_PARAM_2: fmmdl->param2 = param; break;
	default: GF_ASSERT( 0 && "FLDMMDL_ParamSet()�s���Ȉ���" );
	}
}

//--------------------------------------------------------------
/**
 * �w�b�_�[�w��p�����^�擾
 * @param	fmmdl			FLDMMDL *
 * @param	param			FLDMMDL_PARAM_0��
 * @retval	int				�p�����^
 */
//--------------------------------------------------------------
int FLDMMDL_ParamGet( const FLDMMDL * fmmdl, FLDMMDL_H_PARAM param )
{
	switch( param ){
	case FLDMMDL_PARAM_0: return( fmmdl->param0 );
	case FLDMMDL_PARAM_1: return( fmmdl->param1 );
	case FLDMMDL_PARAM_2: return( fmmdl->param2 );
	}
	
	GF_ASSERT( 0 && "FLDMMDL_ParamGet()�s���Ȉ���" );
	return( 0 );
}

//--------------------------------------------------------------
/**
 * �ړ�����X�Z�b�g
 * @param	fmmdl			FLDMMDL * 
 * @param	x				�ړ�����
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveLimitXSet( FLDMMDL * fmmdl, int x )
{
	fmmdl->move_limit_x = x;
}

//--------------------------------------------------------------
/**
 * �ړ�����X�擾
 * @param	fmmdl		FLDMMDL * 
 * @retval	int			�ړ�����X
 */
//--------------------------------------------------------------
int FLDMMDL_MoveLimitXGet( const FLDMMDL * fmmdl )
{
	return( fmmdl->move_limit_x );
}

//--------------------------------------------------------------
/**
 * �ړ�����Z�Z�b�g
 * @param	fmmdl			FLDMMDL * 
 * @param	y				�ړ�����
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveLimitZSet( FLDMMDL * fmmdl, int z )
{
	fmmdl->move_limit_z = z;
}

//--------------------------------------------------------------
/**
 * �ړ�����Z�擾
 * @param	fmmdl		FLDMMDL * 
 * @retval	int			�ړ�����z
 */
//--------------------------------------------------------------
int FLDMMDL_MoveLimitZGet( const FLDMMDL * fmmdl )
{
	return( fmmdl->move_limit_z );
}

//--------------------------------------------------------------
/**
 * �`��X�e�[�^�X�Z�b�g
 * @param	fmmdl		FLDMMDL * 
 * @param	st			DRAW_STA_STOP��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_DrawStatusSet( FLDMMDL * fmmdl, u32 st )
{
	fmmdl->draw_status = st;
}

//--------------------------------------------------------------
/**
 * �`��X�e�[�^�X�擾
 * @param	fmmdl		FLDMMDL * 
 * @retval	u32			DRAW_STA_STOP��
 */
//--------------------------------------------------------------
u32 FLDMMDL_DrawStatusGet( const FLDMMDL * fmmdl )
{
	return( fmmdl->draw_status );
}

//--------------------------------------------------------------
/**
 * ����TCB*�Z�b�g
 * @param	fmmdl			FLDMMDL * 
 * @param	tcb				GFL_TCB *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveTcbPtrSet( FLDMMDL * fmmdl, GFL_TCB * tcb )
{
	fmmdl->tcb = tcb;
}

//--------------------------------------------------------------
/**
 * ����TCB*�擾
 * @param	fmmdl			FLDMMDL * 
 * @retval	GFL_TCB *			GFL_TCB *
 */
//--------------------------------------------------------------
GFL_TCB * FLDMMDL_MoveTcbPtrGet( const FLDMMDL * fmmdl )
{
	return( fmmdl->tcb );
}

//--------------------------------------------------------------
/**
 * ����TCB*���폜
 * @param	fmmdl			FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveTcbDelete( const FLDMMDL * fmmdl )
{
	GFL_TCB_DeleteTask( FLDMMDL_MoveTcbPtrGet(fmmdl) );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS *�Z�b�g
 * @param	fmmdl			FLDMMDL * 
 * @param	sys				FLDMMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_FieldOBJSysSet( FLDMMDL * fmmdl, const FLDMMDLSYS *sys )
{
	fmmdl->fmmdlsys = sys;
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS *�擾
 * @param	fmmdl			FLDMMDL * 
 * @retval	FLDMMDLSYS	FLDMMDLSYS *
 */
//--------------------------------------------------------------
const FLDMMDLSYS *FLDMMDL_FieldOBJSysGet( const FLDMMDL * fmmdl )
{
	return( fmmdl->fmmdlsys );
}

//--------------------------------------------------------------
/**
 * FLDMMDLSYS * �擾 not const
 * @param	fmmdl			FLDMMDL * 
 * @retval	FLDMMDLSYS	FLDMMDLSYS *
 */
//--------------------------------------------------------------
static FLDMMDLSYS *fmmdl_FieldOBJSysGet( const FLDMMDL * fmmdl )
{
	return( fmmdl_ConstSysLocalGet(fmmdl->fmmdlsys) );
}

//--------------------------------------------------------------
/**
 * ����֐��p���[�N�������B
 * size�����[�N�T�C�Y�𒴂��Ă����ꍇ�AASSERT�B
 * ����p���[�N��size���A0�ŏ������B
 * @param	fmmdl	FLDMMDL * 
 * @param	size	�K�v�ȃ��[�N�T�C�Y
 * @retval	void*	���������ꂽ���[�N
 */
//--------------------------------------------------------------
void * FLDMMDL_MoveProcWorkInit( FLDMMDL * fmmdl, int size )
{
	void *work;
	
	GF_ASSERT( size <= FLDMMDL_MOVE_WORK_SIZE );
	work = FLDMMDL_MoveProcWorkGet( fmmdl );
	GFL_STD_MemClear( work, size );
	
	return( work );
}

//--------------------------------------------------------------
/**
 * ����֐��p���[�N�擾�B
 * @param	fmmdl	FLDMMDL * 
 * @retval	void*	���[�N
 */
//--------------------------------------------------------------
void * FLDMMDL_MoveProcWorkGet( FLDMMDL * fmmdl )
{
	return( fmmdl->move_proc_work );
}

//--------------------------------------------------------------
/**
 * ����⏕�֐��p���[�N�������B
 * size�����[�N�T�C�Y�𒴂��Ă����ꍇ�AASSERT�B
 * @param	fmmdl	FLDMMDL * 
 * @param	size	�K�v�ȃ��[�N�T�C�Y
 * @retval	void*	���������ꂽ���[�N
 */
//--------------------------------------------------------------
void * FLDMMDL_MoveSubProcWorkInit( FLDMMDL * fmmdl, int size )
{
	u8 *work;
	
	GF_ASSERT( size <= FLDMMDL_MOVE_SUB_WORK_SIZE &&
			"�t�B�[���h���샂�f�� ����⏕���[�N�T�C�Y���I�[�o�[" );
	
	work = FLDMMDL_MoveSubProcWorkGet( fmmdl );
	GFL_STD_MemClear( work, size );
	
	return( work );
}

//--------------------------------------------------------------
/**
 * ����⏕�֐��p���[�N�擾
 * @param	fmmdl	FLDMMDL * 
 * @retval	void*	���[�N*
 */
//--------------------------------------------------------------
void * FLDMMDL_MoveSubProcWorkGet( FLDMMDL * fmmdl )
{
	return( fmmdl->move_sub_proc_work );
}

//--------------------------------------------------------------
/**
 * ����⏕�֐��p���[�N�������B
 * size�����[�N�T�C�Y�𒴂��Ă����ꍇ�AASSERT�B
 * @param	fmmdl	FLDMMDL * 
 * @param	size	�K�v�ȃ��[�N�T�C�Y
 * @retval	void*	���������ꂽ���[�N
 */
//--------------------------------------------------------------
void * FLDMMDL_MoveCmdWorkInit( FLDMMDL * fmmdl, int size )
{
	u8 *work;
	
	GF_ASSERT( size <= FLDMMDL_MOVE_CMD_WORK_SIZE &&
			"�t�B�[���h���샂�f�� ����R�}���h���[�N�T�C�Y���I�[�o�[" );
	
	work = FLDMMDL_MoveCmdWorkGet( fmmdl );
	GFL_STD_MemClear( work, size );
	
	return( work );
}

//--------------------------------------------------------------
/**
 * ����R�}���h�p���[�N�擾
 * @param	fmmdl	FLDMMDL *
 * @retval	void*	���[�N*
 */
//--------------------------------------------------------------
void * FLDMMDL_MoveCmdWorkGet( FLDMMDL * fmmdl )
{
	return( fmmdl->move_cmd_proc_work );
}

//--------------------------------------------------------------
/**
 * �`��֐��p���[�N�������B
 * size�����[�N�T�C�Y�𒴂��Ă����ꍇ�AASSERT�B
 * @param	fmmdl	FLDMMDL * 
 * @param	size	�K�v�ȃ��[�N�T�C�Y
 * @retval	void*	���������ꂽ���[�N
 */
//--------------------------------------------------------------
void * FLDMMDL_DrawProcWorkInit( FLDMMDL * fmmdl, int size )
{
	u8 *work;
	
	GF_ASSERT( size <= FLDMMDL_DRAW_WORK_SIZE && "fmmdl draw work size over" );
	
	work = FLDMMDL_DrawProcWorkGet( fmmdl );
	GFL_STD_MemClear( work, size );
	
	return( work );
}

//--------------------------------------------------------------
/**
 * �`��֐��p���[�N�擾
 * @param	fmmdl	FLDMMDL * 
 * @retval	void*	���[�N
 */
//--------------------------------------------------------------
void * FLDMMDL_DrawProcWorkGet( FLDMMDL * fmmdl )
{
	return( fmmdl->draw_proc_work );
}

//--------------------------------------------------------------
/**
 * �������֐��Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	init	�������֐�
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveInitProcSet( FLDMMDL * fmmdl, FLDMMDL_MOVE_PROC_INIT init )
{
	fmmdl->move_init_proc = init;
}

//--------------------------------------------------------------
/**
 * �������֐����s
 * @param	fmmdl	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveInitProcCall( FLDMMDL * fmmdl )
{
	fmmdl->move_init_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * ����֐��Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	move	����֐�
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveProcSet( FLDMMDL * fmmdl, FLDMMDL_MOVE_PROC move )
{
	fmmdl->move_proc = move;
}

//--------------------------------------------------------------
/**
 * ����֐����s
 * @param	fmmdl	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveProcCall( FLDMMDL * fmmdl )
{
	fmmdl->move_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * �폜�֐��o�^
 * @param	fmmdl	FLDMMDL * 
 * @param	del		����֐�
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveDeleteProcSet( FLDMMDL * fmmdl, FLDMMDL_MOVE_PROC_DEL del )
{
	fmmdl->move_delete_proc = del;
}

//--------------------------------------------------------------
/**
 * �폜�֐����s
 * @param	fmmdl	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveDeleteProcCall( FLDMMDL * fmmdl )
{
	fmmdl->move_delete_proc( fmmdl );
}

#if 0
//--------------------------------------------------------------
/**
 * ���A�֐��o�^
 * @param	fmmdl	FLDMMDL * 
 * @param	del		����֐�
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveReturnProcSet( FLDMMDL * fmmdl, FLDMMDL_MOVE_PROC_RET ret )
{
	GF_ASSERT( 0 );
//	fmmdl->move_return_proc = ret;
}
#endif

//--------------------------------------------------------------
/**
 * ���A�֐����s
 * @param	fmmdl	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveReturnProcCall( FLDMMDL * fmmdl )
{
#if 0
	fmmdl->move_return_proc( fmmdl );
#else
	const FLDMMDL_MOVE_PROC_LIST *list =
		fmmdl_MoveProcListGet( FLDMMDL_MoveCodeGet(fmmdl) );
	list->return_proc( fmmdl );
#endif
}

//--------------------------------------------------------------
/**
 * �`�揉�����֐��o�^
 * @param	fmmdl	FLDMMDL * 
 * @param	init	�������֐�
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_DrawInitProcSet( FLDMMDL * fmmdl, FLDMMDL_DRAW_PROC_INIT init )
{
	fmmdl->draw_init_proc = init;
}

//--------------------------------------------------------------
/**
 * �`�揉�����֐����s
 * @param	fmmdl	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_DrawInitProcCall( FLDMMDL * fmmdl )
{
	fmmdl->draw_init_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * �`��֐��o�^
 * @param	fmmdl	FLDMMDL * 
 * @param	draw	�`��֐�
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_DrawProcSet( FLDMMDL * fmmdl, FLDMMDL_DRAW_PROC draw )
{
	fmmdl->draw_proc = draw;
}

//--------------------------------------------------------------
/**
 * �`��֐����s
 * @param	fmmdl	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_DrawProcCall( FLDMMDL * fmmdl )
{
#if 0
	fmmdl->draw_proc( fmmdl );
#endif
}

//--------------------------------------------------------------
/**
 * �`��폜�֐��o�^
 * @param	fmmdl	FLDMMDL * 
 * @param	del		�폜�֐�
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_DrawDeleteProcSet( FLDMMDL * fmmdl, FLDMMDL_DRAW_PROC_DEL del )
{
	fmmdl->draw_delete_proc = del;
}

//--------------------------------------------------------------
/**
 * �`��폜�֐����s
 * @param	fmmdl	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_DrawDeleteProcCall( FLDMMDL * fmmdl )
{
	fmmdl->draw_delete_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * �`��ޔ��֐��o�^
 * @param	fmmdl	FLDMMDL * 
 * @param	push	�ޔ��֐�
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_DrawPushProcSet( FLDMMDL * fmmdl, FLDMMDL_DRAW_PROC_PUSH push )
{
	fmmdl->draw_push_proc = push;
}

//--------------------------------------------------------------
/**
 * �`��ޔ��֐����s
 * @param	fmmdl	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_DrawPushProcCall( FLDMMDL * fmmdl )
{
	fmmdl->draw_push_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * �`�敜�A�֐��o�^
 * @param	fmmdl	FLDMMDL * 
 * @param	pop		�ޔ��֐�
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_DrawPopProcSet( FLDMMDL * fmmdl, FLDMMDL_DRAW_PROC_POP pop )
{
	fmmdl->draw_pop_proc = pop;
}

//--------------------------------------------------------------
/**
 * �`�敜�A�֐����s
 * @param	fmmdl	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_DrawPopProcCall( FLDMMDL * fmmdl )
{
	fmmdl->draw_pop_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h�R�[�h�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	code	AC_DIR_U��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_AcmdCodeSet( FLDMMDL * fmmdl, int code )
{
	fmmdl->acmd_code = code;
}

//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h�R�[�h�擾
 * @param	fmmdl	FLDMMDL * 
 * @retval	int		AC_DIR_U��
 */
//--------------------------------------------------------------
int FLDMMDL_AcmdCodeGet( const FLDMMDL * fmmdl )
{
	return( fmmdl->acmd_code );
}

//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h�V�[�P���X�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	no		�V�[�P���X
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_AcmdSeqSet( FLDMMDL * fmmdl, int no )
{
	fmmdl->acmd_seq = no;
}

//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h�V�[�P���X����
 * @param	fmmdl	FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_AcmdSeqInc( FLDMMDL * fmmdl )
{
	fmmdl->acmd_seq++;
}

//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h�V�[�P���X�擾
 * @param	fmmdl	FLDMMDL * 
 * @retval	int		�V�[�P���X
 */
//--------------------------------------------------------------
int FLDMMDL_AcmdSeqGet( const FLDMMDL * fmmdl )
{
	return( fmmdl->acmd_seq );
}

//--------------------------------------------------------------
/**
 * ���݂̃}�b�v�A�g���r���[�g���Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @param	attr	�Z�b�g����A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_NowMapAttrSet( FLDMMDL * fmmdl, u32 attr )
{
	fmmdl->now_attr = attr;
}

//--------------------------------------------------------------
/**
 * ���݂̃}�b�v�A�g���r���[�g���擾
 * @param	fmmdl	FLDMMDL *
 * @retval	u32		�}�b�v�A�g���r���[�g
 */
//--------------------------------------------------------------
u32 FLDMMDL_NowMapAttrGet( const FLDMMDL * fmmdl )
{
	return( fmmdl->now_attr );
}

//--------------------------------------------------------------
/**
 * �ߋ��̃}�b�v�A�g���r���[�g���Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @param	attr	�Z�b�g����A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OldMapAttrSet( FLDMMDL * fmmdl, u32 attr )
{
	fmmdl->old_attr = attr;
}

//--------------------------------------------------------------
/**
 * �ߋ��̃}�b�v�A�g���r���[�g���擾
 * @param	fmmdl	FLDMMDL *
 * @retval	u32		�}�b�v�A�g���r���[�g
 */
//--------------------------------------------------------------
u32 FLDMMDL_OldMapAttrGet( const FLDMMDL * fmmdl )
{
	return( fmmdl->old_attr );
}

//--------------------------------------------------------------
/**
 * FIELDSYS_WORK�擾
 * @param	fmmdl	FLDMMDL *
 * @retval	FIELDSYS_WORK	FIELDSYS_WORK *
 */
//--------------------------------------------------------------
#if 0
void * FLDMMDL_FieldSysWorkGet( const FLDMMDL * fmmdl )
{
	FLDMMDLSYS *fos;
	
	fos = fmmdl_FieldOBJSysGet( fmmdl );
	return( FLDMMDLSYS_FieldSysWorkGet(fos) );
}
#endif

//--------------------------------------------------------------
/**
 * TCB��{�v���C�I���e�B�擾
 * @param	fmmdl	FLDMMDL *
 * @retval	int		TCB��{�v���C�I���e�B
 */
//--------------------------------------------------------------
int FLDMMDL_TCBStandardPriorityGet( const FLDMMDL * fmmdl )
{
	return( FLDMMDLSYS_TCBStandardPriorityGet(FLDMMDL_FieldOBJSysGet(fmmdl)) );
}

//--------------------------------------------------------------
/**
 * �G�C���A�X���̃]�[��ID�擾�B�G�C���A�X���̓C�x���g�t���O���w��]�[��ID
 * @param	fmmdl	FLDMMDL *
 * @retval	int		�]�[��ID
 */
//--------------------------------------------------------------
int FLDMMDL_ZoneIDGetAlies( const FLDMMDL * fmmdl )
{
	GF_ASSERT( FLDMMDL_StatusBitCheck_Alies(fmmdl) == TRUE &&
			"FLDMMDL_ZoneIDGetAlies()alies�ł͖���" );
	return( FLDMMDL_EventFlagGet(fmmdl) );
}

//==============================================================================
//	�t�B�[���h���샂�f���V�X�e���@�X�e�[�^�X�r�b�g�Ȉ�
//==============================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���S�̂̓�������S��~�B
 * ���쏈���A�`�揈�������S��~����B�A�j���[�V�����R�}���h�ɂ��������Ȃ��B
 * @param	fos		FLDMMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_MoveStopAll( FLDMMDLSYS *fos )
{
	FLDMMDLSYS_StatusBit_ON( fos,
		FLDMMDLSYS_STABIT_MOVE_PROC_STOP | FLDMMDLSYS_STABIT_DRAW_PROC_STOP );
	
	#ifdef DEBUG_FLDMMDL_PRINTF
	OS_Printf( "�t�B�[���h���샂�f���S�̂̓�����~���܂���\n" );
	#endif
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���S�̂̓��슮�S��~������
 * @param	fos		FLDMMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_MoveStopAllClear( FLDMMDLSYS *fos )
{
	FLDMMDLSYS_StatusBit_OFF( fos,
		FLDMMDLSYS_STABIT_MOVE_PROC_STOP | FLDMMDLSYS_STABIT_DRAW_PROC_STOP );
	
	#ifdef DEBUG_FLDMMDL_PRINTF
	OS_Printf( "�t�B�[���h���샂�f���S�̂̓����~���������܂���\n" );
	#endif
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���S�̂̓�����ꎞ��~
 * �ŗL�̓��쏈���i�����_���ړ����j���ꎞ��~����B
 * �A�j���[�V�����R�}���h�ɂ͔�������B
 * @param	fos		FLDMMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_MovePauseAll( FLDMMDLSYS *fos )
{
	int max = FLDMMDLSYS_OBJMaxGet( fos );
	FLDMMDL * fmmdl = FLDMMDLSYS_fmmdlWorkGet( fos );
	
	do{
		if( FLDMMDL_StatusBitCheck_Use(fmmdl) ){
			FLDMMDL_MovePause( fmmdl );
		}
		
		fmmdl++;
		max--;
	}while( max );
	
	#ifdef DEBUG_FLDMMDL_PRINTF
	OS_Printf( "�t�B�[���h���샂�f���S�̓�����|�[�Y���܂���\n" );
	#endif
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���S�̓���ꎞ��~������
 * @param	fos		FLDMMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_MovePauseAllClear( FLDMMDLSYS *fos )
{
	int max = FLDMMDLSYS_OBJMaxGet( fos );
	FLDMMDL * fmmdl = FLDMMDLSYS_fmmdlWorkGet( fos );
	
	do{
		if( FLDMMDL_StatusBitCheck_Use(fmmdl) ){
			FLDMMDL_MovePauseClear( fmmdl );
		}
		
		fmmdl++;
		max--;
	}while( max );
	
	#ifdef DEBUG_FLDMMDL_PRINTF
	OS_Printf( "�t�B�[���h���샂�f���S�̓���|�[�Y���������܂���\n" );
	#endif
}

//--------------------------------------------------------------
/**
 * �`�揈��������������Ă��邩�ǂ����`�F�b�N
 * @param	fos				FLDMMDLSYS *
 * @retval	int				TRUE=����������Ă���
 */
//--------------------------------------------------------------
int FLDMMDLSYS_DrawInitCompCheck( const FLDMMDLSYS *fos )
{
	if( FLDMMDLSYS_StatusBit_Check(fos,FLDMMDLSYS_STABIT_DRAW_INIT_COMP) ){
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f������t�B�[���h���샂�f���V�X�e���̃r�b�g�`�F�b�N
 * @param	fmmdl		FLDMMDL *
 * @param	bit			FLDMMDLSYS_STABIT_DRAW_INIT_COMP��
 * @retval	u32			0�ȊO bit�q�b�g
 */
//--------------------------------------------------------------
u32 FLDMMDL_FieldOBJSysStatusBitCheck( const FLDMMDL * fmmdl, u32 bit )
{
	const FLDMMDLSYS *fos = FLDMMDL_FieldOBJSysGet( fmmdl );
	return( FLDMMDLSYS_StatusBit_Check(fos,bit) );
}

//--------------------------------------------------------------
/**
 * �e��t����A�t���Ȃ��̃Z�b�g
 * @param	fos		FLDMMDLSYS *
 * @param	flag	TRUE=�e��t���� FALSE=�e��t���Ȃ�
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDLSYS_ShadowJoinSet( FLDMMDLSYS *fos, int flag )
{
	if( flag == FALSE ){
		FLDMMDLSYS_StatusBit_ON( fos, FLDMMDLSYS_STABIT_SHADOW_JOIN_NOT );
	}else{
		FLDMMDLSYS_StatusBit_OFF( fos, FLDMMDLSYS_STABIT_SHADOW_JOIN_NOT );
	}
}

//--------------------------------------------------------------
/**
 * �e��t����A�t���Ȃ��̃`�F�b�N
 * @param	fos		FLDMMDLSYS *
 * @retval	int		FALSE=�t���Ȃ�
 */
//--------------------------------------------------------------
int FLDMMDLSYS_ShadowJoinCheck( const FLDMMDLSYS *fos )
{
	if( FLDMMDLSYS_StatusBit_Check(fos,FLDMMDLSYS_STABIT_SHADOW_JOIN_NOT) ){
		return( FALSE );
	}
	
	return( TRUE );
}

//==============================================================================
//	�t�B�[���h���샂�f�� �X�e�[�^�X�r�b�g�Ȉ�
//==============================================================================
//--------------------------------------------------------------
/**
 * �g�p�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	int		TRUE=�g�p��
 */
//--------------------------------------------------------------
int FLDMMDL_StatusBitCheck_Use( const FLDMMDL * fmmdl )
{
	return( FLDMMDL_StatusBit_CheckEasy(fmmdl,FLDMMDL_STABIT_USE) );
}

//--------------------------------------------------------------
/**
 * �ړ����쒆�ɂ���
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_StatusBitON_Move( FLDMMDL * fmmdl )
{
	FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_MOVE );
}

//--------------------------------------------------------------
/**
 * �ړ����쒆������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_StatusBitOFF_Move( FLDMMDL * fmmdl )
{
	FLDMMDL_StatusBit_OFF( fmmdl, FLDMMDL_STABIT_MOVE );
}

//--------------------------------------------------------------
/**
 * �ړ����쒆�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	int		TRUE=���쒆
 */
//--------------------------------------------------------------
int FLDMMDL_StatusBitCheck_Move( const FLDMMDL * fmmdl )
{
	return( FLDMMDL_StatusBit_CheckEasy(fmmdl,FLDMMDL_STABIT_MOVE) );
}

//--------------------------------------------------------------
/**
 * �ړ�����J�n�ɂ���
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_StatusBitON_MoveStart( FLDMMDL * fmmdl )
{
	FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_MOVE_START );
}

//--------------------------------------------------------------
/**
 * �ړ�����J�n������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_StatusBitOFF_MoveStart( FLDMMDL * fmmdl )
{
	FLDMMDL_StatusBit_OFF( fmmdl, FLDMMDL_STABIT_MOVE_START );
}

//--------------------------------------------------------------
/**
 * �ړ�����J�n�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	int		TRUE=�ړ�����J�n
 */
//--------------------------------------------------------------
int FLDMMDL_StatusBitCheck_MoveStart( const FLDMMDL * fmmdl )
{
	return( FLDMMDL_StatusBit_CheckEasy(fmmdl,FLDMMDL_STABIT_MOVE_START) );
}

//--------------------------------------------------------------
/**
 * �ړ�����I���ɂ���
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_StatusBitON_MoveEnd( FLDMMDL * fmmdl )
{
	FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_MOVE_END );
}

//--------------------------------------------------------------
/**
 * �ړ�����I��������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_StatusBitOFF_MoveEnd( FLDMMDL * fmmdl )
{
	FLDMMDL_StatusBit_OFF( fmmdl, FLDMMDL_STABIT_MOVE_END );
}

//--------------------------------------------------------------
/**
 * �ړ�����I���`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	int		TRUE=�ړ��I��
 */
//--------------------------------------------------------------
int FLDMMDL_StatusBitCheck_MoveEnd( const FLDMMDL * fmmdl )
{
	return( FLDMMDL_StatusBit_CheckEasy(fmmdl,FLDMMDL_STABIT_MOVE_END) );
}

//--------------------------------------------------------------
/**
 * �`�揉���������ɂ���
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_StatusBitON_DrawProcInitComp( FLDMMDL * fmmdl )
{
	FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_DRAW_PROC_INIT_COMP );
}

//--------------------------------------------------------------
/**
 * �`�揉��������������
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_StatusBitOFF_DrawProcInitComp( FLDMMDL * fmmdl )
{
	FLDMMDL_StatusBit_OFF( fmmdl, FLDMMDL_STABIT_DRAW_PROC_INIT_COMP );
}

//--------------------------------------------------------------
/**
 * �`�揉���������`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	int		TRUE=�`�揉��������
 */
//--------------------------------------------------------------
int FLDMMDL_StatusBitCheck_DrawProcInitComp( const FLDMMDL * fmmdl )
{
	return( FLDMMDL_StatusBit_CheckEasy(fmmdl,FLDMMDL_STABIT_DRAW_PROC_INIT_COMP) );
}

//--------------------------------------------------------------
/**
 * ��\���t���O���`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=��\���@FALSE=�\��
 * @retval	nothing
 */
//--------------------------------------------------------------
int FLDMMDL_StatusBitCheck_Vanish( const FLDMMDL * fmmdl )
{
	return( FLDMMDL_StatusBit_CheckEasy(fmmdl,FLDMMDL_STABIT_VANISH) );
}

//--------------------------------------------------------------
/**
 * ��\���t���O��ݒ�
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=��\���@FALSE=�\��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_StatusBitSet_Vanish( FLDMMDL * fmmdl, int flag )
{
	if( flag == TRUE ){
		FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_VANISH );
	}else{
		FLDMMDL_StatusBit_OFF( fmmdl, FLDMMDL_STABIT_VANISH );
	}
}

//--------------------------------------------------------------
/**
 * OBJ���m�̓����蔻��t���O��ݒ�
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=�q�b�g�A���@FALSE=�q�b�g�i�V
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_StatusBitSet_FellowHit( FLDMMDL * fmmdl, int flag )
{
	if( flag == TRUE ){
		FLDMMDL_StatusBit_OFF( fmmdl, FLDMMDL_STABIT_FELLOW_HIT_NON );
	}else{
		FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_FELLOW_HIT_NON );
	}
}

//--------------------------------------------------------------
/**
 * ���쒆�t���O�̃Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=���쒆�@FALSE=��~��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_StatusBitSet_Move( FLDMMDL * fmmdl, int flag )
{
	if( flag == TRUE ){
		FLDMMDL_StatusBitON_Move( fmmdl );
	}else{
		FLDMMDL_StatusBitOFF_Move( fmmdl );
	}
}

//--------------------------------------------------------------
/**
 * �b�������\�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	int		TRUE=�\ FALSE=�s��
 */
//--------------------------------------------------------------
int FLDMMDL_StatusBitCheck_Talk( FLDMMDL * fmmdl )
{
	if( FLDMMDL_StatusBit_CheckEasy(fmmdl,FLDMMDL_STABIT_TALK_OFF) == TRUE ){
		return( FALSE );
	}
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * �b�������s�t���O���Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=�s�� FALSE=�\
 */
//--------------------------------------------------------------
void FLDMMDL_StatusBitSet_TalkOFF( FLDMMDL * fmmdl, int flag )
{
	if( flag == TRUE ){
		FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_TALK_OFF );
	}else{
		FLDMMDL_StatusBit_OFF( fmmdl, FLDMMDL_STABIT_TALK_OFF );
	}
}

//--------------------------------------------------------------
/**
 * ����|�[�Y
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MovePause( FLDMMDL * fmmdl )
{
	FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_PAUSE_MOVE );
}

//--------------------------------------------------------------
/**
 * ����|�[�Y����
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MovePauseClear( FLDMMDL * fmmdl )
{
	FLDMMDL_StatusBit_OFF( fmmdl, FLDMMDL_STABIT_PAUSE_MOVE );
}

//--------------------------------------------------------------
/**
 * ����|�[�Y�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	int		TRUE=����|�[�Y
 */
//--------------------------------------------------------------
int FLDMMDL_MovePauseCheck( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_StatusBit_CheckEasy(fmmdl,FLDMMDL_STABIT_PAUSE_MOVE) == TRUE ){
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �X�e�[�^�X�r�b�g �`�揈�������������`�F�b�N
 * @param	fmmdl		FLDMMDL *
 * @retval	int			TRUE=�����BFALSE=�܂�
 */
//--------------------------------------------------------------
int FLDMMDL_StatusBit_DrawInitCompCheck( const FLDMMDL * fmmdl )
{
	const FLDMMDLSYS *fos;
	
	fos = FLDMMDL_FieldOBJSysGet( fmmdl );
	
	if( FLDMMDLSYS_DrawInitCompCheck(fos) == FALSE ){
		return( FALSE );
	}
	
	if( FLDMMDL_StatusBit_Check(fmmdl,FLDMMDL_STABIT_DRAW_PROC_INIT_COMP) == 0 ){
		return( FALSE );
	}
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * �����擾���֎~����
 * @param	fmmdl	FLDMMDL *
 * @param	int		TRUE=�����擾OFF FALSE=ON
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_StatusBitSet_HeightGetOFF( FLDMMDL * fmmdl, int flag )
{
	if( flag == TRUE ){
		FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_HEIGHT_GET_OFF );
	}else{
		FLDMMDL_StatusBit_OFF( fmmdl, FLDMMDL_STABIT_HEIGHT_GET_OFF );
	}
}

//--------------------------------------------------------------
/**
 * �����擾���֎~����Ă��邩�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	int		TRUE=�֎~
 */
//--------------------------------------------------------------
int FLDMMDL_HeightOFFCheck( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_StatusBit_Check(fmmdl,FLDMMDL_STABIT_HEIGHT_GET_OFF) ){
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �]�[���؂�ւ����̍폜�֎~
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=�֎~ FALSE=�֎~���Ȃ�
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_NotZoneDeleteSet( FLDMMDL * fmmdl, int flag )
{
	if( flag == TRUE ){
		FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_ZONE_DEL_NOT );
	}else{
		FLDMMDL_StatusBit_OFF( fmmdl, FLDMMDL_STABIT_ZONE_DEL_NOT );
	}
}

//--------------------------------------------------------------
/**
 * �G�C���A�X�t���O���Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=�G�C���A�X FALSE=�Ⴄ
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_StatusBitSet_Alies( FLDMMDL * fmmdl, int flag )
{
	if( flag == TRUE ){
		FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_ALIES );
	}else{
		FLDMMDL_StatusBit_OFF( fmmdl, FLDMMDL_STABIT_ALIES );
	}
}

//--------------------------------------------------------------
/**
 * �G�C���A�X�t���O���`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	flag	TRUE=�G�C���A�X FALSE=�Ⴄ
 */
//--------------------------------------------------------------
int FLDMMDL_StatusBitCheck_Alies( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_StatusBit_Check(fmmdl,FLDMMDL_STABIT_ALIES) ){
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �󐣃G�t�F�N�g�Z�b�g�t���O���Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=�Z�b�g�@FALSE=�N���A
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_StatusBitSet_ShoalEffectSet( FLDMMDL * fmmdl, int flag )
{
	if( flag == TRUE ){
		FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_EFFSET_SHOAL );
	}else{
		FLDMMDL_StatusBit_OFF( fmmdl, FLDMMDL_STABIT_EFFSET_SHOAL );
	}
}

//--------------------------------------------------------------
/**
 * �󐣃G�t�F�N�g�Z�b�g�t���O���`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	flag	TRUE=�Z�b�g�@FALSE=�Ⴄ
 */
//--------------------------------------------------------------
int FLDMMDL_StatusBitCheck_ShoalEffectSet( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_StatusBit_Check(fmmdl,FLDMMDL_STABIT_EFFSET_SHOAL) ){
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�I�t�Z�b�g�ݒ�OFF�Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=�Z�b�g�@FALSE=�N���A
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_StatusBitSet_AttrOffsOFF( FLDMMDL * fmmdl, int flag )
{
	if( flag == TRUE ){
		FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_ATTR_OFFS_OFF );
	}else{
		FLDMMDL_StatusBit_OFF( fmmdl, FLDMMDL_STABIT_ATTR_OFFS_OFF );
	}
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�I�t�Z�b�g�ݒ�OFF�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	flag	TRUE=OFF�@FALSE=�Ⴄ
 */
//--------------------------------------------------------------
int FLDMMDL_StatusBitCheck_AttrOffsOFF( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_StatusBit_Check(fmmdl,FLDMMDL_STABIT_ATTR_OFFS_OFF) ){
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * ���ړ��t���O�Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=�Z�b�g�@FALSE=�N���A
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_StatusBitSet_Bridge( FLDMMDL * fmmdl, int flag )
{
	if( flag == TRUE ){
		FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_BRIDGE );
	}else{
		FLDMMDL_StatusBit_OFF( fmmdl, FLDMMDL_STABIT_BRIDGE );
	}
}

//--------------------------------------------------------------
/**
 * ���ړ��t���O�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	flag	TRUE=���@FALSE=�Ⴄ
 */
//--------------------------------------------------------------
int FLDMMDL_StatusBitCheck_Bridge( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_StatusBit_Check(fmmdl,FLDMMDL_STABIT_BRIDGE) ){
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �f�肱�݃t���O�Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=�Z�b�g�@FALSE=�N���A
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_StatusBitSet_Reflect( FLDMMDL * fmmdl, int flag )
{
	if( flag == TRUE ){
		FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_REFLECT_SET );
	}else{
		FLDMMDL_StatusBit_OFF( fmmdl, FLDMMDL_STABIT_REFLECT_SET );
	}
}

//--------------------------------------------------------------
/**
 * �f�肱�݃t���O�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	flag	TRUE=�Z�b�g�@FALSE=����
 */
//--------------------------------------------------------------
int FLDMMDL_StatusBitCheck_Reflect( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_StatusBit_Check(fmmdl,FLDMMDL_STABIT_REFLECT_SET) ){
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	flag	TRUE=�R�}���h�A���@FALSE=����
 */
//--------------------------------------------------------------
int FLDMMDL_StatusBitCheck_Acmd( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_StatusBit_Check(fmmdl,FLDMMDL_STABIT_ACMD) ){
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �g�����������t���O���Z�b�g
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=�Z�b�g�@FALSE=�N���A
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_StatusBitSet_HeightExpand( FLDMMDL * fmmdl, int flag )
{
	if( flag == TRUE ){
		FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_HEIGHT_EXPAND );
	}else{
		FLDMMDL_StatusBit_OFF( fmmdl, FLDMMDL_STABIT_HEIGHT_EXPAND );
	}
}

//--------------------------------------------------------------
/**
 * �g�����������t���O�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	flag	TRUE=�g�������ɔ�������@FALSE=����
 */
//--------------------------------------------------------------
int FLDMMDL_StatusBitCheck_HeightExpand( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_StatusBit_Check(fmmdl,FLDMMDL_STABIT_HEIGHT_EXPAND) ){
		return( TRUE );
	}
	
	return( FALSE );
}

//==============================================================================
//	����r�b�g�Ȉ�
//==============================================================================
//--------------------------------------------------------------
/**
 * �A�g���r���[�g�擾���~
 * @param	fmmdl	FLDMMDL *
 * @param	flag	TRUE=��~
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveBitSet_AttrGetOFF( FLDMMDL * fmmdl, int flag )
{
	if( flag == TRUE ){
		FLDMMDL_MoveBit_ON( fmmdl, FLDMMDL_MOVEBIT_ATTR_GET_OFF );
	}else{
		FLDMMDL_MoveBit_OFF( fmmdl, FLDMMDL_MOVEBIT_ATTR_GET_OFF );
	}
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�擾���~�@�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	int	TRUE=��~
 */
//--------------------------------------------------------------
int FLDMMDL_MoveBitCheck_AttrGetOFF( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_MoveBit_Check(fmmdl,FLDMMDL_MOVEBIT_ATTR_GET_OFF) ){
		return( TRUE );
	}
	
	return( FALSE );
}

//==============================================================================
//	���W�n�Q��
//==============================================================================
//--------------------------------------------------------------
/**
 * �������W X���W�擾
 * @param	fmmdl	FLDMMDL * 
 * @retval	int		X���W
 */
//--------------------------------------------------------------
int FLDMMDL_InitPosGX_Get( const FLDMMDL * fmmdl )
{
	return( fmmdl->gx_init );
}

//--------------------------------------------------------------
/**
 * �������W X���W�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	x		�Z�b�g������W
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_InitPosGX_Set( FLDMMDL * fmmdl, int x )
{
	fmmdl->gx_init = x;
}

//--------------------------------------------------------------
/**
 * �������W Y���W�擾
 * @param	fmmdl	FLDMMDL * 
 * @retval	int		Y
 */
//--------------------------------------------------------------
int FLDMMDL_InitPosGY_Get( const FLDMMDL * fmmdl )
{
	return( fmmdl->gy_init );
}

//--------------------------------------------------------------
/**
 * �������W Y���W�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	y		�Z�b�g������W
 * @retval	int		y���W
 */
//--------------------------------------------------------------
void FLDMMDL_InitPosGY_Set( FLDMMDL * fmmdl, int y )
{
	fmmdl->gy_init = y;
}

//--------------------------------------------------------------
/**
 * �������W z���W�擾
 * @param	fmmdl	FLDMMDL * 
 * @retval	int		z���W
 */
//--------------------------------------------------------------
int FLDMMDL_InitPosGZ_Get( const FLDMMDL * fmmdl )
{
	return( fmmdl->gz_init );
}

//--------------------------------------------------------------
/**
 * �������W z���W�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	z		�Z�b�g������W
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_InitPosGZ_Set( FLDMMDL * fmmdl, int z )
{
	fmmdl->gz_init = z;
}

//--------------------------------------------------------------
/**
 * �ߋ����W�@X���W�擾
 * @param	fmmdl	FLDMMDL * 
 * @retval	int		X���W
 */
//--------------------------------------------------------------
int FLDMMDL_OldPosGX_Get( const FLDMMDL * fmmdl )
{
	return( fmmdl->gx_old );
}

//--------------------------------------------------------------
/**
 * �ߋ����W X���W�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	x		�Z�b�g������W
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OldPosGX_Set( FLDMMDL * fmmdl, int x )
{
	fmmdl->gx_old = x;
}

//--------------------------------------------------------------
/**
 * �ߋ����W Y���W�擾
 * @param	fmmdl	FLDMMDL * 
 * @retval	int		Y
 */
//--------------------------------------------------------------
int FLDMMDL_OldPosGY_Get( const FLDMMDL * fmmdl )
{
	return( fmmdl->gy_old );
}

//--------------------------------------------------------------
/**
 * �ߋ����W Y���W�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	y		�Z�b�g������W
 * @retval	int		y���W
 */
//--------------------------------------------------------------
void FLDMMDL_OldPosGY_Set( FLDMMDL * fmmdl, int y )
{
	fmmdl->gy_old = y;
}

//--------------------------------------------------------------
/**
 * �ߋ����W z���W�擾
 * @param	fmmdl	FLDMMDL * 
 * @retval	int		z���W
 */
//--------------------------------------------------------------
int FLDMMDL_OldPosGZ_Get( const FLDMMDL * fmmdl )
{
	return( fmmdl->gz_old );
}

//--------------------------------------------------------------
/**
 * �ߋ����W z���W�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	z		�Z�b�g������W
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OldPosGZ_Set( FLDMMDL * fmmdl, int z )
{
	fmmdl->gz_old = z;
}

//--------------------------------------------------------------
/**
 * ���ݍ��W X���W�擾
 * @param	fmmdl	FLDMMDL * 
 * @retval	int		X���W
 */
//--------------------------------------------------------------
int FLDMMDL_NowPosGX_Get( const FLDMMDL * fmmdl )
{
	return( fmmdl->gx_now );
}

//--------------------------------------------------------------
/**
 * ���ݍ��W X���W�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	x		�Z�b�g������W
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_NowPosGX_Set( FLDMMDL * fmmdl, int x )
{
	fmmdl->gx_now = x;
}

//--------------------------------------------------------------
/**
 * ���ݍ��W X���W����
 * @param	fmmdl	FLDMMDL * 
 * @param	x		�����l
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_NowPosGX_Add( FLDMMDL * fmmdl, int x )
{
	fmmdl->gx_now += x;
}

//--------------------------------------------------------------
/**
 * ���ݍ��W Y���W�擾
 * @param	fmmdl	FLDMMDL * 
 * @retval	int		Y
 */
//--------------------------------------------------------------
int FLDMMDL_NowPosGY_Get( const FLDMMDL * fmmdl )
{
	return( fmmdl->gy_now );
}

//--------------------------------------------------------------
/**
 * ���ݍ��W Y���W�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	y		�Z�b�g������W
 * @retval	int		y���W
 */
//--------------------------------------------------------------
void FLDMMDL_NowPosGY_Set( FLDMMDL * fmmdl, int y )
{
	fmmdl->gy_now = y;
}

//--------------------------------------------------------------
/**
 * ���ݍ��W Y���W����
 * @param	fmmdl	FLDMMDL * 
 * @param	y		�����l
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_NowPosGY_Add( FLDMMDL * fmmdl, int y )
{
	fmmdl->gy_now += y;
}

//--------------------------------------------------------------
/**
 * �ߋ����W z���W�擾
 * @param	fmmdl	FLDMMDL * 
 * @retval	int		z���W
 */
//--------------------------------------------------------------
int FLDMMDL_NowPosGZ_Get( const FLDMMDL * fmmdl )
{
	return( fmmdl->gz_now );
}

//--------------------------------------------------------------
/**
 * ���ݍ��W z���W�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	z		�Z�b�g������W
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_NowPosGZ_Set( FLDMMDL * fmmdl, int z )
{
	fmmdl->gz_now = z;
}

//--------------------------------------------------------------
/**
 * ���ݍ��W z���W����
 * @param	fmmdl	FLDMMDL * 
 * @param	z		�����l
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_NowPosGZ_Add( FLDMMDL * fmmdl, int z )
{
	fmmdl->gz_now += z;
}

//--------------------------------------------------------------
/**
 * �����W�擾
 * @param	fmmdl	FLDMMDL * 
 * @param	vec		���W�i�[��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_VecPosGet( const FLDMMDL * fmmdl, VecFx32 *vec )
{
	*vec = fmmdl->vec_pos_now;
}

//--------------------------------------------------------------
/**
 * �����W�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	vec		�Z�b�g���W
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_VecPosSet( FLDMMDL * fmmdl, const VecFx32 *vec )
{
	fmmdl->vec_pos_now = *vec;
}

//--------------------------------------------------------------
/**
 * �����W�|�C���^�擾
 * @param	fmmdl	FLDMMDL * 
 * @retval	VecFx32	�����W�|�C���^
 */
//--------------------------------------------------------------
const VecFx32 * FLDMMDL_VecPosPtrGet( const FLDMMDL * fmmdl )
{
	return( &fmmdl->vec_pos_now );
}

//--------------------------------------------------------------
/**
 * �����WY�l�擾
 * @param	fmmdl	FLDMMDL * 
 * @retval	fx32	����
 */
//--------------------------------------------------------------
fx32 FLDMMDL_VecPosYGet( const FLDMMDL * fmmdl )
{
	return( fmmdl->vec_pos_now.y );
}

//--------------------------------------------------------------
/**
 * �\�����W�I�t�Z�b�g�擾
 * @param	fmmdl	FLDMMDL * 
 * @param	vec		�Z�b�g���W
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_VecDrawOffsGet( const FLDMMDL * fmmdl, VecFx32 *vec )
{
	*vec = fmmdl->vec_draw_offs;
}

//--------------------------------------------------------------
/**
 * �\�����W�I�t�Z�b�g�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	vec		�Z�b�g���W
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_VecDrawOffsSet( FLDMMDL * fmmdl, const VecFx32 *vec )
{
	fmmdl->vec_draw_offs = *vec;
}

//--------------------------------------------------------------
/**
 * �\�����W�I�t�Z�b�g�|�C���^�擾
 * @param	fmmdl		FLDMMDL * 
 * @retval	VecFx32		�I�t�Z�b�g�|�C���^
 */
//--------------------------------------------------------------
VecFx32 * FLDMMDL_VecDrawOffsPtrGet( FLDMMDL * fmmdl )
{
	return( &fmmdl->vec_draw_offs );
}

//--------------------------------------------------------------
/**
 * �O���w��\�����W�I�t�Z�b�g�擾
 * @param	fmmdl	FLDMMDL * 
 * @param	vec		�Z�b�g���W
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_VecDrawOffsOutSideGet( const FLDMMDL * fmmdl, VecFx32 *vec )
{
	*vec = fmmdl->vec_draw_offs_outside;
}

//--------------------------------------------------------------
/**
 * �O���w��\�����W�I�t�Z�b�g�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	vec		�Z�b�g���W
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_VecDrawOffsOutSideSet( FLDMMDL * fmmdl, const VecFx32 *vec )
{
	fmmdl->vec_draw_offs_outside = *vec;
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�ω����W�I�t�Z�b�g�擾
 * @param	fmmdl	FLDMMDL * 
 * @param	vec		�Z�b�g���W
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_VecAttrOffsGet( const FLDMMDL * fmmdl, VecFx32 *vec )
{
	*vec = fmmdl->vec_attr_offs;
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�ω����W�I�t�Z�b�g�Z�b�g
 * @param	fmmdl	FLDMMDL * 
 * @param	vec		�Z�b�g���W
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_VecAttrOffsSet( FLDMMDL * fmmdl, const VecFx32 *vec )
{
	fmmdl->vec_attr_offs = *vec;
}

//--------------------------------------------------------------
/**
 * ����(�O���b�h�P��)���擾
 * @param	fmmdl	FLDMMDL *
 * @retval	int		�����BH_GRID�P��
 */
//--------------------------------------------------------------
int FLDMMDL_HeightGridGet( const FLDMMDL * fmmdl )
{
	fx32 y = FLDMMDL_VecPosYGet( fmmdl );
	int gy = SIZE_H_GRID_FX32( y );
	return( gy );
}

//==============================================================================
//	FLDMMDL_H	�Q��
//==============================================================================
//--------------------------------------------------------------
/**
 * ID �Z�b�g
 * @param	head	FLDMMDL_H
 * @param	id		OBJ ID
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_H_IDSet( FLDMMDL_H *head, int id )
{
	head->id = id;
}

//--------------------------------------------------------------
/**
 * ID �擾
 * @param	head	FLDMMDL_H
 * @retval	u32		ID
 */
//--------------------------------------------------------------
int FLDMMDL_H_IDGet( const FLDMMDL_H *head )
{
	return( head->id );
}

//--------------------------------------------------------------
/**
 * OBJ�R�[�h�Z�b�g
 * @param	head	FLDMMDL_H
 * @param	code	HERO��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_H_OBJCodeSet( FLDMMDL_H *head, int code )
{
	head->obj_code = code;
}

//--------------------------------------------------------------
/**
 * OBJ�R�[�h�擾
 * @param	head	FLDMMDL_H
 * @retval	int		OBJ�R�[�h
 */
//--------------------------------------------------------------
int FLDMMDL_H_OBJCodeGet( const FLDMMDL_H *head )
{
	return( head->obj_code );
}

//--------------------------------------------------------------
/**
 * ����R�[�h�Z�b�g
 * @param	head	FLDMMDL_H
 * @param	code	MV_DIR_RND��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_H_MoveCodeSet( FLDMMDL_H *head, int code )
{
	head->move_code = code;
}

//--------------------------------------------------------------
/**
 * ����R�[�h�擾
 * @param	head	FLDMMDL_H
 * @retval	int		����R�[�h
 */
//--------------------------------------------------------------
int FLDMMDL_H_MoveCodeGet( const FLDMMDL_H *head )
{
	return( head->move_code );
}

//--------------------------------------------------------------
/**
 * �C�x���g�^�C�v�Z�b�g
 * @param	head	FLDMMDL_H
 * @param	type	�C�x���g�^�C�v
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_H_EventTypeSet( FLDMMDL_H *head, int type )
{
	head->event_type = type;
}

//--------------------------------------------------------------
/**
 * �C�x���g�^�C�v�擾
 * @param	head	FLDMMDL_H
 * @retval	int		�C�x���g�^�C�v
 */
//--------------------------------------------------------------
int FLDMMDL_H_EventTypeGet( const FLDMMDL_H *head )
{
	return( head->event_type );
}

//--------------------------------------------------------------
/**
 * �C�x���g�t���O�Z�b�g
 * @param	head	FLDMMDL_H
 * @param	flag	�C�x���g�t���O
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_H_EventFlagSet( FLDMMDL_H *head, int flag )
{
	head->event_flag = flag;
}

//--------------------------------------------------------------
/**
 * �C�x���g�t���O�擾
 * @param	head	FLDMMDL_H
 * @retval	int		�C�x���g�t���O
 */
//--------------------------------------------------------------
int FLDMMDL_H_EventFlagGet( const FLDMMDL_H *head )
{
	return( head->event_flag );
}

//--------------------------------------------------------------
/**
 * �C�x���gID�Z�b�g
 * @param	head	FLDMMDL_H
 * @param	id		�C�x���gID
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_H_EventIDSet( FLDMMDL_H *head, int id )
{
	head->event_id = id;
}

//--------------------------------------------------------------
/**
 * �C�x���gID�擾
 * @param	head	FLDMMDL_H
 * @retval	int		�C�x���g�t���O
 */
//--------------------------------------------------------------
int FLDMMDL_H_EventIDGet( const FLDMMDL_H *head )
{
	return( head->event_id );
}

//--------------------------------------------------------------
/**
 * �����Z�b�g
 * @param	head	FLDMMDL_H
 * @param	dir		DIR_UP��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_H_DirSet( FLDMMDL_H *head, int dir )
{
	head->dir = dir;
}

//--------------------------------------------------------------
/**
 * �����擾
 * @param	head	FLDMMDL_H
 * @retval	int		����
 */
//--------------------------------------------------------------
int FLDMMDL_H_DirGet( const FLDMMDL_H *head )
{
	return( head->dir );
}

//--------------------------------------------------------------
/**
 * �w��p�����^�Z�b�g
 * @param	head	FLDMMDL_H
 * @param	param	�w��p�����^
 * @param	no		FLDMMDL_PARAM_0��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_H_ParamSet( FLDMMDL_H *head, int param, FLDMMDL_H_PARAM no )
{
	switch( no ){
	case FLDMMDL_PARAM_0: head->param0 = param; break;
	case FLDMMDL_PARAM_1: head->param1 = param; break;
	case FLDMMDL_PARAM_2: head->param2 = param; break;
	default: GF_ASSERT( 0 && "FLDMMDL_H_ParamSet()�s���Ȉ���" );
	}
}

//--------------------------------------------------------------
/**
 * �w��p�����^�擾
 * @param	head	FLDMMDL_H
 * @param	no		�擾����p�����^�BFLDMMDL_PARAM_0��
 * @retval	int		�p�����^
 */
//--------------------------------------------------------------
int FLDMMDL_H_ParamGet( const FLDMMDL_H *head, FLDMMDL_H_PARAM no )
{
	switch( no ){
	case FLDMMDL_PARAM_0: return( head->param0 );
	case FLDMMDL_PARAM_1: return( head->param1 );
	case FLDMMDL_PARAM_2: return( head->param2 );
	}
	
	GF_ASSERT( 0 && "FLDMMDL_H_ParamGet()�s���Ȉ���" );
	return( 0 );
}

//--------------------------------------------------------------
/**
 * �ړ�����X�Z�b�g
 * @param	head	FLDMMDL_H
 * @param	x		�ړ�����X
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_H_MoveLimitXSet( FLDMMDL_H *head, int x )
{
	head->move_limit_x = x;
}

//--------------------------------------------------------------
/**
 * �ړ�����X�擾
 * @param	head	FLDMMDL_H
 * @retval	int		�ړ�����X
 */
//--------------------------------------------------------------
int FLDMMDL_H_MoveLimitXGet( const FLDMMDL_H *head )
{
	return( head->move_limit_x );
}

//--------------------------------------------------------------
/**
 * �ړ�����Z�Z�b�g
 * @param	head	FLDMMDL_H
 * @param	z		�ړ�����Z
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_H_MoveLimitZSet( FLDMMDL_H *head, int z )
{
	head->move_limit_z = z;
}

//--------------------------------------------------------------
/**
 * �ړ�����Z�擾
 * @param	head	FLDMMDL_H
 * @retval	int		�ړ�����Y
 */
//--------------------------------------------------------------
int FLDMMDL_H_MoveLimitZGet( const FLDMMDL_H *head )
{
	return( head->move_limit_z );
}

//--------------------------------------------------------------
/**
 * X���W�Z�b�g
 * @param	head	FLDMMDL_H
 * @param	x		X���W
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_H_PosXSet( FLDMMDL_H *head, int x )
{
	head->gx = x;
}

//--------------------------------------------------------------
/**
 * X���W�擾
 * @param	head	FLDMMDL_H
 * @retval	int		X���W
 */
//--------------------------------------------------------------
int FLDMMDL_H_PosXGet( const FLDMMDL_H *head )
{
	return( head->gx );
}

//--------------------------------------------------------------
/**
 * Y���W�Z�b�g
 * @param	head	FLDMMDL_H
 * @param	y		y���W
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_H_PosYSet( FLDMMDL_H *head, int y )
{
	head->gy = y;
}

//--------------------------------------------------------------
/**
 * Y���W�擾
 * @param	head	FLDMMDL_H
 * @retval	int		Y���W
 */
//--------------------------------------------------------------
int FLDMMDL_H_PosYGet( const FLDMMDL_H *head )
{
	return( head->gy );
}

//--------------------------------------------------------------
/**
 * Z���W�Z�b�g
 * @param	head	FLDMMDL_H
 * @param	z		Z���W
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_H_PosZSet( FLDMMDL_H *head, int z )
{
	head->gz = z;
}

//--------------------------------------------------------------
/**
 * Z���W�擾
 * @param	head	FLDMMDL_H
 * @retval	int		Y���W
 */
//--------------------------------------------------------------
int FLDMMDL_H_PosZGet( const FLDMMDL_H *head )
{
	return( head->gz );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���w�b�_�[�z�񂩂�w��ID���������w�b�_�[������
 * @param	id		�w�b�_�[ID
 * @param	max		head�v�f��
 * @param	head	�t�B�[���h���샂�f���w�b�_�[�e�[�u��
 * @retval	FLDMMDL_H id������head���|�C���^�BNULL=��v����
 */
//--------------------------------------------------------------
static const FLDMMDL_H * fmmdl_H_OBJIDSearch( int id, int max, const FLDMMDL_H *head )
{
	int i = 0;
	
	do{
		if( fmmdl_H_AliesCheck(&head[i]) == FALSE ){
			if( FLDMMDL_H_IDGet(&head[i]) == id ){
				return( &head[i] );
			}
		}
		
		i++;
	}while( i < max );
	
	return( NULL );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���w�b�_�[�@�G�C���A�X�`�F�b�N
 * @param	head	FLDMMDL_H
 * @retval	int		TRUE=�G�C���A�X�@FALSE=�Ⴄ
 */
//--------------------------------------------------------------
static int fmmdl_H_AliesCheck( const FLDMMDL_H *head )
{
	u16 id = (u16)FLDMMDL_H_EventIDGet( head );
#if 0
	if( id == SP_SCRID_ALIES ){ return( TRUE ); }
#endif
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���w�b�_�[�@�G�C���A�X���̃]�[��ID�擾�B
 * �G�C���A�X���̓C�x���g�t���O���]�[��ID�ɂȂ�
 * @param	head	FLDMMDL_H
 * @retval	int		�]�[��ID
 */
//--------------------------------------------------------------
static int fmmdl_H_AliesZoneIDGet( const FLDMMDL_H *head )
{
	GF_ASSERT( fmmdl_H_AliesCheck(head) == TRUE && "fmmdl_H_AliesZoneIDGet()alies�ł͂Ȃ�" );
	return( FLDMMDL_H_EventFlagGet(head) );
}

//==============================================================================
//	FLDMMDL_MOVE_PROC �Q��
//==============================================================================
//--------------------------------------------------------------
/**
 * ����e�[�u������w��̃��X�g���o��
 * @param	nothing
 * @retval	FLDMMDL_MOVE_PROC_LIST	FLDMMDL_MOVE_PROC_LIST�e�[�u��
 */
//--------------------------------------------------------------
static const FLDMMDL_MOVE_PROC_LIST * fmmdl_MoveProcListGet( u32 code )
{
	GF_ASSERT( code < MV_CODE_MAX );
	return( DATA_FieldOBJMoveProcListTbl[code] );
}

//--------------------------------------------------------------
/**
 * ����R�[�h�擾
 * @param	list 		FLDMMDL_MOVE_PROC_LIST
 * @retval	u32			MV_PLAYER��
 */
//--------------------------------------------------------------
static u32 fmmdl_MoveProcList_MoveCodeGet( const FLDMMDL_MOVE_PROC_LIST *list )
{
	return( list->move_code );
}

//--------------------------------------------------------------
/**
 * �������֐��擾
 * @param	list FLDMMDL_MOVE_PROC_LIST
 * @retval	FLDMMDL_MOVE_PROC_INIT list�̏������֐�
 */
//--------------------------------------------------------------
static FLDMMDL_MOVE_PROC_INIT fmmdl_MoveProcList_InitGet(
		const FLDMMDL_MOVE_PROC_LIST *list )
{
	return( list->init_proc );
}

//--------------------------------------------------------------
/**
 * ����֐��擾
 * @param	list FLDMMDL_MOVE_PROC_LIST
 * @retval	FLDMMDL_MOVE_PROC list�̓���֐�
 */
//--------------------------------------------------------------
static FLDMMDL_MOVE_PROC fmmdl_MoveProcList_MoveGet( const FLDMMDL_MOVE_PROC_LIST *list )
{
	return( list->move_proc );
}

//--------------------------------------------------------------
/**
 * �폜�֐��擾
 * @param	list FLDMMDL_MOVE_PROC_LIST
 * @retval	FLDMMDL_MOVE_PROC_DEL list�̓���֐�
 */
//--------------------------------------------------------------
static FLDMMDL_MOVE_PROC_DEL fmmdl_MoveProcList_DeleteGet(
		const FLDMMDL_MOVE_PROC_LIST *list )
{
	return( list->delete_proc );
}

//==============================================================================
//	FLDMMDL_DRAW_PROC_LIST FLDMMDL_DRAW_PROC_LIST_REG�Q��
//==============================================================================
//--------------------------------------------------------------
/**
 * �������֐��擾
 * @param	list	FLDMMDL_DRAW_PROC_LIST *
 * @retval	FLDMMDL_DRAW_PROC_INIT	FLDMMDL_DRAW_PROC_INIT *
 */
//--------------------------------------------------------------
static FLDMMDL_DRAW_PROC_INIT fmmdl_DrawProcList_InitGet(
	const FLDMMDL_DRAW_PROC_LIST *list )
{
	return( list->init_proc );
}

//--------------------------------------------------------------
/**
 * �`��֐��擾
 * @param	list	FLDMMDL_DRAW_PROC_LIST *
 * @retval	FLDMMDL_DRAW_PROC		FLDMMDL_DRAW_PROC *
 */
//--------------------------------------------------------------
static FLDMMDL_DRAW_PROC fmmdl_DrawProcList_DrawGet( const FLDMMDL_DRAW_PROC_LIST *list )
{
	return( list->draw_proc );
}

//--------------------------------------------------------------
/**
 * �폜�֐��擾
 * @param	list	FLDMMDL_DRAW_PROC_LIST *
 * @retval	FLDMMDL_DRAW_PROC_DEL		FLDMMDL_DRAW_PROC_DEL *
 */
//--------------------------------------------------------------
static FLDMMDL_DRAW_PROC_DEL fmmdl_DrawProcList_DeleteGet(
		const FLDMMDL_DRAW_PROC_LIST *list )
{
	return( list->delete_proc );
}

//--------------------------------------------------------------
/**
 * �ޔ��֐��擾
 * @param	list	FLDMMDL_DRAW_PROC_LIST *
 * @retval	FLDMMDL_DRAW_PROC_PUSH		FLDMMDL_DRAW_PROC_PUSH *
 */
//--------------------------------------------------------------
static FLDMMDL_DRAW_PROC_PUSH fmmdl_DrawProcList_PushGet(
		const FLDMMDL_DRAW_PROC_LIST *list )
{
	return( list->push_proc );
}

//--------------------------------------------------------------
/**
 * ���A�֐��擾
 * @param	list	FLDMMDL_DRAW_PROC_LIST *
 * @retval	FLDMMDL_DRAW_PROC_PUSH		FLDMMDL_DRAW_PROC_PUSH *
 */
//--------------------------------------------------------------
static FLDMMDL_DRAW_PROC_POP fmmdl_DrawProcList_PopGet(
		const FLDMMDL_DRAW_PROC_LIST *list )
{
	return( list->pop_proc );
}

//--------------------------------------------------------------
/**
 * �e�[�u������w��̃��X�g���o��
 * @param	code	HERO��
 * @retval	list	code�ɑΉ�����FLDMMDL_DRAW_PROC_LIST *
 */
//--------------------------------------------------------------
static const FLDMMDL_DRAW_PROC_LIST * fmmdl_DrawProcListGet( u32 code )
{
#if 0
	const FLDMMDL_DRAW_PROC_LIST_REG *tbl = DATA_FieldOBJDrawProcListRegTbl;
	
	do{
		if( tbl->code == code ){
			return( tbl->list );
		}
		
		tbl++;
	}while( tbl->code != OBJCODEMAX );
	
	GF_ASSERT( 0 && "fmmdl_DrawProcListGet()�s���ȃR�[�h" );
	return( NULL );
#else
	return( NULL );
#endif
}

//==============================================================================
//	�t�B�[���h���샂�f�� �c�[��
//==============================================================================
//--------------------------------------------------------------
/**
 * �w�肳�ꂽ�O���b�hX,Z���W�ɂ���OBJ���擾
 * @param	sys			FLDMMDLSYS *
 * @param	x			�����O���b�hX
 * @param	z			�����O���b�hZ
 * @param	old_hit		TURE=�ߋ����W�����肷��
 * @retval	FLDMMDL	x,z�ʒu�ɂ���FLDMMDL * �BNULL=���̍��W��OBJ�͂��Ȃ�
 */
//--------------------------------------------------------------
FLDMMDL * FLDMMDL_SearchGridPos(
	const FLDMMDLSYS *sys, int x, int z, int old_hit )
{
	int max;
	FLDMMDL * fmmdl;
	
	max = FLDMMDLSYS_OBJMaxGet( sys );
	fmmdl = FLDMMDLSYS_fmmdlWorkGet( sys );
	
	do{
		if( FLDMMDL_StatusBit_Check(fmmdl,FLDMMDL_STABIT_USE) ){
			if( old_hit ){
				if( FLDMMDL_OldPosGX_Get(fmmdl) == x &&
					FLDMMDL_OldPosGZ_Get(fmmdl) == z ){
					return( fmmdl );
				}
			}
			
			if( FLDMMDL_NowPosGX_Get(fmmdl) == x &&
				FLDMMDL_NowPosGZ_Get(fmmdl) == z ){
				return( fmmdl );
			}
		}
		
		fmmdl++;
		max--;
	}while( max );
	
	return( NULL );
}

//--------------------------------------------------------------
/**
 * ���W�A�������������Bfx32�^
 * @param	fmmdl	FLDMMDL *
 * @param	vec		���������W
 * @param	dir		���� DIR_UP��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_VecPosDirInit( FLDMMDL * fmmdl, const VecFx32 *vec, int dir )
{
	int gx,gy,gz;
	
	gx = SIZE_GRID_FX32( vec->x );
	FLDMMDL_NowPosGX_Set( fmmdl, gx );
	
	gy = SIZE_H_GRID_FX32( vec->y );
	FLDMMDL_NowPosGY_Set( fmmdl, gy );
	
	gz = SIZE_GRID_FX32( vec->z );
	FLDMMDL_NowPosGZ_Set( fmmdl, gz );
	
	FLDMMDL_VecPosSet( fmmdl, vec );
	FLDMMDL_GPosUpdate( fmmdl );
	
	FLDMMDL_DirDispSetForce( fmmdl, dir );
	
//	�Ȃ�ł���Ă��̂�
//	fmmdl_WorkInit_MoveProcInit( fmmdl );
//	fmmdl_WorkInit_DrawProcInit( fmmdl );
	
	FLDMMDL_AcmdFree( fmmdl );
	FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_MOVE_START );
	FLDMMDL_StatusBit_OFF( fmmdl, FLDMMDL_STABIT_MOVE | FLDMMDL_STABIT_MOVE_END );
}

//--------------------------------------------------------------
/**
 * ���W�A�������������B�O���b�h���W�^
 * @param	fmmdl	FLDMMDL *
 * @param	x		�O���b�hX���W
 * @param	y		�O���b�hY���W
 * @param	z		�O���b�hZ���W
 * @param	dir		���� DIR_UP��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_GPosDirInit( FLDMMDL * fmmdl, int x, int y, int z, int dir )
{
	VecFx32 vec;
	
	vec.x = GRID_SIZE_FX32( x ) + FLDMMDL_VEC_X_GRID_OFFS_FX32;
	FLDMMDL_NowPosGX_Set( fmmdl, x );
	
	vec.y = H_GRID_SIZE_FX32( y ) + FLDMMDL_VEC_Y_GRID_OFFS_FX32;
	FLDMMDL_NowPosGY_Set( fmmdl, y );
	
	vec.z = GRID_SIZE_FX32( z ) + FLDMMDL_VEC_Z_GRID_OFFS_FX32;
	FLDMMDL_NowPosGZ_Set( fmmdl, z );
	
	FLDMMDL_VecPosSet( fmmdl, &vec );
	FLDMMDL_GPosUpdate( fmmdl );
	
	FLDMMDL_DirDispSetForce( fmmdl, dir );
	
//	�Ȃ�ł���Ă��̂�
//	fmmdl_WorkInit_MoveProcInit( fmmdl );
//	fmmdl_WorkInit_DrawProcInit( fmmdl );

//	�����͈����̒l��M�p����
//	FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_MOVE_START | FLDMMDL_STABIT_HEIGHT_GET_NEED );
	FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_MOVE_START );
	FLDMMDL_StatusBit_OFF( fmmdl, FLDMMDL_STABIT_MOVE | FLDMMDL_STABIT_MOVE_END );
	
	FLDMMDL_AcmdFree( fmmdl );
}

//--------------------------------------------------------------
/**
 * ����R�[�h�ύX
 * @param	fmmdl	FLDMMDL *
 * @param	code	MV_RND��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveCodeChange( FLDMMDL * fmmdl, u32 code )
{
	FLDMMDL_MoveDeleteProcCall( fmmdl );
	FLDMMDL_MoveCodeSet( fmmdl, code );
	fmmdl_WorkInit_MoveProcInit( fmmdl );
	FLDMMDL_MoveInit( fmmdl );
}

//--------------------------------------------------------------
/**
 * OBJ ID��ύX
 * @param	fmmdl	FLDMMDL *
 * @param	id		OBJ ID
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_OBJIDChange( FLDMMDL * fmmdl, int id )
{
	FLDMMDL_OBJIDSet( fmmdl, id );
		
	FLDMMDL_StatusBitON_MoveStart( fmmdl );
	fmmdl_DrawEffectFlagInit( fmmdl );
}

//==============================================================================
//	����֐��_�~�[
//==============================================================================
//--------------------------------------------------------------
/**
 * ���쏉�����֐��_�~�[
 * @param	FLDMMDL	FLDMMDL * 
 * @retval	int			TRUE=����������
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

//==============================================================================
//	�`��֐��_�~�[
//==============================================================================
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
