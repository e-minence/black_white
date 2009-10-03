//======================================================================
/**
 *
 * @file	fieldobj_movedata.c
 * @brief	�t�B�[���h���샂�f������
 * @author	kagaya
 * @date	05.07.20
 *
 */
//======================================================================
#include "fldmmdl.h"
#include "fldmmdl_procmove.h"
#include "fldmmdl_procacmd.h"

#include "field_comm_actor.h" //MV_COMM_ACTOR

//==============================================================================
//	extern�@����֐�
//==============================================================================

//==============================================================================
//	extern	�A�j���[�V�����R�}���h
//==============================================================================
//==============================================================================
//	�t�B�[���h���샂�f�� ����֐���`
//==============================================================================
//--------------------------------------------------------------
///	�_�~�[
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_Dummy =
{
	MV_DMY,													///<����R�[�h
	MMDL_MoveInitProcDummy,								///<�������֐�
	MMDL_MoveProcDummy,									///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_PLAYER	
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_Player =
{
	MV_PLAYER,												///<����R�[�h
	MMDL_MoveInitProcDummy,								///<�������֐�
	MMDL_MoveProcDummy,									///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_DIR_RND
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_DirRnd =
{
	MV_DIR_RND,												///<����R�[�h
	MMDL_MoveDirRnd_Init,								///<�������֐�
	MMDL_MoveDirRnd_Move,								///<����֐�
	MMDL_MoveDirRnd_Delete,								///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RND
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_Rnd =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveRnd_Init,									///<�������֐�
	MMDL_MvRnd_Move,									///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RND_V
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RndV =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveRndV_Init,									///<�������֐�
	MMDL_MvRnd_Move,									///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RND_H
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RndH =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveRndH_Init,									///<�������֐�
	MMDL_MvRnd_Move,									///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RND_H_LIM
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RndHLim =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveRndHLim_Init,								///<�������֐�
	MMDL_MvRnd_Move,									///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RND_UL
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RndUL =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveDirRndUL_Init,								///<�������֐�
	MMDL_MoveDirRnd_Move,									///<����֐�
	MMDL_MoveDirRnd_Delete,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RND_UR
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RndUR =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveDirRndUR_Init,								///<�������֐�
	MMDL_MoveDirRnd_Move,									///<����֐�
	MMDL_MoveDirRnd_Delete,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RND_DL
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RndDL =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveDirRndDL_Init,								///<�������֐�
	MMDL_MoveDirRnd_Move,									///<����֐�
	MMDL_MoveDirRnd_Delete,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RND_DR
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RndDR =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveDirRndDR_Init,								///<�������֐�
	MMDL_MoveDirRnd_Move,									///<����֐�
	MMDL_MoveDirRnd_Delete,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RND_UDL
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RndUDL =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveDirRndUDL_Init,								///<�������֐�
	MMDL_MoveDirRnd_Move,									///<����֐�
	MMDL_MoveDirRnd_Delete,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RND_UDR
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RndUDR =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveDirRndUDR_Init,								///<�������֐�
	MMDL_MoveDirRnd_Move,									///<����֐�
	MMDL_MoveDirRnd_Delete,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RND_ULR
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RndULR =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveDirRndULR_Init,								///<�������֐�
	MMDL_MoveDirRnd_Move,									///<����֐�
	MMDL_MoveDirRnd_Delete,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RND_DLR
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RndDLR =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveDirRndDLR_Init,								///<�������֐�
	MMDL_MoveDirRnd_Move,									///<����֐�
	MMDL_MoveDirRnd_Delete,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RND_UD
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RndUD =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveDirRndUD_Init,								///<�������֐�
	MMDL_MoveDirRnd_Move,									///<����֐�
	MMDL_MoveDirRnd_Delete,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RND_LR
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RndLR =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveDirRndLR_Init,								///<�������֐�
	MMDL_MoveDirRnd_Move,									///<����֐�
	MMDL_MoveDirRnd_Delete,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_UP
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_Up =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveUp_Init,									///<�������֐�
	MMDL_MoveDir_Move,									///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_DOWN
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_Down =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveDown_Init,									///<�������֐�
	MMDL_MoveDir_Move,									///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_LEFT
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_Left =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveLeft_Init,									///<�������֐�
	MMDL_MoveDir_Move,									///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RIGHT
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_Right =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveRight_Init,								///<�������֐�
	MMDL_MoveDir_Move,									///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_SPIN_L
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_SpinLeft =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveSpinLeft_Init,								///<�������֐�
	MMDL_MoveSpin_Move,									///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_SPIN_R
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_SpinRight =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveSpinRight_Init,							///<�������֐�
	MMDL_MoveSpin_Move,									///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_REWAR
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_Rewar =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveRewar_Init,								///<�������֐�
	MMDL_MoveRewar_Move,								///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RT2
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_Route2 =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveRoute2_Init,								///<�������֐�
	MMDL_MoveRoute2_Move,								///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RTURLD
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RouteURLD =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveRouteURLD_Init,							///<�������֐�
	MMDL_MoveRoute3_Move,								///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RTRLDU
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RouteRLDU =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveRouteRLDU_Init,							///<�������֐�
	MMDL_MoveRoute3_Move,								///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RTDURL
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RouteDURL =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveRouteDURL_Init,							///<�������֐�
	MMDL_MoveRoute3_Move,								///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RTLDUR
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RouteLDUR =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveRouteLDUR_Init,							///<�������֐�
	MMDL_MoveRoute3_Move,								///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RTULRD
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RouteULRD =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveRouteULRD_Init,							///<�������֐�
	MMDL_MoveRoute3_Move,								///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RTLRDU
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RouteLRDU =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveRouteLRDU_Init,							///<�������֐�
	MMDL_MoveRoute3_Move,								///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RTDULR
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RouteDULR =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveRouteDULR_Init,							///<�������֐�
	MMDL_MoveRoute3_Move,								///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RTRDUL
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RouteRDUL =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveRouteRDUL_Init,							///<�������֐�
	MMDL_MoveRoute3_Move,								///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RTLUDR
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RouteLUDR =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveRouteLUDR_Init,							///<�������֐�
	MMDL_MoveRoute3_Move,								///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RTUDRL
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RouteUDRL =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveRouteUDRL_Init,							///<�������֐�
	MMDL_MoveRoute3_Move,								///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RTRLUD
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RouteRLUD =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveRouteRLUD_Init,							///<�������֐�
	MMDL_MoveRoute3_Move,								///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RTDRLU
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RouteDRLU =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveRouteDRLU_Init,							///<�������֐�
	MMDL_MoveRoute3_Move,								///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RTRUDL
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RouteRUDL =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveRouteRUDL_Init,							///<�������֐�
	MMDL_MoveRoute3_Move,								///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RTUDLR
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RouteUDLR =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveRouteUDLR_Init,							///<�������֐�
	MMDL_MoveRoute3_Move,								///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RTLRUD
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RouteLRUD =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveRouteLRUD_Init,							///<�������֐�
	MMDL_MoveRoute3_Move,								///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RTDLRU
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RouteDLRU =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveRouteDLRU_Init,							///<�������֐�
	MMDL_MoveRoute3_Move,								///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RTUL
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RouteUL =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveRouteUL_Init,								///<�������֐�
	MMDL_MoveRoute4_Move,								///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RTDR
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RouteDR =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveRouteDR_Init,								///<�������֐�
	MMDL_MoveRoute4_Move,								///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RTLD
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RouteLD =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveRouteLD_Init,								///<�������֐�
	MMDL_MoveRoute4_Move,								///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RTRU
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RouteRU =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveRouteRU_Init,								///<�������֐�
	MMDL_MoveRoute4_Move,								///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RTUR
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RouteUR =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveRouteUR_Init,								///<�������֐�
	MMDL_MoveRoute4_Move,								///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RTDL
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RouteDL =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveRouteDL_Init,								///<�������֐�
	MMDL_MoveRoute4_Move,								///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RTLU
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RouteLU =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveRouteLU_Init,								///<�������֐�
	MMDL_MoveRoute4_Move,								///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_RTRD
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RouteRD =
{
	MV_RND,													///<����R�[�h
	MMDL_MoveRouteRD_Init,								///<�������֐�
	MMDL_MoveRoute4_Move,								///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

#if 0
//--------------------------------------------------------------
///	MV_SEED
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_Seed =
{
	MV_SEED,												///<����R�[�h
	MMDL_MoveSeed_Init,									///<�������֐�
	MMDL_MoveSeed_Move,									///<����֐�
	MMDL_MoveSeed_Delete,								///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};
#endif

//--------------------------------------------------------------
///	MV_PAIR
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_Pair =
{
	MV_RND,													///<����R�[�h
	MMDL_MovePair_Init,									///<�������֐�
	MMDL_MovePair_Move,									///<����֐�
	MMDL_MovePair_Delete,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_TR_PAIR
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_PairTr =
{
	MV_RND,													///<����R�[�h
	MMDL_MovePairTr_Init,								///<�������֐�
	MMDL_MovePairTr_Move,								///<����֐�
	MMDL_MovePairTr_Delete,								///<�폜�֐�
	MMDL_MovePairTr_Return,								///<���A�֐�
};

//--------------------------------------------------------------
///	MV_HIDE_SNOW
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_HideSnow =
{
	MV_HIDE_SNOW,												///<����R�[�h
	MMDL_MoveHideSnow_Init,								///<�������֐�
	MMDL_MoveHide_Move,								///<����֐�
	MMDL_MoveHide_Delete,								///<�폜�֐�
	MMDL_MoveHide_Return,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_HIDE_SAND
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_HideSand =
{
	MV_HIDE_SAND,												///<����R�[�h
	MMDL_MoveHideSand_Init,								///<�������֐�
	MMDL_MoveHide_Move,								///<����֐�
	MMDL_MoveHide_Delete,								///<�폜�֐�
	MMDL_MoveHide_Return,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_HIDE_GRND
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_HideGround =
{
	MV_HIDE_GRND,												///<����R�[�h
	MMDL_MoveHideGround_Init,								///<�������֐�
	MMDL_MoveHide_Move,								///<����֐�
	MMDL_MoveHide_Delete,								///<�폜�֐�
	MMDL_MoveHide_Return,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_HIDE_KUSA
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_HideKusa =
{
	MV_HIDE_KUSA,												///<����R�[�h
	MMDL_MoveHideKusa_Init,								///<�������֐�
	MMDL_MoveHide_Move,								///<����֐�
	MMDL_MoveHide_Delete,								///<�폜�֐�
	MMDL_MoveHide_Return,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_COPYU
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_CopyU =
{
	MV_COPYU,												///<����R�[�h
	MMDL_MoveCopyU_Init,								///<�������֐�
	MMDL_MoveCopy_Move,									///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_COPYD
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_CopyD =
{
	MV_COPYD,												///<����R�[�h
	MMDL_MoveCopyD_Init,								///<�������֐�
	MMDL_MoveCopy_Move,									///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_COPYL
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_CopyL =
{
	MV_COPYL,												///<����R�[�h
	MMDL_MoveCopyL_Init,								///<�������֐�
	MMDL_MoveCopy_Move,									///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_COPYR
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_CopyR =
{
	MV_COPYR,												///<����R�[�h
	MMDL_MoveCopyR_Init,								///<�������֐�
	MMDL_MoveCopy_Move,									///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_COPYLGRASSU
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_CopyLGrassU =
{
	MV_COPYLGRASSU,											///<����R�[�h
	MMDL_MoveCopyLGrassU_Init,								///<�������֐�
	MMDL_MoveCopy_Move,									///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_COPYLGRASSD
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_CopyLGrassD =
{
	MV_COPYLGRASSD,												///<����R�[�h
	MMDL_MoveCopyLGrassD_Init,								///<�������֐�
	MMDL_MoveCopy_Move,									///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_COPYLGRASSL
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_CopyLGrassL =
{
	MV_COPYLGRASSL,												///<����R�[�h
	MMDL_MoveCopyLGrassL_Init,								///<�������֐�
	MMDL_MoveCopy_Move,									///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_COPYLGRASSR
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_CopyLGrassR =
{
	MV_COPYLGRASSR,												///<����R�[�h
	MMDL_MoveCopyLGrassR_Init,								///<�������֐�
	MMDL_MoveCopy_Move,									///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_ALONGW_L
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_AlongWallL =
{
	MV_ALONGW_L,												///<����R�[�h
	MMDL_AlongWallL_Init,								///<�������֐�
	MMDL_AlongWall_Move,									///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_ALONGW_R
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_AlongWallR =
{
	MV_ALONGW_R,												///<����R�[�h
	MMDL_AlongWallR_Init,								///<�������֐�
	MMDL_AlongWall_Move,									///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_ALONGW_LRL
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_AlongWallLRL =
{
	MV_ALONGW_LRL,												///<����R�[�h
	MMDL_AlongWallLRL_Init,								///<�������֐�
	MMDL_AlongWall_Move,									///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
///	MV_ALONGW_LRR
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_AlongWallLRR =
{
	MV_ALONGW_LRR,												///<����R�[�h
	MMDL_AlongWallLRR_Init,								///<�������֐�
	MMDL_AlongWall_Move,									///<����֐�
	MMDL_MoveDeleteProcDummy,							///<�폜�֐�
	MMDL_MoveReturnProcDummy,							///<���A�֐�
};

//--------------------------------------------------------------
/// MV_COMM_ACTOR
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_CommActor =
{
	MV_COMM_ACTOR,												///<����R�[�h
	MMDL_MoveCommActor_Init,						///<�������֐�
	MMDL_MoveCommActor_Move,						///<����֐�
  MMDL_MoveCommActor_Delete,					///<�폜�֐�
	MMDL_MoveReturnProcDummy,					///<���A�֐�
};

//--------------------------------------------------------------
/// MV_RAIL_DMY
//--------------------------------------------------------------
static const MMDL_MOVE_PROC_LIST DATA_FieldOBJMove_RailDmy =
{
	MV_RAIL_DMY,									///<����R�[�h
	MMDL_RailDmy_Init,						///<�������֐�
	MMDL_RailDmy_Move,						///<����֐�
  MMDL_RailDmy_Delete,					///<�폜�֐�
	MMDL_RailDmy_Return,					///<���A�֐�
};

//==============================================================================
//	�t�B�[���h���샂�f�� ����֐��܂Ƃ�
//==============================================================================
//--------------------------------------------------------------
///	����֐��܂Ƃ�
///	���т�fieldobj_code.h MV_DMY���̃R�[�h�ƈ�v
//--------------------------------------------------------------
const MMDL_MOVE_PROC_LIST * const DATA_FieldOBJMoveProcListTbl[MV_CODE_MAX] =
{
	&DATA_FieldOBJMove_Dummy,								//MV_DMY
	&DATA_FieldOBJMove_Player,								//MV_PLAYER
	&DATA_FieldOBJMove_DirRnd,								//MV_DIR_RND
	&DATA_FieldOBJMove_Rnd,									//MV_RND
	&DATA_FieldOBJMove_RndV,								//MV_RND_V
	&DATA_FieldOBJMove_RndH,								//MV_RND_H
	&DATA_FieldOBJMove_RndUL,								//MV_RND_UL
	&DATA_FieldOBJMove_RndUR,								//MV_RND_UR
	&DATA_FieldOBJMove_RndDL,								//MV_RND_DL
	&DATA_FieldOBJMove_RndDR,								//MV_RND_DR
	&DATA_FieldOBJMove_RndUDL,								//MV_RND_UDL
	&DATA_FieldOBJMove_RndUDR,								//MV_RND_UDR
	&DATA_FieldOBJMove_RndULR,								//MV_RND_ULR
	&DATA_FieldOBJMove_RndDLR,								//MV_RND_DLR
	&DATA_FieldOBJMove_Up,									//MV_UP
	&DATA_FieldOBJMove_Down,								//MV_DOWN
	&DATA_FieldOBJMove_Left,								//MV_LEFT
	&DATA_FieldOBJMove_Right,								//MV_RIGHT
	&DATA_FieldOBJMove_SpinLeft,							//MV_SPIN_L
	&DATA_FieldOBJMove_SpinRight,							//MV_SPIN_R
	&DATA_FieldOBJMove_Route2,								//MV_RT2
	&DATA_FieldOBJMove_RouteURLD,							//MV_RTURLD
	&DATA_FieldOBJMove_RouteRLDU,							//MV_RTRLDU
	&DATA_FieldOBJMove_RouteDURL,							//MV_RTDURL
	&DATA_FieldOBJMove_RouteLDUR,							//MV_RTLDUR
	&DATA_FieldOBJMove_RouteULRD,							//MV_RTULRD
	&DATA_FieldOBJMove_RouteLRDU,							//MV_RTLRDU
	&DATA_FieldOBJMove_RouteDULR,							//MV_RTDULR
	&DATA_FieldOBJMove_RouteRDUL,							//MV_RTRDUL
	&DATA_FieldOBJMove_RouteLUDR,							//MV_RTLUDR
	&DATA_FieldOBJMove_RouteUDRL,							//MV_RTUDRL
	&DATA_FieldOBJMove_RouteRLUD,							//MV_RTRLUD
	&DATA_FieldOBJMove_RouteDRLU,							//MV_RTDRLU
	&DATA_FieldOBJMove_RouteRUDL,							//MV_RTRUDL
	&DATA_FieldOBJMove_RouteUDLR,							//MV_RTUDLR
	&DATA_FieldOBJMove_RouteLRUD,							//MV_RTLRUD
	&DATA_FieldOBJMove_RouteDLRU,							//MV_RTDLRU
	&DATA_FieldOBJMove_RouteUL,								//MV_UL
	&DATA_FieldOBJMove_RouteDR,								//MV_DR
	&DATA_FieldOBJMove_RouteLD,								//MV_LD
	&DATA_FieldOBJMove_RouteRU,								//MV_RU
	&DATA_FieldOBJMove_RouteUR,								//MV_UR
	&DATA_FieldOBJMove_RouteDL,								//MV_DL
	&DATA_FieldOBJMove_RouteLU,								//MV_LU
	&DATA_FieldOBJMove_RouteRD,								//MV_RD
	&DATA_FieldOBJMove_RndUD,								//MV_RND_UD
	&DATA_FieldOBJMove_RndLR,								//MV_RND_LR
#if 0
	&DATA_FieldOBJMove_Seed,								//MV_SEED
#else
	NULL,
#endif
	&DATA_FieldOBJMove_Pair,								//MV_PAIR
	&DATA_FieldOBJMove_Rewar,								//MV_REWAR
	&DATA_FieldOBJMove_PairTr,								//MV_TR_PAIR
	&DATA_FieldOBJMove_HideSnow,							//MV_HIDE_SNOW
	&DATA_FieldOBJMove_HideSand,							//MV_HIDE_SAND
	&DATA_FieldOBJMove_HideGround,							//MV_HIDE_GRND
	&DATA_FieldOBJMove_HideKusa,							//MV_HIDE_KUSA
	&DATA_FieldOBJMove_CopyU,								//MV_COPY_U
	&DATA_FieldOBJMove_CopyD,								//MV_COPY_D
	&DATA_FieldOBJMove_CopyL,								//MV_COPY_L
	&DATA_FieldOBJMove_CopyR,								//MV_COPYLGRASS_R
	&DATA_FieldOBJMove_CopyLGrassU,							//MV_COPYLGRASS_U
	&DATA_FieldOBJMove_CopyLGrassD,							//MV_COPYLGRASS_D
	&DATA_FieldOBJMove_CopyLGrassL,							//MV_COPYLGRASS_L
	&DATA_FieldOBJMove_CopyLGrassR,							//MV_COPYLGRASS_R
	&DATA_FieldOBJMove_AlongWallL,							//MV_ALONGW_L
	&DATA_FieldOBJMove_AlongWallR,							//MV_ALONGW_R
	&DATA_FieldOBJMove_AlongWallLRL,						//MV_ALONGW_LRL
	&DATA_FieldOBJMove_AlongWallLRR,						//MV_ALONGW_LRR
	&DATA_FieldOBJMove_RndHLim,								//MV_RND_H_LIM
  &DATA_FieldOBJMove_CommActor,           //MV_COMM_ACTOR
  &DATA_FieldOBJMove_RailDmy,             //MV_RAIL_DMY
};

//==============================================================================
//	�t�B�[���h���샂�f�� �A�j���[�V�����R�}���h
//==============================================================================
//--------------------------------------------------------------
///	�A�j���[�V�����R�}���h�e�[�u���B
///	���т�fieldobj_code.h AC_DIR_U���̃R�[�h�ƈ�v�B
//--------------------------------------------------------------
int (* const * const DATA_AcmdActionTbl[ACMD_MAX])( MMDL * ) =
{
	DATA_AC_DirU_Tbl,										//AC_DIR_U
	DATA_AC_DirD_Tbl,										//AC_DIR_D
	DATA_AC_DirL_Tbl,										//AC_DIR_L
	DATA_AC_DirR_Tbl,										//AC_DIR_R
	DATA_AC_WalkU_32F_Tbl,									//AC_WALK_U_32F
	DATA_AC_WalkD_32F_Tbl,									//AC_WALK_D_32F
	DATA_AC_WalkL_32F_Tbl,									//AC_WALK_L_32F
	DATA_AC_WalkR_32F_Tbl,									//AC_WALK_R_32F
	DATA_AC_WalkU_16F_Tbl,									//AC_WALK_U_16F
	DATA_AC_WalkD_16F_Tbl,									//AC_WALK_D_16F
	DATA_AC_WalkL_16F_Tbl,									//AC_WALK_L_16F
	DATA_AC_WalkR_16F_Tbl,									//AC_WALK_R_16F
	DATA_AC_WalkU_8F_Tbl,									//AC_WALK_U_8F
	DATA_AC_WalkD_8F_Tbl,									//AC_WALK_D_8F
	DATA_AC_WalkL_8F_Tbl,									//AC_WALK_L_8F
	DATA_AC_WalkR_8F_Tbl,									//AC_WALK_R_8F
	DATA_AC_WalkU_4F_Tbl,									//AC_WALK_U_4F
	DATA_AC_WalkD_4F_Tbl,									//AC_WALK_D_4F
	DATA_AC_WalkL_4F_Tbl,									//AC_WALK_L_4F
	DATA_AC_WalkR_4F_Tbl,									//AC_WALK_R_4F
	DATA_AC_WalkU_2F_Tbl,									//AC_WALK_U_2F
	DATA_AC_WalkD_2F_Tbl,									//AC_WALK_D_2F
	DATA_AC_WalkL_2F_Tbl,									//AC_WALK_L_2F
	DATA_AC_WalkR_2F_Tbl,									//AC_WALK_R_2F
	DATA_AC_StayWalkU_32F_Tbl,								//AC_STAY_WALK_U_32F
	DATA_AC_StayWalkD_32F_Tbl,								//AC_STAY_WALK_D_32F
	DATA_AC_StayWalkL_32F_Tbl,								//AC_STAY_WALK_L_32F
	DATA_AC_StayWalkR_32F_Tbl,								//AC_STAY_WALK_R_32F
	DATA_AC_StayWalkU_16F_Tbl,								//AC_STAY_WALK_U_16F
	DATA_AC_StayWalkD_16F_Tbl,								//AC_STAY_WALK_D_16F
	DATA_AC_StayWalkL_16F_Tbl,								//AC_STAY_WALK_L_16F
	DATA_AC_StayWalkR_16F_Tbl,								//AC_STAY_WALK_R_16F
	DATA_AC_StayWalkU_8F_Tbl,								//AC_STAY_WALK_U_8F
	DATA_AC_StayWalkD_8F_Tbl,								//AC_STAY_WALK_D_8F
	DATA_AC_StayWalkL_8F_Tbl,								//AC_STAY_WALK_L_8F
	DATA_AC_StayWalkR_8F_Tbl,								//AC_STAY_WALK_R_8F
	DATA_AC_StayWalkU_4F_Tbl,								//AC_STAY_WALK_U_4F
	DATA_AC_StayWalkD_4F_Tbl,								//AC_STAY_WALK_D_4F
	DATA_AC_StayWalkL_4F_Tbl,								//AC_STAY_WALK_L_4F
	DATA_AC_StayWalkR_4F_Tbl,								//AC_STAY_WALK_R_4F
	DATA_AC_StayWalkU_2F_Tbl,								//AC_STAY_WALK_U_2F
	DATA_AC_StayWalkD_2F_Tbl,								//AC_STAY_WALK_D_2F
	DATA_AC_StayWalkL_2F_Tbl,								//AC_STAY_WALK_L_2F
	DATA_AC_StayWalkR_2F_Tbl,								//AC_STAY_WALK_R_2F
	DATA_AC_StayJumpU_16F_Tbl,								//AC_STAY_JUMP_U_16F
	DATA_AC_StayJumpD_16F_Tbl,								//AC_STAY_JUMP_D_16F
	DATA_AC_StayJumpL_16F_Tbl,								//AC_STAY_JUMP_L_16F
	DATA_AC_StayJumpR_16F_Tbl,								//AC_STAY_JUMP_R_16F
	DATA_AC_StayJumpU_8F_Tbl,								//AC_STAY_JUMP_U_8F
	DATA_AC_StayJumpD_8F_Tbl,								//AC_STAY_JUMP_D_8F
	DATA_AC_StayJumpL_8F_Tbl,								//AC_STAY_JUMP_L_8F
	DATA_AC_StayJumpR_8F_Tbl,								//AC_STAY_JUMP_R_8F
	DATA_AC_JumpU_1G_8F_Tbl,								//AC_JUMP_U_1G_8F
	DATA_AC_JumpD_1G_8F_Tbl,								//AC_JUMP_D_1G_8F
	DATA_AC_JumpL_1G_8F_Tbl,								//AC_JUMP_L_1G_8F
	DATA_AC_JumpR_1G_8F_Tbl,								//AC_JUMP_R_1G_8F
	DATA_AC_JumpU_2G_16F_Tbl,								//AC_JUMP_U_2G_16F
	DATA_AC_JumpD_2G_16F_Tbl,								//AC_JUMP_D_2G_16F
	DATA_AC_JumpL_2G_16F_Tbl,								//AC_JUMP_L_2G_16F
	DATA_AC_JumpR_2G_16F_Tbl,								//AC_JUMP_R_2G_16F
	DATA_AC_Wait_1F_Tbl,									//AC_WAIT_1F
	DATA_AC_Wait_2F_Tbl,									//AC_WAIT_2F
	DATA_AC_Wait_4F_Tbl,									//AC_WAIT_4F
	DATA_AC_Wait_8F_Tbl,									//AC_WAIT_8F
	DATA_AC_Wait_15F_Tbl,									//AC_WAIT_15F
	DATA_AC_Wait_16F_Tbl,									//AC_WAIT_16F
	DATA_AC_Wait_32F_Tbl,									//AC_WAIT_32F
	DATA_AC_WarpUp_Tbl,										//AC_WARP_UP
	DATA_AC_WarpDown_Tbl,									//AC_WARP_DOWN
	DATA_AC_VanishON_Tbl,									//AC_VANISH_ON
	DATA_AC_VanishOFF_Tbl,									//AC_VANISH_OFF
	DATA_AC_DirPauseON_Tbl,									//AC_DIR_PAUSE_ON
	DATA_AC_DirPauseOFF_Tbl,								//AC_DIR_PAUSE_OFF
	DATA_AC_AnmPauseON_Tbl,									//AC_ANM_PAUSE_ON
	DATA_AC_AnmPauseOFF_Tbl,								//AC_ANM_PAUSE_OFF
	DATA_AC_MarkGyoe_Tbl,									//AC_MARK_GYOE
	DATA_AC_WalkU6F_Tbl,									//AC_WALK_U_6F
	DATA_AC_WalkD6F_Tbl,									//AC_WALK_D_6F
	DATA_AC_WalkL6F_Tbl,									//AC_WALK_L_6F
	DATA_AC_WalkR6F_Tbl,									//AC_WALK_R_6F
	DATA_AC_WalkU3F_Tbl,									//AC_WALK_U_3F
	DATA_AC_WalkD3F_Tbl,									//AC_WALK_D_3F
	DATA_AC_WalkL3F_Tbl,									//AC_WALK_L_3F
	DATA_AC_WalkR3F_Tbl,									//AC_WALK_R_3F
	DATA_AC_WalkU_1F_Tbl,									//AC_WALK_U_1F
	DATA_AC_WalkD_1F_Tbl,									//AC_WALK_D_1F
	DATA_AC_WalkL_1F_Tbl,									//AC_WALK_L_1F
	DATA_AC_WalkR_1F_Tbl,									//AC_WALK_R_1F
	DATA_AC_DashU_4F_Tbl,									//AC_DASH_U_4F
	DATA_AC_DashD_4F_Tbl,									//AC_DASH_D_4F
	DATA_AC_DashL_4F_Tbl,									//AC_DASH_L_4F
	DATA_AC_DashR_4F_Tbl,									//AC_DASH_R_4F
	DATA_AC_JumpHiL_1G_16F_Tbl,								//AC_JUMPHI_L_1G_16F
	DATA_AC_JumpHiR_1G_16F_Tbl,								//AC_JUMPHI_R_1G_16F
	DATA_AC_JumpHiL_3G_32F_Tbl,								//AC_JUMPHI_L_3G_32F
	DATA_AC_JumpHiR_3G_32F_Tbl,								//AC_JUMPHI_R_3G_32F
	DATA_AC_WalkU7F_Tbl,									//AC_WALK_U_7F
	DATA_AC_WalkD7F_Tbl,									//AC_WALK_D_7F
	DATA_AC_WalkL7F_Tbl,									//AC_WALK_L_7F
	DATA_AC_WalkR7F_Tbl,									//AC_WALK_R_7F
	DATA_AC_PcBow_Tbl,										//AC_PC_BOW
	DATA_AC_HidePullOFF_Tbl,								//AC_HIDE_PULLOFF
	DATA_AC_HeroBanzai_Tbl,									//AC_HERO_BANZAI
	DATA_AC_MarkSaisen_Tbl,									//AC_MARK_SAISEN
	DATA_AC_HeroBanzaiUke_Tbl,								//AC_HERO_BANZAI_UKE
	DATA_AC_WalkGLU8F_Tbl,									//AC_WALKGL_U_8F
	DATA_AC_WalkGLD8F_Tbl,									//AC_WALKGL_D_8F
	DATA_AC_WalkGLL8F_Tbl,									//AC_WALKGL_L_8F
	DATA_AC_WalkGLR8F_Tbl,									//AC_WALKGL_R_8F
	
	DATA_AC_WalkGRU8F_Tbl,									//AC_WALKGL_U_8F
	DATA_AC_WalkGRD8F_Tbl,									//AC_WALKGL_D_8F
	DATA_AC_WalkGRL8F_Tbl,									//AC_WALKGL_L_8F
	DATA_AC_WalkGRR8F_Tbl,									//AC_WALKGL_R_8F
	
	DATA_AC_WalkGUU8F_Tbl,									//AC_WALKGU_U_8F
	DATA_AC_WalkGUD8F_Tbl,									//AC_WALKGU_D_8F
	DATA_AC_WalkGUL8F_Tbl,									//AC_WALKGU_L_8F
	DATA_AC_WalkGUR8F_Tbl,									//AC_WALKGU_R_8F
	
	DATA_AC_JumpU_3G_24F_Tbl,								//AC_JUMP_U_3G_24F
	DATA_AC_JumpD_3G_24F_Tbl,								//AC_JUMP_D_3G_24F
	DATA_AC_JumpL_3G_24F_Tbl,								//AC_JUMP_L_3G_24F
	DATA_AC_JumpR_3G_24F_Tbl,								//AC_JUMP_R_3G_24F
	
	DATA_AC_DashGLU4F_Tbl,									//AC_DASHGL_U_8F
	DATA_AC_DashGLD4F_Tbl,									//AC_DASHGL_D_8F
	DATA_AC_DashGLL4F_Tbl,									//AC_DASHGL_L_8F
	DATA_AC_DashGLR4F_Tbl,									//AC_DASHGL_R_8F
	
	DATA_AC_DashGRU4F_Tbl,									//AC_DASHGL_U_8F
	DATA_AC_DashGRD4F_Tbl,									//AC_DASHGL_D_8F
	DATA_AC_DashGRL4F_Tbl,									//AC_DASHGL_L_8F
	DATA_AC_DashGRR4F_Tbl,									//AC_DASHGL_R_8F
	
	DATA_AC_DashGUU4F_Tbl,									//AC_DASHGU_U_8F
	DATA_AC_DashGUD4F_Tbl,									//AC_DASHGU_D_8F
	DATA_AC_DashGUL4F_Tbl,									//AC_DASHGU_L_8F
	DATA_AC_DashGUR4F_Tbl,									//AC_DASHGU_R_8F
	
	DATA_AC_JumpGLU1G_8F_Tbl,							//AC_JUMPGL_U_1G_8F
	DATA_AC_JumpGLD1G_8F_Tbl,
	DATA_AC_JumpGLL1G_8F_Tbl,
	DATA_AC_JumpGLR1G_8F_Tbl,
	
	DATA_AC_JumpGRU1G_8F_Tbl,
	DATA_AC_JumpGRD1G_8F_Tbl,
	DATA_AC_JumpGRL1G_8F_Tbl,
	DATA_AC_JumpGRR1G_8F_Tbl,
	
	DATA_AC_JumpGUU1G_8F_Tbl,
	DATA_AC_JumpGUD1G_8F_Tbl,
	DATA_AC_JumpGUL1G_8F_Tbl,
	DATA_AC_JumpGUR1G_8F_Tbl,							//AC_JUMPGU_R_1G_8F
	
	DATA_AC_WalkGUU4F_Tbl,									//AC_WALKGU_U_4F
	DATA_AC_WalkGUD4F_Tbl,									//AC_WALKGU_D_4F
	DATA_AC_WalkGUL4F_Tbl,									//AC_WALKGU_L_4F
	DATA_AC_WalkGUR4F_Tbl,									//AC_WALKGU_R_4F
	
	DATA_AC_WalkGUU2F_Tbl,									//AC_WALKGU_U_2F
	DATA_AC_WalkGUD2F_Tbl,									//AC_WALKGU_D_2F
	DATA_AC_WalkGUL2F_Tbl,									//AC_WALKGU_L_2F
	DATA_AC_WalkGUR2F_Tbl,									//AC_WALKGU_R_2F
	
	DATA_AC_MarkGyoeTWait_Tbl,							//AC_MARK_GYOE_TWAIT

  DATA_AC_RailDirU_Tbl,                   // AC_RAIL_DIR_U
  DATA_AC_RailDirD_Tbl,                   // AC_RAIL_DIR_D
  DATA_AC_RailDirL_Tbl,                   // AC_RAIL_DIR_L
  DATA_AC_RailDirR_Tbl,                   // AC_RAIL_DIR_R
  DATA_AC_RailWalkU_16F_Tbl,              // AC_RAIL_WALK_U_16F
  DATA_AC_RailWalkD_16F_Tbl,              // AC_RAIL_WALK_D_16F
  DATA_AC_RailWalkL_16F_Tbl,              // AC_RAIL_WALK_L_16F
  DATA_AC_RailWalkR_16F_Tbl,              // AC_RAIL_WALK_R_16F
  DATA_AC_RailWalkU_8F_Tbl,               // AC_RAIL_WALK_U_8F
  DATA_AC_RailWalkD_8F_Tbl,               // AC_RAIL_WALK_D_8F
  DATA_AC_RailWalkL_8F_Tbl,               // AC_RAIL_WALK_L_8F
  DATA_AC_RailWalkR_8F_Tbl,               // AC_RAIL_WALK_R_8F
  DATA_AC_RailWalkU_4F_Tbl,               // AC_RAIL_WALK_U_4F
  DATA_AC_RailWalkD_4F_Tbl,               // AC_RAIL_WALK_D_4F
  DATA_AC_RailWalkL_4F_Tbl,               // AC_RAIL_WALK_L_4F
  DATA_AC_RailWalkR_4F_Tbl,               // AC_RAIL_WALK_R_4F
  DATA_AC_RailWalkU_2F_Tbl,               // AC_RAIL_WALK_U_2F
  DATA_AC_RailWalkD_2F_Tbl,               // AC_RAIL_WALK_D_2F
  DATA_AC_RailWalkL_2F_Tbl,               // AC_RAIL_WALK_L_2F
  DATA_AC_RailWalkR_2F_Tbl,               // AC_RAIL_WALK_R_2F
  DATA_AC_RailDashU_4F_Tbl,               // AC_RAIL_DASH_U_4F
  DATA_AC_RailDashD_4F_Tbl,               // AC_RAIL_DASH_D_4F
  DATA_AC_RailDashL_4F_Tbl,               // AC_RAIL_DASH_L_4F
  DATA_AC_RailDashR_4F_Tbl,               // AC_RAIL_DASH_R_4F
  DATA_AC_RailStayWalkU_16F_Tbl,          // AC_RAIL_STAY_WALK_U_16F	
  DATA_AC_RailStayWalkD_16F_Tbl,          // AC_RAIL_STAY_WALK_D_16F	
  DATA_AC_RailStayWalkL_16F_Tbl,          // AC_RAIL_STAY_WALK_L_16F	
  DATA_AC_RailStayWalkR_16F_Tbl,          // AC_RAIL_STAY_WALK_R_16F	
  DATA_AC_RailStayWalkU_2F_Tbl,           // AC_RAIL_STAY_WALK_U_2F	
  DATA_AC_RailStayWalkD_2F_Tbl,           // AC_RAIL_STAY_WALK_D_2F	
  DATA_AC_RailStayWalkL_2F_Tbl,           // AC_RAIL_STAY_WALK_L_2F	
  DATA_AC_RailStayWalkR_2F_Tbl,           // AC_RAIL_STAY_WALK_R_2F	
};

//==============================================================================
//	�t�B�[���h���샂�f�� �A�j���[�V�����R�}���h�R�[�h�@�����܂Ƃ�
//==============================================================================
//--------------------------------------------------------------
///	�R�[�h�ʑΉ����s�e�[�u���쐬�B
/// ���т�DIR_UP,DIR_DOWN,DIR_LEFT,DIR_RIGHT
//--------------------------------------------------------------
static const int DATA_CodeTbl_AC_DIR_U[] =
{AC_DIR_U,AC_DIR_D,AC_DIR_L,AC_DIR_R};

static const int DATA_CodeTbl_AC_WALK_U_32F[] =
{AC_WALK_U_32F,AC_WALK_D_32F,AC_WALK_L_32F,AC_WALK_R_32F };

static const int DATA_CodeTbl_AC_WALK_U_16F[] =
{AC_WALK_U_16F,AC_WALK_D_16F,AC_WALK_L_16F,AC_WALK_R_16F };

static const int DATA_CodeTbl_AC_WALK_U_8F[] =
{AC_WALK_U_8F,AC_WALK_D_8F,AC_WALK_L_8F,AC_WALK_R_8F };

static const int DATA_CodeTbl_AC_WALK_U_4F[] =
{AC_WALK_U_4F,AC_WALK_D_4F,AC_WALK_L_4F,AC_WALK_R_4F };

static const int DATA_CodeTbl_AC_WALK_U_2F[] =
{AC_WALK_U_2F,AC_WALK_D_2F,AC_WALK_L_2F,AC_WALK_R_2F };

static const int DATA_CodeTbl_AC_STAY_WALK_U_32F[] =
{AC_STAY_WALK_U_32F,AC_STAY_WALK_D_32F,AC_STAY_WALK_L_32F,AC_STAY_WALK_R_32F };

static const int DATA_CodeTbl_AC_STAY_WALK_U_16F[] =
{AC_STAY_WALK_U_16F,AC_STAY_WALK_D_16F,AC_STAY_WALK_L_16F,AC_STAY_WALK_R_16F };

static const int DATA_CodeTbl_AC_STAY_WALK_U_8F[] =
{AC_STAY_WALK_U_8F,AC_STAY_WALK_D_8F,AC_STAY_WALK_L_8F,AC_STAY_WALK_R_8F };

static const int DATA_CodeTbl_AC_STAY_WALK_U_4F[] =
{AC_STAY_WALK_U_4F,AC_STAY_WALK_D_4F,AC_STAY_WALK_L_4F,AC_STAY_WALK_R_4F };

static const int DATA_CodeTbl_AC_STAY_WALK_U_2F[] =
{AC_STAY_WALK_U_2F,AC_STAY_WALK_D_2F,AC_STAY_WALK_L_2F,AC_STAY_WALK_R_2F };

static const int DATA_CodeTbl_AC_STAY_JUMP_U_16F[] =
{AC_STAY_JUMP_U_16F,AC_STAY_JUMP_D_16F,AC_STAY_JUMP_L_16F,AC_STAY_JUMP_R_16F };

static const int DATA_CodeTbl_AC_STAY_JUMP_U_8F[] =
{AC_STAY_JUMP_U_8F,AC_STAY_JUMP_D_8F,AC_STAY_JUMP_L_8F,AC_STAY_JUMP_R_8F };

static const int DATA_CodeTbl_AC_JUMP_U_1G_8F[] =
{AC_JUMP_U_1G_8F,AC_JUMP_D_1G_8F,AC_JUMP_L_1G_8F,AC_JUMP_R_1G_8F };

static const int DATA_CodeTbl_AC_JUMP_U_2G_16F[] =
{AC_JUMP_U_2G_16F,AC_JUMP_D_2G_16F,AC_JUMP_L_2G_16F,AC_JUMP_R_2G_16F };

static const int DATA_CodeTbl_AC_WALK_U_6F[] =
{AC_WALK_U_6F,AC_WALK_D_6F,AC_WALK_L_6F,AC_WALK_R_6F };

static const int DATA_CodeTbl_AC_WALK_U_3F[] =
{AC_WALK_U_3F,AC_WALK_D_3F,AC_WALK_L_3F,AC_WALK_R_3F };

static const int DATA_CodeTbl_AC_WALK_U_1F[] =
{AC_WALK_U_1F,AC_WALK_D_1F,AC_WALK_L_1F,AC_WALK_R_1F };

static const int DATA_CodeTbl_AC_DASH_U_4F[] =
{AC_DASH_U_4F,AC_DASH_D_4F,AC_DASH_L_4F,AC_DASH_R_4F };

static const int DATA_CodeTbl_AC_JUMPHI_L_1G_16F[] =
{AC_JUMPHI_L_1G_16F,AC_JUMPHI_R_1G_16F,AC_JUMPHI_L_1G_16F,AC_JUMPHI_R_1G_16F};

static const int DATA_CodeTbl_AC_JUMPHI_L_3G_32F[] =
{AC_JUMPHI_L_3G_32F,AC_JUMPHI_R_3G_32F,AC_JUMPHI_L_3G_32F,AC_JUMPHI_R_3G_32F};

static const int DATA_CodeTbl_AC_WALK_U_7F[] =
{AC_WALK_U_7F,AC_WALK_D_7F,AC_WALK_L_7F,AC_WALK_R_7F };

static const int DATA_CodeTbl_AC_JUMP_U_3G_24F[] =
{AC_JUMP_U_3G_24F,AC_JUMP_D_3G_24F,AC_JUMP_L_3G_24F,AC_JUMP_R_3G_24F };

static const int DATA_CodeTbl_AC_WALKGL_U_8F[] =
{AC_WALKGL_U_8F,AC_WALKGL_D_8F,AC_WALKGL_L_8F,AC_WALKGL_R_8F};
static const int DATA_CodeTbl_AC_WALKGR_U_8F[] =
{AC_WALKGR_U_8F,AC_WALKGR_D_8F,AC_WALKGR_L_8F,AC_WALKGR_R_8F};
static const int DATA_CodeTbl_AC_WALKGU_U_8F[] =
{AC_WALKGU_U_8F,AC_WALKGU_D_8F,AC_WALKGU_L_8F,AC_WALKGU_R_8F};
static const int DATA_CodeTbl_AC_WALKGU_U_4F[] =
{AC_WALKGU_U_4F,AC_WALKGU_D_4F,AC_WALKGU_L_4F,AC_WALKGU_R_4F};
static const int DATA_CodeTbl_AC_WALKGU_U_2F[] =
{AC_WALKGU_U_2F,AC_WALKGU_D_2F,AC_WALKGU_L_2F,AC_WALKGU_R_2F};

static const int DATA_CodeTbl_AC_DASHGL_U_4F[] =
{AC_DASHGL_U_4F,AC_DASHGL_D_4F,AC_DASHGL_L_4F,AC_DASHGL_R_4F};
static const int DATA_CodeTbl_AC_DASHGR_U_4F[] =
{AC_DASHGR_U_4F,AC_DASHGR_D_4F,AC_DASHGR_L_4F,AC_DASHGR_R_4F};
static const int DATA_CodeTbl_AC_DASHGU_U_4F[] =
{AC_DASHGU_U_4F,AC_DASHGU_D_4F,AC_DASHGU_L_4F,AC_DASHGU_R_4F};

static const int DATA_CodeTbl_AC_JUMPGL_U_1G_8F[] =
{AC_JUMPGL_U_1G_8F,AC_JUMPGL_D_1G_8F,AC_JUMPGL_L_1G_8F,AC_JUMPGL_R_1G_8F};
static const int DATA_CodeTbl_AC_JUMPGR_U_1G_8F[] =
{AC_JUMPGR_U_1G_8F,AC_JUMPGR_D_1G_8F,AC_JUMPGR_L_1G_8F,AC_JUMPGR_R_1G_8F};
static const int DATA_CodeTbl_AC_JUMPGU_U_1G_8F[] =
{AC_JUMPGU_U_1G_8F,AC_JUMPGU_D_1G_8F,AC_JUMPGU_L_1G_8F,AC_JUMPGU_R_1G_8F};

// RAIL
static const int DATA_CodeTbl_RAIL_DIR_U[] =
{AC_RAIL_DIR_U,AC_RAIL_DIR_D,AC_RAIL_DIR_L,AC_RAIL_DIR_R};
static const int DATA_CodeTbl_RAIL_WALK_U_16F[] =
{AC_RAIL_WALK_U_16F,AC_RAIL_WALK_D_16F,AC_RAIL_WALK_L_16F,AC_RAIL_WALK_R_16F};
static const int DATA_CodeTbl_RAIL_WALK_U_8F[] =
{AC_RAIL_WALK_U_8F,AC_RAIL_WALK_D_8F,AC_RAIL_WALK_L_8F,AC_RAIL_WALK_R_8F};
static const int DATA_CodeTbl_RAIL_WALK_U_4F[] =
{AC_RAIL_WALK_U_4F,AC_RAIL_WALK_D_4F,AC_RAIL_WALK_L_4F,AC_RAIL_WALK_R_4F};
static const int DATA_CodeTbl_RAIL_WALK_U_2F[] =
{AC_RAIL_WALK_U_2F,AC_RAIL_WALK_D_2F,AC_RAIL_WALK_L_2F,AC_RAIL_WALK_R_2F};
static const int DATA_CodeTbl_RAIL_DASH_U_4F[] =
{AC_RAIL_DASH_U_4F,AC_RAIL_DASH_D_4F,AC_RAIL_DASH_L_4F,AC_RAIL_DASH_R_4F};
static const int DATA_CodeTbl_RAIL_STAY_WALK_U_16F[] =
{AC_RAIL_STAY_WALK_U_16F,AC_RAIL_STAY_WALK_D_16F,AC_RAIL_STAY_WALK_L_16F,AC_RAIL_STAY_WALK_R_16F};
static const int DATA_CodeTbl_RAIL_STAY_WALK_U_2F[] =
{AC_RAIL_STAY_WALK_U_2F,AC_RAIL_STAY_WALK_D_2F,AC_RAIL_STAY_WALK_L_2F,AC_RAIL_STAY_WALK_R_2F};

//--------------------------------------------------------------
///	MMDL_ChangeDirAcmdCode()�őΉ�����R�[�h�B
/// ���т͕s���B
//--------------------------------------------------------------
const int * const DATA_AcmdCodeDirChangeTbl[] =
{
	DATA_CodeTbl_AC_DIR_U,
	DATA_CodeTbl_AC_WALK_U_32F,
	DATA_CodeTbl_AC_WALK_U_16F,
	DATA_CodeTbl_AC_WALK_U_8F,
	DATA_CodeTbl_AC_WALK_U_4F,
	DATA_CodeTbl_AC_WALK_U_2F,
	DATA_CodeTbl_AC_STAY_WALK_U_32F,
	DATA_CodeTbl_AC_STAY_WALK_U_16F,
	DATA_CodeTbl_AC_STAY_WALK_U_8F,
	DATA_CodeTbl_AC_STAY_WALK_U_4F,
	DATA_CodeTbl_AC_STAY_WALK_U_2F,
	DATA_CodeTbl_AC_STAY_JUMP_U_16F,
	DATA_CodeTbl_AC_STAY_JUMP_U_8F,
	DATA_CodeTbl_AC_JUMP_U_1G_8F,
	DATA_CodeTbl_AC_JUMP_U_2G_16F,
	DATA_CodeTbl_AC_WALK_U_6F,
	DATA_CodeTbl_AC_WALK_U_3F,
	DATA_CodeTbl_AC_WALK_U_1F,
	DATA_CodeTbl_AC_DASH_U_4F,
	DATA_CodeTbl_AC_JUMPHI_L_1G_16F,
	DATA_CodeTbl_AC_JUMPHI_L_3G_32F,
	DATA_CodeTbl_AC_WALK_U_7F,
	DATA_CodeTbl_AC_WALKGL_U_8F,
	DATA_CodeTbl_AC_WALKGR_U_8F,
	DATA_CodeTbl_AC_JUMP_U_3G_24F,
	DATA_CodeTbl_AC_WALKGU_U_8F,
	DATA_CodeTbl_AC_DASHGL_U_4F,
	DATA_CodeTbl_AC_DASHGR_U_4F,
	DATA_CodeTbl_AC_DASHGU_U_4F,
	DATA_CodeTbl_AC_JUMPGL_U_1G_8F,
	DATA_CodeTbl_AC_JUMPGR_U_1G_8F,
	DATA_CodeTbl_AC_JUMPGU_U_1G_8F,
	DATA_CodeTbl_AC_WALKGU_U_4F,
	DATA_CodeTbl_AC_WALKGU_U_2F,
  DATA_CodeTbl_RAIL_DIR_U,
  DATA_CodeTbl_RAIL_WALK_U_16F,
  DATA_CodeTbl_RAIL_WALK_U_8F,
  DATA_CodeTbl_RAIL_WALK_U_4F,
  DATA_CodeTbl_RAIL_WALK_U_2F,
  DATA_CodeTbl_RAIL_DASH_U_4F,
  DATA_CodeTbl_RAIL_STAY_WALK_U_16F,
  DATA_CodeTbl_RAIL_STAY_WALK_U_2F,
	NULL,	//�I�[����
};

