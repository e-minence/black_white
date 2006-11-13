//****************************************************************************
/**
 *
 *@file		touchpanel.c
 *@brief	��`�ƃ^�b�`�p�l���V�X�e�����Ƃ̓����蔻�菈��
 *@author	taya  >  katsumi ohno
 *@data		2005.07.29
 *
 */
//****************************************************************************

#include "gflib.h"

#define __TOUCHPANEL_H_GLOBAL__
#include "touchpanel.h"

//------------------------------------------------------------------
/**
 * @brief	TPSYS �^�錾
 * �^�b�`�p�l���̏��̕ێ�
  */
//------------------------------------------------------------------
struct _TPSYS {
	u16		tp_x;			///< �^�b�`�p�l��X���W
	u16		tp_y;			///< �^�b�`�p�l��Y���W
	u16		tp_trg;			///< �^�b�`�p�l���ڐG����g���K
	u16		tp_cont;		///< �^�b�`�p�l���ڐG������
};	// 8bytes


//==============================================================
// Prototype
//==============================================================
static BOOL circle_hitcheck( const TP_HIT_TBL *tbl, u32 x, u32 y );
static BOOL rect_hitcheck( const TP_HIT_TBL *tbl, u32 x, u32 y );

//------------------------------------------------------------------
/**
 * �~�`�Ƃ��Ă̓����蔻��i�P���j
 *
 * @param   tbl		�����蔻��e�[�u��
 * @param   x		X���W
 * @param   y		Y���W
 *
 * @retval  BOOL	TRUE�œ�����
 */
//------------------------------------------------------------------
static BOOL circle_hitcheck( const TP_HIT_TBL *tbl, u32 x, u32 y )
{
	x = (tbl->circle.x - x) * (tbl->circle.x - x);
	y = (tbl->circle.y - y) * (tbl->circle.y - y);

	if( x + y < (tbl->circle.r * tbl->circle.r) )
	{
		return TRUE;
	}

	return FALSE;
}
//------------------------------------------------------------------
/**
 * ��`�Ƃ��Ă̓����蔻��i�P���j
 *
 * @param   tbl		�����蔻��e�[�u��
 * @param   x		�w���W
 * @param   y		�x���W
 *
 * @retval  BOOL		TRUE�œ�����
 */
//------------------------------------------------------------------
static BOOL rect_hitcheck( const TP_HIT_TBL *tbl, u32 x, u32 y )
{
	if( ((u32)( x - tbl->rect.left) < (u32)(tbl->rect.right - tbl->rect.left))
	&	((u32)( y - tbl->rect.top) < (u32)(tbl->rect.bottom - tbl->rect.top))
	){
		return TRUE;
	}
	return FALSE;
}

//------------------------------------------------------------------
/**
 * ���^�C�v�i��`�E�~�`�j�����Ȃ��画�肷��i�x�^���́j
 *
 * @param   tpsys	�^�b�`�p�l���V�X�e��
 * @param   tbl		�����蔻��e�[�u���i�I�[�R�[�h����j
 *
 * @retval  int		�����肪����΂��̗v�f�ԍ��A�Ȃ���� TP_HIT_NONE
 */
//------------------------------------------------------------------
int GF_TP_HitCont( const TPSYS* tpsys, const TP_HIT_TBL *tbl )
{
	if( tpsys->tp_cont ){
		int i;

		for(i = 0; i < tbl[i].circle.code != TP_HIT_END; i++)
		{
			if( tbl[i].circle.code == TP_USE_CIRCLE )
			{
				if( circle_hitcheck( &tbl[i], tpsys->tp_x, tpsys->tp_y ) ){
					return i;
				}
			}
			else
			{
				if( rect_hitcheck( &tbl[i], tpsys->tp_x, tpsys->tp_y ) ){
					return i;
				}
			}
		}
	}
	return TP_HIT_NONE;
}
//------------------------------------------------------------------
/**
 * ���^�C�v�i��`�E�~�`�j�����Ȃ��画�肷��i�g���K���́j
 *
 * @param   tbl		�����蔻��e�[�u���i�I�[�R�[�h����j
 *
 * @retval  int		�����肪����΂��̗v�f�ԍ��A�Ȃ���� TP_HIT_NONE
 */
//------------------------------------------------------------------
int GF_TP_HitTrg( const TPSYS* tpsys, const TP_HIT_TBL *tbl )
{
	if( tpsys->tp_trg ){
		int i;

		for(i = 0; i < tbl[i].circle.code != TP_HIT_END; i++)
		{
			if( tbl[i].circle.code == TP_USE_CIRCLE )
			{
				if( circle_hitcheck( &tbl[i], tpsys->tp_x, tpsys->tp_y ) ){
					return i;
				}
			}
			else
			{
				if( rect_hitcheck( &tbl[i], tpsys->tp_x, tpsys->tp_y ) ){
					return i;
				}
			}
		}
	}
	return TP_HIT_NONE;
}

//------------------------------------------------------------------
/**
 * �^�b�`�p�l���ɐG��Ă��邩
 *
 * @retval  BOOL		TRUE�ŐG��Ă���
 */
//------------------------------------------------------------------
BOOL GF_TP_GetCont( const TPSYS* tpsys )
{
	return tpsys->tp_cont;
}
//------------------------------------------------------------------
/**
 * �^�b�`�p�l���ɐG��Ă��邩�i�g���K�j
 *
 * @retval  BOOL		TRUE�ŐG�ꂽ
 */
//------------------------------------------------------------------
BOOL GF_TP_GetTrg( const TPSYS* tpsys )
{
	return tpsys->tp_trg;
}

//------------------------------------------------------------------
/**
 * �^�b�`�p�l���ɐG��Ă���Ȃ炻�̍��W�擾�i�x�^���́j
 *
 * @param   x		�w���W�󂯎��ϐ��A�h���X
 * @param   y		�x���W�󂯎��ϐ��A�h���X
 *
 * @retval  BOOL	TRUE�ŐG��Ă���BFALSE���Ԃ����ꍇ�A�����ɂ͉������Ȃ��B
 */
//------------------------------------------------------------------
BOOL GF_TP_GetPointCont( const TPSYS* tpsys, u32* x, u32* y )
{
	if( tpsys->tp_cont )
	{
		*x = tpsys->tp_x;
		*y = tpsys->tp_y;
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------
/**
 * �^�b�`�p�l���ɐG��Ă���Ȃ炻�̍��W�擾�i�g���K���́j
 *
 * @param   x		�w���W�󂯎��ϐ��A�h���X
 * @param   y		�x���W�󂯎��ϐ��A�h���X
 *
 * @retval  BOOL	TRUE�ŐG��Ă���BFALSE���Ԃ����ꍇ�A�����ɂ͉������Ȃ��B
 */
//------------------------------------------------------------------
BOOL GF_TP_GetPointTrg( const TPSYS* tpsys, u32* x, u32* y )
{
	if( tpsys->tp_trg )
	{
		*x = tpsys->tp_x;
		*y = tpsys->tp_y;
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *
 *	@brief	�w�肵�����W�ŁA�����蔻����s���܂��B	����
 *
 *	@param	tbl		�����蔻��e�[�u���i�z��j
 *	@param	x		���肘���W
 *	@param	y		���肙���W
 *
 *	@retval  int		�����肪����΂��̗v�f�ԍ��A�Ȃ���� TP_HIT_NONE
 *
 *
 */
//-----------------------------------------------------------------------------
int GF_TP_HitSelf( const TP_HIT_TBL *tbl, u32 x, u32 y )
{
	int i;
	
	for(i = 0; i < tbl[i].circle.code != TP_HIT_END; i++)
	{
		if( tbl[i].circle.code == TP_USE_CIRCLE )
		{
			if( circle_hitcheck( &tbl[i], x, y ) ){
				return i;
			}
		}
		else
		{
			if( rect_hitcheck( &tbl[i], x, y ) ){
				return i;
			}
		}
	}
	return TP_HIT_NONE;
}

