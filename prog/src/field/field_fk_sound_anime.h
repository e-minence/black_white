//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_fk_sound_anime.h
 *	@brief  �l�V���@�����f���p�@�T�E���h�A�j���[�V�����V�X�e��
 *	@author	tomoya takahashi
 *	@date		2009.12.24
 *
 *	���W���[�����FFIELD_FK_SOUND_ANIME
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include <gflib.h>

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------


//-------------------------------------
///	�Ǘ�SE�ő吔
//=====================================
enum
{
  FIELD_FK_SOUND_ANIME_SE_IDX_TRANS_X,    // TRANS X�ɔ�������SE
  FIELD_FK_SOUND_ANIME_SE_IDX_TRANS_Y,    // TRANS Y�ɔ�������SE
  FIELD_FK_SOUND_ANIME_SE_IDX_TRANS_Z,    // TRANS Z�ɔ�������SE
  FIELD_FK_SOUND_ANIME_SE_IDX_SCALE_X,    // SCALE X�ɔ�������SE
  FIELD_FK_SOUND_ANIME_SE_IDX_SCALE_Y,    // SCALE Y�ɔ�������SE
  FIELD_FK_SOUND_ANIME_SE_IDX_SCALE_Z,    // SCALE Z�ɔ�������SE
  FIELD_FK_SOUND_ANIME_SE_IDX_ROTATE_X,   // ROTATE X�ɔ�������SE
  FIELD_FK_SOUND_ANIME_SE_IDX_ROTATE_Y,   // ROTATE Y�ɔ�������SE
  FIELD_FK_SOUND_ANIME_SE_IDX_ROTATE_Z,   // ROTATE Z�ɔ�������SE

  FIELD_FK_SOUND_ANIME_SE_MAX,
};


//-------------------------------------
///	SE�@NONE
//=====================================
#define FIELD_FK_SOUND_ANIME_SE_NONE  (0xffff)  // �e�[�u����SE�������Ƃ��ɐݒ肷��l


// �A�j���[�V�����X�s�[�h
#define FIELD_FK_SOUND_ANIME_SPEED (FX32_ONE)


//-------------------------------------
///	SE�A�j���[�V�����@�X�g���[�~���O�ǂݍ��݃f�t�H���g�l
//=====================================
#define FIELD_FK_SOUND_ANIME_SE_BUF_INTERVAL_SIZE ( 10 )

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�t�B�[���h�l�V���@�T�E���h�A�j���[�V����
//=====================================
typedef struct _FIELD_FK_SOUND_ANIME FIELD_FK_SOUND_ANIME;


//-------------------------------------
///	�Z�b�g�A�b�v�f�[�^
//=====================================
typedef struct 
{
  u16 se_tbl[ FIELD_FK_SOUND_ANIME_SE_MAX ];   
} FIELD_FK_SOUND_ANIME_DATA;



//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

extern FIELD_FK_SOUND_ANIME* FIELD_FK_SOUND_ANIME_Create( const FIELD_FK_SOUND_ANIME_DATA* cp_param, u32 arcID, u32 dataID, int buf_interval, HEAPID heapID );
extern void FIELD_FK_SOUND_ANIME_Delete( FIELD_FK_SOUND_ANIME* p_wk );
extern BOOL FIELD_FK_SOUND_ANIME_Update( FIELD_FK_SOUND_ANIME* p_wk );

extern BOOL FIELD_FK_SOUND_ANIME_IsAnime( const FIELD_FK_SOUND_ANIME* cp_wk );


#ifdef _cplusplus
}	// extern "C"{
#endif



