//=============================================================================
/**
 * @file	  app_printsys_common.c
 * @brief	  PRINTSYS_PrintStreamGetState�̏��������ʉ�����v���O����
 * @author  k.ohno
 * @date	  10/02/22
 */
//=============================================================================

#include <gflib.h>
#include "app/app_printsys_common.h"


//----------------------------------------------------------------------------
/**
 *	@brief	�v�����g�V�X�e�����䏉����
 *	@param	�g���K�[�𐧌䂷�胏�[�N u8
 */
//-----------------------------------------------------------------------------

void APP_PRINTSYS_COMMON_PrintStreamInit(u8* trgWork)
{
 *trgWork = FALSE; 
}


//----------------------------------------------------------------------------
/**
 *	@brief	�v�����g�V�X�e�����s�֐�
 *  @param  PRINT_STREAM* �X�g���[���n���h��
 *	@param	�������œn�����g���K�[�𐧌䂷�胏�[�N u8
 */
//-----------------------------------------------------------------------------

BOOL APP_PRINTSYS_COMMON_PrintStreamFunc(PRINT_STREAM* handle,u8* trgWork)
{
  if(handle){
    int state = PRINTSYS_PrintStreamGetState( handle );
    switch(state){
    case PRINTSTREAM_STATE_DONE:
		  return TRUE;// �I����Ă���
      break;
    case PRINTSTREAM_STATE_PAUSE:
      if(GFL_UI_KEY_GetTrg() & (PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL)){
  		  PRINTSYS_PrintStreamReleasePause( handle );
   		}
      break;
    case PRINTSTREAM_STATE_RUNNING:
      // ���b�Z�[�W�X�L�b�v
			if(*trgWork==FALSE){               //�L�[�𗣂��Ė����ƗL���ɂȂ�Ȃ�
         if( GFL_UI_KEY_GetCont() == 0 ){
            *trgWork=TRUE;
         }
      }
      else{
        if( GFL_UI_KEY_GetCont() & (PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL) )
        {
          PRINTSYS_PrintStreamShortWait( handle, 0 );
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


