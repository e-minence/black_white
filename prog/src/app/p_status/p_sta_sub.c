//======================================================================
/**
 * @file	p_sta_sub.c
 * @brief	�|�P�������X�g �E�������
 * @author	ariizumi
 * @data	09/06/10
 *
 * ���W���[�����FPSTATUS_SUB
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/wipe.h"
#include "system/mcss.h"
#include "system/mcss_tool.h"

#include "p_status_gra.naix"

#include "p_sta_sys.h"
#include "p_sta_sub.h"

#include "test/ariizumi/ari_debug.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

#define PSTATUS_MCSS_POS_X (FX32_CONST(( 208 )/16.0f))
#define PSTATUS_MCSS_POS_Y (FX32_CONST((192.0f-( 120 ))/16.0f))

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
struct _PSTATUS_SUB_WORK
{
  MCSS_SYS_WORK *mcssSys;
  MCSS_WORK     *pokeMcss;
  
  GFL_BMPWIN  *bmpWinUpper;
  GFL_BMPWIN  *bmpWinDown;
};

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
static void PSTATUS_SUB_PokeCreateMcss( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork , const POKEMON_PASO_PARAM *ppp );
static void PSTATUS_SUB_PokeDeleteMcss( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork );

//--------------------------------------------------------------
//	������
//--------------------------------------------------------------
PSTATUS_SUB_WORK* PSTATUS_SUB_Init( PSTATUS_WORK *work )
{
  PSTATUS_SUB_WORK* subWork;
  
  subWork = GFL_HEAP_AllocMemory( work->heapId , sizeof(PSTATUS_SUB_WORK) );
  subWork->pokeMcss = NULL;

  subWork->mcssSys = MCSS_Init( 1 , work->heapId );
  MCSS_SetOrthoMode( subWork->mcssSys );

  return subWork;
}

//--------------------------------------------------------------
//	�J��
//--------------------------------------------------------------
void PSTATUS_SUB_Term( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  PSTATUS_SUB_PokeDeleteMcss( work,subWork );
  MCSS_Exit( subWork->mcssSys );
  GFL_HEAP_FreeMemory( subWork );
}

//--------------------------------------------------------------
//	�X�V
//--------------------------------------------------------------
void PSTATUS_SUB_Main( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  MCSS_Main( subWork->mcssSys );
}

//--------------------------------------------------------------
//	�X�V
//--------------------------------------------------------------
void PSTATUS_SUB_Draw( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  MCSS_Draw( subWork->mcssSys );
}

//--------------------------------------------------------------
//	�y�[�W�̕\��
//--------------------------------------------------------------
void PSTATUS_SUB_DispPage( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  //MCSS
  const POKEMON_PASO_PARAM *ppp = PSTATUS_UTIL_GetCurrentPPP( work );
  PSTATUS_SUB_PokeCreateMcss( work , subWork , ppp );

  //����
//  subWork->bmpWinUpper = GFL_BMPWIN_Create();
}


//--------------------------------------------------------------
//	�y�[�W�̃N���A
//--------------------------------------------------------------
void PSTATUS_SUB_ClearPage( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{


  //MCSS
  PSTATUS_SUB_PokeDeleteMcss( work,subWork );
}

#pragma mark [>Resource
void PSTATUS_SUB_LoadResource( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork , ARCHANDLE *archandle )
{
  GFL_ARCHDL_UTIL_TransVramScreen( archandle , NARC_p_status_gra_p_status_sub_NSCR , 
                    PSTATUS_BG_PLATE ,  0 , 0, FALSE , work->heapId );
  
}

void PSTATUS_SUB_ReleaseResource( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  
}

#pragma mark [>Poke
//--------------------------------------------------------------
//	�|�P����MCSS�쐬
//--------------------------------------------------------------
static void PSTATUS_SUB_PokeCreateMcss( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork , const POKEMON_PASO_PARAM *ppp )
{
  MCSS_ADD_WORK addWork;
  VecFx32 scale = {FX32_ONE*16,FX32_ONE*16,FX32_ONE};
  
  GF_ASSERT( subWork->pokeMcss == NULL );
  
  MCSS_TOOL_MakeMAWPPP( ppp , &addWork , MCSS_DIR_FRONT );
  subWork->pokeMcss = MCSS_Add( subWork->mcssSys , PSTATUS_MCSS_POS_X , PSTATUS_MCSS_POS_Y ,0 , &addWork );
  MCSS_SetScale( subWork->pokeMcss , &scale );
//  MCSS_ResetVanishFlag( subWork->pokeMcss );//�]����Ɏ�����ON�ɂȂ�
//  MCSS_SetMepachiFlag( subWork->pokeMcss );
}

//--------------------------------------------------------------
//	�|�P����MCSS�폜
//--------------------------------------------------------------
static void PSTATUS_SUB_PokeDeleteMcss( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  GF_ASSERT( subWork->pokeMcss != NULL );
  
  MCSS_SetVanishFlag( subWork->pokeMcss );
  MCSS_Del(subWork->mcssSys,subWork->pokeMcss);
  subWork->pokeMcss = NULL;
}

