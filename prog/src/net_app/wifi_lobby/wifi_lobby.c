//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wifi_lobby.c
 *	@brief		WiFi���r�[���C���V�X�e��
 *	@author		tomoya takahashi
 *	@data		2007.09.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>

//#include "system/pm_overlay.h"

#include "savedata/save_control.h"

#include "net_app/wifi_lobby.h"

#include "net\network_define.h"

//#include "wifi/dwc_overlay.h"

#include "wflby_system.h"
#include "wflby_apl.h"

#include <ppwlobby/ppw_lobby.h>

#include "net_app\wifi_lobby.h"
#include "system/main.h"
#include "system/gfl_use.h"
#include "net_old/net_old.h"

//-----------------------------------------------------------------------------
/**
 *					�R�[�f�B���O�K��
 *		���֐���
 *				�P�����ڂ͑啶������ȍ~�͏������ɂ���
 *		���ϐ���
 *				�E�ϐ�����
 *						const�ɂ� c_ ��t����
 *						static�ɂ� s_ ��t����
 *						�|�C���^�ɂ� p_ ��t����
 *						�S�č��킳��� csp_ �ƂȂ�
 *				�E�O���[�o���ϐ�
 *						�P�����ڂ͑啶��
 *				�E�֐����ϐ�
 *						�������Ɓh�Q�h�Ɛ������g�p���� �֐��̈���������Ɠ���
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	WiFi���r�[���C�����[�N
//=====================================
typedef struct {
	SAVE_CONTROL_WORK*		p_save;		// �Z�[�u�f�[�^
	WFLBY_SYSTEM*	p_commsys;	// WiFi���r�[���ʏ����V�X�e��
	WFLBY_APL*		p_apl;		// WiFi���r�[�A�v���Ǘ��V�X�e��
	GFL_TCB* p_vtcb;				// VBLANKTCB
	GAME_COMM_SYS_PTR p_game_comm;
} WFLBY_WK;



//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
static void WFLBY_VBlankFunc( GFL_TCB* p_tcb, void* p_work );


//==============================================================================
//	�f�[�^
//==============================================================================
const GFL_PROC_DATA WFLBY_PROC = {	
	WFLBYProc_Init,
	WFLBYProc_Main,
	WFLBYProc_Exit,
};

//==============================================================================
//	
//==============================================================================
FS_EXTERN_OVERLAY(dpw_common);


//----------------------------------------------------------------------------
/**
 *	@brief	����������
 *
 *	@param	p_proc		���[�N
 *	@param	p_seq		�V�[�P���X
 *
 *	@retval	GFL_PROC_RES_CONTINUE = 0,		///<����p����
 *	@retval	GFL_PROC_RES_FINISH,			///<����I��
 */
//-----------------------------------------------------------------------------
GFL_PROC_RESULT WFLBYProc_Init( GFL_PROC* p_proc, int* p_seq , void * pwk, void * mywk)
{
	WFLBY_WK* p_wk;
	WFLBY_PROC_PARAM* p_param;

	{
		// �I�[�o�[���C
		FS_EXTERN_OVERLAY(wifi2dmap);
		GFL_OVERLAY_Load( FS_OVERLAY_ID(wifi2dmap) );
	
	#if WB_FIX
		// WiFi�I�[�o�[���C�J�n
		DwcOverlayStart();
	#endif

		// ���E�����ighttp���C�u�����̂��߁j
	#if WB_FIX
		DpwCommonOverlayStart();
	#else
		GFL_OVERLAY_Load( FS_OVERLAY_ID(dpw_common) );
    NetOld_Load();
	#endif
	}
	
	// �q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WFLOBBY, 0x5000 );

	// ���[�N�쐬
	p_wk = GFL_PROC_AllocWork( p_proc, sizeof(WFLBY_WK), HEAPID_WFLOBBY );
	GFL_STD_MemFill( p_wk, 0, sizeof(WFLBY_WK) );

	// �p�����[�^���[�N�擾
	p_param = pwk;

	// �Z�[�u�f�[�^�擾
	p_wk->p_save = p_param->p_save;
	p_wk->p_game_comm = p_param->p_game_comm;

	// ���ʏ����V�X�e���쐬
	p_wk->p_commsys = WFLBY_SYSTEM_Init( p_wk->p_save, HEAPID_WFLOBBY );
	p_wk->p_apl = WFLBY_APL_Init( p_param->check_skip, p_param->p_save, 
			p_param->p_wflby_counter, p_wk->p_commsys, HEAPID_WFLOBBY, p_param->p_game_comm );

	// V�u�����NH�u�����N�֐��ݒ�
	//sys_HBlankIntrStop();	//HBlank���荞�ݒ�~


	// �펞����VBLANK�^�X�N����
	p_wk->p_vtcb = GFUser_VIntr_CreateTCB( WFLBY_VBlankFunc, p_wk, 0 );

	// �v���b�N�J�n
	WFLBY_APL_Start( p_wk->p_apl );
	
	return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���C������
 *
 *	@param	p_proc		���[�N
 *	@param	p_seq		�V�[�P���X
 *
 *	@retval	GFL_PROC_RES_CONTINUE = 0,		///<����p����
 *	@retval	GFL_PROC_RES_FINISH,			///<����I��
 */
//-----------------------------------------------------------------------------
GFL_PROC_RESULT WFLBYProc_Main( GFL_PROC* p_proc, int* p_seq, void * pwk, void * mywk )
{
	WFLBY_WK* p_wk;
	WFLBY_APL_RET apl_ret;

	p_wk = mywk;

	// �A�v�����C��
	apl_ret = WFLBY_APL_Main( p_wk->p_apl );

	// ���ʏ������C��
	// �ʒu�̓A�v���̂��ƌŒ�I�I�I�I
	WFLBY_SYSTEM_Main( p_wk->p_commsys );

	// �I���`�F�b�N
	if( apl_ret == WFLBY_APL_RET_END ){
		return GFL_PROC_RES_FINISH;
	}
	return GFL_PROC_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�j������
 *
 *	@param	p_proc		���[�N
 *	@param	p_seq		�V�[�P���X
 *
 *	@retval	GFL_PROC_RES_CONTINUE = 0,		///<����p����
 *	@retval	GFL_PROC_RES_FINISH,			///<����I��
 */
//-----------------------------------------------------------------------------
GFL_PROC_RESULT WFLBYProc_Exit( GFL_PROC* p_proc, int* p_seq, void * pwk, void * mywk )
{
	WFLBY_WK* p_wk;

	p_wk = mywk;

	// �펞����V�u�����NTCB�j��
	GFL_TCB_DeleteTask( p_wk->p_vtcb );
	
	// V�u�����NH�u�����N�֐��ݒ�
//	sys_VBlankFuncChange( NULL, NULL );	// VBlank�Z�b�g
	//sys_HBlankIntrStop();	//HBlank���荞�ݒ�~

	// �A�v���V�X�e���j��
	WFLBY_APL_Exit( p_wk->p_apl );

	// ���ʏ����V�X�e���j��
	WFLBY_SYSTEM_Exit( p_wk->p_commsys );

	// ���[�N�j��
	GFL_PROC_FreeWork( p_proc );

	// �q�[�v�j��
	GFL_HEAP_DeleteHeap( HEAPID_WFLOBBY );

	// �QD�}�b�v�V�X�e�����I�[�o�[���C��j��
	{
		FS_EXTERN_OVERLAY(wifi2dmap);
		GFL_OVERLAY_Unload( FS_OVERLAY_ID(wifi2dmap) );
	
		// ���E�����ighttp���C�u�����̂��߁j
	#if WB_FIX
		DpwCommonOverlayEnd();
	#else
		GFL_OVERLAY_Unload( FS_OVERLAY_ID(dpw_common) );
    NetOld_Unload();
	#endif
	
	#if WB_FIX
		// WiFi�I�[�o�[���C�I��
		DwcOverlayEnd();
	#endif
	}

	return GFL_PROC_RES_FINISH;
}





//-----------------------------------------------------------------------------
/**
 *				�v���C�x�[�g�֐�
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	VBlank�֐�
 *
 *	@param	p_tcb		TCB�|�C���^
 *	@param	p_work		���[�N
 */
//-----------------------------------------------------------------------------
static void WFLBY_VBlankFunc( GFL_TCB* p_tcb, void* p_work )
{
	WFLBY_WK* p_wk;
	p_wk = p_work;

	WFLBY_APL_VBlank( p_wk->p_apl );
	WFLBY_SYSTEM_VBlank( p_wk->p_commsys );
}

