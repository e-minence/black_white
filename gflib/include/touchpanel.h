//*****************************************************************************
/**
 *
 *@file		touchpanel.h
 *@brief	�^�b�`�p�l���f�[�^����
 *@author	tomoya takahashi   >  katsumi ohno
 *@data		2005.03.16
 */
//*****************************************************************************

#ifndef __TOUCHPANEL_H__
#define	__TOUCHPANEL_H__

#undef	GLOBAL
#ifdef	__TOUCHPANEL_H_GLOBAL__
#define	GLOBAL	/* */
#else
#define	GLOBAL	extern
#endif

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
 */
//-----------------------------------------------------------------------------

#define		TP_HIT_END		(0xff)			// �e�[�u���I���R�[�h
#define		TP_USE_CIRCLE	(0xfe)			// �~�`�Ƃ��Ďg��
#define		TP_HIT_NONE		(-1)	// ���������e�[�u���Ȃ�

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
 */
//-----------------------------------------------------------------------------
//-------------------------------------
//	��`�f�[�^�\����
//=====================================
typedef union{
	struct {
		u8	top;	// ��i�������͓���R�[�h�j
		u8	bottom;	// ��
		u8	left;	// ��
		u8	right;	// �E
	}rect;

	struct {
		u8	code;	// TP_USE_CIRCLE ���w��
		u8	x;
		u8	y;
		u8	r;
	}circle;

}RECT_HIT_TBL, TP_HIT_TBL;

/*
 * �g�p��
 *
 * RECT_HIT_TBL rect_data[] ={
 *		{0,64,0,128},				// �z��v�f�D�揇��:0>1>2>3>4>.....�ł�
 *		{128,191,0,64},
 *		{32,94,129,200},
 *		{0,191,0,255},
 *      {TP_USE_CIRCLE, 100, 80, 16 },	// �~�`�Ƃ��Ďg���B
 *		{TP_HIT_END,0,0,0},		// �I���f�[�^
 * };
 * 
 */

//------------------------------------------------------------------
/**
 * @brief	TPSYS �^�錾
 *
 * TCB�V�X�e�����[�N�\���̂̐錾�B
 * ���C�������p�EVBlank�p�ȂǁA�C�ӂ̉ӏ��Ŏ��s�����
 * ������TCB�V�X�e�����쐬���邱�Ƃ��o����B
 *
 * ���e�͉B������Ă���A���ڂ̃A�N�Z�X�͂ł��Ȃ��B
  */
//------------------------------------------------------------------
typedef struct _TPSYS		TPSYS;


//----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
 */
//----------------------------------------------------------------------------

GLOBAL int GF_TP_HitCont( const TPSYS* tpsys, const TP_HIT_TBL *tbl );
GLOBAL int GF_TP_HitTrg( const TPSYS* tpsys, const TP_HIT_TBL *tbl );

GLOBAL BOOL GF_TP_GetCont( const TPSYS* tpsys );
GLOBAL BOOL GF_TP_GetTrg( const TPSYS* tpsys );

GLOBAL int GF_TP_HitSelf( const TP_HIT_TBL *tbl, u32 x, u32 y );

GLOBAL BOOL GF_TP_GetPointCont( const TPSYS* tpsys, u32* x, u32* y );
GLOBAL BOOL GF_TP_GetPointTrg( const TPSYS* tpsys, u32* x, u32* y );


#undef	GLOBAL
#endif

