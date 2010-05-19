//============================================================================================
/**
 * @file		gauge_tool.c
 * @brief		�Q�[�W�֘A����
 * @author	Hiroyuki Nakamura
 * @date		09.09.30
 *
 *	���W���[�����FGAUGETOOL
 */
//============================================================================================
#include <gflib.h>

#include "poke_tool/gauge_tool.h"



//--------------------------------------------------------------------------------------------
/**
 * @brief		���ݒl�̃Q�[�W�h�b�g�����擾
 *
 * @param		prm_now		���ݒl
 * @param		prm_max		�ő�l
 * @param		dot_max		�ő�h�b�g��
 *
 * @return	�h�b�g��
 */
//--------------------------------------------------------------------------------------------
u8 GAUGETOOL_GetNumDotto( u32 prm_now, u32 prm_max, u8 dot_max )
{
	u8 put_dot;
	
	put_dot = prm_now * dot_max / prm_max;
	if( put_dot == 0 && prm_now > 0 )   // �ޯČv�Z�ł�0�ł����ۂ̒l��1�ȏ�Ȃ�1�ޯĂɂ���
  {
		put_dot = 1;
	}
	return put_dot;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   HP�Q�[�W�̃J���[���擾
 *
 * @param   now			���݂̒l
 * @param   max			�ő�l
 *
 * @return	�Q�[�W�J���[
 */
//--------------------------------------------------------------------------------------------
u8 GAUGETOOL_GetGaugeDottoColor( u32 now, u32 max )
{
	now <<= 8;		//����Z�g�p�ׁ̈A�������x���܂Ō����悤�ɌŒ菬����
	max <<= 8;
	
	if( now > (max/2) )
  {
		return GAUGETOOL_HP_DOTTO_GREEN;	// ��
	}
  else if( now > (max/5) )
  {
		return GAUGETOOL_HP_DOTTO_YELLOW;	// ��
	}
  else if( now > 0 )
  {
		return GAUGETOOL_HP_DOTTO_RED;		// ��
	}
	return GAUGETOOL_HP_DOTTO_NULL;			// HP=0
}
