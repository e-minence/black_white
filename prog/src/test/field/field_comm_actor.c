//======================================================================
/**
 *
 * @file	field_comm_actor.c
 * @brief	
 * @author
 * @data
 *
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "net/network_define.h"
#include "arc_def.h"

#include "field_main.h"
#include "field_common.h"
#include "field_actor.h"

#include "field_comm_actor.h"
#include "test_graphic/fld_act.naix"

//======================================================================
//	define
//======================================================================
//--------------------------------------------------------------
///	�r���{�[�h�A�N�^�[�A�j���ԍ�
//--------------------------------------------------------------
enum
{
	PCACTSTOP_UP = 0,
	PCACTSTOP_DOWN,
	PCACTSTOP_LEFT,
	PCACTSTOP_RIGHT,

	PCACTWALK_UP,
	PCACTWALK_DOWN,
	PCACTWALK_LEFT,
	PCACTWALK_RIGHT,

	PCACTRUN_UP,
	PCACTRUN_DOWN,
	PCACTRUN_LEFT,
	PCACTRUN_RIGHT,

	PCACTJUMP_UP,
	PCACTJUMP_DOWN,
	PCACTJUMP_LEFT,
	PCACTJUMP_RIGHT,

	PCACTANMNO_MAX,
};

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
///	FLD_COMM_ACTOR
//--------------------------------------------------------------
struct _TAG_FLD_COMM_ACTOR
{
	const PLAYER_WORK *player;

	HEAPID heapID;
	u16 dir;
	u16 old_dir;
	int anm_id;
	VecFx32 pos;
	VecFx32 old_pos;
	
	GFL_BBDACT_SYS *bbdActSys;
	GFL_BBDACT_RESUNIT_ID	bbdActResUnitID;
	GFL_BBDACT_ACTUNIT_ID	bbdActActUnitID;
};

//======================================================================
//	proto
//======================================================================
static void commActBBDActFunc(
	GFL_BBDACT_SYS *bbdActSys, int actIdx, void* work );

static const GFL_BBDACT_ANM *playerBBDactAnmTable[PCACTANMNO_MAX];

//======================================================================
//	�ʐM�p�A�N�^�[
//======================================================================
//--------------------------------------------------------------
/**
 * �ʐM�p�A�N�^�[�@������
 * @param	player		�Q�Ƃ���PLAYER_WORK *
 * @param	bbdActSys	�g�p����GFL_BBDACT_SYS
 * @param	resUnitID	�g�p����GFL_BBDACT_RESUNIT_ID
 * @param	heapID		���\�[�X�m�ۗp�q�[�vID
 * @retval	FLD_COMM_ACTOR	�ǉ����ꂽFLD_COMM_ACTOR *
 */
//--------------------------------------------------------------
FLD_COMM_ACTOR * FldCommActor_Init(
	const PLAYER_WORK *player,
	GFL_BBDACT_SYS *bbdActSys,
	GFL_BBDACT_RESUNIT_ID resUnitID, HEAPID heapID )
{
	u32 dir;
	FLD_COMM_ACTOR *act;
	GFL_BBDACT_ACTDATA actData;
	const VecFx32 *pos;
	
	pos = PLAYERWORK_getPosition( player );
	dir = PLAYERWORK_getDirection( player );

	act = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLD_COMM_ACTOR) );
	act->heapID = heapID;
	act->dir = dir;
	act->old_dir = DIR_NOT;
	act->pos = *pos;
	act->old_pos = *pos;
	act->bbdActSys = bbdActSys;
	act->bbdActResUnitID = resUnitID;
	act->anm_id = -1;

	actData.resID = 0;
	actData.sizX = FX16_ONE*8-1;
	actData.sizY = FX16_ONE*8-1;
	actData.trans.x = 0;
	actData.trans.y = 0;
	actData.trans.z = 0;
	actData.alpha = 31;
	actData.drawEnable = TRUE;
	actData.func = commActBBDActFunc;
	actData.work = act;
	
	act->bbdActActUnitID =
		GFL_BBDACT_AddAct( bbdActSys, resUnitID, &actData, 1 ); 

	GFL_BBDACT_BindActTexResLoad(
		bbdActSys, act->bbdActActUnitID,
		ARCID_FLDMAP_ACTOR, NARC_fld_act_hero_nsbtx );
	GFL_BBDACT_SetAnimeTable(
		bbdActSys, act->bbdActActUnitID,
		playerBBDactAnmTable, PCACTANMNO_MAX );
	GFL_BBDACT_SetAnimeIdxOn( bbdActSys, act->bbdActActUnitID, 0 );

	return( act );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�ʐM�p�A�N�^�[�@�폜
 * @param	
 * @retval
 */
//--------------------------------------------------------------
void FldCommActor_Delete( FLD_COMM_ACTOR *act )
{
	GFL_HEAP_FreeMemory( act );
}

//======================================================================
//	�p�[�c
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h�ʐM�p�A�N�^�[�@�X�V
 * @param	act	FLD_COMM_ACTOR *
 * @param	pos �\�����W
 * @param	dir �\������
 * @retval	nothing
 */
//--------------------------------------------------------------
void FldCommActor_Update( FLD_COMM_ACTOR *act )
{
	u16 dir;
	int anmBase,anmID;
	const VecFx32 *pos;
	
	pos = PLAYERWORK_getPosition( act->player );
	dir = PLAYERWORK_getDirection( act->player );
	
	act->old_pos = act->pos;
	act->pos = *pos;
	act->old_dir = act->dir;
	act->dir = dir;
	
	if( act->pos.x != act->old_pos.x ||
		act->pos.y != act->old_pos.y ||
		act->pos.z != act->old_pos.z ){
		anmBase = 4;
	}else{
		anmBase = 0;
	}
	
	anmID = anmBase + dir;
	
	if( act->anm_id != anmID ){
		act->anm_id = anmID;
		GFL_BBDACT_SetAnimeIdx(
			act->bbdActSys, act->bbdActActUnitID, anmID );
	}
}

//======================================================================
///	�r���{�[�h�A�N�^�[
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h�ʐM�p�A�N�^�[�@�A�N�^�[����
 * @param	
 * @retval
 */
//--------------------------------------------------------------
static void commActBBDActFunc(
	GFL_BBDACT_SYS *bbdActSys, int actIdx, void* work )
{
	VecFx32 pos;
	FLD_COMM_ACTOR *act = work;
	
	pos = act->pos;
	pos.x += FX32_ONE * 8;
	pos.y += FX32_ONE * 10;
//	pos.z += 1;

	GFL_BBD_SetObjectTrans(
		GFL_BBDACT_GetBBDSystem(act->bbdActSys),
		act->bbdActActUnitID, &pos );
}

//======================================================================
//	data
//======================================================================
//--------------------------------------------------------------
///	�r���{�[�h�A�N�^�[�A�j��
//--------------------------------------------------------------
static const GFL_BBDACT_ANM PCstopLAnm[] = {
	{ 2, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ GFL_BBDACT_ANMCOM_JMP, 0, 0, 0 },
};
static const GFL_BBDACT_ANM PCstopRAnm[] = {
	{ 2, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ GFL_BBDACT_ANMCOM_JMP, 0, 0, 0 },
};
static const GFL_BBDACT_ANM PCstopUAnm[] = {
	{ 0, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ GFL_BBDACT_ANMCOM_JMP, 0, 0, 0 },
};
static const GFL_BBDACT_ANM PCstopDAnm[] = {
	{ 21, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ GFL_BBDACT_ANMCOM_JMP, 0, 0, 0 },
};

static const GFL_BBDACT_ANM PCwalkLAnm[] = {
	{ 1, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 2, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 3, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 2, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ GFL_BBDACT_ANMCOM_JMP, 0, 0, 0 },
};
static const GFL_BBDACT_ANM PCwalkRAnm[] = {
	{ 1, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 2, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 3, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 2, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ GFL_BBDACT_ANMCOM_JMP, 0, 0, 0 },
};
static const GFL_BBDACT_ANM PCwalkUAnm[] = {
	{ 9, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 0, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 20, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 0, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ GFL_BBDACT_ANMCOM_JMP, 0, 0, 0 },
};
static const GFL_BBDACT_ANM PCwalkDAnm[] = {
	{ 22, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 21, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 23, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 21, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ GFL_BBDACT_ANMCOM_JMP, 0, 0, 0 },
};

static const GFL_BBDACT_ANM PCrunLAnm[] = {
	{ 15, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 14, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 16, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 14, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ GFL_BBDACT_ANMCOM_JMP, 0, 0, 0 },
};
static const GFL_BBDACT_ANM PCrunRAnm[] = {
	{ 15, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 14, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 16, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 14, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ GFL_BBDACT_ANMCOM_JMP, 0, 0, 0 },
};
static const GFL_BBDACT_ANM PCrunUAnm[] = {
	{ 8, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 7, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 10, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 7, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ GFL_BBDACT_ANMCOM_JMP, 0, 0, 0 },
};
static const GFL_BBDACT_ANM PCrunDAnm[] = {
	{ 12, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 11, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 13, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 11, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ GFL_BBDACT_ANMCOM_JMP, 0, 0, 0 },
};

static const GFL_BBDACT_ANM PCjumpLAnm[] = {
	{ 15, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ GFL_BBDACT_ANMCOM_END, 0, 0, 0 },
};
static const GFL_BBDACT_ANM PCjumpRAnm[] = {
	{ 15, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ GFL_BBDACT_ANMCOM_END, 0, 0, 0 },
};
static const GFL_BBDACT_ANM PCjumpUAnm[] = {
	{ 8, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ GFL_BBDACT_ANMCOM_END, 0, 0, 0 },
};
static const GFL_BBDACT_ANM PCjumpDAnm[] = {
	{ 12, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ GFL_BBDACT_ANMCOM_END, 0, 0, 0 },
};

static const GFL_BBDACT_ANM* playerBBDactAnmTable[] = { 
	PCstopUAnm, PCstopDAnm, PCstopLAnm, PCstopRAnm,
	PCwalkUAnm, PCwalkDAnm, PCwalkLAnm, PCwalkRAnm,
	PCrunUAnm, PCrunDAnm, PCrunLAnm, PCrunRAnm,
	PCjumpUAnm, PCjumpDAnm, PCjumpLAnm, PCjumpRAnm,
};
