########################################################
#
#
#		�O���t�B�b�N�t�@�C�����낢�됶���R���o�[�^
#
#
########################################################


#���͏o�̓t�@�C��
$GRADATA_FILE_NAME		= "wflby_gradata.xls";
$GRADATA_TXTFILE_NAME	= "gradata.txt";
$GRADATA_OUTGRADATAFILE	= "wflby_3dmapobj_data.c";
$GRADATA_ARCLIST_FILE	= "list.txt";
$GRADATA_CONVLIST_FILE	= "convlist.bat";


@GRADATA_FILE = undef;			#�O���t�B�b�N�f�[�^�t�@�C��

@GRA_LIST = undef;				#�A�[�J�C�u����t�@�C���̃��X�g
@GRA_CONVLIST = undef;			#�R���o�[�g����t�@�C���̃��X�g

@GRA_NORMAL_FIRE = undef;		#�m�[�}���΂̕����t�@�C��
@GRA_NORMAL_WATER = undef;		#�m�[�}�����̕����t�@�C��
@GRA_NORMAL_SPARK = undef;		#�m�[�}���d�C�̕����t�@�C��
@GRA_NORMAL_GRASS = undef;		#�m�[�}�����̕����t�@�C��
@GRA_NORMAL_SPECIAL = undef;	#�m�[�}������̕����t�@�C��

@GRA_SPRING_FIRE = undef;		#�t�΂̕����t�@�C��
@GRA_SPRING_WATER = undef;		#�t���̕����t�@�C��
@GRA_SPRING_SPARK = undef;		#�t�d�C�̕����t�@�C��
@GRA_SPRING_GRASS = undef;		#�t���̕����t�@�C��
@GRA_SPRING_SPECIAL = undef;	#�t����̕����t�@�C��

@GRA_SUMMER_FIRE = undef;		#�ĉ΂̕����t�@�C��
@GRA_SUMMER_WATER = undef;		#�Đ��̕����t�@�C��
@GRA_SUMMER_SPARK = undef;		#�ēd�C�̕����t�@�C��
@GRA_SUMMER_GRASS = undef;		#�đ��̕����t�@�C��
@GRA_SUMMER_SPECIAL = undef;	#�ē���̕����t�@�C��

@GRA_FALL_FIRE = undef;			#�H�΂̕����t�@�C��
@GRA_FALL_WATER = undef;		#�H���̕����t�@�C��
@GRA_FALL_SPARK = undef;		#�H�d�C�̕����t�@�C��
@GRA_FALL_GRASS = undef;		#�H���̕����t�@�C��
@GRA_FALL_SPECIAL = undef;		#�H����̕����t�@�C��

@GRA_WINTER_FIRE = undef;		#�~�΂̕����t�@�C��
@GRA_WINTER_WATER = undef;		#�~���̕����t�@�C��
@GRA_WINTER_SPARK = undef;		#�~�d�C�̕����t�@�C��
@GRA_WINTER_GRASS = undef;		#�~���̕����t�@�C��
@GRA_WINTER_SPECIAL = undef;	#�~����̕����t�@�C��


#�o�b�t�@��INDEXENUM
$GRA_ENUM_DOUZOU00			=  0;		#�傫������
$GRA_ENUM_DOUZOU0100		=  1;		#�����������O�O
$GRA_ENUM_DOUZOU0101		=  2;		#�����������O�P
$GRA_ENUM_DOUZOU0102		=  3;		#�����������O�Q
$GRA_ENUM_DOUZOU0103		=  4;		#�����������O�R
$GRA_ENUM_TREE				=  5;		#��
$GRA_ENUM_MG00				=  6;		#�~�j�Q�[���O�O
$GRA_ENUM_MG01				=  7;		#�~�j�Q�[���O�P
$GRA_ENUM_MG02				=  8;		#�~�j�Q�[���O�Q
$GRA_ENUM_TABLE				=  9;		#�e�[�u��
$GRA_ENUM_KANBAN			= 10;		#�e�[�u��
$GRA_ENUM_LAMP00			= 11;		#�e�[�u��
$GRA_ENUM_LAMP01			= 12;		#�e�[�u��
$GRA_ENUM_HANABI			= 13;		#�ԉ�
$GRA_ENUM_HANABIBIG			= 14;		#�ԉ΍Ō�
$GRA_ENUM_BRHANABI			= 15;		#�����ԉ�
$GRA_ENUM_BRHANABIBIG		= 16;		#�����ԉ΍Ō�
$GRA_ENUM_ANKETO			= 17;		#�����ԉ΍Ō�
$GRA_ENUM_DOUZOU00ANM00		= 18;		#�傫������
$GRA_ENUM_DOUZOU00ANM01		= 19;		#�傫������
$GRA_ENUM_DOUZOU00ANM02		= 20;		#�傫������
$GRA_ENUM_DOUZOU0100ANM00	= 21;		#�����������O�O
$GRA_ENUM_DOUZOU0100ANM01	= 22;		#�����������O�O
$GRA_ENUM_DOUZOU0100ANM02	= 23;		#�����������O�O
$GRA_ENUM_DOUZOU0101ANM00	= 24;		#�����������O�P
$GRA_ENUM_DOUZOU0101ANM01	= 25;		#�����������O�P
$GRA_ENUM_DOUZOU0101ANM02	= 26;		#�����������O�P
$GRA_ENUM_DOUZOU0102ANM00	= 27;		#�����������O�Q
$GRA_ENUM_DOUZOU0102ANM01	= 28;		#�����������O�Q
$GRA_ENUM_DOUZOU0102ANM02	= 29;		#�����������O�Q
$GRA_ENUM_DOUZOU0103ANM00	= 30;		#�����������O�R
$GRA_ENUM_DOUZOU0103ANM01	= 31;		#�����������O�R
$GRA_ENUM_DOUZOU0103ANM02	= 32;		#�����������O�R
$GRA_ENUM_TREEANM00			= 33;		#��
$GRA_ENUM_TREEANM01			= 34;		#��
$GRA_ENUM_TREEANM02			= 35;		#��
$GRA_ENUM_MG00ANM00			= 36;		#�~�j�Q�[���O�O
$GRA_ENUM_MG00ANM01			= 37;		#�~�j�Q�[���O�O
$GRA_ENUM_MG00ANM02			= 38;		#�~�j�Q�[���O�O
$GRA_ENUM_MG01ANM00			= 39;		#�~�j�Q�[���O�P
$GRA_ENUM_MG01ANM01			= 40;		#�~�j�Q�[���O�P
$GRA_ENUM_MG01ANM02			= 41;		#�~�j�Q�[���O�P
$GRA_ENUM_MG02ANM00			= 42;		#�~�j�Q�[���O�Q
$GRA_ENUM_MG02ANM01			= 43;		#�~�j�Q�[���O�Q
$GRA_ENUM_MG02ANM02			= 44;		#�~�j�Q�[���O�Q
$GRA_ENUM_TABLEANM00		= 45;		#�~�j�Q�[���O�Q
$GRA_ENUM_TABLEANM01		= 46;		#�~�j�Q�[���O�Q
$GRA_ENUM_TABLEANM02		= 47;		#�~�j�Q�[���O�Q
$GRA_ENUM_KANBANANM00		= 48;		#�J���o��
$GRA_ENUM_KANBANANM01		= 49;		#�J���o��
$GRA_ENUM_KANBANANM02		= 50;		#�J���o��
$GRA_ENUM_LAMP00ANM00		= 51;		#�����v�O�O
$GRA_ENUM_LAMP00ANM01		= 52;		#�����v�O�O
$GRA_ENUM_LAMP00ANM02		= 53;		#�����v�O�O
$GRA_ENUM_LAMP01ANM00		= 54;		#�����v�O�P
$GRA_ENUM_LAMP01ANM01		= 55;		#�����v�O�P
$GRA_ENUM_LAMP01ANM02		= 56;		#�����v�O�P
$GRA_ENUM_HANABIANM00		= 57;		#�ԉ�
$GRA_ENUM_HANABIANM01		= 58;		#�ԉ�
$GRA_ENUM_HANABIANM02		= 59;		#�ԉ�
$GRA_ENUM_HANABIBIGANM00	= 60;		#�ԉ΍Ō�
$GRA_ENUM_HANABIBIGANM01	= 61;		#�ԉ΍Ō�
$GRA_ENUM_HANABIBIGANM02	= 62;		#�ԉ΍Ō�
$GRA_ENUM_BRHANABIANM00		= 63;		#�����ԉ�
$GRA_ENUM_BRHANABIANM01		= 64;		#�����ԉ�
$GRA_ENUM_BRHANABIANM02		= 65;		#�����ԉ�
$GRA_ENUM_BRHANABIBIGANM00	= 66;		#�����ԉ΍Ō�
$GRA_ENUM_BRHANABIBIGANM01	= 67;		#�����ԉ΍Ō�
$GRA_ENUM_BRHANABIBIGANM02	= 68;		#�����ԉ΍Ō�
$GRA_ENUM_ANKETOANM00		= 69;		#�A���P�[�g
$GRA_ENUM_ANKETOANM01		= 70;		#�A���P�[�g
$GRA_ENUM_ANKETOANM02		= 71;		#�A���P�[�g
$GRA_ENUM_FLOAT00IMD		= 72;		#�t���[�g00IMD
$GRA_ENUM_FLOAT01IMD		= 73;		#�t���[�g01IMD
$GRA_ENUM_FLOAT00TEX00		= 74;		#�t���[�g00�F00
$GRA_ENUM_FLOAT00TEX01		= 75;		#�t���[�g00�F01
$GRA_ENUM_FLOAT00TEX02		= 76;		#�t���[�g00�F02
$GRA_ENUM_FLOAT01TEX00		= 77;		#�t���[�g01�F00
$GRA_ENUM_FLOAT01TEX01		= 78;		#�t���[�g01�F01
$GRA_ENUM_FLOAT01TEX02		= 79;		#�t���[�g01�F02
$GRA_ENUM_FLOAT00ANM00		= 80;		#�t���[�g00�A�j��00
$GRA_ENUM_FLOAT00ANM01		= 81;		#�t���[�g00�A�j��01
$GRA_ENUM_FLOAT00ANM02		= 82;		#�t���[�g00�A�j��02
$GRA_ENUM_FLOAT00ANM03		= 83;		#�t���[�g00�A�j��03
$GRA_ENUM_FLOAT01ANM00		= 84;		#�t���[�g01�A�j��00
$GRA_ENUM_FLOAT01ANM01		= 85;		#�t���[�g01�A�j��01
$GRA_ENUM_FLOAT01ANM02		= 86;		#�t���[�g01�A�j��02
$GRA_ENUM_FLOAT01ANM03		= 87;		#�t���[�g01�A�j��03
$GRA_ENUM_FLOAT00NODENUM	= 88;		#�t���[�g00NODE��
$GRA_ENUM_FLOAT01NODENUM	= 89;		#�t���[�g01NODE��
$GRA_ENUM_FLOAT00ANM01NODE	= 90;		#�t���[�g00�A�j��01NODE
$GRA_ENUM_FLOAT00ANM02NODE	= 91;		#�t���[�g00�A�j��02NODE
$GRA_ENUM_FLOAT00ANM03NODE	= 92;		#�t���[�g00�A�j��03NODE
$GRA_ENUM_FLOAT01ANM01NODE	= 93;		#�t���[�g01�A�j��01NODE
$GRA_ENUM_FLOAT01ANM02NODE	= 94;		#�t���[�g01�A�j��02NODE
$GRA_ENUM_FLOAT01ANM03NODE	= 95;		#�t���[�g01�A�j��03NODE
$GRA_ENUM_MAP				= 96;		#�n�`
$GRA_ENUM_MAT				= 97;		#�}�b�g
$GRA_ENUM_NODENUM			= 98;		#�n�`�m�[�h��
$GRA_ENUM_MAPANM00			= 99;		#�n�`�A�j���O�O
$GRA_ENUM_MAPANM01			=100;		#�n�`�A�j���O�P
$GRA_ENUM_MAPANM02			=101;		#�n�`�A�j���O�Q
$GRA_ENUM_MAPANM03			=102;		#�n�`�A�j���O�R
$GRA_ENUM_MAPANM04			=103;		#�n�`�|�[��
$GRA_ENUM_MAPANM00NODE		=104;		#�n�`�A�j���O�O�m�[�h
$GRA_ENUM_MAPANM01NODE		=105;		#�n�`�A�j���O�P�m�[�h
$GRA_ENUM_MAPANM02NODE		=106;		#�n�`�A�j���O�Q�m�[�h
$GRA_ENUM_MAPANM03NODE		=107;		#�n�`�A�j���O�R�m�[�h
$GRA_ENUM_MAPANM04NODE		=108;		#�n�`�|�[���m�[�h
$GRA_ENUM_NUM				=109;		#��


#�O���t�B�b�N���X�g���f�[�^�萔
$GRADATA_ENUM_ROOMNAME			=  0;		#�����̖��O
$GRADATA_ENUM_DOUZOU00			=  1;		#������IMD
$GRADATA_ENUM_DOUZOU00_A00		=  2;		#������풓�A�j��
$GRADATA_ENUM_DOUZOU00_A01		=  3;		#������풓�A�j��
$GRADATA_ENUM_DOUZOU00_A02		=  4;		#������풓�A�j��
$GRADATA_ENUM_DOUZOU01_00		=  5;		#������IMD
$GRADATA_ENUM_DOUZOU01_00A		=  6;		#�������A�j��
$GRADATA_ENUM_DOUZOU01_01		=  7;		#������IMD
$GRADATA_ENUM_DOUZOU01_01A		=  8;		#�������A�j��
$GRADATA_ENUM_DOUZOU01_02		=  9;		#������IMD
$GRADATA_ENUM_DOUZOU01_02A		= 10;		#�������A�j��
$GRADATA_ENUM_DOUZOU01_03		= 11;		#������IMD
$GRADATA_ENUM_DOUZOU01_03A		= 12;		#�������A�j��
$GRADATA_ENUM_DOUZOU01_03A1		= 13;		#�������A�j��
$GRADATA_ENUM_DOUZOU01_03A2		= 14;		#�������A�j��
$GRADATA_ENUM_TREE				= 15;		#��IMD
$GRADATA_ENUM_KANBAN			= 16;		#�Ŕ�IMD
$GRADATA_ENUM_KANBANANM			= 17;		#�Ŕ�IMD
$GRADATA_ENUM_TABLE				= 18;		#��IMD
$GRADATA_ENUM_LAMP00			= 19;		#�����v�O�OIMD
$GRADATA_ENUM_LAMP01			= 20;		#�����v�O�PIMD
$GRADATA_ENUM_LAMPANM_00		= 21;		#�����v�_���A�j��
$GRADATA_ENUM_LAMPANM_01		= 22;		#�����v���A�j��
$GRADATA_ENUM_LAMPANM_02		= 23;		#�����v���A�j��
$GRADATA_ENUM_HANABI			= 24;		#�ԉ�
$GRADATA_ENUM_HANABIANM00		= 25;		#�ԉΒʏ�A�j��
$GRADATA_ENUM_HANABIANM01		= 26;		#�ԉΒʏ�A�j��
$GRADATA_ENUM_HANABIANM02		= 27;		#�ԉΒʏ�A�j��
$GRADATA_ENUM_HANABIBIG			= 28;		#�ԉ΍Ō�
$GRADATA_ENUM_HANABIBIGANM00	= 29;		#�ԉ΍Ō�A�j��
$GRADATA_ENUM_HANABIBIGANM01	= 30;		#�ԉ΍Ō�A�j��
$GRADATA_ENUM_BRHANABI			= 31;		#�ԉ�
$GRADATA_ENUM_BRHANABIANM00		= 32;		#�ԉΒʏ�A�j��
$GRADATA_ENUM_BRHANABIANM01		= 33;		#�ԉΒʏ�A�j��
$GRADATA_ENUM_BRHANABIANM02		= 34;		#�ԉΒʏ�A�j��
$GRADATA_ENUM_BRHANABIBIG		= 35;		#�ԉ΍Ō�
$GRADATA_ENUM_BRHANABIBIGANM00	= 36;		#�ԉ΍Ō�A�j��
$GRADATA_ENUM_BRHANABIBIGANM01	= 37;		#�ԉ΍Ō�A�j��
$GRADATA_ENUM_BRHANABIBIGANM02	= 38;		#�ԉ΍Ō�A�j��
$GRADATA_ENUM_ANKETO			= 39;		#�ԉ΍Ō�A�j��
$GRADATA_ENUM_FLOAT00_00		= 40;		#�t���[�g00�F00IMD
$GRADATA_ENUM_FLOAT00_01		= 41;		#�t���[�g00�F01IMD
$GRADATA_ENUM_FLOAT00_02		= 42;		#�t���[�g00�F02IMD
$GRADATA_ENUM_FLOAT00_ANM00		= 43;		#�t���[�g00�A�j��00
$GRADATA_ENUM_FLOAT00_ANM01		= 44;		#�t���[�g00�A�j��01
$GRADATA_ENUM_FLOAT00_ANM01N	= 45;		#�t���[�g00�A�j��01NODE
$GRADATA_ENUM_FLOAT00_ANM02		= 46;		#�t���[�g00�A�j��02
$GRADATA_ENUM_FLOAT00_ANM02N	= 47;		#�t���[�g00�A�j��02NODE
$GRADATA_ENUM_FLOAT00_ANM03		= 48;		#�t���[�g00�A�j��03
$GRADATA_ENUM_FLOAT00_ANM03N	= 49;		#�t���[�g00�A�j��03NODE
$GRADATA_ENUM_FLOAT01_00		= 50;		#�t���[�g01�F00IMD
$GRADATA_ENUM_FLOAT01_01		= 51;		#�t���[�g01�F01IMD
$GRADATA_ENUM_FLOAT01_02		= 52;		#�t���[�g01�F02IMD
$GRADATA_ENUM_FLOAT01_ANM00		= 53;		#�t���[�g01�A�j��00
$GRADATA_ENUM_FLOAT01_ANM01		= 54;		#�t���[�g01�A�j��01
$GRADATA_ENUM_FLOAT01_ANM01N	= 55;		#�t���[�g01�A�j��01NODE
$GRADATA_ENUM_FLOAT01_ANM02		= 56;		#�t���[�g01�A�j��02
$GRADATA_ENUM_FLOAT01_ANM02N	= 57;		#�t���[�g01�A�j��02NODE
$GRADATA_ENUM_FLOAT01_ANM03		= 58;		#�t���[�g01�A�j��03
$GRADATA_ENUM_FLOAT01_ANM03N	= 59;		#�t���[�g01�A�j��03NODE
$GRADATA_ENUM_MAP				= 60;		#�}�b�v
$GRADATA_ENUM_MAT				= 61;		#�}�b�g
$GRADATA_ENUM_MAPANM00			= 62;		#�n�`�A�j���O�O
$GRADATA_ENUM_MAPANM00NODE		= 63;		#�n�`�A�j���O�O�m�[�h
$GRADATA_ENUM_MAPANM01			= 64;		#�n�`�A�j���O�P
$GRADATA_ENUM_MAPANM01NODE		= 65;		#�n�`�A�j���O�P�m�[�h
$GRADATA_ENUM_MAPANM02			= 66;		#�n�`�A�j���O�Q
$GRADATA_ENUM_MAPANM02NODE		= 67;		#�n�`�A�j���O�Q�m�[�h
$GRADATA_ENUM_MAPANM03			= 68;		#�n�`�A�j���O�Q
$GRADATA_ENUM_MAPANM03NODE		= 69;		#�n�`�A�j���O�Q�m�[�h
$GRADATA_ENUM_MAPPOLL			= 70;		#�n�`�|�[��
$GRADATA_ENUM_MAPPOLLNODE		= 71;		#�n�`�|�[���m�[�h
$GRADATA_ENUM_MG00				= 72;		#�~�j�Q�[���O�O
$GRADATA_ENUM_MG00ANM00			= 73;		#�~�j�Q�[���O�O�A�j���O�O
$GRADATA_ENUM_MG00ANM01			= 74;		#�~�j�Q�[���O�O�A�j���O�P
$GRADATA_ENUM_MG01				= 75;		#�~�j�Q�[���O�P
$GRADATA_ENUM_MG01ANM00			= 76;		#�~�j�Q�[���O�P�A�j���O�O
$GRADATA_ENUM_MG01ANM01			= 77;		#�~�j�Q�[���O�P�A�j���O�P
$GRADATA_ENUM_MG02				= 78;		#�~�j�Q�[���O�Q
$GRADATA_ENUM_MG02ANM00			= 79;		#�~�j�Q�[���O�Q�A�j���O�O
$GRADATA_ENUM_MG02ANM01			= 80;		#�~�j�Q�[���O�Q�A�j���O�P
$GRADATA_ENUM_NUM				= 81;		#��
                                 
                                 
#�m�[�h����DUMMY�̂Ƃ��̒l       
$NODE_DUMMY		= 0xffffffff;    
                                 
#�}�b�v�z�u�I�u�W�F�A�j����      
$MAPOBJ_ANM_MAX		= 2;	    #wflby_3dmapobj_data.h�̔z�u�I�u�W�F�A�j���ő吔�Ƃ��킹�邱��
                                 
#NARC_NAME                       
$GRA_NARC_TOP				= "NARC_wifi_lobby_";
                                 
                                 
#�t�@�C���ǂݍ��݊J�n�I�t�Z�b �g 
$GRADATA_YOFFS				= 3; 
                                 
                                 
#NODE�C���f�b�N�X�쐬            
system( "perl node_h_make.pl" ); 
                                 
#�t�@�C���ǂݍ���                
&filein();                       
                                 
#�e�����̃f�[�^���擾����        
&filedataset( \@GRA_NORMAL_FIRE,    0 );
&filedataset( \@GRA_NORMAL_WATER,	1 );
&filedataset( \@GRA_NORMAL_SPARK,	2 );
&filedataset( \@GRA_NORMAL_GRASS,	3 );
&filedataset( \@GRA_NORMAL_SPECIAL,	4 );

&filedataset( \@GRA_SPRING_FIRE,	5 );
&filedataset( \@GRA_SPRING_WATER,	6 );
&filedataset( \@GRA_SPRING_SPARK,	7 );
&filedataset( \@GRA_SPRING_GRASS,	8 );
&filedataset( \@GRA_SPRING_SPECIAL,	9 );

&filedataset( \@GRA_SUMMER_FIRE,	10 );
&filedataset( \@GRA_SUMMER_WATER,	11 );
&filedataset( \@GRA_SUMMER_SPARK,	12 );
&filedataset( \@GRA_SUMMER_GRASS,	13 );
&filedataset( \@GRA_SUMMER_SPECIAL,	14 );

&filedataset( \@GRA_FALL_FIRE,	15 );
&filedataset( \@GRA_FALL_WATER,	16 );
&filedataset( \@GRA_FALL_SPARK,	17 );
&filedataset( \@GRA_FALL_GRASS,	18 );
&filedataset( \@GRA_FALL_SPECIAL,19 );

&filedataset( \@GRA_WINTER_FIRE,	20 );
&filedataset( \@GRA_WINTER_WATER,	21 );
&filedataset( \@GRA_WINTER_SPARK,	22 );
&filedataset( \@GRA_WINTER_GRASS,	23 );
&filedataset( \@GRA_WINTER_SPECIAL,	24 );


#�O���t�B�b�N�����f�[�^�����o��
&fileout_gradata();

#�ǂݍ��񂾂̂Ő����t�@�C��������
system( "rm $GRADATA_TXTFILE_NAME" );
system( "rm *.h" );

exit(0);

sub filein{
	system( "../../tools/exceltool/ExcelSeetConv.exe -o $GRADATA_TXTFILE_NAME -p 0 -s csv $GRADATA_FILE_NAME" );
	open( FILEIN, $GRADATA_TXTFILE_NAME );
	@GRADATA_FILE = <FILEIN>;
	close( FILEIN );

}


sub filedataset{
	my( $buff, $idx ) = @_;
	my( @onedata, $list_set ); 
	my( @node_h );

	$idx += $GRADATA_YOFFS;
	
	#@GRADATA_FILE��$idx�Ԗڂ̗v�f��@$buff�Ɋi�[����
	$GRADATA_FILE[ $idx ] =~ s/\r\n//g;
	@onedata = split( /,/, $GRADATA_FILE[ $idx ] );	#,�ŋ�؂��Ĕz��ɂ���

	#�z�u�I�u�W�F
	{
		#�傫������
		$list_set = $onedata[ $GRADATA_ENUM_DOUZOU00 ];
		&gra_conv_list_add( $list_set );		#�R���o�[�g���X�g�ɐݒ�
		$list_set =~ s/imd/nsbmd/;
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_DOUZOU00 ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_DOUZOU00 ] =~ s/\./_/;

		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_DOUZOU00_A00 ], $buff, $GRA_ENUM_DOUZOU00ANM00, $$buff[ $GRA_ENUM_DOUZOU00 ] );
		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_DOUZOU00_A01 ], $buff, $GRA_ENUM_DOUZOU00ANM01, $$buff[ $GRA_ENUM_DOUZOU00 ] );
		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_DOUZOU00_A02 ], $buff, $GRA_ENUM_DOUZOU00ANM02, $$buff[ $GRA_ENUM_DOUZOU00 ] );


		#����������00
		$list_set = $onedata[ $GRADATA_ENUM_DOUZOU01_00 ];
		&gra_conv_list_add( $list_set );		#�R���o�[�g���X�g�ɐݒ�
		$list_set =~ s/imd/nsbmd/;
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_DOUZOU0100 ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_DOUZOU0100 ] =~ s/\./_/;
		
		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_DOUZOU01_00A ], $buff, $GRA_ENUM_DOUZOU0100ANM00, $$buff[ $GRA_ENUM_DOUZOU0100 ] );

		#�A�j���͂Ȃ��̂Ń��f���Ɠ������b�Z�[�W�����Ƃ��܂�
		$$buff[ $GRA_ENUM_DOUZOU0100ANM01 ] = $$buff[ $GRA_ENUM_DOUZOU0100 ];
		$$buff[ $GRA_ENUM_DOUZOU0100ANM02 ] = $$buff[ $GRA_ENUM_DOUZOU0100 ];

		
		#����������01
		$list_set = $onedata[ $GRADATA_ENUM_DOUZOU01_01 ];
		&gra_conv_list_add( $list_set );		#�R���o�[�g���X�g�ɐݒ�
		$list_set =~ s/imd/nsbmd/;
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_DOUZOU0101 ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_DOUZOU0101 ] =~ s/\./_/;
		
		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_DOUZOU01_01A ], $buff, $GRA_ENUM_DOUZOU0101ANM00, $$buff[ $GRA_ENUM_DOUZOU0101 ] );

		#�A�j���͂Ȃ��̂Ń��f���Ɠ������b�Z�[�W�����Ƃ��܂�
		$$buff[ $GRA_ENUM_DOUZOU0101ANM01 ] = $$buff[ $GRA_ENUM_DOUZOU0101 ];
		$$buff[ $GRA_ENUM_DOUZOU0101ANM02 ] = $$buff[ $GRA_ENUM_DOUZOU0101 ];


		#����������02
		$list_set = $onedata[ $GRADATA_ENUM_DOUZOU01_02 ];
		&gra_conv_list_add( $list_set );		#�R���o�[�g���X�g�ɐݒ�
		$list_set =~ s/imd/nsbmd/;
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_DOUZOU0102 ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_DOUZOU0102 ] =~ s/\./_/;
		
		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_DOUZOU01_02A ], $buff, $GRA_ENUM_DOUZOU0102ANM00, $$buff[ $GRA_ENUM_DOUZOU0102 ] );

		#�A�j���͂Ȃ��̂Ń��f���Ɠ������b�Z�[�W�����Ƃ��܂�
		$$buff[ $GRA_ENUM_DOUZOU0102ANM01 ] = $$buff[ $GRA_ENUM_DOUZOU0102 ];
		$$buff[ $GRA_ENUM_DOUZOU0102ANM02 ] = $$buff[ $GRA_ENUM_DOUZOU0102 ];


		#����������03
		$list_set = $onedata[ $GRADATA_ENUM_DOUZOU01_03 ];
		&gra_conv_list_add( $list_set );		#�R���o�[�g���X�g�ɐݒ�
		$list_set =~ s/imd/nsbmd/;
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_DOUZOU0103 ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_DOUZOU0103 ] =~ s/\./_/;
		
		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_DOUZOU01_03A ], $buff, $GRA_ENUM_DOUZOU0103ANM00, $$buff[ $GRA_ENUM_DOUZOU0103 ] );
		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_DOUZOU01_03A1 ], $buff, $GRA_ENUM_DOUZOU0103ANM01, $$buff[ $GRA_ENUM_DOUZOU0103 ] );
		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_DOUZOU01_03A2 ], $buff, $GRA_ENUM_DOUZOU0103ANM02, $$buff[ $GRA_ENUM_DOUZOU0103 ] );

		

		#��
		$list_set = $onedata[ $GRADATA_ENUM_TREE ];
		&gra_conv_list_add( $list_set );		#�R���o�[�g���X�g�ɐݒ�
		$list_set =~ s/imd/nsbmd/;
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_TREE ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_TREE ] =~ s/\./_/;

		#�؃A�j���͂Ȃ��̂Ń��f���Ɠ������b�Z�[�W�����Ƃ��܂�
		$$buff[ $GRA_ENUM_TREEANM00 ] = $$buff[ $GRA_ENUM_TREE ];
		$$buff[ $GRA_ENUM_TREEANM01 ] = $$buff[ $GRA_ENUM_TREE ];
		$$buff[ $GRA_ENUM_TREEANM02 ] = $$buff[ $GRA_ENUM_TREE ];


		#��
		$list_set = $onedata[ $GRADATA_ENUM_TABLE ];
		&gra_conv_list_add( $list_set );		#�R���o�[�g���X�g�ɐݒ�
		$list_set =~ s/imd/nsbmd/;
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_TABLE ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_TABLE ] =~ s/\./_/;

		#�A�j���͂Ȃ��̂Ń��f���Ɠ������b�Z�[�W�����Ƃ��܂�
		$$buff[ $GRA_ENUM_TABLEANM00 ] = $$buff[ $GRA_ENUM_TABLE ];
		$$buff[ $GRA_ENUM_TABLEANM01 ] = $$buff[ $GRA_ENUM_TABLE ];
		$$buff[ $GRA_ENUM_TABLEANM02 ] = $$buff[ $GRA_ENUM_TABLE ];


		#�Ŕ�
		$list_set = $onedata[ $GRADATA_ENUM_KANBAN ];
		&gra_conv_list_add( $list_set );		#�R���o�[�g���X�g�ɐݒ�
		$list_set =~ s/imd/nsbmd/;
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_KANBAN ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_KANBAN ] =~ s/\./_/;

		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_KANBANANM ], $buff, $GRA_ENUM_KANBANANM00, $$buff[ $GRA_ENUM_KANBAN ] );

		#�A�j���͂Ȃ��̂Ń��f���Ɠ������b�Z�[�W�����Ƃ��܂�
		$$buff[ $GRA_ENUM_KANBANANM01 ] = $$buff[ $GRA_ENUM_KANBAN ];
		$$buff[ $GRA_ENUM_KANBANANM02 ] = $$buff[ $GRA_ENUM_KANBAN ];
		

		#�����v�O�O
		$list_set = $onedata[ $GRADATA_ENUM_LAMP00 ];
		&gra_conv_list_add( $list_set );		#�R���o�[�g���X�g�ɐݒ�
		$list_set =~ s/imd/nsbmd/;
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_LAMP00 ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_LAMP00 ] =~ s/\./_/;

		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_LAMPANM_00 ], $buff, $GRA_ENUM_LAMP00ANM00, $$buff[ $GRA_ENUM_LAMP00 ] );
		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_LAMPANM_01 ], $buff, $GRA_ENUM_LAMP00ANM01, $$buff[ $GRA_ENUM_LAMP00 ] );

		#�A�j���͂Ȃ��̂Ń��f���Ɠ������b�Z�[�W�����Ƃ��܂�
		$$buff[ $GRA_ENUM_LAMP00ANM02 ] = $$buff[ $GRA_ENUM_LAMP00 ];


		#�����v�O�P
		$list_set = $onedata[ $GRADATA_ENUM_LAMP01 ];
		&gra_conv_list_add( $list_set );		#�R���o�[�g���X�g�ɐݒ�
		$list_set =~ s/imd/nsbmd/;
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_LAMP01 ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_LAMP01 ] =~ s/\./_/;

		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_LAMPANM_00 ], $buff, $GRA_ENUM_LAMP01ANM00, $$buff[ $GRA_ENUM_LAMP01 ] );
		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_LAMPANM_02 ], $buff, $GRA_ENUM_LAMP01ANM01, $$buff[ $GRA_ENUM_LAMP01 ] );

		#�A�j���͂Ȃ��̂Ń��f���Ɠ������b�Z�[�W�����Ƃ��܂�
		$$buff[ $GRA_ENUM_LAMP01ANM02 ] = $$buff[ $GRA_ENUM_LAMP01 ];

		#�ԉ�
		$list_set = $onedata[ $GRADATA_ENUM_HANABI ];
		&gra_conv_list_add( $list_set );		#�R���o�[�g���X�g�ɐݒ�
		$list_set =~ s/imd/nsbmd/;
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_HANABI ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_HANABI ] =~ s/\./_/;

		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_HANABIANM00 ], $buff, $GRA_ENUM_HANABIANM00, $$buff[ $GRA_ENUM_HANABI ] );
		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_HANABIANM01 ], $buff, $GRA_ENUM_HANABIANM01, $$buff[ $GRA_ENUM_HANABI ] );
		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_HANABIANM02 ], $buff, $GRA_ENUM_HANABIANM02, $$buff[ $GRA_ENUM_HANABI ] );


		#�ԉ΍Ō�
		$list_set = $onedata[ $GRADATA_ENUM_HANABIBIG ];
		&gra_conv_list_add( $list_set );		#�R���o�[�g���X�g�ɐݒ�
		$list_set =~ s/imd/nsbmd/;
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_HANABIBIG ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_HANABIBIG ] =~ s/\./_/;

		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_HANABIBIGANM00 ], $buff, $GRA_ENUM_HANABIBIGANM00, $$buff[ $GRA_ENUM_HANABIBIG ] );
		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_HANABIBIGANM01 ], $buff, $GRA_ENUM_HANABIBIGANM01, $$buff[ $GRA_ENUM_HANABIBIG ] );

		#�A�j���͂Ȃ��̂Ń��f���Ɠ������b�Z�[�W�����Ƃ��܂�
		$$buff[ $GRA_ENUM_HANABIBIGANM02 ] = $$buff[ $GRA_ENUM_HANABIBIG ];


		#�����ԉ�
		$list_set = $onedata[ $GRADATA_ENUM_BRHANABI ];
		&gra_conv_list_add( $list_set );		#�R���o�[�g���X�g�ɐݒ�
		$list_set =~ s/imd/nsbmd/;
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_BRHANABI ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_BRHANABI ] =~ s/\./_/;

		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_BRHANABIANM00 ], $buff, $GRA_ENUM_BRHANABIANM00, $$buff[ $GRA_ENUM_BRHANABI ] );
		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_BRHANABIANM01 ], $buff, $GRA_ENUM_BRHANABIANM01, $$buff[ $GRA_ENUM_BRHANABI ] );
		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_BRHANABIANM02 ], $buff, $GRA_ENUM_BRHANABIANM02, $$buff[ $GRA_ENUM_BRHANABI ] );

		#�����ԉ΍Ō�
		$list_set = $onedata[ $GRADATA_ENUM_BRHANABIBIG ];
		&gra_conv_list_add( $list_set );		#�R���o�[�g���X�g�ɐݒ�
		$list_set =~ s/imd/nsbmd/;
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_BRHANABIBIG ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_BRHANABIBIG ] =~ s/\./_/;

		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_BRHANABIBIGANM00 ], $buff, $GRA_ENUM_BRHANABIBIGANM00, $$buff[ $GRA_ENUM_BRHANABIBIG ] );
		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_BRHANABIBIGANM01 ], $buff, $GRA_ENUM_BRHANABIBIGANM01, $$buff[ $GRA_ENUM_BRHANABIBIG ] );
		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_BRHANABIBIGANM02 ], $buff, $GRA_ENUM_BRHANABIBIGANM02, $$buff[ $GRA_ENUM_BRHANABIBIG ] );

		#�A���P�[�g
		$list_set = $onedata[ $GRADATA_ENUM_ANKETO ];
		&gra_conv_list_add( $list_set );		#�R���o�[�g���X�g�ɐݒ�
		$list_set =~ s/imd/nsbmd/;
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_ANKETO ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_ANKETO ] =~ s/\./_/;

		#�؃A�j���͂Ȃ��̂Ń��f���Ɠ������b�Z�[�W�����Ƃ��܂�
		$$buff[ $GRA_ENUM_ANKETOANM00 ] = $$buff[ $GRA_ENUM_ANKETO ];
		$$buff[ $GRA_ENUM_ANKETOANM01 ] = $$buff[ $GRA_ENUM_ANKETO ];
		$$buff[ $GRA_ENUM_ANKETOANM02 ] = $$buff[ $GRA_ENUM_ANKETO ];


		#�~�j�Q�[���O
		$list_set = $onedata[ $GRADATA_ENUM_MG00 ];
		&gra_conv_list_add( $list_set );		#�R���o�[�g���X�g�ɐݒ�
		$list_set =~ s/imd/nsbmd/;
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_MG00 ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_MG00 ] =~ s/\./_/;
		
		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_MG00ANM00 ], $buff, $GRA_ENUM_MG00ANM00, $$buff[ $GRA_ENUM_MG00 ] );
		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_MG00ANM01 ], $buff, $GRA_ENUM_MG00ANM01, $$buff[ $GRA_ENUM_MG00 ] );

		#�A�j���͂Ȃ��̂Ń��f���Ɠ������b�Z�[�W�����Ƃ��܂�
		$$buff[ $GRA_ENUM_MG00ANM02 ] = $$buff[ $GRA_ENUM_MG00 ];


		#�~�j�Q�[���P
		$list_set = $onedata[ $GRADATA_ENUM_MG01 ];
		&gra_conv_list_add( $list_set );		#�R���o�[�g���X�g�ɐݒ�
		$list_set =~ s/imd/nsbmd/;
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_MG01 ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_MG01 ] =~ s/\./_/;

		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_MG01ANM00 ], $buff, $GRA_ENUM_MG01ANM00, $$buff[ $GRA_ENUM_MG01 ] );
		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_MG01ANM01 ], $buff, $GRA_ENUM_MG01ANM01, $$buff[ $GRA_ENUM_MG01 ] );
		
		#�A�j���͂Ȃ��̂Ń��f���Ɠ������b�Z�[�W�����Ƃ��܂�
		$$buff[ $GRA_ENUM_MG01ANM02 ] = $$buff[ $GRA_ENUM_MG01 ];

		#�~�j�Q�[���Q
		$list_set = $onedata[ $GRADATA_ENUM_MG02 ];
		&gra_conv_list_add( $list_set );		#�R���o�[�g���X�g�ɐݒ�
		$list_set =~ s/imd/nsbmd/;
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_MG02 ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_MG02 ] =~ s/\./_/;

		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_MG02ANM00 ], $buff, $GRA_ENUM_MG02ANM00, $$buff[ $GRA_ENUM_MG02 ] );
		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_MG02ANM01 ], $buff, $GRA_ENUM_MG02ANM01, $$buff[ $GRA_ENUM_MG02 ] );
		
		#�A�j���͂Ȃ��̂Ń��f���Ɠ������b�Z�[�W�����Ƃ��܂�
		$$buff[ $GRA_ENUM_MG02ANM02 ] = $$buff[ $GRA_ENUM_MG02 ];

	
	}

	#FLOAT
	{
		#�t���[�g00IMD
		$list_set = $onedata[ $GRADATA_ENUM_FLOAT00_00 ];
		&gra_conv_list_add( $list_set." -emdl" );		#�R���o�[�g���X�g�ɐݒ�
		$list_set =~ s/imd/nsbmd/;
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_FLOAT00IMD ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_FLOAT00IMD ] =~ s/\./_/;

		#�t���[�g01IMD
		$list_set = $onedata[ $GRADATA_ENUM_FLOAT01_00 ];
		&gra_conv_list_add( $list_set." -emdl" );		#�R���o�[�g���X�g�ɐݒ�
		$list_set =~ s/imd/nsbmd/;
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_FLOAT01IMD ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_FLOAT01IMD ] =~ s/\./_/;

		#�t���[�g00TEX00
		$list_set = $onedata[ $GRADATA_ENUM_FLOAT00_00 ];
		&gra_conv_list_add( $list_set." -etex" );		#�R���o�[�g���X�g�ɐݒ�
		$list_set =~ s/imd/nsbtx/;
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_FLOAT00TEX00 ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_FLOAT00TEX00 ] =~ s/\./_/;
		#�t���[�g00TEX01
		$list_set = $onedata[ $GRADATA_ENUM_FLOAT00_01 ];
		&gra_conv_list_add( $list_set." -etex" );		#�R���o�[�g���X�g�ɐݒ�
		$list_set =~ s/imd/nsbtx/;
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_FLOAT00TEX01 ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_FLOAT00TEX01 ] =~ s/\./_/;
		#�t���[�g00TEX02
		$list_set = $onedata[ $GRADATA_ENUM_FLOAT00_02 ];
		&gra_conv_list_add( $list_set." -etex" );		#�R���o�[�g���X�g�ɐݒ�
		$list_set =~ s/imd/nsbtx/;
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_FLOAT00TEX02 ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_FLOAT00TEX02 ] =~ s/\./_/;

		#�t���[�g01TEX00
		$list_set = $onedata[ $GRADATA_ENUM_FLOAT01_00 ];
		&gra_conv_list_add( $list_set." -etex" );		#�R���o�[�g���X�g�ɐݒ�
		$list_set =~ s/imd/nsbtx/;
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_FLOAT01TEX00 ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_FLOAT01TEX00 ] =~ s/\./_/;
		#�t���[�g01TEX01
		$list_set = $onedata[ $GRADATA_ENUM_FLOAT01_01 ];
		&gra_conv_list_add( $list_set." -etex" );		#�R���o�[�g���X�g�ɐݒ�
		$list_set =~ s/imd/nsbtx/;
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_FLOAT01TEX01 ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_FLOAT01TEX01 ] =~ s/\./_/;
		#�t���[�g01TEX02
		$list_set = $onedata[ $GRADATA_ENUM_FLOAT01_02 ];
		&gra_conv_list_add( $list_set." -etex" );		#�R���o�[�g���X�g�ɐݒ�
		$list_set =~ s/imd/nsbtx/;
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_FLOAT01TEX02 ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_FLOAT01TEX02 ] =~ s/\./_/;

		#�t���[�g00ANM00
		$list_set = $onedata[ $GRADATA_ENUM_FLOAT00_ANM00 ];
		&gra_conv_list_add( $list_set );		#�R���o�[�g���X�g�ɐݒ�
		$list_set = &anm_file_change( $list_set );	#�A�j���t�@�C������ύX
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_FLOAT00ANM00 ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_FLOAT00ANM00 ] =~ s/\./_/;
		#�t���[�g00ANM01
		$list_set = $onedata[ $GRADATA_ENUM_FLOAT00_ANM01 ];
		&gra_conv_list_add( $list_set );		#�R���o�[�g���X�g�ɐݒ�
		$list_set = &anm_file_change( $list_set );	#�A�j���t�@�C������ύX
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_FLOAT00ANM01 ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_FLOAT00ANM01 ] =~ s/\./_/;
		#�t���[�g00ANM02
		$list_set = $onedata[ $GRADATA_ENUM_FLOAT00_ANM02 ];
		&gra_conv_list_add( $list_set );		#�R���o�[�g���X�g�ɐݒ�
		$list_set = &anm_file_change( $list_set );	#�A�j���t�@�C������ύX
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_FLOAT00ANM02 ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_FLOAT00ANM02 ] =~ s/\./_/;

		#�t���[�g00ANM03
		$list_set = $onedata[ $GRADATA_ENUM_FLOAT00_ANM03 ];
		&gra_conv_list_add( $list_set );		#�R���o�[�g���X�g�ɐݒ�
		$list_set = &anm_file_change( $list_set );	#�A�j���t�@�C������ύX
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_FLOAT00ANM03 ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_FLOAT00ANM03 ] =~ s/\./_/;

		#�t���[�g01ANM00
		$list_set = $onedata[ $GRADATA_ENUM_FLOAT01_ANM00 ];
		&gra_conv_list_add( $list_set );		#�R���o�[�g���X�g�ɐݒ�
		$list_set = &anm_file_change( $list_set );	#�A�j���t�@�C������ύX
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_FLOAT01ANM00 ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_FLOAT01ANM00 ] =~ s/\./_/;
		#�t���[�g01ANM01
		$list_set = $onedata[ $GRADATA_ENUM_FLOAT01_ANM01 ];
		&gra_conv_list_add( $list_set );		#�R���o�[�g���X�g�ɐݒ�
		$list_set = &anm_file_change( $list_set );	#�A�j���t�@�C������ύX
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_FLOAT01ANM01 ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_FLOAT01ANM01 ] =~ s/\./_/;
		#�t���[�g01ANM02
		$list_set = $onedata[ $GRADATA_ENUM_FLOAT01_ANM02 ];
		&gra_conv_list_add( $list_set );		#�R���o�[�g���X�g�ɐݒ�
		$list_set = &anm_file_change( $list_set );	#�A�j���t�@�C������ύX
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_FLOAT01ANM02 ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_FLOAT01ANM02 ] =~ s/\./_/;

		#�t���[�g01ANM03
		$list_set = $onedata[ $GRADATA_ENUM_FLOAT01_ANM03 ];
		&gra_conv_list_add( $list_set );		#�R���o�[�g���X�g�ɐݒ�
		$list_set = &anm_file_change( $list_set );	#�A�j���t�@�C������ύX
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_FLOAT01ANM03 ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_FLOAT01ANM03 ] =~ s/\./_/;


		#�m�[�h�f�[�^
		#FLOAT00
		{
			#�t�@�C��������m�[�h�w�b�_��ǂݍ���
			$list_set = $onedata[ $GRADATA_ENUM_FLOAT00_00 ];
			$list_set =~ s/imd/h/g;
			open( FILEIN, "$list_set" );
			@node_h = <FILEIN>;
			close( FILEIN );

			#NODEMAX
			$$buff[ $GRA_ENUM_FLOAT00NODENUM ] = &get_node_h_max( \@node_h );

			#FLOAT00ANM01NODE
			$list_set = $onedata[ $GRADATA_ENUM_FLOAT00_ANM01N ];
			$$buff[ $GRA_ENUM_FLOAT00ANM01NODE ] = &get_node_h_nodeno( \@node_h, $list_set );

			#FLOAT00ANM02NODE
			$list_set = $onedata[ $GRADATA_ENUM_FLOAT00_ANM02N ];
			$$buff[ $GRA_ENUM_FLOAT00ANM02NODE ] = &get_node_h_nodeno( \@node_h, $list_set );

			#FLOAT00ANM03NODE
			$list_set = $onedata[ $GRADATA_ENUM_FLOAT00_ANM03N ];
			$$buff[ $GRA_ENUM_FLOAT00ANM03NODE ] = &get_node_h_nodeno( \@node_h, $list_set );
		}
		#FLOAT01
		{
			#�t�@�C��������m�[�h�w�b�_��ǂݍ���
			$list_set = $onedata[ $GRADATA_ENUM_FLOAT01_00 ];
			$list_set =~ s/imd/h/g;
			open( FILEIN, "$list_set" );
			@node_h = <FILEIN>;
			close( FILEIN );

			#NODEMAX
			$$buff[ $GRA_ENUM_FLOAT01NODENUM ] = &get_node_h_max( \@node_h );

			#FLOAT01ANM01NODE
			$list_set = $onedata[ $GRADATA_ENUM_FLOAT01_ANM01N ];
			$$buff[ $GRA_ENUM_FLOAT01ANM01NODE ] = &get_node_h_nodeno( \@node_h, $list_set );

			#FLOAT01ANM02NODE
			$list_set = $onedata[ $GRADATA_ENUM_FLOAT01_ANM02N ];
			$$buff[ $GRA_ENUM_FLOAT01ANM02NODE ] = &get_node_h_nodeno( \@node_h, $list_set );

			#FLOAT01ANM03NODE
			$list_set = $onedata[ $GRADATA_ENUM_FLOAT01_ANM03N ];
			$$buff[ $GRA_ENUM_FLOAT01ANM03NODE ] = &get_node_h_nodeno( \@node_h, $list_set );
		}
	}
	
	#�}�b�v
	{
		#MAP
		$list_set = $onedata[ $GRADATA_ENUM_MAP ];
		&gra_conv_list_add( $list_set );		#�R���o�[�g���X�g�ɐݒ�
		$list_set =~ s/imd/nsbmd/;
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_MAP ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_MAP ] =~ s/\./_/;

		#MAT
		$list_set = $onedata[ $GRADATA_ENUM_MAT ];
		&gra_conv_list_add( $list_set );		#�R���o�[�g���X�g�ɐݒ�
		$list_set =~ s/imd/nsbmd/;
		&gra_list_add( $list_set );
		$$buff[ $GRA_ENUM_MAT ] = "$GRA_NARC_TOP".$list_set;
		$$buff[ $GRA_ENUM_MAT ] =~ s/\./_/;

		#MAPANM00
		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_MAPANM00 ], $buff, $GRA_ENUM_MAPANM00, $$buff[ $GRA_ENUM_MAP ] );

		#MAPANM01
		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_MAPANM01 ], $buff, $GRA_ENUM_MAPANM01, $$buff[ $GRA_ENUM_MAP ] );

		#MAPANM02
		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_MAPANM02 ], $buff, $GRA_ENUM_MAPANM02, $$buff[ $GRA_ENUM_MAP ] );

		#MAPANM03
		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_MAPANM03 ], $buff, $GRA_ENUM_MAPANM03, $$buff[ $GRA_ENUM_MAP ] );

		#MAPANMPOLL
		&set3dmapobj_anm( $onedata[ $GRADATA_ENUM_MAPPOLL ], $buff, $GRA_ENUM_MAPANM04, $$buff[ $GRA_ENUM_MAP ] );


		#MAP�t�@�C��������m�[�h�w�b�_��ǂݍ���
		$list_set = $onedata[ $GRADATA_ENUM_MAP ];
		$list_set =~ s/imd/h/g;
		open( FILEIN, "$list_set" );
		@node_h = <FILEIN>;
		close( FILEIN );


		#NODEMAX
		$$buff[ $GRA_ENUM_NODENUM ] = &get_node_h_max( \@node_h );

		#MAPANM00NODE
		$list_set = $onedata[ $GRADATA_ENUM_MAPANM00NODE ];
		if( $list_set eq "DUMMY" ){	#DUMMY�������Ă���Ƃ���͎g���Ă��Ȃ��Ƃ���Ȃ̂łǂ�Ȑ����ł����v
			$$buff[ $GRA_ENUM_MAPANM00NODE ] = 0;
		}else{
			$$buff[ $GRA_ENUM_MAPANM00NODE ] = &get_node_h_nodeno( \@node_h, $list_set );
		}

		#MAPANM01NODE
		$list_set = $onedata[ $GRADATA_ENUM_MAPANM01NODE ];
		if( $list_set eq "DUMMY" ){	#DUMMY�������Ă���Ƃ���͎g���Ă��Ȃ��Ƃ���Ȃ̂łǂ�Ȑ����ł����v
			$$buff[ $GRA_ENUM_MAPANM01NODE ] = 0;
		}else{
			$$buff[ $GRA_ENUM_MAPANM01NODE ] = &get_node_h_nodeno( \@node_h, $list_set );
		}

		#MAPANM02NODE
		$list_set = $onedata[ $GRADATA_ENUM_MAPANM02NODE ];
		if( $list_set eq "DUMMY" ){	#DUMMY�������Ă���Ƃ���͎g���Ă��Ȃ��Ƃ���Ȃ̂łǂ�Ȑ����ł����v
			$$buff[ $GRA_ENUM_MAPANM02NODE ] = 0;
		}else{
			$$buff[ $GRA_ENUM_MAPANM02NODE ] = &get_node_h_nodeno( \@node_h, $list_set );
		}

		#MAPANM03NODE
		$list_set = $onedata[ $GRADATA_ENUM_MAPANM03NODE ];
		if( $list_set eq "DUMMY" ){	#DUMMY�������Ă���Ƃ���͎g���Ă��Ȃ��Ƃ���Ȃ̂łǂ�Ȑ����ł����v
			$$buff[ $GRA_ENUM_MAPANM03NODE ] = 0;
		}else{
			$$buff[ $GRA_ENUM_MAPANM03NODE ] = &get_node_h_nodeno( \@node_h, $list_set );
		}

		#MAPANM04NODE
		$list_set = $onedata[ $GRADATA_ENUM_MAPPOLLNODE ];
		if( $list_set eq "DUMMY" ){	#DUMMY�������Ă���Ƃ���͎g���Ă��Ȃ��Ƃ���Ȃ̂łǂ�Ȑ����ł����v
			$$buff[ $GRA_ENUM_MAPANM04NODE ] = 0;
		}else{
			$$buff[ $GRA_ENUM_MAPANM04NODE ] = &get_node_h_nodeno( \@node_h, $list_set );
		}
	}
}


#�}�b�v�z�u�I�u�W�F�A�j���f�[�^�ݒ�֐�
#�}�b�v�I�u�W�F�A�j���f�[�^�ݒ�֐�
sub set3dmapobj_anm{
	my( $list_set, $buff, $buffidx, $mdlfile ) = @_;

	#���f���t�@�C������Ȃ����`�F�b�N
	if( $list_set =~ /imd/ ){

		print( "not anm $list_set\n" );
		$$buff[ $buffidx ] = $mdlfile;
	}else{

		if( $list_set eq "DUMMY" ){
			$$buff[ $buffidx ] = $mdlfile;
		}else{
			&gra_conv_list_add( $list_set );		#�R���o�[�g���X�g�ɐݒ�
			$list_set = &anm_file_change( $list_set );	#�A�j���t�@�C������ύX
			&gra_list_add( $list_set );
			$$buff[ $buffidx ] = "$GRA_NARC_TOP".$list_set;
			$$buff[ $buffidx ] =~ s/\./_/;
		}
	}
}


#node_h����m�[�h�ő吔���擾
sub get_node_h_max{
	my( $file ) = @_;
	
	foreach $data ( @$file ){
		if( $data =~ /NODEIDXMAX[\t]*\(([0-9]*)\)/ ){
			return $1;
		}
	}
	print( "NODEINXMAX none\n" );
	exit(1);
}

#node_h����m�[�h���̃m�[�hNO���擾
sub get_node_h_nodeno{
	my( $file, $name ) = @_;

	#�T�[�`����O�ɂ��̖��O��DUMMY�̂Ƃ��̓_�~�[�̒l��Ԃ�
	if( $name eq "DUMMY" ){
		return $NODE_DUMMY;
	}
	
	foreach $data ( @$file ){
		if( $data =~ /$name[\t]*\(([0-9]*)\)/ ){
			return $1;
		}
	}

	print( "NODENO none $name\n" );
	exit(1);
}


#�A�j���t�@�C�����`�F���W�֐�
sub anm_file_change{
	my( $filename ) = @_;

	$filename =~ s/imd/nsbmd/;
	$filename =~ s/ica/nsbca/;
	$filename =~ s/ima/nsbma/;
	$filename =~ s/itp/nsbtp/;
	$filename =~ s/ita/nsbta/;
	$filename =~ s/iva/nsbva/;
	
	return $filename;
}


#�O���t�B�b�N�A�[�J�C�u���X�g�ɒǉ�
sub gra_list_add{
	my( $str ) = @_;
	my( $i );

	
	#�T�[�`����
	for( $i=0; $i<@GRA_LIST; $i++ ){
		if( $str eq $GRA_LIST[ $i ] ){
			#�d������t�@�C��������̂Œǉ����Ȃ�
			return;
		}else{
		}
	}
	$GRA_LIST[ $i ] = $str;
}

#�R���o�[�g���X�g�ɒǉ�
sub gra_conv_list_add{
	my( $str ) = @_;
	my( $i );

	#�T�[�`����
	for( $i=0; $i<@GRA_LIST; $i++ ){
		if( $str eq $GRA_LIST[ $i ] ){
			#�d������t�@�C��������̂Œǉ����Ȃ�
			return;
		}else{
		}
	}
	$GRA_CONVLIST[ $i ] = $str;
}



#�O���t�B�b�N�����f�[�^�����o��
sub fileout_gradata()
{
	my( $i );
	
	#�A�[�J�C�u���X�g
	open( FILEOUT, ">$GRADATA_ARCLIST_FILE");
	for( $i=1; $i<@GRA_LIST; $i++ ){	#�Ȃ���idx0�ɋ󔒂�����̂�1�炩�o��
		print( FILEOUT "\"".$GRA_LIST[ $i ]."\"\n" );
	}
	close( FILEOUT );

	#�R���o�[�g���X�g
	open( FILEOUT, ">$GRADATA_CONVLIST_FILE" );
	for( $i=1; $i<@GRA_CONVLIST; $i++ ){	#�Ȃ���idx0�ɋ󔒂�����̂�1�炩�o��
		print( FILEOUT "g3dcvtr ".$GRA_CONVLIST[ $i ]."\n" );
	}
	close( FILEOUT );


	#�����ʃO���t�B�b�N�f�[�^
	open( FILEOUT, ">$GRADATA_OUTGRADATAFILE" );
	print( FILEOUT "// ���̃t�@�C����pl_conv/wifi_lobby/wflby_gradta_conv.pl�Ŏ�����������܂�\n\n\n\n\n\n" );
	print( FILEOUT "static const u32 sc_data[ WFLBY_SEASON_NUM * WFLBY_ROOM_TYPE  ] = {\n" );

	print( FILEOUT "// �m�[�}���@�΂̕���\n" );
	for( $i=0; $i<@GRA_NORMAL_FIRE; $i++ ){
		print( FILEOUT "\t".$GRA_NORMAL_FIRE[ $i ].",\n" );
	}
	print( FILEOUT "// �m�[�}���@	���̕���\n" );
	for( $i=0; $i<@GRA_NORMAL_WATER; $i++ ){
		print( FILEOUT "\t".$GRA_NORMAL_WATER[ $i ].",\n" );
	}
	print( FILEOUT "// �m�[�}���@	�d�C�̕���\n" );
	for( $i=0; $i<@GRA_NORMAL_SPARK; $i++ ){
		print( FILEOUT "\t".$GRA_NORMAL_SPARK[ $i ].",\n" );
	}
	print( FILEOUT "// �m�[�}���@	���̕���\n" );
	for( $i=0; $i<@GRA_NORMAL_GRASS; $i++ ){
		print( FILEOUT "\t".$GRA_NORMAL_GRASS[ $i ].",\n" );
	}
	print( FILEOUT "// �m�[�}���@	����̕���\n" );
	for( $i=0; $i<@GRA_NORMAL_SPECIAL; $i++ ){
		print( FILEOUT "\t".$GRA_NORMAL_SPECIAL[ $i ].",\n" );
	}


	print( FILEOUT "// �t�@�΂̕���\n" );
	for( $i=0; $i<@GRA_SPRING_FIRE; $i++ ){
		print( FILEOUT "\t".$GRA_SPRING_FIRE[ $i ].",\n" );
	}
	print( FILEOUT "// �t�@	���̕���\n" );
	for( $i=0; $i<@GRA_SPRING_WATER; $i++ ){
		print( FILEOUT "\t".$GRA_SPRING_WATER[ $i ].",\n" );
	}
	print( FILEOUT "// �t�@	�d�C�̕���\n" );
	for( $i=0; $i<@GRA_SPRING_SPARK; $i++ ){
		print( FILEOUT "\t".$GRA_SPRING_SPARK[ $i ].",\n" );
	}
	print( FILEOUT "// �t�@	���̕���\n" );
	for( $i=0; $i<@GRA_SPRING_GRASS; $i++ ){
		print( FILEOUT "\t".$GRA_SPRING_GRASS[ $i ].",\n" );
	}
	print( FILEOUT "// �t�@	����̕���\n" );
	for( $i=0; $i<@GRA_SPRING_SPECIAL; $i++ ){
		print( FILEOUT "\t".$GRA_SPRING_SPECIAL[ $i ].",\n" );
	}

	print( FILEOUT "// �ā@�΂̕���\n" );
	for( $i=0; $i<@GRA_SUMMER_FIRE; $i++ ){
		print( FILEOUT "\t".$GRA_SUMMER_FIRE[ $i ].",\n" );
	}
	print( FILEOUT "// �ā@	���̕���\n" );
	for( $i=0; $i<@GRA_SUMMER_WATER; $i++ ){
		print( FILEOUT "\t".$GRA_SUMMER_WATER[ $i ].",\n" );
	}
	print( FILEOUT "// �ā@	�d�C�̕���\n" );
	for( $i=0; $i<@GRA_SUMMER_SPARK; $i++ ){
		print( FILEOUT "\t".$GRA_SUMMER_SPARK[ $i ].",\n" );
	}
	print( FILEOUT "// �ā@	���̕���\n" );
	for( $i=0; $i<@GRA_SUMMER_GRASS; $i++ ){
		print( FILEOUT "\t".$GRA_SUMMER_GRASS[ $i ].",\n" );
	}
	print( FILEOUT "// �ā@	����̕���\n" );
	for( $i=0; $i<@GRA_SUMMER_SPECIAL; $i++ ){
		print( FILEOUT "\t".$GRA_SUMMER_SPECIAL[ $i ].",\n" );
	}

	print( FILEOUT "// �H�@�΂̕���\n" );
	for( $i=0; $i<@GRA_FALL_FIRE; $i++ ){
		print( FILEOUT "\t".$GRA_FALL_FIRE[ $i ].",\n" );
	}
	print( FILEOUT "// �H�@	���̕���\n" );
	for( $i=0; $i<@GRA_FALL_WATER; $i++ ){
		print( FILEOUT "\t".$GRA_FALL_WATER[ $i ].",\n" );
	}
	print( FILEOUT "// �H�@	�d�C�̕���\n" );
	for( $i=0; $i<@GRA_FALL_SPARK; $i++ ){
		print( FILEOUT "\t".$GRA_FALL_SPARK[ $i ].",\n" );
	}
	print( FILEOUT "// �H�@	���̕���\n" );
	for( $i=0; $i<@GRA_FALL_GRASS; $i++ ){
		print( FILEOUT "\t".$GRA_FALL_GRASS[ $i ].",\n" );
	}
	print( FILEOUT "// �H�@	����̕���\n" );
	for( $i=0; $i<@GRA_FALL_SPECIAL; $i++ ){
		print( FILEOUT "\t".$GRA_FALL_SPECIAL[ $i ].",\n" );
	}

	print( FILEOUT "// �~�@�΂̕���\n" );
	for( $i=0; $i<@GRA_WINTER_FIRE; $i++ ){
		print( FILEOUT "\t".$GRA_WINTER_FIRE[ $i ].",\n" );
	}
	print( FILEOUT "// �~�@	���̕���\n" );
	for( $i=0; $i<@GRA_WINTER_WATER; $i++ ){
		print( FILEOUT "\t".$GRA_WINTER_WATER[ $i ].",\n" );
	}
	print( FILEOUT "// �~�@	�d�C�̕���\n" );
	for( $i=0; $i<@GRA_WINTER_SPARK; $i++ ){
		print( FILEOUT "\t".$GRA_WINTER_SPARK[ $i ].",\n" );
	}
	print( FILEOUT "// �~�@	���̕���\n" );
	for( $i=0; $i<@GRA_WINTER_GRASS; $i++ ){
		print( FILEOUT "\t".$GRA_WINTER_GRASS[ $i ].",\n" );
	}
	print( FILEOUT "// �~�@	����̕���\n" );
	for( $i=0; $i<@GRA_WINTER_SPECIAL; $i++ ){
		print( FILEOUT "\t".$GRA_WINTER_SPECIAL[ $i ].",\n" );
	}

	print( FILEOUT "};\n" );
	
	close( FILEOUT );
}
