//============================================================================================
/**
 * @file		scroll_bar.c
 * @brief		�X�N���[���o�[�֘A����
 * @author	Hiroyuki Nakamura
 * @date		10.02.03
 *
 *	���W���[�����FSCROLLBAR
 */
//============================================================================================
#include <gflib.h>

#include "system/scroll_bar.h"


//--------------------------------------------------------------------------------------------
/**
 * @brief		�X�N���[���o�[�̕\���x���W����X�N���[���l���擾
 *
 * @param		scrollMax			�X�N���[���ő�l
 * @param		py						�X�N���[���o�[�̕\���x���W
 * @param		railTop				���[���㕔�̂x���W
 * @param		railBottom		���[�������̂x���W
 * @param		barSize				�o�[�̂x�T�C�Y
 *
 * @return	�x���W
 */
//--------------------------------------------------------------------------------------------
u32 SCROLLBAR_GetCount( u32 scrollMax, u32 py, u32 railTop, u32 railBottom, u32 barSize )
{
	u32	cnt;

	barSize /= 2;

	railTop += barSize;
	railBottom -= barSize;

	if( py < railTop ){
		py = railTop;
	}else if( py > railBottom ){
		py = railBottom;
	}
	py -= railTop;

	cnt = ( ( (scrollMax+1) << 8 ) / ( railBottom - railTop ) * py ) >> 8;

	return cnt;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�X�N���[���o�[�̕\���x���W���擾
 *
 * @param		scrollMax			�X�N���[���ő�l
 * @param		scrollCount		���݂̃X�N���[���l
 * @param		railTop				���[���㕔�̂x���W
 * @param		railBottom		���[�������̂x���W
 * @param		barSize				�o�[�̂x�T�C�Y
 *
 * @return	�x���W
 */
//--------------------------------------------------------------------------------------------
u32 SCROLLBAR_GetPosY( u32 scrollMax, u32 scrollCount, u32 railTop, u32 railBottom, u32 barSize )
{
	u32	py;

	barSize /= 2;
	py = railBottom - railTop - barSize;
	py = ( ( py << 8 ) / ( scrollMax + 1 ) * scrollCount ) >> 8;

	return ( railTop + py + barSize );
}
