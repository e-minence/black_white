//=============================================================================
/**
 * @file	FileNgc.cpp
 * @brief	�L�����t�@�C���Ǘ�
 * @author	k.ohno
 * @date    2008.05.12
 */
//=============================================================================

//---------------------------------------------------------
// include 
//---------------------------------------------------------
#include "stdafx.h"
#include "FileNgc.h"
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

String^ FileNCGRead::getMyFilter(void)
{
	return "ncg files (*.ncg)|*.ncg|All files (*.*)|*.*";
}



//--------------------------------------------------------------
/**
 * �t�@�C����ǂݍ���Ŋi�[
 * @param   filename
 * @retval  StreamReader^
 */
//--------------------------------------------------------------

void FileNCGRead::readWithNcl(String^ filename, String^ nowDir,String^ nclName)
{
	if(false == getStreamReader(filename)){
		return;
	}
	MyName = filename;

	ReadNGCHeader();

	ReadNGCDataBlock();

	ReadNGCAttr();

	ReadNGCLink();

	{
		FileNCLRead^ nclHandle = gcnew FileNCLRead();  // NCL�n���h���쐬
		nclHandle->read(nclName, nowDir);   //NCG�ɏ����Ă������p���b�g	��ǂݍ���
		setNCLHandle(nclHandle);   //NCL��HAS����
	}

	userReader->Close();
	addArray();  // ������o�^
	//FileManager::NCGNodeAdd(System::IO::Path::GetFileName(filename), this);


}



//--------------------------------------------------------------
/**
 * �t�@�C����ǂݍ���Ŋi�[
 * @param   filename
 * @retval  StreamReader^
 */
//--------------------------------------------------------------

void FileNCGRead::read(String^ filename, String^ nowDir)
{
	if(false == getStreamReader(filename)){
		return;
	}
	MyName = filename;

	ReadNGCHeader();

	ReadNGCDataBlock();

	ReadNGCAttr();

	ReadNGCLink();

	{
		FileNCLRead^ nclHandle = gcnew FileNCLRead();  // NCL�n���h���쐬
		nclHandle->read(Path::GetDirectoryName(MyName) + "\\" + getNCLName(), nowDir);   //NCG�ɏ����Ă������p���b�g	��ǂݍ���
		setNCLHandle(nclHandle);   //NCL��HAS����
	}

	userReader->Close();
	addArray();  // ������o�^
	//FileManager::NCGNodeAdd(System::IO::Path::GetFileName(filename), this);


}

//--------------------------------------------------------------
/**
 * �p���b�g�n���h�����i�[
 * @param   FileNCLRead NCL�N���X
 * @retval  none
 */
//--------------------------------------------------------------

void FileNCGRead::setNCLHandle(FileNCLRead^ ncl)
{
	nclClass = ncl;
}


//--------------------------------------------------------------
/**
 * NGC�w�b�_�u���b�N�ǂݍ���
 * @param   userReader   �o�C�i�����[�_�[
 * @param   comment      �w�b�_�[�\����
 * @retval  none
 */
//--------------------------------------------------------------

void FileNCGRead::ReadNGCHeader(void)
{
	ngcHead = gcnew NCGHeader;
	ngcHead->FormatID = userReader->ReadUInt32();
	ngcHead->ByteOrderMark = userReader->ReadUInt16();
	ngcHead->FormatVersion = userReader->ReadUInt16();
	ngcHead->FileSize = userReader->ReadUInt32();
	ngcHead->HeaderSize = userReader->ReadUInt16();
	ngcHead->DataBlocks = userReader->ReadUInt16();
}


//--------------------------------------------------------------
/**
 * NGC�f�[�^�u���b�N�ǂݍ���
 * @param   userReader   �o�C�i�����[�_�[
 * @param   comment      �f�[�^�w�b�_�[�\����
 * @retval  none
 */
//--------------------------------------------------------------

void FileNCGRead::ReadNGCDataBlock(void)
{
	ngcData = gcnew NCGDataHeader;

	ngcData->BlockType = userReader->ReadUInt32();
	ngcData->BlockSize = userReader->ReadUInt32();
	ngcData->CharWidth = userReader->ReadUInt32();
	ngcData->CharHeight = userReader->ReadUInt32();
	ngcData->BitMode = userReader->ReadUInt32();
	ngcData->CharData = userReader->ReadBytes(ngcData->BlockSize-0x14);
}



//--------------------------------------------------------------
/**
 * NGCAttr�u���b�N�ǂݍ���
 * @param   userReader   �o�C�i�����[�_�[
 * @param   comment      �A�g���r���[�g�\����
 * @retval  none
 */
//--------------------------------------------------------------

void FileNCGRead::ReadNGCAttr(void)
{
	ngcAttr = gcnew NCGAttr;
	ngcAttr->BlockType = userReader->ReadUInt32();
	ngcAttr->BlockSize = userReader->ReadUInt32();
	ngcAttr->CharWidth = userReader->ReadUInt32();
	ngcAttr->CharHeight = userReader->ReadUInt32();
	ngcAttr->AttrData = userReader->ReadBytes(ngcAttr->BlockSize-0x10);
}

//--------------------------------------------------------------
/**
 * NGCLink�u���b�N�ǂݍ���
 * @param   userReader   �o�C�i�����[�_�[
 * @param   comment      �R�����g�\����
 * @retval  none
 */
//--------------------------------------------------------------

void FileNCGRead::ReadNGCLink(void)
{
	ngcLink = gcnew NCGLinkFile;
	ngcLink->BlockType = userReader->ReadUInt32();
	ngcLink->BlockSize = userReader->ReadUInt32();
	//ngcLink->FileName = userReader->ReadString();

	
	for(unsigned int i = 0 ; i < ngcLink->BlockSize - 0x8 ; i++){
		wchar_t moji = userReader->ReadChar();
		if(moji != 0){
			ngcLink->FileName = ngcLink->FileName + moji.ToString();
		}
	}
	//userReader->ReadBytes(ngcLink->BlockSize-0x8);

}

//--------------------------------------------------------------
/**
 * NCG�ɖ��ߍ��܂�Ă����p���b�g�t�@�C�����𓾂�
 * @retval  �t�@�C����
 */
//--------------------------------------------------------------

String^ FileNCGRead::getNCLName(void)
{
	return ngcLink->FileName;
}



//--------------------------------------------------------------
/**
 * 1�L�����N�^�[��������
 * @param   filename
 * @retval  StreamReader^
 */
//--------------------------------------------------------------

void FileNCGRead::Write1Char(PictureBox^ pict,Bitmap^ bmp, int dotNo, int palNo,int XOf, int YOf)
{
	int dot;

	for(int y = 0; y < 8;y++){
		for(int x = 0; x < 8;x++){
			if(ngcData->BitMode == _NGC_BITMODE4){
				dot = Get4Bit(ngcData->CharData, dotNo);
			}
			else{
				dot = ngcData->CharData[dotNo];
			}
			int palr = nclClass->getPaletteR(palNo,dot);
			int palg = nclClass->getPaletteG(palNo,dot);
			int palb = nclClass->getPaletteB(palNo,dot);
			bmp->SetPixel((XOf*8)+x, (YOf*8)+y, Color::FromArgb(palr,palg,palb)) ;  // �s�N�Z���f�[�^�̐ݒ�
			dotNo++;
		}
	}
}



//--------------------------------------------------------------
/**
 * PictureBox�ɔ��f
 * @param   filename
 * @retval  StreamReader^
 */
//--------------------------------------------------------------

Bitmap^ FileNCGRead::PictureWrite(PictureBox^ pict)
{
	Bitmap^ bmp = gcnew Bitmap(ngcData->CharWidth * 16, ngcData->CharHeight * 16);

	int dotNo = 0;
	int attrIndex=0;
	for(unsigned int YOf = 0; YOf < ngcData->CharHeight; YOf++){
		for(unsigned int XOf = 0; XOf < ngcData->CharWidth; XOf++){
			int palNo = ngcAttr->AttrData[attrIndex];
			attrIndex++;
			/*
			for(int y = 0; y < 8;y++){
				for(int x = 0; x < 8;x++){
					int dot;
					if(ngcData->BitMode == _NGC_BITMODE4){
						dot = Get4Bit(ngcData->CharData, dotNo);
					}
					else{
						dot = ngcData->CharData[dotNo];
					}

					int palr = nclClass->getPaletteR(palNo,dot);
					int palg = nclClass->getPaletteG(palNo,dot);
					int palb = nclClass->getPaletteB(palNo,dot);

					bmp->SetPixel((XOf*8)+x, (YOf*8)+y, Color::FromArgb(palr,palg,palb)) ;  // �s�N�Z���f�[�^�̐ݒ�
					dotNo++;
				}
			}
			*/
			Write1Char(pict, bmp, dotNo, palNo, XOf, YOf);
			dotNo+=64;
		}
	}
	//pict->Image = bmp;
	pict->Size.Width = ngcData->CharWidth * 16;
	pict->Size.Height = ngcData->CharHeight * 16;

	// �傫���������T�C�Y��BMP�쐬
//	pict->Image = gcnew Bitmap(ngcData->CharWidth * 16*2, ngcData->CharHeight * 16*2);
//	Graphics^ g = Graphics::FromImage(pict->Image) ;
	
//	g->InterpolationMode = System::Drawing::Drawing2D::InterpolationMode::Default;
//	g->ScaleTransform(2.0F, 2.0F) ;       // X����Y���̊g�嗦�̐ݒ�
//	g->DrawImage(bmp, 0, 0);//, ngcData->CharWidth * 16, ngcData->CharHeight * 16);
	
	return bmp;
}

//--------------------------------------------------------------
/**
 * PictureBox�ɔ��f
 * @param   filename
 * @retval  StreamReader^
 */
//--------------------------------------------------------------

Bitmap^ FileNCGRead::PictureWriteOffset(Bitmap^ bmp, PictureBox^ pict, int xini, int yini ,int width, int height)
{
	//Bitmap^ bmp = gcnew Bitmap(ngcData->CharWidth * 16, ngcData->CharHeight * 16);

	int dotNo = 0;
	int attrIndex=0;
	for(unsigned int YOf = 0; YOf < ngcData->CharHeight; YOf++){
		for(unsigned int XOf = 0; XOf < ngcData->CharWidth; XOf++){
			int palNo = ngcAttr->AttrData[attrIndex];
			attrIndex++;
			if(YOf < width && XOf < height){
				Write1Char(pict, bmp, dotNo, palNo, XOf+xini, YOf+yini);
			}
			dotNo+=64;
		}
	}
	//pict->Image = bmp;
	pict->Size.Width = ngcData->CharWidth * 16;
	pict->Size.Height = ngcData->CharHeight * 16;

	
	return bmp;
}


//--------------------------------------------------------------
/**
 * �v���W�F�N�g������𓾂�
 */
//--------------------------------------------------------------

String^ FileNCGRead::getProjectDataString()
{
	return String::Format("//RESOURCE NCG {0}",  MyName);
}

//--------------------------------------------------------------
/**
 * VRAM�̐�L�T�C�Y�𓾂�
 */
//--------------------------------------------------------------

int FileNCGRead::getVRAMSize(void)
{
	return ngcData->BlockSize-0x14;
}

