//======================================================================
/**
 * @file	fldmmdl.c
 * @brief	���샂�f��
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
//	MMDL ����A�`��֐����[�N�T�C�Y (byte size)
//--------------------------------------------------------------
#define MMDL_MOVE_WORK_SIZE		(16)	///<����֐��p���[�N�T�C�Y
#define MMDL_MOVE_SUB_WORK_SIZE	(16)	///<����T�u�֐��p���[�N�T�C�Y
#define MMDL_MOVE_CMD_WORK_SIZE	(16)	///<����R�}���h�p���[�N�T�C�Y
#define MMDL_DRAW_WORK_SIZE		(32)	///<�`��֐��p���[�N�T�C�Y

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
	u16 fmmdl_max;					///<MMDL�ő吔
	HEAPID sysHeapID;				///<�V�X�e���p �q�[�vID
	MMDL *pMMdlBuf;			///<MMDL���[�N *
	
	HEAPID heapID;					///<�q�[�vID
	s16 fmmdl_count;				///<�t�B�[���h���샂�f�����ݐ�
	u16 tcb_pri;					///<TCB�v���C�I���e�B
	u16 dummy;						///<�]��
	const FLDMAPPER *pG3DMapper;	///<FLDMAPPER

	void *pTCBSysWork;				///<TCB���[�N
	GFL_TCBSYS *pTCBSys;			///<TCBSYS*
	
	MMDL_BLACTCONT *pBlActCont;	///<MMDL_BLACTCONT
	
	u8 *pOBJCodeParamBuf;			///<OBJCODE_PARAM�o�b�t�@
	const OBJCODE_PARAM *pOBJCodeParamTbl; ///<OBJCODE_PARAM
  
  void *fieldMapWork; ///<FIELDMAP_WORK
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
	const MMDLSYS *pMMdlSys;///<MMDLSYS*
	
	const MMDL_MOVE_PROC_LIST *move_proc_list; ///<����֐����X�g
	const MMDL_DRAW_PROC_LIST *draw_proc_list; ///<�`��֐����X�g
	
	u8 move_proc_work[MMDL_MOVE_WORK_SIZE];///����֐��p���[�N
	u8 move_sub_proc_work[MMDL_MOVE_SUB_WORK_SIZE];///����T�u�֐��p���[�N
	u8 move_cmd_proc_work[MMDL_MOVE_CMD_WORK_SIZE];///����R�}���h�p���[�N
	u8 draw_proc_work[MMDL_DRAW_WORK_SIZE];///�`��֐��p���[�N
};

#define MMDL_SIZE (sizeof(MMDL)) ///<MMDL�T�C�Y 224

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

//======================================================================
//	proto
//======================================================================
//MMDLSYS �v���Z�X
static void MMdlSys_DeleteProc( MMDLSYS *fos );

//MMDL �ǉ��A�폜
static void MMdl_SetHeader(
	MMDL * fmmdl, const MMDL_HEADER *head, void *sys );
static void MMdl_SetHeaderPos(MMDL *fmmdl,const MMDL_HEADER *head);
static void MMdl_InitWork( MMDL * fmmdl, const MMDLSYS *sys );
static void MMdl_InitCallMoveProcWork( MMDL * fmmdl );
static void MMdl_InitMoveWork( const MMDLSYS *fos, MMDL * fmmdl );
#if 0
static void MMdlSys_CheckSetInitMoveWork( MMDLSYS *fos );
static void MMdlSys_CheckSetInitDrawWork( MMDLSYS *fos );
#endif

//MMDL ����֐�
static void MMdl_TCB_MoveProc( GFL_TCB * tcb, void *work );
static void MMdl_TCB_DrawProc( MMDL * fmmdl );

//MMDL_SAVEDATA
static void MMdl_SaveData_SaveMMdl(
	const MMDL *fmmdl, MMDL_SAVEWORK *save );
static void MMdl_SaveData_LoadMMdl(
	MMDL *fmmdl, const MMDL_SAVEWORK *save, const MMDLSYS *fos );

//MMDLSYS �ݒ�A�Q��
static void MMdlSys_OnStatusBit(
	MMDLSYS *fmmdlsys, MMDLSYS_STABIT bit );
static void MMdlSys_OffStatusBit(
	MMDLSYS *fmmdlsys, MMDLSYS_STABIT bit );
static void MMdlSys_IncrementOBJCount( MMDLSYS *fmmdlsys );
static void MMdlSys_DecrementOBJCount( MMDLSYS *fmmdlsys );

//MMDLSYS �c�[��
static MMDL * MMdlSys_SearchSpaceMMdl( const MMDLSYS *sys );
static MMDL * MMdlSys_SearchAlies(
	const MMDLSYS *fos, int obj_id, int zone_id );

//MMDL �c�[��
static void MMdl_AddTCB( MMDL *fmmdl, const MMDLSYS *sys );
static void MMdl_DeleteTCB( MMDL *fmmdl );
static void MMdl_InitDrawWork( MMDL *fmmdl );
static void MMdl_InitCallDrawProcWork( MMDL * fmmdl );
static void MMdl_InitDrawEffectFlag( MMDL * fmmdl );
static void MMdl_ClearWork( MMDL *fmmdl );
static int MMdl_CheckHeaderAlies(
		const MMDL *fmmdl, int h_zone_id, int max,
		const MMDL_HEADER *head );
static MMDL * MMdl_SearchOBJIDZoneID(
		const MMDLSYS *fos, int obj_id, int zone_id );
static void MMdl_InitDrawStatus( MMDL * fmmdl );
static void MMdl_SetDrawDeleteStatus( MMDL * fmmdl );
static void MMdl_ChangeAliesOBJ(
	MMDL *fmmdl, const MMDL_HEADER *head, int zone_id );
static void MMdl_ChangeOBJAlies(
	MMDL * fmmdl, int zone_id, const MMDL_HEADER *head );

//OBJCODE_PARAM
static void MMdlSys_InitOBJCodeParam( MMDLSYS *fmmdlsys );
static void MMdlSys_DeleteOBJCodeParam( MMDLSYS *fmmdlsys );

//parts
static u16 WorkOBJCode_GetOBJCode( void *fsys, int code );
static const MMDL_MOVE_PROC_LIST * MoveProcList_GetList( u16 code );
static const MMDL_DRAW_PROC_LIST * DrawProcList_GetList(
		MMDL_DRAWPROCNO no );
static BOOL MMdlHeader_CheckAlies( const MMDL_HEADER *head );
static int MMdlHeader_GetAliesZoneID( const MMDL_HEADER *head );

//======================================================================
//	�t�B�[���h���샂�f���@�V�X�e��
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS �V�X�e���쐬
 * @param	heapID	HEAPID
 * @param	max	���샂�f���ő吔
 * @retval	MMDLSYS* �쐬���ꂽMMDLSYS*
 */
//--------------------------------------------------------------
MMDLSYS * MMDLSYS_CreateSystem( HEAPID heapID, u32 max )
{
	MMDLSYS *fos;
	fos = GFL_HEAP_AllocClearMemory( heapID, MMDLSYS_SIZE );
	fos->pMMdlBuf = GFL_HEAP_AllocClearMemory( heapID, max*MMDL_SIZE );
	fos->fmmdl_max = max;
	fos->sysHeapID = heapID;
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
	GFL_HEAP_FreeMemory( fos->pMMdlBuf );
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
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDLSYS_SetupProc(
	MMDLSYS *fos, HEAPID heapID, const FLDMAPPER *pG3DMapper )
{
	fos->heapID = heapID;
	fos->pG3DMapper = pG3DMapper;
	
	MMdlSys_InitOBJCodeParam( fos );
	
	fos->pTCBSysWork = GFL_HEAP_AllocMemory(
		heapID, GFL_TCB_CalcSystemWorkSize(fos->fmmdl_max) );
	fos->pTCBSys = GFL_TCB_Init( fos->fmmdl_max, fos->pTCBSysWork );
	
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
	MMDLSYS_DeleteDraw( fos );
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
void MMDLSYS_SetupDrawProc( MMDLSYS *fos )
{
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
	const MMDLSYS *fos, const MMDL_HEADER *header, int zone_id )
{
	MMDL *fmmdl;
	MMDL_HEADER header_data = *header;
	const MMDL_HEADER *head = &header_data;
	
	fmmdl = MMdlSys_SearchSpaceMMdl( fos );
	GF_ASSERT( fmmdl != NULL );
	
	MMdl_SetHeader( fmmdl, head, NULL );
	MMdl_InitWork( fmmdl, fos );
	MMDL_SetZoneID( fmmdl, zone_id );
	
	if( MMDLSYS_CheckStatusBit(fos,MMDLSYS_STABIT_MOVE_INIT_COMP) ){
		MMdl_InitMoveWork( fos, fmmdl );
		MMDL_InitMoveProc( fmmdl );
	}
	
	if( MMDLSYS_CheckStatusBit(fos,MMDLSYS_STABIT_DRAW_INIT_COMP) ){
		MMdl_InitDrawWork( fmmdl );
	}
	
	MMdlSys_IncrementOBJCount( (MMDLSYS*)MMDL_GetMMdlSys(fmmdl) );
	return( fmmdl );
}

//--------------------------------------------------------------
/**
 * MMDLSYS �t�B�[���h���샂�f����ǉ��@����
 * @param	fos			MMDLSYS *
 * @param	header		�ǉ��������Z�߂�MMDL_HEADER *
 * @param	zone_id		�]�[��ID
 * @param	count		header�v�f��
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDLSYS_SetMMdl( const MMDLSYS *fos,
	const MMDL_HEADER *header, int zone_id, int count )
{
	GF_ASSERT( count > 0 );
	GF_ASSERT( header != NULL );
	
	KAGAYA_Printf( "MMDLSYS_SetMMdl Count %d\n", count );
	
	do{
		MMDLSYS_AddMMdl( fos, header, zone_id );
		header++;
		count--;
	}while( count );
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f�����폜
 * @param	fmmdl		�폜����MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_Delete( MMDL * fmmdl )
{
	const MMDLSYS *fos;
	
	fos = MMDL_GetMMdlSys( fmmdl );
	
	if( MMDLSYS_CheckCompleteDrawInit(fos) == TRUE ){
		MMDL_CallDrawDeleteProc( fmmdl );
	}
	
	if( MMDL_CheckMoveBit(fmmdl,MMDL_MOVEBIT_MOVEPROC_INIT) ){
		MMDL_CallMoveDeleteProc( fmmdl );
		MMdl_DeleteTCB( fmmdl );
	}
	
	MMdlSys_DecrementOBJCount( (MMDLSYS*)(fmmdl->pMMdlSys) );
	MMdl_ClearWork( fmmdl );
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
	MMDL *fmmdl;
	
	while( MMDLSYS_SearchUseMMdl(fos,&fmmdl,&no) ){
		MMDL_Delete( fmmdl );
	}
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���@�w�b�_�[��񔽉f
 * @param	fmmdl		�ݒ肷��MMDL * 
 * @param	head		���f�������Z�߂�MMDL_HEADER *
 * @param	fsys		FIELDSYS_WORK *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_SetHeader(
	MMDL * fmmdl, const MMDL_HEADER *head, void *sys )
{
	MMDL_SetOBJID( fmmdl, head->id );
	MMDL_SetOBJCode( fmmdl, WorkOBJCode_GetOBJCode(sys,head->obj_code) );
	MMDL_SetMoveCode( fmmdl, head->move_code );
	MMDL_SetEventType( fmmdl, head->event_type );
	MMDL_SetEventFlag( fmmdl, head->event_flag );
	MMDL_SetEventID( fmmdl, head->event_id );
	fmmdl->dir_head = head->dir;
	MMDL_SetParam( fmmdl, head->param0, MMDL_PARAM_0 );
	MMDL_SetParam( fmmdl, head->param1, MMDL_PARAM_1 );
	MMDL_SetParam( fmmdl, head->param2, MMDL_PARAM_2 );
	MMDL_SetMoveLimitX( fmmdl, head->move_limit_x );
	MMDL_SetMoveLimitZ( fmmdl, head->move_limit_z );
	
	MMdl_SetHeaderPos( fmmdl, head );
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���@���W�n������
 * @param	fmmdl		MMDL * 
 * @param	head		���f�������Z�߂�MMDL_HEADER *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_SetHeaderPos( MMDL *fmmdl, const MMDL_HEADER *head )
{
	int pos;
	VecFx32 vec;
	
	pos = head->gx;
	vec.x = GRID_SIZE_FX32( pos ) + MMDL_VEC_X_GRID_OFFS_FX32;
	MMDL_SetInitGridPosX( fmmdl, pos );
	MMDL_SetOldGridPosX( fmmdl, pos );
	MMDL_SetGridPosX( fmmdl, pos );
	
	pos = head->y;		//pos�ݒ��fx32�^�ŗ���B
	vec.y = (fx32)pos;
	pos = SIZE_GRID_FX32( pos );
	MMDL_SetInitGridPosY( fmmdl, pos );
	MMDL_SetOldGridPosY( fmmdl, pos );
	MMDL_SetGridPosY( fmmdl, pos );
	
	pos = head->gz;
	vec.z = GRID_SIZE_FX32( pos ) + MMDL_VEC_Z_GRID_OFFS_FX32;
	MMDL_SetInitGridPosZ( fmmdl, pos );
	MMDL_SetOldGridPosZ( fmmdl, pos );
	MMDL_SetGridPosZ( fmmdl, pos );
	
	MMDL_SetVectorPos( fmmdl, &vec );
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���@���[�N������
 * @param	fmmdl		MMDL * 
 * @param	sys			MMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_InitWork( MMDL * fmmdl, const MMDLSYS *sys )
{
	MMDL_OnStatusBit( fmmdl,
		MMDL_STABIT_USE |				//�g�p��
		MMDL_STABIT_HEIGHT_GET_ERROR |	//�����擾���K�v
		MMDL_STABIT_ATTR_GET_ERROR );	//�A�g���r���[�g�擾���K�v
	
	if( MMDL_CheckAliesEventID(fmmdl) == TRUE ){
		MMDL_SetStatusBitAlies( fmmdl, TRUE );
	}
	
	fmmdl->pMMdlSys = sys;
	MMDL_SetForceDirDisp( fmmdl, MMDL_GetDirHeader(fmmdl) );
	MMDL_SetDirMove( fmmdl, MMDL_GetDirHeader(fmmdl) );
	MMDL_FreeAcmd( fmmdl );
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f�� ����֐�������
 * @param	fmmdl		MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_InitCallMoveProcWork( MMDL * fmmdl )
{
	fmmdl->move_proc_list =
		MoveProcList_GetList( MMDL_GetMoveCode(fmmdl) );
}

//--------------------------------------------------------------
/**
 * MMDL ���쏉�����ɍs�������Z��
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_InitMoveWork( const MMDLSYS *fos, MMDL *fmmdl )
{
	MMdl_InitCallMoveProcWork( fmmdl );
	MMdl_AddTCB( fmmdl, fos );
	MMDL_OnStatusBit( fmmdl, MMDL_STABIT_MOVE_START );
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
	MMDL *fmmdl;
	
	while( MMDLSYS_SearchUseMMdl(fos,&fmmdl,&i) == TRUE ){
		if( MMDL_CheckMoveBit(fmmdl,	//�������֐��Ăяo���܂�
			MMDL_MOVEBIT_MOVEPROC_INIT) == 0 ){
			MMDL_InitMoveProc( fmmdl );
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
	MMDL *fmmdl;
	
	while( MMDLSYS_SearchUseMMdl(fos,&fmmdl,&i) == TRUE ){
		if( MMDL_CheckStatusBitCompletedDrawInit(fmmdl) == FALSE ){
			MMdl_InitDrawWork( fmmdl );
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
 * @param	fmmdlsys	MMDLSYS
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDLSYS_Push( MMDLSYS *fmmdlsys )
{
	u32 no = 0;
	MMDL *fmmdl;
	
	#ifdef DEBUG_MMDL_PRINT
	if( MMDLSYS_CheckCompleteDrawInit(fmmdlsys) == FALSE ){
		GF_ASSERT( 0 && "WARNING!! ���샂�f�� �`�斢������\n" );
	}
	#endif
	
	while( MMDLSYS_SearchUseMMdl(fmmdlsys,&fmmdl,&no) == TRUE ){
		{ //���쏈���̑ޔ�
			MMdl_DeleteTCB( fmmdl );
			MMDL_OnMoveBit( fmmdl,
				MMDL_MOVEBIT_NEED_MOVEPROC_RECOVER );
		}
		
		{ //�`�揈���̑ޔ�
			MMDL_CallDrawPushProc( fmmdl );
			MMDL_OnStatusBit( fmmdl, MMDL_STABIT_DRAW_PUSH );
		}
	}
}

//--------------------------------------------------------------
/**
 * MMDLSYS ���샂�f�����A
 * @param	fmmdlsys	MMDLSYS
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDLSYS_Pop( MMDLSYS *fmmdlsys )
{
	u32 no = 0;
	MMDL *fmmdl;
	
	while( MMDLSYS_SearchUseMMdl(fmmdlsys,&fmmdl,&no) == TRUE ){
		{	//���쏈�����A
			MMdl_InitMoveWork( fmmdlsys, fmmdl ); //���[�N������
			
			if( MMDL_CheckMoveBit(fmmdl,	//�������֐��Ăяo���܂�
				MMDL_MOVEBIT_MOVEPROC_INIT) == 0 ){
				MMDL_InitMoveProc( fmmdl );
			}
			
			if( MMDL_CheckMoveBit(fmmdl, //�����֐��Ăяo�����K�v
				MMDL_MOVEBIT_NEED_MOVEPROC_RECOVER) ){
				MMDL_CallMovePopProc( fmmdl );
				MMDL_OffMoveBit( fmmdl,
					MMDL_MOVEBIT_NEED_MOVEPROC_RECOVER );
			}
		}
		
		{	//�`�揈�����A
			if( MMDL_CheckStatusBitCompletedDrawInit(fmmdl) == FALSE ){
				MMdl_InitDrawWork( fmmdl );
			}
			
			if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_DRAW_PUSH) ){
				MMDL_CallDrawPopProc( fmmdl );
				MMDL_OffStatusBit( fmmdl, MMDL_STABIT_DRAW_PUSH );
			}
		}

    { //���J�o���[
      MMdl_InitDrawEffectFlag( fmmdl );
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
 * @param	fmmdlsys �Z�[�u����MMDLSYS
 * @param	savedata LDMMDL_SAVEDATA
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SAVEDATA_Save(
	MMDLSYS *fmmdlsys, MMDL_SAVEDATA *savedata )
{
	u32 no = 0;
	MMDL *fmmdl;
	MMDL_SAVEWORK *save = savedata->SaveWorkBuf;
	
	while( MMDLSYS_SearchUseMMdl(fmmdlsys,&fmmdl,&no) == TRUE ){
		MMdl_SaveData_SaveMMdl( fmmdl, save );
		save++;
	}
}

//--------------------------------------------------------------
/**
 * MMDL_SAVEDATA ���샂�f�����[�h
 * @param	fmmdlsys	MMDLSYS
 * @param	save	���[�h����MMDL_SAVEWORK
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SAVEDATA_Load(
	MMDLSYS *fmmdlsys, MMDL_SAVEDATA *savedata )
{
	u32 no = 0;
	MMDL *fmmdl;
	MMDL_SAVEWORK *save = savedata->SaveWorkBuf;
	
	while( no < MMDL_SAVEMMDL_MAX ){
		if( (save->status_bit&MMDL_STABIT_USE) ){
			fmmdl = MMdlSys_SearchSpaceMMdl( fmmdlsys );
			MMdl_SaveData_LoadMMdl( fmmdl, save, fmmdlsys );
		}
		save++;
		no++;
	}

  fmmdlsys->fmmdl_count = no;
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
	const MMDL *fmmdl, MMDL_SAVEWORK *save )
{
	save->status_bit = MMDL_GetStatusBit( fmmdl );
	save->move_bit = MMDL_GetMoveBit( fmmdl );
	save->obj_id = MMDL_GetOBJID( fmmdl );
	save->zone_id = MMDL_GetZoneID( fmmdl );
	save->obj_code = MMDL_GetOBJCode( fmmdl );
	save->move_code = MMDL_GetMoveCode( fmmdl );
	save->event_type = MMDL_GetEventType( fmmdl );
	save->event_flag = MMDL_GetEventFlag( fmmdl );
	save->event_id = MMDL_GetEventID( fmmdl );
	save->dir_head = MMDL_GetDirHeader( fmmdl );
	save->dir_disp = MMDL_GetDirDisp( fmmdl );
	save->dir_move = MMDL_GetDirMove( fmmdl );
	save->param0 = MMDL_GetParam( fmmdl, MMDL_PARAM_0 );
	save->param1 = MMDL_GetParam( fmmdl, MMDL_PARAM_1 );
	save->param2 = MMDL_GetParam( fmmdl, MMDL_PARAM_2 );
	save->move_limit_x = MMDL_GetMoveLimitX( fmmdl );
	save->move_limit_z = MMDL_GetMoveLimitZ( fmmdl );
	save->gx_init = MMDL_GetInitGridPosX( fmmdl );
	save->gy_init = MMDL_GetInitGridPosY( fmmdl );
	save->gz_init = MMDL_GetInitGridPosZ( fmmdl );
	save->gx_now = MMDL_GetGridPosX( fmmdl );
	save->gy_now = MMDL_GetGridPosY( fmmdl );
	save->gz_now = MMDL_GetGridPosZ( fmmdl );
	save->fx32_y = MMDL_GetVectorPosY( fmmdl );
	
	MI_CpuCopy8( MMDL_GetMoveProcWork((MMDL*)fmmdl),
		save->move_proc_work, MMDL_MOVE_WORK_SIZE );
	MI_CpuCopy8( MMDL_GetMoveSubProcWork((MMDL*)fmmdl),
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
	MMDL *fmmdl, const MMDL_SAVEWORK *save, const MMDLSYS *fos )
{
	MMdl_ClearWork( fmmdl );

	fmmdl->status_bit = save->status_bit;
	fmmdl->move_bit = save->move_bit;
	MMDL_SetOBJID( fmmdl, save->obj_id );
	MMDL_SetZoneID( fmmdl, save->zone_id );
	MMDL_SetOBJCode( fmmdl, save->obj_code ); 
	MMDL_SetMoveCode( fmmdl, save->move_code );
	MMDL_SetEventType( fmmdl, save->event_type );
	MMDL_SetEventFlag( fmmdl, save->event_flag );
	MMDL_SetEventID( fmmdl, save->event_id );
	fmmdl->dir_head = save->dir_head;
	MMDL_SetForceDirDisp( fmmdl, save->dir_disp );
	MMDL_SetDirMove( fmmdl, save->dir_move );
	MMDL_SetParam( fmmdl, save->param0, MMDL_PARAM_0 );
	MMDL_SetParam( fmmdl, save->param1, MMDL_PARAM_1 );
	MMDL_SetParam( fmmdl, save->param2, MMDL_PARAM_2 );
	MMDL_SetMoveLimitX( fmmdl, save->move_limit_x );
	MMDL_SetMoveLimitZ( fmmdl, save->move_limit_z );
	MMDL_SetInitGridPosX( fmmdl, save->gx_init );
	MMDL_SetInitGridPosY( fmmdl, save->gy_init );
	MMDL_SetInitGridPosZ( fmmdl, save->gz_init );
	MMDL_SetGridPosX( fmmdl, save->gx_now );
	MMDL_SetGridPosY( fmmdl, save->gy_now );
	MMDL_SetGridPosZ( fmmdl, save->gz_now );
	
	MI_CpuCopy8( save->move_proc_work,
		MMDL_GetMoveProcWork((MMDL*)fmmdl), MMDL_MOVE_WORK_SIZE );
	MI_CpuCopy8( save->move_sub_proc_work,
		MMDL_GetMoveSubProcWork((MMDL*)fmmdl), MMDL_MOVE_SUB_WORK_SIZE );
	
	fmmdl->pMMdlSys = fos;
	
	{ //���W���A
		s16 grid;
		VecFx32 pos = {0,0,0};
		
		grid = MMDL_GetGridPosX( fmmdl );
		MMDL_SetOldGridPosX( fmmdl, grid );
		pos.x = GRID_SIZE_FX32( grid ) + MMDL_VEC_X_GRID_OFFS_FX32;
		
		grid = MMDL_GetGridPosY( fmmdl );
		MMDL_SetOldGridPosY( fmmdl, grid );
		pos.y = save->fx32_y; //�Z�[�u���̍�����M�p����
	
		grid = MMDL_GetGridPosZ( fmmdl );
		MMDL_SetOldGridPosZ( fmmdl, grid );
		pos.z = GRID_SIZE_FX32( grid ) + MMDL_VEC_Z_GRID_OFFS_FX32;
	
		MMDL_SetVectorPos( fmmdl, &pos );
	}

	{ //�X�e�[�^�X�r�b�g���A
		MMDL_OnStatusBit( fmmdl,
			MMDL_STABIT_USE |
//			MMDL_STABIT_HEIGHT_GET_NEED | //�Z�[�u���̍�����M�p����
			MMDL_STABIT_MOVE_START );
		
		MMDL_OffStatusBit( fmmdl,
			MMDL_STABIT_PAUSE_MOVE |
			MMDL_STABIT_VANISH |
			MMDL_STABIT_DRAW_PROC_INIT_COMP |
			MMDL_STABIT_JUMP_START |
			MMDL_STABIT_JUMP_END |
			MMDL_STABIT_MOVE_END |
			MMDL_STABIT_FELLOW_HIT_NON |
			MMDL_STABIT_TALK_OFF |
			MMDL_STABIT_DRAW_PUSH |
			MMDL_STABIT_BLACT_ADD_PRAC |
			MMDL_STABIT_HEIGHT_GET_OFF );
		
		MMDL_OffStatusBit( fmmdl,
			MMDL_STABIT_SHADOW_SET |
			MMDL_STABIT_SHADOW_VANISH |
			MMDL_STABIT_EFFSET_SHOAL	 |
			MMDL_STABIT_REFLECT_SET );
		
		if( MMDL_CheckMoveBit(fmmdl,MMDL_MOVEBIT_MOVEPROC_INIT) == 0 ){
			MMDL_OnMoveBit( fmmdl, MMDL_MOVEBIT_NEED_MOVEPROC_RECOVER );
		}
	}
}


//======================================================================
//	MMDL ����֐�
//======================================================================
//--------------------------------------------------------------
/**
 * MMDL ���암�����s
 * @param	fmmdl	MMDL*
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_UpdateMoveProc( MMDL *fmmdl )
{
	MMDL_UpdateMove( fmmdl );
	
	if( MMDL_CheckStatusBitUse(fmmdl) == TRUE ){
		MMdl_TCB_DrawProc( fmmdl );
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
	MMDL *fmmdl = (MMDL *)work;
	MMDL_UpdateMoveProc( fmmdl );
}

//--------------------------------------------------------------
/**
 * MMDL TCB ����֐�����Ă΂��`��֐�
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_TCB_DrawProc( MMDL * fmmdl )
{
	const MMDLSYS *fos = MMDL_GetMMdlSys(fmmdl);
	
	if( MMDLSYS_CheckCompleteDrawInit(fos) == TRUE ){
		MMDL_UpdateDraw( fmmdl );
	}
}

//======================================================================
//	MMDL �A�j���[�V�����R�}���h
//======================================================================
//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h���\���`�F�b�N
 * @param	fmmdl		�ΏۂƂȂ�MMDL * 
 * @retval	int			TRUE=�\�BFALSE=����
 */
//--------------------------------------------------------------
BOOL MMDL_CheckPossibleAcmd( const MMDL * fmmdl )
{
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_USE) == 0 ){
		return( FALSE );
	}
	
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_MOVE) ){
		return( FALSE );
	}
	
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_ACMD) &&
		MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_ACMD_END) == 0 ){
		return( FALSE );
	}
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h�Z�b�g
 * @param	fmmdl		�ΏۂƂȂ�MMDL * 
 * @param	code		���s����R�[�h�BAC_DIR_U��
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetAcmd( MMDL * fmmdl, u16 code )
{
	GF_ASSERT( code < ACMD_MAX );
	MMDL_SetAcmdCode( fmmdl, code );
	MMDL_SetAcmdSeq( fmmdl, 0 );
	MMDL_OnStatusBit( fmmdl, MMDL_STABIT_ACMD );
	MMDL_OffStatusBit( fmmdl, MMDL_STABIT_ACMD_END );
}

//--------------------------------------------------------------
/**
 * �R�}���h�Z�b�g
 * @param	fmmdl		�ΏۂƂȂ�MMDL * 
 * @param	code		���s����R�[�h�BAC_DIR_U��
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetLocalAcmd( MMDL * fmmdl, u16 code )
{
	MMDL_SetAcmdCode( fmmdl, code );
	MMDL_SetAcmdSeq( fmmdl, 0 );
	MMDL_OffStatusBit( fmmdl, MMDL_STABIT_ACMD_END );
}

//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h�I���`�F�b�N�B
 * @param	fmmdl		�ΏۂƂȂ�MMDL * 
 * @retval	int			TRUE=�I��
 */
//--------------------------------------------------------------
BOOL MMDL_CheckEndAcmd( const MMDL * fmmdl )
{
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_ACMD) == 0 ){
		return( TRUE );
	}
	
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_ACMD_END) == 0 ){
		return( FALSE );
	}
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h�I���`�F�b�N�ƊJ���B
 * �A�j���[�V�����R�}���h���I�����Ă��Ȃ��ꍇ�͊J������Ȃ��B
 * @param	fmmdl		�ΏۂƂȂ�MMDL * 
 * @retval	BOOL	TRUE=�I�����Ă���B
 */
//--------------------------------------------------------------
BOOL MMDL_EndAcmd( MMDL * fmmdl )
{
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_ACMD) == 0 ){
		return( TRUE );
	}
	
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_ACMD_END) == 0 ){
		return( FALSE );
	}
	
	MMDL_OffStatusBit(
		fmmdl, MMDL_STABIT_ACMD|MMDL_STABIT_ACMD_END );
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * �A�j���[�V�����R�}���h�J���B
 * �A�j���[�V�����R�}���h���I�����Ă��Ȃ��Ƃ������J���B
 * @param	fmmdl		�ΏۂƂȂ�MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_FreeAcmd( MMDL * fmmdl )
{
	MMDL_OffStatusBit( fmmdl, MMDL_STABIT_ACMD );
	MMDL_OnStatusBit( fmmdl, MMDL_STABIT_ACMD_END ); //���[�J���R�}���h�t���O
	MMDL_SetAcmdCode( fmmdl, ACMD_NOT );
	MMDL_SetAcmdSeq( fmmdl, 0 );
}

//======================================================================
//	MMDLSYS �p�����^�ݒ�A�Q��
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS �X�e�[�^�X�r�b�g�`�F�b�N
 * @param	fmmdlsys	MMDLSYS*
 * @param	bit	MMDLSYS_STABIT
 * @retval	u32	(status bit & bit)
 */
//--------------------------------------------------------------
u32 MMDLSYS_CheckStatusBit(
	const MMDLSYS *fmmdlsys, MMDLSYS_STABIT bit )
{
	return( (fmmdlsys->status_bit&bit) );
}

//--------------------------------------------------------------
/**
 * MMDLSYS �X�e�[�^�X�r�b�g ON
 * @param	fmmdlsys	MMDLSYS*
 * @param	bit	MMDLSYS_STABIT
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdlSys_OnStatusBit(
	MMDLSYS *fmmdlsys, MMDLSYS_STABIT bit )
{
	fmmdlsys->status_bit |= bit;
}

//--------------------------------------------------------------
/**
 * MMDLSYS �X�e�[�^�X�r�b�g OFF
 * @param	fmmdlsys	MMDLSYS*
 * @param	bit	MMDLSYS_STABIT
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdlSys_OffStatusBit(
	MMDLSYS *fmmdlsys, MMDLSYS_STABIT bit )
{
	fmmdlsys->status_bit &= ~bit;
}

//--------------------------------------------------------------
/**
 * FMMDLSYS ���샂�f���ő吔���擾
 * @param	fmmdlsys	MMDLSYS*
 * @retval	u16	�ő吔
 */
//--------------------------------------------------------------
u16 MMDLSYS_GetMMdlMax( const MMDLSYS *fmmdlsys )
{
	return( fmmdlsys->fmmdl_max );
}

//--------------------------------------------------------------
/**
 * FMMDLSYS ���ݔ������Ă��铮�샂�f���̐����擾
 * @param	fmmdlsys	MMDLSYS*
 * @retval	u16	������
 */
//--------------------------------------------------------------
u16 MMDLSYS_GetMMdlCount( const MMDLSYS *fmmdlsys )
{
	return( fmmdlsys->fmmdl_count );
}

//--------------------------------------------------------------
/**
 * MMDLSYS TCB�v���C�I���e�B���擾
 * @param	fmmdlsys	MMDLSYS*
 * @retval	u16	TCB�v���C�I���e�B
 */
//--------------------------------------------------------------
u16 MMDLSYS_GetTCBPriority( const MMDLSYS *fmmdlsys )
{
	return( fmmdlsys->tcb_pri );
}

//--------------------------------------------------------------
/**
 * MMDLSYS HEAPID�擾
 * @param	fmmdlsys	MMDLSYS*
 * @retval	HEAPID HEAPID
 */
//--------------------------------------------------------------
HEAPID MMDLSYS_GetHeapID( const MMDLSYS *fmmdlsys )
{
	return( fmmdlsys->heapID );
}

//--------------------------------------------------------------
/**
 * MMDLSYS ���ݔ������Ă���OBJ����1����
 * @param	fmmdlsys	MMDLSYS*
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdlSys_IncrementOBJCount( MMDLSYS *fmmdlsys )
{
	fmmdlsys->fmmdl_count++;
}

//--------------------------------------------------------------
/**
 * MMDLSYS ���ݔ������Ă���OBJ����1����
 * @param	fmmdlsys	MMDLSYS*
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdlSys_DecrementOBJCount( MMDLSYS *fmmdlsys )
{
	fmmdlsys->fmmdl_count--;
	GF_ASSERT( fmmdlsys->fmmdl_count >= 0 );
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
 * MMDLSYS MMDL_BLACTCONT�Z�b�g
 * @param	fmmdlsys	MMDLSYS
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDLSYS_SetBlActCont(
	MMDLSYS *fmmdlsys, MMDL_BLACTCONT *pBlActCont )
{
	fmmdlsys->pBlActCont = pBlActCont;
}

//--------------------------------------------------------------
/**
 * MMDLSYS MMDL_BLACTCONT�擾
 * @param	fmmdlsys MMDLSYS
 * @retval	MMDL_BLACTCONT*
 */
//--------------------------------------------------------------
MMDL_BLACTCONT * MMDLSYS_GetBlActCont( MMDLSYS *fmmdlsys )
{
	GF_ASSERT( fmmdlsys->pBlActCont != NULL );
	return( fmmdlsys->pBlActCont );
}

//--------------------------------------------------------------
/**
 * MMDLSYS FLDMAPPER�擾
 * @param	fmmdlsys	MMDLSYS
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
 * @param fmmdlsys MMDLSYS
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
 * @param fmmdlsys MMDLSYS
 * @retval fieldMapWork FIELDMAP_WORK
 */
//--------------------------------------------------------------
void * MMDLSYS_GetFieldMapWork( MMDLSYS *fos )
{
  GF_ASSERT( fos->fieldMapWork != NULL );
  return( fos->fieldMapWork );
}

//======================================================================
//	MMDL�@�p�����^�Q�ƁA�ݒ�
//======================================================================
//--------------------------------------------------------------
/**
 * MMDL �X�e�[�^�X�r�b�gON
 * @param	fmmdl	fmmdl
 * @param	bit		MMDL_STABIT
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_OnStatusBit( MMDL *fmmdl, MMDL_STABIT bit )
{
	fmmdl->status_bit |= bit;
}

//--------------------------------------------------------------
/**
 * MMDL �X�e�[�^�X�r�b�gOFF
 * @param	fmmdl	fmmdl
 * @param	bit		MMDL_STABIT
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_OffStatusBit( MMDL *fmmdl, MMDL_STABIT bit )
{
	fmmdl->status_bit &= ~bit;
}

//--------------------------------------------------------------
/**
 * MMDL �X�e�[�^�X�r�b�g�擾
 * @param	fmmdl		MMDL * 
 * @retval	MMDL_STABIT �X�e�[�^�X�r�b�g
 */
//--------------------------------------------------------------
MMDL_STABIT MMDL_GetStatusBit( const MMDL * fmmdl )
{
	return( fmmdl->status_bit );
}

//--------------------------------------------------------------
/**
 * MMDL �X�e�[�^�X�r�b�g�`�F�b�N
 * @param	fmmdl	MMDL*
 * @param	bit	MMDL_STABIT
 * @retval	u32	(status bit & bit)
 */
//--------------------------------------------------------------
u32 MMDL_CheckStatusBit( const MMDL *fmmdl, MMDL_STABIT bit )
{
	return( (fmmdl->status_bit&bit) );
}

//--------------------------------------------------------------
/**
 * MMDL ����r�b�g�@�擾
 * @param	fmmdl	MMDL *
 * @retval	u32		����r�b�g
 */
//--------------------------------------------------------------
MMDL_MOVEBIT MMDL_GetMoveBit( const MMDL * fmmdl )
{
	return( fmmdl->move_bit );
}

//--------------------------------------------------------------
/**
 * MMDL ����r�b�g�@ON
 * @param	fmmdl	MMDL *
 * @param	bit		���Ă�r�b�g MMDL_MOVEBIT_NON��
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_OnMoveBit( MMDL * fmmdl, MMDL_MOVEBIT bit )
{
	fmmdl->move_bit |= bit;
}

//--------------------------------------------------------------
/**
 * MMDL ����r�b�g�@OFF
 * @param	fmmdl	MMDL *
 * @param	bit		���낷�r�b�g MMDL_MOVEBIT_NON��
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_OffMoveBit( MMDL * fmmdl, MMDL_MOVEBIT bit )
{
	fmmdl->move_bit &= ~bit;
}

//--------------------------------------------------------------
/**
 * MMDL ����r�b�g�@�`�F�b�N
 * @param	fmmdl	MMDL *
 * @param	bit		�`�F�b�N����r�b�g MMDL_MOVEBIT_NON��
 * @retval	nothing
 */
//--------------------------------------------------------------
u32 MMDL_CheckMoveBit( const MMDL * fmmdl, MMDL_MOVEBIT bit )
{
	return( (fmmdl->move_bit & bit) );
}

//--------------------------------------------------------------
/**
 * MMDL OBJ ID�Z�b�g
 * @param	fmmdl	MMDL * 
 * @param	id		obj id
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetOBJID( MMDL * fmmdl, u16 obj_id )
{
	fmmdl->obj_id = obj_id;
}

//--------------------------------------------------------------
/**
 * MMDL OBJ ID�擾
 * @param	fmmdl	MMDL *
 * @retval	u16		OBJ ID
 */
//--------------------------------------------------------------
u16 MMDL_GetOBJID( const MMDL * fmmdl )
{
	return( fmmdl->obj_id );
}

//--------------------------------------------------------------
/**
 * MMDL ZONE ID�Z�b�g
 * @param	fmmdl	MMDL *
 * @param	zone_id	�]�[��ID
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetZoneID( MMDL * fmmdl, u16 zone_id )
{
	fmmdl->zone_id = zone_id;
}

//--------------------------------------------------------------
/**
 * MMDL ZONE ID�擾
 * @param	fmmdl	MMDL *
 * @retval	int		ZONE ID
 */
//--------------------------------------------------------------
u16 MMDL_GetZoneID( const MMDL * fmmdl )
{
	return( fmmdl->zone_id );
}

//--------------------------------------------------------------
/**
 * MMDL OBJ�R�[�h�Z�b�g
 * @param	fmmdl			MMDL * 
 * @param	code			�Z�b�g����R�[�h
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetOBJCode( MMDL * fmmdl, u16 code )
{
	fmmdl->obj_code = code;
}

//--------------------------------------------------------------
/**
 * MMDL OBJ�R�[�h�擾
 * @param	fmmdl			MMDL * 
 * @retval	u16				OBJ�R�[�h
 */
//--------------------------------------------------------------
u16 MMDL_GetOBJCode( const MMDL * fmmdl )
{
	return( fmmdl->obj_code );
}

//--------------------------------------------------------------
/**
 * MMDL ����R�[�h�Z�b�g
 * @param	fmmdl			MMDL * 
 * @param	code			����R�[�h
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetMoveCode( MMDL * fmmdl, u16 code )
{
	fmmdl->move_code = code;
}

//--------------------------------------------------------------
/**
 * MMDL ����R�[�h�擾
 * @param	fmmdl			MMDL * 
 * @retval	u32				����R�[�h
 */
//--------------------------------------------------------------
u16 MMDL_GetMoveCode( const MMDL * fmmdl )
{
	return( fmmdl->move_code );
}

//--------------------------------------------------------------
/**
 * MMDL �C�x���g�^�C�v�Z�b�g
 * @param	fmmdl		MMDL * 
 * @param	type		Event Type
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetEventType( MMDL * fmmdl, u16 type )
{
	fmmdl->event_type = type;
}

//--------------------------------------------------------------
/**
 * MMDL �C�x���g�^�C�v�擾
 * @param	fmmdl		MMDL * 
 * @retval	u32			Event Type
 */
//--------------------------------------------------------------
u16 MMDL_GetEventType( const MMDL * fmmdl )
{
	return( fmmdl->event_type );
}

//--------------------------------------------------------------
/**
 * MMDL �C�x���g�t���O�Z�b�g
 * @param	fmmdl		MMDL * 
 * @param	flag		Event Flag
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetEventFlag( MMDL * fmmdl, u16 flag )
{
	fmmdl->event_flag = flag;
}

//--------------------------------------------------------------
/**
 * MMDL �C�x���g�t���O�擾
 * @param	fmmdl		MMDL * 
 * @retval	u16			Event Flag
 */
//--------------------------------------------------------------
u16 MMDL_GetEventFlag( const MMDL * fmmdl )
{
	return( fmmdl->event_flag );
}

//--------------------------------------------------------------
/**
 * MMDL �C�x���gID�Z�b�g
 * @param	fmmdl		MMDL *
 * @param	id			Event ID
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetEventID( MMDL * fmmdl, u16 id )
{
	fmmdl->event_id = id;
}

//--------------------------------------------------------------
/**
 * MMDL �C�x���gID�擾
 * @param	fmmdl		MMDL * 
 * @retval	u16			Event ID
 */
//--------------------------------------------------------------
u16 MMDL_GetEventID( const MMDL * fmmdl )
{
	return( fmmdl->event_id );
}

//--------------------------------------------------------------
/**
 * MMDL �C�x���gID���G�C���A�X���ǂ����`�F�b�N
 * @param	fmmdl		MMDL * 
 * @retval	int			TRUE=�G�C���A�XID�ł��� FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MMDL_CheckAliesEventID( const MMDL * fmmdl )
{
	u16 id = (u16)MMDL_GetEventID( fmmdl );
	
	if( id == SP_SCRID_ALIES ){
		return( TRUE );
	}

	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �w�b�_�[�w������擾
 * @param	fmmdl		MMDL * 
 * @retval	u32			DIR_UP��
 */
//--------------------------------------------------------------
u32 MMDL_GetDirHeader( const MMDL * fmmdl )
{
	return( fmmdl->dir_head );
}

//--------------------------------------------------------------
/**
 * MMDL �\�������Z�b�g�@����
 * @param	fmmdl			MMDL * 
 * @param	dir				DIR_UP��
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetForceDirDisp( MMDL * fmmdl, u16 dir )
{
	fmmdl->dir_disp_old = fmmdl->dir_disp;
	fmmdl->dir_disp = dir;
}

//--------------------------------------------------------------
/**
 * MMDL �\�������Z�b�g
 * @param	fmmdl			MMDL * 
 * @param	dir				DIR_UP��
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetDirDisp( MMDL * fmmdl, u16 dir )
{
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_PAUSE_DIR) == 0 ){
		fmmdl->dir_disp_old = fmmdl->dir_disp;
		fmmdl->dir_disp = dir;
	}
}

//--------------------------------------------------------------
/**
 * MMDL �\�������擾
 * @param	fmmdl	MMDL * 
 * @retval	u16 	DIR_UP��
 */
//--------------------------------------------------------------
u16 MMDL_GetDirDisp( const MMDL * fmmdl )
{
	return( fmmdl->dir_disp );
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ��\�������擾
 * @param	fmmdl			MMDL * 
 * @retval	dir				DIR_UP��
 */
//--------------------------------------------------------------
u16 MMDL_GetDirDispOld( const MMDL * fmmdl )
{
	return( fmmdl->dir_disp_old );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ������Z�b�g
 * @param	fmmdl			MMDL * 
 * @param	dir				DIR_UP��
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetDirMove( MMDL * fmmdl, u16 dir )
{
	fmmdl->dir_move_old = fmmdl->dir_move;
	fmmdl->dir_move = dir;
}

//--------------------------------------------------------------
/**
 * MMDL �ړ������擾
 * @param	fmmdl			MMDL * 
 * @retval	u16		DIR_UP��
 */
//--------------------------------------------------------------
u16 MMDL_GetDirMove( const MMDL * fmmdl )
{
	return( fmmdl->dir_move );
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ��ړ������擾
 * @param	fmmdl			MMDL * 
 * @retval	u16	DIR_UP��
 */
//--------------------------------------------------------------
u16 MMDL_GetDirMoveOld( const MMDL * fmmdl )
{
	return( fmmdl->dir_move_old );
}

//--------------------------------------------------------------
/**
 * MMDL �\���A�ړ������Z�b�g
 * @param	fmmdl			MMDL * 
 * @param	dir				DIR_UP��
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetDirAll( MMDL * fmmdl, u16 dir )
{
	MMDL_SetDirDisp( fmmdl, dir );
	MMDL_SetDirMove( fmmdl, dir );
}

//--------------------------------------------------------------
/**
 * MMDL �w�b�_�[�w��p�����^�Z�b�g
 * @param	fmmdl	MMDL * 
 * @param	param	�p�����^
 * @param	no		�Z�b�g����p�����^�ԍ��@MMDL_PARAM_0��
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetParam( MMDL *fmmdl, u16 param, MMDL_H_PARAM no )
{
	switch( no ){
	case MMDL_PARAM_0: fmmdl->param0 = param; break;
	case MMDL_PARAM_1: fmmdl->param1 = param; break;
	case MMDL_PARAM_2: fmmdl->param2 = param; break;
	default: GF_ASSERT( 0 );
	}
}

//--------------------------------------------------------------
/**
 * MMDL �w�b�_�[�w��p�����^�擾
 * @param	fmmdl			MMDL *
 * @param	param			MMDL_PARAM_0��
 * @retval	u16 �p�����^
 */
//--------------------------------------------------------------
u16 MMDL_GetParam( const MMDL * fmmdl, MMDL_H_PARAM param )
{
	switch( param ){
	case MMDL_PARAM_0: return( fmmdl->param0 );
	case MMDL_PARAM_1: return( fmmdl->param1 );
	case MMDL_PARAM_2: return( fmmdl->param2 );
	}
	
	GF_ASSERT( 0 );
	return( 0 );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ�����X�Z�b�g
 * @param	fmmdl			MMDL * 
 * @param	x				�ړ�����
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetMoveLimitX( MMDL * fmmdl, s16 x )
{
	fmmdl->move_limit_x = x;
}

//--------------------------------------------------------------
/**
 * MMDL �ړ�����X�擾
 * @param	fmmdl		MMDL * 
 * @retval	s16			�ړ�����X
 */
//--------------------------------------------------------------
s16 MMDL_GetMoveLimitX( const MMDL * fmmdl )
{
	return( fmmdl->move_limit_x );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ�����Z�Z�b�g
 * @param	fmmdl			MMDL * 
 * @param	z				�ړ�����
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetMoveLimitZ( MMDL * fmmdl, s16 z )
{
	fmmdl->move_limit_z = z;
}

//--------------------------------------------------------------
/**
 * MMDL �ړ�����Z�擾
 * @param	fmmdl		MMDL * 
 * @retval	s16		�ړ�����z
 */
//--------------------------------------------------------------
s16 MMDL_GetMoveLimitZ( const MMDL * fmmdl )
{
	return( fmmdl->move_limit_z );
}

//--------------------------------------------------------------
/**
 * MMDL �`��X�e�[�^�X�Z�b�g
 * @param	fmmdl		MMDL * 
 * @param	st			DRAW_STA_STOP��
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetDrawStatus( MMDL * fmmdl, u16 st )
{
	fmmdl->draw_status = st;
}

//--------------------------------------------------------------
/**
 * MMDL �`��X�e�[�^�X�擾
 * @param	fmmdl		MMDL * 
 * @retval	u16			DRAW_STA_STOP��
 */
//--------------------------------------------------------------
u16 MMDL_GetDrawStatus( const MMDL * fmmdl )
{
	return( fmmdl->draw_status );
}

//--------------------------------------------------------------
/**
 * MMDL MMDLSYS *�擾
 * @param	fmmdl			MMDL * 
 * @retval	MMDLSYS	MMDLSYS *
 */
//--------------------------------------------------------------
const MMDLSYS * MMDL_GetMMdlSys( const MMDL *fmmdl )
{
	return( fmmdl->pMMdlSys );
}

//--------------------------------------------------------------
/**
 * MMDL ����֐��p���[�N�������B
 * size�����[�N�T�C�Y�𒴂��Ă����ꍇ�AASSERT�B
 * ����p���[�N��size���A0�ŏ������B
 * @param	fmmdl	MMDL * 
 * @param	size	�K�v�ȃ��[�N�T�C�Y
 * @retval	void*	���������ꂽ���[�N
 */
//--------------------------------------------------------------
void * MMDL_InitMoveProcWork( MMDL * fmmdl, int size )
{
	void *work;
	GF_ASSERT( size <= MMDL_MOVE_WORK_SIZE );
	work = MMDL_GetMoveProcWork( fmmdl );
	GFL_STD_MemClear( work, size );
	return( work );
}

//--------------------------------------------------------------
/**
 * MMDL ����֐��p���[�N�擾�B
 * @param	fmmdl	MMDL * 
 * @retval	void*	���[�N
 */
//--------------------------------------------------------------
void * MMDL_GetMoveProcWork( MMDL * fmmdl )
{
	return( fmmdl->move_proc_work );
}

//--------------------------------------------------------------
/**
 * MMDL ����⏕�֐��p���[�N�������B
 * size�����[�N�T�C�Y�𒴂��Ă����ꍇ�AASSERT�B
 * @param	fmmdl	MMDL * 
 * @param	size	�K�v�ȃ��[�N�T�C�Y
 * @retval	void*	���������ꂽ���[�N
 */
//--------------------------------------------------------------
void * MMDL_InitMoveSubProcWork( MMDL * fmmdl, int size )
{
	u8 *work;
	
	GF_ASSERT( size <= MMDL_MOVE_SUB_WORK_SIZE );
	work = MMDL_GetMoveSubProcWork( fmmdl );
	GFL_STD_MemClear( work, size );
	return( work );
}

//--------------------------------------------------------------
/**
 * MMDL ����⏕�֐��p���[�N�擾
 * @param	fmmdl	MMDL * 
 * @retval	void*	���[�N*
 */
//--------------------------------------------------------------
void * MMDL_GetMoveSubProcWork( MMDL * fmmdl )
{
	return( fmmdl->move_sub_proc_work );
}

//--------------------------------------------------------------
/**
 * MMDL ����R�}���h�p���[�N�������B
 * size�����[�N�T�C�Y�𒴂��Ă����ꍇ�AASSERT�B
 * @param	fmmdl	MMDL * 
 * @param	size	�K�v�ȃ��[�N�T�C�Y
 * @retval	void*	���������ꂽ���[�N
 */
//--------------------------------------------------------------
void * MMDL_InitMoveCmdWork( MMDL * fmmdl, int size )
{
	u8 *work;
	
	GF_ASSERT( size <= MMDL_MOVE_CMD_WORK_SIZE );
	work = MMDL_GetMoveCmdWork( fmmdl );
	GFL_STD_MemClear( work, size );
	return( work );
}

//--------------------------------------------------------------
/**
 * MMDL ����R�}���h�p���[�N�擾
 * @param	fmmdl	MMDL *
 * @retval	void*	���[�N*
 */
//--------------------------------------------------------------
void * MMDL_GetMoveCmdWork( MMDL * fmmdl )
{
	return( fmmdl->move_cmd_proc_work );
}

//--------------------------------------------------------------
/**
 * MMDL �`��֐��p���[�N�������B
 * size�����[�N�T�C�Y�𒴂��Ă����ꍇ�AASSERT�B
 * @param	fmmdl	MMDL * 
 * @param	size	�K�v�ȃ��[�N�T�C�Y
 * @retval	void*	���������ꂽ���[�N
 */
//--------------------------------------------------------------
void * MMDL_InitDrawProcWork( MMDL * fmmdl, int size )
{
	u8 *work;
	
	GF_ASSERT( size <= MMDL_DRAW_WORK_SIZE );
	work = MMDL_GetDrawProcWork( fmmdl );
	GFL_STD_MemClear( work, size );
	return( work );
}

//--------------------------------------------------------------
/**
 * MMDL �`��֐��p���[�N�擾
 * @param	fmmdl	MMDL * 
 * @retval	void*	���[�N
 */
//--------------------------------------------------------------
void * MMDL_GetDrawProcWork( MMDL * fmmdl )
{
	return( fmmdl->draw_proc_work );
}

//--------------------------------------------------------------
/**
 * MMDL ���쏉�����֐����s
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_CallMoveInitProc( MMDL * fmmdl )
{
	GF_ASSERT( fmmdl->move_proc_list );
	GF_ASSERT( fmmdl->move_proc_list->init_proc );
	fmmdl->move_proc_list->init_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * MMDL ����֐����s
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_CallMoveProc( MMDL * fmmdl )
{
	GF_ASSERT( fmmdl->move_proc_list );
	GF_ASSERT( fmmdl->move_proc_list->move_proc );
	fmmdl->move_proc_list->move_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * MMDL �폜�֐����s
 * @param	fmmdl	MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_CallMoveDeleteProc( MMDL * fmmdl )
{
	GF_ASSERT( fmmdl->move_proc_list );
	GF_ASSERT( fmmdl->move_proc_list->delete_proc );
	fmmdl->move_proc_list->delete_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * MMDL ���A�֐����s
 * @param	fmmdl	MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_CallMovePopProc( MMDL * fmmdl )
{
	GF_ASSERT( fmmdl->move_proc_list );
	GF_ASSERT( fmmdl->move_proc_list->recover_proc );
	fmmdl->move_proc_list->recover_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * MMDL �`�揉�����֐����s
 * @param	fmmdl	MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_CallDrawInitProc( MMDL * fmmdl )
{
	GF_ASSERT( fmmdl->draw_proc_list );
	GF_ASSERT( fmmdl->draw_proc_list->init_proc );
	fmmdl->draw_proc_list->init_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * MMDL �`��֐����s
 * @param	fmmdl	MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_CallDrawProc( MMDL * fmmdl )
{
	GF_ASSERT( fmmdl->draw_proc_list );
	GF_ASSERT( fmmdl->draw_proc_list->draw_proc );
	fmmdl->draw_proc_list->draw_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * MMDL �`��폜�֐����s
 * @param	fmmdl	MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_CallDrawDeleteProc( MMDL * fmmdl )
{
	GF_ASSERT( fmmdl->draw_proc_list );
	GF_ASSERT( fmmdl->draw_proc_list->delete_proc );
	fmmdl->draw_proc_list->delete_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * MMDL �`��ޔ��֐����s
 * @param	fmmdl	MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_CallDrawPushProc( MMDL * fmmdl )
{
	GF_ASSERT( fmmdl->draw_proc_list );
	GF_ASSERT( fmmdl->draw_proc_list->push_proc );
	fmmdl->draw_proc_list->push_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * MMDL �`�敜�A�֐����s
 * @param	fmmdl	MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_CallDrawPopProc( MMDL * fmmdl )
{
	GF_ASSERT( fmmdl->draw_proc_list );
	GF_ASSERT( fmmdl->draw_proc_list->pop_proc );
	fmmdl->draw_proc_list->pop_proc( fmmdl );
}

//--------------------------------------------------------------
/**
 * MMDL �`��擾�֐����s
 * @param	fmmdl	MMDL*
 * @param	state	�擾�֐��ɗ^������
 * @retval	nothing
 */
//--------------------------------------------------------------
u32 MMDL_CallDrawGetProc( MMDL *fmmdl, u32 state )
{
	GF_ASSERT( fmmdl->draw_proc_list );
	GF_ASSERT( fmmdl->draw_proc_list->get_proc );
	return( fmmdl->draw_proc_list->get_proc(fmmdl,state) );
}

//--------------------------------------------------------------
/**
 * MMDL �A�j���[�V�����R�}���h�R�[�h�Z�b�g
 * @param	fmmdl	MMDL * 
 * @param	code	AC_DIR_U��
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetAcmdCode( MMDL * fmmdl, u16 code )
{
	fmmdl->acmd_code = code;
}

//--------------------------------------------------------------
/**
 * MMDL �A�j���[�V�����R�}���h�R�[�h�擾
 * @param	fmmdl	MMDL * 
 * @retval	u16	AC_DIR_U��
 */
//--------------------------------------------------------------
u16 MMDL_GetAcmdCode( const MMDL * fmmdl )
{
	return( fmmdl->acmd_code );
}

//--------------------------------------------------------------
/**
 * MMDL �A�j���[�V�����R�}���h�V�[�P���X�Z�b�g
 * @param	fmmdl	MMDL * 
 * @param	no		�V�[�P���X
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetAcmdSeq( MMDL * fmmdl, u16 no )
{
	fmmdl->acmd_seq = no;
}

//--------------------------------------------------------------
/**
 * MMDL �A�j���[�V�����R�}���h�V�[�P���X����
 * @param	fmmdl	MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_IncAcmdSeq( MMDL * fmmdl )
{
	fmmdl->acmd_seq++;
}

//--------------------------------------------------------------
/**
 * MMDL �A�j���[�V�����R�}���h�V�[�P���X�擾
 * @param	fmmdl	MMDL * 
 * @retval	u16 �V�[�P���X
 */
//--------------------------------------------------------------
u16 MMDL_GetAcmdSeq( const MMDL * fmmdl )
{
	return( fmmdl->acmd_seq );
}

//--------------------------------------------------------------
/**
 * MMDL ���݂̃}�b�v�A�g���r���[�g���Z�b�g
 * @param	fmmdl	MMDL *
 * @param	attr	�Z�b�g����A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetMapAttr( MMDL * fmmdl, u32 attr )
{
	fmmdl->now_attr = attr;
}

//--------------------------------------------------------------
/**
 * MMDL ���݂̃}�b�v�A�g���r���[�g���擾
 * @param	fmmdl	MMDL *
 * @retval	u32		�}�b�v�A�g���r���[�g
 */
//--------------------------------------------------------------
u32 MMDL_GetMapAttr( const MMDL * fmmdl )
{
	return( fmmdl->now_attr );
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ��̃}�b�v�A�g���r���[�g���Z�b�g
 * @param	fmmdl	MMDL *
 * @param	attr	�Z�b�g����A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetMapAttrOld( MMDL * fmmdl, u32 attr )
{
	fmmdl->old_attr = attr;
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ��̃}�b�v�A�g���r���[�g���擾
 * @param	fmmdl	MMDL *
 * @retval	u32		�}�b�v�A�g���r���[�g
 */
//--------------------------------------------------------------
u32 MMDL_GetMapAttrOld( const MMDL * fmmdl )
{
	return( fmmdl->old_attr );
}

//--------------------------------------------------------------
/**
 * MMDL �G�C���A�X���̃]�[��ID�擾�B
 * ���G�C���A�X���̓C�x���g�t���O���w��]�[��ID
 * @param	fmmdl	MMDL *
 * @retval	u16 �]�[��ID
 */
//--------------------------------------------------------------
u16 MMDL_GetZoneIDAlies( const MMDL * fmmdl )
{
	GF_ASSERT( MMDL_CheckStatusBitAlies(fmmdl) == TRUE );
	return( MMDL_GetEventFlag(fmmdl) );
}

//--------------------------------------------------------------
/**
 * MMDL �������W �O���b�hX���W�擾
 * @param	fmmdl	MMDL * 
 * @retval	s16 X���W
 */
//--------------------------------------------------------------
s16 MMDL_GetInitGridPosX( const MMDL * fmmdl )
{
	return( fmmdl->gx_init );
}

//--------------------------------------------------------------
/**
 * MMDL �������W �O���b�hX���W�Z�b�g
 * @param	fmmdl	MMDL * 
 * @param	x		�Z�b�g������W
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetInitGridPosX( MMDL * fmmdl, s16 x )
{
	fmmdl->gx_init = x;
}

//--------------------------------------------------------------
/**
 * MMDL �������W Y���W�擾
 * @param	fmmdl	MMDL * 
 * @retval	s16		Y
 */
//--------------------------------------------------------------
s16 MMDL_GetInitGridPosY( const MMDL * fmmdl )
{
	return( fmmdl->gy_init );
}

//--------------------------------------------------------------
/**
 * MMDL �������W Y���W�Z�b�g
 * @param	fmmdl	MMDL * 
 * @param	y		�Z�b�g������W
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetInitGridPosY( MMDL * fmmdl, s16 y )
{
	fmmdl->gy_init = y;
}

//--------------------------------------------------------------
/**
 * MMDL �������W z���W�擾
 * @param	fmmdl	MMDL * 
 * @retval	s16		z���W
 */
//--------------------------------------------------------------
s16 MMDL_GetInitGridPosZ( const MMDL * fmmdl )
{
	return( fmmdl->gz_init );
}

//--------------------------------------------------------------
/**
 * MMDL �������W z���W�Z�b�g
 * @param	fmmdl	MMDL * 
 * @param	z		�Z�b�g������W
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetInitGridPosZ( MMDL * fmmdl, s16 z )
{
	fmmdl->gz_init = z;
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ����W�@X���W�擾
 * @param	fmmdl	MMDL * 
 * @retval	s16		X���W
 */
//--------------------------------------------------------------
s16 MMDL_GetOldGridPosX( const MMDL * fmmdl )
{
	return( fmmdl->gx_old );
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ����W X���W�Z�b�g
 * @param	fmmdl	MMDL * 
 * @param	x		�Z�b�g������W
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetOldGridPosX( MMDL * fmmdl, s16 x )
{
	fmmdl->gx_old = x;
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ����W Y���W�擾
 * @param	fmmdl	MMDL * 
 * @retval	s16		Y
 */
//--------------------------------------------------------------
s16 MMDL_GetOldGridPosY( const MMDL * fmmdl )
{
	return( fmmdl->gy_old );
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ����W Y���W�Z�b�g
 * @param	fmmdl	MMDL * 
 * @param	y		�Z�b�g������W
 * @retval	s16		y���W
 */
//--------------------------------------------------------------
void MMDL_SetOldGridPosY( MMDL * fmmdl, s16 y )
{
	fmmdl->gy_old = y;
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ����W z���W�擾
 * @param	fmmdl	MMDL * 
 * @retval	s16		z���W
 */
//--------------------------------------------------------------
s16 MMDL_GetOldGridPosZ( const MMDL * fmmdl )
{
	return( fmmdl->gz_old );
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ����W z���W�Z�b�g
 * @param	fmmdl	MMDL * 
 * @param	z		�Z�b�g������W
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetOldGridPosZ( MMDL * fmmdl, s16 z )
{
	fmmdl->gz_old = z;
}

//--------------------------------------------------------------
/**
 * MMDL ���ݍ��W X���W�擾
 * @param	fmmdl	MMDL * 
 * @retval	s16		X���W
 */
//--------------------------------------------------------------
s16 MMDL_GetGridPosX( const MMDL * fmmdl )
{
	return( fmmdl->gx_now );
}

//--------------------------------------------------------------
/**
 * MMDL ���ݍ��W X���W�Z�b�g
 * @param	fmmdl	MMDL * 
 * @param	x		�Z�b�g������W
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetGridPosX( MMDL * fmmdl, s16 x )
{
	fmmdl->gx_now = x;
}

//--------------------------------------------------------------
/**
 * MMDL ���ݍ��W X���W����
 * @param	fmmdl	MMDL * 
 * @param	x		�����l
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_AddGridPosX( MMDL * fmmdl, s16 x )
{
	fmmdl->gx_now += x;
}

//--------------------------------------------------------------
/**
 * MMDL ���ݍ��W Y���W�擾
 * @param	fmmdl	MMDL * 
 * @retval	s16		Y
 */
//--------------------------------------------------------------
s16 MMDL_GetGridPosY( const MMDL * fmmdl )
{
	return( fmmdl->gy_now );
}

//--------------------------------------------------------------
/**
 * MMDL ���ݍ��W Y���W�Z�b�g
 * @param	fmmdl	MMDL * 
 * @param	y		�Z�b�g������W
 * @retval	s16		y���W
 */
//--------------------------------------------------------------
void MMDL_SetGridPosY( MMDL * fmmdl, s16 y )
{
	fmmdl->gy_now = y;
}

//--------------------------------------------------------------
/**
 * MMDL ���ݍ��W Y���W����
 * @param	fmmdl	MMDL * 
 * @param	y		�����l
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_AddGridPosY( MMDL * fmmdl, s16 y )
{
	fmmdl->gy_now += y;
}

//--------------------------------------------------------------
/**
 * MMDL �ߋ����W z���W�擾
 * @param	fmmdl	MMDL * 
 * @retval	s16		z���W
 */
//--------------------------------------------------------------
s16 MMDL_GetGridPosZ( const MMDL * fmmdl )
{
	return( fmmdl->gz_now );
}

//--------------------------------------------------------------
/**
 * MMDL ���ݍ��W z���W�Z�b�g
 * @param	fmmdl	MMDL * 
 * @param	z		�Z�b�g������W
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetGridPosZ( MMDL * fmmdl, s16 z )
{
	fmmdl->gz_now = z;
}

//--------------------------------------------------------------
/**
 * MMDL ���ݍ��W z���W����
 * @param	fmmdl	MMDL * 
 * @param	z		�����l
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_AddGridPosZ( MMDL * fmmdl, s16 z )
{
	fmmdl->gz_now += z;
}

//--------------------------------------------------------------
/**
 * MMDL �����W�|�C���^�擾
 * @param	fmmdl	MMDL * 
 * @retval VecFx32*
 */
//--------------------------------------------------------------
const VecFx32 * MMDL_GetVectorPosAddress( const MMDL * fmmdl )
{
  return( &fmmdl->vec_pos_now );
}

//--------------------------------------------------------------
/**
 * MMDL �����W�擾
 * @param	fmmdl	MMDL * 
 * @param	vec		���W�i�[��
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_GetVectorPos( const MMDL * fmmdl, VecFx32 *vec )
{
	*vec = fmmdl->vec_pos_now;
}

//--------------------------------------------------------------
/**
 * MMDL �����W�Z�b�g
 * @param	fmmdl	MMDL * 
 * @param	vec		�Z�b�g���W
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetVectorPos( MMDL * fmmdl, const VecFx32 *vec )
{
	fmmdl->vec_pos_now = *vec;
}

//--------------------------------------------------------------
/**
 * MMDL �����WY�l�擾
 * @param	fmmdl	MMDL * 
 * @retval	fx32	����
 */
//--------------------------------------------------------------
fx32 MMDL_GetVectorPosY( const MMDL * fmmdl )
{
	return( fmmdl->vec_pos_now.y );
}

//--------------------------------------------------------------
/**
 * MMDL �\�����W�I�t�Z�b�g�擾
 * @param	fmmdl	MMDL * 
 * @param	vec		�Z�b�g���W
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_GetVectorDrawOffsetPos( const MMDL * fmmdl, VecFx32 *vec )
{
	*vec = fmmdl->vec_draw_offs;
}

//--------------------------------------------------------------
/**
 * MMDL �\�����W�I�t�Z�b�g�Z�b�g
 * @param	fmmdl	MMDL * 
 * @param	vec		�Z�b�g���W
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetVectorDrawOffsetPos( MMDL * fmmdl, const VecFx32 *vec )
{
	fmmdl->vec_draw_offs = *vec;
}

//--------------------------------------------------------------
/**
 * MMDL �O���w��\�����W�I�t�Z�b�g�擾
 * @param	fmmdl	MMDL * 
 * @param	vec		�Z�b�g���W
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_GetVectorOuterDrawOffsetPos( const MMDL * fmmdl, VecFx32 *vec )
{
	*vec = fmmdl->vec_draw_offs_outside;
}

//--------------------------------------------------------------
/**
 * MMDL �O���w��\�����W�I�t�Z�b�g�Z�b�g
 * @param	fmmdl	MMDL * 
 * @param	vec		�Z�b�g���W
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetVectorOuterDrawOffsetPos( MMDL * fmmdl, const VecFx32 *vec )
{
	fmmdl->vec_draw_offs_outside = *vec;
}

//--------------------------------------------------------------
/**
 * MMDL �A�g���r���[�g�ω����W�I�t�Z�b�g�擾
 * @param	fmmdl	MMDL * 
 * @param	vec		�Z�b�g���W
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_GetVectorAttrDrawOffsetPos( const MMDL * fmmdl, VecFx32 *vec )
{
	*vec = fmmdl->vec_attr_offs;
}

//--------------------------------------------------------------
/**
 * MMDL �A�g���r���[�g�ω����W�I�t�Z�b�g�Z�b�g
 * @param	fmmdl	MMDL * 
 * @param	vec		�Z�b�g���W
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetVectorAttrDrawOffsetPos( MMDL * fmmdl, const VecFx32 *vec )
{
	fmmdl->vec_attr_offs = *vec;
}

//--------------------------------------------------------------
/**
 * MMDL ����(�O���b�h�P��)���擾
 * @param	fmmdl	MMDL *
 * @retval	s16		�����BH_GRID�P��
 */
//--------------------------------------------------------------
s16 MMDL_GetHeightGrid( const MMDL * fmmdl )
{
	fx32 y = MMDL_GetVectorPosY( fmmdl );
	s16 gy = SIZE_GRID_FX32( y );
	return( gy );
}

//--------------------------------------------------------------
/**
 * MMDL MMDL_BLACTCONT���擾
 * @param	fmmdl	MMDL *
 * @retval	MMDL_BLACTCONT*
 */
//--------------------------------------------------------------
MMDL_BLACTCONT * MMDL_GetBlActCont( MMDL *fmmdl )
{
	return( MMDLSYS_GetBlActCont((MMDLSYS*)MMDL_GetMMdlSys(fmmdl)) );
}

//======================================================================
//	MMDLSYS �X�e�[�^�X����
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS �`�揈��������������Ă��邩�ǂ����`�F�b�N
 * @param	fmmdlsys	MMDLSYS *
 * @retval	BOOL	TRUE=����������Ă���
 */
//--------------------------------------------------------------
BOOL MMDLSYS_CheckCompleteDrawInit( const MMDLSYS *fmmdlsys )
{
	if( MMDLSYS_CheckStatusBit(
			fmmdlsys,MMDLSYS_STABIT_DRAW_INIT_COMP) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDLSYS �`�揈�������������Z�b�g
 * @param	fmmdlsys	MMDLSYS*
 * @param	flag	TRUE=����������
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDLSYS_SetCompleteDrawInit( MMDLSYS *fmmdlsys, BOOL flag )
{
	if( flag == TRUE ){
		MMdlSys_OnStatusBit( fmmdlsys, MMDLSYS_STABIT_DRAW_INIT_COMP );
	}else{
		MMdlSys_OffStatusBit( fmmdlsys, MMDLSYS_STABIT_DRAW_INIT_COMP );
	}
}

//--------------------------------------------------------------
/**
 * MMDLSYS �e��t����A�t���Ȃ��̃Z�b�g
 * @param	fmmdlsys MMDLSYS *
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
 * @param	fmmdlsys MMDLSYS *
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
 * @param	fmmdlsys	MMDLSYS*
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDLSYS_StopProc( MMDLSYS *fmmdlsys )
{
	MMdlSys_OnStatusBit( fmmdlsys,
		MMDLSYS_STABIT_MOVE_PROC_STOP|MMDLSYS_STABIT_DRAW_PROC_STOP );
}

//--------------------------------------------------------------
/**
 * MMDLSYS MMDLSYS_StopProc()�̉����B
 * @param	fmmdlsys	MMDLSYS*
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDLSYS_PlayProc( MMDLSYS *fmmdlsys )
{
	MMdlSys_OffStatusBit( fmmdlsys,
		MMDLSYS_STABIT_MOVE_PROC_STOP|MMDLSYS_STABIT_DRAW_PROC_STOP );
}

//--------------------------------------------------------------
/**
 * MMDLSYS �t�B�[���h���샂�f���S�̂̓�����ꎞ��~
 * �ŗL�̓��쏈���i�����_���ړ����j���ꎞ��~����B
 * �A�j���[�V�����R�}���h�ɂ͔�������B
 * @param	fmmdlsys	MMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDLSYS_PauseMoveProc( MMDLSYS *fmmdlsys )
{
	u32 no = 0;
	MMDL *fmmdl;
	
	while( MMDLSYS_SearchUseMMdl(fmmdlsys,&fmmdl,&no) == TRUE ){
		MMDL_OnStatusBitMoveProcPause( fmmdl );
	}
}

//--------------------------------------------------------------
/**
 * MMDLSYS �t�B�[���h���샂�f���S�̂̈ꎞ��~������
 * @param	fmmdlsys	MMDLSYS *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDLSYS_ClearPauseMoveProc( MMDLSYS *fmmdlsys )
{
	u32 no = 0;
	MMDL *fmmdl;

	while( MMDLSYS_SearchUseMMdl(fmmdlsys,&fmmdl,&no) == TRUE ){
		MMDL_OffStatusBitMoveProcPause( fmmdl );
	}
}

//======================================================================
//	MMDL �X�e�[�^�X�r�b�g�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f������
 * �t�B�[���h���샂�f���V�X�e���̃r�b�g�`�F�b�N
 * @param	fmmdl		MMDL*
 * @param	bit			MMDLSYS_STABIT_DRAW_INIT_COMP��
 * @retval	u32			0�ȊO bit�q�b�g
 */
//--------------------------------------------------------------
u32 MMDL_CheckMMdlSysStatusBit(
	const MMDL *fmmdl, MMDLSYS_STABIT bit )
{
	const MMDLSYS *fos = MMDL_GetMMdlSys( fmmdl );
	return( MMDLSYS_CheckStatusBit(fos,bit) );
}

//--------------------------------------------------------------
/**
 * MMDL �g�p�`�F�b�N
 * @param	fmmdl	MMDL *
 * @retval	BOOL	TRUE=�g�p��
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitUse( const MMDL *fmmdl )
{
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_USE) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ����쒆�ɂ���
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_OnStatusBitMove( MMDL *fmmdl )
{
	MMDL_OnStatusBit( fmmdl, MMDL_STABIT_MOVE );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ����쒆������
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_OffStatusBitMove( MMDL * fmmdl )
{
	MMDL_OffStatusBit( fmmdl, MMDL_STABIT_MOVE );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ����쒆�`�F�b�N
 * @param	fmmdl	MMDL *
 * @retval	BOOL TRUE=���쒆
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitMove( const MMDL *fmmdl )
{
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_MOVE) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ�����J�n�ɂ���
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_OnStatusBitMoveStart( MMDL * fmmdl )
{
	MMDL_OnStatusBit( fmmdl, MMDL_STABIT_MOVE_START );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ�����J�n������
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_OffStatusBitMoveStart( MMDL * fmmdl )
{
	MMDL_OffStatusBit( fmmdl, MMDL_STABIT_MOVE_START );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ�����J�n�`�F�b�N
 * @param	fmmdl	MMDL *
 * @retval	BOOL TRUE=�ړ�����J�n
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitMoveStart( const MMDL * fmmdl )
{
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_MOVE_START) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ�����I���ɂ���
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_OnStatusBitMoveEnd( MMDL * fmmdl )
{
	MMDL_OnStatusBit( fmmdl, MMDL_STABIT_MOVE_END );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ�����I��������
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_OffStatusBitMoveEnd( MMDL * fmmdl )
{
	MMDL_OffStatusBit( fmmdl, MMDL_STABIT_MOVE_END );
}

//--------------------------------------------------------------
/**
 * MMDL �ړ�����I���`�F�b�N
 * @param	fmmdl	MMDL *
 * @retval	BOOL TRUE=�ړ��I��
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitMoveEnd( const MMDL * fmmdl )
{
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_MOVE_END) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �`�揉���������ɂ���
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_OnStatusBitCompletedDrawInit( MMDL * fmmdl )
{
	MMDL_OnStatusBit( fmmdl, MMDL_STABIT_DRAW_PROC_INIT_COMP );
}

//--------------------------------------------------------------
/**
 * MMDL �`�揉��������������
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_OffStatusBitCompletedDrawInit( MMDL * fmmdl )
{
	MMDL_OffStatusBit( fmmdl, MMDL_STABIT_DRAW_PROC_INIT_COMP );
}

//--------------------------------------------------------------
/**
 * MMDL �`�揉���������`�F�b�N
 * @param	fmmdl	MMDL *
 * @retval	BOOL TRUE=�`�揉��������
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitCompletedDrawInit( const MMDL * fmmdl )
{
	if( MMDL_CheckStatusBit(
			fmmdl,MMDL_STABIT_DRAW_PROC_INIT_COMP) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL ��\���t���O���`�F�b�N
 * @param	fmmdl	MMDL *
 * @retval	BOOL TRUE=��\���@FALSE=�\��
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitVanish( const MMDL * fmmdl )
{
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_VANISH) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL ��\���t���O��ݒ�
 * @param	fmmdl	MMDL *
 * @param	flag	TRUE=��\���@FALSE=�\��
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitVanish( MMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		MMDL_OnStatusBit( fmmdl, MMDL_STABIT_VANISH );
	}else{
		MMDL_OffStatusBit( fmmdl, MMDL_STABIT_VANISH );
	}
}

//--------------------------------------------------------------
/**
 * MMDL OBJ���m�̓����蔻��t���O��ݒ�
 * @param	fmmdl	MMDL *
 * @param	flag	TRUE=�q�b�g�A���@FALSE=�q�b�g�i�V
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitFellowHit( MMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		MMDL_OffStatusBit( fmmdl, MMDL_STABIT_FELLOW_HIT_NON );
	}else{
		MMDL_OnStatusBit( fmmdl, MMDL_STABIT_FELLOW_HIT_NON );
	}
}

//--------------------------------------------------------------
/**
 * MMDL ���쒆�t���O�̃Z�b�g
 * @param	fmmdl	MMDL *
 * @param	flag	TRUE=���쒆�@FALSE=��~��
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitMove( MMDL * fmmdl, int flag )
{
	if( flag == TRUE ){
		MMDL_OnStatusBitMove( fmmdl );
	}else{
		MMDL_OffStatusBitMove( fmmdl );
	}
}

//--------------------------------------------------------------
/**
 * MMDL �b�������\�`�F�b�N
 * @param	fmmdl	MMDL *
 * @retval	BOOL TRUE=�\ FALSE=�s��
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitTalk( MMDL * fmmdl )
{
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_TALK_OFF) ){
		return( FALSE );
	}
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * MMDL �b�������s�t���O���Z�b�g
 * @param	fmmdl	MMDL *
 * @param	flag	TRUE=�s�� FALSE=�\
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitTalkOFF( MMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		MMDL_OnStatusBit( fmmdl, MMDL_STABIT_TALK_OFF );
	}else{
		MMDL_OffStatusBit( fmmdl, MMDL_STABIT_TALK_OFF );
	}
}

//--------------------------------------------------------------
/**
 * MMDL ����|�[�Y
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_OnStatusBitMoveProcPause( MMDL * fmmdl )
{
	MMDL_OnStatusBit( fmmdl, MMDL_STABIT_PAUSE_MOVE );
}

//--------------------------------------------------------------
/**
 * MMDL ����|�[�Y����
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_OffStatusBitMoveProcPause( MMDL * fmmdl )
{
	MMDL_OffStatusBit( fmmdl, MMDL_STABIT_PAUSE_MOVE );
}

//--------------------------------------------------------------
/**
 * MMDL ����|�[�Y�`�F�b�N
 * @param	fmmdl	MMDL *
 * @retval	BOOL TRUE=����|�[�Y
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitMoveProcPause( const MMDL * fmmdl )
{
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_PAUSE_MOVE) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �X�e�[�^�X�r�b�g �`�揈�������������`�F�b�N
 * @param	fmmdl		MMDL *
 * @retval	BOOL TRUE=�����BFALSE=�܂�
 */
//--------------------------------------------------------------
BOOL MMDL_CheckCompletedDrawInit( const MMDL * fmmdl )
{
	const MMDLSYS *fos;
	
	fos = MMDL_GetMMdlSys( fmmdl );
	
	if( MMDLSYS_CheckCompleteDrawInit(fos) == FALSE ){
		return( FALSE );
	}
	
	if( MMDL_CheckStatusBitCompletedDrawInit(fmmdl) ){
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �����擾���֎~����
 * @param	fmmdl	MMDL *
 * @param	int		TRUE=�����擾OFF FALSE=ON
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitHeightGetOFF( MMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		MMDL_OnStatusBit( fmmdl, MMDL_STABIT_HEIGHT_GET_OFF );
	}else{
		MMDL_OffStatusBit( fmmdl, MMDL_STABIT_HEIGHT_GET_OFF );
	}
}

//--------------------------------------------------------------
/**
 * MMDL �����擾���֎~����Ă��邩�`�F�b�N
 * @param	fmmdl	MMDL *
 * @retval	BOOL TRUE=�֎~
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitHeightGetOFF( const MMDL * fmmdl )
{
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_HEIGHT_GET_OFF) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �]�[���؂�ւ����̍폜�֎~
 * @param	fmmdl	MMDL *
 * @param	flag	TRUE=�֎~ FALSE=�֎~���Ȃ�
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitNotZoneDelete( MMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		MMDL_OnStatusBit( fmmdl, MMDL_STABIT_ZONE_DEL_NOT );
	}else{
		MMDL_OffStatusBit( fmmdl, MMDL_STABIT_ZONE_DEL_NOT );
	}
}

//--------------------------------------------------------------
/**
 * MMDL �G�C���A�X�t���O���Z�b�g
 * @param	fmmdl	MMDL *
 * @param	flag	TRUE=�G�C���A�X FALSE=�Ⴄ
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitAlies( MMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		MMDL_OnStatusBit( fmmdl, MMDL_STABIT_ALIES );
	}else{
		MMDL_OffStatusBit( fmmdl, MMDL_STABIT_ALIES );
	}
}

//--------------------------------------------------------------
/**
 * MMDL �G�C���A�X�t���O���`�F�b�N
 * @param	fmmdl	MMDL *
 * @retval	BOOL TRUE=�G�C���A�X FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitAlies( const MMDL * fmmdl )
{
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_ALIES) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �󐣃G�t�F�N�g�Z�b�g�t���O���Z�b�g
 * @param	fmmdl	MMDL *
 * @param	flag	TRUE=�Z�b�g�@FALSE=�N���A
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitShoalEffect( MMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		MMDL_OnStatusBit( fmmdl, MMDL_STABIT_EFFSET_SHOAL );
	}else{
		MMDL_OffStatusBit( fmmdl, MMDL_STABIT_EFFSET_SHOAL );
	}
}

//--------------------------------------------------------------
/**
 * MMDL �󐣃G�t�F�N�g�Z�b�g�t���O���`�F�b�N
 * @param	fmmdl	MMDL *
 * @retval	BOOL TRUE=�Z�b�g�@FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitShoalEffect( const MMDL * fmmdl )
{
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_EFFSET_SHOAL) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �A�g���r���[�g�I�t�Z�b�g�ݒ�OFF�Z�b�g
 * @param	fmmdl	MMDL *
 * @param	flag	TRUE=�Z�b�g�@FALSE=�N���A
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitAttrOffsetOFF( MMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		MMDL_OnStatusBit( fmmdl, MMDL_STABIT_ATTR_OFFS_OFF );
	}else{
		MMDL_OffStatusBit( fmmdl, MMDL_STABIT_ATTR_OFFS_OFF );
	}
}

//--------------------------------------------------------------
/**
 * MMDL �A�g���r���[�g�I�t�Z�b�g�ݒ�OFF�`�F�b�N
 * @param	fmmdl	MMDL *
 * @retval	BOOL TRUE=OFF�@FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitAttrOffsetOFF( const MMDL * fmmdl )
{
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_ATTR_OFFS_OFF) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL ���ړ��t���O�Z�b�g
 * @param	fmmdl	MMDL *
 * @param	flag	TRUE=�Z�b�g�@FALSE=�N���A
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitBridge( MMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		MMDL_OnStatusBit( fmmdl, MMDL_STABIT_BRIDGE );
	}else{
		MMDL_OffStatusBit( fmmdl, MMDL_STABIT_BRIDGE );
	}
}

//--------------------------------------------------------------
/**
 * MMDL ���ړ��t���O�`�F�b�N
 * @param	fmmdl	MMDL *
 * @retval	BOOL TRUE=���@FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitBridge( const MMDL * fmmdl )
{
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_BRIDGE) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �f�肱�݃t���O�Z�b�g
 * @param	fmmdl	MMDL *
 * @param	flag	TRUE=�Z�b�g�@FALSE=�N���A
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitReflect( MMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		MMDL_OnStatusBit( fmmdl, MMDL_STABIT_REFLECT_SET );
	}else{
		MMDL_OffStatusBit( fmmdl, MMDL_STABIT_REFLECT_SET );
	}
}

//--------------------------------------------------------------
/**
 * MMDL �f�肱�݃t���O�`�F�b�N
 * @param	fmmdl	MMDL *
 * @retval	BOOL TRUE=�Z�b�g�@FALSE=����
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitReflect( const MMDL * fmmdl )
{
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_REFLECT_SET) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �A�j���[�V�����R�}���h�`�F�b�N
 * @param	fmmdl	MMDL *
 * @retval	BOOL TRUE=�R�}���h�A���@FALSE=����
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitAcmd( const MMDL * fmmdl )
{
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_ACMD) ){
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * MMDL �g�����������t���O���Z�b�g
 * @param	fmmdl	MMDL *
 * @param	flag	TRUE=�Z�b�g�@FALSE=�N���A
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetStatusBitHeightExpand( MMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		MMDL_OnStatusBit( fmmdl, MMDL_STABIT_HEIGHT_EXPAND );
	}else{
		MMDL_OffStatusBit( fmmdl, MMDL_STABIT_HEIGHT_EXPAND );
	}
}

//--------------------------------------------------------------
/**
 * MMDL �g�����������t���O�`�F�b�N
 * @param	fmmdl	MMDL *
 * @retval	BOOL TRUE=�g�������ɔ�������@FALSE=����
 */
//--------------------------------------------------------------
BOOL MMDL_CheckStatusBitHeightExpand( const MMDL * fmmdl )
{
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_HEIGHT_EXPAND) ){
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
 * @param	fmmdl	MMDL *
 * @param	flag	TRUE=��~
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_SetMoveBitAttrGetOFF( MMDL * fmmdl, BOOL flag )
{
	if( flag == TRUE ){
		MMDL_OnMoveBit( fmmdl, MMDL_MOVEBIT_ATTR_GET_OFF );
	}else{
		MMDL_OffMoveBit( fmmdl, MMDL_MOVEBIT_ATTR_GET_OFF );
	}
}

//--------------------------------------------------------------
/**
 * MMDL �A�g���r���[�g�擾���~�@�`�F�b�N
 * @param	fmmdl	MMDL *
 * @retval	int	TRUE=��~
 */
//--------------------------------------------------------------
int MMDL_CheckMoveBitAttrGetOFF( const MMDL * fmmdl )
{
	if( MMDL_CheckMoveBit(fmmdl,MMDL_MOVEBIT_ATTR_GET_OFF) ){
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
 * @param	fmmdl	MMDL*�i�[��
 * @param	no	�����J�n���[�Nno�B�擪���猟������ۂ͏����l0���w��B
 * @retval BOOL TRUE=���샂�f���擾���� FALSE=no����I�[�܂Ō������擾����
 * @note ����no�͌Ăяo����A�擾�ʒu+1�̒l�ɂȂ�B
 * @note ����FOBJ ID 1�Ԃ̓��샂�f����T���B
 * u32 no=0;
 * MMDL *fmmdl;
 * while( MMDLSYS_SearchUseMMdl(fos,&fmmdl,&no) == TRUE ){
 * 		if( MMDL_GetOBJID(fmmdl) == 1 ){
 * 			break;
 * 		}
 * }
 */
//--------------------------------------------------------------
BOOL MMDLSYS_SearchUseMMdl(
	const MMDLSYS *fos, MMDL **fmmdl, u32 *no )
{
	u32 max = MMDLSYS_GetMMdlMax( fos );
	
	if( (*no) < max ){
		MMDL *check_obj = &(((MMDLSYS*)fos)->pMMdlBuf[*no]);
		
		do{
			(*no)++;
			if( MMDL_CheckStatusBit(check_obj,MMDL_STABIT_USE) ){
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
 * MMDLSYS �w�肳�ꂽ�O���b�hX,Z���W�ɂ���OBJ���擾
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
	u32 no = 0;
	MMDL *fmmdl;
	
	while( MMDLSYS_SearchUseMMdl(sys,&fmmdl,&no) == TRUE ){
		if( old_hit ){
			if( MMDL_GetOldGridPosX(fmmdl) == x &&
				MMDL_GetOldGridPosZ(fmmdl) == z ){
				return( fmmdl );
			}
		}
		
		if( MMDL_GetGridPosX(fmmdl) == x &&
			MMDL_GetGridPosZ(fmmdl) == z ){
			return( fmmdl );
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
	MMDL *fmmdl;
	
	while( MMDLSYS_SearchUseMMdl(fos,&fmmdl,&no) == TRUE ){
		if( MMDL_GetMoveCode(fmmdl) == mv_code ){
			return( fmmdl );
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
	MMDL *fmmdl;

	while( MMDLSYS_SearchUseMMdl(fos,&fmmdl,&no) == TRUE ){
		if( MMDL_CheckStatusBitAlies(fmmdl) == FALSE ){
			if( MMDL_GetOBJID(fmmdl) == id ){
				return( fmmdl );
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
	MMDL *fmmdl;
	
	i = 0;
	max = MMDLSYS_GetMMdlMax( sys );
	fmmdl = ((MMDLSYS*)sys)->pMMdlBuf;
	
	do{
		if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_USE) == 0 ){
			return( fmmdl );
		}
		
		fmmdl++;
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
	MMDL * fmmdl;
	
	while( MMDLSYS_SearchUseMMdl(fos,&fmmdl,&no) ){
		if( MMDL_CheckStatusBitAlies(fmmdl) == TRUE ){
			if( MMDL_GetOBJID(fmmdl) == obj_id ){
				if( MMDL_GetZoneIDAlies(fmmdl) == zone_id ){
					return( fmmdl );
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
	MMDL *fmmdl;
	
	while( MMDLSYS_SearchUseMMdl(fos,&fmmdl,&no) ){
		//�{���ł���΍X�ɃG�C���A�X�`�F�b�N������
		if( MMDL_CheckStatusBit(
				fmmdl,MMDL_STABIT_ZONE_DEL_NOT) == 0 ){
			if( MMDL_GetOBJID(fmmdl) == 0xff ){
				GF_ASSERT( 0 );
			}
			MMDL_Delete( fmmdl );
		}
	}
}

//======================================================================
//	MMDL �c�[��
//======================================================================
//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���@TCB����֐��ǉ�
 * @param	fmmdl	MMDL*
 * @param	sys		MMDLSYS*
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_AddTCB( MMDL *fmmdl, const MMDLSYS *sys )
{
	int pri,code;
	GFL_TCB * tcb;
	
	pri = MMDLSYS_GetTCBPriority( sys );
	code = MMDL_GetMoveCode( fmmdl );
	
	if( code == MV_PAIR || code == MV_TR_PAIR ){
		pri += MMDL_TCBPRI_OFFS_AFTER;
	}
	
	tcb = GFL_TCB_AddTask( MMDLSYS_GetTCBSYS((MMDLSYS*)sys),
			MMdl_TCB_MoveProc, fmmdl, pri );
	GF_ASSERT( tcb != NULL );
	
	fmmdl->pTCB = tcb;
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���@TCB����֐��폜
 * @param	fmmdl	MMDL*
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_DeleteTCB( MMDL *fmmdl )
{
	GF_ASSERT( fmmdl->pTCB );
	GFL_TCB_DeleteTask( fmmdl->pTCB );
	fmmdl->pTCB = NULL;
}

//--------------------------------------------------------------
/**
 * MMDL ���ݔ������Ă���t�B�[���h���샂�f����OBJ�R�[�h���Q��
 * @param	fmmdl	MMDL * 
 * @param	code	�`�F�b�N����R�[�h�BHERO��
 * @retval	BOOL	TRUE=fmmdl�ȊO�ɂ�code�������Ă���z������
 */
//--------------------------------------------------------------
BOOL MMDL_SearchUseOBJCode( const MMDL *fmmdl, u16 code )
{
	u32 no = 0;
	u16 check_code;
	MMDL *cmmdl;
	const MMDLSYS *fos = MMDL_GetMMdlSys( fmmdl );
	
	while( MMDLSYS_SearchUseMMdl(fos,&cmmdl,&no) == TRUE ){
		if( cmmdl != fmmdl ){
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
 * @param	fmmdl	MMDL *
 * @param	vec		���������W
 * @param	dir		���� DIR_UP��
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_InitPosition( MMDL * fmmdl, const VecFx32 *vec, u16 dir )
{
	int grid = SIZE_GRID_FX32( vec->x );
	MMDL_SetGridPosX( fmmdl, grid );
	
	grid = SIZE_GRID_FX32( vec->y );
	MMDL_SetGridPosY( fmmdl, grid );
	
	grid = SIZE_GRID_FX32( vec->z );
	MMDL_SetGridPosZ( fmmdl, grid );
	
	MMDL_SetVectorPos( fmmdl, vec );
	MMDL_UpdateGridPosCurrent( fmmdl );
	
	MMDL_SetForceDirDisp( fmmdl, dir );
	
	MMDL_FreeAcmd( fmmdl );
	MMDL_OnStatusBit( fmmdl, MMDL_STABIT_MOVE_START );
	MMDL_OffStatusBit( fmmdl, MMDL_STABIT_MOVE|MMDL_STABIT_MOVE_END );
}

//--------------------------------------------------------------
/**
 * MMDL ����R�[�h�ύX
 * @param	fmmdl	MMDL *
 * @param	code	MV_RND��
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_ChangeMoveCode( MMDL *fmmdl, u16 code )
{
	MMDL_CallMoveDeleteProc( fmmdl );
	MMDL_SetMoveCode( fmmdl, code );
	MMdl_InitCallMoveProcWork( fmmdl );
	MMDL_InitMoveProc( fmmdl );
}

//--------------------------------------------------------------
/**
 * MMDL �`�揉�����ɍs�������Z��
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_InitDrawWork( MMDL *fmmdl )
{
	const MMDLSYS *fos = MMDL_GetMMdlSys( fmmdl );
	
	if( MMDLSYS_CheckCompleteDrawInit(fos) == FALSE ){
		return; //�`��V�X�e���������������Ă��Ȃ�
	}
	
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_HEIGHT_GET_ERROR) ){
		MMDL_UpdateCurrentHeight( fmmdl );
	}
	
	MMDL_SetDrawStatus( fmmdl, 0 );
	#ifndef MMDL_PL_NULL
	MMDL_BlActAddPracFlagSet( fmmdl, FALSE );
	#endif
	
	if( MMDL_CheckStatusBitCompletedDrawInit(fmmdl) == FALSE ){
		MMdl_InitCallDrawProcWork( fmmdl );
		MMDL_CallDrawInitProc( fmmdl );
		MMDL_OnStatusBitCompletedDrawInit( fmmdl );
	}
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f�� �`��֐�������
 * @param	fmmdl		MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_InitCallDrawProcWork( MMDL * fmmdl )
{
	const MMDL_DRAW_PROC_LIST *list;
	u16 code = MMDL_GetOBJCode( fmmdl );
	const OBJCODE_PARAM *prm = MMDL_GetOBJCodeParam( fmmdl, code );
	list = DrawProcList_GetList( prm->draw_proc_no );
	fmmdl->draw_proc_list = list;
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���֘A�G�t�F�N�g�̃t���O�������B
 * �G�t�F�N�g�֘A�̃t���O���������܂Ƃ߂�B
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_InitDrawEffectFlag( MMDL * fmmdl )
{
	MMDL_OffStatusBit( fmmdl,
		MMDL_STABIT_SHADOW_SET		|
		MMDL_STABIT_SHADOW_VANISH	|
		MMDL_STABIT_EFFSET_SHOAL		|
		MMDL_STABIT_REFLECT_SET );
}

//--------------------------------------------------------------
/**
 * MMDL OBJ ID��ύX
 * @param	fmmdl	MMDL *
 * @param	id		OBJ ID
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_ChangeOBJID( MMDL * fmmdl, u16 id )
{
	MMDL_SetOBJID( fmmdl, id );
	MMDL_OnStatusBitMoveStart( fmmdl );
	MMdl_InitDrawEffectFlag( fmmdl );
}

//--------------------------------------------------------------
/**
 * MMDL ���[�N����
 * @param	fmmdl	MMDL
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_ClearWork( MMDL *fmmdl )
{
	GFL_STD_MemClear( fmmdl, MMDL_SIZE );
}

//--------------------------------------------------------------
/**
 * MMDL �w�肳�ꂽ�t�B�[���h���샂�f�����G�C���A�X�w�肩�ǂ����`�F�b�N
 * @param	fmmdl		MMDL *
 * @param	h_zone_id	head��ǂݍ��ރ]�[��ID
 * @param	max			head�v�f��
 * @param	head		MMDL_H
 * @retval	int			RET_ALIES_NOT��
 */
//--------------------------------------------------------------
static int MMdl_CheckHeaderAlies(
		const MMDL *fmmdl, int h_zone_id, int max,
		const MMDL_HEADER *head )
{
	u16 obj_id;
	int zone_id;
	
	while( max ){
		obj_id = head->id;
		
		if( MMDL_GetOBJID(fmmdl) == obj_id ){
			//�G�C���A�X�w�b�_�[
			if( MMdlHeader_CheckAlies(head) == TRUE ){
				//�G�C���A�X�̐��K�]�[��ID
				zone_id = MMdlHeader_GetAliesZoneID( head );
				//�ΏۃG�C���A�X
				if( MMDL_CheckStatusBitAlies(fmmdl) == TRUE ){
					if( MMDL_GetZoneIDAlies(fmmdl) == zone_id ){
						return( RET_ALIES_EXIST );	//Alies�Ƃ��Ċ��ɑ���
					}
				}else if( MMDL_GetZoneID(fmmdl) == zone_id ){
					return( RET_ALIES_CHANGE );		//Alies�Ώۂł���
				}
			}else{ //�ʏ�w�b�_�[
				if( MMDL_CheckStatusBitAlies(fmmdl) == TRUE ){
					//�����G�C���A�X�ƈ�v
					if( MMDL_GetZoneIDAlies(fmmdl) == h_zone_id ){
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
	MMDL *fmmdl;
	
	while( MMDLSYS_SearchUseMMdl(fos,&fmmdl,&no) == TRUE ){
		if( MMDL_GetOBJID(fmmdl) == obj_id &&
			MMDL_GetZoneID(fmmdl) == zone_id ){
			return( fmmdl );
		}
	}
	
	return( NULL );
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���`�揉�����ɍs������
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_InitDrawStatus( MMDL * fmmdl )
{
	MMDL_OnStatusBit( fmmdl, MMDL_STABIT_MOVE_START );
	MMdl_InitDrawEffectFlag( fmmdl );
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���`��폜���ɍs������
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_SetDrawDeleteStatus( MMDL * fmmdl )
{
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���@�G�C���A�X���琳�KOBJ�ւ̕ύX
 * @param	fmmdl		MMDL * 
 * @param	head		�Ώۂ�MMDL_H
 * @param	zone_id		���K�̃]�[��ID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_ChangeAliesOBJ(
	MMDL *fmmdl, const MMDL_HEADER *head, int zone_id )
{
	GF_ASSERT( MMDL_CheckStatusBitAlies(fmmdl) == TRUE );
	MMDL_SetStatusBitAlies( fmmdl, FALSE );
	MMDL_SetZoneID( fmmdl, zone_id );
	MMDL_SetEventID( fmmdl, head->event_id );
	MMDL_SetEventFlag( fmmdl, head->event_flag );
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���@���KOBJ����G�C���A�X�ւ̕ύX
 * @param	fmmdl		MMDL * 
 * @param	head		�Ώۂ�MMDL_H
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_ChangeOBJAlies(
	MMDL * fmmdl, int zone_id, const MMDL_HEADER *head )
{
	GF_ASSERT( MMdlHeader_CheckAlies(head) == TRUE );
	MMDL_SetStatusBitAlies( fmmdl, TRUE );
	MMDL_SetEventID( fmmdl, head->event_id );
	MMDL_SetEventFlag( fmmdl, MMdlHeader_GetAliesZoneID(head) );
	MMDL_SetZoneID( fmmdl, zone_id );
}

//--------------------------------------------------------------
/**
 * MMDL �t�B�[���h���샂�f���̓���`�F�b�N�B
 * ���S�A����ւ�肪�������Ă��邩�`�F�b�N����B
 * @param	fmmdl	MMDL *
 * @param	obj_id	����Ƃ݂Ȃ�OBJ ID
 * @param	zone_id	����Ƃ݂Ȃ�ZONE ID
 * @retval	int		TRUE=����BFALSE=����ł͂Ȃ�
 */
//--------------------------------------------------------------
BOOL MMDL_CheckSameID( const MMDL * fmmdl, u16 obj_id, u16 zone_id )
{
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_USE) == 0 ){
		return( FALSE );
	}
	
	if( MMDL_GetOBJID(fmmdl) != obj_id ){
		return( FALSE );
	}
	
	if( MMDL_GetZoneID(fmmdl) != zone_id ){
		if( MMDL_CheckStatusBitAlies(fmmdl) == FALSE ){
			return( FALSE );
		}
		
		if( MMDL_GetZoneIDAlies(fmmdl) != zone_id ){
			return( FALSE );
		}
	}
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���̓���`�F�b�N�BOBJ�R�[�h�܂�
 * ���S�A����ւ�肪�������Ă��邩�`�F�b�N����B
 * @param	fmmdl	MMDL *
 * @param	code	����Ƃ݂Ȃ�OBJ�R�[�h
 * @param	obj_id	����Ƃ݂Ȃ�OBJ ID
 * @param	zone_id	����Ƃ݂Ȃ�ZONE ID
 * @retval	int		TRUE=����BFALSE=����ł͂Ȃ�
 */
//--------------------------------------------------------------
BOOL MMDL_CheckSameIDCode(
		const MMDL * fmmdl, u16 code, u16 obj_id, u16 zone_id )
{
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_USE) ){
		return( FALSE );
	}
	
	{
		u16 ret = MMDL_GetOBJCode( fmmdl );
		if( ret != code ){
			return( FALSE );
		}
	}
	
	return( MMDL_CheckSameID(fmmdl,obj_id,zone_id) );
}

//======================================================================
//	OBJCODE_PARAM
//======================================================================
//--------------------------------------------------------------
/**
 * MMDLSYS OBJCODE_PARAM ������
 * @param	fmmdlsys	MMDLSYS
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdlSys_InitOBJCodeParam( MMDLSYS *fmmdlsys )
{
	u8 *p = GFL_ARC_LoadDataAlloc( ARCID_MMDL_PARAM, 
			NARC_fldmmdl_mdlparam_fldmmdl_mdlparam_bin,
			fmmdlsys->heapID );
	fmmdlsys->pOBJCodeParamBuf = p;
	fmmdlsys->pOBJCodeParamTbl = (const OBJCODE_PARAM*)(&p[OBJCODE_PARAM_TOTAL_NUMBER_SIZE]);
}

//--------------------------------------------------------------
/**
 * MMDLSYS OBJCODE_PARAM �폜
 * @param	fmmdlsys	MMDLSYS
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdlSys_DeleteOBJCodeParam( MMDLSYS *fmmdlsys )
{
	GFL_HEAP_FreeMemory( fmmdlsys->pOBJCodeParamBuf );
	fmmdlsys->pOBJCodeParamBuf = NULL;
}

//--------------------------------------------------------------
/**
 * MMDLSYS OBJCODE_PARAM �擾
 * @param	fmmdlsys	MMDLSYS *
 * @param	code	�擾����OBJ�R�[�h
 * @retval	OBJCODE_PARAM*
 */
//--------------------------------------------------------------
const OBJCODE_PARAM * MMDLSYS_GetOBJCodeParam(
		const MMDLSYS *fmmdlsys, u16 code )
{
	GF_ASSERT( code < OBJCODEMAX );
  GF_ASSERT( fmmdlsys->pOBJCodeParamTbl != NULL );
	return( &(fmmdlsys->pOBJCodeParamTbl[code]) );
}

//--------------------------------------------------------------
/**
 * MMDL OBJCODE_PARAM �擾
 * @param	fmmdl	MMDL*
 * @param	code	�擾����OBJ�R�[�h
 * @retval	OBJCODE_PARAM*
 */
//--------------------------------------------------------------
const OBJCODE_PARAM * MMDL_GetOBJCodeParam(
		const MMDL *fmmdl, u16 code )
{
	const MMDLSYS *fmmdlsys = MMDL_GetMMdlSys( fmmdl );
	return( MMDLSYS_GetOBJCodeParam(fmmdlsys,code) );
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
void MMDL_MoveInitProcDummy( MMDL * fmmdl )
{
}

//--------------------------------------------------------------
/**
 * ����֐��_�~�[
 * @param	MMDL	MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_MoveProcDummy( MMDL * fmmdl )
{
}

//--------------------------------------------------------------
/**
 * ����폜�֐��_�~�[
 * @param	MMDL	MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_MoveDeleteProcDummy( MMDL * fmmdl )
{
}

//--------------------------------------------------------------
/**
 * ���앜�A�֐��_�~�[
 * @param	MMDL *	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_MoveReturnProcDummy( MMDL * fmmdl )
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
void MMDL_DrawInitProcDummy( MMDL * fmmdl )
{
}

//--------------------------------------------------------------
/**
 * �`��֐��_�~�[
 * @param	MMDL	MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_DrawProcDummy( MMDL * fmmdl )
{
}

//--------------------------------------------------------------
/**
 * �`��폜�֐��_�~�[
 * @param	MMDL	MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_DrawDeleteProcDummy( MMDL * fmmdl )
{
}

//--------------------------------------------------------------
/**
 * �`��ޔ��֐��_�~�[
 * @param	MMDL	MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_DrawPushProcDummy( MMDL * fmmdl )
{
}

//--------------------------------------------------------------
/**
 * �`�敜�A�֐��_�~�[
 * @param	MMDL	MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_DrawPopProcDummy( MMDL * fmmdl )
{
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
	GF_ASSERT( code < OBJCODEMAX );
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
void MMDL_ChangeOBJCode( MMDL *fmmdl, u16 code )
{
	const MMDLSYS *fos;
	fos = MMDL_GetMMdlSys( fmmdl );
  
	if( MMDLSYS_CheckCompleteDrawInit(fos) == TRUE ){
    if( MMDL_CheckStatusBitCompletedDrawInit(fmmdl) == TRUE ){
		  MMDL_CallDrawDeleteProc( fmmdl );
    }
  }
  
  MMDL_SetOBJCode( fmmdl, code );
  MMDL_OffStatusBitCompletedDrawInit( fmmdl );
  MMdl_InitDrawStatus( fmmdl );
  MMdl_InitDrawWork( fmmdl );
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
	MMDL *fmmdl;
	
	fmmdl = MMdlSys_SearchSpaceMMdl( fos );
	GF_ASSERT( fmmdl != NULL );
	
	OS_Printf( "MMDL LoadNo %d\n", no );
	
	*fmmdl = buf->fldMMdlBuf[no];
	
	MMdl_InitWork( fmmdl, fos );
	
	if( MMDL_CheckMoveBit(fmmdl,MMDL_MOVEBIT_MOVEPROC_INIT) == 0 ){
		MMdl_InitMoveWork( fmmdl );
	}else{
		MMdl_InitCallMoveProcWork( fmmdl );
	}
	
	MMDL_OffStatusBitCompletedDrawInit( fmmdl );
	MMdl_InitDrawWork( fmmdl );
	
	MMdlSys_AddMMdlTCB( fos, fmmdl );
	MMdlSys_IncrementOBJCount( (MMDLSYS*)MMDL_GetMMdlSys(fmmdl) );
	
	return( fmmdl );
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
	MMDL *fmmdl;

	for( i = 0; i < MMDL_BUFFER_MAX; i++ ){
		fmmdl = &buf->fldMMdlBuf[i];
		if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_USE) ){
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
	MMDL *fmmdl;
	u32 i = 0, no = 0;
	
	MMDL_BUFFER_InitBuffer( buf );
	while( MMDLSYS_SearchUseMMdl(fos,&fmmdl,&i) == TRUE ){
		OS_Printf( "MMDL BUFFER WorkNo %d, BufferNo %d Save\n", i-1, no );
		buf->fldMMdlBuf[no] = *fmmdl;
		no++;
	}
}
#endif
