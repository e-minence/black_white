################################################
#
#   �Ӓ�m�f�[�^�R���o�[�^�[
#
#     conv.pl out.tab item_header
#
#
#
#
################################################

if( @ARGV < 2 ){
  print( "perl conv.pl out.tab item_header\n" ); 
  exit(1);
}


#�t�@�C���ǂݍ���
open( FILEIN, $ARGV[0] );
@EXCEL_FILE = <FILEIN>;
close( FILEIN );

open( FILEIN, $ARGV[1] );
@ITEM_HEADER = <FILEIN>;
close( FILEIN );


#�ǂݍ��ݏ��
@ITEM_LIST = undef;
@GOURMET_LIST = undef;        #�O�����@�����f�[�^
@STONE_MANIA_LIST = undef;    #�΃}�j�A�����f�[�^
@RICH_LIST = undef;           #��x���@�����f�[�^
@KOUKO_LIST = undef;          #�l�Êw�ҁ@�����f�[�^

$DATA_NUM = 0;



$data_in = 0;

foreach $one ( @EXCEL_FILE )
{
  $one =~ s/\r\n/\t/g;
  $one =~ s/\n/\t/g;


  @line = split( /\t/, $one );

  if( $data_in == 0 )
  {
    if( "".$line[1] eq "���" ){
      $data_in = 1;
    }
  }
  elsif( $data_in == 1 )
  {
    if( "".$line[1] eq "#END" ){
      $data_in = 0;
    }else{

      $ITEM_LIST[ $DATA_NUM ] = $line[1];
      $GOURMET_LIST[ $DATA_NUM ] = $line[2];
      $STONE_MANIA_LIST[ $DATA_NUM ] = $line[3];
      $RICH_LIST[ $DATA_NUM ] = $line[4];
      $KOUKO_LIST[ $DATA_NUM ] = $line[5];
      $DATA_NUM ++;
    }
  }
}

#�A�C�e���𐔒l�ɕύX
for( $i=0; $i<$DATA_NUM; $i++ ){
  $ITEM_LIST[ $i ] = &GET_ItemNo( $ITEM_LIST[ $i ] );
}



#�e�l�̃A�C�e���Ƌ��z���o��
open( FILEOUT, ">gourmet.dat" );
binmode( FILEOUT );
&DEBUG_Puts("gourmet\n");
for( $i=0; $i<$DATA_NUM; $i++ ){
  if( $GOURMET_LIST[ $i ] > 0 ){
    &DEBUG_Puts($ITEM_LIST[ $i ]."\n");
    &DEBUG_Puts($GOURMET_LIST[ $i ]."\n");
    print( FILEOUT pack( "I", $ITEM_LIST[ $i ] ) );
    print( FILEOUT pack( "I", $GOURMET_LIST[ $i ] ) );
  }
}
close( FILEOUT );


open( FILEOUT, ">stone_mania.dat" );
binmode( FILEOUT );
for( $i=0; $i<$DATA_NUM; $i++ ){
  if( $STONE_MANIA_LIST[ $i ] > 0 ){
    &DEBUG_Puts($ITEM_LIST[ $i ]."\n");
    &DEBUG_Puts($STONE_MANIA_LIST[ $i ]."\n");
    print( FILEOUT pack( "I", $ITEM_LIST[ $i ] ) );
    print( FILEOUT pack( "I", $STONE_MANIA_LIST[ $i ] ) );
  }
}
close( FILEOUT );

open( FILEOUT, ">rich.dat" );
binmode( FILEOUT );
for( $i=0; $i<$DATA_NUM; $i++ ){
  if( $RICH_LIST[ $i ] > 0 ){
    &DEBUG_Puts($ITEM_LIST[ $i ]."\n");
    &DEBUG_Puts($RICH_LIST[ $i ]."\n");
    print( FILEOUT pack( "I", $ITEM_LIST[ $i ] ) );
    print( FILEOUT pack( "I", $RICH_LIST[ $i ] ) );
  }
}
close( FILEOUT );

open( FILEOUT, ">kouko.dat" );
binmode( FILEOUT );
for( $i=0; $i<$DATA_NUM; $i++ ){
  if( $KOUKO_LIST[ $i ] > 0 ){
    &DEBUG_Puts($ITEM_LIST[ $i ]."\n");
    &DEBUG_Puts($KOUKO_LIST[ $i ]."\n");
    print( FILEOUT pack( "I", $ITEM_LIST[ $i ] ) );
    print( FILEOUT pack( "I", $KOUKO_LIST[ $i ] ) );
  }
}
close( FILEOUT );


exit(0);


sub GET_ItemNo
{
  my( $item ) = @_;
  my( $item_def, $item_count, $item_no );
  

  $item =~ s/\s//g;

  for( $item_count=0; $item_count<@ITEM_HEADER; $item_count++ ){
    #//comment�@�����𒊏o
    $item_def = $ITEM_HEADER[$item_count];

    if( $item_def =~ /\( ([0-9]*) \)\t\t\/\/(.*)$/ ){

      $item_no = $1;
      $item_def = $2;
      $item_def =~ s/\s//g;
      &DEBUG_Puts( "$item_def == $item  $item_no\n" );
      if( "".$item_def eq "".$item ){
        &DEBUG_Puts( "return \n" );
        return $item_no;
      }
    }
  }

  print( "$item ���݂���Ȃ�\n" );
  exit(1);
}

sub DEBUG_Puts
{
  my( $str ) = @_;
  #print( $str );
}
