#####################################################################
#
# �J�������͈͂̎�������
#
# conv.pl map_matrix�̃��f����  �u���b�N�����@�u���b�N�c���@�}�g���b�N�XID���@�m���X�N���[�������@�m���X�N���[���c��
#
#
#####################################################################

if( @ARGV != 6 )
{
  print( "conv.pl map_matrix�̃��f����  �u���b�N�����@�u���b�N�c���@�}�g���b�N�XID���@�m���X�N���[�������@�m���X�N���[���c��\n" );
  exit(1);
}

$model_top = 0;
$model_left = 0;
$model_size_x = 0;
$model_size_z = 0;
$max_model_top = 0;
$max_model_left = 0;
$max_model_size_x = 0;
$max_model_size_z = 0;
$count_x;
$count_z;
@imd;
$filename;
$area_min_x;
$area_max_x;
$area_min_z;
$area_max_z;

$roop_num;


#���[�v�������߂�
if( $ARGV[1] <= 1 ){
  $roop_num = 1;
}else{
  $roop_num = 2;
}

#���̍ő�����߂�
for( $count_z=0; $count_z<$ARGV[2]; $count_z++ )
{
  $count_x = 0;
  for( $i=0; $i<$roop_num; $i++ )
  {
    
    $filename = $ARGV[0];
    $filename = &getImdFileName( $filename, $count_x, $count_z );
    
    print( "load $filename..." );
    open( FILEIN, $filename );
    @imd = <FILEIN>;
    close( FILEIN );

    if( @imd == 0 )
    {
      print( "err not found\n" );

      #�ő�T�C�Y�ɂ���
      $local_size_x = 512;

      #�J�n�ʒu�̎擾
      $local_left = 0;
    }
    else
    {
      print( "ok\n" );

      #�T�C�Y�̎擾
      $local_size_x = &getImdSizeWidth( \@imd );

      #�J�n�ʒu�̎擾
      $local_left = &getImdLeft( \@imd );
    }
    

    print( "local size_x $local_size_x  left $local_left\n\n" );

    if( ($count_z == $count_x) && ($count_z == 0) )
    {
      #�ŏ��̃u���b�N
      $model_left = $local_left;
      $model_size_x = $local_size_x;
    }
    else
    {
      #���̌�̃u���b�N
      $model_size_x = ($count_x * 512) + $local_size_x - $local_left;
    }


    #count_x���J�E���g
    $count_x = $ARGV[1] - 1;
  }

  if( $count_z == 0 )
  {
    $max_model_left = $model_left;
    $max_model_size_x = $model_size_x;
  }
  else
  {
    #���[�@���Ȃ��ق���ۑ�
    if( $max_model_left > $model_left )
    {
      $max_model_left = $model_left;
    }

    #�T�C�Y�@�傫���ق���ۑ�
    if( $max_model_size_x > $model_size_x )
    {
      $max_model_size_x = $model_size_x;
    }
  }
}


#���[�v�������߂�
if( $ARGV[2] <= 1 ){
  $roop_num = 1;
}else{
  $roop_num = 2;
}

#�c�̍ő�����߂�
for( $count_x=0; $count_x<$ARGV[1]; $count_x++ )
{
  $count_z = 0;
  
  for( $i=0; $i<$roop_num; $i++ )
  {
    $filename = $ARGV[0];
    $filename = &getImdFileName( $filename, $count_x, $count_z );
    
    print( "load $filename..." );
    open( FILEIN, $filename );
    @imd = <FILEIN>;
    close( FILEIN );

    if( @imd == 0 )
    {
      print( "err not found\n" );

      #�ő�T�C�Y�ɂ���
      $local_size_z = 512;

      #�J�n�ʒu�̎擾
      $local_top  = 0;
    }
    else
    {
      print( "ok\n" );

      #�T�C�Y�̎擾
      $local_size_z = &getImdSizeHeight( \@imd );

      #�J�n�ʒu�̎擾
      $local_top  = &getImdTop( \@imd );
    }
    

    print( "local size_z $local_size_z  top $local_top\n\n" );

    if( ($count_z == $count_x) && ($count_z == 0) )
    {
      #�ŏ��̃u���b�N
      $model_top  = $local_top;
      $model_size_z = $local_size_z;
    }
    else
    {
      #�ŏI�̃u���b�N
      $model_size_z += ($count_z * 512) + $local_size_z - $local_top;
    }


    $count_z += $ARGV[2] - 1;
  }

  if( $count_x == 0 )
  {
    $max_model_top = $model_top;
    $max_model_size_z = $model_size_z;
  }
  else
  {
    #���[�@���Ȃ��ق���ۑ�
    if( $max_model_top > $model_top )
    {
      $max_model_top = $model_top;
    }

    #�T�C�Y�@�傫���ق���ۑ�
    if( $max_model_size_z > $model_size_z )
    {
      $max_model_size_z = $model_size_z;
    }
  }

}


#�u���b�N�̃T�C�Y����A�X�N���[���G���A�����߂�
$area_min_x = $max_model_left + $ARGV[4];
$area_max_x = $max_model_left + $max_model_size_x - $ARGV[4];
$area_min_z = $max_model_top + $ARGV[5];
$area_max_z = $max_model_top + $max_model_size_z - $ARGV[5];

if( $area_min_x > $area_max_x )
{
  #�����𓯂����W�ɂ���@����
  $area_min_x = ($max_model_left + $max_model_size_x) / 2;
  $area_max_x = ($max_model_left + $max_model_size_x) / 2;
}

if( $area_min_z > $area_max_z )
{
  #�����𓯂����W�ɂ���@����
  $area_min_z = ($max_model_top + $max_model_size_z) / 2;
  $area_max_z = ($max_model_top + $max_model_size_z) / 2;
}

print( "area min_x $area_min_x  max_x $area_max_x  min_z $area_min_z max_z $area_max_z\n" );

#�o��
open( FILEOUT, ">".$ARGV[3].".bin" );

#�J�����f�[�^�Ƃ��ďo��
binmode( FILEOUT );

print( FILEOUT pack( "I", 1 ) );    #tblnum

print( FILEOUT pack( "I", 1 ) );    #FIELD_CAMERA_AREA_RECT 
print( FILEOUT pack( "I", 0 ) );    #FIELD_CAMERA_AREA_CONT_TARGET 

print( FILEOUT pack( "i", $area_min_x ) );
print( FILEOUT pack( "i", $area_max_x ) );
print( FILEOUT pack( "i", $area_min_z ) );
print( FILEOUT pack( "i", $area_max_z ) );

close( FILEOUT );

exit(0);



#imd�̉��T�C�Y���擾����
sub getImdSizeWidth
{
  my( $imdfile ) = @_;
  my( $pos_scale, $width, $one );

  foreach $one ( @$imdfile )
  {
    if( $one =~ /box_test\spos_scale=\"([0-9]*)/ )
    {
      $pos_scale = 1<<$1;

      if( $one =~ /whd=\"([^\s]*)\s/ )
      {
        $width = $1 * $pos_scale;
        return $width;
      }
    }
  }

  print( "getImdSizeWidth not found box_test\n" );
  exit(1);
}


#imd�̏c�T�C�Y���擾����
sub getImdSizeHeight
{
  my( $imdfile ) = @_;
  my( $pos_scale, $height, $one );

  foreach $one ( @$imdfile )
  {
    if( $one =~ /box_test pos_scale=\"([0-9]*)/ )
    {
      $pos_scale = 1<<$1;
      
      if( $one =~ /whd=\"[^\s]*\s[^\s]*\s([^\s]*)\s/ )
      {
        $height = $1 * $pos_scale;
        return $height;
      }
    }
  }

  print( "getImdSizeHeight not found box_test\n" );
  exit(1);
}


#imd�̏�ʒu���擾����
sub getImdTop
{
  my( $imdfile ) = @_;
  my( $pos_scale, $top, $one );

  foreach $one ( @$imdfile )
  {
    if( $one =~ /box_test pos_scale=\"([0-9]*)/ )
    {
      $pos_scale = 1<<$1;
      
      if( $one =~ /xyz=\"[^\s]*\s[^\s]*\s([^\s]*)\s/ )
      {
        $top = $1 * $pos_scale;
        $top += 256;
        return $top;
      }
    }
  }

  print( "getImdTop not found box_test\n" );
  exit(1);
}


#imd�̉��ʒu���擾����
sub getImdLeft
{
  my( $imdfile ) = @_;
  my( $pos_scale, $left, $one );

  foreach $one ( @$imdfile )
  {
    if( $one =~ /box_test pos_scale=\"([0-9]*)/ )
    {
      $pos_scale = 1<<$1;
      
      if( $one =~ /xyz=\"([^\s]*) / )
      {
        $left = $1 * $pos_scale;
        $left += 256;
        return $left;
      }
    }
  }

  print( "getImdLeft not found box_test\n" );
  exit(1);
}


#�t�@�C���������߂�
sub getImdFileName
{
  my( $basename, $block_x, $block_z ) = @_;
  my( $local_filename );

  if( $block_x < 10 )
  {
    $local_filename = $basename."0".$block_x;
  }
  else
  {
    $local_filename = $basename.$count_x;
  }

  if( $count_z < 10 )
  {
    $local_filename = $local_filename."_0".$block_z.".imd";
  }
  else
  {
    $local_filename = $local_filename."_".$block_z.".imd";
  }

  return $local_filename;
}
