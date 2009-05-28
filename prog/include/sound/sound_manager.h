//============================================================================================
/**
 * @file	sound_manager.h
 * @brief	�T�E���h�Ǘ�
 * @author	
 * @date	
 */
//============================================================================================
#ifndef __SND_MANAGER_H__
#define __SND_MANAGER_H__

//============================================================================================
/**
 *
 *
 * @brief	�T�E���h�Ǘ��V�X�e��������
 *				�ȉ��̊֐����g�p����O�ɕK���Ăяo���K�v������
 *
 */
//============================================================================================
extern void	SOUNDMAN_Init(NNSSndHeapHandle* pHeapHandle);





//============================================================================================
/**
 * @brief	�K�w�\���v���[���[�֐�
 */
//============================================================================================
// �K�w�v���[���[�ݒ�\����
typedef struct {
	u16	hierarchyPlayerSize;
	u16	hierarchyPlayerStartNo;
	u16	playerPlayableChannel;
}SOUNDMAN_HIERARCHY_PLAYER_DATA;

// �X���b�h�g�p�����\����
typedef struct {
	u32	soundIdx;
	u8	volume;
	u8	dummy[3];
}SOUNDMAN_HIERARCHY_PLAYTHREAD_ARG;

//------------------------------------------------------------------
// �K�w�v���[���[������
extern void	SOUNDMAN_InitHierarchyPlayer( const SOUNDMAN_HIERARCHY_PLAYER_DATA* playerData );

// �K�w�v���[���[��Ԏ擾
extern u32				SOUNDMAN_GetHierarchyPlayerSoundIdx( void );
extern u32				SOUNDMAN_GetHierarchyPlayerPlayerNoIdx( void );
extern NNSSndHandle*	SOUNDMAN_GetHierarchyPlayerSndHandle( void );
extern int				SOUNDMAN_GetHierarchyPlayerSoundHeapLv( void );
// �K�w�v���[���[��Ԑݒ�
extern void				SOUNDMAN_UpdateHierarchyPlayerSoundHeapLv( void );

// �K�w�v���[���[��ԕ����@�����ݍĐ����̃T�E���h�f�[�^���ǂݍ��܂ꂽ���
extern void	SOUNDMAN_RecoverHierarchyPlayerState( void );

// �K�w�v���[���[�T�E���h����
extern BOOL	SOUNDMAN_PlayHierarchyPlayer( u32 soundIdx );
extern void	SOUNDMAN_StopHierarchyPlayer( void );
extern void	SOUNDMAN_PauseHierarchyPlayer( BOOL flag );
extern BOOL	SOUNDMAN_PushHierarchyPlayer( void );
extern BOOL	SOUNDMAN_PopHierarchyPlayer( void );

// �K�w�v���[���[�T�E���h����(�X���b�h�Đ��p)
extern void	SOUNDMAN_PlayHierarchyPlayer_forThread_heapsv( void );
extern BOOL	SOUNDMAN_PlayHierarchyPlayer_forThread_play( u32 soundIdx );

//============================================================================================
/**
 * @brief	�T�E���h�v���Z�b�g�֐�
 */
//============================================================================================
// �T�E���h�v���Z�b�g�p�n���h��
typedef struct _SOUNDMAN_PRESET_HANDLE	SOUNDMAN_PRESET_HANDLE;

// �v���Z�b�g����у����[�X
extern SOUNDMAN_PRESET_HANDLE*	SOUNDMAN_PresetSoundTbl( const u32* soundIdxTbl, u32 tblNum );
extern SOUNDMAN_PRESET_HANDLE*	SOUNDMAN_PresetGroup( u32 groupIdx );
extern void						SOUNDMAN_ReleasePresetData( SOUNDMAN_PRESET_HANDLE* handle );

#endif
