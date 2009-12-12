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
$BLOCK_TAG_PARA_SIZ_X = 1;
$BLOCK_TAG_PARA_SIZ_Z = 2;
$BLOCK_TAG_PARA_NUM00 = 3;
$BLOCK_TAG_PARA_NUM01 = 4;
$BLOCK_TAG_PARA_NUM02 = 5;
$BLOCK_TAG_PARA_NUM03 = 6;
$BLOCK_TAG_PARA_NUM04 = 7;
$BLOCK_TAG_PARA_NUM05 = 8;
$BLOCK_TAG_PARA_NUM06 = 9;
$BLOCK_TAG_PARA_NUM07 = 10;
$BLOCK_TAG_PARA_NUM08 = 11;
$BLOCK_TAG_PARA_NUM09 = 12;
$BLOCK_TAG_PARA_NUM10 = 13;
$BLOCK_TAG_PARA_NUM11 = 14;
$BLOCK_TAG_PARA_NUM12 = 15;
$BLOCK_TAG_PARA_NUM13 = 16;
$BLOCK_TAG_PARA_NUM14 = 17;
$BLOCK_TAG_PARA_NUM15 = 18;
$BLOCK_TAG_PARA_MAX = 19;


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


#�u���b�N�z�u���
#X�@Z�@�u���b�N�C���f�b�N�X�@�Z�b�g�̏����쐬
$BLOCK_ALL_DATA_PARA_SIZE_X = 0;
$BLOCK_ALL_DATA_PARA_SIZE_Z = 1;
$BLOCK_ALL_DATA_PARA_TAG    = 2;
$BLOCK_ALL_DATA_PARA_MAX    = 3;

#���
@BLOCK_ALL_DATA = ( 
  0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 
  0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 
  0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 
  0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 
  0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 
  0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 
  0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 
  0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM, 0,0,$BLOCK_NONE_NUM
);

#�z�u�����쐬
for( $i=0; $i<$BLOCK_DATA_INDEX; $i++ )
{
  $tag_index = &getTagIndex( $BLOCK_DATA[ $i ] );
  if( $tag_index != $BLOCK_NONE_NUM )
  {
    $block_x = $BLOCK_TAG[ ($tag_index*$BLOCK_TAG_PARA_MAX) + $BLOCK_TAG_PARA_SIZ_X ];
    $block_z = $BLOCK_TAG[ ($tag_index*$BLOCK_TAG_PARA_MAX) + $BLOCK_TAG_PARA_SIZ_Z ];

    if( $block_x >= 8 )
    {
      print( "�u���b�N�T�C�Y�I�[�o�[ �u���b�NNo=$tag_index\n" );
      exit(1);
    }
    if( $block_z >= 8 )
    {
      print( "�u���b�N�T�C�Y�I�[�o�[ �u���b�NNo=$tag_index\n" );
      exit(1);
    }

    #print( "block x $block_x  block z $block_z \n" );

    $block_idx_z = int($i/8);
    $block_idx_x = $i%8;
    
    #�͈͓��ɁA�f�[�^��ݒ�
    for( $z = 0; $z<$block_z; $z++ )
    {
      for( $x = 0; $x<$block_x; $x++ )
      {
        $block_all_index = ((($z+$block_idx_z) * 8) + ($x+$block_idx_x)) * $BLOCK_ALL_DATA_PARA_MAX;

        #print( "block index $block_all_index tag $tag_index \n" );
        
        $BLOCK_ALL_DATA[ $block_all_index + $BLOCK_ALL_DATA_PARA_SIZE_X ] = $x;
        $BLOCK_ALL_DATA[ $block_all_index + $BLOCK_ALL_DATA_PARA_SIZE_Z ] = $z;
        $BLOCK_ALL_DATA[ $block_all_index + $BLOCK_ALL_DATA_PARA_TAG ] = $tag_index;
      }
    }
  }
}



#���̏o��
open( FILEOUT, ">".$ARGV[2] );
binmode( FILEOUT );

#�z�u�����o��
for( $i=0; $i<64; $i++ )  # 8 * 8 �� 64
{
  $pos = ($BLOCK_ALL_DATA[($i*$BLOCK_ALL_DATA_PARA_MAX) + $BLOCK_ALL_DATA_PARA_SIZE_X]) | ($BLOCK_ALL_DATA[($i*$BLOCK_ALL_DATA_PARA_MAX) + $BLOCK_ALL_DATA_PARA_SIZE_Z] << 4);

  #print( "block tag".$BLOCK_ALL_DATA[($i*$BLOCK_ALL_DATA_PARA_MAX) + $BLOCK_ALL_DATA_PARA_TAG]." pos x".$BLOCK_ALL_DATA[($i*$BLOCK_ALL_DATA_PARA_MAX) + $BLOCK_ALL_DATA_PARA_SIZE_X]." pos z".$BLOCK_ALL_DATA[($i*$BLOCK_ALL_DATA_PARA_MAX) + $BLOCK_ALL_DATA_PARA_SIZE_Z]."\n" );
  
  print( FILEOUT pack( "C", $pos ) ); 
  print( FILEOUT pack( "C", $BLOCK_ALL_DATA[($i*$BLOCK_ALL_DATA_PARA_MAX) + $BLOCK_ALL_DATA_PARA_TAG] ) ); 
}

#�^�O�f�[�^���o��
for( $i=0; $i<$BLOCK_TAG_INDEX; $i++ )
{
  for( $j=$BLOCK_TAG_PARA_NUM00; $j<$BLOCK_TAG_PARA_MAX; $j++ )
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



