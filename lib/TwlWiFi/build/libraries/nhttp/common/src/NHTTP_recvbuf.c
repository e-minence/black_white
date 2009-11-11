/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - NHTTP - libraries
  File:     NHTTP_recvbuf.c

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 1024 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/
#include "nhttp.h"
#include "NHTTPlib_int.h"
#include "NHTTPlib_int_socket.h"
#include "NHTTP_recvbuf.h"
#include "NHTTP_nonport.h"

/*---------------------------------------------------------------------------*
    funcition prototype
 *---------------------------------------------------------------------------*/
static void setupGetcharFromHdrRecvBuf  ( const NHTTPRes* res_p, int pos, NHTTPi_HDRBUFLIST** hdrbuf_pp, int* blockpos_p );

static char getcharFromHdrRecvBuf       ( const NHTTPRes* res_p, NHTTPi_HDRBUFLIST** hdrbuf_pp, int* blockpos_p );

/*---------------------------------------------------------------------------*
    Constants
 *---------------------------------------------------------------------------*/
#define CR      0x0d
#define LF      0x0a
#define COLON   0x3a

/*-------------------------------------------------------------------------*
  Name:         setupGetcharFromHdrRecvBuf
  Description:  �G���e�B�e�B�w�b�_�̎�M�o�b�t�@�ɂ��āA�ǂݏo���_��o�^����                
  Arguments:    res_p
                pos
                hdrbuf_pp
                blockpos_p
  Return:       �Ȃ�
 *-------------------------------------------------------------------------*/
static void setupGetcharFromHdrRecvBuf(const NHTTPRes* res_p, int pos, NHTTPi_HDRBUFLIST** hdrbuf_pp, int* blockpos_p)
{
    NHTTPi_HDRBUFLIST*  a_hdrbuf_p;
    int                 blockidx;
    
    if (pos < NHTTP_HDRRECVBUF_INILEN)
    {
        *hdrbuf_pp = NULL;
        *blockpos_p = pos;
        return;
    }
    
    pos -= NHTTP_HDRRECVBUF_INILEN;
    blockidx = pos >> NHTTP_HDRRECVBUF_BLOCKSHIFT;
    
    for (a_hdrbuf_p = res_p->hdrBufBlock_p;  blockidx--;)
    {
        a_hdrbuf_p = a_hdrbuf_p->next_p;
    }
    
    pos &= NHTTP_HDRRECVBUF_BLOCKMASK;
    *hdrbuf_pp = a_hdrbuf_p;
    *blockpos_p = pos;
}

/*-------------------------------------------------------------------------*
  Name:         getcharFromHdrRecvBuf
  Description:  �G���e�B�e�B�w�b�_�̎�M�o�b�t�@���A���݂̓ǂݏo���_����ǂݏo�� 
  Arguments:    res_p     
                hdrbuf_pp
                blockpos_p
 *-------------------------------------------------------------------------*/
static char getcharFromHdrRecvBuf(const NHTTPRes* res_p, NHTTPi_HDRBUFLIST** hdrbuf_pp, int* blockpos_p)
{
    if (!(*hdrbuf_pp))
    {
        if (*blockpos_p < NHTTP_HDRRECVBUF_INILEN)
        {
            return res_p->hdrBufFirst[(*blockpos_p)++];
        }
        *blockpos_p = 0;
        *hdrbuf_pp = res_p->hdrBufBlock_p;
    }
    else
    {
        if (*blockpos_p == NHTTP_HDRRECVBUF_BLOCKLEN)
        {
            *blockpos_p = 0;
            *hdrbuf_pp = (*hdrbuf_pp)->next_p;
        }
    }
    
    return (*hdrbuf_pp)->block[(*blockpos_p)++];
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_findNextLineHdrRecvBuf
  Description:  �G���e�B�e�B�w�b�_��M�o�b�t�@���Apos���玟�̉��s����̈ʒu�𓾂�
                ���łɍŏ��́u:�v�̈ʒu������
  Arguments:    res_p
                pos
                limit		�Ō�ɒT�����镶���C���f�b�N�X+1
                colon_pos_p
                return_code_size_p
  Returns:      0  ���s��͏I�[
                <0 ���s�Ȃ�               
 *-------------------------------------------------------------------------*/
extern int NHTTPi_findNextLineHdrRecvBuf(const NHTTPRes* res_p, int pos, const int limit, int* colon_pos_p, int* return_code_size_p)
{
    NHTTPi_HDRBUFLIST* hdrbuf_p;
    int                blockpos;
    char               c;
    
    if (colon_pos_p)
    {
        *colon_pos_p = -1;
    }
        
    if (pos < limit)
    {
        int return_pos = -1;
        int findCR     = FALSE;
        
        setupGetcharFromHdrRecvBuf(res_p, pos, &hdrbuf_p, &blockpos);
        
        for (; pos < limit; pos++)
        {
            c = getcharFromHdrRecvBuf(res_p, &hdrbuf_p, &blockpos);

            // : �̈ʒu���o���Ă���
            if ((c == COLON) && (colon_pos_p) && (*colon_pos_p < 0))
            {
                *colon_pos_p = pos;
            }

            // ���łɑO��̃��[�v��CR�𔭌��ς݂̏ꍇ�͂����Ŕ�����
            if (findCR)
            {
                // LF�𔭌�
                if (c == LF)
                {
                    return_pos = (pos == (limit-1)) ? 0 : (pos+1);
                    if( return_code_size_p )
                    {
                        *return_code_size_p = 2;
                    }
                }
                return return_pos;
            }
            
            // CR�𔭌�
            if (c == CR)
            {
                // �Ԃ��ꏊ���o���Ă���
                return_pos = (pos == (limit-1))? 0 : (pos+1);
                findCR = TRUE;
                if (return_code_size_p)
                {
                    *return_code_size_p = 1;
                }
            }
            
            // LF�𔭌�
            if (c == LF)
            {
                return_pos = (pos == (limit-1))? 0 : (pos+1);
                if (return_code_size_p)
                {
                    *return_code_size_p = 1;
                }
                return return_pos;
            }
        }
    }
    
    return -1;
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_skipSpaceHdrRecvBuf
  Description:  �G���e�B�e�B�w�b�_�̎�M�o�b�t�@���Apos�����(0x20)��ǂݔ�΂�
  Arguments:    res_p	���X�|���X�\���̂̃|�C���^
                pos
                limit	�Ō�ɒT�����镶���C���f�b�N�X+1
  Returns:      0  pos
                < 0 ���s�Ȃ�               
 *-------------------------------------------------------------------------*/
extern int NHTTPi_skipSpaceHdrRecvBuf(const NHTTPRes* res_p, int pos, const int limit)
{
    NHTTPi_HDRBUFLIST*  hdrbuf_p;
    int                 blockpos;
    
    if (pos < limit)
    {
        setupGetcharFromHdrRecvBuf(res_p, pos, &hdrbuf_p, &blockpos);
        
        for (;  pos < limit;  pos++)
        {
            if (getcharFromHdrRecvBuf(res_p, &hdrbuf_p, &blockpos) != ' ')
            {
                return pos;
            }
        }
    }
    
    return -1;
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_compareTokenN_HdrRecvBuf

  Description:  �G���e�B�e�B�w�b�_�̎�M�o�b�t�@���Apos����A�P����r����
                �啶���Ə������͋�ʂ���Ȃ�
  Arguments:    res_p
                pos
                limit	�Ō�ɒT�����镶���C���f�b�N�X+1
                str_p
                extdeli	�󔒈ȊO�̃f���~�^����
  Returns:      0  
                <0             
 *-------------------------------------------------------------------------*/
extern int NHTTPi_compareTokenN_HdrRecvBuf(const NHTTPRes* res_p, int pos, const int limit, const char* str_p, const char extdeli)
{
    NHTTPi_HDRBUFLIST* hdrbuf_p;
    int                blockpos;
    char               c;

    if (pos < limit)
    {
        setupGetcharFromHdrRecvBuf(res_p, pos, &hdrbuf_p, &blockpos);
        
        for (c = getcharFromHdrRecvBuf(res_p, &hdrbuf_p, &blockpos);  LOWERCASE(c) == LOWERCASE(*str_p);  pos++, str_p++)
        {
            if ((*str_p == 0) || (*str_p == ' ') || (*str_p == extdeli) || (pos == (limit-1)))
            {
                return 0;
            }
            
            c = getcharFromHdrRecvBuf(res_p, &hdrbuf_p, &blockpos);
        }
    }
    return -1;
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_compareTokenN_HdrRecvBuf
  Description:  �G���e�B�e�B�w�b�_��M�o�b�t�@��pos����Adst�ɓ��e���R�s�[����
                �R�s�[����len�ɖ����Ȃ���Ύ��s��Ԃ�
  Arguments:    res_p
                dst_p
                pos
                len
  Returns:      TRUE	�����ɐ�������
                FALSE	�����Ɏ��s����
 *-------------------------------------------------------------------------*/
extern int NHTTPi_loadFromHdrRecvBuf(const NHTTPRes* res_p, char* dst_p, int pos, int len)
{
    NHTTPi_HDRBUFLIST*  hdrbuf_p;
    int                 blockidx;
    int                 blocklen;
    
    if ((pos+len) <= (int)(res_p->headerLen))
    {
        if (len)
        {
            if (pos < NHTTP_HDRRECVBUF_INILEN)
            {
                blocklen = MIN(len, NHTTP_HDRRECVBUF_INILEN - pos);
                NHTTPi_memcpy(dst_p, &res_p->hdrBufFirst[pos], blocklen);
                pos += blocklen;
                len -= blocklen;
                dst_p += blocklen;
            }
            
            if (len)
            {
                pos -= NHTTP_HDRRECVBUF_INILEN;
                blockidx = pos >> NHTTP_HDRRECVBUF_BLOCKSHIFT;
                pos &= NHTTP_HDRRECVBUF_BLOCKMASK;
                
                for (hdrbuf_p = res_p->hdrBufBlock_p;  blockidx--;)
                {
                    hdrbuf_p = hdrbuf_p->next_p;
                }
                
                while (len)
                {
                    blocklen = MIN(len, NHTTP_HDRRECVBUF_BLOCKLEN - pos);
                    NHTTPi_memcpy(dst_p, &hdrbuf_p->block[pos], blocklen);
                    hdrbuf_p = hdrbuf_p->next_p;
                    pos += blocklen;
                    pos &= NHTTP_HDRRECVBUF_BLOCKMASK;
                    len -= blocklen;
                    dst_p += blocklen;
                }
            }
        }
        return TRUE;
    }
    
    return FALSE;
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_isRecvBufFull
  Description:  pos���G���e�B�e�B�{�f�B�̎�M�o�b�t�@���͂ݏo���Ă�����TRUE               
  Arguments:    res_p
                pos
  Returns:
 *-------------------------------------------------------------------------*/
extern int NHTTPi_isRecvBufFull(const NHTTPRes* res_p, const int pos)
{
    return (res_p->recvBufLen <= (unsigned long)pos);
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_RecvBuf
  Description:  socket����G���e�B�e�B�{�f�B�̎�M�o�b�t�@��recv����
  Arguments:    req_p     ���N�G�X�g�\���̂ւ̃|�C���^
                socket
                pos
                flags
  Returns:      
 *-------------------------------------------------------------------------*/
extern int NHTTPi_RecvBuf(NHTTPMutexInfo* mutexInfo_p, NHTTPReq* req_p, const SOCKET socket, const int pos, const int flags)
{
    NHTTPRes* res_p = req_p->res_p;
    
    return NHTTPi_SocRecv(mutexInfo_p, req_p, socket, &res_p->recvBuf_p[pos], (int)(res_p->recvBufLen - pos), flags);
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_RecvBufN
  Description:  socket����G���e�B�e�B�{�f�B�̎�M�o�b�t�@�ɍő�len[byte]����recv����
  Arguments:    req_p     ���N�G�X�g�\���̂ւ̃|�C���^
                socket
                pos
                len
                flags                
  Returns:      
 *-------------------------------------------------------------------------*/
extern int NHTTPi_RecvBufN(NHTTPMutexInfo* mutexInfo_p, NHTTPReq* req_p, const SOCKET socket, const int pos, int len, const int flags)
{
    NHTTPRes*   res_p = req_p->res_p;
    
    if (NHTTPi_isRecvBufFull(res_p, pos))
    {
        return NHTTPi_NB_ERROR_BUFFERFULL;
    }
    
    len = MIN(len, (int)(res_p->recvBufLen - pos));
    
    return NHTTPi_SocRecv(mutexInfo_p, req_p, socket, &res_p->recvBuf_p[pos], len, flags);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

