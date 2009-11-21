//======================================================================
/**
 * @file	field_debug.c
 * @brief	�t�B�[���h�f�o�b�O����
 * @author	kagaya
 * @date	08.09.29
 */
//======================================================================
#include "field_debug.h"
#include "fieldmap.h"

#include "fldmmdl.h"
#include "field/map_matrix.h"
#include "field/zonedata.h"

#include "arc/others.naix"

//======================================================================
//	define
//======================================================================
#define DEBUG_BGFRAME (GFL_BG_FRAME2_M) //�g�p����BG�t���[��
#define DEBUG_PANO_FONT (15) //�t�H���g�Ŏg�p����p���b�gNo


#define BG_COLORMODE    (GX_BG_COLORMODE_16)
#define BG_SCRBASE      (GX_BG_SCRBASE_0xf800)
#define BG_CHARBASE      (GX_BG_CHARBASE_0x18000)

//======================================================================
//	typedef struct
//======================================================================

//--------------------------------------------------------------
///	FIELD_DEBUG_WORK 
//--------------------------------------------------------------
struct _TAG_FIELD_DEBUG_WORK
{
	HEAPID heapID;	//�f�o�b�O�Ŏg�p����HEAPID
	u32 bgFrame;	//�f�o�b�O�Ŏg�p����BG FRAME
	FIELDMAP_WORK *pFieldMainWork; //FIELDMAP_WORK*
	
	BOOL flag_bgscr_load;	//�f�o�b�OBG�ʂ̃X�N���[�����[�h
	BOOL flag_pos_print;	//���W�\���؂�ւ�
};

//======================================================================
//	proto
//======================================================================
static void DebugFont_Init( FIELD_DEBUG_WORK *work );
static void DebugFont_Put( u16 *screen, char c, u16 x, u16 y );
static void DebugFont_Print(
	FIELD_DEBUG_WORK *work, u16 x, u16 y, const char *msgBuf );
static void DebugFont_ClearLine( FIELD_DEBUG_WORK *work, u16 y );

static void DebugFieldPosPrint_Proc( FIELD_DEBUG_WORK *work );

//======================================================================
//	�t�B�[���h�f�o�b�O�V�X�e��
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h�f�o�b�O�V�X�e�� ������
 * @param 	pFieldMainWork FIELDMAP_WORK
 * @retval	FIELD_DEBUG_WORK
 */
//--------------------------------------------------------------
FIELD_DEBUG_WORK * FIELD_DEBUG_Init(
	FIELDMAP_WORK *pFieldMainWork, HEAPID heapID )
{
	FIELD_DEBUG_WORK *work;
	work = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_DEBUG_WORK) );
	
	work->heapID = heapID;
	work->bgFrame = DEBUG_BGFRAME;
	work->pFieldMainWork = pFieldMainWork;
	
	{	//�f�o�b�O�p�t�H���g������
		DebugFont_Init( work );
	}
	
	{	//�e�f�o�b�O�@�\������
	}
	
	return( work );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�f�o�b�O�V�X�e�� �폜
 * @param 	work	FIELD_DEBUG_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
void FIELD_DEBUG_Delete( FIELD_DEBUG_WORK *work )
{
	GFL_HEAP_FreeMemory( work );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�f�o�b�O�V�X�e�� �풓����
 * @param	work	FIELD_DEBUG_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
void FIELD_DEBUG_UpdateProc( FIELD_DEBUG_WORK *work )
{
	if( work->flag_pos_print == TRUE ){ //���W�\��
		DebugFieldPosPrint_Proc( work );
	}
	
	if( work->flag_bgscr_load == TRUE ){ //�f�o�b�O�pBG�X�N���[�����f
		GFL_BG_LoadScreenReq( work->bgFrame );
		work->flag_bgscr_load = FALSE;
	}
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�f�o�b�O�V�X�e���@�a�f�ݒ蕜�A
 * @param	work	FIELD_DEBUG_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
void FIELD_DEBUG_RecoverBgCont( FIELD_DEBUG_WORK *work )
{
  //�Z�b�g�A�b�v���Ȃ���
  G2_SetBG2ControlText(
      GX_BG_SCRSIZE_TEXT_256x256,
      BG_COLORMODE,
      BG_SCRBASE,
      BG_CHARBASE);
}

//======================================================================
//	�V�X�e���t�H���g�\��
//======================================================================
#if 0
//------------------------------------------------------------------
/**
 * @brief	�w��t�@�C���̓ǂݍ���
 * @param	path	�t�@�C���ւ̃p�X
 * @param	buf	�ǂݏo�����f�[�^���i�[����|�C���^
 * @return	int	�ǂݍ��񂾃t�@�C���T�C�Y
 */
//------------------------------------------------------------------
static void * ReadFile( const char *path, HEAPID heapID )
{
	FSFile file;
	void*  memory;

	FS_InitFile(&file);
	if (FS_OpenFile(&file, path)){

		u32	fileSize = FS_GetLength(&file);		
		memory = GFL_HEAP_AllocMemory( heapID, fileSize );

		if (memory == NULL){
			OS_Printf("no enough memory.\n");
		}else{
			if(FS_ReadFile(&file, memory, fileSize) != fileSize){
				// �t�@�C���T�C�Y���ǂݍ��߂Ă��Ȃ��ꍇ
				GFL_HEAP_FreeMemory( memory );
				memory = NULL;
				OS_Printf("file reading failed.\n");
				GF_ASSERT( 0 );
			}
		}
		(void)FS_CloseFile(&file);
	}else{
		OS_Printf("FS_OpenFile(\"%s\") ... ERROR!\n",path);
		GF_ASSERT( 0 );
		memory = NULL;
	}
	return memory;
}

//------------------------------------------------------------------
/**
 * @brief	�w��t�@�C���̃T�C�Y��Ԃ�
 * @param	path	�t�@�C���ւ̃p�X
 * @return	int	�t�@�C���T�C�Y(bytes)
 */
//------------------------------------------------------------------
static int GetFileSize(char *path)
{
  FSFile *fp;
  u32 size = 0;
  fp = OS_AllocFromMainArenaLo(sizeof(FSFile), 4);
  if(FS_OpenFile(fp, path) == TRUE){
    size = FS_GetLength(fp);
    (void)FS_CloseFile(fp);
  } else {
    SDK_ASSERT(FALSE);
  }
  return (int)size;
}

//--------------------------------------------------------------
/**
 * �t�@�C���p�X���w�肵�ăf�[�^�̓ǂݍ���
 *
 * @param	heapID	�������m�ۂ�����q�[�vID
 * @param	path	�t�@�C���p�X
 *
 * @retval	�f�[�^��ǂݍ��񂾃A�h���X
 */
//--------------------------------------------------------------
static void * LoadFile( HEAPID heapID, const char *path )
{
	void	*buf;
	buf = ReadFile(path,heapID);
	return buf;
}

//--------------------------------------------------------------
/**
 * NITRO-CHARACTER�̃p���b�g�f�[�^��W�J
 * @param	mem		�W�J�ꏊ
 * @param	heapID	�w��q�[�v�̈��`
 * @param	path	�t�@�C���p�X
 * @return	�p���b�g�f�[�^
 * @li		pal->pRawData = �p���b�g�f�[�^
 * @li		pal->szByte   = �T�C�Y
*/
//--------------------------------------------------------------
static NNSG2dPaletteData * DebugFont_OpenPalette(
		void **mem, HEAPID heapID, u32 arc_id, u32 arc_idxconst char * path )
{
	NNSG2dPaletteData * pal;
	
	*mem = GFL_ARC_LoadDataAlloc(
		ARCID_OTHERS, NARC_others_nfont_NCLR, HEAPID );
	
	if( mem == NULL ){
		GF_ASSERT( 0 );
	}
	
	if( NNS_G2dGetUnpackedPaletteData(*mem,&pal) == FALSE ){
		GF_ASSERT( 0 );
	}
	
	return pal;
}

//--------------------------------------------------------------
/**
 * NITRO-CHARACTER�̃L�����f�[�^��ǂݍ���
 *
 * @param	frmnum	BG�t���[���ԍ�
 * @param	path	�t�@�C���p�X
 * @param	offs	�I�t�Z�b�g�i�L�����P��
 *
 * @return	none
 */
//--------------------------------------------------------------
static void NTRCHR_BGCharLoad(
	u8 frmnum, const char * path, u32 offs, HEAPID heapID )
{
	void * buf;
	NNSG2dCharacterData * dat;
	
	buf = LoadFile( heapID, path );
	
	if( buf == NULL ){
		GF_ASSERT( 0 );
	}

	if( NNS_G2dGetUnpackedBGCharacterData(buf,&dat) == FALSE ){
		GF_ASSERT( 0 );
	}
	
	GFL_BG_LoadCharacter( frmnum, dat->pRawData, dat->szByte, offs );
	GFL_HEAP_FreeMemory( buf );
}
#endif

//--------------------------------------------------------------
/**
 * �V�X�e���t�H���g�\���@�O���t�B�b�N������
 * @param	heapID HEAPID
 * @retval	nothing
 */
//--------------------------------------------------------------
static void DebugFont_SetGraphic( HEAPID heapID )
{
	void *buf;
	
	{	//�p���b�g
		NNSG2dPaletteData *pal;
		buf = GFL_ARC_LoadDataAlloc(
			ARCID_OTHERS, NARC_others_nfont_NCLR, heapID );
		GF_ASSERT( buf != NULL );
		
		if( NNS_G2dGetUnpackedPaletteData(buf,&pal) == FALSE ){
			GF_ASSERT( 0 );
		}
		
		GFL_BG_LoadPalette( DEBUG_BGFRAME,
			pal->pRawData, 32, DEBUG_PANO_FONT*32 );
		GFL_HEAP_FreeMemory( buf );
	}
	
	{	//�L����
		NNSG2dCharacterData *chr;
		buf = GFL_ARC_LoadDataAlloc(
			ARCID_OTHERS, NARC_others_nfont_NCGR, heapID );
		GF_ASSERT( buf != NULL );
		
		if( NNS_G2dGetUnpackedBGCharacterData(buf,&chr) == FALSE ){
			GF_ASSERT( 0 );
		}
		GFL_BG_LoadCharacter( DEBUG_BGFRAME,
			chr->pRawData, chr->szByte, 0 );
		GFL_HEAP_FreeMemory( buf );
	}
}

//--------------------------------------------------------------
/**
 * �V�X�e���t�H���g�\���@������
 * @param	work	FIELD_DEBUG_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void DebugFont_Init( FIELD_DEBUG_WORK *work )
{
	{	//BG Frame
		GFL_BG_BGCNT_HEADER bgcntText = {
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, BG_COLORMODE,
			BG_SCRBASE, BG_CHARBASE, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		
		GFL_BG_SetBGControl( work->bgFrame, &bgcntText, GFL_BG_MODE_TEXT );
		GFL_BG_SetVisible( work->bgFrame, VISIBLE_ON );
		GFL_BG_SetPriority( work->bgFrame, 0 );
		GFL_BG_FillCharacter( work->bgFrame, 0x00, 1, 0 );
		GFL_BG_FillScreen( work->bgFrame,
			0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
		GFL_BG_LoadScreenReq( work->bgFrame );
	}
	
	DebugFont_SetGraphic( work->heapID );
}

//--------------------------------------------------------------
/**
 * �V�X�e���t�H���g�\�� �ꕶ���\��
 * @param	work	FIELD_DEBUG_WORK
 * @param	x		�\������X���W �L�����P��
 * @param	y		�\������Y���W �L�����P��
 * @param	msgBuf	�\�����镶����
 * @retval	nothing
 */
//--------------------------------------------------------------
static void DebugFont_Put( u16 *screen, char c, u16 x, u16 y )
{
	if(c>=0x61&&c<=(0x61+6)){ //sprintf()16�i�p���R�[�h��u��
		c = (c-0x61) + 0x41; //0x41 = 'A'
	}
	if(c==0x20){
		c = 0x29;
	}
	if(c=='='){
		c = 0x30+10;
	}
	if(c=='-'){   // �}�C�i�X�\���������̂�M�ɒu��
		c = 'M';
	}
	screen[x+y*32] = (DEBUG_PANO_FONT<<12)+(c-0x2F);
}

//--------------------------------------------------------------
/**
 * �V�X�e���t�H���g�\�� �\��
 * @param	work	FIELD_DEBUG_WORK
 * @param	x		�\������X���W �L�����P��
 * @param	y		�\������Y���W �L�����P��
 * @param	msgBuf	�\�����镶����
 * @retval	nothing
 */
//--------------------------------------------------------------
static void DebugFont_Print(
	FIELD_DEBUG_WORK *work, u16 x, u16 y, const char *msgBuf )
{
	u16 n = 0;
	u16 *screen;
	screen = GFL_BG_GetScreenBufferAdrs( DEBUG_BGFRAME );
	while( msgBuf[n] != 0 ){
		DebugFont_Put(screen,msgBuf[n],x+n,y);
		n++;
	}
	
	work->flag_bgscr_load = TRUE;
}

//--------------------------------------------------------------
/**
 * �V�X�e���t�H���g�\���@�s�N���A
 * @param	y
 * @retval	nothing
 */
//--------------------------------------------------------------
static void DebugFont_ClearLine( FIELD_DEBUG_WORK *work, u16 y )
{
	char buf[33];
	MI_CpuFill8( buf, 0x20, 32 );
	buf[32] = 0;
	DebugFont_Print( work, 0, y, buf );
}

//======================================================================
//	�t�B�[���h�f�o�b�O�@�t�B�[���h���W�\��
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h���W�\��
 * @param	work	FIELD_DEBUG_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
void FIELD_DEBUG_SetPosPrint( FIELD_DEBUG_WORK *work )
{
	if( work->flag_pos_print == FALSE ){
		work->flag_pos_print = TRUE;
		GFL_BG_SetVisible( work->bgFrame, VISIBLE_ON );
	}else{
		work->flag_pos_print = FALSE;
		GFL_BG_SetVisible( work->bgFrame, VISIBLE_OFF );
	}
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���W�\��
 * @param	work	FIELD_DEBUG_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void DebugFieldPosPrint_Proc( FIELD_DEBUG_WORK *work )
{
	char str[256];
	GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( work->pFieldMainWork );
	PLAYER_WORK *player = GAMESYSTEM_GetMyPlayerWork( gsys );
	const VecFx32 *pos = PLAYERWORK_getPosition( player );
  FLDMAPPER * g3Dmapper = FIELDMAP_GetFieldG3Dmapper( work->pFieldMainWork );
	
	{	//���W�\��
    u32 gx, gz;

		DebugFont_ClearLine( work, 0 );
		sprintf( str, "X %d %xH GRID %d",
			FX_Whole(pos->x), pos->x, SIZE_GRID_FX32(pos->x) );
		DebugFont_Print( work, 0, 0, str );
		
		DebugFont_ClearLine( work, 1 );
		sprintf( str, "Y %d %xH GRID %d",
			FX_Whole(pos->y), pos->y, SIZE_GRID_FX32(pos->y) );
		DebugFont_Print( work, 0, 1, str );
		
		DebugFont_ClearLine( work, 2 );
		sprintf( str, "Z %d %xH GRID %d",
			FX_Whole(pos->z), pos->z, SIZE_GRID_FX32(pos->z) );
		DebugFont_Print( work, 0, 2, str );

		DebugFont_ClearLine( work, 3 );
    FLDMAPPER_GetBlockXZPos( g3Dmapper, &gx, &gz );
    sprintf( str, "BLOCK (%d,%d) LOCAL (%d,%d)", gx, gz, SIZE_GRID_FX32(pos->x)%32, SIZE_GRID_FX32(pos->z)%32 );
    DebugFont_Print( work, 0, 3, str );
	}
	
	{	//�}�b�v�A�g���r���[�g�\��
		u32 attr;
		int x,y,z;
		VecFx32 a_pos;
		const FLDMAPPER *pG3DMapper;
		FLDMAPPER_GRIDINFO gridInfo;
		
		pG3DMapper = FIELDMAP_GetFieldG3Dmapper( work->pFieldMainWork );
		
		if( pG3DMapper == NULL ){
			return;
		}
		
		sprintf( str, "ATTRIBUTE" );
		DebugFont_ClearLine( work, 4 );
		DebugFont_Print( work, 0, 4, str );
		
		a_pos = *pos;
		a_pos.z -= GRID_SIZE_FX32( 1 );
		
		for( z = 0, y = 5; z < 3; z++, y++, a_pos.z += GRID_SIZE_FX32(1) ){
			DebugFont_ClearLine( work, y );
			for( x = 0, a_pos.x = pos->x - GRID_SIZE_FX32(1);
					x < 3*10; x += 10, a_pos.x += GRID_SIZE_FX32(1) ){
#if 0
				if( FLDMAPPER_GetGridInfo(pG3DMapper,&a_pos,&gridInfo) ){
					attr = gridInfo.gridData[0].attr;
					sprintf( str, "%08xH", attr );
				}else{
					sprintf( str, "GET ERROR", attr );
				}
#else
        {
          FLDMAPPER_GRIDINFODATA gridData;
          if( FLDMAPPER_GetGridData(pG3DMapper,&a_pos,&gridData)){
					  sprintf( str, "%08xH", gridData.attr );
          }else{
  					sprintf( str, "GET ERROR", gridData.attr );
          }
        }
#endif
				DebugFont_Print( work, x, y, str );
			}
		}
	}
	
	{	//�}�b�v�u���b�N�@ZONE_ID�\��
		u32 zone_id = MAP_MATRIX_ZONE_ID_NON;
    GAMEDATA * gamedata = GAMESYSTEM_GetGameData( gsys );
		MAP_MATRIX *pMatrix = GAMEDATA_GetMapMatrix( gamedata );
		
		if( MAP_MATRIX_CheckVectorPosRange(pMatrix,pos->x,pos->z) == TRUE ){
			zone_id = MAP_MATRIX_GetVectorPosZoneID(
					pMatrix, pos->x, pos->z );
		}
		
		DebugFont_ClearLine( work, 8 );
		sprintf( str, "ZONE ID " );
		DebugFont_Print( work, 0, 8, str );
		
		if( zone_id == MAP_MATRIX_ZONE_ID_NON ){
			sprintf( str, "ERROR" );
		}else{
			ZONEDATA_DEBUG_GetZoneName( str, zone_id );
		}
		
		DebugFont_Print( work, 8, 8, str );
	}

  { // ���[�����W�̕\��
    RAIL_LOCATION location = {0};

    
    if( PLAYERWORK_getPosType(player) == LOCATION_POS_TYPE_RAIL )
    {
      location = *PLAYERWORK_getRailPosition( player );
      sprintf( str, "RAIL INDEX %d FRONT %d SIDE %d",
        location.rail_index, location.line_grid, location.width_grid );
    }
    else
    {
      sprintf( str, "RAIL DATA NONE",
        location.rail_index, location.line_grid, location.width_grid );
    }
		DebugFont_ClearLine( work, 9 );
		DebugFont_Print( work, 0, 9, str );
  }
}
