//======================================================================
/**
 * @file	sta_act_script.h
 * @brief	�X�e�[�W �X�N���v�g���� ���[�J����`
 * @author	ariizumi
 * @data	09/03/06
 */
//======================================================================

#ifndef STA_ACT_SCRIPT_DEF_H__
#define STA_ACT_SCRIPT_DEF_H__

#include "sta_act_script.h"
#include "../sta_acting.h"
#include "../sta_act_effect.h"
#include "../sta_act_poke.h"
#include "../sta_act_obj.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define
#define SCRIPT_NUM (3)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
typedef enum
{
	SFT_CONTINUE,	//�p��
	SFT_SUSPEND,	//���t���[����
	SFT_END,		//�X�N���v�g�I��
	
}SCRIPT_FINISH_TYPE;


//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

//�X�N���v�g�P�{�̃f�[�^
typedef struct
{
	void	*scriptData;
	void	*loadPos;
	
	u16		waitCnt;
	
}STA_SCRIPT_WORK;

//�X�N���v�g�Ǘ�
struct _STA_SCRIPT_SYS
{
	HEAPID heapId;
	
	GFL_TCBSYS	*tcbSys;
	void		*tcbWork;
	STA_SCRIPT_WORK *scriptWork[SCRIPT_NUM];
	ACTING_WORK *actWork;
};


typedef SCRIPT_FINISH_TYPE(*STA_SCRIPT_FUNC)(STA_SCRIPT_SYS *work,STA_SCRIPT_WORK *scriptWork);

//--------------------------------------------------------------
//	�X�N���v�g�p�ɊO���񋟊֐�
//--------------------------------------------------------------
STA_POKE_SYS*	STA_ACT_GetPokeSys( ACTING_WORK *work );
STA_POKE_WORK*	STA_ACT_GetPokeWork( ACTING_WORK *work , const u8 idx );
void			STA_ACT_SetPokeWork( ACTING_WORK *work , STA_POKE_WORK *pokeWork , const u8 idx );
STA_OBJ_SYS*	STA_ACT_GetObjectSys( ACTING_WORK *work );
STA_OBJ_WORK*	STA_ACT_GetObjectWork( ACTING_WORK *work , const u8 idx );
void			STA_ACT_SetObjectWork( ACTING_WORK *work , STA_OBJ_WORK *objWork , const u8 idx );
STA_EFF_SYS*	STA_ACT_GetEffectSys( ACTING_WORK *work );
STA_EFF_WORK*	STA_ACT_GetEffectWork( ACTING_WORK *work , const u8 idx );
void			STA_ACT_SetEffectWork( ACTING_WORK *work , STA_EFF_WORK *effWork , const u8 idx );

u16		STA_ACT_GetCurtainHeight( ACTING_WORK *work );
void	STA_ACT_SetCurtainHeight( ACTING_WORK *work , const u16 height );
u16		STA_ACT_GetStageScroll( ACTING_WORK *work );
void	STA_ACT_SetStageScroll( ACTING_WORK *work , const u16 scroll );

#endif STA_ACT_SCRIPT_DEF_H__
