//=============================================================================
/**
 * @file	NetIRC.cpp
 * @brief	�ԊO�����C�u�������b�p�[�N���X
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

#pragma comment(lib,"libpokemon.lib") // libpokemon.lib�������N����

using namespace PokeIRC;
using namespace System::Windows::Forms;
using namespace System::Threading;

#define COMMAND_SND (2)
#define COMMAND_SNDV (3)
#define COMMAND_SNDE (9)



//--------------------------------------------------------------
/**
 * @breif   �R�[���o�b�N
 * @param   data     �󂯎�����f�[�^
 * @param   size     �T�C�Y
 * @param   command  �R�}���h
 * @param   value    ����ɒl
 * @retval  none
 */
//--------------------------------------------------------------
static bool flgsend = false;


//void NetIRC::callback(u8 *data, u8 size, u8 command, u8 value)
static void callback(u8 *data, u8 size, u8 command, u8 value)
{
	unsigned char data_up[64]={0};

	switch(command){
	case 0x02: // �Z���_�[��2���󂯎��
		IRC_Send((u8*)data_up, 64, 0x04, (u8)1);
		break;
	case 0x04: // ���V�[�o�[ ��4���󂯎��
		NetIRC::RecvURLCOMMAND(data,size-4,value);
		if(NetIRC::SendLoop()){
			IRC_Send(data_up, 64, 2, (u8)1 ); 
		}
		flgsend=true;
		break;
	case 0xF4: // COMMAND_SHUTDOWN:
		Debug::WriteLine("CALLBACK: �V���b�g�_�E���R�}���h����M\n");
		break;
	}
}



//--------------------------------------------------------------
/**
 * @breif   ������
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
 * @breif   �`��i�Ƃ������v���Z�X���������Ă���j
 * @param   mform    ���C���t�H�[��
 * @retval  none
 */
//--------------------------------------------------------------
void NetIRC::draw(Form^ fm)
{
	MainForm^ mform = dynamic_cast<MainForm^>(fm);
	unsigned char data_up[64]={0,1,2,3};

	/*
	if(IRC_IsConnectUsb()){
		String^ coms = gcnew String((wchar_t *)IRC_GetPortName());
		mform->StripStatusLabel->Text = "USB�F����" + coms;
	}else{
		mform->StripStatusLabel->Text = "USB�ؒf��";
	}
	if(IRC_IsConnect()){
		if(IRC_IsSender()){
			mform->StripStatusLabel->Text += "Ir �e";
		}else{
			mform->StripStatusLabel->Text += "Ir �q";
		}
	}else{
		mform->StripStatusLabel->Text += "Ir ��";
	}
	if(dataArray!=nullptr){
		mform->StripStatusLabelCenter->Text = dataArray->Length.ToString();
	}
	*/

	{
		bool tmp;
		tmp = ( IRC_IsConnect() == TRUE ) ? true : false;
		IRC_Move();

		if(IRC_IsConnect() && (tmp == false)){ // �Ȃ������u�Ԃ̂�
			Debug::WriteLine("�ŏ��̃R�}���h(2)���M" + IRC_IsSender());
			if(!IRC_IsSender()){  // �Ȃ������u�Ԃ̂ݎq���ŏ��̃p�P�b�g�𓊂���
				IRC_Send((u8*)NULL, 0, 0x02, 0);
				isSend = true;
			}
		}
	}
	if((!IRC_IsConnect()) && isEmulate){
		IRC_Connect();
	} // �����Đڑ�����
}


//--------------------------------------------------------------
/**
 * @breif   �ڑ�
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
 * @breif   �ؒf
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
 * @breif   �f�[�^�𑗂�
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------
bool NetIRC::sendData(u8 value)
{
	if(isDataSend){
		return false;
	}
	recvDataSize=0;
	isRecv = false;
	SendValue = value;
	isDataSend = true;
	connect();
	return true;
}

//--------------------------------------------------------------
/**
 * @breif   �f�[�^�𑗂�
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
 
	if(length > SENDBYTE_TEST){  //�ł�������SendValue
		IRC_Send(wptr, SENDBYTE_TEST, COMMAND_SND, SendValue);
		array<unsigned char>^ dummyArray = {};
		Array::Resize(dummyArray,length-SENDBYTE_TEST);
		Array::ConstrainedCopy(dataArray,SENDBYTE_TEST,dummyArray,0,length-SENDBYTE_TEST);
		delete dataArray;
		dataArray = dummyArray;
	}
	else{
		IRC_Send(wptr, length, COMMAND_SND, SendValue+1);  //����鎞��SendValue+1
		isDataSend=false;
	}
	Debug::WriteLine("���M" + SendValue + "�T�C�Y"+length);
	return false;
	
}

//--------------------------------------------------------------
/**
 * @breif   �񓯊��֐����I������܂ő҂�
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
 * @breif   �񓯊��֐����I������܂ő҂�
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------
void NetIRC::RequestUpload(void)
{
	s32 result = 0;

	// �a����
	{
		Dpw_Tr_Data* pData;
		pin_ptr<unsigned char> wptr = &dataArray[0];
		pData = (Dpw_Tr_Data*)wptr;

		Dpw_Tr_UploadAsync(pData);
		result = WaitForAsync();
		if(result == 0 )
		{
			// �a����̂��m��
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
 * @breif   �񓯊��֐����I������܂ő҂�
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------
void NetIRC::RequestServerState(void)
{
	s32 result = 0;

	// �T�[�o��Ԏ��W
	{
		Dpw_Tr_Data* pData;
		pin_ptr<unsigned char> wptr = &dataArray[0];
		pData = (Dpw_Tr_Data*)wptr;

		Dpw_Tr_GetServerStateAsync();
		result = WaitForAsync();
		_resultSend( result );
		if(result == DPW_TR_STATUS_SERVER_OK )
		{
			Debug::WriteLine("����.");
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
	// �������ꂽ���̂��������
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

//--------------------------------------------------------------
/**
 * @breif   �ۂ������������
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------

void NetIRC::RequestDownload(void)
{
	s32 result = 0;
	// �������
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
 * @breif   �ۂ������������
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------

void NetIRC::RequestDownloadMatchData(void)
{
	s32 result = 0;
	Dpw_Tr_Data match_data_buf[DPW_TR_DOWNLOADMATCHDATA_MAX];	// �f�[�^�̌������ʂ�����o�b�t�@�B
	// �������g���[�h
	{
		// ����
		Dpw_Tr_PokemonSearchDataEx* pSearch_data;
/*		
		search_data.characterNo = 14;
		search_data.gender = DPW_TR_GENDER_FEMALE;
		search_data.level_min = 20;
		search_data.level_max = 20;	// 0���w�肷��ƁA����͐ݒ肵�Ȃ����ƂɂȂ�
        search_data.maxNum = DPW_TR_DOWNLOADMATCHDATA_MAX;
        search_data.countryCode = 103;
*/		
		pin_ptr<unsigned char> wptr = &recvdataArray[0];
		pSearch_data = (Dpw_Tr_PokemonSearchDataEx*)wptr;

		Dpw_Tr_DownloadMatchDataExAsync(pSearch_data, match_data_buf);
		result = WaitForAsync();
		if(result < 0 )
		{
			Debug::WriteLine("Failed to search.");
		}
		else if(result == 0)
		{
			Debug::WriteLine("No such pokemon.");
		}
		else{
			Debug::WriteLine("Found " + result + " pokemons.\n");
		}
	}
	_resultSend( result );

}


//--------------------------------------------------------------
/**
 * @breif   �ԊO�����痈��GTS�R�}���h���󂯎��
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------

void NetIRC::RecvURLCOMMAND(unsigned char * data,int size,unsigned char value)
{
	int i;
	System::Threading::Thread^ threadA;

	if(value != 0){
		Debug::WriteLine("command "+value);
		if(URLNo == 0){
			URLNo = value;
			recvdataArray = gcnew array<unsigned char>(10000);
			Array::Resize(recvdataArray, 100000);
			recvDataSize=0;
		}
		for(i=0;i<size;i++){
			recvdataArray[recvDataSize + i] = data[i];
		}
		recvDataSize+=size;

		if((value == IRC_COMMAND_BOXLISTEND) || (value == IRC_COMMAND_BOXPOKEEND)){
			isRecv = true;
		}
	}

	if((value >= TR_URL_UPLOAD) && (value < TR_URL_MAX)){
		switch(URLNo){
			case TR_URL_UPLOAD:

				threadA = gcnew Thread(gcnew ThreadStart(&NetIRC::RequestUpload));  //
				threadA->IsBackground = true;                // �o�b�N�O���E���h�E�X���b�h�Ƃ���
				threadA->Priority = ThreadPriority::Highest; // �D��x���u�ŗD��v�ɂ���
			    threadA->Start(); // 
				break;
			case TR_URL_UPLOADFINISH:
				break;
			case TR_URL_DOWNLOAD:
				threadA = gcnew Thread(gcnew ThreadStart(&NetIRC::RequestDownload));  //
				threadA->IsBackground = true;                // �o�b�N�O���E���h�E�X���b�h�Ƃ���
				threadA->Priority = ThreadPriority::Highest; // �D��x���u�ŗD��v�ɂ���
			    threadA->Start(); // 
				break;
			case TR_URL_GETUPLOADRESULT:
				threadA = gcnew Thread(gcnew ThreadStart(&NetIRC::RequestPickupTraded));  //
				threadA->IsBackground = true;                // �o�b�N�O���E���h�E�X���b�h�Ƃ���
				threadA->Priority = ThreadPriority::Highest; // �D��x���u�ŗD��v�ɂ���
			    threadA->Start(); // 

				break;
			case TR_URL_DELETE:
				break;
			case TR_URL_RETURN:
				break;
			case TR_URL_DOWNLOADMATCHDATA:

				threadA = gcnew Thread(gcnew ThreadStart(&NetIRC::RequestDownloadMatchData));  //
				threadA->IsBackground = true;                // �o�b�N�O���E���h�E�X���b�h�Ƃ���
				threadA->Priority = ThreadPriority::Highest; // �D��x���u�ŗD��v�ɂ���
			    threadA->Start(); // 

				break;
			case TR_URL_TRADE:
				break;
			case TR_URL_TRADEFINISH:
				break;
			case TR_URL_GETSERVERSTATE:
				threadA = gcnew Thread(gcnew ThreadStart(&NetIRC::RequestServerState));  //
				threadA->IsBackground = true;                // �o�b�N�O���E���h�E�X���b�h�Ƃ���
				threadA->Priority = ThreadPriority::Highest; // �D��x���u�ŗD��v�ɂ���
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

}

void NetIRC::_resultSend(int value)
{
	NetIRC::dataArray = gcnew array<unsigned char>(1);
	NetIRC::dataArray[0] = (unsigned char)value;
	NetIRC::sendData('R');
}


//--------------------------------------------------------------
/**
 * @breif   HTTP�v���L�V���Z�b�g����BNULL��������""�ŉ���
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

//--------------------------------------------------------------
/**
 * @breif   �|�P�����{�b�N�X���X�g�v��
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------
/*
void NetIRC::GetPokeBoxList(void)
{
	isRecv = false;
	NetIRC::dataArray = gcnew array<unsigned char>(1);
	NetIRC::dataArray[0] = IRC_COMMAND_BOXLIST;
	NetIRC::sendData(IRC_COMMAND_BOXLIST);


	while(!isRecv){
		Sleep(10);
	}
	MessageBox::Show("�}�Ӄ��X�g�����Ƃ�");

}
*/