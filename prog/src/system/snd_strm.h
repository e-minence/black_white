//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		snd_strm.h
 *	@brief		�T�E���h	�X�g���[�~���O�Đ�
 *	@author		tomoya takahashi
 *	@data		2008.12.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __SND_STRM_H__
#define __SND_STRM_H__


//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�T�E���h�f�[�^�^�C�v
//=====================================
typedef enum {
	SND_STRM_PCM8,
	SND_STRM_PCM16,

	SND_STRM_TYPE_MAX,	// �V�X�e�����Ŏg�p
} SND_STRM_TYPE;

//-------------------------------------
///	���g���^�C�v
//=====================================
typedef enum {
	SND_STRM_8KHZ,		// 8.000KHz
	SND_STRM_11KHZ,		// 11.025KHz
	SND_STRM_22KHZ,		// 22.050KHz

	SND_STRM_HZMAX,		// �V�X�e�����Ŏg�p
} SND_STRM_HZ;



//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
// �V�X�e���̏�����
extern void SND_STRM_Init( u32 heapID );
extern void SND_STRM_Exit( void );
extern void SND_STRM_Main( void );

// ���y���̓ǂݍ��݁E�j��
extern void SND_STRM_SetUp( u32 arcid, u32 dataid, SND_STRM_TYPE type, SND_STRM_HZ hz );
extern void SND_STRM_Release( void );
extern BOOL SND_STRM_CheckSetUp( void );

// �Đ��A��~����
extern void SND_STRM_Play( void );
extern void SND_STRM_Stop( void );

#endif		// __SND_STRM_H__

