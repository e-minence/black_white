//======================================================================
/**
 * @file  field_msgbg.h
 * @brief �t�B�[���h�@���b�Z�[�WBG�֘A
 * @authaor kagaya
 * @data  2008.12.11
 */
//======================================================================
#ifndef __FLDMSGBG_H__
#define __FLDMSGBG_H__
#include <gflib.h>
#include "system/gfl_use.h"
#include "print/printsys.h"
#include "print/gf_font.h"
#include "system/talkmsgwin.h"
#include "system/bmp_menulist.h"

//======================================================================
//  define
//======================================================================
//--------------------------------------------------------------
/// ���j���[�߂�l
//--------------------------------------------------------------
#define FLDMENUFUNC_NULL (0xffffffff)   ///<���ɖ���
#define FLDMENUFUNC_CANCEL (0xfffffffe)   ///<�L�����Z��

//--------------------------------------------------------------
/// ���j���[�X�L�b�v����
//--------------------------------------------------------------
typedef enum
{
  FLDMENUFUNC_SKIP_NON =    (0),      ///<�X�L�b�v����
  FLDMENUFUNC_SKIP_LRKEY =  (1),      ///<LR�L�[�X�L�b�v
  FLDMENUFUNC_SKIP_LRBTN =  (2),      ///<LR�{�^���X�L�b�v
}FLDMENUFUNC_SKIP;

//--------------------------------------------------------------
/// �͂��A������
//--------------------------------------------------------------
typedef enum
{
  FLDMENUFUNC_YESNO_YES = 0, ///<�͂�
  FLDMENUFUNC_YESNO_NO, ///<������
  
  FLDMENUFUNC_YESNO_NULL, ///<�I��
}FLDMENUFUNC_YESNO;

//--------------------------------------------------------------
/// FLDTALKMSGWIN_IDX �����o���E�B���h�E�C���f�b�N�X
//--------------------------------------------------------------
typedef enum
{
  FLDTALKMSGWIN_IDX_UPPER = 0, ///<��ʏ㕔��
  FLDTALKMSGWIN_IDX_LOWER, ///<��ʉ�����
  FLDTALKMSGWIN_IDX_AUTO, ///<�����o�����̍��W���l���������Ŕz�u
  FLDTALKMSGWIN_IDX_SUBWIN0, ///<�T�u�E�B���h�E
  FLDTALKMSGWIN_IDX_SUBWIN1, ///<�T�u�E�B���h�E
  FLDTALKMSGWIN_IDX_SUBWIN2, ///<�T�u�E�B���h�E
  FLDTALKMSGWIN_IDX_SUBWIN3, ///<�T�u�E�B���h�E
  FLDTALKMSGWIN_IDX_SUBWIN4, ///<�T�u�E�B���h�E
  FLDTALKMSGWIN_IDX_SUBWIN5, ///<�T�u�E�B���h�E
  FLDTALKMSGWIN_IDX_SUBWIN6, ///<�T�u�E�B���h�E
  FLDTALKMSGWIN_IDX_SUBWIN7, ///<�T�u�E�B���h�E
}FLDTALKMSGWIN_IDX;

//--------------------------------------------------------------
/// �T�u�E�B���h�E
//--------------------------------------------------------------
#define FLDSUBMSGWIN_MAX (8) ///<�T�u�E�B���h�E�ő吔
#define FLDMSGBG_STRLEN_SUBWIN (64) ///<�T�u�E�B���h�E�p�����񒷂�

//--------------------------------------------------------------
/// FLDBGWIN BG�^�C�v
//--------------------------------------------------------------
typedef enum
{
  FLDBGWIN_TYPE_INFO = 0,
  FLDBGWIN_TYPE_TOWN,
  FLDBGWIN_TYPE_POST,
  FLDBGWIN_TYPE_ROAD,
  FLDBGWIN_TYPE_MAX,
}FLDBGWIN_TYPE;

//--------------------------------------------------------------
/// FLDSPWIN �E�B���h�E�^�C�v
//--------------------------------------------------------------
typedef enum
{
  FLDSPWIN_TYPE_LETTER,
  FLDSPWIN_TYPE_BOOK,
  FLDSPWIN_TYPE_MAX,
}FLDSPWIN_TYPE;

//======================================================================
//  struct
//======================================================================
typedef struct _TAG_FLDMSGBG FLDMSGBG;      ///<FLDMSGBG
typedef struct _TAG_FLDMSGPRINT FLDMSGPRINT;  ///<FLDMSGPRINT
typedef struct _TAG_FLDMSGWIN FLDMSGWIN;    ///<FLDMSGWIN
typedef struct _TAG_FLDSYSWIN FLDSYSWIN;    ///<FLDSYSWIN
typedef struct _TAG_FLDMSGPRINT_STREAM FLDMSGPRINT_STREAM; ///<FLDMSGPRINT_STREAM
typedef struct _TAG_FLDMSGWIN_STREAM FLDMSGWIN_STREAM; ///<FLDMSGWIN_STREAM
typedef struct _TAG_FLDSYSWIN_STREAM FLDSYSWIN_STREAM; ///<FLDSYSWIN_STREAM
typedef struct _TAG_FLDMENUFUNC FLDMENUFUNC;  ///<FLDMENUFUNC
typedef struct _TAG_FLDTALKMSGWIN FLDTALKMSGWIN; ///FLDTALKMSGWIN
typedef struct _TAG_FLDPLAINMSGWIN FLDPLAINMSGWIN; ///<FLDPLAINMSGWIN
typedef struct _TAG_FLDSUBMSGWIN FLDSUBMSGWIN; ///<FLDSUBMSGWIN
typedef struct _TAG_FLDBGWIN FLDBGWIN; ///<FLDBGWIN
typedef struct _TAG_FLDSPWIN FLDSPWIN; ///<FLDSPWIN

//--------------------------------------------------------------
/// FLDMENUFUNC_LISTDATA
//--------------------------------------------------------------
typedef struct
{
  const void *str;    ///<�\�����镶����
  u32 param;        ///<�I�����ꂽ�ۂɕԂ��p�����^
}FLDMENUFUNC_LISTDATA;

//--------------------------------------------------------------
/// FLDMENUFUNC_LIST
//--------------------------------------------------------------
typedef struct
{
  u32 str_id;       ///<�\�����镶����ID
  void *selectParam;    ///<�I�����ꂽ�ۂɕԂ��p�����^
}FLDMENUFUNC_LIST;

//--------------------------------------------------------------
/// FLDMENUFUNC_HEADER
//--------------------------------------------------------------
typedef struct
{
  u16   count;        //���X�g���ڐ�
  u16   line;       //�\���ő區�ڐ�
  u8    rabel_x;      //���x���\���w���W
  u8    data_x;       //���ڕ\���w���W
  u8    cursor_x;     //�J�[�\���\���w���W
  u8    line_y:4;     //�\���x���W
  u8    f_col:4;      //�\�������F
  u8    b_col:4;      //�\���w�i�F
  u8    s_col:4;      //�\�������e�F
  u16   msg_spc:3;      //�����Ԋu�w
  u16   line_spc:4;     //�����Ԋu�x
  FLDMENUFUNC_SKIP page_skip:2; //�y�[�W�X�L�b�v�^�C�v
  u16   font_size_x;    //�����T�C�YX(�h�b�g
  u16   font_size_y;    //�����T�C�YY(�h�b�g
  
  u16   bmppos_x;     //�\�����WX �L�����P��
  u16   bmppos_y;     //�\�����WY �L�����P��
  u16   bmpsize_x;      //�\���T�C�YX �L�����P��
  u16   bmpsize_y;      //�\���T�C�YY �L�����P��
	//���\�����Ƃ̃R�[���o�b�N�֐�
	BMPMENULIST_PRINT_CALLBACK icon;
}FLDMENUFUNC_HEADER;

//======================================================================
//  extern
//======================================================================
//���b�Z�[�WBG
extern FLDMSGBG * FLDMSGBG_Create( HEAPID heapID, GFL_G3D_CAMERA *g3Dcamera );
extern void FLDMSGBG_SetupResource( FLDMSGBG *fmb );
extern void FLDMSGBG_Delete( FLDMSGBG *fmb );
extern void FLDMSGBG_ReleaseBGResouce( FLDMSGBG *fmb );
extern void FLDMSGBG_ResetBGResource( FLDMSGBG *fmb );
extern void FLDMSGBG_PrintMain( FLDMSGBG *fmb );
extern void FLDMSGBG_PrintG3D( FLDMSGBG *fmb );
extern void FLDMSGBG_ClearPrintQue( FLDMSGBG *fmb );
extern void FLDMSGBG_AllPrint( FLDMSGBG *fmb );
extern BOOL FLDMSGBG_CheckFinishPrint( FLDMSGBG *fmb );
extern PRINT_QUE * FLDMSGBG_GetPrintQue( FLDMSGBG *fmb );
extern GFL_FONT * FLDMSGBG_GetFontHandle( FLDMSGBG *fmb );
extern GFL_MSGDATA * FLDMSGBG_CreateMSGDATA( FLDMSGBG *fmb, u32 arcDatIDMsg );
extern void FLDMSGBG_DeleteMSGDATA( GFL_MSGDATA *msgData );

//���b�Z�[�W�\��
extern FLDMSGPRINT * FLDMSGPRINT_SetupPrint(
  FLDMSGBG *fmb, GFL_MSGDATA *msgData, GFL_BMPWIN *bmpwin );
extern void FLDMSGPRINT_Delete( FLDMSGPRINT *msgPrint );
extern void FLDMSGPRINT_Print(
  FLDMSGPRINT *msgPrint, u32 x, u32 y, u32 strID );
extern void FLDMSGPRINT_PrintColor(
    FLDMSGPRINT *msgPrint, u32 x, u32 y, u32 strID, PRINTSYS_LSB color );
extern void FLDMSGPRINT_PrintStrBuf(
  FLDMSGPRINT *msgPrint, u32 x, u32 y, STRBUF *strBuf );
extern void FLDMSGPRINT_PrintStrBufColorFontHandle( FLDMSGPRINT *msgPrint,
    u32 x, u32 y, STRBUF *strBuf, PRINTSYS_LSB color, GFL_FONT *fontHandle );
extern void FLDMSGPRINT_PrintStrBufColor( FLDMSGPRINT *msgPrint,
    u32 x, u32 y, STRBUF *strBuf, PRINTSYS_LSB color );
extern BOOL FLDMSGPRINT_CheckPrintTrans( FLDMSGPRINT *msgPrint );
extern void FLDMSGPRINT_ChangeBmpWin(
    FLDMSGPRINT *msgPrint, GFL_BMPWIN *bmpwin );
extern void FLDMSGPRINT_ClearBmp( FLDMSGPRINT *msgPrint );
extern void FLDMSGPRINT_FillClearBmp(
    FLDMSGPRINT *msgPrint, u32 x, u32 y, u32 size_x, u32 size_y );
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
extern void FLDMSGWIN_PrintStrBufColor( FLDMSGWIN *msgWin,
    u16 x, u16 y, STRBUF *strBuf, PRINTSYS_LSB color );
extern void FLDMSGWIN_PrintStrBufColorFontHandle( 
  FLDMSGWIN *msgWin, u16 x, u16 y, STRBUF *strBuf, PRINTSYS_LSB color, GFL_FONT *font_handle );
extern BOOL FLDMSGWIN_CheckPrintTrans( FLDMSGWIN *msgWin );
extern void FLDMSGWIN_ClearWindow( FLDMSGWIN *msgWin );
extern void FLDMSGWIN_FillClearWindow(
    FLDMSGWIN *msgWin, u32 x, u32 y, u32 size_x, u32 size_y );
extern FLDMSGWIN * FLDMSGWIN_AddTalkWin( FLDMSGBG *fmb, GFL_MSGDATA *msgData );

//�V�X�e���E�B���h�E
extern FLDSYSWIN * FLDSYSWIN_Add(
    FLDMSGBG *fmb, GFL_MSGDATA *msgData, u16 bmppos_y );
extern void FLDSYSWIN_Delete( FLDSYSWIN *msgWin );
extern void FLDSYSWIN_Print( FLDSYSWIN *msgWin, u16 x, u16 y, u32 strID );
extern void FLDSYSWIN_PrintStrBuf( FLDSYSWIN *msgWin, u16 x, u16 y, STRBUF *strBuf );
extern void FLDSYSWIN_PrintStrBufColor( FLDSYSWIN *sysWin,
    u16 x, u16 y, STRBUF *strBuf, PRINTSYS_LSB color );
extern BOOL FLDSYSWIN_CheckPrintTrans( FLDSYSWIN *msgWin );
extern void FLDSYSWIN_ClearWindow( FLDSYSWIN *msgWin );
extern void FLDSYSWIN_FillClearWindow(
    FLDSYSWIN *msgWin, u32 x, u32 y, u32 size_x, u32 size_y );
extern FLDSYSWIN * FLDSYSWIN_AddTalkWin( FLDMSGBG *fmb, GFL_MSGDATA *msgData );
extern void FLDMSGBG_TransMsgWindowFrame( HEAPID heapID, int bgFrame );


//���j���[
extern FLDMENUFUNC * FLDMENUFUNC_AddMenuListEx( FLDMSGBG *fmb,
	const FLDMENUFUNC_HEADER *pMenuHead,
	FLDMENUFUNC_LISTDATA *pMenuListData,
  u16 list_pos, u16 cursor_pos,
  BMPMENULIST_CURSOR_CALLBACK callback,
  BMPMENULIST_PRINT_CALLBACK icon,
  void *work );
extern FLDMENUFUNC * FLDMENUFUNC_AddMenuList( FLDMSGBG *fmb,
  const FLDMENUFUNC_HEADER *pMenuHead,
  FLDMENUFUNC_LISTDATA *pMenuListData,
  u16 list_pos, u16 cursor_pos );
extern FLDMENUFUNC * FLDMENUFUNC_AddMenu( FLDMSGBG *fmb,
  const FLDMENUFUNC_HEADER *pMenuHead,
  FLDMENUFUNC_LISTDATA *pMenuListData );
extern FLDMENUFUNC * FLDMENUFUNC_AddEventMenuList( FLDMSGBG *fmb,
	const FLDMENUFUNC_HEADER *pMenuHead,
	FLDMENUFUNC_LISTDATA *pMenuListData,
  BMPMENULIST_CURSOR_CALLBACK callback, void* cb_work,
  u16 list_pos, u16 cursor_pos, BOOL cancel );
extern void FLDMENUFUNC_DeleteMenu( FLDMENUFUNC *menuFunc );
extern u32 FLDMENUFUNC_ProcMenu( FLDMENUFUNC *menuFunc );
extern void FLDMENUFUNC_Rewrite( FLDMENUFUNC *menuFunc );
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
extern void FLDMENUFUNC_ListSTRBUFDelete(FLDMENUFUNC_LISTDATA *listData);
extern u32 FLDMENUFUNC_GetListLengthMax( const FLDMENUFUNC_LISTDATA *listData );
extern u32 FLDMENUFUNC_GetListMax( const FLDMENUFUNC_LISTDATA *listData );
extern u32 FLDMENUFUNC_GetListMenuWidth(
    const FLDMENUFUNC_LISTDATA *listData, u32 font_size, u32 font_space );
extern u32 FLDMENUFUNC_GetListMenuHeight(
    const FLDMENUFUNC_LISTDATA *listData, u32 font_size, u32 font_space );
extern u32 FLDMENUFUNC_GetListMenuLen( u32 num, u32 font_size, u32 space );
extern void FLDMENUFUNC_InputHeaderListSize(
  FLDMENUFUNC_HEADER *head, u16 count,
  u16 bmppos_x, u16 bmppos_y, u16 bmpsize_x, u16 bmpsize_y );

//�͂��A�������I�����j���[
extern FLDMENUFUNC * FLDMENUFUNC_AddYesNoMenu(
    FLDMSGBG *fmb, FLDMENUFUNC_YESNO pos );
extern FLDMENUFUNC_YESNO FLDMENUFUNC_ProcYesNoMenu( FLDMENUFUNC *menuFunc );

//�v�����g�X�g���[��
extern FLDMSGPRINT_STREAM * FLDMSGPRINT_STREAM_SetupPrintColor(
  FLDMSGBG *fmb, const STRBUF *strbuf,
  GFL_BMPWIN *bmpwin, u16 x, u16 y, int wait, u16 n_color );
extern FLDMSGPRINT_STREAM * FLDMSGPRINT_STREAM_SetupPrint(
  FLDMSGBG *fmb, const STRBUF *strbuf,
  GFL_BMPWIN *bmpwin, u16 x, u16 y, int wait );
extern void FLDMSGPRINT_STREAM_Delete( FLDMSGPRINT_STREAM *stm );
extern BOOL FLDMSGPRINT_STREAM_ProcPrint( FLDMSGPRINT_STREAM *stm );

//���b�Z�[�W�E�B���h�E�@�v�����g�X�g���[��
extern FLDMSGWIN_STREAM * FLDMSGWIN_STREAM_Add(
    FLDMSGBG *fmb, const GFL_MSGDATA *msgData,
    u16 bmppos_x, u16 bmppos_y, u16 bmpsize_x, u16 bmpsize_y );
extern void FLDMSGWIN_STREAM_Delete( FLDMSGWIN_STREAM *msgWin );
extern void FLDMSGWIN_STREAM_PrintStart(
    FLDMSGWIN_STREAM *msgWin, u16 x, u16 y, u32 strID );
extern void FLDMSGWIN_STREAM_PrintStrBufStart(
    FLDMSGWIN_STREAM *msgWin, u16 x, u16 y, const STRBUF *strBuf );
extern BOOL FLDMSGWIN_STREAM_Print( FLDMSGWIN_STREAM *msgWin );
extern FLDMSGWIN_STREAM * FLDMSGWIN_STREAM_AddTalkWin(
    FLDMSGBG *fmb, GFL_MSGDATA *msgData );
extern void FLDMSGWIN_STREAM_ClearMessage( FLDMSGWIN_STREAM *msgWin );
extern void FLDMSGWIN_STREAM_ClearWindow( FLDMSGWIN_STREAM *msgWin );
extern void FLDMSGWIN_STREAM_WriteWindow( FLDMSGWIN_STREAM *msgWin );
extern void FLDMSGWIN_STREAM_AllPrintStrBuf(
    FLDMSGWIN_STREAM *msgWin, u16 x, u16 y, STRBUF *strBuf );
extern BOOL FLDMSGWIN_STREAM_CheckAllPrintTrans( FLDMSGWIN_STREAM *msgWin );

//�V�X�e���E�B���h�E�@�v�����g�X�g���[��
extern FLDSYSWIN_STREAM * FLDSYSWIN_STREAM_Add(
    FLDMSGBG *fmb, const GFL_MSGDATA *msgData, u16 bmppos_y );
extern void FLDSYSWIN_STREAM_Delete( FLDSYSWIN_STREAM *sysWin );
extern void FLDSYSWIN_STREAM_PrintStart(
    FLDSYSWIN_STREAM *sysWin, u16 x, u16 y, u32 strID );
extern void FLDSYSWIN_STREAM_PrintStrBufStart(
    FLDSYSWIN_STREAM *sysWin, u16 x, u16 y, const STRBUF *strBuf );
extern BOOL FLDSYSWIN_STREAM_Print( FLDSYSWIN_STREAM *sysWin );
extern FLDSYSWIN_STREAM * FLDSYSWIN_STREAM_AddTalkWin(
    FLDMSGBG *fmb, GFL_MSGDATA *msgData );
extern void FLDSYSWIN_STREAM_ClearMessage( FLDSYSWIN_STREAM *sysWin );
extern void FLDSYSWIN_STREAM_ClearWindow( FLDSYSWIN_STREAM *sysWin );
extern void FLDSYSWIN_STREAM_WriteWindow( FLDSYSWIN_STREAM *sysWin );
extern void FLDSYSWIN_STREAM_AllPrintStrBuf(
    FLDSYSWIN_STREAM *sysWin, u16 x, u16 y, STRBUF *strBuf );
extern BOOL FLDSYSWIN_STREAM_CheckAllPrintTrans( FLDSYSWIN_STREAM *sysWin );

//�����o�����b�Z�[�W�E�B���h�E
extern FLDTALKMSGWIN * FLDTALKMSGWIN_Add( FLDMSGBG *fmb,
    FLDTALKMSGWIN_IDX idx, const VecFx32 *pos,
    const GFL_MSGDATA *msgData, u32 msgID, TALKMSGWIN_TYPE type );
extern FLDTALKMSGWIN * FLDTALKMSGWIN_AddStrBuf( FLDMSGBG *fmb,
    FLDTALKMSGWIN_IDX idx, const VecFx32 *pos,
    STRBUF *strBuf, TALKMSGWIN_TYPE type );
extern void FLDTALKMSGWIN_StartClose( FLDTALKMSGWIN *tmsg );
extern BOOL FLDTALKMSGWIN_WaitClose( FLDTALKMSGWIN *tmsg );
extern void FLDTALKMSGWIN_Delete( FLDTALKMSGWIN *tmsg );
extern BOOL FLDTALKMSGWIN_Print( FLDTALKMSGWIN *tmsg );

//�v���[���E�B���h�E
extern FLDPLAINMSGWIN * FLDPLAINMSGWIN_Add(
    FLDMSGBG *fmb,  const GFL_MSGDATA *msgData,
    u16 bmppos_x, u16 bmppos_y, u16 bmpsize_x, u16 bmpsize_y );
extern void FLDPLAINMSGWIN_Delete( FLDPLAINMSGWIN *plnwin );
extern void FLDPLAINMSGWIN_ClearMessage( FLDPLAINMSGWIN *plnwin );
extern void FLDPLAINMSGWIN_ClearWindow( FLDPLAINMSGWIN *plnwin );
extern void FLDPLAINMSGWIN_WriteWindow( FLDPLAINMSGWIN *plnwin );
extern void FLDPLAINMSGWIN_Print(
    FLDPLAINMSGWIN *plnwin, u16 x, u16 y, u32 strID );
extern void FLDPLAINMSGWIN_PrintStrBuf(
    FLDPLAINMSGWIN *plnwin, u16 x, u16 y, STRBUF *strBuf );
extern void FLDPLAINMSGWIN_PrintStrBufColor(
    FLDPLAINMSGWIN *plnwin, 
    u16 x, u16 y, STRBUF *strBuf, PRINTSYS_LSB color );
extern void FLDPLAINMSGWIN_PrintStreamStart(
    FLDPLAINMSGWIN *plnwin, u16 x, u16 y, u32 strID );
extern void FLDPLAINMSGWIN_PrintStreamStartStrBuf(
    FLDPLAINMSGWIN *plnwin, u16 x, u16 y, const STRBUF *strBuf );
extern BOOL FLDPLAINMSGWIN_PrintStream( FLDPLAINMSGWIN *plnwin );

//�T�u�E�B���h�E
extern void FLDSUBMSGWIN_Add( FLDMSGBG *fmb,
    const GFL_MSGDATA *msgData, u32 msgID,
    int id, u8 x, u8 y, u8 sx, u8 sy );
extern void FLDSUBMSGWIN_AddStrBuf( FLDMSGBG *fmb,
    const STRBUF *strBuf, int id, u8 x, u8 y, u8 sx, u8 sy );
extern void FLDSUBMSGWIN_Delete( FLDMSGBG *fmb, int id );
extern void FLDSUBMSGWIN_DeleteAll( FLDMSGBG *fmb );
extern BOOL FLDSUBMSGWIN_CheckExistWindow( FLDMSGBG *fmb );

//BG�E�B���h�E
extern FLDBGWIN * FLDBGWIN_Add( FLDMSGBG *fmb, FLDBGWIN_TYPE type );
extern void FLDBGWIN_Delete( FLDBGWIN *bgWin );
extern BOOL FLDBGWIN_PrintStrBuf( FLDBGWIN *bgWin, const STRBUF *strBuf );

//����E�B���h�E
extern FLDSPWIN * FLDSPWIN_Add( FLDMSGBG *fmb, FLDSPWIN_TYPE type,
  u16 bmppos_x, u16 bmppos_y, u16 bmpsize_x, u16 bmpsize_y );
extern void FLDSPWIN_Delete( FLDSPWIN *spWin );
extern void FLDSPWIN_PrintStrBufStart(
    FLDSPWIN *spWin, u16 x, u16 y, const STRBUF *strBuf );
extern BOOL FLDSPWIN_Print( FLDSPWIN *spWin );
extern u32 FLDSPWIN_GetNeedWindowWidthCharaSize(
    FLDMSGBG *fmb, const STRBUF *strbuf, u32 margin );
extern u32 FLDSPWIN_GetNeedWindowHeightCharaSize(
    FLDMSGBG *fmb, const STRBUF *strbuf, u32 margin );

//���̑�
extern void FLDMSGBG_SetBlendAlpha( BOOL set );

extern void FLDMSGBG_RecoveryBG( FLDMSGBG *fmb );

#endif //__FLDMSGBG_H__
