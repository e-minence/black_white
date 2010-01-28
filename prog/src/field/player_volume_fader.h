/////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �v���C���[�{�����[���̃t�F�[�h�Ǘ�
 * @file   player_volume_fader.h
 * @author obata
 * @date   2010.01.27
 */
/////////////////////////////////////////////////////////////////////////////////////
#pragma once


// �V�X�e���̕s���S�^�錾
typedef struct _PLAYER_VOLUME_FADER PLAYER_VOLUME_FADER;


// �V�X�e������/�j��
extern PLAYER_VOLUME_FADER* PLAYER_VOLUME_FADER_Create( HEAPID heapID, u8 playerNo );
extern void                 PLAYER_VOLUME_FADER_Delete( PLAYER_VOLUME_FADER* fader );

// �V�X�e������
extern void PLAYER_VOLUME_FADER_Main( PLAYER_VOLUME_FADER* fader );

// �{�����[���ύX
extern void PLAYER_VOLUME_FADER_SetVolume( PLAYER_VOLUME_FADER* fader, u8 volume, u16 frames );
