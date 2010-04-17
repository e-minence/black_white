//============================================================================================
/**
 * @file	musical_local.h
 * @brief	�~���[�W�J�����p��`
 * @author	ariizumi
 * @date	2008.11.14
 */
//============================================================================================

#pragma once

#include "gamesystem/game_data.h"


//�f�t�H���g��ROM�ɓ����Ă���z�M�f�[�^
#define MUS_PROGRAM_LOCAL_NUM (4)

//�~���[�W�J���f�o�b�O�p
#if PM_DEBUG
#define USE_MUSICAL_EDIT (1)
#endif
//�~���[�W�J���|�P�����̑����ӏ�(�p���b�g�Ή�
typedef enum
{
	MUS_POKE_PLT_SHADOW		=0x04,	//�e�p���W
	MUS_POKE_PLT_ROTATE		=0x05,	//��]�p���W
	
	MUS_POKE_PLT_EAR_R		=0x07,	//�E��
	MUS_POKE_PLT_EAR_L		=0x08,	//����
	MUS_POKE_PLT_HEAD		=0x09,	//��
	MUS_POKE_PLT_EYE		=0x0A,	//��
	MUS_POKE_PLT_FACE		=0x0B,	//��
	MUS_POKE_PLT_BODY		=0x0C,	//��
	MUS_POKE_PLT_WAIST		=0x0D,	//��
	MUS_POKE_PLT_HAND_R		=0x0E,	//�E��
	MUS_POKE_PLT_HAND_L		=0x0F,	//����
	
	MUS_POKE_PLT_START = MUS_POKE_PLT_EAR_R,	//�J�n�ԍ�
}MUS_POKE_EQUIP_PLT;

//�����̍ő�X��
#define MUS_POKE_EQUIP_ANGLE_MAX (15*0x10000/360)

//�p���b�g�ԍ����瑕���ӏ��֕ϊ�
#define MUS_POKE_PLT_TO_POS(val) ((MUS_POKE_EQUIP_POS)(val-MUS_POKE_PLT_START))
//�O���[�v�ԍ������]�֕ϊ�(32����)
//#define MUS_POKE_GRP_TO_ROT(val) (val*(360/32)*65536/360);
#define MUS_POKE_GRP_TO_ROT(val) (val*(65536/32))	//360��ʕ�

//�f�[�^�p�b�N
typedef struct
{
  u32 seqSize;
  u32 bankSize;
  u32 waveSize;
}MUS_DIST_MIDI_HEADER;

typedef struct
{
  u8 programNo;
  
  void *programData;
  void *messageData;
  void *scriptData;
  void *midiData; //���O�{�T�C�Y�w�b�_���p�b�N���Ă���
  void *midiSeqData;
  void *midiBnkData;
  void *midiWaveData;

  u32  programDataSize;
  u32  messageDataSize;
  u32  scriptDataSize;
  u32  midiDataSize;
}MUSICAL_DISTRIBUTE_DATA;

MUSICAL_DISTRIBUTE_DATA* MUSICAL_SYSTEM_InitDistributeData( HEAPID workHeapId );
void MUSICAL_SYSTEM_TermDistributeData( MUSICAL_DISTRIBUTE_DATA *distData );
void MUSICAL_SYSTEM_LoadDistributeData( MUSICAL_DISTRIBUTE_DATA *distData , SAVE_CONTROL_WORK *saveWork , GAMEDATA *gamedata, const u8 programNo , HEAPID strmHeapId);
