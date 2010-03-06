/*---------------------------------------------------------------------------*
  Project:  DWC

  Copyright 2005-2010 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.
 *---------------------------------------------------------------------------*/

#ifndef DWC_RNK_H_
#define DWC_RNK_H_

#ifdef __cplusplus
extern "C"
{
#endif

    /* -------------------------------------------------------------------------
            define
       ------------------------------------------------------------------------- */

    /**
     *  ���[�U�[��`�f�[�^�Ɏg�p�ł���ő�T�C�Y
     *
     */
#define DWC_RNK_DATA_MAX		764

    /**
     *  �J�e�S���[�Ƃ��Ďw��ł���ő�l
     *
     */
#define DWC_RNK_CATEGORY_MAX	1000

/// �擾���郉���L���O���X�g�̏����ƍ~���������萔
    enum
    {
        DWC_RNK_ORDER_ASC	=	0,	///< ����
        DWC_RNK_ORDER_DES	=	1	///< �~��
    };

    /**
     *  �擾���郉���L���O���X�g�̍ő吔�̏���l
     *
     */
#define DWC_RNK_GET_MAX         30

    /**
     *  �F�B�̍ő吔
     *
     */
#define DWC_RNK_FRIENDS_MAX		64


    /* -------------------------------------------------------------------------
            enum
       ------------------------------------------------------------------------- */

    /**
     *  �G���[�R�[�h
     *
     */
    typedef enum
    {

        DWC_RNK_SUCCESS = 0,						///< ����
        DWC_RNK_IN_ERROR,							///< �G���[������

        DWC_RNK_ERROR_INVALID_PARAMETER,			///< �s���ȃp�����[�^�[

        //
        // DWC_RnkInitialize �̃G���[�l
        //
        DWC_RNK_ERROR_INIT_ALREADYINITIALIZED,		///< ��������
        DWC_RNK_ERROR_INIT_INVALID_INITDATASIZE,	///< �s���ȏ������f�[�^�T�C�Y
        DWC_RNK_ERROR_INIT_INVALID_INITDATA,		///< �s���ȏ������f�[�^
        DWC_RNK_ERROR_INIT_INVALID_USERDATA,		///< ���[�U�[��񂪕s��

        //
        // DWC_RnkPutScoreAsync �̃G���[�l
        //
        DWC_RNK_ERROR_PUT_NOTREADY,					///< ����������Ă��Ȃ�
        DWC_RNK_ERROR_PUT_INVALID_KEY,				///< �Í����L�[���s��
        DWC_RNK_ERROR_PUT_NOMEMORY,					///< �������[�s��

        //
        // DWC_RnkGetScoreAsync �̃G���[�l
        //
        DWC_RNK_ERROR_GET_NOTREADY,					///< ����������Ă��Ȃ�
        DWC_RNK_ERROR_GET_INVALID_KEY,				///< �Í����L�[���s��
        DWC_RNK_ERROR_GET_NOMEMORY,					///< �������[�s��

        //
        // DWC_RnkCancelProcess �̃G���[�l
        //
        DWC_RNK_ERROR_CANCEL_NOTASK,				///< �񓯊��������̏����Ȃ�

        //
        // DWC_RnkProcess �̃G���[�l
        //
        DWC_RNK_PROCESS_NOTASK,				        ///< �񓯊��������̏����Ȃ�
        DWC_RNK_PROCESS_TIMEOUT,                    ///< �^�C���A�E�g

        //
        // DWC_RnkGetResponse �̃G���[�l
        //
        DWC_RNK_ERROR_INVALID_MODE,					///< �s���ȃ��[�h
        DWC_RNK_ERROR_NOTCOMPLETED,					///< �ʐM������
        DWC_RNK_ERROR_EMPTY_RESPONSE				///< ��̃��X�|���X


    } DWCRnkError;


    /**
     * ���W���[���X�e�[�^�X
     *
     */
    typedef enum
    {

        DWC_RNK_STATE_NOTREADY	= 0,	///< �����������

        DWC_RNK_STATE_INITIALIZED,		///< ����������
        DWC_RNK_STATE_PUT_ASYNC,		///< PUT�񓯊�������
        DWC_RNK_STATE_GET_ASYNC,		///< GET�񓯊�������
        DWC_RNK_STATE_COMPLETED,		///< ����

        DWC_RNK_STATE_TIMEOUT,			///< �^�C���A�E�g
        DWC_RNK_STATE_ERROR				///< �G���[����

    } DWCRnkState;


    /**
     * ���[�W�����R�[�h
     *
     */
    typedef enum
    {

        DWC_RNK_REGION_JP	= 0x01,	///< Japan
        DWC_RNK_REGION_US	= 0x02,	///< United States
        DWC_RNK_REGION_EU	= 0x04,	///< Europe
        DWC_RNK_REGION_KR	= 0x08,	///< Korea
        DWC_RNK_REGION_CH	= 0x10,	///< China(����`)
        DWC_RNK_REGION_CN	= 0x10,	///< China

        DWC_RNK_REGION_ALL	= 0xff	///< World

    } DWCRnkRegion;


    /**
     * DWC_RnkGetScoreAsync �擾���[�h
     *
     */
    typedef enum
    {

        DWC_RNK_GET_MODE_ORDER,		///< ����
        DWC_RNK_GET_MODE_TOPLIST,	///< �n�C�X�R�A
        DWC_RNK_GET_MODE_NEAR,		///< ���ӃX�R�A
        DWC_RNK_GET_MODE_FRIENDS,	///< �F�B�w�胉���L���O
        DWC_RNK_GET_MODE_NEAR_HI,	///< ���ӃX�R�A(�����������ʂ�����)
        DWC_RNK_GET_MODE_NEAR_LOW	///< ���ӃX�R�A(�����������ʂ��Ⴂ)

    } DWCRnkGetMode;


    /* -------------------------------------------------------------------------
            typedef - function
       ------------------------------------------------------------------------- */


    /* -------------------------------------------------------------------------
            struct
       ------------------------------------------------------------------------- */

    /**
     * �X�R�A���i�[�p�\����
     */
    typedef struct
    {

        u32					order;		///< ����
        s32					pid;		///< GameSpy Profile ID
        s32					score;		///< �X�R�A
        DWCRnkRegion		region;		///< ���[�W�����R�[�h
        u32					lastupdate;	///< �O��̍X�V����̌o�ߎ���(��)

        u32					size;		///< userdata�̃T�C�Y(bytes)
        void*				userdata;	///< ���[�U�[��`�f�[�^�̃o�b�t�@

    }
    DWCRnkData;

    /**
     * ���ʎ擾���[�h�Ŏw�肷��p�����[�^
     *
     * ���ʎ擾���[�h(DWC_RNK_GET_MODE_ORDER)�Ŏ擾����Ƃ��̃p�����[�^���w�肵�܂��B
     * DWCRnkGetParam�̃����o�ł��B
     *
     * See also:  DWCRnkGetParam
     *
     */
    typedef struct
    {

        u32	sort;							///< DWC_RNK_ORDER_ASC:����
        ///< DWC_RNK_ORDER_DES:�~��
        u32	since;							///< �����O���炩

    }
    DWCRnkGetParam_order;

    /**
     * �g�b�v�����L���O���X�g�擾���[�h�Ŏw�肷��p�����[�^
     *
     * �g�b�v�����L���O���X�g�擾���[�h(DWC_RNK_GET_MODE_TOPLIST)�Ŏ擾����Ƃ��̃p
     * �����[�^���w�肵�܂��B
     * DWCRnkGetParam�̃����o�ł��B
     *
     * See also:  DWCRnkGetParam
     *
     */
    typedef struct
    {

        u32	sort;							///< DWC_RNK_ORDER_ASC:����
        ///< DWC_RNK_ORDER_DES:�~��
        u32	limit;							///< �擾����ő吔(2�`30(DWC_RNK_GET_MAX))
        u32	since;							///< �����O���炩

    }
    DWCRnkGetParam_toplist;

    /**
     * ���Ӄ����L���O���X�g�擾���[�h�Ŏw�肷��p�����[�^
     *
     * ���Ӄ����L���O���X�g�擾���[�h(DWC_RNK_GET_MODE_NEAR)�Ŏ擾����Ƃ��̃p����
     * �[�^���w�肵�܂��B
     * DWCRnkGetParam�̃����o�ł��B
     *
     * See also:  DWCRnkGetParam
     *
     */
    typedef struct
    {

        u32	sort;							///< DWC_RNK_ORDER_ASC:����
        ///< DWC_RNK_ORDER_DES:�~��
        u32	limit;							///< �擾����ő吔(2�`30(DWC_RNK_GET_MAX))
        u32	since;							///< �����O���炩

    }
    DWCRnkGetParam_nearby;

    /**
     * �F�B�����L���O���X�g�擾���[�h�Ŏw�肷��p�����[�^
     *
     * �F�B�����L���O���X�g�擾���[�h(DWC_RNK_GET_MODE_FRIENDS)�Ŏ擾����Ƃ��̃p����
     * �[�^���w�肵�܂��B
     * DWCRnkGetParam�̃����o�ł��B
     *
     * See also:  DWCRnkGetParam
     *
     */
    typedef struct
    {

        u32	sort;							///< DWC_RNK_ORDER_ASC:����
        ///< DWC_RNK_ORDER_DES:�~��
        u32	limit;							///< �擾����ő吔(2�`65(DWC_RNK_FRIENDS_MAX+1))
        u32	since;							///< �����O���炩
        s32	friends[DWC_RNK_FRIENDS_MAX];	///< �F�B��PID���X�g

    }
    DWCRnkGetParam_friends;

    /**
     * DWC_RnkGetScoreAsync�Ŏw�肷��p�����[�^
     *
     * DWC_RnkGetScoreAsync�ŃX�R�A���擾����Ƃ��Ɏw�肷��p�����[�^�ł��B
     * DWC_RnkGetScoreAsync�Ŏw�肷��X�R�A�擾���[�h�ɉ����ēK�؂ȃp�����[�^��ݒ�
     * ���Ă��������B
     *
     * See also:  DWC_RnkGetScoreAsync
     *
     */
    typedef struct
    {

        u32				size;	///< byte�P�ʂ̃T�C�Y�B
        ///< �擾���[�h�ɉ������p�����[�^�̃T�C�Y���w�肵�܂��B
        ///< ��F���ʎ擾���[�h�̂Ƃ��́Asizeof(DWCRnkGetParam_order)
        ///< ���w�肵�܂��B

        /// �e���[�h���Ƃ̃p�����[�^
        union
        {

            DWCRnkGetParam_order    order;   ///< ���[�h = DWC_RNK_GET_MODE_ORDER �̂Ƃ��Ɏw��
            DWCRnkGetParam_toplist  toplist; ///< ���[�h = DWC_RNK_GET_MODE_TOPLIST �̂Ƃ��Ɏw��
            DWCRnkGetParam_nearby   nearby;  ///< ���[�h = DWC_RNK_GET_MODE_NEAR_* �̂Ƃ��Ɏw��
            DWCRnkGetParam_friends  friends; ///< ���[�h = DWC_RNK_GET_MODE_FRIENDS �̂Ƃ��Ɏw��

            u32 data;   ///< ���C�u�������Ŏg�p

        };

    }
    DWCRnkGetParam;


    /* -------------------------------------------------------------------------
            function - external
       ------------------------------------------------------------------------- */

    DWCRnkError	DWC_RnkInitialize(		const char* initdata,
                                    const DWCUserData* userdata);

    DWCRnkError	DWC_RnkShutdown();

    DWCRnkError	DWC_RnkPutScoreAsync(	u32 category,
                                      DWCRnkRegion region,
                                      s32 score,
                                      void* data,
                                      u32 size );

    DWCRnkError	DWC_RnkGetScoreAsync(	DWCRnkGetMode mode,
                                      u32 category,
                                      DWCRnkRegion region,
                                      DWCRnkGetParam* param );

    DWCRnkError	DWC_RnkCancelProcess();

    DWCRnkError	DWC_RnkProcess();

    DWCRnkState	DWC_RnkGetState();

//
// ���X�|���X�擾�p
//
    DWCRnkError	DWC_RnkResGetRow(		DWCRnkData* out,
                                   u32 index );

    DWCRnkError	DWC_RnkResGetRowCount(	u32* count );

    DWCRnkError	DWC_RnkResGetOrder(		u32* order );

    DWCRnkError	DWC_RnkResGetTotal(		u32* total );


#ifdef __cplusplus
}
#endif

#endif // DWC_RNK_H_
