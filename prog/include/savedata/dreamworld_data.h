//============================================================================================
/**
 * @file	  dreamworld_data.h
 * @brief	  �|�P�����h���[�����[���h�ɕK�v�ȃf�[�^
 * @author	k.ohno
 * @date	  2009.11.08
 */
//============================================================================================
#pragma once

#include "poke_tool/poke_tool.h"
#include "system/gf_date.h"

#define DREAM_WORLD_DATA_MAX_ITEMBOX (20)          ///< �󂯎��A�C�e���{�b�N�X�̐�
#define DREAM_WORLD_DATA_MAX_ITEMNUM (20)          ///< �󂯎��A�C�e����MAX��
#define DREAM_WORLD_DATA_FURNITURE_NAME_NUM (12)  ///< �Ƌ������NUM
#define DREAM_WORLD_DATA_MAX_FURNITURE (5)        ///< �Ƌ��MAX �T��
#define DREAM_WORLD_NOPICTURE (0)              ///< �G�������Ă��Ȃ����
#define DREAM_WORLD_NOFURNITURE (0x7f)            ///< �Ƌ�I���̕s���l
#define DREAM_WORLD_INVALID_FURNITURE (0)      ///< �Ƌ�ԍ��̕s���l


typedef struct _DREAMWORLD_SAVEDATA DREAMWORLD_SAVEDATA;


/// �Q�Ă�|�P�����̏��
typedef enum{
  DREAM_WORLD_SLEEP_TYPE_NONE,    ///< �a���Ă��Ȃ��B�Q�����Ă��Ȃ��B
  DREAM_WORLD_SLEEP_TYPE_SLEEP,   ///< �Q�����ăT�[�o�ɑ�����
  DREAM_WORLD_SLEEP_TYPE_PLAYED,  ///< �h���[�����[���h�ŗV��
  DREAM_WORLD_SLEEP_TYPE_MAX,
} DREAM_WORLD_SLEEP_TYPE;


/// �Ƌ�\���� 26byte
typedef struct {
  u16 id;
  u16 furnitureName[DREAM_WORLD_DATA_FURNITURE_NAME_NUM];
} DREAM_WORLD_FURNITUREDATA;

//�A�C�e�����O��������o����
typedef struct {
  u16 itemindex;
  u16 itemnum;
} DREAMWORLD_ITEM_DATA;


typedef enum{
  DREAMWORLD_PRESENT_ITEM,
  DREAMWORLD_PRESENT_KIND_MAX,
}DREAMWORLD_PRESENT_KIND;

extern int DREAMWORLD_SV_GetWorkSize(void);
extern DREAMWORLD_SAVEDATA* DREAMWORLD_SV_AllocWork(HEAPID heapID);
extern void DREAMWORLD_SV_Init(DREAMWORLD_SAVEDATA* pSV);
extern POKEMON_PARAM* DREAMWORLD_SV_GetSleepPokemon(DREAMWORLD_SAVEDATA* pSV);
extern void DREAMWORLD_SV_SetSleepPokemon(DREAMWORLD_SAVEDATA* pSV,POKEMON_PARAM* pp);

extern u16 DREAMWORLD_SV_GetItem(DREAMWORLD_SAVEDATA* pSV,int index);
extern u8 DREAMWORLD_SV_GetItemNum(DREAMWORLD_SAVEDATA* pSV, int index);
extern void DREAMWORLD_SV_SetItem(DREAMWORLD_SAVEDATA* pSV,int index ,int itemNo, int num);
extern void DREAMWORLD_SV_DeleteItem(DREAMWORLD_SAVEDATA* pSV,int index);
extern void DREAMWORLD_SV_ClearAllItem(DREAMWORLD_SAVEDATA* pSV);
extern int DREAMWORLD_SV_GetItemRestNum(DREAMWORLD_SAVEDATA* pSV);
extern BOOL DREAMWORLD_SV_GetRestItem(DREAMWORLD_SAVEDATA* pSV, int* index, DREAMWORLD_ITEM_DATA* pItem);


extern DREAM_WORLD_FURNITUREDATA* DREAMWORLD_SV_GetFurnitureData(DREAMWORLD_SAVEDATA* pSV,int index);
extern void DREAMWORLD_SV_SetFurnitureData(DREAMWORLD_SAVEDATA* pSV,int index,DREAM_WORLD_FURNITUREDATA* pF);

extern DREAM_WORLD_SLEEP_TYPE DREAMWORLD_SV_GetPokemonStatus(DREAMWORLD_SAVEDATA* pSV);
extern void DREAMWORLD_SV_SetPokemonStatus(DREAMWORLD_SAVEDATA* pSV,DREAM_WORLD_SLEEP_TYPE status);

extern void DREAMWORLD_SV_SetSleepPokemonFlg(DREAMWORLD_SAVEDATA* pSV,BOOL bFlg);
extern BOOL DREAMWORLD_SV_GetSleepPokemonFlg(DREAMWORLD_SAVEDATA* pSV);

extern void DREAMWORLD_SV_SetSignin(DREAMWORLD_SAVEDATA* pSV,BOOL bFlg);  //PDW�o�^�ς�
extern BOOL DREAMWORLD_SV_GetSignin(DREAMWORLD_SAVEDATA* pSV);

extern void DREAMWORLD_SV_SetAccount(DREAMWORLD_SAVEDATA* pSV,BOOL bFlg);  //�A�J�E���g�o�^�ς�
extern BOOL DREAMWORLD_SV_GetAccount(DREAMWORLD_SAVEDATA* pSV);

extern void DREAMWORLD_SV_SetRatemode(DREAMWORLD_SAVEDATA* pSV,BOOL bFlg);  //���[�e�B���O�o�g���J��
extern BOOL DREAMWORLD_SV_GetRatemode(DREAMWORLD_SAVEDATA* pSV);

extern GFDATE DREAMWORLD_SV_GetTime(DREAMWORLD_SAVEDATA* pSV);  //����
extern void DREAMWORLD_SV_SetTime(DREAMWORLD_SAVEDATA* pSV,GFDATE date); //����


extern int DREAMWORLD_SV_GetUploadCount(DREAMWORLD_SAVEDATA* pSV);   
extern void DREAMWORLD_SV_SetUploadCount(DREAMWORLD_SAVEDATA* pSV,int count);

extern int DREAMWORLD_SV_GetMusicalNo(DREAMWORLD_SAVEDATA* pSV);
extern void DREAMWORLD_SV_SetMusicalNo(DREAMWORLD_SAVEDATA* pSV,int no);
extern int DREAMWORLD_SV_GetCGearNo(DREAMWORLD_SAVEDATA* pSV);
extern void DREAMWORLD_SV_SetCGearNo(DREAMWORLD_SAVEDATA* pSV,int no);
extern int DREAMWORLD_SV_GetZukanNo(DREAMWORLD_SAVEDATA* pSV);
extern void DREAMWORLD_SV_SetZukanNo(DREAMWORLD_SAVEDATA* pSV,int no);

extern int DREAMWORLD_SV_GetSelectFurnitureNo(DREAMWORLD_SAVEDATA* pSV);
extern void DREAMWORLD_SV_SetSelectFurnitureNo(DREAMWORLD_SAVEDATA* pSV,int no);
extern BOOL DREAMWORLD_SV_GetIsSyncFurniture(DREAMWORLD_SAVEDATA* pSV);
extern void DREAMWORLD_SV_SetIsSyncFurniture(DREAMWORLD_SAVEDATA* pSV,BOOL flg);

//--------------------------------------------------------------------------------------------
/**
 * @brief   ���鎖���ł���|�P�������ǂ���
 * @param	  u16 pData   ���郊�X�g
 * @param	  u16 monsno  �����X�^�[�ԍ�
 * @return  �˂ނ���TRUE
 */
//--------------------------------------------------------------------------------------------
extern BOOL DREAMWORLD_SV_GetSleepEnable(u16* pData,u16 monsno);

extern DREAMWORLD_SAVEDATA* DREAMWORLD_SV_GetDreamWorldSaveData(SAVE_CONTROL_WORK* pSave);


