///////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �p���b�g�A�j���[�V����
 * @file   palette_anime.h
 * @author obata
 * @date   2010.03.04
 */
///////////////////////////////////////////////////////////////////////////////
#pragma once
#include <gflib.h>


//=============================================================================
// ���A�j���[�V�������s���[�N�̕s���S�^
//=============================================================================
typedef struct _PALETTE_ANIME PALETTE_ANIME;


//=============================================================================
// ���萔
//============================================================================= 
// �A�j���[�V�����^�C�v
typedef enum {
  ANIME_TYPE_SIN_FADE,      // sin �t�F�[�h
  ANIME_TYPE_SIN_FADE_FAST, // sin �t�F�[�h ( �� )
  ANIME_TYPE_SIN_FADE_SLOW, // sin �t�F�[�h ( �x )
  ANIME_TYPE_BLINK,         // �_��
  ANIME_TYPE_BLINK_SHORT,   // �_�� ( �Z )
  ANIME_TYPE_BLINK_LONG,    // �_�� ( �� )
} ANIME_TYPE;


//=============================================================================
// �������E�j��
//=============================================================================
// ����
extern PALETTE_ANIME* PALETTE_ANIME_Create( HEAPID heapID );
// �j��
extern void PALETTE_ANIME_Delete( PALETTE_ANIME* anime );
// �Z�b�g�A�b�v
extern void PALETTE_ANIME_Setup( PALETTE_ANIME* anime, u16* dest, u16* src, u8 colorNum );

//=============================================================================
// ������
//=============================================================================
// �A�j���[�V�������X�V����
extern void PALETTE_ANIME_Update( PALETTE_ANIME* anime );
// �A�j���[�V�������J�n����
extern void PALETTE_ANIME_Start( PALETTE_ANIME* anime, ANIME_TYPE type, u16 color );
// �A�j���[�V�������~����
extern void PALETTE_ANIME_Stop( PALETTE_ANIME* anime );
// ���삵���p���b�g���A�j���[�V�����J�n���_�̏�ԂɃ��Z�b�g����
extern void PALETTE_ANIME_Reset( PALETTE_ANIME* anime );
// �A�j���[�V���������ǂ������`�F�b�N����
extern BOOL PALETTE_ANIME_CheckAnime( const PALETTE_ANIME* anime );
