//=============================================================================
/**
 * @file	FileBase.cpp
 * @brief	�t�@�C���̊��N���X
 * @author	k.ohno
 * @date    2008.05.08
 */
//=============================================================================

//---------------------------------------------------------
// include 
//---------------------------------------------------------
#include "stdafx.h"
#include "FileBase.h"
//#include "FileManager.h"
//#include "ApplicationData.h"

using namespace PokeIRC;
using namespace System::Windows::Forms;

//--------------------------------------------------------------
/**
 * �t�@�C�����̓��͂��s���Ă��炤
 * @param   nowDir �����DIR��
 * @retval  �t�@�C����������
 */
//--------------------------------------------------------------

String^ FileBase::inputFileName( void )
{

	int index = -1;
	System::IO::Stream^ myStream;
	OpenFileDialog^ openFileDialog1 = gcnew OpenFileDialog;

	openFileDialog1->InitialDirectory = "";
	openFileDialog1->Filter = getMyFilter();
	openFileDialog1->FilterIndex = 1;
	openFileDialog1->RestoreDirectory = true;

	if ( openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK )
	{
		if ( (myStream = openFileDialog1->OpenFile()) != nullptr )
		{
			myStream->Close();
			//ApplicationData::RWDir = System::IO::Path::GetDirectoryName(openFileDialog1->FileName);
			return openFileDialog1->FileName;
		}
	}
	return nullptr;
}



//--------------------------------------------------------------
/**
 * �t�@�C����ǂݍ���StreamReader��Ԃ�
 * @param   NowDir
 * @retval  BinaryReader^
 */
//--------------------------------------------------------------
bool FileBase::getStreamReader(String^ FileName)
{
	FileStream^ userStream;


	try {
		userStream = gcnew FileStream( FileName, FileMode::Open );
	}
	catch ( IOException^ e ) {
		MessageBox::Show( "load err " + dynamic_cast<String^>(e) + FileName );
		return false;
    }
	

	try {
		userReader = gcnew BinaryReader(userStream);
	}
	catch ( ArgumentException^ ){
		MessageBox::Show( "load err ");
		return false;
	}
	return true;
}

//--------------------------------------------------------------
/**
 * �t�@�C������������StreamWriter��Ԃ�
 * @param   NowDir
 * @retval  BinaryReader^
 */
//--------------------------------------------------------------
bool FileBase::getStreamWriter(String^ FileName)
{
	FileStream^ userStream;


	if(true == System::IO::File::Exists(FileName)){
		System::IO::File::Copy(FileName, FileName + ".bak", true);
	}
	try {
		userStream = gcnew FileStream( FileName, FileMode::Create );
	}
	catch ( IOException^ e ) {
      MessageBox::Show( "load err " + dynamic_cast<String^>(e) + FileName );
	  return false;
    }
	try {
		userWriter = gcnew BinaryWriter(userStream);
	}
	catch ( ArgumentException^ ){
		MessageBox::Show( "load err ");
		return false;
	}
	return true;
}


//--------------------------------------------------------------
/**
 * 4Bit�Ԋu�̃f�[�^���擾
 * @param   data  �o�C�i���f�[�^
 * @param   no    ���o���ꏊ
 * @retval  int ���o�����f�[�^
 */
//--------------------------------------------------------------


unsigned int FileBase::Get4Bit(array<unsigned char>^ data,int no)
{
	unsigned int dataBit = data[no / 2];
	if((no % 2)){  //��̏ꍇ ���4BIT�Ƀf�[�^������
		dataBit = dataBit >> 4;
	}
	return dataBit & 0x0f;
}


//--------------------------------------------------------------
/**
 * �t�@�C�����̏o�͂��s���Ă��炤
 * @param   nowDir �����DIR��
 * @retval  �t�@�C����������
 */
//--------------------------------------------------------------

String^ FileBase::outputFileName( void )
{

	int index = -1;
	System::IO::Stream^ myStream;
	SaveFileDialog^ saveFileDialog1 = gcnew SaveFileDialog;

//	saveFileDialog1->InitialDirectory = ApplicationData::RWDir;
	saveFileDialog1->Filter = getMyFilter();
	saveFileDialog1->FilterIndex = 1;
	saveFileDialog1->RestoreDirectory = true;

	if ( saveFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK )
	{
		if ( (myStream = saveFileDialog1->OpenFile()) != nullptr )
		{
			myStream->Close();
//			ApplicationData::RWDir = System::IO::Path::GetDirectoryName(saveFileDialog1->FileName);
			return saveFileDialog1->FileName;
		}
	}
	return nullptr;
}



//--------------------------------------------------------------
/**
 * �������g���\���̂ɒǉ�
 * @param   nowDir �����DIR��
 * @retval  �t�@�C����������
 */
//--------------------------------------------------------------

void FileBase::addArray(void)
{
//  FileManager::addReadFile(this);
}
