//======================================================================
/**
 * @file	p_sta_ribbon.c
 * @brief	�|�P�����X�e�[�^�X ���{���y�[�W
 * @author	ariizumi
 * @data	09/07/06
 *
 * ���W���[�����FPSTATUS_RIBBON
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/wipe.h"
#include "system/mcss.h"
#include "system/mcss_tool.h"
#include "print/global_msg.h"
#include "print/printsys.h"
#include "print/wordset.h"

#include "p_status_gra.naix"

#include "p_sta_sys.h"
#include "p_sta_ribbon.h"

#include "test/ariizumi/ari_debug.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

//�t�H���g�J���[(�\��Ɛ��l�p
#define PSTATUS_SUB_STR_COL_TITLE (PRINTSYS_LSB_Make(0xf,2,0))
#define PSTATUS_SUB_STR_COL_VALUE (PRINTSYS_LSB_Make(1,2,0))

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum


//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
struct _PSTATUS_RIBBON_WORK
{
  BOOL isUpdateStr;

  NNSG2dScreenData *scrDataDown;
  void *scrResDown;
  NNSG2dScreenData *scrDataUp;
  void *scrResUp;
};

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

//--------------------------------------------------------------
//	������
//--------------------------------------------------------------
PSTATUS_RIBBON_WORK* PSTATUS_RIBBON_Init( PSTATUS_WORK *work )
{
  PSTATUS_RIBBON_WORK* ribbonWork;
  
  ribbonWork = GFL_HEAP_AllocMemory( work->heapId , sizeof(PSTATUS_RIBBON_WORK) );
  ribbonWork->isUpdateStr = FALSE;


  return ribbonWork;
}

//--------------------------------------------------------------
//	�J��
//--------------------------------------------------------------
void PSTATUS_RIBBON_Term( PSTATUS_WORK *work , PSTATUS_RIBBON_WORK *ribbonWork )
{
  GFL_HEAP_FreeMemory( ribbonWork );
}

//--------------------------------------------------------------
//	�X�V
//--------------------------------------------------------------
void PSTATUS_RIBBON_Main( PSTATUS_WORK *work , PSTATUS_RIBBON_WORK *ribbonWork )
{

}

#pragma mark [>Resource
//--------------------------------------------------------------
//	���\�[�X�ǂݍ���
//--------------------------------------------------------------
void PSTATUS_RIBBON_LoadResource( PSTATUS_WORK *work , PSTATUS_RIBBON_WORK *ribbonWork , ARCHANDLE *archandle )
{
  //���������p�X�N���[���ǂݍ���
  ribbonWork->scrResDown = GFL_ARCHDL_UTIL_LoadScreen( archandle , NARC_p_status_gra_p_status_ribbon_NSCR ,
                    FALSE , &ribbonWork->scrDataDown , work->heapId );
  ribbonWork->scrResUp = GFL_ARCHDL_UTIL_LoadScreen( archandle , NARC_p_status_gra_p_status_ribbon_u_NSCR ,
                    FALSE , &ribbonWork->scrDataUp , work->heapId );
}

//--------------------------------------------------------------
//	���\�[�X�J��
//--------------------------------------------------------------
void PSTATUS_RIBBON_ReleaseResource( PSTATUS_WORK *work , PSTATUS_RIBBON_WORK *ribbonWork )
{
  //�����ŕ`�敨�̊J�������Ă���

  PSTATUS_RIBBON_ClearPage( work , ribbonWork );

  GFL_HEAP_FreeMemory( ribbonWork->scrResDown );
  GFL_HEAP_FreeMemory( ribbonWork->scrResUp );

}

#pragma mark [>Disp
//--------------------------------------------------------------
//	�y�[�W�̕\��
//--------------------------------------------------------------
void PSTATUS_RIBBON_DispPage( PSTATUS_WORK *work , PSTATUS_RIBBON_WORK *ribbonWork )
{
  //Window���n�̒��ւ�
  GFL_BG_WriteScreenExpand( PSTATUS_BG_PLATE , 
                    0 , 0 , PSTATUS_MAIN_PAGE_WIDTH , 24 ,
                    ribbonWork->scrDataDown->rawData ,
                    0 , 0 , 32 , 32 );
  GFL_BG_LoadScreenV_Req( PSTATUS_BG_PLATE );

  GFL_BG_LoadScreen( PSTATUS_BG_SUB_PLATE, 
                     ribbonWork->scrDataUp->rawData, 
                     ribbonWork->scrDataUp->szByte, 
                     0 );
  GFL_BG_LoadScreenBuffer( PSTATUS_BG_SUB_PLATE, 
                     ribbonWork->scrDataUp->rawData, 
                     ribbonWork->scrDataUp->szByte );
  GFL_BG_LoadScreenV_Req( PSTATUS_BG_SUB_PLATE );

}

//--------------------------------------------------------------
//	�y�[�W�̃N���A
//--------------------------------------------------------------
void PSTATUS_RIBBON_ClearPage( PSTATUS_WORK *work , PSTATUS_RIBBON_WORK *infoWork )
{
  
  GFL_BG_FillScreen( PSTATUS_BG_PARAM , 0 , 0 , 0 , 
                     PSTATUS_MAIN_PAGE_WIDTH , 24 ,
                     GFL_BG_SCRWRT_PALNL );
  GFL_BG_LoadScreenV_Req( PSTATUS_BG_PARAM );
}

