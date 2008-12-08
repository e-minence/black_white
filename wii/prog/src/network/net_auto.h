//=============================================================================
/**
 * @file	net_auto.h
 * @brief	
 * @author	Katsumi Ohno
 * @date    2005.07.26
 */
//=============================================================================

#ifndef __NET_AUTO_H__
#define __NET_AUTO_H__


typedef void (*PSyncroCallback)(void* pWork);



//==============================================================================
/**
 * @brief   �����ŃT�[�o�𗧂��グ�� �F�ؓ��͂��ׂĂ��C��
 * @param   num     �ڑ������ɕK�v�Ȑl��
 * @param   num     �l�����̓�������ꂽ�Ƃ��Ă΂��R�[���o�b�N
 * @retval  none
 */
//==============================================================================

extern void GFL_NET_AutoInitServer(int num,PSyncroCallback pSyncroCallback);



#endif //__NET_AUTO_H__

