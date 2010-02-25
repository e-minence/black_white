//======================================================================
/**
 * @file   battle_examination.h
 * @brief  �o�g������Z�[�u�f�[�^
 * @author k.ohno
 * @date	 10.01.08
 */
//======================================================================


#include "savedata/bsubway_savedata.h"
//#include "battle/bsubway_battle_data.h"


//typedef _BSUBWAY_PARTNER_DATA BSUBWAY_PARTNER_DATA;

#define BATTLE_EXAMINATION_MAX (5)   ///< �o�g������̐l��
#define BATTLE_EXAMINATION_TITLE_MSG_MAX (34+2)   ///< ���b�Z�[�W�̐�

typedef struct _BATTLE_EXAMINATION_SAVEDATA BATTLE_EXAMINATION_SAVEDATA;

extern int BATTLE_EXAMINATION_SAVE_GetWorkSize(void);
extern BATTLE_EXAMINATION_SAVEDATA* BATTLE_EXAMINATION_SAVE_AllocWork(HEAPID heapID);
extern void BATTLE_EXAMINATION_SAVE_Init(BATTLE_EXAMINATION_SAVEDATA* pSV);
extern BATTLE_EXAMINATION_SAVEDATA *BATTLE_EXAMINATION_SAVE_GetSvPtr( SAVE_CONTROL_WORK * p_sv);
extern BSUBWAY_PARTNER_DATA *BATTLE_EXAMINATION_SAVE_GetData(BATTLE_EXAMINATION_SAVEDATA* pSV, const u32 inIdx);

