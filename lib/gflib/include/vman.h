//==============================================================================
/**
 *
 *@file		vman.h
 *@brief	OBJ�pVRAM�̈�Ǘ�
 *@author	taya
 *@data		2006.11.28
 *
 */
//==============================================================================
#ifndef __VMAN_H__
#define __VMAN_H__

#ifdef __cplusplus
extern "C" {
#endif

//==============================================================
//	�Ǘ��I�u�W�F�N�g�s���S�^��`
//==============================================================
typedef struct _GFL_VMAN	GFL_VMAN;

//==============================================================
//	�Ǘ��^�C�v�萔
//==============================================================
typedef enum {
	GFL_VMAN_TYPE_BG,
	GFL_VMAN_TYPE_OBJ,
}GFL_VMAN_TYPE;

//==============================================================
//	�̈�\�񎞂̏���ۑ����Ă������߂̍\���́B
//	�̈����̍ۂɕK�v�ƂȂ�B
//
//	�\���̓��ɊȒP�ɑg�ݍ��߂�悤�Ƀ����o���J���Ă��邪�A
//	�����𑀍삷�邱�Ƃ͋֎~�B
//==============================================================
typedef struct {
	u32   pos;
	u32   size;
}GFL_VMAN_RESERVE_INFO;


extern u32 GFI_VRAM_CalcVramSize( u32 vramBank );
extern void GFL_VMAN_sysInit( void );
extern void GFL_VMAN_sysExit( void );
extern GFL_VMAN* GFL_VMAN_Create( u16 heapID, GFL_VMAN_TYPE type, u32 vramBank );
extern void GFL_VMAN_Delete( GFL_VMAN* man );
extern void GFL_VMAN_InitReserveInfo( GFL_VMAN_RESERVE_INFO* reserveInfo );
extern BOOL GFL_VMAN_IsReserveInfoInitialized( const GFL_VMAN_RESERVE_INFO* reserveInfo );
extern BOOL GFL_VMAN_Reserve( GFL_VMAN* man, u32 byteSize, GFL_VMAN_RESERVE_INFO* reserveInfo );
extern BOOL GFL_VMAN_ReserveAssignArea( GFL_VMAN* man, u32 reserveSize, u32 startOffset, u32 areaSize, GFL_VMAN_RESERVE_INFO* reserveInfo );
extern BOOL GFL_VMAN_ReserveFixPos( GFL_VMAN* man, u32 reserveSize, u32 offset, GFL_VMAN_RESERVE_INFO* reserveInfo );
extern void GFL_VMAN_Release( GFL_VMAN* man, GFL_VMAN_RESERVE_INFO* reserveInfo );
extern u32 GFL_VMAN_GetByteOffset( GFL_VMAN* man, const GFL_VMAN_RESERVE_INFO* reserveInfo );


#ifdef PM_DEBUG
extern void GFL_VMAN_SetPrintDebug( GFL_VMAN* man, BOOL flag );
#endif


#ifdef __cplusplus
}/* extern "C" */
#endif

#endif	/* #ifndef __VMAN_H__ */

