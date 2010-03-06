/*---------------------------------------------------------------------------*
  Project:  DWC

  Copyright 2005-2010 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.
 *---------------------------------------------------------------------------*/

#ifndef DWC_TRANSPORT_H_
#define DWC_TRANSPORT_H_


#ifdef  __cplusplus
extern "C"
{
#endif

//#define DWC_RELIABLE_IS_PRIVATE ///< reliable����M�@�\���O���Ɍ��J���Ȃ�

#define DWC_TRANSPORT_MTU_SIZE                      1500    ///< Reliable/Unreliable���M�ő��M�ł���ő�IP�p�P�b�g�T�C�Y
#define DWC_TRANSPORT_RELIABLE_ALL_HEADER_SIZE      35      ///< Reliable���M�ŕt�������w�b�_�T�C�Y
// IP�w�b�_(20byte) + UDP�w�b�_(8byte) + GameSpy�̃w�b�_(7byte)

#define DWC_TRANSPORT_UNRELIABLE_ALL_HEADER_SIZE    38      ///< Unreliable���M�ŕt�������w�b�_�T�C�Y
// IP�w�b�_(20byte) + UDP�w�b�_(8byte) + DWC�̃w�b�_(8byte) + GameSpy�̃w�b�_(2byte)

#define	DWC_TRANSPORT_SEND_RELIABLE_MAX         (DWC_TRANSPORT_MTU_SIZE-DWC_TRANSPORT_RELIABLE_ALL_HEADER_SIZE)     ///< Reliable���M�ň�x�ɑ��M�ł���ő�f�[�^�T�C�Y�B
#define	DWC_TRANSPORT_SEND_UNRELIABLE_MAX       (DWC_TRANSPORT_MTU_SIZE-DWC_TRANSPORT_UNRELIABLE_ALL_HEADER_SIZE)   ///< Unreliable���M�ň�x�ɑ��M�ł���ő�f�[�^�T�C�Y�B

#define DWC_TRANSPORT_SEND_MAX                  DWC_TRANSPORT_SEND_RELIABLE_MAX                                     ///< Reliable���M�ň�x�ɑ��M�ł���ő�f�[�^�T�C�Y�B(�݊����̂��߂Ɏc����Ă��܂��BDWC_TRANSPORT_SEND_RELIABLE_MAX���g�p���Ă��������B
    /** -----------------------------------------------------------------
      �֐���typedef
      -------------------------------------------------------------------*/

    /**
     * �f�[�^�̑��M�����������Ƃ��ɌĂяo�����R�[���o�b�N�֐��ł��B
     * 
     * DWC_SendReliable / DWC_SendReliableBitmap / DWC_SendUnreliable /
     * DWC_SendUnreliableBitmap �֐��ɂ��f�[�^�̑��M�������������ɌĂяo����܂�
     *
     * AID�̃r�b�g�}�b�v�ɂ�鑗�M���s�����ꍇ�́A1��AID�ɑ΂��鑗�M����������x
     * �ɁA�R�[���o�b�N���Ăяo����܂��B
     *
     * �{�R�[���o�b�N�֐���DWC_SetUserSendCallback�֐��ɂ���Đݒ肷�邱�Ƃ��ł���
     * ���B
     *
     * Param:    size    ���M�����f�[�^�̃T�C�Y
     * Param:    aid     ���M���AID
     * Param:    param   �R�[���o�b�N�p�p�����[�^
     *
     */
    typedef void	(*DWCUserSendCallback)( int size, u8 aid, void* param );

    /**
     * �f�[�^�̎�M���������Ƃ��ɌĂяo�����R�[���o�b�N�֐��ł��B
     * 
     * DWC_SendReliable / DWC_SendReliableBitmap / DWC_SendUnreliable /
     * DWC_SendUnreliableBitmap �֐��ɂ���đ����Ă����f�[�^����M�������ɌĂяo
     * ����܂��B
     *
     * �{�R�[���o�b�N�֐���DWC_SetUserRecvCallback�֐��ɂ���Đݒ肷�邱�Ƃ��ł���
     * ���B
     *
     * Param:    aid     ���M����AID
     * Param:    buffer  ��M�����f�[�^�ւ̃|�C���^
     * Param:    size    ��M�����f�[�^�̃T�C�Y
     * Param:    param   �R�[���o�b�N�p�p�����[�^
     *
     */
    typedef void	(*DWCUserRecvCallback)( u8 aid, u8* buffer, int size, void* param );

    /**
     * �����ԃf�[�^�̎�M���Ȃ��Ƃ��ɌĂяo�����R�[���o�b�N�֐��ł��B
     * 
     * DWC_SetRecvTimeoutTime�֐��Őݒ肵�����ԁA����̃z�X�g����f�[�^����M���Ȃ�
     * �������ɁA�Ăяo����܂��B
     *
     * �{�R�[���o�b�N�֐���DWC_SetUserRecvTimeoutCallback�֐��ɂ���Đݒ肷�邱�Ƃ�
     * �ł��܂��B
     *
     * Param:    aid ���M����AID
     * Param:    param �R�[���o�b�N�p�p�����[�^
     *
     * See also:   DWC_SetUserRecvTimeoutCallback
     * See also:   DWC_SetRecvTimeoutTime
     *
     */
    typedef void	(*DWCUserRecvTimeoutCallback)( u8 aid, void* param );

    /**
     * DWC_Ping�֐��ł̑��茋�ʂ�ʒm����R�[���o�b�N�֐��ł��B
     * 
     * DWC_Ping�֐��ł̑��茋�ʂ�ʒm����R�[���o�b�N�֐��ł��B
     * 
     * �{�R�[���o�b�N�֐���DWC_SetUserPingCallback�֐��ɂ���Đݒ肷�邱�Ƃ��ł���
     * ���B
     *
     * Param:   latency Ping�ɗv�����������ԁi�~���b�j
     * Param:   aid     Ping������s���������AID
     * Param:   param   �R�[���o�b�N�p�p�����[�^
     *
     */
    typedef void	(*DWCUserPingCallback)( int latency, u8 aid, void* param );


    /* -----------------------------------------------------------------
      �O���֐�
      -------------------------------------------------------------------*/
#ifndef DWC_RELIABLE_IS_PRIVATE
    BOOL	DWC_SendReliable				( u8 aid, const void* buffer, int size );		// Relible���M
    u32		DWC_SendReliableBitmap			( u32 bitmap, const void* buffer, int size );	// Bitmap�w��Relible���M
#endif
    BOOL	DWC_SendUnreliable				( u8 aid, const void* buffer, int size );				// Unreliable���M
    u32		DWC_SendUnreliableBitmap		( u32 bitmap, const void* buffer, int size );			// Bitmap�w��Unreliable���M
    BOOL	DWC_Ping						( u8 aid );												// Ping�l����
    BOOL	DWC_SetRecvBuffer				( u8 aid, void* recvBuffer, int size );					// ��M�o�b�t�@��ݒ�
    BOOL	DWC_SetUserSendCallback			( DWCUserSendCallback callback, void* param );			// ���M�R�[���o�b�N
    BOOL	DWC_SetUserRecvCallback			( DWCUserRecvCallback callback, void* param );			// ��M�R�[���o�b�N
    BOOL	DWC_SetUserRecvTimeoutCallback	( DWCUserRecvTimeoutCallback callback, void* param );	// ��M�^�C���A�E�g�R�[���o�b�N
    BOOL	DWC_SetRecvTimeoutTime			( u8 aid, u32 time );									// ��M�^�C���A�E�g���ԁi�P�ʃ~���b�j��ݒ�
    BOOL	DWC_SetUserPingCallback			( DWCUserPingCallback callback, void* param );			// Ping���芮���R�[���o�b�N
#ifndef DWC_RELIABLE_IS_PRIVATE
    BOOL	DWC_SetSendSplitMax				( u16 sendSplitMax );							// ���M�����ő�T�C�Y��ݒ�
    BOOL	DWC_IsSendableReliable			( u8 aid );										// Reliable���M�\����֐�
#endif
    BOOL    DWC_SetConnectionKeepAliveTime	( u32 time );									// �z�X�g�̃^�C���A�E�g���Ԃ̐ݒ�
    u32     DWC_GetConnectionKeepAliveTime	( void );										// �z�X�g�̃^�C���A�E�g���Ԃ̎擾

#ifndef	RVLDWC_FINAL

    BOOL	DWC_SetSendDelay				( u16 delay, u8 aid );							// ���M���ɒx����ݒ�
    BOOL	DWC_SetRecvDelay				( u16 delay, u8 aid );							// ��M���ɒx����ݒ�
    BOOL	DWC_SetSendDrop					( u8 drop, u8 aid );							// ���M���p�P�b�g���X����ݒ�
    BOOL	DWC_SetRecvDrop					( u8 drop, u8 aid );							// ��M���p�P�b�g���X����ݒ�

#else

#define	DWC_SetSendDelay( delay,  aid )	( (BOOL) 0 )
#define	DWC_SetRecvDelay( delay,  aid )	( (BOOL) 0 )
#define	DWC_SetSendDrop( drop, aid )	( (BOOL) 0 )
#define	DWC_SetRecvDrop( drop, aid )	( (BOOL) 0 )

#endif	// RVLDWC_FINAL


#ifdef  __cplusplus
}		/* extern "C" */
#endif


#endif // DWC_TRANSPORT_H_
