// PokeIRC.cpp : ���C�� �v���W�F�N�g �t�@�C���ł��B

#include "stdafx.h"
#include "MainForm.h"

using namespace PokeIRC;
using namespace System::Threading;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// �R���g���[�����쐬�����O�ɁAWindows XP �r�W���A�����ʂ�L���ɂ��܂�
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// ���C�� �E�B���h�E���쐬���āA���s���܂��F��
	//Application::Run(gcnew MainForm());

	MainForm^ form = gcnew MainForm();

	form->Show();

	while( form->Created ){
		Thread::Sleep( 10 );
		Application::DoEvents();
		form->Draw();
	}
	return 0;
}
