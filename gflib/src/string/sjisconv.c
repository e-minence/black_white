//============================================================================================
/**
 * @file	sjisconv.c
 * @brief	�r�������R�[�h����f�e�k�V�X�e���t�H���g�R�[�h�ւ̕ϊ�
 */
//============================================================================================
#include "gflib.h"
#include "systemfontcode.h"

#include "sjisconv_inter.h"


#define UNKNOWN_CHARCODE ( period_ )
#define IGNORE_CHARCODE  ( 0xfffe )

static STRCODE convert1( const unsigned char sjisCode );
static STRCODE convert2( const unsigned short sjisCode );

//------------------------------------------------------------------
/**
 * ������R���o�[�g�i Sjis > SystemFontCode �j
 *
 * @param   src				�ϊ��OSjis������|�C���^
 * @param   dst				�ϊ���STRCODE������|�C���^
 * @param   dstCodeCount	STRCODE�i�[�ő啶�����i�I�[�o�[�t���[���p�j
 *
 */
//------------------------------------------------------------------
void 
	GFL_STR_CONV_Sjis2Systemfontcode
		( const char* src, STRCODE* dst, int dstCodeCount )
{
	STRCODE code;
	int	i = 0;

	while( (*src != '\0')&&(i < (dstCodeCount-1)) ){
		unsigned char sjis = *src;
		src++;

		// 2byte������1byte��(ASCII/JIS���[�}���A���p�J�^�J�i�G���A�̃`�F�b�N)
		if( ((sjis >= 0x81) && (sjis <= 0x9f)) || ((sjis >= 0xe0) && (sjis <= 0xff)) )
		{
			code = convert2( (sjis << 8) + (short)(*src & 0xff) );// 2byte�̈�R���o�[�g
			src++;
		} else {
			code = convert1( sjis );// 1byte�̈�R���o�[�g
		}
		if( code != IGNORE_CHARCODE ){
			*dst = code;
			dst++;
			i++;
		}
	}
	*dst = EOM_;
}


//------------------------------------------------------------------
/**
 * �P�����R���o�[�g�y1byte�̈�z�i Sjis > SystemFontCode �j
 *
 * @param   sjisCode
 *
 * @retval	fontCode  
 */
//------------------------------------------------------------------
static const struct {
	unsigned char  sjis_code;
	STRCODE        font_code;
}code_translate_tbl[] = {
	{  ' ',   h_spc_ },
	{  '\n',  CR_ },
	{  '?',   hate_ },
	{  '=',   equal_ },
	{  '_',   uber_ },
	{  '[',   kakukako_ },
	{  ']',   kakukakot_ },
	{  '@',   atmark_ },
	{  ':',   colon_ },
	{  ';',   semicolon_ },
	{  '\'',  yen_ },
	{  '\r',  IGNORE_CHARCODE },
};

static STRCODE convert1( const unsigned char sjisCode )
{
	if((sjisCode >= 'a') && (sjisCode <= 'z' ))		return a__ + (sjisCode - 'a');
	if((sjisCode >= 'A') && (sjisCode <= 'Z' ))		return A__ + (sjisCode - 'A');
	if((sjisCode >= '0') && (sjisCode <= '9' ))		return h_n0_ + (sjisCode - '0');
	if((sjisCode >= '!') && (sjisCode <= '/' ))		return gyoe_ + (sjisCode - '!');
	{
		int i;

		for(i=0; i<NELEMS(code_translate_tbl); i++)
		{
			if( sjisCode == code_translate_tbl[i].sjis_code )
			{
				return code_translate_tbl[i].font_code;
				break;
			}
		}
	}
	return UNKNOWN_CHARCODE;
}


//------------------------------------------------------------------
/**
 * �P�����R���o�[�g�y2byte�̈�z�i Sjis > SystemFontCode �j
 *
 * @param   sjisCode
 *
 * @retval	fontCode  
 */
//------------------------------------------------------------------
static STRCODE convert2( const unsigned short sjisCode )
{
	if((sjisCode >= '��') && (sjisCode <= '��' ))	return aa_ + (sjisCode - '��');
	if((sjisCode >= '�@') && (sjisCode <= '��' ))	return AA_ + (sjisCode - '�@');
	if((sjisCode >= '�O') && (sjisCode <= '�X' ))	return n0_ + (sjisCode - '�O');
	if(sjisCode == '�@') return spc_;
	if(sjisCode == '�[') return bou_;

	return UNKNOWN_CHARCODE;
}


