#
#			�����|�P�����f�[�^�@�R���o�[�^�[
#			�f�[�^��13�Œ�
#

@FLD_TRADE_FILE = undef;	#�f�[�^�t�@��
@MONSNO_H_FILE	= undef;	#�����X�^�[�i���o�[�f�t�@�C��
@TOKUSYU_H_FILE	= undef;	#����i���o�[�f�t�@�C��
@SEIKAKU_H_FILE = undef;	#���i�i���o�[�f�t�@�C��
@ITEMSYM_H_FILE = undef;	#�A�C�e���i���o�[�f�t�@�C��
@PMVER_H_FILE = undef;		#���R�[�h�i���o�[�f�t�@�C��
@OUTPUT0_FILE = undef;		#�����o���f�[�^
@OUTPUT1_FILE = undef;		#�����o���f�[�^
@OUTPUT2_FILE = undef;		#�����o���f�[�^
@OUTPUT3_FILE = undef;		#�����o���f�[�^
@OUTPUT4_FILE = undef;		#�����o���f�[�^
@OUTPUT5_FILE = undef;		#�����o���f�[�^
@OUTPUT6_FILE = undef;		#�����o���f�[�^
@OUTPUT7_FILE = undef;		#�����o���f�[�^
@OUTPUT7_FILE = undef;		#�����o���f�[�^
@OUTPUT8_FILE = undef;		#�����o���f�[�^
@OUTPUT9_FILE = undef;		#�����o���f�[�^
@OUTPUT10_FILE = undef;		#�����o���f�[�^
@OUTPUT11_FILE = undef;		#�����o���f�[�^
@OUTPUT12_FILE = undef;		#�����o���f�[�^

@OUTPUT_DATA_ARRAY = undef;

$FILE_NUM	= 12;

# ��C���f�b�N�X
@ROW_INDEX = undef;

# �s�C���f�b�N�X
$LINE_MONSNO        = 1;   # �����X�^�[�i���o�[ 
$LINE_FORM          = 3;   # �t�H�[���i���o�[
$LINE_LEVEL         = 4;   # ���x��
$LINE_HP_RND        = 5;   # HP����
$LINE_AT_RND        = 6;   # �U������
$LINE_DF_RND        = 7;   # �h�䗐��
$LINE_AG_RND        = 8;   # ��������
$LINE_SA_RND        = 9;   # ���U����
$LINE_SD_RND        = 10;  # ���h����
$LINE_TOKUSEI       = 11;  # ����\��
$LINE_SEIKAKU       = 12;  # ���i
$LINE_SEX           = 13;  # ����
$LINE_ID            = 14;  # ID
$LINE_STYPE         = 15;  # �������悳
$LINE_BEAUTIFUL     = 16;  # ��������
$LINE_CUTE          = 17;  # ���킢��
$LINE_CLEVER        = 18;  # ��������
$LINE_STRONG        = 19;  # �����܂���
$LINE_ITEM          = 20;  # �A�C�e��
$LINE_OYA_SEX       = 22;  # �e����
$LINE_FUR           = 23;  # �щ�
$LINE_COUNTRY       = 24;  # �e�̍��R�[�h
$LINE_CHANGE_MONSNO = 25;  # �������郂���X�^�[�i���o�[
$LINE_CHANGE_SEX    = 26;  # ��������|�P�����̐���

# �s�C���f�b�N�X
@LINE_INDEX = ($LINE_MONSNO,
               $LINE_FORM,
               $LINE_LEVEL,
               $LINE_HP_RND,
               $LINE_AT_RND,
               $LINE_DF_RND,
               $LINE_AG_RND,
               $LINE_SA_RND,
               $LINE_SD_RND,
               $LINE_TOKUSEI,
               $LINE_SEIKAKU,
               $LINE_SEX,
               $LINE_ID,
               $LINE_STYPE,
               $LINE_BEAUTIFUL,
               $LINE_CUTE,
               $LINE_CLEVER,
               $LINE_STRONG,
               $LINE_ITEM,
               $LINE_OYA_SEX,
               $LINE_FUR,
               $LINE_COUNTRY,
               $LINE_CHANGE_MONSNO,
               $LINE_CHANGE_SEX);

# �o�̓f�[�^�C���f�b�N�X
# @DATA_INDEX[$LINE_MONSNO] = �o�̓f�[�^�z�����, �����X�^�[No.�̃C���f�b�N�X
@DATA_INDEX = undef;
for( $idx=0; $idx<=$#LINE_INDEX; $idx++ )
{
  @DATA_INDEX[@LINE_INDEX[$idx]] = $idx;
}
               

#############################################################
#
#	���C������
#
#############################################################

#-----------------
#�t�@�C���ǂݍ���
#-----------------
&file_open();

#---------------------------------
#�L���f�[�^�̗�C���f�b�N�X������
#---------------------------------
@line_data = split("\t", @FLD_TRADE_FILE[ $LINE_MONSNO ]);
foreach $data (@line_data)
{
  # �����X�^�[No.�̎w��𔭌�������L���f�[�^�s�Ƃ���
  if($data =~ "MONSNO_")
  {
    push(@ROW_INDEX, $row_idx);
  }
  $row_idx++;
}
splice(@ROW_INDEX, 0, 1);  # �ŏ���1��(�S�~)����菜��

#--------------------
# �e�f�[�^�̎��o��
#--------------------

foreach $line (@FLD_TRADE_FILE)
{
  my @line_data = split("\t", $line);
  my @array = undef;

  foreach $data (@line_data)
  {
    push(@array, $data);
  }
  push(@OUTPUT_DATA_ARRAY, [@array]);
}

$DATA_NUM = 0;
# �e�����f�[�^���擾
foreach $row_idx (@ROW_INDEX)
{ 
  print "row = $row_idx\n";
  $data_idx = 0;
  @OUTPUT_DATA = undef;
  # �e��f�[�^���擾
  foreach $line_idx ( @LINE_INDEX ){ 
    $line = @FLD_TRADE_FILE[$line_idx];
    @line_data = split("\t", $line);
    @OUTPUT_DATA[$data_idx] = @line_data[$row_idx];
    $data_idx++;
  }
  # �f�[�^��l�ɕϊ�
  @OUTPUT_DATA[@DATA_INDEX[$LINE_MONSNO]]  = &get_monsno(@OUTPUT_DATA[@DATA_INDEX[$LINE_MONSNO]]);
  @OUTPUT_DATA[@DATA_INDEX[$LINE_FORM]]    = &get_formno(@OUTPUT_DATA[@DATA_INDEX[$LINE_FORM]]);
  @OUTPUT_DATA[@DATA_INDEX[$LINE_TOKUSEI]] = &get_tokusyu(@OUTPUT_DATA[@DATA_INDEX[$LINE_TOKUSEI]]);
  @OUTPUT_DATA[@DATA_INDEX[$LINE_SEIKAKU]] = &get_seikaku(@OUTPUT_DATA[@DATA_INDEX[$LINE_SEIKAKU]]);
  @OUTPUT_DATA[@DATA_INDEX[$LINE_ITEM]]    = &get_item(@OUTPUT_DATA[@DATA_INDEX[$LINE_ITEM]]);
  @OUTPUT_DATA[@DATA_INDEX[$LINE_COUNTRY]] = &get_pmver(@OUTPUT_DATA[@DATA_INDEX[$LINE_COUNTRY]]);
  # �z��ɓo�^
  @OUTPUT_DATA_ARRAY[$DATA_NUM] = \@OUTPUT_DATA;
  $DATA_NUM++;
}

# �f�[�^�\��
foreach $output_data_ref (@OUTPUT_DATA_ARRAY){
}
for( $idx=0; $idx<=$#OUTPUT_DATA_ARRAY; $idx++ )
{
  @out_data = @OUTPUT_DATA_ARRAY[$idx];
  print @out_data->[0][0];
  print "-------------------------\n";
  foreach $data (@out_data)
  {
    print "$data\n";
  }
  print "-------------------------\n";
}


#----------
# �����o��
#----------
open( FILEOUT_0, ">fld_trade_00.dat" );
open( FILEOUT_0_T, ">fld_trade_00.txt" );
binmode( FILEOUT_0 );

open( FILEOUT_1, ">fld_trade_01.dat" );
open( FILEOUT_1_T, ">fld_trade_01.txt" );
binmode( FILEOUT_1 );

open( FILEOUT_2, ">fld_trade_02.dat" );
open( FILEOUT_2_T, ">fld_trade_02.txt" );
binmode( FILEOUT_2 );

open( FILEOUT_3, ">fld_trade_03.dat" );
open( FILEOUT_3_T, ">fld_trade_03.txt" );
binmode( FILEOUT_3 );

open( FILEOUT_4, ">fld_trade_04.dat" );
open( FILEOUT_4_T, ">fld_trade_04.txt" );
binmode( FILEOUT_4 );

open( FILEOUT_5, ">fld_trade_05.dat" );
open( FILEOUT_5_T, ">fld_trade_05.txt" );
binmode( FILEOUT_5 );

open( FILEOUT_6, ">fld_trade_06.dat" );
open( FILEOUT_6_T, ">fld_trade_06.txt" );
binmode( FILEOUT_6 );

open( FILEOUT_7, ">fld_trade_07.dat" );
open( FILEOUT_7_T, ">fld_trade_07.txt" );
binmode( FILEOUT_7 );

open( FILEOUT_8, ">fld_trade_08.dat" );
open( FILEOUT_8_T, ">fld_trade_08.txt" );
binmode( FILEOUT_8 );

open( FILEOUT_9, ">fld_trade_09.dat" );
open( FILEOUT_9_T, ">fld_trade_09.txt" );
binmode( FILEOUT_9 );

open( FILEOUT_10, ">fld_trade_10.dat" );
open( FILEOUT_10_T, ">fld_trade_10.txt" );
binmode( FILEOUT_10 );

open( FILEOUT_11, ">fld_trade_11.dat" );
open( FILEOUT_11_T, ">fld_trade_11.txt" );
binmode( FILEOUT_11 );

open( FILEOUT_12, ">fld_trade_12.dat" );
open( FILEOUT_12_T, ">fld_trade_12.txt" );
binmode( FILEOUT_12 );

for( $i=0; $i<24; $i++ ){
	print( FILEOUT_0 pack("I", $OUTPUT0_FILE[$i]) );
	print( FILEOUT_1 pack("I", $OUTPUT1_FILE[$i]) );
	print( FILEOUT_2 pack("I", $OUTPUT2_FILE[$i]) );
	print( FILEOUT_3 pack("I", $OUTPUT3_FILE[$i]) );
	print( FILEOUT_4 pack("I", $OUTPUT4_FILE[$i]) );
	print( FILEOUT_5 pack("I", $OUTPUT5_FILE[$i]) );
	print( FILEOUT_6 pack("I", $OUTPUT6_FILE[$i]) );
	print( FILEOUT_7 pack("I", $OUTPUT7_FILE[$i]) );
	print( FILEOUT_8 pack("I", $OUTPUT8_FILE[$i]) );
	print( FILEOUT_9 pack("I", $OUTPUT9_FILE[$i]) );
	print( FILEOUT_10 pack("I", $OUTPUT10_FILE[$i]) );
	print( FILEOUT_11 pack("I", $OUTPUT11_FILE[$i]) );
	print( FILEOUT_12 pack("I", $OUTPUT12_FILE[$i]) );

	print( FILEOUT_0_T $OUTPUT0_FILE[$i]."\r\n" );
	print( FILEOUT_1_T $OUTPUT1_FILE[$i]."\r\n" );
	print( FILEOUT_2_T $OUTPUT2_FILE[$i]."\r\n" );
	print( FILEOUT_3_T $OUTPUT3_FILE[$i]."\r\n" );
	print( FILEOUT_4_T $OUTPUT4_FILE[$i]."\r\n" );
	print( FILEOUT_5_T $OUTPUT5_FILE[$i]."\r\n" );
	print( FILEOUT_6_T $OUTPUT6_FILE[$i]."\r\n" );
	print( FILEOUT_7_T $OUTPUT7_FILE[$i]."\r\n" );
	print( FILEOUT_8_T $OUTPUT8_FILE[$i]."\r\n" );
	print( FILEOUT_9_T $OUTPUT9_FILE[$i]."\r\n" );
	print( FILEOUT_10_T $OUTPUT10_FILE[$i]."\r\n" );
	print( FILEOUT_11_T $OUTPUT11_FILE[$i]."\r\n" );
	print( FILEOUT_12_T $OUTPUT12_FILE[$i]."\r\n" );
}

close( FILEOUT_0 );
close( FILEOUT_1 );
close( FILEOUT_2 );
close( FILEOUT_3 );
close( FILEOUT_4 );
close( FILEOUT_5 );
close( FILEOUT_6 );
close( FILEOUT_7 );
close( FILEOUT_8 );
close( FILEOUT_9 );
close( FILEOUT_10 );
close( FILEOUT_11 );
close( FILEOUT_12 );

close( FILEOUT_0_T );
close( FILEOUT_1_T );
close( FILEOUT_2_T );
close( FILEOUT_3_T );
close( FILEOUT_4_T );
close( FILEOUT_5_T );
close( FILEOUT_6_T );
close( FILEOUT_7_T );
close( FILEOUT_8_T );
close( FILEOUT_9_T );
close( FILEOUT_10_T );
close( FILEOUT_11_T );
close( FILEOUT_12_T );

exit(0);


#===================================================================================
# ���T�u���[�`��
#===================================================================================

sub file_open{
	system( "../../tools/exceltool/ExcelSeetConv -s tab fld_trade_poke.xls" );
	open( FILEIN, "fld_trade_poke.txt" );
	@FLD_TRADE_FILE = <FILEIN>;
	close( FILEIN );

	open( FILEIN, "monsno_def.h" );
	@MONSNO_H_FILE = <FILEIN>;
	close( FILEIN );

	open( FILEIN, "tokusyu_def.h" );
	@TOKUSYU_H_FILE = <FILEIN>;
	close( FILEIN );

	open( FILEIN, "poke_tool.h" );
	@SEIKAKU_H_FILE = <FILEIN>;
	close( FILEIN );

	open( FILEIN, "itemsym.h" );
	@ITEMSYM_H_FILE = <FILEIN>;
	close( FILEIN );

	open( FILEIN, "pm_version.h" );
	@PMVER_H_FILE = <FILEIN>;
	close( FILEIN );
}
sub get_monsno{
	my( $key )  = @_;
	
  print "$key = ";
	foreach $one ( @MONSNO_H_FILE ){

		if( $one =~ /#define\t*$key\t*\( ([0-9]*) \)/ ){
      print "$1\n";
			return $1;	#�q�b�g�����i���o�[��Ԃ�
		}
	}
  print "�~\n";
	return 0;
}
sub get_formno{
	my( $key )  = @_;
	
  print "$key = ";
	foreach $one ( @MONSNO_H_FILE ){

		if( $one =~ /#define\t*$key\t*\( ([0-9]*) \)/ ){
      print "$1\n";
			return $1;	#�q�b�g�����i���o�[��Ԃ�
		}
	}
  print "�~\n";
	return 0;
}
sub get_tokusyu{
	my( $key )  = @_;
	
  print "$key = ";
	foreach $one ( @TOKUSYU_H_FILE ){

		if( $one =~ /#define\t*$key\t*\(([0-9]*)\)/ ){
      print "$1\n";
			return $1;	#�q�b�g�����i���o�[��Ԃ�
		}
	}
  print "�~\n";
	return 0; 
}
sub get_seikaku{
	my( $key )  = @_;
	
  print "$key = ";
	foreach $one ( @SEIKAKU_H_FILE ){

		if( $one =~ /#define\s*$key\s*\( ([0-9]*) \)/ ){
      print "$1\n";
			return $1;	#�q�b�g�����i���o�[��Ԃ�
		}
	}
  print "�~\n";
	return 0; 
}
sub get_item{
	my( $key )  = @_;
	
  print "$key = ";
	foreach $one ( @ITEMSYM_H_FILE ){

		if( $one =~ /#define $key\t*\( ([0-9]*) \)/ ){
      print "$1\n";
			return $1;	#�q�b�g�����i���o�[��Ԃ�
		}
	}
  print "�~\n";
	return 0;
}
sub get_pmver{
	my( $key )  = @_;
	
  print "$key = ";
	foreach $one ( @PMVER_H_FILE ){

		if( $one =~ /#define\t*$key\t*([0-9]*)/ ){
      print "$1\n";
			return $1;	#�q�b�g�����i���o�[��Ԃ�
		}
	}
  print "�~\n";
	return 0;
}
