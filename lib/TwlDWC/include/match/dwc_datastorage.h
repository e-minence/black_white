/*---------------------------------------------------------------------------*
  Project:  DWC

  Copyright 2005-2010 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.
 *---------------------------------------------------------------------------*/

#ifndef DWC_DATASTORAGE_H_
#define DWC_DATASTORAGE_H_


#ifdef __cplusplus
extern "C"
{
#endif

//----------------------------------------------------------------------------
// enum
//----------------------------------------------------------------------------

    /// Persistent�T�[�o���O�C�����
    enum
    {
        DWC_PERS_STATE_INIT = 0,   ///< �������
        DWC_PERS_STATE_LOGIN,      ///< ���O�C����
        DWC_PERS_STATE_CONNECTED,  ///< �ڑ����
        DWC_PERS_STATE_NUM
    };

//----------------------------------------------------------------------------
// typedef - function
//----------------------------------------------------------------------------

    /**
     * �f�[�^�X�g���[�W�T�[�o�E���O�C���R�[���o�b�N�^
     *
     * �f�[�^�X�g���[�W�T�[�o�ւ̃��O�C����������������ƌĂяo����܂��B
     * 
     * �{�R�[���o�b�N�֐���DWC_LoginToStorageServerAsync�֐��ɂ���Đݒ肷�邱�Ƃ��ł��܂��B 
     * 
     * DWC_LogoutFromStorageServer()���R�[���o�b�N���ŌĂяo���Ȃ��ł��������B
     *
     * Param:    error   DWC�G���[��ʁB���e�̏ڍׂ́ADWC_GetLastErrorEx���Q�Ƃ��Ă��������B
     * Param:    param   DWC_LoginToStorageServerAsync�֐��Ŏw�肵���R�[���o�b�N�p�p�����[�^
     *
     */
    typedef void (*DWCStorageLoginCallback)(DWCError error, void* param);

    /**
     * �f�[�^�X�g���[�W�T�[�o�E�Z�[�u�����R�[���o�b�N�^
     *
     * �f�[�^�X�g���[�W�ւ̃Z�[�u�����������Ƃ��ɌĂяo����܂��B
     *
     * �{�R�[���o�b�N�֐���DWC_SetStorageServerCallback�֐��ɂ���Đݒ肷�邱�Ƃ��ł��܂��B 
     * 
     * DWC_LogoutFromStorageServer()���R�[���o�b�N���ŌĂяo���Ȃ��ł��������B
     *
     * Param:    success     TRUE:�Z�[�u����
     *                      FALSE:�Z�[�u���s
     * Param:    isPublic    TRUE:�Z�[�u�����f�[�^���p�u���b�N�f�[�^
     *                      FALSE:�v���C�x�[�g�f�[�^
     *	                    success = FALSE �̏ꍇ���L���Ȓl�������Ă��܂��B
     * Param:    param       DWC_SavePublicDataAsync / DWC_SavePrivateDataAsync�֐��Ŏw�肵���R�[���o�b�N�p�p�����[�^
     *
     */
    typedef void (*DWCSaveToServerCallback)(BOOL success, BOOL isPublic, void* param);

    /**
     * �f�[�^�X�g���[�W�T�[�o�E���[�h�����R�[���o�b�N�^
     *
     * ���[�h�֐��Ŏw�肵���L�[�����݂��Ȃ������ꍇ��A���l��
     * DWC_SavePrivateDataAsync()�ŃZ�[�u�����L�[��ǂ����Ƃ����ꍇ��
     * success��TRUE�Adata��""(�󕶎���)�Alen��0�ɂȂ�܂��B
     *	
     * �{�R�[���o�b�N�֐���DWC_SetStorageServerCallback�֐��ɂ���Đݒ肷�邱�Ƃ��ł��܂��B 
     *
     * DWC_LogoutFromStorageServer()���R�[���o�b�N���ŌĂяo���Ȃ��ł��������B
     *
     * Param:    success TRUE:�ʐM����
     *                  FALSE:�ʐM�G���[
     * Param:    index   ���[�h���f�[�^�ێ��҂̗F�B���X�g�C���f�b�N�X
     *                  �����̏ꍇ�ƁA���[�h�����O�ɗF�B�łȂ��Ȃ����ꍇ��-1�ɂȂ�B
     * Param:    data    ���[�h�����f�[�^�ikey/value�g�̕�����j
     * Param:    len     ���[�h�f�[�^��
     * Param:    param   DWC_LoadOwnPrivateDataAsync / DWC_LoadOthersDataAsync�֐��Ŏw�肵���R�[���o�b�N�p�p�����[�^
     *
     */
    typedef void (*DWCLoadFromServerCallback)(BOOL success, int index, char* data, int len, void* param);

//----------------------------------------------------------------------------
// struct
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// function - external
//----------------------------------------------------------------------------

    BOOL    DWC_LoginToStorageServerAsync(DWCStorageLoginCallback callback, void* param );
    void    DWC_LogoutFromStorageServer ( void );
    BOOL    DWC_SetStorageServerCallback( DWCSaveToServerCallback saveCallback, DWCLoadFromServerCallback loadCallback );
    BOOL    DWC_SavePublicDataAsync     ( char* keyvalues, void* param );
    BOOL    DWC_SavePrivateDataAsync    ( char* keyvalues, void* param );
    BOOL    DWC_LoadOwnPublicDataAsync  ( char* keys, void* param );
    BOOL    DWC_LoadOwnPrivateDataAsync ( char* keys, void* param );
    BOOL    DWC_LoadOthersDataAsync     ( char* keys, int index, void* param );

#ifdef __cplusplus
}
#endif


#endif // DWC_DATASTORAGE_H_
