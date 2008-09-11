//=============================================================================================
/**
 * @file	gf_font.c
 * @brief	�t�H���g�f�[�^�}�l�[�W��
 * @author	taya
 *
 * @date	2005.09.14	�쐬
 * @date	2008.10.11	�]���̓Ǝ��t�H�[�}�b�g����NitroFont�t�H�[�}�b�g�ɕύX
 */
//=============================================================================================
#include	<gflib.h>
#include	<arc_tool.h>

#include	"print\gf_font.h"

typedef u8 (*pWidthGetFunc)(const GFL_FONT*, u32);
typedef void (*pGetBitmapFunc)(const GFL_FONT*, u32, void*, u16*, u16* );


enum {
	SRC_CHAR_SIZE = 0x10,
	SRC_CHAR_MAX = 4,

	SRC_CHAR_MAXSIZE = SRC_CHAR_SIZE*SRC_CHAR_MAX,
};

// �P����������̃L�����T�C�Y
enum {
	LETTERSIZE_1x1 = 0,
	LETTERSIZE_1x2,
	LETTERSIZE_2x1,
	LETTERSIZE_2x2,
};

/// �f�t�H���g�J���[�ݒ�
enum {
	COLOR_DEFAULT_LETTER		= 0x01,
	COLOR_DEFAULT_SHADOW		= 0x02,
	COLOR_DEFAULT_BACKGROUND	= 0x00,
};

typedef struct {

	ARCHANDLE*    file_handle;
	u8            read_buffer[SRC_CHAR_MAXSIZE];


}FILEREAD_TYPE_WORK;



/**
 *	CodeMap
 */
typedef struct {
	u16  codeBegin;
	u16  codeEnd;
	u16  mappingMethod;
	u16  reserved;
	u32  nextOfs;
	u16  mapInfo[0];
}NNSCodeMap;

typedef struct {
	u16		indexBegin;
	u16		indexEnd;
	u32		nextOfs;
}NNSFontWidth;

typedef struct {
	u8		cellWidth;
	u8		cellHeight;
	u16		cellSize;
	s8		baselinePos;
	u8		maxCharWidth;
	u8		bpp;
	u8		flags;
}NNSGlyphInfo;

typedef struct {
	u8		fontType;
	u8		linefeed;
	u16		alterCharIndex;

	s8		leftWidth;
	u8		glyphWidth;
	s8		totalWidth;
	u8		encoding;

	u32		ofsGlyph;
	u32		ofsWidth;
	u32		ofsMap;

}NNSFontInfo;

typedef struct {
	const u8*	src;
	u32			rem_bits;
	u8			read_bit;
}BIT_READER;


//------------------------------------------------------------------
/**
 * �}�l�[�W�����[�N
 */
//------------------------------------------------------------------
struct  _GFL_FONT	{
	GFL_FONT_LOADTYPE        loadType;
	pGetBitmapFunc           GetBitmapFunc;

	u32                      charShape;
	u32                      letterCharSize;

	u8*                      fontBitData;
	u8                       readBuffer[SRC_CHAR_MAXSIZE];

	ARCHANDLE*               fileHandle;

	NNSFontInfo              fontHeader;
	BOOL                     fixedFontFlag;

	pWidthGetFunc            WidthGetFunc;

	NNSGlyphInfo			glyphInfo;
	u16						glyph2ndCharBits;
	u16						glyph2ndCharShift;
	u32						fontDataImgOfs;

	NNSFontWidth*			widthTblTop;
	NNSCodeMap*				codeMapTop;
	u32						ofsGlyphTop;
	u8*						glyphBuf;
};

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void load_font_header( GFL_FONT* wk, u32 datID, BOOL fixedFontFlag, u32 heapID );
static void unload_font_header( GFL_FONT* wk );
static void setup_font_datas( GFL_FONT* wk, GFL_FONT_LOADTYPE loadType, u32 heapID );
static void setup_type_on_memory( GFL_FONT* wk, u32 heapID );
static void setup_type_read_file( GFL_FONT* wk, u32 heapID );
static void cleanup_font_datas( GFL_FONT* wk );
static void cleanup_type_on_memory( GFL_FONT* wk );
static void cleanup_type_read_file( GFL_FONT* wk );
static u32 get_glyph_index( const GFL_FONT* wk, u32 code );
static void GetBitmapOnMemory( const GFL_FONT* wk, u32 index, void* dst, u16* sizeX, u16* sizeY );
static void GetBitmapFileRead( const GFL_FONT* wk, u32 index, void* dst, u16* sizeX, u16* sizeY );
static inline void expand_ntr_glyph_block( const u8* srcData, u16 blockPos, u16 cellWidth, BOOL maskFlag, u8* dst );
static u8 GetWidthProportionalFont( const GFL_FONT* wk, u32 index );
static u8 GetWidthFixedFont( const GFL_FONT* wk, u32 index );
static inline void ExpandFontData( const void* src_p, void* dst_p );
static inline void BitReader_Init( BIT_READER* br, const u8* src );
static inline void BitReader_SetNextBit( BIT_READER* br );
static u8 BitReader_Read( BIT_READER* br, u8 bits );
static inline void expand_ntr_glyph_2bit( const u8* glyphSrc, u16 gl2ndCharBits, u16 gl2ndCharShift, u16* dst1st, u16* dst2nd );



//==============================================================================================
/**
 * �t�H���g�f�[�^�}�l�[�W���쐬
 *
 * @param   arcID			�t�H���g�f�[�^���i�[����Ă���A�[�J�C�uID
 * @param   datID			�t�H���g�f�[�^���i�[����Ă���A�[�J�C�u���t�@�C��ID
 * @param   loadType		�t�H���g�f�[�^�̓ǂݏo������
 * @param   fixedFontFlag	TRUE�Ȃ瓙���t�H���g�Ƃ��Ĉ���
 * @param   heapID			�}�l�[�W���쐬��q�[�vID
 *
 * @retval  GFL_FONT*	�t�H���g�f�[�^�}�l�[�W���|�C���^
 *
 * @li  loadType �� FONTDATA_LOADTYPE_ON_MEMORY �̏ꍇ�A�}�l�[�W���Ɠ����q�[�v�̈�Ƀt�H���g�f�[�^��ǂݍ���
 */
//==============================================================================================
GFL_FONT* GFL_FONT_CreateHandle( ARCHANDLE* arcHandle, u32 datID, GFL_FONT_LOADTYPE loadType, BOOL fixedFontFlag, u32 heapID )
{
	GFL_FONT* wk = GFL_HEAP_AllocMemory( heapID, sizeof(GFL_FONT) );
	if( wk )
	{
		wk->fileHandle = arcHandle;
		load_font_header( wk, datID, fixedFontFlag, heapID );
		setup_font_datas( wk, loadType, heapID );
	}
	return wk;
}
//==============================================================================================
/**
 * �t�H���g�f�[�^�}�l�[�W���폜
 *
 * @param   wk		�t�H���g�f�[�^�}�l�[�W���̃|�C���^
 *
 */
//==============================================================================================
void FontDataMan_Delete( GFL_FONT* wk )
{
	cleanup_font_datas( wk );
	unload_font_header( wk );
	GFL_HEAP_FreeMemory( wk );
}
//==============================================================================================
/**
 * �t�H���g�r�b�g�f�[�^�̓ǂݍ��݃^�C�v��ύX����
 *
 * @param   wk			�t�H���g�f�[�^�}�l�[�W���|�C���^
 * @param   loadType	�ύX��̓ǂݍ��݃^�C�v
 *
 */
//==============================================================================================
void FontDataMan_ChangeLoadType( GFL_FONT* wk, GFL_FONT_LOADTYPE loadType, u32 heapID )
{
//	������
//	{
//		cleanup_font_datas( wk );
//		setup_font_datas( wk, loadType, heapID );
//	}
}


//------------------------------------------------------------------
/**
 * ���^�C�v�ŋ��L����w�b�_�f�[�^��ǂݍ��݁E�\�z
 *
 * @param   wk				���[�N�|�C���^
 * @param   datID			�t�H���g�t�@�C���̃f�[�^ID
 * @param   fixedFontFlag	�Œ�t�H���g�t���O
 * @param   heapID			�q�[�vID
 *
 */
//------------------------------------------------------------------
static void load_font_header( GFL_FONT* wk, u32 datID, BOOL fixedFontFlag, u32 heapID )
{
	if( wk->fileHandle )
	{
		GFL_ARC_LoadDataOfsByHandle( wk->fileHandle, datID, 24, sizeof(wk->fontHeader), &(wk->fontHeader) );
		GFL_ARC_LoadDataImgofsByHandle( wk->fileHandle, datID, &wk->fontDataImgOfs );

		wk->fixedFontFlag = fixedFontFlag;
		if( fixedFontFlag )
		{
			wk->widthTblTop = NULL;
			wk->WidthGetFunc = GetWidthFixedFont;
		}
		else
		{
			u32 widthTblSize = wk->fontHeader.ofsMap - wk->fontHeader.ofsWidth;
			wk->widthTblTop = GFL_HEAP_AllocMemory( heapID, widthTblSize );
			wk->WidthGetFunc = GetWidthProportionalFont;

			GFL_ARC_LoadDataOfsByHandle( wk->fileHandle, datID, wk->fontHeader.ofsWidth, 
						widthTblSize, (void*)(wk->widthTblTop) );
		}

		GFL_ARC_LoadDataOfsByHandle( wk->fileHandle, datID, wk->fontHeader.ofsGlyph,
					sizeof(wk->glyphInfo), (void*)(&wk->glyphInfo) );
		wk->glyph2ndCharBits = (8 - (16 - wk->glyphInfo.cellWidth)) * 2;
		wk->glyph2ndCharShift = ((8 - wk->glyph2ndCharBits) / 2) * 4;

//		OS_TPrintf("Gl2ndCharBits=%d, Shift=%d, CellSize=%d\n",
//					wk->glyph2ndCharBits, wk->glyph2ndCharShift, wk->glyphInfo.cellSize);

		wk->ofsGlyphTop = wk->fontHeader.ofsGlyph + sizeof(NNSGlyphInfo);
		wk->glyphBuf = GFL_HEAP_AllocMemory( heapID, wk->glyphInfo.cellSize );

		{
			u32  mapTblSize = GFL_ARC_GetDataSizeByHandle( wk->fileHandle, datID ) - wk->fontHeader.ofsMap;
			wk->codeMapTop = GFL_HEAP_AllocMemory( heapID, mapTblSize );
			GFL_ARC_LoadDataOfsByHandle( wk->fileHandle, datID, wk->fontHeader.ofsMap,
						mapTblSize, (void*)(wk->codeMapTop) );
		}

		#if 0
		{
			static const u8 charShapeTbl[2][2] = {
				{  LETTERSIZE_1x1, LETTERSIZE_1x2 },
				{  LETTERSIZE_2x1, LETTERSIZE_2x2 },
			};

			GF_ASSERT( wk->fontHeader.letterCharX <= 2 && wk->fontHeader.letterCharY <= 2 );

			wk->charShape = charShapeTbl[ wk->fontHeader.letterCharX-1 ][ wk->fontHeader.letterCharY-1 ];
			wk->letterCharSize = SRC_CHAR_SIZE * wk->fontHeader.letterCharX * wk->fontHeader.letterCharY;
		}
		#endif
	}
}
//------------------------------------------------------------------
/**
 * �\�z�����w�b�_�f�[�^�̉��
 *
 * @param   wk		���[�N�|�C���^
 *
 */
//------------------------------------------------------------------
static void unload_font_header( GFL_FONT* wk )
{
	if( wk->glyphBuf )
	{
		GFL_HEAP_FreeMemory( wk->glyphBuf );
	}
	if( wk->widthTblTop )
	{
		GFL_HEAP_FreeMemory( wk->widthTblTop );
	}
	if( wk->codeMapTop )
	{
		GFL_HEAP_FreeMemory( wk->codeMapTop );
	}
	if( wk->fileHandle )
	{
		GFL_ARC_CloseDataHandle( wk->fileHandle );
	}
}



//------------------------------------------------------------------
/**
 * �Ǘ�����t�H���g�f�[�^�̓ǂݍ��ݏ���
 *
 * @param   wk				�}�l�[�W�����[�N�|�C���^
 * @param   loadType		�ǂݍ��݃^�C�v
 * @param   heapID			�풓�^�C�v�̏ꍇ�A�t�H���g�r�b�g�f�[�^�m�ۗp�q�[�v�̎w��B
 *							�t�@�C�����[�h�^�C�v�̏ꍇ�A�g�p���Ȃ�
 *
 */
//------------------------------------------------------------------
static void setup_font_datas( GFL_FONT* wk, GFL_FONT_LOADTYPE loadType, u32 heapID )
{
	static void (* const setup_func[])( GFL_FONT*, u32 ) = {
		setup_type_read_file,
		setup_type_on_memory,
	};

	GF_ASSERT(loadType == GFL_FONT_LOADTYPE_FILE);

	wk->loadType = loadType;

	setup_func[loadType]( wk, heapID );
}
//------------------------------------------------------------------
/**
 * �Ǘ�����t�H���g�f�[�^�̓ǂݍ��ݏ����i�r�b�g�f�[�^�풓�^�C�v�j
 *
 * @param   wk				�}�l�[�W�����[�N�|�C���^
 * @param   heapID			�r�b�g�f�[�^�̈�m�ۗp�̃q�[�vID
 */
//------------------------------------------------------------------
static void setup_type_on_memory( GFL_FONT* wk, u32 heapID )
{
	#if 0
//	void* fontData = ArcUtil_Load( arcID, datID, FALSE, heapID, ALLOC_TOP );
	u32  bit_data_size = wk->letterCharSize * wk->fontHeader.letterMax;

	wk->fontBitData = GFL_HEAP_AllocMemory( heapID, bit_data_size );
	wk->GetBitmapFunc = GetBitmapOnMemory;

	GFL_ARC_LoadDataOfsByHandle( wk->fileHandle, wk->fileDatID, wk->fontHeader.bitDataOffs,
						bit_data_size, wk->fontBitData );
	#else
	// WB�ł̓I�����������������Ȃ��Ǝv���邽�ߖ������B
	// �Ƃ肠�����t�@�C���ǂݍ��݃^�C�v�Ɠ������������Ă���
	wk->GetBitmapFunc = GetBitmapFileRead;
	#endif
}
//------------------------------------------------------------------
/**
 * �Ǘ�����t�H���g�f�[�^�̓ǂݍ��ݏ����i�r�b�g�f�[�^�����ǂݏo���^�C�v�j
 *
 * @param   wk				�}�l�[�W�����[�N�|�C���^
 * @param   heapID			�g�p���Ȃ�
 */
//------------------------------------------------------------------
static void setup_type_read_file( GFL_FONT* wk, u32 heapID )
{
	wk->GetBitmapFunc = GetBitmapFileRead;
}

//------------------------------------------------------------------
/**
 * �ǂݍ��񂾃t�H���g�f�[�^�̔j��
 *
 * @param   wk		�t�H���g�f�[�^�}�l�[�W���|�C���^
 *
 */
//------------------------------------------------------------------
static void cleanup_font_datas( GFL_FONT* wk )
{
	static void (* const cleanup_func[])( GFL_FONT* ) = {
		cleanup_type_on_memory,
		cleanup_type_read_file,
	};

	cleanup_func[ wk->loadType ]( wk );
}
//------------------------------------------------------------------
/**
 * �ǂݍ��񂾃t�H���g�f�[�^�̔j���i�r�b�g�f�[�^�풓�^�C�v�j
 *
 * @param   wk		�t�H���g�f�[�^�}�l�[�W���|�C���^
 *
 */
//------------------------------------------------------------------
static void cleanup_type_on_memory( GFL_FONT* wk )
{
	GFL_HEAP_FreeMemory( wk->fontBitData );
	wk->fontBitData = NULL;
}
//------------------------------------------------------------------
/**
 * �ǂݍ��񂾃t�H���g�f�[�^�̔j���i�r�b�g�f�[�^�����ǂݍ��݃^�C�v�j
 *
 * @param   wk		�t�H���g�f�[�^�}�l�[�W���|�C���^
 *
 */
//------------------------------------------------------------------
static void cleanup_type_read_file( GFL_FONT* wk )
{
	
}



//==============================================================================================
//==============================================================================================


//==============================================================================================
/*
 *	�����r�b�g�}�b�v�f�[�^�擾
 *
 * @param	wk			[in]  �t�H���g�f�[�^�}�l�[�W��
 * @param	code		[in]  �����R�[�h
 * @param	dst			[out] �r�b�g�}�b�v�f�[�^�擾�o�b�t�@
 * @param	sizeX		[out] �������擾�ϐ��|�C���^
 * @param	sizeY		[out] �������擾�ϐ��|�C���^
 *
 */
//==============================================================================================
void GFL_FONT_GetBitMap( const GFL_FONT* wk, u32 code, void* dst, u16* sizeX, u16* sizeY )
{
	u32 index;

	index = get_glyph_index( wk, code );
	wk->GetBitmapFunc( wk, index, dst, sizeX, sizeY );

	#if 0
	{
		Gtick[4] = Gtick[1] - Gtick[0];
		Gtick[5] = OS_TicksToMilliSeconds( Gtick[4] );
		Gtick[6] = Gtick[2] - Gtick[1];
		Gtick[7] = OS_TicksToMilliSeconds( Gtick[6] );

		OS_TPrintf("[TICK] code=%04x, idx=%d, ", code, index);
		OS_TPrintf("GetIndex=%ld", Gtick[4]);
		OS_TPrintf(" (%ld msec)", Gtick[5]);
		OS_TPrintf(" GetBitmap=%ld", Gtick[6]);
		OS_TPrintf(" (%ld msec) ", Gtick[7]);
		OS_TPrintf(" Width=%d\n", *sizeX);
	}
	#endif
}


//--------------------------------------------------------------------------
/**
 * �����R�[�h���O���t�C���f�b�N�X
 *
 * @param   wk			[in] �t�H���g�f�[�^�}�l�[�W��
 * @param   code		[in] �����R�[�h�iUTF-16�j
 *
 * @retval  u32			�O���t�C���f�b�N�X
 */
//--------------------------------------------------------------------------
static u32 get_glyph_index( const GFL_FONT* wk, u32 code )
{
	enum {
		PRINTFLAG = FALSE,
	};

	const NNSCodeMap* pMap = wk->codeMapTop;


	while( 1 )
	{
		if( (pMap->codeBegin <= code) && (pMap->codeEnd >= code) )
		{
			switch( pMap->mappingMethod ){
			case NNS_G2D_MAPMETHOD_DIRECT:
				return ( code - pMap->codeBegin ) + pMap->mapInfo[0];

			case NNS_G2D_MAPMETHOD_TABLE:
			{
				u16 aidx = code - pMap->codeBegin;
				return pMap->mapInfo[aidx];
			}

			case NNS_G2D_MAPMETHOD_SCAN:
			{
				u16 numEntries;
				int min, max, mid, midIdx;

				numEntries = pMap->mapInfo[0];
				min = 1;
				max = numEntries;
//				TAYA_PrintfEx( PRINTFLAG, "GetGlyphIndex scanMethod code=%04x, numEntries=%d\n", code, numEntries );

				while( min <= max )
				{
					mid = (min + (max - min) / 2);
					midIdx = 1 + mid * 2;
//					TAYA_PrintfEx( PRINTFLAG, "  %4d-%4d (mid:%4d, code=%04x)\n",
//							min, max, mid, pMap->mapInfo[midIdx] );

					if( pMap->mapInfo[midIdx] < code )
					{
						min = mid + 1;
						continue;
					}
					if( code < pMap->mapInfo[midIdx] )
					{
						max = mid - 1;
						continue;
					}
					return pMap->mapInfo[midIdx+1];
				}
			}
			break;

			default:
				GF_ASSERT(0);
				break;
			}

			return 461;
		}
		else if( pMap->nextOfs )
		{
			u32 ofs = pMap->nextOfs - wk->fontHeader.ofsMap;
			pMap = (const NNSCodeMap*)((u32)(wk->codeMapTop) + ofs);
		}
		else
		{
			return 0;
		}
	}
}



//------------------------------------------------------------------
/**
 * �����r�b�g�}�b�v�f�[�^�擾�����i�r�b�g�f�[�^�풓�^�C�v�j
 *
 * @param	wk			[in]  �t�H���g�f�[�^�}�l�[�W��
 * @param	fcode		[in]  �����C���f�b�N�X
 * @param	dst			[out] �r�b�g�}�b�v�f�[�^�擾�o�b�t�@
 * @param	sizeX		[out] �������擾�ϐ��|�C���^
 * @param	sizeY		[out] �������擾�ϐ��|�C���^
 *
 */
//------------------------------------------------------------------
static void GetBitmapOnMemory( const GFL_FONT* wk, u32 index, void* dst, u16* sizeX, u16* sizeY )
{
	GF_ASSERT(0);
	#if 0
	u8*	fdata_adrs;

	fdata_adrs = &wk->fontBitData[index * wk->letterCharSize];

	switch( wk->charShape ){
	case LETTERSIZE_1x1:
		ExpandFontData(fdata_adrs + 0x10* 0, (u8*)dst+0x20*0);
		break;
	case LETTERSIZE_1x2:
		ExpandFontData(fdata_adrs + 0x10* 0, (u8*)dst+0x20*0);
		ExpandFontData(fdata_adrs + 0x10* 1, (u8*)dst+0x20*2);
		break;
	case LETTERSIZE_2x1:
		ExpandFontData(fdata_adrs + 0x10* 0, (u8*)dst+0x20*0);
		ExpandFontData(fdata_adrs + 0x10* 1, (u8*)dst+0x20*1);
		break;
	case LETTERSIZE_2x2:
		ExpandFontData(fdata_adrs + 0x10* 0, (u8*)dst+0x20*0);
		ExpandFontData(fdata_adrs + 0x10* 1, (u8*)dst+0x20*1);
		ExpandFontData(fdata_adrs + 0x10* 2, (u8*)dst+0x20*2);
		ExpandFontData(fdata_adrs + 0x10* 3, (u8*)dst+0x20*3);
		break;
	}

	*sizeX = wk->WidthGetFunc( wk, index );
	*sizeY = wk->fontHeader.maxHeight;
	#endif
}

//#define GLOBAL_PRINT_FLAG

#ifdef GLOBAL_PRINT_FLAG
static BOOL PRINTFLG = FALSE;
#endif

//------------------------------------------------------------------
/**
 * �����r�b�g�}�b�v�f�[�^�擾�����i�r�b�g�f�[�^�����ǂݍ��݃^�C�v�j
 *
 * @param   wk			���[�N�|�C���^
 * @param   fcode		�����R�[�h
 * @param   dst			�r�b�g�f�[�^�ǂݍ��ݐ�o�b�t�@
 *
 */
//------------------------------------------------------------------
static void GetBitmapFileRead( const GFL_FONT* wk, u32 index, void* dst, u16* sizeX, u16* sizeY )
{
	u32 ofsTop = wk->ofsGlyphTop + index * wk->glyphInfo.cellSize;

	GFL_ARC_SeekDataByHandle( wk->fileHandle, wk->fontDataImgOfs+ofsTop );
	GFL_ARC_LoadDataByHandleContinue( wk->fileHandle, wk->glyphInfo.cellSize, (void*)(wk->glyphBuf) );
//	GFL_ARC_SeekAndRead( wk->fileHandle, wk->fontDataImgOfs+ofsTop, wk->glyphInfo.cellSize, (void*)(wk->glyphBuf) );

	#ifdef GLOBAL_PRINT_FLAG
	PRINTFLG = ((index==332)||(index==322));
	#endif

	expand_ntr_glyph_2bit( wk->glyphBuf, wk->glyph2ndCharBits, wk->glyph2ndCharShift, (u16*)dst, (u16*)((u8*)dst+0x20) );

	*sizeX = wk->WidthGetFunc( wk, index );
	*sizeY = wk->fontHeader.linefeed;
}

//--------------------------------------------------------------------------
/**
 * 
 *
 * @param   srcData		
 * @param   startBit		
 * @param   dst		
 *
 */
//--------------------------------------------------------------------------
static inline void expand_ntr_glyph_block( const u8* srcData, u16 blockPos, u16 cellWidth, BOOL maskFlag, u8* dst )
{
	#ifndef GLOBAL_PRINT_FLAG
	enum {
		PRINTFLG = FALSE,
	};
	#endif

	static const struct {
		u8  mask1;
		u8  shift1;
		u8  mask2;
		u8  shift2;
	}ptbl[] = {
		{ 0xff, 0, 0x00, 1 },	// 0
		{ 0x7f, 1, 0x80, 0 },	// 1
		{ 0x3f, 2, 0xc0, 0 },	// 2
		{ 0x1f, 3, 0xe0, 0 },	// 3
		{ 0x0f, 4, 0xf0, 0 },	// 4
		{ 0x07, 5, 0xf8, 0 },	// 5
		{ 0x03, 6, 0xfc, 0 },	// 6
		{ 0x01, 7, 0xfe, 0 },	// 7
	};
	u32 byte, bit, i;
	u32 startBit;

	startBit = (cellWidth * (blockPos / 2) * 8) + ((blockPos&1) * 8);


	for(i=0; i<8; i++)
	{
		byte = startBit / 8;
		bit  = startBit & 7;
		dst[i] = srcData[byte] & ptbl[bit].mask1;
		dst[i] <<= ptbl[bit].shift1;
		dst[i] |= ((srcData[byte+1] & ptbl[bit].mask2) >> (8 - ptbl[bit].shift1));
		startBit += cellWidth;
	}

	if( maskFlag )
	{
		u8 mask = 0xff;
		int shift = 8 - (cellWidth & 7);
		mask = (mask >> shift) << shift;
		for(i=0; i<8; i++)
		{
			dst[i] &= mask;
		}
	}

}

//==============================================================================================
/**
 * 
 *
 * @param   wk		
 * @param   index	
 *
 * @retval  u16		
 */
//==============================================================================================
u16 GFL_FONT_GetWidth( const GFL_FONT* wk, u32 code )
{
	u32 index = get_glyph_index( wk, code );
	return  wk->WidthGetFunc( wk, index );
}

//==============================================================================================
/**
 * 
 *
 * @param   wk		
 * @param   index	
 *
 * @retval  u16		
 */
//==============================================================================================
u16 GFL_FONT_GetHeight( const GFL_FONT* wk, u32 index )
{
	return wk->fontHeader.linefeed;
}

//------------------------------------------------------------------
/**
 * �P�������擾�֐��i�v���|�[�V���i���j
 *
 * @param   wk		
 * @param   bcode		
 *
 * @retval  u8		
 */
//------------------------------------------------------------------
static u8 GetWidthProportionalFont( const GFL_FONT* wk, u32 index )
{
	const NNSFontWidth*  wtbl = wk->widthTblTop;
	u8 ret = wk->fontHeader.totalWidth + 1;


	while( 1 )
	{
		if( wtbl->indexBegin <= index && index <= wtbl->indexEnd )
		{
			u8* adrs = (u8*)((u32)wtbl + sizeof(NNSFontWidth) + (index - wtbl->indexBegin) * 3);
			if( adrs[1] )
			{
				ret = adrs[1];
			}
			else
			{
				ret = 8;
			}
			break;
		}
		else if( wtbl->nextOfs )
		{
			wtbl = (const NNSFontWidth*)
					(u32)(wk->widthTblTop) + (u32)(wtbl->nextOfs - (u32)(wk->widthTblTop));
		}
		else
		{
			GF_ASSERT(0);
			break;
		}
	}


	return ret;

}
//------------------------------------------------------------------
/**
 * �P�������擾�֐��i�����j
 *
 * @param   wk		
 * @param   bcode		
 *
 * @retval  u8		
 */
//------------------------------------------------------------------
static u8 GetWidthFixedFont( const GFL_FONT* wk, u32 index )
{
	return wk->fontHeader.totalWidth;
}







//==============================================================================================
// �t�H���g�f�[�^�W�J����
//==============================================================================================

static u16 DotTbl[256];

static struct {
	u8  letter;
	u8  shadow;
	u8  back;
}CurrentColor;


void GFL_FONTSYS_Init( void )
{
	GFL_FONTSYS_SetDefaultColor();
}

void GFL_FONTSYS_SetDefaultColor( void )
{
	GFL_FONTSYS_SetColor( COLOR_DEFAULT_LETTER, COLOR_DEFAULT_SHADOW, COLOR_DEFAULT_BACKGROUND );
}


void GFL_FONTSYS_SetColor( u8 letterColor, u8 shadowColor, u8 backColor )
{
	int d1,d2,d3,d4,n;
	u32 col[4];

	col[0] = 0;
	col[1] = letterColor;
	col[2] = shadowColor;
	col[3] = backColor;

	CurrentColor.letter = letterColor;
	CurrentColor.shadow = shadowColor;
	CurrentColor.back   = backColor;

	n = 0;
	for(d1=0; d1<4; d1++)
	{
		for(d2=0; d2<4; d2++)
		{
			for(d3=0; d3<4; d3++)
			{
				for(d4=0; d4<4; d4++)
				{
					DotTbl[n++] =	(col[d4]<<12) | 
									(col[d3]<<8) |
									(col[d2]<<4) |
									(col[d1]);
				}
			}
		}
	}
}

void GFL_FONTSYS_GetColor( u8* letter, u8* shadow, u8* back )
{
	*letter = CurrentColor.letter;
	*shadow = CurrentColor.shadow;
	*back = CurrentColor.back;
}

static inline void ExpandFontData( const void* src_p, void* dst_p )
{
	const u8* src;
	u32* dst;

	src = src_p;
	dst = dst_p;

	*dst++ = DotTbl[ *src++ ];
	*dst++ = DotTbl[ *src++ ];
	*dst++ = DotTbl[ *src++ ];
	*dst++ = DotTbl[ *src++ ];
	*dst++ = DotTbl[ *src++ ];
	*dst++ = DotTbl[ *src++ ];
	*dst++ = DotTbl[ *src++ ];
	*dst++ = DotTbl[ *src++ ];
}





static inline void BitReader_Init( BIT_READER* br, const u8* src )
{
	br->src = src;
	br->read_bit = *src;
	br->rem_bits = 8;
}

static inline void BitReader_SetNextBit( BIT_READER* br )
{
	br->src++;
	br->read_bit = *(br->src);
	br->rem_bits = 8;
}

static u8 BitReader_Read( BIT_READER* br, u8 bits )
{
	u8 ret;

	if( br->rem_bits >= bits )
	{
		ret = br->read_bit >> (8 - bits);
		br->read_bit <<= bits;
		br->rem_bits -= bits;
		if( br->rem_bits == 0 )
		{
			BitReader_SetNextBit( br );
		}
	}
	else
	{
		u32 bits2nd = bits - br->rem_bits;
		ret = (br->read_bit >> (8 - br->rem_bits)) << bits2nd;
		BitReader_SetNextBit( br );
		ret |= BitReader_Read( br, bits2nd );
	}

	return ret;
}


static BIT_READER BitReader = {0};

static inline void expand_ntr_glyph_2bit( const u8* glyphSrc, u16 gl2ndCharBits, u16 gl2ndCharShift, u16* dst1st, u16* dst2nd )
{
	u8 dots;

	BitReader_Init( &BitReader, glyphSrc );

		dots = BitReader_Read( &BitReader, 8 );
		*dst1st++ = DotTbl[dots];
		dots = BitReader_Read( &BitReader, 8 );
		*dst1st++ = DotTbl[dots];
		dots = BitReader_Read( &BitReader, gl2ndCharBits );
		*dst2nd++ = DotTbl[dots];
		dst2nd++;

		dots = BitReader_Read( &BitReader, 8 );
		*dst1st++ = DotTbl[dots];
		dots = BitReader_Read( &BitReader, 8 );
		*dst1st++ = DotTbl[dots];
		dots = BitReader_Read( &BitReader, gl2ndCharBits );
		*dst2nd++ = DotTbl[dots];
		dst2nd++;

		dots = BitReader_Read( &BitReader, 8 );
		*dst1st++ = DotTbl[dots];
		dots = BitReader_Read( &BitReader, 8 );
		*dst1st++ = DotTbl[dots];
		dots = BitReader_Read( &BitReader, gl2ndCharBits );
		*dst2nd++ = DotTbl[dots];
		dst2nd++;

		dots = BitReader_Read( &BitReader, 8 );
		*dst1st++ = DotTbl[dots];
		dots = BitReader_Read( &BitReader, 8 );
		*dst1st++ = DotTbl[dots];
		dots = BitReader_Read( &BitReader, gl2ndCharBits );
		*dst2nd++ = DotTbl[dots];
		dst2nd++;

		dots = BitReader_Read( &BitReader, 8 );
		*dst1st++ = DotTbl[dots];
		dots = BitReader_Read( &BitReader, 8 );
		*dst1st++ = DotTbl[dots];
		dots = BitReader_Read( &BitReader, gl2ndCharBits );
		*dst2nd++ = DotTbl[dots];
		dst2nd++;

		dots = BitReader_Read( &BitReader, 8 );
		*dst1st++ = DotTbl[dots];
		dots = BitReader_Read( &BitReader, 8 );
		*dst1st++ = DotTbl[dots];
		dots = BitReader_Read( &BitReader, gl2ndCharBits );
		*dst2nd++ = DotTbl[dots];
		dst2nd++;

		dots = BitReader_Read( &BitReader, 8 );
		*dst1st++ = DotTbl[dots];
		dots = BitReader_Read( &BitReader, 8 );
		*dst1st++ = DotTbl[dots];
		dots = BitReader_Read( &BitReader, gl2ndCharBits );
		*dst2nd++ = DotTbl[dots];
		dst2nd++;

		dots = BitReader_Read( &BitReader, 8 );
		*dst1st++ = DotTbl[dots];
		dots = BitReader_Read( &BitReader, 8 );
		*dst1st++ = DotTbl[dots];
		dots = BitReader_Read( &BitReader, gl2ndCharBits );
		*dst2nd++ = DotTbl[dots];
		dst2nd++;

	dst1st += 16;
	dst2nd += 16;

		dots = BitReader_Read( &BitReader, 8 );
		*dst1st++ = DotTbl[dots];
		dots = BitReader_Read( &BitReader, 8 );
		*dst1st++ = DotTbl[dots];
		dots = BitReader_Read( &BitReader, gl2ndCharBits );
		*dst2nd++ = DotTbl[dots] << gl2ndCharShift;
		dst2nd++;

		dots = BitReader_Read( &BitReader, 8 );
		*dst1st++ = DotTbl[dots];
		dots = BitReader_Read( &BitReader, 8 );
		*dst1st++ = DotTbl[dots];
		dots = BitReader_Read( &BitReader, gl2ndCharBits );
		*dst2nd++ = DotTbl[dots] << gl2ndCharShift;
		dst2nd++;

		dots = BitReader_Read( &BitReader, 8 );
		*dst1st++ = DotTbl[dots];
		dots = BitReader_Read( &BitReader, 8 );
		*dst1st++ = DotTbl[dots];
		dots = BitReader_Read( &BitReader, gl2ndCharBits );
		*dst2nd++ = DotTbl[dots] << gl2ndCharShift;
		dst2nd++;

		dots = BitReader_Read( &BitReader, 8 );
		*dst1st++ = DotTbl[dots];
		dots = BitReader_Read( &BitReader, 8 );
		*dst1st++ = DotTbl[dots];
		dots = BitReader_Read( &BitReader, gl2ndCharBits );
		*dst2nd = DotTbl[dots] << gl2ndCharShift;

}


