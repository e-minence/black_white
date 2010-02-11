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
#include "print/wordset.h"

//#include "field/fieldobj.h"
//#include "field/comm_union_beacon.h"
//#include "field/comm_union_view_common.h"



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
static void CommOekakiBoardChildJoin(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void CommOekakiBoardFreeze(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);



// CommCommandOekaki.c�����`����R�}���h�őΉ�����R�[���o�b�N�֐�
const NetRecvFuncTable _OekakiCommPacketTbl[] = {
  // ���G���������镔��
  {CommOekakiBoardPicture,       _setPictureBuff}, // CO_OEKAKI_GRAPHICDATA  �݂�Ȃŕ`���Ă����摜�f�[�^
  {CommOekakiBoardLinePos,       NULL, },          // CO_OEKAKI_LINEPOS,   �^�b�`�p�l���Ŏ擾�����|�W�V�����f�[�^
  {CommOekakiBoardLinePosServer, NULL  },          // CO_OEKAKI_LINEPOS,   �^�b�`�p�l���Ŏ擾�����|�W�V�����f�[�^
  {CommOekakiBoardStop,          NULL  },          // CO_OEKAKI_STOP,      �����҂������̂ň�U�X�g�b�v
  {CommOekakiBoardReStart,       NULL  },          // CO_OEKAKI_RESTART,   �����ҏ������I������̂ōĉ�
  {CommOekakiBoardEndChild,      NULL  },          // CO_OEKAKI_END_CHILD,   �q�@�����E
  {CommOekakiBoardEnd,           NULL  },          // CO_OEKAKI_END,     �I��
  {CommOekakiBoardChildJoin,     NULL  },          // CO_OEKAKI_CHILD_JOIN   �q�@��������錾
  {CommOekakiBoardFreeze,        NULL  },          // CO_OEKAKI_OUT_CONTROL  ���E�֎~����
};

//  {CommOekakiBoardPicture,       _getPictureSize, _setPictureBuff}, // CO_OEKAKI_GRAPHICDATA  �݂�Ȃŕ`���Ă����摜�f�[�^
//  {CommOekakiBoardLinePos,       _getLinePosSize   },               // CO_OEKAKI_LINEPOS,   �^�b�`�p�l���Ŏ擾�����|�W�V�����f�[�^
//  {CommOekakiBoardLinePosServer, _getLinePosServerSize},            // CO_OEKAKI_LINEPOS,   �^�b�`�p�l���Ŏ擾�����|�W�V�����f�[�^
//  {CommOekakiBoardStop,          _getOne  },                        // CO_OEKAKI_STOP,      �����҂������̂ň�U�X�g�b�v
//  {CommOekakiBoardReStart,       _getZero },                        // CO_OEKAKI_RESTART,   �����ҏ������I������̂ōĉ�
//  {CommOekakiBoardEndChild,      _getCOECW},                        // CO_OEKAKI_END_CHILD,   �q�@�����E
//  {CommOekakiBoardEnd,           _getZero },                        // CO_OEKAKI_END,     �I��
//  {CommOekakiBoardChildJoin,     _getZero },                        // CO_OEKAKI_CHILD_JOIN   �q�@��������錾
//  {CommOekakiBoardFreeze,        _getZero },                        // CO_OEKAKI_OUT_CONTROL  ���E�֎~����



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
//      CommSendData_ServerSide( CO_OEKAKI_RESTART, NULL, 0);
      u32 ret;
      ret=GFL_NET_SendData( GFL_NET_GetNetHandle( GFL_NET_NETID_SERVER), CO_OEKAKI_RESTART, 0, NULL);
      
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

  OS_Printf("id[%d]line pos recv\n", netID);
  // �q�@���������f�[�^���i�[����
  if(netID!=0){
    wk->ParentTouchResult[netID] = *result;
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
 * @param   netID   
 * @param   size    
 * @param   pData   
 * @param   pWk   
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
  
  if(GFL_NET_SystemGetCurrentID()==0){
    // �G�����L�����ڑ��l�����X�V
    wk->shareNum = Union_App_GetMemberNum( wk->param->uniapp );
    wk->shareBit = Union_App_GetMemberNetBit(wk->param->uniapp);
    wk->banFlag  = OEKAKI_BAN_OFF;
  }
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
  COMM_OEKAKI_END_CHILD_WORK trans_work;
  COMM_OEKAKI_END_CHILD_WORK *recieve_work = (COMM_OEKAKI_END_CHILD_WORK*)pData;
  u32 ret;

  // �e�@���炫���ꍇ�݂͂�ȂŁu�����������Ȃ��Ȃ�܂����v�Ƃ���
  if(netID!=0){
    //�q�@(���E��)����̑��M
    //���E���܂��ʒm
    if(GFL_NET_SystemGetCurrentID()==0){
      // �q�@����(�e�@���󂯎��)
      // �q�@�����Ȃ��Ȃ�������S���ɒʒm����
      trans_work = *recieve_work;
      trans_work.ridatu_id = netID;
      trans_work.oya_share_num = wk->shareNum;
      switch(recieve_work->request){
      case COEC_REQ_RIDATU_CHECK:
        if(wk->shareNum != Union_App_GetMemberNum( wk->param->uniapp ) 
            || wk->shareNum != MATH_CountPopulation(Union_App_GetMemberNetBit(wk->param->uniapp))){
          trans_work.ridatu_kyoka = FALSE;  //���ENG�I
        }
        else{
          wk->ridatu_bit |= 1 << netID;
          trans_work.ridatu_kyoka = TRUE;
          //���EOK�Ȃ̂ŎQ��������������(����������΂����瑤�Ő������͂������͂�)
//          CommStateSetLimitNum(Union_App_GetMemberNum( wk->param->uniapp ));
            Union_App_Parent_ResetEntryBlock( wk->param->uniapp);   // ����OK
        }
        break;
      case COEC_REQ_RIDATU_EXE:
        break;
      }

//    CommSendData_ServerSide( CO_OEKAKI_END_CHILD, &trans_work, 
//        sizeof(COMM_OEKAKI_END_CHILD_WORK) );
      ret=GFL_NET_SendData( GFL_NET_GetNetHandle( GFL_NET_NETID_SERVER), CO_OEKAKI_END_CHILD, 
                            sizeof(COMM_OEKAKI_END_CHILD_WORK), &trans_work);

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
        }
        else{
          wk->oya_share_num = recieve_work->oya_share_num;
          OekakiBoard_MainSeqForceChange( wk, OEKAKI_MODE_END_SELECT_ANSWER_OK, recieve_work->ridatu_id );
        }
      }
      break;
    case COEC_REQ_RIDATU_EXE:
      OS_TPrintf("�e�@���q�@%d�̗��E�ʒm����\n", recieve_work->ridatu_id);
    //  OekakiBoard_MainSeqCheckChange( wk, OEKAKI_MODE_LOGOUT_CHILD, id );
      OekakiBoard_MainSeqForceChange( wk, OEKAKI_MODE_LOGOUT_CHILD, recieve_work->ridatu_id );
      break;
    }
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

  // �e�@�ȊO�͂��̃R�}���h�ŋ������イ��傤����
  if(GFL_NET_SystemGetCurrentID() != 0){
    OekakiBoard_MainSeqForceChange( wk, OEKAKI_MODE_FORCE_END, 0  );
  }
}

//==============================================================================
/**
 * @brief   �R��ځE�S��ځE�T��ڂ̎q�@���u�G������[�����v�ƌ���
 *
 * @param   netID   
 * @param   size    
 * @param   pData   
 * @param   pWk   
 *
 * @retval  none    
 */
//==============================================================================
static void CommOekakiBoardChildJoin(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  OEKAKI_WORK *wk = (OEKAKI_WORK*)pWk;
  u32 ret;
  u8 id;
  GF_ASSERT(GFL_NET_SystemGetCurrentID()==0 && "�q�@�������Ƃ���");
OS_Printf("_�q�@(%d)����̗����^�G�̂��������ʒm\n",netID);
  // �e�@���󂯎������(�Ƃ������e�����󂯎��Ȃ����ǁj
  if(GFL_NET_SystemGetCurrentID()==0){
    if(wk->firstChild!=0){
      id  = netID;
    
      // �S��Ɂu���ꂩ��G�𑗂�̂Ŏ~�܂��Ă��������v�Ƒ��M����
//      CommSendData_ServerSide( CO_OEKAKI_STOP, &id, 1 );
      ret=GFL_NET_SendData( GFL_NET_GetNetHandle( GFL_NET_NETID_SERVER), CO_OEKAKI_STOP, 1, &id);

      OS_Printf("�q�@(%d = %d)����̗����^�G�̂��������ʒm\n",id,netID);
    }else{
      OS_Printf("�ŏ��̎q�@%d����̗����Ȃ̂Ŗ�������\n",netID);
      wk->firstChild = 1;
    }
  }
  // ���G�������ɂ͐ڑ��ؒf�ŃG���[�������Ȃ�(�r����TRUE,TRUE�ɖ߂鎖������̂ōēx�j
//  CommStateSetErrorCheck(FALSE,TRUE);
  
}

//------------------------------------------------------------------
/**
 * @brief   ���E�֎~�R�}���h����
 *
 * @param   netID   
 * @param   size    
 * @param   pData   
 * @param   pWk   
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void CommOekakiBoardFreeze(
    const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  OEKAKI_WORK *wk = (OEKAKI_WORK*)pWk;

  // ���E�֎~
  wk->banFlag = 1;
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
//  CommSendHugeData_ServerSide( CO_OEKAKI_GRAPHICDATA, &wk->send_buf, sizeof(OEKAKIG_SPLIT_DATA));
//  CommSendHugeData(CO_OEKAKI_GRAPHICDATA, &wk->send_buf, 1004);

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
