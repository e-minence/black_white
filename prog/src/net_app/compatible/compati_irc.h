//==============================================================================
/**
 * @file	compati_irc.h
 * @brief	�ԊO���Ōq�����Ă��邩�̃w�b�_
 * @author	matsuda
 * @date	2009.02.13(��)
 */
//==============================================================================
#ifndef __COMPATI_IRC_H__
#define __COMPATI_IRC_H__


//==============================================================================
//	�萔��`
//==============================================================================
enum{
	COMPATIIRC_RESULT_FALSE,		///<�ڑ����Ă��Ȃ�
	COMPATIIRC_RESULT_CONNECT,		///<�ڑ����Ă���
	COMPATIIRC_RESULT_ERROR,		///<�G���[������
	COMPATIIRC_RESULT_EXIT,			///<�ʐM�V�X�e���I��
};

//==============================================================================
//	�O���֐��錾
//==============================================================================
extern void CompatiIrc_Init(COMPATI_IRC_SYS *ircsys, COMPATI_CONNECT_SYS *commsys);
extern BOOL CompatiIrc_Main(COMPATI_IRC_SYS *ircsys, COMPATI_CONNECT_SYS *commsys);
extern void CompatiIrc_Shoutdown(COMPATI_IRC_SYS *ircsys, COMPATI_CONNECT_SYS *commsys);


#endif	//__COMPATI_IRC_H__
