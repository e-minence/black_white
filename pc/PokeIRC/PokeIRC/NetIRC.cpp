//=============================================================================
/**
 * @file	NetIRC.cpp
 * @brief	赤外線ライブラリラッパークラス
 * @author	k.ohno
 * @date    2008.09.30
 */
//=============================================================================

#include "stdafx.h"
#include "MainForm.h" 
#include "NetIRC.h"

#include < stdio.h >
#include < stdlib.h >
#include < vcclr.h >

typedef int BOOL;
typedef wchar_t TCHAR;
typedef unsigned char u8;

#include "irc.h"

#pragma comment(lib,"libpokemon.lib") // libpokemon.libをリンクする

using namespace PokeIRC;
using namespace System::Windows::Forms;
using namespace System::Threading;

#define COMMAND_SND (2)
#define COMMAND_SNDV (3)
#define COMMAND_SNDE (9)

//--------------------------------------------------------------
/**
 * @breif   コールバック
 * @param   data     受け取ったデータ
 * @param   size     サイズ
 * @param   command  コマンド
 * @param   value    さらに値
 * @retval  none
 */
//--------------------------------------------------------------
static bool flgsend = false;


//void NetIRC::callback(u8 *data, u8 size, u8 command, u8 value)
static void callback(u8 *data, u8 size, u8 command, u8 value)
{
	unsigned char data_up[64]={0};

	switch(command){
	case 0x02: // センダー は2を受け取る
		IRC_Send((u8*)data_up, 64, 0x04, (u8)1);
		break;
	case 0x04: // レシーバー は4を受け取る
		NetIRC::RecvURLCOMMAND(data,size,value);
		if(NetIRC::SendLoop()){
			IRC_Send(data_up, 64, 2, (u8)1 ); 
		}
		flgsend=true;
		break;
	case 0xF4: // COMMAND_SHUTDOWN:
		Debug::WriteLine("CALLBACK: シャットダウンコマンドを受信\n");
		break;
	}
}



//--------------------------------------------------------------
/**
 * @breif   初期化
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------
void NetIRC::Init(void)
{
	isEmulate = false;
	isDataSend = false;
	isSend = false;
	IRC_Init();
	IRC_SetRecvCallback(callback);
	IRC_SetDisconnectTime(500);
}

//--------------------------------------------------------------
/**
 * @breif   描画（というよりプロセスが混ざっている）
 * @param   mform    メインフォーム
 * @retval  none
 */
//--------------------------------------------------------------
void NetIRC::draw(Form^ fm)
{
	MainForm^ mform = dynamic_cast<MainForm^>(fm);
	unsigned char data_up[64]={0,1,2,3};
	
	if(IRC_IsConnectUsb()){
		String^ coms = gcnew String((wchar_t *)IRC_GetPortName());
		mform->StripStatusLabel->Text = "USB認識中" + coms;
	}else{
		mform->StripStatusLabel->Text = "USB切断中";
	}
	if(IRC_IsConnect()){
		if(IRC_IsSender()){
			mform->StripStatusLabel->Text += "Ir 親";
		}else{
			mform->StripStatusLabel->Text += "Ir 子";
		}
	}else{
		mform->StripStatusLabel->Text += "Ir 切";
	}
	if(dataArray!=nullptr){
		mform->StripStatusLabelCenter->Text = dataArray->Length.ToString();
	}

	{
		bool tmp;
		tmp = ( IRC_IsConnect() == TRUE ) ? true : false;
		IRC_Move();

		if(IRC_IsConnect() && (tmp == false)){ // つながった瞬間のみ
			Debug::WriteLine("最初のコマンド(2)発信" + IRC_IsSender());
			if(!IRC_IsSender()){  // つながった瞬間のみ子が最初のパケットを投げる
				IRC_Send((u8*)NULL, 0, 0x02, 0);
				isSend = true;
			}
		}
	}
	if((!IRC_IsConnect()) && isEmulate){
		IRC_Connect();
	} // 自動再接続処理
}


//--------------------------------------------------------------
/**
 * @breif   接続
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------
void NetIRC::connect(void)
{
	isEmulate = true;
}

//--------------------------------------------------------------
/**
 * @breif   切断
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------
void NetIRC::shutdown(void)
{
	IRC_Shutdown();
	isEmulate = false;
}

//--------------------------------------------------------------
/**
 * @breif   データを送る
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------
bool NetIRC::sendData(void)
{
	if(isDataSend){
		return false;
	}
	isDataSend = true;
	connect();
	return true;
}

//--------------------------------------------------------------
/**
 * @breif   データを送る
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------

#define SENDBYTE_TEST (100)

bool NetIRC::SendLoop(void)
{
	if(!isDataSend){
		return true;
	}
	if(!IRC_IsConnect()){
		return true;
	}
	int length = dataArray->GetLength(0);

	pin_ptr<unsigned char> wptr = &dataArray[0];

	if(length > SENDBYTE_TEST){
		IRC_Send(wptr, SENDBYTE_TEST, COMMAND_SND, COMMAND_SNDV);
		array<unsigned char>^ dummyArray = {};
		Array::Resize(dummyArray,length-SENDBYTE_TEST);
		Array::ConstrainedCopy(dataArray,SENDBYTE_TEST,dummyArray,0,length-SENDBYTE_TEST);
		delete dataArray;
		dataArray = dummyArray;
	}
	else{
		IRC_Send(wptr, length, COMMAND_SND, COMMAND_SNDE);
		isDataSend=false;
	}
	Debug::WriteLine("snd送信"+length);
	return false;
	
}

//--------------------------------------------------------------
/**
 * @breif   非同期関数が終了するまで待つ
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------

int NetIRC::WaitForAsync(void)
{
	while(!Dpw_Tr_IsAsyncEnd())
	{
		Dpw_Tr_Main();
		Sleep(10);
	}
	return Dpw_Tr_GetAsyncResult();
}



//--------------------------------------------------------------
/**
 * @breif   非同期関数が終了するまで待つ
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------
void NetIRC::RequestUpload(void)
{
	s32 result = 0;

	// 預ける
	{
		Dpw_Tr_Data* pData;
		pin_ptr<unsigned char> wptr = &dataArray[0];
		pData = (Dpw_Tr_Data*)wptr;

		Dpw_Tr_UploadAsync(pData);
		result = WaitForAsync();
		if(result == 0 )
		{
			// 預けるのを確定
			Dpw_Tr_UploadFinishAsync();
			if(WaitForAsync() != 0 )
			{
				Debug::WriteLine("Failed to uploadFinish.");
	//			return false;
			}
		}
		else if(result == DPW_TR_ERROR_ILLIGAL_REQUEST)
		{
			Debug::WriteLine("Already uploaded.");
		}
		else
		{
			Debug::WriteLine("Failed to upload.");
	//		return false;
		}
	}
	//return true;
}



//--------------------------------------------------------------
/**
 * @breif   非同期関数が終了するまで待つ
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------
void NetIRC::RequestServerState(void)
{
	s32 result = 0;

	// サーバ状態収集
	{
		Dpw_Tr_Data* pData;
		pin_ptr<unsigned char> wptr = &dataArray[0];
		pData = (Dpw_Tr_Data*)wptr;

		Dpw_Tr_GetServerStateAsync();
		result = WaitForAsync();
		_resultSend( result );
		if(result == DPW_TR_STATUS_SERVER_OK )
		{
			Debug::WriteLine("正常.");
		}
		else
		{
			Debug::WriteLine("Failed to ServerState.");
		}
	}
}




void NetIRC::RequestPickupTraded(void)
{
	s32 result = 0;

	//		Dpw_Tr_Init(100000, TEST_KEY | 100000);
	SetProxy("");
	// 交換されたものを引き取る
	{
		Dpw_Tr_Data sData;
		Dpw_Tr_Data* pData;
		pData = (Dpw_Tr_Data*)&sData;
		Dpw_Tr_GetUploadResultAsync(pData);

		result = WaitForAsync();
		if( result == 0 )
		{
			Debug::WriteLine("Not traded.");
			_resultSend( result );
			return ;
		}
		else if(result == 1)
		{
			Debug::WriteLine("Traded.");
		}
		else
		{
			Debug::WriteLine("Failed to upload result."+result);
			_resultSend( result );
			return ;
		}
		Dpw_Tr_DeleteAsync();
		result = WaitForAsync();
		if(result != 0 )
		{
			_resultSend( result );
			Debug::WriteLine("Failed to delete.");
			return ;
		}
	}
	_resultSend( result );
	return ;
}


void NetIRC::RequestDownload(void)
{
	s32 result = 0;
	// 引き取る
	{
		Dpw_Tr_Data data;
		Dpw_Tr_DownloadAsync(&data);
		result = WaitForAsync();
		if(result != 0 )
		{
			Debug::WriteLine("Failed to download.");
		}
	}
	_resultSend( result );
}


//--------------------------------------------------------------
/**
 * @breif   赤外線から来たGTSコマンドを受け取る
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------

void NetIRC::RecvURLCOMMAND(unsigned char * data,int size,unsigned char value)
{
	int i;
	System::Threading::Thread^ threadA;

	if((value < TR_URL_UPLOAD) || (value >= TR_URL_MAX)){
		if(URLNo == 0){
			return;
		}
		switch(URLNo){
			case TR_URL_UPLOAD:

				threadA = gcnew Thread(gcnew ThreadStart(&NetIRC::RequestUpload));  //
				threadA->IsBackground = true;                // バックグラウンド・スレッドとする
				threadA->Priority = ThreadPriority::Highest; // 優先度を「最優先」にする
			    threadA->Start(); // 
				break;
			case TR_URL_UPLOADFINISH:
				break;
			case TR_URL_DOWNLOAD:
				threadA = gcnew Thread(gcnew ThreadStart(&NetIRC::RequestDownload));  //
				threadA->IsBackground = true;                // バックグラウンド・スレッドとする
				threadA->Priority = ThreadPriority::Highest; // 優先度を「最優先」にする
			    threadA->Start(); // 
				break;
			case TR_URL_GETUPLOADRESULT:
				threadA = gcnew Thread(gcnew ThreadStart(&NetIRC::RequestPickupTraded));  //
				threadA->IsBackground = true;                // バックグラウンド・スレッドとする
				threadA->Priority = ThreadPriority::Highest; // 優先度を「最優先」にする
			    threadA->Start(); // 

				break;
			case TR_URL_DELETE:
				break;
			case TR_URL_RETURN:
				break;
			case TR_URL_DOWNLOADMATCHDATA:
				break;
			case TR_URL_TRADE:
				break;
			case TR_URL_TRADEFINISH:
				break;
			case TR_URL_GETSERVERSTATE:
				threadA = gcnew Thread(gcnew ThreadStart(&NetIRC::RequestServerState));  //
				threadA->IsBackground = true;                // バックグラウンド・スレッドとする
				threadA->Priority = ThreadPriority::Highest; // 優先度を「最優先」にする
			    threadA->Start(); // 
				break;
			case TR_URL_INIT:
				break;
			case TR_URL_UPDATE:
				break;
			case TR_URL_SHUFFLE:
				break;
			default:
				break;
		}
		URLNo=0;
		return;
	}
	else{
		Debug::WriteLine("command "+value);
		if(URLNo == 0){
			URLNo = value;
			recvdataArray = gcnew array<unsigned char>(10000);
			Array::Resize(recvdataArray, 100000);
		}
		for(i=0;i<size;i++){
			recvdataArray[i] = data[i];
		}
	}


}

void NetIRC::_resultSend(int value)
{
	NetIRC::dataArray = gcnew array<unsigned char>(3);
	NetIRC::dataArray[0] = 'R';
	NetIRC::dataArray[1] = 'T';
	NetIRC::dataArray[2] = (unsigned char)value;
	NetIRC::sendData();
}


//--------------------------------------------------------------
/**
 * @breif   HTTPプロキシをセットする。NULLもしくは""で解除
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------
void NetIRC::SetProxy(String^ proxy)
{
	s_currentProxy = proxy;
	pin_ptr<const wchar_t> wch = PtrToStringChars(s_currentProxy);

	size_t convertedChars = 0;
	size_t  sizeInBytes = ((s_currentProxy->Length + 1) * 2);
	errno_t err = 0;
	char    *ch = (char *)malloc(sizeInBytes);

	err = wcstombs_s(&convertedChars, 
                    ch, sizeInBytes,
                    wch, sizeInBytes);


	ghttpSetProxy(ch);
	free(ch);
}
