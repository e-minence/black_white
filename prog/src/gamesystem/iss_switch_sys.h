////////////////////////////////////////////////////////////////////////////
/**
 * @brief  ISS�X�C�b�`�V�X�e��
 * @file   iss_switch_sys.h
 * @author obata
 * @date   2009.11.17
 */
////////////////////////////////////////////////////////////////////////////
#pragma once 
#include <gflib.h>
#include "iss_switch_set.h"


//==========================================================================
// ���V�X�e���̕s���S�^�錾
//==========================================================================
typedef struct _ISS_SWITCH_SYS ISS_SWITCH_SYS; 


//==========================================================================
// ���쐬�E�j��
//==========================================================================
extern ISS_SWITCH_SYS* ISS_SWITCH_SYS_Create( HEAPID heapID );
extern void ISS_SWITCH_SYS_Delete( ISS_SWITCH_SYS* system );

//==========================================================================
// ���V�X�e������
//==========================================================================
extern void ISS_SWITCH_SYS_Update( ISS_SWITCH_SYS* system );

//==========================================================================
// ���V�X�e������
//==========================================================================
// �N��/��~
extern void ISS_SWITCH_SYS_On( ISS_SWITCH_SYS* system );
extern void ISS_SWITCH_SYS_Off( ISS_SWITCH_SYS* system );
// �]�[���ύX�ʒm
extern void ISS_SWITCH_SYS_ZoneChange( ISS_SWITCH_SYS* system, u16 zoneID );

//==========================================================================
// ���X�C�b�`����
//==========================================================================
// �X�C�b�` ON/OFF
extern void ISS_SWITCH_SYS_SwitchOn( ISS_SWITCH_SYS* system, SWITCH_INDEX switchIdx );
extern void ISS_SWITCH_SYS_SwitchOff( ISS_SWITCH_SYS* system, SWITCH_INDEX switchIdx );
// �X�C�b�`�� ON/OFF �擾
extern BOOL ISS_SWITCH_SYS_IsSwitchOn( const ISS_SWITCH_SYS* system, SWITCH_INDEX switchIdx );
// �X�C�b�`�̃��Z�b�g
extern void ISS_SWITCH_SYS_ResetSwitch( ISS_SWITCH_SYS* system );
