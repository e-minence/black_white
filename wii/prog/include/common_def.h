#ifdef __cplusplus
extern "C" {
#endif

#ifndef COMMON_DEF_H__
#define COMMON_DEF_H__
#include <revolution.h>
#include <revolution/mp.h>
#include <revolution/mpds.h>
#include <revolution/mem.h>
#include <revolution/enc.h>
#include "crc.h"

#define	 MY_NICKNAME		 L"Wii" // �q�@�̐e�@�I����ʂŕ\������j�b�N�l�[��
#define	 MY_NICKNAME_LENGTH  3	  // MY_NICKNAME �� UTF16BE �ł̕����� (�ő�10����)

#define	 MY_MAX_NODES	4   // �ڑ������s���q�@�䐔�̏�� (Wii �� Sequential �ʐM�ł͍ő�8��܂ŕۏ�)
#define	 MY_DS_NODES	 4  // DataSharing ���s���e�@���܂߂��m�[�h�� (NITRO-SDK �� dataShare-Model �f���ɍ��킹�Ă���)
								// �ʏ�̃A�v���P�[�V�����ł���� (MY_MAX_NODES+1)

//#define	 MY_DS_LENGTH	12  // DataSharing �ŋ��L����1�䂠����̃f�[�^�̃T�C�Y
#define	 MY_DS_LENGTH	0x30  // DataSharing �ŋ��L����1�䂠����̃f�[�^�̃T�C�Y
#define	 MY_DS_PORT	  13  // DataSharing �Ŏg�p����|�[�g�ԍ� (NITRO-SDK �� dataShare-Model �f���ɍ��킹�Ă���)

// DataSharing ���s���ꍇ�́A1�䂠����̋��L�T�C�Y�~���L�䐔�{4�o�C�g�̑��M�T�C�Y���K�v
#define	 MY_PARENT_MAX_SIZE ( (MY_DS_LENGTH * MY_DS_NODES) + MPDS_HEADER_SIZE )

#define	 STACK_SIZE	  8192 // �X���b�h�̃X�^�b�N�T�C�Y; ���荞�ݏ����Ŏg�p����镪���l���ɂ����K�v������
#define	 THREAD_PRIORITY 14   // �X���b�h�̗D��x; ���C���X���b�h (16) ��荂���D��x���w�肷��


// ���L�f�[�^�\����
// DS �ɍ��킹�� Little Endian �ŏ�������K�v�����邱�Ƃɒ���
#pragma pack(push,1)
typedef struct ShareData
{
	u8	data[MY_DS_LENGTH];
/*
	u8 data:3;			   // �O���t�p
	u8 level:2;			  // �d�g��M���x
	u8 command:3;			// �w�߁i�Q�[����Ԃ̈�Đ؂芷���ȂǂɎg�p�j
	u16 key;				 // �L�[�f�[�^
	u32 count:24;			// �t���[����
*/
}
ShareData;
#pragma pack(pop)

extern void EnterCriticalSection( void );
extern void LeaveCriticalSection( void );

extern void* mpAlloc( u32 size );
extern void mpFree( void* ptr );
//DS���玝���Ă����t�@�C���̌݊���ۂ���

extern MATHCRC16Table *crcTable;
typedef int HEAPID;
#define GFL_HEAPID_SYSTEM 0
#define MI_CpuCopy8(src,dest,size) NETMemCpy(dest,src,(u32)size)
#define MI_CpuFill8(src,dest,size) NETMemSet(src,dest,(u32)size)
#define GFL_STD_MemCopy(src,dest,size) NETMemCpy(dest,src,(u32)size)
#define GFL_STD_MemClear(src,size) NETMemSet(src,0,(u32)size)
#define OS_GetMacAddress NETGetWirelessMacAddress
//#define NET_PRINT(...) OSReport(__VA_ARGS__)
#define NET_PRINT(...) ((void)0)
#define OS_TPrintf OSReport 
#define GF_ASSERT ASSERT
#define OS_Panic OSHalt
#define GFL_HEAP_AllocMemory(work,size) mpAlloc((u32)size)
#define GFL_HEAP_FreeMemory mpFree

#define GFL_STD_CrcCalc(adr,len) MATH_CalcCRC16CCITT(crcTable,adr,len)

extern void* GFL_HEAP_AllocClearMemory(int work, int size);


#endif //COMMON_DEF_H__

#ifdef __cplusplus
} /* extern "C" */
#endif
