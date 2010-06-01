###########################################################
#
#   �Đ�g���[�i�[���̃R���o�[�^�[
#
#   conv.pl excel.tab tr_no_header outdata_name objcode_header
#
#
#
#
###########################################################


if( @ARGV < 4 ){

  print( "conv.pl excel.tab tr_no_header outdata_name objcode_header\n" );
  exit(1);
}


#�t�@�C���ǂݍ���
open( FILEIN, $ARGV[0] );
@EXCEL_FILE = <FILEIN>;
close( FILEIN );

#�w�b�_�[�ǂݍ���
open( FILEIN, $ARGV[1] );
@HEADER_FILE = <FILEIN>;
close( FILEIN );

#�w�b�_�[�ǂݍ���
open( FILEIN, $ARGV[3] );
@OBJCODE_HEADER_FILE = <FILEIN>;
close( FILEIN );




#��`
$IDX_LABEL = 0;
$IDX_GADGE = 3;
$IDX_TRID  = 6;
$IDX_OBJCODE  = 8;
$IDX_RANDOM  = 2;


#�������낦��
@LABEL = undef;
@BADGE_NUM = undef;
@TR_ID = undef;
@OBJCODE = undef;

@LABEL_TBL = undef;

$DATA_NUM = 0;

$RANDOM_START_IDX = 0;
$random_start_idx_in = 0;


$data_in = 0;

foreach $one ( @EXCEL_FILE )
{
  $one =~ s/\r\n/\t/g;
  $one =~ s/\n/\t/g;


  @line = split( /\t/, $one );

  if( $data_in == 0 )
  {
    if( "".$line[$IDX_LABEL] eq "LABEL" ){
      $data_in = 1;
    }
  }
  elsif( $data_in == 1 )
  {
    if( "".$line[$IDX_LABEL] eq "#END" ){
      $data_in = 0;
    }else{

      $LABEL[ $DATA_NUM ] = $line[$IDX_LABEL];
      $BADGE_NUM[ $DATA_NUM ] = $line[$IDX_GADGE];
      $TR_ID[ $DATA_NUM ] = $line[$IDX_TRID];
      $OBJCODE[ $DATA_NUM ] = $line[$IDX_OBJCODE];

      if( $random_start_idx_in == 0 ){
        if( "".$line[ $IDX_RANDOM ] eq "�����_��" ){
          $RANDOM_START_IDX = $DATA_NUM;
          $random_start_idx_in = 1;
        }
      }

      $DATA_NUM ++;
    }
  }
}

#�g���[�i�[ID�𐔒l��
for( $i=0; $i<$DATA_NUM; $i++ ){
  $TR_ID[ $i ] = &GET_TrID( $TR_ID[ $i ] );
}

#OBJCODE�𐔒l��
for( $i=0; $i<$DATA_NUM; $i++ ){
  $OBJCODE[ $i ] = &GET_ObjCodeID( $OBJCODE[ $i ] );
}

#���x���̏d�����J�b�g
$label_num = 0;
for( $i=0; $i<$DATA_NUM; $i++ ){
  
  $data_input = 1;
  for( $j=0; $j<$label_num; $j++ ){
    if( "".$LABEL_TBL[$j] eq "".$LABEL[$i] ){
      #�i�[���Ȃ�
      $data_input = 0;
    }
  }

  if( $data_input ){
    $LABEL_TBL[$label_num] = $LABEL[$i];
    $label_num ++;
  }
}

#�d�����J�b�g�����A�����_���J�n�C���f�b�N�X�擾
$RANDOM_START_IDX = &GET_LabelID( $LABEL[$RANDOM_START_IDX] );


#�w�b�_�[�̎擾
open( FILEOUT, ">".$ARGV[2].".h" );

print( FILEOUT "#pragma once\n" );
print( FILEOUT "// ���̃w�b�_�[�́Aresource/rebattle_trainer/conv.pl�ŏo�͂���Ă��܂��B\n" );

for( $i=0; $i<@LABEL_TBL; $i++ ){
  print( FILEOUT "#define ".$LABEL_TBL[$i]." ($i)\n" );
}
print( FILEOUT "#define RB_MAX ($i)\n" );
print( FILEOUT "#define RB_DATA_TBL_MAX ($DATA_NUM)\n" );
print( FILEOUT "#define RB_RANDOM_START ($RANDOM_START_IDX)\n" );


close( FILEOUT );

#binary�f�[�^
open( FILEOUT, ">".$ARGV[2].".dat" );
binmode( FILEOUT );

for( $i=0; $i<$DATA_NUM; $i++ ){

  print( FILEOUT pack( "S", &GET_LabelID( $LABEL[$i] ) ) );
  print( FILEOUT pack( "S", $BADGE_NUM[$i] ) );
  print( FILEOUT pack( "S", $OBJCODE[$i] ) );
  print( FILEOUT pack( "S", $TR_ID[$i] ) );
}

close( FILEOUT );

exit(0);


sub GET_LabelID
{
  my( $label ) = @_;
  my( $count, $max );

  $max = @LABEL_TBL;
  for( $count=0; $count<$max; $count++ ){

    if( "".$label eq "".$LABEL_TBL[$count] ){
      return $count;
    }
  }

  print( "$label ���݂���܂���B\n" );
  exit(1);
}


sub GET_ObjCodeID
{
  my( $objcode ) = @_;
  my( $objcode_def, $objcode_count, $objcode_no );
  
  

  $objcode =~ s/\s//g;

  $objcode = uc($objcode);

  for( $objcode_count=0; $objcode_count<@OBJCODE_HEADER_FILE; $objcode_count++ ){
    #//comment�@�����𒊏o
    $objcode_def = $OBJCODE_HEADER_FILE[$objcode_count];

    if( $objcode_def =~ /#define\s([^\s]*)\s\([^\)]*\)\s\/\/([0-9]*)/ ){

      $objcode_no = $2;
      $objcode_def = $1;
      $objcode_def =~ s/\s//g;
      &DEBUG_Puts( "$objcode_def == $objcode  $objcode_no\n" );
      if( "".$objcode_def eq "".$objcode ){
        &DEBUG_Puts( "return \n" );
        return $objcode_no;
      }
    }
  }

  print( "$objcode ���݂���Ȃ�\n" );
  exit(1);
}

sub GET_TrID
{
  my( $trid ) = @_;
  my( $trid_def, $trid_count, $trid_no );
  
  

  $trid =~ s/\s//g;

  $trid = uc($trid);

  for( $trid_count=0; $trid_count<@HEADER_FILE; $trid_count++ ){
    #//comment�@�����𒊏o
    $trid_def = $HEADER_FILE[$trid_count];

    if( $trid_def =~ /#define\s([^\s]*)[\s]*\( ([0-9]*) \)/ ){

      $trid_no = $2;
      $trid_def = $1;
      $trid_def =~ s/\s//g;
      &DEBUG_Puts( "$trid_def == $trid  $trid_no\n" );
      if( "".$trid_def eq "TRID_".$trid ){
        &DEBUG_Puts( "return \n" );
        return $trid_no;
      }
    }
  }

  print( "$trid ���݂���Ȃ�\n" );
  exit(1);
}

sub DEBUG_Puts
{
  my( $str ) = @_;
  #print( $str );
}
