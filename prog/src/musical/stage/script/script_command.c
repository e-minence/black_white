//======================================================================
/**
 * @file	sta_act_script.h
 * @brief	�X�e�[�W �X�N���v�g���� �R�}���h�Q
 * @author	ariizumi
 * @data	09/03/06
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "print/gf_font.h"

#include "stage_gra.naix"
#include "script_table.h"
#include "../sta_local_def.h"
#include "test/ariizumi/ari_debug.h"


//======================================================================
//	define
//======================================================================
#pragma mark [> define
#define SCRIPT_PRINT_LABEL(str) ARI_TPrintf("SCRIPT Frame[%4d] No[%d]%s\n",scriptWork->frame, SCRIPT_ENUM_ ## str ,#str);

#define ScriptFunc_GetValueS32() (VMGetU32(scriptWork->vmHandle))
#define ScriptFunc_GetValuefx32() (VMGetU32(scriptWork->vmHandle))

//�ʏ�̃^�X�N�v���C�I���e�B
//�Ǐ]�n�Ȃǂ͒Ⴍ�ݒ肷�邱�ƂŁA�{�̂̈ړ���D�悳����
#define SCRIPT_TCB_PRI_NORMAL (10)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
typedef enum
{
	RMR_WAIT,	//�ҋ@
	RMR_ACTION,	//�A�N�V�������s
	RMR_END,	//�I��
}REPEAT_MNG_RETURN;


//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
typedef struct
{
	//TODO ���x�m�ۂ̂��߂�fx32�Ƃ��ł�����ق����ǂ�����
	//		���󐸓x�m�ۂ̂��߂ɖ��t���[�����Z���Ă�
	ACTING_WORK *actWork;
	s32 start;
	s32 end;
	s32 stepVal;	//���t���[���̑�����
	u16 frame;
	u16 step;
}MOVE_WORK_S32;

typedef struct
{
	ACTING_WORK *actWork;
	VecFx32 start;
	VecFx32 end;
	VecFx32 stepVal;	//���t���[���̑�����
	u16 frame;
	u16 step;
}MOVE_WORK_VEC;

typedef struct
{
	STA_POKE_WORK *pokeWork;
	MOVE_WORK_VEC moveWork;
}MOVE_POKE_VEC;

typedef struct
{
	STA_OBJ_WORK *objWork;
	MOVE_WORK_VEC moveWork;
}MOVE_OBJ_VEC;

typedef struct
{
	STA_LIGHT_WORK *lightWork;
	MOVE_WORK_VEC moveWork;
}MOVE_LIGHT_VEC;

//�w��񐔌J��Ԃ����̊Ǘ�
typedef struct
{
	ACTING_WORK *actWork;
	u32 step;
	u32 interval;
	u32 num;
}REPEAT_MNG_WORK;

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

//�֐���S����錾
//VMCMD_RESULT STA_SCRIPT_FuncName_Func(VMHANDLE *vmh, void *context_work);
#define SCRIPT_FUNC_DEF(str) VMCMD_RESULT STA_SCRIPT_ ## str ## _Func(VMHANDLE *vmh, void *context_work);
#include "script_table.dat"
#undef SCRIPT_FUNC_DEF

//TCB�p�֐���`
//���W�⊮�ړ�
static BOOL SCRIPT_TCB_UpdateMoveS32( MOVE_WORK_S32 *moveWork , s32 *newPos );
static BOOL SCRIPT_TCB_UpdateMoveVec( MOVE_WORK_VEC *moveWork , VecFx32 *newPos );
static REPEAT_MNG_RETURN SCRIPT_TCB_UpdateRepeat( REPEAT_MNG_WORK *repeatWork );

//�ړ��n
static void SCRIPT_TCB_MoveCurtainTCB(  GFL_TCB *tcb, void *work );
static void SCRIPT_TCB_MoveStageTCB(  GFL_TCB *tcb, void *work );
static void SCRIPT_TCB_MovePokeTCB(  GFL_TCB *tcb, void *work );
static void SCRIPT_TCB_MoveObjTCB(  GFL_TCB *tcb, void *work );
static void SCRIPT_TCB_MoveLightTCB(  GFL_TCB *tcb, void *work );

//�A�N�V�����n
static void SCRIPT_TCB_PokeAct_Jump(  GFL_TCB *tcb, void *work );

static void SCRIPT_TCB_EffectRepeat(  GFL_TCB *tcb, void *work );

//======================================================================
//	data
//======================================================================

//------------------------------------------------------------------
//	�ėp�֐�
//------------------------------------------------------------------
static BOOL SCRIPT_TCB_UpdateMoveS32( MOVE_WORK_S32 *moveWork , s32 *newPos )
{
	moveWork->step++;
	
	if( moveWork->step >= moveWork->frame )
	{
		*newPos = moveWork->end;
		return TRUE;
	}
	else
	{
		const s32 ofs = moveWork->end - moveWork->start;
		*newPos = moveWork->start + (ofs * moveWork->step / moveWork->frame);
		return FALSE;
	}
}

static BOOL SCRIPT_TCB_UpdateMoveVec( MOVE_WORK_VEC *moveWork , VecFx32 *newPos )
{
	moveWork->step++;
	
	if( moveWork->step >= moveWork->frame )
	{
		newPos->x = moveWork->end.x;
		newPos->y = moveWork->end.y;
		newPos->z = moveWork->end.z;
		return TRUE;
	}
	else
	{
		VEC_MultAdd( FX32_CONST(moveWork->step) , &moveWork->stepVal ,
					 &moveWork->start , newPos );
		return FALSE;
	}
}

static REPEAT_MNG_RETURN SCRIPT_TCB_UpdateRepeat( REPEAT_MNG_WORK *repeatWork )
{
	repeatWork->step++;
	
	if( repeatWork->step >= repeatWork->interval )
	{
		repeatWork->num--;
		repeatWork->step = 0;

		if( repeatWork->num <= 0 )
		{
			return RMR_END;
		}
		else
		{
			return RMR_ACTION;
		}
	}
	return RMR_WAIT;
}



#define SCRIPT_FUNC_DEF(str) VMCMD_RESULT STA_SCRIPT_ ## str ## _Func(VMHANDLE *vmh, void *context_work)

//------------------------------------------------------------------
//	�V�X�e��
//------------------------------------------------------------------
#pragma mark [>System
//�X�N���v�g�̏I��
SCRIPT_FUNC_DEF( ScriptFinish )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	SCRIPT_PRINT_LABEL(ScriptFinish);
	scriptWork->isFinish = TRUE;
	return SFT_SUSPEND;
}

//�w��t���[���ҋ@
SCRIPT_FUNC_DEF( FrameWait )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	const s32 count = ScriptFunc_GetValueS32();
	scriptWork->waitCnt = count;

	SCRIPT_PRINT_LABEL(FrameWait);
	return SFT_SUSPEND;
}

//�w��t���[���܂ő҂�
SCRIPT_FUNC_DEF( FrameWaitTime )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	STA_SCRIPT_SYS *work = scriptWork->sysWork;
	const s32 count = ScriptFunc_GetValueS32();

	SCRIPT_PRINT_LABEL(FrameWaitTime);
	
	if( scriptWork->frame >= count )
	{
		return SFT_CONTINUE;
	} 
	else
	{
		scriptWork->waitCnt = count - scriptWork->frame;
		return SFT_SUSPEND;
	}
	
}

//------------------------------------------------------------------
//	�J�[�e��
//------------------------------------------------------------------
#pragma mark [>Curtain

//�J�[�e���グ��(�Œ葬�x
SCRIPT_FUNC_DEF( CurtainUp )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	STA_SCRIPT_SYS *work = scriptWork->sysWork;
	MOVE_WORK_S32 *moveWork;
	SCRIPT_PRINT_LABEL(CurtainUp);

	moveWork = GFL_HEAP_AllocMemory( work->heapId , sizeof( MOVE_WORK_S32 ));
	moveWork->actWork = work->actWork;
	moveWork->step = 0;
	moveWork->start = STA_ACT_GetCurtainHeight( work->actWork );
	moveWork->end = ACT_CURTAIN_SCROLL_MAX;
	moveWork->frame = MATH_ABS(moveWork->end-moveWork->start)/ACT_CURTAIN_SCROLL_SPEED;
	moveWork->stepVal = (moveWork->end-moveWork->start)/moveWork->frame;
	
	GFL_TCB_AddTask( work->tcbSys , SCRIPT_TCB_MoveCurtainTCB , (void*)moveWork , 10 );
	
	return SFT_CONTINUE;
	
}
//�J�[�e��������(�Œ葬�x
SCRIPT_FUNC_DEF( CurtainDown )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	STA_SCRIPT_SYS *work = scriptWork->sysWork;
	MOVE_WORK_S32 *moveWork;
	SCRIPT_PRINT_LABEL(CurtainDown);
	moveWork = GFL_HEAP_AllocMemory( work->heapId , sizeof( MOVE_WORK_S32 ));
	moveWork->actWork = work->actWork;
	moveWork->step = 0;
	moveWork->start = STA_ACT_GetCurtainHeight( work->actWork );
	moveWork->end = ACT_CURTAIN_SCROLL_MIN;
	moveWork->frame = MATH_ABS(moveWork->end-moveWork->start)/ACT_CURTAIN_SCROLL_SPEED;
	moveWork->stepVal = (moveWork->end-moveWork->start)/moveWork->frame;
	
	GFL_TCB_AddTask( work->tcbSys , SCRIPT_TCB_MoveCurtainTCB , (void*)moveWork , 10 );
	
	return SFT_CONTINUE;
}
//�J�[�e��������
SCRIPT_FUNC_DEF( CurtainMove )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	STA_SCRIPT_SYS *work = scriptWork->sysWork;
	const s32 frame = ScriptFunc_GetValueS32();
	const s32 pos = ScriptFunc_GetValuefx32();
	
	if( frame == 0 )
	{
		STA_ACT_SetCurtainHeight( work->actWork , pos);
	}
	else
	{
		MOVE_WORK_S32 *moveWork;
		moveWork = GFL_HEAP_AllocMemory( work->heapId , sizeof( MOVE_WORK_S32 ));
		moveWork->actWork = work->actWork;
		moveWork->step = 0;
		moveWork->start = STA_ACT_GetCurtainHeight( work->actWork );
		moveWork->end = pos;
		moveWork->frame = frame;
		moveWork->stepVal = (moveWork->end-moveWork->start)/moveWork->frame;
	
		GFL_TCB_AddTask( work->tcbSys , SCRIPT_TCB_MoveCurtainTCB , (void*)moveWork , 10 );
	}
	
	SCRIPT_PRINT_LABEL(CurtainMove);
	return SFT_CONTINUE;
}
//�J�[�e������TCB
static void SCRIPT_TCB_MoveCurtainTCB(  GFL_TCB *tcb, void *work )
{
	MOVE_WORK_S32 *moveWork = (MOVE_WORK_S32*)work;
	
	s32 newPos;
	
	const BOOL isFinish = SCRIPT_TCB_UpdateMoveS32( moveWork , &newPos );
	
	STA_ACT_SetCurtainHeight( moveWork->actWork , newPos);
	if( isFinish == TRUE )
	{
		GFL_HEAP_FreeMemory( work );
		GFL_TCB_DeleteTask( tcb );
	}
}

//------------------------------------------------------------------
//	����n
//------------------------------------------------------------------
#pragma mark [>Stage

//�X�e�[�W���ړ�������
SCRIPT_FUNC_DEF( StageMove )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	STA_SCRIPT_SYS *work = scriptWork->sysWork;
	const s32 frame = ScriptFunc_GetValueS32();
	const s32 pos = ScriptFunc_GetValuefx32();
	
	if( frame == 0 )
	{
		STA_ACT_SetStageScroll( work->actWork , pos);
	}
	else
	{
		MOVE_WORK_S32 *moveWork;
		moveWork = GFL_HEAP_AllocMemory( work->heapId , sizeof( MOVE_WORK_S32 ));
		moveWork->actWork = work->actWork;
		moveWork->step = 0;
		moveWork->start = STA_ACT_GetStageScroll( work->actWork );
		moveWork->end = pos;
		moveWork->frame = frame;
		moveWork->stepVal = (moveWork->end-moveWork->start)/moveWork->frame;
	
		GFL_TCB_AddTask( work->tcbSys , SCRIPT_TCB_MoveStageTCB , (void*)moveWork , 10 );
	}

	SCRIPT_PRINT_LABEL(StageMove);
	return SFT_CONTINUE;
}

//�X�e�[�W����TCB
static void SCRIPT_TCB_MoveStageTCB(  GFL_TCB *tcb, void *work )
{
	MOVE_WORK_S32 *moveWork = (MOVE_WORK_S32*)work;
	
	s32 newPos;
	
	const BOOL isFinish = SCRIPT_TCB_UpdateMoveS32( moveWork , &newPos );
	
	STA_ACT_SetStageScroll( moveWork->actWork , newPos);
	if( isFinish == TRUE )
	{
		GFL_HEAP_FreeMemory( work );
		GFL_TCB_DeleteTask( tcb );
	}
}

//BG�ǂݑւ�
SCRIPT_FUNC_DEF( StageChangeBg )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	STA_SCRIPT_SYS *work = scriptWork->sysWork;
	const s32 bgNo = ScriptFunc_GetValueS32();
	
	STA_ACT_LoadBg( work->actWork , bgNo );

	SCRIPT_PRINT_LABEL(StageChangeBg);
	return SFT_CONTINUE;
}

//------------------------------------------------------------------
//	�|�P����
//------------------------------------------------------------------
#pragma mark [>Pokemon

//�|�P�����\������
SCRIPT_FUNC_DEF( PokeShow )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	STA_SCRIPT_SYS *work = scriptWork->sysWork;
	const s32 pokeNo = ScriptFunc_GetValueS32();
	const s32 flg = ScriptFunc_GetValueS32();

	STA_POKE_SYS  *pokeSys = STA_ACT_GetPokeSys( work->actWork );
	STA_POKE_WORK *pokeWork = STA_ACT_GetPokeWork( work->actWork , (u8)pokeNo );

	STA_POKE_SetShowFlg( pokeSys , pokeWork , flg );

	SCRIPT_PRINT_LABEL(PokeShow);
	return SFT_CONTINUE;
}

//�|�P��������
SCRIPT_FUNC_DEF( PokeDir )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	STA_SCRIPT_SYS *work = scriptWork->sysWork;
	const s32 pokeNo = ScriptFunc_GetValueS32();
	const s32 dir = ScriptFunc_GetValueS32();

	STA_POKE_SYS  *pokeSys = STA_ACT_GetPokeSys( work->actWork );
	STA_POKE_WORK *pokeWork = STA_ACT_GetPokeWork( work->actWork , (u8)pokeNo );

	STA_POKE_SetPokeDir( pokeSys , pokeWork , (dir==0 ? SPD_LEFT : SPD_RIGHT) );

	SCRIPT_PRINT_LABEL(PokeDir);
	return SFT_CONTINUE;
}

//�|�P�����ړ�
SCRIPT_FUNC_DEF( PokeMove )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	STA_SCRIPT_SYS *work = scriptWork->sysWork;
	const s32 pokeNo = ScriptFunc_GetValueS32();
	const s32 frame = ScriptFunc_GetValueS32();
	const fx32 posX = ScriptFunc_GetValuefx32();
	const fx32 posY = ScriptFunc_GetValuefx32();
	const fx32 posZ = ScriptFunc_GetValuefx32();
	
	STA_POKE_SYS  *pokeSys = STA_ACT_GetPokeSys( work->actWork );
	STA_POKE_WORK *pokeWork = STA_ACT_GetPokeWork( work->actWork , (u8)pokeNo );

	if( frame == 0 )
	{
		VecFx32 pos;
		VEC_Set( &pos , posX,posY,posZ );
		STA_POKE_SetPosition( pokeSys , pokeWork , &pos );		
	}
	else
	{
		VecFx32 subVec;
		MOVE_POKE_VEC *movePoke;
		movePoke = GFL_HEAP_AllocMemory( work->heapId , sizeof( MOVE_POKE_VEC ));
		movePoke->pokeWork = pokeWork;
		movePoke->moveWork.actWork = work->actWork;
		movePoke->moveWork.step = 0;
		STA_POKE_GetPosition( pokeSys , pokeWork , &movePoke->moveWork.start );
		VEC_Set( &movePoke->moveWork.end , posX,posY,posZ );
		movePoke->moveWork.frame = frame;

		VEC_Subtract( &movePoke->moveWork.end , &movePoke->moveWork.start , &subVec );
		movePoke->moveWork.stepVal.x = subVec.x / frame;
		movePoke->moveWork.stepVal.y = subVec.y / frame;
		movePoke->moveWork.stepVal.z = subVec.z / frame;

		GFL_TCB_AddTask( work->tcbSys , SCRIPT_TCB_MovePokeTCB , (void*)movePoke , 10 );
	}
	
	SCRIPT_PRINT_LABEL(PokeMove);
	return SFT_CONTINUE;
}
//�|�P�����ړ�����TCB
static void SCRIPT_TCB_MovePokeTCB(  GFL_TCB *tcb, void *work )
{
	MOVE_POKE_VEC *movePoke = (MOVE_POKE_VEC*)work;
	STA_POKE_SYS  *pokeSys = STA_ACT_GetPokeSys( movePoke->moveWork.actWork );
	VecFx32 newPos;
	
	const BOOL isFinish = SCRIPT_TCB_UpdateMoveVec( &movePoke->moveWork , &newPos );
	STA_POKE_SetPosition( pokeSys , movePoke->pokeWork , &newPos );
	
	if( isFinish == TRUE )
	{
		GFL_HEAP_FreeMemory( work );
		GFL_TCB_DeleteTask( tcb );
	}
}

//�|�P�����A�j����~
SCRIPT_FUNC_DEF( PokeStopAnime )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	STA_SCRIPT_SYS *work = scriptWork->sysWork;
	const s32 pokeNo = ScriptFunc_GetValueS32();
	
	STA_POKE_SYS  *pokeSys = STA_ACT_GetPokeSys( work->actWork );
	STA_POKE_WORK *pokeWork = STA_ACT_GetPokeWork( work->actWork , (u8)pokeNo );
	
	STA_POKE_StopAnime( pokeSys , pokeWork );
	
	SCRIPT_PRINT_LABEL(PokeStopAnime);
	return SFT_CONTINUE;
}

//�|�P�����A�j���J�n
SCRIPT_FUNC_DEF( PokeStartAnime )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	STA_SCRIPT_SYS *work = scriptWork->sysWork;
	const s32 pokeNo = ScriptFunc_GetValueS32();
	
	STA_POKE_SYS  *pokeSys = STA_ACT_GetPokeSys( work->actWork );
	STA_POKE_WORK *pokeWork = STA_ACT_GetPokeWork( work->actWork , (u8)pokeNo );

	STA_POKE_StartAnime( pokeSys , pokeWork );

	SCRIPT_PRINT_LABEL(PokeStartAnime);
	return SFT_CONTINUE;
}

//�|�P�����A�j���ύX
SCRIPT_FUNC_DEF( PokeChangeAnime )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	STA_SCRIPT_SYS *work = scriptWork->sysWork;
	const s32 pokeNo = ScriptFunc_GetValueS32();
	const s32 anmIdx = ScriptFunc_GetValueS32();
	
	STA_POKE_SYS  *pokeSys = STA_ACT_GetPokeSys( work->actWork );
	STA_POKE_WORK *pokeWork = STA_ACT_GetPokeWork( work->actWork , (u8)pokeNo );

	STA_POKE_ChangeAnime( pokeSys , pokeWork , anmIdx );

	SCRIPT_PRINT_LABEL(PokeChangeAnime);
	return SFT_CONTINUE;
}

#pragma mark [>PokemonAction
//�|�P�����A�N�V�����E���˂�
typedef struct
{
	REPEAT_MNG_WORK repeatWork;
	STA_POKE_WORK *pokeWork;
	
	fx32	height;
}POKE_ACT_JUMP_WORK;

SCRIPT_FUNC_DEF( PokeActionJump )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	STA_SCRIPT_SYS *work = scriptWork->sysWork;
	const s32 pokeNo = ScriptFunc_GetValueS32();
	const s32 interval = ScriptFunc_GetValueS32();
	const s32 num = ScriptFunc_GetValuefx32();
	const fx32 height = ScriptFunc_GetValuefx32();

	STA_POKE_SYS  *pokeSys = STA_ACT_GetPokeSys( work->actWork );
	STA_POKE_WORK *pokeWork = STA_ACT_GetPokeWork( work->actWork , (u8)pokeNo );
	POKE_ACT_JUMP_WORK	*jumpWork = GFL_HEAP_AllocMemory( work->heapId , sizeof(POKE_ACT_JUMP_WORK));
	
	jumpWork->pokeWork = pokeWork;
	jumpWork->height = height;
	
	jumpWork->repeatWork.actWork = work->actWork;
	jumpWork->repeatWork.step = 0;
	jumpWork->repeatWork.interval = interval;
	jumpWork->repeatWork.num = num;
	
	GFL_TCB_AddTask( work->tcbSys , SCRIPT_TCB_PokeAct_Jump , (void*)jumpWork , 10 );

	SCRIPT_PRINT_LABEL(PokeActionJump);
	return SFT_CONTINUE;

}

//�|�P�����A�N�V�����@�W�����v����
static void SCRIPT_TCB_PokeAct_Jump(  GFL_TCB *tcb, void *work )
{
	POKE_ACT_JUMP_WORK *jumpWork = (POKE_ACT_JUMP_WORK*)work;
	STA_POKE_SYS  *pokeSys = STA_ACT_GetPokeSys( jumpWork->repeatWork.actWork );
	
	const REPEAT_MNG_RETURN ret = SCRIPT_TCB_UpdateRepeat( &jumpWork->repeatWork );
	if( ret == RMR_END )
	{
		GFL_HEAP_FreeMemory( work );
		GFL_TCB_DeleteTask( tcb );
		return;
	}
	else
	{
		VecFx32 ofs;
		
		ofs.x = 0;
		ofs.y = -FX_Mul(FX_SinIdx( 0x8000 * jumpWork->repeatWork.step / jumpWork->repeatWork.interval ) , jumpWork->height );
		ofs.z = 0;
		
		STA_POKE_SetPositionOffset( pokeSys , jumpWork->pokeWork , &ofs );
		
	}
}

//------------------------------------------------------------------
//�I�u�W�F�N�g�n
//------------------------------------------------------------------
#pragma mark [>Object

//�I�u�W�F�N�g�쐬
SCRIPT_FUNC_DEF( ObjectCreate )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	STA_SCRIPT_SYS *work = scriptWork->sysWork;
	const s32 objNo = ScriptFunc_GetValueS32();
	const s32 objType = ScriptFunc_GetValueS32();
	
	STA_OBJ_SYS  *objSys = STA_ACT_GetObjectSys( work->actWork );
	STA_OBJ_WORK *objWork;

	objWork = STA_OBJ_CreateObject( objSys , objType );

	STA_ACT_SetObjectWork( work->actWork , objWork , objNo );

	SCRIPT_PRINT_LABEL(ObjectCreate);
	return SFT_CONTINUE;
}

//�I�u�W�F�N�g�j��
SCRIPT_FUNC_DEF( ObjectDelete )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	STA_SCRIPT_SYS *work = scriptWork->sysWork;
	const s32 objNo = ScriptFunc_GetValueS32();
	
	STA_OBJ_SYS  *objSys = STA_ACT_GetObjectSys( work->actWork );
	STA_OBJ_WORK *objWork = STA_ACT_GetObjectWork( work->actWork , objNo );
	
	STA_OBJ_DeleteObject( objSys , objWork );

	SCRIPT_PRINT_LABEL(ObjectDelete);
	return SFT_CONTINUE;
}

//�I�u�W�F�N�g�\��
SCRIPT_FUNC_DEF( ObjectShow )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	STA_SCRIPT_SYS *work = scriptWork->sysWork;
	const s32 objNo = ScriptFunc_GetValueS32();
	
	STA_OBJ_SYS  *objSys = STA_ACT_GetObjectSys( work->actWork );
	STA_OBJ_WORK *objWork = STA_ACT_GetObjectWork( work->actWork , objNo );
	
	STA_OBJ_SetShowFlg( objSys , objWork , TRUE );
	
	SCRIPT_PRINT_LABEL(ObjectShow);
	return SFT_CONTINUE;
}

//�I�u�W�F�N�g��\��
SCRIPT_FUNC_DEF( ObjectHide )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	STA_SCRIPT_SYS *work = scriptWork->sysWork;
	const s32 objNo = ScriptFunc_GetValueS32();
	
	STA_OBJ_SYS  *objSys = STA_ACT_GetObjectSys( work->actWork );
	STA_OBJ_WORK *objWork = STA_ACT_GetObjectWork( work->actWork , objNo );
	
	STA_OBJ_SetShowFlg( objSys , objWork , FALSE );
	
	SCRIPT_PRINT_LABEL(ObjectHide);
	return SFT_CONTINUE;
}

//�I�u�W�F�N�g�ړ�
SCRIPT_FUNC_DEF( ObjectMove )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	STA_SCRIPT_SYS *work = scriptWork->sysWork;
	const s32 objNo = ScriptFunc_GetValueS32();
	const s32 frame = ScriptFunc_GetValueS32();
	const fx32 posX = ScriptFunc_GetValuefx32();
	const fx32 posY = ScriptFunc_GetValuefx32();
	const fx32 posZ = ScriptFunc_GetValuefx32();
	
	STA_OBJ_SYS  *objSys = STA_ACT_GetObjectSys( work->actWork );
	STA_OBJ_WORK *objWork = STA_ACT_GetObjectWork( work->actWork , objNo );

	if( frame == 0 )
	{
		VecFx32 pos;
		VEC_Set( &pos , posX,posY,posZ );
		STA_OBJ_SetPosition( objSys , objWork , &pos );		
	}
	else
	{
		VecFx32 subVec;
		MOVE_OBJ_VEC *moveObj;
		moveObj = GFL_HEAP_AllocMemory( work->heapId , sizeof( MOVE_OBJ_VEC ));
		moveObj->objWork = objWork;
		moveObj->moveWork.actWork = work->actWork;
		moveObj->moveWork.step = 0;
		STA_OBJ_GetPosition( objSys , objWork , &moveObj->moveWork.start );
		VEC_Set( &moveObj->moveWork.end , posX,posY,posZ );
		moveObj->moveWork.frame = frame;

		VEC_Subtract( &moveObj->moveWork.end , &moveObj->moveWork.start , &subVec );
		moveObj->moveWork.stepVal.x = subVec.x / frame;
		moveObj->moveWork.stepVal.y = subVec.y / frame;
		moveObj->moveWork.stepVal.z = subVec.z / frame;

		GFL_TCB_AddTask( work->tcbSys , SCRIPT_TCB_MoveObjTCB , (void*)moveObj , 10 );
	}

	SCRIPT_PRINT_LABEL(ObjectMove);
	return SFT_CONTINUE;
}

//�I�u�W�F�N�g�ړ�����TCB
static void SCRIPT_TCB_MoveObjTCB(  GFL_TCB *tcb, void *work )
{
	MOVE_OBJ_VEC *moveObj = (MOVE_OBJ_VEC*)work;
	STA_OBJ_SYS  *objSys = STA_ACT_GetObjectSys( moveObj->moveWork.actWork );
	VecFx32 newPos;
	
	const BOOL isFinish = SCRIPT_TCB_UpdateMoveVec( &moveObj->moveWork , &newPos );
	STA_OBJ_SetPosition( objSys , moveObj->objWork , &newPos );
	
	if( isFinish == TRUE )
	{
		GFL_HEAP_FreeMemory( work );
		GFL_TCB_DeleteTask( tcb );
	}
}

//------------------------------------------------------------------
//	�G�t�F�N�g
//------------------------------------------------------------------
#pragma mark [>Effect
//�G�t�F�N�g�쐬
SCRIPT_FUNC_DEF( EffectCreate )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	STA_SCRIPT_SYS *work = scriptWork->sysWork;
	const s32 effectNo = ScriptFunc_GetValueS32();
	const s32 fileNo = ScriptFunc_GetValueS32();
	
	STA_EFF_SYS  *effSys = STA_ACT_GetEffectSys( work->actWork );
	STA_EFF_WORK *effWork = STA_EFF_CreateEffect( effSys , NARC_stage_gra_mus_eff_00_spa + fileNo );

	STA_ACT_SetEffectWork( work->actWork , effWork , effectNo );

	SCRIPT_PRINT_LABEL(EffectCreate);
	return SFT_CONTINUE;
}

//�G�t�F�N�g�j��
SCRIPT_FUNC_DEF( EffectDelete )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	STA_SCRIPT_SYS *work = scriptWork->sysWork;
	const s32 effectNo = ScriptFunc_GetValueS32();
	
	STA_EFF_SYS  *effSys = STA_ACT_GetEffectSys( work->actWork );
	STA_EFF_WORK *effWork = STA_ACT_GetEffectWork( work->actWork , effectNo );
	
	STA_EFF_DeleteEffect( effSys , effWork );

	SCRIPT_PRINT_LABEL(EffectDelete);
	return SFT_CONTINUE;
}

//�G�t�F�N�g�Đ�
SCRIPT_FUNC_DEF( EffectStart )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	STA_SCRIPT_SYS *work = scriptWork->sysWork;
	const s32 effectNo = ScriptFunc_GetValueS32();
	const s32 emiterNo = ScriptFunc_GetValueS32();
	const fx32 posX = ScriptFunc_GetValuefx32();
	const fx32 posY = ScriptFunc_GetValuefx32();
	const fx32 posZ = ScriptFunc_GetValuefx32();
	VecFx32	pos;

//	STA_EFF_SYS  *effSys = STA_ACT_GetEffectSys( work->actWork );
	STA_EFF_WORK *effWork = STA_ACT_GetEffectWork( work->actWork , effectNo );

	VEC_Set( &pos , ACT_POS_X_FX(posX),ACT_POS_Y_FX(posY),posZ );
	STA_EFF_CreateEmitter( effWork , emiterNo , &pos );

	SCRIPT_PRINT_LABEL(EffectStart);
	return SFT_CONTINUE;
}

//�G�t�F�N�g��~
SCRIPT_FUNC_DEF( EffectStop )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	STA_SCRIPT_SYS *work = scriptWork->sysWork;
	const s32 effectNo = ScriptFunc_GetValueS32();
	const s32 emiterNo = ScriptFunc_GetValueS32();

//	STA_EFF_SYS  *effSys = STA_ACT_GetEffectSys( work->actWork );
	STA_EFF_WORK *effWork = STA_ACT_GetEffectWork( work->actWork , effectNo );

	STA_EFF_DeleteEmitter( effWork , emiterNo );

	SCRIPT_PRINT_LABEL(EffectStop);
	return SFT_CONTINUE;
}

//�G�t�F�N�g�A���Đ�
typedef struct
{
	REPEAT_MNG_WORK repeatWork;
	STA_EFF_WORK	*effWork;
	VecFx32 pos;
	VecFx32 range;
	u16	emitNo;
}EFFECT_REPEAT_WORK;

SCRIPT_FUNC_DEF( EffectRepeatStart )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	STA_SCRIPT_SYS *work = scriptWork->sysWork;
	const s32 effectNo = ScriptFunc_GetValueS32();
	const s32 emiterNo = ScriptFunc_GetValueS32();
	const s32 interval = ScriptFunc_GetValueS32();
	const s32 num = ScriptFunc_GetValuefx32();
	const fx32 posXStart = ScriptFunc_GetValuefx32();
	const fx32 posXEnd = ScriptFunc_GetValuefx32();
	const fx32 posYStart = ScriptFunc_GetValuefx32();
	const fx32 posYEnd = ScriptFunc_GetValuefx32();
	const fx32 posZStart = ScriptFunc_GetValuefx32();
	const fx32 posZEnd = ScriptFunc_GetValuefx32();
	
//	STA_EFF_SYS  *effSys = STA_ACT_GetEffectSys( work->actWork );
	STA_EFF_WORK *effWork = STA_ACT_GetEffectWork( work->actWork , effectNo );
	
	EFFECT_REPEAT_WORK *effRepeat = GFL_HEAP_AllocMemory( work->heapId , sizeof(EFFECT_REPEAT_WORK));
	
	GF_ASSERT( posXStart <= posXEnd );
	GF_ASSERT( posYStart <= posYEnd );
	GF_ASSERT( posZStart <= posZEnd );

	effRepeat->effWork = effWork;
	effRepeat->emitNo = emiterNo;
	VEC_Set( &effRepeat->pos , posXStart,posYStart,posZStart );
	VEC_Set( &effRepeat->range , posXEnd-posXStart,posYEnd-posYStart,posZEnd-posZStart );
	
	effRepeat->repeatWork.actWork = work->actWork;
	effRepeat->repeatWork.step = 0;
	effRepeat->repeatWork.interval = interval;
	effRepeat->repeatWork.num = num;

	GFL_TCB_AddTask( work->tcbSys , SCRIPT_TCB_EffectRepeat , (void*)effRepeat , 10 );

	SCRIPT_PRINT_LABEL(EffectRepeatStart);
	return SFT_CONTINUE;
}
//�G�t�F�N�g�A���Đ�����
static void SCRIPT_TCB_EffectRepeat(  GFL_TCB *tcb, void *work )
{
	EFFECT_REPEAT_WORK *effRepeat = (EFFECT_REPEAT_WORK*)work;
//	STA_EFF_SYS  *effSys = STA_ACT_GetEffectSys( work->actWork );
	
	const REPEAT_MNG_RETURN ret = SCRIPT_TCB_UpdateRepeat( &effRepeat->repeatWork );
	if( ret == RMR_END )
	{
		GFL_HEAP_FreeMemory( work );
		GFL_TCB_DeleteTask( tcb );
		return;
	}
	else if( ret == RMR_ACTION )
	{
		VecFx32 pos;
		pos.x = ACT_POS_X_FX(effRepeat->pos.x + GFL_STD_MtRand0( effRepeat->range.x ));
		pos.y = ACT_POS_Y_FX(effRepeat->pos.y + GFL_STD_MtRand0( effRepeat->range.y ));
		pos.z = effRepeat->pos.z + GFL_STD_MtRand0( effRepeat->range.z );

		STA_EFF_CreateEmitter( effRepeat->effWork , effRepeat->emitNo , &pos );
	}
}

//------------------------------------------------------------------
//	���C�g
//------------------------------------------------------------------
#pragma mark [>SpotLight

//���C�gON(�~�`
SCRIPT_FUNC_DEF( LightShowCircle )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	STA_SCRIPT_SYS *work = scriptWork->sysWork;
	const s32 lightNo = ScriptFunc_GetValueS32();
	const fx32 rad = ScriptFunc_GetValueS32();
	
	STA_LIGHT_SYS  *lightSys = STA_ACT_GetLightSys( work->actWork );
	STA_LIGHT_WORK *lightWork;
	
	lightWork = STA_LIGHT_CreateObject( lightSys , ALT_CIRCLE );
	STA_LIGHT_SetColor( lightSys , lightWork , GX_RGB(31,31,0) , 16 );
	STA_LIGHT_SetOptionValue( lightSys , lightWork , rad , 0 );
	
	STA_ACT_SetLightWork( work->actWork , lightWork , lightNo );
	
	SCRIPT_PRINT_LABEL(LightShowCircle);
	return SFT_CONTINUE;
}

//���C�gOFF
SCRIPT_FUNC_DEF( LightHide )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	STA_SCRIPT_SYS *work = scriptWork->sysWork;
	const s32 lightNo = ScriptFunc_GetValueS32();

	STA_LIGHT_SYS  *lightSys = STA_ACT_GetLightSys( work->actWork );
	STA_LIGHT_WORK *lightWork = STA_ACT_GetLightWork( work->actWork , lightNo );
	
	STA_LIGHT_DeleteObject( lightSys , lightWork );

	SCRIPT_PRINT_LABEL(LightHide);
	return SFT_CONTINUE;
}

//���C�g�ړ�
SCRIPT_FUNC_DEF( LightMove )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	STA_SCRIPT_SYS *work = scriptWork->sysWork;
	const s32 lightNo = ScriptFunc_GetValueS32();
	const s32 frame = ScriptFunc_GetValueS32();
	const fx32 posX = ScriptFunc_GetValuefx32();
	const fx32 posY = ScriptFunc_GetValuefx32();
	//����Z�͌v�Z���ĂȂ��B�L���ɂ��邱�Ƃ͉\�����K�v�H
	const fx32 posZ = ScriptFunc_GetValuefx32();
	
	STA_LIGHT_SYS  *lightSys = STA_ACT_GetLightSys( work->actWork );
	STA_LIGHT_WORK *lightWork = STA_ACT_GetLightWork( work->actWork , lightNo );

	if( frame == 0 )
	{
		VecFx32 pos;
		VEC_Set( &pos , posX,posY,posZ );
		STA_LIGHT_SetPosition( lightSys , lightWork , &pos );
	}
	else
	{
		VecFx32 subVec;
		MOVE_LIGHT_VEC *moveLight;
		moveLight = GFL_HEAP_AllocMemory( work->heapId , sizeof( MOVE_LIGHT_VEC ));
		moveLight->lightWork = lightWork;
		moveLight->moveWork.actWork = work->actWork;
		moveLight->moveWork.step = 0;
		STA_LIGHT_GetPosition( lightSys , lightWork , &moveLight->moveWork.start );
		VEC_Set( &moveLight->moveWork.end , posX,posY,posZ );
		moveLight->moveWork.frame = frame;

		VEC_Subtract( &moveLight->moveWork.end , &moveLight->moveWork.start , &subVec );
		moveLight->moveWork.stepVal.x = subVec.x / frame;
		moveLight->moveWork.stepVal.y = subVec.y / frame;
		moveLight->moveWork.stepVal.z = subVec.z / frame;

		GFL_TCB_AddTask( work->tcbSys , SCRIPT_TCB_MoveLightTCB , (void*)moveLight , 10 );
	}
	
	SCRIPT_PRINT_LABEL(LightMove);
	return SFT_CONTINUE;
}

//���C�g�ړ�����TCB
static void SCRIPT_TCB_MoveLightTCB(  GFL_TCB *tcb, void *work )
{
	MOVE_LIGHT_VEC *moveLight = (MOVE_LIGHT_VEC*)work;
	STA_LIGHT_SYS  *lightSys = STA_ACT_GetLightSys( moveLight->moveWork.actWork );
	VecFx32 newPos;
	
	const BOOL isFinish = SCRIPT_TCB_UpdateMoveVec( &moveLight->moveWork , &newPos );
	STA_LIGHT_SetPosition( lightSys , moveLight->lightWork , &newPos );
	
	if( isFinish == TRUE )
	{
		GFL_HEAP_FreeMemory( work );
		GFL_TCB_DeleteTask( tcb );
	}
}

//���C�g�F�ݒ�
SCRIPT_FUNC_DEF( LightColor )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	STA_SCRIPT_SYS *work = scriptWork->sysWork;
	const s32 lightNo = ScriptFunc_GetValueS32();
	const s32 colR = ScriptFunc_GetValueS32();
	const s32 colG = ScriptFunc_GetValueS32();
	const s32 colB = ScriptFunc_GetValueS32();
	const s32 alpha = ScriptFunc_GetValuefx32();
	const GXRgb col = GX_RGB( (u8)colR , (u8)colG , (u8)colB );
	
	STA_LIGHT_SYS  *lightSys = STA_ACT_GetLightSys( work->actWork );
	STA_LIGHT_WORK *lightWork = STA_ACT_GetLightWork( work->actWork , lightNo );

	STA_LIGHT_SetColor( lightSys , lightWork , col , alpha );
	
	SCRIPT_PRINT_LABEL(LightColor);
	return SFT_CONTINUE;
}

//------------------------------------------------------------------
//	���b�Z�[�W
//------------------------------------------------------------------
#pragma mark [>Message

//���b�Z�[�W�\��
SCRIPT_FUNC_DEF( MessageShow )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	STA_SCRIPT_SYS *work = scriptWork->sysWork;
	const s32 msgNo = ScriptFunc_GetValueS32();
	const s32 dispSpd = ScriptFunc_GetValueS32();

	STA_ACT_ShowMessage( work->actWork , msgNo , dispSpd );
	SCRIPT_PRINT_LABEL(MessageShow);
	return SFT_CONTINUE;
}

//���b�Z�[�W����
SCRIPT_FUNC_DEF( MessageHide )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	STA_SCRIPT_SYS *work = scriptWork->sysWork;
	
	STA_ACT_HideMessage( work->actWork );
	SCRIPT_PRINT_LABEL(MessageHide);
	return SFT_CONTINUE;
}

//���b�Z�[�W����
SCRIPT_FUNC_DEF( MessageColor )
{
	STA_SCRIPT_WORK *scriptWork = (STA_SCRIPT_WORK*)context_work;
	STA_SCRIPT_SYS *work = scriptWork->sysWork;
	const s32 msgCol = ScriptFunc_GetValueS32();
	const s32 shadowCol = ScriptFunc_GetValueS32();
	const s32 backNo = ScriptFunc_GetValueS32();
	GFL_FONTSYS_SetColor( msgCol,shadowCol,backNo );

	SCRIPT_PRINT_LABEL(MessageColor);
	return SFT_CONTINUE;
}

#undef SCRIPT_FUNC_DEF
