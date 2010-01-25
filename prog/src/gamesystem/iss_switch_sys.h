////////////////////////////////////////////////////////////////////////////
/**
 * @brief  ISS�X�C�b�`�V�X�e��
 * @file   iss_switch_sys.h
 * @author obata
 * @date   2009.11.17
 */
////////////////////////////////////////////////////////////////////////////
#pragma once 


//==========================================================================
// ���V�X�e���̕s���S�^�錾
//==========================================================================
typedef struct _ISS_SWITCH_SYS ISS_SWITCH_SYS;


//==========================================================================
// ���萔
//==========================================================================
// �X�C�b�`�ԍ�
typedef enum{
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
// ���쐬�E�j��
//==========================================================================
extern ISS_SWITCH_SYS* ISS_SWITCH_SYS_Create( HEAPID heapID );
extern void            ISS_SWITCH_SYS_Delete( ISS_SWITCH_SYS* sys );

//==========================================================================
// ���V�X�e������
//==========================================================================
extern void ISS_SWITCH_SYS_Update( ISS_SWITCH_SYS* sys );

//==========================================================================
// ���V�X�e������
//==========================================================================
// �N��/��~
extern void ISS_SWITCH_SYS_On( ISS_SWITCH_SYS* sys );
extern void ISS_SWITCH_SYS_Off( ISS_SWITCH_SYS* sys );
// �]�[���ύX�ʒm
extern void ISS_SWITCH_SYS_ZoneChange( ISS_SWITCH_SYS* sys, u16 zoneID );

//==========================================================================
// ���X�C�b�`����
//==========================================================================
// �X�C�b�` ON/OFF
extern void ISS_SWITCH_SYS_SwitchOn( ISS_SWITCH_SYS* sys, SWITCH_INDEX switchIdx );
extern void ISS_SWITCH_SYS_SwitchOff( ISS_SWITCH_SYS* sys, SWITCH_INDEX switchIdx );
// �X�C�b�`�� ON/OFF �擾
extern BOOL ISS_SWITCH_SYS_IsSwitchOn( const ISS_SWITCH_SYS* sys, SWITCH_INDEX switchIdx );
