//=============================================================================================
/**
 * @file	textprint.c
 * @brief	�r�b�g�}�b�v������\���p�V�X�e��
 * 
 */
//=============================================================================================
#include "gflib.h"

#include "textprint.h"
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
	GFL_TEXT_sysInit
		( const char* fontDataPath )
{
	GFL_FONT_sysInit( fontDataPath );
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
	GFL_TEXT_sysExit
		( void )
{
	GFL_HEAP_FreeMemory( tw );
	GFL_FONT_sysExit();
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
		( STRBUF* text, GFL_TEXT_PRINTPARAM* param )
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
		( STRCODE* textcode, GFL_TEXT_PRINTPARAM* param )
{
	u8				sizemaxY	= GFL_FONT_GetSizeMaxY();
	u8				sizeSPC		= GFL_FONT_GetSizeSPC();
	STRCODE			fcode;
	GFL_BMP_DATA	bmpfont;

	//�`��ʒu������
	tw->nowx = param->writex;
	tw->nowy = param->writey;

	//�F�ݒ�
	GFL_FONT_SetColor( param->colorF, param->colorB );

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

		case spc_:	/* ���s�R�[�h */
			tw->nowx += sizeSPC;
			break;

		default:
			GFL_FONT_GetData( fcode - STR_DEFAULT_CODE_MAX, &tw->fdata );
			//�t�H���g�r�b�g�}�b�v���쐬
			bmpfont.adrs = tw->fdata.data;
			bmpfont.size_x = tw->fdata.sizex;
			bmpfont.size_y = sizemaxY;

			if( param->mode == GFL_TEXT_WRITE_16 ){
				GFL_BMP_PrintMain(	&bmpfont, param->bmp, 0, 0, tw->nowx, tw->nowy, 
										bmpfont.size_x, bmpfont.size_y, 0 );
			} else {
				GFL_BMP_PrintMain256(	&bmpfont, param->bmp, 0, 0, tw->nowx, tw->nowy, 
											bmpfont.size_x, bmpfont.size_y, 0 );
			}
			//���̕����̕`��ʒu��ݒ�
			tw->nowx += ( tw->fdata.sizex + param->spacex );
			break;
		}
	}
}



