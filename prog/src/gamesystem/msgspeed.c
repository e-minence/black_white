//==============================================================================
/**
 * @file    msgspeed.c
 * @brief   ���[�U�R���t�B�O���A�e��������烁�b�Z�[�W�\�����x�����肷��
 * @author  taya
 * @date    2009.04.06
 */
//==============================================================================
#include <gflib.h>

#include "savedata\config.h"
#include "gamesystem\msgspeed.h"



/*--------------------------------------------------------------------------*/
/* Tables                                                                   */
/*--------------------------------------------------------------------------*/

/**
 *  �E�F�C�g�t���[�����e�[�u��
 *
 *  �W���I�ȕ������̍��ɂ��v���C�S�̂̑��x�����ɘa���邽�߁A
 *  ���؂͒x�߁A���Ă͑��߂ɐݒ肵�Ă���B
 */
#if( (PM_LANG==LANG_JAPAN) || (PM_LANG==LANG_KOREA) )
static const s8 WaitFrameTable[] = {
    7,
    3,
    0,
   -2,    // �ő�
   12,    // �Œx
};
#else
static const s8 WaitFrameTable[] = {
    3,
    1,
   -2,
   -4,  // �ő�
    6,  // �Œx
};
#endif

/*--------------------------------------------------------------------------*/
/* Globals                                                                  */
/*--------------------------------------------------------------------------*/
static const SAVE_CONTROL_WORK*  SaveCtrl = NULL;


//=============================================================================================
/**
 * �V�X�e�������� - ���x����ɕK�v�ȏ��ւ̃A�N�Z�T��n�� -
 * ���Q�[���N����ɂP�񂾂��Ăт���
 *
 * @param   ctrl    �Z�[�u�f�[�^�R���g���[��
 *
 */
//=============================================================================================
void MSGSPEED_InitSystem( const SAVE_CONTROL_WORK* ctrl )
{
  SaveCtrl = ctrl;
}

//=============================================================================================
/**
 * �W���E�F�C�g�t���[�����擾
 *
 * @retval  int   �E�F�C�g�t���[�����i���̒l�� PRINTSYS_PrintStream ���� wait �l�Ƃ��Ă��̂܂܎g���j
 */
//=============================================================================================
int MSGSPEED_GetWait( void )
{
  CONFIG*  cfg = SaveData_GetConfig( (SAVE_CONTROL_WORK*)SaveCtrl );
  MSGSPEED speed = CONFIG_GetMsgSpeed( cfg );

// @@@ �ʐM����͖��Ή��B�>GFL_NET_GetConnectNum

  if( speed >= NELEMS(WaitFrameTable) ){
    GF_ASSERT(0);
    speed = NELEMS(WaitFrameTable) - 1;
  }

  return WaitFrameTable[ speed ];
}

//=============================================================================================
/**
 * �g�����h�E�F�C�g�t���[�������擾�igmm�Ń��b�Z�[�W���x�R���g���[�� 'FAST' �ɂ������̃t���[�����j
 *
 * @retval  int
 */
//=============================================================================================
int MSGSPEED_GetWaitFast( void )
{
  CONFIG*  cfg = SaveData_GetConfig( (SAVE_CONTROL_WORK*)SaveCtrl );
  MSGSPEED speed = CONFIG_GetMsgSpeed( cfg );

  switch( speed ){
  case MSGSPEED_SLOW:   speed = MSGSPEED_NORMAL; break;
  case MSGSPEED_NORMAL: speed = MSGSPEED_FAST; break;
  case MSGSPEED_FAST:   speed = MSGSPEED_FAST_EX; break;
  }

  return MSGSPEED_GetWaitByConfigParam( speed );
}

//=============================================================================================
/**
 * �g�x���h�E�F�C�g�t���[�������擾�igmm�Ń��b�Z�[�W���x�R���g���[�� 'SLOW' �ɂ������̃t���[�����j
 *
 * @retval  int
 */
//=============================================================================================
int MSGSPEED_GetWaitSlow( void )
{
  CONFIG*  cfg = SaveData_GetConfig( (SAVE_CONTROL_WORK*)SaveCtrl );
  MSGSPEED speed = CONFIG_GetMsgSpeed( cfg );

  switch( speed ){
  case MSGSPEED_SLOW:   speed = MSGSPEED_SLOW_EX; break;
  case MSGSPEED_NORMAL: speed = MSGSPEED_SLOW; break;
  case MSGSPEED_FAST:   speed = MSGSPEED_NORMAL; break;
  }

  return MSGSPEED_GetWaitByConfigParam( speed );
}

//=============================================================================================
/**
 * CONFIG�ݒ�l�ɑΉ������E�F�C�g�t���[�����擾
 *
 * @param   speed   CONFIG�ݒ�l
 *
 * @retval  int   �E�F�C�g�t���[�����擾�i���̒l�� PRINTSYS_PrintStream ���� wait �l�Ƃ��Ă��̂܂܎g���j
 */
//=============================================================================================
int MSGSPEED_GetWaitByConfigParam( MSGSPEED speed )
{
  if( speed >= NELEMS(WaitFrameTable) ){
    GF_ASSERT(0);
    speed = NELEMS(WaitFrameTable) - 1;
  }

  return WaitFrameTable[ speed ];
}
