//============================================================================================
/**
 * @file	field_g3d_mapper.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"

#include "arc\arc_def.h"	// �A�[�J�C�u�f�[�^

#include "mapdatafunc/field_func_mapeditor_file.h"
#include "mapdatafunc/field_func_pmcustom_file.h"
#include "mapdatafunc/field_func_bridge_file.h"
#include "mapdatafunc/field_func_nogrid_file.h"
#include "mapdatafunc/field_func_wbnormal_file.h"
#include "mapdatafunc/field_func_wbcross_file.h"
#include "mapdatafunc/field_func_random_generate.h"


#include "field_g3d_mapper.h"
#include "fieldmap_resist.h"
#include "field_g3dmap_exwork.h"
#include "field_ground_anime.h"
#include "field_wfbc.h"
#include "field/field_status_local.h" //FIELD_STATUS

#include "system/g3d_tool.h"

#include "field_hit_check.h"

#include "field/field_const.h"  //for FIELD_CONST_GRID_FX32_SIZE
#include "field/map_matrix.h"   // for MAP_MATRIX_MAX
#include "map_attr.h" //MAPATTR_IsEnable

//============================================================================================
/**
 *
 *
 *
 * @brief	�R�c�}�b�v�R���g���[��
 *
 *
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�^�錾
 */
//------------------------------------------------------------------
#define MAPID_NULL			(0xffffffff)
#define MAPARC_NULL			(0xffffffff)

//�g�������̌�
#define EX_HEIGHT_NUM	(16)

//------------------------------------------------------------------
//------------------------------------------------------------------




//------------------------------------------------------------------
typedef struct {
	u32			blockIdx;
	VecFx32		trans;
}BLOCKINFO;

//-------------------------------------
// GFL_G3D_MAP�g�����[�N
struct _FLD_G3D_MAP_EXWORK{
	FIELD_GRANM_WORK* p_granm_wk;		// �n�ʃA�j���[�V����
	FIELD_BMODEL_MAN * bmodel_man;	// 
	FIELD_WFBC * wfbcwork;	// 
	u32               mapIndex;     //�}�b�v��index(�����_���}�b�v�Ŏg�p
};


//-------------------------------------
///	�u���b�N�Ǘ����[�N
typedef struct {
	GFL_G3D_MAP*		g3Dmap;       // �}�b�v���
	FLD_G3D_MAP_EXWORK	g3DmapExWork; // �g�����[�N
	BLOCKINFO				blockInfo;    //  ���̏��
} BLOCK_WORK;

//-------------------------------------
///	�u���b�N�X�V���N�G�X�g���[�N
typedef struct {
  BLOCKINFO       newBlockInfo; //  ���N�G�X�g���
} BLOCK_NEWREQ;


//------------------------------------------------------------------
struct _FLD_G3D_MAPPER {
	HEAPID					heapID;

	FLDMAPPER_FILETYPE	g3DmapFileType;	//g3Dmap�t�@�C�����ʃ^�C�v�i���j
	fx32				blockWidth;		//�u���b�N�P�ӂ̕�
	fx32				blockHeight;		//�u���b�N����
	FLDMAPPER_MODE		mode;		//���샂�[�h
	u32					arcID;		//�O���t�B�b�N�A�[�J�C�u�h�c

	u16					sizex;		//���u���b�N��
	u16					sizez;		//�c�u���b�N��
	u32					totalSize;	//�z��T�C�Y
	const FLDMAPPER_MAPDATA*	blocks;	//���}�b�v�f�[�^
	
  FLDMAPPER_TEXTYPE gtexType;

  BLOCK_WORK*   blockWk;      // �u���b�N���[�N
  BLOCK_NEWREQ* blockNew;     // �u���b�N�X�V���N�G�X�g���[�N
  u8            blockXNum;    // �������z�u���u���b�N��
  u8            blockZNum;    // �������z�u�c�u���b�N��
  u16           blockNum;     // �������z�u�u���b�N����
  
	u32					nowBlockIdx;				
	VecFx32				posCont;

	VecFx32 globalDrawOffset;		//���ʍ��W�I�t�Z�b�g

	GFL_G3D_RES*			globalTexture;					//���ʒn�`�e�N�X�`��

  FIELD_BMODEL_MAN* bmodel_man; //�z�u���f���}�l�W���[
	FIELD_GRANM*	granime;	// �n�ʃA�j���[�V�����V�X�e��
  FIELD_WFBC* wfbcwork;   // �����_���}�b�v���[�N

  EHL_PTR	ExHeightList;   //�g�������f�[�^�̈�
};


//------------------------------------------------------------------
static void CreateGlobalTexture( FLDMAPPER* g3Dmapper, const FLDMAPPER_RESISTDATA* resistData );
static void DeleteGlobalTexture( FLDMAPPER* g3Dmapper );


static void GetMapperBlockIdxAll( const FLDMAPPER* g3Dmapper, const VecFx32* pos, BLOCK_NEWREQ* new );
static void GetMapperBlockIdxXZ( const FLDMAPPER* g3Dmapper, const VecFx32* pos, BLOCK_NEWREQ* new );
static void GetMapperBlockIdxXZ_Loop( const FLDMAPPER* g3Dmapper, const VecFx32* pos, BLOCK_NEWREQ* new );
static void GetMapperBlockIdxY( const FLDMAPPER* g3Dmapper, const VecFx32* pos, BLOCK_NEWREQ* new );
static BOOL	ReloadMapperBlock( FLDMAPPER* g3Dmapper, BLOCK_NEWREQ* new );

static s32 GetMapperBlockStartIdxEven( u32 blockNum, fx32 width, fx32 pos );
static s32 GetMapperBlockStartIdxUnEven( u32 blockNum, fx32 width, fx32 pos );
static s32 GetMapperBlockStartIdxLoop( u32 blockNum, fx32 width, fx32 pos );



static FIELD_GRANM * createGroundAnime( u32 blockNum, GFL_G3D_RES* globalTexture,
    const FLDMAPPER_RESIST_GROUND_ANIME * resistGroundAnimeData, u32 heapID );
// FLD_G3D_MAP_EXWORK�@����֐�
static void FLD_G3D_MAP_ExWork_Init( FLD_G3D_MAP_EXWORK* p_wk, FLDMAPPER* g3Dmapper, u32 index );
static void FLD_G3D_MAP_ExWork_Exit( FLD_G3D_MAP_EXWORK* p_wk );
static void FLD_G3D_MAP_ExWork_ClearBlockData( FLD_G3D_MAP_EXWORK* p_wk );
static BOOL FLD_G3D_MAP_ExWork_IsGranm( const FLD_G3D_MAP_EXWORK* cp_wk );
static FIELD_GRANM_WORK* FLD_G3D_MAP_ExWork_GetGranmWork( const FLD_G3D_MAP_EXWORK* cp_wk );

static void BLOCKINFO_init(BLOCKINFO * info);
static void BLOCKINFO_SetBlockIdx(BLOCKINFO * info, u32 blockIdx);
static BOOL BLOCKINFO_IsInBlockData(const BLOCKINFO * info);
static u32 BLOCKINFO_GetBlockIdx(const BLOCKINFO * info);
static void BLOCKINFO_SetBlockTrans(BLOCKINFO * info, fx32 x, fx32 y, fx32 z );
static void BLOCKINFO_SetBlockTransVec(BLOCKINFO * info, const VecFx32* trans);
static void BLOCKINFO_GetBlockTrans(const BLOCKINFO * info, VecFx32* trans);
static void GetExHight( const FLDMAPPER* g3Dmapper, const VecFx32 *pos, FLDMAPPER_GRIDINFO* gridInfo );

//------------------------------------------------------------------
/**
 * @brief	�Z�b�g�A�b�v
 */
//------------------------------------------------------------------
static const GFL_G3D_MAP_FILE_FUNC mapFileFuncTbl[] = {
	{ WBGRIDPACK_HEADER, FieldLoadMapData_WBNormalFile, FieldGetAttr_WBNormalFile },
	{ WBGCROSSPACK_HEADER, FieldLoadMapData_WBCrossFile, FieldGetAttr_WBCrossFile },
	{ DP3PACK_HEADER, FieldLoadMapData_MapEditorFile, FieldGetAttr_MapEditorFile },
  { NOGRIDPACK_HEADER, FieldLoadMapData_NoGridFile, FieldGetAttr_NoGridFile },
	{ BRIDGEPACK_HEADER, FieldLoadMapData_BridgeFile, FieldGetAttr_BridgeFile },
	{ MAPFILE_FUNC_DEFAULT, FieldLoadMapData_PMcustomFile, FieldGetAttr_PMcustomFile },	//TableEnd&default	
};

static const GFL_G3D_MAP_FILE_FUNC randommapFileFuncTbl[] = {
	{ WBGRIDPACK_HEADER, FieldLoadMapData_RandomGenerate, FieldGetAttr_RandomGenerate },
	{ DP3PACK_HEADER, FieldLoadMapData_MapEditorFile, FieldGetAttr_MapEditorFile },
  { NOGRIDPACK_HEADER, FieldLoadMapData_NoGridFile, FieldGetAttr_NoGridFile },
	{ BRIDGEPACK_HEADER, FieldLoadMapData_BridgeFile, FieldGetAttr_BridgeFile },
	{ MAPFILE_FUNC_DEFAULT, FieldLoadMapData_PMcustomFile, FieldGetAttr_PMcustomFile },	//TableEnd&default	
};

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�R���g���[���V�X�e���쐬
 */
//------------------------------------------------------------------
FLDMAPPER*	FLDMAPPER_Create( HEAPID heapID )
{
	FLDMAPPER* g3Dmapper = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLDMAPPER) );

	g3Dmapper->heapID = heapID;

	VEC_Set( &g3Dmapper->posCont, 0, 0, 0 );
	g3Dmapper->sizex = 0;
	g3Dmapper->sizez = 0;
	g3Dmapper->totalSize = 0;
	g3Dmapper->blocks = NULL;
	g3Dmapper->blockWidth = 0;
	g3Dmapper->blockHeight = 0;
	g3Dmapper->mode = FLDMAPPER_MODE_SCROLL_XZ;
	g3Dmapper->arcID = MAPARC_NULL;
	
  g3Dmapper->granime = NULL;
  //  �z�u���f���}�l�W���[����
  g3Dmapper->bmodel_man = FIELD_BMODEL_MAN_Create(g3Dmapper->heapID, g3Dmapper);
  // WFBC���[�N�̐���
  g3Dmapper->wfbcwork = FIELD_WFBC_Create( heapID );


  //�g�������f�[�^�̈�m��
  g3Dmapper->ExHeightList = EXH_AllocHeightList(EX_HEIGHT_NUM, heapID);
	return g3Dmapper;
}

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�R���g���[���V�X�e���j��
 */
//------------------------------------------------------------------
void	FLDMAPPER_Delete( FLDMAPPER* g3Dmapper )
{
	GF_ASSERT( g3Dmapper );

  //�g�������f�[�^�̈���
	EXH_FreeHeightList(g3Dmapper->ExHeightList);

	FLDMAPPER_ReleaseData( g3Dmapper );	//�o�^���ꂽ�܂܂̏ꍇ��z�肵�č폜

  // WFBC���[�N�̔j��
  FIELD_WFBC_Delete( g3Dmapper->wfbcwork );

  // �z�u���f���}�l�W���[�j��
  if(g3Dmapper->bmodel_man) 
  { 
    FIELD_BMODEL_MAN_Delete(g3Dmapper->bmodel_man);
    g3Dmapper->bmodel_man = NULL;
  }
	GFL_HEAP_FreeMemory( g3Dmapper );
}

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�R���g���[���V�X�e�����C��
 */
//------------------------------------------------------------------
void	FLDMAPPER_Main( FLDMAPPER* g3Dmapper )
{
	int i;

	GF_ASSERT( g3Dmapper );

	if( g3Dmapper->blocks == NULL ){
		return;
	}
	for( i=0; i<g3Dmapper->blockNum; i++ ){
    BLOCKINFO_init(&g3Dmapper->blockNew[i].newBlockInfo);
	}

	switch( g3Dmapper->mode ){
	case FLDMAPPER_MODE_SCROLL_NONE: 
		GetMapperBlockIdxAll( g3Dmapper, &g3Dmapper->posCont, g3Dmapper->blockNew );
		break;
	default:
	case FLDMAPPER_MODE_SCROLL_XZ: 
		GetMapperBlockIdxXZ( g3Dmapper, &g3Dmapper->posCont, g3Dmapper->blockNew );
		break;
	case FLDMAPPER_MODE_SCROLL_XZ_LOOP: 
		GetMapperBlockIdxXZ_Loop( g3Dmapper, &g3Dmapper->posCont, g3Dmapper->blockNew );
		break;
	case FLDMAPPER_MODE_SCROLL_Y: 
		GetMapperBlockIdxY( g3Dmapper, &g3Dmapper->posCont, g3Dmapper->blockNew );
		break;
	}
	ReloadMapperBlock( g3Dmapper, g3Dmapper->blockNew );

	//�u���b�N���䃁�C��
	for( i=0; i<g3Dmapper->blockNum; i++ ){
		GFL_G3D_MAP_Main( g3Dmapper->blockWk[i].g3Dmap );
	}

	//���݃u���b�N��index�擾
	{
		u32 blockx = FX_Whole( FX_Div( g3Dmapper->posCont.x, g3Dmapper->blockWidth ) );
		u32 blockz = FX_Whole( FX_Div( g3Dmapper->posCont.z, g3Dmapper->blockWidth ) );

		g3Dmapper->nowBlockIdx = blockz * g3Dmapper->sizex + blockx;
	}
  //�z�u���f���X�V�����i���݂͓d���f���̂݁j
  FIELD_BMODEL_MAN_Main( g3Dmapper->bmodel_man );

	// �n�ʃA�j���[�V�����Ǘ�
	if( g3Dmapper->granime ){
		FIELD_GRANM_Main( g3Dmapper->granime );
	}
}

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�R���g���[���V�X�e���f�B�X�v���C
 */
//------------------------------------------------------------------
void	FLDMAPPER_Draw( const FLDMAPPER* g3Dmapper, GFL_G3D_CAMERA* g3Dcamera )
{
	int i;
	VecFx32 org_pos,draw_pos;
	
	GF_ASSERT( g3Dmapper );
	
///	GFL_G3D_MAP_StartDraw();

	for( i=0; i<g3Dmapper->blockNum; i++ ){
		GFL_G3D_MAP_GetTrans( g3Dmapper->blockWk[i].g3Dmap, &org_pos );
		draw_pos.x = org_pos.x + g3Dmapper->globalDrawOffset.x;
		draw_pos.y = org_pos.y + g3Dmapper->globalDrawOffset.y;
		draw_pos.z = org_pos.z + g3Dmapper->globalDrawOffset.z;
		GFL_G3D_MAP_SetTrans( g3Dmapper->blockWk[i].g3Dmap, &draw_pos );
		GFL_G3D_MAP_Draw( g3Dmapper->blockWk[i].g3Dmap, g3Dcamera );
		GFL_G3D_MAP_SetTrans( g3Dmapper->blockWk[i].g3Dmap, &org_pos );
	}

  FIELD_BMODEL_MAN_Draw( g3Dmapper->bmodel_man );
///	GFL_G3D_MAP_EndDraw();
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL FLDMAPPER_CheckTrans( const FLDMAPPER* g3Dmapper )
{
	int i;
	for ( i=0; i<g3Dmapper->blockNum; i++ ){
		GFL_G3D_MAP_LOAD_STATUS *ldst;
		GFL_G3D_MAP_GetLoadStatusPointer( g3Dmapper->blockWk[i].g3Dmap, &ldst );
		if (ldst->seq != GFL_G3D_MAP_LOAD_IDLING) {
			return FALSE;
		}
	}
	return TRUE;
}


//--------------------------------------------------------------
/**
 * @brief �}�b�v�ڑ�
 *
 * @param g3Dmapper �ڑ��Ώۃ}�b�p�[
 * @param matrix    �ڑ�����}�b�v�̃}�b�v�}�g���b�N�X
 *
 * @return �ڑ��ł�����TRUE
 */
//--------------------------------------------------------------
BOOL FLDMAPPER_Connect( FLDMAPPER* g3Dmapper, const MAP_MATRIX* matrix )
{ 
  int i, ix, iz;
  u16 add_sizex, add_sizez; // �ǉ��}�b�v�̉��E�c�u���b�N��
  u32 add_totalSize;        // �ǉ��}�b�v�̑��u���b�N��
  const u32* add_blocks;    // �ǉ��}�b�v�̎��}�b�v�f�[�^
  u16 sizex, sizez;         // �V�}�b�v�̉��E�c�u���b�N��
  u32 totalSize;            // �V�}�b�v�̑��u���b�N��
  FLDMAPPER_MAPDATA* blocks;// �V�}�b�v�̎��}�b�v�f�[�^

  OBATA_Printf( "FLDMAPPER_Connect: start\n" );

  // �ǉ��}�b�v�̃T�C�Y���擾
  add_sizex     = MAP_MATRIX_GetMapBlockSizeWidth( matrix );
  add_sizez     = MAP_MATRIX_GetMapBlockSizeHeight( matrix );
  add_totalSize = MAP_MATRIX_GetMapBlockTotalSize( matrix );
  add_blocks    = MAP_MATRIX_GetMapResIDTable( matrix );

  // �V�}�b�v�̃p�����[�^�v�Z
  sizex     = g3Dmapper->sizex + add_sizex;
  sizez     = g3Dmapper->sizez;
  totalSize = g3Dmapper->totalSize + add_totalSize;

  // z�����̑傫�����قȂ�}�b�v�͐ڑ��ł��Ȃ�
  if( g3Dmapper->sizez != add_sizez )
  {
    OS_Printf( "---------------------------------------------\n" );
    OS_Printf( "error in FLDMAPPER_Connect. Map size conflict\n" );
    OS_Printf( "---------------------------------------------\n" );
    return FALSE;
  }

  // �傫������}�b�v�͐ڑ��ł��Ȃ�
  if( (MAP_MATRIX_MAX < totalSize)    ||
      (MAP_MATRIX_WIDTH_MAX < sizex ) ||
      (MAP_MATRIX_HEIGHT_MAX < sizez )  )
  {
    OS_Printf( "------------------------------------------------\n" );
    OS_Printf( "error in FLDMAPPER_Connect. Map matrix size over\n" );
    OS_Printf( "sizex=%d, sizez=%d, totalSize=%x\n", sizex, sizez, totalSize );
    OS_Printf( "------------------------------------------------\n" );
    return FALSE;
  }

  // ���}�b�v�̈ꎞ�o�b�t�@���m��
  blocks = GFL_HEAP_AllocMemoryLo( 
                 g3Dmapper->heapID, sizeof(FLDMAPPER_MAPDATA) * totalSize );

  // ���g�̃f�[�^���R�s�[
  for( iz=0; iz<g3Dmapper->sizez; iz++ )
  {
    for( ix=0; ix<g3Dmapper->sizex; ix++ )
    {
      int old_index = iz * g3Dmapper->sizex + ix;
      int new_index = iz * sizex + ix;
      blocks[ new_index ] = g3Dmapper->blocks[ old_index ];
    }
  }
  // ���N�G�X�g�̃f�[�^���R�s�[
  for( iz=0; iz<add_sizez; iz++ )
  {
    for( ix=0; ix<add_sizex; ix++ )
    {
      int old_index = iz * add_sizex + ix;
      int new_index = iz * sizex + g3Dmapper->sizex + ix;
      blocks[ new_index ].datID = add_blocks[ old_index ];
    }
  }

  // �V�}�b�v�f�[�^���Z�b�g
  for( i=0; i<totalSize; i++ )
  {
    ( (FLDMAPPER_MAPDATA*)g3Dmapper->blocks )[i] = blocks[i];
  }
  g3Dmapper->sizex     = sizex;
  g3Dmapper->sizez     = sizez; 
  g3Dmapper->totalSize = totalSize; 

  // ��n��
  GFL_HEAP_FreeMemory( blocks );
  OBATA_Printf( "FLDMAPPER_Connect: end\n" );
  return TRUE;
}


//--------------------------------------------------------------
/**
 * @brief �f�o�b�O�o�͊֐�
 *
 * @param g3Dmapper ��Ԃ��o�͂������}�b�p�[
 */
//--------------------------------------------------------------
void FLDMAPPER_DebugPrint( const FLDMAPPER* g3Dmapper )
{
  int ix, iz, i, index; 

  OS_Printf( "================================================\n" );

  OS_Printf( "blockWidth  = %d\n", FX_Whole(g3Dmapper->blockWidth) );
  OS_Printf( "blockHeight = %d\n", FX_Whole(g3Dmapper->blockHeight) );
  switch( g3Dmapper->mode )
  {
  case FLDMAPPER_MODE_SCROLL_NONE:    OS_Printf( "mode = FLDMAPPER_MODE_SCROLL_NONE\n" ); break;
  case FLDMAPPER_MODE_SCROLL_XZ:      OS_Printf( "mode = FLDMAPPER_MODE_SCROLL_XZ\n" ); break;
  case FLDMAPPER_MODE_SCROLL_XZ_LOOP: OS_Printf( "mode = FLDMAPPER_MODE_SCROLL_XZ_LOOP\n" ); break;
  case FLDMAPPER_MODE_SCROLL_Y:       OS_Printf( "mode = FLDMAPPER_MODE_SCROLL_Y\n" ); break;
  default:                            OS_Printf( "mode = error\n" ); break;
  }
  OS_Printf( "sizex  = %d\n", g3Dmapper->sizex );
  OS_Printf( "sizez  = %d\n", g3Dmapper->sizez );
  OS_Printf( "totalSize  = %d\n", g3Dmapper->totalSize );

  OS_Printf( "blocks-------------------\n" );
  index = 0;
  for( iz=0; iz<g3Dmapper->sizez; iz++ ) {
    for( ix=0; ix<g3Dmapper->sizex; ix++ ) {
      OS_Printf( "%d ", g3Dmapper->blocks[ index++ ] ); 
    }
    OS_Printf( "\n" );
  }

  OS_Printf( "blockXNum = %d\n", g3Dmapper->blockXNum );
  OS_Printf( "blockZNum = %d\n", g3Dmapper->blockZNum );
  OS_Printf( "blockNum = %d\n", g3Dmapper->blockNum );

  OS_Printf( "nowBlockIdx = %d\n", g3Dmapper->nowBlockIdx );
  OS_Printf( "posCont = %d, %d, %d\n", 
      FX_Whole(g3Dmapper->posCont.x), FX_Whole(g3Dmapper->posCont.y), FX_Whole(g3Dmapper->posCont.z) );

  OS_Printf( "blockWk-----------------------\n" );
  index = 0;
  for( iz=0; iz<g3Dmapper->sizez; iz++ )
  {
    for( ix=0; ix<g3Dmapper->sizex; ix++ )
    {
      BOOL on_memory = FALSE;
      for( i=0; i<g3Dmapper->blockNum; i++ )
      {
        if( g3Dmapper->blockWk[i].blockInfo.blockIdx == index )
        {
          on_memory = TRUE;
          break;
        }
      }
      if( on_memory )
      {
        OS_Printf( "��" );
      }
      else
      {
        OS_Printf( "��" );
      }
      index++;
    }
    OS_Printf( "\n" );
  }

  OS_Printf( "================================================\n" );
}

//--------------------------------------------------------------
/**
 * @brief �g�������f�[�^���X�g�|�C���^�擾
 *
 * @param g3Dmapper ��Ԃ��o�͂������}�b�p�[
 *
 * @return  �g�������f�[�^���X�g�|�C���^
 */
//--------------------------------------------------------------
EHL_PTR	FLDMAPPER_GetExHegihtPtr( FLDMAPPER* g3Dmapper )
{
	GF_ASSERT( g3Dmapper );
	return g3Dmapper->ExHeightList;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static NNSG3dResFileHeader * createDummyTexHeader( HEAPID heapID, void * file )
{
  NNSG3dResTex * resTex = NNS_G3dGetTex( (NNSG3dResFileHeader*)file );
  NNSG3dResFileHeader * header;
  u8* texImgStartAddr;
  u32 newSize;

  // �e�N�X�`���^�p���b�g�́A4x4COMP�ȊO�̃e�N�X�`���E4x4COMP�̃e�N�X�`���E�p���b�g
  // �̏��ԂŊi�[����Ă��܂��B�����4x4COMP�ȊO�̃e�N�X�`���̊J�n�A�h���X�ȍ~��
  // �s�v�ɂȂ�܂��B�Ȃ��A4x4COMP�t�H�[�}�b�g�ȊO�̃e�N�X�`���C���[�W�����݂��Ȃ�
  // �ꍇ�ł�resTex->texInfo.ofsTex�ɂ͓K�؂Ȓl�������Ă��܂��B
  SDK_ASSERT(resTex->texInfo.ofsTex != 0);
  texImgStartAddr = (u8*)resTex + resTex->texInfo.ofsTex;

  // �q�[�v�̐擪����e�N�X�`���C���[�W�܂ł̃T�C�Y
  newSize = (u32)(texImgStartAddr - (u8*)file);

  header = GFL_HEAP_AllocClearMemory( heapID, newSize );
  GFL_STD_MemCopy32( file, header, newSize );

  return header;
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�}�b�v�f�[�^�o�^
 */
//------------------------------------------------------------------
void FLDMAPPER_ResistData( FLDMAPPER* g3Dmapper, const FLDMAPPER_RESISTDATA* resistData )
{
	GF_ASSERT( g3Dmapper );

	g3Dmapper->g3DmapFileType = resistData->g3DmapFileType;
	g3Dmapper->blockWidth = resistData->blockWidth;
	g3Dmapper->blockHeight = resistData->blockHeight;
	g3Dmapper->arcID = resistData->arcID;
	g3Dmapper->sizex = resistData->sizex;
	g3Dmapper->sizez = resistData->sizez;
	g3Dmapper->totalSize = resistData->totalSize;
	g3Dmapper->blocks = resistData->blocks;
	g3Dmapper->mode = resistData->mode;
  g3Dmapper->blockXNum  = resistData->blockXNum;
  g3Dmapper->blockZNum  = resistData->blockZNum;
  g3Dmapper->blockNum   = resistData->blockXNum * resistData->blockZNum;

  {//mode�ُ�̑Ώ� 
    switch( g3Dmapper->mode ){
    case FLDMAPPER_MODE_SCROLL_NONE:
      if( g3Dmapper->totalSize > g3Dmapper->blockNum ){
        OS_Printf("mapper mode set Error\n");
        g3Dmapper->mode = FLDMAPPER_MODE_SCROLL_XZ;
      }
      break;
    case FLDMAPPER_MODE_SCROLL_Y:
      if( g3Dmapper->sizex * g3Dmapper->sizez > g3Dmapper->blockNum/2 ){
        OS_Printf("mapper mode set Error\n");
        g3Dmapper->mode = FLDMAPPER_MODE_SCROLL_XZ;
      }
      break;
    }
  }

  g3Dmapper->gtexType = resistData->gtexType;
	//�O���[�o���e�N�X�`���쐬
	CreateGlobalTexture( g3Dmapper, resistData );
	//�O���[�o���I�u�W�F�N�g�쐬
	//CreateGlobalObject( g3Dmapper, resistData );
  

	// �n�ʃA�j���[�V�����쐬
  g3Dmapper->granime = createGroundAnime(g3Dmapper->blockNum, g3Dmapper->globalTexture, &resistData->ground_anime, g3Dmapper->heapID );
	

	{
		int i;
		GFL_G3D_MAP_SETUP setup;

		if (g3Dmapper->globalTexture != NULL) {
			setup.mapDataHeapSize = resistData->memsize;
			setup.texVramSize = 0;
			setup.mapFileFunc = mapFileFuncTbl;
			setup.externalWork = NULL;
      setup.mapLoadSize = 0x2000;
		} else {
			setup.mapDataHeapSize = resistData->memsize;
			setup.texVramSize = FLD_MAPPER_MAPTEX_SIZE;
			setup.mapFileFunc = mapFileFuncTbl;
			setup.externalWork = NULL;
      setup.mapLoadSize = 0x2000;
		}
		
		if( resistData->g3DmapFileType == FLDMAPPER_FILETYPE_RANDOM )
		{
      OS_TPrintf("C8!!!\n");
			setup.mapFileFunc = randommapFileFuncTbl;
    }


		//�u���b�N����n���h���쐬
    GF_ASSERT( resistData->blockXNum > 0 );
    GF_ASSERT( resistData->blockZNum > 0 );
    g3Dmapper->blockWk = GFL_HEAP_AllocClearMemory( g3Dmapper->heapID, sizeof(BLOCK_WORK) * g3Dmapper->blockNum );
    g3Dmapper->blockNew = GFL_HEAP_AllocClearMemory( g3Dmapper->heapID, sizeof(BLOCK_NEWREQ) * g3Dmapper->blockNum );
		for( i=0; i<g3Dmapper->blockNum; i++ ){
      GFL_G3D_MAP * g3dmap;

      BLOCKINFO_init(&g3Dmapper->blockWk[i].blockInfo);

			// �g�����[�N�̏�����
			FLD_G3D_MAP_ExWork_Init( &g3Dmapper->blockWk[i].g3DmapExWork, g3Dmapper, i );
			setup.externalWork = &g3Dmapper->blockWk[i].g3DmapExWork;

			// �u���b�N��񐶐�
      g3dmap = GFL_G3D_MAP_Create( &setup, g3Dmapper->heapID );
			g3Dmapper->blockWk[i].g3Dmap = g3dmap;

      //�V�A�[�J�C�u�h�c��o�^
      GFL_G3D_MAP_ResistArc( g3dmap, g3Dmapper->arcID, g3Dmapper->heapID );

      //�O���[�o�����\�[�X�o�^
      if( g3Dmapper->globalTexture != NULL ){
        GFL_G3D_MAP_ResistGlobalTexResource( g3dmap, g3Dmapper->globalTexture );
      }
      GFL_G3D_MAP_ResistGlobalObjResource( g3dmap,
          FIELD_BMODEL_MAN_GetGlobalObjects(g3Dmapper->bmodel_man) );

      //�t�@�C�����ʐݒ�i���j
      GFL_G3D_MAP_ResistFileType( g3dmap, resistData->g3DmapFileType );
    }

	}

	//�O���[�o���I�u�W�F�N�g�쐬
	//CreateGlobalObject( g3Dmapper, resistData );

}

//------------------------------------------------------------------
//------------------------------------------------------------------
void FLDMAPPER_ReleaseData( FLDMAPPER* g3Dmapper )
{
	int i;

	GF_ASSERT( g3Dmapper );

  if( g3Dmapper->blockWk ){

    //�}�b�v�u���b�N�������
    for( i=0; i<g3Dmapper->blockNum; i++ ){
      GFL_G3D_MAP_ReleaseGlobalObjResource( g3Dmapper->blockWk[i].g3Dmap );
      GFL_G3D_MAP_ReleaseGlobalTexResource( g3Dmapper->blockWk[i].g3Dmap );
      GFL_G3D_MAP_ReleaseArc( g3Dmapper->blockWk[i].g3Dmap );

      GFL_G3D_MAP_Delete( g3Dmapper->blockWk[i].g3Dmap );

      // �g�����[�N�̔j��
      FLD_G3D_MAP_ExWork_Exit( &g3Dmapper->blockWk[i].g3DmapExWork );
    }


    GFL_HEAP_FreeMemory( g3Dmapper->blockWk );
    GFL_HEAP_FreeMemory( g3Dmapper->blockNew );
    g3Dmapper->blockNew = NULL;
    g3Dmapper->blockWk = NULL;
    g3Dmapper->blockNum = 0;
  }

	// �n�ʃA�j���[�V�����j��
	if( g3Dmapper->granime != NULL ){
		FIELD_GRANM_Delete( g3Dmapper->granime );
		g3Dmapper->granime = NULL;
	}
	
	//DeleteGlobalObject( g3Dmapper, FIELD_BMODEL_MAN_GetGlobalObjects(g3Dmapper->bmodel_man) );
	DeleteGlobalTexture( g3Dmapper );
}

#include "system/palanm.h"
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_comm.h"
//--------------------------------------------------------------
/**
 * �f�o�b�O�F�N���t�B�[���h�������e�X�g
 *
 * @param   g3Dres		
 *
 * @retval  BOOL		
 */
//--------------------------------------------------------------
BOOL DEBUG_Field_Grayscale(GFL_G3D_RES *g3Dres)
{
	NNSG3dResFileHeader*	header;
	NNSG3dResTex*			texture;
  u32 sz;
  void* pData;
  int invasion_netid, white_mode = 0;
  GAME_COMM_SYS_PTR game_comm;
  GAMEDATA *gamedata;
  
  gamedata = GAMESYSTEM_GetGameData(DEBUG_GameSysWorkPtrGet());
  //if(GAMEDATA_GetMapMode( gamedata ) != MAPMODE_INTRUDE){
  if (FIELD_STATUS_GetMapMode( GAMEDATA_GetFieldStatus( gamedata) ) != MAPMODE_INTRUDE ) {
    return FALSE;
  }
  game_comm = GAMESYSTEM_GetGameCommSysPtr( DEBUG_GameSysWorkPtrGet() );
  
  invasion_netid = GameCommStatus_GetPlayerStatus_InvasionNetID(game_comm, GAMEDATA_GetIntrudeMyID(gamedata));
  if(invasion_netid == GAMEDATA_GetIntrudeMyID(gamedata)){
    return FALSE;
  }
  white_mode = invasion_netid & 1;

	header = GFL_G3D_GetResourceFileHeader(g3Dres);
	texture = NNS_G3dGetTex( header ); 

	if( texture ){
    sz = (u32)texture->plttInfo.sizePltt << 3;
    pData = (u8*)texture + texture->plttInfo.ofsPlttData;
    if(white_mode == 0){
      PaletteGrayScale(pData, sz / sizeof(u16));
    }
    else{
      PaletteGrayScaleFlip(pData, sz / sizeof(u16));
    }
		return TRUE;
	}
	return FALSE;
}

#define TEXTURE_RELEASE
//------------------------------------------------------------------
/**
 * @brief	�O���[�o���e�N�X�`���쐬
 */
//------------------------------------------------------------------
static void CreateGlobalTexture( FLDMAPPER* g3Dmapper, const FLDMAPPER_RESISTDATA* resistData )
{
	switch (resistData->gtexType) {
	case FLDMAPPER_TEXTYPE_USE:
		{
#ifdef TEXTURE_RELEASE
      void * buffer;
			const FLDMAPPER_RESIST_TEX* gtexData = &resistData->gtexData;

      	TAMADA_Printf("TEX:%06x PLT:%04x\n",
            DEBUG_GFL_G3D_GetBlankTextureSize(),
            DEBUG_GFL_G3D_GetBlankPaletteSize());


      g3Dmapper->globalTexture = GFL_HEAP_AllocClearMemory(
          g3Dmapper->heapID, GFL_G3D_GetResourceHeaderSize() );
      //�o�C�i���t�@�C�����[�h
      buffer = GFL_ARC_LoadDataAlloc( gtexData->arcID, gtexData->datID,
          GFL_HEAP_LOWID(g3Dmapper->heapID) );
      GFL_G3D_CreateResourceAuto( g3Dmapper->globalTexture, buffer );
      DEBUG_Field_Grayscale(g3Dmapper->globalTexture);
      GFL_G3D_TransVramTexture( g3Dmapper->globalTexture );
      
      	TAMADA_Printf("TEX:%06x PLT:%04x\n",
            DEBUG_GFL_G3D_GetBlankTextureSize(),
            DEBUG_GFL_G3D_GetBlankPaletteSize());

      {
        //NNSG3dResTex�̃w�b�_�������R�s�[�������̂�GFL_G3D_RES����蒼��
        void * header;
        header = createDummyTexHeader( g3Dmapper->heapID, buffer );
        GFL_G3D_CreateResourceAuto( g3Dmapper->globalTexture, header );
        //�o�C�i���t�@�C�����
        GFL_HEAP_FreeMemory( buffer );
        buffer = NULL;
      }
#else
			const FLDMAPPER_RESIST_TEX* gtexData = &resistData->gtexData;
			g3Dmapper->globalTexture = GFL_G3D_CreateResourceArc( gtexData->arcID, gtexData->datID );
			DEBUG_Field_Grayscale(g3Dmapper->globalTexture);
			GFL_G3D_TransVramTexture( g3Dmapper->globalTexture );
#endif
		}
		break;
	case FLDMAPPER_TEXTYPE_NONE:
		g3Dmapper->globalTexture = NULL;
	}
}

static void DeleteGlobalTexture( FLDMAPPER* g3Dmapper )
{
	if( g3Dmapper->globalTexture != NULL ){
#ifdef TEXTURE_RELEASE
		GFL_G3D_FreeVramTexture( g3Dmapper->globalTexture );
		GFL_G3D_DeleteResource( g3Dmapper->globalTexture );
    //GFL_HEAP_FreeMemory( g3Dmapper->globalTexture );
		g3Dmapper->globalTexture = NULL;
#else
		GFL_G3D_FreeVramTexture( g3Dmapper->globalTexture );
		GFL_G3D_DeleteResource( g3Dmapper->globalTexture );
		g3Dmapper->globalTexture = NULL;
#endif
	}
}



//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�}�b�v�ʒu�Z�b�g
 */
//------------------------------------------------------------------
void FLDMAPPER_SetPos( FLDMAPPER* g3Dmapper, const VecFx32* pos )
{
	GF_ASSERT( g3Dmapper );

	VEC_Set( &g3Dmapper->posCont, pos->x, pos->y, pos->z );
}
//------------------------------------------------------------------
/**
 * @brief	�}�b�v�ʒu����J�����g�u���b�N�A�N�Z�Xindex���擾
 */
//------------------------------------------------------------------
int FLDMAPPER_GetCurrentBlockAccessIdx( const FLDMAPPER* g3Dmapper )
{
  u32 i;
	GF_ASSERT( g3Dmapper );

  for( i=0; i<g3Dmapper->blockNum; i++ ){
	  VecFx32 trans;
		fx32 min_x, min_z, max_x, max_z;

		if( GFL_G3D_MAP_GetDrawSw( g3Dmapper->blockWk[i].g3Dmap ) == FALSE ){
      continue;
    }
		GFL_G3D_MAP_GetTrans( g3Dmapper->blockWk[i].g3Dmap, &trans );
		min_x = trans.x - g3Dmapper->blockWidth/2;
		min_z = trans.z - g3Dmapper->blockWidth/2;
		max_x = trans.x + g3Dmapper->blockWidth/2;
		max_z = trans.z + g3Dmapper->blockWidth/2;

		//�u���b�N�͈͓��`�F�b�N�i�}�b�v�u���b�N�̂w�y���ʏ�n�_�j
		if(	(g3Dmapper->posCont.x >= min_x)&&(g3Dmapper->posCont.x < max_x) &&
        (g3Dmapper->posCont.z >= min_z)&&(g3Dmapper->posCont.z < max_z) ){
      return i;
    }
  }
	GF_ASSERT( 0 );
  return 0;
}

//------------------------------------------------------------------
/**
 * @brief	�}�b�v�X�V�u���b�N�擾
 */
//------------------------------------------------------------------
static void GetMapperBlockIdxAll( const FLDMAPPER* g3Dmapper, const VecFx32* pos, BLOCK_NEWREQ* new )
{
	u32		idx, county, countxz;
	fx32	blockWidth, blockHeight;
  fx32  blockHalfWidth;
	int		i, j, offsx, offsz;

	countxz = g3Dmapper->sizex * g3Dmapper->sizez;
	county = g3Dmapper->totalSize / countxz;
	if( g3Dmapper->totalSize % countxz ){
		county++;
	}
	blockWidth      = g3Dmapper->blockWidth;
  blockHalfWidth  = FX_Div( g3Dmapper->blockWidth, 2*FX32_ONE );
	blockHeight     = g3Dmapper->blockHeight;

	idx = 0;

	for( i=0; i<county; i++ ){
		for( j=0; j<countxz; j++ ){
			offsx = j % g3Dmapper->sizex;
			offsz = j / g3Dmapper->sizex;

			if( idx >= g3Dmapper->totalSize ){
				idx = MAPID_NULL;
			}
      BLOCKINFO_SetBlockIdx( &new[idx].newBlockInfo, idx );
      BLOCKINFO_SetBlockTrans( &new[idx].newBlockInfo, 
         FX_Mul(offsx<<FX32_SHIFT, blockWidth) + blockHalfWidth,
         FX_Mul( i<<FX32_SHIFT,blockHeight ),
         FX_Mul(offsz<<FX32_SHIFT, blockWidth) + blockHalfWidth );
			idx++;
		}
	}
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void GetMapperBlockIdxXZ( const FLDMAPPER* g3Dmapper, const VecFx32* pos, BLOCK_NEWREQ* new )
{
	u16		sizex, sizez;
	u32		idx, idxmax;
	fx32	blockWidth, blockHalfWidth;
	int		i, j;
  int   start_x, start_z;
  int   set_x, set_z;


	sizex = g3Dmapper->sizex;
	sizez = g3Dmapper->sizez;
	idxmax = sizex * sizez;
	blockWidth = g3Dmapper->blockWidth;
  blockHalfWidth = FX_Div( g3Dmapper->blockWidth, 2*FX32_ONE );

  // XZ�����u���b�N�J�n�ʒu�����߂�
  if( (g3Dmapper->blockXNum % 2) == 0 ){
    // �����̏ꍇ
    start_x = GetMapperBlockStartIdxEven( g3Dmapper->blockXNum, blockWidth, pos->x );
  }else{
    // ��̏ꍇ
    start_x = GetMapperBlockStartIdxUnEven( g3Dmapper->blockXNum, blockWidth, pos->x );
  }
  if( (g3Dmapper->blockZNum % 2) == 0 ){
    // �����̏ꍇ
    start_z = GetMapperBlockStartIdxEven( g3Dmapper->blockZNum, blockWidth, pos->z );
  }else{
    // ��̏ꍇ
    start_z = GetMapperBlockStartIdxUnEven( g3Dmapper->blockZNum, blockWidth, pos->z );
  }

  // �ǂݍ��݃��N�G�X�g����ݒ肷��
	for( i=0; i<g3Dmapper->blockZNum; i++ )
  {
    // �ݒ肷��u���b�N��Z�l�����߂� 
    // �u���b�N�f�[�^�͈͓̔����`�F�b�N����
    set_z = start_z + i;
    if( (set_z >= 0) && (set_z < sizez) )
    {
  	  for( j=0; j<g3Dmapper->blockXNum; j++ )
      {
        // �ݒ肷��u���b�N��X�l�����߂� 
        // �u���b�N�f�[�^�͈͓̔����`�F�b�N����
        set_x = start_x + j;
        if( (set_x >= 0) && (set_x < sizex) )
        {
          idx = (i*g3Dmapper->blockXNum) + j;
          BLOCKINFO_SetBlockIdx( &new[idx].newBlockInfo, (set_z * sizex) + set_x );
          BLOCKINFO_SetBlockTrans( &new[idx].newBlockInfo, 
             FX_Mul(set_x<<FX32_SHIFT, blockWidth) + blockHalfWidth,
             0,
             FX_Mul(set_z<<FX32_SHIFT, blockWidth) + blockHalfWidth );
        }
      }
    }
  }
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void GetMapperBlockIdxXZ_Loop( const FLDMAPPER* g3Dmapper, const VecFx32* pos, BLOCK_NEWREQ* new )
{
	u16		sizex, sizez;
	u32		idx, idxmax;
	fx32	blockWidth, blockHalfWidth;
	int		i, j;
  int   start_x, start_z;
  int   set_x, set_z;
  int   ix, iz;


	sizex = g3Dmapper->sizex;
	sizez = g3Dmapper->sizez;
	idxmax = sizex * sizez;
	blockWidth = g3Dmapper->blockWidth;
  blockHalfWidth = FX_Div( g3Dmapper->blockWidth, 2*FX32_ONE );

  // XZ�����u���b�N�J�n�ʒu�����߂�
  start_x = GetMapperBlockStartIdxLoop( g3Dmapper->blockXNum, blockWidth, pos->x );
  start_z = GetMapperBlockStartIdxLoop( g3Dmapper->blockZNum, blockWidth, pos->z );

  // �ǂݍ��݃��N�G�X�g����ݒ肷��
	for( i=0; i<g3Dmapper->blockZNum; i++ )
  {
    // �ݒ肷��u���b�N��Z�l�����߂� 
    set_z = start_z + i;
    iz = set_z % sizez;
    if( iz < 0 )
    {
      iz += sizez;
    }
    for( j=0; j<g3Dmapper->blockXNum; j++ )
    {
      // �ݒ肷��u���b�N��X�l�����߂� 
      set_x = start_x + j;
      ix = set_x % sizex;
      if(ix < 0)
      {
        ix += sizex;
      }
      {
        idx = (i*g3Dmapper->blockXNum) + j;
        BLOCKINFO_SetBlockIdx( &new[idx].newBlockInfo, (iz * sizex) + ix );
        BLOCKINFO_SetBlockTrans( &new[idx].newBlockInfo, 
            FX_Mul(set_x<<FX32_SHIFT, blockWidth) + blockHalfWidth,
            0,
            FX_Mul(set_z<<FX32_SHIFT, blockWidth) + blockHalfWidth ); 
      }
    }
  } 
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void GetMapperBlockIdxY( const FLDMAPPER* g3Dmapper, const VecFx32* pos, BLOCK_NEWREQ* new )
{
	u16		sizex, sizez;
	u32		idx, blocky, countxz;
	fx32	blockWidth, blockHeight;
  fx32  blockHalfWidth;
	int		i, p, offsx, offsy, offsz;

	sizex = g3Dmapper->sizex;
	sizez = g3Dmapper->sizez;
	countxz = sizex * sizez;
	blockWidth      = g3Dmapper->blockWidth;
  blockHalfWidth  = FX_Div( g3Dmapper->blockWidth, 2*FX32_ONE );
	blockHeight     = g3Dmapper->blockHeight;

	blocky = FX_Whole( FX_Div( pos->y, blockHeight ) );
	if( pos->y - ( blocky * blockHeight ) > blockHeight/2 ){
		offsy = 1;
	} else {
		offsy = -1;
	}
	p = 0;

	for( i=0; i<countxz; i++ ){
		offsx = i % g3Dmapper->sizex;
		offsz = i / g3Dmapper->sizex;

		idx = blocky * countxz + offsz * sizex + offsx;

		if( idx >= g3Dmapper->totalSize ){
			idx = MAPID_NULL;
		}
    BLOCKINFO_SetBlockIdx( &new[p].newBlockInfo, idx );
    BLOCKINFO_SetBlockTrans( &new[p].newBlockInfo, 
       FX_Mul( offsx<<FX32_SHIFT, blockWidth ) + blockHalfWidth,
       FX_Mul( blocky<<FX32_SHIFT, blockHeight ),
       FX_Mul( offsz<<FX32_SHIFT, blockWidth) + blockHalfWidth );
		p++;
	}
	for( i=0; i<countxz; i++ ){
		offsx = i % g3Dmapper->sizex;
		offsz = i / g3Dmapper->sizex;

		idx = (blocky + offsy) * countxz + offsz * sizex + offsx;

		if( idx >= g3Dmapper->totalSize ){
			idx = MAPID_NULL;
		}
    BLOCKINFO_SetBlockIdx( &new[p].newBlockInfo, idx );
    BLOCKINFO_SetBlockTrans( &new[p].newBlockInfo, 
       FX_Mul( offsx<<FX32_SHIFT, blockWidth ) + blockHalfWidth,
       FX_Mul( (blocky + offsy)<<FX32_SHIFT, blockHeight ),
       FX_Mul( offsz<<FX32_SHIFT, blockWidth ) + blockHalfWidth );
		p++;
	}
}

//------------------------------------------------------------------
/**
 * @brief	�}�b�v�u���b�N�X�V�`�F�b�N
 */
//------------------------------------------------------------------
static BOOL	ReloadMapperBlock( FLDMAPPER* g3Dmapper, BLOCK_NEWREQ* new )
{
	BOOL addFlag, delFlag, delProcFlag, addProcFlag, reloadFlag;
	int i, j, c;
  u32 now_blockIdx, new_blockIdx;

	reloadFlag = FALSE;

	//�X�V�u���b�N�`�F�b�N
	delProcFlag = FALSE;
  // �S�J�����g�u���b�N�𑖍�
	for( i=0; i<g3Dmapper->blockNum; i++ )
  {
		if( BLOCKINFO_IsInBlockData( &g3Dmapper->blockWk[i].blockInfo)  )
    {
			delFlag = FALSE;
      now_blockIdx = BLOCKINFO_GetBlockIdx( &g3Dmapper->blockWk[i].blockInfo );
      
      // �J�����g�u���b�N�Ɠ������N�G�X�g�����O
			for( j=0; j<g3Dmapper->blockNum; j++ )
      {
        new_blockIdx = BLOCKINFO_GetBlockIdx( &new[j].newBlockInfo );
				if( (now_blockIdx == new_blockIdx) && (delFlag == FALSE) ) 
        { 
          // ���O����O��, ���W���R�s�[
          // (�������}�b�v�Ń��[�v�\������ꍇ, ����ID�̃u���b�N�����W�����ύX���Č���邱�Ƃ����邽��)
          VecFx32 trans;
          BLOCKINFO_GetBlockTrans( &new[j].newBlockInfo, &trans );
          GFL_G3D_MAP_SetTrans( g3Dmapper->blockWk[i].g3Dmap, &trans );
          BLOCKINFO_init( &new[j].newBlockInfo );
					delFlag = TRUE;
				}
			}
      // �J�����g�u���b�N�����N�G�X�g���ɑ��݂��Ȃ���΃N���A����
			if( delFlag == FALSE )
      {
        BLOCKINFO_init( &g3Dmapper->blockWk[i].blockInfo );
				GFL_G3D_MAP_SetDrawSw( g3Dmapper->blockWk[i].g3Dmap, FALSE );

        FIELD_BMODEL_MAN_ReleaseAllMapObjects( g3Dmapper->bmodel_man, g3Dmapper->blockWk[i].g3Dmap );
				// �g�����[�N�̏����N���A
				FLD_G3D_MAP_ExWork_ClearBlockData( &g3Dmapper->blockWk[i].g3DmapExWork );
				delProcFlag = TRUE;
			}
		}
	}

	//�X�V
	addProcFlag = FALSE;
	c = 0;

  // �S���N�G�X�g�u���b�N�𑖍�
	for( i=0; i<g3Dmapper->blockNum; i++ )
  { 
    // ���N�G�X�g�������Ă�����,
		if( BLOCKINFO_IsInBlockData( &new[i].newBlockInfo) )
    {
			addFlag = FALSE;
      // �J�����g�u���b�N��������, �󂫗v�f�ɃR�s�[����
			for( j=0; j<g3Dmapper->blockNum; j++ )
      { 
				if(( BLOCKINFO_IsInBlockData( &g3Dmapper->blockWk[j].blockInfo ) == FALSE )&&(addFlag == FALSE ))
        {
          u32 blockIdx = BLOCKINFO_GetBlockIdx( &new[i].newBlockInfo );
					u32 mapdatID = g3Dmapper->blocks[blockIdx].datID;
          VecFx32 trans;

          BLOCKINFO_GetBlockTrans( &new[i].newBlockInfo, &trans );

					if( mapdatID != FLDMAPPER_MAPDATA_NULL )
          { 
						GFL_G3D_MAP_SetLoadReq( g3Dmapper->blockWk[j].g3Dmap, mapdatID );
						GFL_G3D_MAP_SetTrans( g3Dmapper->blockWk[j].g3Dmap, &trans );
						GFL_G3D_MAP_SetDrawSw( g3Dmapper->blockWk[j].g3Dmap, TRUE );
					}
					g3Dmapper->blockWk[j].blockInfo = new[i].newBlockInfo;
					addFlag = TRUE;
					addProcFlag = TRUE;
				}
			}
      // �󂫗v�f�������̂͂�������
			if( addFlag == FALSE ){
				GF_ASSERT(0);
			}
		}
	}
	if( addProcFlag == TRUE ){
		reloadFlag = TRUE;
	}

	return reloadFlag;
}



//----------------------------------------------------------------------------
/**
 *	@brief  �u���b�N�������̂Ƃ��́A�u���b�N�J�n�ʒu�����߂�
 *
 *	@param	blockNum    �u���b�N��
 *	@param	width       �P�u���b�N��
 *	@param	pos         �����̈ʒu
 *
 *	@return �u���b�N�J�n�ʒu  �i���̒l�̏ꍇ������j
 */
//-----------------------------------------------------------------------------
static s32 GetMapperBlockStartIdxEven( u32 blockNum, fx32 width, fx32 pos )
{
  fx32 offs;
  fx32 block;
  fx32 width_half;
  s32 block_start;

  block_start = -((blockNum/2)-1); // �u���b�N�̒[�̈ʒu�����߂�A���炩���ߎ������܂߂邽�߂�-1���Ă���

  block = pos / width;
  offs  = pos % width;
  width_half = FX_Div( width, 2*FX32_ONE );

  if( offs < width_half ){
    block_start += block-1;
  }else{
    block_start += block;
  }

  return block_start;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �u���b�N����̂Ƃ��́A�u���b�N�J�n�ʒu�����߂�
 *
 *	@param	blockNum    �u���b�N��
 *	@param	width       �P�u���b�N��
 *	@param	pos         �����̈ʒu
 *
 *	@return �u���b�N�J�n�ʒu  �i���̒l�̏ꍇ������j
 */
//-----------------------------------------------------------------------------
static s32 GetMapperBlockStartIdxUnEven( u32 blockNum, fx32 width, fx32 pos )
{
  fx32 block;
  s32 block_start;

  block_start = -(blockNum/2);  // �u���b�N�̒[�̈ʒu�����߂�B
                                // ��́A�Q�Ŋ��邱�ƂŁA����Ɏ����̕�������

  block = pos / width;
  block_start += block;

  return block_start;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static s32 GetMapperBlockStartIdxLoop( u32 blockNum, fx32 width, fx32 pos )
{
  s32 cur_block;
  s32 start_block;
  cur_block = FX_Whole( FX_Div(pos, width) );
  start_block = cur_block - (blockNum / 2);
  if( blockNum % 2 == 0 )
  {
    fx32 offset = FX_Mod(pos, width);
    fx32 width_half = FX_Div( width, 2*FX32_ONE );
    if( width_half < offset ) start_block++;
  }
  return start_block;
}


//============================================================================================
/**
 *
 *
 *
 * @brief	�R�c�}�b�v���擾
 *
 *
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	���[�N������
 */
//------------------------------------------------------------------
void FLDMAPPER_GRIDINFODATA_Init( FLDMAPPER_GRIDINFODATA* gridInfoData )
{
	VEC_Fx16Set( &gridInfoData->vecN, 0, 0, 0 );
	gridInfoData->attr = 0;
	gridInfoData->height = 0;
}

void FLDMAPPER_GRIDINFO_Init( FLDMAPPER_GRIDINFO* gridInfo )
{
	int i;

	for( i=0; i<FLDMAPPER_GRIDINFO_MAX; i++ ){
		FLDMAPPER_GRIDINFODATA_Init( &gridInfo->gridData[i] );
	}
	gridInfo->count = 0;
}

//------------------------------------------------------------------
/**
 * @brief	�O���b�h�A�g���r���[�g�̑S�K�w�����������Ƀ��[�h���ꂽ�u���b�N�f�[�^�Q����擾
 *
 * @param g3Dmapper 
 * @param pos       x,z�Ƀf�[�^���擾������pos�̍��W�Ay�Ɍ��݂̍������i�[
 * @param gridInfo  �O���b�h�f�[�^���擾����FLDMAPPER_GRIDINFO�\���̌^�ϐ��ւ̃|�C���^
 *
 * @li  �ő�16�w�܂ł̕��w�A�g���r���[�g����z��Ɏ擾���܂��B
 *      FLDMAPPER_GRIDINFO->count > 1 �̎��́A�ǂ̃f�[�^���擾���邩�A�擾���őI������K�v������܂��B
 *
 *    �@�����K�w�y�я璷�f�[�^�������Ȃ�pos�ł� FLDMAPPER_GRIDINFO->gridData[0]�ւ̃A�N�Z�X�ŏ����擾�ł��܂����A
 *      �z��v�f�֒��ڃA�N�Z�X�̓I�X�X�����܂���B
 *      GetGridInfo()�����b�v���A���݂�Y�l�ɍł��߂��K�w�̃A�g���r���[�g���擾����A
 *      FLDMAPPER_GetGridData()�֐�������̂ŁA���������{�I�ɂ͗p���Ă��������B
 *      pos���P��̃f�[�^���������Ȃ��ꍇ�AgridData[0]�̒l��Ԃ��܂�
 *
 * @li  090915���_�̃}�b�v�\�z�ł́A�璷�ȃA�g���r���[�g�f�[�^(����pos�̃A�g���r���[�g���A�אڂ��镡���̃u���b�N�������Ă���ꍇ)
 *      �̑��݂͍l������Ă��܂���B����pos�̃A�g���r���[�g���𕡐��̃u���b�N�����ꍇ�A
 *      �A�g���r���[�g����`�̍���u���b�N���珇�ɃT�[�`���邽�߁A
 *      ��������ɂǂ̂悤�Ƀu���b�N�f�[�^���}�b�s���O����Ă��邩�ɂ��A�A�g���r���[�g�l�̔z��ւ̊i�[�����ς��܂��B
 *      GetGridInfo����̃��x���ł́A�z��̂ǂ����ǂ̃u���b�N���璊�o���ꂽ�f�[�^���͂킩��Ȃ�����
 *      ���Ƃ��΁A���݂�pos��������u���b�N�f�[�^��D�悷��A�Ȃǂ̃v���C�I���e�B���䂪�ł��Ȃ��̂Œ��ӂ��Ă�������
 */
//------------------------------------------------------------------
BOOL FLDMAPPER_GetGridInfo
	( const FLDMAPPER* g3Dmapper, const VecFx32* pos, FLDMAPPER_GRIDINFO* gridInfo )
{
	GFL_G3D_MAP_ATTRINFO attrInfo;
	VecFx32 trans;
	int		i, p;

	GF_ASSERT( g3Dmapper );
	if( g3Dmapper->blocks == NULL ){
		OS_Printf("�f�[�^���ǂݍ��܂�Ă��Ȃ�\n");
		return FALSE;
	}

	FLDMAPPER_GRIDINFO_Init( gridInfo );
	
	p = 0;

	for( i=0; i<g3Dmapper->blockNum; i++ ){
		if( GFL_G3D_MAP_GetDrawSw( g3Dmapper->blockWk[i].g3Dmap ) == TRUE ){
			fx32 min_x, min_z, max_x, max_z;

			GFL_G3D_MAP_GetTrans( g3Dmapper->blockWk[i].g3Dmap, &trans );

			min_x = trans.x - g3Dmapper->blockWidth/2;
			min_z = trans.z - g3Dmapper->blockWidth/2;
			max_x = trans.x + g3Dmapper->blockWidth/2;
			max_z = trans.z + g3Dmapper->blockWidth/2;

			//�u���b�N�͈͓��`�F�b�N�i�}�b�v�u���b�N�̂w�y���ʏ�n�_�j
			if(	(pos->x >= min_x)&&(pos->x < max_x)&&(pos->z >= min_z)&&(pos->z < max_z) ){

				GFL_G3D_MAP_GetAttr( &attrInfo, g3Dmapper->blockWk[i].g3Dmap, pos, g3Dmapper->blockWidth );
				if( attrInfo.mapAttrCount ){
					int j;

					if( (p + attrInfo.mapAttrCount) >= FLDMAPPER_GRIDINFO_MAX ){
						GF_ASSERT("height count over <- FLDMAPPER_GetGridInfo()\n");
            break;  //����ȏ�i�ނƃ��������j�󂳂��̂ŃX�g�b�v
					}
					for( j=0; j<attrInfo.mapAttrCount; j++ ){
						gridInfo->gridData[p+j].vecN = attrInfo.mapAttr[j].vecN;
						gridInfo->gridData[p+j].attr = attrInfo.mapAttr[j].attr;
						gridInfo->gridData[p+j].height = attrInfo.mapAttr[j].height;
					}
					p += attrInfo.mapAttrCount;
				}
			}
		}
	}
	gridInfo->count = p;

  //�g�����̓o�^
  GetExHight( g3Dmapper, pos, gridInfo );
	
  if( gridInfo->count ){
		return TRUE;
	}
//	OS_Printf("�f�[�^�����݂��Ă��Ȃ�\n");
	return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief	�O���b�h�A�g���r���[�g���擾
 *
 * @param g3Dmapper
 * @param pos       x,z�Ƀf�[�^���擾������pos�̍��W�Ay�Ɍ��݂̍������i�[
 * @param gridInfo  �O���b�h�f�[�^���擾����FLDMAPPER_GRIDINFO�\���̌^�ϐ��ւ̃|�C���^
 *
 * @li  ������FLDMAPPER_GetGridInfo()���Ăяo���A�S�A�g���r���[�g�f�[�^���擾�������
 *      ���݂�Y�l�ɍł��߂��K�w�̃A�g���r���[�g�f�[�^��Ԃ��܂��B
 */
//------------------------------------------------------------------
BOOL FLDMAPPER_GetGridData
	( const FLDMAPPER* g3Dmapper, const VecFx32* pos, FLDMAPPER_GRIDINFODATA* pGridData )
{
	FLDMAPPER_GRIDINFO gridInfo;
  fx32    o_diff;
	int		i, target;

  FLDMAPPER_GRIDINFODATA_Init(pGridData);

  if(FLDMAPPER_GetGridInfo(g3Dmapper,pos,&gridInfo) == FALSE){
    return FALSE;
  }
  if(gridInfo.count == 1){
    *pGridData = gridInfo.gridData[0];
	  return TRUE;
  }

  target = 0;
  o_diff = FX32_CONST(4095);//gridInfo.gridData[0].height - pos->y;

//  IWASAWA_Printf("CrossGrid %d pos = %08x\n",gridInfo.count,pos->y);

	for( i=0; i<gridInfo.count; i++ ){
    fx32  diff;

    //�A�g���r���[�gValue�������̃f�[�^�͌��ɂ���Ȃ�
    if( MAPATTR_IsEnable(gridInfo.gridData[i].attr) == FALSE){
    //if( MAPATTR_GetHitchFlag(gridInfo.gridData[i].attr) == TRUE){
      continue;
    }
    diff = gridInfo.gridData[i].height - pos->y;
    if(diff < 0){
      diff = FX_Mul(diff,FX32_CONST(-1));
    }
//    IWASAWA_Printf("idx = %d diff %08x , o_diff %08x\n",i,diff,o_diff);
    
		if( diff < o_diff ){
      target  = i;  //�^�[�Q�b�g�ύX
      o_diff = diff;
    }
	}
//  IWASAWA_Printf("TargetIs %d Y = %08x, attr = %08x\n",target,gridInfo.gridData[target].height,gridInfo.gridData[target].attr);
  *pGridData = gridInfo.gridData[target];
	return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief	�O���b�h�A�g���r���[�g���擾(�G�t�F�N�g�G���J�E���g��p�I�@���ł͎g��Ȃ��ŁI)
 *
 * @param g3Dmapper 
 * @param pos       x,z�Ƀf�[�^���擾������pos�̍��W���i�[
 * @param gridInfo  �O���b�h�f�[�^���擾����FLDMAPPER_GRIDINFO�\���̌^�ϐ��ւ̃|�C���^
 *
 * @li  ���w�f�[�^�y�ъg�������f�[�^�𖳎����x�[�X�K�w�f�[�^�̂݁��J�����g�u���b�N�݂̂���f�[�^���擾���܂�
 */
//------------------------------------------------------------------
BOOL FLDMAPPER_GetGridDataForEffectEncount
	( const FLDMAPPER* g3Dmapper, int blockIdx, const VecFx32* pos, FLDMAPPER_GRIDINFODATA* pGridData )
{
	GFL_G3D_MAP_ATTRINFO attrInfo;
  GFL_G3D_MAP* map;

	GF_ASSERT( g3Dmapper );
	if( g3Dmapper->blocks == NULL ){
		OS_Printf("�f�[�^���ǂݍ��܂�Ă��Ȃ�\n");
		return FALSE;
	}

  FLDMAPPER_GRIDINFODATA_Init(pGridData);
	
  map = g3Dmapper->blockWk[blockIdx].g3Dmap;
  if( GFL_G3D_MAP_GetDrawSw( map ) == FALSE ){
    return FALSE;
  }
	GFL_G3D_MAP_GetAttr( &attrInfo, map, pos, g3Dmapper->blockWidth );
	if( attrInfo.mapAttrCount == 0 ){
    return FALSE;
  }
	pGridData->vecN = attrInfo.mapAttr[0].vecN;
	pGridData->attr = attrInfo.mapAttr[0].attr;
	pGridData->height = attrInfo.mapAttr[0].height;
	return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief	�͈͊O�`�F�b�N
 */
//------------------------------------------------------------------
BOOL FLDMAPPER_CheckOutRange( const FLDMAPPER* g3Dmapper, const VecFx32* pos )
{
	fx32 widthx, widthz;

	GF_ASSERT( g3Dmapper );
	if( g3Dmapper->blocks == NULL ){
		return TRUE;
	}
	if( g3Dmapper->mode == FLDMAPPER_MODE_SCROLL_XZ_LOOP )
  {
    return FALSE;
  }

	widthx = g3Dmapper->sizex * g3Dmapper->blockWidth;
	widthz = g3Dmapper->sizez * g3Dmapper->blockWidth;

	if( ( pos->x >= 0 )&&( pos->x < widthx )&&( pos->z >= 0 )&&( pos->z < widthz ) ){
		return FALSE;
	}
	return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief	�T�C�Y�擾
 */
//------------------------------------------------------------------
void FLDMAPPER_GetSize( const FLDMAPPER* g3Dmapper, fx32* x, fx32* z )
{
	GF_ASSERT( g3Dmapper );
	if( g3Dmapper->blocks == NULL ){
		*x = 0;
		*z = 0;
		return;
	}
	*x = g3Dmapper->sizex * g3Dmapper->blockWidth;
	*z = g3Dmapper->sizez * g3Dmapper->blockWidth;
	return;
}

//----------------------------------------------------------------------------
/**
 * @brief �u���b�N�P�ʂł�XZ�ʒu���擾
 * @param	g3Dmapper     �t�B�[���h�}�b�p�[�|�C���^
 * @param blockx      X�ʒu���󂯎�邽�߂̃|�C���^
 * @param blockz      Z�ʒu���󂯎�邽�߂̃|�C���^
 */
//----------------------------------------------------------------------------
void FLDMAPPER_GetBlockXZPos( const FLDMAPPER * g3Dmapper, u32 * blockx, u32 * blockz )
{
  *blockx = FX_Whole( FX_Div( g3Dmapper->posCont.x, g3Dmapper->blockWidth ) );
  *blockz = FX_Whole( FX_Div( g3Dmapper->posCont.z, g3Dmapper->blockWidth ) );
}

#if 0
//--------------------------------------------------------------
/**
 * �}�b�v�f�[�^�t�@�C���^�C�v���擾
 * @param
 * @retval
 */
//--------------------------------------------------------------
FLDMAPPER_FILETYPE FLDMAPPER_GetFileType( const FLDMAPPER *g3Dmapper )
{
	return( g3Dmapper->g3DmapFileType );
}
#endif

//--------------------------------------------------------------
//	�`��I�t�Z�b�g
//--------------------------------------------------------------
void FLDMAPPER_SetDrawOffset( FLDMAPPER *g3Dmapper, const VecFx32 *offs )
{
	g3Dmapper->globalDrawOffset = *offs;
}

void FLDMAPPER_GetDrawOffset( const FLDMAPPER *g3Dmapper, VecFx32 *offs )
{
	*offs = g3Dmapper->globalDrawOffset;
}


//--------------------------------------------------------------
/**
 * @brief �z�u���f���}�l�W���[���擾����
 */
//--------------------------------------------------------------
FIELD_BMODEL_MAN * FLDMAPPER_GetBuildModelManager( FLDMAPPER* g3Dmapper)
{ 
  return g3Dmapper->bmodel_man;
}

//----------------------------------------------------------------------------
/**
 *	@brief  WFBC���[�N�̎擾
 *
 *	@param	g3Dmapper �}�b�p�[
 *
 *	@return WFBC���[�N
 */
//-----------------------------------------------------------------------------
FIELD_WFBC* FLDMAPPER_GetWfbcWork( const FLDMAPPER* g3Dmapper)
{
  GF_ASSERT( g3Dmapper );
  return g3Dmapper->wfbcwork;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �`�悷��WFBC�X����ݒ肷��
 *
 *	@param	g3Dmapper
 *	@param	cp_core 
 */
//-----------------------------------------------------------------------------
void FLDMAPPER_SetWfbcData( FLDMAPPER* g3Dmapper, const FIELD_WFBC_CORE* cp_core )
{
  GF_ASSERT( g3Dmapper );
  FIELD_WFBC_SetUp( g3Dmapper->wfbcwork, cp_core );
}



//============================================================================================
//============================================================================================
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static FIELD_GRANM * createGroundAnime( u32 blockNum, GFL_G3D_RES* globalTexture,
    const FLDMAPPER_RESIST_GROUND_ANIME * resistGroundAnimeData, u32 heapID )
{ 
  static FIELD_GRANM_SETUP granm_setup = {
    FALSE, FALSE,
    0,
    ARCID_AREA_ITA, 0,
    ARCID_AREA_ITP_TBL, 0, 
    ARCID_AREA_ITP, ARCID_AREA_ITP_TEX,
  };

  //  �u���b�N��
  granm_setup.block_num = blockNum;

  // ITA�A�j���[�V�����̐ݒ�
  if( resistGroundAnimeData->ita_datID == FLDMAPPER_MAPDATA_NULL ){
    granm_setup.ita_use			= FALSE;
  }else{
    granm_setup.ita_use			= TRUE;
    granm_setup.ita_dataID	= resistGroundAnimeData->ita_datID;
  }

  // ITP�A�j���[�V�����̐ݒ�
  if( resistGroundAnimeData->itp_anm_datID == FLDMAPPER_MAPDATA_NULL ){
    granm_setup.itp_use			= FALSE;
  }else{
    granm_setup.itp_use			= TRUE;
    granm_setup.itp_tblID		= resistGroundAnimeData->itp_anm_datID;
  }
	return FIELD_GRANM_Create( &granm_setup, globalTexture, heapID );
}


//============================================================================================
//============================================================================================
//-----------------------------------------------------------------------------
/**
 *			GFL_G3D_MAP�g�����[�N����
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief	�g�����[�N����A�n�ʃA�j���[�V�����̗L�����擾
 *
 *	@param	cp_wk			���[�N
 *
 *	@retval	TRUE	����
 *	@retval	FALSE	�Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL FLD_G3D_MAP_EXWORK_IsGranm( const FLD_G3D_MAP_EXWORK* cp_wk )
{
	return FLD_G3D_MAP_ExWork_IsGranm( cp_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�g�����[�N����A�n�ʃA�j���[�V�������[�N���擾
 *
 *	@param	cp_wk		���[�N
 *	
 *	@return	�n�ʃA�j���[�V������[��
 */
//-----------------------------------------------------------------------------
FIELD_GRANM_WORK* FLD_G3D_MAP_EXWORK_GetGranmWork( const FLD_G3D_MAP_EXWORK* cp_wk )
{
	return FLD_G3D_MAP_ExWork_GetGranmWork( cp_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�z�u���f���̐��䃏�[�N�擾
 *
 *	@param	cp_wk		���[�N
 *
 *	@return	�z�u���f���̐��䃏�[�N
 */
//-----------------------------------------------------------------------------
FIELD_BMODEL_MAN* FLD_G3D_MAP_EXWORK_GetBModelMan( const FLD_G3D_MAP_EXWORK* cp_wk )
{
	return  cp_wk->bmodel_man;
}

//----------------------------------------------------------------------------
/**
 *	@brief  WFBC�X���
 *
 *	@param	cp_wk   ���[�N
 */
//-----------------------------------------------------------------------------
FIELD_WFBC* FLD_G3D_MAP_EXWORK_GetWFBCWork( const FLD_G3D_MAP_EXWORK* cp_wk )
{
  return cp_wk->wfbcwork;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�}�b�v�̃C���f�b�N�X�擾
 *
 *	@param	cp_wk		���[�N
 *
 *	@return	�}�b�v�̃C���f�b�N�X
 */
//-----------------------------------------------------------------------------
const u32 FLD_G3D_MAP_EXWORK_GetMapIndex( const FLD_G3D_MAP_EXWORK* cp_wk )
{
	return  cp_wk->mapIndex;
}

//----------------------------------------------------------------------------
/**
 *	@brief	GFL_G3D_MAP�̊g�����[�N������
 *
 *	@param	p_wk				���[�N
 *	@param	g3Dmapper		�}�b�p�[���[�N
 *	@param	index				�u���b�N�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
static void FLD_G3D_MAP_ExWork_Init( FLD_G3D_MAP_EXWORK* p_wk, FLDMAPPER* g3Dmapper, u32 index )
{
	// �n�ʃA�j��
	p_wk->p_granm_wk = FIELD_GRANM_GetWork( g3Dmapper->granime, index );
  p_wk->bmodel_man = g3Dmapper->bmodel_man;
  p_wk->wfbcwork = g3Dmapper->wfbcwork;
  p_wk->mapIndex = index;
}

//----------------------------------------------------------------------------
/**
 *	@brief	GFL_G3D_MAP�̊g�����[�N�j��
 *
 *	@param	p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void FLD_G3D_MAP_ExWork_Exit( FLD_G3D_MAP_EXWORK* p_wk )
{
	if( p_wk->p_granm_wk ){
		// �u���b�N���f���Ƃ̃����N������
		FIELD_GRANM_WORK_Release( p_wk->p_granm_wk );
		p_wk->p_granm_wk = NULL;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�u���b�N�Ɋ��蓖�ĂĂ��������N���A����
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void FLD_G3D_MAP_ExWork_ClearBlockData( FLD_G3D_MAP_EXWORK* p_wk )
{
	if( p_wk->p_granm_wk ){
		// �u���b�N���f���Ƃ̃����N������
		FIELD_GRANM_WORK_Release( p_wk->p_granm_wk );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�n�ʃA�j���[�V���������邩�`�F�b�N
 *
 *	@param	cp_wk		���[�N
 *
 *	@retval	TRUE	����
 *	@retval	FALSE	�Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL FLD_G3D_MAP_ExWork_IsGranm( const FLD_G3D_MAP_EXWORK* cp_wk )
{
	if( cp_wk->p_granm_wk ){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�n�ʃA�j���[�V�����̃��[�N���擾����
 *
 *	@param	cp_wk		���[�N
 *
 *	@retval	�n�ʃA�j���[�V�����̃��[�N
 *	@retval	NULL �n�ʃA�j���[�V�������Ȃ�
 */
//-----------------------------------------------------------------------------
static FIELD_GRANM_WORK* FLD_G3D_MAP_ExWork_GetGranmWork( const FLD_G3D_MAP_EXWORK* cp_wk )
{
	return cp_wk->p_granm_wk;
}

//============================================================================================
//============================================================================================
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void BLOCKINFO_init(BLOCKINFO * info)
{ 
		info->blockIdx = MAPID_NULL;
		VEC_Set( &info->trans, 0, 0, 0 );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �u���b�N�C���f�b�N�X�̐ݒ�
 *
 *	@param	info      �C���t�H���[�N
 *	@param	blockIdx  �u���b�N�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
static void BLOCKINFO_SetBlockIdx(BLOCKINFO * info, u32 blockIdx)
{
  info->blockIdx = blockIdx;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �u���b�N��񂪓����Ă��邩�`�F�b�N
 *
 *	@param	info  �C���t�H���[�N
 *
 *	@retval TRUE  �����Ă���
 *	@retval FALSE �����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL BLOCKINFO_IsInBlockData(const BLOCKINFO * info)
{
  if( info->blockIdx == MAPID_NULL ){
    return FALSE;
  }
  return TRUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  
 *
 *	@param	info
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static u32 BLOCKINFO_GetBlockIdx(const BLOCKINFO * info)
{
  return info->blockIdx;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �u���b�N�ʒu��ݒ�
 *  
 *	@param	info      ���[�N
 *	@param	trans     �ʒu
 */
//-----------------------------------------------------------------------------
static void BLOCKINFO_SetBlockTrans(BLOCKINFO * info, fx32 x, fx32 y, fx32 z )
{
	VEC_Set( &info->trans, x, y, z );
}
static void BLOCKINFO_SetBlockTransVec(BLOCKINFO * info, const VecFx32* trans)
{
  info->trans = *trans;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �u���b�N�ʒu�̎擾
 *
 *	@param	info      ���[�N
 *	@param	trans     �ʒu�i�[��
 */
//-----------------------------------------------------------------------------
static void BLOCKINFO_GetBlockTrans(const BLOCKINFO * info, VecFx32* trans)
{
   *trans = info->trans;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����擾�֐�
 *
 *	@param	g3Dmapper     �t�B�[���h�}�b�p�[�|�C���^
 *	@param  pos           �w��ʒu���W
 *	@param  gridInfo      �O���b�h���i�[�o�b�t�@
 */
//-----------------------------------------------------------------------------
static void GetExHight( const FLDMAPPER* g3Dmapper, const VecFx32 *pos, FLDMAPPER_GRIDINFO* gridInfo )
{
  int i,n;
  int count;
  int num;
  int gridX,gridZ;
  VecFx16 normVec = {0,FX32_ONE,0};

  n = 0;
  count = gridInfo->count;
  num = EXH_GetDatNum(g3Dmapper->ExHeightList);

  gridX = pos->x / FIELD_CONST_GRID_FX32_SIZE;
  gridZ = pos->z / FIELD_CONST_GRID_FX32_SIZE;

  for (i=0;i<num;i++){
    BOOL rc;
    rc = EXH_HitCheckHeight(gridX, gridZ, g3Dmapper->ExHeightList, i);
    if ( rc ){
      int idx;
      n++;
      idx = count+n-1;
      if(idx >= FLDMAPPER_GRIDINFO_MAX){
        break;  //����ȏ�͐ς߂Ȃ��̂Ŕ�����
      }
      gridInfo->gridData[idx].vecN = normVec;
			gridInfo->gridData[idx].attr = 0;
			gridInfo->gridData[idx].height = EXH_GetHeight(i, g3Dmapper->ExHeightList);
    }
  }
  gridInfo->count += n;

  GF_ASSERT((gridInfo->count < FLDMAPPER_GRIDINFO_MAX)&&"�O���b�h�f�[�^�̊K�w�I�[�o�[\n");

}



