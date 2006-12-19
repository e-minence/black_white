//============================================================================================
/**
 * @file	test_net.c
 * @brief	通信動作テスト用関数
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
// NETのテスト
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
                GFL_NET_ClientConnectTo(pHandle,(u8*)"001656A80D74"); //大野のDSのMACADDR
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


// 通信初期化構造体
GFLNetInitializeStruct aGFLNetInit = {
    1,  //gsid
    0,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
    GFL_HEAPID_SYSTEM,  //allocNo
    NULL,  // 受信関数テーブル
    NULL,  // ビーコンデータ取得関数
    NULL,  // ビーコンデータサイズ取得関数
    NULL,  // ビーコンのサービスを比較して繋いで良いかどうか判断する
    NULL,  // 通信不能なエラーが起こった場合呼ばれる 切断するしかない
    NULL,  // 通信切断時に呼ばれる関数
    NULL,  // 通信切断時に呼ばれる関数
    NULL,  // 通信切断時に呼ばれる関数
    _netGetSSID,  // 親子接続時に認証する為のバイト列  
    2,     // 最大接続人数
    16,    // 最大ビーコン収集数
    1,     // 通信を開始するかどうか
};


void TEST_NET_Init(void)
{

    GFL_NET_Initialize(&aGFLNetInit, GFL_HEAPID_SYSTEM);

}



