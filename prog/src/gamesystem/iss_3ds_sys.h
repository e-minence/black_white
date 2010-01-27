/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  ��ISS�V�X�e�� ( 3D�T�E���h�V�X�e�� )
 * @file   iss_3d_sys.h
 * @author obata
 * @date   2009.09.08
 */
/////////////////////////////////////////////////////////////////////////////////
#pragma once 


//===============================================================================
// ���s���S�^�̐錾
//===============================================================================
typedef struct _ISS_3DS_SYS ISS_3DS_SYS;


//===============================================================================
// ���萔
//===============================================================================
// 3D�T�E���h���j�b�g �C���f�b�N�X
typedef enum
{
  ISS3DS_UNIT_TRACK01,  // �g���b�N 1�𑀍삷�郆�j�b�g
  ISS3DS_UNIT_TRACK02,  // �g���b�N 2�𑀍삷�郆�j�b�g
  ISS3DS_UNIT_TRACK03,  // �g���b�N 3�𑀍삷�郆�j�b�g
  ISS3DS_UNIT_TRACK04,  // �g���b�N 4�𑀍삷�郆�j�b�g
  ISS3DS_UNIT_TRACK05,  // �g���b�N 5�𑀍삷�郆�j�b�g
  ISS3DS_UNIT_TRACK06,  // �g���b�N 6�𑀍삷�郆�j�b�g
  ISS3DS_UNIT_TRACK07,  // �g���b�N 7�𑀍삷�郆�j�b�g
  ISS3DS_UNIT_TRACK08,  // �g���b�N 8�𑀍삷�郆�j�b�g
  ISS3DS_UNIT_TRACK09,  // �g���b�N 9�𑀍삷�郆�j�b�g
  ISS3DS_UNIT_TRACK10,  // �g���b�N10�𑀍삷�郆�j�b�g
  ISS3DS_UNIT_TRACK11,  // �g���b�N11�𑀍삷�郆�j�b�g
  ISS3DS_UNIT_TRACK12,  // �g���b�N12�𑀍삷�郆�j�b�g
  ISS3DS_UNIT_TRACK13,  // �g���b�N13�𑀍삷�郆�j�b�g
  ISS3DS_UNIT_TRACK14,  // �g���b�N14�𑀍삷�郆�j�b�g
  ISS3DS_UNIT_TRACK15,  // �g���b�N15�𑀍삷�郆�j�b�g
  ISS3DS_UNIT_TRACK16,  // �g���b�N16�𑀍삷�郆�j�b�g
  ISS3DS_UNIT_NUM
} ISS3DS_UNIT_INDEX;


//===============================================================================
// ���V�X�e��
//===============================================================================
// �쐬 / �j��
extern ISS_3DS_SYS* ISS_3DS_SYS_Create( HEAPID heapID );
extern void         ISS_3DS_SYS_Delete( ISS_3DS_SYS* system );

// ���� 
extern void ISS_3DS_SYS_Main( ISS_3DS_SYS* system );

// ����
extern void ISS_3DS_SYS_On             ( ISS_3DS_SYS* system );
extern void ISS_3DS_SYS_Off            ( ISS_3DS_SYS* system ); 
extern void ISS_3DS_SYS_ZoneChange     ( ISS_3DS_SYS* system );
extern void ISS_3DS_SYS_SetMasterVolume( ISS_3DS_SYS* system, u8 volume );


//===============================================================================
// �����j�b�g
//=============================================================================== 
// �o�^
extern void ISS_3DS_SYS_RegisterUnit( ISS_3DS_SYS* system, ISS3DS_UNIT_INDEX unitIdx, 
                                      fx32 effectiveRange, int maxVolume );

// �o�^�ς݂��ǂ���
extern BOOL ISS_3DS_SYS_IsUnitRegistered( const ISS_3DS_SYS* system, ISS3DS_UNIT_INDEX unitIdx );

// �ړ�
extern void ISS_3DS_SYS_SetUnitPos( ISS_3DS_SYS* system, 
                                    ISS3DS_UNIT_INDEX unitIdx, const VecFx32* pos );


//===============================================================================
// ���ϑ���
//===============================================================================
// �ړ�
extern void ISS_3DS_SYS_SetObserverPos( ISS_3DS_SYS* system, 
                                        const VecFx32* pos, const VecFx32* targetPos );
