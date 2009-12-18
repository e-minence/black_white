//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		misc.c
 *	@brief	�����ރZ�[�u�f�[�^�A�N�Z�X
 *	@author	Toru=Nagihashi
 *	@data		2009.10.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include <gflib.h>
#include "savedata/misc.h"
#include "savedata/save_tbl.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�����ރZ�[�u�f�[�^
//	�E�ǂ��ɕ��ނ���ׂ������f�ɖ������́B
//	�E�K�͂����������̂��߂Ƀu���b�N�m�ۂ�����̂����������Ȃ�����
//=====================================
struct _MISC
{	
	u16 favorite_monsno;		//���C�ɓ���|�P����
	u8  favorite_form_no:7;		//���C�ɓ���|�P�����̃t�H�����ԍ�
	u8  favorite_egg_flag:1;	//���C�ɓ���|�P�����̃^�}�S�t���O
	u8	namein_mode[NAMEIN_MAX];	//5��

	PMS_DATA gds_self_introduction;		// GDS�v���t�B�[���̎��ȏЉ�b�Z�[�W
	
	//�p���p�[�N
	u32 palpark_highscore:28;
	u32 palpark_finish_state:4;
};

//=============================================================================
/**
 *	�v���g�^�C�v
 */
//=============================================================================
//-------------------------------------
///	���O����
//=====================================
static void MISC_InitNameIn( MISC *p_misc );

//=============================================================================
/**
 *	�Z�[�u�f�[�^�V�X�e���Ɉˑ�����֐�
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	MISC�\���̂̃T�C�Y�擾
 *
 *	@return	MISC�\���̂̃T�C�Y
 */
//-----------------------------------------------------------------------------
int MISC_GetWorkSize( void )
{	
	return sizeof(MISC);
}
//----------------------------------------------------------------------------
/**
 *	@brief	MISC�\���̏�����
 *
 *	@param	MISC *p_msc ���[�N
 */
//-----------------------------------------------------------------------------
void MISC_Init( MISC *p_msc )
{	
	GFL_STD_MemClear( p_msc, sizeof(MISC) );

	MISC_InitNameIn( p_msc );
	
	p_msc->palpark_highscore = 100; //��
}

//=============================================================================
/**
 *	�Z�[�u�f�[�^�擾�̂��߂̊֐�
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�����ރf�[�^��CONST�Ń|�C���^�擾
 *
 *	@param	SAVE_CONTROL_WORK * cp_sv	�Z�[�u�f�[�^�ێ����[�N�ւ̃|�C���^
 *
 *	@return	MISC
 */
//-----------------------------------------------------------------------------
const MISC * SaveData_GetMiscConst( const SAVE_CONTROL_WORK * cp_sv)
{	
	return (const MISC *)SaveData_GetMisc( (SAVE_CONTROL_WORK *)cp_sv);
}
//----------------------------------------------------------------------------
/**
 *	@brief	�����ރf�[�^�̃|�C���^�擾
 *
 *	@param	SAVE_CONTROL_WORK * p_sv	�Z�[�u�f�[�^�ێ����[�N�ւ̃|�C���^
 *
 *	@return	MISC
 */
//-----------------------------------------------------------------------------
MISC * SaveData_GetMisc( SAVE_CONTROL_WORK * p_sv)
{	
	MISC	*p_msc;
	p_msc = SaveControl_DataPtrGet(p_sv, GMDATA_ID_MISC);
	return p_msc;
}

//=============================================================================
/**
 *	���O���͂̊֐�
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	���O���͂̃��[�h�ݒ�
 *
 *	@param	MISC *p_misc	MISC
 *	@param	mode					���[�h
 *	@param	input_type		���̓^�C�v
 */
//-----------------------------------------------------------------------------
void MISC_SetNameInMode( MISC *p_misc, NAMEIN_MODE mode, u8 input_type )
{	
	p_misc->namein_mode[ mode ]	= input_type;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���O���͂̃��[�h�擾
 *
 *	@param	const MISC *cp_misc		MISC
 *	@param	mode									���[�h
 *
 *	@return	���̓^�C�v
 */
//-----------------------------------------------------------------------------
u8 MISC_GetNameInMode( const MISC *cp_misc, NAMEIN_MODE mode )
{	
	return cp_misc->namein_mode[ mode ];
}

//----------------------------------------------------------------------------
/**
 *	@brief	���O���͂̏�����
 *
 *	@param	MISC *p_misc ���[�N
 */
//-----------------------------------------------------------------------------
static void MISC_InitNameIn( MISC *p_misc )
{
	p_misc->namein_mode[ NAMEIN_POKEMON ] = 1;
}

//==============================================================================
//  GDS
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   ���C�ɓ���|�P�����̃Z�b�g
 *
 * @param   misc		
 * @param   monsno		�|�P�����ԍ�
 * @param   form_no		�t�H�����ԍ�
 * @param   egg_flag	�^�}�S�t���O
 */
//--------------------------------------------------------------
void MISC_SetFavoriteMonsno(MISC * misc, int monsno, int form_no, int egg_flag)
{
	misc->favorite_monsno = monsno;
	misc->favorite_form_no = form_no;
	misc->favorite_egg_flag = egg_flag;
}

//--------------------------------------------------------------
/**
 * @brief   ���C�ɓ���|�P�����擾
 *
 * @param   misc		
 * @param   monsno		�|�P�����ԍ�(���C�ɓ����ݒ肵�Ă��Ȃ��ꍇ��0)
 * @param   form_no		�t�H�����ԍ�
 * @param   egg_flag	�^�}�S�t���O
 */
//--------------------------------------------------------------
void MISC_GetFavoriteMonsno(const MISC * misc, int *monsno, int *form_no, int *egg_flag)
{
	*monsno = misc->favorite_monsno;
	*form_no = misc->favorite_form_no;
	*egg_flag = misc->favorite_egg_flag;
}

//--------------------------------------------------------------
/**
 * @brief   GDS���ȏЉ�b�Z�[�W���擾
 *
 * @param   misc		
 * @param   pms			�����
 */
//--------------------------------------------------------------
void MISC_GetGdsSelfIntroduction(const MISC *misc, PMS_DATA *pms)
{
	*pms = misc->gds_self_introduction;
}

//--------------------------------------------------------------
/**
 * @brief   GDS���ȏЉ�b�Z�[�W���Z�b�g����
 *
 * @param   misc		
 * @param   pms			�Z�b�g���郁�b�Z�[�W
 */
//--------------------------------------------------------------
void MISC_SetGdsSelfIntroduction(MISC *misc, const PMS_DATA *pms)
{
	misc->gds_self_introduction = *pms;
}


//--------------------------------------------------------------
/**
 * @brief   �p���p�[�N�F�n�C�X�R�A�擾/�ݒ�
 *
 * @param   misc		
 */
//--------------------------------------------------------------
const u32  MISC_GetPalparkHighscore(const MISC *misc)
{
	return misc->palpark_highscore;
}
void  MISC_SetPalparkHighscore(MISC *misc , u32 score)
{
	misc->palpark_highscore = score;
}

//--------------------------------------------------------------
/**
 * @brief   �p���p�[�N�F�I���X�e�[�g�擾/�ݒ�
 *          script/palpark_scr_local.h �ɒ�`
 * @param   misc		
 */
//--------------------------------------------------------------
const u8  MISC_GetPalparkFinishState(const MISC *misc)
{
	return misc->palpark_finish_state;
}
void  MISC_SetPalparkFinishState(MISC *misc , u8 state)
{
	misc->palpark_finish_state = state;
}
