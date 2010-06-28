//=============================================================================
/**
 * @file  comm_command_oekaki.c
 * @brief �f�[�^�𑗂邽�߂̃R�}���h���e�[�u�������Ă��܂�
 *        ���G�����{�[�h�p�ł�
 * @author  Akito Mori
 * @date    2010.01.20
 */
//=============================================================================

#include <gflib.h>
#include "net/network_define.h"
#include "arc_def.h"

#include "print/wordset.h"
#include "system/bmp_menu.h"
#include "system/net_err.h"
#include "print/wordset.h"
#include "net_app/oekaki.h"
#include "oekaki_local.h"
#include "comm_command_oekaki.h"



static u8* _setPictureBuff( int netID, void* pWk, int size);
static int _getCRECW(void);
static int _getCOECW(void);

static void Oekaki_GraphicDataSend( OEKAKI_WORK *wk, int no, GFL_NETHANDLE *pNet);


static void CommOekakiBoardPicture(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void CommOekakiBoardLinePos(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void CommOekakiBoardLinePosServer(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void CommOekakiBoardStop(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void CommOekakiBoardReStart(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void CommOekakiBoardEndChild(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void CommOekakiBoardEnd(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void CommOekakiBoardEndChildDecide(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);


// CommCommandOekaki.c�����`����R�}���h�őΉ�����R�[���o�b�N�֐�
const NetRecvFuncTable _OekakiCommPacketTbl[] = {
  // ���G���������镔��
  {CommOekakiBoardPicture,        _setPictureBuff}, // CO_OEKAKI_GRAPHICDATA  �݂�Ȃŕ`���Ă����摜�f�[�^
  {CommOekakiBoardLinePos,        NULL, },          // CO_OEKAKI_LINEPOS,   �^�b�`�p�l���Ŏ擾�����|�W�V�����f�[�^
  {CommOekakiBoardLinePosServer,  NULL  },          // CO_OEKAKI_LINEPOS,   �^�b�`�p�l���Ŏ擾�����|�W�V�����f�[�^
  {CommOekakiBoardStop,           NULL  },          // CO_OEKAKI_STOP,      �����҂������̂ň�U�X�g�b�v
  {CommOekakiBoardReStart,        NULL  },          // CO_OEKAKI_RESTART,   �����ҏ������I������̂ōĉ�
  {CommOekakiBoardEndChild,       NULL  },          // CO_OEKAKI_END_CHILD,   �q�@�����E
  {CommOekakiBoardEnd,            NULL  },          // CO_OEKAKI_END,     �I��
  {CommOekakiBoardEndChildDecide, NULL  },          // CO_OEKAKI_END_CHILD_DECIDE, �q�@�����E�V�[�P���X�Ɉڂ���
};


//==============================================================================
/**
 * ���G�����{�[�h�p�ʐM�R�}���h�o�^����
 *
 * @param   pWk   OEKAKI_WORK*
 *
 * @retval  none
 */
//==============================================================================
void OEKAKIBOARD_CommandInitialize( void* pWk )
{
  GFL_NET_AddCommandTable( GFL_NET_CMD_PICTURE,_OekakiCommPacketTbl,
                           NELEMS(_OekakiCommPacketTbl), pWk);
}


//==============================================================================
/**
 * �e�@���ڑ�����܂łɂ�����Ă����摜����C�ɑ��M����
 * 
 * 
 * @param   netID   
 * @param   size    
 * @param   pData   
 * @param   pWk   
 *
 * @retval  none    
 */
//==============================================================================
static void CommOekakiBoardPicture(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  OEKAKI_WORK *wk = (OEKAKI_WORK *)pWk;

  // �q�@�̏ꍇ�͐e�@�����������摜�f�[�^����荞��
  if(GFL_NET_SystemGetCurrentID()!=0){
    OEKAKIG_SPLIT_DATA *osd = (OEKAKIG_SPLIT_DATA *)pData;
    OS_Printf("�摜�擾 no=%d, xor=%08x\n",osd->no, osd->_xor);
    if(osd->no*1000 > OEKAKI_GRAPHI_SIZE){
      MI_CpuCopyFast( osd->chara, &wk->canvas_buf[osd->no*1000], OEKAKI_GRAPHI_SIZE%1000);
    }else{
      MI_CpuCopyFast( osd->chara, &wk->canvas_buf[osd->no*1000],  1000);
    }
    // BMPWIN�̃o�b�t�@�ɃR�s�[����
    {
      u8 *adr = GFL_BMP_GetCharacterAdrs( GFL_BMPWIN_GetBmp(wk->OekakiBoard) );
      MI_CpuCopyFast( wk->canvas_buf, adr, OEKAKI_GRAPHI_SIZE);
    }
    // ��ʂɔ��f
    GFL_BMPWIN_MakeTransWindow( wk->OekakiBoard );

  // �e�@�͎��̃f�[�^�𑗐M����
  }else{
    OEKAKIG_SPLIT_DATA *osd = (OEKAKIG_SPLIT_DATA *)pData;
    OS_Printf("�摜�擾 no=%d, xor=%08x",osd->no, osd->_xor);
  
    if(wk->send_num*1000 < OEKAKI_GRAPHI_SIZE){
      wk->send_num++;
      Oekaki_GraphicDataSend(wk, wk->send_num, pNetHandle);
    }else{
      // �ĊJ
      Oekaki_SendDataRequest( wk, CO_OEKAKI_RESTART, 0 );
      
    }
  }

}


//==============================================================================
/**
 * �J�[�\���ʒu������M�����Ƃ��̃R�[���o�b�N
 *
 * @param   netID   
 * @param   size    
 * @param   pData   
 * @param   pWk   
 *
 * @retval  none    
 */
//==============================================================================
static void CommOekakiBoardLinePos(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  OEKAKI_WORK *wk     = (OEKAKI_WORK*)pWk;
  TOUCH_INFO  *result = (TOUCH_INFO*)pData;

  // OS_Printf("id[%d]line pos recv\n", netID);
  // �q�@���������f�[�^���i�[����
  if(netID!=0){
//  wk->ParentTouchResult[netID] = *result;
    OekakiTouchFifo_Set( result, &wk->TouchFifo, netID );
  }

}



//==============================================================================
/**
 * @brief   ���C���f�[�^��e�@�����M�����i�S���`��)
 *
 * @param   netID   
 * @param   size    
 * @param   pData   
 * @param   pWk   
 *
 * @retval  none    
 */
//==============================================================================
static void CommOekakiBoardLinePosServer(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  OEKAKI_WORK *wk     = (OEKAKI_WORK*)pWk;
  TOUCH_INFO  *result = (TOUCH_INFO*)pData;

    if(wk==NULL){  // ���̂Ƃ��͂��G�����������ł��Ă��Ȃ��̂Ŗ���
        return;
    }

    
  if(netID==0){ // ��΂ɐe���炵�����Ȃ��͂�����
    int i;
    for(i=0;i<OEKAKI_MEMBER_MAX;i++){
      wk->AllTouchResult[i] = result[i];
    }
  }
  
  if(wk->AllTouchResult[0].banFlag==OEKAKI_BAN_ON){
    OS_TPrintf("�e�@���瑀��֎~����\n");
  }
}
//==============================================================================
/**
 * �ʐM��ʂ��J�n����
 *
 * @param   netID   �ʐMID
 * @param   size    �f�[�^�T�C�Y
 * @param   pData   ��M�f�[�^�|�C���^
 * @param   pWk     OEKAKI_WORK*
 *
 * @retval  none    
 */
//==============================================================================
static void CommOekakiBoardReStart(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  OEKAKI_WORK *wk = (OEKAKI_WORK*)pWk;

  OS_Printf("�e�@���炨�G�����ĊJ�ʒm\n");
  OekakiBoard_MainSeqForceChange( wk, OEKAKI_MODE_NEWMEMBER_END, 0 );
  
  MI_CpuClearFast( wk->canvas_buf, OEKAKI_GRAPHI_SIZE );
  
  // �G�����L�����ڑ��l�����X�V
  wk->shareNum = Union_App_GetMemberNum( wk->param->uniapp );
  wk->shareBit = Union_App_GetMemberNetBit(wk->param->uniapp);
  wk->banFlag  = OEKAKI_BAN_OFF;
  OS_Printf("shareBit�X�V=%d\n", wk->shareBit);

}
//==============================================================================
/**
 * @brief   �q�@�����E
 *
 * @param   netID   
 * @param   size    
 * @param   pData   ���E�҂�ID(�e����̑��M�ōŏ�ʃr�b�g�������Ă���ꍇ�͗��ENG)
 * @param   pWk   OEKAKI_WORK*
 *
 * @retval  none    
 */
//==============================================================================
static void CommOekakiBoardEndChild(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  OEKAKI_WORK *wk = (OEKAKI_WORK*)pWk;
  COMM_OEKAKI_END_CHILD_WORK *recieve_work = (COMM_OEKAKI_END_CHILD_WORK*)pData;
  u32 ret;

  // �e�@���炫���ꍇ�݂͂�ȂŁu�����������Ȃ��Ȃ�܂����v�Ƃ���
  if(netID!=0){
    //�q�@(���E��)����̑��M
    //���E���܂��ʒm
    if(GFL_NET_SystemGetCurrentID()==0){
      // �q�@����(�e�@���󂯎��)
      // �q�@�����Ȃ��Ȃ�������S���ɒʒm����
      wk->send_req.trans_work = *recieve_work;
      wk->send_req.trans_work.ridatu_id = netID;
      wk->send_req.trans_work.oya_share_num = wk->shareNum;
      switch(recieve_work->request){
      case COEC_REQ_RIDATU_CHECK:
        if(wk->shareNum != Union_App_GetMemberNum( wk->param->uniapp ) 
            || wk->shareNum != MATH_CountPopulation(Union_App_GetMemberNetBit(wk->param->uniapp))){
          wk->send_req.trans_work.ridatu_kyoka = FALSE;  //���ENG�I
        }
        else{
//          wk->ridatu_bit |= 1 << netID;
          wk->send_req.trans_work.ridatu_kyoka = TRUE;
          //���EOK�Ȃ̂ŎQ��������������(����������΂����瑤�Ő������͂������͂�)
          Union_App_Parent_ResetEntryBlock( wk->param->uniapp);   // ����NG
        }
        break;
      case COEC_REQ_RIDATU_EXE:
        break;
      }
      Oekaki_SendDataRequest( wk, CO_OEKAKI_END_CHILD, 0 );

      OS_Printf("�q�@%d���痣�E���󂯎���������M\n",netID);
    }
  
  }else{
  // �e�@����q�@���E�̒ʒm������
    switch(recieve_work->request){
    case COEC_REQ_RIDATU_CHECK:
      //���E�m�F�Ȃ̂ŁA���E���悤�Ƃ����q�@�ɂ̂݌��ʂ𑗂�
      if(recieve_work->ridatu_id == GFL_NET_SystemGetCurrentID()){
        if(recieve_work->ridatu_kyoka == FALSE){
          OekakiBoard_MainSeqForceChange( wk, OEKAKI_MODE_END_SELECT_ANSWER_NG, recieve_work->ridatu_id );
          OS_Printf("���ENG����M\n");
        }
        else{
          wk->oya_share_num = recieve_work->oya_share_num;
          OekakiBoard_MainSeqForceChange( wk, OEKAKI_MODE_END_SELECT_ANSWER_OK, recieve_work->ridatu_id );
          wk->status_end = TRUE;
          OS_Printf("���EOK����M\n");
        }
      }
      break;
    case COEC_REQ_RIDATU_EXE:
      OS_TPrintf("�e�@���q�@%d�̗��E�ʒm����\n", recieve_work->ridatu_id);
      // ���������E���Ă���Ȃ�؂�ւ���q�@�̗��E�҂��ɐ؂�ւ���K�v�͖���
      OS_Printf("wk->status_end=%d\n",wk->status_end);
      if(wk->status_end){
        OS_Printf("���ɗ��E�҂��Ȃ̂Ől�̗��E������҂K�v������\n");
      }else{
        OekakiBoard_MainSeqForceChange( wk, OEKAKI_MODE_LOGOUT_CHILD, recieve_work->ridatu_id );
      }
      break;
    }
  }
}


//==============================================================================
/**
 * @brief   �q�@���{���ɗ��E���J�n����(�����ǂ�Ȏ��������Ă��q�@�̃V�[�P���X�͏����ς��Ȃ��j
 *
 * @param   netID 
 * @param   size  
 * @param   pData 
 * @param   pWk   OEKAKI_WORK*
 *
 * @retval  none    
 */
//==============================================================================
static void CommOekakiBoardEndChildDecide(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  OEKAKI_WORK *wk = (OEKAKI_WORK*)pWk;

  // �e�@�͗��E�q�@��bit���ŕۑ�(���̎q�@�͐�΂ɔ�����j
  if(GFL_NET_SystemGetCurrentID()==0){
    wk->ridatu_bit |= 1 << netID;
    OS_Printf("�q�@%d����w��Δ����܂��I�x���Ă�����\n", netID);
  }
}


//==============================================================================
/**
 * @brief   �u�q�@���������Ă����̂ň�U�G�𑗂��~�܂��Ăˁv
 *      �Ɛe�@�����M���Ă������̃R�[���o�b�N
 *
 * @param   netID   
 * @param   size    
 * @param   pData   
 * @param   pWk   
 *
 * @retval  none    
 */
//==============================================================================
static void CommOekakiBoardStop(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{

  OEKAKI_WORK *wk = (OEKAKI_WORK*)pWk;
  u8 id;

  // �����I�����������牽�����Ȃ�
  if(wk->force_end){
    return;
  }

  id = *(u8*)pData;
  // ���C���V�[�P���X�ύX
  OekakiBoard_MainSeqForceChange( wk, OEKAKI_MODE_NEWMEMBER, id );

  // �e�@���摜�f�[�^���M���J�n����
  if(GFL_NET_SystemGetCurrentID()==0 && wk->proc_seq == SEQ_MAIN){
    wk->send_num = 0;

    //�O���t�B�b�N�������M
    Oekaki_GraphicDataSend(wk, wk->send_num, pNetHandle);
  }


  OS_Printf("�e�@����́u�q�@%d�ɊG�𑗂邩��~�܂��Ăˁv�ʒm\n",id);

}


static int _shareBit_count( int shareBit )
{
  int i,count=0;
  for(i=0;i<5;i++){
    if(shareBit&1){
      count++;
    }
    shareBit>>=1;
  }
  return count;
}

//==============================================================================
/**
 * @brief   �e�@����߂�̂ŋ����I��������
 *
 * @param   netID   
 * @param   size    
 * @param   pData   
 * @param   pWk   
 *
 * @retval  none    
 */
//==============================================================================
static void CommOekakiBoardEnd(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  OEKAKI_WORK *wk = (OEKAKI_WORK*)pWk;
  OS_Printf("�e�@����̏I���ʒm�������̂ł�߂�\n");

  // ���������𖳎�����
  wk->force_end = TRUE;
  
  OS_Printf("share�l��=%d\n",_shareBit_count(wk->shareBit));
  OS_Printf("union_app�l��=%d\n",Union_App_GetMemberNum(wk->param->uniapp));
  // �e�@�ȊO�͂��̃R�}���h�ŋ������イ��傤����
  if(GFL_NET_SystemGetCurrentID() != 0){
    OekakiBoard_MainSeqForceChange( wk, OEKAKI_MODE_FORCE_END, 0  );
  }
}


//------------------------------------------------------------------
/**
 * @brief   �O���t�B�b�N�f�[�^�𕪊����M
 *
 * @param   wk    
 * @param   no    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void Oekaki_GraphicDataSend( OEKAKI_WORK *wk, int no, GFL_NETHANDLE *pNet)
{
  // ���M�o�b�t�@�ɃR�s�[
  u8 sendbit;
  u8 *p = GFL_BMP_GetCharacterAdrs( GFL_BMPWIN_GetBmp(wk->OekakiBoard) );
  MI_CpuCopyFast( &p[no*1000], wk->send_buf.chara, 1000 ); 

  // xor�擾�E�i�[
  {
    int i;
    u32 *p,result;
    p = (u32*)wk->send_buf.chara;
    for(result=0,i=0;i<1000/4;i++){
      result ^= p[i];
    }
    wk->send_buf._xor        = result;
  }

  // �����ԍ�����������
  wk->send_buf.no = no;
  
  
  // ���M�J�n
  sendbit = Union_App_GetMemberNetBit( wk->param->uniapp );
  GFL_NET_COMMAND_SendHugeData( GFL_NET_GetNetHandle( GFL_NET_NETID_SERVER), sendbit, 
                                CO_OEKAKI_GRAPHICDATA, sizeof(OEKAKIG_SPLIT_DATA),&wk->send_buf);
  OS_TPrintf("sendbit=%d, systemConnect=%d\n", sendbit, GFL_NET_SystemGetConnectNum());

  OS_Printf("���M�f�[�^ no=%d, xor = %08x\n",  wk->send_buf.no,  wk->send_buf._xor);
}




//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
// �p���b�g�T�C�Y�ʒm�֐��Q
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------



//--------------------------------------------------------------------------------
// ��M�f�[�^�T�C�Y��Ԃ��֐�
//--------------------------------------------------------------------------------
static u8* _setPictureBuff( int netID, void* pWk, int size)
{
  OEKAKI_WORK *wk = (OEKAKI_WORK*)pWk;

  return (u8*)&wk->split_temp[netID];
}

//--------------------------------------------------------------------------------
// �o�C�g��\���֐�
//--------------------------------------------------------------------------------
static int _getCRECW(void)
{
  return 4;
}

//--------------------------------------------------------------------------------
// �o�C�g��\���֐�
//--------------------------------------------------------------------------------
static int _getCOECW(void)
{
  return sizeof(COMM_OEKAKI_END_CHILD_WORK);
}
