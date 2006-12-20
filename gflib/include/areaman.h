//==============================================================================
/**
 *
 *@file		areaman.h
 *@brief	VRAM�L�����f�[�^�̈�}�l�[�W��
 *@author	taya
 *@data		2006.10.27
 *
 */
//==============================================================================
#ifndef __AREAMAN_H__
#define __AREAMAN_H__

//#define AREAMAN_DEBUG
//--------------------------------------------
// �s���S�^��`
//--------------------------------------------
typedef struct _GFL_AREAMAN		GFL_AREAMAN;

//--------------------------------------------
// 
//--------------------------------------------
typedef	u32		GFL_AREAMAN_POS;

#define AREAMAN_POS_NOTFOUND	(0xffffffff)

//------------------------------------------------------------------
/**
 * �̈�}�l�[�W���쐬
 *
 * @param   maxBlock		[in] ���̃}�l�[�W���[���Ǘ�����u���b�N��	
 * @param   heapID			[in] �g�p�q�[�v�h�c
 *
 * @retval  GFL_AREAMAN*	�̈�}�l�[�W���|�C���^
 */
//------------------------------------------------------------------
extern GFL_AREAMAN*
	GFL_AREAMAN_Create
		( u32 maxBlock, u16 heapID );
//------------------------------------------------------------------
/**
 * �̈�}�l�[�W���j��
 *
 * @param   man			[in] �̈�}�l�[�W���|�C���^
 */
//------------------------------------------------------------------
extern void
	GFL_AREAMAN_Delete
		( GFL_AREAMAN* man );
//------------------------------------------------------------------
/**
 * �̈�擪���疖���܂ŁA�󂢂Ă��鏊��T���Ċm��
 *
 * @param   man				[in] �}�l�[�W��
 * @param   blockNum		[in] �m�ۂ������u���b�N��
 *
 * @retval  GFL_AREAMAN_POS		�m�ۂł����ʒu�i�ł��Ȃ���� AREAMAN_POS_NOTFOUND�j
 */
//------------------------------------------------------------------
extern u32
	GFL_AREAMAN_ReserveAuto
		( GFL_AREAMAN* man, u32 blockNum );
//------------------------------------------------------------------
/**
 * �̈�̎w��͈͓�����󂢂Ă��鏊��T���Ċm��
 *
 * @param   man				[in] �}�l�[�W��
 * @param   startBlock		[in] �T���J�n�u���b�N
 * @param   numBlockArea	[in] �T���u���b�N�͈�
 * @param   numBlockReserve	[in] �m�ۂ������u���b�N��
 *
 * @retval  GFL_AREAMAN_POS		�m�ۂł����ʒu�i�ł��Ȃ���� AREAMAN_POS_NOTFOUND�j
 */
//------------------------------------------------------------------
extern u32
	GFL_AREAMAN_ReserveAssignArea
		( GFL_AREAMAN* man, u32 startBlock, u32 numBlockArea, u32 numBlockReserve );
//------------------------------------------------------------------
/**
 * �̈�̎w��ʒu����m��
 *
 * @param   man				[in] �}�l�[�W��
 * @param   blockNum		[in] �m�ۂ������u���b�N��
 *
 * @retval  BOOL			TRUE�Ő���
 */
//------------------------------------------------------------------
extern BOOL
	GFL_AREAMAN_ReserveAssignPos
		( GFL_AREAMAN* man, GFL_AREAMAN_POS pos, u32 blockNum );
//------------------------------------------------------------------
/**
 * �̈���J��
 *
 * @param   man			[in] �}�l�[�W��
 * @param   pos			[in] �m�ۂ��Ă���ʒu
 * @param   blockNum	[in] �m�ۂ��Ă���u���b�N��
 */
//------------------------------------------------------------------
extern void 
	GFL_AREAMAN_Release
		( GFL_AREAMAN* man, GFL_AREAMAN_POS pos, u32 blockNum );

#ifdef AREAMAN_DEBUG
//------------------------------------------------------------------
/**
 * �f�o�b�O�o��
 *
 * @param   man		
 * @param   flag		
 */
//------------------------------------------------------------------
extern void
	GFL_AREAMAN_SetPrintDebug
		( GFL_AREAMAN* man, BOOL flag );
#endif

#endif	// __AREAMAN_H__
