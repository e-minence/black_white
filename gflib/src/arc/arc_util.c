//============================================================================================
/**
 * @file	arc_util.c
 * @bfief	�A�[�J�C�u�f�[�^��֗��Ɏg�����߂̃��[�e�B���e�B�֐��Q
 * @author	taya
 * @date	05.08.30
 */
//============================================================================================
#include "gflib.h"

#include "arc_tool.h"
#include "arc_util.h"


//==============================================================
// Prototype
//==============================================================

vu32	*chr=NULL;
vu32	*scr=NULL;

//------------------------------------------------------------------
/**
 * BG����ް��� VRAM �]��
 *
 * @param   fileIdx			������̧�ٲ��ޯ��
 * @param   dataIndex		�������ް����ޯ��
 * @param   frm				�]�����ڰ�����
 * @param   offs			�]���̾�āi��גP�ʁj
 * @param	transSize		�]�����黲�ށi�޲ĒP�� ==0�őS�]���j
 * @param   compressedFlag	���k����Ă����ް����H
 * @param   heapID			�ް��ǂݍ��݁E�������؂Ƃ��Ďg��˰��ID
 *
 * @return  �]�������f�[�^�T�C�Y�i�o�C�g�j
 */
//------------------------------------------------------------------
u32 GFL_ARC_UtilBgCharSet(u32 fileIdx, u32 dataIdx, u32 frm, u32 offs, u32 transSize, BOOL compressedFlag, u32 heapID)
{
	void* arcData = GFL_ARC_UtilLoad( fileIdx, dataIdx, compressedFlag, heapID );

	chr=arcData;

	{
		NNSG2dCharacterData* charData;

		if( NNS_G2dGetUnpackedBGCharacterData( arcData, &charData ) )
		{
			if( transSize == 0 )
			{
				transSize = charData->szByte;
			}
			GFL_BG_LoadCharacter(frm, charData->pRawData, transSize, offs);
		}

		GFL_HEAP_FreeMemory( arcData );
	}

    return transSize;
}

//------------------------------------------------------------------
/**
 * BG����ް��� VRAM �]���i�G���A�}�l�[�W�����g�p���ĊJ���Ă���̈�Ɏ����œ]���j
 *
 * @param   fileIdx			������̧�ٲ��ޯ��
 * @param   dataIndex		�������ް����ޯ��
 * @param   frm				�]�����ڰ�����
 * @param	transSize		�]�����黲�ށi�޲ĒP�� ==0�őS�]���j
 * @param   compressedFlag	���k����Ă����ް����H
 * @param   heapID			�ް��ǂݍ��݁E�������؂Ƃ��Ďg��˰��ID
 *
 * @return  �]�������ʒu
 */
//------------------------------------------------------------------
u32 GFL_ARC_UtilBgCharSetAreaMan(u32 fileIdx, u32 dataIdx, u32 frm, u32 transSize, BOOL compressedFlag, u32 heapID)
{
	void* arcData = GFL_ARC_UtilLoad( fileIdx, dataIdx, compressedFlag, heapID );
	u32	offs;

	chr=arcData;

	{
		NNSG2dCharacterData* charData;

		if( NNS_G2dGetUnpackedBGCharacterData( arcData, &charData ) )
		{
			if( transSize == 0 )
			{
				transSize = charData->szByte;
			}
			offs = GFL_BG_LoadCharAreaSet( frm, charData->pRawData, transSize );
		}

		GFL_HEAP_FreeMemory( arcData );
	}

	if(GFL_BG_ScreenColorModeGet( frm ) == GX_BG_COLORMODE_16 ){
		return offs;
	}
	else{
		return offs/2;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * ��ذ��ް��� VRAM �]��
 * �� BGL���� ��ذ��ޯ̧ ���p�ӂ���Ă���΁A��ذ��ޯ̧ �ւ̓]�����s��
 *
 * @param   fileIdx			������̧�ٲ��ޯ��
 * @param   arcIndex		�������ް����ޯ��
 * @param   frm				�]�����ڰ�����
 * @param   offs			�]���̾�āi��גP�ʁj
 * @param	transSize		�]�����黲�ށi�޲ĒP�� ==0�őS�]���j
 * @param   compressedFlag	���k����Ă����ް����H
 * @param   heapID			�ް��ǂݍ��݁E�������؂Ƃ��Ďg��˰��ID
 *
 */
//--------------------------------------------------------------------------------------------
void GFL_ARC_UtilScrnSet(u32 fileIdx, u32 dataIdx, u32 frm, u32 offs, u32 transSize, BOOL compressedFlag, u32 heapID)
{
	void* arcData = GFL_ARC_UtilLoad( fileIdx, dataIdx, compressedFlag, heapID );

	scr=arcData;

	{
		NNSG2dScreenData* scrnData;

		if( NNS_G2dGetUnpackedScreenData( arcData, &scrnData ) )
		{
			if( transSize == 0 )
			{
				transSize = scrnData->szByte;
			}

			if( GFL_BG_ScreenAdrsGet( frm ) != NULL )
			{
				GFL_BG_ScreenBufSet( frm, scrnData->rawData, transSize );
			}
			GFL_BG_ScrAreaSet( frm, offs, transSize );
			GFL_BG_LoadScreen( frm, scrnData->rawData, transSize, offs );
		}
		GFL_HEAP_FreeMemory( arcData );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * ��ذ��ް��� VRAM �]���i�L�����̃I�t�Z�b�g���w��j
 * �� BGL���� ��ذ��ޯ̧ ���p�ӂ���Ă���΁A��ذ��ޯ̧ �ւ̓]�����s��
 *
 * @param   fileIdx			������̧�ٲ��ޯ��
 * @param   arcIndex		�������ް����ޯ��
 * @param   frm				�]�����ڰ�����
 * @param   offs			�]���̾�āi��גP�ʁj
 * @param	transSize		�]�����黲�ށi�޲ĒP�� ==0�őS�]���j
 * @param   compressedFlag	���k����Ă����ް����H
 * @param   heapID			�ް��ǂݍ��݁E�������؂Ƃ��Ďg��˰��ID
 *
 */
//--------------------------------------------------------------------------------------------
void GFL_ARC_UtilScrnSetCharOfs(u32 fileIdx, u32 dataIdx, u32 frm, u32 offs, u32 chr_ofs, u32 transSize, BOOL compressedFlag, u32 heapID)
{
	void* arcData = GFL_ARC_UtilLoad( fileIdx, dataIdx, compressedFlag, heapID );

	scr=arcData;

	{
		NNSG2dScreenData* scrnData;

		if( NNS_G2dGetUnpackedScreenData( arcData, &scrnData ) )
		{
			if( transSize == 0 )
			{
				transSize = scrnData->szByte;
			}

			if( GFL_BG_ScreenAdrsGet( frm ) != NULL )
			{
				GFL_BG_ScreenBufSet( frm, scrnData->rawData, transSize );
			}
			if(chr_ofs){
				if(GFL_BG_ScreenColorModeGet(frm) == GX_BG_COLORMODE_16 ){
					{
						int	i;
						u16	*scr;

						scr=(u16 *)&scrnData->rawData;

						for(i=0;i<transSize/2;i++){
							scr[i]+=chr_ofs;
						}
					}
				}
				else{
					{
						int	i;
						u8	*scr;

						scr=(u8 *)&scrnData->rawData;

						for(i=0;i<transSize;i++){
							scr[i]+=chr_ofs;
						}
					}
				}
			}
			GFL_BG_ScrAreaSet( frm, offs, transSize );
			GFL_BG_LoadScreen( frm, scrnData->rawData, transSize, offs );
		}
		GFL_HEAP_FreeMemory( arcData );
	}
}

//------------------------------------------------------------------
/**
 * ��گ��ް� �� VRAM �]��
 *
 * @param   fileIdx		������̧�ٲ��ޯ��
 * @param   dataIdx		�������ް����ޯ��
 * @param   palType		��گē]��������
 * @param   offs		��گē]����̾��
 * @param   transSize	��گē]�����ށi0 �őS�]���j
 * @param   heapID		�ް��ǂݍ��������؂Ƃ��Ďg��˰��ID
 *
 */
//------------------------------------------------------------------
void GFL_ARC_UtilPalSet( u32 fileIdx, u32 dataIdx, PALTYPE palType, u32 offs, u32 transSize, u32 heapID )
{
	GFL_ARC_UtilPalSetEx( fileIdx, dataIdx, palType, 0, offs, transSize, heapID );
}

//------------------------------------------------------------------
/**
 * ��گ��ް� �� VRAM �]���i�]�����̓ǂݍ��݊J�n�̾�Ďw��Łj
 *
 * @param   fileIdx		������̧�ٲ��ޯ��
 * @param   dataIdx		�������ް����ޯ��
 * @param   palType		��گē]��������
 * @param   srcOfs		��گē]�����ǂݍ��݊J�n�̾��
 * @param   dstOfs		��گē]����̾��
 * @param   transSize	��گē]�����ށi0 �őS�]���j
 * @param   heapID		�ް��ǂݍ��������؂Ƃ��Ďg��˰��ID
 *
 */
//------------------------------------------------------------------
void GFL_ARC_UtilPalSetEx( u32 fileIdx, u32 dataIdx, PALTYPE palType, u32 srcOfs, u32 dstOfs, u32 transSize, u32 heapID )
{
		static void (* const load_func[])(const void*, u32, u32) = {
		GX_LoadBGPltt,
		GX_LoadOBJPltt,
		GX_LoadBGExtPltt,
		GX_LoadOBJExtPltt,
		GXS_LoadBGPltt,
		GXS_LoadOBJPltt,
		GXS_LoadBGExtPltt,
		GXS_LoadOBJExtPltt,
	};

	void* arcData = GFL_ARC_UtilLoad( fileIdx, dataIdx, FALSE, heapID );

	if( arcData != NULL )
	{
		NNSG2dPaletteData* palData;

		if( NNS_G2dGetUnpackedPaletteData( arcData, &palData ) )
		{
			(u8*)(palData->pRawData) += srcOfs;
			if( transSize == 0 )
			{
				transSize = palData->szByte - srcOfs;
			}
			DC_FlushRange( palData->pRawData, transSize );

			switch( palType ){
			case PALTYPE_MAIN_BG_EX:
				GX_BeginLoadBGExtPltt();
				load_func[ palType ]( palData->pRawData, dstOfs, transSize );
				GX_EndLoadBGExtPltt();
				break;

			case PALTYPE_SUB_BG_EX:
				GXS_BeginLoadBGExtPltt();
				load_func[ palType ]( palData->pRawData, dstOfs, transSize );
				GXS_EndLoadBGExtPltt();
				break;

			case PALTYPE_MAIN_OBJ_EX:
				GX_BeginLoadOBJExtPltt();
				load_func[ palType ]( palData->pRawData, dstOfs, transSize );
				GX_EndLoadOBJExtPltt();
				break;

			case PALTYPE_SUB_OBJ_EX:
				GXS_BeginLoadOBJExtPltt();
				load_func[ palType ]( palData->pRawData, dstOfs, transSize );
				GXS_EndLoadOBJExtPltt();
				break;

			default:
				load_func[ palType ]( palData->pRawData, dstOfs, transSize );
				break;
			}
		}

		GFL_HEAP_FreeMemory( arcData );
	}
}

//------------------------------------------------------------------
/**
 * OBJ ����ް� �� VRAM �]��
 *
 * @param   fileIdx				������̧�ٲ��ޯ��
 * @param   dataIdx				�������ް����ޯ��
 * @param   objType				OBJ����(OBJTYPE_MAIN or OBJTYPE_SUB�j
 * @param   offs				�̾�āi�޲ĒP�ʁj
 * @param   transSize			�]�����ށi�޲ĒP�� : 0 �őS�]���j
 * @param   compressedFlag		���k���ꂽ�ް����H
 * @param   heapID				�ǂݍ��݁E�������؂Ƃ��Ďg��˰��ID
 *
 * @return  �]�������f�[�^�T�C�Y�i�o�C�g�j
 */
//------------------------------------------------------------------
u32 GFL_ARC_UtilObjCharSet( u32 fileIdx, u32 dataIdx, OBJTYPE objType, u32 offs, u32 transSize, BOOL compressedFlag, u32 heapID )
{
	static void (* const load_func[])(const void*, u32, u32) = {
		GX_LoadOBJ,
		GXS_LoadOBJ,
	};

	void* arcData = GFL_ARC_UtilLoad( fileIdx, dataIdx, compressedFlag, heapID );

	if( arcData != NULL )
	{
		NNSG2dCharacterData* charData;

		if( NNS_G2dGetUnpackedCharacterData( arcData, &charData ) )
		{
			if( transSize == 0 )
			{
				transSize = charData->szByte;
			}

			DC_FlushRange( charData->pRawData, transSize );
			load_func[ objType ]( charData->pRawData, offs, transSize );
		}
		GFL_HEAP_FreeMemory( arcData );
	}
	return transSize;
}

//------------------------------------------------------------------
/**
 * ��گ��ް� �� VRAM �]���� NITRO System ��گ���۷� ���쐬
 *�i3D, OBJ �p�ɂ̂ݑΉ��BBG �ɂ͎g���܂���j
 *
 * @param   fileIdx		[in] ������̧�ٲ��ޯ��
 * @param   dataIdx		[in] �������ް����ޯ��
 * @param   type		[in] �]��������
 * @param   offs		[in] �]���̾��
 * @param   heapID		[in] ˰��ID
 * @param   proxy		[out]�쐬������۷����ڽ
 *
 *	[ type ]
 *		NNS_G2D_VRAM_TYPE_3DMAIN = 0,
 *		NNS_G2D_VRAM_TYPE_2DMAIN = 1,
 *		NNS_G2D_VRAM_TYPE_2DSUB  = 2,
 *
 */
//------------------------------------------------------------------
void GFL_ARC_UtilPalSysLoad( u32 fileIdx, u32 dataIdx, NNS_G2D_VRAM_TYPE type, u32 offs, u32 heapID, NNSG2dImagePaletteProxy* proxy )
{
	void* arcData = GFL_ARC_UtilLoad( fileIdx, dataIdx, FALSE, heapID );

	if( arcData != NULL )
	{
		NNSG2dPaletteData*  palData;
		NNSG2dPaletteCompressInfo*  cmpData;
		BOOL  cmpFlag;

		cmpFlag = NNS_G2dGetUnpackedPaletteCompressInfo( arcData, &cmpData );

		if( NNS_G2dGetUnpackedPaletteData( arcData, &palData ) )
		{
			if( cmpFlag )
			{
				NNS_G2dLoadPaletteEx( palData, cmpData, offs, type, proxy );
			}
			else
			{
				NNS_G2dLoadPalette( palData, offs, type, proxy );
			}
		}

		GFL_HEAP_FreeMemory( arcData );
	}
}
//------------------------------------------------------------------
/**
 * ����ް� �� VRAM �]���� NITRO System �Ұ����۷� ���쐬
 *�i3D, OBJ �p�ɂ̂ݑΉ��BBG �ɂ͎g���܂���j
 *
 *
 * @param   fileIdx			[in] ������̧�ٲ��ޯ��
 * @param   dataIdx			[in] �������ް����ޯ��
 * @param   compressedFlag	[in] ���k����Ă��邩
 * @param   mapType			[in] ϯ��ݸ�����
 * @param   transSize		[in] �]���T�C�Y�B�O�Ȃ�S�]���B
 * @param   vramType		[in] �]��������
 * @param   offs			[in] �]���̾��
 * @param   heapID			[in] �����؂Ɏg��˰��ID
 * @param   proxy			[out] �쐬������۷����ڽ
 *
 *
 *	[ mapType ]
 *		CHAR_MAP_1D = 0,
 *		CHAR_MAP_2D = 1,
 *
 *	�� VRAM�]���^�͕ʊ֐� GFL_ARC_UtilTranCharSysLoad ���g��
 *
 *	[ vramType ]
 *		NNS_G2D_VRAM_TYPE_3DMAIN = 0,
 *		NNS_G2D_VRAM_TYPE_2DMAIN = 1,
 *		NNS_G2D_VRAM_TYPE_2DSUB  = 2,
 *
 */
//------------------------------------------------------------------
u32 GFL_ARC_UtilCharSysLoad( u32 fileIdx, u32 dataIdx, BOOL compressedFlag, CHAR_MAPPING_TYPE mapType, u32 transSize,
	NNS_G2D_VRAM_TYPE vramType, u32 offs, u32 heapID, NNSG2dImageProxy* proxy )
{
	static void (* const load_func[])(const NNSG2dCharacterData*, u32, NNS_G2D_VRAM_TYPE, NNSG2dImageProxy*) = {
		NNS_G2dLoadImage1DMapping,
		NNS_G2dLoadImage2DMapping,
	};

	void* arcData = GFL_ARC_UtilLoad( fileIdx, dataIdx, compressedFlag, heapID );
	u32  transed_size = 0;

	if( arcData != NULL )
	{
		NNSG2dCharacterData* charData;

		if( NNS_G2dGetUnpackedCharacterData( arcData, &charData ) )
		{
			if( transSize )
			{
				charData->szByte = transSize;
			}
			load_func[ mapType ]( charData, offs, vramType, proxy );
			transed_size = charData->szByte;
		}
		GFL_HEAP_FreeMemory( arcData );
	}

	return transed_size;
}
//------------------------------------------------------------------
/**
 * ����ް� �� VRAM �]���� NITRO System �Ұ����۷� ���쐬
 *�i3D, OBJ �p�ɂ̂ݑΉ��BBG �ɂ͎g���܂���j
 *
 * ����ް���ϯ��ݸ�Ӱ�ޒl���A���݂�ڼ޽��ݒ�ɍ��킹�ď��������܂�
 *
 * @param   fileIdx			[in] ������̧�ٲ��ޯ��
 * @param   dataIdx			[in] �������ް����ޯ��
 * @param   compressedFlag	[in] ���k����Ă��邩
 * @param   mapType			[in] ϯ��ݸ�����
 * @param   transSize		[in] �]���T�C�Y�B�O�Ȃ�S�]���B
 * @param   vramType		[in] �]��������
 * @param   offs			[in] �]���̾��
 * @param   heapID			[in] �����؂Ɏg��˰��ID
 * @param   proxy			[out] �쐬������۷����ڽ
 *
 *
 *	[ mapType ]
 *		CHAR_MAP_1D = 0,
 *		CHAR_MAP_2D = 1,
 *
 *	�� VRAM�]���^�͕ʊ֐� GFL_ARC_UtilTranCharSysLoad ���g��
 *
 *	[ vramType ]
 *		NNS_G2D_VRAM_TYPE_3DMAIN = 0,
 *		NNS_G2D_VRAM_TYPE_2DMAIN = 1,
 *		NNS_G2D_VRAM_TYPE_2DSUB  = 2,
 *
 */
//------------------------------------------------------------------
void GFL_ARC_UtilCharSysLoadSyncroMappingMode( u32 fileIdx, u32 dataIdx, BOOL compressedFlag, CHAR_MAPPING_TYPE mapType, u32 transSize,
	NNS_G2D_VRAM_TYPE vramType, u32 offs, u32 heapID, NNSG2dImageProxy* proxy )
{
	static void (* const load_func[])(const NNSG2dCharacterData*, u32, NNS_G2D_VRAM_TYPE, NNSG2dImageProxy*) = {
		NNS_G2dLoadImage1DMapping,
		NNS_G2dLoadImage2DMapping,
	};

	void* arcData = GFL_ARC_UtilLoad( fileIdx, dataIdx, compressedFlag, heapID );

	if( arcData != NULL )
	{
		NNSG2dCharacterData* charData;

		if( NNS_G2dGetUnpackedCharacterData( arcData, &charData ) )
		{
			if( transSize )
			{
				charData->szByte = transSize;
			}

			switch( vramType ){
			case NNS_G2D_VRAM_TYPE_2DMAIN:
				charData->mapingType = GX_GetOBJVRamModeChar();
				break;
			case NNS_G2D_VRAM_TYPE_2DSUB:
				charData->mapingType = GXS_GetOBJVRamModeChar();
				break;
			}
			load_func[ mapType ]( charData, offs, vramType, proxy );
		}
		GFL_HEAP_FreeMemory( arcData );
	}
}

//------------------------------------------------------------------
/**
 * ����ް� �� VRAM �]���� NITRO System �Ұ����۷� ���쐬�BVRAM�]���^�̉摜�f�ޗp�B
 *
 * �� ���̊֐����g���Ă��AVRAM�ɉ摜�͓]������܂���
 *    ���̊֐���۰�ނ����ް��͉������܂���B�߂�l��const void*���Ǘ����āA
 *    �s�v�ɂȂ��������������s���Ă��������B
 *
 * @param   fileIdx			[in] ������̧�ٲ��ޯ��
 * @param   dataIdx			[in] �������ް����ޯ��
 * @param   compressedFlag	[in] ���k����Ă��邩
 * @param   vramType		[in] �]��������
 * @param   offs			[in] �]���̾��
 * @param   heapID			[in] �����؂Ɏg��˰��ID
 * @param   proxy			[out] �쐬������۷��̱��ڽ
 * @param   charData		[out] ����ް����ڽ��ێ������߲���̱��ڽ
 *
 * @retval  const void*		۰�ނ����ް��̱��ڽ
 *
 *	[ vramType ]
 *		NNS_G2D_VRAM_TYPE_3DMAIN = 0,
 *		NNS_G2D_VRAM_TYPE_2DMAIN = 1,
 *		NNS_G2D_VRAM_TYPE_2DSUB  = 2,
 *
 */
//------------------------------------------------------------------
const void* GFL_ARC_UtilTransTypeCharSysLoad( u32 fileIdx, u32 dataIdx, BOOL compressedFlag, 
	NNS_G2D_VRAM_TYPE vramType, u32 offs, u32 heapID, NNSG2dImageProxy* proxy, NNSG2dCharacterData** charData )
{
	void* arcData = GFL_ARC_UtilLoad( fileIdx, dataIdx, compressedFlag, heapID );

	if( arcData != NULL )
	{
		if( NNS_G2dGetUnpackedCharacterData( arcData, charData ) )
		{
			NNS_G2dLoadImageVramTransfer( *charData, offs, vramType, proxy );
		}
	}
	return arcData;
}



//------------------------------------------------------------------
/**
 * ����ް��� ۰�ނ��� Unpack ���邾���ł��B����͊e���ŁB
 *
 * @param   fileIdx				[in] ������̧�ٲ��ޯ��
 * @param   dataIdx				[in] �ް����ޯ��
 * @param   compressedFlag		[in] ���k����Ă��邩
 * @param   charData			[out] ����ް����ڽ��ێ������߲���̱��ڽ
 * @param   heapID				[in] ˰��ID
 *
 * @retval  void*		۰�ނ����ް��̐擪�߲��
 */
//------------------------------------------------------------------
void* GFL_ARC_UtilCharDataGet( u32 fileIdx, u32 dataIdx, BOOL compressedFlag, NNSG2dCharacterData** charData, u32 heapID )
{
	void* arcData = GFL_ARC_UtilLoad( fileIdx, dataIdx, compressedFlag, heapID );

	if( arcData != NULL )
	{
		if( NNS_G2dGetUnpackedBGCharacterData( arcData, charData ) == FALSE)
		{
			// ���s������NULL
			GFL_HEAP_FreeMemory( arcData );
			return NULL;
		}
	}
	return arcData;
}
//--------------------------------------------------------------------------------------------
/**
 * ��ذ��ް��� ۰�ނ��� Unpack ���邾���ł��B����͊e���ŁB
 *
 * @param   fileIdx			[in] ������̧�ٲ��ޯ��
 * @param   arcIndex		[in] �������ް����ޯ��
 * @param   compressedFlag	[in] ���k����Ă����ް����H
 * @param   scrnData		[out] ��ذ��ް����ڽ��ێ������߲���̱��ڽ
 * @param   heapID			[in] ˰��ID
 *
 * @retval  void*		۰�ނ����ް��̐擪�߲��
 */
//--------------------------------------------------------------------------------------------
void* GFL_ARC_UtilScrnDataGet(u32 fileIdx, u32 dataIdx, BOOL compressedFlag, NNSG2dScreenData** scrnData, u32 heapID)
{
	void* arcData = GFL_ARC_UtilLoad( fileIdx, dataIdx, compressedFlag, heapID );

	if( arcData != NULL )
	{
		if( NNS_G2dGetUnpackedScreenData( arcData, scrnData ) == FALSE )
		{
			GFL_HEAP_FreeMemory( arcData );
			return NULL;
		}
	}
	return arcData;
}
//------------------------------------------------------------------
/**
 * ��گ��ް���۰�ނ��� Unpack ���邾���ł��B����͊e���ŁB
 *
 * @param   fileIdx			������̧�ٲ��ޯ��
 * @param   arcIndex		�������ް����ޯ��
 * @param   palData			��گ��ް����ڽ��ێ������߲���̱��ڽ
 * @param   heapID			˰��ID
 *
 * @retval  void*		۰�ނ����ް��̐擪�߲��
 */
//------------------------------------------------------------------
void* GFL_ARC_UtilPalDataGet( u32 fileIdx, u32 dataIdx, NNSG2dPaletteData** palData, u32 heapID )
{
	void* arcData = GFL_ARC_UtilLoad( fileIdx, dataIdx, FALSE, heapID );

	if( arcData != NULL )
	{
		if( NNS_G2dGetUnpackedPaletteData( arcData, palData ) == FALSE )
		{
			GFL_HEAP_FreeMemory( arcData );
			return NULL;
		}
	}
	return arcData;
}
//--------------------------------------------------------------------------------------------
/**
 * ����ݸ�ް��� ۰�ނ��� Unpack ���邾���ł��B����͊e���ŁB
 *
 * @param   fileIdx			������̧�ٲ��ޯ��
 * @param   arcIndex		�������ް����ޯ��
 * @param   compressedFlag	���k����Ă����ް����H
 * @param   cellBank		����ݸ�ް����ڽ��ێ������߲���̱��ڽ
 * @param   heapID			˰��ID
 * @param   pSize			����ݸ���޲Ļ��ނ��󂯎��ϐ��߲���i�v��Ȃ����NULL�Łj
 *
 * @retval  void*		۰�ނ����ް��̐擪�߲��
 */
//--------------------------------------------------------------------------------------------
void* GFL_ARC_UtilCellBankDataGet(u32 fileIdx, u32 dataIdx, BOOL compressedFlag, NNSG2dCellDataBank** cellBank, u32 heapID )
{
	void* arcData;

	arcData = GFL_ARC_UtilLoad( fileIdx, dataIdx, compressedFlag, heapID );

	if( arcData != NULL )
	{
		if( NNS_G2dGetUnpackedCellBank( arcData, cellBank ) == FALSE )
		{
			GFL_HEAP_FreeMemory( arcData );
			return NULL;
		}
	}
	return arcData;
}
//--------------------------------------------------------------------------------------------
/**
 * �����ݸ�ް��� ۰�ނ��� Unpack ���邾���ł��B����͊e���ŁB
 *
 * @param   fileIdx			������̧�ٲ��ޯ��
 * @param   arcIndex		�������ް����ޯ��
 * @param   compressedFlag	���k����Ă����ް����H
 * @param   anmBank			�����ݸ�ް����ڽ��ێ������߲���̱��ڽ
 * @param   heapID			˰��ID
 *
 * @retval  void*		۰�ނ����ް��̐擪�߲��
 */
//--------------------------------------------------------------------------------------------
void* GFL_ARC_UtilAnimBankDataGet(u32 fileIdx, u32 dataIdx, BOOL compressedFlag, NNSG2dAnimBankData** anmBank, u32 heapID)
{
	void* arcData = GFL_ARC_UtilLoad( fileIdx, dataIdx, compressedFlag, heapID );

	if( arcData != NULL )
	{
		if( NNS_G2dGetUnpackedAnimBank( arcData, anmBank ) == FALSE )
		{
			GFL_HEAP_FreeMemory( arcData );
			return NULL;
		}
	}
	return arcData;
}











//------------------------------------------------------------------
/**
 * LZ���k��A�[�J�C�u����Ă���f�[�^��ǂݏo���A�𓀂��ĕԂ�
 *
 * @param   fileIdx		�A�[�J�C�u�t�@�C���C���f�b�N�X
 * @param   dataIdx		�A�[�J�C�u�f�[�^�C���f�b�N�X
 * @param   heapID		�ǂݏo���E�𓀂Ɏg���q�[�v�h�c
 *
 * @retval  void*		�𓀌�̃f�[�^�ۑ���A�h���X
 */
//------------------------------------------------------------------
void* GFL_ARC_UtilUnCompress(u32 fileIdx, u32 dataIdx, u32 heapID)
{
	return GFL_ARC_UtilLoad(fileIdx, dataIdx, TRUE, heapID );
}

//------------------------------------------------------------------
/**
 * �A�[�J�C�u�f�[�^�̓ǂݏo���i�������m�ۂ���j
 *
 * @param   fileIdx			�A�[�J�C�u�t�@�C���C���f�b�N�X
 * @param   dataIdx			�A�[�J�C�u�f�[�^�C���f�b�N�X
 * @param   compressedFlag	���k����Ă��邩�H
 * @param   heapID			�������m�ۂɎg���q�[�v�h�c
 *
 * @retval  void*			�ǂݏo���̈�|�C���^
 */
//------------------------------------------------------------------
void* GFL_ARC_UtilLoad(u32 fileIdx, u32 dataIdx, BOOL compressedFlag, u32 heapID )
{
	void* arcData;

	if( compressedFlag )
	{
		arcData = GFL_HEAP_AllocMemoryLow( heapID, GFL_ARC_DataSizeGet(fileIdx, dataIdx) );
	}
	else
	{
		arcData = GFL_HEAP_AllocMemory( heapID, GFL_ARC_DataSizeGet(fileIdx, dataIdx) );
	}

	GFL_ARC_DataLoad(arcData, fileIdx, dataIdx);
	if( compressedFlag )
	{
		void* data;

		data = GFL_HEAP_AllocMemory( heapID, MI_GetUncompressedSize( arcData ) );

		MI_UncompressLZ8( arcData, data );
		GFL_HEAP_FreeMemory( arcData );

		arcData = data;
	}
	return arcData;
}
//------------------------------------------------------------------
/**
 * �A�[�J�C�u�f�[�^�̓ǂݏo���i�ǂݏo�����T�C�Y���擾�ł���j
 *
 * @param   fileIdx			�A�[�J�C�u�t�@�C���C���f�b�N�X
 * @param   dataIdx			�A�[�J�C�u�f�[�^�C���f�b�N�X
 * @param   compressedFlag	���k����Ă��邩�H
 * @param   heapID			�������m�ۂɎg���q�[�v�h�c
 * @param   pSize			���f�[�^�̃o�C�g�T�C�Y�i���k����Ă���ꍇ�͓W�J��j
 *
 * @retval  void*			�ǂݏo���̈�|�C���^
 */
//------------------------------------------------------------------
void* GFL_ARC_UtilLoadEx(u32 fileIdx, u32 dataIdx, BOOL compressedFlag, u32 heapID, u32* pSize)
{
	void* arcData;

	*pSize = GFL_ARC_DataSizeGet(fileIdx, dataIdx);

	if( compressedFlag )
	{
		arcData = GFL_HEAP_AllocMemoryLow( heapID, *pSize );
	}
	else
	{
		arcData = GFL_HEAP_AllocMemory( heapID, *pSize );
	}

	GFL_ARC_DataLoad(arcData, fileIdx, dataIdx);
	if( compressedFlag )
	{
		void* data;

		*pSize = MI_GetUncompressedSize( arcData );

		data = GFL_HEAP_AllocMemory( heapID, *pSize );

		MI_UncompressLZ8( arcData, data );
		GFL_HEAP_FreeMemory( arcData );

		arcData = data;
	}
	return arcData;
}
