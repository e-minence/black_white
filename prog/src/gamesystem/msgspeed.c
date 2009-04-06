//==============================================================================
/**
 * @file		msgspeed.c
 * @brief		���[�U�R���t�B�O���A�e��������烁�b�Z�[�W�\�����x�����肷��
 * @author	taya
 * @date		2009.04.06
 */
//==============================================================================
#include <gflib.h>

#include "savedata\config.h"
#include "gamesystem\msgspeed.h"


static const SAVE_CONTROL_WORK*  SaveCtrl = NULL;


//=============================================================================================
/**
 * �V�X�e�������� - ���x����ɕK�v�ȏ��ւ̃A�N�Z�T��n�� - 
 * ���Q�[���N����ɂP�񂾂��Ăт���
 *
 * @param   ctrl		�Z�[�u�f�[�^�R���g���[��
 *
 */
//=============================================================================================
void MSGSPEED_InitSystem( const SAVE_CONTROL_WORK* ctrl )
{
	SaveCtrl = ctrl;
}

//=============================================================================================
/**
 * ���b�Z�[�W���x�擾
 *
 * @retval  int		���b�Z�[�W���x�i���̒l�� PRINTSYS_PrintStream ���� wait �l�Ƃ��Ă��̂܂܎g���j
 */
//=============================================================================================
int MSGSPEED_GetWait( void )
{
	CONFIG*  cfg = SaveData_GetConfig( (SAVE_CONTROL_WORK*)SaveCtrl );

// @@@ GFL_NET_GetConnectNum

	switch( CONFIG_GetMsgSpeed(cfg) ){
	case MSGSPEED_SLOW:		return  3;
	case MSGSPEED_NORMAL:	return  1;
	case MSGSPEED_FAST:		return -2;
	default:
		GF_ASSERT(0);
		return -2;
	}
}

