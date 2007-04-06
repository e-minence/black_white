//=============================================================================================
/**
 * @file	textprint.c
 * @brief	�r�b�g�}�b�v������\���p�V�X�e��
 * 
 */
//=============================================================================================
#include "gflib.h"

#include "textprint.h"
#include "sjisconv_inter.h"
//------------------------------------------------------------------
/**
 * ���[�N�\����
 */
//------------------------------------------------------------------
typedef struct {
	GFL_FONT_DATA		fdata;
	u16					nowx;
	u16					nowy;
}TEXT_WORK;

TEXT_WORK* tw;

//------------------------------------------------------------------
/**
 *
 * ������
 *
 */
//------------------------------------------------------------------
void
	GFL_TEXT_CreateSystem
		( const char* fontDataPath )
{
	GFL_FONT_Create( fontDataPath );
	tw = GFL_HEAP_AllocMemory( GFL_HEAPID_SYSTEM, sizeof(TEXT_WORK) );
}


//------------------------------------------------------------------
/**
 *
 * �I��
 *
 */
//------------------------------------------------------------------
void
	GFL_TEXT_DeleteSystem
		( void )
{
	GFL_HEAP_FreeMemory( tw );
	GFL_FONT_Delete();
}


//------------------------------------------------------------------
/**
 * �r�b�g�}�b�v�̓���CGX�̈�ɕ�����`�悷��(STRBUF)
 *
 * @param   text		������
 * @param   param		�`��p�p�����[�^
 */
//------------------------------------------------------------------
void
	GFL_TEXT_Print
		( const STRBUF* text, GFL_TEXT_PRINTPARAM* param )
{
	STRCODE* strdat = (STRCODE*)GFL_STR_GetStringCodePointer( text );
	GFL_TEXT_PrintCode( strdat, param );
}


//------------------------------------------------------------------
/**
 * �r�b�g�}�b�v�̓���CGX�̈�ɕ�����`�悷��(STRCODE)
 *
 * @param   textcode	������
 * @param   param		�`��p�p�����[�^
 */
//------------------------------------------------------------------
void
	GFL_TEXT_PrintCode
		( const STRCODE* textcode, GFL_TEXT_PRINTPARAM* param )
{
	u8				sizemaxY	= GFL_FONT_GetSizeMaxY();
	u8				sizeSPC		= GFL_FONT_GetSizeSPC();
	STRCODE			fcode;
	GFL_BMP_DATA	*bmpfont;

	//�`��ʒu������
	tw->nowx = param->writex;
	tw->nowy = param->writey;

	//�F�ݒ�
	GFL_FONT_SetColor( param->colorF, param->colorB );

	//�t�H���g�r�b�g�}�b�v���쐬
	bmpfont = GFL_BMP_CreateWithData( &tw->fdata.data[0], tw->fdata.sizex, sizemaxY, GFL_BMP_16_COLOR, GFL_HEAPID_SYSTEM );
	
	//�t�H���g�f�[�^�擾
	while( (fcode = *textcode ) != EOM_ ) 
	{
		textcode++;

		switch(fcode){

		case EOM_:	/* �I���R�[�h */
			GFL_HEAP_FreeMemory( tw );
			return;

		case CR_:	/* ���s�R�[�h */
			tw->nowx = param->writex;
			tw->nowy += ( sizemaxY + param->spacey ); 
			break;

		case spc_:	/* �S�p�X�y�[�X�R�[�h */
			tw->nowx += sizeSPC;
			break;

		case h_spc_:	/* ���p�X�y�[�X�R�[�h */
			tw->nowx += (sizeSPC/2);
			break;

		default:
			GFL_FONT_GetData( fcode - STR_DEFAULT_CODE_MAX, &tw->fdata );

			GFL_BMP_Print( bmpfont, param->bmp, 0, 0, tw->nowx, tw->nowy, tw->fdata.sizex, sizemaxY, 0 );

			//���̕����̕`��ʒu��ݒ�
			tw->nowx += ( tw->fdata.sizex + param->spacex );
			break;
		}
	}
	GFL_BMP_Delete(bmpfont);
}


//------------------------------------------------------------------
/**
 * �r�b�g�}�b�v�̓���CGX�̈�ɕ�����`�悷��(SjisCode)
 *
 * @param   textcode	������
 * @param   param		�`��p�p�����[�^
 */
//------------------------------------------------------------------
#define SJISCONVSIZE_MAX (256)//�b��256�����܂�
void
	GFL_TEXT_PrintSjisCode
		( const char* textcode, GFL_TEXT_PRINTPARAM* param )
{
	STRCODE* tmpcode = GFL_HEAP_AllocMemory( GFL_HEAPID_SYSTEM, SJISCONVSIZE_MAX*2 );

	GFL_STR_CONV_Sjis2Systemfontcode( textcode, tmpcode, SJISCONVSIZE_MAX );
	GFL_TEXT_PrintCode( tmpcode, param );

	GFL_HEAP_FreeMemory( tmpcode );
}


