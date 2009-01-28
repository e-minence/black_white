//==============================================================================
/**
 * @file	huge_beacon.h
 * @brief	�r�[�R�����g�p���ċ���f�[�^�𑗎�M�̃w�b�_
 * @author	matsuda
 * @date	2009.01.23(��)
 */
//==============================================================================
#ifndef __HUGE_BEACON_H__
#define __HUGE_BEACON_H__

//==============================================================================
//	�萔��`
//==============================================================================
#define GGID_HUGE_DATA		(0x589)	//GGID�F�r�[�R������f�[�^����M�e�X�g�p


//==============================================================================
//	�O���֐��錾
//==============================================================================
extern void HUGEBEACON_SystemCreate(int heap_id, u32 send_data_size, const void *send_data, void *receive_buf);
extern void HUGEBEACON_SystemExit(void);
extern void HUGEBEACON_Start(void);
extern void HUGEBEACON_Main(void);

#endif	//__HUGE_BEACON_H__
