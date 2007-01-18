
//=============================================================================================
/**
 * @file	fade.c
 * @brief	��ʃt�F�[�h�V�X�e��
 * @author	Hisashi Sogabe
 * @date	2007/01/18
 */
//=============================================================================================
#include "gflib.h"

#define	__FADE_H_GLOBAL__
#include "fade.h"

struct	_GFL_FADE_SYS
{
	int	mode;
	int	start_evy;
	int	now_evy;
	int	end_evy;
	int	wait;
	int	wait_tmp;
	int	fade_value;
};

typedef struct	_GFL_FADE_SYS	GFL_FADE_SYS;

static	GFL_FADE_SYS	*fade_sys=NULL;

static	void	GFL_FADE_MasterBrightAct( void );

#define	DEFAULT_FADE_VALUE	(1)		//�}�X�^�[�P�x��evy�v�Z�̊�{�l

//--------------------------------------------------------------------------------------------
/**
 * �t�F�[�h�V�X�e��������
 *
 * @param	heapID	�q�[�v�h�c
 *
 * @return	�擾�����������̃A�h���X
 */
//--------------------------------------------------------------------------------------------
void GFL_FADE_sysInit( u32 heapID )
{
	fade_sys  = (GFL_FADE_SYS *)GFL_HEAP_AllocMemory( heapID, sizeof(GFL_FADE_SYS) );
}

//--------------------------------------------------------------------------------------------
/**
 * �t�F�[�h�V�X�e�����C��
 */
//--------------------------------------------------------------------------------------------
void	GFL_FADE_sysMain( void )
{
	if( fade_sys == NULL ){
		return;
	}
	if( fade_sys->mode & GFL_FADE_MASTER_BRIGHT ){
		if( fade_sys->wait ){
			fade_sys->wait--;
		}
		else{

			fade_sys->wait = fade_sys->wait_tmp;
			fade_sys->now_evy += fade_sys->fade_value;

			if( ( ( fade_sys->now_evy >= fade_sys->end_evy ) && ( fade_sys->fade_value >0 ) ) ||
				( ( fade_sys->now_evy <= fade_sys->end_evy ) && ( fade_sys->fade_value <0 ) ) ){
				fade_sys->now_evy = fade_sys->end_evy;
			}
			 
			GFL_FADE_MasterBrightAct();

			if( fade_sys->now_evy == fade_sys->end_evy ){
				fade_sys->mode = 0;
			}

		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �t�F�[�h�V�X�e���I��
 */
//--------------------------------------------------------------------------------------------
void	GFL_FADE_sysExit( void )
{
	GFL_HEAP_FreeMemory( fade_sys );
	fade_sys = NULL;
}

//--------------------------------------------------------------------------------------------
/**
 * �}�X�^�[�P�x���N�G�X�g
 *
 * @param	mode		�t�F�[�h���[�h�iGFL_FADE_MASTER_BRIGHT_BLACKOUT or GFL_FADE_MASTER_BRIGHT_WHITEOUT�j
 * @param	start_evy	�X�^�[�g�P�x�i0�`16�j
 * @param	end_evy		�G���h�P�x�i0�`16�j
 * @param	wait		�t�F�[�h�X�s�[�h
 */
//--------------------------------------------------------------------------------------------
void	GFL_FADE_MasterBrightReq( int mode, int start_evy, int end_evy, int wait )
{
	fade_sys->mode = mode;
	fade_sys->start_evy = start_evy;
	fade_sys->end_evy = end_evy;

	if( wait < 0 ){
		fade_sys->fade_value = DEFAULT_FADE_VALUE + MATH_ABS( wait );
		fade_sys->wait = 0;
		fade_sys->wait_tmp = 0;
	}
	else{
		fade_sys->fade_value = DEFAULT_FADE_VALUE;
		fade_sys->wait = wait;
		fade_sys->wait_tmp = wait;
	}

	if( fade_sys->start_evy > fade_sys->end_evy ){
		fade_sys->fade_value *= -1;
	}

	fade_sys->now_evy = fade_sys->start_evy;

	GFL_FADE_MasterBrightAct();
}

//--------------------------------------------------------------------------------------------
/**
 * �t�F�[�h���s���̃`�F�b�N
 *
 * @reval	TRUE:���s�� FALSE:�����s
 */
//--------------------------------------------------------------------------------------------
BOOL	GFL_FADE_FadeCheck( void )
{
	return ( fade_sys->mode !=0 );
}

//--------------------------------------------------------------------------------------------
/**
 * �t�F�[�h�������s
 */
//--------------------------------------------------------------------------------------------
static	void	GFL_FADE_MasterBrightAct( void )
{
	if( fade_sys->mode & GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN){
		GX_SetMasterBrightness( -fade_sys->now_evy );
	}
	if( fade_sys->mode & GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB){
		GXS_SetMasterBrightness( -fade_sys->now_evy );
	}
	if( fade_sys->mode & GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN){
		GX_SetMasterBrightness( fade_sys->now_evy );
	}
	if( fade_sys->mode & GFL_FADE_MASTER_BRIGHT_WHITEOUT_SUB){
		GXS_SetMasterBrightness( fade_sys->now_evy );
	}
}

