//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		irc_compatible_savedata.c
 *	@brief	�ԊO�������`�F�b�N�p�Z�[�u�f�[�^
 *	@author	Toru=Nagihashi
 *	@data		2009.07.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include <gflib.h>
#include "system/main.h"
#include "print/str_tool.h"
#include "buflen.h"
#include "savedata/save_tbl.h"

#include "savedata/irc_compatible_savedata.h"

#include "system/rtc_tool.h"  //�o�C�I���Y���̂���

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
#define IRC_COMPATIBLE_SV_EXIST_NONE		(0xFF)		//�����N�ɂ��Ȃ�

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�����`�F�b�N�����L���O
//=====================================
typedef struct 
{
	STRCODE name[IRC_COMPATIBLE_SV_DATA_NAME_LEN];		// 16
	u32			score			:7;													//	4	
	u32			play_cnt	:10;
  u32     sex       : 1;
  u32     birth_month:  4;
  u32     birth_day :  4;
	u32			dummy			:6;
	u32			trainerID;														// 4
  u16     mons_no;
  u8      form_no;
  u8      mons_sex  : 6;
  u8      egg_flag  : 1;
  u8      only_day  : 1;
}IRC_COMPATIBLE_RANKING_DATA;

//-------------------------------------
///	�����`�F�b�N�p�Z�[�u�f�[�^
//=====================================
struct _IRC_COMPATIBLE_SAVEDATA
{	
	IRC_COMPATIBLE_RANKING_DATA	rank[IRC_COMPATIBLE_SV_RANKING_MAX];
	u32	new_rank		:8;
  u32 dummy       :24;
};
//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
static u8 Irc_Compatible_SV_InsertRanking( IRC_COMPATIBLE_RANKING_DATA *p_rank, u16 len, const IRC_COMPATIBLE_RANKING_DATA *cp_new );
static BOOL Irc_Compatible_SV_IsExits( const IRC_COMPATIBLE_RANKING_DATA *cp_rank, u16 len, const IRC_COMPATIBLE_RANKING_DATA *cp_new );
static void Irc_Compatible_SV_SetData( IRC_COMPATIBLE_RANKING_DATA *p_data, const STRCODE *cp_name, u8 score, u16 play_cnt, u8 sex, u8 birth_month, u8 birth_day, u32 trainerID, u16 mons_no, u8 form_no, u8 mons_sex, u8 egg );
static u16 Irc_Compatible_SV_GetPlayCount( const IRC_COMPATIBLE_RANKING_DATA *cp_rank, u16 len, u32 trainerID );
static void Irc_Compatible_SV_AddPlayCount( IRC_COMPATIBLE_RANKING_DATA *p_rank, u16 len, const IRC_COMPATIBLE_RANKING_DATA *cp_new );
//=============================================================================
/**
 *	�Z�[�u�f�[�^�擾�擾
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�Z�[�u�f�[�^�擾
 *
 *	@param	SAVE_CONTROL_WORK* p_sv_ctrl	�Z�[�uControl
 *
 *	@return	�����`�F�b�N�p���[�ԂŁ[��
 */
//-----------------------------------------------------------------------------
IRC_COMPATIBLE_SAVEDATA * IRC_COMPATIBLE_SV_GetSavedata( SAVE_CONTROL_WORK* p_sv_ctrl )
{	
	return SaveControl_DataPtrGet( p_sv_ctrl, GMDATA_ID_IRCCOMPATIBLE );
}

//=============================================================================
/**
 *	�f�[�^�擾�֐�
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�����N�C�����Ă���l�̐����擾
 *
 *	@param	const IRC_COMPATIBLE_SAVEDATA *cp_sv	�Z�[�u
 *
 *	@return	�����N�C����
 */
//-----------------------------------------------------------------------------
u32 IRC_COMPATIBLE_SV_GetRankNum( const IRC_COMPATIBLE_SAVEDATA *cp_sv )
{	
	int i;

	//�����N�C���̐��̓Z�[�u���Ă��炸�Aplay_cnt�����Čv�Z����
	for( i = 0; i < IRC_COMPATIBLE_SV_RANKING_MAX; i++ )
	{	
		if( cp_sv->rank[i].play_cnt == 0 )
		{	
			break;
		}
	}

	return i;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�v���C���[�����擾
 *
 *	@param	const IRC_COMPATIBLE_SAVEDATA *cp_sv	�Z�[�u
 *	@param	rank																	����
 *	@retval	STRCODE
 */
//-----------------------------------------------------------------------------
const STRCODE* IRC_COMPATIBLE_SV_GetPlayerName( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank )
{	
	GF_ASSERT( rank < IRC_COMPATIBLE_SV_RANKING_MAX );
  return cp_sv->rank[ rank ].name;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�X�R�A���擾
 *
 *	@param	const IRC_COMPATIBLE_SAVEDATA *cp_sv	�Z�[�u
 *	@param	rank																	����
 *
 *	@return	�X�R�A
 */
//-----------------------------------------------------------------------------
u8	IRC_COMPATIBLE_SV_GetScore( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank )
{	
	GF_ASSERT( rank < IRC_COMPATIBLE_SV_RANKING_MAX );
	return cp_sv->rank[ rank ].score;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�v���C�񐔂��擾
 *
 *	@param	const IRC_COMPATIBLE_SAVEDATA *cp_sv	�Z�[�u
 *	@param	rank																	����
 *
 *	@return	�v���C��
 */
//-----------------------------------------------------------------------------
u16 IRC_COMPATIBLE_SV_GetPlayCount( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank )
{	
	GF_ASSERT( rank < IRC_COMPATIBLE_SV_RANKING_MAX );
	return cp_sv->rank[ rank ].play_cnt;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���ʎ擾
 *
 *	@param	const IRC_COMPATIBLE_SAVEDATA *cp_sv  �Z�[�u
 *	@param  rank                                  ����
 *
 *	@return ����
 */
//-----------------------------------------------------------------------------
u32 IRC_COMPATIBLE_SV_GetSex( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank )
{ 
	GF_ASSERT( rank < IRC_COMPATIBLE_SV_RANKING_MAX );
	return cp_sv->rank[ rank ].sex;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �a�����擾
 *
 *	@param	const IRC_COMPATIBLE_SAVEDATA *cp_sv  �Z�[�u
 *	@param  rank                                  ����
 *
 *	@return �a����
 */
//-----------------------------------------------------------------------------
u32 IRC_COMPATIBLE_SV_GetBirthMonth( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank )
{ 
	GF_ASSERT( rank < IRC_COMPATIBLE_SV_RANKING_MAX );
	return cp_sv->rank[ rank ].birth_month;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �a�����擾
 *
 *	@param	const IRC_COMPATIBLE_SAVEDATA *cp_sv  �Z�[�u
 *	@param  rank                                  ����
 *
 *	@return �a����
 */
//-----------------------------------------------------------------------------
u32 IRC_COMPATIBLE_SV_GetBirthDay( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank )
{ 
	GF_ASSERT( rank < IRC_COMPATIBLE_SV_RANKING_MAX );
	return cp_sv->rank[ rank ].birth_day;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �|�P���������擾
 *
 *	@param	const IRC_COMPATIBLE_SAVEDATA *cp_sv  �Z�[�u
 *	@param	rank                                  ����
 *	@param	*p_mons_no          �����X�^�[�ԍ�
 *	@param	*p_form_no          �t�H�����ԍ�
 *	@param	*p_mons_sex         ����
 *	@param	*p_egg              �^�}�S�t���O
 */
//-----------------------------------------------------------------------------
void IRC_COMPATIBLE_SV_GetPokeData( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank, u16 *p_mons_no, u8 *p_form_no, u8 *p_mons_sex, u8 *p_egg )
{ 
	GF_ASSERT( rank < IRC_COMPATIBLE_SV_RANKING_MAX );
	*p_mons_no  = cp_sv->rank[ rank ].mons_no;
  *p_form_no  = cp_sv->rank[ rank ].form_no;
  *p_mons_sex = cp_sv->rank[ rank ].mons_sex;
  *p_egg      = cp_sv->rank[ rank ].egg_flag;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�V�K���擾
 *
 *	@param	const IRC_COMPATIBLE_SAVEDATA *cp_sv	�Z�[�u
 *
 *	@return	�V�K����
 */
//-----------------------------------------------------------------------------
u8	IRC_COMPATIBLE_SV_GetNewRank( const IRC_COMPATIBLE_SAVEDATA *cp_sv )
{	
	return cp_sv->new_rank;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �o�C�I���Y�����擾
 *
 *	@param	const IRC_COMPATIBLE_SAVEDATA *cp_sv  �Z�[�u
 *	@param	rank                                  ����
 *	@param  cp_now_date Irc_Compatible_SV_CalcBioRhythm�̐������Q��
 *
 *	@return 0�`100�̒l
 */
//-----------------------------------------------------------------------------
u8 IRC_COMPATIBLE_SV_GetBioRhythm( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank, const RTCDate * cp_now_date )
{ 
  u8 month, day;

  GF_ASSERT( rank < IRC_COMPATIBLE_SV_RANKING_MAX );

  month  = IRC_COMPATIBLE_SV_GetBirthMonth( cp_sv, rank );
  day    = IRC_COMPATIBLE_SV_GetBirthDay( cp_sv, rank ); 
  return Irc_Compatible_SV_CalcBioRhythm( month, day, cp_now_date );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ��Ԏ����ƃo�C�I���Y���������Ă���l���擾
 *
 *	@param	const IRC_COMPATIBLE_SAVEDATA *cp_sv  �Z�[�u
 *	@param	player_birth_month                    �v���C���[�̒a����
 *	@param	player_birth_day                      �v���C���[�̒a����
 *	@param  cp_now_date Irc_Compatible_SV_CalcBioRhythm�̐������Q��
 *
 *	@return ��Ԏ����ƃo�C�I���Y���������Ă���l�̏���
 */
//-----------------------------------------------------------------------------
u32 IRC_COMPATIBLE_SV_GetBestBioRhythm( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u8 player_birth_month, u8 player_birth_day, const RTCDate * cp_now_date )
{ 
  const u32 max         = IRC_COMPATIBLE_SV_GetRankNum( cp_sv );
  const u32 player_bio  = Irc_Compatible_SV_CalcBioRhythm( player_birth_month, player_birth_day, cp_now_date );

  int i;
  u32 best_idx  = 0;
  u32 best_dif  = 0xFFFFFFFF;
  u32 dif;

  for( i = 0; i < max; i++ )
  { 
    dif = MATH_IAbs( (s32)IRC_COMPATIBLE_SV_GetBioRhythm( cp_sv, i, cp_now_date ) - (s32)player_bio );

    if( best_dif > dif )
    { 
      best_idx  = i;
      best_dif  = dif;
    }
  }

  return best_idx;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �P���P��t���O���擾
 *
 *	@param	const IRC_COMPATIBLE_SAVEDATA *cp_sv  ���[�N
 *	@param	trainerID                             �`�F�b�N����g���[�i�[ID
 *
 *	@return TRUE�Ȃ�΍�����x�v���C���Ă���  FALSE�Ȃ�Ζ{���n�߂ăv���C
 */
//-----------------------------------------------------------------------------
BOOL IRC_COMPATIBLE_SV_IsDayFlag( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 trainerID )
{ 
  int i;

	for( i = 0; i < IRC_COMPATIBLE_SV_RANKING_MAX; i++ )
	{	
		if( cp_sv->rank[i].trainerID == trainerID && 
        cp_sv->rank[i].play_cnt > 0 )
		{
		  return cp_sv->rank[i].only_day;
		}
	}
	return FALSE;

}
//----------------------------------------------------------------------------
/**
 *	@brief  �v���C�������Ƃ����邩�`�F�b�N
 *
 *	@param	const IRC_COMPATIBLE_SAVEDATA *cp_sv  ���[�N
 *	@param	trainerID                           �g���[�i�[ID
 *
 *	@return TRUE�P��ł��v���C�������Ƃ�����  FALSE�Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL IRC_COMPATIBLE_SV_IsPlayed( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 trainerID )
{ 
  int i;

	for( i = 0; i < IRC_COMPATIBLE_SV_RANKING_MAX; i++ )
	{	
		if( cp_sv->rank[i].trainerID == trainerID && 
        cp_sv->rank[i].play_cnt > 0 )
		{
		  return TRUE;
		}
	}
	return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �o�C�I���Y�����v�Z
 *
 *	@param	month ��
 *	@param	day   ��
 *	@param  cp_now_date �C�x���g�p������DS�������̗�����n����悤��
 *	                    �C�x���g���Ŏg���Ƃ���ev_time.h��RTC��
 *	                    �C�x���g�O�Ŏg���Ƃ���GFL_RTC_GetDate���O�Ŏg�p���ēn��
 *
 *	@return 0�`100�̒l
 */
//-----------------------------------------------------------------------------
u8 Irc_Compatible_SV_CalcBioRhythm( u8 birth_month, u8 birth_day, const RTCDate * cp_now_date )
{ 
  enum
  { 
    BIORHYTHM_CYCLE = 30,  //����
  };
 
  u32 days;
  u32 now_days; //�����̓��t�������
  u32 days_diff;
  fx32 sin;
  u32 bio;

  //�����܂ł̑��������v�Z�i�N�����Ȃ��̂ŁA��N�����Ƃ���j
  { 
    RTCDate date  = *cp_now_date;
    now_days  = GFL_RTC_GetDaysOffset(&date);
    OS_TFPrintf( 0, "���݂̓����@%d�N%d��%d��=[%d]\n", date.year, date.month, date.day, now_days );

    date.month  = birth_month;
    date.day    = birth_day;
    days        = GFL_RTC_GetDaysOffset(&date);
    OS_TFPrintf( 0, "�����̒a�����@%d�N%d��%d��=[%d]\n", date.year, date.month, date.day, days );
  }

  //�a�������獡���܂ŉ����������Ă��邩
  if( now_days >= days  )
  { 
    days  += 365;
  }
  days_diff = days - now_days;

  days_diff %= BIORHYTHM_CYCLE;

  sin = FX_SinIdx( 0xFFFF * days_diff / BIORHYTHM_CYCLE );

  bio = ((sin + FX32_ONE) * 50 ) >> FX32_SHIFT;


  OS_TFPrintf( 0, "�o�C�I���Y�� %d �a���o��%d ����%d,��%d\n", bio, days, now_days, days_diff );

  return bio;
}


//=============================================================================
/**
 *	�f�[�^�ݒ�֐�
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�����L���O�Ƀf�[�^���Z�b�g
 *
 *	@param	IRC_COMPATIBLE_SAVEDATA *p_sv	�Z�[�u
 *	@param	STRCODE *cp_name							���O
 *	@param	score													�_���i�O�`�P�O�O�j
 *	@param	trainerID											ID
 *
 *	@return	����	or �����L���O�ɓ���Ȃ������ꍇIRC_COMPATIBLE_SV_ADD_NONE
 */
//-----------------------------------------------------------------------------
u8 IRC_COMPATIBLE_SV_AddRanking( IRC_COMPATIBLE_SAVEDATA *p_sv, const STRCODE *cp_name, u8 score, u8 sex, u8 birth_month, u8 birth_day, u32 trainerID, u16 mons_no, u8 form_no, u8 mons_sex, u8 egg )
{	
	IRC_COMPATIBLE_RANKING_DATA	new_data;
	u16 play_cnt;

	//�񐔎擾
	play_cnt	= Irc_Compatible_SV_GetPlayCount( p_sv->rank, IRC_COMPATIBLE_SV_RANKING_MAX, trainerID );

	//�f�[�^�쐬
	Irc_Compatible_SV_SetData( &new_data, cp_name, score, play_cnt, sex, birth_month, birth_day, trainerID, mons_no, form_no, mons_sex, egg );

	//�f�[�^�}��
	p_sv->new_rank	= Irc_Compatible_SV_InsertRanking( p_sv->rank, IRC_COMPATIBLE_SV_RANKING_MAX, &new_data );

	//�f�[�^����new�Ɠ������̂���������A�J�E���g�A�b�v
	//�i��L�ł��ꂽ�f�[�^���J�E���g�A�b�v����܂��j
	if( Irc_Compatible_SV_IsExits( p_sv->rank, IRC_COMPATIBLE_SV_RANKING_MAX, &new_data ) )
	{	
		Irc_Compatible_SV_AddPlayCount( p_sv->rank, IRC_COMPATIBLE_SV_RANKING_MAX, &new_data );
	}

	return p_sv->new_rank;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ������t���O������
 *
 *	@param	IRC_COMPATIBLE_SAVEDATA *p_sv ���[�N
 */
//-----------------------------------------------------------------------------
void IRC_COMPATIBLE_SV_ClearDayFlag( IRC_COMPATIBLE_SAVEDATA *p_sv )
{ 
	int i;

	for( i = 0; i < IRC_COMPATIBLE_SV_RANKING_MAX; i++ )
	{	
	  p_sv->rank[i].only_day  = 0;
	}
}

//=============================================================================
/**
 *	�Z�[�u�f�[�^�V�X�e���Ŏg�p����֐�
 */
//=============================================================================

//----------------------------------------------------------------------------
/**
 *	@brief	�Z�[�u�f�[�^�̈�̃T�C�Y
 *
 *	@param	void 
 *
 *	@return	�T�C�Y
 */
//-----------------------------------------------------------------------------
int IRC_COMPATIBLE_SV_GetWorkSize( void )
{	
	return sizeof(IRC_COMPATIBLE_SAVEDATA);
}
//----------------------------------------------------------------------------
/**
 *	@brief	�Z�[�u�f�[�^������
 *
 *	@param	IRC_COMPATIBLE_SAVEDATA *p_sv		���[�N
 *
 */
//-----------------------------------------------------------------------------
void IRC_COMPATIBLE_SV_Init( IRC_COMPATIBLE_SAVEDATA *p_sv )
{	
	GFL_STD_MemClear( p_sv, sizeof(IRC_COMPATIBLE_SAVEDATA) );
	p_sv->new_rank	= IRC_COMPATIBLE_SV_NEW_NONE;
}
//=============================================================================
/**
 *	PRIVATE
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�����L���O�f�[�^��}������
 *
 *	@param	IRC_COMPATIBLE_RANKING_DATA *p_rank		�}�����郉���L���O�z��
 *	@param	len																�v�f��
 *	@param	IRC_COMPATIBLE_RANKING_DATA *cp_new�@	�}������f�[�^
 *
 *	@return	�}�������ꏊ�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
static u8 Irc_Compatible_SV_InsertRanking( IRC_COMPATIBLE_RANKING_DATA *p_rank, u16 len, const IRC_COMPATIBLE_RANKING_DATA *cp_new )
{
	int i, j;
	int insert	= -1;
	IRC_COMPATIBLE_SAVEDATA temp;

	//�}���ӏ����`�F�b�N
	for( i = 0; i < len; i++ )
	{	
		if( p_rank[i].score < cp_new->score )
		{	
			insert	= i;
			break;
		}
	}

	//�}���ł��Ȃ��`�F�b�N
	if( insert == -1 )
	{	
		return IRC_COMPATIBLE_SV_ADD_NONE;
	}

	//��납��1�����炷
	for( j = len - 1; j > insert ; j-- )
	{	
		//�����y���̂��߂ɗ����̈ړ��悪play_cnt�Ȃ�΁i�f�[�^�Ȃ��Ƃ݂Ȃ��āA�s��Ȃ��j
		if( p_rank[j].play_cnt != 0 ||  p_rank[j-1].play_cnt != 0 )
		{	
			GFL_STD_MemCopy( &p_rank[j-1], &p_rank[j], sizeof(IRC_COMPATIBLE_RANKING_DATA) );
		}
	}

	//�}��
	GFL_STD_MemCopy( cp_new, &p_rank[insert], sizeof(IRC_COMPATIBLE_RANKING_DATA) );
	return insert;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����l��������x��������`�F�b�N����
 *
 *	@param	const IRC_COMPATIBLE_SAVEDATA *cp_rank	�`�F�b�N���郉���L���O�z��
 *	@param	len																			�v�f��
 *	@param	IRC_COMPATIBLE_SAVEDATA *cp_new					�`�F�b�N����f�[�^
 *
 *	@return	���ʃC���f�b�N�X�@�܂��� ���Ȃ��Ȃ��IRC_COMPATIBLE_SV_EXIST_NONE
 */
//-----------------------------------------------------------------------------
static BOOL Irc_Compatible_SV_IsExits( const IRC_COMPATIBLE_RANKING_DATA *cp_rank, u16 len, const IRC_COMPATIBLE_RANKING_DATA *cp_new )
{	
	int i;

	for( i = 0; i < len; i++ )
	{	
		if( cp_rank[i].trainerID == cp_new->trainerID &&
        cp_rank[i].play_cnt > 0 )
		{
			return TRUE;
		}
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�f�[�^��ݒ�
 *
 *	@param	IRC_COMPATIBLE_SAVEDATA *p_data	���[�N
 *	@param	STRCODE *cp_name								���O
 *	@param	score														���_
 *	@param  sex                             ����  1���j 0����
 *	@param	trainerID												ID
 *
 */
//-----------------------------------------------------------------------------
static void Irc_Compatible_SV_SetData( IRC_COMPATIBLE_RANKING_DATA *p_data, const STRCODE *cp_name, u8 score, u16 play_cnt, u8 sex, u8 birth_month, u8 birth_day, u32 trainerID, u16 mons_no, u8 form_no, u8 mons_sex, u8 egg )
{	
	GF_ASSERT( score <= IRC_COMPATIBLE_SV_DATA_SCORE_MAX );

	//�N���A
	GFL_STD_MemClear( p_data, sizeof(IRC_COMPATIBLE_RANKING_DATA) );
	//������
  STRTOOL_Copy( cp_name, p_data->name, IRC_COMPATIBLE_SV_DATA_NAME_LEN );
	p_data->score			  = score;
	p_data->trainerID	  = trainerID;
  p_data->sex         = sex;
  p_data->birth_month = birth_month;
  p_data->birth_day   = birth_day;
	p_data->play_cnt	  = play_cnt;
  p_data->mons_no     = mons_no;
  p_data->form_no     = form_no;
  p_data->mons_sex    = mons_sex;
  p_data->egg_flag    = egg & 0x1;
  p_data->only_day    = 1;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�v���C�񐔂��擾
 *
 *	@param	const IRC_COMPATIBLE_RANKING_DATA *cp_rank	���[�N
 *	@param	len																					����
 *	@param	trainerID																		ID
 *
 *	@retval	�v���C��
 *
 */
//-----------------------------------------------------------------------------
static u16 Irc_Compatible_SV_GetPlayCount( const IRC_COMPATIBLE_RANKING_DATA *cp_rank, u16 len, u32 trainerID )
{	
	int i;

	for( i = 0; i < len; i++ )
	{	
		if( cp_rank[i].trainerID == trainerID )
		{
			return cp_rank[i].play_cnt;
		}
	}

	return 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	NEW�Ɠ���ID�̐l�̃����L���O�𑝂₷
 *
 *	@param	IRC_COMPATIBLE_SAVEDATA *p_rank		�����L���O�z��
 *	@param	len																�v�f��
 *	@param	IRC_COMPATIBLE_SAVEDATA *cp_new		���₷�f�[�^
 *
 */
//-----------------------------------------------------------------------------
static void Irc_Compatible_SV_AddPlayCount( IRC_COMPATIBLE_RANKING_DATA *p_rank, u16 len, const IRC_COMPATIBLE_RANKING_DATA *cp_new )
{	
	int i;

	for( i = 0; i < len; i++ )
	{	
		if( p_rank[i].trainerID == cp_new->trainerID )
		{
			p_rank[i].play_cnt++;
      p_rank[i].only_day  = 1;
			p_rank[i].play_cnt	= MATH_CLAMP( p_rank[i].play_cnt, 0, IRC_COMPATIBLE_SV_DATA_PLAYCNT_MAX );
		}
	}
}
