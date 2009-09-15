/*---------------------------------------------------------------------------*
  Project:  TwlDWC

  Copyright 2005-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.
 *---------------------------------------------------------------------------*/

#ifndef DWC_NASLOGIN_H_
#define DWC_NASLOGIN_H_

#ifdef __cplusplus
extern "C"
{
#endif

    /* -------------------------------------------------------------------------
            enum
       ------------------------------------------------------------------------- */

    /**
     * ingamesn�`�F�b�N���ʗ񋓎q
     */
    typedef enum
    {
        DWC_INGAMESN_NOT_CHECKED	= 0,    ///< ���`�F�b�N
        DWC_INGAMESN_VALID			= 1,    ///< �L��
        DWC_INGAMESN_INVALID		= 2     ///< ����
    } DWCIngamesnCheckResult;

    /**
     *  �C�V���F�؃T�[�o�ւ̔F�؏����̏󋵂������񋓌^�ł��B
     */
    typedef enum
    {
        DWC_NASLOGIN_STATE_DIRTY	= 0,	///< �F�؏���������������Ă��܂���B
        DWC_NASLOGIN_STATE_IDLE,			///< �F�؏������J�n����Ă��܂���B�ҋ@��Ԃł��B
        DWC_NASLOGIN_STATE_HTTP,			///< �F�؏����̂��߂�HTTP�ʐM�����s���ł��B
        DWC_NASLOGIN_STATE_SUCCESS,		    ///< �F�؏������������܂����B
        DWC_NASLOGIN_STATE_ERROR,		    ///< �F�؏��������s���܂����B
        DWC_NASLOGIN_STATE_CANCELED,	    ///< �F�؏������L�����Z������܂����B
        DWC_NASLOGIN_STATE_MAX
    } DWCNasLoginState;



    /* -------------------------------------------------------------------------
            function - external
       ------------------------------------------------------------------------- */

    DWCIngamesnCheckResult DWC_GetIngamesnCheckResult(void);

    BOOL DWC_GetDateTime(RTCDate *date, RTCTime *time);

    BOOL                DWC_NASLoginAsync(void);
    DWCNasLoginState    DWC_NASLoginProcess(void);
    void                DWC_NASLoginAbort(void);



#ifdef __cplusplus
}
#endif

#endif // DWC_NASLOGIN_H_
