//============================================================================================
/**
 * @file	skill_cont.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"

#include "setup.h"
#include "team_cont.h"
#include "skill_cont.h"

//============================================================================================
//
//
//	�X�L���R���g���[��
//
//
//============================================================================================
#define	SKILL_SETUP_MAX	(64)
//------------------------------------------------------------------
/**
 * @brief	�\���̒�`
 */
//------------------------------------------------------------------
typedef struct {
	PLAYER_SKILL_COMMAND	skillCommand;
	u16						seq;
	TEAM_CONTROL*			tc;			//�����������v���[���[�̃`�[��
	PLAYER_CONTROL*			pc;			//�����������v���[���[
	VecFx32					trans;
	u16						direction;
	u32						work[16];
}SKILL_WORK;

typedef struct {
	SKILL_WORK*		sw;
	u16				work[8];
}SKILL_EFFECT_WORK;

struct _SKILL_CONTROL {
	HEAPID			heapID;
	GAME_SYSTEM*	gs;
	TEAM_CONTROL**	p_tc;
	int				teamCount;

	SKILL_WORK		skill[SKILL_SETUP_MAX];
	u16				g3DutilUnitIdx;
	u16				g3DutilObjIdx;
	BOOL			onGameFlag;
};

typedef void	(SKILL_FUNC)( SKILL_CONTROL*, SKILL_WORK* );

typedef struct {
	SKILL_FUNC*	init;
	SKILL_FUNC*	main;
}SKILL_PROC;

//------------------------------------------------------------------
/**
 * @brief	�R�c�Z�b�g�A�b�v�f�[�^
 */
//------------------------------------------------------------------
#include "graphic_data/test_graphic/test3d.naix"

//------------------------------------------------------------------
enum {
	G3DRES_EFFECT_ARROW = 0,
};

//�R�c�O���t�B�b�N���\�[�X�e�[�u��
static const GFL_G3D_UTIL_RES g3DresTbl[] = {
	{ ARCID_SAMPLE, NARC_haruka_effect_arrow_nsbmd,  GFL_G3D_UTIL_RESARC },
};

//------------------------------------------------------------------
enum {
	G3DOBJ_EFFECT_ARROW = 0,
};

//�R�c�I�u�W�F�N�g��`�e�[�u��
static const GFL_G3D_UTIL_OBJ g3DobjTbl[] = {
	{ G3DRES_EFFECT_ARROW, 0, G3DRES_EFFECT_ARROW, NULL, 0 },
};

//------------------------------------------------------------------
//�ݒ�e�[�u���f�[�^
static const GFL_G3D_UTIL_SETUP g3Dsetup = {
	g3DresTbl, NELEMS(g3DresTbl),
	g3DobjTbl, NELEMS(g3DobjTbl),
};

//------------------------------------------------------------------
static void moveTraceSkill( GFL_G3D_SCENEOBJ* sceneObj, void* work );

typedef struct {
	const GFL_G3D_SCENEOBJ_DATA*	data;
	int								count;
}SKILL_EFFECT_TBL;

static const GFL_G3D_SCENEOBJ_DATA skillEffectData1[] = {
	{ 
		G3DOBJ_EFFECT_ARROW, 0, 1, 24, FALSE, TRUE, 
		{	{ 0, 0, 0 },
			{ FX32_ONE/8 , FX32_ONE/8, FX32_ONE/8 },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},moveTraceSkill,
	},
};
static const GFL_G3D_SCENEOBJ_DATA skillEffectData2[] = {
	{ 
		G3DOBJ_EFFECT_ARROW, 0, 1, 24, FALSE, TRUE, 
		{	{ 0, 0, 0 },
			{ FX32_ONE , FX32_ONE, FX32_ONE },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},moveTraceSkill,
	},
};
static const GFL_G3D_SCENEOBJ_DATA skillEffectData3[] = {
	{ 
		G3DOBJ_EFFECT_ARROW, 0, 1, 24, FALSE, TRUE, 
		{	{ 0, 0, 0 },
			{ FX32_ONE/2 , FX32_ONE/2, FX32_ONE/2 },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},moveTraceSkill,
	},
};

typedef enum {
	SKILL_EFFECT_TEST1 = 0,
	SKILL_EFFECT_TEST2,
	SKILL_EFFECT_TEST3,
}SKILL_EFFECTID;

static const SKILL_EFFECT_TBL skillEffectTable[] = {
	{ skillEffectData1, NELEMS(skillEffectData1) },
	{ skillEffectData2, NELEMS(skillEffectData2) },
	{ skillEffectData3, NELEMS(skillEffectData3) },
};

static const SKILL_PROC skillProcTable[8];
//------------------------------------------------------------------
/**
 * @brief	�X�L���R���g���[���Z�b�g
 */
//------------------------------------------------------------------
SKILL_CONTROL* AddSkillControl( SKILLCONT_SETUP* setup )
{
	SKILL_CONTROL* sc = GFL_HEAP_AllocClearMemory( setup->heapID, sizeof(SKILL_CONTROL) );
	TEAM_CONTROL** p_tc = GFL_HEAP_AllocClearMemory
							( setup->heapID, setup->teamCount * sizeof(TEAM_CONTROL*) );
	int i, j;

	for( i=0; i<setup->teamCount; i++ ){
		p_tc[i] = setup->p_tc[i];
	}

	sc->heapID = setup->heapID;
	sc->gs = setup->gs;
	sc->p_tc = p_tc;
	sc->teamCount = setup->teamCount;

	for( i=0; i<SKILL_SETUP_MAX; i++ ){
		sc->skill[i].skillCommand = PSC_NOP;
		sc->skill[i].seq = 0;
		sc->skill[i].pc = NULL;
		for( j=0; j<8; j++ ){
			sc->skill[i].work[j] = 0;
		}
	}
	//�R�c�f�[�^�Z�b�g�A�b�v�e�X�g
	{
		GFL_G3D_SCENE* g3Dscene = Get_GS_G3Dscene( sc->gs );
		sc->g3DutilUnitIdx = GFL_G3D_SCENE_AddG3DutilUnit( g3Dscene, &g3Dsetup );
		sc->g3DutilObjIdx = GFL_G3D_SCENE_GetG3DutilUnitObjIdx( g3Dscene, sc->g3DutilUnitIdx );
	}
	return sc;
}

//------------------------------------------------------------------
/**
 * @brief	�X�L���R���g���[���I��
 */
//------------------------------------------------------------------
void RemoveSkillControl( SKILL_CONTROL* sc )
{
	GFL_G3D_SCENE_DelG3DutilUnit( Get_GS_G3Dscene( sc->gs ), sc->g3DutilUnitIdx );

	GFL_HEAP_FreeMemory( sc->p_tc ); 
	GFL_HEAP_FreeMemory( sc ); 
}

//------------------------------------------------------------------
/**
 * @brief	�X�L���̒ǉ�
 */
//------------------------------------------------------------------
void SetSkillControlCommand
	( SKILL_CONTROL* sc, TEAM_CONTROL* tc, PLAYER_CONTROL* pc, PLAYER_SKILL_COMMAND skillCommand )
{
	int i;

	if( skillCommand ){
		for( i=0; i<SKILL_SETUP_MAX; i++ ){
			if( sc->skill[i].skillCommand == PSC_NOP ){
				sc->skill[i].skillCommand = skillCommand;
				sc->skill[i].seq = 0;
				sc->skill[i].tc = tc;
				sc->skill[i].pc = pc;			//�����������v���[���[
				GetPlayerControlTrans( pc, &sc->skill[i].trans );
				GetPlayerDirection( pc, &sc->skill[i].direction );

				SetPlayerSkillBusyFlag( pc );
				( skillProcTable[sc->skill[i].skillCommand].init )( sc, &sc->skill[i] );
				return;
			}
		}
		GF_ASSERT(0);
	}
}

//------------------------------------------------------------------
/**
 * @brief	�X�L�����C���R���g���[��
 */
//------------------------------------------------------------------
void MainSkillControl( SKILL_CONTROL* sc, BOOL onGameFlag )
{
	int i;

	sc->onGameFlag = onGameFlag;

	for( i=0; i<SKILL_SETUP_MAX; i++ ){
		if( sc->skill[i].skillCommand != PSC_NOP ){
			( skillProcTable[sc->skill[i].skillCommand].main )( sc, &sc->skill[i] );
		}
	}
}





//------------------------------------------------------------------
/**
 *
 *
 *
 * @brief	�X�L���v���O����
 *
 *
 *
 */
//------------------------------------------------------------------
static void calc_VecDir( u16 direction, VecFx32* vecDir )
{
	vecDir->x = -FX_SinIdx( direction );
	vecDir->y = 0;
	vecDir->z = -FX_CosIdx( direction );
}

//------------------------------------------------------------------
//	
//
//	�e��X�L���ʊ֐�
//
//
//------------------------------------------------------------------
static void DeleteSkill( SKILL_WORK* sw )
{
	sw->skillCommand = PSC_NOP;
	ResetPlayerSkillBusyFlag( sw->pc );
}
	
//------------------------------------------------------------------
//�@�Ȃɂ����Ȃ�
//------------------------------------------------------------------
static void NopInit( SKILL_CONTROL* sc, SKILL_WORK* sw )
{
}

static void NopMain( SKILL_CONTROL* sc, SKILL_WORK* sw )
{
	DeleteSkill( sw );
}

//------------------------------------------------------------------
/**
 * @brief	�X�L���e�[�u��
 */
//------------------------------------------------------------------
static const SKILL_PROC skillProcTable[8] = {
	{ NopInit,		NopMain },
	{ NopInit,		NopMain },
	{ NopInit,		NopMain },
	{ NopInit,		NopMain },
	{ NopInit,		NopMain },
	{ NopInit,		NopMain },
	{ NopInit,		NopMain },
	{ NopInit,		NopMain },
};


