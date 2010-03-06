/*---------------------------------------------------------------------------*
  Project:  DWC

  Copyright 2005-2010 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.
 *---------------------------------------------------------------------------*/

#ifndef DWC_SSL_H_
#define DWC_SSL_H_

#ifdef __cplusplus
extern "C"
{
#endif


    /* -------------------------------------------------------------------------
            define
       ------------------------------------------------------------------------- */


    /* -------------------------------------------------------------------------
            enum
       ------------------------------------------------------------------------- */
    
    /// SSL�ʐM�̏�Ԃ������񋓌^�ł��B
    typedef enum
    {
        DWC_SSL_TYPE_SERVER_AUTH,           ///< �T�[�o�F�؂��s��SSL�ŒʐM���܂��B
        DWC_SSL_TYPE_SERVER_CLIENT_AUTH,    ///< �T�[�o�F�؂ƃN���C�A���g�F�؂��s��SSL�ŒʐM���܂��B��Nitro,Twl�ł͎g�p�ł��܂���B
        DWC_SSL_TYPE_NUM
    } DWCSSLType;


#ifdef __cplusplus
}
#endif

#endif // DWC_SSL_H_
