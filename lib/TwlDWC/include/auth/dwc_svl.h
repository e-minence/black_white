/*---------------------------------------------------------------------------*
  Project:  TwlDWC

  Copyright 2005-2010 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.
 *---------------------------------------------------------------------------*/

#ifndef DWC_SVL_H
#define DWC_SVL_H

#ifdef __cplusplus
extern "C"
{
#endif


/* -------------------------------------------------------------------------
        define
   ------------------------------------------------------------------------- */

#define DWC_SVL_ERROR_BASENUM		(-24000)    ///<
#define DWC_SVL_ERROR_HTTPBASENUM	(-25000)    ///<

#define DWC_SVL_ERROR_FATAL			(-100)      ///<
#define DWC_SVL_ERROR_BADDATA		(-101)      ///<

#define DWC_SVL_HOST_LENGTH		    64      ///< �z�X�g���̍ő啶����
#define DWC_SVL_TOKEN_LENGTH	    300     ///< �g�[�N���̍ő啶����
#define DWC_SVL_STATUSDATA_LENGTH	1       ///< �X�e�[�^�X�̃T�C�Y


/* -------------------------------------------------------------------------
        enum
   ------------------------------------------------------------------------- */

///  �T�[�r�X���P�[�^���擾���̃X�e�[�g�������񋓌^
typedef enum
{
    DWC_SVL_STATE_DIRTY	= 0,///< ��������
    DWC_SVL_STATE_IDLE,		///< �������s���ł͂Ȃ�
    DWC_SVL_STATE_HTTP,		///< HTTP�ʐM��
    DWC_SVL_STATE_SUCCESS,	///< �g�[�N���擾����
    DWC_SVL_STATE_ERROR,	///< �g�[�N���擾���s
    DWC_SVL_STATE_CANCELED,	///< �g�[�N���擾�L�����Z��
    DWC_SVL_STATE_MAX
} DWCSvlState;


/* -------------------------------------------------------------------------
        struct
   ------------------------------------------------------------------------- */

/**
 * �T�[�r�X���P�[�^���擾���ʂ��i�[����\����
 *
 * �T�[�r�X���P�[�^�����i�[���邽�߂̍\���̂ł��B 
 * 
 * See also:  DWC_SVLGetTokenAsync
 */
typedef struct
{
    BOOL	status;                             ///< �Ή�����T�[�o�̉ғ��󋵂ł��B
    ///< TRUE : �T�[�o�͉ғ����Ă��܂��B
    ///< FALSE : �T�[�o�͉ғ����Ă��܂���B
    ///�����̒l�͏����̊g���̂��߂ɗp�ӂ���Ă���A����͕K��TRUE���i�[����܂��B���̒l�͖������Ă��������B
    
    char    svlhost[DWC_SVL_HOST_LENGTH+1];     ///< �Ή�����T�[�o��URL�ł��B
    ///< �k�������I�[�����ő�64������ASCII������ł��B
    ///�����̒l�͏����̊g���̂��߂ɗp�ӂ���Ă��܂��B���̒l�͖������Ă��������B

    char    svltoken[DWC_SVL_TOKEN_LENGTH+1];   ///< �Ή�����T�[�o�����̔F�؃g�[�N���ł��B
    ///< �k�������I�[�����ő�300������ASCII������ł��B
}
DWCSvlResult;



/* -------------------------------------------------------------------------
        function - external
   ------------------------------------------------------------------------- */

BOOL        DWC_SVLGetTokenAsync(char *svl, DWCSvlResult *result);
DWCSvlState DWC_SVLProcess(void);
void        DWC_SVLAbort(void);




#ifdef __cplusplus
}
#endif

#endif
