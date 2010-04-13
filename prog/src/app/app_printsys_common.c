//=============================================================================
/**
 * @file	  app_printsys_common.c
 * @brief	  PRINTSYS_PrintStreamGetState�̏��������ʉ�����v���O����
 * @author  k.ohno
 * @date	  10/02/22
 */
//=============================================================================

#include <gflib.h>

#ifdef MULTI_BOOT_MAKE //�ʏ펞����
  #include "multiboot/wb_sound_palpark.sadl"
#endif

#include "sound/pm_sndsys.h"
#include "app/app_printsys_common.h"


//-------------------------------------
///	�v���g�^�C�v
//=====================================
static BOOL App_PrintSys_IsTrg( APP_PRINTSYS_COMMON_TYPE type );
static BOOL App_PrintSys_IsCont( APP_PRINTSYS_COMMON_TYPE type );
static BOOL App_PrintSys_IsRelease( APP_PRINTSYS_COMMON_TYPE type );

//----------------------------------------------------------------------------
/**
 *	@brief	�v�����g�V�X�e�����䏉����
 *	@param	APP_PRINTSYS_COMMON_WORK  ���[�N
 *	@param  type                      ���̓^�C�v
 */
//-----------------------------------------------------------------------------
void APP_PRINTSYS_COMMON_PrintStreamInit(APP_PRINTSYS_COMMON_WORK *wk, APP_PRINTSYS_COMMON_TYPE type)
{
  GFL_STD_MemClear( wk, sizeof(APP_PRINTSYS_COMMON_WORK) );
  wk->type  = type;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�v�����g�V�X�e�����s�֐�
 *  @param  PRINT_STREAM* �X�g���[���n���h��
 *	@param  handle  �v�����g�X�g���[��
 *
 *	@retval TRUE�ŏI��  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
BOOL APP_PRINTSYS_COMMON_PrintStreamFunc( APP_PRINTSYS_COMMON_WORK *wk, PRINT_STREAM* handle )
{
  if(handle){
    int state = PRINTSYS_PrintStreamGetState( handle );
    BOOL is_input = FALSE;

    switch(state){
    case PRINTSTREAM_STATE_DONE:
		  return TRUE;// �I����Ă���
      break;

    case PRINTSTREAM_STATE_PAUSE:
      //����
      if( App_PrintSys_IsTrg( wk->type ) )
      { 
        PMSND_PlaySE( APP_PRINTSYS_COMMON_SE_TRG );
  		  PRINTSYS_PrintStreamReleasePause( handle );
   		}
      break;

    case PRINTSTREAM_STATE_RUNNING:
      // ���b�Z�[�W�X�L�b�v
      if( (wk->type & APP_PRINTSYS_COMMON_TYPE_NOTSKIP) == FALSE )
      { 
        if(wk->trg == FALSE){               //�L�[�𗣂��Ė����ƗL���ɂȂ�Ȃ�
          if( App_PrintSys_IsRelease( wk->type ) )
          {
            wk->trg=TRUE;
          }
        }
        else{
          if( App_PrintSys_IsCont( wk->type ) )
          {
            PRINTSYS_PrintStreamShortWait( handle, 0 );
          }
        }
      }
      break;
    default:
      break;
    }
    return FALSE;  //�܂��I����ĂȂ�
  }
  return TRUE;// �I����Ă���
}

//----------------------------------------------------------------------------
/**
 *	@brief  �g���K��Ԃ����m
 *
 *	@param	APP_PRINTSYS_COMMON_TYPE type ���̓^�C�v
 *
 *	@return TRUE�g���K���Ă��� FALSE�ŉ��������Ă���
 */
//-----------------------------------------------------------------------------
static BOOL App_PrintSys_IsTrg( APP_PRINTSYS_COMMON_TYPE type )
{
  BOOL is_trg = FALSE;

  if(type == APP_PRINTSYS_COMMON_TYPE_THROUGH){
    is_trg  = TRUE;
  }
  //�L�[�̏ꍇ
  if( type & APP_PRINTSYS_COMMON_TYPE_KEY ){ 
    if(GFL_UI_KEY_GetTrg() & APP_PRINTSYS_COMMON_TRG_KEY){
      is_trg  = TRUE;
    }
  }
  //�^�b�`�̏ꍇ
  if( type & APP_PRINTSYS_COMMON_TYPE_TOUCH ){ 
    if( GFL_UI_TP_GetTrg() ){ 
      is_trg  = TRUE;
    }
  }

  return is_trg;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ����������Ԃ����m
 *
 *	@param	APP_PRINTSYS_COMMON_TYPE type ���̓^�C�v
 *
 *	@return TRUE�ŉ��������Ă��� FALSE�ŉ��������Ă���
 */
//-----------------------------------------------------------------------------
static BOOL App_PrintSys_IsCont( APP_PRINTSYS_COMMON_TYPE type )
{   
  BOOL is_cont = FALSE;
  
  if(type == APP_PRINTSYS_COMMON_TYPE_THROUGH){
    is_cont  = TRUE;
  }
  if( type & APP_PRINTSYS_COMMON_TYPE_KEY ){ 
    if( GFL_UI_KEY_GetCont() & APP_PRINTSYS_COMMON_TRG_KEY ){ 
      is_cont  = TRUE;
    }
  }

  if( type & APP_PRINTSYS_COMMON_TYPE_TOUCH ){ 
    if( GFL_UI_TP_GetCont() ){ 

      is_cont  = TRUE;
    }
  }

  return is_cont;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ������Ԃ����m
 *
 *	@param	APP_PRINTSYS_COMMON_TYPE type ���̓^�C�v
 *
 *	@return TRUE�ŗ����Ă��� FALSE�ŉ��������Ă���
 */
//-----------------------------------------------------------------------------
static BOOL App_PrintSys_IsRelease( APP_PRINTSYS_COMMON_TYPE type )
{ 
  BOOL is_release = FALSE;


  if(type == APP_PRINTSYS_COMMON_TYPE_THROUGH){
    is_release  = TRUE;
  }

  if( type & APP_PRINTSYS_COMMON_TYPE_KEY ){ 
    if( GFL_UI_KEY_GetCont() == 0 ){ 
      is_release  = TRUE;
    }
  }

  if( type & APP_PRINTSYS_COMMON_TYPE_TOUCH ){ 
    if( GFL_UI_TP_GetCont() == 0 ){ 

      is_release  = TRUE;
    }
  }

  return is_release;
}
