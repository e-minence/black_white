####################################################
#
#   conv.pl excel_tab tag_header arc_list
#
####################################################


#�����`�F�b�N
if( @ARGV < 3 )
{
  print( "conv.pl excel_tab tag_header arc_list\n" );
  exit(1);
}


#Excel�t�@�C���ǂݍ���
@EXCEL_FILE = undef;
open( FILEIN, $ARGV[0] );
@EXCEL_FILE = <FILEIN>; 
close( FILEIN );


#-------------------------------------
# ���e
#=====================================
$PARA_TAG_IDX   = 0;
$PARA_XGRID_IDX = 1;
$PARA_ZGRID_IDX = 2;
$PARA_ATTR_IDX  = 3;
$PARA_MDL_IDX   = 4;
$PARA_MAX       = 5;


#�����쐬
@INPUT_DATA = undef;
$INPUT_DATA_MAX = 0;
$data_in = 0;
foreach $one ( @EXCEL_FILE )
{
  @line = split( /\t/, $one );
  
  if( $data_in == 0 )
  {
    if( "".$line[0] eq "TAG" )
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
      #�f�[�^�i�[
      for( $i=0; $i<$PARA_MAX; $i++ )
      {
        $INPUT_DATA[ ($INPUT_DATA_MAX * $PARA_MAX) + $i ] = $line[$i];
      }
      $INPUT_DATA_MAX ++;
    }
  }
}

#�A�[�N���X�g�̏o��
open( ARCLIST_FILE, ">".$ARGV[2] );

#�w�b�_�[�̏o��
open( HEADER_FILE, ">".$ARGV[1] );
print( HEADER_FILE "// output resource/fldmapdata/land_data_patch/conv.pl\n" );
print( HEADER_FILE "#pragma once\n\n" );


#�A�g���r���[�g�����O���b�h�P�ʂ̏��ɕύX
#binary�Ƃ��Čł߂�
for( $i=0; $i<$INPUT_DATA_MAX; $i++ )
{
  $input_grid_x = $INPUT_DATA[ ($i * $PARA_MAX) + $PARA_XGRID_IDX ];
  $input_grid_z = $INPUT_DATA[ ($i * $PARA_MAX) + $PARA_ZGRID_IDX ];
  
  #.bin
  $filename = $INPUT_DATA[ ($i * $PARA_MAX) + $PARA_ATTR_IDX ];
  open( FILEIN, $filename );
  binmode( FILEIN );

  @data_s = unpack( "S*", <FILEIN> ); #unsigned short�ł݂�
  @data_c = unpack( "C*", <FILEIN> ); #unsigned char�ł݂�

  close( FILEIN );

  #.3dmd
  $filename_3dmd = $INPUT_DATA[ ($i * $PARA_MAX) + $PARA_MDL_IDX ];
  open( FILEIN, $filename_3dmd );
  binmode( FILEIN );

  @data3dmd_c = unpack( "C*", <FILEIN> ); #unsigned char�ł݂�

  close( FILEIN );

  #�ŏ���2�̓O���b�h��
  $grid_x =  $data_s[0];
  $grid_z =  $data_s[1];
  #print( "gridx=$grid_x girdz=$grid_x\n" );

  #�t�@�C���T�C�Y����A�P�̃f�[�^�̃T�C�Y���v�Z
  ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,
    $atime,$mtime,$ctime,$blksize,$blocks) = stat($filename);

  #1�O���b�h�̃f�[�^�T�C�Y���v�Z
  $size     -= 4; #�w�b�_�[�������炷
  $size_one = $size;
  $size_one /= ($grid_x * $grid_z);

  #print( "one_size = $size_one\n" );


  #�o�̓t�@�C���ɕϊ�
  #binary�t�@�C�����܂Ƃ߂ďo�͂��܂��B
  $filename = $INPUT_DATA[ ($i * $PARA_MAX) + $PARA_TAG_IDX ]."\.dat";

  #�K�v�����̂ݏo�͂��Ȃ���
  open( FILEOUT, ">".$filename );
  binmode( FILEOUT );
  

  #�e���ւ̃I�t�Z�b�g
  print( FILEOUT pack( "S", 4 ) );
  print( FILEOUT pack( "S", ($size + 4) ) );
  
  #�O���b�h�T�C�Y
  print( FILEOUT pack( "S", $input_grid_x ) );
  print( FILEOUT pack( "S", $input_grid_z ) );

  #print( "input_grid_x = $input_grid_x\n" );
  #print( "input_grid_z = $input_grid_z\n" );

  #print( "ofs attr = 8\n" );
  #$size_ff = $size + 8;
  #print( "ofs 3dmd = ".$size_ff."\n" );

  #�K�v�����̂ݏo��
  for( $j=0; $j<$input_grid_z; $j++ )
  {
    for( $k=0; $k<$input_grid_x; $k++ )
    {
      $data_index = (($j * $grid_x) + $k) * $size_one;

      #�T�C�Y�������o��
      for( $l=0; $l<$size_one; $l++ )
      {
        print( FILEOUT pack( "C", $data_c[ $data_index + $l ] ) );
      }

    }
  }

  #3dmd���o��
  print( FILEOUT pack( "C*", @data3dmd_c ) );
  
  close( FILEOUT );


  #�w�b�_�[�ɒ�`��ǉ�
  print( HEADER_FILE "#define LAND_DATA_PATCH_".$INPUT_DATA[ ($i * $PARA_MAX) + $PARA_TAG_IDX ]." ( $i )\n" );

  #�A�[�N���X�g�ɒǉ�
  print( ARCLIST_FILE "\"$filename\"\n" );
}

#�w�b�_�[�ɒ�`��ǉ�
print( HEADER_FILE "#define LAND_DATA_PATCH_MAX ( $INPUT_DATA_MAX )\n" );



#�A�[�N���X�g�̏o�͊���
close( ARCLIST_FILE );

#�w�b�_�[�̏o�͊���
close( HEADER_FILE );

exit(0);


