//=============================================================================
/**
 *
 *	@file		intro_cmd.c
 *	@brief  3D�f���R�}���h
 *	@author	hosaka genya
 *	@data		2009.12.09
 *
 */
//=============================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "system/brightness.h"

#include "msg/msg_intro.h"  // for GMM Index

// ������֘A
#include "font/font.naix"
#include "message.naix"
#include "print/gf_font.h"
#include "print/wordset.h"
#include "print/printsys.h" // for PRINT_QUE

//�A�[�J�C�u
#include "arc_def.h" // for ARCID_XXX

#include "intro.naix"

#include "intro_sys.h"

#include "intro_graphic.h"

//�f�[�^
#include "intro_cmd_data.h"

#include "intro_msg.h" // for INTRO_MSG_WORK
#include "intro_mcss.h" // for INTRO_MCSS_WORK

#include "intro_cmd.h" // for extern�錾

#include "sound/pm_sndsys.h" // for SEQ_SE_XXX

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================
enum
{ 
  STORE_NUM = 8,  ///< �������s�R�}���h�̌��E��
};


//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================

//--------------------------------------------------------------
///	
//==============================================================
typedef struct {
  int   seq;      // �R�}���h���V�[�P���X
  void* wk_user;  // ���[�U�[���[�N
} INTRO_STORE_DATA;

//--------------------------------------------------------------
///	�R�}���h���[�N
//==============================================================
struct _INTRO_CMD_WORK {
  // [IN]
  HEAPID heap_id;
  INTRO_PARAM* init_param;
  INTRO_MCSS_WORK* mcss;
  // [PRIVATE]
  INTRO_SCENE_ID scene_id;
  const INTRO_CMD_DATA* store[ STORE_NUM ];
  // @TODO ���Ԃ��������烊�t�@�N�^
  // INTRO_STORE_DATA��INTRO_STORE_DATA�ɓ����`���̕����X�b�L������B
  INTRO_STORE_DATA store_data[ STORE_NUM ]; // �e�R�}���h�p���[�N
  int cmd_idx;
  INTRO_MSG_WORK* wk_msg;
};

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================
static BOOL cmd_store( INTRO_CMD_WORK* wk, const INTRO_CMD_DATA* data );
static BOOL cmd_store_exec( INTRO_CMD_WORK* wk );

//=============================================================================
// �R�}���h
//=============================================================================

// ����R�}���h
static BOOL CMD_SET_SCENE( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param );
static BOOL CMD_START_SCENE( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param );
static BOOL CMD_YESNO( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param );
static BOOL CMD_COMP( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param );

// ��ʃR�}���h
static BOOL CMD_LOAD_BG( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param );
static BOOL CMD_SET_RETCODE( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param );
static BOOL CMD_FADE_REQ( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param );
static BOOL CMD_BRIGHTNESS_SET( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param );
static BOOL CMD_BRIGHTNESS_REQ( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param );
static BOOL CMD_BRIGHTNESS_WAIT( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param );
static BOOL CMD_BGM( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param );
static BOOL CMD_SE( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param );
static BOOL CMD_SE_STOP( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param);
static BOOL CMD_KEY_WAIT( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param );
static BOOL CMD_LOAD_GMM( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param );
static BOOL CMD_WORDSET( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param );
static BOOL CMD_PRINT_MSG( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param );
static BOOL CMD_MCSS_LOAD( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param );
static BOOL CMD_MCSS_SET_VISIBLE( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param );
static BOOL CMD_MCSS_SET_ANIME( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param );

// �C���g���p�R�}���h
static BOOL CMD_SELECT_MOJI( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param );
static BOOL CMD_SELECT_SEX( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param );
static BOOL CMD_POKEMON_APPER( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param );

// INTRO_CMD_TYPE �ƑΉ�
//--------------------------------------------------------------
///	�R�}���h�e�[�u�� (�֐��|�C���^�e�[�u��)
//==============================================================
static BOOL (*c_cmdtbl[ INTRO_CMD_TYPE_MAX ])() = 
{ 
  NULL, // null
  
  //-----------------------------------------
  // �� ����R�}���h ��
  //-----------------------------------------
  CMD_SET_SCENE,
  CMD_START_SCENE,
  CMD_YESNO,
  CMD_COMP,

  //-----------------------------------------
  // �� ��ʃR�}���h ��
  //-----------------------------------------
  CMD_LOAD_BG,
  CMD_SET_RETCODE,
  CMD_FADE_REQ,
  CMD_BRIGHTNESS_SET,
  CMD_BRIGHTNESS_REQ,
  CMD_BRIGHTNESS_WAIT,
  CMD_BGM,
  CMD_SE,
  CMD_SE_STOP,
  CMD_KEY_WAIT,
  CMD_LOAD_GMM,
  CMD_WORDSET,
  CMD_PRINT_MSG,
  CMD_MCSS_LOAD,
  CMD_MCSS_SET_VISIBLE,
  CMD_MCSS_SET_ANIME,

  //-----------------------------------------
  // �� �C���g���f���p�R�}���h ��
  //-----------------------------------------
  CMD_SELECT_MOJI,
  CMD_SELECT_SEX,
  CMD_POKEMON_APPER,
  NULL, // end
};

//=============================================================================
// ����֐�
//=============================================================================
static BOOL CMD_COMP_SEX( INTRO_CMD_WORK* wk );

// INTRO_CMD_COMP �ƑΉ�
//--------------------------------------------------------------
///	����֐��e�[�u�� (�֐��|�C���^�e�[�u��)
//==============================================================
static BOOL (*c_cmdtbl_comp[ INTRO_CMD_COMP_MAX ])() = 
{ 
  CMD_COMP_SEX,
};

//=============================================================================
// WORDSET�֐�
//=============================================================================
static void CMD_WORDSET_TRAINER( INTRO_CMD_WORK* wk, int bufID );

// INTRO_WORDSET �ƑΉ�
//--------------------------------------------------------------
///	WORDSET�e�[�u�� (�֐��|�C���^�e�[�u��)
//==============================================================
static void (*c_cmdtbl_wordset[ INTRO_WORDSET_MAX ])() = 
{ 
  CMD_WORDSET_TRAINER,
};

//-----------------------------------------------------------------------------
/**
 *	@brief  ���̃V�[�����Z�b�g
 *
 *	@param	INTRO_CMD_WORK* wk
 *	@param	param 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL CMD_SET_SCENE( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param )
{
  // ���̃V�[��
  wk->scene_id = param[0];
  // �R�}���h�Z�b�g
  wk->cmd_idx = 0;

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �V�[���J�n
 *
 *	@param	param 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL CMD_START_SCENE( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param )
{
  int inner_param[1];
      
  inner_param[0] = wk->init_param->scene_id;

  // �������[�v�΍�
  if( inner_param[0] == INTRO_SCENE_ID_INIT )
  {
    GF_ASSERT(0);
    inner_param[0] += 1;
  }

  CMD_SET_SCENE( wk, NULL, inner_param );

  OS_TPrintf("\n*** start scene_id=%d ***\n",wk->scene_id);

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  YESNO�I��
 *
 *	@param	param 
 * 
 *  @note   TRUE�Ȃ����̃R�}���h���X�g�A�AFALSE�Ȃ�2���̃R�}���h���X�g�A
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL CMD_YESNO( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param )
{
  switch( sdat->seq )
  {
  case 0:
    // SETUP
    {
      INTRO_LIST_DATA data[2];

      data[0].str_id  = param[0];
      data[0].param   = 0;

      data[1].str_id  = param[1];
      data[1].param   = 1;

      INTRO_MSG_LIST_Start( wk->wk_msg, data, NELEMS(data) );

      sdat->seq++;
    }
    break;

  case 1:
    INTRO_MSG_LIST_Main( wk->wk_msg );

    {
      u32 select_id;

      if( INTRO_MSG_LIST_IsDecide( wk->wk_msg, &select_id ) == INTRO_LIST_SELECT_DECIDE )
      {
        const INTRO_CMD_DATA* data;

        data = Intro_DATA_GetCmdData( wk->scene_id );
        data += wk->cmd_idx;

        HOSAKA_Printf( "select_id=%d\n" ,select_id );

        // ���X�g�J��
        INTRO_MSG_LIST_Finish( wk->wk_msg );

        if( select_id == 0 )
        {
          if( data != NULL )
          {
            // �オ�I�����ꂽ�ꍇ�͒���̃R�}���h���X�g�A
            cmd_store( wk, data );
          }
        }
        else
        {
          data++;
          if( data != NULL )
          {
            cmd_store( wk, data );
          }
        }
         
        // ���ʃR�}���h�̎����V�[�N
        wk->cmd_idx += 2;

        return TRUE;
      }
    }
    break;

  default : GF_ASSERT(0);
  }

  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ����֐��Ăяo��
 *
 *  @param  PARAM[0] INTRO_CMD_COMP
 
 *  @note   TRUE�Ȃ����̃R�}���h���X�g�A�AFALSE�Ȃ�2���̃R�}���h���X�g�A
 *
 *	@retval TRUE=�R�}���h�I��
 */
//-----------------------------------------------------------------------------
static BOOL CMD_COMP( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param )
{
  BOOL is_comp;
  const INTRO_CMD_DATA* data;

  data = Intro_DATA_GetCmdData( wk->scene_id );
  data += wk->cmd_idx;

  is_comp = c_cmdtbl_comp[ param[0] ]( wk );

  // ����R�}���h�����s
  if( is_comp == FALSE )
  {
    data++;
  }
  
  OS_TPrintf("comp = %d \n", is_comp );
  
  if( data != NULL )
  {
    cmd_store( wk, data );
  }
  
  // ���ʃR�}���h�̎����V�[�N
  wk->cmd_idx += 2;

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �O���t�B�b�N���[�h
 *
 *	@param	INTRO_CMD_WORK* wk
 *	@param	param 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL CMD_LOAD_BG( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param )
{
  HEAPID heap_id;
	ARCHANDLE	*handle;

  heap_id = wk->heap_id;
  // @TODO �ėp��
	handle	= GFL_ARC_OpenDataHandle( ARCID_INTRO_GRA, heap_id );

	// �㉺��ʂa�f�p���b�g�]��
	GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_intro_intro_bg_NCLR, PALTYPE_MAIN_BG, PLTID_BG_BACK_M, 0x20, heap_id );
	GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_intro_intro_bg_NCLR, PALTYPE_SUB_BG, PLTID_BG_BACK_S, 0x20, heap_id );
	
  //	----- ����� -----
	GFL_ARCHDL_UTIL_TransVramBgCharacter(	handle, NARC_intro_intro_bg_NCGR,
						BG_FRAME_BACK_S, 0, 0, 0, heap_id );
	GFL_ARCHDL_UTIL_TransVramScreen(	handle, NARC_intro_intro_bg_main_NSCR,
						BG_FRAME_BACK_S, 0, 0, 0, heap_id );	

	//	----- ���� -----
	GFL_ARCHDL_UTIL_TransVramBgCharacter(	handle, NARC_intro_intro_bg_NCGR,
						BG_FRAME_BACK_M, 0, 0, 0, heap_id );
	GFL_ARCHDL_UTIL_TransVramScreen(	handle, NARC_intro_intro_bg_sub_NSCR,
						BG_FRAME_BACK_M, 0, 0, 0, heap_id );		
	
  GFL_ARC_CloseDataHandle( handle );

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �O���A�v���ɑ΂���߂�l��ݒ�
 *
 *	@param	param[0] INTRO_RETCODE
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL CMD_SET_RETCODE( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param )
{
  wk->init_param->retcode =  param[0];

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �t�F�[�h���N�G�X�g
 *
 *  PARAM[0]  GFL_FADE_MASTER_BRIGHT_XXX
 *  PARAM[1]  �X�^�[�g�P�x(0�`16)
 *  PARAM[2]  �G���h�P�x(0�`16)
 *  PARAM[3]  �t�F�[�h�X�s�[�h
 */
//-----------------------------------------------------------------------------
static BOOL CMD_FADE_REQ( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param )
{ 
  switch( sdat->seq )
  {
  case 0:
    GFL_FADE_SetMasterBrightReq( param[0], param[1], param[2], param[3] );
    sdat->seq++;
    break;
  case 1:
    if( GFL_FADE_CheckFade() == FALSE )
    {
      return TRUE;
    }
    break;
  default : GF_ASSERT(0);
  };
  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �u���C�h�l�X���Z�b�g
 *
 *	@param	param[0]  �P�x
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL CMD_BRIGHTNESS_SET( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param )
{
  SetBrightness( param[0], (PLANEMASK_BG0|PLANEMASK_BG2|PLANEMASK_BG3|PLANEMASK_OBJ), MASK_MAIN_DISPLAY );

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	param[0]
 *	@param	param[1]
 *	@param	param[2]
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL CMD_BRIGHTNESS_REQ( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param )
{
  ChangeBrightnessRequest( param[0], param[1], param[2], (PLANEMASK_BG0|PLANEMASK_BG2|PLANEMASK_BG3|PLANEMASK_OBJ), MASK_MAIN_DISPLAY );

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	param[0]
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL CMD_BRIGHTNESS_WAIT( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param )
{
  IsFinishedBrightnessChg( MASK_MAIN_DISPLAY );

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  BGM�Đ��R�}���h
 *
 *	@param	param[0] BGM_Label
 *	@param  param[1] fadeInFrame
 *	@param  param[2] fadeOutFrame
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL CMD_BGM( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param )
{
  HOSAKA_Printf( "play bgm =%d fadeInFrame=%d, fadeOutFrame=%d \n", param[0], param[1], param[2] );
//  PMSND_PlayBGM( param[0] );
  PMSND_PlayNextBGM( param[0], param[1], param[2] );

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  SE�Đ��R�}���h
 *
 *	@param	param[0] SE_Label
 *	@param	param[1] volume : 0�Ȃ疳��
 *	@param	param[2] pan : 0�Ȃ疳��
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL CMD_SE( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param )
{
  int player_no;

  GFL_SOUND_PlaySE( param[0] );
  
  player_no = GFL_SOUND_GetPlayerNo( param[0] );

  // volume
  if( param[1] != 0 )
  {
    GFL_SOUND_SetVolume( player_no, param[1] );
  }

  // pan
  if( param[2] != 0 )
  {
    GFL_SOUND_SetPan( player_no, 0xFFFF, param[2] );
  }

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  SE:�Đ���~
 *
 *	@param	param[0] SE_Label
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL CMD_SE_STOP( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param)
{
  int player_no;

  player_no = GFL_SOUND_GetPlayerNo( param[0] );

  GFL_SOUND_StopPlayerNo( player_no );

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �L�[�E�F�C�g
 *
 *	@param	INTRO_CMD_WORK* wk
 *	@param	param 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL CMD_KEY_WAIT( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param )
{
  if( GFL_UI_TP_GetTrg() || GFL_UI_KEY_GetTrg() )
  {
    return TRUE;
  }

  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  GMM�������[�h
 *
 *	@param	param[0] GflMsgLoadType
 *	@param	param[1] msg_dat_id
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL CMD_LOAD_GMM( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param )
{
  INTRO_MSG_LoadGmm( wk->wk_msg, param[0], param[1] );

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  WORDSET����
 *
 *	@param	param[0] INTRO_WORDSET ���W�X�g�����ނ�ݒ�
 *	@param	param[1] ���W�X�g����ID���w��
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL CMD_WORDSET( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param )
{
  GF_ASSERT( param[0] < INTRO_WORDSET_MAX );

  // �֐��e�[�u�������������o��
  c_cmdtbl_wordset[ param[0] ]( wk, param[1] );

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���b�Z�[�W�\��
 *
 *	@param	param[0] str_id
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL CMD_PRINT_MSG( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param )
{
  switch( sdat->seq )
  {
    case 0:
      INTRO_MSG_SetPrint( wk->wk_msg, param[0] );

      sdat->seq++;
      break;

    case 1:
      if( INTRO_MSG_PrintProc( wk->wk_msg ) )
      {
        return TRUE;
      }
      break;

  default : GF_ASSERT(0);
  }

  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �C���g����ʗpMCSS�����[�h
 *	@param  param[0] MCSS_ID
 *	@param  param[1] 0=���m, MONSNO=�|�P����
 */
//-----------------------------------------------------------------------------
static BOOL CMD_MCSS_LOAD( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param )
{
  GF_ASSERT( param[1] <= MONSNO_MAX );

  if( param[1] == 0 )
  {
    static const MCSS_ADD_WORK add = 
    {
      ARCID_INTRO_GRA,
      NARC_intro_intro_doctor_NCBR,
      NARC_intro_intro_doctor_NCLR,
      NARC_intro_intro_doctor_NCER,
      NARC_intro_intro_doctor_NANR,
      NARC_intro_intro_doctor_NMCR,
      NARC_intro_intro_doctor_NMAR,
      NARC_intro_intro_doctor_NCEC,
    };

    // ���m�\��
    INTRO_MCSS_Add( wk->mcss, param[2], param[3], 0, &add, param[0] );
  }
  else
  {
    // �|�P�����\��
    INTRO_MCSS_AddPoke( wk->mcss, param[2], param[3], 0, param[1], param[0] );
  }

  return TRUE;
}


//-----------------------------------------------------------------------------
/**
 *	@brief  MCSS�\���ؑ�
 *
 *	@param	param[0] MCSS_ID
 *	@param	param[1] 0:��\��, 1:�\��
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL CMD_MCSS_SET_VISIBLE( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param )
{
  INTRO_MCSS_SetVisible( wk->mcss, param[1], param[0] );

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  MCSS�A�j���[�V�����w��
 *
 *	@param	param[0]  MCSS_ID
 *	@param	param[1]  �A�j���[�V����ID
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL CMD_MCSS_SET_ANIME( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param )
{
  INTRO_MCSS_SetAnimeIndex( wk->mcss, param[0], param[1] );
  return TRUE;
}

//=============================================================================
/**
 * �C���g���p�R�}���h
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  �Ђ炪�ȁ^�������[�h������
 *
 *	@param	param[0] 0=�Ђ炪�� �^ 1=����
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL CMD_SELECT_MOJI( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param )
{
  CONFIG* config;
  
  GF_ASSERT( param[0] == 0 || param[0] == 1 );

  config = SaveData_GetConfig( wk->init_param->save_ctrl );
  
  if( param[0] == 0 )
  {
    GFL_MSGSYS_SetLangID( 0 );
    CONFIG_SetMojiMode( config, MOJIMODE_HIRAGANA );
  }
  else
  {
    GFL_MSGSYS_SetLangID( 1 );
    CONFIG_SetMojiMode( config, MOJIMODE_KANJI );
  }

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���ʂ�����
 *
 *	@param	param[0]  0=���Ƃ��̂�,1=����Ȃ̂�
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL CMD_SELECT_SEX( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param )
{
  MYSTATUS* mystatus;

  mystatus = SaveData_GetMyStatus( wk->init_param->save_ctrl );

  if( param[0] == 0 )
  {
    MyStatus_SetMySex( mystatus , PTL_SEX_MALE );
  }
  else
  { 
    MyStatus_SetMySex( mystatus , PTL_SEX_FEMALE );
  }

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �|�P�����o�ꉉ�o
 *
 *	@param	param 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL CMD_POKEMON_APPER( INTRO_CMD_WORK* wk, INTRO_STORE_DATA* sdat, int* param )
{
  //@TODO

  return TRUE;
}

//=============================================================================
// ����֐�
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  �j������
 *
 *	@param	INTRO_CMD_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL CMD_COMP_SEX( INTRO_CMD_WORK* wk )
{
  MYSTATUS* mystatus;

  mystatus = SaveData_GetMyStatus( wk->init_param->save_ctrl );

  if( MyStatus_GetMySex( mystatus ) == PTL_SEX_MALE )
  {
    return TRUE;
  }

  return FALSE;
}

//=============================================================================
// WORDSET�֐�
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  �g���[�i�[�������W�X�g
 *
 *	@param	INTRO_CMD_WORK* wk
 *	@param	bufID 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void CMD_WORDSET_TRAINER( INTRO_CMD_WORK* wk, int bufID )
{
  WORDSET* wordset;
  const MYSTATUS* mystatus;

  wordset   = INTRO_MSG_GetWordSet( wk->wk_msg );
  mystatus  = SaveData_GetMyStatus( wk->init_param->save_ctrl );

  WORDSET_RegisterPlayerName( wordset, bufID, mystatus );
}



//=============================================================================
/**
 *								�O�����J�֐�
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  3D�f���R�}���h�R���g���[���[ ������
 *
 *	@param  init_param
 *	@param	heap_id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
INTRO_CMD_WORK* Intro_CMD_Init( INTRO_MCSS_WORK* mcss, INTRO_PARAM* init_param, HEAPID heap_id )
{
  INTRO_CMD_WORK* wk;

  GF_ASSERT( mcss );

  // ���C�����[�N �A���P�[�V����
  wk = GFL_HEAP_AllocClearMemory(  heap_id, sizeof( INTRO_CMD_WORK ) );

  // �����o������
  wk->heap_id     = heap_id;
  wk->init_param  = init_param;
  wk->mcss = mcss;

  // �������샂�W���[��������
  wk->wk_msg = INTRO_MSG_Create( heap_id );


  return wk;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  3D�f���R�}���h�R���g���[���[ �j��
 *
 *	@param	INTRO_CMD_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void Intro_CMD_Exit( INTRO_CMD_WORK* wk )
{
  // �I�������W���[���j��
  INTRO_MSG_Exit( wk->wk_msg );

  // �q�[�v�J��
  GFL_HEAP_FreeMemory( wk );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  3D�f���R�}���h�R���g���[���[ �又��
 *
 *	@param	INTRO_CMD_WORK* wk 
 *
 *	@retval TRUE:�p�� FALSE:�I��
 */
//-----------------------------------------------------------------------------
BOOL Intro_CMD_Main( INTRO_CMD_WORK* wk )
{
  INTRO_MSG_Main( wk->wk_msg );

  // �X�g�A���ꂽ�R�}���h�����s
  if( cmd_store_exec( wk ) == FALSE )
  {
    int i;

    // �R�}���h�����ׂďI��������A���̃R�}���h���Ăяo��
    for( i=0; i<STORE_NUM+1; i++ )
    {
      const INTRO_CMD_DATA* data;

      GF_ASSERT( i < STORE_NUM ); ///< �X�g�A���E�`�F�b�N
      
      data  = Intro_DATA_GetCmdData( wk->scene_id );
      data += wk->cmd_idx;
      
      //=======================================================
      // ��O�R�}���h
      //=======================================================
      // �V�[���ύX
      if( data->type == INTRO_CMD_TYPE_SET_SCENE )
      {
        // CMD_SET_SCENE
        c_cmdtbl[ data->type ]( wk, &wk->store_data[i], data->param );
                
        OS_TPrintf("\n*** next scene_id=%d ***\n",wk->scene_id);

        // ���̃V�[���̐擪�R�}���h
        data = Intro_DATA_GetCmdData( wk->scene_id );
      }
      //---------------------------------------------

      // �R�}���h�I������
      if( data->type == INTRO_CMD_TYPE_END )
      {
        // �I��
        return FALSE;
      }

      // ���̃R�}���h�������Ă���
      wk->cmd_idx++;

      // �X�g�A
      if( cmd_store( wk, data ) == FALSE )
      {
        // ���̃R�}���h��ǂݍ��܂Ȃ��ꍇ�̓��[�v���甲����
        break;
      }
    }
  }

  return TRUE;
}

//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  �R�}���h���X�g�A
 *
 *	@param	INTRO_CMD_DATA* data 
 *
 *	@retval TRUE:�p�����ēǂݍ���
 */
//-----------------------------------------------------------------------------
static BOOL cmd_store( INTRO_CMD_WORK* wk, const INTRO_CMD_DATA* data )
{
  GF_ASSERT( data->type != INTRO_CMD_TYPE_NULL );
  GF_ASSERT( data->type < INTRO_CMD_TYPE_END );

  // �R�}���h���X�g�A
  {
    int i;
    BOOL is_store = FALSE;

    // �󂫂�T��
    for( i=0; i<STORE_NUM; i++ )
    {
      if( wk->store[i] == NULL )
      {
        wk->store[i] = data;

        is_store = TRUE;
          
        // cmd_idx�͂��̒i�K�Ŏ���ID���w���Ă���̂�-1
        OS_TPrintf("store [%d] cmd_idx=%d type=%d [%d,%d,%d,%d]\n", i, wk->cmd_idx-1, data->type,
            data->param[0],
            data->param[1],
            data->param[2],
            data->param[3] );
        
        break;
      }
    }

    GF_ASSERT( is_store == TRUE );
  }
  
  return data->read_next;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �X�g�A���ꂽ�R�}���h�����s
 *
 *	@param	INTRO_CMD_WORK* wk 
 *
 *	@retval TRUE:�p�� FALSE:�S�ẴR�}���h���I��
 */
//-----------------------------------------------------------------------------
static BOOL cmd_store_exec( INTRO_CMD_WORK* wk )
{
  int i;
  BOOL is_continue = FALSE;

  for( i=0; i<STORE_NUM; i++ )
  {
    if( wk->store[i] )
    {
      const INTRO_CMD_DATA* data;

      data = wk->store[i];

      if( c_cmdtbl[ data->type ]( wk, &wk->store_data[i], data->param ) == FALSE )
      {
        // ��ł����s���̂��̂�����Ύ����[�v�����s
        is_continue = TRUE;
      }
      else
      {
        // �I�������R�}���h������
        wk->store[i] = NULL;
        // �V�[�P���X���N���A
        wk->store_data[i].seq = 0; 
        HOSAKA_Printf("store [%d] is finish \n", i );
      }
    }
  }

  return is_continue;
}


