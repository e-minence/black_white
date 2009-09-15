 /*---------------------------------------------------------------------------*
  Project:  TwlSDK - MB - include
  File:     mb_cache.h

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 8573 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#if	!defined(NITRO_MB_CACHE_H_)
#define NITRO_MB_CACHE_H_


#if	defined(__cplusplus)
extern  "C"
{
#endif


/* constant ---------------------------------------------------------------- */


#define	MB_CACHE_INFO_MAX	4

#define MB_CACHE_STATE_EMPTY   0
#define MB_CACHE_STATE_BUSY    1
#define MB_CACHE_STATE_READY   2
#define MB_CACHE_STATE_LOCKED  3


/* struct ------------------------------------------------------------------ */


/*
 * �y�[�W���O�ɂ�鉼�z�������̂��߂̍\����.
 * ���̍\���̔z��ő�e�ʃA�h���X���(CARD��)���L���b�V���Ǘ�����.
 */
    typedef struct
    {
        u32     src;                   /* logical source address */
        u32     len;                   /* cache length */
        u8     *ptr;                   /* pointer to cache buffer */
        u32     state;                 /* if 1, ready to use */
    }
    MBiCacheInfo;

    typedef struct
    {
        u32     lifetime;              /* if hit or timeout, set 0. */
        u32     recent;                /* unused */
        MBiCacheInfo *p_list;          /* unused */
        u32     size;                  /* unused */
        char    arc_name[FS_ARCHIVE_NAME_LEN_MAX + 1];  /* target archive */
        u32     arc_name_len;          /* archive name length */
        FSArchive *arc_pointer;
        u8      reserved[32 - FS_ARCHIVE_NAME_LEN_MAX - 1 - sizeof(u32) - sizeof(FSArchive*)];
        MBiCacheInfo list[MB_CACHE_INFO_MAX];
    }
    MBiCacheList;


/* function ---------------------------------------------------------------- */



/*---------------------------------------------------------------------------*
  Name:         MBi_InitCache

  Description:  �L���b�V�����X�g������������.
                

  Arguments:    pl         �L���b�V�����X�g

  Returns:      None.
 *---------------------------------------------------------------------------*/
    void    MBi_InitCache(MBiCacheList * pl);

/*---------------------------------------------------------------------------*
  Name:         MBi_AttachCacheBuffer

  Description:  �L���b�V�����X�g�փo�b�t�@�����蓖�Ă�.
                
  Arguments:    pl         �L���b�V�����X�g
                ptr        ���蓖�Ă�o�b�t�@
                src        ptr �̃\�[�X�A�h���X
                len        ptr �̃o�C�g�T�C�Y
                state      �w�肷�鏉�����
                           (MB_CACHE_STATE_READY �� MB_CACHE_STATE_LOCKED)

  Returns:      None.
 *---------------------------------------------------------------------------*/
    void    MBi_AttachCacheBuffer(MBiCacheList * pl, u32 src, u32 len, void *ptr, u32 state);

/*---------------------------------------------------------------------------*
  Name:         MBi_ReadFromCache

  Description:  �w�肵���A�h���X�̓��e���L���b�V�����烊�[�h����.

  Arguments:    pl         �L���b�V�����X�g
                src        ���[�h���A�h���X
                len        ���[�h�T�C�Y (BYTE)
                dst        ���[�h��A�h���X.

  Returns:      �L���b�V���Ƀq�b�g����΃��[�h���� TRUE, �����łȂ���� FALSE.
 *---------------------------------------------------------------------------*/
    BOOL    MBi_ReadFromCache(MBiCacheList * pl, u32 src, void *dst, u32 len);

/*---------------------------------------------------------------------------*
  Name:         MBi_TryLoadCache

  Description:  �w�肵���A�h���X�̓��e���L���b�V���փ��[�h����.
                �ł��Ԓn�̏����� READY �̃y�[�W�L���b�V�����j�������.

  Arguments:    pl         �L���b�V�����X�g
                src        �����[�h���A�h���X
                len        �����[�h�T�C�Y (BYTE)

  Returns:      �����[�h���J�n�ł���� TRUE, �����łȂ���� FALSE.
                (�����[�h�����̃G���W�����V�X�e�����ŒP��̏ꍇ,
                 �O��̏����𖢗��Ȃ炱�̊֐��� FALSE ��Ԃ��ł��낤)
 *---------------------------------------------------------------------------*/
    BOOL    MBi_TryLoadCache(MBiCacheList * pl, u32 src, u32 len);



#if	defined(__cplusplus)
}
#endif


#endif                                 /* NITRO_MB_CACHE_H_ */
