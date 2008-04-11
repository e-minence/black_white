//******************************************************************************
/**
 * 
 * @file	guru2.c
 * @brief	���邮�����
 * @author	kagaya
 * @data	05.07.13
 *
 */
//******************************************************************************
#include "guru2_local.h"

#include "..\..\field\fieldsys.h"
#include "..\..\field\field_subproc.h"

//==============================================================================
//	define
//==============================================================================
//--------------------------------------------------------------
///	�Ǘ��V�[�P���X
//--------------------------------------------------------------
enum
{
	SEQNO_G2P_INIT = 0,
	SEQNO_G2P_RECEIPT,
	SEQNO_G2P_SELECT,
	SEQNO_G2P_STATUS,
	SEQNO_G2P_MAIN,
	SEQNO_G2P_CANCEL,
	SEQNO_G2P_OYA_CANCEL_START,
	SEQNO_G2P_OYA_CANCEL_TIMING_WAIT,
	SEQNO_G2P_END,
};

//==============================================================================
//	struct
//==============================================================================
//--------------------------------------------------------------
///	GURU2_CALL_WORK
//--------------------------------------------------------------
typedef struct
{
	int seq_no;
	GURU2_PARAM param;
	GURU2PROC_WORK *g2p;
	FIELDSYS_WORK *fsys;
	PLIST_DATA *plist;
	PSTATUS_DATA *psd;
}GURU2_CALL_WORK;

//==============================================================================
//	proto
//==============================================================================
static const PROC_DATA Guru2Receipt_Proc;
static const PROC_DATA Guru2Main_Proc;

//==============================================================================
//	���邮�����
//==============================================================================
//--------------------------------------------------------------
/**
 * ���邮������@�v���Z�X�@������
 * @param	param	GURU2_PARAM
 * @param	heap_id	�q�[�vID
 * @retval	GURU2PROC_WORK	GURU2PROC_WORK
 */
//--------------------------------------------------------------
GURU2PROC_WORK * Guru2_ProcInit( GURU2_PARAM *param, u32 heap_id )
{
	GURU2PROC_WORK *g2p;
	
	g2p = sys_AllocMemory( heap_id, sizeof(GURU2PROC_WORK) );
	memset( g2p, 0, sizeof(GURU2PROC_WORK) );
	
	//�O���f�[�^�Z�b�g
	g2p->param = *param;
	
	//�ʐM������
	g2p->g2c = Guru2Comm_WorkInit( g2p, heap_id );
	
	return( g2p );
}

//--------------------------------------------------------------
/**
 * ���邮������@�v���Z�X�@�I��
 * @param	g2p		GURU2PROC_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
void Guru2_ProcDelete( GURU2PROC_WORK *g2p )
{
	//���[�N���f
	
	//���j�I�����[���ʐM��
	Guru2Comm_CommUnionRoomChange( g2p->g2c );
	
	//�ʐM���[�N�폜
	Guru2Comm_WorkDelete( g2p->g2c );
	
	//���\�[�X�J��
	sys_FreeMemoryEz( g2p );
}

//==============================================================================
//	�p�[�c
//==============================================================================
//--------------------------------------------------------------
/**
 * ��t�I���@�߂�l�`�F�b�N
 * @param	g2p	GURU2PROC_WORK *
 * @retval	BOOL	TRUE=�p�� FALSE=�I��
 */
//--------------------------------------------------------------
BOOL Guru2_ReceiptRetCheck( GURU2PROC_WORK *g2p )
{
	return( g2p->receipt_ret );
}

//==============================================================================
//	�C�x���g
//==============================================================================
//--------------------------------------------------------------
/**
 * ���邮����������p���[�N�쐬
 * @param	FIELDSYS_WORK *fsys
 * @retval	void*	
 */
//--------------------------------------------------------------
void * EventCmd_Guru2ProcWorkAlloc( FIELDSYS_WORK *fsys )
{
	GURU2_CALL_WORK *work;
	
	work = sys_AllocMemory( HEAPID_WORLD, sizeof(GURU2_CALL_WORK) );
	memset( work, 0, sizeof(GURU2_CALL_WORK) );
	
	work->fsys = fsys;
	work->param.sv = fsys->savedata;
	work->param.union_view = fsys->union_view;
	work->param.config = SaveData_GetConfig( fsys->savedata );
	work->param.record = SaveData_GetRecord( fsys->savedata );
	work->param.win_type = CONFIG_GetWindowType( work->param.config );
	work->param.fsys = fsys;
	return( work );
}


//--------------------------------------------------------------
/**
 * ���邮������C�x���g����
 * @param	wk		EventCmd_Guru2ProcWorkAlloc()�̖߂�l�@�I�����J��
 * @retval	BOOL	TRUE=�I��
 */
//--------------------------------------------------------------
BOOL EventCmd_Guru2Proc( void *wk )
{
	GURU2_CALL_WORK *work = wk;
	FIELDSYS_WORK *fsys = work->fsys;
	
	switch( work->seq_no ){
	case SEQNO_G2P_INIT:	//������
		work->g2p = Guru2_ProcInit( &work->param, HEAPID_WORLD );
		work->seq_no = SEQNO_G2P_RECEIPT;
		
		GameSystem_StartSubProc( fsys, &Guru2Receipt_Proc, work->g2p );
		break;
	case SEQNO_G2P_RECEIPT:	//��t
		if( FieldEvent_Cmd_WaitSubProcEnd(fsys) ){
			break;
		}
		
		if( Guru2_ReceiptRetCheck(work->g2p) == FALSE ){
			work->seq_no = SEQNO_G2P_END;
			break;
		}
		
		work->plist = Guru2ListEvent_SetProc( fsys );
		work->g2p->guru2_mode = GURU2MODE_POKESEL;
		work->seq_no = SEQNO_G2P_SELECT;
		break;
	case SEQNO_G2P_SELECT: //�|�P�����I��
		if( FieldEvent_Cmd_WaitSubProcEnd(fsys) ){
			break;
		}
		
		{
			int ret = work->plist->ret_sel;
			
			sys_FreeMemoryEz( work->plist );		//���X�g���[�N�J��
			
			if( ret == PL_SEL_POS_EXIT ){			//�L�����Z��
				work->seq_no = SEQNO_G2P_CANCEL;
				break;
			}
			
			if( work->plist->ret_mode == PL_RET_STATUS ){	//�X�e�[�^�X�{��
				work->psd = PSTATUS_Temoti_Create(
					fsys, HEAPID_BASE_APP, PST_MODE_NORMAL );
				work->psd->pos = ret;
				FieldPokeStatus_SetProc( fsys, work->psd );	
				work->seq_no = SEQNO_G2P_STATUS;
				break;
			}
			
			work->g2p->trade_no = ret;
		}
		
		GameSystem_StartSubProc( fsys, &Guru2Main_Proc, work->g2p );
		work->g2p->guru2_mode = GURU2MODE_GAME_MAIN;
		work->seq_no = SEQNO_G2P_MAIN;
		break;
	case SEQNO_G2P_STATUS:	//�|�P�����X�e�[�^�X
		if( FieldEvent_Cmd_WaitSubProcEnd(fsys) ){
			break;
		}
		
		sys_FreeMemoryEz( work->psd );
		work->plist = Guru2ListEvent_SetProc( fsys );
		work->seq_no = SEQNO_G2P_SELECT;
		break;
	case SEQNO_G2P_MAIN:	//���邮�郁�C��
		if( FieldEvent_Cmd_WaitSubProcEnd(fsys) ){
			break;
		}
		
		work->seq_no = SEQNO_G2P_END;
		break;
	case SEQNO_G2P_CANCEL:	//�L�����Z��
		if( CommGetCurrentID() == 0 ){	//�e
			if( Guru2Comm_PokeSelCancelCheck(work->g2p->g2c) ){ //�S�L�����Z��
				work->seq_no = SEQNO_G2P_END;
			}else{
				u16 bit = G2COMM_GMSBIT_CANCEL;
				Guru2Comm_SendData(
					work->g2p->g2c, G2COMM_GM_SIGNAL, &bit, 2 );
				work->g2p->guru2_mode = GURU2MODE_POKESEL_CANCEL;
				work->seq_no = SEQNO_G2P_OYA_CANCEL_START;
			}
		}else{
			int oya_end = work->g2p->g2c->comm_psel_oya_end_flag;
			
			if( oya_end == G2C_OYA_END_FLAG_NON ){
				Guru2Comm_SendData(
					work->g2p->g2c, G2COMM_GM_CANCEL, NULL, 0 );
				work->g2p->guru2_mode = GURU2MODE_POKESEL_CANCEL;
				work->seq_no = SEQNO_G2P_END;
				break;
			}
			
			if( oya_end == G2C_OYA_END_FLAG_EXE ){	//�e�ؒf���s��
				break;
			}
			
			work->seq_no = SEQNO_G2P_END;	//�e�ؒf�ς�
		}
		
		break;
	case SEQNO_G2P_OYA_CANCEL_START:		//�e�L�����Z������󂯎��҂�
		CommTimingSyncStart( COMM_GURU2_TIMINGSYNC_NO );
		work->seq_no = SEQNO_G2P_OYA_CANCEL_TIMING_WAIT;
		break;
	case SEQNO_G2P_OYA_CANCEL_TIMING_WAIT:	//�e�L�����Z���ʐM�I���҂�
		if( CommIsTimingSync(COMM_GURU2_TIMINGSYNC_NO) ){
			work->seq_no = SEQNO_G2P_END;
		}
		break;
	case SEQNO_G2P_END:		//�I��
		Guru2_ProcDelete( work->g2p );
		sys_FreeMemoryEz( wk );
		return( TRUE );
	}
	
	return( FALSE );
}

//==============================================================================
//	data
//==============================================================================
// �I�[�o�[���CID�錾
FS_EXTERN_OVERLAY(guru2);

//--------------------------------------------------------------
///	���邮���t�v���Z�X�f�[�^
//--------------------------------------------------------------
static const PROC_DATA Guru2Receipt_Proc = {
	Guru2Receipt_Init,
	Guru2Receipt_Main,
	Guru2Receipt_End,
	FS_OVERLAY_ID(guru2),
};

//--------------------------------------------------------------
///	���邮�郁�C���v���Z�X�f�[�^
//--------------------------------------------------------------
static const PROC_DATA Guru2Main_Proc = {
	Guru2Main_Init,
	Guru2Main_Main,
	Guru2Main_End,
	FS_OVERLAY_ID(guru2),
};

