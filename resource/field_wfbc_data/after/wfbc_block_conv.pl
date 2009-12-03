#############################################
#
# WFBC  �z�u�u���b�N��񐶐�
#
# 
#   wfbc_block_conv.pl tab_file land_data_patch output
#
#
#
#
#############################################


#���������`�F�b�N
if( @ARGV < 3 )
{
  print( "wfbc_block_conv.pl tab_file land_data_patch output\n" );
  exit(1);
}

#�u���b�N���Ȃ��ꏊ�Ɋi�[����f�[�^
$BLOCK_NONE_NUM = 0xff;

#
# �u���b�N�����
#
$BLOCK_DEF_X = 8; #�u���b�N���̐�
$BLOCK_DEF_Z = 8; #�u���b�N�c�̐�

$BLOCK_TAG_PARA_TAG = 0;
$BLOCK_TAG_PARA_NUM00 = 1;
$BLOCK_TAG_PARA_NUM01 = 2;
$BLOCK_TAG_PARA_NUM02 = 3;
$BLOCK_TAG_PARA_NUM03 = 4;
$BLOCK_TAG_PARA_NUM04 = 5;
$BLOCK_TAG_PARA_NUM05 = 6;
$BLOCK_TAG_PARA_NUM06 = 7;
$BLOCK_TAG_PARA_NUM07 = 8;
$BLOCK_TAG_PARA_NUM08 = 9;
$BLOCK_TAG_PARA_NUM09 = 10;
$BLOCK_TAG_PARA_NUM10 = 11;
$BLOCK_TAG_PARA_NUM11 = 12;
$BLOCK_TAG_PARA_NUM12 = 13;
$BLOCK_TAG_PARA_NUM13 = 14;
$BLOCK_TAG_PARA_NUM14 = 15;
$BLOCK_TAG_PARA_NUM15 = 16;
$BLOCK_TAG_PARA_NUM16 = 17;
$BLOCK_TAG_PARA_NUM17 = 18;
$BLOCK_TAG_PARA_NUM18 = 19;
$BLOCK_TAG_PARA_NUM19 = 20;
$BLOCK_TAG_PARA_NUM20 = 21;
$BLOCK_TAG_PARA_MAX = 22;


#�t�@�C���ǂݍ���
open( FILEIN, $ARGV[0] );
@EXCEL_DATA = <FILEIN>;
close( FILEIN );



#land_data_patch�ǂݍ���
open( FILEIN, $ARGV[1] );
@LAND_DATA_PATCH = <FILEIN>;
close( FILEIN );



#�u���b�N����ǂݍ��ށI
@BLOCK_DATA = undef;
$BLOCK_DATA_INDEX = 0;

@BLOCK_TAG = undef;
$BLOCK_TAG_INDEX = 0;

$data_in = 0;
$data_in_tag = 0;
foreach $one ( @EXCEL_DATA )
{
  $one =~ s/\r\n//g;
  $one =~ s/\n//g;


  @line = split( /\t/, $one );

  #�z�u���̎擾
  if( $data_in == 0 )
  {
    if( "".$line[0] eq "#START" )
    {
      $data_in = 1;
    }
  }
  else
  {
    if( "".$line[0] eq "#END" )
    {
      $data_in = 0;
    }
    else
    {
      for( $i=0; $i<$BLOCK_DEF_X; $i++ )
      {
        $BLOCK_DATA[ $BLOCK_DATA_INDEX ] = $line[1 + $i];
        $BLOCK_DATA_INDEX ++;
      }
    }
  }


  #�u���b�N�^�O���
  if( $data_in_tag == 0 )
  {
    if( "".$line[0] eq "TAG" )
    {
      $data_in_tag = 1;
    }
  }
  else
  {
    if( "".$line[0] eq "#END" )
    {
      $data_in_tag = 0;
    }
    else
    {
      for( $i=0; $i<$BLOCK_TAG_PARA_MAX; $i++ )
      {
        $BLOCK_TAG[ ($BLOCK_TAG_PARA_MAX * $BLOCK_TAG_INDEX) + $i ] = $line[$i];
      }
      $BLOCK_TAG_INDEX ++;
    }
  }
}


#���̏o��
open( FILEOUT, ">".$ARGV[2] );
binmode( FILEOUT );

#�z�u�����o��
for( $i=0; $i<$BLOCK_DATA_INDEX; $i++ )
{
  #print( "block idx $i = ".$BLOCK_DATA[ $i ]."\n" );
  print( FILEOUT pack( "C", &getTagIndex( $BLOCK_DATA[ $i ] ) ) ); 
}

#�^�O�f�[�^���o��
for( $i=0; $i<$BLOCK_TAG_INDEX; $i++ )
{
  for( $j=1; $j<$BLOCK_TAG_PARA_MAX; $j++ )
  {
    #print( "block idx $i $j = ".$BLOCK_TAG[ ($i*$BLOCK_TAG_PARA_MAX) + $j ]."\n" );
    print( FILEOUT pack( "I", &getBlockIndex( $BLOCK_TAG[ ($i*$BLOCK_TAG_PARA_MAX) + $j ] ) ) ); 
  }
}


close( FILEOUT );

exit(0);





#�^�O�C���f�b�N�X�̎擾
sub getTagIndex
{
  my( $tag ) = @_;
  my( $one, $index, $i );

  for( $i=0; $i<$BLOCK_TAG_INDEX; $i++ )
  {
    if( "".$BLOCK_TAG[($i * $BLOCK_TAG_PARA_MAX) + $BLOCK_TAG_PARA_TAG] eq "".$tag )
    {
      return $i;
    }
  }

  #�m��Ȃ��ꍇ�́ANONE��Ԃ�
  return $BLOCK_NONE_NUM;
}


#�u���b�N�C���f�b�N�X�̎擾
sub getBlockIndex
{
  my( $block_idx ) = @_;
  my( $one, $index, $i, @line );

  foreach $one ( @LAND_DATA_PATCH )
  {
    $one =~ s/\r\n//g;
    $one =~ s/\n//g;

    @line = split( /\s/, $one );

    if( "LAND_DATA_PATCH_".$block_idx  eq "".$line[1] )
    {
      return $line[ 3 ];
    }
  }

  print( "block�̒�`���݂�����܂��� $block_idx \n" );
  exit(1);
}



