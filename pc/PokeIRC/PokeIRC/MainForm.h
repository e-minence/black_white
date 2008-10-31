#pragma once

#include "dpw_bt.h"
#include "dpw_tr.h"
#include < stdio.h >
#include < stdlib.h >
#include < vcclr.h >

namespace PokeIRC {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

#define BOX_MAX_RAW				(5)
#define BOX_MAX_COLUMN			(6)
#define BOX_MAX_TRAY			(18)
#define BOX_TRAYNAME_MAXLEN		(8)
#define BOX_TRAYNAME_BUFSIZE	(20)	// ���{��W�����{EOM�B�C�O�ŗp�̗]�T�����Ă��̒��x�B
#define BOX_MAX_POS				(BOX_MAX_RAW*BOX_MAX_COLUMN)


	static const char PROGRAM_NAME[] = "Global Pokemon FunClub";

	typedef struct{
		int no;
		char* name;
	} PokeData;

	// �{�b�N�X���X�g
	typedef struct {
	    short pokeno[BOX_MAX_POS];
	    short pokelv[BOX_MAX_POS];
	    unsigned short trayName[BOX_TRAYNAME_BUFSIZE+2];
	} BoxTrayData;


	static const	PokeData	PokeGraNoTable[]={
	0,			"�O�I���W��",
	  1,		"�t�V�M�_�l",
	  2,		"�t�V�M�\�E",
	  3,		"�t�V�M�o�i",
	  4,		"�q�g�J�Q",
	  5,		"���U�[�h",
	  6,		"���U�[�h��",
	  7,		"�[�j�K��",
	  8,		"�J���[��",
	  9,		"�J���b�N�X",
	 10,		"�L���^�s�[",
	 11,		"�g�����Z��",
	 12,		"�o�^�t���[",
	 13,		"�r�[�h��",
	 14,		"�R�N�[��",
	 15,		"�X�s�A�[",
	 16,		"�|�b�|",
	 17,		"�s�W����",
	 18,		"�s�W���b�g",
	 19,		"�R���b�^",
	 20,		"���b�^",
	 21,		"�I�j�X�Y��",
	 22,		"�I�j�h����",
	 23,		"�A�[�{",
	 24,		"�A�[�{�b�N",
	 25,		"�s�J�`���E",
	 26,		"���C�`���E",
	 27,		"�T���h",
	 28,		"�T���h�p��",
	 29,		"�j�h������",
	 30,		"�j�h���[�i",
	 31,		"�j�h�N�C��",
	 32,		"�j�h������",
	 33,		"�j�h���[�m",
	 34,		"�j�h�L���O",
	 35,		"�s�b�s",
	 36,		"�s�N�V�[",
	 37,		"���R��",
	 38,		"�L���E�R��",
	 39,		"�v����",
	 40,		"�v�N����",
	 41,		"�Y�o�b�g",
	 42,		"�S���o�b�g",
	 43,		"�i�]�m�N�T",
	 44,		"�N�T�C�n�i",
	 45,		"���t���V�A",
	 46,		"�p���X",
	 47,		"�p���Z�N�g",
	 48,		"�R���p��",
	 49,		"�����t�H��",
	 50,		"�f�B�O�_",
	 51,		"�_�O�g���I",
	 52,		"�j���[�X",
	 53,		"�y���V�A��",
	 54,		"�R�_�b�N",
	 55,		"�S���_�b�N",
	 56,		"�}���L�[",
	 57,		"�I�R���U��",
	 58,		"�K�[�f�B",
	 59,		"�E�C���f�B",
	 60,		"�j������",
	 61,		"�j�����]",
	 62,		"�j�����{��",
	 63,		"�P�[�V�B",
	 64,		"�����Q���[",
	 65,		"�t�[�f�B��",
	 66,		"�������L�[",
	 67,		"�S�[���L�[",
	 68,		"�J�C���L�[",
	 69,		"�}�_�c�{�~",
	 70,		"�E�c�h��",
	 71,		"�E�c�{�b�g",
	 72,		"���m�N���Q",
	 73,		"�h�N�N���Q",
	 74,		"�C�V�c�u�e",
	 75,		"�S���[��",
	 76,		"�S���[�j��",
	 77,		"�|�j�[�^",
	 78,		"�M�����b�v",
	 79,		"���h��",
	 80,		"���h����",
	 81,		"�R�C��",
	 82,		"���A�R�C��",
	 83,		"�J���l�M",
	 84,		"�h�[�h�[",
	 85,		"�h�[�h���I",
	 86,		"�p�E���E",
	 87,		"�W���S��",
	 88,		"�x�g�x�^�[",
	 89,		"�x�g�x�g��",
	 90,		"�V�F���_�[",
	 91,		"�p���V�F��",
	 92,		"�S�[�X",
	 93,		"�S�[�X�g",
	 94,		"�Q���K�[",
	 95,		"�C���[�N",
	 96,		"�X���[�v",
	 97,		"�X���[�p�[",
	 98,		"�N���u",
	 99,		"�L���O���[",
	100,		"�r�����_�}",
	101,		"�}���}�C��",
	102,		"�^�}�^�}",
	103,		"�i�b�V�[",
	104,		"�J���J��",
	105,		"�K���K��",
	106,		"�T�������[",
	107,		"�G�r�����[",
	108,		"�x�������K",
	109,		"�h�K�[�X",
	110,		"�}�^�h�K�X",
	111,		"�T�C�z�[��",
	112,		"�T�C�h��",
	113,		"���b�L�[",
	114,		"�����W����",
	115,		"�K���[��",
	116,		"�^�b�c�[",
	117,		"�V�[�h��",
	118,		"�g�T�L���g",
	119,		"�A�Y�}�I�E",
	120,		"�q�g�f�}��",
	121,		"�X�^�[�~�[",
	122,		"�o�����[�h",
	123,		"�X�g���C�N",
	124,		"���[�W����",
	125,		"�G���u�[",
	126,		"�u�[�o�[",
	127,		"�J�C���X",
	128,		"�P���^���X",
	129,		"�R�C�L���O",
	130,		"�M�����h�X",
	131,		"���v���X",
	132,		"���^����",
	133,		"�C�[�u�C",
	134,		"�V�����[�Y",
	135,		"�T���_�[�X",
	136,		"�u�[�X�^�[",
	137,		"�|���S��",
	138,		"�I���i�C�g",
	139,		"�I���X�^�[",
	140,		"�J�u�g",
	141,		"�J�u�g�v�X",
	142,		"�v�e��",
	143,		"�J�r�S��",
	144,		"�t���[�U�[",
	145,		"�T���_�[",
	146,		"�t�@�C���[",
	147,		"�~�j�����E",
	148,		"�n�N�����[",
	149,		"�J�C�����[",
	150,		"�~���E�c�[",
	151,		"�~���E",
	152,		"�`�R���[�^",
	153,		"�x�C���[�t",
	154,		"���K�j�E��",
	155,		"�q�m�A���V",
	156,		"�}�O�}���V",
	157,		"�o�N�t�[��",
	158,		"���j�m�R",
	159,		"�A���Q�C�c",
	160,		"�I�[�_�C��",
	161,		"�I�^�`",
	162,		"�I�I�^�`",
	163,		"�z�[�z�[",
	164,		"�����m�Y�N",
	165,		"���f�B�o",
	166,		"���f�B�A��",
	167,		"�C�g�}��",
	168,		"�A���A�h�X",
	169,		"�N���o�b�g",
	170,		"�`�����`�[",
	171,		"�����^�[��",
	172,		"�s�`���[",
	173,		"�s�B",
	174,		"�v�v����",
	175,		"�g�Q�s�[",
	176,		"�g�Q�`�b�N",
	177,		"�l�C�e�B",
	178,		"�l�C�e�B�I",
	179,		"�����[�v",
	180,		"���R�R",
	181,		"�f�������E",
	182,		"�L���C�n�i",
	183,		"�}����",
	184,		"�}������",
	185,		"�E�\�b�L�[",
	186,		"�j�����g�m",
	187,		"�n�l�b�R",
	188,		"�|�|�b�R",
	189,		"���^�b�R",
	190,		"�G�C�p��",
	191,		"�q�}�i�b�c",
	192,		"�L�}����",
	193,		"���������}",
	194,		"�E�p�[",
	195,		"�k�I�[",
	196,		"�G�[�t�B",
	197,		"�u���b�L�[",
	198,		"���~�J���X",
	199,		"���h�L���O",
	200,		"���E�}",
	201,		"�A���m�[��",
	202,		"�\�[�i���X",
	203,		"�L�������L",
	204,		"�N�k�M�_�}",
	205,		"�t�H���g�X",
	206,		"�m�R�b�`",
	207,		"�O���C�K�[",
	208,		"�n�K�l�[��",
	209,		"�u���[",
	210,		"�O�����u��",
	211,		"�n���[�Z��",
	212,		"�n�b�T��",
	213,		"�c�{�c�{",
	214,		"�w���N���X",
	215,		"�j���[��",
	216,		"�q���O�}",
	217,		"�����O�}",
	218,		"�}�O�}�b�O",
	219,		"�}�O�J���S",
	220,		"�E�����[",
	221,		"�C�m���[",
	222,		"�T�j�[�S",
	223,		"�e�b�|�E�I",
	224,		"�I�N�^��",
	225,		"�f���o�[�h",
	226,		"�}���^�C��",
	227,		"�G�A�[���h",
	228,		"�f���r��",
	229,		"�w���K�[",
	230,		"�L���O�h��",
	231,		"�S�}�]�E",
	232,		"�h���t�@��",
	233,		"�|���S���Q",
	234,		"�I�h�V�V",
	235,		"�h�[�u��",
	236,		"�o���L�[",
	237,		"�J�|�G���[",
	238,		"���`���[��",
	239,		"�G���L�b�h",
	240,		"�u�r�B",
	241,		"�~���^���N",
	242,		"�n�s�i�X",
	243,		"���C�R�E",
	244,		"�G���e�C",
	245,		"�X�C�N��",
	246,		"���[�M���X",
	247,		"�T�i�M���X",
	248,		"�o���M���X",
	249,		"���M�A",
	250,		"�z�E�I�E",
	251,		"�Z���r�B",
	252,		"�L����",
	253,		"�W���v�g��",
	254,		"�W���J�C��",
	255,		"�A�`����",
	256,		"���J�V����",
	257,		"�o�V���[��",
	258,		"�~�Y�S���E",
	259,		"�k�}�N���[",
	260,		"���O���[�W",
	261,		"�|�`�G�i",
	262,		"�O���G�i",
	263,		"�W�O�U�O�}",
	264,		"�}�b�X�O�}",
	265,		"�P���b�\",
	266,		"�J���T���X",
	267,		"�A�Q�n���g",
	268,		"�}�����h",
	269,		"�h�N�P�C��",
	270,		"�n�X�{�[",
	271,		"�n�X�u����",
	272,		"�����p�b�p",
	273,		"�^�l�{�[",
	274,		"�R�m�n�i",
	275,		"�_�[�e���O",
	276,		"�X�o��",
	277,		"�I�I�X�o��",
	278,		"�L������",
	279,		"�y���b�p�[",
	280,		"�����g�X",
	281,		"�L�����A",
	282,		"�T�[�i�C�g",
	283,		"�A���^�}",
	284,		"�A�����[�X",
	285,		"�L�m�R�R",
	286,		"�L�m�K�b�T",
	287,		"�i�}�P��",
	288,		"�����L���m",
	289,		"�P�b�L���O",
	290,		"�c�`�j��",
	291,		"�e�b�J�j��",
	292,		"�k�P�j��",
	293,		"�S�j���j��",
	294,		"�h�S�[��",
	295,		"�o�N�I���O",
	296,		"�}�N�m�V�^",
	297,		"�n���e���}",
	298,		"������",
	299,		"�m�Y�p�X",
	300,		"�G�l�R",
	301,		"�G�l�R����",
	302,		"���~���~",
	303,		"�N�`�[�g",
	304,		"�R�R�h��",
	305,		"�R�h��",
	306,		"�{�X�S�h��",
	307,		"�A�T�i��",
	308,		"�`���[����",
	309,		"���N���C",
	310,		"���C�{���g",
	311,		"�v���X��",
	312,		"�}�C�i��",
	313,		"�o���r�[�g",
	314,		"�C���~�[�[",
	315,		"���[���A",
	316,		"�S�N����",
	317,		"�}���m�[��",
	318,		"�L�o�j�A",
	319,		"�T���n�_�[",
	320,		"�z�G���R",
	321,		"�z�G���I�[",
	322,		"�h������",
	323,		"�o�N�[�_",
	324,		"�R�[�^�X",
	325,		"�o�l�u�[",
	326,		"�u�[�s�b�O",
	327,		"�p�b�`�[��",
	328,		"�i�b�N���[",
	329,		"�r�u���[�o",
	330,		"�t���C�S��",
	331,		"�T�{�l�A",
	332,		"�m�N�^�X",
	333,		"�`���b�g",
	334,		"�`���^���X",
	335,		"�U���O�[�X",
	336,		"�n�u�l�[�N",
	337,		"���i�g�[��",
	338,		"�\�����b�N",
	339,		"�h�W���b�`",
	340,		"�i�}�Y��",
	341,		"�w�C�K�j",
	342,		"�V�U���K�[",
	343,		"���W����",
	344,		"�l���h�[��",
	345,		"�����[��",
	346,		"�����C�h��",
	347,		"�A�m�v�X",
	348,		"�A�[�}���h",
	349,		"�q���o�X",
	350,		"�~���J���X",
	351,		"�|������",
	352,		"�J�N���I��",
	353,		"�J�Q�{�E�Y",
	354,		"�W���y�b�^",
	355,		"���}����",
	356,		"�T�}���[��",
	357,		"�g���s�E�X",
	358,		"�`���[��",
	359,		"�A�u�\��",
	360,		"�\�[�i�m",
	361,		"���L�����V",
	362,		"�I�j�S�[��",
	363,		"�^�}�U���V",
	364,		"�g�h�O���[",
	365,		"�g�h�[���K",
	366,		"�p�[����",
	367,		"�n���e�[��",
	368,		"�T�N���r�X",
	369,		"�W�[�����X",
	370,		"���u�J�X",
	371,		"�^�c�x�C",
	372,		"�R�����[",
	373,		"�{�[�}���_",
	374,		"�_���o��",
	375,		"���^���O",
	376,		"���^�O���X",
	377,		"���W���b�N",
	378,		"���W�A�C�X",
	379,		"���W�X�`��",
	380,		"���e�B�A�X",
	381,		"���e�B�I�X",
	382,		"�J�C�I�[�K",
	383,		"�O���[�h��",
	384,		"���b�N�E�U",
	385,		"�W���[�`",
	386,		"�f�I�L�V�X",
	432,		"�i�G�g��",
	433,		"�n���V�K��",
	434,		"�h�_�C�g�X",
	430,		"�q�R�U��",
	435,		"���E�J�U��",
	436,		"�S�E�J�U��",
	431,		"�|�b�`���}",
	437,		"�|�b�^�C�V",
	438,		"�G���y���g",
	429,		"���b�N��",
	448,		"���N�o�[�h",
	449,		"���N�z�[�N",
	461,		"�r�b�p",
	462,		"�r�[�_��",
	480,		"�R���{�[�V",
	481,		"�R���g�b�N",
	472,		"�R�����N",
	473,		"���N�V�I",
	474,		"�����g���[",
	428,		"�X�{�~�[",
	421,		"���Y���C�h",
	489,		"�Y�K�C�h�X",
	490,		"�����p���h",
	487,		"�^�e�g�v�X",
	488,		"�g���f�v�X",
	455,		"�~�m���b�`",
	457,		"�~�m�}�_��",
	456,		"�K�[���C��",
	470,		"�~�c�n�j�[",
	471,		"�r�[�N�C��",
	477,		"�p�`���X",
	418,		"�u�C�[��",
	440,		"�t���[�[��",
	482,		"�`�F�����{",
	483,		"�`�F����",
	458,		"�J���i�N�V",
	459,		"�g���g�h��",
	496,		"�G�e�{�[�X",
	450,		"�t�����e",
	451,		"�t�����C�h",
	420,		"�~�~����",
	454,		"�~�~���b�v",
	500,		"���E�}�[�W",
	494,		"�h���J���X",
	426,		"�j�����}�[",
	463,		"�u�j���b�g",
	445,		"���[�V����",
	468,		"�X�J���v�[",
	469,		"�X�J�^���N",
	478,		"�h�[�~���[",
	479,		"�h�[�^�N��",
	413,		"�E�\�n�`",
	414,		"�}�l�l",
	444,		"�s���v�N",
	419,		"�y���b�v",
	460,		"�~�J���Q",
	491,		"�t�J�}��",
	492,		"�K�o�C�g",
	493,		"�K�u���A�X",
	412,		"�S���x",
	439,		"���I��",
	416,		"���J���I",
	464,		"�q�|�|�^�X",
	465,		"�J�o���h��",
	485,		"�X�R���s",
	486,		"�h���s�I��",
	452,		"�O���b�O��",
	453,		"�h�N���b�O",
	484,		"�}�X�L�b�p",
	466,		"�P�C�R�E�I",
	467,		"�l�I�����g",
	417,		"�^�}���^",
	475,		"���L�J�u��",
	476,		"���L�m�I�[",
	415,		"�}�j���[��",
	443,		"�W�o�R�C��",
	501,		"�x���x���g",
	446,		"�h�T�C�h��",
	422,		"���W�����{",
	427,		"�G���L�u��",
	502,		"�u�[�o�[��",
	495,		"�g�Q�L�b�X",
	499,		"���K�����}",
	505,		"���[�t�B�A",
	506,		"�O���C�V�A",
	447,		"�O���C�I��",
	424,		"�}�����[",
	504,		"�|���S���y",
	498,		"�G�����C�h",
	497,		"�_�C�m�[�Y",
	423,		"���m���[��",
	425,		"���L���m�R",
	519,		"���g��",
	511,		"���N�V�[",
	512,		"�G�����b�g",
	513,		"�A�O�m��",
	514,		"�f�B�A���K",
	515,		"�p���L�A",
	508,		"�q�[�h����",
	510,		"���W�M�K�X",
	509,		"�M���e�B�i",
	507,		"�N���Z���A",
	441,		"�t�B�I�l",
	442,		"�}�i�t�B",
	517,		"�_�[�N���C",
	516,		"�V�F�C�~",
	518,		"�A���Z�E�X",
};



	/// <summary>
	/// Form1 �̊T�v
	///
	/// �x��: ���̃N���X�̖��O��ύX����ꍇ�A���̃N���X���ˑ����邷�ׂĂ� .resx �t�@�C���Ɋ֘A�t����ꂽ
	///          �}�l�[�W ���\�[�X �R���p�C�� �c�[���ɑ΂��� 'Resource File Name' �v���p�e�B��
	///          �ύX����K�v������܂��B���̕ύX���s��Ȃ��ƁA
	///          �f�U�C�i�ƁA���̃t�H�[���Ɋ֘A�t����ꂽ���[�J���C�Y�ς݃��\�[�X�Ƃ��A
	///          ���������݂ɗ��p�ł��Ȃ��Ȃ�܂��B
	/// </summary>
	public ref class MainForm : public System::Windows::Forms::Form
	{
	public:
		MainForm(void)
		{
			InitializeComponent();
			//
			//TODO: �����ɃR���X�g���N�^ �R�[�h��ǉ����܂�
			//

     //   button1->Text = "����̓{�^��";
	//	button1->Dock = DockStyle::Top;
  //      button1->Click += new EventHandler(button1_Click);
		webBrowser1->Dock = DockStyle::Fill;
    //    Controls->Add(webBrowser1);
      //  Controls->Add(button1);
//        Load += new EventHandler(Form1_Load);

		}

	protected:
		/// <summary>
		/// �g�p���̃��\�[�X�����ׂăN���[���A�b�v���܂��B
		/// </summary>
		~MainForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::WebBrowser^  webBrowser1;
	private: System::Windows::Forms::StatusStrip^  statusStrip1;
	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^  fileFToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  sToolStripMenuItem;
	public: System::Windows::Forms::ToolStripStatusLabel^  StripStatusLabel;
	private: System::Windows::Forms::Timer^  timer;
	private: System::Windows::Forms::ToolStripMenuItem^  sendDataDToolStripMenuItem;
	private: System::Windows::Forms::OpenFileDialog^  openFileDialog1;
	public: System::Windows::Forms::ToolStripStatusLabel^  StripStatusLabelCenter;

	private: System::ComponentModel::IContainer^  components;
	public: System::ComponentModel::BackgroundWorker^ cmBackgroundWorker;
	private: System::Windows::Forms::ToolStripMenuItem^  gTSTestGToolStripMenuItem;
	public: 
	private: System::Threading::Thread^ threadA;

	static const int POKMEON_BOX_NUM = 18;  //BOX18��
	static const int POKMEON_BOX_POKENUM = 30; //�|�P�����R�O��
	int dispBoxNo;
	static int targetPoke;   // ���I�����Ă���|�P����No
	static const unsigned long long TEST_KEY = 0x100000000UL;
	static const int POKEMON_MAX = 494;
	bool bBoxListRecv;

	private: System::Windows::Forms::ToolStripMenuItem^  gTSResetToolStripMenuItem;
	private: System::Windows::Forms::ToolStripContainer^  toolStripContainer1;
	private: System::Windows::Forms::PictureBox^  pictureBox1;
	private: System::Windows::Forms::ToolStripMenuItem^  pokemonToolStripMenuItem;
	private: System::Windows::Forms::SplitContainer^  splitContainer1;
	private: System::Windows::Forms::Button^  button3;
	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::ToolTip^  toolTip1;
	private: System::Windows::Forms::ToolStripMenuItem^  exitEToolStripMenuItem;
	private: System::Windows::Forms::ContextMenuStrip^  contextMenuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^  sendGTSSToolStripMenuItem;









	private: System::Windows::Forms::ToolStripMenuItem^  dSGTSSyncTToolStripMenuItem;




	protected: 

	private:
		/// <summary>
		/// �K�v�ȃf�U�C�i�ϐ��ł��B
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// �f�U�C�i �T�|�[�g�ɕK�v�ȃ��\�b�h�ł��B���̃��\�b�h�̓��e��
		/// �R�[�h �G�f�B�^�ŕύX���Ȃ��ł��������B
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->webBrowser1 = (gcnew System::Windows::Forms::WebBrowser());
			this->statusStrip1 = (gcnew System::Windows::Forms::StatusStrip());
			this->StripStatusLabel = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->StripStatusLabelCenter = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->fileFToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->sToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->sendDataDToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->gTSTestGToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->gTSResetToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->dSGTSSyncTToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->pokemonToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->exitEToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->timer = (gcnew System::Windows::Forms::Timer(this->components));
			this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->toolStripContainer1 = (gcnew System::Windows::Forms::ToolStripContainer());
			this->splitContainer1 = (gcnew System::Windows::Forms::SplitContainer());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->toolTip1 = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->contextMenuStrip1 = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->sendGTSSToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->statusStrip1->SuspendLayout();
			this->menuStrip1->SuspendLayout();
			this->toolStripContainer1->BottomToolStripPanel->SuspendLayout();
			this->toolStripContainer1->ContentPanel->SuspendLayout();
			this->toolStripContainer1->TopToolStripPanel->SuspendLayout();
			this->toolStripContainer1->SuspendLayout();
			this->splitContainer1->Panel1->SuspendLayout();
			this->splitContainer1->Panel2->SuspendLayout();
			this->splitContainer1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->BeginInit();
			this->contextMenuStrip1->SuspendLayout();
			this->SuspendLayout();
			// 
			// webBrowser1
			// 
			this->webBrowser1->Location = System::Drawing::Point(176, 198);
			this->webBrowser1->MinimumSize = System::Drawing::Size(20, 20);
			this->webBrowser1->Name = L"webBrowser1";
			this->webBrowser1->Size = System::Drawing::Size(115, 77);
			this->webBrowser1->TabIndex = 0;
			this->webBrowser1->DocumentCompleted += gcnew System::Windows::Forms::WebBrowserDocumentCompletedEventHandler(this, &MainForm::webBrowser1_DocumentCompleted);
			// 
			// statusStrip1
			// 
			this->statusStrip1->Dock = System::Windows::Forms::DockStyle::None;
			this->statusStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->StripStatusLabel, 
				this->StripStatusLabelCenter});
			this->statusStrip1->Location = System::Drawing::Point(0, 0);
			this->statusStrip1->Name = L"statusStrip1";
			this->statusStrip1->Size = System::Drawing::Size(638, 22);
			this->statusStrip1->TabIndex = 1;
			this->statusStrip1->Text = L"statusStrip1";
			// 
			// StripStatusLabel
			// 
			this->StripStatusLabel->Name = L"StripStatusLabel";
			this->StripStatusLabel->Size = System::Drawing::Size(11, 17);
			this->StripStatusLabel->Text = L"1";
			// 
			// StripStatusLabelCenter
			// 
			this->StripStatusLabelCenter->Name = L"StripStatusLabelCenter";
			this->StripStatusLabelCenter->Size = System::Drawing::Size(11, 17);
			this->StripStatusLabelCenter->Text = L"2";
			// 
			// menuStrip1
			// 
			this->menuStrip1->Dock = System::Windows::Forms::DockStyle::None;
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->fileFToolStripMenuItem});
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(638, 24);
			this->menuStrip1->TabIndex = 2;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// fileFToolStripMenuItem
			// 
			this->fileFToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(7) {this->sToolStripMenuItem, 
				this->sendDataDToolStripMenuItem, this->gTSTestGToolStripMenuItem, this->gTSResetToolStripMenuItem, this->dSGTSSyncTToolStripMenuItem, 
				this->pokemonToolStripMenuItem, this->exitEToolStripMenuItem});
			this->fileFToolStripMenuItem->Name = L"fileFToolStripMenuItem";
			this->fileFToolStripMenuItem->Size = System::Drawing::Size(51, 20);
			this->fileFToolStripMenuItem->Text = L"File(&F)";
			// 
			// sToolStripMenuItem
			// 
			this->sToolStripMenuItem->Name = L"sToolStripMenuItem";
			this->sToolStripMenuItem->Size = System::Drawing::Size(153, 22);
			this->sToolStripMenuItem->Text = L"DS�Ɛڑ�(&S)";
			this->sToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::sToolStripMenuItem_Click);
			// 
			// sendDataDToolStripMenuItem
			// 
			this->sendDataDToolStripMenuItem->Name = L"sendDataDToolStripMenuItem";
			this->sendDataDToolStripMenuItem->Size = System::Drawing::Size(153, 22);
			this->sendDataDToolStripMenuItem->Text = L"SendData(&D)";
			this->sendDataDToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::sendDataDToolStripMenuItem_Click);
			// 
			// gTSTestGToolStripMenuItem
			// 
			this->gTSTestGToolStripMenuItem->Name = L"gTSTestGToolStripMenuItem";
			this->gTSTestGToolStripMenuItem->Size = System::Drawing::Size(153, 22);
			this->gTSTestGToolStripMenuItem->Text = L"GTSTest(&G)";
			this->gTSTestGToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::gTSTestGToolStripMenuItem_Click);
			// 
			// gTSResetToolStripMenuItem
			// 
			this->gTSResetToolStripMenuItem->Name = L"gTSResetToolStripMenuItem";
			this->gTSResetToolStripMenuItem->Size = System::Drawing::Size(153, 22);
			this->gTSResetToolStripMenuItem->Text = L"GTSReset(&R)";
			this->gTSResetToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::gTSResetToolStripMenuItem_Click);
			// 
			// dSGTSSyncTToolStripMenuItem
			// 
			this->dSGTSSyncTToolStripMenuItem->Name = L"dSGTSSyncTToolStripMenuItem";
			this->dSGTSSyncTToolStripMenuItem->Size = System::Drawing::Size(153, 22);
			this->dSGTSSyncTToolStripMenuItem->Text = L"DS-GTSSync(&T)";
			this->dSGTSSyncTToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::dSGTSSyncTToolStripMenuItem_Click);
			// 
			// pokemonToolStripMenuItem
			// 
			this->pokemonToolStripMenuItem->Name = L"pokemonToolStripMenuItem";
			this->pokemonToolStripMenuItem->Size = System::Drawing::Size(153, 22);
			this->pokemonToolStripMenuItem->Text = L"PutPokemon(&P)";
			this->pokemonToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::pokemonToolStripMenuItem_Click);
			// 
			// exitEToolStripMenuItem
			// 
			this->exitEToolStripMenuItem->Name = L"exitEToolStripMenuItem";
			this->exitEToolStripMenuItem->Size = System::Drawing::Size(153, 22);
			this->exitEToolStripMenuItem->Text = L"Exit(&E)";
			this->exitEToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::exitEToolStripMenuItem_Click);
			// 
			// timer
			// 
			this->timer->Interval = 8;
			this->timer->Tick += gcnew System::EventHandler(this, &MainForm::timer_Tick);
			// 
			// openFileDialog1
			// 
			this->openFileDialog1->FileName = L"openFileDialog1";
			// 
			// toolStripContainer1
			// 
			// 
			// toolStripContainer1.BottomToolStripPanel
			// 
			this->toolStripContainer1->BottomToolStripPanel->Controls->Add(this->statusStrip1);
			// 
			// toolStripContainer1.ContentPanel
			// 
			this->toolStripContainer1->ContentPanel->AutoScroll = true;
			this->toolStripContainer1->ContentPanel->Controls->Add(this->splitContainer1);
			this->toolStripContainer1->ContentPanel->Size = System::Drawing::Size(638, 452);
			this->toolStripContainer1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->toolStripContainer1->Location = System::Drawing::Point(0, 0);
			this->toolStripContainer1->Name = L"toolStripContainer1";
			this->toolStripContainer1->Size = System::Drawing::Size(638, 498);
			this->toolStripContainer1->TabIndex = 3;
			this->toolStripContainer1->Text = L"toolStripContainer1";
			// 
			// toolStripContainer1.TopToolStripPanel
			// 
			this->toolStripContainer1->TopToolStripPanel->Controls->Add(this->menuStrip1);
			// 
			// splitContainer1
			// 
			this->splitContainer1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer1->Location = System::Drawing::Point(0, 0);
			this->splitContainer1->Name = L"splitContainer1";
			this->splitContainer1->Orientation = System::Windows::Forms::Orientation::Horizontal;
			// 
			// splitContainer1.Panel1
			// 
			this->splitContainer1->Panel1->Controls->Add(this->button3);
			this->splitContainer1->Panel1->Controls->Add(this->button2);
			this->splitContainer1->Panel1->Controls->Add(this->button1);
			// 
			// splitContainer1.Panel2
			// 
			this->splitContainer1->Panel2->Controls->Add(this->webBrowser1);
			this->splitContainer1->Panel2->Controls->Add(this->pictureBox1);
			this->splitContainer1->Size = System::Drawing::Size(638, 452);
			this->splitContainer1->SplitterDistance = 91;
			this->splitContainer1->TabIndex = 2;
			// 
			// button3
			// 
			this->button3->Dock = System::Windows::Forms::DockStyle::Fill;
			this->button3->Location = System::Drawing::Point(526, 0);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(112, 91);
			this->button3->TabIndex = 2;
			this->button3->Text = L">>";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &MainForm::button3_Click);
			// 
			// button2
			// 
			this->button2->Dock = System::Windows::Forms::DockStyle::Left;
			this->button2->Location = System::Drawing::Point(68, 0);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(458, 91);
			this->button2->TabIndex = 1;
			this->button2->Text = L"�{�b�N�X�P";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &MainForm::button2_Click);
			// 
			// button1
			// 
			this->button1->Dock = System::Windows::Forms::DockStyle::Left;
			this->button1->Location = System::Drawing::Point(0, 0);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(68, 91);
			this->button1->TabIndex = 0;
			this->button1->Text = L"<<";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &MainForm::button1_Click);
			// 
			// pictureBox1
			// 
			this->pictureBox1->Location = System::Drawing::Point(41, 47);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(250, 83);
			this->pictureBox1->TabIndex = 1;
			this->pictureBox1->TabStop = false;
			this->toolTip1->SetToolTip(this->pictureBox1, L"�|�P����");
			this->pictureBox1->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::pictureBox1_MouseMove);
			this->pictureBox1->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &MainForm::pictureBox1_DragDrop);
			this->pictureBox1->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::pictureBox1_MouseClick);
			this->pictureBox1->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::pictureBox1_MouseDown);
			this->pictureBox1->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &MainForm::pictureBox1_DragEnter);
			// 
			// contextMenuStrip1
			// 
			this->contextMenuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->sendGTSSToolStripMenuItem});
			this->contextMenuStrip1->Name = L"contextMenuStrip1";
			this->contextMenuStrip1->Size = System::Drawing::Size(133, 26);
			// 
			// sendGTSSToolStripMenuItem
			// 
			this->sendGTSSToolStripMenuItem->Name = L"sendGTSSToolStripMenuItem";
			this->sendGTSSToolStripMenuItem->Size = System::Drawing::Size(132, 22);
			this->sendGTSSToolStripMenuItem->Text = L"SendGTS(&S)";
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(638, 498);
			this->Controls->Add(this->toolStripContainer1);
			this->MainMenuStrip = this->menuStrip1;
			this->Name = L"MainForm";
			this->Text = L"PC-IRC-DS";
			this->Load += gcnew System::EventHandler(this, &MainForm::Form1_Load);
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &MainForm::MainForm_FormClosing);
			this->statusStrip1->ResumeLayout(false);
			this->statusStrip1->PerformLayout();
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			this->toolStripContainer1->BottomToolStripPanel->ResumeLayout(false);
			this->toolStripContainer1->BottomToolStripPanel->PerformLayout();
			this->toolStripContainer1->ContentPanel->ResumeLayout(false);
			this->toolStripContainer1->TopToolStripPanel->ResumeLayout(false);
			this->toolStripContainer1->TopToolStripPanel->PerformLayout();
			this->toolStripContainer1->ResumeLayout(false);
			this->toolStripContainer1->PerformLayout();
			this->splitContainer1->Panel1->ResumeLayout(false);
			this->splitContainer1->Panel2->ResumeLayout(false);
			this->splitContainer1->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->EndInit();
			this->contextMenuStrip1->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
private: System::Void sToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e);
private: System::Void timer_Tick(System::Object^  sender, System::EventArgs^  e);
private: System::Void MainForm_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e);
private: System::Void sendDataDToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void DoWork( System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e );
private: System::Void RunWorkerCompleted( System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e );
private: void ThreadWork(void);
private: void TestUploadDownload(int pid, String^ proxy);
private: static void SetProxy(String^ proxy);
private: static bool RequestCheckServerState(void);
private: static int WaitForAsync(void);
private: static bool RequestSetProfile(void);
private: static void RequestUpload(void);
private: static void SetTrData(Dpw_Tr_Data* upload_data, int pokeNo);
private: static void SetProfile(Dpw_Common_Profile* profile);
private: void TestDownload(int pid,  String^ proxy);
private: static bool RequestPickupTraded(void);
private: int getPokemonNumberFromThePlace(int x,int y);
private: int getBoxPositionFromThePlace(int x,int y);
private: void GetPokeBoxList(void);




private: System::Void gTSTestGToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);

private: System::Void gTSResetToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);

private: System::Void webBrowser1_DocumentCompleted(System::Object^  sender, System::Windows::Forms::WebBrowserDocumentCompletedEventArgs^  e) {
		 }
private: System::Void dSGTSSyncTToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);

public: void CallProg(String^ message);

private: System::Void pokemonToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);

private: System::Void pictureBox1_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
private: System::Void pictureBox1_DragDrop(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e) {



		 }
private: System::Void pictureBox1_DragEnter(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e) {
			 if ( e->Data->GetDataPresent(DataFormats::FileDrop) ){
					 e->Effect = DragDropEffects::All;
				}
		 }

private: void pokemonListDisp(int boxNo);
private: System::Void button3_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void exitEToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void pictureBox1_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
private: System::Void pictureBox1_MouseClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);

private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
};
}

