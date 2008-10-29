//=============================================================================
/**
 * @file	MainForm.cpp
 * @brief	���C���̃t�H�[��
 * @author	k.ohno
 * @date    2008.09.30
 */
//=============================================================================

//---------------------------------------------------------
// include 
//---------------------------------------------------------
#include "stdafx.h"
#include "MainForm.h"
#include "NetIRC.h"
#include "FileNGC.h"

#include < stdio.h >
#include < stdlib.h >
#include < vcclr.h >



using namespace PokeIRC;
using namespace System::Windows::Forms;
using namespace System::Threading;

//--------------------------------------------------------------
/**
 * @breif   �t�@�C��->Sync���j���[���I�����ꂽ���̏���
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------

System::Void MainForm::sToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{
//	NetIRC::connect();

	threadA = gcnew Thread(gcnew ThreadStart(this,&MainForm::ThreadWork)); //

	threadA->IsBackground = true; // �o�b�N�O���E���h�E�X���b�h�Ƃ���
	threadA->Priority = ThreadPriority::Highest; //�D��x���u�ŗD��v�ɂ���

    threadA->Start(); // 

	{
		String^ proxy = "";
		Dpw_Tr_Init(100000, TEST_KEY | 100000);
		SetProxy(proxy);
	}


}

//--------------------------------------------------------------
/**
 * @breif   �t�H�[�������[�h���ꂽ���̏��� ������
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------
System::Void MainForm::Form1_Load(System::Object^  sender, System::EventArgs^  e)
{
	NetIRC::Init();
	webBrowser1->Url= gcnew Uri("http://wb.gamefreak.co.jp/ando/world/index.php");


	//�E�F�u�u���E�U�Ƃ̃����N�e�X�g


	webBrowser1->AllowWebBrowserDrop = false;
    webBrowser1->IsWebBrowserContextMenuEnabled = false;
    webBrowser1->WebBrowserShortcutsEnabled = false;

    webBrowser1->ObjectForScripting = this;
	//�t�H�[����web1�Ƀ����N�����Ă���
	//
    // Uncomment the following line when you are finished debugging.
    //webBrowser1->ScriptErrorsSuppressed = true;
/*
    webBrowser1->DocumentText =
           "<html><head><script>" +
            "function test3(message) { alert(message); }" +
            "</script></head><body><button " +
            "onclick=\"window.external.CallProg('called from script code')\">" +
            "���̃{�^���������ƃT�E���h�]���v���O��������т܂�</button>" +
            "</body></html>";
*/
	webBrowser1->Show();
	pictureBox1->Hide();


	this->Width = 10 * 8 * 6 + 10;
	this->Height = 10 * 8 * 6 + 30;

//	button1->Hide();
//	button2->Hide();
//	button3->Hide();
	splitContainer1->Panel1->Hide();

	splitContainer1->SplitterDistance = 0;  //�ŏ��{�^���͌����Ȃ�
	dispBoxNo=0;

	{
		putPoke = gcnew array<int,2>(POKMEON_BOX_NUM,POKMEON_BOX_POKENUM);
		int i,j;
		for(j=0;j < POKMEON_BOX_NUM;j++){
			for(i=0;i < POKMEON_BOX_POKENUM;i++){
				int no = j * POKMEON_BOX_POKENUM + i + 1;
				if(POKEMON_MAX <= no){
					no -= 200;
				}
				putPoke[j,i] = no;
			}
		}
	}
}

//--------------------------------------------------------------
/**
 * @breif   �^�C�}�[�C�x���g 1/60 �̂���ɔ����ŌĂ΂��
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------
System::Void MainForm::timer_Tick(System::Object^  sender, System::EventArgs^  e)
{
	//NetIRC::draw(this);
}

//--------------------------------------------------------------
/**
 * @breif   �t�H�[�������鎞�ɌĂ΂��
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------
System::Void MainForm::MainForm_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e)
{
	//timer->Stop();
	if(threadA != nullptr){
		threadA->Suspend();
	}
	//threadA->Finalize();
	NetIRC::shutdown();

}

//--------------------------------------------------------------
/**
 * @breif   �f�[�^�𑗐M����
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------
System::Void MainForm::sendDataDToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{
	int index = -1;
	System::IO::Stream^ myStream;
	BinaryReader^ userReader;   // �t�@�C����ǂݍ��ވׂ̃n���h��

	openFileDialog1->InitialDirectory = "C:\\home";
	openFileDialog1->Filter = "bin files (*.swav)|*.swav|All files (*.*)|*.*";
	openFileDialog1->FilterIndex = 1;
	openFileDialog1->RestoreDirectory = true;

	if ( openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK )
	{
		if ( (myStream = openFileDialog1->OpenFile()) != nullptr )
		{
			userReader = gcnew BinaryReader(myStream);
			NetIRC::dataArray = userReader->ReadBytes((int)myStream->Length);
			NetIRC::sendData('A');
			myStream->Close();
		}
	}

}

//--------------------------------------------------------------
/**
 * @breif   BackGroundWorker�̃^�X�N����
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------

System::Void MainForm::DoWork( System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e )
{
    // �����ɁA����������
	NetIRC::draw(this);
	Thread::Sleep(1); //2msec�҂� 
}

//--------------------------------------------------------------
/**
 * @breif   �X���b�h���I���������ɌĂ΂��
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------

System::Void MainForm::RunWorkerCompleted( System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e )
{
    // �X���b�h���I�������Ƃ��ɌĂ΂��
}

void MainForm::ThreadWork(void)
{
	while(1){
		NetIRC::draw(this);
		Thread::Sleep(3); //2msec�҂� 
	}
}

// �v���t�B�[�����Z�b�g����
void MainForm::SetProfile(Dpw_Common_Profile* profile)
{
	memset(profile, 0, sizeof(*profile));
	profile->version = 12;
	profile->language = 1;
	profile->countryCode = 103;
	profile->localCode = 1;
	profile->playerId = 87654322;
	profile->playerName[0] = 0x66;      // �T
	profile->playerName[1] = 0x79;      // �g
	profile->playerName[2] = 0x69;      // �V
	profile->playerName[3] = 0xffff;    // �I�[
	profile->flag = 0;
	strcpy(profile->mailAddr, "ohno_katsumi@gamefreak.co.jp");
	profile->mailAddrAuthPass = DPW_MAIL_ADDR_AUTH_DEBUG_PASSWORD;   // �f�o�b�O�p�p�X���[�h���g�p����B
	profile->mailAddrAuthVerification = 456;
	profile->mailRecvFlag = DPW_PROFILE_MAILRECVFLAG_EXCHANGE;
}

// Dpw_Tr_Data���Z�b�g����
void MainForm::SetTrData(Dpw_Tr_Data* upload_data)
{
	// �R�N�[��
	u8 data[] = {57,148,138,197,0,0,19,71,0,225,226,116,104,187,11,169,205,198,239,184,227,255,202,161,239,133,127,140,240,161,240,216,218,41,84,70,114,199,108,49,9,11,13,170,10,159,215,139,122,119,196,182,144,222,10,165,255,5,39,199,92,169,30,208,159,170,35,150,233,137,68,57,61,192,222,48,184,97,214,129,199,149,235,2,51,78,168,8,50,63,184,113,88,235,164,121,39,174,75,187,206,137,149,99,23,87,77,12,166,84,227,11,122,23,165,0,177,112,190,172,49,209,41,5,54,142,254,210,176,165,17,74,129,129,171,153,64,114,145,74,27,162,201,134,219,36,246,122,83,172,14,162,147,57,250,245,81,152,113,165,218,37,51,51,228,198,16,92,111,223,224,104,234,160,34,71,70,131,62,212,7,7,90,177,247,109,244,134,227,190,220,50,54,197,124,119,9,133,210,16,1,10,178,178,29,251,90,150,53,231,242,188,35,194,198,246,18,178,21,67,39,33,129,179,97,4,0,76,14,66,18,45,12,186,7,205};

	memset(upload_data, 0, sizeof(*upload_data));
	memcpy(&upload_data->postData, data, sizeof(upload_data->postData));
	upload_data->postSimple.characterNo = 14;    // �R�N�[��
	upload_data->postSimple.gender = DPW_TR_GENDER_FEMALE;
	upload_data->postSimple.level = 20;
	upload_data->wantSimple.characterNo = 14;    // �R�N�[��
	upload_data->wantSimple.gender = DPW_TR_GENDER_FEMALE;
	upload_data->wantSimple.level_min = 0;
	upload_data->wantSimple.level_max = 0;
	upload_data->name[0] = 0x66; // �T
	upload_data->name[1] = 0x79; // �g
	upload_data->name[2] = 0x69; // �V
	upload_data->name[3] = 0xffff;
	upload_data->trainerID = 1 << 12;
	upload_data->countryCode = 103;
	upload_data->langCode = 1;
	upload_data->localCode = 1;
	upload_data->trainerType = 3;
	upload_data->versionCode = 12;
	upload_data->gender = 1;
}

//--------------------------------------------------------------
/**
 * @breif   �񓯊��֐����I������܂ő҂�
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------
bool MainForm::RequestUpload(void)
{
	s32 result = 0;

	// �a����
	{
		Dpw_Tr_Data data;
		SetTrData(&data);
		Dpw_Tr_UploadAsync(&data);
		result = WaitForAsync();
		if(result == 0 )
		{
			// �a����̂��m��
			Dpw_Tr_UploadFinishAsync();
			if(WaitForAsync() != 0 )
			{
				Debug::WriteLine("Failed to uploadFinish.");
				return false;
			}
		}
		else if(result == DPW_TR_ERROR_ILLIGAL_REQUEST)
		{
			Debug::WriteLine("Already uploaded.");
		}
		else
		{
			Debug::WriteLine("Failed to upload.");
			return false;
		}
	}
	return true;
}


//--------------------------------------------------------------
/**
 * @breif   �������ꂽ���̂��������
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------

bool MainForm::RequestPickupTraded(void)
{
	s32 result = 0;

	// �������ꂽ���̂��������
	{
		Dpw_Tr_Data data;
		Dpw_Tr_GetUploadResultAsync(&data);

		result = WaitForAsync();
		if(WaitForAsync() == 0 )
		{
			puts("Not traded.");
			return true;
		}
		else if(result == 1)
		{
			puts("Traded.");
		}
		else
		{
			puts("Failed to upload result.");
			return false;
		}

		Dpw_Tr_DeleteAsync();
		if(WaitForAsync() != 0 )
		{
			puts("Failed to delete.");
			return false;
		}
	}
	return true;
}

//--------------------------------------------------------------
/**
 * @breif   �񓯊��֐����I������܂ő҂�
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------

bool MainForm::RequestSetProfile(void)
{
	// �v���t�B�[���Z�b�g
	{
		Dpw_Common_Profile profile;
		Dpw_Common_ProfileResult profile_result;
		SetProfile(&profile);
		Dpw_Tr_SetProfileAsync(&profile, &profile_result);
		if(WaitForAsync() != 0 )
		{
			Debug::WriteLine("Failed to set profile.");
			return false;
		}
	}
	return true;
}

//--------------------------------------------------------------
/**
 * @breif   �񓯊��֐����I������܂ő҂�
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------

int MainForm::WaitForAsync(void)
{
	while(!Dpw_Tr_IsAsyncEnd())
	{
		Dpw_Tr_Main();
		Sleep(10);
	}
	// ���N�G�X�g���I�������ēx�v���L�V���Z�b�g���Ȃ���΂Ȃ�Ȃ��B
	SetProxy("");
	return Dpw_Tr_GetAsyncResult();
}

//--------------------------------------------------------------
/**
 * @breif   �T�[�o�X�e�[�^�X�`�F�b�N
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------

bool MainForm::RequestCheckServerState(void)
{
	// �T�[�o�X�e�[�^�X�`�F�b�N
	{
		Dpw_Tr_GetServerStateAsync();
		if(WaitForAsync() != DPW_TR_STATUS_SERVER_OK )
		{
			Debug::WriteLine("Invalid server status.");
			return false;
		}
	}
	return true;
}

//--------------------------------------------------------------
/**
 * @breif   HTTP�v���L�V���Z�b�g����BNULL��������""�ŉ���
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------
void MainForm::SetProxy(String^ proxy)
{
	NetIRC::SetProxy(proxy);
}


//--------------------------------------------------------------
/**
 * @breif   GTS�ڑ��e�X�g
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------

void MainForm::TestUploadDownload(int pid,  String^ proxy)
{
	static int count = 1;
	Dpw_Tr_Init(pid, TEST_KEY | pid);

	SetProxy(proxy);

	if(!RequestCheckServerState())
	{
		return;
	}

	if(!RequestSetProfile())
	{
		return;
	}

	if(!RequestUpload())
	{
		return;
	}
/*
	if(!RequestPickup())
	{
		return;
	}
	*/
	Dpw_Tr_Db_UpdateServer();
	Dpw_Tr_End();

	Debug::WriteLine("Finished Test\n");
}

//--------------------------------------------------------------
/**
 * @breif   GTS�ڑ��e�X�g
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------

void MainForm::TestDownload(int pid,  String^ proxy)
{
	static int count = 1;
	Dpw_Tr_Init(pid, TEST_KEY | pid);

	SetProxy(proxy);
	if(!RequestCheckServerState())
	{
		return;
	}
	if(!RequestSetProfile())
	{
		return;
	}
	if(!RequestPickupTraded())
	{
		return;
	}
	Dpw_Tr_End();
	Debug::WriteLine("Delete - Finish\n");
}

//--------------------------------------------------------------
/**
 * @breif   GTS�ɂ���f�[�^�̍폜���s��
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------

System::Void MainForm::gTSResetToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{
	String^ proxy = "";

	// �������
	TestDownload(100000, proxy);
}

//--------------------------------------------------------------
/**
 * @breif   GTS�ڑ��e�X�g
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------

System::Void MainForm::gTSTestGToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{

	// HTTP�v���L�V���K�v�ȏꍇ�̓v���L�V�̃A�h���X��"<server>[:port]"�̗l�Ɏw�肵�Ă��������B
	//String^ proxy = "proxy.server.com:8080";
	String^ proxy = "";

	// �a���Ĉ������
	TestUploadDownload(100000, proxy);
}

//--------------------------------------------------------------
/**
 * @breif   �ԊO���ʐM����DS����GTS������s��
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------


System::Void MainForm::dSGTSSyncTToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{
	NetIRC::dataArray = gcnew array<unsigned char>(1);
	NetIRC::dataArray[0] = 'W';
	NetIRC::sendData('W');
}

//--------------------------------------------------------------
/**
 * @breif   �ԊO���ʐM����DS����GTS������s��
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------

void MainForm::CallProg(String^ message)
{
	if(message =="PokeSend"){
		pokemonListDisp(0);
	}
	else{
		MessageBox::Show(message, "client code");
	}

}

void MainForm::pokemonListDisp(int boxNo)
{
	int i;

	dispBoxNo = boxNo;
	Bitmap^ bmp = gcnew Bitmap(10 * 6 * 16, 10 * 16 * 5);

	int printBoxNo = boxNo + 1;
	button2->Text = "�{�b�N�X " + printBoxNo;

	for(i=0;i < POKMEON_BOX_POKENUM;i++){
		if(putPoke[boxNo,i]==0){
			continue;
		}
		int tno = putPoke[boxNo,i];
		if(tno >= POKEMON_MAX ){
			continue;
		}

		int no = PokeGraNoTable[putPoke[boxNo,i]].no;

		String^ dirname = "C:\\home\\wb\\pokemon_wb\\pc\\PokeIRC\\PokeIRC\\pokegra\\";
		String^ ncgname = dirname + "pmpl_" + no.ToString("000") + "_frnt_m.ncg";
		String^ nclname = dirname + "pmpl_" + no.ToString("000") + "_n.ncl";
	
		FileNCGRead^ fngc = gcnew FileNCGRead;

		fngc->readWithNcl(ncgname,"", nclname);
	
		pictureBox1->Image = fngc->PictureWriteOffset(bmp, pictureBox1,10*(i%6), 10*(i/6),10,10);

		pictureBox1->Dock = DockStyle::Fill;
		webBrowser1->Dock = DockStyle::Fill;

		webBrowser1->Visible = false;
		pictureBox1->Visible = true;
	}
	splitContainer1->Panel1->Show();
	button1->Width = this->Width / 5;
	button2->Width = (this->Width / 5) * 3;
	button3->Width = this->Width / 5;
}


//--------------------------------------------------------------
/**
 * @breif   �莝���|�P�����\��
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------

System::Void MainForm::pokemonToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{
	pokemonListDisp(0);

}

//--------------------------------------------------------------
/**
 * @breif   �ꏊ����Pokemon�ԍ��𓾂�
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------

int MainForm::getPokemonNumberFromThePlace(int x,int y)
{
	int i = x  / (10*8);
	if(i < 0){
		return 0;
	}
	else if(i >= 6){
		return 0;
	}
	int j = (y)  / (10*8);
	if(j < 0){
		return 0;
	}
	else if(j >= 5){
		return 0;
	}
	i = i + j * 6;
	if((i >= 0) && (i < POKMEON_BOX_POKENUM)){
		return putPoke[dispBoxNo,i];
	}
	return 0;
}

//--------------------------------------------------------------
/**
 * @breif   �}�E�X�ړ����Ƀ|�P�����̖��O�Ȃǂ�Tip�ŕ\��
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------


System::Void MainForm::pictureBox1_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
{
	static int backupPoke = -1;

	int pokeno = getPokemonNumberFromThePlace(e->X,e->Y);

	if((pokeno != 0) && (backupPoke != pokeno)){
		char* name = PokeGraNoTable[pokeno].name;
		String^ pknm = gcnew String(name);
		toolTip1->Show(pknm, this, e->X, e->Y+20, 30000);
		backupPoke = pokeno;
	}
}

//--------------------------------------------------------------
/**
 * @breif   �}�E�X���E�{�^���ŉ��������Ƀ��j���[�\��
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------

System::Void MainForm::pictureBox1_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
{
	if((e->Button::get()==System::Windows::Forms::MouseButtons::Right)){
		contextMenuStrip1->Show(Control::MousePosition::get());
	}
}

//--------------------------------------------------------------
/**
 * @breif   �}�E�X���N���b�N�������Ƀ|�P������a���邩�ǂ����₢���킹��
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------

System::Void MainForm::pictureBox1_MouseClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
{
	int pokeno = getPokemonNumberFromThePlace(e->X,e->Y);

	if(pokeno != 0){
		String^ nm = gcnew String(PROGRAM_NAME);
		char* name = PokeGraNoTable[pokeno].name;
		String^ pknm = gcnew String(name);

		if ( MessageBox::Show(pknm + "��a���܂����H", nm,
			 MessageBoxButtons::OKCancel,
			 MessageBoxIcon::Question )
				 == System::Windows::Forms::DialogResult::OK ) {





		}
	}
}

//--------------------------------------------------------------
/**
 * @breif   �c�[�����I������
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------
System::Void MainForm::exitEToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{
	String^ nm = gcnew String(PROGRAM_NAME);
	if ( MessageBox::Show("�A�v���P�[�V�������I�����܂����H", nm,
		 MessageBoxButtons::OKCancel,
		 MessageBoxIcon::Question )
			 == System::Windows::Forms::DialogResult::OK ) {
				 this->Close();
	}
}

//--------------------------------------------------------------
/**
 * @breif   �}�Ӑ؂�ւ�
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------
System::Void MainForm::button3_Click(System::Object^  sender, System::EventArgs^  e)
{
	dispBoxNo++;
	if(dispBoxNo>=POKMEON_BOX_NUM){
		dispBoxNo = 0;
	}
	pokemonListDisp(dispBoxNo);
}

//--------------------------------------------------------------
/**
 * @breif   �}�Ӑ؂�ւ�
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------

System::Void MainForm::button1_Click(System::Object^  sender, System::EventArgs^  e)
{
	if(dispBoxNo <= 0){
		dispBoxNo = POKMEON_BOX_NUM;
	}
	dispBoxNo--;
	pokemonListDisp(dispBoxNo);
}
