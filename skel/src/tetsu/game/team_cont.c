//============================================================================================
/**
 * @file	team_cont.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"

#include "setup.h"

#include "team_cont.h"
//============================================================================================
//
//
//	�`�[���R���g���[��
//
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�\���̒�`
 */
//------------------------------------------------------------------
#define CASTLE_EMPTY			(0)
#define SUMMON_EMPTY			(0)
#define PLAYER_EMPTY			(NULL)

typedef struct {
	u16			type;
	u16			hpNow;
	u16			hpMax;
	VecFx32		trans;
	int			objID;
}SUMMON_STATUS;

typedef struct {
	u16			type;
	u16			hpNow;
	u16			hpMax;
	VecFx32		trans;
	int			objID;
}CASTLE_STATUS;

struct _TEAM_CONTROL {
	HEAPID			heapID;
	GAME_SYSTEM*	gs;

	CASTLE_STATUS	castle;

	SUMMON_STATUS	summon[TEAM_SUMMON_COUNT_MAX];

	PLAYER_CONTROL*	player[TEAM_PLAYER_COUNT_MAX];
	int				playerCount;

	GFL_BMP_DATA*	mapArea;
	BOOL			mapAreaDrawFlag;
	u32				mapAreaScore;
};

typedef struct {
	u16			type;
	u16			hp;
	u16			areaIconType;
	u16			radius;
}SUMMON_TYPETABLE;

#define MAPAREA_SIZX	(16)	//�L�����P��
#define MAPAREA_SIZY	(16)	//�L�����P��

#define MAPAREAMASK_COL	(3)//(PLT_2D_COL_BLACK)
					

static void CalcCastleDamage( TEAM_CONTROL* gc, int* damage );
static const SUMMON_TYPETABLE	summonTypeTable[9];

//------------------------------------------------------------------
/**
 * @brief	�`�[���R���g���[���N��
 */
//------------------------------------------------------------------
TEAM_CONTROL* AddTeamControl( GAME_SYSTEM* gs, HEAPID heapID )
{
	int i;
	TEAM_CONTROL* tc = GFL_HEAP_AllocClearMemory( heapID, sizeof(TEAM_CONTROL) );
	tc->heapID = heapID;
	tc->gs = gs;
	tc->playerCount = 0;

	tc->castle.type = CASTLE_EMPTY;

	for( i=0; i<TEAM_SUMMON_COUNT_MAX; i++ ){ 
		tc->summon[i].type = SUMMON_EMPTY;
	}

	for( i=0; i<TEAM_PLAYER_COUNT_MAX; i++ ){ 
		tc->player[i] = PLAYER_EMPTY;
	}
	tc->mapArea = GFL_BMP_Create( MAPAREA_SIZX, MAPAREA_SIZY, GFL_BMP_16_COLOR, heapID );
	GFL_BMP_Clear( tc->mapArea, MAPAREAMASK_COL );
#if 1
	//�w�i�F�p���b�g�쐬���]��
	{
		u16* plt = GFL_HEAP_AllocClearMemoryLo( heapID, 16*2 );
		plt[2] = 0x07c0;
		plt[3] = 0x05e0;
		GFL_BG_LoadPalette( GFL_BG_FRAME0_S, plt, 8*2, 16*2 );	//�T�u��ʂ̔w�i�F�]��
		GFL_HEAP_FreeMemory( plt );
	}
#endif
	tc->mapAreaDrawFlag = TRUE;
	return tc;
}

//------------------------------------------------------------------
/**
 * @brief	�`�[���R���g���[���I��
 */
//------------------------------------------------------------------
void RemoveTeamControl( TEAM_CONTROL* tc )
{
	int i;

	GFL_BMP_Delete( tc->mapArea );
	
	for( i=0; i<TEAM_PLAYER_COUNT_MAX; i++ ){ 
		ReleasePlayer( tc, i );
	}
	GFL_HEAP_FreeMemory( tc ); 
}

//------------------------------------------------------------------
/**
 * @brief	�`�[�����C���R���g���[��
 */
//------------------------------------------------------------------
void MainTeamControl( TEAM_CONTROL* tc, BOOL onGameFlag )
{
	int i, j;
#if 0
	//�v���[���[����
	{
		PLAYER_CONTROL* pc;
		SUMMON_STATUS*	ss;
		PLAYER_BUILD_COMMAND buildCommand;
		VecFx32 setTrans;
		for( i=0; i<TEAM_PLAYER_COUNT_MAX; i++ ){ 
			if( tc->player[i] != PLAYER_EMPTY ){
				pc = tc->player[i];
			
				MainPlayerControl( pc );
				buildCommand = GetPlayerBuildCommand( pc );
	
				if( onGameFlag == TRUE ){
	
					if( buildCommand == PBC_SUMMON ){
						GetPlayerControlTrans( pc, &setTrans );
						if( GetMapAreaMaskStatus( tc, &setTrans ) == TRUE ){
							if( CreateSummonObject( tc, 1, &setTrans ) != -1 ){
								MakeTeamMapAreaMask( tc );
							}
						}
					}
					if( GetPlayerDeadFlag( pc ) == TRUE ){	//���S���̋��_�_���[�W
						int deadDamage = 100;
	
						SetCastleDamage( tc, &deadDamage );
						ResetPlayerDeadFlag( pc );
					}
				} else {
					if( buildCommand == PBC_BUILD_CASTLE ){
						GetPlayerControlTrans( pc, &setTrans );
						if( CreateCastle( tc, &setTrans ) != -1 ){
							MakeTeamMapAreaMask( tc );
						}
					}
				}
				ResetPlayerBuildCommand( pc );
				ResetPlayerDeadFlag( pc );
			}
		}
	}
	//����������
	if( onGameFlag == TRUE ){
		int summonHp, summonHpMax;

		for( i=0; i<TEAM_SUMMON_COUNT_MAX; i++ ){ 
			if( tc->summon[i].type != SUMMON_EMPTY ){
				GetSummonObjectHP( tc, i, &summonHp, &summonHpMax );
				if( summonHp == 0 ){
					DeleteSummonObject( tc, i );
					MakeTeamMapAreaMask( tc );
				}
			}
		}
	}
#endif
}
	
//------------------------------------------------------------------
/**
 * @brief	�v���[���[�o�^
 */
//------------------------------------------------------------------
int RegistPlayer( TEAM_CONTROL* tc, PLAYER_STATUS* ps, int playerObjID, int netID, VecFx32* trans )
{
	int	i;

	for( i=0; i<TEAM_PLAYER_COUNT_MAX; i++ ){
		if( tc->player[i] == PLAYER_EMPTY ){
			tc->player[i] = AddPlayerControl(	tc->gs,
												G3DSCOBJ_PLAYER1 + playerObjID,
												netID,
												tc->heapID );
			SetPlayerStatus( tc->player[i], ps ); 
			SetPlayerControlTrans( tc->player[i], trans );
			tc->playerCount++;
			return i;
		}
	}
	return -1;
}

//------------------------------------------------------------------
/**
 * @brief	�v���[���[�o�^�폜
 */
//------------------------------------------------------------------
void ReleasePlayer( TEAM_CONTROL* tc, int playerID )
{
	if( tc->player[playerID] != PLAYER_EMPTY ){
		RemovePlayerControl( tc->player[playerID] ); 
		tc->player[playerID] = PLAYER_EMPTY;
		tc->playerCount--;
	}
}

//------------------------------------------------------------------
/**
 * @brief	�v���[���[�o�^���擾
 */
//------------------------------------------------------------------
int GetTeamPlayerCount( TEAM_CONTROL* tc )
{
	return tc->playerCount;
}

//------------------------------------------------------------------
/**
 * @brief	�v���[���[�R���g���[���擾
 */
//------------------------------------------------------------------
PLAYER_CONTROL* GetTeamPlayerControl( TEAM_CONTROL* tc, int playerID )
{
	if( tc->player[playerID] != PLAYER_EMPTY ){
		return tc->player[playerID];
	}
	return NULL;
}

//------------------------------------------------------------------
/**
 * @brief	���ׂĂ̓o�^�v���[���[�̃R���g���[���ɑ΂��ăR�[���o�b�N���������s����
 */
//------------------------------------------------------------------
void ProcessingAllTeamPlayer( TEAM_CONTROL* tc, TEAM_PLAYER_CALLBACK* proc, void* work )
{
	int i, num;

	for( i=0, num=0; i<TEAM_PLAYER_COUNT_MAX; i++ ){
		if( tc->player[i] != PLAYER_EMPTY ){
			proc( tc->player[i], num, work );
			num++;
		}
	}
}

//------------------------------------------------------------------
/**
 * @brief	���_�G���A�_���[�W�擾
 */
//------------------------------------------------------------------
void GetCastleAreaDamage( TEAM_CONTROL* tc, int* damage )
{
	*damage = tc->mapAreaScore/100;
}

//------------------------------------------------------------------
/**
 * @brief	���_�_���[�W�ݒ�
 */
//------------------------------------------------------------------
void SetCastleDamage( TEAM_CONTROL* tc, int* damage )
{
	CASTLE_STATUS* cs = &tc->castle;

	if( cs->type == CASTLE_EMPTY ){
		return;
	}
	if( cs->hpNow < *damage ){
		cs->hpNow = 0;
	} else {
		cs->hpNow -= *damage;
	}
}

//------------------------------------------------------------------
/**
 * @brief	���_�ݒu
 */
//------------------------------------------------------------------
int CreateCastle( TEAM_CONTROL* tc, VecFx32* trans )
{
	CASTLE_STATUS* cs = &tc->castle;

	if( cs->type == CASTLE_EMPTY ){
		cs->objID	= AddObject3Dmap( Get_GS_SceneMap( tc->gs ), EXMAPOBJ_WALL, trans );
		cs->type	= 1;
		cs->hpMax	= 50000;
		cs->hpNow	= cs->hpMax;
		cs->trans	= *trans;
		return 0;
	}
	return -1;
}

//------------------------------------------------------------------
/**
 * @brief	���_�폜
 */
//------------------------------------------------------------------
void DeleteCastle( TEAM_CONTROL* tc )
{
	CASTLE_STATUS* cs = &tc->castle;

	if( cs->type == CASTLE_EMPTY ){
		RemoveObject3Dmap( Get_GS_SceneMap( tc->gs ), cs->objID ); 
		cs->type = CASTLE_EMPTY;
	}
}

//------------------------------------------------------------------
/**
 * @brief	���_�g�o�擾
 */
//------------------------------------------------------------------
BOOL GetCastleHP( TEAM_CONTROL* tc, int* hp, int* hpMax )
{
	CASTLE_STATUS* cs = &tc->castle;

	if( cs->type == CASTLE_EMPTY ){
		return FALSE;
	} else {
		*hp = cs->hpNow;
		*hpMax = cs->hpMax;
		return TRUE;
	}
}

//------------------------------------------------------------------
/**
 * @brief	���_���W�擾
 */
//------------------------------------------------------------------
BOOL GetCastleTrans( TEAM_CONTROL* tc, VecFx32* trans )
{
	CASTLE_STATUS* cs = &tc->castle;

	if( cs->type == CASTLE_EMPTY ){
		return FALSE;
	} else {
		*trans = cs->trans;
		return TRUE;
	}
}

//------------------------------------------------------------------
/**
 * @brief	�������ݒu
 */
//------------------------------------------------------------------
int CreateSummonObject( TEAM_CONTROL* tc, int summonTypeID, VecFx32* trans )
{
	int	i;
	SUMMON_STATUS* ss;

	GF_ASSERT( summonTypeID < NELEMS(summonTypeTable) );

	for( i=0; i<TEAM_SUMMON_COUNT_MAX; i++ ){
		ss = &tc->summon[i];

		if( ss->type == SUMMON_EMPTY ){
			ss->objID	= AddObject3Dmap( Get_GS_SceneMap( tc->gs ), 
											summonTypeTable[summonTypeID].type, trans );
			ss->type	= summonTypeID;
			ss->hpMax	= summonTypeTable[summonTypeID].hp;
			ss->hpNow	= ss->hpMax;
			ss->trans	= *trans;
			return i;
		}
	}
	return -1;
}

//------------------------------------------------------------------
/**
 * @brief	�������폜
 */
//------------------------------------------------------------------
void DeleteSummonObject( TEAM_CONTROL* tc, int summonID )
{
	SUMMON_STATUS* ss = &tc->summon[summonID];

	if( ss->type != SUMMON_EMPTY ){
		RemoveObject3Dmap( Get_GS_SceneMap( tc->gs ), ss->objID ); 
		ss->type = SUMMON_EMPTY;
	}
}

//------------------------------------------------------------------
/**
 * @brief	�������`��X�C�b�`�ݒ�
 */
//------------------------------------------------------------------
void SetSummonObjectDrawSW( TEAM_CONTROL* tc, int summonID, BOOL* drawSW )
{
	SUMMON_STATUS* ss = &tc->summon[summonID];

	if( ss->type != SUMMON_EMPTY ){
		Set3DmapDrawSw( Get_GS_SceneMap( tc->gs ), ss->objID, drawSW ); 
	}
}

//------------------------------------------------------------------
/**
 * @brief	�������`��X�C�b�`�擾
 */
//------------------------------------------------------------------
void GetSummonObjectDrawSW( TEAM_CONTROL* tc, int summonID, BOOL* drawSW )
{
	SUMMON_STATUS* ss = &tc->summon[summonID];

	if( ss->type != SUMMON_EMPTY ){
		Get3DmapDrawSw( Get_GS_SceneMap( tc->gs ), ss->objID, drawSW ); 
	}
}

//------------------------------------------------------------------
/**
 * @brief	�������_���[�W�ݒ�
 */
//------------------------------------------------------------------
void SetSummonObjectDamage( TEAM_CONTROL* tc, int summonID, int* damage )
{
	SUMMON_STATUS* ss = &tc->summon[summonID];

	if( ss->type == SUMMON_EMPTY ){
		return;
	}
	if( ss->hpNow < *damage ){
		ss->hpNow = 0;
	} else {
		ss->hpNow -= *damage;
	}
}

//------------------------------------------------------------------
/**
 * @brief	�������g�o�擾
 */
//------------------------------------------------------------------
void GetSummonObjectHP( TEAM_CONTROL* tc, int summonID, int* hp, int* hpMax )
{
	SUMMON_STATUS* ss = &tc->summon[summonID];

	if( ss->type != SUMMON_EMPTY ){
		*hp = ss->hpNow;
		*hpMax = ss->hpMax;
	} else {
		*hp = 0;
		*hpMax = 0;
	}
}

//------------------------------------------------------------------
/**
 * @brief	���������W�擾
 */
//------------------------------------------------------------------
void GetSummonObjectTrans( TEAM_CONTROL* tc, int summonID, VecFx32* trans )
{
	SUMMON_STATUS* ss = &tc->summon[summonID];

	if( ss->type != SUMMON_EMPTY ){
		*trans = ss->trans;
	} else {
		trans->x = 0;
		trans->y = 0;
		trans->z = 0;
	}
}

//------------------------------------------------------------------
/**
 * @brief	���ׂĂ̓o�^�������ɑ΂��ăR�[���o�b�N���������s����
 */
//------------------------------------------------------------------
void ProcessingAllTeamSummonObject( TEAM_CONTROL* tc, TEAM_SUMMON_CALLBACK* proc, void* work )
{
	int i, num;
	SUMMON_STATUS* ss;

	for( i=0, num=0; i<TEAM_SUMMON_COUNT_MAX; i++ ){
		ss = &tc->summon[i];
		if( ss->type != SUMMON_EMPTY ){
			proc( tc, i, num, work );
			num++;
		}
	}
}

//------------------------------------------------------------------
/**
 * @brief	���f�[�^
 */
//------------------------------------------------------------------
static const SUMMON_TYPETABLE	summonTypeTable[] = {
	{ 0, 0, 0 },	//dummy
	{ EXMAPOBJ_WALL, 100, 0, 8 },
	{ EXMAPOBJ_WALL, 100, 0, 8 },
	{ EXMAPOBJ_WALL, 100, 0, 8 },
	{ EXMAPOBJ_WALL, 100, 0, 8 },
	{ EXMAPOBJ_WALL, 100, 0, 8 },
	{ EXMAPOBJ_WALL, 100, 0, 8 },
	{ EXMAPOBJ_WALL, 100, 0, 8 },
	{ EXMAPOBJ_WALL, 100, 0, 8 },
};

//------------------------------------------------------------------
/**
 * @brief	�̈�Ǘ��r�b�g�}�b�v�ύX�t���O�擾
 */
//------------------------------------------------------------------
BOOL GetMapAreaMaskDrawFlag( TEAM_CONTROL* tc )
{
	return tc->mapAreaDrawFlag;
}

//------------------------------------------------------------------
/**
 * @brief	�̈�Ǘ��r�b�g�}�b�v�ύX�t���O�Z�b�g
 */
//------------------------------------------------------------------
void SetMapAreaMaskDrawFlag( TEAM_CONTROL* tc, BOOL* sw )
{
	tc->mapAreaDrawFlag = *sw;
}

//------------------------------------------------------------------
/**
 * @brief	�̈�Ǘ��r�b�g�}�b�v�擾
 */
//------------------------------------------------------------------
void GetMapAreaMask( TEAM_CONTROL* tc, GFL_BMP_DATA** mapArea )
{
	*mapArea = tc->mapArea;
}

//------------------------------------------------------------------
/**
 * @brief	�̈�Ǘ��r�b�g�}�b�v��̍��W�擾
 */
//------------------------------------------------------------------
#define MAP_CENTER_POSX ( 8*MAPAREA_SIZX/2 )
#define MAP_CENTER_POSY ( 8*MAPAREA_SIZY/2 )
#define MAP_LENGTHX		( 8*32 )
#define MAP_LENGTHZ		( 8*32 )

void GetMapAreaMaskPos( VecFx32* trans, u16* posx, u16* posy )
{
	*posx = MAP_CENTER_POSX + ( trans->x / FX32_ONE ) * MAPAREA_SIZX / MAP_LENGTHX;
	*posy = MAP_CENTER_POSY + ( trans->z / FX32_ONE ) * MAPAREA_SIZY / MAP_LENGTHZ;
}

//------------------------------------------------------------------
/**
 * @brief	�̈�Ǘ��r�b�g�}�b�v��̏�Ԏ擾
 */
//------------------------------------------------------------------
BOOL GetMapAreaMaskStatus( TEAM_CONTROL* tc, VecFx32* trans )
{
	u16		posx, posy, chrx, chry, dotx, doty;
	u32*	bmp  = (u32*)GFL_BMP_GetCharacterAdrs( tc->mapArea );
	u32		data, mask;

	GetMapAreaMaskPos( trans, &posx, &posy );
	chrx = posx/8;
	dotx = posx%8;
	chry = posy/8;
	doty = posy%8;
	data = bmp[ (chry*MAPAREA_SIZX+chrx)*(0x20/sizeof(u32)) + doty ];
	mask = 0x0f<<dotx*4;

	if( data & mask ){
		return FALSE;
	} else {
		return TRUE;
	}
}


//------------------------------------------------------------------
/**
 * @brief	�̈�Ǘ��r�b�g�}�b�v�쐬
 */
//------------------------------------------------------------------
static const u32 dummyMaskTable[] = {
	0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x10000000,0x11000000,
	0x00000000,0x10000000,0x11100000,0x11111000,0x11111100,0x11111111,0x11111111,0x11111111,
	0x11111000,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,
	0x00011111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,
	0x00000000,0x00000001,0x00000111,0x00011111,0x00111111,0x11111111,0x11111111,0x11111111,
	0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000001,0x00000011,

	0x11100000,0x11100000,0x11110000,0x11111000,0x11111000,0x11111100,0x11111100,0x11111110,
	0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,
	0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,
	0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,
	0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,
	0x00000111,0x00000111,0x00001111,0x00011111,0x00011111,0x00111111,0x00111111,0x01111111,

	0x11111110,0x11111110,0x11111110,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,
	0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,
	0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,
	0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,
	0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,
	0x01111111,0x01111111,0x01111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,

	0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111110,0x11111110,0x11111110,
	0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,
	0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,
	0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,
	0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,
	0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x01111111,0x01111111,0x01111111,

	0x11111110,0x11111100,0x11111100,0x11111000,0x11111000,0x11110000,0x11100000,0x11100000,
	0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,
	0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,
	0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,
	0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,
	0x01111111,0x00111111,0x00111111,0x00011111,0x00011111,0x00001111,0x00000111,0x00000111,

	0x11000000,0x10000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
	0x11111111,0x11111111,0x11111111,0x11111100,0x11111000,0x11100000,0x10000000,0x00000000,
	0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111000,
	0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x11111111,0x00011111,
	0x11111111,0x11111111,0x11111111,0x00111111,0x00011111,0x00000111,0x00000001,0x00000000,
	0x00000011,0x00000001,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
};

static void DrawMapAreaMask
		( GFL_BMP_DATA* mapArea, VecFx32* trans, int radius, HEAPID heapID )
{
	GFL_BMP_DATA* circleBmp;
	int bmpSize,bmpWidth;
	u16 posx, posy;

	bmpWidth = radius*2;
	bmpSize = bmpWidth/8;	//�L�������v�Z
	if( bmpWidth%8 ){
		bmpSize++;
	}
	circleBmp = GFL_BMP_Create( bmpSize, bmpSize, GFL_BMP_16_COLOR, heapID );
	GFL_BMP_Clear( circleBmp, 0 );

	GetMapAreaMaskPos( trans, &posx, &posy );
#if 1
	{
		u8* dst = GFL_BMP_GetCharacterAdrs( circleBmp );
		GFL_STD_MemCopy32( dummyMaskTable, dst, bmpSize*bmpSize*0x20 );
		DC_FlushRange( dummyMaskTable, bmpSize*bmpSize*0x20 );
	}
#endif
	GFL_BMP_Print( circleBmp, mapArea, 0, 0, posx-radius,  posy-radius, radius*2, radius*2, 0 );
	GFL_BMP_Delete( circleBmp );
}
	
void MakeTeamMapAreaMask( TEAM_CONTROL* tc )
{
	int i, radius;
	CASTLE_STATUS*	cs;
	SUMMON_STATUS*	ss;
	GFL_BMP_DATA*	mapAreaTmp;

	mapAreaTmp = GFL_BMP_Create( MAPAREA_SIZX, MAPAREA_SIZY, GFL_BMP_16_COLOR, tc->heapID );
	GFL_BMP_Clear( mapAreaTmp, 0 );

	cs = &tc->castle;
	if( cs->type != CASTLE_EMPTY ){
		radius = 3*8;
		DrawMapAreaMask( mapAreaTmp, &cs->trans, radius, tc->heapID );
	}

	for( i=0; i<TEAM_SUMMON_COUNT_MAX; i++ ){
		ss = &tc->summon[i];
		if( ss->type != SUMMON_EMPTY ){
			radius = summonTypeTable[ ss->type ].radius;
			radius = 3*8;
			DrawMapAreaMask( mapAreaTmp, &ss->trans, radius, tc->heapID );
		}
	}
	{
		u32*	src = (u32*)GFL_BMP_GetCharacterAdrs( mapAreaTmp );
		u32*	dst = (u32*)GFL_BMP_GetCharacterAdrs( tc->mapArea );
		u16		siz = (MAPAREA_SIZX*8) * (MAPAREA_SIZY*8);
		u32		refData, setData;
		int		i, j, count;

		count = 0;

		for( i=0; i<siz/8; i++ ){
			refData = src[i];
			setData = 0;
			for( j=0; j<8; j++ ){
				if( !(refData & (0x0f<<j*4)) ){
					setData |= ( MAPAREAMASK_COL<<j*4);
				} else {
					count++;
				}
			}
			dst[i] = setData;
		}
		tc->mapAreaScore = count;
	}
	GFL_BMP_Delete( mapAreaTmp );

	tc->mapAreaDrawFlag = TRUE;
}

