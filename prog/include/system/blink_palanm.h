//============================================================================================
/**
 * @file		blink_palanm.h
 * @brief		�_�Ńp���b�g�A�j������
 * @author	Hiroyuki Nakamura
 * @date		10.01.18
 *
 *	���W���[�����FBLINKPALANM
 *
 *	�w��J���[����w��J���[�ւ̃p���b�g�A�j���ł��B
 *	�J�[�\���A�j���Ŏg�p���Ă��܂��B
 */
//============================================================================================
#pragma	once

typedef struct _BLINK_PALANM_WORK	BLINKPALANM_WORK;		// �_�Ńp���b�g�A�j�����[�N


//--------------------------------------------------------------------------------------------
/**
 * �_�Ńp���b�g�A�j�����[�N�쐬
 *
 * @param		offs		�A�j��������p���b�g�ʒu�i�J���[�P�ʁj
 * @param		size		�A�j��������p���b�g�̃T�C�Y�i�J���[�P�ʁj
 * @param		bgfrm		�a�f�t���[��
 * @param		hapID		�q�[�v�h�c
 *
 * @return	�A�j�����[�N
 */
//--------------------------------------------------------------------------------------------
extern BLINKPALANM_WORK * BLINKPALANM_Create( u16 offs, u16 size, u16 bgfrm, HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * �p���b�g�ݒ�
 *
 * @param		wk				�p���b�g�A�j�����[�N
 * @param		arcID			�A�[�J�C�u�h�c
 * @param		datID			�f�[�^�h�c
 * @param		startPos	�J�n��p���b�g�ʒu�i�J���[�P�ʁj
 * @param		endPos		�I����p���b�g�ʒu�i�J���[�P�ʁj
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BLINKPALANM_SetPalBufferArc( BLINKPALANM_WORK * wk, ARCID arcID, ARCDATID datID, u32 startPos, u32 endPos );

//--------------------------------------------------------------------------------------------
/**
 * �p���b�g�ݒ�i�n���h���w��j
 *
 * @param		wk				�p���b�g�A�j�����[�N
 * @param		ah				�A�[�N�n���h��
 * @param		datID			�f�[�^�h�c
 * @param		startPos	�J�n��p���b�g�ʒu�i�J���[�P�ʁj
 * @param		endPos		�I����p���b�g�ʒu�i�J���[�P�ʁj
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BLINKPALANM_SetPalBufferArcHDL( BLINKPALANM_WORK * wk, ARCHANDLE * ah, ARCDATID datID, u32 startPos, u32 endPos );

//--------------------------------------------------------------------------------------------
/**
 * �_�Ńp���b�g�A�j�����[�N�폜
 *
 * @param		wk		�p���b�g�A�j�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BLINKPALANM_Exit( BLINKPALANM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �_�Ńp���b�g�A�j�����상�C��
 *
 * @param		wk		�p���b�g�A�j�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BLINKPALANM_Main( BLINKPALANM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �A�j������J�E���^�[��������
 *
 * @param		wk		�p���b�g�A�j�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BLINKPALANM_InitAnimeCount( BLINKPALANM_WORK * wk );
