//============================================================================================
/**
 * @file	str_tool.h
 * @brief	�����񑀍�֐��Q
 * @author	taya
 * @date	2008.11.17
 *
 * �������z��������֐��Q�^�g�p�͌����A�����Ƃ��܂��B
 */
//============================================================================================
#ifndef __STR_TOOL_H__
#define __STR_TOOL_H__

//------------------------------------------------------------------
/**
 * ���l�𕶎��񉻂��鎞�̐��`�^�C�v
 */
//------------------------------------------------------------------
typedef enum {

	STR_NUM_DISP_LEFT,		///< ���l��
	STR_NUM_DISP_SPACE,		///< �E�l�߁i�X�y�[�X���߁j
	STR_NUM_DISP_ZERO,		///< �E�l�߁i�[�����߁j

}StrNumberDispType;



//------------------------------------------------------------------
/**
 * ���l�𕶎��񉻂��鎞�̕����R�[�h�^�C�v
 */
//------------------------------------------------------------------
typedef enum {

	STR_NUM_CODE_ZENKAKU,		///< �S�p
	STR_NUM_CODE_HANKAKU,		///< ���p

	#if PM_LANG == LANG_JAPAN
	STR_NUM_CODE_DEFAULT = STR_NUM_CODE_ZENKAKU,
	#else
	STR_NUM_CODE_DEFAULT = STR_NUM_CODE_HANKAKU,
	#endif

}StrNumberCodeType;

extern void STRTOOL_SetNumber( STRBUF* dst, int number, u32 keta, StrNumberDispType dispType, StrNumberCodeType codeType );
extern void STRTOOL_SetHexNumber( STRBUF* dst, u32 number, u32 keta, StrNumberDispType dispType, StrNumberCodeType codeType );
extern void STRTOOL_Copy( const STRCODE* src, STRCODE* dst, u32 dstElems );
extern BOOL STRTOOL_Comp( const STRCODE* src, const STRCODE* dst );


#endif
