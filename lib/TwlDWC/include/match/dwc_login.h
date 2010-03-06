/*---------------------------------------------------------------------------*
  Project:  DWC

  Copyright 2005-2010 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/

#ifndef DWC_LOGIN_H_
#define DWC_LOGIN_H_


#ifdef __cplusplus
extern "C"
{
#endif


//----------------------------------------------------------------------------
// enum
//----------------------------------------------------------------------------

/// ���O�C���i�s�󋵗񋓎q
    typedef enum {
        DWC_LOGIN_STATE_INIT = 0,         ///< �������
        DWC_LOGIN_STATE_REMOTE_AUTH,      ///< �����[�g�F�ؒ�
        DWC_LOGIN_STATE_CONNECTING,       ///< GP�T�[�o�ڑ���
        DWC_LOGIN_STATE_GPGETINFO,        ///< GP��login�����Ƃ���lastname�擾��
        DWC_LOGIN_STATE_GPSETINFO,        ///< GP�ɏ��߂�login�����Ƃ���lastname�ݒ�
        DWC_LOGIN_STATE_CONNECTED,        ///< �R�l�N�g�������
        DWC_LOGIN_STATE_NUM
    } DWCLoginState;

//----------------------------------------------------------------------------
// typedef - function
//----------------------------------------------------------------------------

    /**
     * ���O�C�������R�[���o�b�N�^
     *
     * DWC_LoginAsync�֐��ŊJ�n�������O�C����������������ƌĂяo����܂��B
     *
     * Param:    error       DWC�G���[��ʁB���e�̏ڍׂ́ADWC_GetLastErrorEx���Q�Ƃ��Ă��������B
     * Param:    profileID   �擾�ł���������GS�v���t�@�C��ID
     * Param:    param       DWC_LoginAsync�֐��Ŏw�肵���R�[���o�b�N�p�p�����[�^
     *
     * See also:   DWC_LoginAsync
     *
     */
    typedef void (*DWCLoginCallback)(DWCError error, int profileID, void *param);

    void    DWC_AllowMovedUserData(BOOL allowMovedUserData);

#ifdef __cplusplus
}
#endif


#endif // DWC_LOGIN_H_
