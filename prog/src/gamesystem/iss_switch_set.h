///////////////////////////////////////////////////////////////////////////
/**
 * @brief  ISS �X�C�b�`�Z�b�g
 * @file   iss_switch_set.h
 * @author obata
 * @date   2010.03.24
 */
////////////////////////////////////////////////////////////////////////////
#pragma once 


//==========================================================================
// ���X�C�b�`�̕s���S�^
//==========================================================================
typedef struct _ISS_SWITCH_SET ISS_SWITCH_SET;


//==========================================================================
// ���萔
//==========================================================================
// �X�C�b�`�ԍ�
typedef enum {
  SWITCH_00,  // �X�C�b�`0 ==> �f�t�H���g��ON
  SWITCH_01,  // �X�C�b�`1
  SWITCH_02,  // �X�C�b�`2
  SWITCH_03,  // �X�C�b�`3
  SWITCH_04,  // �X�C�b�`4
  SWITCH_05,  // �X�C�b�`5
  SWITCH_06,  // �X�C�b�`6
  SWITCH_07,  // �X�C�b�`7
  SWITCH_08,  // �X�C�b�`8
  SWITCH_NUM,
  SWITCH_MAX = SWITCH_NUM - 1
} SWITCH_INDEX;


//==========================================================================
// ���X�C�b�`�Z�b�g����֐�
//========================================================================== 
// �X�C�b�`�Z�b�g�𐶐�����
extern ISS_SWITCH_SET* ISS_SWITCH_SET_Create( HEAPID heapID );
// �X�C�b�`�Z�b�g��j������
extern void ISS_SWITCH_SET_Delete( ISS_SWITCH_SET* set );
// �X�C�b�`�f�[�^�����[�h����
extern void ISS_SWITCH_SET_Load( ISS_SWITCH_SET* set, ARCDATID datID, HEAPID heapID );
// �X�C�b�`�Z�b�g�̃��C������
extern void ISS_SWITCH_SET_Main( ISS_SWITCH_SET* set );
// �X�C�b�`�Z�b�g������
extern void ISS_SWITCH_SET_SwitchOn( ISS_SWITCH_SET* set, SWITCH_INDEX idx );
// �X�C�b�`�Z�b�g�𗣂�
extern void ISS_SWITCH_SET_SwitchOff( ISS_SWITCH_SET* set, SWITCH_INDEX idx );
// �X�C�b�`�Z�b�g�����Z�b�g����
extern void ISS_SWITCH_SET_ResetSwitch( ISS_SWITCH_SET* set );
// �Ή�����V�[�P���X�ԍ����擾����
extern u32 ISS_SWITCH_SET_GetSoundIdx( const ISS_SWITCH_SET* set );
// �X�C�b�`�Z�b�g���L���ȏꏊ���ǂ����𔻒肷��
extern BOOL ISS_SWITCH_SET_IsValidAt( const ISS_SWITCH_SET* set, u16 zoneID );
// �X�C�b�`��������Ă��邩�ǂ����𔻒肷��
extern BOOL ISS_SWITCH_SET_CheckSwitchOn( const ISS_SWITCH_SET* set, SWITCH_INDEX idx );
// �X�C�b�`�Z�b�g�̏�Ԃ�BGM�̃{�����[���ɔ��f������
extern void ISS_SWITCH_SET_SetBGMVolume( const ISS_SWITCH_SET* set ); 
