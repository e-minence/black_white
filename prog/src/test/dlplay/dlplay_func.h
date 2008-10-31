//======================================================================
/**
 *
 * @file	dlplay_func.h
 * @brief	ダウンロード用機能関数
 * @author	ariizumi
 * @data	08/10/20
 */
//======================================================================

#ifndef DLPLAY_FUNC_H__
#define DLPLAY_FUNC_H__

#define DLPLAY_FUNC_USE_PRINT (1)

typedef struct _DLPLAY_MSG_SYS DLPLAY_MSG_SYS;

enum DLPLAY_BG_MSG
{
	MSG_SEARCH_CHILD,
	MSG_FIND_CHILD,
	MSG_WAIT_CHILD_CONNECT,
	MSG_CONNECT_PARENT,
	MSG_CONNECTED_CHILD,
	MSG_WAIT_CHILD_PROC,
	MSG_CONNECTED_PARENT,
	MSG_WAIT_LOAD,
	MSG_WAIT_SEND_DATA,
	MSG_SEND_DATA_PARENT,
	MSG_POST_DATA_CHILD,
	MSG_SELECT_BOX,
	MSG_SAVE,
	MSG_SAVE_END,

	DLPLAY_MSG_MAX,
};

extern DLPLAY_MSG_SYS*	DLPlayFunc_MsgInit( int	heapID , u8 bgPlane );
extern void	DLPlayFunc_MsgTerm( DLPLAY_MSG_SYS *msgSys );

extern void DLPlayFunc_PutString( char* str , DLPLAY_MSG_SYS *msgSys);
extern void DLPlayFunc_PutStringLine( u8 line , char* str , DLPLAY_MSG_SYS *msgSys );
extern void DLPlayFunc_PutStringLineDiv( u8 line , char* str1 , char* str2 , DLPLAY_MSG_SYS *msgSys );
extern void DLPlayFunc_ClearString( DLPLAY_MSG_SYS *msgSys );
extern void DLPlayFunc_MsgUpdate( DLPLAY_MSG_SYS *msgSys , const u8 line , const BOOL isRefresh );

extern void DLPlayFunc_ChangeBgMsg( u8 msgIdx , u8 plane );

extern const u16 DLPlayFunc_DPTStrCode_To_UTF16( const u16 *dptStr , u16 *utfStr , const u16 len );
#endif //DLPLAY_FUNC_H__

