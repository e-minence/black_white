//============================================================================================
/**
 * @file	test_net.c
 * @brief	�ʐM����e�X�g�p�֐�
 * @author	ohno
 * @date	2006.12.19
 */
//============================================================================================


#include "gflib.h"
#include "procsys.h"
#include "gf_standard.h"
#include "test_net.h"
#include "net.h"
#include "ui.h"


//------------------------------------------------------------------
// NET�̃e�X�g
//------------------------------------------------------------------

enum{
    _TEST_CONNECT,
    _TEST_1,
    _TEST_2,

};


static int testNo = 0;

void TEST_NET_Main(void)
{

    if(PAD_BUTTON_B == GFL_UI_KeyGetTrg()){
        switch(testNo){
          case _TEST_CONNECT:
            {
                GFL_NETHANDLE* pHandle = GFL_NET_CreateHandle();
                GFL_NET_ClientConnectTo(pHandle,(u8*)"001656A80D74"); //����DS��MACADDR
            }
            testNo++;
            break;
        }
    }

    GFL_NET_MainProc();


}

#define SSID "GF_ORG01"

static u8* _netGetSSID(void)
{
    return (u8*)SSID;
}


// �ʐM�������\����
GFLNetInitializeStruct aGFLNetInit = {
    1,  //gsid
    0,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
    GFL_HEAPID_SYSTEM,  //allocNo
    NULL,  // ��M�֐��e�[�u��
    NULL,  // �r�[�R���f�[�^�擾�֐�
    NULL,  // �r�[�R���f�[�^�T�C�Y�擾�֐�
    NULL,  // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
    NULL,  // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
    NULL,  // �ʐM�ؒf���ɌĂ΂��֐�
    NULL,  // �ʐM�ؒf���ɌĂ΂��֐�
    NULL,  // �ʐM�ؒf���ɌĂ΂��֐�
    _netGetSSID,  // �e�q�ڑ����ɔF�؂���ׂ̃o�C�g��  
    2,     // �ő�ڑ��l��
    16,    // �ő�r�[�R�����W��
    1,     // �ʐM���J�n���邩�ǂ���
};


void TEST_NET_Init(void)
{

    GFL_NET_Initialize(&aGFLNetInit, GFL_HEAPID_SYSTEM);

}



