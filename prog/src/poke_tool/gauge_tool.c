//============================================================================================
/**
 * @file		gage_tool.c
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
 * @brief	 ���ݒl�̃Q�[�W�h�b�g�����擾
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
 * @brief   �\���h�b�g�ƍő�h�b�g����HP�Q�[�W�̐F���擾����
 *
 * @param   put_dot		�\���h�b�g��
 * @param   max_dot		�ő�h�b�g��
 *
 * @retval  �Q�[�W�J���[
 */
//--------------------------------------------------------------------------------------------
u8 GAUGETOOL_GetGaugeDottoColor( u32 put_dot, u32 max_dot )
{
	put_dot <<= 8;		//����Z�g�p�ׁ̈A�������x���܂Ō����悤�ɌŒ菬����
	max_dot <<= 8;
	
	if( put_dot > (max_dot/2) )
  {
		return GAUGETOOL_HP_DOTTO_GREEN;		// ��
	}
  else if( put_dot > ( max_dot / 5 ) )
  {
		return GAUGETOOL_HP_DOTTO_YELLOW;	// ��
	}
  else if( put_dot > 0 )
  {
		return GAUGETOOL_HP_DOTTO_RED;			// ��
	}
	return GAUGETOOL_HP_DOTTO_NULL;			// HP=0
}
