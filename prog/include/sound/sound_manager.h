//============================================================================================
/**
 * @file	sound_manager.h
 * @brief	�T�E���h�Ǘ�
 * @author	
 * @date	
 */
//============================================================================================
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
// �K�w�v���[���[��`
typedef struct {
	u16	hierarchyPlayerSize;
	u16	hierarchyPlayerStartNo;
	u16	playerPlayableChannel;
}SOUNDMAN_HIERARCHY_PLAYER_DATA;

//------------------------------------------------------------------
// �K�w�v���[���[������
extern void	SOUNDMAN_InitHierarchyPlayer( const SOUNDMAN_HIERARCHY_PLAYER_DATA* playerData );

// �K�w�v���[���[��Ԏ擾
extern u32				SOUNDMAN_GetHierarchyPlayerSoundIdx( void );
extern u32				SOUNDMAN_GetHierarchyPlayerPlayerNo( void );
extern NNSSndHandle*	SOUNDMAN_GetHierarchyPlayerSndHandle( void );
extern int				SOUNDMAN_GetHierarchyPlayerSoundHeapLv( void );
// �K�w�v���[���[��Ԑݒ�
extern void				SOUNDMAN_UpdateHierarchyPlayerSoundHeapLv( void );

// �K�w�v���[���[��ԕ����@�����ݍĐ����̃T�E���h�f�[�^���ǂݍ��܂ꂽ���
extern void	SOUNDMAN_RecoverHierarchyPlayerState( void );

// �K�w�v���[���[�T�E���h����
extern BOOL	SOUNDMAN_PlayHierarchyPlayer( u32 soundIdx );
extern void	SOUNDMAN_StopHierarchyPlayer( void );
extern BOOL	SOUNDMAN_PushHierarchyPlayer( void );
extern BOOL	SOUNDMAN_PopHierarchyPlayer( void );

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

