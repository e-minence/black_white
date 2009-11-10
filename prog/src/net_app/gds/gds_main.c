//==============================================================================
/**
 * @file	gds_main.c
 * @brief	GDS���[�h�FPROC����
 * @author	matsuda
 * @date	2008.01.17(��)
 */
//==============================================================================
#include <gflib.h>
#include <dwc.h>
#include "system/main.h"
#include "net/dwc_rap.h"
#include "savedata\save_control.h"
#include "gds_battle_rec.h"
#include "gds_ranking.h"
#include "gds_boxshot.h"
#include "gds_dressup.h"

//#include "savedata/imageclip_data.h"
#include "savedata/box_savedata.h"
#include "poke_tool/monsno_def.h"
#include "savedata/gds_profile.h"

#include <arc_tool.h>
#include "print/wordset.h"
#include "message.naix"
//#include "system/fontproc.h"
//#include "gflib/strbuf_family.h"

//#include "communication\comm_system.h"
//#include "communication\comm_state.h"
//#include "communication\comm_def.h"
//#include "communication/wm_icon.h"
//#include "communication\communication.h"

#include "gds_battle_rec.h"
#include "gds_ranking.h"
#include "gds_boxshot.h"
#include "gds_dressup.h"

#include "gds_rap.h"
#include "gds_rap_response.h"
#include "gds_data_conv.h"

#include "savedata/config.h"
#include "savedata\system_data.h"
#include "system/bmp_menu.h"
#include <procsys.h>
#include "system/wipe.h"

#include "net_app/gds_main.h"
//#include "application/br_sys.h"

#include "sound/pm_sndsys.h"


//==============================================================================
//	�萔��`
//==============================================================================
#define MYDWC_HEAPSIZE		0x20000

///GDS�v���b�N���䂪�g�p����q�[�v�T�C�Y
#define GDSPROC_HEAP_SIZE		(MYDWC_HEAPSIZE + 0x8000)

//==============================================================================
//	�O���[�o���ϐ�
//==============================================================================
static NNSFndHeapHandle _wtHeapHandle;

//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
static GFL_PROC_RESULT GdsMainProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GdsMainProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT GdsMainProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static void GdsMain_CommInitialize(GDSPROC_MAIN_WORK *gmw);
static void GdsMain_CommFree(GDSPROC_MAIN_WORK *gmw);
static void *AllocFunc( DWCAllocType name, u32   size, int align );
static void FreeFunc(DWCAllocType name, void* ptr,  u32 size);

//==============================================================================
//	�f�[�^
//==============================================================================
///GDS���C���v���Z�X��`�f�[�^
const GFL_PROC_DATA GdsMainProcData = {
	GdsMainProc_Init,
	GdsMainProc_Main,
	GdsMainProc_End,
};


//--------------------------------------------------------------
/**
 * @brief   �v���Z�X�֐��F������
 *
 * @param   proc		�v���Z�X�f�[�^
 * @param   seq			�V�[�P���X
 *
 * @retval  ������
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT GdsMainProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GDSPROC_MAIN_WORK *gmw;
	
	//sys_HBlankIntrStop();	//HBlank���荞�ݒ�~

	GFL_DISP_GX_SetVisibleControlDirect(0);
	GFL_DISP_GXS_SetVisibleControlDirect(0);
	GX_SetVisiblePlane(0);
	GXS_SetVisiblePlane(0);
	GX_SetVisibleWnd(GX_WNDMASK_NONE);
	GXS_SetVisibleWnd(GX_WNDMASK_NONE);
	G2_BlendNone();
	G2S_BlendNone();

	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_GDS_MAIN, GDSPROC_HEAP_SIZE );

	gmw = GFL_PROC_AllocWork(proc, sizeof(GDSPROC_MAIN_WORK), HEAPID_GDS_MAIN );
	GFL_STD_MemClear(gmw, sizeof(GDSPROC_MAIN_WORK));
	gmw->proc_param = pwk;

	PMSND_PlayBGM(SEQ_BGM_WIN1);

	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
/**
 * @brief   �v���Z�X�֐��F���C��
 *
 * @param   proc		�v���Z�X�f�[�^
 * @param   seq			�V�[�P���X
 *
 * @retval  ������
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT GdsMainProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GDSPROC_MAIN_WORK * gmw  = mywk;
	enum{
		SEQ_INIT_DPW,
		SEQ_INIT_DPW_WAIT,
		
		SEQ_WIFI_CONNECT,
		SEQ_WIFI_CONNECT_MAIN,
		
		SEQ_BATTLE_RECORDER,
		SEQ_BATTLE_RECORDER_MAIN,
		
		SEQ_WIFI_CLEANUP,
		SEQ_WIFI_CLEANUP_MAIN,
		
		SEQ_END,
	};
	
	switch(*seq){
	case SEQ_INIT_DPW:	//�ʐM���C�u����������
		GdsMain_CommInitialize(gmw);
		*seq = SEQ_INIT_DPW_WAIT;
		break;
	case SEQ_INIT_DPW_WAIT:
		if(1){//if(CommIsVRAMDInitialize()){
			_wtHeapHandle = gmw->heapHandle;
	
			// wifi�������Ǘ��֐��Ăяo��
			DWC_SetMemFunc( AllocFunc, FreeFunc );
			
			gmw->comm_initialize_ok = TRUE;
			(*seq)++;
		}
		break;
		
	case SEQ_WIFI_CONNECT:	//WIFI�ڑ�
    gmw->proc_sys = GFL_PROC_LOCAL_boot(HEAPID_GDS_MAIN);
		GFL_PROC_LOCAL_CallProc(gmw->proc_sys, NO_OVERLAY_ID, &GdsConnectProcData, gmw);
		(*seq)++;
		break;
	case SEQ_WIFI_CONNECT_MAIN:
    if(GFL_PROC_LOCAL_Main(gmw->proc_sys) == GFL_PROC_MAIN_NULL){
			if(gmw->ret_connect == TRUE){
				gmw->connect_success = TRUE;
				(*seq)++;
			}
			else{
				*seq = SEQ_END;
			}
		}
		break;
	
	case SEQ_BATTLE_RECORDER:	//�o�g�����R�[�_�[(GDS���[�h)
		{
		#if 0 //��check �V�o�g�����R�[�_�[�p�̌Ăяo���ɕς��� 2009.11.09(��)
			const GFL_PROC_DATA *br_proc;
			
			br_proc = BattleRecoder_ProcDataGet(gmw->proc_param->gds_mode);
  		GFL_PROC_LOCAL_CallProc(gmw->proc_sys, NO_OVERLAY_ID, &br_proc, gmw->proc_param->fsys);
		#endif
			(*seq)++;
		}
		break;
	case SEQ_BATTLE_RECORDER_MAIN:
    if(GFL_PROC_LOCAL_Main(gmw->proc_sys) == GFL_PROC_MAIN_NULL){
			(*seq)++;
		}
		break;
	
	case SEQ_WIFI_CLEANUP:		//WIFI�ؒf
		GFL_PROC_LOCAL_CallProc(gmw->proc_sys, NO_OVERLAY_ID, &GdsConnectProcData, gmw);
		(*seq)++;
		break;
	case SEQ_WIFI_CLEANUP_MAIN:
    if(GFL_PROC_LOCAL_Main(gmw->proc_sys) == GFL_PROC_MAIN_NULL){
			gmw->connect_success = FALSE;
			(*seq)++;
		}
		break;

	case SEQ_END:
	  if(gmw->proc_sys != NULL){
      GFL_PROC_LOCAL_Exit(gmw->proc_sys);
    }
		return GFL_PROC_RES_FINISH;
	}

	if(gmw->comm_initialize_ok == TRUE && gmw->connect_success == TRUE && gmw->ret_connect == TRUE){
		// ��M���x�����N�𔽉f������
		DWC_UpdateConnection();

		// Dpw_Tr_Main() �����͗�O�I�ɂ��ł��Ăׂ�
	//	Dpw_Tr_Main();
	}

	return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   �v���Z�X�֐��F�I��
 *
 * @param   proc		�v���Z�X�f�[�^
 * @param   seq			�V�[�P���X
 *
 * @retval  ������
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT GdsMainProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GDSPROC_MAIN_WORK * gmw  = mywk;

	GdsMain_CommFree(gmw);
	
	//GDS�v���b�N�Ăяo���p�����[�^���
	GFL_HEAP_FreeMemory(gmw->proc_param);
	
	GFL_PROC_FreeWork( proc );				// PROC���[�N�J��

	GFL_HEAP_DeleteHeap( HEAPID_GDS_MAIN );

	return GFL_PROC_RES_FINISH;
}


//--------------------------------------------------------------
/**
 * @brief   �ʐM���C�u�����֘A�̏���������
 *
 * @param   gmw		
 */
//--------------------------------------------------------------
static void GdsMain_CommInitialize(GDSPROC_MAIN_WORK *gmw)
{
	if(gmw->comm_initialize_ok == FALSE){
		OS_TPrintf("Comm�������J�n\n");
		
		//���E������Wifi�ʐM���߂��g�p���邽�߃I�[�o�[���C��ǂݏo��(dpw_tr.c��)
//		Overlay_Load(FS_OVERLAY_ID(worldtrade), OVERLAY_LOAD_NOT_SYNCHRONIZE);

		// DWC���C�u�����iWifi�j�ɓn�����߂̃��[�N�̈���m��
		gmw->heapPtr    = GFL_HEAP_AllocMemory(HEAPID_GDS_MAIN, MYDWC_HEAPSIZE + 32);
		gmw->heapHandle = NNS_FndCreateExpHeap( (void *)( ((u32)gmw->heapPtr + 31) / 32 * 32 ), MYDWC_HEAPSIZE);

		OS_TPrintf("Comm�������I��\n");
	}
}

//--------------------------------------------------------------
/**
 * @brief   �ʐM���C�u�����֘A�̉������
 *
 * @param   gmw		
 */
//--------------------------------------------------------------
static void GdsMain_CommFree(GDSPROC_MAIN_WORK *gmw)
{
	if(gmw->comm_initialize_ok == TRUE){
		OS_TPrintf("Comm����J�n\n");
		
		NNS_FndDestroyExpHeap(gmw->heapHandle);
		GFL_HEAP_FreeMemory( gmw->heapPtr );

//		Overlay_UnloadID(FS_OVERLAY_ID(worldtrade));
		
		gmw->comm_initialize_ok = FALSE;

		OS_TPrintf("Comm�������\n");
	}
}

/*---------------------------------------------------------------------------*
  �������m�ۊ֐�
 *---------------------------------------------------------------------------*/
static void *AllocFunc( DWCAllocType name, u32   size, int align )
{
#pragma unused( name )
    void * ptr;
    OSIntrMode old;
    old = OS_DisableInterrupts();
    ptr = NNS_FndAllocFromExpHeapEx( _wtHeapHandle, size, align );
    OS_RestoreInterrupts( old );
    if(ptr == NULL){
	}
	
    return ptr;
}

/*---------------------------------------------------------------------------*
  �������J���֐�
 *---------------------------------------------------------------------------*/
static void FreeFunc(DWCAllocType name, void* ptr,  u32 size)
{
#pragma unused( name, size )
    OSIntrMode old;

    if ( !ptr ) return;
    old = OS_DisableInterrupts();
    NNS_FndFreeToExpHeap( _wtHeapHandle, ptr );
    OS_RestoreInterrupts( old );
}
