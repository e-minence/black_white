/*---------------------------------------------------------------------------*
  Project:  REX DEMO shared
  File:	 report.c

  Copyright 2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: graphic.c,v $
  Revision 1.3  2007/07/18 06:55:43  inaba_kimitaka
  NTSC�ȊO�̎󑜕����ɑΉ�

  Revision 1.2  2006/08/31 14:05:18  yosizaki
  �`��ݒ�֐���ǉ�.

  Revision 1.1  2006/08/29 07:18:14  adachi_hiroaki
  ���ʂ̃O���t�B�b�N������ǉ�


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <revolution.h>
#include <revolution/kpad.h>
#include <revolution/mem.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "rexdemo/graphic.h"

/*---------------------------------------------------------------------------*
	�萔��`
 *---------------------------------------------------------------------------*/
#define	 REXDEMO_FIFO_SIZE	( 256 * 1024 )
#define	 REXDEMO_XFB_COUNT	( 2 )

#define	 REXDEMO_RAW_COUNT	( 24 + 1 )
#define	 REXDEMO_COL_COUNT	( 64 )

#define	 REXDEMO_SHOW_FRAME_COUNT	 1

/*---------------------------------------------------------------------------*
	�\���̒�`
 *---------------------------------------------------------------------------*/
typedef struct REXDEMOCamera
{
	Vec	 pos;
	Vec	 up;
	Vec	 target;
	f32	 fovy;
	f32	 aspect;
	f32	 znear;
	f32	 zfar;

} REXDEMOCamera;

/*---------------------------------------------------------------------------*
	�����ϐ���`
 *---------------------------------------------------------------------------*/
static s32			  sampleFrameCount	= 0;
static s32			  sampleFrameCycle;
static GXRenderModeObj  sampleRend;
static u8			   sampleFifoBuf[ REXDEMO_FIFO_SIZE ] ATTRIBUTE_ALIGN( 32 );
static void*			sampleXFB[ REXDEMO_XFB_COUNT ];

static u8			   sampleFontBuf[ OSRoundUp32B( OS_FONT_SIZE_ANSI + OS_FONT_SIZE_SJIS ) ] ATTRIBUTE_ALIGN( 32 );
static OSFontHeader*	sampleFont  = (OSFontHeader*)( &( sampleFontBuf[ 0 ] ) );
static char			 sampleString[ REXDEMO_RAW_COUNT ][ 256 ];
static GXColor		  sampleStringCol[ REXDEMO_RAW_COUNT ][ 256 ];
static s32			  sampleRawIndex;
static s32			  sampleNextRawIndex;
static s32			  sampleNextColIndex;
static REXDEMOCamera	sampleCamera;
static GXLightObj	   sampleLight;

static GXColor		  sampleTextCol;
static GXColor		  sampleGroundCol;
static int			  sampleFontWidth;
static int			  sampleFontHeight;
static BOOL			 sampleIsBeginRenderCalled;

/*---------------------------------------------------------------------------*
	�����֐��v���g�^�C�v
 *---------------------------------------------------------------------------*/
static void	 InitString( void );
static void	 RenderString( void );
static void*	RenderThread( void* arg );
static int	  REXDEMODrawChar( int x, int y, int z, int w, int h, const char **string );


/*===========================================================================*/

/*---------------------------------------------------------------------------*
  Name		: REXDEMOGraphicInit
  Description : ��ʕ\�����s���ׂ̏������������s���B
  Arguments   : �Ȃ�
  Returns	 : �Ȃ�
 *---------------------------------------------------------------------------*/
void
REXDEMOInitScreen( BOOL autoRender )
{
	/* Initialize VI */
	{
		GXRenderModeObj*  viConf  = &GXNtsc480IntDf;
	
		VIInit();
		
		switch (VIGetTvFormat())
		{
		  case VI_NTSC:
			viConf = &GXNtsc480IntDf;
			break;
		  case VI_PAL:
			viConf = &GXPal528IntDf;
			break;
		  case VI_EURGB60:
			viConf = &GXEurgb60Hz480IntDf;
			break;
		  case VI_MPAL:
			viConf = &GXMpal480IntDf;
			break;
		  default:
			OSHalt("DEMOInit: invalid TV format\n");
			break;
		}
		
		(void)memcpy( &sampleRend, viConf, sizeof( GXRenderModeObj ) );
		
		// Trim off from top & bottom 16 scanlines (which will be overscanned).
		// So almost all demos actually render only 448 lines (in NTSC case.)
		// Since this setting is just for SDK demos, you can specify this
		// in order to match your application requirements.
		GXAdjustForOverscan( &sampleRend, &sampleRend, 0, 16 );
		sampleFrameCycle	= ( ( ( sampleRend.viTVmode >> 2 ) == VI_PAL ) ? 50 : 60 );
		VIConfigure( &sampleRend );
	}

	/* Allocate eXternal frame buffer */
	{
		void*   arena_s;
		u32	 xfbSize;
		s32	 i;
		
		arena_s = OSGetMEM1ArenaLo();
		xfbSize = (u32)( VIPadFrameBufferWidth( sampleRend.fbWidth ) * sampleRend.xfbHeight * VI_DISPLAY_PIX_SZ );
		for( i = 0; i < REXDEMO_XFB_COUNT; i ++ )
		{
			sampleXFB[ i ]  = (void*)OSRoundUp32B( (u32)arena_s );
			arena_s = (void*)OSRoundUp32B( (u32)( sampleXFB[ i ] ) + xfbSize );
		}
		OSSetMEM1ArenaLo( arena_s );
	}

	/* Initialize GX */
	{
		GXFifoObj*  gxFifo;
		f32		 yScale;
		u16		 xfbHeight;
	
		gxFifo  = GXInit( sampleFifoBuf, REXDEMO_FIFO_SIZE );
		GXSetViewport( 0.0F, 0.0F, (f32)sampleRend.fbWidth, (f32)sampleRend.efbHeight, 0.0F, 1.0F );
		GXSetScissor( 0, 0, (u32)sampleRend.fbWidth, (u32)sampleRend.efbHeight );
		yScale  = GXGetYScaleFactor( sampleRend.efbHeight, sampleRend.xfbHeight );
		xfbHeight   = (u16)GXSetDispCopyYScale( yScale );
		GXSetDispCopySrc( 0, 0, sampleRend.fbWidth, sampleRend.efbHeight );
		GXSetDispCopyDst( sampleRend.fbWidth, xfbHeight );
		GXSetCopyFilter( sampleRend.aa, sampleRend.sample_pattern, GX_TRUE, sampleRend.vfilter );
		GXSetDispCopyGamma( GX_GM_1_0 );
		if( sampleRend.aa )
		{
			GXSetPixelFmt( GX_PF_RGB565_Z16, GX_ZC_LINEAR );
		}
		else
		{
			GXSetPixelFmt( GX_PF_RGB8_Z24, GX_ZC_LINEAR );
		}
		GXCopyDisp( sampleXFB[ 0 ], GX_TRUE );
	}

	/* Initialize display settings */
	{
		static const GXColor strCol = { 0x00, 0x00, 0x00, 0xff };
		static const GXColor backCol = { 0xff, 0xff, 0xff, 0xff };
		sampleTextCol			 = strCol;
		sampleGroundCol		   = backCol;
		sampleIsBeginRenderCalled = FALSE;
		sampleFontWidth		   = -1;
		sampleFontHeight		  = -1;
	}

	InitString();

	/* Wait for first frame */
	{
		VISetNextFrameBuffer( sampleXFB[ 0 ] );
		VISetBlack( FALSE );
		VIFlush();
		VIWaitForRetrace();
		if( (u32)( sampleRend.viTVmode ) & 0x1 )
		{
			VIWaitForRetrace();
		}
	}
	if (autoRender)
	{
		static OSThread thread;
		static u32	  stack[2048];
		if ( !OSCreateThread( &thread, RenderThread, NULL,
							 (void*)( (u8*)stack + sizeof(stack) ),
							 sizeof(stack), 17, OS_THREAD_ATTR_DETACH ) )
		{
			OSReport("OSCreateThread() failed.\n");
			return;
		}
		(void)OSResumeThread(&thread);
	}
}

/*---------------------------------------------------------------------------*
  Name		: REXDEMOWaitRetrace
  Description : V-Blank ���荞�݂���������܂ő҂B
  Arguments   : �Ȃ�
  Returns	 : �Ȃ�
 *---------------------------------------------------------------------------*/
void
REXDEMOWaitRetrace( void )
{
	REXDEMOBeginRender();
	RenderString();
	GXCopyDisp( sampleXFB[ sampleFrameCount % REXDEMO_XFB_COUNT ], GX_TRUE );
	VISetNextFrameBuffer( sampleXFB[ sampleFrameCount % REXDEMO_XFB_COUNT ] );
	VIFlush();
	VIWaitForRetrace();
	sampleFrameCount ++;
	sampleIsBeginRenderCalled = FALSE;
}

/*---------------------------------------------------------------------------*
  Name		: REXDEMOReportEx
  Description : ��������f�o�b�O�\������B
  Arguments   : col - �����F�B
				msg - �\�����镶����
				... - ���z�����B
  Returns	 : �Ȃ�
 *---------------------------------------------------------------------------*/
void
REXDEMOReportEx( GXColor col, const char* msg, ... )
{
	va_list	 vl;
	char		temp[ 256 ];
	int		 len;
	s32		 i;
	char*	   p;

	(void)memset( temp, 0, sizeof( temp ) );
	va_start( vl, msg );
	len = vsprintf( temp, msg, vl );
	ASSERT( len < 256 );
	va_end( vl );

	OSReport( temp );
	p   = &temp[ 0 ];
	for( i = 0; i < 256; i ++ )
	{
		if( *p == 0x00 )
		{
			sampleString[ sampleNextRawIndex % REXDEMO_RAW_COUNT ][ sampleNextColIndex ] = 0x00;
			break;
		}
		if( *p == 0x0d )
		{
			p ++;
			continue;
		}
		if( *p == 0x0a )
		{
			sampleString[ sampleNextRawIndex % REXDEMO_RAW_COUNT ][ sampleNextColIndex ] = 0x00;
			sampleNextColIndex  = 0;
			sampleNextRawIndex  ++;
			if( sampleNextRawIndex >= REXDEMO_RAW_COUNT )
			{
				sampleRawIndex = ( sampleRawIndex + 1 ) % REXDEMO_RAW_COUNT;
			}
			p ++;
			continue;
		}
		sampleString[ sampleNextRawIndex % REXDEMO_RAW_COUNT ][ sampleNextColIndex ] = *p;
		sampleStringCol[ sampleNextRawIndex % REXDEMO_RAW_COUNT ][ sampleNextColIndex ] = col;
		if( ( ++ sampleNextColIndex ) > 255 )
		{
			sampleNextColIndex  = 0;
			sampleNextRawIndex  ++;
			if( sampleNextRawIndex >= REXDEMO_RAW_COUNT )
			{
				sampleRawIndex = ( sampleRawIndex + 1 ) % REXDEMO_RAW_COUNT;
			}
		}
		p ++;
	}
}

/*---------------------------------------------------------------------------*
  Name		: REXDEMOSetTextColor
  Description : �`��e�L�X�g�F��ݒ肷��B
  Arguments   : col	- �e�L�X�g�F
  Returns	 : �Ȃ�
 *---------------------------------------------------------------------------*/
void
REXDEMOSetTextColor( const GXColor col )
{
	sampleTextCol = col;
	GXSetTevColor(GX_TEVREG0, col);
}

/*---------------------------------------------------------------------------*
  Name		: REXDEMOSetGroundColor
  Description : �`��w�i�F��ݒ肷��B
  Arguments   : col	- �w�i�F
  Returns	 : �Ȃ�
 *---------------------------------------------------------------------------*/
void
REXDEMOSetGroundColor( const GXColor col )
{
	sampleGroundCol = col;
	sampleGroundCol.a = 0xFF;
}

/*---------------------------------------------------------------------------*
  Name		: REXDEMOSetFontSize
  Description : �t�H���g�̃T�C�Y��ݒ肷��B
  Arguments   : width  - X�� (0�ȉ����w�肷��Ǝ����v�Z)
				height - Y�� (0�ȉ����w�肷��Ǝ����v�Z)
  Returns	 : �Ȃ�
 *---------------------------------------------------------------------------*/
void
REXDEMOSetFontSize( int width, int height )
{
	sampleFontWidth  = width;
	sampleFontHeight = height;
}

/*---------------------------------------------------------------------------*
  Name		: REXDEMOBeginRender
  Description : �e�t���[���̕`��J�n�O�̏����B
  Arguments   : �Ȃ�
  Returns	 : �Ȃ�
 *---------------------------------------------------------------------------*/
void
REXDEMOBeginRender(void)
{
	f32	 scrWidth	= (f32)sampleRend.fbWidth;
	f32	 scrHeight   = (f32)sampleRend.efbHeight;

	const GXColor   ambientCol  = { 0xff, 0xff, 0xff, 0xff };

	if (sampleIsBeginRenderCalled)
	{
		return;
	}
	sampleIsBeginRenderCalled = TRUE;

	/* �`�揀�� */
	{
		Mtx44   projMtx;
		Mtx	 posMtx;
	
		GXInvalidateTexAll();
		/* �r���[�|�[�g�ݒ� */
		if( (u32)( sampleRend.viTVmode ) & 0x1 )
		{
			GXSetViewportJitter( 0.0f, 0.0f, scrWidth, scrHeight, 0.0F, 1.0F, VIGetNextField() );
		}
		else
		{
			GXSetViewport( 0.0f, 0.0f, scrWidth, scrHeight, 0.0F, 1.0F );
		}
		GXSetScissor( 0, 0, (u32)scrWidth, (u32)scrHeight );
		/* �J�����ݒ� */
		MTXOrtho( projMtx, 0.0f, scrHeight, 0.0f, scrWidth, sampleCamera.znear, sampleCamera.zfar );
		GXSetProjection( projMtx, GX_ORTHOGRAPHIC );
		MTXIdentity( posMtx );
		GXLoadPosMtxImm( posMtx, GX_PNMTX0 );
		GXSetCurrentMtx( GX_PNMTX0 );
		GXSetZMode( GX_TRUE, GX_LEQUAL, GX_TRUE );
		GXSetBlendMode( GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR );
		/* ���C�g�ݒ� */
		GXSetNumChans( 1 );
		GXSetChanAmbColor( GX_COLOR0A0, ambientCol );
		GXLoadLightObjImm( &sampleLight, GX_LIGHT0 );
		GXSetChanCtrl( GX_COLOR0, GX_ENABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT0, GX_DF_NONE, GX_AF_NONE );
		GXSetChanCtrl( GX_ALPHA0, GX_DISABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT0, GX_DF_NONE, GX_AF_NONE );
	}

	/* �w�i�`�� */
	{
		s16	 z   = -1023;
	
		/* vertex �ݒ� */
		GXClearVtxDesc();
		GXSetVtxDesc( GX_VA_POS, GX_DIRECT );
		GXSetVtxAttrFmt( GX_VTXFMT1, GX_VA_POS, GX_POS_XYZ, GX_S16, 0 );
		/* �e�N�X�`���ݒ� */
		GXSetNumTexGens( 0 );
		GXSetNumTevStages( 1 );
		GXSetTevOp( GX_TEVSTAGE0, GX_PASSCLR );
		GXSetTevOrder( GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0 );
		/* �l�p�|���S���`�� */
		GXSetChanMatColor( GX_COLOR0A0, sampleGroundCol );
		GXBegin( GX_QUADS, GX_VTXFMT1, 4 );
			GXPosition3s16( 0, 0, z );
			GXPosition3s16( (s16)scrWidth, 0, z );
			GXPosition3s16( (s16)scrWidth, (s16)scrHeight, z );
			GXPosition3s16( 0, (s16)scrHeight, z );
		GXEnd();
	}

	/* ������`�揀�� */
	{
		static const GXColor strCol = { 0x00, 0x00, 0x00, 0xff };
		Mtx		 scaleMtx;

		/* vertex �ݒ� */
		GXClearVtxDesc();
		GXSetVtxDesc( GX_VA_POS, GX_DIRECT );
		GXSetVtxDesc( GX_VA_TEX0, GX_DIRECT );
		GXSetVtxAttrFmt( GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0 );
		GXSetVtxAttrFmt( GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 0 );
		/* �e�N�X�`���ݒ� */
		GXSetNumTexGens( 1 );
		MTXScale( scaleMtx, ( 1.0f / sampleFont->sheetWidth ), ( 1.0f / sampleFont->sheetHeight ), 1.0f );
		GXLoadTexMtxImm( scaleMtx, GX_TEXMTX0, GX_MTX2x4 );
		GXSetTexCoordGen( GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0 );
		GXSetNumTevStages( 1 );
		GXSetTevOp( GX_TEVSTAGE0, GX_MODULATE );
		GXSetTevOrder( GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0 );
		GXSetChanMatColor( GX_COLOR0A0, strCol );
		GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColor( GX_TEVREG0, sampleTextCol );
	}
}

/*---------------------------------------------------------------------------*
  Name		: REXDEMOPrintf
  Description : ����������R�Ȉʒu�ɕ`��B
  Arguments   : x	  - X ���W
				y	  - Y ���W
				z	  - Z ���W
				fmt	- ����������
				...	- �ψ���
  Returns	 : �Ȃ�
 *---------------------------------------------------------------------------*/
void
REXDEMOPrintf( int x, int y, int z, const char *fmt, ... )
{
	/* ������������ꎞ�o�b�t�@�֕ϊ� */
	char	tmpbuf[256];
	va_list va;
	va_start(va, fmt);
	(void)vsprintf(tmpbuf, fmt, va);
	va_end(va);
	/* ������`�� */
	{
		const char* ptr = tmpbuf;
		int		 h;
		int		 ox = x;
		h = (sampleFontHeight > 0) ? sampleFontHeight :
			(int)( sampleRend.efbHeight / ( REXDEMO_RAW_COUNT - 1 ) );
		while( *ptr )
		{
			if( ( *ptr == '\r' ) || ( *ptr == '\n' ) )
			{
				y += h;

				ox = x;
				++ptr;
			}
			else
			{
				ox += REXDEMODrawChar( ox, y, z, -1, -1, &ptr );
			}
		}
	}
}

/*===========================================================================*/

/*---------------------------------------------------------------------------*
  Name		: InitString
  Description : ��������f�o�b�O�\�����邽�߂ɕK�v�ȏ������������s���B
  Arguments   : �Ȃ�
  Returns	 : �Ȃ�
 *---------------------------------------------------------------------------*/
static void
InitString( void )
{
	/* ������o�b�t�@������ */
	(void)memset( sampleString, 0, sizeof( sampleString ) );
	sampleRawIndex  = 0;
	sampleNextRawIndex  = 0;
	sampleNextColIndex  = 0;

	/* �t�H���g�f�[�^���[�h */
	(void)OSInitFont( sampleFont );

	/* �J���������� */
	{
		sampleCamera.fovy	   = 33.3f;
		sampleCamera.aspect	 = (f32)( 10.0f / 7.0f );
		sampleCamera.znear	  = 0.001f;
		sampleCamera.zfar	   = 65535.999f;
		sampleCamera.pos.x	  = 0.0f;
		sampleCamera.pos.y	  = 0.0f;
		sampleCamera.pos.z	  = -20.0f;
		sampleCamera.up.x	   = 0.0f;
		sampleCamera.up.y	   = 1.0f;
		sampleCamera.up.z	   = 0.0f;
		sampleCamera.target.x   = 0.0f;
		sampleCamera.target.y   = 0.0f;
		sampleCamera.target.z   = 0.0f;
	}

	/* ���C�g������ */
	{
		const Vec	   lpos	= { 0.0f, 0.0f, 0.0f };
		const Vec	   ldir	= { 0.0f, 0.0f, -1.0f };
	
		GXInitLightPosv( &sampleLight, &lpos );
		GXInitLightDirv( &sampleLight, &ldir );
		GXInitLightColor( &sampleLight, REXDEMO_COLOR_WHITE );
		GXInitLightSpot( &sampleLight, 0.03f, GX_SP_COS );
		GXInitLightDistAttn( &sampleLight, 3.0f, 0.5f, GX_DA_GENTLE );
	}
}

/*---------------------------------------------------------------------------*
  Name		: REXDEMODrawChar
  Description : �w����W��1���������`����s���B
  Arguments   : x	  - X ���W
				y	  - Y ���W
				z	  - Z ���W
				w	  - X ��
				h	  - Y ��
				string - �`�悷�镶�����w���|�C���^�̃|�C���^
  Returns	 : �`�悳�ꂽ X ��.
 *---------------------------------------------------------------------------*/
static int
REXDEMODrawChar( int x, int y, int z, int w, int h, const char **string )
{
	f32	 scrWidth	= (f32)( sampleRend.fbWidth );
	f32	 scrHeight   = (f32)( sampleRend.efbHeight );

	GXTexObj	tobj;
	void*	   image;
	s32		 fx, fy, fw;

	const char *ptr = *string;
	ptr = OSGetFontTexture( ptr, &image, &fx, &fy, &fw );
	*string = ptr;

	if (w < 0)
	{
		w = (sampleFontWidth > 0) ? sampleFontWidth :
			(int)( ( fw * scrWidth ) / ( 12 * REXDEMO_COL_COUNT ) );
	}
	if (h < 0)
	{
		h = (sampleFontHeight > 0) ? sampleFontHeight :
			(int)( scrHeight / ( REXDEMO_RAW_COUNT - 1 ) );
	}

	GXInitTexObj( &tobj, image, sampleFont->sheetWidth, sampleFont->sheetHeight, GX_TF_I4, GX_REPEAT, GX_REPEAT, GX_FALSE );
	GXLoadTexObj( &tobj, GX_TEXMAP0 );
	GXBegin( GX_QUADS, GX_VTXFMT0, 4 );
	{
		GXPosition3s16( (s16)( x + 0 ), (s16)( y + 0 ), (s16)z );
		GXTexCoord2s16( (s16)( fx +  0 ), (s16)( fy +  0 ) );
		GXPosition3s16( (s16)( x + w ), (s16)( y + 0 ), (s16)z );
		GXTexCoord2s16( (s16)( fx + fw ), (s16)( fy +  0 ) );
		GXPosition3s16( (s16)( x + w ), (s16)( y + h ), (s16)z );
		GXTexCoord2s16( (s16)( fx + fw ), (s16)( fy + 24 ) );
		GXPosition3s16( (s16)( x + 0 ), (s16)( y + h ), (s16)z );
		GXTexCoord2s16( (s16)( fx +  0 ), (s16)( fy + 24 ) );
	}
	GXEnd();
	return w;
}

/*---------------------------------------------------------------------------*
  Name		: RenderString
  Description : ��������f�o�b�O�\�����邽�߂ɖ��s�N�`���[�t���[���K�v��
				�����_�����O�������s���B
  Arguments   : �Ȃ�
  Returns	 : �Ȃ�
 *---------------------------------------------------------------------------*/
static void
RenderString( void )
{
	s32	 i, j;
	f32	 scrWidth	= (f32)( sampleRend.fbWidth );
	f32	 scrHeight   = (f32)( sampleRend.efbHeight );

#if ( REXDEMO_SHOW_FRAME_COUNT == 1 )
	/* �t���[���J�E���g�`�� */
	{
		s16		 x, y, z, w, h;
		char	tempStr[ 20 ];
		const char* ptr;

		(void)sprintf( tempStr, "%d", sampleFrameCount );
		GXSetChanMatColor( GX_COLOR0A0, REXDEMO_COLOR_PURPLE );
		for( i = 0; i < 20; i ++ )
		{
			if( tempStr[ i ] == 0x00 )
			{
				break;
			}
		}
		w   = 16;
		h   = 32;
		x   = (s16)( ( scrWidth - ( i * w ) ) - 4 );
		y   = 2;	/* �ʒu�͉�ʉE�� */
		z   = -1000;
		ptr = tempStr;
		for( j = 0; j < i; j ++ )
		{
			x   += REXDEMODrawChar( x, y, z, w, h, &ptr );
		}
	}
#endif

	/* ������o�b�t�@�`�� */
	for( i = 0; i < ( REXDEMO_RAW_COUNT - 1 ); i ++ )
	{
		s16		 x   = 2;
		s16		 y   = (s16)( ( scrHeight * i ) / ( REXDEMO_RAW_COUNT - 1 ) );
		s16		 z   = -1;
	
		const char* ptr	   = &( sampleString[ ( sampleRawIndex + i ) % REXDEMO_RAW_COUNT ][ 0 ] );
		const GXColor* strCol = &( sampleStringCol[ ( sampleRawIndex + i ) % REXDEMO_RAW_COUNT ][ 0 ] );
		char*	   oldPtr;

		for( j = 0; j < 256; j ++ )
		{
			if( ( *ptr == 0x00 ) || ( *ptr == 0x0d ) || ( *ptr == 0x0a ) )
			{
				break;
			}
			REXDEMOSetTextColor( *( (const GXColor*)strCol ) );
			oldPtr  = (char*)ptr;
			x   += REXDEMODrawChar( x, y, z, -1, -1, &ptr );
			strCol  += ( (u32)ptr - (u32)oldPtr );
			if( x >= scrWidth )
			{
				break;
			}
		}
	}
}

static void* RenderThread( void* arg )
{
#pragma unused(arg)
	while(TRUE)
	{
		REXDEMOWaitRetrace();
	}
}

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
