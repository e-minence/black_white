//============================================================================================
/**
 * @file	game_net.h
 * @brief	通信動作用関数
 */
//============================================================================================
extern void		InitGameNet(void);
extern BOOL		ConnectGameNet(void);
extern void		MainGameNet(void);
extern BOOL		ExitGameNet(void);
extern void		SendGameNet( int comm, int size,void* commWork );
extern NetID	GetNetID(void);

//#define NET_WORK_ON

