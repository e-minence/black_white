/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - WCM - libraries
  File:     aplist.c

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-12-10#$
  $Rev: 1108 $
  $Author: okajima_manabu $
 *---------------------------------------------------------------------------*/
#include "wcm_private.h"
#include "wcm_message.h"

/*---------------------------------------------------------------------------*
    �萔��`
 *---------------------------------------------------------------------------*/
#if WCM_DEBUG

// �x�����p�e�L�X�g���`
#endif

/*---------------------------------------------------------------------------*
    �����֐��v���g�^�C�v
 *---------------------------------------------------------------------------*/
static WCMApList*   WcmAllocApList(void);
static void         WcmFreeApList(WCMApList* aplist);
static WCMApList*   WcmGetOldestApList(void);
static WCMApList*   WcmGetLastApList(void);
static WCMApList*   WcmSearchApList(u8* bssid);
static WCMApList*   WcmSearchIndexedApList(u32 index);
static void         WcmAppendApList(WCMApList* aplist);

/*---------------------------------------------------------------------------*
  Name:         WCM_ClearApList

  Description:  AP ���ێ����X�g�̓��e���������ď�����Ԃɖ߂��B
                ���b�N��Ԕ@���Ɋւ�炸���ێ��̈���N���A����B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WCM_ClearApList(void)
{
    OSIntrMode  e = OS_DisableInterrupts();
    WCMWork*    w = WCMi_GetSystemWork();

    // �������ς݂��m�F
    if (w == NULL)
    {
        WCMi_Warning(WCMi_WText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return;
    }

    // AP ���ێ����X�g�p�o�b�t�@��K�v�ɉ����ăN���A
    if ((w->config.pbdbuffer != NULL) && (w->config.nbdbuffer > 0))
    {
        MI_CpuClear8(w->config.pbdbuffer, (u32) (w->config.nbdbuffer));
    }

    (void)OS_RestoreInterrupts(e);
}

/*---------------------------------------------------------------------------*
  Name:         WCM_CountApList

  Description:  AP ���ێ����X�g���ŕێ�����Ă��� AP ���Ǘ��u���b�N����
                �擾����B
    NOTICE:     WCM_LockApList�֐��ɂă��b�N����Ă��Ȃ��ꍇ�ɂ͊֐��ďo�����
                ���荞�݂ɂ���ău���b�N���͑�������\��������_�ɒ��ӁB

  Arguments:    None.

  Returns:      s32     -   AP ���Ǘ��u���b�N����Ԃ��B
                            ����Ƀu���b�N�����擾�ł��Ȃ��ꍇ�ɂ� 0 ��Ԃ��B
 *---------------------------------------------------------------------------*/
s32 WCM_CountApList(void)
{
    OSIntrMode          e = OS_DisableInterrupts();
    WCMWork*        w = WCMi_GetSystemWork();
    s32         count = 0;
    WCMApListHeader*    pHeader;

    // �������ς݂��m�F
    if (w == NULL)
    {
        WCMi_Warning(WCMi_WText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return 0;
    }

    // AP ���ێ����X�g����A�ێ����Ă��� AP ���Ǘ��u���b�N�����擾
    pHeader = (WCMApListHeader *) (w->config.pbdbuffer);
    if ((pHeader != NULL) && (w->config.nbdbuffer > sizeof(WCMApListHeader)))
    {
        count = (s32) (pHeader->count);
    }

    // ����I��
    (void)OS_RestoreInterrupts(e);
    return count;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_LockApList

  Description:  AP ���ێ����X�g�̃��b�N��Ԃ�ύX����B
    NOTICE:     ���b�N���͊��荞�݂ɂ���� AP �𔭌������ꍇ�ł� AP ���ێ�
                ���X�g�̓��e�͕ύX���ꂸ�A�������� AP ���͔j�������_�ɒ��ӁB

  Arguments:    lock    -   WCM_APLIST_LOCK     : ���b�N����B
                            WCM_APLIST_UNLOCK   : ���b�N���������B
                            ���̑�              : WCM_APLIST_LOCK �Ƃ݂Ȃ��B

  Returns:      s32     -   �Ăяo���O�̃��b�N��Ԃ�Ԃ��B
                            WCM_APLIST_LOCK     : ���b�N����Ă����B
                            WCM_APLIST_UNLOCK   : ���b�N�͉������Ă����B
 *---------------------------------------------------------------------------*/
s32 WCM_LockApList(s32 lock)
{
    OSIntrMode  e = OS_DisableInterrupts();
    WCMWork*    w = WCMi_GetSystemWork();

    // �������ς݂��m�F
    if (w == NULL)
    {
        WCMi_Warning(WCMi_WText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return 0;
    }

    // �����ɍ��킹�ă��b�N��Ԃ�ύX
    if (lock != WCM_APLIST_UNLOCK)
    {
        lock = (w->apListLock ? WCM_APLIST_LOCK : WCM_APLIST_UNLOCK);
        w->apListLock = WCM_APLIST_LOCK;
    }
    else
    {
        lock = (w->apListLock ? WCM_APLIST_LOCK : WCM_APLIST_UNLOCK);
        w->apListLock = WCM_APLIST_UNLOCK;
    }

    // ����I��
    (void)OS_RestoreInterrupts(e);
    return lock;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_PointApList

  Description:  AP ���ێ����X�g������A�w�肳�ꂽ index �����蓖�Ă��Ă���
                AP ���𒊏o����B
    NOTICE:     WCM_LockApList�֐��ɂ���ă��b�N����Ă��Ȃ��ꍇ�ɂ͊֐��ďo��
                ��Ɋ��荞�݂ɂ���Ď擾�����|�C���^�������o�b�t�@�̓��e������
                �����\��������_�ɒ��ӁB

  Arguments:    index       -   index ���w�肷��BWCM_CountApList�֐��ɂ����
                                �����鐔 ������ index ���w�肷��K�v������B

  Returns:      WCMBssDesc* -   ���o���� AP ���ւ̃|�C���^��Ԃ��B
                                ���o�Ɏ��s�����ꍇ�� NULL ��Ԃ��B
 *---------------------------------------------------------------------------*/
WMBssDesc*  WCM_PointApList(s32 index)
{
    return (WMBssDesc*) WCM_PointApListEx(index);
}

WCMBssDesc* WCM_PointApListEx(s32 index)
{
    OSIntrMode  e = OS_DisableInterrupts();
    WCMWork*    w = WCMi_GetSystemWork();
    WCMBssDesc*  bd = NULL;
    WCMApList*  p;

    // �������ς݂��m�F
    if (w == NULL)
    {
        WCMi_Warning(WCMi_WText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return NULL;
    }

    // index �Ŏw�肳�ꂽ AP ���Ǘ��u���b�N������
    p = WcmSearchIndexedApList((u32) index);

    // ������Ȃ������ꍇ�ُ͈�I��
    if (p == NULL)
    {
        WCMi_Warning(WCMi_WText_NoIndexData, index);
        (void)OS_RestoreInterrupts(e);
        return NULL;
    }

    // ����I��
    bd = (WCMBssDesc *) (p->data);
    (void)OS_RestoreInterrupts(e);
    return bd;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_PointApListLinkLevel

  Description:  AP ���ێ����X�g������A�w�肳�ꂽ index �����蓖�Ă��Ă���
                AP ��񂪓o�^���ꂽ�ۂ̓d�g���x�𒊏o����B

  Arguments:    index       -   index ���w�肷��BWCM_CountApList�֐��ɂ����
                                �����鐔 ������ index ���w�肷��K�v������B

  Returns:      WMLinkLevel -   ���o���� AP ���o�^���̓d�g���x��Ԃ��B
                                ���o�Ɏ��s�����ꍇ�� WM_LINK_LEVEL_0 ��Ԃ��B
 *---------------------------------------------------------------------------*/
WMLinkLevel WCM_PointApListLinkLevel(s32 index)
{
    OSIntrMode  e = OS_DisableInterrupts();
    WCMWork*    w = WCMi_GetSystemWork();
    WMLinkLevel ll = WM_LINK_LEVEL_0;
    WCMApList*  p;

    // �������ς݂��m�F
    if (w == NULL)
    {
        WCMi_Warning(WCMi_WText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return WM_LINK_LEVEL_0;
    }

    // index �Ŏw�肳�ꂽ AP ���Ǘ��u���b�N������
    p = WcmSearchIndexedApList((u32) index);

    // ������Ȃ������ꍇ�ُ͈�I��
    if (p == NULL)
    {
        WCMi_Warning(WCMi_WText_NoIndexData, index);
        (void)OS_RestoreInterrupts(e);
        return WM_LINK_LEVEL_0;
    }

    // ����I��
    ll = (WMLinkLevel) (p->linkLevel);
    (void)OS_RestoreInterrupts(e);
    return ll;
} 

/*---------------------------------------------------------------------------*
  Name:         WCMi_EntryApList

  Description:  �X�L�����̌��� AP ���������ꂽ�ۂɌĂяo���������֐��B
                ���b�N���������Ă����Ԃł���΁AAP ���ێ����X�g��
                config �ݒ�ɏ]���ĕҏW����B
    NOTICE:     ���荞�݋֎~���ɌĂяo����邱�Ƃ�O��Ƃ��Ă��邽�߁A�C�ӂ�
                �ꏊ����̌Ăяo���͋֎~�B

  Arguments:    bssDesc     -   �������ꂽ AP ���ւ̃|�C���^�B
                linkLevel   -   AP ���������ꂽ�ۂ̓d�g���x�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WCMi_EntryApList(WCMBssDesc* bssDesc, u16 linkLevel)
{
    WCMWork*    w = WCMi_GetSystemWork();
    WCMApList*  aplist;

    // �������m�F
    if (w == NULL)
    {
        return;
    }

    // AP ���X�g�ҏW���m�F
    if (w->apListLock != WCM_APLIST_UNLOCK)
    {
        return;
    }

    // AP �ȊO�� NITRO �e�@�̏ꍇ�̓X�L�b�v
#ifdef SDK_TWL
    if (OS_IsRunOnTwl())
    {
        if (w->deviceId == WCM_DEVICEID_NWM)
        {
            if (WCMi_GetGameInfo(bssDesc) != NULL)
            {
                return;
            }
        }
    }
#endif
    if (w->deviceId == WCM_DEVICEID_WM)
    {
        if (bssDesc->gameInfoLength != 0)
        {
            return;
        }
    }

    // �ҏW���ׂ� AP ���ێ��o�b�t�@�ւ̃|�C���^���擾
    {
        // ���ɏ���ێ�����Ă��� AP ���ǂ����𔻒�
        aplist = WcmSearchApList(bssDesc->bssid);

        // ���̕ێ�����Ă��Ȃ� AP �ł������ꍇ
        if (aplist == NULL)
        {
            // �V�K�ɏ���ێ����郁�����G���A���m��
            aplist = WcmAllocApList();
        }

        // ����ێ�����o�b�t�@����t�ŁA����"����"�ݒ�ɂȂ��Ă���ꍇ
        if ((aplist == NULL) && (w->config.nbdmode == WCM_APLIST_MODE_EXCHANGE))
        {
            // �ł���񂪌Â� AP ��񂪕ێ�����Ă��郁�����G���A���擾
            aplist = WcmGetOldestApList();
        }
    }

    // bssDesc ����s�v�� Infomation Elements ����菜�������̂��R�s�[
    // AP ���ێ��o�b�t�@��ҏW
    if (aplist != NULL)
    {
        WCMBssDesc* destBssDesc = (WCMBssDesc*) aplist->data;

        WCMi_BssDescCopy(bssDesc, destBssDesc); //BssDesc �̃R�s�[
        aplist->linkLevel = linkLevel;

        // ���X�g�̍Ō�� ( = �ŐV�� AP ��� ) �Ɉړ�
        WcmAppendApList(aplist);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcmAllocApList

  Description:  AP ���ێ����X�g�̈悩��A�V���� AP �����Ǘ�����u���b�N��
                ���蓖�Ă�B�m�ۂ���u���b�N�� index �� 0 ���珇�ԂƂ���B

  Arguments:    None.

  Returns:      WCMApList*  -   ���蓖�Ă� AP ���Ǘ��u���b�N�ւ̃|�C���^�B
                                ���蓖�ĂɎ��s�����ꍇ�� NULL ��Ԃ��B
 *---------------------------------------------------------------------------*/
static WCMApList* WcmAllocApList(void)
{
    WCMWork*        w = WCMi_GetSystemWork();
    WCMApListHeader*    pHeader = (WCMApListHeader *) (w->config.pbdbuffer);
    WCMApList*          p = NULL;

    // AP ���ێ����X�g���g�p�\�ȏ�Ԃɂ��邱�Ƃ��m�F
    if ((pHeader != NULL) && (w->config.nbdbuffer > sizeof(WCMApListHeader)))
    {
        // �����u���b�N���m�ۉ\���v�Z
        u32 maxBlock = (w->config.nbdbuffer - sizeof(WCMApListHeader)) / sizeof(WCMApList);

        // �ЂƂł��u���b�N���m�ۉ\��
        if (maxBlock > 0)
        {
            s32 i;

            // �]�胁�����u���b�N�����邩
            if (maxBlock > pHeader->count)
            {
                // �]�胁�����u���b�N������
                for (i = 0; i < maxBlock; i++)
                {
                    p = (WCMApList *) ((u32) pHeader + sizeof(WCMApListHeader) + (i * sizeof(WCMApList)));
                    if (p->state == WCM_APLIST_BLOCK_EMPTY)
                    {
                        break;
                    }
                }

                if (i < maxBlock)
                {
                    // �������u���b�N��"�g�p��"�ɕύX
                    p->state = WCM_APLIST_BLOCK_OCCUPY;
                    p->index = pHeader->count;

                    // ���X�g�̍Ō���ɐV�K�Ɋm�ۂ����u���b�N��ǉ�
                    p->next = NULL;
                    p->previous = pHeader->tail;
                    pHeader->tail = p;
                    if (p->previous)
                    {
                        ((WCMApList *) (p->previous))->next = p;
                    }
                    else
                    {
                        pHeader->head = p;
                    }

                    // ���X�g�����C���N�������g
                    pHeader->count++;
                }
            }
        }
    }

    return p;
}

/*---------------------------------------------------------------------------*
  Name:         WcmFreeApList

  Description:  AP ���ێ����X�g�̈悩�犄�蓖�Ă��Ă��� AP �����Ǘ�����
                �u���b�N��������čė��p�ł����Ԃɖ߂��B
                �w�肳�ꂽ�u���b�N�����݂��Ȃ��ꍇ�ȂǁA����Ƀu���b�N���������
                �s���Ȃ��ꍇ�ł������ʒm���Ȃ��B
    NOTICE:     ���X�g�Ǘ��̗���������߂ɑ��݂��A�����ł͎g�p����Ă��Ȃ��B

  Arguments:    aplist  -   ������� AP ���Ǘ��u���b�N�ւ̃|�C���^�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmFreeApList(WCMApList* aplist)
{
    WCMWork*        w = WCMi_GetSystemWork();
    WCMApListHeader*    pHeader = (WCMApListHeader *) (w->config.pbdbuffer);
    WCMApList*          p;

    // �p�����[�^���m�F
    if (aplist == NULL)
    {
        return; // �p�����[�^�ُ�
    }

    // AP ���ێ����X�g���g�p�\�ȏ�Ԃɂ��邱�Ƃ��m�F
    if ((pHeader != NULL) && (w->config.nbdbuffer > sizeof(WCMApListHeader)))
    {
        // �|�C���^�̈�v���� AP���Ǘ��u���b�N�����X�g���猟��
        p = pHeader->head;
        while (p)
        {
            if (p == aplist)
            {
                // ���X�g���珜��
                if (aplist->previous)
                {
                    ((WCMApList *) (aplist->previous))->next = aplist->next;
                }
                else
                {
                    pHeader->head = aplist->next;
                }

                if (aplist->next)
                {
                    ((WCMApList *) (aplist->next))->previous = aplist->previous;
                }
                else
                {
                    pHeader->tail = aplist->previous;
                }

                // ���������u���b�N�� index �������p��( index �̎�������h���� )
                if (pHeader->count > 0)
                {
                    u32 index = aplist->index;

                    aplist = WcmSearchIndexedApList((u32) (pHeader->count - 1));
                    if (aplist != NULL)
                    {
                        aplist->index = index;
                    }

                    pHeader->count--;
                }

                // �g�p���Ă����u���b�N�����
                MI_CpuClear8(aplist, sizeof(WCMApList));
            }

            p = p->next;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcmGetOldestApList

  Description:  AP ���ێ����X�g�̈���ɊǗ�����Ă��� AP ��񂩂�A�ł��Â���
                �o�^���ꂽ AP ���Ǘ��u���b�N����������B

  Arguments:    None.

  Returns:      WCMApList*  -   AP ���Ǘ��u���b�N�ւ̃|�C���^��Ԃ��B
                                ���X�g�Ɉ�� AP ��񂪓o�^����Ă��Ȃ��ꍇ�ɂ�
                                NULL ��Ԃ��B
 *---------------------------------------------------------------------------*/
static WCMApList* WcmGetOldestApList(void)
{
    WCMWork*        w = WCMi_GetSystemWork();
    WCMApListHeader*    pHeader = (WCMApListHeader *) (w->config.pbdbuffer);

    // AP ���ێ����X�g���g�p�\�ȏ�Ԃɂ��邱�Ƃ��m�F
    if ((pHeader != NULL) && (w->config.nbdbuffer > sizeof(WCMApListHeader)))
    {
        // ���X�g�̐擪�� AP ���Ǘ��u���b�N���Ō�
        return pHeader->head;
    }

    // �ُ�I��
    return NULL;
}

/*---------------------------------------------------------------------------*
  Name:         WcmGetLastApList

  Description:  AP ���ێ����X�g�̈���ɊǗ�����Ă��� AP ��񂩂�A�ł��V����
                �o�^���ꂽ AP ���Ǘ��u���b�N����������B
    NOTICE:     ���X�g�Ǘ��̗���������߂ɑ��݂��A�����ł͎g�p����Ă��Ȃ��B

  Arguments:    None.

  Returns:      WCMApList*  -   AP ���Ǘ��u���b�N�ւ̃|�C���^��Ԃ��B
                                ���X�g�Ɉ�� AP ��񂪓o�^����Ă��Ȃ��ꍇ�ɂ�
                                NULL ��Ԃ��B
 *---------------------------------------------------------------------------*/
static WCMApList* WcmGetLastApList(void)
{
    WCMWork*        w = WCMi_GetSystemWork();
    WCMApListHeader*    pHeader = (WCMApListHeader *) (w->config.pbdbuffer);

    // AP ���ێ����X�g���g�p�\�ȏ�Ԃɂ��邱�Ƃ��m�F
    if ((pHeader != NULL) && (w->config.nbdbuffer > sizeof(WCMApListHeader)))
    {
        // ���X�g�̍Ō���� AP ���Ǘ��u���b�N���ŐV
        return pHeader->tail;
    }

    // �ُ�I��
    return NULL;
}

/*---------------------------------------------------------------------------*
  Name:         WcmSearchApList

  Description:  AP ���ێ����X�g�̈���ɊǗ�����Ă��� AP ��񂩂�A�w�肳�ꂽ
                BSSID ������ AP ���Ǘ��u���b�N����������B

  Arguments:    bssid       -   ���������� BSSID �ւ̃|�C���^���w�肷��B

  Returns:      WCMApList*  -   AP ���Ǘ��u���b�N�ւ̃|�C���^��Ԃ��B
                                BSSID �̍��v���� AP ��񂪑��݂��Ȃ��ꍇ�ɂ�
                                NULL ��Ԃ��B
 *---------------------------------------------------------------------------*/
static WCMApList* WcmSearchApList(u8* bssid)
{
    WCMWork*        w = WCMi_GetSystemWork();
    WCMApListHeader*    pHeader = (WCMApListHeader *) (w->config.pbdbuffer);
    WCMApList*          p = NULL;

    // �p�����[�^�m�F
    if (bssid == NULL)
    {
        return NULL;    // �p�����[�^�ُ�
    }

    // AP ���ێ����X�g���g�p�\�ȏ�Ԃɂ��邱�Ƃ��m�F
    if ((pHeader != NULL) && (w->config.nbdbuffer > sizeof(WCMApListHeader)))
    {
        // BSSID �̈�v���� AP ���Ǘ��u���b�N������
        p = pHeader->head;
        while (p)
        {
            if (WCM_CompareBssID(((WMBssDesc *) (p->data))->bssid, bssid))
            {
                break;
            }

            p = p->next;
        }
    }

    return p;
}

/*---------------------------------------------------------------------------*
  Name:         WcmSearchIndexedApList

  Description:  AP ���ێ����X�g�̈���ɊǗ�����Ă��� AP ��񂩂�A�w�肳�ꂽ
                index ������ AP ���Ǘ��u���b�N����������B

  Arguments:    index       -   index ���w�肷��B

  Returns:      WCMApList*  -   AP ���Ǘ��u���b�N�ւ̃|�C���^��Ԃ��B
                                index �̍��v���� AP ��񂪑��݂��Ȃ��ꍇ�ɂ�
                                NULL ��Ԃ��B
 *---------------------------------------------------------------------------*/
static WCMApList* WcmSearchIndexedApList(u32 index)
{
    WCMWork*        w = WCMi_GetSystemWork();
    WCMApListHeader*    pHeader = (WCMApListHeader *) (w->config.pbdbuffer);
    WCMApList*          p = NULL;

    // AP ���ێ����X�g���g�p�\�ȏ�Ԃɂ��邱�Ƃ��m�F
    if ((pHeader != NULL) && (w->config.nbdbuffer > sizeof(WCMApListHeader)))
    {
        p = pHeader->head;
        while (p)
        {
            // index �̈�v���� AP ���Ǘ��u���b�N������
            if (p->index == index)
            {
                break;
            }

            p = p->next;
        }
    }

    return p;
}

/*---------------------------------------------------------------------------*
  Name:         WcmAppendApList

  Description:  AP ���ێ����X�g���ŊǗ�����Ă��� AP ���Ǘ��u���b�N���ŐV
                ���Ƃ��ēo�^�������B���̍ہA�u���b�N�ɗ^�����Ă��� index ��
                �ύX����Ȃ��B���X�g�ɊǗ�����Ă��Ȃ��ꍇ�ɂ͐V�K�ɒǉ����ꂽ
                �u���b�N�Ƃ��Ĉ����B

  Arguments:    aplist  -   �o�^������ AP ���Ǘ��u���b�N�ւ̃|�C���^���w��B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmAppendApList(WCMApList* aplist)
{
    WCMWork*        w = WCMi_GetSystemWork();
    WCMApListHeader*    pHeader = (WCMApListHeader *) (w->config.pbdbuffer);
    WCMApList*          p = NULL;

    // �p�����[�^�m�F
    if (aplist == NULL)
    {
        return; // �p�����[�^�ُ�
    }

    // AP ���ێ����X�g���g�p�\�ȏ�Ԃɂ��邱�Ƃ��m�F
    if ((pHeader != NULL) && (w->config.nbdbuffer > sizeof(WCMApListHeader)))
    {
        // ���Ƀ��X�g�ɑ��݂��邱�Ƃ��m�F
        p = pHeader->head;
        while (p)
        {
            if (p == aplist)
            {
                // ��U���X�g���珜��
                if (p->previous)
                {
                    ((WCMApList *) (p->previous))->next = p->next;
                }
                else
                {
                    pHeader->head = p->next;
                }

                if (p->next)
                {
                    ((WCMApList *) (p->next))->previous = p->previous;
                }
                else
                {
                    pHeader->tail = p->previous;
                }
                break;
            }

            p = p->next;
        }

        // ���X�g�̍Ō���ɒǉ�
        aplist->next = NULL;
        aplist->previous = pHeader->tail;
        pHeader->tail = aplist;
        if (aplist->previous)
        {
            ((WCMApList *) (aplist->previous))->next = aplist;
        }
        else
        {
            pHeader->head = aplist;
        }

        // �V�K�ɒǉ������ꍇ�̓��X�g�����C���N�������g
        if (p == NULL)
        {
            aplist->index = pHeader->count;
            pHeader->count++;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_BssDescCopy

  Description:  WCMBssDesc ��s�v�� Infomation Elements �������ăR�s�[����B

  Arguments:    srcBssDesc  - �R�s�[���� AP ���
                destBssDesc - �R�s�[��� AP ���

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WCMi_BssDescCopy(WCMBssDesc* srcBssDesc, WCMBssDesc* destBssDesc)
{
    WCMWork*        w = WCMi_GetSystemWork();
    
    MI_CpuClearFast(destBssDesc, WCM_APLIST_SIZE);
    MI_CpuCopyFast(srcBssDesc, destBssDesc, WCM_APLIST_SIZE - WM_SIZE_GAMEINFO );       // IE �ȊO���R�s�[

    destBssDesc -> otherElementCount = 0;

    // BSSDesc.length �� TWL �����ł̓o�C�g�P�ʁANITRO �����ł̓n�[�t���[�h�P��
    if( w -> deviceId == WCM_DEVICEID_WM )
    {
        destBssDesc -> length            = (WCM_APLIST_SIZE - WM_SIZE_GAMEINFO) / sizeof(u16);
    }
    else
    {
        destBssDesc -> length            = (WCM_APLIST_SIZE - WM_SIZE_GAMEINFO);
    }

    // �K�v�� Infomation Elements �݂̂𒊏o�i������ 192 Byte �Ɏ��܂�͈͂Łj
    {
        u8* pSrc    =(u8*)&(srcBssDesc->gameInfo);
        u8* pDest   =(u8*)&(destBssDesc->gameInfo);
        u8  ieLength=0;
        u16 i       =0; //�ُ�� BssDesc �ł� 256 �𒴂��� IE ���܂܂�Ă���\��������

        const u8 wpaTag[4]      ={0x00,0x50,0xF2,0x01};

        const u8 wmmTag[5]      ={0x00,0x50,0xF2,0x02,0x00};
//        const u8 wmmParamTag[5] ={0x00,0x50,0xF2,0x02,0x01};

        for( i=0; i < srcBssDesc->otherElementCount; i++ )
        {
            u8 id  = pSrc[0];
            u8 len = pSrc[1];

            if( (id == WCM_IE_ID_EX_SUPPORT_RATE)   ||
                (id == WCM_IE_ID_ERP_INFORMATION_A)  ||
                (id == WCM_IE_ID_ERP_INFORMATION_B)  ||
                (id == WCM_IE_ID_COUNTRY)           ||
                (id == WCM_IE_ID_RSN) )
            {
                /* �K�v��IE �����������̂ŃR�s�[ */
                MI_CpuCopy(pSrc, pDest, (u8)(len + 2) );

                pDest += len + 2;
                ieLength += len + 2;
                destBssDesc->otherElementCount++;
            }
            /* WPA IE �͕ʓr���� */
            else if(( id == WCM_IE_ID_VENDOR_SPECIFIC) &&
                    ((MI_CpuComp8(pSrc + 2, wpaTag, 4) == 0) || (MI_CpuComp8(pSrc + 2, wmmTag, 5) == 0)) )
            {
                MI_CpuCopy(pSrc, pDest, (u8)(len + 2) );

                pDest += len + 2;
                ieLength += len + 2;
                destBssDesc->otherElementCount++;
            }

            pSrc += len +2;
        }
        // BSSDesc.length �� TWL �����ł̓o�C�g�P�ʁANITRO �����ł̓n�[�t���[�h�P��
        if( w -> deviceId == WCM_DEVICEID_WM )
        {
            destBssDesc -> length += ieLength + 1 / sizeof(u16);
        }
        else // TWL ����
        {
            destBssDesc -> length += ieLength;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_GetGameInfo

  Description:  WCMBssDesc �̒��g�𒲍����� MP �e�@�̃r�[�R���������ꍇ WMGameInfo ���擾����

  Arguments:    bssDesc  - AP ���

  Returns:      WMGameInfo
 *---------------------------------------------------------------------------*/
WMGameInfo* WCMi_GetGameInfo(const WCMBssDesc* bssDesc)
{
    WCMWork*        w = WCMi_GetSystemWork();
    const u8 tag[4] = { 0x00, 0x09, 0xBF, 0x00 };   // GameInfo �ŗL�̃^�O
    
    /* NULL check */
    if (bssDesc == NULL)
    {
        return NULL;
    }
    
#ifdef SDK_TWL
    if (OS_IsRunOnTwl())
    {
        if(w->deviceId == WCM_DEVICEID_NWM) /* TWL �������g�����ꍇ */
        {
            NWMInfoElements *elem = NULL;
            // WM �� NWM �� BssDesc �ɂ����� IE �̈����������قȂ邽�ߏ����𕪂���
            if (bssDesc->elementCount > 0)
            {
                elem = NWM_GetVenderInfoElements((NWMBssDesc*)bssDesc, NWM_ELEMENTID_NINTENDO, (u8*)tag);
            }

            if (elem != NULL)
            {
                WMGameInfo* gameInfo = (WMGameInfo*)((elem->element) + 4);
                if (gameInfo->magicNumber == 0x0001) /* WM �̃��t�@�����X�ɏ]���A�}�W�b�N�i���o�[���`�F�b�N */
                {
                    return gameInfo;
                }
            }
        }
    }
    else
#endif
    {
        if(w->deviceId == WCM_DEVICEID_WM) /* NITRO �������g�����ꍇ */
        {
            if( (bssDesc->gameInfoLength != 0) && (WM_IsValidGameBeacon((WMBssDesc*)bssDesc) == TRUE) )
            {
                return &(((WMBssDesc*)bssDesc)->gameInfo);
            }
        }
    }
        return NULL;
}
/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
