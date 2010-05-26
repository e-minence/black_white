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

#include "app/name_input.h" //NAMEIN_POKEMON�̂���
#include "field/research_team_def.h"
#include "../../../resource/research_radar/data/question_id.h" // for QUESTION_ID_xxxx

#include "system/main.h"             // for HEAPID_xxxx
#include "arc/arc_def.h"             // for ARCID_xxxx
#include "arc/script_message.naix"   // for NARC_xxxx
#include "msg/script/msg_c03r0101.h" // for msg_xxxx

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



#ifdef PM_DEBUG
//�f�o�b�O�p�N�C�Y�ԍ�
int DebugQuizNo = 0;
//�f�o�b�O�e���r�p�ԑg�ԍ�
u32 DebugTvNo = 0;
#endif

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
	
	p_msc->palpark_highscore = 0;
	
  // ����Ⴂ�f�t�H���g���b�Z�[�W
  {
    GFL_MSGDATA* msg;
    STRBUF* strbuf;

    msg = GFL_MSG_Create( 
        GFL_MSG_LOAD_NORMAL, ARCID_SCRIPT_MESSAGE,
        NARC_script_message_c03r0101_dat, GetHeapLowID(HEAPID_SAVE) );

    // ���A
    strbuf = GFL_MSG_CreateString( msg, msg_c03r0101_input_a );
    GFL_STR_GetStringCode( 
        strbuf, p_msc->self_introduction, SAVE_SURETIGAI_SELFINTRODUCTION_LEN );
    GFL_STR_DeleteBuffer( strbuf );

    // ����
    strbuf = GFL_MSG_CreateString( msg, msg_c03r0101_input_b );
    GFL_STR_GetStringCode( 
        strbuf, p_msc->thankyou_message, SAVE_SURETIGAI_THANKYOU_LEN );
    GFL_STR_DeleteBuffer( strbuf );

    GFL_MSG_Delete( msg );
  }

  p_msc->research_team_rank = RESEARCH_TEAM_RANK_0;
  p_msc->research_request_id = RESEARCH_REQ_ID_NONE;
  p_msc->research_question_id[0] = QUESTION_ID_DUMMY;
  p_msc->research_question_id[1] = QUESTION_ID_DUMMY;
  p_msc->research_question_id[2] = QUESTION_ID_DUMMY;
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
void MISC_SetNameInMode( MISC *p_misc, u32 mode, u8 input_type )
{	
  GF_ASSERT( mode < NAMEIN_SAVEMODE_MAX );
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
u8 MISC_GetNameInMode( const MISC *cp_misc, u32 mode )
{	
  GF_ASSERT( mode < NAMEIN_SAVEMODE_MAX );
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
  GFL_STD_MemClear( p_misc->namein_mode, sizeof(u8)*NAMEIN_SAVEMODE_MAX );
	p_misc->namein_mode[ NAMEIN_POKEMON ] = 1;  //�|�P���������J�^�J�i�J�n
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
 * @param   sex       ����
 */
//--------------------------------------------------------------
void MISC_SetFavoriteMonsno(MISC * misc, int monsno, int form_no, int egg_flag, int sex )
{
  //OS_TPrintf( "���C�ɓ���|�P������%d�Ńt�H������%d�ŁA���ʂ� %d�A�^�}�S�t���O%d�ł�\n", monsno, form_no, egg_flag );
	misc->favorite_monsno = monsno;
	misc->favorite_form_no = form_no;
	misc->favorite_egg_flag = egg_flag;
  misc->favorite_sex  = (sex == PTL_SEX_FEMALE);  //�|�P�����A�C�R���p�̐��ʂȂ̂Ł��̂Ƃ��̂�ON
}

//--------------------------------------------------------------
/**
 * @brief   ���C�ɓ���|�P�����擾
 *
 * @param   misc		
 * @param   monsno		�|�P�����ԍ�(���C�ɓ����ݒ肵�Ă��Ȃ��ꍇ��0)
 * @param   form_no		�t�H�����ԍ�
 * @param   egg_flag	�^�}�S�t���O
 * @param   p_sex     ����
 */
//--------------------------------------------------------------
void MISC_GetFavoriteMonsno(const MISC * misc, int *monsno, int *form_no, int *egg_flag, int *p_sex )
{
	*monsno = misc->favorite_monsno;
	*form_no = misc->favorite_form_no;
	*egg_flag = misc->favorite_egg_flag;
  *p_sex    = misc->favorite_sex;
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
 * ����Ⴂ�ʐM�F���������ꂽ�񐔂��Z�b�g
 *
 * @param   misc		
 */
//==================================================================
void MISC_CrossComm_SetThanksRecvCount(MISC *misc, u32 count )
{
  if(misc->thanks_recv_count <= CROSS_COMM_THANKS_RECV_COUNT_MAX){
    misc->thanks_recv_count = count;
  }
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
 * ����Ⴂ�����l�����Z�b�g
 *
 * @param   misc		
 */
//==================================================================
void MISC_CrossComm_SetSuretigaiCount( MISC *misc, u32 count )
{
  if(misc->suretigai_count <= CROSS_COMM_SURETIGAI_COUNT_MAX){
    misc->suretigai_count = count;
  }
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

//==================================================================
/**
 * ����Ⴂ�ʐM�̒��������������N���Q�b�g
 *
 * @param   misc		
 * 
 * @retval  RESEARCH_TEAM_RANK_xxx
 */
//==================================================================
int MISC_CrossComm_GetResearchTeamRank(const MISC *misc)
{
  return misc->research_team_rank;
}

//==================================================================
/**
 * ����Ⴂ�ʐM�̒��������������N���Z�b�g
 *
 * @param   misc		
 * @param   rank		RESEARCH_TEAM_RANK_xxx
 */
//==================================================================
void MISC_CrossComm_SetResearchTeamRank(MISC *misc, int rank)
{
  misc->research_team_rank = rank;
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
//
//==================================================================
//----------------------------------------------------------
//----------------------------------------------------------
void MISC_SetGymVictoryInfo( MISC * misc, int gym_id, const u16 * monsnos )
{
  int i;
  GF_ASSERT( gym_id < GYM_MAX );
  for (i = 0; i < TEMOTI_POKEMAX; i++)
  {
    misc->gym_win_monsno[gym_id][i] = monsnos[i];
  }
}

//----------------------------------------------------------
//----------------------------------------------------------
void MISC_GetGymVictoryInfo( const MISC * misc, int gym_id, u16 * monsnos )
{
  int i;
  GF_ASSERT( gym_id < GYM_MAX );
  for (i = 0; i < TEMOTI_POKEMAX; i++)
  {
    monsnos[i] = misc->gym_win_monsno[gym_id][i];
  }
}


//==============================================================================
//  �^�C�g�����j���[
//==============================================================================

//----------------------------------------------------------
/**
 * @brief		�t���O�Z�b�g
 * @param		misc
 * @param		type		���j���[�^�C�v
 * @param		flg			�t���O
 * @return  
 */
//----------------------------------------------------------
void MISC_SetStartMenuFlag( MISC * misc, MISC_STARTMENU_TYPE type, MISC_STARTMENU_FLAG flg )
{
	misc->start_menu_open |= ( flg << (type<<1) );
}

//----------------------------------------------------------
/**
 * @brief		�t���O�擾
 * @param		misc
 * @param		type		���j���[�^�C�v
 * @retval	�t���O
 */
//----------------------------------------------------------
u8 MISC_GetStartMenuFlag( const MISC * misc, MISC_STARTMENU_TYPE type )
{
	return ( ( misc->start_menu_open >> (type<<1) ) & (MISC_STARTMENU_FLAG_OPEN|MISC_STARTMENU_FLAG_VIEW) );
}

//----------------------------------------------------------
/**
 * @brief		�t���O�擾�i�S�́j
 * @param		misc
 * @retval	�t���O
 */
//----------------------------------------------------------
u8 MISC_GetStartMenuFlagAll( const MISC * misc )
{
	return misc->start_menu_open;
}

//----------------------------------------------------------
/**
 * @brief		�t���O�Z�b�g�i�S�́j
 * @param		misc
 * @param		flg			�t���O
 * @return  
 */
//----------------------------------------------------------
void MISC_SetStartMenuFlagAll( MISC * misc, u8 flg )
{
	misc->start_menu_open = flg;
}


//==============================================================================
// ����Ⴂ������
//==============================================================================
//----------------------------------------------------------
/**
 * @brief �󂯂Ă��钲���˗�ID���擾����
 *
 * @param misc
 *
 * @return �����˗�ID ( RESEARCH_REQ_ID_xxxx )
 */
//----------------------------------------------------------
u8 MISC_GetResearchRequestID( const MISC* misc )
{
  return misc->research_request_id;
}

//----------------------------------------------------------
/**
 * @brief �������Ƃ��Ē��ׂĂ��钲���˗�ID���Z�b�g����
 *
 * @param misc
 * @param id  �Z�b�g���钲���˗�ID ( RESEARCH_REQ_ID_xxxx )
 */
//----------------------------------------------------------
void MISC_SetResearchRequestID( MISC* misc, u8 id )
{
  GF_ASSERT( id <= RESEARCH_REQ_ID_MAX );
  misc->research_request_id = id;
}

//----------------------------------------------------------
/**
 * @brief �������Ƃ��Ē��ׂĂ��鎿��ID���擾����
 *
 * @param misc
 * @param idx  �擾����C���f�b�N�X
 *
 * @return ����ID ( QUESTION_ID_xxxx )
 */
//----------------------------------------------------------
u8 MISC_GetResearchQuestionID( const MISC* misc, u8 idx )
{
  GF_ASSERT( idx < MAX_QNUM_PER_RESEARCH_REQ );
  return misc->research_question_id[ idx ];
}

//----------------------------------------------------------
/**
 * @brief �������Ƃ��Ē��ׂĂ��鎿��ID���Z�b�g����
 *
 * @param misc
 * @param idx �Z�b�g����C���f�b�N�X
 * @param id  �Z�b�g���钲���˗�ID ( QUESTION_ID_xxxx )
 */
//----------------------------------------------------------
void MISC_SetResearchQuestionID( MISC* misc, u8 idx, u8 id )
{
  GF_ASSERT( idx < MAX_QNUM_PER_RESEARCH_REQ );
  GF_ASSERT( (id == QUESTION_ID_DUMMY) || (id <= QUESTION_ID_MAX) );
  misc->research_question_id[ idx ] = id;
}
//----------------------------------------------------------
/**
 * @brief �������Ƃ��Ē������̎���ɂ���,
 *        �������J�n���Ă���̒����l�����擾����
 *
 * @param misc
 * @param idx  �����˗����̎���C���f�b�N�X ( 0 �` MAX_QNUM_PER_RESEARCH_REQ-1 )
 *
 * @return �J�n����̒����l��
 */
//----------------------------------------------------------
u16 MISC_GetResearchCount( const MISC* misc, u8 idx )
{
  GF_ASSERT( idx < MAX_QNUM_PER_RESEARCH_REQ );
  return misc->research_count[ idx ];
}

//----------------------------------------------------------
/**
 * @brief �������Ƃ��Ē������̎���ɂ���,
 *        �������J�n���Ă���̒����l�����Z�b�g����
 *
 * @param misc
 * @param idx   �����˗����̎���C���f�b�N�X ( 0 �` MAX_QNUM_PER_RESEARCH_REQ-1 ) 
 * @param count �Z�b�g����l��
 */
//----------------------------------------------------------
void MISC_SetResearchCount( MISC* misc, u8 idx, u16 count )
{
  GF_ASSERT( idx < MAX_QNUM_PER_RESEARCH_REQ );
  misc->research_count[idx] = count;
}

//----------------------------------------------------------
/**
 * @brief �������Ƃ��Ē������̎���ɂ���,
 *        �������J�n���Ă���̒����l�������Z����
 *
 * @param misc
 * @param idx   �����˗����̎���C���f�b�N�X ( 0 �` MAX_QNUM_PER_RESEARCH_REQ-1 ) 
 * @param count ���Z�l��
 */
//----------------------------------------------------------
void MISC_AddResearchCount( MISC* misc, u8 idx, u16 add )
{
  u32 now, sum;

  GF_ASSERT( idx < MAX_QNUM_PER_RESEARCH_REQ );

  now = misc->research_count[idx];
  sum = now + add; 
  if( 0xffff < sum ) { sum = 0xffff; }

  misc->research_count[idx] = sum;
}

//----------------------------------------------------------
/**
 * @brief �����˗����󂯂����̎���[�b]���擾����
 *
 * @param misc
 *
 * @return �����J�n���_�ł̎���[�b]
 */
//----------------------------------------------------------
s64 MISC_GetResearchStartTimeBySecond( const MISC* misc )
{
  return misc->research_start_time;
}

//----------------------------------------------------------
/**
 * @brief �����˗����󂯂����̎���[�b]���Z�b�g����
 *
 * @param misc
 * @param time �����J�n���_�̎���[�b]
 */
//----------------------------------------------------------
void MISC_SetResearchStartTimeBySecond( MISC* misc, s64 time )
{
  misc->research_start_time = time;
}

//----------------------------------------------------------
/**
 * @brief �C�x���g���b�N�l���Z�b�g
 *
 * @param misc
 * @param inNo    �C�x���g�i���o�[
 * @param inValue �l
 */
//----------------------------------------------------------
void MISC_SetEvtRockValue( MISC* misc, const int inNo, const u32 inValue )
{
  if ( inNo < EVT_ROCK_MAX ) misc->event_lock[inNo] = inValue;
}

//----------------------------------------------------------
/**
 * @brief �C�x���g���b�N�l���擾
 *
 * @param misc
 * @param inNo    �C�x���g�i���o�[
 * @return u32    �l
 */
//----------------------------------------------------------
u32 MISC_GetEvtRockValue( MISC* misc, const int inNo )
{
  if (inNo >= EVT_ROCK_MAX)
  {
    GF_ASSERT_MSG(0,"No Error");
    return 0;
  }

  return misc->event_lock[inNo];
}

//----------------------------------------------------------
/**
 * @brief �`�����s�I���j���[�X�̎c�莞�Ԃ��擾����
 *
 * @param  misc
 *
 * @return �d���f���Ń`�����v�j���[�X�𗬂��c�莞��[min]
 */
//----------------------------------------------------------
u16 MISC_GetChampNewsMinutes( const MISC* misc )
{
  return misc->champ_news_minutes;
}

//----------------------------------------------------------
/**
 * @brief �`�����s�I���j���[�X�̎c�莞�Ԃ�ݒ肷��
 *
 * @param misc
 * @param minutes �c�莞��[min]
 */
//----------------------------------------------------------
void MISC_SetChampNewsMinutes( MISC* misc, u16 minutes )
{
  misc->champ_news_minutes = minutes;
}

//----------------------------------------------------------
/**
 * @brief �`�����s�I���j���[�X�̎c�莞�Ԃ��f�N�������g����
 *
 * @param misc
 * @param minutes �o�ߎ���[min]
 */
//----------------------------------------------------------
void MISC_DecChampNewsMinutes( MISC* misc, u16 minutes )
{
  if( minutes <= misc->champ_news_minutes ) {
    misc->champ_news_minutes -= minutes;
  }
  else {
    misc->champ_news_minutes = 0;
  }
}
//==============================================================================
//  �o�g�����R�[�_�[
//==============================================================================
//----------------------------------------------------------------------------
/** 
 *	@brief  �o�g�����R�[�_�[�̐F���擾
 *
 *	@param	const MISC * misc   �~�X�N
 *
 *	@return �o�g�����R�[�_�[�̐F
 */
//-----------------------------------------------------------------------------
u8 MISC_GetBattleRecorderColor( const MISC * misc )
{ 
  return misc->battle_recorder_flag;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �o�g�����R�[�_�[�̐F��ݒ�
 *
 *	@param	MISC * misc �~�X�N
 *	@param	color       �F�inet_app/battle_recorder/br_res.h�ɂ���BR_RES_COLOR_TYPE�񋓁j
 */
//-----------------------------------------------------------------------------
void MISC_SetBattleRecorderColor( MISC * misc, u8 color )
{ 
  misc->battle_recorder_flag  = color;
}
