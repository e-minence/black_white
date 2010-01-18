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
}IRC_COMPATIBLE_RANKING_DATA;

//-------------------------------------
///	�����`�F�b�N�p�Z�[�u�f�[�^
//=====================================
struct _IRC_COMPATIBLE_SAVEDATA
{	
	IRC_COMPATIBLE_RANKING_DATA	rank[IRC_COMPATIBLE_SV_RANKING_MAX];
	u32	new_rank		:8;
	u32 dummy				:24;
};
//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
static u8 Irc_Compatible_SV_InsertRanking( IRC_COMPATIBLE_RANKING_DATA *p_rank, u16 len, const IRC_COMPATIBLE_RANKING_DATA *cp_new );
static BOOL Irc_Compatible_SV_IsExits( const IRC_COMPATIBLE_RANKING_DATA *cp_rank, u16 len, const IRC_COMPATIBLE_RANKING_DATA *cp_new );
static void Irc_Compatible_SV_SetData( IRC_COMPATIBLE_RANKING_DATA *p_data, const STRCODE *cp_name, u8 score, u16 play_cnt, u8 sex, u8 birth_month, u8 birth_day, u32 trainerID );
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
 *	@brief  �����L���O�ɓ����Ă���j���̐l�����擾
 *
 *	@param	const IRC_COMPATIBLE_SAVEDATA *cp_sv  �Z�[�u
 *
 *	@return �j���̐l��
 */
//-----------------------------------------------------------------------------
u32 IRC_COMPATIBLE_SV_GetSex( const IRC_COMPATIBLE_SAVEDATA *cp_sv, u32 rank )
{ 
	GF_ASSERT( rank < IRC_COMPATIBLE_SV_RANKING_MAX );
	return cp_sv->rank[ rank ].sex;
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
u8 IRC_COMPATIBLE_SV_AddRanking( IRC_COMPATIBLE_SAVEDATA *p_sv, const STRCODE *cp_name, u8 score, u8 sex, u8 birth_month, u8 birth_day, u32 trainerID )
{	
	IRC_COMPATIBLE_RANKING_DATA	new_data;
	u16 play_cnt;

	//�񐔎擾
	play_cnt	= Irc_Compatible_SV_GetPlayCount( p_sv->rank, IRC_COMPATIBLE_SV_RANKING_MAX, trainerID );

	//�f�[�^�쐬
	Irc_Compatible_SV_SetData( &new_data, cp_name, score, play_cnt, sex, birth_month, birth_day, trainerID );

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
		if( cp_rank[i].trainerID == cp_new->trainerID )
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
static void Irc_Compatible_SV_SetData( IRC_COMPATIBLE_RANKING_DATA *p_data, const STRCODE *cp_name, u8 score, u16 play_cnt, u8 sex, u8 birth_month, u8 birth_day, u32 trainerID )
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
			p_rank[i].play_cnt	= MATH_CLAMP( p_rank[i].play_cnt, 0, IRC_COMPATIBLE_SV_DATA_PLAYCNT_MAX );
		}
	}
}
