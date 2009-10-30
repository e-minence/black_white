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
$count_x;
$count_z;
@imd;
$filename;
$area_min_x;
$area_max_x;
$area_min_z;
$area_max_z;

for( $count_z=0; $count_z<$ARGV[2]; $count_z++ )
{
  for( $count_x=0; $count_x<$ARGV[1]; $count_x++ )
  {
    $filename = $ARGV[0];
    
    if( $count_x < 10 )
    {
      $filename = $filename."0".$count_x;
    }
    else
    {
      $filename = $filename.$count_x;
    }

    if( $count_z < 10 )
    {
      $filename = $filename."_0".$count_z.".imd";
    }
    else
    {
      $filename = $filename."_".$count_z.".imd";
    }

    print( "load $filename..." );
    open( FILEIN, $filename );
    @imd = <FILEIN>;
    close( FILEIN );

    if( @imd == 0 )
    {
      print( "err not found\n" );

      #�ő�T�C�Y�ɂ���
      $local_size_x = 512;
      $local_size_z = 512;

      #�J�n�ʒu�̎擾
      $local_top  = 0;
      $local_left = 0;
    }
    else
    {
      print( "ok\n" );

      #�T�C�Y�̎擾
      $local_size_x = &getImdSizeWidth( \@imd );
      $local_size_z = &getImdSizeHeight( \@imd );

      #�J�n�ʒu�̎擾
      $local_top  = &getImdTop( \@imd );
      $local_left = &getImdLeft( \@imd );
    }
    

    print( "local size_x $local_size_x  size_z $local_size_z  top $local_top left $local_left\n\n" );

    if( ($count_z == $count_x) && ($count_z == 0) )
    {
      #�ŏ��̃u���b�N
      $model_top  = $local_top;
      $model_left = $local_left;
      $model_size_x = $local_size_x;
      $model_size_z = $local_size_z;
    }
    else
    {
      #���̌�̃u���b�N
      $model_size_x += $local_size_x - $local_left;
      $model_size_z += $local_size_z - $local_top;
    }
  }
}


#�u���b�N�̃T�C�Y����A�X�N���[���G���A�����߂�
$area_min_x = $model_left + $ARGV[4];
$area_max_x = $model_left + $model_size_x - $ARGV[4];
$area_min_z = $model_top + $ARGV[5];
$area_max_z = $model_top + $model_size_z - $ARGV[5];

if( $area_min_x > $area_max_x )
{
  #�����𓯂����W�ɂ���@����
  $area_min_x = ($model_left + $model_size_x) / 2;
  $area_max_x = ($model_left + $model_size_x) / 2;
}

if( $area_min_z > $area_max_z )
{
  #�����𓯂����W�ɂ���@����
  $area_min_z = ($model_top + $model_size_z) / 2;
  $area_max_z = ($model_top + $model_size_z) / 2;
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
