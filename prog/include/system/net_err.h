//==============================================================================
/**
 * @file	net_err.h
 * @brief	�ʐM�G���[��ʐ���̃w�b�_
 * @author	matsuda
 * @date	2008.11.17(��)
 */
//==============================================================================
#ifndef __NET_ERR_H__
#define __NET_ERR_H__


//==============================================================================
//	�萔��`
//==============================================================================
///�G���[��ʃV�X�e���̏�
typedef enum{
	NET_ERR_STATUS_NULL,			///<�����Ȃ�
	NET_ERR_STATUS_ERROR,			///<�G���[����
	NET_ERR_STATUS_REQ,				///<�G���[��ʌĂяo�����N�G�X�g������
}NET_ERR_STATUS;


//--------------------------------------------------------------
//  �g�p�q�[�v�T�C�Y
//--------------------------------------------------------------
///�L�����N�^VRAM�ޔ��T�C�Y
#define NETERR_PUSH_CHARVRAM_SIZE		(0x4000)
///�X�N���[��VRAM�ޔ��T�C�Y
#define NETERR_PUSH_SCRNVRAM_SIZE		(0x800)
///�p���b�gVRAM�ޔ��T�C�Y
#define NETERR_PUSH_PLTTVRAM_SIZE		(0x20)


//==============================================================================
//	�O���֐��錾
//==============================================================================

//--------------------------------------------------------------
//	�V�X�e�����Ŏg�p����֐�
//--------------------------------------------------------------
extern void NetErr_SystemInit(void);
extern void NetErr_SystemCreate(int heap_id);
extern void NetErr_SystemExit(void);
extern void NetErr_Main(void);
extern void NetErr_ErrorSet(void);
extern BOOL NetErr_DispCall(void);
extern void NetErr_DispCallPushPop(void);

extern void NetErr_GetTempArea( u8** charArea , u16** scrnArea , u16** plttArea );


//--------------------------------------------------------------
//	�A�v�����Ŏg�p����֐�
//--------------------------------------------------------------
extern BOOL NetErr_App_CheckError(void);
extern void NetErr_App_ReqErrorDisp(void);


//--------------------------------------------------------------
//  �f�o�b�O�p
//--------------------------------------------------------------
#if PM_DEBUG
extern void NetErr_DEBUG_ErrorSet(void);
#endif  //PM_DEBUG

#endif	//__NET_ERR_H__
