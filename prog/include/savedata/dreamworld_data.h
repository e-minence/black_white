//============================================================================================
/**
 * @file	  dreamworld_data.h
 * @brief	  �|�P�����h���[�����[���h�ɕK�v�ȃf�[�^
 * @author	k.ohno
 * @date	  2009.11.08
 */
//============================================================================================
#pragma once

#define DREAM_WORLD_DATA_MAX_EVENT (100)          ///< �󂯎��C�x���g�̎��
#define DREAM_WORLD_DATA_FURNITURE_NAME_NUM (12)  ///< �Ƌ������NUM
#define DREAM_WORLD_DATA_MAX_FURNITURE (5)        ///< �Ƌ��MAX �T��


typedef struct _DREAMWORLD_SAVEDATA DREAMWORLD_SAVEDATA;

/// PDW�ł��炤�^�C�v
typedef enum{
  DREAM_WORLD_RESULT_TYPE_EVENT,   ///< �C�x���g
  DREAM_WORLD_RESULT_TYPE_ITEM,    ///< �A�C�e��
  DREAM_WORLD_RESULT_TYPE_ENCOUNT, ///< �G���J�E���g�^�C�v
} DREAM_WORLD_RESULT_TYPE;


typedef struct {
  u16 eventNo;   // �C�x���g�ԍ�
} DREAM_WORLD_EVENTDATA;

typedef struct {
	u16 itemID;   //	�A�C�e��ID						2byte			��M�����A�C�e���̎��
} DREAM_WORLD_ITEMDATA;

typedef struct {
	u16 encountType; //�G���J�E���g�^�C�v
} DREAM_WORLD_ENCOUNTDATA;

/// �Ƌ�\���� 26byte
typedef struct {
  u16 id;
  u16 furnitureName[DREAM_WORLD_DATA_FURNITURE_NAME_NUM];
} DREAM_WORLD_FURNITUREDATA;

/// ���J���f�[�^���p��
typedef union {
  DREAM_WORLD_EVENTDATA event;
  DREAM_WORLD_ITEMDATA item;
  DREAM_WORLD_ENCOUNTDATA enc;
} DREAM_WORLD_TREAT_DATA;





extern int DREAMWORLD_SV_GetWorkSize(void);
extern DREAMWORLD_SAVEDATA* DREAMWORLD_SV_AllocWork(HEAPID heapID);
extern void DREAMWORLD_SV_Init(DREAMWORLD_SAVEDATA* pSV);
extern BOOL DREAMWORLD_SV_IsEventRecvFlag(DREAMWORLD_SAVEDATA* pSV,int num);
extern void DREAMWORLD_SV_SetEventRecvFlag(DREAMWORLD_SAVEDATA* pSV, int num);
extern DREAMWORLD_SAVEDATA* DREAMWORLD_SV_GetDreamWorldSaveData(SAVE_CONTROL_WORK* pSave);


