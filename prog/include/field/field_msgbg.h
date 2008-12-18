//======================================================================
/**
 * @file	field_msgbg.h
 * @brief	�t�B�[���h�@���b�Z�[�WBG�֘A
 * @authaor	kagaya
 * @data	2008.12.11
 */
//======================================================================
#ifndef __FLDMSGBG_H__
#define	__FLDMSGBG_H__
#include <gflib.h>
#include "system/gfl_use.h"
#include "print/printsys.h"
#include "print/gf_font.h"

//======================================================================
//	define
//======================================================================
//--------------------------------------------------------------
///	���j���[�߂�l
//--------------------------------------------------------------
#define FLDMENUFUNC_NULL (0xffffffff)		///<���ɖ���
#define FLDMENUFUNC_CANCEL (0xfffffffe)		///<�L�����Z��

//--------------------------------------------------------------
///	���j���[�X�L�b�v����
//--------------------------------------------------------------
#define FLDMENUFUNC_SKIP_NON	(0)			///<�X�L�b�v����
#define FLDMENUFUNC_SKIP_LRKEY	(1)			///<LR�L�[�X�L�b�v
#define FLDMENUFUNC_SKIP_LRBTN	(2)			///<LR�{�^���X�L�b�v

//======================================================================
//	struct
//======================================================================
typedef struct _TAG_FLDMSGBG FLDMSGBG;			///<FLDMSGBG
typedef struct _TAG_FLDMSGPRINT FLDMSGPRINT;	///<FLDMSGPRINT
typedef struct _TAG_FLDMSGWIN FLDMSGWIN;		///<FLDMSGWIN
typedef struct _TAG_FLDMENUFUNC FLDMENUFUNC;	///<FLDMENUFUNC

//--------------------------------------------------------------
///	FLDMENUFUNC_LISTDATA
//--------------------------------------------------------------
typedef struct
{
	const void *str;		///<�\�����镶����
	u32 param;				///<�I�����ꂽ�ۂɕԂ��p�����^
}FLDMENUFUNC_LISTDATA;

//--------------------------------------------------------------
///	FLDMENUFUNC_LIST
//--------------------------------------------------------------
typedef struct
{
	u32 str_id;				///<�\�����镶����ID
	void *selectParam;		///<�I�����ꂽ�ۂɕԂ��p�����^
}FLDMENUFUNC_LIST;

//--------------------------------------------------------------
///	FLDMENUFUNC_HEADER
//--------------------------------------------------------------
typedef struct
{
	u16		count;				//���X�g���ڐ�
	u16		line;				//�\���ő區�ڐ�
	u8		rabel_x;			//���x���\���w���W
	u8		data_x;				//���ڕ\���w���W
	u8		cursor_x;			//�J�[�\���\���w���W
	u8		line_y:4;			//�\���x���W
	u8		f_col:4;			//�\�������F
	u8		b_col:4;			//�\���w�i�F
	u8		s_col:4;			//�\�������e�F
	u16		msg_spc:3;			//�����Ԋu�w
	u16		line_spc:4;			//�����Ԋu�x
	u16		page_skip:2; //�y�[�W�X�L�b�v�^�C�v:FLDMENUFUNC_SKIP_NON��
	u16		font_size_x;		//�����T�C�YX(�h�b�g
	u16		font_size_y;		//�����T�C�YY(�h�b�g
	
	u16		bmppos_x;			//�\�����WX �L�����P��
	u16		bmppos_y;			//�\�����WY �L�����P��
	u16		bmpsize_x;			//�\���T�C�YX �L�����P��
	u16		bmpsize_y;			//�\���T�C�YY �L�����P��
}FLDMENUFUNC_HEADER;

//======================================================================
//	extern
//======================================================================
//���b�Z�[�WBG
extern FLDMSGBG * FLDMSGBG_Setup( HEAPID heapID );
extern void FLDMSGBG_Delete( FLDMSGBG *fmb );
extern void FLDMSGBG_PrintMain( FLDMSGBG *fmb );
extern void FLDMSGBG_ClearPrintQue( FLDMSGBG *fmb );
extern BOOL FLDMSGBG_CheckFinishPrint( FLDMSGBG *fmb );
extern PRINT_QUE * FLDMSGBG_GetPrintQue( FLDMSGBG *fmb );
extern GFL_MSGDATA * FLDMSGBG_CreateMSGDATA( FLDMSGBG *fmb, u32 arcDatIDMsg );

//���b�Z�[�W�\��
extern FLDMSGPRINT * FLDMSGPRINT_SetupPrint(
	FLDMSGBG *fmb, GFL_MSGDATA *msgData, GFL_BMPWIN *bmpwin );
extern void FLDMSGPRINT_Delete( FLDMSGPRINT *msgPrint );
extern void FLDMSGPRINT_Print(
	FLDMSGPRINT *msgPrint, u32 x, u32 y, u32 strID );
extern void FLDMSGPRINT_PrintStrBuf(
	FLDMSGPRINT *msgPrint, u32 x, u32 y, STRBUF *strBuf );
extern BOOL FLDMSGPRINT_CheckPrintTrans( FLDMSGPRINT *msgPrint );
extern void FLDMSGPRINT_ChangeBmpWin(
		FLDMSGPRINT *msgPrint, GFL_BMPWIN *bmpwin );
extern void FLDMSGPRINT_ClearBmp( FLDMSGPRINT *msgPrint );
extern PRINT_QUE * FLDMSGPRINT_GetPrintQue( FLDMSGPRINT *msgPrint );
extern GFL_FONT * FLDMSGPRINT_GetFontHandle( FLDMSGPRINT *msgPrint );
extern PRINT_UTIL * FLDMSGPRINT_GetPrintUtil( FLDMSGPRINT *msgPrint );
extern STRBUF * FLDMSGPRINT_GetStrBuf( FLDMSGPRINT *msgPrint );
extern GFL_MSGDATA * FLDMSGPRINT_GetMsgData( FLDMSGPRINT *msgPrint );

//���b�Z�[�W�E�B���h�E
extern FLDMSGWIN * FLDMSGWIN_Add( FLDMSGBG *fmb, GFL_MSGDATA *msgData,
	u16 bmppos_x, u16 bmppos_y, u16 bmpsize_x, u16 bmpsize_y );
extern void FLDMSGWIN_Delete( FLDMSGWIN *msgWin );
extern void FLDMSGWIN_Print( FLDMSGWIN *msgWin, u16 x, u16 y, u32 strID );
extern void FLDMSGWIN_PrintStrBuf( FLDMSGWIN *msgWin, u16 x, u16 y, STRBUF *strBuf );
extern BOOL FLDMSGWIN_CheckPrintTrans( FLDMSGWIN *msgWin );
extern void FLDMSGWIN_ClearWindow( FLDMSGWIN *msgWin );
extern FLDMSGWIN * FLDMSGWIN_AddTalkWin( FLDMSGBG *fmb, GFL_MSGDATA *msgData );

//���j���[
extern FLDMENUFUNC * FLDMENUFUNC_AddMenu(
	FLDMSGBG *fmb, GFL_MSGDATA *msgData,
	const FLDMENUFUNC_HEADER *pMenuHead,
	FLDMENUFUNC_LISTDATA *pMenuListData );
extern void FLDMENUFUNC_DeleteMenu( FLDMENUFUNC *menuFunc );
extern u32 FLDMENUFUNC_ProcMenu( FLDMENUFUNC *menuFunc );
extern FLDMENUFUNC_LISTDATA * FLDMENUFUNC_CreateListData( int max, HEAPID heapID );
extern void FLDMENUFUNC_DeleteListData( FLDMENUFUNC_LISTDATA *listData );
extern FLDMENUFUNC_LISTDATA * FLDMENUFUNC_CreateMakeListData(
	const FLDMENUFUNC_LIST *menuList,
	int max, GFL_MSGDATA *msgData, HEAPID heapID );
extern void FLDMENUFUNC_AddStringListData( FLDMENUFUNC_LISTDATA *listData,
		const STRBUF *strBuf, u32 param, HEAPID heapID  );
extern void FLDMENUFUNC_AddArcStringListData(
		FLDMENUFUNC_LISTDATA *listData,
		GFL_MSGDATA *msgData, u32 strID, u32 param, HEAPID heapID );
extern void FLDMENUFUNC_InputHeaderListSize(
	FLDMENUFUNC_HEADER *head, u16 count,
	u16 bmppos_x, u16 bmppos_y, u16 bmpsize_x, u16 bmpsize_y );

#endif //__FLDMSGBG_H__
