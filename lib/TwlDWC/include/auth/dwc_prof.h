/*---------------------------------------------------------------------------*
  Project:  TwlDWC

  Copyright 2005-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.
 *---------------------------------------------------------------------------*/

#ifndef DWC_PROF_H
#define DWC_PROF_H

#ifdef __cplusplus
extern "C"
{
#endif


    /* -------------------------------------------------------------------------
            define
       ------------------------------------------------------------------------- */

#define DWC_PROF_RESULT_STRING_LENGTH 51
#define DWC_PROF_MAX_WORDSNUM 50    // RVLDWC�Ƃ̌݊����̂���



    /* -------------------------------------------------------------------------
            enum
       ------------------------------------------------------------------------- */

    /**
     * �s���l�[���`�F�b�N���̃X�e�[�g
     *
     */
    typedef enum
    {
        DWC_PROF_STATE_NOT_INITIALIZED	= 0, ///< ��������
        DWC_PROF_STATE_OPERATING,		     ///< HTTP�ʐM��
        DWC_PROF_STATE_SUCCESS,				 ///< ���O�`�F�b�N����
        DWC_PROF_STATE_FAIL					 ///< ���O�`�F�b�N���s
    } DWCProfState;

    /**
     * �s��������`�F�b�N�̃��[�W�����������񋓌^
     *
     */
    typedef enum
    {
        DWC_PROF_REGION_SELF  = 0x00,   ///< ���i�R�[�h�ɏ]��
        DWC_PROF_REGION_JP    = 0x01,   ///< ���{
        DWC_PROF_REGION_US    = 0x02,   ///< �k��
        DWC_PROF_REGION_EU    = 0x04,   ///< ���B
        DWC_PROF_REGION_KR    = 0x08,   ///< �؍�
        DWC_PROF_REGION_CN    = 0x10,   ///< ����
        DWC_PROF_REGION_ALL   = 0x80    ///< �S���[�W����
    } DWCProfRegion;



    /* -------------------------------------------------------------------------
            function - external
       ------------------------------------------------------------------------- */

    BOOL DWC_CheckProfanityAsync(const u16 **words, int wordsnum, const char *reserved, int timeout, char *result, int *badwordsnum);
    BOOL DWC_CheckProfanityExAsync(const u16 **words, int wordsnum, const char *reserved, int timeout, char *result, int *badwordsnum, DWCProfRegion region);
    DWCProfState DWC_CheckProfanityProcess(void);



#ifdef __cplusplus
}
#endif

#endif
