//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		c_gear_power_onoff.c
 *	@brief  CGEAR�@�d���@ON�@OFF
 *	@author	takahashi tomoya
 *	@date		2010.04.09
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>

#include "arc_def.h"
#include "message.naix"
#include "font/font.naix"

#include "system/gfl_use.h"
#include "system/ds_system.h"
#include "system/wipe.h"
#include "system/bmp_winframe.h"

#include "gamesystem/msgspeed.h"

#include "print/printsys.h"
#include "print/wordset.h"
#include "print/gf_font.h"

#include "app/app_taskmenu.h"
#include "app/app_keycursor.h"

#include "msg/msg_cg_power.h"

#include "c_gear_power_onoff.h"

#include "c_gear.naix"

#include "../event_subscreen.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	����^�C�v
//=====================================
typedef enum {
  CG_POWER_ONOFF_TYPE_ON,
  CG_POWER_ONOFF_TYPE_OFF,
  CG_POWER_ONOFF_TYPE_CANNOT,
  
} CG_POWER_ONOFF_TYPE;


//-------------------------------------
///	�V�[�P���X
//=====================================
enum {
  SEQ_ON_INIT,
  SEQ_ON_FADEIN_WAIT,

  SEQ_ON_YESNO_WAIT,

  SEQ_ON_MSG_PRINT01,
  SEQ_ON_MSG_PRINT01_WAIT,
  SEQ_ON_MSG_PRINT01_TOUCHWAIT,


  SEQ_ON_END,
} ;

enum {
  SEQ_OFF_INIT,
  SEQ_OFF_FADEIN_WAIT,


  SEQ_OFF_YESNO_WAIT,

  SEQ_OFF_OFF_WAIT,

  SEQ_OFF_END,
} ;

enum {
  SEQ_CANNOT_INIT,
  SEQ_CANNOT_FADEIN_WAIT,

  SEQ_CANNOT_TOUCH_WAIT,

  SEQ_CANNOT_END,
} ;



//-------------------------------------
///	�t���[��
//=====================================
enum
{	
	BG_FRAME_BACK_S	= GFL_BG_FRAME2_S,
	BG_FRAME_MSG_S	= GFL_BG_FRAME1_S,
	BG_FRAME_YESNO_S	= GFL_BG_FRAME0_S,

	BG_FRAME_PLATE_M	= GFL_BG_FRAME1_M,

};


//-------------------------------------
///	�p���b�g
//=====================================
enum
{	
	//�T�uBG
	PLTID_BG_BACK_S				=	0,
	PLTID_BG_MSG_S,
	PLTID_BG_WIN_S,
	PLTID_BG_YESNO00_S,
	PLTID_BG_YESNO01_S,

};


//-------------------------------------
///	�C�x���g���N�G�X�g
//=====================================
typedef enum {
  POWER_EVENT_REQ_RET_NONE,
  POWER_EVENT_REQ_RET_CGEAR,

  POWER_EVENT_REQ_MAX,
} POWER_EVENT_REQ_TYPE;



//-------------------------------------
///	BG�t���[��
//=====================================
static const GFL_BG_BGCNT_HEADER BGFrameBack = {
  0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
  GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000, 0x6800, GX_BG_EXTPLTT_01,
  3, 0, 0, FALSE
};
static const GFL_BG_BGCNT_HEADER BGFrameMsg = {
  0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
  GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x00000, 0x6800, GX_BG_EXTPLTT_01,
  2, 0, 0, FALSE
};
static const GFL_BG_BGCNT_HEADER BGFrameYesNo = {
  0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
  GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x00000, 0x6800, GX_BG_EXTPLTT_01,
  2, 0, 0, FALSE
};

//-------------------------------------
///	��b
//=====================================
#define TALK_MSG_POS_X  (1)
#define TALK_MSG_POS_Y  (1)
#define TALK_MSG_POS_SIZX  (30)
#define TALK_MSG_POS_SIZY  (4)

#define SYS_MSG_POS_X  (1)
#define SYS_MSG_POS_Y  (13)
#define SYS_MSG_POS_SIZX  (30)
#define SYS_MSG_POS_SIZY  (10)

#define SYS_CANNOT_MSG_POS_Y  (6)


//-------------------------------------
///	YesNo�{�^�����
//=====================================
enum{
  YESNO_LIST_YES,
  YESNO_LIST_NO,
  YESNO_LIST_MAX,
};
static APP_TASKMENU_ITEMWORK s_APP_TASKMENU_ITEMWORK[YESNO_LIST_MAX] = {
  {
    NULL,
    APP_TASKMENU_ITEM_MSGCOLOR,
    APP_TASKMENU_WIN_TYPE_NORMAL,
  },
  {
    NULL,
    APP_TASKMENU_ITEM_MSGCOLOR,
    APP_TASKMENU_WIN_TYPE_NORMAL,
  },
};

static APP_TASKMENU_INITWORK s_APP_TASKMENU_INITWORK = {
  0,
  
  YESNO_LIST_MAX,
  s_APP_TASKMENU_ITEMWORK,

  ATPT_LEFT_UP,
  21,
  6,
  0,0, 
};

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�d��ON�@OFF ����
//=====================================
struct _CGEAR_POWER_ONOFF {

  HEAPID heapID;

  GAMESYS_WORK* p_gamesys;
  GAME_COMM_SYS_PTR p_gamecomm;
  FIELD_SUBSCREEN_WORK* p_subscreen;

  GFL_ARCUTIL_TRANSINFO back_area;
  GFL_ARCUTIL_TRANSINFO winframe_area;
  GFL_BMPWIN*   p_talkmsg;
  GFL_BMPWIN*   p_sysmsg;


  GFL_TCBLSYS * p_tcbl;
  PRINT_STREAM* p_printwk;
  STRBUF*       p_str;
  BOOL stream_clear_flg;
  APP_KEYCURSOR_WORK* p_keycursor;

  GFL_FONT*       p_font;
  GFL_MSGDATA*    p_msgdata;
  PRINT_QUE*      p_printQue;

  APP_TASKMENU_RES*       p_yesno_res;
  APP_TASKMENU_WORK*  p_yesno;

  u16 type;
  u16 seq;

  BOOL off;

};

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

// �����ڊ֌W
static void PowerOnOff_InitGraphic( CGEAR_POWER_ONOFF* p_sys, HEAPID heapID );
static void PowerOnOff_ExitGraphic( CGEAR_POWER_ONOFF* p_sys );
static void PowerOnOff_InitBg( CGEAR_POWER_ONOFF* p_sys, HEAPID heapID );
static void PowerOnOff_ExitBg( CGEAR_POWER_ONOFF* p_sys );
static void PowerOnOff_InitYesNo( CGEAR_POWER_ONOFF* p_sys, HEAPID heapID );
static void PowerOnOff_ExitYesNo( CGEAR_POWER_ONOFF* p_sys );
static void PowerOnOff_InitWin( CGEAR_POWER_ONOFF* p_sys, HEAPID heapID );
static void PowerOnOff_ExitWin( CGEAR_POWER_ONOFF* p_sys );

// ���b�Z�[�W�\��
static void PowerOnOff_PrintMsg( CGEAR_POWER_ONOFF* p_sys, GFL_BMPWIN* p_win, u32 strID );
static void PowerOnOff_PrintStreamMsg( CGEAR_POWER_ONOFF* p_sys, GFL_BMPWIN* p_win, u32 strID, HEAPID heapID );
static BOOL PowerOnOff_PrintStreamUpdate( CGEAR_POWER_ONOFF* p_sys, GFL_BMPWIN* p_win );

// �{�^��
static void PowerOnOff_CreateYesNo( CGEAR_POWER_ONOFF* p_sys, HEAPID heapID );
static void PowerOnOff_DeleteYesNo( CGEAR_POWER_ONOFF* p_sys );
static void PowerOnOff_UpdateYesNo( CGEAR_POWER_ONOFF* p_sys );
static BOOL PowerOnOff_IsYesNoEnd( const CGEAR_POWER_ONOFF* cp_sys );
static u32 PowerOnOff_GetYesNo( const CGEAR_POWER_ONOFF* p_sys );

// ���ꂼ��̃��C���V�[�P���X
static BOOL PowerOnOff_UpdateOn( CGEAR_POWER_ONOFF* p_sys );
static BOOL PowerOnOff_UpdateOff( CGEAR_POWER_ONOFF* p_sys );
static BOOL PowerOnOff_UpdateCannot( CGEAR_POWER_ONOFF* p_sys );

//----------------------------------------------------------------------------
/**
 *	@brief  CGEAR�@�p���[�ݒ�
 *
 *	@param	p_subscrn   �T�u�X�N���[�����[�N
 *	@param	p_gamesys   �Q�[���V�X�e��
 *	@param	heapID      �q�[�vID
 */
//-----------------------------------------------------------------------------
CGEAR_POWER_ONOFF* CGEAR_POWER_ONOFF_Create( FIELD_SUBSCREEN_WORK* p_subscrn,GAMESYS_WORK* p_gamesys, HEAPID heapID )
{
  CGEAR_POWER_ONOFF* p_sys;

  p_sys = GFL_HEAP_AllocClearMemory( heapID, sizeof(CGEAR_POWER_ONOFF) );

  p_sys->p_gamesys  = p_gamesys;
  p_sys->p_gamecomm = GAMESYSTEM_GetGameCommSysPtr( p_gamesys );
  p_sys->p_subscreen = p_subscrn;
  
  p_sys->heapID     = heapID;

  // �`����̐���
  PowerOnOff_InitGraphic( p_sys, heapID );

  // DS�ݒ�ŒʐM�s�\
  if( DS_SYSTEM_IsAvailableWireless() == FALSE ){
    
    p_sys->type = CG_POWER_ONOFF_TYPE_CANNOT;
    
  }else{
    // ���̏�񂩂�A�\���������߂�B
    if( GAMESYSTEM_GetAlwaysNetFlag( p_gamesys ) == TRUE ){
      // �ؒf
      p_sys->type = CG_POWER_ONOFF_TYPE_OFF;
      
    }else{
      // �ڑ�
      p_sys->type = CG_POWER_ONOFF_TYPE_ON;

    }
  }
  
  // �������V�[�P���X�����s
  CGEAR_POWER_ONOFF_Main( p_sys, TRUE );
  
  return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief  CGEAR�@�p���[�@�j��
 *
 *	@param	p_sys   �V�X�e��
 */
//-----------------------------------------------------------------------------
void CGEAR_POWER_ONOFF_Delete( CGEAR_POWER_ONOFF* p_sys )
{
  // �`����̔j��
  PowerOnOff_ExitGraphic( p_sys );

  GFL_HEAP_FreeMemory( p_sys );
}


//----------------------------------------------------------------------------
/**
 *	@brief  CGEAR�@�p���[�@ONOFF�ݒ�@���C��
 *
 *	@param	p_sys   �V�X�e��
 *	@param	active  �A�N�e�B�u
 */
//-----------------------------------------------------------------------------
void CGEAR_POWER_ONOFF_Main( CGEAR_POWER_ONOFF* p_sys, BOOL active )
{
  static BOOL (* pFunc[])( CGEAR_POWER_ONOFF* p_sys ) = {
    PowerOnOff_UpdateOn,
    PowerOnOff_UpdateOff,
    PowerOnOff_UpdateCannot,
  };

  // ���ʂ̓A�N�e�B�u�łȂ����傤�����ł���K�v������B
  

  if( pFunc[ p_sys->type ]( p_sys ) )
  {
    FIELD_SUBSCREEN_SetAction( p_sys->p_subscreen, FIELD_SUBSCREEN_ACTION_CGEAR_POWER_EXIT );
  }

  // �L���[���C��
  PRINTSYS_QUE_Main( p_sys->p_printQue );
}




//-----------------------------------------------------------------------------
/**
 *      private�֐�
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief  �����ڂ̐���
 *
 *	@param	p_sys   �V�X�e��
 *	@param	heapID  �q�[�vID
 */
//-----------------------------------------------------------------------------
static void PowerOnOff_InitGraphic( CGEAR_POWER_ONOFF* p_sys, HEAPID heapID )
{
  // �n���h��
  p_sys->p_font = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr , GFL_FONT_LOADTYPE_FILE , FALSE , heapID );
  GFL_FONTSYS_SetDefaultColor();
  
  // ���b�Z�[�W
  p_sys->p_msgdata = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , NARC_message_cg_power_dat, heapID );

  // Print�L���[
  p_sys->p_printQue = PRINTSYS_QUE_Create( heapID );


  // BG��
  PowerOnOff_InitBg( p_sys, heapID );

  // YesNo
  PowerOnOff_InitYesNo( p_sys, heapID );

  // Win
  PowerOnOff_InitWin( p_sys, heapID );

  // NET ICON
  GFL_NET_ReloadIcon();


  // ���ʂɃp�l�����o���B
  FIELD_SUBSCREEN_SetMainLCDNavigationScreen( heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����ڂ̔j��
 *
 *	@param	p_sys   �V�X�e��
 */
//-----------------------------------------------------------------------------
static void PowerOnOff_ExitGraphic( CGEAR_POWER_ONOFF* p_sys )
{
  // 
  FIELD_SUBSCREEN_MainDispBrightnessOff( p_sys->heapID );
  
  // Win
  PowerOnOff_ExitWin( p_sys );

  // YesNo��
  PowerOnOff_ExitYesNo( p_sys );

  // BG��
  PowerOnOff_ExitBg( p_sys );


  // �v�����g�L���[
  PRINTSYS_QUE_Clear( p_sys->p_printQue );
  PRINTSYS_QUE_Delete( p_sys->p_printQue );

  // ���b�Z�[�W
  GFL_MSG_Delete( p_sys->p_msgdata );

  // �t�H���g
  GFL_FONT_Delete( p_sys->p_font );
}

//----------------------------------------------------------------------------
/**
 *	@brief  BG�\�����̏�����
 *
 *	@param	p_sys     �V�X�e��
 *	@param	heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void PowerOnOff_InitBg( CGEAR_POWER_ONOFF* p_sys, HEAPID heapID )
{
  int i = 0;
  for(i = GFL_BG_FRAME0_S;i <= GFL_BG_FRAME3_S ; i++)
  {
    GFL_BG_SetVisible( i, VISIBLE_OFF );
  }
  
  // BGControl
  GFL_BG_SetBGControl( BG_FRAME_BACK_S, &BGFrameBack, GFL_BG_MODE_TEXT );
  GFL_BG_ClearScreen( BG_FRAME_BACK_S );
  GFL_BG_LoadScreenReq( BG_FRAME_BACK_S );
  GFL_BG_SetVisible( BG_FRAME_BACK_S, VISIBLE_ON );

  GFL_BG_SetBGControl( BG_FRAME_MSG_S, &BGFrameMsg, GFL_BG_MODE_TEXT );
  GFL_BG_ClearScreen( BG_FRAME_MSG_S );
  GFL_BG_LoadScreenReq( BG_FRAME_MSG_S );
  GFL_BG_SetVisible( BG_FRAME_MSG_S, VISIBLE_ON );

  GFL_BG_SetBGControl( BG_FRAME_YESNO_S, &BGFrameYesNo, GFL_BG_MODE_TEXT );
  GFL_BG_ClearScreen( BG_FRAME_YESNO_S );
  GFL_BG_LoadScreenReq( BG_FRAME_YESNO_S );
  GFL_BG_SetVisible( BG_FRAME_YESNO_S, VISIBLE_ON );

  // �o�b�N�O���E���h��ݒ�
  {
    ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_C_GEAR, heapID );
    MYSTATUS* p_my = GAMEDATA_GetMyStatus( GAMESYSTEM_GetGameData(p_sys->p_gamesys) );
    u32 sex = MyStatus_GetMySex(p_my);
    static const u32 _bgpal[]={NARC_c_gear_c_gear_m_NCLR,NARC_c_gear_c_gear_f_NCLR,NARC_c_gear_c_gear_m_NCLR};
    static const u32 _bgcgx[]={NARC_c_gear_c_gear_m1_NCGR,NARC_c_gear_c_gear_f1_NCGR,NARC_c_gear_c_gear_m1_NCGR};
    
    GFL_ARCHDL_UTIL_TransVramPalette( p_handle, _bgpal[ sex ],
                                      PALTYPE_SUB_BG, PLTID_BG_BACK_S, 1,  heapID);
    
    // �T�u���BG�L�����]��
    p_sys->back_area = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, _bgcgx[sex],
                                                                  BG_FRAME_BACK_S, 0, FALSE, heapID);

    GFL_ARCHDL_UTIL_TransVramScreenCharOfs(p_handle,
                                           NARC_c_gear_c_gear01_n_NSCR,
                                           BG_FRAME_BACK_S, 0,
                                           GFL_ARCUTIL_TRANSINFO_GetPos(p_sys->back_area), 0, 0,
                                           heapID);

    GFL_ARC_CloseDataHandle( p_handle );
  }
  
}

//----------------------------------------------------------------------------
/**
 *	@brief  BG�\�����̔j��
 *
 *	@param	p_sys   �V�X�e��
 */
//-----------------------------------------------------------------------------
static void PowerOnOff_ExitBg( CGEAR_POWER_ONOFF* p_sys )
{
  GFL_BG_FreeCharacterArea( BG_FRAME_BACK_S, 
      GFL_ARCUTIL_TRANSINFO_GetPos(p_sys->back_area),
      GFL_ARCUTIL_TRANSINFO_GetSize(p_sys->back_area) );
  
  GFL_BG_FreeBGControl(BG_FRAME_BACK_S);
  GFL_BG_FreeBGControl(BG_FRAME_MSG_S);
  GFL_BG_FreeBGControl(BG_FRAME_YESNO_S);


  GFL_BG_SetVisible( BG_FRAME_BACK_S, VISIBLE_OFF );
  GFL_BG_SetVisible( BG_FRAME_MSG_S, VISIBLE_OFF );
  GFL_BG_SetVisible( BG_FRAME_YESNO_S, VISIBLE_OFF );
}

//----------------------------------------------------------------------------
/**
 *	@brief  YesNo�\�����̏�����
 *
 *	@param	p_sys     �V�X�e��
 *	@param	heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void PowerOnOff_InitYesNo( CGEAR_POWER_ONOFF* p_sys, HEAPID heapID )
{
  p_sys->p_yesno_res = APP_TASKMENU_RES_Create( BG_FRAME_YESNO_S, PLTID_BG_YESNO00_S, p_sys->p_font, p_sys->p_printQue, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief  YesNo�\�����̔j��
 *
 *	@param	p_sys     �V�X�e��
 */
//-----------------------------------------------------------------------------
static void PowerOnOff_ExitYesNo( CGEAR_POWER_ONOFF* p_sys )
{
  APP_TASKMENU_RES_Delete( p_sys->p_yesno_res );
}

//----------------------------------------------------------------------------
/**
 *	@brief  window�������̏�����
 *
 *	@param	p_sys     �V�X�e��
 *	@param	heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void PowerOnOff_InitWin( CGEAR_POWER_ONOFF* p_sys, HEAPID heapID )
{
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr, PALTYPE_SUB_BG, PLTID_BG_MSG_S*32, 32, heapID );
  p_sys->winframe_area = BmpWinFrame_GraphicSetAreaMan( BG_FRAME_MSG_S, PLTID_BG_WIN_S, MENU_TYPE_SYSTEM, heapID );

  p_sys->p_talkmsg = GFL_BMPWIN_Create( BG_FRAME_MSG_S, 
                                    TALK_MSG_POS_X, 
                                    TALK_MSG_POS_Y,
                                    TALK_MSG_POS_SIZX, 
                                    TALK_MSG_POS_SIZY, 
                                    PLTID_BG_MSG_S,
                                    GFL_BMP_CHRAREA_GET_B );

  p_sys->p_sysmsg = GFL_BMPWIN_Create( BG_FRAME_MSG_S, 
                                    SYS_MSG_POS_X, 
                                    SYS_MSG_POS_Y,
                                    SYS_MSG_POS_SIZX, 
                                    SYS_MSG_POS_SIZY, 
                                    PLTID_BG_MSG_S,
                                    GFL_BMP_CHRAREA_GET_B );

  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( p_sys->p_talkmsg ) , 15 );
  GFL_BMPWIN_TransVramCharacter( p_sys->p_talkmsg );
  GFL_BMPWIN_MakeScreen( p_sys->p_talkmsg );


  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( p_sys->p_sysmsg ) , 15 );
  GFL_BMPWIN_TransVramCharacter( p_sys->p_sysmsg );
  GFL_BMPWIN_MakeScreen( p_sys->p_sysmsg );


  p_sys->p_tcbl = GFL_TCBL_Init( heapID, heapID, 1, 4 );

  // �L�[�J�[�\��
  p_sys->p_keycursor = APP_KEYCURSOR_Create( 15, FALSE, TRUE, heapID );
  

}

//----------------------------------------------------------------------------
/**
 *	@brief  window�\�����̔j��
 *
 *	@param	p_sys   �V�X�e��
 */
//-----------------------------------------------------------------------------
static void PowerOnOff_ExitWin( CGEAR_POWER_ONOFF* p_sys )
{
  if( p_sys->p_printwk ){
    PRINTSYS_PrintStreamDelete( p_sys->p_printwk );
    p_sys->p_printwk = NULL;

    GFL_STR_DeleteBuffer( p_sys->p_str );
    p_sys->p_str = NULL;
  }

  // �L�[�J�[�\��
  APP_KEYCURSOR_Delete( p_sys->p_keycursor );

  GFL_TCBL_Exit( p_sys->p_tcbl );
  GFL_BMPWIN_Delete( p_sys->p_sysmsg );
  GFL_BMPWIN_Delete( p_sys->p_talkmsg );
  BmpWinFrame_GraphicFreeAreaMan( BG_FRAME_MSG_S, p_sys->winframe_area );
}



// ���b�Z�[�W�\��
//----------------------------------------------------------------------------
/**
 *	@brief  ���b�Z�[�W�̕\������
 *
 *	@param	p_sys   �V�X�e��
 *	@param	p_win   window
 *	@param	strID   ������ID
 */
//-----------------------------------------------------------------------------
static void PowerOnOff_PrintMsg( CGEAR_POWER_ONOFF* p_sys, GFL_BMPWIN* p_win, u32 strID )
{
  STRBUF* p_str;

  p_str = GFL_MSG_CreateString( p_sys->p_msgdata, strID );
  
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(p_win), 0, 0, p_str, p_sys->p_font );
  BmpWinFrame_WriteAreaMan( p_win, WINDOW_TRANS_ON, 
      p_sys->winframe_area, PLTID_BG_WIN_S );
  
  GFL_BMPWIN_TransVramCharacter( p_win );
  
  GFL_STR_DeleteBuffer( p_str );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���b�Z�[�W�̃X�g���[���Đ�
 *
 *	@param	p_sys   �V�X�e��
 *	@param	p_win   window
 *	@param	strID   ������
 */
//-----------------------------------------------------------------------------
static void PowerOnOff_PrintStreamMsg( CGEAR_POWER_ONOFF* p_sys, GFL_BMPWIN* p_win, u32 strID, HEAPID heapID )
{
  GF_ASSERT( p_sys->p_printwk == NULL );
  
  p_sys->p_str = GFL_MSG_CreateString( p_sys->p_msgdata, strID );

  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( p_win ) , 15 );
  
  p_sys->p_printwk = PRINTSYS_PrintStream(
    p_win, 0, 0, p_sys->p_str, p_sys->p_font,
    MSGSPEED_GetWait(), p_sys->p_tcbl, 0, heapID, 15 );

  BmpWinFrame_WriteAreaMan( p_win, WINDOW_TRANS_ON_V, 
      p_sys->winframe_area, PLTID_BG_WIN_S );
}

static BOOL PowerOnOff_PrintStreamUpdate( CGEAR_POWER_ONOFF* p_sys, GFL_BMPWIN* p_win )
{
  GFL_TCBL_Main( p_sys->p_tcbl );
  GFL_TCBL_Main( p_sys->p_tcbl );

  // �L�[�J�[�\��
  APP_KEYCURSOR_Main( p_sys->p_keycursor, p_sys->p_printwk, p_win );
  APP_KEYCURSOR_Main( p_sys->p_keycursor, p_sys->p_printwk, p_win );


  switch( PRINTSYS_PrintStreamGetState(p_sys->p_printwk) ){
  case PRINTSTREAM_STATE_RUNNING: //���s��
    if( GFL_UI_TP_GetCont() == TRUE ){
      PRINTSYS_PrintStreamShortWait( p_sys->p_printwk, 0 );
    }
    p_sys->stream_clear_flg = FALSE;
    break;

  case PRINTSTREAM_STATE_PAUSE: //�ꎞ��~��
    if( p_sys->stream_clear_flg == FALSE ){
      if( GFL_UI_TP_GetTrg() == TRUE ){
        PMSND_PlaySystemSE( SEQ_SE_MESSAGE );
        PRINTSYS_PrintStreamReleasePause( p_sys->p_printwk );
        p_sys->stream_clear_flg = TRUE;
      }
    }
    break;

  case PRINTSTREAM_STATE_DONE: //�I��
    PRINTSYS_PrintStreamDelete( p_sys->p_printwk );
    p_sys->p_printwk = NULL;
    p_sys->stream_clear_flg = FALSE;

    GFL_STR_DeleteBuffer( p_sys->p_str );
    p_sys->p_str = NULL;
    return TRUE;
  }

  return FALSE;
}


// �{�^��
//----------------------------------------------------------------------------
/**
 *	@brief  �{�^���\��
 *
 *	@param	p_sys 
 */
//-----------------------------------------------------------------------------
static void PowerOnOff_CreateYesNo( CGEAR_POWER_ONOFF* p_sys, HEAPID heapID )
{
  s_APP_TASKMENU_INITWORK.heapId = heapID;

  s_APP_TASKMENU_ITEMWORK[ YESNO_LIST_YES ].str = GFL_MSG_CreateString( p_sys->p_msgdata, cg_power_yes );
  s_APP_TASKMENU_ITEMWORK[ YESNO_LIST_NO ].str = GFL_MSG_CreateString( p_sys->p_msgdata, cg_power_no );


	p_sys->p_yesno	= APP_TASKMENU_OpenMenuEx( &s_APP_TASKMENU_INITWORK, p_sys->p_yesno_res );
  APP_TASKMENU_SetDisableKey( p_sys->p_yesno, TRUE );

  GFL_STR_DeleteBuffer( s_APP_TASKMENU_ITEMWORK[ YESNO_LIST_YES ].str );
  GFL_STR_DeleteBuffer( s_APP_TASKMENU_ITEMWORK[ YESNO_LIST_NO ].str );
}

//----------------------------------------------------------------------------
/**
 *	@brief  YesNo�j��
 */
//-----------------------------------------------------------------------------
static void PowerOnOff_DeleteYesNo( CGEAR_POWER_ONOFF* p_sys )
{
  if( p_sys->p_yesno ){
    APP_TASKMENU_CloseMenu( p_sys->p_yesno );
    p_sys->p_yesno = NULL;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  �{�^�����ʂ܂�
 *
 *	@param	p_sys 
 */
//-----------------------------------------------------------------------------
static void PowerOnOff_UpdateYesNo( CGEAR_POWER_ONOFF* p_sys )
{
  if( p_sys->p_yesno ){
    APP_TASKMENU_UpdateMenu( p_sys->p_yesno );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �I�����ʂ̎擾
 *
 *	@param	p_sys 
 *
 *	@return �I������
 */
//-----------------------------------------------------------------------------
static u32 PowerOnOff_GetYesNo( const CGEAR_POWER_ONOFF* p_sys )
{
  return APP_TASKMENU_GetCursorPos( p_sys->p_yesno );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �͂��@�������@�����̊����҂�
 *
 *	@param	cp_sys 
 *
 *	@retval TRUE    �͂�
 *	@retval FALSE   ������
 */
//-----------------------------------------------------------------------------
static BOOL PowerOnOff_IsYesNoEnd( const CGEAR_POWER_ONOFF* cp_sys )
{
  return APP_TASKMENU_IsFinish( cp_sys->p_yesno );
}




// ���ꂼ��̃��C���V�[�P���X
//----------------------------------------------------------------------------
/**
 *	@brief  ON
 *
 *	@param	p_sys 
 */
//-----------------------------------------------------------------------------
static BOOL PowerOnOff_UpdateOn( CGEAR_POWER_ONOFF* p_sys )
{
  switch( p_sys->seq ){
  case SEQ_ON_INIT:
    // �ŏ��̃��b�Z�[�W�́A��C�ɕ\�����Ă����B
    PowerOnOff_PrintMsg( p_sys, p_sys->p_talkmsg, cg_power_01 );
    PowerOnOff_PrintMsg( p_sys, p_sys->p_sysmsg, cg_power_04 );
    PowerOnOff_CreateYesNo( p_sys, p_sys->heapID );

    p_sys->seq++;
    break;

  case SEQ_ON_FADEIN_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      p_sys->seq++;
    }
    break;

  case SEQ_ON_YESNO_WAIT:
    if( PowerOnOff_IsYesNoEnd( p_sys ) ){
      if( PowerOnOff_GetYesNo( p_sys ) == YESNO_LIST_YES ){
        // �I��
        GAMESYSTEM_SetAlwaysNetFlag( p_sys->p_gamesys, TRUE );
        GAMESYSTEM_CommBootAlways( p_sys->p_gamesys );
        p_sys->seq = SEQ_ON_MSG_PRINT01;
      }else{
        p_sys->seq = SEQ_ON_END;
      }
      PowerOnOff_DeleteYesNo( p_sys );
    }
    break;

  case SEQ_ON_MSG_PRINT01:
    PowerOnOff_PrintStreamMsg( p_sys, p_sys->p_talkmsg, cg_power_02, p_sys->heapID );
    p_sys->seq ++;
    break;

  case SEQ_ON_MSG_PRINT01_WAIT:
    if( PowerOnOff_PrintStreamUpdate( p_sys, p_sys->p_talkmsg ) ){
      p_sys->seq ++;
    }
    break;


  case SEQ_ON_MSG_PRINT01_TOUCHWAIT:
    if( GFL_UI_TP_GetTrg() ){
      p_sys->seq ++;
    }
    break;

  case SEQ_ON_END:
    return TRUE;

  }

  PowerOnOff_UpdateYesNo( p_sys );

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  OFF
 */
//-----------------------------------------------------------------------------
static BOOL PowerOnOff_UpdateOff( CGEAR_POWER_ONOFF* p_sys )
{
  switch( p_sys->seq ){
  case SEQ_OFF_INIT:
    // �ŏ��̃��b�Z�[�W�́A��C�ɕ\�����Ă����B
    PowerOnOff_PrintMsg( p_sys, p_sys->p_talkmsg, cg_power_03 );
    PowerOnOff_PrintMsg( p_sys, p_sys->p_sysmsg, cg_power_04 );
    PowerOnOff_CreateYesNo( p_sys, p_sys->heapID );

    p_sys->seq++;
    break;
  case SEQ_OFF_FADEIN_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      p_sys->seq++;
    }
    break;

  case SEQ_OFF_YESNO_WAIT:
    if( PowerOnOff_IsYesNoEnd( p_sys ) ){
      if( PowerOnOff_GetYesNo( p_sys ) == YESNO_LIST_YES ){
        // �I�t
        if( GameCommSys_BootCheck( p_sys->p_gamecomm ) != GAME_COMM_NO_NULL ){
          GameCommSys_ExitReq( p_sys->p_gamecomm );
        }
        p_sys->off = TRUE;
      }else{
        p_sys->off = FALSE;
      }
      p_sys->seq = SEQ_OFF_OFF_WAIT;
      PowerOnOff_DeleteYesNo( p_sys );
    }
    break;

  case SEQ_OFF_OFF_WAIT:
    if( p_sys->off ){
      if( GameCommSys_BootCheck( p_sys->p_gamecomm ) != GAME_COMM_NO_NULL ){
        break;
      }

      GAMESYSTEM_SetAlwaysNetFlag( p_sys->p_gamesys, FALSE );
    }
    p_sys->seq++;
    break;

  case SEQ_OFF_END:
    return TRUE;
  }

  PowerOnOff_UpdateYesNo( p_sys );

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ʐM���o���Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL PowerOnOff_UpdateCannot( CGEAR_POWER_ONOFF* p_sys )
{
  switch( p_sys->seq ){
  case SEQ_CANNOT_INIT:

    // window�ʒu��ύX
    GFL_BMPWIN_SetPosY( p_sys->p_sysmsg, SYS_CANNOT_MSG_POS_Y );
    GFL_BMPWIN_TransVramCharacter( p_sys->p_sysmsg );
    GFL_BMPWIN_MakeScreen( p_sys->p_sysmsg );
    
    // �ŏ��̃��b�Z�[�W�́A��C�ɕ\�����Ă����B
    PowerOnOff_PrintMsg( p_sys, p_sys->p_sysmsg, cg_power_05 );

    p_sys->seq++;
    break;
    
  case SEQ_CANNOT_FADEIN_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      p_sys->seq++;
    }
    break;

  case SEQ_CANNOT_TOUCH_WAIT:
    if( GFL_UI_TP_GetTrg() ){
      p_sys->seq++;
    }
    break;

  case SEQ_CANNOT_END:
    return TRUE;
  
  }

  return FALSE;
}



