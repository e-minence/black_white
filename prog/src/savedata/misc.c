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
#include "misc_local.h"

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
	
	p_msc->self_introduction[0] = GFL_STR_GetEOMCode();
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


//==============================================================================
//  ����Ⴂ�ʐM
//==============================================================================
//==================================================================
/**
 * ����Ⴂ�ʐM�F���������ꂽ�񐔂��擾
 *
 * @param   misc		
 *
 * @retval  u32		���������ꂽ��
 */
//==================================================================
u32 MISC_CrossComm_GetThanksRecvCount(const MISC *misc)
{
  return misc->thanks_recv_count;
}

//==================================================================
/**
 * ����Ⴂ�ʐM�F���������ꂽ�񐔂��C���N�������g
 *
 * @param   misc		
 *
 * @retval  u32		�C���N�������g��̂��������ꂽ��
 */
//==================================================================
u32 MISC_CrossComm_IncThanksRecvCount(MISC *misc)
{
  if(misc->thanks_recv_count < CROSS_COMM_THANKS_RECV_COUNT_MAX){
    misc->thanks_recv_count++;
  }
  return misc->thanks_recv_count;
}

//==================================================================
/**
 * ����Ⴂ�����l�����擾
 *
 * @param   misc		
 *
 * @retval  u32		����Ⴂ�������l��
 */
//==================================================================
u32 MISC_CrossComm_GetSuretigaiCount(const MISC *misc)
{
  return misc->suretigai_count;
}

//==================================================================
/**
 * ����Ⴂ�������l�����C���N�������g
 *
 * @param   misc		
 *
 * @retval  u32		�C���N�������g��̂���Ⴂ�������l��
 */
//==================================================================
u32 MISC_CrossComm_IncSuretigaiCount(MISC *misc)
{
  if(misc->suretigai_count < CROSS_COMM_SURETIGAI_COUNT_MAX){
    misc->suretigai_count++;
  }
  return misc->suretigai_count;
}

//==================================================================
/**
 * ����Ⴂ�ʐM�̎��ȏЉ���擾
 *
 * @param   misc		
 *
 * @retval  const STRCODE *		
 */
//==================================================================
const STRCODE * MISC_CrossComm_GetSelfIntroduction(const MISC *misc)
{
  return misc->self_introduction;
}

//==================================================================
/**
 * ����Ⴂ�ʐM�̎��ȏЉ���Z�b�g
 *
 * @param   misc		
 * @param   srcbuf		
 */
//==================================================================
void MISC_CrossComm_SetSelfIntroduction(MISC *misc, const STRBUF *srcbuf)
{
  GFL_STR_GetStringCode( srcbuf, misc->self_introduction, SAVE_SURETIGAI_SELFINTRODUCTION_LEN );
}

//==================================================================
/**
 * ����Ⴂ�ʐM�̂��烁�b�Z�[�W���擾
 *
 * @param   misc		
 *
 * @retval  const STRCODE *		
 */
//==================================================================
const STRCODE * MISC_CrossComm_GetThankyouMessage(const MISC *misc)
{
  return misc->thankyou_message;
}

//==================================================================
/**
 * ����Ⴂ�ʐM�̂��烁�b�Z�[�W���Z�b�g
 *
 * @param   misc		
 * @param   srcbuf		
 */
//==================================================================
void MISC_CrossComm_SetThankyouMessage(MISC *misc, const STRBUF *srcbuf)
{
  GFL_STR_GetStringCode( srcbuf, misc->thankyou_message, SAVE_SURETIGAI_THANKYOU_LEN );
}

//----------------------------------------------------------
/**
 * @brief �o�b�W�ێ���Ԃ̎擾
 * @param my      ������ԕێ����[�N�ւ̃|�C���^
 * @param badge_id  �o�b�W�̎w��
 * @return  BOOL  �o�b�W�������Ă��邩�ǂ���
 */
//----------------------------------------------------------
BOOL MISC_GetBadgeFlag(const MISC * misc, int badge_id)
{
  if (misc->badge & (1 << badge_id)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

//----------------------------------------------------------
/**
 * @brief �o�b�W�擾�̃Z�b�g
 * @param misc      ������ԕێ����[�N�ւ̃|�C���^
 * @param badge_id  �o�b�W�̎w��
 */
//----------------------------------------------------------
void MISC_SetBadgeFlag(MISC * misc, int badge_id)
{
  misc->badge |= (1 << badge_id);
}

//----------------------------------------------------------
/**
 * @brief �����Ă���o�b�W�̐����擾����
 * @param misc      ������ԕێ����[�N�ւ̃|�C���^
 * @return  int   �����Ă���o�b�W�̐��i�O�`�R�Q�j
 */
//----------------------------------------------------------
int MISC_GetBadgeCount(const MISC * misc)
{
  int count = 0;
  u32 badge;

  for (badge = misc->badge; badge != 0; badge >>= 1) {
    if (badge & 1) {
      count ++;
    }
  }
  return count;
}
//----------------------------------------------------------
/**
 * @brief �����Ă�����z���擾����
 * @param my    ������ԕێ����[�N�ւ̃|�C���^
 * @return  u32   �����Ă�����z
 */
//----------------------------------------------------------
u32 MISC_GetGold(const MISC * misc)
{
  return misc->gold;
}

//----------------------------------------------------------
/**
 * @brief   ������ݒ肷��
 * @param misc    ������ԕێ����[�N�ւ̃|�C���^
 * @param gold  �Z�b�g������z
 * @return  u32   ���݂̎莝�����z
 */
//----------------------------------------------------------
u32 MISC_SetGold(MISC * misc, u32 gold)
{
  if (gold > MY_GOLD_MAX) {
    gold = MY_GOLD_MAX;
  }
  misc->gold = gold;
  return misc->gold;
}

//----------------------------------------------------------
/**
 * @brief  �����𑝂₷
 * @param misc    ������ԕێ����[�N�ւ̃|�C���^
 * @param add   ��������z
 * @return  u32   ���݂̎莝�����z
 */
//----------------------------------------------------------
u32 MISC_AddGold(MISC * misc, u32 add)
{
  if (add > MY_GOLD_MAX) {
    misc->gold = MY_GOLD_MAX;
  } else {
    misc->gold += add;
  }
  if (misc->gold > MY_GOLD_MAX) {
    misc->gold = MY_GOLD_MAX;
  }
  return misc->gold;
}

//----------------------------------------------------------
/**
 * @brief    ���������炷
 * @param misc    ������ԕێ����[�N�ւ̃|�C���^
 * @param sub   �����o�����z
 * @return  u32   ���݂̎莝�����z
 */
//----------------------------------------------------------
u32 MISC_SubGold(MISC * misc, u32 sub)
{
  if (misc->gold < sub) {
    misc->gold = 0;
  } else {
    misc->gold -= sub;
  }
  return misc->gold;
}

//==================================================================
//
//
//      �T�u�C�x���g�֘A
//
//==================================================================
//----------------------------------------------------------
//----------------------------------------------------------
void MISC_SetGymVictoryInfo( int gym_id, u16 * monsnos )
{
  GF_ASSERT( gym_id < GYM_MAX );
}
