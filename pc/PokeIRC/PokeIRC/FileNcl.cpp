//=============================================================================
/**
 * @file	FileNcl.cpp
 * @brief	�p���b�g�t�@�C���Ǘ�
 * @author	k.ohno
 * @date    2008.05.12
 */
//=============================================================================

//---------------------------------------------------------
// include 
//---------------------------------------------------------
#include "stdafx.h"
#include "FileNcl.h"
//#include "FileManager.h"

using namespace PokeIRC;
using namespace System::Windows::Forms;



//---------------------------------------------------------
// ��������FileNCGRead�N���X
//---------------------------------------------------------

//--------------------------------------------------------------
/**
 * �t�B���^�[��ʂ�Ԃ�
 * @param   filename
 * @retval  StreamReader^
 */
//--------------------------------------------------------------

String^ FileNCLRead::getMyFilter(void)
{
	return "ncl files (*.ncl)|*.ncl|All files (*.*)|*.*";
}

//--------------------------------------------------------------
/**
 * �t�@�C����ǂݍ���Ŋi�[
 * @param   filename
 * @retval  StreamReader^
 */
//--------------------------------------------------------------

void FileNCLRead::read(String^ filename, String^ nowDir)
{
	if(false == getStreamReader(filename)){
		return;
	}
	MyName = filename;
	ReadNCLHeader();
	ReadNCLDataBlock();
	userReader->Close();
	addArray();  // ������o�^
	//FileManager::NCLNodeAdd(System::IO::Path::GetFileName(filename), this);
}


//--------------------------------------------------------------
/**
 * NCL�w�b�_�u���b�N�ǂݍ���
 */
//--------------------------------------------------------------

void FileNCLRead::ReadNCLHeader(void)
{
	nclHead = gcnew NCLHeader;
	nclHead->FormatID = userReader->ReadUInt32();
	nclHead->ByteOrderMark = userReader->ReadUInt16();
	nclHead->FormatVersion = userReader->ReadUInt16();
	nclHead->FileSize = userReader->ReadUInt32();
	nclHead->HeaderSize = userReader->ReadUInt16();
	nclHead->DataBlocks = userReader->ReadUInt16();
}


//--------------------------------------------------------------
/**
 * NCL�f�[�^�u���b�N�ǂݍ���
 */
//--------------------------------------------------------------

void FileNCLRead::ReadNCLDataBlock(void)
{
	nclData = gcnew NCLPalData;

	nclData->BlockType = userReader->ReadUInt32();
	nclData->BlockSize = userReader->ReadUInt32();
	nclData->PaletteColorNumber = userReader->ReadUInt32();
	nclData->PaletteNumber = userReader->ReadUInt32();
	nclData->PaletteData = userReader->ReadBytes(nclData->BlockSize-0x10);
}

//--------------------------------------------------------------
/**
 * NCL�f�[�^�u���b�N�ǂݍ���
 */
//--------------------------------------------------------------

unsigned int FileNCLRead::getPaletteR(int line,int no)
{
	int pal = nclData->PaletteData[line*32+no*2]+nclData->PaletteData[line*32+no*2+1]*0x100;
	return (pal & 0x1f) << 3;
}
//--------------------------------------------------------------
/**
 * NCL�f�[�^�u���b�N�ǂݍ���
 */
//--------------------------------------------------------------

unsigned int FileNCLRead::getPaletteG(int line,int no)
{
	int pal = nclData->PaletteData[line*32+no*2]+nclData->PaletteData[line*32+no*2+1]*0x100;
	return ((pal>>5) & 0x1f)<<3;
}

//--------------------------------------------------------------
/**
 * NCL�f�[�^�u���b�N�ǂݍ���
 */
//--------------------------------------------------------------

unsigned int FileNCLRead::getPaletteB(int line,int no)
{
	int pal = nclData->PaletteData[line*32+no*2]+nclData->PaletteData[line*32+no*2+1]*0x100;
	return ((pal>>10) & 0x1f)<<3;
}

//--------------------------------------------------------------
/**
 * �v���W�F�N�g������𓾂�
 */
//--------------------------------------------------------------
String^ FileNCLRead::getProjectDataString(void)
{
	return String::Format("//RESOURCE NCL {0}", MyName);
}

