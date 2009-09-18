#####################################################################
#
# �J�������͈͂̎�������
#
# conv.pl   map_matrix�@imd�t�H���_�@�A�[�J�C�u���X�g
#
#
#####################################################################

if( @ARGV != 3 )
{
  print( "conv.pl map_matrix imd�t�H���_ �A�[�J�C�u���X�g\n" );
  exit(1);
}


open( FILEIN, $ARGV[0] );
@map_matrix = <FILEIN>;
close( FILEIN );

open( FILEOUT, ">".$ARGV[2] );

$skip = 1;
foreach $one ( @map_matrix )
{
  @data = split( /\s/, $one );
  
  if( $skip == 1 )
  {
    $skip = 0;
  }
  elsif( $data[0] eq "#END" )
  {
  }
  else
  {
    #�^�C���X�^���v�`�F�b�N
    #bin�t�@�C�������邩�H
    #$ARGV[1].$data[3]00_00�`bin���ŐV�̃t�@�C�������邩�H
    $combine = 0;
   
    #bin�t�@�C�������邩�H
    if( -e $data[0].".bin" )
    {
      @bin_status = stat( $data[0].".bin" );
      
      #$ARGV[1].$data[3]00_00�`bin���ŐV�̃t�@�C�������邩�H
      for( $z=0; $z<$data[2]; $z++ )
      {
        for( $x=0; $x<$data[1]; $x++ )
        {
          $filename = $ARGV[1].$data[3];
          if( $x < 10 )
          {
            $filename = $filename."0".$x."_";
          }
          else
          {
            $filename = $filename.$x."_";
          }

          if( $z < 10 )
          {
            $filename = $filename."0".$z.".imd";
          }
          else
          {
            $filename = $filename.$z.".imd";
          }

          if( -e $filename )
          {
            @status = stat( $filename );

            if( $status[9] > $bin_status[9] )
            {
              $combine = 1;
            }
          }
        }
      }
    }
    else
    {
      $combine = 1;
    }

    if( $combine )
    {
      print( "output ".$data[0]."...\n" );

      #�G���A���̏o��             �t�@�C����          �@�����@�@�@�@�c���@�@�@�@�@�}�g���N�XID�@
      $cmd = "perl area_output.pl ".$ARGV[1].$data[3]." ".$data[1]." ".$data[2]." ".$data[0]." 72 40";
      if( system( $cmd ) )
      {
        close( FILEOUT );
        exit(1);
      }
    }

    #�A�[�J�C�u���X�g�̍쐬
    print( FILEOUT "\"".$data[0].".bin\"\n" ); 
  }
}

close( FILEOUT );


exit(0);
