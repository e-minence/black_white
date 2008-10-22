//======================================================================
/**
 *
 * @file	dlplay_child_main.c	
 * @brief	�_�E�����[�h�v���C�q�@
 * @author	ariizumi
 * @data	08/10/21
 */
//======================================================================
#include <gflib.h>

#include "system/main.h"
#include "../../pokemon_wb/prog/src/test/dlplay/dlplay_comm_func.h"
#include "../../pokemon_wb/prog/src/test/dlplay/dlplay_func.h"


//======================================================================
//	define
//======================================================================
//BG�ʒ�`
#define DLPLAY_MSG_PLANE			(GFL_BG_FRAME3_M)
#define DLPLAY_MSG_PLANE_PRI		(0)

//======================================================================
//	enum
//======================================================================
enum DLPLAY_CHILD_STATE
{
	DCS_INIT_COMM,
	DCS_WAIT_INIT_COMM,
	DCS_MAX,
	
};


//======================================================================
//	typedef struct
//======================================================================
typedef struct
{
	int	heapID_;

	u8	mainSeq_;

	DLPLAY_COMM_DATA *commSys_;
	DLPLAY_MSG_SYS	 *msgSys_;
}DLPLAY_CHILD_DATA;
//======================================================================
//	proto
//======================================================================

DLPLAY_CHILD_DATA *childData;
void	DLPlayChild_SetProc(void);
void	DLPlayChild_InitBg(void);

//============================================================================================
//
//
//	�v���Z�X�R���g���[��
//
//
//============================================================================================
static const	GFL_PROC_DATA DLPlayChild_ProcData;

//------------------------------------------------------------------
/**
 * @brief		�v���Z�X�ݒ�
 */
//------------------------------------------------------------------
void	DLPlayChild_SetProc(void)
{
	GFL_PROC_SysCallProc(NO_OVERLAY_ID, &DLPlayChild_ProcData, NULL);
}

//------------------------------------------------------------------
/**
 * @brief		������
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT DLPlayChild_ProcInit(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_ARIIZUMI_DEBUG, 0x200000 );

	DLPlayChild_InitBg();


	childData = GFL_HEAP_AllocClearMemory( HEAPID_ARIIZUMI_DEBUG, sizeof( DLPLAY_CHILD_DATA ) );

	childData->heapID_ = HEAPID_ARIIZUMI_DEBUG;
	childData->mainSeq_ = DCS_INIT_COMM;

	childData->msgSys_	= DLPlayFunc_MsgInit( childData->heapID_ , DLPLAY_MSG_PLANE );	 
	childData->commSys_ = DLPlayComm_InitData( childData->heapID_ );

	DLPlayFunc_PutString("",childData->msgSys_);
	DLPlayFunc_PutString("System Initialize complete.",childData->msgSys_);
	
	return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
/**
 * @brief		���C��
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT DLPlayChild_ProcMain(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	switch( childData->mainSeq_ )
	{
	case DCS_INIT_COMM:
		//DLPlayComm_InitSystem( childData->commSys_ );
		//childData->mainSeq_ = DCS_WAIT_INIT_COMM;
		break;

	case DCS_WAIT_INIT_COMM:
		if( DLPlayComm_IsFinish_InitSystem( childData->commSys_ ) ){
			DLPlayFunc_PutString("Commnicate system initialize omplete.",childData->msgSys_);
			DLPlayFunc_PutString("Press A Button to start.",childData->msgSys_);
			childData->mainSeq_ = DCS_MAX;
		}
		break;
	}

	return GFL_PROC_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief		�I��
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT DLPlayChild_ProcEnd(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
/**
 * @brief		�v���Z�X�֐��e�[�u��
 */
//------------------------------------------------------------------
const GFL_PROC_DATA DLPlayChild_ProcData = {
	DLPlayChild_ProcInit,
	DLPlayChild_ProcMain,
	DLPlayChild_ProcEnd,
};


//------------------------------------------------------------------
//BG������
//------------------------------------------------------------------
//------------------------------------------------------------------
//	VRAM�p��`
//------------------------------------------------------------------
static const GFL_BG_SYS_HEADER bgsysHeader = {
	GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_3D
};	
static const GFL_BG_BGCNT_HEADER bgCont3 = {
	0, 0, 0x800, 0,
	GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
	GX_BG_SCRBASE_0x2800, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
	GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
};

void	DLPlayChild_InitBg(void)
{
	//VRAM�n������
	//BG�V�X�e���N��
	GFL_BG_Init( HEAPID_ARIIZUMI_DEBUG );

	//VRAM�ݒ�
	GX_SetBankForBG( GX_VRAM_BG_128_A );
	GX_SetBankForOBJ( GX_VRAM_OBJ_128_B );

	//BG���[�h�ݒ�
	GFL_BG_SetBGMode( &bgsysHeader );

	//�a�f�R���g���[���ݒ�
	GFL_BG_SetBGControl( DLPLAY_MSG_PLANE, &bgCont3, GFL_BG_MODE_TEXT );
	GFL_BG_SetPriority( DLPLAY_MSG_PLANE, DLPLAY_MSG_PLANE_PRI );
	GFL_BG_SetVisible( DLPLAY_MSG_PLANE, VISIBLE_ON );

	//�r�b�g�}�b�v�E�C���h�E�V�X�e���̋N��
	GFL_BMPWIN_Init( HEAPID_ARIIZUMI_DEBUG );

}


