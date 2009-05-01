//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_subscreen.c
 *	@brief
 *	@author	 
 *	@data		2009.03.26
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>
#include "infowin/infowin.h"
#include "field_subscreen.h"
#include "c_gear/c_gear.h"

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

#define CGEAR_ON (1)

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

struct _FIELD_SUBSCREEN_WORK {
  C_GEAR_WORK* cgearWork;
};

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------
/**
 *	@brief	�C���t�H�[�o�[�̏�����
 *	
 *	@param	heapID	�q�[�v�h�c
 */
//-----------------------------------------------------------------------------
FIELD_SUBSCREEN_WORK* FIELD_SUBSCREEN_Init( u32 heapID )
{
  FIELD_SUBSCREEN_WORK* pWork = GFL_HEAP_AllocClearMemory(heapID, sizeof(FIELD_SUBSCREEN_WORK));

  {
    // BG3 SUB (�C���t�H�o�[
    static const GFL_BG_BGCNT_HEADER header_sub3 = {
      0, 0, 0x800, 0,	// scrX, scrY, scrbufSize, scrbufofs,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x00000,0x6000,
		GX_BG_EXTPLTT_01, 0, 0, 0, FALSE	// pal, pri, areaover, dmy, mosaic
	};

	GFL_BG_SetBGControl( FIELD_SUBSCREEN_BGPLANE, &header_sub3, GFL_BG_MODE_TEXT );
	GFL_BG_SetVisible( FIELD_SUBSCREEN_BGPLANE, VISIBLE_ON );
	GFL_BG_ClearFrame( FIELD_SUBSCREEN_BGPLANE );
	
	GFL_DISP_GXS_SetVisibleControl(GX_PLANEMASK_OBJ,VISIBLE_ON);

	INFOWIN_Init( FIELD_SUBSCREEN_BGPLANE , FIELD_SUBSCREEN_PALLET , heapID);
	if( INFOWIN_IsInitComm() == TRUE )
	{
		GFL_NET_ReloadIcon();
	}
  }
#if CGEAR_ON
  pWork->cgearWork = CGEAR_Init();
#endif
  
  return pWork;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�C���t�H�[�o�[�̔j��
 */
//-----------------------------------------------------------------------------
void FIELD_SUBSCREEN_Exit( FIELD_SUBSCREEN_WORK* pWork )
{
#if CGEAR_ON
  CGEAR_Exit(pWork->cgearWork);
#endif
	INFOWIN_Exit();
	GFL_BG_FreeBGControl(FIELD_SUBSCREEN_BGPLANE);
  GFL_HEAP_FreeMemory(pWork);
}

//----------------------------------------------------------------------------
/**
 *	@brief	�C���t�H�[�o�[�̍X�V
 */
//-----------------------------------------------------------------------------
void FIELD_SUBSCREEN_Main( FIELD_SUBSCREEN_WORK* pWork )
{
#if CGEAR_ON
  CGEAR_Main(pWork->cgearWork);
#endif
	INFOWIN_Update();
}

