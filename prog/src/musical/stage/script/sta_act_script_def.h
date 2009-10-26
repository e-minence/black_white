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

#include "system/vm_cmd.h"

#include "sta_act_script.h"
#include "../sta_acting.h"
#include "../sta_act_effect.h"
#include "../sta_act_poke.h"
#include "../sta_act_obj.h"
#include "../sta_act_light.h"
#include "../sta_act_audience.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define
#define SCRIPT_NUM (8)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
typedef enum
{
	SFT_CONTINUE = VMCMD_RESULT_CONTINUE,	//�p��
	SFT_SUSPEND  = VMCMD_RESULT_SUSPEND,	//���t���[����
	
}SCRIPT_FINISH_TYPE;

typedef enum
{
	SFB_IS_FINISH		= 1<<0,		//�I���������H
	SFB_IS_TARGET_SYNC	= 1<<1,		//�����҂��̑Ώۂɂ��邩�H
	SFB_WAIT_SYNC		= 1<<2,		//�����҂������H
}SCRIPT_FLAG_BIT;

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

//TCB�Ǘ�
struct _STA_SCRIPT_TCB_OBJECT
{
  NNSFndLink  linkObj;
  void        *tcbWork;
  GFL_TCB     *tcbTask;
};

//�X�N���v�g�P�{�̃f�[�^
typedef struct
{
	STA_SCRIPT_SYS *sysWork;
	VMHANDLE	*vmHandle;
	
	void	*scriptData;
	void	*loadPos;
	
	u32		frame;
	u16		waitCnt;
	u16		isFlag;
	
	u8		trgPokeFlg;
	
}STA_SCRIPT_WORK;

//�X�N���v�g�Ǘ�
struct _STA_SCRIPT_SYS
{
	HEAPID heapId;
	
	u32 befVCount;
	
	GFL_TCBSYS	*tcbSys;
	void		*tcbWork;
	STA_SCRIPT_WORK *scriptWork[SCRIPT_NUM];
	ACTING_WORK *actWork;
	
	NNSFndList tcbList;
};


typedef SCRIPT_FINISH_TYPE(*STA_SCRIPT_FUNC)(STA_SCRIPT_SYS *work,STA_SCRIPT_WORK *scriptWork);


//--------------------------------------------------------------
//	�X�N���v�g�̊֐�
//--------------------------------------------------------------
extern STA_SCRIPT_TCB_OBJECT* STA_SCRIPT_CreateTcbTask( STA_SCRIPT_SYS *work , GFL_TCB_FUNC *func , void* tcbWork , u32 pri );
extern void STA_SCRIPT_DeleteTcbTask( STA_SCRIPT_SYS *work , STA_SCRIPT_TCB_OBJECT *tcbObj );

//--------------------------------------------------------------
//	�X�N���v�g�p�@�\�֐�
//--------------------------------------------------------------
extern void STA_ACT_StartMainPart( ACTING_WORK *work );
extern void STA_ACT_FinishMainPart( ACTING_WORK *work );
extern void	STA_ACT_LoadBg( ACTING_WORK *work , const u8 bgNo );
extern void STA_ACT_ShowMessage( ACTING_WORK *work , const u16 msgNo , const u8 msgSpd );
extern void STA_ACT_HideMessage( ACTING_WORK *work );
extern void  STA_ACT_StartBgm(  ACTING_WORK *work );
extern void  STA_ACT_StopBgm(  ACTING_WORK *work );

//--------------------------------------------------------------
//	�X�N���v�g�p�ɊO���񋟊֐�
//--------------------------------------------------------------
extern STA_POKE_SYS*	STA_ACT_GetPokeSys( ACTING_WORK *work );
extern STA_POKE_WORK*	STA_ACT_GetPokeWork( ACTING_WORK *work , const u8 idx );
extern void			STA_ACT_SetPokeWork( ACTING_WORK *work , STA_POKE_WORK *pokeWork , const u8 idx );
extern STA_OBJ_SYS*	STA_ACT_GetObjectSys( ACTING_WORK *work );
extern STA_OBJ_WORK*	STA_ACT_GetObjectWork( ACTING_WORK *work , const u8 idx );
extern void			STA_ACT_SetObjectWork( ACTING_WORK *work , STA_OBJ_WORK *objWork , const u8 idx );
extern STA_EFF_SYS*	STA_ACT_GetEffectSys( ACTING_WORK *work );
extern STA_EFF_WORK*	STA_ACT_GetEffectWork( ACTING_WORK *work , const u8 idx );
extern void			STA_ACT_SetEffectWork( ACTING_WORK *work , STA_EFF_WORK *effWork , const u8 idx );
extern STA_LIGHT_SYS* 	STA_ACT_GetLightSys( ACTING_WORK *work );
extern STA_LIGHT_WORK* STA_ACT_GetLightWork( ACTING_WORK *work , const u8 idx );
extern void 			STA_ACT_SetLightWork( ACTING_WORK *work , STA_LIGHT_WORK *lightWork , const u8 idx );
extern STA_AUDI_SYS* STA_ACT_GetAudienceSys( ACTING_WORK *work );

extern u16		STA_ACT_GetCurtainHeight( ACTING_WORK *work );
extern void	STA_ACT_SetCurtainHeight( ACTING_WORK *work , const u16 height );

extern void STA_ACT_SetLightUpFlg( ACTING_WORK *work , const u8 pokeIdx , const BOOL flg );
extern void STA_ACT_PlayTransEffect( ACTING_WORK *work , const u8 idx );
extern const u8 STA_ACT_GetPokeEquipPoint( ACTING_WORK *work , const u8 pokeNo );
extern void STA_ACT_SetForceScroll( ACTING_WORK *work , const BOOL flg );

#endif STA_ACT_SCRIPT_DEF_H__
