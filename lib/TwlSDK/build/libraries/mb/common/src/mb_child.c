/*---------------------------------------------------------------------------*
  Project:  TwlSDK - MB - libraries
  File:     mb_child.c

  Copyright 2007-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-04#$
  $Rev: 10698 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#include "mb_private.h"


// ----------------------------------------------------------------------------
// definition

#define MY_ROUND(n, a)      (((u32) (n) + (a) - 1) & ~((a) - 1))

// --- for child
static int MBi_CommRequestFile(u8 file_no, u32 ggid);
static void MBi_CommChangeChildState(int state, void *arg);
static void MBi_CommChangeChildStateCallbackOnly(int state, void *arg);
static void MBi_CommChildRecvData(void *arg);
static int MBi_CommChildSendData(void);
static void MBi_CommBeaconRecvCallback(MbBeaconMsg msg, MBGameInfoRecvList * gInfop, int index);

// --- miscellany
static BOOL checkRecvFlag(int x);
static void setRecvFlag(int x);
static u16 countRemainingblocks(void);
static u16 get_next_blockno(void);
static BOOL checkPollbmp(u16 pollbmp, u16 child_aid);
static void clearRecvRegion(void);
static void MBi_CommCallChildError(u16 errcode);


/* StartScanEx �p�̃X�L�����o�b�t�@ */
static u8 scanBuf[WM_SIZE_SCAN_EX_BUF] ATTRIBUTE_ALIGN(32);
extern WMscanExParam mbiScanParam;



/*  ============================================================================

    �q�@�p�֐�

    ============================================================================*/

/*---------------------------------------------------------------------------*
  Name:         MB_CommDownloadRequest

  Description:  �_�E�����[�h�v�����o���i�e�@�ڑ���File�v�����_�E�����[�h�Ƃ������A
                ��A�̓���̃g���K�ƂȂ�֐��j
                ����MB_CommDownloadRequest���p�b�N���āAmbrs���X�g�̃C���f�b�N�X
                �ԍ��Őڑ��v����������́B
                �C���f�b�N�X�ԍ����o���Ă������ƂŁA�ڑ��Ɏ��s�������ɓ����ł���
                �e�@�����N���A�ł���B

  Arguments:    index - �v������MbBeaconRecvStatus�̃��X�g�C���f�b�N�X�ԍ�

  Returns:      
 *---------------------------------------------------------------------------*/

int MB_CommDownloadRequest(int index)
{
    const MbBeaconRecvStatus *mbrsp;   // �e�@����M�X�e�[�^�X
    const WMBssDesc *bssDescp;         // �ڑ���̐e�@���̃|�C���^
    u8      fileNo;                    // �@�V        �ɗv������t�@�C��No.
    int     ret;
    OSIntrMode enabled = OS_DisableInterrupts();        /* ���荞�݋֎~ */

    mbrsp = MB_GetBeaconRecvStatus();  // �e�@����M�X�e�[�^�X
    bssDescp = &(mbrsp->list[index].bssDesc);
    fileNo = mbrsp->list[index].gameInfo.fileNo;

    pCwork->connectTargetNo = index;   // �ڑ������݂�e�@�̃��X�gNo.��ۑ��B
    pCwork->fileid = fileNo;           // ���N�G�X�g�t�@�C��ID��\�ߓo�^
    pCwork->last_recv_seq_no = -1;     //�ŏI��M�u���b�N�V�[�P���X�ԍ�������

    // bssDescription�����[�N���ɃR�s�[
    MI_CpuCopy16(bssDescp, &pCwork->bssDescbuf, WM_BSS_DESC_SIZE);

    ret = MBi_CommConnectToParent((const WMBssDesc *)&pCwork->bssDescbuf);      // �܂��͐ڑ��ɓ���
    (void)OS_RestoreInterrupts(enabled);        /* ���荞�݋֎~���� */

    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         MBi_CommRequestFile

  Description:  �t�@�C���i���o�[���w�肵�āA�v�����o��

  Arguments:    file_no

  Returns:      
 *---------------------------------------------------------------------------*/

static int MBi_CommRequestFile(u8 file_no, u32 ggid)
{
    int     errcode;
    MBCommChildBlockHeader hd;
    u8     *databuf;
    MBCommRequestData req_data;

    /* ���N�G�X�g����Q�[����GGID�AUserInfo�AVersion��o�^ */
    req_data.ggid = ggid;
    MI_CpuCopy8(&pCwork->common.user, &req_data.userinfo, sizeof(MBUserInfo));
    req_data.version = MB_IPL_VERSION;
    req_data.fileid = file_no;

    hd.type = MB_COMM_TYPE_CHILD_FILEREQ;
    // ���M�o�b�t�@�Ƀf�[�^���R�s�[
    hd.req_data.piece = MBi_SendRequestDataPiece(hd.req_data.data, &req_data);

    databuf = MBi_MakeChildSendBuffer(&hd, (u8 *)pCwork->common.sendbuf);

    if (!databuf)
    {
        return MB_SENDFUNC_STATE_ERR;
    }
    errcode = MBi_BlockHeaderEnd(MB_COMM_CHILD_HEADER_SIZE, 0xFFFF, pCwork->common.sendbuf);
    return errcode;
}

/*---------------------------------------------------------------------------*
  Name:         MB_CommSetChildStateCallback

  Description:  �q�@�C�x���g�R�[���o�b�N�̐ݒ�

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/

void MB_CommSetChildStateCallback(MBCommCStateCallbackFunc callback)
{
    OSIntrMode enabled;

    SDK_ASSERT(pCwork != 0);

    enabled = OS_DisableInterrupts();  /* ���荞�݋֎~ */

    pCwork->child_callback = callback;

    (void)OS_RestoreInterrupts(enabled);        /* ���荞�݋֎~���� */
}

/*---------------------------------------------------------------------------*
  Name:         MB_CommGetChildState

  Description:  Download��Ԃ̎擾

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/

int MB_CommGetChildState(void)
{
    if (pCwork)
    {
        return pCwork->c_comm_state;
    }
    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         MB_GetChildProgressPercentage

  Description:  Download�i���p�[�Z���e�[�W�̎擾

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/

u16 MB_GetChildProgressPercentage(void)
{
    OSIntrMode enabled;
    u16     ret = 0;

    enabled = OS_DisableInterrupts();  /* ���荞�݋֎~ */

    if (pCwork->total_block > 0)
    {
        ret = (u16)((u32)(pCwork->got_block * 100) / pCwork->total_block);
    }

    (void)OS_RestoreInterrupts(enabled);        /* ���荞�݋֎~���� */

    return ret;                        // TotalBlock����0�̂Ƃ��́A0%��Ԃ��Ă���
}


/*---------------------------------------------------------------------------*
  Name:         MB_CommStartDownload

  Description:  �_�E�����[�h���J�n����

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/

BOOL MB_CommStartDownload(void)
{
    OSIntrMode enabled;
    BOOL    ret = FALSE;

    enabled = OS_DisableInterrupts();  /* ���荞�݋֎~ */

    if (pCwork
        && pCwork->c_comm_state == MB_COMM_CSTATE_DLINFO_ACCEPTED
        && pCwork->user_req == MB_COMM_USER_REQ_NONE)
    {

        pCwork->user_req = MB_COMM_USER_REQ_DL_START;

        ret = TRUE;
    }

    (void)OS_RestoreInterrupts(enabled);        /* ���荞�݋֎~���� */
    return ret;
}


/*---------------------------------------------------------------------------*
  Name:         MBi_CommChangeChildState

  Description:  �q�@��Ԃ̕ύX���R�[���o�b�N�Ăяo��

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/

static void MBi_CommChangeChildState(int state, void *arg)
{
    pCwork->c_comm_state = state;

    MBi_CommChangeChildStateCallbackOnly(state, arg);

}

/*---------------------------------------------------------------------------*
  Name:         MBi_CommChangeChildStateCallbackOnly

  Description:  �q�@��Ԓʒm���R�[���o�b�N�Ăяo���݂̂ōs��
                �����̏�Ԃ͕ύX����

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/

static void MBi_CommChangeChildStateCallbackOnly(int state, void *arg)
{
    if (pCwork->child_callback)        // ��ԑJ�ڃR�[���o�b�N
    {
        (*pCwork->child_callback) ((u32)state, arg);
    }

}

/*---------------------------------------------------------------------------*
  Name:         MBi_CommChildCallback

  Description:  �q�@�R�[���o�b�N�{��

  Arguments:    type:WM_TYPE event arg:callback argument

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_CommChildCallback(u16 type, void *arg)
{
    MB_COMM_WMEVENT_OUTPUT(type, arg);

    switch (type)
    {
    case MB_CALLBACK_INIT_COMPLETE:
        /* ���������� */
        MBi_CommChangeChildState(MB_COMM_CSTATE_INIT_COMPLETE, arg);

        // �e�@���������팸(�f�b�h�X�g���b�v)�̂��߂ɃX�L�����o�b�t�@���q�@�������������Ɏw��
        mbiScanParam.scanBuf = (WMBssDesc*)scanBuf;
        mbiScanParam.scanBufSize = WM_SIZE_SCAN_EX_BUF;
        break;

    case MB_CALLBACK_PARENT_FOUND:
        {
            u16 *linkLevel = (u16 *)arg;
            int     parent_no = MBi_GetLastFountParent();
            WMBssDesc *bssDescp = MBi_GetParentBssDesc(parent_no);

            /* �e�@���� */
            /* �r�[�R���o�R�Őe�@�Q�[�������擾 */
            (void)MB_RecvGameInfoBeacon(MBi_CommBeaconRecvCallback, *linkLevel, bssDescp);

            MB_CountGameInfoLifetime(MBi_CommBeaconRecvCallback, TRUE);
        }
        break;
    case MB_CALLBACK_PARENT_NOT_FOUND:
        /* �e�@�Q�[�����̎����J�E���g */
        MB_CountGameInfoLifetime(MBi_CommBeaconRecvCallback, FALSE);
        break;

    case MB_CALLBACK_CONNECTED_TO_PARENT:
        MBi_CommChangeChildState(MB_COMM_CSTATE_CONNECT, arg);
        break;

    case MB_CALLBACK_MP_CHILD_RECV:
        MBi_CommChildRecvData(arg);
        break;

    case MB_CALLBACK_MP_SEND_ENABLE:
        (void)MBi_CommChildSendData();
        break;

    case MB_CALLBACK_END_COMPLETE:
        /* �I������ */

        /* BOOT���N�G�X�g��M��ԂŁAMBi_CommEnd()���Ă΂ꂽ�ꍇ */
        if (pCwork->c_comm_state == MB_COMM_CSTATE_BOOTREQ_ACCEPTED && pCwork->boot_end_flag == 1)
        {
            // ����̗̈��DownloadFileInfo, bssDesc���Z�b�g�����[�_�[�ŗp����
            MI_CpuCopy16(&pCwork->dl_fileinfo,
                         (void *)MB_DOWNLOAD_FILEINFO_ADDRESS, sizeof(MBDownloadFileInfo));
            MI_CpuCopy16(&pCwork->bssDescbuf, (void *)MB_BSSDESC_ADDRESS, MB_BSSDESC_SIZE);
            MBi_CommChangeChildState(MB_COMM_CSTATE_BOOT_READY, NULL);  // �u�[�g��������
        }
        /* ����ȊO�̏ꍇ(�L�����Z��) */
        else
        {
            MBi_CommChangeChildState(MB_COMM_CSTATE_CANCELLED, NULL);   // �L�����Z��

            /* PlayerNo ��0�N���A */
            pCwork->common.user.playerNo = 0;
            clearRecvRegion();

            pCwork->c_comm_state = MB_COMM_CSTATE_NONE;
        }

        break;

    case MB_CALLBACK_CONNECT_FAILED:
        /* �ڑ����s */
        /* Callback�����ɁAWMstartConnectCallback�^��arg��n���B */
        MBi_CommChangeChildState(MB_COMM_CSTATE_CONNECT_FAILED, arg);
        MB_DeleteRecvGameInfo(pCwork->connectTargetNo); // �ڑ��Ɏ��s�����e�@�̃Q�[�������폜�B
        pCwork->connectTargetNo = 0;
        (void)MBi_RestartScan();       // �R�[���o�b�N�ʒm��ɃX�L�����ĊJ
        (void)MBi_CommEnd();

        break;

    case MB_CALLBACK_DISCONNECTED_FROM_PARENT:
        /* �ؒf�ʒm */
        /* Callback�����ɁAWMstartConnectCallback�^��arg��n���B */
        MBi_CommChangeChildState(MB_COMM_CSTATE_DISCONNECTED_BY_PARENT, arg);
        (void)MBi_RestartScan();       // �R�[���o�b�N�ʒm��ɃX�L�����ĊJ
        (void)MBi_CommEnd();
        break;

    case MB_CALLBACK_API_ERROR:
        /* ARM9��WM API���R�[���������_�ł̕Ԃ�l�G���[ */
        {
            u16     apiid, errcode;

            apiid = ((u16 *)arg)[0];
            errcode = ((u16 *)arg)[1];

            switch (errcode)
            {
            case WM_ERRCODE_INVALID_PARAM:
            case WM_ERRCODE_FAILED:
            case WM_ERRCODE_WM_DISABLE:
            case WM_ERRCODE_NO_DATASET:
            case WM_ERRCODE_FIFO_ERROR:
            case WM_ERRCODE_TIMEOUT:
                MBi_CommCallChildError(MB_ERRCODE_FATAL);
                break;
            case WM_ERRCODE_OPERATING:
            case WM_ERRCODE_ILLEGAL_STATE:
            case WM_ERRCODE_NO_CHILD:
            case WM_ERRCODE_OVER_MAX_ENTRY:
            case WM_ERRCODE_NO_ENTRY:
            case WM_ERRCODE_INVALID_POLLBITMAP:
            case WM_ERRCODE_NO_DATA:
            case WM_ERRCODE_SEND_QUEUE_FULL:
            case WM_ERRCODE_SEND_FAILED:
            default:
                MBi_CommCallChildError(MB_ERRCODE_WM_FAILURE);
                break;
            }
        }
        break;
    case MB_CALLBACK_ERROR:
        {
            /* WM API�R�[����ɕԂ��Ă����R�[���o�b�N�ɂ�����G���[ */
            WMCallback *pWmcb = (WMCallback *)arg;
            switch (pWmcb->apiid)
            {
            case WM_APIID_INITIALIZE:
            case WM_APIID_SET_LIFETIME:
            case WM_APIID_SET_P_PARAM:
            case WM_APIID_SET_BEACON_IND:
            case WM_APIID_START_PARENT:
            case WM_APIID_START_MP:
            case WM_APIID_SET_MP_DATA:
            case WM_APIID_START_DCF:
            case WM_APIID_SET_DCF_DATA:
            case WM_APIID_DISCONNECT:
            case WM_APIID_START_KS:
                /* �ȏ�̃G���[�́AWM�ŏ������̕K�v�ȃG���[ */
                MBi_CommCallChildError(MB_ERRCODE_FATAL);
                break;
            case WM_APIID_RESET:
            case WM_APIID_END:
            default:
                /* ���̑��̃G���[�ɂ��ẮA�R�[���o�b�N�G���[�Ƃ��ĕԂ� */
                MBi_CommCallChildError(MB_ERRCODE_WM_FAILURE);
                break;
            }
        }
        break;

    default:
        break;
    }

#if ( CALLBACK_WM_STATE == 1 )
    if (pCwork->child_callback)
    {
        (*pCwork->child_callback) ((u32)(MB_COMM_CSTATE_WM_EVENT | type), arg);
    }
#endif

    if (type == MB_CALLBACK_END_COMPLETE)
    {
        // ���[�N�̊J��
        MI_CpuClear16(pCwork, sizeof(MB_CommCWork));
        pCwork = NULL;
    }
}


/*---------------------------------------------------------------------------*
  Name:         MBi_CommChildRecvData

  Description:  �q�@�f�[�^��M

  Arguments:    arg : pointer to callback argument

  Returns:      None.
 *---------------------------------------------------------------------------*/

static void MBi_CommChildRecvData(void *arg)
{
    WMPortRecvCallback *bufp = (WMPortRecvCallback *)arg;
    MB_CommCWork *const p_child = pCwork;
    MBCommParentBlockHeader hd;
    u8     *databuf;
    u16     aid = MBi_GetAid();

    // ��M�o�b�t�@�̃L���b�V����j��
//  DC_InvalidateRange( bufp->data, MY_ROUND(bufp->length, 32) );

    databuf = MBi_SetRecvBufferFromParent(&hd, (u8 *)bufp->data);

    MB_DEBUG_OUTPUT("RECV ");
    MB_COMM_TYPE_OUTPUT(hd.type);

    switch (hd.type)                   // ��MBlock�^�C�v�ɂ���ԑJ��
    {
    case MB_COMM_TYPE_PARENT_SENDSTART:
        // �e�@����̑��M�J�n���b�Z�[�W
        if (p_child->c_comm_state == MB_COMM_CSTATE_CONNECT)
        {
            MB_DEBUG_OUTPUT("Allowed to request file from parent!\n");
            MBi_CommChangeChildState(MB_COMM_CSTATE_REQ_ENABLE, NULL);
        }
        break;

    case MB_COMM_TYPE_PARENT_KICKREQ: // �e�@�����Kick���b�Z�[�W
        if (p_child->c_comm_state == MB_COMM_CSTATE_REQ_ENABLE)
        {
            MB_DEBUG_OUTPUT("Kicked from parent!\n");
            MBi_CommChangeChildState(MB_COMM_CSTATE_REQ_REFUSED, NULL);
        }
        break;

    case MB_COMM_TYPE_PARENT_MEMBER_FULL:      // �e�@����̃����o�[���߃��b�Z�[�W
        if (p_child->c_comm_state == MB_COMM_CSTATE_REQ_ENABLE)
        {
            MB_DEBUG_OUTPUT("Member full!\n");
            MBi_CommChangeChildState(MB_COMM_CSTATE_MEMBER_FULL, NULL);
        }
        break;

    case MB_COMM_TYPE_PARENT_DL_FILEINFO:
        // MbDownloadFileInfoHeader�̎�M
        if (p_child->c_comm_state == MB_COMM_CSTATE_REQ_ENABLE)
        {

            // MbDownloadFileInfoHeader���p�o�b�t�@�Ɋi�[
            MI_CpuCopy8(databuf, &p_child->dl_fileinfo, sizeof(MBDownloadFileInfo));

            MB_DEBUG_OUTPUT("Download File Info has received (Total block num = %d)\n",
                            p_child->total_block);
            if (!MBi_MakeBlockInfoTable(&p_child->blockinfo_table,
                                        (MbDownloadFileInfoHeader *) & p_child->dl_fileinfo))
            {
                /* �󂯎����DownloadFileInfo���s���Ȃ��̂ł����� */
                MBi_CommCallChildError(MB_ERRCODE_INVALID_DLFILEINFO);
                OS_TWarning("The received DownloadFileInfo is illegal.\n");
                return;
            }

            /* PlayerNo �����蓖�Ă� */
            p_child->common.user.playerNo = aid;

            // ���u���b�N�����i�[
            p_child->total_block = MBi_get_blocknum(&p_child->blockinfo_table);

            /* ��M����MbDownloadFileInfo�������Ƃ��ēn���B */
            MBi_CommChangeChildState(MB_COMM_CSTATE_DLINFO_ACCEPTED, (void *)&p_child->dl_fileinfo);
        }

        break;

    case MB_COMM_TYPE_PARENT_DATA:
        /* �u���b�N�f�[�^�̎�M */
        if (p_child->c_comm_state == MB_COMM_CSTATE_DLINFO_ACCEPTED
            && p_child->user_req == MB_COMM_USER_REQ_DL_START)
        {
            MBi_CommChangeChildState(MB_COMM_CSTATE_RECV_PROCEED, NULL);
            p_child->user_req = MB_COMM_USER_REQ_NONE;
            /* State��ύX�����瑦��M�f�[�^�擾�� */
        }

        if (p_child->c_comm_state == MB_COMM_CSTATE_RECV_PROCEED)
        {
            u16     block_num;
            MB_BlockInfo bi;

            // MB_COMM_TYPE_DATA�̎��݂̂̏���
            block_num = p_child->total_block;

            if (block_num == 0 || block_num >= MB_MAX_BLOCK)
            {

                MBi_CommCallChildError(MB_ERRCODE_INVALID_BLOCK_NUM);

                OS_TWarning("Illegal Number of Block! [%d]\n", block_num);
                return;
            }

            /* �u���b�NNo.�Ɋւ���]�� */
            if (hd.seqno < 0 ||
                hd.seqno >= block_num ||
                MBi_get_blockinfo(&bi, &p_child->blockinfo_table, hd.seqno,
                                  &p_child->dl_fileinfo.header) == FALSE)
            {
                /* �u���b�N�ԍ����s�� */
                MBi_CommCallChildError(MB_ERRCODE_INVALID_BLOCK_NO);
                OS_TWarning("The illegal block No.[%d] has been received! (maxnum %d)\n",
                            hd.seqno, block_num);
                goto CheckRemainBlock;
            }

            if (hd.fid != p_child->fileid)
            {
                /* FileID���v�����Ă�����̂ƈقȂ� */
                MBi_CommCallChildError(MB_ERRCODE_INVALID_FILE);
                OS_TWarning("Received File ID [%d] differs from what was requested!\n", hd.fid);
                goto CheckRemainBlock;
            }

            if (!MBi_IsAbleToRecv(bi.segment_no, bi.child_address, bi.size))
            {
                /* ��M�A�h���X���s�� */
                MBi_CommCallChildError(MB_ERRCODE_INVALID_RECV_ADDR);
                OS_TWarning("The receive address of Block No.%d is illegal. [%08x - %08x]\n",
                            hd.seqno, bi.child_address, bi.child_address + bi.size);
                goto CheckRemainBlock;
            }

            /* ��M�A�h���X�`�F�b�N��A�w��A�h���X�փR�s�[ */
            if (checkRecvFlag(hd.seqno) == FALSE)
            {
                MB_DEBUG_OUTPUT("DATA : BLOCK(%d)/REMAIN(%d), Recv address[%x] size[%x]\n",
                                hd.seqno, countRemainingblocks(), bi.child_address, bi.size);
                MI_CpuCopy8(databuf, (void *)bi.child_address, bi.size);
                p_child->got_block++;
                setRecvFlag(hd.seqno);
            }

          CheckRemainBlock:
            /* �S�Ẵu���b�N���󂯎�ꂽ���H */
            if (0 == countRemainingblocks())
            {
                MBi_CommChangeChildState(MB_COMM_CSTATE_RECV_COMPLETE, NULL);   // ��M����
            }
        }
        break;

    case MB_COMM_TYPE_PARENT_BOOTREQ:
        if (p_child->c_comm_state == MB_COMM_CSTATE_RECV_COMPLETE)
        {
            MBi_CommChangeChildState(MB_COMM_CSTATE_BOOTREQ_ACCEPTED, NULL);
        }
        else if (p_child->c_comm_state == MB_COMM_CSTATE_BOOTREQ_ACCEPTED)
        {
            p_child->boot_end_flag = 1;
            (void)MBi_CommEnd();       // �ʐM���I��������
        }
        break;
    default:
        break;
    }
    return;
}

/*---------------------------------------------------------------------------*
  Name:         MBi_CommChildSendData

  Description:  �q�@�f�[�^���M

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/

static int MBi_CommChildSendData(void)
{
    u16     block_num = pCwork->total_block;
    MBCommChildBlockHeader hd;
    int     errcode = 0;
    u16     pollbmp = 0xffff;

    switch (pCwork->c_comm_state)
    {
    default:
        // MP�ʐM�m���̂��߁ADUMMY MP�𑗐M
        hd.type = MB_COMM_TYPE_DUMMY;
        (void)MBi_MakeChildSendBuffer(&hd, (u8 *)pCwork->common.sendbuf);
        errcode = MBi_BlockHeaderEnd(MB_COMM_CHILD_HEADER_SIZE, pollbmp, pCwork->common.sendbuf);
        break;

    case MB_COMM_CSTATE_REQ_ENABLE:
        {
            const MbBeaconRecvStatus *mbrsp = MB_GetBeaconRecvStatus();
            // FileRequest�̑��M
            errcode = MBi_CommRequestFile(pCwork->fileid,
                                          mbrsp->list[pCwork->connectTargetNo].gameInfo.ggid);
            MB_DEBUG_OUTPUT("Requested File (errcode:%d)\n", errcode);
            // RequestFile��MP���Z�b�g���Ă���
        }
        break;

    case MB_COMM_CSTATE_DLINFO_ACCEPTED:
        // DownloadInfo��̃��b�Z�[�W
        hd.type = MB_COMM_TYPE_CHILD_ACCEPT_FILEINFO;
        (void)MBi_MakeChildSendBuffer(&hd, (u8 *)pCwork->common.sendbuf);
        errcode = MBi_BlockHeaderEnd(MB_COMM_CHILD_HEADER_SIZE, pollbmp, pCwork->common.sendbuf);
        break;

    case MB_COMM_CSTATE_RECV_PROCEED:
        // �u���b�N�]���p�����b�Z�[�W
        MI_CpuClear8(&hd, sizeof(MBCommChildBlockHeader));
        hd.type = MB_COMM_TYPE_CHILD_CONTINUE;
        hd.data.req = get_next_blockno();
        hd.data.reserved[0] = (u8)(0x00ff & pCwork->got_block); // Lo
        hd.data.reserved[1] = (u8)((0xff00 & pCwork->got_block) >> 8);  // Hi
        (void)MBi_MakeChildSendBuffer(&hd, (u8 *)pCwork->common.sendbuf);
        errcode = MBi_BlockHeaderEnd(MB_COMM_CHILD_HEADER_SIZE, pollbmp, pCwork->common.sendbuf);
        break;

    case MB_COMM_CSTATE_RECV_COMPLETE:
        // �u���b�N�]����~���b�Z�[�W(�e����BOOTREQ������܂ő���Â���)
        hd.type = MB_COMM_TYPE_CHILD_STOPREQ;
        (void)MBi_MakeChildSendBuffer(&hd, (u8 *)pCwork->common.sendbuf);
        errcode = MBi_BlockHeaderEnd(MB_COMM_CHILD_HEADER_SIZE, pollbmp, pCwork->common.sendbuf);
        break;

    case MB_COMM_CSTATE_BOOTREQ_ACCEPTED:
        hd.type = MB_COMM_TYPE_CHILD_BOOTREQ_ACCEPTED;
        (void)MBi_MakeChildSendBuffer(&hd, (u8 *)pCwork->common.sendbuf);
        errcode = MBi_BlockHeaderEnd(MB_COMM_CHILD_HEADER_SIZE, pollbmp, pCwork->common.sendbuf);
        break;
    }

    return errcode;
}

/*---------------------------------------------------------------------------*
  Name:         MBi_CommBeaconRecvCallback

  Description:  �q�@�̃r�[�R����M�R�[���o�b�N

  Arguments:    msg    : �r�[�R����M���b�Z�[�W
                gInfop : �e�@�Q�[�����
                index  : �r�[�R���C���f�N�X

  Returns:      None.
 *---------------------------------------------------------------------------*/

static void MBi_CommBeaconRecvCallback(MbBeaconMsg msg, MBGameInfoRecvList * gInfop, int index)
{
#pragma unused(index)
    void* arg = (gInfop == NULL)? NULL : (void *)&gInfop->gameInfo;
    
    switch (msg)
    {
    case MB_BC_MSG_GINFO_VALIDATED:
        MBi_CommChangeChildStateCallbackOnly(MB_COMM_CSTATE_GAMEINFO_VALIDATED,
                                             arg);
        MB_DEBUG_OUTPUT("Parent Info Enable\n");
        break;
    case MB_BC_MSG_GINFO_INVALIDATED:
        MBi_CommChangeChildStateCallbackOnly(MB_COMM_CSTATE_GAMEINFO_INVALIDATED,
                                             arg);
        break;
    case MB_BC_MSG_GINFO_LOST:
        MBi_CommChangeChildStateCallbackOnly(MB_COMM_CSTATE_GAMEINFO_LOST,
                                             arg);
        break;
    case MB_BC_MSG_GINFO_LIST_FULL:
        MBi_CommChangeChildStateCallbackOnly(MB_COMM_CSTATE_GAMEINFO_LIST_FULL,
                                             arg);
        break;
    }
}

/*---------------------------------------------------------------------------*
  Name:         MB_ReadMultiBootParentBssDesc

  Description:  setup WMBssDesc structure information
                with MB_GetMultiBootParentBssDesc() data,
                in order to use by WM_StartConnect().

  Arguments:    p_desc            pointer to destination WMBssDesc
                parent_max_size   max packet length of parent in MP-protocol.
                                    (must be equal to parent's WMParentParam!)
                child_max_size    max packet length of child in MP-protocol.
                                    (must be equal to parent's WMParentParam!)
                ks_flag           if use key-sharing mode, TRUE.
                                    (must be equal to parent's WMParentParam!)
                cs_flag           if use continuous mode, TRUE.
                                    (must be equal to parent's WMParentParam!)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MB_ReadMultiBootParentBssDesc(WMBssDesc *p_desc,
                                   u16 parent_max_size, u16 child_max_size, BOOL ks_flag,
                                   BOOL cs_flag)
{
    const MBParentBssDesc *parentInfo = MB_GetMultiBootParentBssDesc();
    SDK_NULL_ASSERT(parentInfo);
    MI_CpuCopy8(parentInfo, p_desc, sizeof(MBParentBssDesc));

    p_desc->gameInfoLength = 0x10;
    p_desc->gameInfo.magicNumber = 0x0001;
    p_desc->gameInfo.ver = 0;
    p_desc->gameInfo.ggid =
        (u32)(*(const u16 *)(&p_desc->ssid[0]) | (*(const u16 *)(&p_desc->ssid[2]) << 16));
    p_desc->gameInfo.tgid = *(const u16 *)(&p_desc->ssid[4]);
    p_desc->gameInfo.userGameInfoLength = 0;
    p_desc->gameInfo.parentMaxSize = parent_max_size;
    p_desc->gameInfo.childMaxSize = child_max_size;
    MI_WriteByte(&p_desc->gameInfo.attribute,
                 (u8)((ks_flag ? WM_ATTR_FLAG_KS : 0) |
                      (cs_flag ? WM_ATTR_FLAG_CS : 0) | WM_ATTR_FLAG_ENTRY));
}


/*  ============================================================================

    miscellany functions

    ============================================================================*/

static BOOL checkRecvFlag(int x)
{
    int     pos = (x >> 3);
    int     bit = (x & 0x7);

    SDK_ASSERT(x < MB_MAX_BLOCK);

    if (pCwork->recvflag[pos] & (1 << bit))
    {
        return TRUE;
    }
    return FALSE;
}

static void setRecvFlag(int x)
{
    int     pos = (x >> 3);
    int     bit = (x & 0x7);

    SDK_ASSERT(x < MB_MAX_BLOCK);

    pCwork->recvflag[pos] |= (1 << bit);

    pCwork->last_recv_seq_no = x;
}


static u16 countRemainingblocks(void)
{
    return (u16)(pCwork->total_block - pCwork->got_block);
}

static u16 get_next_blockno(void)
{
    int     req;
    int     search_count = 0;
    req = pCwork->last_recv_seq_no;
    req++;

    while (1)
    {
        if (req < 0 || req >= pCwork->total_block)
        {
            req = 0;
        }
        else if (checkRecvFlag(req))
        {
            req++;
        }
        else
        {
            return (u16)req;
        }

        if (pCwork->last_recv_seq_no == req)
        {
            return (u16)(pCwork->total_block);
        }
        search_count++;

        if (search_count > 1000)
        {
            pCwork->last_recv_seq_no = req;
            return (u16)req;
        }
    }
}

// pollbmp�Ɏw�肵���q�@AID�̃r�b�g�������Ă��邩���`�F�b�N
static BOOL checkPollbmp(u16 pollbmp, u16 child_aid)
{
    if (pollbmp & (u16)(1 << child_aid))
    {
        return TRUE;
    }
    return FALSE;
}

/* ��M�̈���N���A */
static void clearRecvRegion(void)
{
    /* FINALROM�ȊO�̎��́A�_�E�����[�h�\�̈���N���A���Ȃ��悤�ɂ���D */
#ifdef SDK_FINALROM
    /* ARM9���[�h�\�̈���N���A */
    MI_CpuClearFast((void *)MB_LOAD_AREA_LO, MB_LOAD_MAX_SIZE);
    /* ARM7���[�h�o�b�t�@�A�h���X�̈���N���A */
    MI_CpuClearFast((void *)MB_ARM7_STATIC_RECV_BUFFER, MB_ARM7_STATIC_RECV_BUFFER_SIZE);
    /* MBbssDesc, MBDownloadFileInfo �̈���N���A */
    MI_CpuClear16((void *)MB_BSSDESC_ADDRESS,
                  MB_DOWNLOAD_FILEINFO_ADDRESS + MB_DOWNLOAD_FILEINFO_SIZE - MB_BSSDESC_ADDRESS);
    /* ROM Header �̈���N���A */
    MI_CpuClear16((void *)HW_ROM_HEADER_BUF, ROM_HEADER_SIZE_FULL);
#endif

}


static void MBi_CommCallChildError(u16 errcode)
{
    MBErrorStatus e_stat;
    e_stat.errcode = errcode;

    MBi_CommChangeChildStateCallbackOnly(MB_COMM_CSTATE_ERROR, &e_stat);

}
