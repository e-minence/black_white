//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *  @file   namein.c
 *  @brief  ���O���͉��
 *  @author Toru=Nagihashi
 *  @data   2009.10.07
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//  lib
#include <gflib.h>

//  SYSTEM
#include "system/main.h"  //HEAPID
#include "system/gfl_use.h"
#include "gamesystem/gamesystem.h"
#include "system/bmp_winframe.h"
#include "system/nsbtx_to_clwk.h"
#include "sound/pm_sndsys.h"
#include "gamesystem/msgspeed.h"

//  module
#include "app/app_menu_common.h"
#include "pokeicon/pokeicon.h"

//  archive
#include "arc_def.h"
#include "message.naix"
#include "namein_gra.naix"
#include "font/font.naix"
#include "fieldmap/fldmmdl_mdlres.naix" //�t�B�[���h�l��OBJ
#include "msg/msg_namein.h"
#include "wifi_unionobj_plt.cdat" //���j�I��OBJ�̃p���b�g�ʒu
#include "wifi_unionobj.naix"

//  print
#include "print/str_tool.h"
#include "print/gf_font.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "print/global_msg.h"

//  savedata
#include "savedata/misc.h"

//  se
#include "sound/sound_manager.h"

//  mine
#include "namein_snd.h"     //�T�E���h��`
#include "namein_graphic.h" //�O���t�B�b�N�ݒ�
#include "namein_data.h"    //���O���̓f�[�^�i�L�[�z��ƕϊ��j
#include "namein_local.h"
#include "app/name_input.h" //�O���Q��

#include "debug/debug_str_conv.h"

//=============================================================================
/**
 *          �萔�錾
*/
//=============================================================================
//-------------------------------------
/// �}�N���֌W
//=====================================
#ifdef PM_DEBUG
//#define DEBUG_PRINT_ON
#endif //PM_DEBUG

#ifdef DEBUG_PRINT_ON
//���j�R�[�h�f�o�b�O�v�����g
static inline void DEBUG_NAMEIN_Print( STRCODE *x , int len )
{ 
  int i;  
  char str[3] = {0};
  for( i = 0; i < len; i++ )
  {
    DEB_STR_CONV_StrcodeToSjis( &cp_code[i] , str , 1 );
    NAGI_Printf( "%s ", str );
  }
  NAGI_Printf("\n");
}
#else
#define DEBUG_NAMEIN_Print(x,len) /*  */
#endif //DEBUG_PRINT_ON


#define NAMEIN_KEY_TOUCH  //�L�[�ƃ^�b�`�̑���킯����ON

#define NAMEIN_STRINPUT_CENTERING
//�Z���^�����O���Ă������A�ő啶�������P�O�ɂȂ����̂ŁA
//�Z���^�����O�ł��Ȃ��Ȃ��� -> �ő啶�����W�ɂւ�܂����̂Ŗ߂��܂�

//���̕����Ŗ��܂��Ă�����s������
#define NAMEIN_STR_ILLEGAL_STR  (L'�@')

//=============================================================================
/**
 *          �v���g�^�C�v
*/
//=============================================================================
//-------------------------------------
/// �v���Z�X
//=====================================
static GFL_PROC_RESULT NAMEIN_PROC_Init
  ( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT NAMEIN_PROC_Exit
  ( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT NAMEIN_PROC_Main
  ( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
//-------------------------------------
/// BG���\�[�X�Ȃ�
//=====================================
static void BG_Init( BG_WORK *p_wk, HEAPID heapID );
static void BG_Exit( BG_WORK *p_wk );
//-------------------------------------
/// OBJ���\�[�X�Ȃ�
//=====================================
static void OBJ_Init( OBJ_WORK *p_wk, GFL_CLUNIT *p_clunit, HEAPID heapID );
static void OBJ_Exit( OBJ_WORK *p_wk );
static GFL_CLWK *OBJ_GetClwk( const OBJ_WORK *cp_wk, CLWKID clwkID );
//-------------------------------------
/// �p���b�g�A�j��
//=====================================
static void KEYANM_Init( KEYANM_WORK *p_wk, NAMEIN_INPUTTYPE mode, const OBJ_WORK *cp_obj, HEAPID heapID );
static void KEYANM_Exit( KEYANM_WORK *p_wk );
static void KEYANM_Start( KEYANM_WORK *p_wk, KEYANM_TYPE type, const GFL_RECT *cp_rect, BOOL is_shift, NAMEIN_INPUTTYPE mode );
static void KEYANM_Main( KEYANM_WORK *p_wk, NAMEIN_INPUTTYPE mode, const GFL_RECT *cp_rect );
static BOOL KeyAnm_PltFade( u16 *p_buff, u16 *p_cnt, u16 add, u8 plt_num, u8 plt_col, GXRgb start, GXRgb end );
//-------------------------------------
/// STRINPUT
//=====================================
static void STRINPUT_Init( STRINPUT_WORK *p_wk, const STRBUF * cp_default_str, u8 strlen, GFL_FONT *p_font, PRINT_QUE *p_que, const OBJ_WORK *cp_obj, HEAPID heapID );
static void STRINPUT_Exit( STRINPUT_WORK *p_wk );
static void STRINPUT_Main( STRINPUT_WORK *p_wk );
static BOOL STRINPUT_BackSpace( STRINPUT_WORK *p_wk );
static BOOL STRINPUT_SetStr( STRINPUT_WORK *p_wk, STRCODE code );
static BOOL STRINPUT_SetChangeStr( STRINPUT_WORK *p_wk, STRCODE code, BOOL is_shift );
static BOOL STRINPUT_SetChangeSP( STRINPUT_WORK *p_wk, STRINPUT_SP_CHANGE type );
static BOOL STRINPUT_SetChangeAuto( STRINPUT_WORK *p_wk );
static BOOL STRINPUT_PrintMain( STRINPUT_WORK *p_wk );
static void STRINPUT_CopyStr( const STRINPUT_WORK *cp_wk, STRBUF *p_strbuf );
static void STRINPUT_DeleteChangeStr( STRINPUT_WORK *p_wk );
static void STRINPUT_DecideChangeStr( STRINPUT_WORK *p_wk );
static void STRINPUT_Delete( STRINPUT_WORK *p_wk );
static void STRINPUT_SetLongStr( STRINPUT_WORK *p_wk, const STRCODE *cp_code );
static BOOL STRINPUT_IsInputEnd( const STRINPUT_WORK *cp_wk );
static u32  STRINPUT_GetStrLength( const STRINPUT_WORK *cp_wk );
static u32  STRINPUT_GetChangeStrLength( const STRINPUT_WORK *cp_wk );
static BOOL StrInput_ChangeStrToStr( STRINPUT_WORK *p_wk, BOOL is_shift );
static BOOL STRINPUT_IsValidStr( const STRINPUT_WORK *cp_wk);
//-------------------------------------
/// KEYMAP
//=====================================
static void KEYMAP_Create( KEYMAP_WORK *p_wk, NAMEIN_INPUTTYPE mode, NAMEIN_KEYMAP_HANDLE *p_keymap_handle, HEAPID heapID );
static void KEYMAP_Delete( KEYMAP_WORK *p_wk );
static KEYMAP_KEYTYPE KEYMAP_GetKeyInfo( const KEYMAP_WORK *cp_wk, const GFL_POINT *cp_cursor, STRCODE *p_code );
static BOOL KEYMAP_GetTouchCursor( const KEYMAP_WORK *cp_wk, const GFL_POINT *cp_trg, GFL_POINT *p_cursor );
static void KEYMAP_MoveCursor( KEYMAP_WORK *p_wk, GFL_POINT *p_now, const GFL_POINT *cp_add );
static void KEYMAP_Print( const KEYMAP_WORK *cp_wk, PRINT_UTIL *p_util, PRINT_QUE *p_que, GFL_FONT *p_font, HEAPID heapID );
static BOOL KEYMAP_GetRange( const KEYMAP_WORK *cp_wk, const GFL_POINT *cp_cursor, GFL_RECT *p_rect );

static BOOL KEYMAP_GetCursorByKeyType( const KEYMAP_WORK *cp_wk, KEYMAP_KEYTYPE key, GFL_POINT *p_cursor );
static void KEYMAP_TOOL_ChangeCursorPos( GFL_POINT *p_cursor, NAMEIN_INPUTTYPE pre, NAMEIN_INPUTTYPE next );
static BOOL KeyMap_KANA_GetRect( const GFL_POINT *cp_cursor, GFL_RECT *p_rect );
static BOOL KeyMap_QWERTY_GetRect( const GFL_POINT *cp_cursor, GFL_RECT *p_rect );
static void KeyMap_KANA_GetMoveCursor( GFL_POINT *p_now, const GFL_POINT *cp_add, KEYMOVE_BUFF *p_buff );
static void KeyMap_QWERTY_GetMoveCursor( GFL_POINT *p_now, const GFL_POINT *cp_add, KEYMOVE_BUFF *p_buff );
static KEYMAP_KEYTYPE KeyMap_KANA_GetKeyType( const GFL_POINT *cp_cursor, GFL_POINT *p_strpos );
static KEYMAP_KEYTYPE KeyMap_KIGOU_GetKeyType( const GFL_POINT *cp_cursor, GFL_POINT *p_strpos );
static KEYMAP_KEYTYPE KeyMap_QWERTY_GetKeyType( const GFL_POINT *cp_cursor, GFL_POINT *p_strpos );
//-------------------------------------
/// KEYBOARD_WORK
//=====================================
static void KEYBOARD_Init( KEYBOARD_WORK *p_wk, NAMEIN_INPUTTYPE mode, GFL_FONT *p_font, PRINT_QUE *p_que, NAMEIN_KEYMAP_HANDLE *p_keymap_handle, const OBJ_WORK *cp_obj, HEAPID heapID );
static void KEYBOARD_Exit( KEYBOARD_WORK *p_wk );
static void KEYBOARD_Main( KEYBOARD_WORK *p_wk, const STRINPUT_WORK *cp_strinput );
static KEYBOARD_STATE KEYBOARD_GetState( const KEYBOARD_WORK *cp_wk );
static KEYBOARD_INPUT KEYBOARD_GetInput( const KEYBOARD_WORK *cp_wk, STRCODE *p_str );
static BOOL KEYBOARD_IsShift( const KEYBOARD_WORK *cp_wk );
static BOOL KEYBOARD_PrintMain( KEYBOARD_WORK *p_wk );
static NAMEIN_INPUTTYPE KEYBOARD_GetInputType( const KEYBOARD_WORK *cp_wk );
static BOOL Keyboard_StartMove( KEYBOARD_WORK *p_wk, NAMEIN_INPUTTYPE mode );
static BOOL Keyboard_MainMove( KEYBOARD_WORK *p_wk );
static void Keyboard_ChangeMode( KEYBOARD_WORK *p_wk, NAMEIN_INPUTTYPE mode );

typedef struct
{ 
  KEYMAP_KEYTYPE  type;
  BOOL            is_push;    //�{�^����������
  GFL_POINT       anm_pos;
}KEYBOARD_INPUT_REQUEST;
//�ʏ펞
static BOOL Keyboard_KeyReq( KEYBOARD_WORK *p_wk, KEYBOARD_INPUT_REQUEST *p_req );
static BOOL Keyboard_BtnReq( KEYBOARD_WORK *p_wk, KEYBOARD_INPUT_REQUEST *p_req );
static BOOL Keyboard_TouchReq( KEYBOARD_WORK *p_wk, KEYBOARD_INPUT_REQUEST *p_req );
static void Keyboard_Decide( KEYBOARD_WORK *p_wk, const KEYBOARD_INPUT_REQUEST *cp_req );
//���[�h�ؑ֎�
static BOOL Keyboard_Move_KeyReq( KEYBOARD_WORK *p_wk, KEYBOARD_INPUT_REQUEST *p_req );
static BOOL Keyboard_Move_BtnReq( KEYBOARD_WORK *p_wk, KEYBOARD_INPUT_REQUEST *p_req );
static BOOL Keyboard_Move_TouchReq( KEYBOARD_WORK *p_wk, KEYBOARD_INPUT_REQUEST *p_req );
static void Keyboard_Move_Decide( KEYBOARD_WORK *p_wk, const KEYBOARD_INPUT_REQUEST *cp_req );

//-------------------------------------
/// MSGWND
//=====================================
static void MSGWND_Init( MSGWND_WORK* p_wk, GFL_FONT *p_font, GFL_MSGDATA *p_msg, PRINT_QUE *p_que, WORDSET *p_word, HEAPID heapID );
static void MSGWND_Exit( MSGWND_WORK* p_wk );
static void MSGWND_Print( MSGWND_WORK* p_wk, u32 strID );
static void MSGWND_ExpandPrintPoke( MSGWND_WORK *p_wk, u32 strID, u16 mons_no, u16 form, HEAPID heapID );
static void MSGWND_ExpandPrintPP( MSGWND_WORK *p_wk, u32 strID, const POKEMON_PARAM *cp_pp );
static BOOL MSGWND_PrintMain( MSGWND_WORK* p_wk );

//-------------------------------------
/// PS(PrintStream)
//=====================================
static void PS_Init( PS_WORK* p_wk, HEAPID heapID );
static void PS_Exit( PS_WORK* p_wk );
static void PS_SetupBox( PS_WORK* p_wk, MSGWND_WORK* p_msgwnd_wk, NAMEIN_PARAM* p_param, HEAPID heapID );

//-------------------------------------
/// ICON
//=====================================
static void ICON_Init( ICON_WORK *p_wk, ICON_TYPE type, u32 param1, u32 param2, GFL_CLUNIT *p_unit, GFL_FONT *p_font, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, BMPOAM_SYS_PTR p_bmpoam_sys, HEAPID heapID );
static void ICON_Exit( ICON_WORK *p_wk );
static void ICON_Main( ICON_WORK *p_wk );
static void ICON_PrintMain( ICON_WORK *p_wk );
static BOOL ICON_IsTrg( const ICON_WORK *cp_wk );
static void Icon_StartMove( ICON_WORK *p_wk );
static BOOL Icon_MainMove( ICON_WORK *p_wk );

static ICON_TYPE ICON_GetModoToType( NAMEIN_MODE mode, int param1 );
//-------------------------------------
/// ���̑�
//=====================================
static BOOL COLLISION_IsRectXPos( const GFL_RECT *cp_rect, const GFL_POINT *cp_pos );
static STRBUF* INITNAME_CreateStr( const NAMEIN_WORK *cp_wk, NAMEIN_MODE mode, HEAPID heapID );
static void FinishInput( NAMEIN_WORK *p_wk );
static BOOL CanDecide( NAMEIN_WORK *p_wk );
//-------------------------------------
/// SEQ
//=====================================
static void SEQ_Init( SEQ_WORK *p_wk, void *p_param, SEQ_FUNCTION seq_function );
static void SEQ_Exit( SEQ_WORK *p_wk );
static void SEQ_Main( SEQ_WORK *p_wk );
static BOOL SEQ_IsEnd( const SEQ_WORK *cp_wk );
static void SEQ_SetNext( SEQ_WORK *p_wk, SEQ_FUNCTION seq_function );
static void SEQ_End( SEQ_WORK *p_wk );
//-------------------------------------
/// SEQFUNC
//=====================================
static void SEQFUNC_WaitPrint( SEQ_WORK *p_seqwk, int *p_seq, void *p_param );
static void SEQFUNC_FadeOut( SEQ_WORK *p_seqwk, int *p_seq, void *p_param );
static void SEQFUNC_FadeIn( SEQ_WORK *p_seqwk, int *p_seq, void *p_param );
static void SEQFUNC_PrintStream( SEQ_WORK *p_seqwk, int *p_seq, void *p_param );
static void SEQFUNC_Main( SEQ_WORK *p_seqwk, int *p_seq, void *p_param );
static void SEQFUNC_End( SEQ_WORK *p_seqwk, int *p_seq, void *p_param );
static void SEQFUNC_SaveEnd( SEQ_WORK *p_seqwk, int *p_seq, void *p_param );
//=============================================================================
/**
 *          �O���Q��
*/
//=============================================================================
//-------------------------------------
/// �v���Z�X�e�[�u��
//=====================================
const GFL_PROC_DATA NameInputProcData =
{ 
  NAMEIN_PROC_Init,
  NAMEIN_PROC_Main,
  NAMEIN_PROC_Exit,
};

//-------------------------------------
/// SE
//=====================================
const u32 NAMEIN_SE_PresetData[]  =
{ 
  NAMEIN_SE_MOVE_CURSOR,
  NAMEIN_SE_DELETE_STR,
  NAMEIN_SE_DECIDE_STR,
  NAMEIN_SE_CHANGE_MODE,
  NAMEIN_SE_DECIDE,
  NAMEIN_SE_BEEP,
};

const u32 NAMEIN_SE_PresetNum = 6;
//=============================================================================
/**
 *          proc
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  ���O����  ���C���v���Z�X������
 *
 *  @param  GFL_PROC *p_proc  �v���Z�X
 *  @param  *p_seq            �V�[�P���X
 *  @param  *p_param          �e���[�N
 *  @param  *p_work           ���[�N
 *
 *  @return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT NAMEIN_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{ 
  NAMEIN_WORK   *p_wk;
  NAMEIN_PARAM  *p_param;
  NAMEIN_INPUTTYPE  mode  = NAMEIN_INPUTTYPE_KANA;

  u32 heap_size;
  GFL_FONT_LOADTYPE font_load;
  GflMsgLoadType    msg_load;

  //�����󂯎��
  p_param = p_param_adrs;

  //�Z�[�u�f�[�^�󂯎��
  if( p_param->p_misc )
  { 
    mode    = MISC_GetNameInMode( p_param->p_misc, p_param->mode );
  }
  
  //�Ȃ��������P���B
  //�C���g���ŃZ�[�u���Ȃ��疼�O���͂�ʉ߂��邽�߁A
  //���ׂĂ��I���������ɂ��Ȃ���΂Ȃ炸�A�傫���q�[�v��p�ӂ��Ă��܂����B
  //�C���g���ȊO�ŌĂяo�����Ƃ��q�[�v������Ȃ����Ƃ�z�肵�Đ؂�ւ�����悤�ɂ����̂ł����A
  //�q�[�v�T�C�Y���ς���Ă��܂��ƃf�o�b�O���ɉe�����ł邽�߁A
  //�؂�ւ����Ȃ��ɂȂ�܂����B���̖��c�ł��B
  //�����ł͂����ƃq�[�v�������Ȃ邱�Ƃ�\�z���Đ؂蕪���邵���݂����͎c���Ă����܂��B
  if( 1 )
  { 
    heap_size = NAMEIN_HEAP_HIGH_SIZE;
    font_load = GFL_FONT_LOADTYPE_MEMORY;
    msg_load  = GFL_MSG_LOAD_FAST;
  }
  else
  { 
    heap_size = NAMEIN_HEAP_LOW_SIZE;
    font_load = GFL_FONT_LOADTYPE_FILE;
    msg_load  = GFL_MSG_LOAD_NORMAL;
  }


  //�q�[�v�쐬
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_NAME_INPUT, heap_size );

  //�v���Z�X���[�N�쐬
  p_wk  = GFL_PROC_AllocWork( p_proc, sizeof(NAMEIN_WORK), HEAPID_NAME_INPUT );
  GFL_STD_MemClear( p_wk, sizeof(NAMEIN_WORK) );

  //������
  p_wk->p_param = p_param;

  //���ʃ��W���[���̍쐬
  p_wk->p_font    = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
      font_load, FALSE, HEAPID_NAME_INPUT );
  p_wk->p_que     = PRINTSYS_QUE_CreateEx( 4096, HEAPID_NAME_INPUT );
  p_wk->p_msg   = GFL_MSG_Create( msg_load, ARCID_MESSAGE, 
                        NARC_message_namein_dat, HEAPID_NAME_INPUT );
  p_wk->p_word  = WORDSET_Create( HEAPID_NAME_INPUT );

  //�O���t�B�b�N�ݒ�
  p_wk->p_graphic = NAMEIN_GRAPHIC_Init( 0, HEAPID_NAME_INPUT );
  p_wk->p_bmpoam_sys  = BmpOam_Init(HEAPID_NAME_INPUT, NAMEIN_GRAPHIC_GetClunit( p_wk->p_graphic ));

  //�ǂ݂���
  BG_Init( &p_wk->bg, HEAPID_NAME_INPUT );
  { 
    GFL_CLUNIT  *p_clunit = NAMEIN_GRAPHIC_GetClunit( p_wk->p_graphic );
    OBJ_Init( &p_wk->obj, p_clunit, HEAPID_NAME_INPUT );
    ICON_Init( &p_wk->icon, ICON_GetModoToType(p_param->mode,p_param->param1), p_param->param1, p_param->param2, p_clunit, p_wk->p_font, p_wk->p_que, p_wk->p_msg, p_wk->p_bmpoam_sys, HEAPID_NAME_INPUT );
  }
  p_wk->p_keymap_handle = NAMEIN_KEYMAP_HANDLE_Alloc( HEAPID_NAME_INPUT );

  //�����\������
  { 
    p_wk->p_initdraw_str  = GFL_STR_CreateCopyBuffer( p_param->strbuf, HEAPID_NAME_INPUT );

    if( GFL_STR_GetBufferLength( p_wk->p_initdraw_str ) > 0 )
    { 
      if( !NAMEIN_DATA_CheckInputStr( 
            GFL_STR_GetStringCodePointer( p_wk->p_initdraw_str ), HEAPID_NAME_INPUT ) )
      { 
        GFL_STR_ClearBuffer( p_wk->p_initdraw_str );
      }
    }
  }

  //���W���[���쐬
  SEQ_Init( &p_wk->seq, p_wk, SEQFUNC_WaitPrint );
  STRINPUT_Init( &p_wk->strinput, p_wk->p_initdraw_str, p_param->wordmax, p_wk->p_font, p_wk->p_que, &p_wk->obj, HEAPID_NAME_INPUT );
  KEYBOARD_Init( &p_wk->keyboard, mode, p_wk->p_font, p_wk->p_que, p_wk->p_keymap_handle, &p_wk->obj, HEAPID_NAME_INPUT );
  MSGWND_Init( &p_wk->msgwnd, p_wk->p_font, p_wk->p_msg, p_wk->p_que, p_wk->p_word, HEAPID_NAME_INPUT );
  PS_Init( &p_wk->ps, HEAPID_NAME_INPUT );
  p_wk->p_prof  = PROF_WORD_AllocWork( HEAPID_NAME_INPUT );

  //�����`��
  if( p_param->mode == NAMEIN_POKEMON )
  { 
    //�|�P�����̏ꍇ�́A�푰����P��o�^
    if( p_param->pp == NULL )
    { 
      MSGWND_ExpandPrintPoke( &p_wk->msgwnd, NAMEIN_MSG_INFO_000 + p_param->mode, p_param->param1, p_param->param2, HEAPID_NAME_INPUT );
    }
    else
    { 
      MSGWND_ExpandPrintPP( &p_wk->msgwnd, NAMEIN_MSG_INFO_000 + p_param->mode, p_param->pp );
    }
  }
  else
  { 
    //���͒ʏ�
    MSGWND_Print( &p_wk->msgwnd, NAMEIN_MSG_INFO_000 + p_param->mode );
  }

  if( GFL_NET_IsInit() )
  { 
    GFL_NET_ChangeIconPosition(GFL_WICON_POSX,GFL_WICON_POSY);
    GFL_NET_WirelessIconEasy_HoldLCD( TRUE, HEAPID_NAME_INPUT );
    GFL_NET_ReloadIcon();
  }

  if( p_wk->p_param->p_intr_sv )
  { 
    IntrSave_Resume( p_wk->p_param->p_intr_sv );
  }


  return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���O����  ���C���v���Z�X�j��
 *
 *  @param  GFL_PROC *p_proc  �v���Z�X
 *  @param  *p_seq            �V�[�P���X
 *  @param  *p_param          �e���[�N
 *  @param  *p_work           ���[�N
 *
 *  @return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT NAMEIN_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{ 
  NAMEIN_WORK *p_wk = p_wk_adrs;

  //���W���[���j��
  PROF_WORD_FreeWork( p_wk->p_prof );
  PS_Exit( &p_wk->ps );
  MSGWND_Exit( &p_wk->msgwnd );
  KEYBOARD_Exit( &p_wk->keyboard );

  GFL_STR_DeleteBuffer( p_wk->p_initdraw_str );
  NAMEIN_KEYMAP_HANDLE_Free( p_wk->p_keymap_handle );
  STRINPUT_Exit( &p_wk->strinput );
  SEQ_Exit( &p_wk->seq );

  //�O���t�B�b�N�j��
  ICON_Exit( &p_wk->icon );
  OBJ_Exit( &p_wk->obj );
  BG_Exit( &p_wk->bg );

  BmpOam_Exit( p_wk->p_bmpoam_sys );
  NAMEIN_GRAPHIC_Exit( p_wk->p_graphic );

  //���ʃ��W���[���̔j��
  WORDSET_Delete( p_wk->p_word );
  GFL_MSG_Delete( p_wk->p_msg );
  PRINTSYS_QUE_Clear( p_wk->p_que );
  PRINTSYS_QUE_Delete( p_wk->p_que );
  GFL_FONT_Delete( p_wk->p_font );

  //�v���Z�X���[�N�j��
  GFL_PROC_FreeWork( p_proc );
  //�q�[�v�j��
  GFL_HEAP_DeleteHeap( HEAPID_NAME_INPUT );

  
  return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���O����  ���C���v���Z�X���C������
 *
 *  @param  GFL_PROC *p_proc  �v���Z�X
 *  @param  *p_seq            �V�[�P���X
 *  @param  *p_param          �e���[�N
 *  @param  *p_work           ���[�N
 *
 *  @return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT NAMEIN_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{ 
  NAMEIN_WORK *p_wk = p_wk_adrs;

  //�V�[�P���X
  SEQ_Main( &p_wk->seq );

  //�`��
  NAMEIN_GRAPHIC_2D_Draw( p_wk->p_graphic );

  //�v�����g
  PRINTSYS_QUE_Main( p_wk->p_que );
  ICON_PrintMain( &p_wk->icon );

  //�I��
  if( SEQ_IsEnd( &p_wk->seq ) )
  { 
    return GFL_PROC_RES_FINISH;
  }
  else
  { 
    return GFL_PROC_RES_CONTINUE;
  }
}
//=============================================================================
/**
 *    BG���\�[�X�ǂ݂���
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  BG���\�[�X�ǂ݂���
 *
 *  @param  BG_WORK *p_wk ���[�N
 *  @param  heapID        �q�[�vID
 */
//-----------------------------------------------------------------------------
static void BG_Init( BG_WORK *p_wk, HEAPID heapID )
{ 
  { 
    ARCHANDLE * p_handle  = GFL_ARC_OpenDataHandle( ARCID_NAMEIN_GRA, heapID );

    //����
    GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_namein_gra_name_bg_NCLR,
        PALTYPE_SUB_BG, PLT_BG_BACK_S*0x20, 0, heapID );
    //CHR
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_namein_gra_name_bg_NCGR, 
        BG_FRAME_BACK_S, 0, 0, FALSE, heapID );
    //SCR
    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_namein_gra_name_bgu_NSCR,
        BG_FRAME_BACK_S, 0, 0, FALSE, heapID );

    //�����
    //PLT
    GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_namein_gra_name_bg_NCLR,
        PALTYPE_MAIN_BG, PLT_BG_STR_M*0x20, 0, heapID );
    GFL_ARCHDL_UTIL_TransVramPaletteEx( p_handle, NARC_namein_gra_name_bg_NCLR,
        PALTYPE_MAIN_BG, PLT_BG_KEY_MOVE_M*0x20, PLT_BG_KEY_DECIDE_M*0x20, 0x20, heapID );
    GFL_ARCHDL_UTIL_TransVramPaletteEx( p_handle, NARC_namein_gra_name_bg_NCLR,
        PALTYPE_MAIN_BG, PLT_BG_KEY_MOVE_M*0x20, PLT_BG_KEY_PRESS_M*0x20, 0x20, heapID );

    //CHR
    GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_namein_gra_name_bg_NCGR, 
        BG_FRAME_STR_M, 0, 0, FALSE, heapID );

    //SCR
    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_namein_gra_name_base_NSCR,
        BG_FRAME_STR_M, 0, 0, FALSE, heapID );
    GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_namein_gra_name_btn_NSCR,
        BG_FRAME_BTN_M, 0, 0, FALSE, heapID );
  
    GFL_ARC_CloseDataHandle( p_handle );
  }

  { 
    ARCHANDLE * p_handle  = GFL_ARC_OpenDataHandle( ARCID_FONT, heapID );


    //���ʃt�H���g�p���b�g
    GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_font_talkwin_nclr,
        PALTYPE_SUB_BG, PLT_BG_FONT_S*0x20, 0x20, heapID );

    GFL_ARC_CloseDataHandle( p_handle );
  }

  { 
    //���ʃe�L�X�g�g
    GFL_BG_FillCharacter( BG_FRAME_TEXT_S, 0, 1,  0 );
    BmpWinFrame_GraphicSet(
        BG_FRAME_TEXT_S, MSGWND_FRAME_CHR, PLT_BG_FRAME_S, MENU_TYPE_SYSTEM, heapID );
  }

  //GFL_BG_SetBackGroundColor( BG_FRAME_TEXT_S, 0 );
}
//----------------------------------------------------------------------------
/**
 *  @brief  BG���\�[�X�j��
 *
 *  @param  BG_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void BG_Exit( BG_WORK *p_wk )
{ 
  GFL_BG_FillCharacterRelease( BG_FRAME_TEXT_S, 1, 0 );
}
//----------------------------------------------------------------------------
/**
 *  @brief  BG�p���b�g�t�F�[�h
 *
 *  @param  u16 *p_buff �F�ۑ��o�b�t�@
 *  @param  *p_cnt      �J�E���^�o�b�t�@
 *  @param  add         �J�E���^���Z�l
 *  @param  plt_num     �p���b�g�c�ԍ�
 *  @param  plt_col     �p���b�g���ԍ�
 *  @param  start       �J�n�F
 *  @param  end         �I���F
 */
//-----------------------------------------------------------------------------
static void BG_MainPltAnm( u16 *p_buff, u16 *p_cnt, u16 add, u8 plt_num, u8 plt_col, GXRgb start, GXRgb end )
{ 
  //�p���b�g�A�j��
  if( *p_cnt + add >= 0x10000 )
  {
    *p_cnt = *p_cnt+add-0x10000;
  }
  else
  {
    *p_cnt += add;
  }
  {
    //1�`0�ɕϊ�
    const fx16 cos = (FX_CosIdx(*p_cnt)+FX16_ONE)/2;
    const u8 start_r  = (start & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
    const u8 start_g  = (start & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
    const u8 start_b  = (start & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;
    const u8 end_r  = (end & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
    const u8 end_g  = (end & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
    const u8 end_b  = (end & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;

    const u8 r = start_r + (((end_r-start_r)*cos)>>FX16_SHIFT);
    const u8 g = start_g + (((end_g-start_g)*cos)>>FX16_SHIFT);
    const u8 b = start_b + (((end_b-start_b)*cos)>>FX16_SHIFT);
    
    *p_buff = GX_RGB(r, g, b);
    
    NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_BG_PLTT_MAIN,
                                        plt_num * 32 + plt_col * 2,
                                        p_buff, 2 );
  }
}
//=============================================================================
/**
 *    OBJ���\�[�X�ǂ݂���
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  OBJ���\�[�X�ǂ݂���
 *
 *  @param  OBJ_WORK *p_wk  ���[�N
 *  @param  clunit          ���j�b�g
 *  @param  HEAPID heapID   �q�[�vID
 */
//-----------------------------------------------------------------------------
static void OBJ_Init( OBJ_WORK *p_wk, GFL_CLUNIT *p_clunit, HEAPID heapID )
{ 
  //���\�[�X�ǂ݂���
  { 
    ARCHANDLE * p_handle  = GFL_ARC_OpenDataHandle( ARCID_NAMEIN_GRA, heapID );

    p_wk->res[ RESID_OBJ_COMMON_PLT ] = GFL_CLGRP_PLTT_Register( p_handle, 
        NARC_namein_gra_name_obj_NCLR, CLSYS_DRAW_MAIN, PLT_OBJ_CURSOR_M*0x20, heapID );
    p_wk->res[ RESID_OBJ_COMMON_CEL ] = GFL_CLGRP_CELLANIM_Register( p_handle,
        NARC_namein_gra_name_obj_NCER, NARC_namein_gra_name_obj_NANR, heapID );
    p_wk->res[ RESID_OBJ_COMMON_CHR ] = GFL_CLGRP_CGR_Register( p_handle,
        NARC_namein_gra_name_obj_NCGR, FALSE, CLSYS_DRAW_MAIN, heapID );

    GFL_ARC_CloseDataHandle( p_handle );
  }
  //CLWK�o�^
  {
    int i;
    GFL_CLWK_DATA cldata;
    GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );

    //�o�[
    cldata.anmseq = 0;
    for( i = CLWKID_BAR_TOP; i < CLWKID_BAR_END; i++ )
    { 
      p_wk->p_clwk[i] =   GFL_CLACT_WK_Create( p_clunit,
          p_wk->res[RESID_OBJ_COMMON_CHR],
          p_wk->res[RESID_OBJ_COMMON_PLT],
          p_wk->res[RESID_OBJ_COMMON_CEL],
          &cldata,
          CLSYS_DEFREND_MAIN,
          heapID );
    }

    //�p�\�R��
    cldata.anmseq = 1;
    p_wk->p_clwk[CLWKID_PC] =   GFL_CLACT_WK_Create( p_clunit,
        p_wk->res[RESID_OBJ_COMMON_CHR],
        p_wk->res[RESID_OBJ_COMMON_PLT],
        p_wk->res[RESID_OBJ_COMMON_CEL],
        &cldata,
        CLSYS_DEFREND_MAIN,
        heapID );
    GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[CLWKID_PC], FALSE );

    //�J�[�\��
    for( i = CLWKID_CURSOR_TOP; i < CLWKID_CURSOR_END; i++ )
    { 
      cldata.pos_x  = -32;  //�ŏ�������̂ł������Ă���
      cldata.pos_y  = -32;
      cldata.anmseq = 3 + i - CLWKID_CURSOR_TOP;
      p_wk->p_clwk[i] =   GFL_CLACT_WK_Create( p_clunit,
          p_wk->res[RESID_OBJ_COMMON_CHR],
          p_wk->res[RESID_OBJ_COMMON_PLT],
          p_wk->res[RESID_OBJ_COMMON_CEL],
          &cldata,
          CLSYS_DEFREND_MAIN,
          heapID );
      GFL_CLACT_WK_SetAutoAnmFlag( p_wk->p_clwk[i], TRUE );
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[i], FALSE );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  OBJ���\�[�X�j��
 *
 *  @param  BG_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void OBJ_Exit( OBJ_WORK *p_wk )
{ 
  //CLWK�j��
  { 
    int i;
    for( i = 0; i < CLWKID_MAX; i++ )
    { 
      if( p_wk->p_clwk[i] )
      { 
        GFL_CLACT_WK_Remove( p_wk->p_clwk[i] );
      }
    }
  }

  //���\�[�X�j��
  { 
    GFL_CLGRP_PLTT_Release( p_wk->res[RESID_OBJ_COMMON_PLT] );
    GFL_CLGRP_CGR_Release( p_wk->res[RESID_OBJ_COMMON_CHR] );
    GFL_CLGRP_CELLANIM_Release( p_wk->res[RESID_OBJ_COMMON_CEL] );
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  CLWK�擾
 *
 *  @param  const OBJ_WORK *p_wk    ���[�N
 *  @param  clwkID                  CLWK�ԍ�
 *
 *  @return CLWK
 */
//-----------------------------------------------------------------------------
static GFL_CLWK *OBJ_GetClwk( const OBJ_WORK *cp_wk, CLWKID clwkID )
{ 
  return cp_wk->p_clwk[ clwkID ];
}
//=============================================================================
/**
 *          KEYANM
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  �L�[�A�j��  ������
 *
 *  @param  KEYANM_WORK *p_wk ���[�N
 *  @param  mode              ���[�h
 *  @param  heapID            �q�[�vID
 */
//-----------------------------------------------------------------------------
static void KEYANM_Init( KEYANM_WORK *p_wk, NAMEIN_INPUTTYPE mode, const OBJ_WORK *cp_obj, HEAPID heapID )
{ 
  void *p_buff;
  NNSG2dPaletteData *p_plt;

  //�N���A
  GFL_STD_MemClear( p_wk, sizeof(KEYANM_WORK) );
  p_wk->mode  = mode;

  //OBJ�擾
  {
    int i;
    for( i = 0; i < OBJ_CURSOR_MAX; i++ )
    { 
      p_wk->p_cursor[i] = OBJ_GetClwk( cp_obj, CLWKID_CURSOR_TOP + i );
    }
  }

  //���Ƃ̃p���b�g����F����ۑ�
  p_buff  = GFL_ARC_UTIL_LoadPalette( ARCID_NAMEIN_GRA, NARC_namein_gra_name_bg_NCLR, &p_plt, heapID );

  { 
    int i;
    const u16 *cp_plt_adrs;

    cp_plt_adrs = p_plt->pRawData;
    //�P���C����
    for( i = 0; i < 0x10; i++ )
    { 
      p_wk->plt_normal[i] = cp_plt_adrs[i+0x10];
    }
    //�Q���C����
    for( i = 0; i < 0x10; i++ )
    { 
      p_wk->plt_flash[i]  = cp_plt_adrs[i+0x20];
    }
  }

  //�p���b�g�j��
  GFL_HEAP_FreeMemory( p_buff );

}
//----------------------------------------------------------------------------
/**
 *  @brief  �L�[�A�j��  �j��
 *
 *  @param  KEYANM_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void KEYANM_Exit( KEYANM_WORK *p_wk )
{ 
  //�N���A
  GFL_STD_MemClear( p_wk, sizeof(KEYANM_WORK) );
}
//----------------------------------------------------------------------------
/**
 *  @brief  �L�[�A�j��  �J�n
 *
 *  @param  KEYANM_WORK *p_wk   ���[�N
 *  @param  type                �ړ��^�C�v
 *  @param  GFL_RECT *cp_rect   ��`
 *  @param  is_shift            �V�t�g�������Ă��邩
 *  @param  mode                ���[�h
 */
//-----------------------------------------------------------------------------
static void KEYANM_Start( KEYANM_WORK *p_wk, KEYANM_TYPE type, const GFL_RECT *cp_rect, BOOL is_shift, NAMEIN_INPUTTYPE mode )
{ 
  p_wk->is_start  = FALSE;
  p_wk->type      = type;
  p_wk->is_shift  = is_shift;
  p_wk->mode      = mode;

  //���̃L�[��S�Č��ɖ߂�
  GFL_BG_ChangeScreenPalette( BG_FRAME_BTN_M, 0, 0, 32, 24, PLT_BG_KEY_NORMAL_M );
  GFL_BG_ChangeScreenPalette( BG_FRAME_KEY_M, 0, 0, 32, 24, PLT_BG_KEY_NORMAL_M );

  //SHIFT�����Ă���΁A������F�ς�
  if( is_shift && mode == NAMEIN_INPUTTYPE_QWERTY )
  { 
    GFL_BG_ChangeScreenPalette( BG_FRAME_KEY_M, 3, 16,
      4, 3, PLT_BG_KEY_PRESS_M );
    GFL_BG_ChangeScreenPalette( BG_FRAME_KEY_M, 23, 16,
      4, 3, PLT_BG_KEY_PRESS_M );
  }

  //���[�h�̃{�^����F�ς�
  { 
    GFL_BG_ChangeScreenPalette( BG_FRAME_KEY_M, 3 + mode * 2, 20,
      2, 3, PLT_BG_KEY_PRESS_M );
    GFL_BG_ChangeScreenPalette( BG_FRAME_BTN_M, 3 + mode * 2, 20,
      2, 3, PLT_BG_KEY_PRESS_M );
  }

  //�L�[�̐F�𒣑ւ�
  //�����Ă����ɂ����Ƃ��킩��Ȃ��̂ŁA
  //���݂̃L�[�̐F�ς���D�悵���
  if( cp_rect && type != KEYANM_TYPE_NONE )
  { 
    p_wk->range = *cp_rect;
    
    //����̂Ƃ���������Ȃ�
    if( type == KEYANM_TYPE_DECIDE )
    { 
      GFL_BG_ChangeScreenPalette( BG_FRAME_BTN_M, cp_rect->left, cp_rect->top,
          cp_rect->right - cp_rect->left, cp_rect->bottom - cp_rect->top,
          PLT_BG_KEY_NORMAL_M + type );

      GFL_BG_ChangeScreenPalette( BG_FRAME_KEY_M, cp_rect->left, cp_rect->top,
          cp_rect->right - cp_rect->left, cp_rect->bottom - cp_rect->top,
          PLT_BG_KEY_NORMAL_M + type );
    }

    //OBJ��\�����邱�ƂɂȂ����̂ŁAcp_rect����obj�̍��W�֕ϊ�
    if( GFL_UI_CheckTouchOrKey() == GFL_APP_KTST_KEY )
    { 
      const u32 x_ofs = 7;
      const u32 y_ofs = 3;

      p_wk->cursor_pos[OBJ_CURSOR_L].x = cp_rect->left * 8 + x_ofs;
      p_wk->cursor_pos[OBJ_CURSOR_L].y = cp_rect->top * 8 + (cp_rect->bottom - cp_rect->top) * 4 - y_ofs;
      p_wk->cursor_pos[OBJ_CURSOR_R].x = cp_rect->right * 8 - x_ofs;
      p_wk->cursor_pos[OBJ_CURSOR_R].y = cp_rect->top * 8 + (cp_rect->bottom - cp_rect->top) * 4 - y_ofs;

      GFL_CLACT_WK_ResetAnm( p_wk->p_cursor[OBJ_CURSOR_L] );
      GFL_CLACT_WK_ResetAnm( p_wk->p_cursor[OBJ_CURSOR_R] );
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_cursor[OBJ_CURSOR_L], TRUE );
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_cursor[OBJ_CURSOR_R], TRUE );
    }
  }

  if( type == KEYANM_TYPE_NONE )
  { 
    GFL_CLACT_WK_SetDrawEnable( p_wk->p_cursor[OBJ_CURSOR_L], FALSE );
    GFL_CLACT_WK_SetDrawEnable( p_wk->p_cursor[OBJ_CURSOR_R], FALSE );
  }

  //�X�N���[�����N�G�X�g
  GFL_BG_LoadScreenReq( BG_FRAME_BTN_M );
  GFL_BG_LoadScreenReq( BG_FRAME_KEY_M );


  if( type != KEYANM_TYPE_NONE )
  { 
    //�J�E���g�o�b�t�@�N���A
    GFL_STD_MemClear( p_wk->cnt, sizeof(u16)*0x10 );
    p_wk->decide_cnt  = 0;

    //�A�j���J�n
    p_wk->is_start  = TRUE;
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  �L�[�A�j��  ���C������
 *
 *  @param  KEYANM_WORK *p_wk ���[�N
 *  @param  NAMEIN_INPUTTYPE  ���݂̃��[�h
 *  @param  RECT  �����̍ŐV�̃J�[�\���������`
 */
//-----------------------------------------------------------------------------
static void KEYANM_Main( KEYANM_WORK *p_wk, NAMEIN_INPUTTYPE mode, const GFL_RECT *cp_rect )
{ 
  enum
  { 
    KEY_MOVE_PLTFADE_SPEED  = 0x51E,
  };

  u8 plt_num;
  u16 add;
  int i;
  BOOL is_end;

  static int s_add = KEY_MOVE_PLTFADE_SPEED;
#if 0
  if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP )
  { 
    s_add += 0x1;
    OS_Printf( "anm speed 0x%x\n", s_add );
  }
  else if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN )
  { 
    s_add -= 0x1;
    OS_Printf( "anm speed 0x%x\n", s_add );
  }
#endif

  //���[�h�ʂ�������
  if( p_wk->is_start )
  { 
    //���[�h�ʏ���
    switch( p_wk->type )
    { 
    case KEYANM_TYPE_MOVE:  
      plt_num = PLT_BG_KEY_MOVE_M;
      add     = s_add;
      break;

    case KEYANM_TYPE_DECIDE:
      plt_num = PLT_BG_KEY_DECIDE_M;
      add     = 0xFFFF/10;
      //�I�������玩���Ń��[�h�ؑ�
      if( p_wk->decide_cnt++ > KEYANM_DECIDE_CNT )
      { 
        //�L�[���[�h���L�[�Ȃ�΃J�[�\���ʒu������
        if( GFL_UI_CheckTouchOrKey() == GFL_APP_KTST_KEY )
        { 
          //���[�h�{�^��+�J�[�\���ʒu�����̂���
          KEYANM_Start( p_wk, KEYANM_TYPE_MOVE, cp_rect, p_wk->is_shift, mode );
        }
        else
        { 
          //���[�h�{�^�������̂���
          KEYANM_Start( p_wk, KEYANM_TYPE_NONE, cp_rect, p_wk->is_shift, mode );
        }
      }
      break;
    case KEYANM_TYPE_NONE:
      GF_ASSERT(0);
      break;
    }

    //���ꂼ��̃p���b�g���A�j��
    for( i = 0; i < 0x10; i++ )
    { 
      KeyAnm_PltFade( &p_wk->color[i], &p_wk->cnt[i], add, plt_num, i, p_wk->plt_normal[i], p_wk->plt_flash[i] );
    }
  }

  //OBJ���W�ݒ�iBG�X�N���[���ɍ��킹��j
  { 
    GFL_CLACTPOS  pos;
    pos = p_wk->cursor_pos[OBJ_CURSOR_L];
    pos.y -= GFL_BG_GetScrollY( BG_FRAME_BTN_M ) * 8;
    GFL_CLACT_WK_SetPos( p_wk->p_cursor[OBJ_CURSOR_L], &pos, CLSYS_DEFREND_MAIN );
    pos = p_wk->cursor_pos[OBJ_CURSOR_R];
    pos.y -= GFL_BG_GetScrollY( BG_FRAME_BTN_M ) * 8;
    GFL_CLACT_WK_SetPos( p_wk->p_cursor[OBJ_CURSOR_R], &pos, CLSYS_DEFREND_MAIN );
  }

  //���[�h�ؑ֎��̃��t���b�V��
  if( p_wk->mode != mode )
  { 
    //�L�[���[�h���L�[�Ȃ�΃J�[�\���ʒu������
    if( GFL_UI_CheckTouchOrKey() == GFL_APP_KTST_KEY )
    { 
      //���[�h�{�^��+�J�[�\���ʒu�����̂���
      KEYANM_Start( p_wk, KEYANM_TYPE_MOVE, &p_wk->range, p_wk->is_shift, mode );
    }
    else
    { 
      //���[�h�{�^���̂���
      KEYANM_Start( p_wk, KEYANM_TYPE_NONE, &p_wk->range, p_wk->is_shift, mode );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  �L�[�A�j��  �t�F�[�h
 *
 *  @param  u16 *p_buff �F�ۑ��o�b�t�@�iVBlank�]���̂��߁j
 *  @param  *p_cnt      �J�E���^�o�b�t�@
 *  @param  add         �J�E���^���Z�l
 *  @param  plt_num     �p���b�g�c�ԍ�
 *  @param  plt_col     �p���b�g���ԍ�
 *  @param  start       �J�n�F
 *  @param  end         �I���F
 *
 *  @retval TRUE��END�ɂȂ����B�@FALSE�ł܂�  �iEND�Ŏ~�܂炸�A���[�v���܂��j
 */
//-----------------------------------------------------------------------------
static BOOL KeyAnm_PltFade( u16 *p_buff, u16 *p_cnt, u16 add, u8 plt_num, u8 plt_col, GXRgb start, GXRgb end )
{ 
  //�p���b�g�A�j��
  if( *p_cnt + add >= 0x10000 )
  {
    *p_cnt = *p_cnt+add-0x10000;
  }
  else
  {
    *p_cnt += add;
  }
  {
    BOOL ret;
    //1�`0�ɕϊ�
    const fx16 cos = (FX_CosIdx(*p_cnt)+FX16_ONE)/2;
    const u8 start_r  = (start & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
    const u8 start_g  = (start & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
    const u8 start_b  = (start & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;
    const u8 end_r  = (end & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
    const u8 end_g  = (end & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
    const u8 end_b  = (end & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;

    const u8 r = start_r + (((end_r-start_r)*cos)>>FX16_SHIFT);
    const u8 g = start_g + (((end_g-start_g)*cos)>>FX16_SHIFT);
    const u8 b = start_b + (((end_b-start_b)*cos)>>FX16_SHIFT);
    
    *p_buff = GX_RGB(r, g, b);
    
    ret = NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_BG_PLTT_MAIN ,
                                        plt_num * 32 + plt_col *2 ,
                                        p_buff, 2 );

    GF_ASSERT( ret );
  }

  return *p_buff == end;
}
//=============================================================================
/**
 *          STRINPUT
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  ���͕�����  ������
 *
 *  @param  STRINPUT_WORK *p_wk   ���[�N
 *  @param  default_str           �f�t�H���g������
 *  @param  strlen                ������
 *  @param  p_font                �t�H���g
 *  @param  p_que                 �v�����g�L���[
 *  @param  cp_obj                clwk�擾�pOBJ���[�N
 *  @param  heapID                �q�[�vID
 */
//-----------------------------------------------------------------------------
static void STRINPUT_Init( STRINPUT_WORK *p_wk, const STRBUF * cp_default_str, u8 strlen, GFL_FONT *p_font, PRINT_QUE *p_que, const OBJ_WORK *cp_obj, HEAPID heapID )
{ 
  GF_ASSERT( strlen <= STRINPUT_STR_LEN );

  //�N���A
  GFL_STD_MemClear( p_wk, sizeof(STRINPUT_WORK) );
  p_wk->p_font  = p_font;
  p_wk->strlen  = strlen;
  p_wk->p_que   = p_que;


  //BMPWIN�쐬
  p_wk->p_bmpwin  = GFL_BMPWIN_Create( BG_FRAME_STR_M, STRINPUT_BMPWIN_X, STRINPUT_BMPWIN_Y, STRINPUT_BMPWIN_W, STRINPUT_BMPWIN_H, PLT_BG_FONT_M, GFL_BMP_CHRAREA_GET_B );
  GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin );
  //�v�����g�L���[�ݒ�
  PRINT_UTIL_Setup( &p_wk->util, p_wk->p_bmpwin );


  //�����o�b�t�@
  p_wk->p_strbuf  = GFL_STR_CreateBuffer( STRINPUT_STR_LEN + STRINPUT_CHANGE_LEN + 1, heapID );

  //�����p�f�[�^�ǂ݂���
  { 
    int i;
    for( i = 0; i < NAMEIN_STRCHANGETYPE_MAX; i++ )
    { 
      p_wk->p_changedata[i] = NAMEIN_STRCHANGE_Alloc( i, heapID );
    }
  }
  p_wk->p_changedata_ex = NAMEIN_STRCHANGE_EX_Alloc( heapID );


  //CLWK�擾
  {  
    int i;
    for( i = 0; i < OBJ_BAR_NUM; i++ )
    { 
      p_wk->p_clwk[ i ] = OBJ_GetClwk( cp_obj, CLWKID_BAR_TOP + i );
    }
  }

  if( GFL_STR_GetBufferLength( cp_default_str ) == 0  )
  { 
    p_wk->input_str[0]  = GFL_STR_GetEOMCode();
  }
  else
  {
    GFL_STR_GetStringCode( cp_default_str, p_wk->input_str, strlen + 1 );
    p_wk->input_idx = GFL_STR_GetBufferLength( cp_default_str );
  }

  //CLWK�ݒ�
  {  
    int i;
    u16 x;
    GFL_CLACTPOS  clpos;
    clpos.y = STRINPUT_BAR_Y;

#ifdef NAMEIN_STRINPUT_CENTERING

    x = (256 / 2 - STRINPUT_BMPWIN_X*8 ) - (p_wk->strlen * STRINPUT_STR_OFFSET_X) /2 ;  //�J�n
#else
    x = 0;
#endif
    for( i = 0; i < OBJ_BAR_NUM; i++ )
    { 
      clpos.x = STRINPUT_BAR_START_X + x + i * STRINPUT_BAR_OFFSET_X;

      GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk[i], 1 );
      GFL_CLACT_WK_SetAutoAnmFlag( p_wk->p_clwk[i], TRUE );
      GFL_CLACT_WK_SetPos( p_wk->p_clwk[i], &clpos, CLSYS_DEFREND_MAIN );
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[i], TRUE );
    }
    for( i = p_wk->strlen; i < OBJ_BAR_NUM; i++ )
    { 
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[i], FALSE );
    }

    GFL_CLACT_WK_StartAnm( p_wk->p_clwk[ p_wk->input_idx ] );
  }


  //�����`�悷�邽�߂�1��UPDATE
  p_wk->is_update = TRUE;
  STRINPUT_Main( p_wk );
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���͕�����  �j��
 *
 *  @param  STRINPUT_WORK *p_wk ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void STRINPUT_Exit( STRINPUT_WORK *p_wk )
{ 
  //�����p�f�[�^�j��
  { 
    int i;
    NAMEIN_STRCHANGE_EX_Free( p_wk->p_changedata_ex );
    for( i = 0; i < NAMEIN_STRCHANGETYPE_MAX; i++ )
    { 
      NAMEIN_STRCHANGE_Free( p_wk->p_changedata[i] );
    }
  }

  //�����o�b�t�@�j��
  GFL_STR_DeleteBuffer( p_wk->p_strbuf );
  
  //BMPWIN�j��
  GFL_BMPWIN_Delete( p_wk->p_bmpwin );

  //�N���A
  GFL_STD_MemClear( p_wk, sizeof(STRINPUT_WORK) );
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���͕�����  ���C������
 *
 *  @param  STRINPUT_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void STRINPUT_Main( STRINPUT_WORK *p_wk )
{ 
  if( p_wk->is_update )
  { 
    STRCODE buff[ STRINPUT_STR_LEN + STRINPUT_CHANGE_LEN + 1 ];

    //�m�蕶���񂪂���ΘA��
    if( p_wk->input_idx > 0 )
    { 
      STRTOOL_Copy( p_wk->input_str, buff, p_wk->input_idx + 1 );
    }
    //�ϊ������񂪂���΁A����ɘA��
    if( p_wk->change_idx > 0 )
    { 
      STRTOOL_Copy( p_wk->change_str, &buff[p_wk->input_idx], p_wk->change_idx + 1 );
    }

    //�A������������������΁A�����`��
    //�Ȃ���΃N���A
    if( p_wk->input_idx + p_wk->change_idx > 0 )
    { 
      int i;
      STRCODE code[2];
      u16 x;
  
      //��[�N���A
      GFL_BMP_Clear(GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 0 );

      //�����ɂ��邽�߂�1�������v�����g
      for( i = 0; i < p_wk->input_idx + p_wk->change_idx; i++ )
      { 
        code[0] = buff[i];
        code[1] = GFL_STR_GetEOMCode();


        GFL_STR_SetStringCodeOrderLength( p_wk->p_strbuf, code, 2 );
#ifdef NAMEIN_STRINPUT_CENTERING
        x = (256 / 2 - STRINPUT_BMPWIN_X*8 ) - (p_wk->strlen * STRINPUT_STR_OFFSET_X) /2 ;  //�J�n
        x += i * STRINPUT_STR_OFFSET_X + STRINPUT_BAR_OFFSET_X/2;               //�P�������Z���^�����O
        x -= PRINTSYS_GetStrWidth( p_wk->p_strbuf, p_wk->p_font, 0 )/2;
#else
        x = 0;
        x += i * STRINPUT_STR_OFFSET_X;
        x += (STRINPUT_BAR_OFFSET_X/2 - PRINTSYS_GetStrWidth( p_wk->p_strbuf, p_wk->p_font, 0 )/2);
#endif

        //�ϊ��������͒��̓t�H���g�̐F��ς���
        if( i >= p_wk->input_idx )
        { 
          PRINT_UTIL_PrintColor( &p_wk->util, p_wk->p_que, x, STRINPUT_STR_Y, p_wk->p_strbuf, p_wk->p_font, PRINTSYS_LSB_Make(3,4,0) );
        }
        else
        { 
          PRINT_UTIL_Print( &p_wk->util, p_wk->p_que, x, STRINPUT_STR_Y, p_wk->p_strbuf, p_wk->p_font );
        }
      }
    }
    else
    { 
      GFL_BMP_Clear(GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 0 );
      GFL_BMPWIN_TransVramCharacter(p_wk->p_bmpwin);
    }

    //�o�[�̃A�j����ύX
    { 
      int i;
      for( i = 0; i < p_wk->strlen; i++ )
      { 
        GFL_CLACT_WK_SetAnmFrame( p_wk->p_clwk[i], 0 );
        GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk[ i ], 1 );
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[i], TRUE );
      }
      for( ; i < OBJ_BAR_NUM; i++ )
      { 
        GFL_CLACT_WK_SetAnmFrame( p_wk->p_clwk[i], 0 );
        GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk[ i ], 1 );
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[i], FALSE );
      }
      for(i = 0; i <= p_wk->input_idx && i < p_wk->strlen; i++ )
      { 
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[i], TRUE );
      }
      GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk[ p_wk->input_idx ], 0 );
    }

    p_wk->is_update = FALSE;
  }
  
  STRINPUT_PrintMain( p_wk );
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���͕�����  1��������
 *
 *  @param  STRINPUT_WORK *p_wk ���[�N
 *  @rerval TRUE ���� FALSE ���s
 */
//-----------------------------------------------------------------------------
static BOOL STRINPUT_BackSpace( STRINPUT_WORK *p_wk )
{ 
  //�ϊ����Ȃ�΁A�ϊ����̕�����
  //�����łȂ��Ȃ�΁A���ʂɏ���
  if( p_wk->change_idx > 0 )
  { 
    p_wk->change_idx--;
    p_wk->change_str[ p_wk->change_idx ] = GFL_STR_GetEOMCode();
    p_wk->is_update = TRUE;

    return TRUE;
  }
  else
  { 
    if( p_wk->input_idx > 0 )
    { 
      p_wk->input_idx--;
      p_wk->input_str[ p_wk->input_idx ] = GFL_STR_GetEOMCode();
      p_wk->is_update = TRUE;

      return TRUE;
    }
  }

  return FALSE;
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���͕�����  ������ݒ�
 *
 *  @param  STRINPUT_WORK *p_wk ���[�N
 *  @param  STRCODE code
 */
//-----------------------------------------------------------------------------
static BOOL STRINPUT_SetStr( STRINPUT_WORK *p_wk, STRCODE code )
{ 
  //���͂ł��邩
  if( p_wk->input_idx < p_wk->strlen )
  { 
    p_wk->input_str[ p_wk->input_idx ] = code;
    p_wk->input_idx++;
    p_wk->input_str[ p_wk->input_idx ] = GFL_STR_GetEOMCode();

    OS_Printf( "\nSetStr\n " );
    DEBUG_NAMEIN_Print(p_wk->input_str,p_wk->input_idx);

    p_wk->is_update = TRUE;

    return TRUE;
  }

  return FALSE;
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���͕�����  �ϊ�������ݒ�
 *
 *  @param  STRINPUT_WORK *p_wk ���[�N
 *  @param  code                �ϊ�������ɂ���镶��
 *  @param  is_shift            �V�t�g�����͂���Ă��邩�ǂ���
 *
 *  @retval TRUE�@������ϊ������@FALSE���Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL STRINPUT_SetChangeStr( STRINPUT_WORK *p_wk, STRCODE code, BOOL is_shift )
{ 
  //���͂ł��邩
  if( p_wk->input_idx < p_wk->strlen
      && p_wk->change_idx < STRINPUT_CHANGE_LEN )
  { 
    p_wk->change_str[ p_wk->change_idx ] = code;
    p_wk->change_idx++;
    p_wk->change_str[ p_wk->change_idx ] = GFL_STR_GetEOMCode();

    //�ϊ��o�b�t�@����m��o�b�t�@�ւ̕ϊ�����
    StrInput_ChangeStrToStr( p_wk, is_shift );

    p_wk->is_update = TRUE;
    return TRUE;
  }

  return FALSE;
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���͕�����  ���_�A�����_��ϊ�����
 *
 *  @param  STRINPUT_WORK *p_wk ���[�N
 *
 *  @retval TRUE�@������ϊ������@FALSE���Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL STRINPUT_SetChangeSP( STRINPUT_WORK *p_wk, STRINPUT_SP_CHANGE type )
{ 
  NAMEIN_STRCHANGE  *p_data;

  switch( type )
  { 
  case STRINPUT_SP_CHANGE_DAKUTEN:      //���_
    //���������T�[�`
    p_data  =  p_wk->p_changedata[NAMEIN_STRCHANGETYPE_DAKUTEN];
    break;
  case STRINPUT_SP_CHANGE_HANDAKUTEN: //�����_
    //���������T�[�`
    p_data  =  p_wk->p_changedata[NAMEIN_STRCHANGETYPE_HANDAKUTEN];
    break;
  default:
    GF_ASSERT(0);
    return FALSE;
  }

  //�m�蕶����̈�Ԍ������āA�ϊ��\��������A�ς���
  if( 0 < p_wk->input_idx )
  { 
    u16 idx;
    STRCODE code[NAMEIN_STRCHANGE_CODE_LEN+1];
    u8 len;
    STRCODE delete[NAMEIN_STRCHANGE_CODE_LEN+1];
    u8 delete_len;
    int i;

    idx = NAMEIN_STRCHANGE_GetIndexByInputStr( p_data, &p_wk->input_str[ p_wk->input_idx-1 ] ); 
    if( idx != NAMEIN_STRCHANGE_NONE )
    { 
      if( NAMEIN_STRCHANGE_GetChangeStr( p_data, idx, code, NAMEIN_STRCHANGE_CODE_LEN+1, &len ) )
      { 
        //�m��o�b�t�@����ϊ�������擾
        NAMEIN_STRCHANGE_GetInputStr( p_data, idx, delete, NAMEIN_STRCHANGE_CODE_LEN+1, &delete_len );
        //�m��o�b�t�@����폜
        for( i = 0; i < delete_len; i++ )
        { 
          STRINPUT_BackSpace( p_wk );
        }

        //�m��o�b�t�@�ɒǉ�
        for( i = 0; i < len; i++ )
        { 
          STRINPUT_SetStr( p_wk, code[i] );
        }

        return TRUE;
      }
    }
  }

  return FALSE;
}


//----------------------------------------------------------------------------
/**
 *  @brief  ���͕�����  ���_�A�����_�A�������A�Ȃǂ��z�ϊ�����
 *
 *	@param	STRINPUT_WORK *p_wk   ���[�N
 *
 *	@return TRUE�Ŕ���  FALSE�Ŗ�����
 */
//-----------------------------------------------------------------------------
static BOOL STRINPUT_SetChangeAuto( STRINPUT_WORK *p_wk )
{ 
  //�m�蕶����̈�Ԍ������āA�ϊ��\��������A�ς���
  if( 0 < p_wk->input_idx )
  { 
    BOOL ret;
    STRCODE code;
    
    ret = NAMEIN_STRCHANGE_EX_GetChangeStr( p_wk->p_changedata_ex, &p_wk->input_str[ p_wk->input_idx-1 ], &code );
    if( ret )
    { 
      STRINPUT_BackSpace( p_wk );

      STRINPUT_SetStr( p_wk, code );

      return TRUE;
    }
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���͕�����  �`�惁�C��
 *
 *  @param  STRINPUT_WORK *p_wk   ���[�N
 *
 * @retval  BOOL  �]�����I����Ă���ꍇ��TRUE�^�I����Ă��Ȃ��ꍇ��FALSE
 */
//-----------------------------------------------------------------------------
static BOOL STRINPUT_PrintMain( STRINPUT_WORK *p_wk )
{ 
  return PRINT_UTIL_Trans( &p_wk->util, p_wk->p_que );
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���͕�����  ���͂�����������R�s�[
 *
 *  @param  const STRINPUT_WORK *cp_wk  ���[�N
 *  @param  *p_strbuf                   STRBUF
 */
//-----------------------------------------------------------------------------
static void STRINPUT_CopyStr( const STRINPUT_WORK *cp_wk, STRBUF *p_strbuf )
{ 
  GFL_STR_SetStringCodeOrderLength( p_strbuf, cp_wk->input_str, cp_wk->input_idx + 1 );
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���͕�����  �ϊ������������
 *
 *  @param  STRINPUT_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void STRINPUT_DeleteChangeStr( STRINPUT_WORK *p_wk )
{ 
  if( p_wk->change_idx > 0 )
  { 
    p_wk->change_idx  = 0;
    p_wk->change_str[ p_wk->change_idx ] = GFL_STR_GetEOMCode();
    p_wk->is_update = TRUE;
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���͕�����  �ϊ���������m��
 *
 *  @param  STRINPUT_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void STRINPUT_DecideChangeStr( STRINPUT_WORK *p_wk )
{
  int i;
  for( i = 0; i < p_wk->change_idx; i++ )
  { 
    STRINPUT_SetStr( p_wk, p_wk->change_str[ i ] );
  }
  p_wk->change_idx  = 0;
  p_wk->change_str[ p_wk->change_idx ] = GFL_STR_GetEOMCode();
  p_wk->is_update = TRUE;
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���͕�����  ���͂���Ă��镶����S�ď���
 *
 *  @param  STRINPUT_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void STRINPUT_Delete( STRINPUT_WORK *p_wk )
{ 
  int i;
  if( p_wk->input_idx > 0 )
  { 
    p_wk->input_idx = 0;
    p_wk->input_str[ p_wk->input_idx ] = GFL_STR_GetEOMCode();
    p_wk->is_update = TRUE;
  } 
  STRINPUT_DeleteChangeStr( p_wk );
  
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���͕�����
 *
 *  @param  STRINPUT_WORK *p_wk   ���[�N
 *  @param  STRCODE *cp_code      ������
 *
 */
//-----------------------------------------------------------------------------
static void STRINPUT_SetLongStr( STRINPUT_WORK *p_wk, const STRCODE *cp_code )
{ 
  while( *cp_code != GFL_STR_GetEOMCode() )
  { 
    if( !STRINPUT_SetStr( p_wk, *cp_code ))
    { 
      break;
    }
    cp_code++;
  }

}
//----------------------------------------------------------------------------
/**
 *  @brief  ���͕�����  ���͐������E���ǂ���
 *
 *  @param  const STRINPUT_WORK *cp_wk  ���[�N
 *
 *  @return TRUE�Ȃ�Γ��͍ő�  FALSE�Ȃ�΂܂�
 */
//-----------------------------------------------------------------------------
static BOOL STRINPUT_IsInputEnd( const STRINPUT_WORK *cp_wk )
{ 
  return cp_wk->input_idx == cp_wk->strlen;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �m����͕�������Ԃ�
 *
 *  @param  const STRINPUT_WORK *cp_wk ���[�N
 *
 *  @return �m����͕�����
 */
//-----------------------------------------------------------------------------
static u32  STRINPUT_GetStrLength( const STRINPUT_WORK *cp_wk )
{ 
  return cp_wk->input_idx;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ϊ������񐔂�Ԃ�
 *
 *	@param	const STRINPUT_WORK *cp_wk  ���[�N
 *
 *	@return �ϊ�������
 */
//-----------------------------------------------------------------------------
static u32  STRINPUT_GetChangeStrLength( const STRINPUT_WORK *cp_wk )
{ 
  return cp_wk->change_idx;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �ϊ������񂩂�m�蕶����ւ̕ϊ�
 *
 *  @param  STRINPUT_WORK *p_wk   ���[�N
 *  @param  is_shift              �V�t�g�����͂���Ă��邩�ǂ���
 *
 *  @return TRUE�ŕϊ�����  FALSE�ŕϊ����Ȃ�����
 */
//-----------------------------------------------------------------------------
static BOOL StrInput_ChangeStrToStr( STRINPUT_WORK *p_wk, BOOL is_shift )
{ 
    //�ϊ�����
  int i,j;
  u16 idx;
  u8  len;
  STRCODE code[NAMEIN_STRCHANGE_CODE_LEN+1];

  BOOL (*GetChangeStr)( const NAMEIN_STRCHANGE *cp_wk, u16 idx, STRCODE *p_code, u8 code_len, u8 *p_len );
  
  //�V�t�g���͂ɂ���Ċ֐����Ⴄ
  if( is_shift )
  { 
    GetChangeStr  = NAMEIN_STRCHANGE_GetChangeShiftStr;
  }
  else
  { 
    GetChangeStr  = NAMEIN_STRCHANGE_GetChangeStr;
  }


  for( i = 0; i < p_wk->change_idx; i++ )
  { 
    //���������T�[�`
    idx = NAMEIN_STRCHANGE_GetIndexByInputStr( p_wk->p_changedata[NAMEIN_STRCHANGETYPE_QWERTY], &p_wk->change_str[ i ] ); 
    if( idx != NAMEIN_STRCHANGE_NONE )
    { 
      //�ϊ����Ă�������̑O�̕������m�蕶����ɂ���
      NAGI_Printf( "�ϊ������m�肷�镶���� ����%d\n", i );
      DEBUG_NAMEIN_Print(p_wk->change_str,i);
      for( j = 0; j < i; j++ )
      { 
        STRINPUT_SetStr( p_wk, p_wk->change_str[ j ] );
      } 
      for( j = 0; j < i; j++ )
      { 
        STRINPUT_BackSpace( p_wk );
      }

      //�u�����m��o�b�t�@�ɑ���
      if( GetChangeStr( p_wk->p_changedata[NAMEIN_STRCHANGETYPE_QWERTY], idx, code, NAMEIN_STRCHANGE_CODE_LEN+1, &len ) )
      { 
        NAGI_Printf( "�ϊ��㕶���� ����%d\n", len );
        DEBUG_NAMEIN_Print(code,len);
        for( j = 0; j < len; j++ )
        { 
          STRINPUT_SetStr( p_wk, code[j] );
        }
        //�u�������̂ŁA�����o�b�t�@����폜
        NAMEIN_STRCHANGE_GetInputStr( p_wk->p_changedata[NAMEIN_STRCHANGETYPE_QWERTY], idx, code, NAMEIN_STRCHANGE_CODE_LEN+1, &len );

        NAGI_Printf( "���������� ����%d\n", len );
        DEBUG_NAMEIN_Print(code,len);
        for( j = 0; j < len; j++ )
        { 
          STRINPUT_BackSpace( p_wk );
        }

        //�c�镶����ǉ�
        NAMEIN_STRCHANGE_GetRestStr( p_wk->p_changedata[NAMEIN_STRCHANGETYPE_QWERTY], idx, code, NAMEIN_STRCHANGE_CODE_LEN+1, &len ); 
        NAGI_Printf( "�c�镶���� ����%d\n", len );
        DEBUG_NAMEIN_Print(code,len);
        for( j = 0; j < len; j++ )
        { 
          p_wk->change_str[ p_wk->change_idx ] = code[j];
          p_wk->change_idx++; 
          p_wk->change_str[ p_wk->change_idx ] = GFL_STR_GetEOMCode();
        }

        return TRUE;
      }
    }
  }
  return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���������ł��邩�`�F�b�N  �i��l�����ł͌���ł����A����ȊO�ł̓f�t�H���g�ɖ߂�j
 *
 *	@param	const STRINPUT_WORK *cp_wk  ���[�N
 *
 *	@return TRUE������  FASE�s������
 */
//-----------------------------------------------------------------------------
static BOOL STRINPUT_IsValidStr( const STRINPUT_WORK *cp_wk )
{ 
  int i;
  int cnt;

  if( cp_wk->input_idx == 0 )
  { 
    return FALSE;
  }

  cnt = 0;
  for( i = 0; i < cp_wk->input_idx; i++ )
  { 
    if(  cp_wk->input_str[ i ] == NAMEIN_STR_ILLEGAL_STR )
    { 
      cnt++;
    }
  }

  return i != cnt;
}
//=============================================================================
/**
 *          KEYMAP
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  �L�[�z��  �쐬
 *
 *  @param  KEYMAP_WORK *p_wk ���[�N
 *  @param  mode              �z�񃂁[�h
 *  @param  heapID            �q�[�vID
 */
//-----------------------------------------------------------------------------
static void KEYMAP_Create( KEYMAP_WORK *p_wk, NAMEIN_INPUTTYPE mode, NAMEIN_KEYMAP_HANDLE *p_keymap_handle, HEAPID heapID )
{
  GF_ASSERT( mode < NAMEIN_INPUTTYPE_MAX );

  //�N���A
  GFL_STD_MemClear( p_wk, sizeof(KEYMAP_WORK) );
  p_wk->p_key = NAMEIN_KEYMAP_HANDLE_GetData( p_keymap_handle, mode );

  switch( mode )
  {
  case NAMEIN_INPUTTYPE_QWERTY:
    p_wk->w     = 11;
    p_wk->h     = 4;
    p_wk->get_keyrect     = KeyMap_QWERTY_GetRect;
    p_wk->get_movecursor  = KeyMap_QWERTY_GetMoveCursor;
    p_wk->get_keytype     = KeyMap_QWERTY_GetKeyType;
    break;
  case NAMEIN_INPUTTYPE_KANA:
    /* fallthrough */
  case NAMEIN_INPUTTYPE_KATA:
    p_wk->w     = 13;
    p_wk->h     = 6;
    p_wk->get_keyrect     = KeyMap_KANA_GetRect;
    p_wk->get_movecursor  = KeyMap_KANA_GetMoveCursor;
    p_wk->get_keytype     = KeyMap_KANA_GetKeyType;
    break;
  case NAMEIN_INPUTTYPE_ABC:
    /* fallthrough */
  case NAMEIN_INPUTTYPE_KIGOU:
    p_wk->w     = 13;
    p_wk->h     = 6;
    p_wk->get_keyrect     = KeyMap_KANA_GetRect;
    p_wk->get_movecursor  = KeyMap_KANA_GetMoveCursor;
    p_wk->get_keytype     = KeyMap_KIGOU_GetKeyType;
    break;
  }

  //�f�t�H���g�ړ��l
  { 
    int i;
    for( i = 0; i < KEYMAP_KEYMOVE_BUFF_MAX; i++ )
    { 
      p_wk->buff.data[i]  = sc_keymove_default[i];
    }
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  KEYMAP_WORK *p_wk 
 *
 *  @return
 */
//-----------------------------------------------------------------------------
static void KEYMAP_Delete( KEYMAP_WORK *p_wk )
{ 
 // �S���������ɂ����悤�ɂ����̂ŏ����̂͑匳�ɂ���
  //NAMEIN_KEYMAP_Free( p_wk->p_key );
  //�N���A
  GFL_STD_MemClear( p_wk, sizeof(KEYMAP_WORK) );
}
//----------------------------------------------------------------------------
/**
 *  @brief  �L�[�z��  �J�[�\���̈ʒu�̕������擾����
 *
 *  @param  const KEYMAP_WORK *cp_wk  ���[�N
 *  @param  GFL_POINT *cp_cursor      �J�[�\���̈ʒu
 *  @param  ������
 *
 *  @return �L�[�̎��
 */
//-----------------------------------------------------------------------------
static KEYMAP_KEYTYPE KEYMAP_GetKeyInfo( const KEYMAP_WORK *cp_wk, const GFL_POINT *cp_cursor, STRCODE *p_code )
{ 
  KEYMAP_KEYTYPE  type;
  GFL_POINT strpos;

  //�L�[�^�C�v�擾
  type  = cp_wk->get_keytype( cp_cursor, &strpos );

  if( type == KEYMAP_KEYTYPE_STR ||
      type == KEYMAP_KEYTYPE_DAKUTEN ||
      type == KEYMAP_KEYTYPE_HANDAKUTEN )
  { 
    if( p_code )
    { 
      *p_code = NAMEIN_KEYMAP_GetStr( cp_wk->p_key, strpos.x, strpos.y );
    }
  }
  else if( type == KEYMAP_KEYTYPE_SPACE )
  { 
    if( p_code )
    {
      *p_code = L'�@';
    }
  }


  return type;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �L�[�z��  �^�b�`���W���J�[�\�����W�ɕϊ�����
 *
 *  @param  const KEYMAP_WORK *cp_wk  ���[�N
 *  @param  GFL_POINT *cp_trg         �^�b�`���W
 *  @param  *cp_cursor                �J�[�\�����W
 *
 *  @retval TRUE�ϊ��ł��� FALSE�͈͊O������
 */
//-----------------------------------------------------------------------------
static BOOL KEYMAP_GetTouchCursor( const KEYMAP_WORK *cp_wk, const GFL_POINT *cp_trg, GFL_POINT *p_cursor )
{ 
  GFL_POINT pos;
  GFL_RECT rect;

  for( pos.y = 0; pos.y < cp_wk->h; pos.y++ )
  { 
    for( pos.x = 0; pos.x < cp_wk->w; pos.x++ )
    { 
      if( KEYMAP_GetRange( cp_wk, &pos, &rect ) )
      {
        //�L�����P�ʂ��h�b�g�P�ʂ�
        rect.left   *= 8;
        rect.top    *= 8;
        rect.right  *= 8;
        rect.bottom *= 8;

        //�����蔻��
        if( COLLISION_IsRectXPos( &rect, cp_trg ) )
        { 
          *p_cursor = pos;
          return TRUE;
        }
      }
    }
  }

  return FALSE;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �L�[�z��  �L�[�̌q������l�����āA���Z�l������
 *
 *  @param  KEYMAP_WORK *p_wk   ���[�N
 *  @param  *p_now              �J�[�\���̌��ݍ��W
 *  @param  GFL_POINT *cp_add   ���Z�l
 */
//-----------------------------------------------------------------------------
static void KEYMAP_MoveCursor( KEYMAP_WORK *p_wk, GFL_POINT *p_now, const GFL_POINT *cp_add )
{ 
  p_wk->get_movecursor( p_now, cp_add, &p_wk->buff );
}
//----------------------------------------------------------------------------
/**
 *  @brief  �L�[�z��  �L�[�z���BMPWIN�ɏ�������
 *
 *  @param  const KEYMAP_WORK *cp_wk  ���[�N
 *  @param  *p_util   �v�����g�֗��\����
 *  @param  *p_que    �L���[
 *  @param  *p_font   �t�H���g
 *  @param  heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void KEYMAP_Print( const KEYMAP_WORK *cp_wk, PRINT_UTIL *p_util, PRINT_QUE *p_que, GFL_FONT *p_font, HEAPID heapID )
{ 
  STRBUF  *p_strbuf;
  GFL_POINT pos;
  GFL_RECT  rect;
  u16 x, y;
  KEYMAP_KEYTYPE keytype;
  STRCODE get;

  //�o�b�t�@�쐬
  p_strbuf  = GFL_STR_CreateBuffer( 2, heapID );

  //�S�������v�����g
  for( pos.y = 0; pos.y < NAMEIN_KEYMAP_GetHeight( cp_wk->p_key ); pos.y++ )
  { 
    for( pos.x = 0; pos.x < NAMEIN_KEYMAP_GetWidth( cp_wk->p_key ); pos.x++ )
    { 
      if( KEYMAP_GetRange( cp_wk, &pos, &rect ) )
      { 
        keytype = KEYMAP_GetKeyInfo( cp_wk, &pos, &get );
        if( keytype == KEYMAP_KEYTYPE_STR ||
            keytype == KEYMAP_KEYTYPE_DAKUTEN ||
            keytype == KEYMAP_KEYTYPE_HANDAKUTEN)
        { 
          //�����擾���A�o�b�t�@�ɔ[�߂�
          STRCODE code[2];
          code[0] = get;
          code[1] = GFL_STR_GetEOMCode();
          GFL_STR_SetStringCodeOrderLength( p_strbuf, code, 2 );

          //�h�b�g�P�ʂ�
          rect.left   *=  8;
          rect.top    *=  8;
          rect.right  *=  8;
          rect.bottom *=  8;

          //��`����A�����𒆐S�ɕ`�悷�邽�߂ɁA�ʒu�Z�o
          x = rect.left + (rect.right - rect.left)/2 - KEYBOARD_BMPWIN_X*8;
          y = rect.top + (rect.bottom - rect.top)/2 - KEYBOARD_BMPWIN_Y*8;
          x -= PRINTSYS_GetStrWidth( p_strbuf, p_font, 0 )/2;
          y -= PRINTSYS_GetStrHeight( p_strbuf, p_font )/2;

          //�`��
          //  �G�̃f�[�^������ƁA�킭��������E�ցA15�h�b�g�܂ł����Ȃ�����
          //  �G�Ƃ��킹�邽�߂�-1
          //  (�����Ƃ��Ă����`���W�̓L�����P�ʂȂ̂�16)
          PRINT_UTIL_Print( p_util, p_que, x-1, y, p_strbuf, p_font );
        }
      }
    }
  }

  //�o�b�t�@�N���A
  GFL_STR_DeleteBuffer( p_strbuf );
}
//----------------------------------------------------------------------------
/**
 *  @brief  �L�[�z��  �J�[�\������L�[�̃L�������W���󂯎��
 *
 *  @param  const KEYMAP_WORK *cp_wk  ���[�N
 *  @param  GFL_POINT *cp_cursor      �J�[�\��
 *  @param  *p_rect                   �L������`���
 *  @retval TRUE�Ȃ�󂯎�ꂽ  FALSE�Ȃ�Ύ󂯎��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL KEYMAP_GetRange( const KEYMAP_WORK *cp_wk, const GFL_POINT *cp_cursor, GFL_RECT *p_rect )
{ 
  return cp_wk->get_keyrect( cp_cursor, p_rect );
}
//----------------------------------------------------------------------------
/**
 *  @brief  �L�[�z��  �L�[�̎�ނ���L�[�̃^�C�v��{��
 *
 *  @param  const KEYMAP_WORK *cp_wk  ���[�N
 *  @param  key         �L�[�̎��
 *  @param  *p_cursor   �J�[�\���ʒu�󂯎��
 *
 *  @return TRUE�ő���  FALSE�łȂ�
 */
//-----------------------------------------------------------------------------
static BOOL KEYMAP_GetCursorByKeyType( const KEYMAP_WORK *cp_wk, KEYMAP_KEYTYPE key, GFL_POINT *p_cursor )
{ 
  GFL_POINT       pos;
    
  //���̃L�[�̎�ނ�T�����Ă�������J�[�\���ʒu�ύX
  for( pos.y = 0; pos.y <= cp_wk->h; pos.y++ )
  { 
    for( pos.x = 0; pos.x <= cp_wk->w; pos.x++ )
    { 
      if( key == cp_wk->get_keytype( &pos, NULL ) )
      { 
        *p_cursor = pos;
        return TRUE;
      }
    }
  }
  return FALSE;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �L�[�z��  �J�[�\���ʒu�؂�ւ�
 *
 *  @param  GFL_POINT *p_cursor �J�[�\��
 *  @param  pre                 �O�̃��[�h
 *  @param  next                ���̃��[�h
 */
//-----------------------------------------------------------------------------
static void KEYMAP_TOOL_ChangeCursorPos( GFL_POINT *p_cursor, NAMEIN_INPUTTYPE pre, NAMEIN_INPUTTYPE next )
{ 

  s16 w, h;
  GET_KEYTYPE_FUNC  prev_get_keytype;
  GET_KEYTYPE_FUNC  next_get_keytype;

  //���[�h�Ŏg���֐����Ⴄ
  if( pre == NAMEIN_INPUTTYPE_QWERTY )
  { 
    prev_get_keytype  = KeyMap_QWERTY_GetKeyType;
    next_get_keytype  = KeyMap_KANA_GetKeyType;
    w = KEYMAP_KANA_WIDTH;
    h = KEYMAP_KANA_HEIGHT;
  }
  else if( next == NAMEIN_INPUTTYPE_QWERTY )
  { 
    prev_get_keytype  = KeyMap_KANA_GetKeyType;
    next_get_keytype  = KeyMap_QWERTY_GetKeyType;
    w = KEYMAP_QWERTY_WIDTH;
    h = KEYMAP_QWERTY_HEIGHT;
  }

  //�L�[�̎�ނƓ����ꏊ��T��
  { 
    KEYMAP_KEYTYPE  key;
    GFL_POINT       pos;
    
    //���݂̃L�[�̎��
    key = prev_get_keytype( p_cursor, NULL );

    //���̃L�[�̎�ނ�T�����Ă�������J�[�\���ʒu�ύX
    for( pos.y = 0; pos.y <= h; pos.y++ )
    { 
      for( pos.x = 0; pos.x <= w; pos.x++ )
      { 
        if( key == next_get_keytype( &pos, NULL ) )
        { 
          *p_cursor = pos;
          return;
        }
      }
    }

  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���ȃJ�i�Ȃǂ̌�Ղ̖ڏ�̋�`�󂯎��
 *
 *  @param  const GFL_POINT *cp_cursor  �J�[�\��
 *  @param  *p_rect                     ��`�󂯎��(�L�����P��)
 *  @retval TRUE�Ȃ�΋�`�����݂���  FALSE�Ȃ�΂��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL KeyMap_KANA_GetRect( const GFL_POINT *cp_cursor, GFL_RECT *p_rect )
{ 
  enum
  { 
    KANA_START_X  = 3,
    KANA_START_Y  = 5,
    KANA_KEY_W    = 2,
    KANA_KEY_H    = 3,

    KANA_LONGKEY_W    = 8,
  };

  //�L�[�̎�ގ擾
  KEYMAP_KEYTYPE  key;
  GFL_POINT       strpos;

  key = KeyMap_KANA_GetKeyType( cp_cursor, &strpos );

  switch( key )
  { 
  case KEYMAP_KEYTYPE_STR:    //��������
    /*  fallthrough */
  case KEYMAP_KEYTYPE_DAKUTEN:  //���_
    /*  fallthrough */
  case KEYMAP_KEYTYPE_HANDAKUTEN: //�����_
    p_rect->left    = KANA_START_X + strpos.x * KANA_KEY_W;
    p_rect->top     = KANA_START_Y + strpos.y * KANA_KEY_H;
    p_rect->right   = p_rect->left + KANA_KEY_W;
    p_rect->bottom  = p_rect->top  + KANA_KEY_H;
    break;
  case KEYMAP_KEYTYPE_KANA:   //���ȃ��[�h
    /*  fallthrough */
  case KEYMAP_KEYTYPE_KATA:   //�J�i���[�h
    /*  fallthrough */
  case KEYMAP_KEYTYPE_ABC:    //�A���t�@�x�b�g���[�h
    /*  fallthrough */
  case KEYMAP_KEYTYPE_KIGOU:  //�L�����[�h
    /*  fallthrough */
  case KEYMAP_KEYTYPE_QWERTY: //���[�}�����[�h
    p_rect->left    = KANA_START_X + cp_cursor->x * KANA_KEY_W;
    p_rect->top     = KANA_START_Y + cp_cursor->y * KANA_KEY_H;
    p_rect->right   = p_rect->left + KANA_KEY_W;
    p_rect->bottom  = p_rect->top  + KANA_KEY_H;
    break;
  case KEYMAP_KEYTYPE_DELETE: //����
    p_rect->left    = KANA_START_X + cp_cursor->x * KANA_KEY_W;
    p_rect->top     = KANA_START_Y + cp_cursor->y * KANA_KEY_H;
    p_rect->right   = p_rect->left + KANA_LONGKEY_W;
    p_rect->bottom  = p_rect->top  + KANA_KEY_H;
    break;  
    break;
  case KEYMAP_KEYTYPE_DECIDE: //��߂�
    p_rect->left    = KANA_START_X + (cp_cursor->x-1) * KANA_KEY_W + KANA_LONGKEY_W;
    p_rect->top     = KANA_START_Y + (cp_cursor->y) * KANA_KEY_H;
    p_rect->right   = p_rect->left + KANA_LONGKEY_W;
    p_rect->bottom  = p_rect->top  + KANA_KEY_H;
    break;
  case KEYMAP_KEYTYPE_NONE:
    return FALSE;
  }

  switch( cp_cursor->y )
  { 
  case 5:
    if( cp_cursor->x > 6 )
    { 
      return FALSE;
    }
    break;
  }


  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *  @brief  PC�̃L�[�{�[�h��̋�`�󂯎��
 *
 *  @param  const GFL_POINT *cp_cursor  �J�[�\��
 *  @param  *p_rect                     ��`�󂯎��
 *  @retval TRUE�Ȃ�΋�`�����݂���  FALSE�Ȃ�΂��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL KeyMap_QWERTY_GetRect( const GFL_POINT *cp_cursor, GFL_RECT *p_rect )
{ 
  enum
  { 
    KANA_KEY_W        = 2,
    KANA_KEY_H        = 3,

    KANA_LONGKEY_W    = 4,  
    KANA_ENTER_W      = 5,  
    KANA_SPACE_W      = 14, 
  };

  //�����L�[�̍s���Ƃ̊J�n�ʒu
  static const int sc_start_x[] =
  { 
    5,6,7
  };
  static const int sc_start_y[] =
  { 
    8,12,16
  };


  //�L�[�̎�ގ擾
  KEYMAP_KEYTYPE  key;
  GFL_POINT       strpos;
  
  key = KeyMap_QWERTY_GetKeyType( cp_cursor, &strpos );

  switch( key )
  { 
  case KEYMAP_KEYTYPE_STR:    //��������
    p_rect->left    = sc_start_x[ strpos.y ] + strpos.x * KANA_KEY_W;
    p_rect->top     = sc_start_y[ strpos.y ];
    p_rect->right   = p_rect->left + KANA_KEY_W;
    p_rect->bottom  = p_rect->top  + KANA_KEY_H;
    break;
  case KEYMAP_KEYTYPE_KANA:   //���ȃ��[�h
    /*  fallthrough */
  case KEYMAP_KEYTYPE_KATA:   //�J�i���[�h
    /*  fallthrough */
  case KEYMAP_KEYTYPE_ABC:    //�A���t�@�x�b�g���[�h
    /*  fallthrough */
  case KEYMAP_KEYTYPE_KIGOU:  //�L�����[�h
    /*  fallthrough */
  case KEYMAP_KEYTYPE_QWERTY: //���[�}�����[�h
    p_rect->left    = 3 + cp_cursor->x * KANA_KEY_W;
    p_rect->top     = 20;
    p_rect->right   = p_rect->left + KANA_KEY_W;
    p_rect->bottom  = p_rect->top  + KANA_KEY_H;
    break;
  case KEYMAP_KEYTYPE_DELETE: //����
    p_rect->left    = 25;
    p_rect->top     = 8;
    p_rect->right   = p_rect->left + KANA_LONGKEY_W;
    p_rect->bottom  = p_rect->top  + KANA_KEY_H;
    break;  
    break;
  case KEYMAP_KEYTYPE_DECIDE: //��߂�
    p_rect->left    = 24;
    p_rect->top     = 12;
    p_rect->right   = p_rect->left + KANA_ENTER_W;
    p_rect->bottom  = p_rect->top  + KANA_KEY_H;
    break;
  case KEYMAP_KEYTYPE_SHIFT : //�V�t�g
    if( cp_cursor->x == 0 )
    { 
      p_rect->left    = 3;
      p_rect->top     = 16;
      p_rect->right   = p_rect->left + KANA_LONGKEY_W;
      p_rect->bottom  = p_rect->top  + KANA_KEY_H;
    }
    else
    { 
      p_rect->left    = 23;
      p_rect->top     = 16;
      p_rect->right   = p_rect->left + KANA_LONGKEY_W;
      p_rect->bottom  = p_rect->top  + KANA_KEY_H;
    }
    break;
  case KEYMAP_KEYTYPE_SPACE : //�X�y�[�X
    p_rect->left    = 13;
    p_rect->top     = 20;
    p_rect->right   = p_rect->left + KANA_SPACE_W;
    p_rect->bottom  = p_rect->top  + KANA_KEY_H;
    break;
  case KEYMAP_KEYTYPE_NONE:
    return FALSE;
  }
  
  switch( cp_cursor->y )
  { 
  case 1:
    if( cp_cursor->x > 9 ) 
    { 
      return FALSE;
    }
    break;
  case 2:
    if( cp_cursor->x > 9 ) 
    { 
      return FALSE;
    }
    break;
  case 3:
    if( cp_cursor->x > 5 ) 
    { 
      return FALSE;
    }
    break;
  }

  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���ȃJ�i�Ȃǂ̌�Ղ̖ڏ�̃J�[�\���ړ�
 *
 *  @param  GFL_POINT *p_now    �J�[�\��
 *  @param  GFL_POINT *cp_add   �ړ��l
 *  @param  p_buff              �J�[�\���ړ��L���o�b�t�@
 */
//-----------------------------------------------------------------------------
static void KeyMap_KANA_GetMoveCursor( GFL_POINT *p_now, const GFL_POINT *cp_add, KEYMOVE_BUFF *p_buff )
{
  //�ʒu
  enum
  { 
    ROW_START   = 0,
    ROW_KEY_END = 4,
    ROW_BTN     = 5,
    ROW_END     = ROW_BTN,

    COL_START   = 0,
    COL_KEY_END = 12,
    COL_BTN_END = 6,
  };


  BOOL is_btn_in  = FALSE;
  BOOL is_btn_out = FALSE;

  switch( p_now->y )
  { 
  case ROW_START:
    //�L�[�ŏ�i
    p_now->x  += cp_add->x;
    p_now->x  = MATH_ROUND( p_now->x, COL_START, COL_KEY_END );
    p_now->y  += cp_add->y;
    p_now->y  = MATH_ROUND( p_now->y, ROW_START, ROW_END );

    if( p_now->y == ROW_BTN )
    { 
      is_btn_in = TRUE;
    }
    break;
  case ROW_KEY_END:
    //�L�[�ŉ��i

    p_now->x  += cp_add->x;
    p_now->x  = MATH_ROUND( p_now->x, COL_START, COL_KEY_END );
    p_now->y  += cp_add->y;
    p_now->y  = MATH_ROUND( p_now->y, ROW_START, ROW_END );

    if( p_now->y == ROW_BTN )
    { 
      is_btn_in = TRUE;
    }
    break;
  case ROW_BTN:
    //�{�^��

    p_now->x  += cp_add->x;
    p_now->x  = MATH_ROUND( p_now->x, COL_START, COL_BTN_END );
    p_now->y  += cp_add->y;
    p_now->y  = MATH_ROUND( p_now->y, ROW_START, ROW_END );

    if( p_now->y != ROW_BTN )
    { 
      is_btn_out  = TRUE;
    }
    break;
  default:
    //�L�[����
    p_now->x  += cp_add->x;
    p_now->x  = MATH_ROUND( p_now->x, COL_START, COL_KEY_END );
    p_now->y  += cp_add->y;
    p_now->y  = MATH_ROUND( p_now->y, ROW_START, ROW_END );
  }

  //�{�^���ɓ������Ƃ��̏C��
  if( is_btn_in )
  {
    if( 5 <= p_now->x  && p_now->x <= 8 )
    { 
      //�����{�^��
      p_buff->data[KEYMAP_KEYMOVE_BUFF_DELETE]  = p_now->x;
      p_now->x  = 5;
    }
    else if( 9 <= p_now->x  && p_now->x <= 12 )
    { 
      //��߂�{�^��
      p_buff->data[KEYMAP_KEYMOVE_BUFF_DICEDE]  = p_now->x;
      p_now->x  = 6;
    }
  }

  //�{�^������o���Ƃ��̏C��
  if( is_btn_out )
  { 
    if( p_now->x == 5 )
    { 
      //�����{�^��
      p_now->x  = p_buff->data[KEYMAP_KEYMOVE_BUFF_DELETE];
    }
    else if( p_now->x == 6 )
    { 
      p_now->x  = p_buff->data[KEYMAP_KEYMOVE_BUFF_DICEDE];
    }
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  PC�̃L�[�{�[�h��̃J�[�\���ړ�
 *
 *  @param  GFL_POINT *p_now    �J�[�\��
 *  @param  GFL_POINT *cp_add   �ړ��l
 *  @param  p_buff              �J�[�\���ړ��L���o�b�t�@
 */
//-----------------------------------------------------------------------------
static void KeyMap_QWERTY_GetMoveCursor( GFL_POINT *p_now, const GFL_POINT *cp_add, KEYMOVE_BUFF *p_buff )
{ 
  //�ʒu
  enum
  { 
    ROW_START   = 0,
    ROW_LINE0   = ROW_START,
    ROW_LINE1,
    ROW_LINE2,
    ROW_LINE3,
    ROW_END     = ROW_LINE3,

    COL_START     = 0,
    COL_LINE1_END = 10,
    COL_LINE2_END = 9,
    COL_LINE3_END = 9,
    COL_LINE4_END = 5,
  };

  switch( p_now->y )
  { 
  case ROW_LINE0:
    p_now->x  += cp_add->x;
    p_now->x  = MATH_ROUND( p_now->x, COL_START, COL_LINE1_END );
    p_now->y  += cp_add->y;
    p_now->y  = MATH_ROUND( p_now->y, ROW_START, ROW_END );

    if( p_now->y == ROW_LINE3 )
    { 
      //�o�b�t�@
      switch( p_now->x )
      { 
      case 8:
      case 9:
        p_buff->data[KEYMAP_KEYMOVE_BUFF_L_U] = p_now->x;
        p_buff->data[KEYMAP_KEYMOVE_BUFF_SPACE_D] = p_now->x;
        p_buff->data[KEYMAP_KEYMOVE_BUFF_SPACE_U] = 8;
        break;
      case 4:
      case 5:
      case 6:
      case 7:
        p_buff->data[KEYMAP_KEYMOVE_BUFF_SPACE_D] = p_now->x;
        p_buff->data[KEYMAP_KEYMOVE_BUFF_SPACE_U] = p_now->x;
        break;
      case 10:
        p_buff->data[KEYMAP_KEYMOVE_BUFF_SPACE_D] = 10;
        p_buff->data[KEYMAP_KEYMOVE_BUFF_SPACE_U] = 9;
        break;
      }

      //ROW_LINE3�ֈړ�
      switch( p_now->x )
      { 
      case 0: //Q
        p_now->x  = p_buff->data[KEYMAP_KEYMOVE_BUFF_Q_U];
        break;
      case 1:
        /* fallthrough */
      case 2:
        /* fallthrough */
      case 3:
        p_now->x  += 1;
        break;
      default:  //4�`10
        p_now->x  = 5;
        break;
      }
    }
    else if( p_now->y == ROW_LINE1 )
    { 
      //�o�b�t�@
      switch( p_now->x )
      { 
      case 8:
      case 9:
        p_buff->data[KEYMAP_KEYMOVE_BUFF_L_U] = p_now->x;
        p_buff->data[KEYMAP_KEYMOVE_BUFF_SPACE_D] = p_now->x;
        p_buff->data[KEYMAP_KEYMOVE_BUFF_SPACE_U] = MATH_CLAMP( p_now->x, 4,9 );
        break;
      case 4:
      case 5:
      case 6:
      case 7:
      case 10:
        p_buff->data[KEYMAP_KEYMOVE_BUFF_SPACE_D] = p_now->x;
        p_buff->data[KEYMAP_KEYMOVE_BUFF_SPACE_U] = MATH_CLAMP( p_now->x, 4,9 );
        break;
      }

      //ROW_LINE1�ֈړ�
      switch( p_now->x )
      { 
      case 8:   //O
        /* fallthrough */
      case 9:   //P
        p_now->x  = 8;
        break;
      case 10:  //�~
        p_now->x  = 9;
        break;
      default:  //0�`7�܂ł͂��̂܂�
        p_now->x  = p_now->x;
        break;
      }
    }
    break;
  case ROW_LINE1:
    p_now->x  += cp_add->x;
    p_now->x  = MATH_ROUND( p_now->x, COL_START, COL_LINE2_END );
    p_now->y  += cp_add->y;
    p_now->y  = MATH_ROUND( p_now->y, ROW_START, ROW_END );
    if( p_now->y == ROW_LINE0 )
    { 
      //�o�b�t�@
      switch( p_now->x )
      { 
      case 7:
        /* fallthrough  */
      case 8:
        p_buff->data[KEYMAP_KEYMOVE_BUFF_BOU_U] = p_now->x;
        break;
      }

      //ROW_LINE0�ֈړ�
      switch( p_now->x )
      { 
      case 8: //L 
        p_now->x  = p_buff->data[KEYMAP_KEYMOVE_BUFF_L_U];
        break;
      case 9: //ENTER
        p_now->x  = 10;
        break;
      default:  //0�`�V
        p_now->x  = p_now->x;
        break;
      }
    }
    else if( p_now->y == ROW_LINE2 )
    { 
      //�o�b�t�@
      switch( p_now->x )
      { 
      case 7:
        /* fallthrough  */
      case 8:
        p_buff->data[KEYMAP_KEYMOVE_BUFF_BOU_U] = p_now->x;
        break;
      }

      //ROW_LINE2�ֈړ�
      switch( p_now->x )
      { 
      case 0: //a
        p_now->x  = p_buff->data[KEYMAP_KEYMOVE_BUFF_A_D];
        break;
      case 7:
        /* fallthrough */
      case 8:
        p_now->x  = 8;
        break;
      case 9:
        p_now->x  = 9;
        break;
      default:  //1�`6
        p_now->x  += 1;
        break;
      }
    }
    break;
  case ROW_LINE2:
    p_now->x  += cp_add->x;
    p_now->x  = MATH_ROUND( p_now->x, COL_START, COL_LINE3_END );
    p_now->y  += cp_add->y;
    p_now->y  = MATH_ROUND( p_now->y, ROW_START, ROW_END );
    if( p_now->y == ROW_LINE1 )
    { 
      //�o�b�t�@
      switch( p_now->x )
      { 
      case 0://SHIFT
      case 1://Z
        p_buff->data[KEYMAP_KEYMOVE_BUFF_A_D] = p_now->x;
        break;

      case 4:
      case 5:
      case 6:
      case 7:
      case 8: //4
      case 9:
        p_buff->data[KEYMAP_KEYMOVE_BUFF_SPACE_U] = p_now->x;
        p_buff->data[KEYMAP_KEYMOVE_BUFF_SPACE_D] = p_now->x;
        break;
      }

      //ROW_LINE1�ֈړ�
      switch( p_now->x )
      { 
      case 0: //SHIFT L
        /* fallthrough */
      case 1: //Z
        p_now->x  = 0;
        break;
      case 8: //-
        p_now->x  = p_buff->data[KEYMAP_KEYMOVE_BUFF_BOU_U];
        break;
      case 9: //SHIFT R
        p_now->x  = 9;
        break;
      default:  //2~7
        p_now->x  -= 1;
      }

    }
    else if( p_now->y == ROW_LINE3 )
    { 
      //�o�b�t�@
      switch( p_now->x )
      { 
      case 0://SHIFT
      case 1://Z
        p_buff->data[KEYMAP_KEYMOVE_BUFF_A_D] = p_now->x;
        break;

      case 4:
      case 5:
      case 6:
      case 7:
      case 8: //4�`9
        p_buff->data[KEYMAP_KEYMOVE_BUFF_SPACE_U] = p_now->x;
        p_buff->data[KEYMAP_KEYMOVE_BUFF_SPACE_D] = p_now->x;
        break;
      case 9: 
        p_buff->data[KEYMAP_KEYMOVE_BUFF_SPACE_U] = 9;
        p_buff->data[KEYMAP_KEYMOVE_BUFF_SPACE_D] = 10;
        break;
      }

      //ROW_LINE3�ֈړ�
      switch( p_now->x )
      { 
      case 0: //SHIFT L
        p_now->x  = p_buff->data[KEYMAP_KEYMOVE_BUFF_LSHIFT_D];
        break;
      case 1: //Z
        /* fallthrough */
      case 2: //X
        /* fallthrough */
      case 3: //C
        p_now->x  += 1;
        break;
      default: //4�`8
        p_now->x  = 5;
      }
    }
    break;
  case ROW_LINE3:
    p_now->x  += cp_add->x;
    p_now->x  = MATH_ROUND( p_now->x, COL_START, COL_LINE4_END );
    p_now->y  += cp_add->y;
    p_now->y  = MATH_ROUND( p_now->y, ROW_START, ROW_END );
    if( p_now->y == ROW_LINE2 )
    { 
      //�o�b�t�@����
      switch( p_now->x )
      { 
      case 0:
      case 1:
        p_buff->data[KEYMAP_KEYMOVE_BUFF_LSHIFT_D]  = p_now->x;
        p_buff->data[KEYMAP_KEYMOVE_BUFF_Q_U] = p_now->x;
        break;
      }

      //ROW_LINE2�ֈړ�
      switch( p_now->x )
      { 
      case 0: //����
        /* fallthrough */
      case 1: //�J�i
        p_now->x  = 0;
        break;
      case 2: //ABC
        /* fallthrough */
      case 3: //�L��
        /* fallthrough */
      case 4: //���[�}��
        p_now->x  -= 1;
        break;
      case 5: //SPCASE
        p_now->x  = p_buff->data[KEYMAP_KEYMOVE_BUFF_SPACE_U];
        break;
      }
    }
    else if( p_now->y == ROW_LINE0 )
    { 
      //�o�b�t�@����
      switch( p_now->x )
      { 
      case 0:
      case 1:
        p_buff->data[KEYMAP_KEYMOVE_BUFF_LSHIFT_D]  = p_now->x;
        p_buff->data[KEYMAP_KEYMOVE_BUFF_Q_U] = p_now->x;
        break;
      }

      //ROW_LINE0�ֈړ�
      switch( p_now->x )
      { 
      case 0: //����
        /* fallthrough */
      case 1: //�J�i
        p_now->x  = 0;
        break;
      case 2: //ABC
        /* fallthrough */
      case 3: //�L��
        /* fallthrough */
      case 4: //���[�}��
        p_now->x  -= 1;
        break;
      case 5: //SPCASE
        p_now->x  = p_buff->data[KEYMAP_KEYMOVE_BUFF_SPACE_D];
        break;
      }
    }
    break;
  }

}
//----------------------------------------------------------------------------
/**
 *  @brief  ���ȃJ�i�Ȃǂ̌�Ղ̖ڏ�̃L�[��ގ擾
 *
 *  @param  const GFL_POINT *cp_cursor  �J�[�\��
 *  @param  p_strpos                    �����󂯎����W
 *
 *  @return �L�[���
 */
//-----------------------------------------------------------------------------
static KEYMAP_KEYTYPE KeyMap_KANA_GetKeyType( const GFL_POINT *cp_cursor, GFL_POINT *p_strpos )
{ 
  if( cp_cursor->y < KEYMAP_KANA_HEIGHT )
  { 
    if( cp_cursor->y == 0 && cp_cursor->x == 12 )
    { 
      if( p_strpos )
      { 
        p_strpos->x = cp_cursor->x;
        p_strpos->y = cp_cursor->y;
      }
      //���_
      return KEYMAP_KEYTYPE_DAKUTEN;
    }
    else if( cp_cursor->y == 1 && cp_cursor->x == 12 )
    { 
      if( p_strpos )
      { 
        p_strpos->x = cp_cursor->x;
        p_strpos->y = cp_cursor->y;
      }
      //�����_
      return KEYMAP_KEYTYPE_HANDAKUTEN;
    }
    else
    { 
      //���̑�����
      //��Ղ̖ڏ�̓J�[�\���ʒu�Ɠ���
      if( p_strpos )
      { 
        p_strpos->x = cp_cursor->x;
        p_strpos->y = cp_cursor->y;
      }
      return KEYMAP_KEYTYPE_STR;
    }
  }
  else if( cp_cursor->x < 7 )
  { 
    return cp_cursor->x + KEYMAP_KEYTYPE_KANA;
  }

  return KEYMAP_KEYTYPE_NONE;
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���ȃJ�i�Ȃǂ̌�Ղ̖ڏ�̃L�[��ގ擾
 *
 *  @param  const GFL_POINT *cp_cursor  �J�[�\��
 *  @param  p_strpos                    �����󂯎����W
 *
 *  @return �L�[���
 */
//-----------------------------------------------------------------------------
static KEYMAP_KEYTYPE KeyMap_KIGOU_GetKeyType( const GFL_POINT *cp_cursor, GFL_POINT *p_strpos )
{ 
  if( cp_cursor->y < KEYMAP_KANA_HEIGHT )
  { 
    //���̑�����
    //��Ղ̖ڏ�̓J�[�\���ʒu�Ɠ���
    if( p_strpos )
    { 
      p_strpos->x = cp_cursor->x;
      p_strpos->y = cp_cursor->y;
    }

    return KEYMAP_KEYTYPE_STR;
  }
  else if( cp_cursor->x < 7 )
  { 
    return cp_cursor->x + KEYMAP_KEYTYPE_KANA;
  }

  return KEYMAP_KEYTYPE_NONE;
}
//----------------------------------------------------------------------------
/**
 *  @brief  PC�̃L�[�{�[�h��̃L�[��ގ擾
 *
 *  @param  const GFL_POINT *cp_cursor  �J�[�\��
 *  @param  GFL_POINT *p_strpos         �����󂯎����W
 *
 *  @return �L�[���
 */
//-----------------------------------------------------------------------------
static KEYMAP_KEYTYPE KeyMap_QWERTY_GetKeyType( const GFL_POINT *cp_cursor, GFL_POINT *p_strpos )
{ 
  GFL_POINT strpos;
  KEYMAP_KEYTYPE  ret = KEYMAP_KEYTYPE_NONE;

  //���ꂼ��̍s����L�[�̎�ނ��󂯎��
  switch( cp_cursor->y )
  {
  case 0:
    if( cp_cursor->x == 10 )
    { 
      ret =  KEYMAP_KEYTYPE_DELETE;
    }
    else
    { 
      strpos  = *cp_cursor;
      ret =  KEYMAP_KEYTYPE_STR;
    }
    break;
  case 1:
    if( cp_cursor->x == 9 )
    { 
      ret =  KEYMAP_KEYTYPE_DECIDE;
    }
    else
    { 
      strpos  = *cp_cursor;
      ret =  KEYMAP_KEYTYPE_STR;
    }
    break;
  case 2:
    if( cp_cursor->x == 0 || cp_cursor->x == 9 )
    { 
      ret =  KEYMAP_KEYTYPE_SHIFT;
    }
    else
    { 
      //���V�t�g�̕����炷
      strpos.x  = cp_cursor->x - 1;
      strpos.y  = cp_cursor->y;
      ret =  KEYMAP_KEYTYPE_STR;
    }
    break;
  case 3:
    if( cp_cursor->x == 5 )
    { 
      ret =  KEYMAP_KEYTYPE_SPACE;
    }
    else
    { 
      ret =  KEYMAP_KEYTYPE_KANA + cp_cursor->x;
    }
    break;
  }

  //�����ʒu���󂯎��
  if( ret == KEYMAP_KEYTYPE_STR )
  { 
    if( p_strpos )
    { 
      *p_strpos = strpos;
    }
  }


  return ret;
}
//=============================================================================
/**
 *          KEYBOARD_WORK
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  �L�[�{�[�h  ������
 *
 *  @param  KEYBOARD_WORK *p_wk ���[�N
 *  @param  mode    ���̓��[�h
 *  @param  p_font  �t�H���g
 *  @param  p_que   �L���[
 *  @param  heapID  �q�[�vID
 */
//-----------------------------------------------------------------------------
static void KEYBOARD_Init( KEYBOARD_WORK *p_wk, NAMEIN_INPUTTYPE mode, GFL_FONT *p_font, PRINT_QUE *p_que, NAMEIN_KEYMAP_HANDLE *p_keymap_handle, const OBJ_WORK *cp_obj, HEAPID heapID )
{ 
  //�N���A
  GFL_STD_MemClear( p_wk, sizeof(KEYBOARD_WORK) );
  p_wk->cursor.x  = 0;
  p_wk->cursor.y  = 0;
  p_wk->p_font    = p_font;
  p_wk->p_que     = p_que;
  p_wk->heapID    = heapID;
  p_wk->mode      = mode;
  p_wk->change_mode = mode;
  p_wk->p_keymap_handle = p_keymap_handle;

  //���[�h�ɂ���ēǂݍ��ރX�N���[�����Ⴄ�̂Ń������Ɏ���Ă���
  p_wk->p_scr_adrs[0] = GFL_ARC_UTIL_LoadScreen(ARCID_NAMEIN_GRA, NARC_namein_gra_name_romaji_NSCR, FALSE, &p_wk->p_scr[0], heapID );
  p_wk->p_scr_adrs[1] = GFL_ARC_UTIL_LoadScreen(ARCID_NAMEIN_GRA, NARC_namein_gra_name_kana_NSCR, FALSE, &p_wk->p_scr[1], heapID );

  if( mode == NAMEIN_INPUTTYPE_QWERTY )
  { 
    GFL_BG_LoadScreenBuffer( BG_FRAME_KEY_M, p_wk->p_scr[0]->rawData, 0x800 );
    GFL_BG_SetVisible( BG_FRAME_BTN_M, FALSE );
  }
  else
  { 
    GFL_BG_LoadScreenBuffer( BG_FRAME_KEY_M, p_wk->p_scr[1]->rawData, 0x800 );
    GFL_BG_SetVisible( BG_FRAME_BTN_M, TRUE );
  }
  GFL_BG_LoadScreenReq( BG_FRAME_KEY_M );

  //�L�[�A�j���쐬
  KEYANM_Init( &p_wk->keyanm, mode, cp_obj, heapID );

  //�L�[�}�b�v�쐬
  KEYMAP_Create( &p_wk->keymap, mode, p_wk->p_keymap_handle, heapID );

  //�L�[�pBMPWIN�쐬
  p_wk->p_bmpwin  = GFL_BMPWIN_Create( BG_FRAME_FONT_M, KEYBOARD_BMPWIN_X, KEYBOARD_BMPWIN_Y, KEYBOARD_BMPWIN_W, KEYBOARD_BMPWIN_H, PLT_BG_FONT_M, GFL_BMP_CHRAREA_GET_B );
  GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin );

  //�v�����g�L���[�ݒ�
  PRINT_UTIL_Setup( &p_wk->util, p_wk->p_bmpwin );

  //�`��
  KEYMAP_Print( &p_wk->keymap, &p_wk->util, p_que, p_font, heapID );


 //�L�[���[�h���L�[�Ȃ�΃J�[�\���ʒu������
  { 
    GFL_RECT rect;
    if( GFL_UI_CheckTouchOrKey() == GFL_APP_KTST_KEY )
    { 
      //���[�h�{�^��+�J�[�\���ʒu�����̂���
      if( KEYMAP_GetRange( &p_wk->keymap, &p_wk->cursor, &rect ) )
      { 
        KEYANM_Start( &p_wk->keyanm, KEYANM_TYPE_MOVE, &rect, p_wk->is_shift, p_wk->mode );  
      }
    }
    else
    { 
      //���[�h�{�^�������̂���
      if( KEYMAP_GetRange( &p_wk->keymap, &p_wk->cursor, &rect ) )
      { 
        KEYANM_Start( &p_wk->keyanm, KEYANM_TYPE_NONE, &rect, p_wk->is_shift, p_wk->mode );  
      }
    }
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  �L�[�{�[�h  �j��
 *
 *  @param  KEYBOARD_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void KEYBOARD_Exit( KEYBOARD_WORK *p_wk )
{ 
  //BMPWIN�j��
  GFL_BMPWIN_Delete( p_wk->p_bmpwin );

  //�L�[�}�b�v�j��
  KEYMAP_Delete( &p_wk->keymap );

  //�L�[�A�j���j��
  KEYANM_Exit( &p_wk->keyanm );

  GFL_HEAP_FreeMemory( p_wk->p_scr_adrs[0] );
  GFL_HEAP_FreeMemory( p_wk->p_scr_adrs[1] );

  //�N���A
  GFL_STD_MemClear( p_wk, sizeof(KEYBOARD_WORK) );
}
//----------------------------------------------------------------------------
/**
 *  @brief  �L�[�{�[�h  ���C������
 *
 *  @param  KEYBOARD_WORK *p_wk   ���[�N
 *  @param  STRINPUT_WORK *cp_strinput  �������͗�
 *
 */
//-----------------------------------------------------------------------------
static void KEYBOARD_Main( KEYBOARD_WORK *p_wk, const STRINPUT_WORK *cp_strinput )
{
  //���͂�������
  p_wk->input = KEYBOARD_INPUT_NONE;

  //��Ԃ��Ƃ̏���
  switch( p_wk->state )
  { 
  case KEYBOARD_STATE_WAIT:   //�����҂���
    { 
      BOOL  is_update = FALSE;
      KEYBOARD_INPUT_REQUEST req;

      GFL_STD_MemClear( &req, sizeof(KEYBOARD_INPUT_REQUEST) );

      is_update = Keyboard_TouchReq( p_wk, &req );
      if( !is_update )
      { 
        Keyboard_KeyReq( p_wk, &req );
        is_update = Keyboard_BtnReq( p_wk, &req );
      }

      if( is_update )
      { 
        Keyboard_Decide( p_wk, &req );
      }
    }
    break;

  case KEYBOARD_STATE_MOVE:   //�ړ���
    //�ړ����ɂ����[�h�ؑւ̂݉\
    { 
      BOOL  is_update = FALSE;
      KEYBOARD_INPUT_REQUEST req;

      GFL_STD_MemClear( &req, sizeof(KEYBOARD_INPUT_REQUEST) );

      is_update = Keyboard_Move_TouchReq( p_wk, &req );
      if( !is_update )
      { 
        Keyboard_Move_KeyReq( p_wk, &req );
        is_update = Keyboard_Move_BtnReq( p_wk, &req );
      }

      if( is_update )
      { 
        Keyboard_Move_Decide( p_wk, &req );
      }
    }

    //�ړ�
    if( Keyboard_MainMove( p_wk ) )
    { 
      p_wk->state = KEYBOARD_STATE_WAIT;
    }
    break;

  case KEYBOARD_STATE_INPUT:  //����
    if( STRINPUT_IsInputEnd( cp_strinput ) )
    { 
      KEYMAP_GetCursorByKeyType( &p_wk->keymap, KEYMAP_KEYTYPE_DECIDE, &p_wk->cursor );
      p_wk->input = KEYBOARD_INPUT_LAST;
    }
    p_wk->state = KEYBOARD_STATE_WAIT;
    break;

  default:
    GF_ASSERT(0);
  }

  { 
    GFL_RECT rect;
    KEYMAP_GetRange( &p_wk->keymap, &p_wk->cursor, &rect );
    KEYANM_Main( &p_wk->keyanm, p_wk->mode, &rect );
  }

  KEYBOARD_PrintMain( p_wk );
}
//----------------------------------------------------------------------------
/**
 *  @brief  �L�[�{�[�h  ���݂̏����擾
 *
 *  @param  const KEYBOARD_WORK *cp_wk ���[�N
 *
 *  @return ���
 */
//-----------------------------------------------------------------------------
static KEYBOARD_STATE KEYBOARD_GetState( const KEYBOARD_WORK *cp_wk )
{ 
  return cp_wk->state;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �L�[�{�[�h  ���݂̓��͂��擾
 *
 *  @param  const KEYBOARD_WORK *cp_wk  ���[�N
 *  @param  *p_str                      ���͂���������
 *
 *  @return ���͏�
 */
//-----------------------------------------------------------------------------
static KEYBOARD_INPUT KEYBOARD_GetInput( const KEYBOARD_WORK *cp_wk, STRCODE *p_str )
{ 

  //���͂����Ƃ��͕������Ԃ�
  if( cp_wk->input == KEYBOARD_INPUT_STR ||
      cp_wk->input == KEYBOARD_INPUT_CHANGESTR ||
      cp_wk->input == KEYBOARD_INPUT_SPACE
      )
  { 
    if( p_str )
    { 
      *p_str  = cp_wk->code;
    }
  }

  return cp_wk->input;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �L�[�{�[�h  �V�t�g�����Ă��邩�ǂ���
 *
 *  @param  const KEYBOARD_WORK *cp_wk  ���[�N
 *
 *  @return TRUE �V�t�g�����Ă��� FALSE�V�t�g�����ĂȂ�
 */
//-----------------------------------------------------------------------------
static BOOL KEYBOARD_IsShift( const KEYBOARD_WORK *cp_wk )
{ 
  return cp_wk->is_shift;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �L�[�{�[�h  �`�惁�C��
 *
 *  @param  KEYBOARD_WORK *p_wk ���[�N
 *
 *  @retval  BOOL  �]�����I����Ă���ꍇ��TRUE�^�I����Ă��Ȃ��ꍇ��FALSE
 */
//-----------------------------------------------------------------------------
static BOOL KEYBOARD_PrintMain( KEYBOARD_WORK *p_wk )
{ 
  return PRINT_UTIL_Trans( &p_wk->util, p_wk->p_que );
}
//----------------------------------------------------------------------------
/**
 *  @brief  �L�[�{�[�h  ���̓��[�h��Ԃ�
 *
 *  @param  const KEYBOARD_WORK *cp_wk  ���[�N
 *
 *  @return ���̓��[�h��Ԃ�
 */
//-----------------------------------------------------------------------------
static NAMEIN_INPUTTYPE KEYBOARD_GetInputType( const KEYBOARD_WORK *cp_wk )
{ 
  return cp_wk->mode;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �L�[�{�[�h  ���̓��[�h�ύX���̓���A�j���J�n
 *
 *  @param  KEYBOARD_WORK *p_wk ���[�N
 *  @param  mode                ���[�h
 *  @retval TRUE �J�n�\ FALSE�s�\
 */
//-----------------------------------------------------------------------------
static BOOL Keyboard_StartMove( KEYBOARD_WORK *p_wk, NAMEIN_INPUTTYPE mode )
{ 
  if( p_wk->change_mode != mode )
  { 
    p_wk->change_mode     = mode;
    p_wk->change_move_cnt = 0;
    p_wk->change_move_seq = 0;
    p_wk->is_change_anm   = TRUE;

    p_wk->is_btn_move  = p_wk->change_mode == NAMEIN_INPUTTYPE_QWERTY
                        || p_wk->mode == NAMEIN_INPUTTYPE_QWERTY;

  
    if( p_wk->is_btn_move )
    { 
      //�t�H���g�ƃt���[���ƃ{�^��������
//      G2_SetBlendBrightness( GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 |
//          GX_BLEND_PLANEMASK_BG3, KEYBOARD_CHANGEMOVE_START_ALPHA );
  
      G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3,
          GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 |
          GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_BD,
          KEYBOARD_CHANGEMOVE_START_ALPHA, 16 );
    }
    else
    {
      //�t�H���g�ƃt���[����������
//      G2_SetBlendBrightness( GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG3,
//          KEYBOARD_CHANGEMOVE_START_ALPHA );
//
      G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG3,
          GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 |
          GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_BD,
          KEYBOARD_CHANGEMOVE_START_ALPHA, 16 );
    }
    return TRUE;
  }
  return FALSE;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �L�[�{�[�h  ���̓��[�h�ύX���̓���A�j������
 *
 *  @param  KEYBOARD_WORK *p_wk ���[�N
 *
 *  @retval TRUE�I��  FALSE������or�������Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL Keyboard_MainMove( KEYBOARD_WORK *p_wk )
{
  enum
  { 
    SEQ_DOWN_MAIN,
    SEQ_CHANGE_MODE,
    SEQ_UP_INIT,
    SEQ_UP_MAIN,
    SEQ_END,
  };
  s16 scroll_y;
  s16 alpha;

  if( p_wk->is_change_anm )
  { 
    switch( p_wk->change_move_seq )
    { 
    case SEQ_DOWN_MAIN:
      GFL_BG_GetScrollY( BG_FRAME_KEY_M );
      scroll_y  = KEYBOARD_CHANGEMOVE_START_Y
        + (KEYBOARD_CHANGEMOVE_END_Y-KEYBOARD_CHANGEMOVE_START_Y)
        * p_wk->change_move_cnt / KEYBOARD_CHANGEMOVE_SYNC;

      alpha     = KEYBOARD_CHANGEMOVE_START_ALPHA
        + (KEYBOARD_CHANGEMOVE_END_ALPHA-KEYBOARD_CHANGEMOVE_START_ALPHA)
        * p_wk->change_move_cnt / KEYBOARD_CHANGEMOVE_SYNC;

      G2_ChangeBlendAlpha( alpha, 16 );
      //G2_ChangeBlendBrightness( alpha );
      GFL_BG_SetScroll( BG_FRAME_KEY_M, GFL_BG_SCROLL_Y_SET, scroll_y );
      GFL_BG_SetScroll( BG_FRAME_FONT_M, GFL_BG_SCROLL_Y_SET, scroll_y );
      if( p_wk->is_btn_move )
      { 
        GFL_BG_SetScroll( BG_FRAME_BTN_M, GFL_BG_SCROLL_Y_SET, scroll_y );
      }
      if( p_wk->change_move_cnt++ > KEYBOARD_CHANGEMOVE_SYNC )
      { 
        if( p_wk->is_btn_move )
        { 
          GFL_BG_SetVisible( BG_FRAME_KEY_M, FALSE );
          GFL_BG_SetVisible( BG_FRAME_BTN_M, FALSE );
          GFL_BG_SetVisible( BG_FRAME_FONT_M, FALSE );
        }
        else
        { 
          GFL_BG_SetVisible( BG_FRAME_KEY_M, FALSE );
          GFL_BG_SetVisible( BG_FRAME_FONT_M, FALSE );
        }
        p_wk->change_move_cnt = 0;
        p_wk->change_move_seq     = SEQ_CHANGE_MODE;
      }
      break;

    case SEQ_CHANGE_MODE:
      GFL_BG_SetScroll( BG_FRAME_KEY_M, GFL_BG_SCROLL_Y_SET, KEYBOARD_CHANGEMOVE_END_Y );
      GFL_BG_SetScroll( BG_FRAME_FONT_M, GFL_BG_SCROLL_Y_SET, KEYBOARD_CHANGEMOVE_END_Y );
      if( p_wk->is_btn_move )
      { 
        GFL_BG_SetScroll( BG_FRAME_BTN_M, GFL_BG_SCROLL_Y_SET, KEYBOARD_CHANGEMOVE_END_Y );
      }

      Keyboard_ChangeMode( p_wk, p_wk->change_mode );
      p_wk->change_move_seq = SEQ_UP_INIT;
      break;

    case SEQ_UP_INIT:
      p_wk->change_move_cnt = KEYBOARD_CHANGEMOVE_SYNC;
      p_wk->change_move_seq = SEQ_UP_MAIN;
      GFL_BG_SetVisible( BG_FRAME_KEY_M, TRUE );
      GFL_BG_SetVisible( BG_FRAME_FONT_M, TRUE );
      if( p_wk->mode != NAMEIN_INPUTTYPE_QWERTY )
      { 
        GFL_BG_SetVisible( BG_FRAME_BTN_M, TRUE );
      }
      G2_ChangeBlendAlpha( 0, 16 );
      break;

    case SEQ_UP_MAIN:
      scroll_y  = KEYBOARD_CHANGEMOVE_START_Y
        + (KEYBOARD_CHANGEMOVE_END_Y-KEYBOARD_CHANGEMOVE_START_Y)
        * p_wk->change_move_cnt / KEYBOARD_CHANGEMOVE_SYNC;

      alpha     = KEYBOARD_CHANGEMOVE_START_ALPHA
        + (KEYBOARD_CHANGEMOVE_END_ALPHA-KEYBOARD_CHANGEMOVE_START_ALPHA)
        * p_wk->change_move_cnt / KEYBOARD_CHANGEMOVE_SYNC;

      G2_ChangeBlendAlpha( alpha, 16 );
      //G2_ChangeBlendBrightness( alpha );
      GFL_BG_SetScroll( BG_FRAME_KEY_M, GFL_BG_SCROLL_Y_SET, scroll_y );
      GFL_BG_SetScroll( BG_FRAME_FONT_M, GFL_BG_SCROLL_Y_SET, scroll_y );
      if( p_wk->is_btn_move )   
      { 
        GFL_BG_SetScroll( BG_FRAME_BTN_M, GFL_BG_SCROLL_Y_SET, scroll_y );
      }
      if( p_wk->change_move_cnt-- == 0 )
      { 
        p_wk->change_move_seq = SEQ_END;
      }
      break;

    case SEQ_END:
      GFL_BG_SetScroll( BG_FRAME_KEY_M, GFL_BG_SCROLL_Y_SET, KEYBOARD_CHANGEMOVE_START_Y );
      GFL_BG_SetScroll( BG_FRAME_FONT_M, GFL_BG_SCROLL_Y_SET, KEYBOARD_CHANGEMOVE_START_Y );
      GFL_BG_SetScroll( BG_FRAME_BTN_M, GFL_BG_SCROLL_Y_SET, KEYBOARD_CHANGEMOVE_START_Y );

      p_wk->is_btn_move = FALSE;

      G2_BlendNone();
      p_wk->is_change_anm = FALSE;
      return TRUE;
    }
  }

  return FALSE;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �L�[�{�[�h  ���[�h�ؑ�
 *
 *  @param  KEYBOARD_WORK *p_wk ���[�N
 *  @param  mode                ���[�h
 */
//-----------------------------------------------------------------------------
static void Keyboard_ChangeMode( KEYBOARD_WORK *p_wk, NAMEIN_INPUTTYPE mode )
{ 
  if( p_wk->mode != mode )
  { 
    //�L�[�z��쐬���Ȃ���
    KEYMAP_Delete( &p_wk->keymap );
    KEYMAP_Create( &p_wk->keymap, mode, p_wk->p_keymap_handle, p_wk->heapID );


    //�L�[�z��`��
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 0 );
    KEYMAP_Print( &p_wk->keymap, &p_wk->util, p_wk->p_que, p_wk->p_font, p_wk->heapID );

    //���[�h�ɂ��A�ǂݍ��ރX�N���[����؂�ւ�
    if( mode == NAMEIN_INPUTTYPE_QWERTY )
    { 
      GFL_BG_LoadScreenBuffer( BG_FRAME_KEY_M, p_wk->p_scr[0]->rawData, 0x800 );
      GFL_BG_SetVisible( BG_FRAME_BTN_M, FALSE );
    }
    else
    { 
      GFL_BG_LoadScreenBuffer( BG_FRAME_KEY_M, p_wk->p_scr[1]->rawData, 0x800 );
    }
    GFL_BG_LoadScreenReq( BG_FRAME_KEY_M );

    //�O���オQWERTY�������ꍇ�A�J�[�\���ʒu�ύX
    if( mode == NAMEIN_INPUTTYPE_QWERTY || p_wk->mode == NAMEIN_INPUTTYPE_QWERTY )
    { 
      KEYMAP_TOOL_ChangeCursorPos( &p_wk->cursor, p_wk->mode, mode );
    }

    //�ύX�����̂Ń��[�h���
    p_wk->mode  = mode;
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  �L�[����
 *
 *  @param  KEYBOARD_WORK *p_wk ���[�N
 *  @param  *p_req              ���N�G�X�g���
 */
//-----------------------------------------------------------------------------
static BOOL Keyboard_KeyReq( KEYBOARD_WORK *p_wk, KEYBOARD_INPUT_REQUEST *p_req )
{ 
  
  GFL_POINT pos;
  BOOL is_move  = FALSE;

  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_UP )
  { 
    pos.x = 0;
    pos.y = -1;
    is_move = TRUE;
  }
  else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN )
  { 
    pos.x = 0;
    pos.y = 1;
    is_move = TRUE;
  }
  else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT )
  { 
    pos.x = -1;
    pos.y = 0;
    is_move = TRUE;
  }
  else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT )
  { 
    pos.x = 1;
    pos.y = 0;
    is_move = TRUE;
  }
  else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START )
  {
    //����{�^���Ɉړ����邾��
    pos.x = 0;
    pos.y = 0;
    is_move = TRUE;

    KEYMAP_GetCursorByKeyType( &p_wk->keymap, KEYMAP_KEYTYPE_DECIDE, &p_wk->cursor );
    KEYMAP_MoveCursor( &p_wk->keymap, &p_wk->cursor, &pos );
  }

  if( is_move )
  { 
    GFL_RECT rect;

#ifdef NAMEIN_KEY_TOUCH
    //�ȑO���^�b�`��ԂȂ�΁A�������A�L�[��Ԃ֐؂�ւ��
    if( GFL_UI_CheckTouchOrKey() == GFL_APP_KTST_TOUCH )
    {
      if( KEYMAP_GetRange( &p_wk->keymap, &p_wk->cursor, &rect ) )
      { 
        GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
        KEYANM_Start( &p_wk->keyanm, KEYANM_TYPE_MOVE, &rect, p_wk->is_shift, p_wk->mode );
        PMSND_PlaySE( NAMEIN_SE_MOVE_CURSOR );
      }
    }
    else
#endif
    { 
      //�ȑO���L�[��ԂȂ�΁A�ړ��A�j��
      KEYMAP_MoveCursor( &p_wk->keymap, &p_wk->cursor, &pos );
      if( KEYMAP_GetRange( &p_wk->keymap, &p_wk->cursor, &rect ) )
      { 
        PMSND_PlaySE( NAMEIN_SE_MOVE_CURSOR );
        KEYANM_Start( &p_wk->keyanm, KEYANM_TYPE_MOVE, &rect, p_wk->is_shift, p_wk->mode );
      }
    }
  }

  return FALSE;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �{�^������
 *
 *  @param  KEYBOARD_WORK *p_wk ���[�N
 *  @param  *p_req              ���N�G�X�g���
 */
//-----------------------------------------------------------------------------
static BOOL Keyboard_BtnReq( KEYBOARD_WORK *p_wk, KEYBOARD_INPUT_REQUEST *p_req )
{
  BOOL ret  = FALSE;

  if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_DECIDE )
  { 
    p_req->anm_pos  = p_wk->cursor;
    p_req->is_push    = TRUE;

    ret = TRUE;
  }
  if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_CANCEL )
  { 
    GFL_POINT pos;
    //�A�j��
    KEYMAP_GetCursorByKeyType( &p_wk->keymap, KEYMAP_KEYTYPE_DELETE, &p_req->anm_pos );
    //�o�b�N�X�y�[�X�ɂȂ�
    p_req->type = KEYMAP_KEYTYPE_DELETE;
    ret = TRUE;
  }
  if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_SELECT )
  { 
    GFL_POINT pos;
    GFL_RECT  rect;
    //���[�h���z�ړ�
    p_req->type = KEYMAP_KEYTYPE_KANA + p_wk->mode + 1;
    if( p_req->type > KEYMAP_KEYTYPE_QWERTY )
    { 
      p_req->type = KEYMAP_KEYTYPE_KANA;
    }
    //�A�j��
    KEYMAP_GetCursorByKeyType( &p_wk->keymap, p_req->type, &p_wk->cursor );
    p_req->anm_pos  = p_wk->cursor;
    ret = TRUE;
  }
  if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_R )
  { 
    //�����ϊ�
    //�A�j���͂��Ȃ�
    p_wk->input = KEYBOARD_INPUT_AUTOCHANGE;
    ret = FALSE;
  }

#ifdef NAMEIN_KEY_TOUCH
  if( ret && GFL_UI_CheckTouchOrKey() == GFL_APP_KTST_TOUCH )
  {
    GFL_RECT rect;
    if( KEYMAP_GetRange( &p_wk->keymap, &p_wk->cursor, &rect ) )
    { 
      PMSND_PlaySE( NAMEIN_SE_MOVE_CURSOR );
      GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
      KEYANM_Start( &p_wk->keyanm, KEYANM_TYPE_MOVE, &rect, p_wk->is_shift, p_wk->mode );
      return FALSE;
    }
  }
#endif

  return ret;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �^�b�`����
 *
 *  @param  KEYBOARD_WORK *p_wk ���[�N
 *  @param  *p_req              ���N�G�X�g���
 */
//-----------------------------------------------------------------------------
static BOOL Keyboard_TouchReq( KEYBOARD_WORK *p_wk, KEYBOARD_INPUT_REQUEST *p_req )
{ 
  GFL_POINT pos;
  u32 x, y;
  if( GFL_UI_TP_GetPointTrg( &x, &y ) )
  { 
    pos.x = x;
    pos.y = y;
    if( KEYMAP_GetTouchCursor( &p_wk->keymap, &pos, &p_wk->cursor ) )
    { 
      p_req->anm_pos    = p_wk->cursor;
      p_req->is_push    = TRUE;
      GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
      return TRUE;
    }
  }
  return FALSE;
}
//----------------------------------------------------------------------------
/**
 *  @brief  ����
 *
 *  @param  KEYBOARD_WORK *p_wk ���[�N
 *  @param  *p_req              ���N�G�X�g���
 */
//-----------------------------------------------------------------------------
static void Keyboard_Decide( KEYBOARD_WORK *p_wk, const KEYBOARD_INPUT_REQUEST *cp_req )
{ 
  GFL_RECT rect;
  KEYMAP_KEYTYPE  type;

  if( cp_req->is_push )
  { 
    type  = KEYMAP_GetKeyInfo( &p_wk->keymap, &p_wk->cursor, &p_wk->code );
  }
  else
  { 
    type  = cp_req->type;
  }

  switch( type )
  { 
  case KEYMAP_KEYTYPE_STR:      //��������
    //QWERTY�̂Ƃ��͕ϊ�����
    //���̂Ƃ��͒ʏ����
    p_wk->input = p_wk->mode == NAMEIN_INPUTTYPE_QWERTY ?
      KEYBOARD_INPUT_CHANGESTR: KEYBOARD_INPUT_STR;
    p_wk->state = KEYBOARD_STATE_INPUT;
    break;
  case KEYMAP_KEYTYPE_SPACE:    //�X�y�[�X����
    p_wk->input = KEYBOARD_INPUT_SPACE;
    p_wk->state = KEYBOARD_STATE_INPUT;
    break;

  case KEYMAP_KEYTYPE_KANA:   //���ȃ��[�h
    /* fallthrough */
  case KEYMAP_KEYTYPE_KATA:   //�J�i���[�h
    /* fallthrough */
  case KEYMAP_KEYTYPE_ABC:      //�A���t�@�x�b�g���[�h
    /* fallthrough */
  case KEYMAP_KEYTYPE_KIGOU:    //�L�����[�h
    /* fallthrough */
  case KEYMAP_KEYTYPE_QWERTY: //���[�}�����[�h
    if( Keyboard_StartMove( p_wk, type - KEYMAP_KEYTYPE_KANA ) )
    { 
      PMSND_PlaySE( NAMEIN_SE_CHANGE_MODE );

      p_wk->state = KEYBOARD_STATE_MOVE;
      p_wk->input = KEYBOARD_INPUT_CHAGETYPE;
    }
    break;

  case KEYMAP_KEYTYPE_DELETE: //����
    p_wk->input = KEYBOARD_INPUT_BACKSPACE;
    break;

  case KEYMAP_KEYTYPE_DECIDE: //��߂�
    p_wk->input = KEYBOARD_INPUT_EXIT;

    break;

  case KEYMAP_KEYTYPE_SHIFT:    //�V�t�g
    PMSND_PlaySE( NAMEIN_SE_DECIDE_STR );
    p_wk->input = KEYBOARD_INPUT_SHIFT;
    p_wk->is_shift  ^= 1;
    break;

  case KEYMAP_KEYTYPE_DAKUTEN:    //���_
    p_wk->input = KEYBOARD_INPUT_DAKUTEN;
    break;

  case KEYMAP_KEYTYPE_HANDAKUTEN:   //�����_
    p_wk->input = KEYBOARD_INPUT_HANDAKUTEN;
    break;
  }

  //����A�j��
  if( KEYMAP_GetRange( &p_wk->keymap, &cp_req->anm_pos, &rect ) )
  { 
    KEYANM_Start( &p_wk->keyanm, KEYANM_TYPE_DECIDE, &rect, p_wk->is_shift, p_wk->mode );
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  �L�[
 *
 *  @param  KEYBOARD_WORK *p_wk ���[�N
 *  @param  *p_req              ���N�G�X�g���
 */
//-----------------------------------------------------------------------------
static BOOL Keyboard_Move_KeyReq( KEYBOARD_WORK *p_wk, KEYBOARD_INPUT_REQUEST *p_req )
{ 
  GFL_POINT pos;
  GFL_POINT next_cursor;
  BOOL is_move  = FALSE;

  if( !p_wk->is_btn_move )
  { 
    if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP )
    { 
      pos.x = 0;
      pos.y = -1;
      is_move = TRUE;
    }
    else if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN )
    { 
      pos.x = 0;
      pos.y = 1;
      is_move = TRUE;
    }
    else if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT )
    { 
      pos.x = -1;
      pos.y = 0;
      is_move = TRUE;
    }
    else if( GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT )
    { 
      pos.x = 1;
      pos.y = 0;
      is_move = TRUE;
    }
  }

  if( is_move )
  { 
    KEYMAP_KEYTYPE  type;
    GFL_RECT rect;

    //�����̍��W���쐬
    next_cursor = p_wk->cursor;
    KEYMAP_MoveCursor( &p_wk->keymap, &next_cursor, &pos );
    type  = KEYMAP_GetKeyInfo( &p_wk->keymap, &next_cursor, NULL );
    if( type == KEYMAP_KEYTYPE_KANA
        || type == KEYMAP_KEYTYPE_KATA
        || type == KEYMAP_KEYTYPE_ABC
        || type == KEYMAP_KEYTYPE_KIGOU
        || type == KEYMAP_KEYTYPE_QWERTY
      )
    { 

#ifdef NAMEIN_KEY_TOUCH
      //�ȑO���^�b�`��ԂȂ�΁A�������A�L�[��Ԃ֐؂�ւ��
      if( GFL_UI_CheckTouchOrKey() == GFL_APP_KTST_TOUCH )
      {
        if( KEYMAP_GetRange( &p_wk->keymap, &p_wk->cursor, &rect ) )
        { 
          //�ړ��A�j��
          PMSND_PlaySE( NAMEIN_SE_MOVE_CURSOR );
          GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
          KEYANM_Start( &p_wk->keyanm, KEYANM_TYPE_MOVE, &rect, p_wk->is_shift, p_wk->mode );
        }
      }
      else
#endif
      { 
        KEYMAP_MoveCursor( &p_wk->keymap, &p_wk->cursor, &pos );
        //�ړ��A�j��
        if( KEYMAP_GetRange( &p_wk->keymap, &p_wk->cursor, &rect ) )
        { 
          PMSND_PlaySE( NAMEIN_SE_MOVE_CURSOR );
          KEYANM_Start( &p_wk->keyanm, KEYANM_TYPE_MOVE, &rect, p_wk->is_shift, p_wk->mode );
        }
      }
    }
  }

  return FALSE;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �{�^��
 *
 *  @param  KEYBOARD_WORK *p_wk ���[�N
 *  @param  *p_req              ���N�G�X�g���
 */
//-----------------------------------------------------------------------------
static BOOL Keyboard_Move_BtnReq( KEYBOARD_WORK *p_wk, KEYBOARD_INPUT_REQUEST *p_req )
{ 
  BOOL ret  = FALSE;

  if( !p_wk->is_btn_move )
  { 
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE )
    { 
      p_req->anm_pos  = p_wk->cursor;
      p_req->is_push    = TRUE;

      ret = TRUE;
    }
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT )
    { 
      GFL_POINT pos;
      //���[�h���z�ړ�
      p_req->type = KEYMAP_KEYTYPE_KANA + p_wk->mode + 1;
      if( p_req->type > KEYMAP_KEYTYPE_QWERTY )
      { 
        p_req->type = KEYMAP_KEYTYPE_KANA;
      }
      //�A�j��
      KEYMAP_GetCursorByKeyType( &p_wk->keymap, p_req->type, &p_wk->cursor );
      p_req->anm_pos  = p_wk->cursor;

      ret = TRUE;
    }

#ifdef NAMEIN_KEY_TOUCH
    if( ret && GFL_UI_CheckTouchOrKey() == GFL_APP_KTST_TOUCH )
    {
      GFL_RECT  rect;
      if( KEYMAP_GetRange( &p_wk->keymap, &p_wk->cursor, &rect ) )
      { 
        PMSND_PlaySE( NAMEIN_SE_MOVE_CURSOR );
        GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
        KEYANM_Start( &p_wk->keyanm, KEYANM_TYPE_MOVE, &rect, p_wk->is_shift, p_wk->mode );  
        return FALSE;
      }
    }
#endif
  }

  return ret;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �^�b�`
 *
 *  @param  KEYBOARD_WORK *p_wk ���[�N
 *  @param  *p_req              ���N�G�X�g���
 */
//-----------------------------------------------------------------------------
static BOOL Keyboard_Move_TouchReq( KEYBOARD_WORK *p_wk, KEYBOARD_INPUT_REQUEST *p_req )
{ 
  GFL_POINT pos;
  u32 x, y;

  //QWERTY�̓^�b�`�ł��Ȃ�
  //���[�h�ȊO�^�b�`�ł��Ȃ�
  if( !p_wk->is_btn_move )
  { 
    if( GFL_UI_TP_GetPointTrg( &x, &y ) )
    { 
      pos.x = x;
      pos.y = y;
      if( KEYMAP_GetTouchCursor( &p_wk->keymap, &pos, &p_wk->cursor ) )
      { 
        KEYMAP_KEYTYPE  type;
        type  = KEYMAP_GetKeyInfo( &p_wk->keymap, &p_wk->cursor, NULL );
        if( type == KEYMAP_KEYTYPE_KANA
            || type == KEYMAP_KEYTYPE_KATA
            || type == KEYMAP_KEYTYPE_ABC
            || type == KEYMAP_KEYTYPE_KIGOU
            || type == KEYMAP_KEYTYPE_QWERTY
            )
        { 
          p_req->anm_pos    = p_wk->cursor;
          p_req->is_push    = TRUE;
          GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
          return TRUE;
        }
      }
    }
  }
  return FALSE;
}
//----------------------------------------------------------------------------
/**
 *  @brief  ����
 *
 *  @param  KEYBOARD_WORK *p_wk ���[�N
 *  @param  *p_req              ���N�G�X�g���
 */
//-----------------------------------------------------------------------------
static void Keyboard_Move_Decide( KEYBOARD_WORK *p_wk, const KEYBOARD_INPUT_REQUEST *cp_req )
{ 
  GFL_RECT rect;
  KEYMAP_KEYTYPE  type;

  if( cp_req->is_push )
  { 
    type  = KEYMAP_GetKeyInfo( &p_wk->keymap, &p_wk->cursor, &p_wk->code );
  }
  else
  { 
    type  = cp_req->type;
  }

  switch( type )
  {

  case KEYMAP_KEYTYPE_KANA:   //���ȃ��[�h
    /* fallthrough */
  case KEYMAP_KEYTYPE_KATA:   //�J�i���[�h
    /* fallthrough */
  case KEYMAP_KEYTYPE_ABC:      //�A���t�@�x�b�g���[�h
    /* fallthrough */
  case KEYMAP_KEYTYPE_KIGOU:    //�L�����[�h
    /* fallthrough */
  case KEYMAP_KEYTYPE_QWERTY: //���[�}�����[�h
    if( Keyboard_StartMove( p_wk, type - KEYMAP_KEYTYPE_KANA ) )
    { 
      PMSND_PlaySE( NAMEIN_SE_CHANGE_MODE );

      p_wk->state = KEYBOARD_STATE_MOVE;
      p_wk->input = KEYBOARD_INPUT_CHAGETYPE;
    }
    break;
  }


  //����A�j��
  if( KEYMAP_GetRange( &p_wk->keymap, &cp_req->anm_pos, &rect ) )
  { 
    KEYANM_Start( &p_wk->keyanm, KEYANM_TYPE_DECIDE, &rect, p_wk->is_shift, p_wk->mode );
  }
}
//=============================================================================
/**
 *          MSGWND
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�E�B���h�E  ������
 *  @param  MSGWND_WORK* p_wk ���[�N
 *  @param  *p_font           �t�H���g
 *  @param  *p_msg            ���b�Z�[�W�f�[�^
 *  @param  *p_que            �L���[
 *  @param  *p_word           �P��
 *  @param  heapID            �q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void MSGWND_Init( MSGWND_WORK* p_wk, GFL_FONT *p_font, GFL_MSGDATA *p_msg, PRINT_QUE *p_que, WORDSET *p_word, HEAPID heapID )
{ 
  //�N���A
  GFL_STD_MemClear( p_wk, sizeof(MSGWND_WORK) );
  //������
  p_wk->clear_chr = 0xF;
  p_wk->p_font    = p_font;
  p_wk->p_msg     = p_msg;
  p_wk->p_que     = p_que;
  p_wk->p_word    = p_word;

  //�o�b�t�@�쐬
  p_wk->p_strbuf  = GFL_STR_CreateBuffer( 255, heapID );

  //BMPWIN�쐬
  p_wk->p_bmpwin  = GFL_BMPWIN_Create( BG_FRAME_TEXT_S, MSGWND_BMPWIN_X, MSGWND_BMPWIN_Y, MSGWND_BMPWIN_W, MSGWND_BMPWIN_H, PLT_BG_FONT_S, GFL_BMP_CHRAREA_GET_B );

  //�t���[����������
  BmpWinFrame_Write( p_wk->p_bmpwin, WINDOW_TRANS_OFF, MSGWND_FRAME_CHR, PLT_BG_FRAME_S );

  //�v�����g�L���[�ݒ�
  PRINT_UTIL_Setup( &p_wk->util, p_wk->p_bmpwin );

  //�]��
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->clear_chr );
  GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin );
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�E�B���h�E  �j��
 *
 *  @param  MSGWND_WORK* p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void MSGWND_Exit( MSGWND_WORK* p_wk )
{ 
  //BMPWIN�j��
  GFL_BMPWIN_Delete( p_wk->p_bmpwin );

  //�o�b�t�@�j��
  GFL_STR_DeleteBuffer( p_wk->p_strbuf );

  //�N���A
  GFL_STD_MemClear( p_wk, sizeof(MSGWND_WORK) );
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�E�B���h�E  �v�����g�J�n
 *
 *  @param  MSGWND_WORK* p_wk ���[�N
 *  @param  strID             ����ID
 */
//-----------------------------------------------------------------------------
static void MSGWND_Print( MSGWND_WORK* p_wk, u32 strID )
{ 
  //��[����
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->clear_chr );  

  //������쐬
  GFL_MSG_GetString( p_wk->p_msg, strID, p_wk->p_strbuf );

  //������`��
  PRINT_UTIL_Print( &p_wk->util, p_wk->p_que, 0, 0, p_wk->p_strbuf, p_wk->p_font );
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�E�B���h�E  �P��o�^���ăv�����g�J�n
 *
 *  @param  MSGWND_WORK *p_wk ���[�N
 *  @param  strID       ����ID
 *  @param  mons_no     �����X�^�[�ԍ�
 *  @param  form        �t�H����
 *  @param  heapID      �o�b�t�@�쐬�p�q�[�vID
 */
//-----------------------------------------------------------------------------
static void MSGWND_ExpandPrintPoke( MSGWND_WORK *p_wk, u32 strID, u16 mons_no, u16 form, HEAPID heapID )
{ 
  //�o�^
  { 
    POKEMON_PARAM *p_pp;

    //PP�쐬
    p_pp  = PP_Create( mons_no, 1, 0, heapID );
    PP_Put( p_pp, ID_PARA_form_no, form );

    //�v�����g
    MSGWND_ExpandPrintPP( p_wk, strID, p_pp );

    //�o�b�t�@�N���A
    GFL_HEAP_FreeMemory( p_pp );
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�E�B���h�E  PP�̒P��o�^���ăv�����g�J�n
 *
 *  @param  MSGWND_WORK *p_wk     ���[�N
 *  @param  strID                 ����ID
 *  @param  POKEMON_PARAM *cp_pp  �|�P�����p����
 */
//-----------------------------------------------------------------------------
static void MSGWND_ExpandPrintPP( MSGWND_WORK *p_wk, u32 strID, const POKEMON_PARAM *cp_pp )
{ 
  //�P��o�^
  WORDSET_RegisterPokeMonsName( p_wk->p_word, 0, cp_pp );

  //�v�����g
  { 
    STRBUF  *p_strbuf;

    //��[����
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->clear_chr );  

    //������쐬
    p_strbuf  = GFL_MSG_CreateString( p_wk->p_msg, strID );

    //�P��W�J
    WORDSET_ExpandStr( p_wk->p_word, p_wk->p_strbuf, p_strbuf );  

    //������`��
    PRINT_UTIL_Print( &p_wk->util, p_wk->p_que, 0, 0, p_wk->p_strbuf, p_wk->p_font );

    //�o�b�t�@�N���A
    GFL_STR_DeleteBuffer( p_strbuf );
  }

}

//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�E�B���h�E  QUE�̕`��҂�
 *
 *  @param  const MSGWND_WORK* cp_wk  ���[�N
 *
 *  @return TRUE�ŃX�g���[���I��  FALSE�͏�����
 */
//-----------------------------------------------------------------------------
static BOOL MSGWND_PrintMain( MSGWND_WORK* p_wk )
{ 
  return PRINT_UTIL_Trans( &p_wk->util, p_wk->p_que );
}

//=============================================================================
/**
 *          PS(PrintStream)
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  �v�����g�X�g���[��  ������
 *  @param  PS_WORK* p_wk     ���[�N
 *  @param  heapID            �q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void PS_Init( PS_WORK* p_wk, HEAPID heapID )
{
  //�N���A
  GFL_STD_MemClear( p_wk, sizeof(PS_WORK) );

  // GFL_TCBL�V�X�e��
  p_wk->p_tcblsys = GFL_TCBL_Init( heapID, heapID, 1, 0 );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �v�����g�X�g���[��  �j��
 *
 *  @param  PS_WORK* p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void PS_Exit( PS_WORK* p_wk )
{
  // �v�����g�X�g���[��
  if( p_wk->p_stream )
  {
    PRINTSYS_PrintStreamDelete( p_wk->p_stream );
  }

  // GFL_TCBL�V�X�e��
  GFL_TCBL_Exit( p_wk->p_tcblsys );

  //�N���A
  GFL_STD_MemClear( p_wk, sizeof(PS_WORK) );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �|�P�����ߊl�Ń{�b�N�X�]���ɂȂ������̃v�����g�X�g���[���̏���
 *
 *  @param  PS_WORK* p_wk            PS���[�N
 *  @param  MSGWND_WORK* p_msgwnd_wk MSGWND���[�N
 *  @param  NAMEIN_PARAM* p_param    NAMEIN_PARAM���[�N
 *  @param  heapID                   �q�[�vID
 */
//-----------------------------------------------------------------------------
static void PS_SetupBox( PS_WORK* p_wk, MSGWND_WORK* p_msgwnd_wk, NAMEIN_PARAM* p_param, HEAPID heapID )
{
  const u32 tcbpri = 2;
  
  const u32 buf_id_nickname = 0;
  const u32 buf_id_box = 1;
  WORDSET *wordset;  // �|�P�����̃j�b�N�l�[���A�{�b�N�X�������Ă���

  // WORDSET����
  wordset = WORDSET_Create( heapID );
  
  // �|�P�����̃j�b�N�l�[���𓾂�
  if( NAMEIN_IsCancel(p_param) )
  {
    WORDSET_RegisterPokeNickName( wordset, buf_id_nickname, p_param->pp );
  }
  else
  {
    const u32  sex           = 0;  // WORDSET_RegisterPokeNickName�ł͕K�v�Ȃ��̂ŁA
    const BOOL singular_flag = 0;  // static void InitParam(WORDSET_PARAM* param)���Q�l�ɐ��l�ݒ�B
    const u32  lang          = 0;
    
    STRBUF *strbuf = GFL_STR_CreateCopyBuffer( p_param->strbuf, heapID );
    NAMEIN_CopyStr( p_param, strbuf );
    WORDSET_RegisterWord( wordset, buf_id_nickname, strbuf, sex, singular_flag, lang );
    GFL_STR_DeleteBuffer( strbuf );
  }

  // �{�b�N�X���𓾂�
  WORDSET_RegisterBoxName( wordset, buf_id_box, p_param->box_manager, p_param->box_tray );

  // �����`��֘A�̐ݒ���N���A����
  if( p_wk->p_stream )
  {
    PRINTSYS_PrintStreamDelete( p_wk->p_stream );
  }
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( p_msgwnd_wk->p_bmpwin ), p_msgwnd_wk->clear_chr );
  GFL_STR_ClearBuffer( p_msgwnd_wk->p_strbuf );
  
  // �����`��֘A�̐ݒ���s��
  WORDSET_ExpandStr( wordset, p_msgwnd_wk->p_strbuf, p_param->box_strbuf );
  p_wk->p_stream = PRINTSYS_PrintStream( p_msgwnd_wk->p_bmpwin, 0, 0, p_msgwnd_wk->p_strbuf,
                                         p_msgwnd_wk->p_font, MSGSPEED_GetWait(),
                                         p_wk->p_tcblsys, tcbpri, heapID, p_msgwnd_wk->clear_chr );

  // ���̑��̐ݒ���s��
  p_wk->next_seq_function = SEQFUNC_FadeIn;
  p_wk->wait_max = 60;
  p_wk->wait_count = 0;
 
  // WORDSET�j��
  WORDSET_Delete( wordset );
}

//=============================================================================
/**
 *    �A�C�R��
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  ICON_WORK *p_wk ���[�N
 *  @param  type    �A�C�R���̎��
 *  @param  param1  �����P  �A�C�R���̎�ނɂ���ĈႤ�ȉ��Q��
 *  @param  param2  �����Q  �A�C�R���̎�ނɂ���ĈႤ�ȉ��Q��
 *  @param  *p_unit OBJ�ǂ݂��ݗpCLUNIT
 *  @param  heapID  �q�[�vID
 *
 *  @note{  
 *    param1,param2�ɂ���
 *    ICON_TYPE_HERO�̏ꍇ
 *      param1�@�c  ����
 *    ICON_TYPE_PERSON�̏ꍇ
 *      param1  �c  �t�B�[���hOBJ�̎��
 *      param2  �c  ���g�p
 *    ICON_TYPE_POKE�̏ꍇ
 *      param1  �c  �����X�^�[�ԍ�
 *      param2  �c  ����8bit�F�t�H�����@���8bit�F����
 *    ICON_TYPE_BOX�̏ꍇ
 *      param1  �c  ���g�p
 *      param2  �c  ���g�p
 *  }
 */
//-----------------------------------------------------------------------------
static void ICON_Init( ICON_WORK *p_wk, ICON_TYPE type, u32 param1, u32 param2, GFL_CLUNIT *p_unit, GFL_FONT *p_font, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, BMPOAM_SYS_PTR p_bmpoam_sys, HEAPID heapID )
{ 
  BOOL is_sex_visible = FALSE;
  int sex;

  //�N���A
  GFL_STD_MemClear( p_wk, sizeof(ICON_WORK) );

  //�^�C�v�ʂ̓ǂ݂���
  { 
    ARCID     arcID;
    ARCDATID  plt, chr, cel, anm;
    u8        plt_src_ofs = 0;
    BOOL      is_comp = FALSE;
    ARCHANDLE * p_handle;

    switch( type )
    { 
    case ICON_TYPE_RIVAL:
      /* fallthrough */
    case ICON_TYPE_HERO:
      arcID = APP_COMMON_GetArcId();
      plt   = APP_COMMON_GetNull4x4PltArcIdx();
      chr   = APP_COMMON_GetNull4x4CharArcIdx();
      cel   = APP_COMMON_GetNull4x4CellArcIdx( APP_COMMON_MAPPING_128K );
      anm   = APP_COMMON_GetNull4x4AnimeArcIdx( APP_COMMON_MAPPING_128K );
      is_sex_visible  = FALSE;
      sex = param1;
      break;
    case ICON_TYPE_PERSON:
      arcID = ARCID_WIFIUNIONCHAR;
      plt   = NARC_wifi_unionobj_wifi_union_obj_NCLR;
      plt_src_ofs = sc_wifi_unionobj_plt[param1];
      chr   = NARC_wifi_unionobj_front00_NCGR+param1;
      cel   = NARC_wifi_unionobj_front00_NCER;
      anm   = NARC_wifi_unionobj_front00_NANR;
      is_sex_visible  = FALSE;
      sex = param1 < NELEMS(sc_wifi_unionobj_plt)/2 ? PTL_SEX_MALE : PTL_SEX_FEMALE;
      break;
    case ICON_TYPE_POKE:
      is_sex_visible  = TRUE;
      sex = ( param2 & 0xff00 ) >> 8;

      if( sex == PTL_SEX_UNKNOWN )
      { 
        is_sex_visible  = FALSE;
      }

      arcID = ARCID_POKEICON;
      plt   = POKEICON_GetPalArcIndex();
      chr   = POKEICON_GetCgxArcIndexByMonsNumber( param1, param2 & 0x00ff, sex, FALSE );
      cel   = POKEICON_GetCellArcIndex();
      anm   = POKEICON_GetAnmArcIndex();
      is_comp = TRUE;
      break;

    case ICON_TYPE_HATENA:
      /* fallthrough */
    case ICON_TYPE_BOX:
      arcID = ARCID_NAMEIN_GRA;
      plt   = NARC_namein_gra_name_obj_NCLR;
      chr   = NARC_namein_gra_name_obj_NCGR;
      cel   = NARC_namein_gra_name_obj_NCER;
      anm   = NARC_namein_gra_name_obj_NANR;
      is_sex_visible  = FALSE;
      plt_src_ofs = 1;
      break;
    default:
      GF_ASSERT(0);
    }

    //�ǂ݂���
    p_handle  = GFL_ARC_OpenDataHandle( arcID, heapID );
    if( is_comp )
    { 
      p_wk->plt = GFL_CLGRP_PLTT_RegisterComp( p_handle, 
          plt, CLSYS_DRAW_MAIN, PLT_OBJ_ICON_M*0x20, heapID );
    }
    else
    { 
      p_wk->plt = GFL_CLGRP_PLTT_RegisterEx( p_handle, 
          plt, CLSYS_DRAW_MAIN, PLT_OBJ_ICON_M*0x20, plt_src_ofs, 1, heapID );
    }
    p_wk->cel = GFL_CLGRP_CELLANIM_Register( p_handle,
        cel, anm, heapID );
    p_wk->ncg = GFL_CLGRP_CGR_Register( p_handle,
        chr, FALSE, CLSYS_DRAW_MAIN, heapID );
    GFL_ARC_CloseDataHandle( p_handle );
  }

  //CLWK�쐬
  { 
    GFL_CLWK_DATA cldata;
    GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
    cldata.pos_x  = ICON_POS_X;
    cldata.pos_y  = ICON_POS_Y;
    p_wk->p_clwk  =   GFL_CLACT_WK_Create( p_unit,
        p_wk->ncg,
        p_wk->plt,
        p_wk->cel,
        &cldata,
        CLSYS_DEFREND_MAIN,
        heapID );
    GFL_CLACT_WK_SetAutoAnmFlag( p_wk->p_clwk, TRUE );
  }

  //�^�C�v�ʐݒ�
  { 
    switch( type )
    { 
    case ICON_TYPE_HERO:
      if( param1 == PTL_SEX_MALE )
      { 
        CLWK_TransNSBTX( p_wk->p_clwk, ARCID_MMDL_RES, NARC_fldmmdl_mdlres_hero_nsbtx, 3, NSBTX_DEF_SX, NSBTX_DEF_SY, 0, CLSYS_DEFREND_MAIN, heapID );
      }
      else 
      { 
        CLWK_TransNSBTX( p_wk->p_clwk, ARCID_MMDL_RES, NARC_fldmmdl_mdlres_heroine_nsbtx, 3, NSBTX_DEF_SX, NSBTX_DEF_SY, 0, CLSYS_DEFREND_MAIN, heapID );
      }
      break;
    case ICON_TYPE_RIVAL:
      //���C�o���̃��\�[�X�@�i���C�o�������͂͂Ȃ��Ȃ�܂����{���C�o���̃��\�[�X�܂��Ă�ł܂��񂪔O�̂��ߎc���Ă����܂��j
      CLWK_TransNSBTX( p_wk->p_clwk, ARCID_MMDL_RES, NARC_fldmmdl_mdlres_hero_nsbtx, 3, NSBTX_DEF_SX, NSBTX_DEF_SY, 0, CLSYS_DEFREND_MAIN, heapID );
    case ICON_TYPE_PERSON:
      GFL_CLACT_WK_SetPlttOffs( p_wk->p_clwk, 0, CLWK_PLTTOFFS_MODE_PLTT_TOP );
      break;
    case ICON_TYPE_POKE:
      GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk, POKEICON_ANM_HPMAX );
      GFL_CLACT_WK_SetPlttOffs( p_wk->p_clwk, 
          POKEICON_GetPalNum( param1, param2 & 0x00ff, ( param2 & 0xff00 ) >> 8, FALSE ),
          CLWK_PLTTOFFS_MODE_OAM_COLOR );
      break;
    case ICON_TYPE_HATENA:
      GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk, 5 );
      GFL_CLACT_WK_SetPlttOffs( p_wk->p_clwk, 0, CLWK_PLTTOFFS_MODE_PLTT_TOP );
      break;
    case ICON_TYPE_BOX:
      GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk, 2 );
      GFL_CLACT_WK_SetPlttOffs( p_wk->p_clwk, 0, CLWK_PLTTOFFS_MODE_PLTT_TOP );
      break;
    default:
      GF_ASSERT(0);
    }
  }

  if( is_sex_visible )
  { 
    PRINTSYS_LSB  color;
    BMPOAM_ACT_DATA	actdata;
    ARCHANDLE * p_handle;

    p_handle  = GFL_ARC_OpenDataHandle( ARCID_FONT, heapID );
    p_wk->p_bmp = GFL_BMP_Create( 2, 2, GFL_BMP_16_COLOR, heapID );

    p_wk->font_plt  = GFL_CLGRP_PLTT_RegisterComp( p_handle, 
          NARC_font_default_nclr, CLSYS_DRAW_MAIN, PLT_OBJ_SEX_M*0x20, heapID );
    GFL_ARC_CloseDataHandle( p_handle );	

		GFL_STD_MemClear( &actdata, sizeof(BMPOAM_ACT_DATA) );
		actdata.bmp	= p_wk->p_bmp;
		actdata.x		= ICON_POS_SEX_X;
		actdata.y		= ICON_POS_Y;
		actdata.pltt_index	= p_wk->font_plt;
		actdata.soft_pri		= 0;
		actdata.setSerface	= CLSYS_DRAW_MAIN;
		actdata.draw_type		= CLSYS_DRAW_MAIN;
		actdata.bg_pri			= 0;
    actdata.pal_offset  = 0;

		p_wk->p_bmpoam_wk	  = BmpOam_ActorAdd( p_bmpoam_sys, &actdata );

    { 
      u16 msgID;
      if( sex == PTL_SEX_FEMALE )
      { 
        msgID = NAMEIN_ICON_001;
        color = PRINTSYS_LSB_Make( 3,4,0 );
      }
      else
      { 
        msgID = NAMEIN_ICON_000;
        color = PRINTSYS_LSB_Make( 5,6,0 );
      }
      p_wk->p_strbuf  = GFL_MSG_CreateString( p_msg, msgID );
    }

    GFL_BMP_Clear( p_wk->p_bmp, 0 );	
    PRINTSYS_PrintQueColor( p_que, p_wk->p_bmp, 0, 0, p_wk->p_strbuf, p_font, 
        color );
    p_wk->p_que = p_que;
    p_wk->is_trans_req  = TRUE;
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  �A�C�R���j��
 *
 *  @param  ICON_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void ICON_Exit( ICON_WORK *p_wk )
{ 
  if( p_wk->p_bmpoam_wk )
  { 
    GFL_CLGRP_PLTT_Release( p_wk->font_plt );
    GFL_STR_DeleteBuffer( p_wk->p_strbuf );
    BmpOam_ActorDel( p_wk->p_bmpoam_wk );
    GFL_BMP_Delete( p_wk->p_bmp );
  }

  //���\�[�X�j��
  { 
    GFL_CLGRP_PLTT_Release( p_wk->plt );
    GFL_CLGRP_CGR_Release( p_wk->ncg );
    GFL_CLGRP_CELLANIM_Release( p_wk->cel );
  }
  //CLWK����
  GFL_CLACT_WK_Remove( p_wk->p_clwk );
  //�N���A
  GFL_STD_MemClear( p_wk, sizeof(ICON_WORK) );
}
//----------------------------------------------------------------------------
/**
 *  @brief  �A�C�R��  ���C������
 *
 *  @param  ICON_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void ICON_Main( ICON_WORK *p_wk )
{ 
  enum
  { 
    SEQ_MAIN, //�^�b�`�\���
    SEQ_ANM,  //�A�C�R���A�j��
  };



  switch( p_wk->seq )
  { 
  case SEQ_MAIN:
    { 

      u32 x, y;
      GFL_CLACTPOS  clpos;
      GFL_CLACT_WK_GetPos( p_wk->p_clwk, &clpos, CLSYS_DRAW_MAIN );

      p_wk->is_trg   = FALSE;

      if( GFL_UI_TP_GetPointTrg( &x, &y) )
      { 
        GFL_POINT pos;
        GFL_RECT  rect;

        rect.left   = clpos.x - ICON_WIDTH/2;
        rect.top    = clpos.y - ICON_HEIGHT/2;
        rect.right  = clpos.x + ICON_WIDTH/2;
        rect.bottom = clpos.y + ICON_HEIGHT/2;

        pos.x = x;
        pos.y = y;

        if( COLLISION_IsRectXPos( &rect, &pos ) )
        {
          Icon_StartMove( p_wk );
          p_wk->seq = SEQ_ANM;
        }
      }
    }
    break;

  case SEQ_ANM:
    if( Icon_MainMove( p_wk ) )
    { 
      p_wk->is_trg  = TRUE;
      p_wk->seq = SEQ_MAIN;
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����\�����C���i���ʂ̕����\���̂��߁j
 *
 *	@param	ICON_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void ICON_PrintMain( ICON_WORK *p_wk )
{ 
  if( p_wk->is_trans_req )
  { 
    if( !PRINTSYS_QUE_IsExistTarget( p_wk->p_que, p_wk->p_bmp ) )
    { 
      BmpOam_ActorBmpTrans( p_wk->p_bmpoam_wk );
      p_wk->is_trans_req  = FALSE;
    } 
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  �A�C�R�����͏��
 *
 *  @param  const ICON_WORK *cp_wk  ���[�N
 *
 *  @return TRUE����  FALSE���͂��Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL ICON_IsTrg( const ICON_WORK *cp_wk )
{ 
  return cp_wk->is_trg;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �A�C�R���ړ��J�n
 *
 *  @param  ICON_WORK *p_wk ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void Icon_StartMove( ICON_WORK *p_wk )
{ 
  p_wk->sync  = 0;
  p_wk->is_move_start = TRUE;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �A�C�R���ړ�����
 *
 *  @param  ICON_WORK *p_wk ���[�N
 *
 *  @return TRUE�A�C�R���ړ��I��  FALSE�A�C�R���ړ���
 */
//-----------------------------------------------------------------------------
static BOOL Icon_MainMove( ICON_WORK *p_wk )
{ 
  if( p_wk->is_move_start )
  { 
    u16   now;
    fx16  sin;
    GFL_CLACTPOS  clpos;

    now = ICON_MOVE_START + ICON_MOVE_DIF * p_wk->sync++ / ICON_MOVE_SYNC;
    sin = FX_SinIdx( now );

    clpos.x = ICON_POS_X;
    clpos.y = ICON_POS_Y - ((ICON_MOVE_Y*sin) >> FX16_SHIFT);

    GFL_CLACT_WK_SetPos( p_wk->p_clwk, &clpos, CLSYS_DRAW_MAIN );
  
    NAGI_Printf( "sin 0x%x y %d \n", sin, ((ICON_MOVE_Y*sin) >> FX16_SHIFT) );

    if( p_wk->sync > ICON_MOVE_SYNC )
    { 
      p_wk->is_move_start = FALSE;
    }

    return FALSE;
  }

  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���[�h����^�C�v�ւ́@�ϊ�
 *
 *	@param	NAMEIN_MODE mode  ���[�h
 *	@param1 param1            ����
 *
 *	@return �^�C�v
 */
//-----------------------------------------------------------------------------
static ICON_TYPE ICON_GetModoToType( NAMEIN_MODE mode, int param1 )
{ 
  switch( mode )
  { 
  case NAMEIN_FREE_WORD:
    /* fallthrough */
  case NAMEIN_GREETING_WORD:
    /* fallthrough */
  case NAMEIN_THANKS_WORD:
    /* fallthrough */
	case NAMEIN_MYNAME:
    return ICON_TYPE_HERO;

	case NAMEIN_POKEMON:
    return ICON_TYPE_POKE;
    
	case NAMEIN_BOX:
    return ICON_TYPE_BOX;

	case NAMEIN_RIVALNAME:
    return ICON_TYPE_RIVAL;

	case NAMEIN_FRIENDNAME:
    if( param1 == NAMEIN_TRAINER_VIEW_UNKNOWN )
    { 
      return ICON_TYPE_HATENA;
    }
    else
    { 
      return ICON_TYPE_PERSON;
    }

  default:
    GF_ASSERT( 0 );
    return 0;
  }

}
//=============================================================================
/**
 *    ���̑�
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  ��`�Ɠ_�̏Փˌ��m
 *
 *  @param  const GFL_RECT *cp_rect   ��`
 *  @param  GFL_POINT *cp_pos         �_
 *
 *  @return TRUE�Ńq�b�g  FALSE�Ńq�b�g���Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL COLLISION_IsRectXPos( const GFL_RECT *cp_rect, const GFL_POINT *cp_pos )
{ 
  return ( ((u32)( cp_pos->x - cp_rect->left) <= (u32)(cp_rect->right - cp_rect->left))
            & ((u32)( cp_pos->y - cp_rect->top) <= (u32)(cp_rect->bottom - cp_rect->top)));
}

//----------------------------------------------------------------------------
/**
 *  @brief  �f�t�H���g��  ���쐬���Ԃ�
 *
 *  @param  p_wk              ���[�N
 *  @param  NAMEIN_MODE mode  ���[�h
 *  @param  HEAPID            �q�[�v
 *  @retval STRBUF
 */
//-----------------------------------------------------------------------------
static STRBUF* INITNAME_CreateStr( const NAMEIN_WORK *cp_wk, NAMEIN_MODE mode, HEAPID heapID )
{ 
  STRBUF *p_msg_str;
  switch( mode )
  { 
  case NAMEIN_MYNAME:
    return NULL;//GFL_MSG_CreateString( cp_wk->p_msg, NAMEIN_DEF_NAME_000 + GFUser_GetPublicRand( NAMEIN_DEFAULT_NAME_MAX ) );

  case NAMEIN_POKEMON:
    if( GFL_STR_GetBufferLength(cp_wk->p_initdraw_str) == 0 )
    { 
      return GFL_MSG_CreateString( GlobalMsg_PokeName, cp_wk->p_param->mons_no );
    }
    else
    { 
      return GFL_STR_CreateCopyBuffer( cp_wk->p_initdraw_str, heapID );
    }

  case NAMEIN_BOX:
    return GFL_STR_CreateCopyBuffer( cp_wk->p_initdraw_str, heapID );

  case NAMEIN_RIVALNAME:
    //���C�o���� //���C�o�������͂͂Ȃ��Ȃ�܂���
    return NULL;//GFL_MSG_CreateString( cp_wk->p_msg, NAMEIN_DEF_NAME_000 + GFUser_GetPublicRand( NAMEIN_DEFAULT_NAME_MAX ) );

  case NAMEIN_FRIENDNAME:
    return GFL_STR_CreateCopyBuffer( cp_wk->p_initdraw_str, heapID );

  case NAMEIN_GREETING_WORD:
    return GFL_STR_CreateCopyBuffer( cp_wk->p_initdraw_str, heapID );
    
  case NAMEIN_THANKS_WORD:
    return GFL_STR_CreateCopyBuffer( cp_wk->p_initdraw_str, heapID );

  case NAMEIN_FREE_WORD:
    return GFL_STR_CreateCopyBuffer( cp_wk->p_initdraw_str, heapID );
  }

  return NULL;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���͂��I�������Ƃ��� ��������m�肷�� or �L�����Z�����m�肷�� ����
 *
 *  @param  p_wk              ���[�N
 */
//-----------------------------------------------------------------------------
static void FinishInput( NAMEIN_WORK *p_wk )
{
  //��[�L�����Z���t���O��OFF
  p_wk->p_param->cancel = FALSE;


  //���̕�����Ɠ��͕�����̈�v�m�F����
  //��v���Ă���΃L�����Z���Ƃ݂Ȃ�
  { 
    STRBUF *p_src_str;
    p_src_str = GFL_STR_CreateCopyBuffer( p_wk->p_param->strbuf, HEAPID_NAME_INPUT );

    //����̕������Ԃ�
    STRINPUT_CopyStr( &p_wk->strinput, p_wk->p_param->strbuf );

    //�ȑO�̕�����ƈ�v�����������͂Ȃ�΁A�L�����Z���Ƃ݂Ȃ�
    { 
      if( GFL_STR_CompareBuffer( p_src_str, p_wk->p_param->strbuf ) )
      { 
        p_wk->p_param->cancel = TRUE;
      }
    }

    //�s�������Ȃ�΁A�����L�����Z��
    { 
      if( !STRINPUT_IsValidStr( &p_wk->strinput ) )
      { 
        GFL_STR_CopyBuffer( p_wk->p_param->strbuf, p_src_str );
        p_wk->p_param->cancel = TRUE;
      }
    }


    GFL_STR_DeleteBuffer( p_src_str );
  }

  //���͐���0�Ȃ��
  //��l�����͂Ȃ�΃f�t�H���g��������A
  //���̂ق��Ȃ�L�����Z���Ƃ݂Ȃ�
  if( GFL_STR_GetBufferLength( p_wk->p_param->strbuf ) == 0 )
  { 
    if( p_wk->p_param->mode == NAMEIN_MYNAME ||
        p_wk->p_param->mode == NAMEIN_RIVALNAME )
    {
      GF_ASSERT( 0 ); //�f�t�H���g�������ꂸ�A����ł��Ȃ����ƂɂȂ���
    }
    else
    { 
      p_wk->p_param->cancel = TRUE;
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ����ł��邩�ǂ���
 *
 *	@param	NAMEIN_WORK *p_wk   ���[�N
 *
 *	@return TRUE����\  FALSE����s�\
 */
//-----------------------------------------------------------------------------
static BOOL CanDecide( NAMEIN_WORK *p_wk )
{ 
  if( p_wk->p_param->mode == NAMEIN_MYNAME ||
      p_wk->p_param->mode == NAMEIN_RIVALNAME )
  { 
    //��l�������͂̂Ƃ��́A��������ĂȂ��Ƃ��͌���ł��Ȃ�
    if( STRINPUT_GetStrLength( &p_wk->strinput ) == 0 && STRINPUT_GetChangeStrLength( & p_wk->strinput) == 0 )
    {
      return FALSE;
    }

    //��l���ȓ��͂̂Ƃ��́A�X�y�[�X�݂̂̂Ƃ��͓��͂ł��Ȃ�
    if( !STRINPUT_IsValidStr( &p_wk->strinput ) )
    { 
      return FALSE;
    }
  }

  return TRUE;
}
//=============================================================================
/**
 *            SEQ
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  SEQ ������
 *
 *  @param  SEQ_WORK *p_wk  ���[�N
 *  @param  *p_param        �p�����[�^
 *  @param  seq_function    �V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_Init( SEQ_WORK *p_wk, void *p_param, SEQ_FUNCTION seq_function )
{ 
  //�N���A
  GFL_STD_MemClear( p_wk, sizeof(SEQ_WORK) );

  //������
  p_wk->p_param = p_param;

  //�Z�b�g
  SEQ_SetNext( p_wk, seq_function );
}
//----------------------------------------------------------------------------
/**
 *  @brief  SEQ �j��
 *
 *  @param  SEQ_WORK *p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void SEQ_Exit( SEQ_WORK *p_wk )
{ 
  //�N���A
  GFL_STD_MemClear( p_wk, sizeof(SEQ_WORK) );
}
//----------------------------------------------------------------------------
/**
 *  @brief  SEQ ���C������
 *
 *  @param  SEQ_WORK *p_wk ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_Main( SEQ_WORK *p_wk )
{ 
  if( !p_wk->is_end )
  { 
    p_wk->seq_function( p_wk, &p_wk->seq, p_wk->p_param );
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  SEQ �I���擾
 *
 *  @param  const SEQ_WORK *cp_wk   ���[�N
 *
 *  @return TRUE�Ȃ�ΏI��  FALSE�Ȃ�Ώ�����
 */ 
//-----------------------------------------------------------------------------
static BOOL SEQ_IsEnd( const SEQ_WORK *cp_wk )
{ 
  return cp_wk->is_end;
}
//----------------------------------------------------------------------------
/**
 *  @brief  SEQ ���̃V�[�P���X��ݒ�
 *
 *  @param  SEQ_WORK *p_wk  ���[�N
 *  @param  seq_function    �V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_SetNext( SEQ_WORK *p_wk, SEQ_FUNCTION seq_function )
{ 
  p_wk->seq_function  = seq_function;
  p_wk->seq = 0;
}
//----------------------------------------------------------------------------
/**
 *  @brief  SEQ �I��
 *
 *  @param  SEQ_WORK *p_wk  ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_End( SEQ_WORK *p_wk )
{ 
  p_wk->is_end  = TRUE;
}
//=============================================================================
/**
 *          SEQFUNC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  �`�抮���҂�
 *
 *  @param  SEQ_WORK *p_seqwk �V�[�P���X���[�N
 *  @param  *p_seq          �V�[�P���X
 *  @param  *p_param        ���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_WaitPrint( SEQ_WORK *p_seqwk, int *p_seq, void *p_param )
{
  NAMEIN_WORK *p_wk = p_param;

  BOOL ret;

  ret = KEYBOARD_PrintMain( &p_wk->keyboard );
  ret |= STRINPUT_PrintMain( &p_wk->strinput );
  ret |= MSGWND_PrintMain( &p_wk->msgwnd );

  if( ret )
  { 
    SEQ_SetNext( p_seqwk, SEQFUNC_FadeOut );
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  �t�F�[�hOUT
 *
 *  @param  SEQ_WORK *p_seqwk �V�[�P���X���[�N
 *  @param  *p_seq          �V�[�P���X
 *  @param  *p_param        ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_FadeOut( SEQ_WORK *p_seqwk, int *p_seq, void *p_param )
{ 
  enum
  { 
    SEQ_FADEOUT_START,
    SEQ_FADEOUT_WAIT,
    SEQ_END,
  };

  switch( *p_seq )
  { 
  case SEQ_FADEOUT_START:
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
    *p_seq  = SEQ_FADEOUT_WAIT;
    break;

  case SEQ_FADEOUT_WAIT:
    if( !GFL_FADE_CheckFade() )
    { 
      *p_seq  = SEQ_END;
    }
    break;

  case SEQ_END:
    SEQ_SetNext( p_seqwk, SEQFUNC_Main );
    break;

  default:
    GF_ASSERT(0);
  }
  
}
//----------------------------------------------------------------------------
/**
 *  @brief  �t�F�[�hIN
 *
 *  @param  SEQ_WORK *p_seqwk �V�[�P���X���[�N
 *  @param  *p_seq          �V�[�P���X
 *  @param  *p_param        ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_FadeIn( SEQ_WORK *p_seqwk, int *p_seq, void *p_param )
{
  enum
  {
    SEQ_FADEIN_START,
    SEQ_FADEIN_WAIT,
    SEQ_EXIT,
  };  

  switch( *p_seq )
  { 
  case SEQ_FADEIN_START:
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
    *p_seq  = SEQ_FADEIN_WAIT;
    break;

  case SEQ_FADEIN_WAIT:
    if( !GFL_FADE_CheckFade() )
    { 
      *p_seq  = SEQ_EXIT;
    }
    break;

  case SEQ_EXIT:
    SEQ_SetNext( p_seqwk, SEQFUNC_End );
    break;

  default:
    GF_ASSERT(0);
  }

}

//----------------------------------------------------------------------------
/**
 *  @brief  �v�����g�X�g���[��
 *
 *  @param  SEQ_WORK *p_seqwk �V�[�P���X���[�N
 *  @param  *p_seq          �V�[�P���X
 *  @param  *p_param        ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_PrintStream( SEQ_WORK *p_seqwk, int *p_seq, void *p_param )
{
  enum
  {
    SEQ_PS_STREAM,
    SEQ_PS_WAIT,
    SEQ_PS_EXIT,
  };

  NAMEIN_WORK *p_wk = p_param;
  PS_WORK *p_ps_wk = &(p_wk->ps);

  switch( *p_seq )
  {
  case SEQ_PS_STREAM:
    {
      PRINTSTREAM_STATE state;
      GFL_TCBL_Main( p_ps_wk->p_tcblsys );
      state = PRINTSYS_PrintStreamGetState( p_ps_wk->p_stream );
      switch( state )
      {
      case PRINTSTREAM_STATE_RUNNING:
        {
          if( GFL_UI_KEY_GetCont()&(PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL) || GFL_UI_TP_GetCont() )
          {
            PRINTSYS_PrintStreamShortWait( p_ps_wk->p_stream, 0 );
#ifdef NAMEIN_KEY_TOUCH
            if( GFL_UI_KEY_GetCont()&(PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL) )
              GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
            else
              GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
#endif
          }
        }
        break;
      case PRINTSTREAM_STATE_DONE:
        {
          *p_seq = SEQ_PS_WAIT;
        }
        break;
      case PRINTSTREAM_STATE_PAUSE:
        {
          if( GFL_UI_KEY_GetTrg()&(PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL) || GFL_UI_TP_GetTrg() )
          {
            PRINTSYS_PrintStreamReleasePause( p_ps_wk->p_stream );
#ifdef NAMEIN_KEY_TOUCH
            if( GFL_UI_KEY_GetTrg()&(PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL) )
              GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
            else
              GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
#endif
          }
        }
        break;
      }
    }
    break;
  case SEQ_PS_WAIT:
    {
      if( p_ps_wk->wait_count >= p_ps_wk->wait_max )
      {
        *p_seq = SEQ_PS_EXIT;
      }
      else
      {
        p_ps_wk->wait_count++;
      }
    }
    break;
  case SEQ_PS_EXIT:
    {
      SEQ_SetNext( p_seqwk, p_ps_wk->next_seq_function );
    }
    break;
  default:
    {
      GF_ASSERT(0);
    }
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �ݒ��ʃ��C������
 *
 *  @param  SEQ_WORK *p_seqwk �V�[�P���X���[�N
 *  @param  *p_seq          �V�[�P���X
 *  @param  *p_param        ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Main( SEQ_WORK *p_seqwk, int *p_seq, void *p_param )
{ 
  NAMEIN_WORK *p_wk = p_param;

  //���͏���
  {
    KEYBOARD_INPUT  input;
    BOOL            is_shift;
    STRCODE         code;
    input     = KEYBOARD_GetInput( &p_wk->keyboard, &code );
    is_shift  = KEYBOARD_IsShift( &p_wk->keyboard );

    //�s���������b�Z�[�W����������
    if( p_wk->is_illegal_msg == TRUE )
    { 
      if( input != KEYBOARD_INPUT_NONE || ICON_IsTrg( &p_wk->icon ) || (GFL_UI_KEY_GetTrg() & ~PAD_BUTTON_DECIDE) )
      { 
        //�����`��
        if( p_wk->p_param->mode == NAMEIN_POKEMON )
        { 
          //�|�P�����̏ꍇ�́A�푰����P��o�^
          if( p_wk->p_param->pp == NULL )
          { 
            MSGWND_ExpandPrintPoke( &p_wk->msgwnd, NAMEIN_MSG_INFO_000 + p_wk->p_param->mode, p_wk->p_param->param1, p_wk->p_param->param2, HEAPID_NAME_INPUT );
          }
          else
          { 
            MSGWND_ExpandPrintPP( &p_wk->msgwnd, NAMEIN_MSG_INFO_000 + p_wk->p_param->mode, p_wk->p_param->pp );
          }
        }
        else
        { 
          //���͒ʏ�
          MSGWND_Print( &p_wk->msgwnd, NAMEIN_MSG_INFO_000 + p_wk->p_param->mode );
        }
        p_wk->is_illegal_msg = FALSE;
      }
    }

    //���ꂼ��̓��͂ɑ΂��鏈��
    switch( input )
    { 
    case KEYBOARD_INPUT_STR:        //��������
      if( STRINPUT_SetStr( &p_wk->strinput, code ) )
      { 
        PMSND_PlaySE( NAMEIN_SE_DECIDE_STR );
      }
      else
      { 
        PMSND_PlaySE( NAMEIN_SE_BEEP );
      }
      break;
    case KEYBOARD_INPUT_CHANGESTR:  //�ϊ���������
      if( STRINPUT_SetChangeStr( &p_wk->strinput, code, is_shift ) )
      { 
        PMSND_PlaySE( NAMEIN_SE_DECIDE_STR );
      }
      else
      { 
        PMSND_PlaySE( NAMEIN_SE_BEEP );
      }
      break;
    case KEYBOARD_INPUT_DAKUTEN:    //���_
      {
        BOOL is_change  = TRUE;
        if( !STRINPUT_SetChangeSP( &p_wk->strinput, STRINPUT_SP_CHANGE_DAKUTEN ) )
        { 
          is_change = FALSE;
        }

        if( is_change )
        { 
          PMSND_PlaySE( NAMEIN_SE_DECIDE_STR );
        }
        else
        { 
          PMSND_PlaySE( NAMEIN_SE_BEEP );
        }
      }
      break;
    case KEYBOARD_INPUT_HANDAKUTEN: //�����_
      {
        BOOL is_change  = TRUE;
        if( !STRINPUT_SetChangeSP( &p_wk->strinput, STRINPUT_SP_CHANGE_HANDAKUTEN ) )
        { 
          is_change = FALSE;
        }

        if( is_change )
        { 
          PMSND_PlaySE( NAMEIN_SE_DECIDE_STR );
        }
        else
        { 
          PMSND_PlaySE( NAMEIN_SE_BEEP );
        }
      }
      break;
    case KEYBOARD_INPUT_AUTOCHANGE: //����
      if( STRINPUT_SetChangeAuto( &p_wk->strinput ) )
      { 
        PMSND_PlaySE( NAMEIN_SE_DECIDE_STR );
      }
      else
      { 
        PMSND_PlaySE( NAMEIN_SE_BEEP );
      }
      break;
    case KEYBOARD_INPUT_BACKSPACE:  //��O�ɖ߂�
      if( STRINPUT_BackSpace( &p_wk->strinput ) )
      { 
        PMSND_PlaySE( NAMEIN_SE_DELETE_STR );
      }
      else
      { 
        PMSND_PlaySE( NAMEIN_SE_BEEP );
      }
      break;
    case KEYBOARD_INPUT_CHAGETYPE:  //���̓^�C�v�ύX
      STRINPUT_DeleteChangeStr( &p_wk->strinput );
      break;
    case KEYBOARD_INPUT_LAST: //�Ō�܂œ��͂���
      STRINPUT_DecideChangeStr( &p_wk->strinput );
      break;
    case KEYBOARD_INPUT_EXIT:       //�I��  
      //�ϊ������񂪂Ȃ�������I��
      if( STRINPUT_GetChangeStrLength( & p_wk->strinput) == 0 )
      { 
        //��l�������͂̂Ƃ��A����ł��Ȃ������當�����o���Ė߂�
        if( !CanDecide( p_wk) )
        { 
          PMSND_PlaySE( NAMEIN_SE_BEEP );
          MSGWND_Print( &p_wk->msgwnd, NAMEIN_MSG_INFO_008 );
          p_wk->is_illegal_msg  = TRUE;
          break;
        }

        //�s�������`�F�b�N
        if( PROF_WORD_CheckProfanityWordCode( p_wk->p_prof, 
              p_wk->strinput.input_str, 
              HEAPID_NAME_INPUT ) )
        { 
          PMSND_PlaySE( NAMEIN_SE_BEEP );
          break;
        }

        //�s�����l�`�F�b�N
        if( PROF_WORD_CheckProfanityNumberCode(
              p_wk->strinput.input_str,
              p_wk->strinput.input_idx,
              HEAPID_NAME_INPUT ) )
        { 
          PMSND_PlaySE( NAMEIN_SE_BEEP );
          MSGWND_Print( &p_wk->msgwnd, NAMEIN_MSG_INFO_009 );
          p_wk->is_illegal_msg  = TRUE;
          break;
        }


        PMSND_PlaySE( NAMEIN_SE_DECIDE );

        // ���͂��I�������Ƃ��� ��������m�肷�� or �L�����Z�����m�肷�� ����
        FinishInput( p_wk );

        if( p_wk->p_param->mode==NAMEIN_POKEMON && p_wk->p_param->box_strbuf!=NULL )  // �|�P�����ߊl���Ń{�b�N�X�]��
        {
          PS_SetupBox( &p_wk->ps, &p_wk->msgwnd, p_wk->p_param, HEAPID_NAME_INPUT );
          SEQ_SetNext( p_seqwk, SEQFUNC_PrintStream );
        }
        else
        {
          SEQ_SetNext( p_seqwk, SEQFUNC_FadeIn );
        }
      }
      else
      {
        //�ϊ������񂪂�������m��
        STRINPUT_DecideChangeStr( &p_wk->strinput );
        PMSND_PlaySE( NAMEIN_SE_DECIDE );
      }
      break;
    case KEYBOARD_INPUT_NONE:       //�������Ă��Ȃ�
  
      //�A�C�R���^�b�`������A�f�t�H���g��������
      if( ICON_IsTrg( &p_wk->icon ) )
      { 
        STRBUF  *p_default;
        p_default = INITNAME_CreateStr( p_wk, p_wk->p_param->mode, HEAPID_NAME_INPUT );

        //��[�S�Ă���
        STRINPUT_Delete( &p_wk->strinput );

        //�f�t�H���g������
        if( p_default != NULL )
        { 
          STRINPUT_SetLongStr( &p_wk->strinput, GFL_STR_GetStringCodePointer(p_default) );
          GFL_STR_DeleteBuffer( p_default );
        }

        PMSND_PlaySE( NAMEIN_SE_DECIDE_STR );
      }
      break;
    case KEYBOARD_INPUT_SHIFT:      //�V�t�g
      /*  �������Ȃ� */
      break;
    case KEYBOARD_INPUT_SPACE:      //�X�y�[�X����
      STRINPUT_DecideChangeStr( &p_wk->strinput );
      if( STRINPUT_SetStr( &p_wk->strinput, code ) )
      { 
        PMSND_PlaySE( NAMEIN_SE_DECIDE_STR );
      }
      else
      { 
        PMSND_PlaySE( NAMEIN_SE_BEEP );
      }
      break;
    default:
      GF_ASSERT(0);
    }
  }

  //���C������
  KEYBOARD_Main( &p_wk->keyboard, &p_wk->strinput );
  STRINPUT_Main( &p_wk->strinput );
  ICON_Main( &p_wk->icon );
  MSGWND_PrintMain( &p_wk->msgwnd );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �ݒ��ʏI������
 *
 *  @param  SEQ_WORK *p_seqwk �V�[�P���X���[�N
 *  @param  *p_seq          �V�[�P���X
 *  @param  *p_param        ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_End( SEQ_WORK *p_seqwk, int *p_seq, void *p_param )
{ 
  NAMEIN_WORK *p_wk = p_param;

  //SEQFUNC_Main�ֈړ�FinishInput( p_wk );

  //���̓��[�h��ۑ�����
  if( p_wk->p_param->p_misc )
  { 
    NAMEIN_INPUTTYPE input_type;
    input_type  = KEYBOARD_GetInputType( &p_wk->keyboard );
    MISC_SetNameInMode( p_wk->p_param->p_misc, p_wk->p_param->mode, input_type );
  }

  //�I��
  if( p_wk->p_param->p_intr_sv )
  { 
    SEQ_SetNext( p_seqwk, SEQFUNC_SaveEnd );
  }
  else
  { 
    SEQ_End( p_seqwk );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �Z�[�u�I���҂�
 *
 *  @param  SEQ_WORK *p_seqwk �V�[�P���X���[�N
 *  @param  *p_seq          �V�[�P���X
 *  @param  *p_param        ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_SaveEnd( SEQ_WORK *p_seqwk, int *p_seq, void *p_param )
{ 
  enum
  { 
    SEQ_SAVE_SUSPEND,
    SEQ_SAVE_WAIT,
  };

  NAMEIN_WORK *p_wk = p_param;

  switch( *p_seq )
  { 
  case SEQ_SAVE_SUSPEND:
    IntrSave_ReqSuspend( p_wk->p_param->p_intr_sv );
    *p_seq  = SEQ_SAVE_WAIT;
    break;

  case SEQ_SAVE_WAIT:
    if(IntrSave_CheckSuspend(p_wk->p_param->p_intr_sv) == TRUE){
      SEQ_End( p_seqwk );
    }
    break;
  } 
}
