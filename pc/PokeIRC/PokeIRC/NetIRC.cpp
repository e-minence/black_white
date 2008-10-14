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

typedef int BOOL;
typedef wchar_t TCHAR;
typedef unsigned char u8;

#include "irc.h"

#pragma comment(lib,"libpokemon.lib") // libpokemon.lib�������N����

using namespace PokeIRC;
using namespace System::Windows::Forms;

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
	case 0x02: // �Z���_�[ ��2���󂯎��
		IRC_Send((u8*)data_up, 64, 0x04, (u8)1);
		break;
	case 0x04: // ���V�[�o�[ ��4���󂯎��
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

	{
		bool tmp;
		tmp = IRC_IsConnect();
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
	Debug::WriteLine("snd���M"+length);
	return false;
	
}