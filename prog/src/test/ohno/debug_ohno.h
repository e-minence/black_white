//============================================================================================
/**
 * @file	debug_ohno.h
 * @brief	�f�o�b�O�p�֐���`
 * @author	ohno
 * @date	2006.11.29
 */
//============================================================================================


#ifndef __DEBUG_OHNO_H__
#define __DEBUG_OHNO_H__


#include "map/dp3format.h"

typedef BOOL (*NetTestFunc)(void* pCtl);


//------------------------------------------------------------------
typedef struct {
  u32 debug_heap_id;
  GFL_PROCSYS * psys;
  NetTestFunc funcNet;
  int bMoveRecv;
}DEBUG_OHNO_CONTROL;


enum NetDebugOhnoCommand_e {
  NET_CMD_MOVE = GFL_NET_CMD_COMMAND_MAX,
  NET_CMD_TALK,
};


#define _MAXNUM   (4)         // �ő�ڑ��l��
#define _MAXSIZE  (84)        // �ő呗�M�o�C�g��
#define _BCON_GET_NUM (16)    // �ő�r�[�R�����W��

extern void DebugOhnoInit(HEAPID heap_id);
extern void DebugOhnoMain(void);
extern void DebugOhnoExit(void);


#endif //__DEBUG_OHNO_H__
