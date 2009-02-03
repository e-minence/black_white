//============================================================================================
/**
 * @file	arc_util.h
 * @bfief	�A�[�J�C�u�f�[�^��֗��Ɏg�����߂̃��[�e�B���e�B�֐��Q
 * @author	taya
 * @date	05.08.30
 */
//============================================================================================
#ifndef __ARC_UTIL_H__
#define __ARC_UTIL_H__

#include "bg_sys.h"
#include "arc_tool.h"

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------
/**
 *  �p���b�g�]����^�C�v�w��
 */
//------------------------------------------------------------------
typedef enum {
	PALTYPE_MAIN_BG,			// ���C��BG
	PALTYPE_MAIN_OBJ,			// ���C��OBJ
	PALTYPE_MAIN_BG_EX,			// ���C���g��BG
	PALTYPE_MAIN_OBJ_EX,		// ���C���g��OBJ
	PALTYPE_SUB_BG,				// �T�uBG
	PALTYPE_SUB_OBJ,			// �T�uOBJ
	PALTYPE_SUB_BG_EX,			// �T�u�g��BG
	PALTYPE_SUB_OBJ_EX,			// �T�u�g��OBJ
}PALTYPE;

//------------------------------------------------------------------
/**
 *  OBJ�L�����f�[�^�]����^�C�v�w��
 */
//------------------------------------------------------------------
typedef enum {
	OBJTYPE_MAIN,
	OBJTYPE_SUB,
}OBJTYPE;

//------------------------------------------------------------------
/**
 *  �L�����f�[�^�}�b�s���O�^�C�v
 */
//------------------------------------------------------------------
typedef enum {
	CHAR_MAP_1D,		// 1D 
	CHAR_MAP_2D,		// 2D
}CHAR_MAPPING_TYPE;

//------------------------------------------------------------------
/**
 * BG����ް��� VRAM �]��
 *
 * @param   arcID			�A�[�J�C�u�t�@�C���C���f�b�N�X
 * @param   datID			�A�[�J�C�u�f�[�^�C���f�b�N�X
 * @param   frm				�]�����ڰ�����
 * @param   offs			�]���̾�āi��גP�ʁj
 * @param	transSize		�]�����黲�ށi�޲ĒP�� ==0�őS�]���j
 * @param   compressedFlag	���k����Ă����ް����H
 * @param   heapID			�ް��ǂݍ��݁E�������؂Ƃ��Ďg��˰��ID
 *
 * @return  �]�������f�[�^�T�C�Y�i�o�C�g�j
 */
//------------------------------------------------------------------
extern u32 GFL_ARC_UTIL_TransVramBgCharacter(ARCID arcID, ARCDATID datID, u32 frm, u32 offs, u32 transSize, BOOL compressedFlag, HEAPID heapID);
extern u32 GFL_ARCHDL_UTIL_TransVramBgCharacter(ARCHANDLE *handle, ARCDATID datID, u32 frm, u32 offs, u32 transSize, BOOL compressedFlag, HEAPID heapID);

//------------------------------------------------------------------
/**
 * BG����ް��� VRAM �]���i�G���A�}�l�[�W�����g�p���ĊJ���Ă���̈�Ɏ����œ]���j
 *
 * @param   arcID			������̧�ٲ��ޯ��
 * @param   datID			�������ް����ޯ��
 * @param   frm				�]�����ڰ�����
 * @param	transSize		�]�����黲�ށi�޲ĒP�� ==0�őS�]���j
 * @param   compressedFlag	���k����Ă����ް����H
 * @param   heapID			�ް��ǂݍ��݁E�������؂Ƃ��Ďg��˰��ID
 *
 * @return  �]�������ʒu
 */
//------------------------------------------------------------------
extern	u32 GFL_ARC_UTIL_TransVramBgCharacterAreaMan(ARCID arcID, ARCDATID datID, u32 frm, u32 transSize, BOOL compressedFlag, HEAPID heapID);
extern u32 GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan(ARCHANDLE *handle, ARCDATID datID, u32 frm, u32 transSize, BOOL compressedFlag, HEAPID heapID);

//------------------------------------------------------------------
/**
 * OBJ ����ް� �� VRAM �]��
 *
 * @param   arcID				�A�[�J�C�u�t�@�C���C���f�b�N�X
 * @param   datID				�A�[�J�C�u�f�[�^�C���f�b�N�X
 * @param   objType				OBJ����
 * @param   offs				�̾�āi�޲ĒP�ʁj
 * @param   transSize			�]�����ށi�޲ĒP�� : 0 �őS�]���j
 * @param   compressedFlag		���k���ꂽ�ް����H
 * @param   heapID				�ǂݍ��݁E�������؂Ƃ��Ďg��˰��ID
 *
 * @return  �]�������f�[�^�T�C�Y�i�o�C�g�j
 */
//------------------------------------------------------------------
extern u32 GFL_ARC_UTIL_TransVramObjCharacter( ARCID arcID, ARCDATID datID, OBJTYPE objType, u32 offs, u32 transSize, BOOL compressedFlag, HEAPID heapID );
extern u32 GFL_ARCHDL_UTIL_TransVramObjCharacter( ARCHANDLE *handle, ARCDATID datID, OBJTYPE objType, u32 offs, u32 transSize, BOOL compressedFlag, HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * ��ذ��ް��� VRAM �]��
 * �� BGL���� ��ذ��ޯ̧ ���p�ӂ���Ă���΁A��ذ��ޯ̧ �ւ̓]�����s��
 *
 * @param   arcID			�A�[�J�C�u�t�@�C���C���f�b�N�X
 * @param   datID			�A�[�J�C�u�f�[�^�C���f�b�N�X
 * @param   frm				�]�����ڰ�����
 * @param   offs			�]���̾�āi��גP�ʁj
 * @param	transSize		�]�����黲�ށi�޲ĒP�� ==0�őS�]���j
 * @param   compressedFlag	���k����Ă����ް����H
 * @param   heapID			�ް��ǂݍ��݁E�������؂Ƃ��Ďg��˰��ID
 *
 */
//--------------------------------------------------------------------------------------------
extern void GFL_ARC_UTIL_TransVramScreen(u32 arcFile, u32 dataIdx, u32 frm, u32 offs, u32 transSize, BOOL compressedFlag, HEAPID heapID);
extern void GFL_ARCHDL_UTIL_TransVramScreen(ARCHANDLE *handle, ARCDATID datID, u32 frm, u32 offs, u32 transSize, BOOL compressedFlag, HEAPID heapID);

//--------------------------------------------------------------------------------------------
/**
 * ��ذ��ް��� VRAM �]���i�L�����̃I�t�Z�b�g���w��j
 * �� BGL���� ��ذ��ޯ̧ ���p�ӂ���Ă���΁A��ذ��ޯ̧ �ւ̓]�����s��
 *
 * @param   arcID			������̧�ٲ��ޯ��
 * @param   datID			�������ް����ޯ��
 * @param   frm				�]�����ڰ�����
 * @param   offs			�]���̾�āi��גP�ʁj
 * @param   chr_ofs			�X�N���[���f�[�^�̃L����No�ւ̵̾��
 * @param	transSize		�]�����黲�ށi�޲ĒP�� ==0�őS�]���j
 * @param   compressedFlag	���k����Ă����ް����H
 * @param   heapID			�ް��ǂݍ��݁E�������؂Ƃ��Ďg��˰��ID
 *
 */
//--------------------------------------------------------------------------------------------
extern	void GFL_ARC_UTIL_TransVramScreenCharOfs(ARCID arcID, ARCDATID datID, u32 frm, u32 offs, u32 chr_ofs, u32 transSize, BOOL compressedFlag, HEAPID heapID);
extern void GFL_ARCHDL_UTIL_TransVramScreenCharOfs(ARCHANDLE *handle, ARCDATID datID, u32 frm, u32 offs, u32 chr_ofs, u32 transSize, BOOL compressedFlag, HEAPID heapID);

//------------------------------------------------------------------
/**
 * ��گ��ް� �� VRAM �]��
 *
 * @param   arcID		�A�[�J�C�u�t�@�C���C���f�b�N�X
 * @param   datID		�A�[�J�C�u�f�[�^�C���f�b�N�X
 * @param   palType		��گē]��������
 * @param   offs		��گē]����̾��
 * @param   transSize	��گē]�����ށi0 �őS�]���j
 * @param   heapID		�ް��ǂݍ��������؂Ƃ��Ďg��˰��ID
 *
 */
//------------------------------------------------------------------
extern void GFL_ARC_UTIL_TransVramPalette( ARCID arcID, ARCDATID datID, PALTYPE palType, u32 offs, u32 transSize, HEAPID heapID );
extern void GFL_ARCHDL_UTIL_TransVramPalette( ARCHANDLE *handle, ARCDATID datID, PALTYPE palType, u32 offs, u32 transSize, HEAPID heapID );


//------------------------------------------------------------------
/**
 * ��گ��ް� �� VRAM �]���i�]�����̓ǂݍ��݊J�n�̾�Ďw��Łj
 *
 * @param   arcID		������̧�ٲ��ޯ��
 * @param   datID		�������ް����ޯ��
 * @param   palType		��گē]��������
 * @param   srcOfs		��گē]�����ǂݍ��݊J�n�̾��
 * @param   dstOfs		��گē]����̾��
 * @param   transSize	��گē]�����ށi0 �őS�]���j
 * @param   heapID		�ް��ǂݍ��������؂Ƃ��Ďg��˰��ID
 *
 */
//------------------------------------------------------------------
extern void GFL_ARC_UTIL_TransVramPaletteEx( ARCID arcID, ARCDATID datID, PALTYPE palType, u32 srcOfs, u32 dstOfs, u32 transSize, HEAPID heapID );
extern void GFL_ARCHDL_UTIL_TransVramPaletteEx( ARCHANDLE *handle, ARCDATID datID, PALTYPE palType, u32 srcOfs, u32 dstOfs, u32 transSize, HEAPID heapID );

//------------------------------------------------------------------
/**
 * ��گ��ް� �� VRAM �]���� NITRO System ��گ���۷� ���쐬
 *�i3D, OBJ �p�ɂ̂ݑΉ��BBG �ɂ͎g���܂���j
 *
 * @param   arcID		������̧�ٲ��ޯ��
 * @param   datID		�������ް����ޯ��
 * @param   type		�]��������
 * @param   offs		�]���̾��
 * @param   heapID		˰��ID
 * @param   proxy		�쐬������۷��̱��ڽ
 *
 *	[ type ]
 *		NNS_G2D_VRAM_TYPE_3DMAIN = 0,
 *		NNS_G2D_VRAM_TYPE_2DMAIN = 1,
 *		NNS_G2D_VRAM_TYPE_2DSUB  = 2,
 *
 */
//------------------------------------------------------------------
extern void GFL_ARC_UTIL_TransVramPaletteMakeProxy( ARCID arcID, ARCDATID datID, NNS_G2D_VRAM_TYPE type, u32 offs, HEAPID heapID, NNSG2dImagePaletteProxy* proxy );
extern void GFL_ARCHDL_UTIL_TransVramPaletteMakeProxy( ARCHANDLE *handle, ARCDATID datID, NNS_G2D_VRAM_TYPE type, u32 offs, HEAPID heapID, NNSG2dImagePaletteProxy* proxy );

//------------------------------------------------------------------
/**
 * ����ް� �� VRAM �]���� NITRO System �Ұ����۷� ���쐬
 *�i3D, OBJ �p�ɂ̂ݑΉ��BBG �ɂ͎g���܂���j
 *
 *
 * @param   arcID			������̧�ٲ��ޯ��
 * @param   datID			�������ް����ޯ��
 * @param   compressedFlag	���k����Ă��邩
 * @param   mapType			ϯ��ݸ�����
 * @param   transSize		�]�����ށi0�Ȃ�S�]���j
 * @param   vramType		�]��������
 * @param   offs			�]���̾��
 * @param   heapID			�����؂Ɏg��˰��ID
 * @param   proxy			�쐬������۷��̱��ڽ
 *
 * @retval  �]�����ꂽ�ް����ށi�޲ĒP�ʁj
 *
 *	[ mapType ]
 *		MAP_TYPE_1D = 0,
 *		MAP_TYPE_2D = 1,
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
extern u32 GFL_ARC_UTIL_TransVramCharacterMakeProxy( ARCID arcID, ARCDATID datID, BOOL compressedFlag, CHAR_MAPPING_TYPE mapType,
	u32 transSize, NNS_G2D_VRAM_TYPE vramType, u32 offs, HEAPID heapID, NNSG2dImageProxy* proxy );
extern u32 GFL_ARCHDL_UTIL_TransVramCharacterMakeProxy(ARCHANDLE *handle, ARCDATID datID, BOOL compressedFlag, CHAR_MAPPING_TYPE mapType, u32 transSize, NNS_G2D_VRAM_TYPE vramType, u32 offs, HEAPID heapID, NNSG2dImageProxy* proxy );


//------------------------------------------------------------------
/**
 * ����ް� �� VRAM �]���� NITRO System �Ұ����۷� ���쐬
 *�i3D, OBJ �p�ɂ̂ݑΉ��BBG �ɂ͎g���܂���j
 *
 * ����ް���ϯ��ݸ�Ӱ�ޒl���A���݂�ڼ޽��ݒ�ɍ��킹�ď��������܂�
 *
 * @param   arcID			������̧�ٲ��ޯ��
 * @param   datID			�������ް����ޯ��
 * @param   compressedFlag	���k����Ă��邩
 * @param   mapType			ϯ��ݸ�����
 * @param   transSize		�]���T�C�Y�B�O�Ȃ�S�]���B
 * @param   vramType		�]��������
 * @param   offs			�]���̾��
 * @param   heapID			�����؂Ɏg��˰��ID
 * @param   proxy			�쐬������۷����ڽ
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
extern void GFL_ARC_UTIL_TransVramCharacterMakeProxySyncroMappingMode( ARCID arcID, ARCDATID datID, BOOL compressedFlag, CHAR_MAPPING_TYPE mapType, u32 transSize,
	NNS_G2D_VRAM_TYPE vramType, u32 offs, HEAPID heapID, NNSG2dImageProxy* proxy );
extern void GFL_ARCHDL_UTIL_TransVramCharacterMakeProxySyncroMappingMode( ARCHANDLE *handle, ARCDATID datID, BOOL compressedFlag, CHAR_MAPPING_TYPE mapType, u32 transSize,
	NNS_G2D_VRAM_TYPE vramType, u32 offs, HEAPID heapID, NNSG2dImageProxy* proxy );

//------------------------------------------------------------------
/**
 * ����ް� �� VRAM �]���� NITRO System �Ұ����۷� ���쐬�BVRAM�]���^�̉摜�f�ޗp�B
 *
 * �� ���̊֐����g���Ă��AVRAM�ɉ摜�͓]������܂���
 *    ���̊֐���۰�ނ����ް��͉������܂���B�߂�l��const void*���Ǘ����āA
 *    �s�v�ɂȂ��������������s���Ă��������B
 *
 * @param   arcID			������̧�ٲ��ޯ��
 * @param   datID			�������ް����ޯ��
 * @param   compressedFlag	���k����Ă��邩
 * @param   vramType		�]��������
 * @param   offs			�]���̾��
 * @param   heapID			�����؂Ɏg��˰��ID
 * @param   proxy			�쐬������۷��̱��ڽ
 * @param   charData		����ް����ڽ��ێ������߲���̱��ڽ
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
extern const void* GFL_ARC_UTIL_LoadTransTypeCharacterMakeProxy( ARCID arcID, ARCDATID datID, BOOL compressedFlag, 
	NNS_G2D_VRAM_TYPE vramType, u32 offs, HEAPID heapID, NNSG2dImageProxy* proxy, NNSG2dCharacterData** charData );
extern const void* GFL_ARCHDL_UTIL_LoadTransTypeCharacterMakeProxy( ARCHANDLE *handle, ARCDATID datID, BOOL compressedFlag, 
	NNS_G2D_VRAM_TYPE vramType, u32 offs, HEAPID heapID, NNSG2dImageProxy* proxy, NNSG2dCharacterData** charData );

//------------------------------------------------------------------
/**
 * ����ް��� ۰�ނ��� Unpack ���邾���ł��B����͊e���ŁB�iBG�p�j
 *
 * @param   arcID				[in] ������̧�ٲ��ޯ��
 * @param   datID				[in] �ް����ޯ��
 * @param   compressedFlag		[in] ���k����Ă��邩
 * @param   charData			[out] ����ް����ڽ��ێ������߲���̱��ڽ
 * @param   heapID				[in] ˰��ID
 *
 * @retval  void*		۰�ނ����ް��̐擪�߲��
 */
//------------------------------------------------------------------
extern void* GFL_ARC_UTIL_LoadBGCharacter( ARCID arcID, ARCDATID datID, BOOL compressedFlag, NNSG2dCharacterData** charData, HEAPID heapID );
extern void* GFL_ARCHDL_UTIL_LoadBGCharacter( ARCHANDLE *handle, ARCDATID datID, BOOL compressedFlag, NNSG2dCharacterData** charData, HEAPID heapID );

//------------------------------------------------------------------
/**
 * ����ް��� ۰�ނ��� Unpack ���邾���ł��B����͊e���ŁB�iOBJ�p�j
 *
 * @param   arcID				[in] ������̧�ٲ��ޯ��
 * @param   datID				[in] �ް����ޯ��
 * @param   compressedFlag		[in] ���k����Ă��邩
 * @param   charData			[out] ����ް����ڽ��ێ������߲���̱��ڽ
 * @param   heapID				[in] ˰��ID
 *
 * @retval  void*		۰�ނ����ް��̐擪�߲��
 */
//------------------------------------------------------------------
extern void* GFL_ARC_UTIL_LoadOBJCharacter( ARCID arcID, ARCDATID datID, BOOL compressedFlag, NNSG2dCharacterData** charData, HEAPID heapID );
extern void* GFL_ARCHDL_UTIL_LoadOBJCharacter( ARCHANDLE *handle, ARCDATID datID, BOOL compressedFlag, NNSG2dCharacterData** charData, HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * ��ذ��ް��� ۰�ނ��� Unpack ���邾���ł��B����͊e���ŁB
 *
 * @param   arcID			������̧�ٲ��ޯ��
 * @param   datID			�������ް����ޯ��
 * @param   compressedFlag	���k����Ă����ް����H
 * @param   scrnData		��ذ��ް����ڽ��ێ������߲���̱��ڽ
 * @param   heapID			�ް��ǂݍ��݁E�������؂Ƃ��Ďg��˰��ID
 *
 * @retval  void*		۰�ނ����ް��̐擪�߲��
 */
//--------------------------------------------------------------------------------------------
extern void* GFL_ARC_UTIL_LoadScreen(ARCID arcID, ARCDATID datID, BOOL compressedFlag, NNSG2dScreenData** scrnData, HEAPID heapID);
extern void* GFL_ARCHDL_UTIL_LoadScreen(ARCHANDLE *handle, ARCDATID datID, BOOL compressedFlag, NNSG2dScreenData** scrnData, HEAPID heapID);

//------------------------------------------------------------------
/**
 * ��گ��ް���۰�ނ��� Unpack ���邾���ł��B����͊e���ŁB
 *
 * @param   arcID			������̧�ٲ��ޯ��
 * @param   datID			�������ް����ޯ��
 * @param   palData			��گ��ް����ڽ��ێ������߲���̱��ڽ
 * @param   heapID			˰��ID
 *
 * @retval  void*		۰�ނ����ް��̐擪�߲��
 */
//------------------------------------------------------------------
extern void* GFL_ARC_UTIL_LoadPalette( ARCID arcID, ARCDATID datID, NNSG2dPaletteData** palData, HEAPID heapID );
extern void* GFL_ARCHDL_UTIL_LoadPalette(ARCHANDLE *handle, ARCDATID datID, NNSG2dPaletteData** palData, HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * ����ݸ�ް��� ۰�ނ��� Unpack ���邾���ł��B����͊e���ŁB
 *
 * @param   arcID			������̧�ٲ��ޯ��
 * @param   datID			�������ް����ޯ��
 * @param   compressedFlag	���k����Ă����ް����H
 * @param   cellBank		����ݸ�ް����ڽ��ێ������߲���̱��ڽ
 * @param   heapID			˰��ID
 *
 * @retval  void*		۰�ނ����ް��̐擪�߲��
 */
//--------------------------------------------------------------------------------------------
extern void* GFL_ARC_UTIL_LoadCellBank(ARCID arcID, ARCDATID datID, BOOL compressedFlag, NNSG2dCellDataBank** cellBank, HEAPID heapID );
extern void* GFL_ARCHDL_UTIL_LoadCellBank(ARCHANDLE *handle, ARCDATID datID, BOOL compressedFlag, NNSG2dCellDataBank** cellBank, HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * �����ݸ�ް��� ۰�ނ��� Unpack ���邾���ł��B����͊e���ŁB
 *
 * @param   arcID			������̧�ٲ��ޯ��
 * @param   datID			�������ް����ޯ��
 * @param   compressedFlag	���k����Ă����ް����H
 * @param   anmBank			�����ݸ�ް����ڽ��ێ������߲���̱��ڽ
 * @param   heapID			˰��ID
 *
 * @retval  void*		۰�ނ����ް��̐擪�߲��
 */
//--------------------------------------------------------------------------------------------
extern void* GFL_ARC_UTIL_LoadAnimeBank(ARCID arcID, ARCDATID datID, BOOL compressedFlag, NNSG2dAnimBankData** anmBank, HEAPID heapID);
extern void* GFL_ARCHDL_UTIL_LoadAnimeBank(ARCHANDLE *handle, ARCDATID datID, BOOL compressedFlag, NNSG2dAnimBankData** anmBank, HEAPID heapID);

//--------------------------------------------------------------------------------------------
/**
 * �������ݸ�ް��� ۰�ނ��� Unpack ���邾���ł��B����͊e���ŁB
 *
 * @param   arcID			������̧�ٲ��ޯ��
 * @param   datID			�������ް����ޯ��
 * @param   compressedFlag	���k����Ă����ް����H
 * @param   cellBank		�������ݸ�ް����ڽ��ێ������߲���̱��ڽ
 * @param   heapID			˰��ID
 * @param   pSize			����ݸ���޲Ļ��ނ��󂯎��ϐ��߲���i�v��Ȃ����NULL�Łj
 *
 * @retval  void*		۰�ނ����ް��̐擪�߲��
 */
//--------------------------------------------------------------------------------------------
extern	void* GFL_ARC_UTIL_LoadMultiCellBank(ARCID arcID, ARCDATID datID, BOOL compressedFlag, NNSG2dMultiCellDataBank** cellBank, HEAPID heapID );
extern void* GFL_ARCHDL_UTIL_LoadMultiCellBank(ARCHANDLE *handle, ARCDATID datID, BOOL compressedFlag, NNSG2dMultiCellDataBank** cellBank, HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * �����ݸ�ް��� ۰�ނ��� Unpack ���邾���ł��B����͊e���ŁB
 *
 * @param   arcID			������̧�ٲ��ޯ��
 * @param   datID			�������ް����ޯ��
 * @param   compressedFlag	���k����Ă����ް����H
 * @param   anmBank			����ٱ����ݸ�ް����ڽ��ێ������߲���̱��ڽ
 * @param   heapID			˰��ID
 *
 * @retval  void*		۰�ނ����ް��̐擪�߲��
 */
//--------------------------------------------------------------------------------------------
extern	void* GFL_ARC_UTIL_LoadMultiAnimeBank(ARCID arcID, ARCDATID datID, BOOL compressedFlag, NNSG2dMultiCellAnimBankData** anmBank, HEAPID heapID);
extern void* GFL_ARCHDL_UTIL_LoadMultiAnimeBank(ARCHANDLE *handle, ARCDATID datID, BOOL compressedFlag, NNSG2dMultiCellAnimBankData** anmBank, HEAPID heapID);

//------------------------------------------------------------------
/**
 * LZ���k��A�[�J�C�u����Ă���f�[�^��ǂݏo���A�𓀂��ĕԂ�
 *
 * @param   arcID		�A�[�J�C�u�t�@�C���C���f�b�N�X
 * @param   datID		�A�[�J�C�u�f�[�^�C���f�b�N�X
 * @param   heapID		�ǂݏo���E�𓀂Ɏg���q�[�v�h�c
 *
 * @retval  void*		�𓀌�̃f�[�^�ۑ���A�h���X
 */
//------------------------------------------------------------------
extern void* GFL_ARC_UTIL_LoadUnCompress(ARCID arcID, ARCDATID datID, HEAPID heapID);
extern void* GFL_ARCHDL_UTIL_LoadUnCompress(ARCHANDLE *handle, ARCDATID datID, HEAPID heapID);


//------------------------------------------------------------------
/**
 * �A�[�J�C�u�f�[�^�̓ǂݏo��
 *
 * @param   arcID			�A�[�J�C�u�t�@�C���C���f�b�N�X
 * @param   datID			�A�[�J�C�u�f�[�^�C���f�b�N�X
 * @param   compressedFlag	���k����Ă��邩�H
 * @param   heapID			�������m�ۂɎg���q�[�v�h�c
 * @param   allocType		�q�[�v�̂ǂ̈ʒu���烁�����m�ۂ��邩
 *
 * @retval  void*			�ǂݏo���̈�|�C���^
 */
//------------------------------------------------------------------
extern void* GFL_ARC_UTIL_Load(ARCID arcID, ARCDATID datID, BOOL compressedFlag, HEAPID heapID );
extern void* GFL_ARCHDL_UTIL_Load(ARCHANDLE *handle, ARCDATID datID, BOOL compressedFlag, HEAPID heapID);

//------------------------------------------------------------------
/**
 * �A�[�J�C�u�f�[�^�̓ǂݏo�����f�[�^�T�C�Y�擾�i���k����Ă�����𓀌�̃T�C�Y���擾����j
 *
 * @param   arcID			�A�[�J�C�u�t�@�C���C���f�b�N�X
 * @param   datID			�A�[�J�C�u�f�[�^�C���f�b�N�X
 * @param   compressedFlag	���k����Ă��邩�H
 * @param   heapID			�������m�ۂɎg���q�[�v�h�c
 * @param   allocType		�q�[�v�̂ǂ̈ʒu���烁�����m�ۂ��邩
 * @param   pSize			���f�[�^�̃o�C�g�T�C�Y���󂯎��ϐ��̃|�C���^
 *
 * @retval  void*			�ǂݏo���̈�|�C���^
 */
//------------------------------------------------------------------
extern void* GFL_ARC_UTIL_LoadEx(ARCID arcID, ARCDATID datID, BOOL compressedFlag, HEAPID heapID, u32* pSize);
extern void* GFL_ARCHDL_UTIL_LoadEx(ARCHANDLE* handle, ARCDATID datID, BOOL compressedFlag, u32 heapID, u32* pSize);





#ifdef __cplusplus
}/* extern "C" */
#endif

#endif
