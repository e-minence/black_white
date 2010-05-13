############################
#
#   �e�N�X�`���A�p���b�g�j��
#     texdel.pl imd_name texname
#
#
############################

$ARG_IDX_IMDNAME  =0;
$ARG_IDX_TEXNAME  =1;
$ARG_IDX_MAX  =2;

if( @ARGV < $ARG_IDX_MAX ){
  print( "texdel.pl imd_name texname\n" );
  exit(1);
}



@all_tex_file = glob( "../src_imd_files/*.imd" );

$imd_name = $ARGV[ $ARG_IDX_IMDNAME ];
$tex_name = $ARGV[ $ARG_IDX_TEXNAME ];
foreach $filename ( @all_tex_file ){


  if( $filename =~ /$imd_name/ ){
    open( FILEIN, $filename );
    @imdData = <FILEIN>;
    close( FILEIN );

    @outPut = undef;
    $outPutIdx = 0;

    $tex_del_after = 0;
    $plt_del_after = 0;
    
    $tex_del_idx = 0;
    $plt_del_idx = 0;

    $output_cut = 0;
    
    #�f�[�^�폜
    foreach $line ( @imdData ){

      if( $output_cut == 2 ){
        $output_cut = 0;
      }
      

      #�e�N�X�`����
      if( $line =~ /tex_image_array size="([0-9]*)"/ ){
        $datanum = $1;
        $setnum  = $datanum - 1;

        $line =~ s/$datanum/$setnum/; #���l�̒u��
      }

      #�p���b�g��
      if( $line =~ /tex_palette_array size="([0-9]*)"/ ){
        $datanum = $1;
        $setnum  = $datanum - 1;

        $line =~ s/$datanum/$setnum/; #���l�̒u��
      }

      #�e�N�X�`���폜
      if( $tex_del_after ){

        #�C���f�b�N�X���P�폜
        if( $line =~ /tex_image index="([0-9]*)"/ ){
          $datanum = $1;
          $setnum  = $datanum - 1;

          $line =~ s/$datanum/$setnum/; #���l�̒u��
        }elsif( $line =~ /tex_image_idx="([0-9]*)"/ ){
          $datanum = $1;
          if( $tex_del_idx <= $datanum ){
            $setnum  = $datanum - 1;

            $line =~ s/$datanum/$setnum/; #���l�̒u��
          }
        }

      }else{
        
        if( $output_cut ){
          #�폜�I���҂�
          if( $line =~ /\/tex_image/ ){
            $output_cut = 2;
            $tex_del_after = 1;
          }
        }else{

          #�폜�J�n
          if( $line =~ /tex_image index="([0-9]*)" name="$tex_name"/ ){
            $tex_del_idx = $1;
            $output_cut = 1;
          }
        }
        
      }

      #�p���b�g�폜
      if( $plt_del_after ){

        #�C���f�b�N�X���P�폜
        if( $line =~ /tex_palette index="([0-9]*)"/ ){
          $datanum = $1;
          $setnum  = $datanum - 1;

          $line =~ s/$datanum/$setnum/; #���l�̒u��
        }elsif( $line =~ /tex_palette_idx="([0-9]*)"/ ){
          $datanum = $1;
          if( $plt_del_idx <= $datanum ){
            $setnum  = $datanum - 1;

            $line =~ s/$datanum/$setnum/; #���l�̒u��
          }
        }

        
      }else{

        if( $output_cut ){
          #�폜�I���҂�
          if( $line =~ /\/tex_palette/ ){
            $output_cut = 2;
            $plt_del_after = 1;
          }
        }else{

          #�폜�J�n
          $check = 'tex_palette index="[0-9]*" name="'.$tex_name.'_pl"';
          if( $line =~ /$check/ ){
            if( $line =~ /tex_palette index="([0-9]*)"/ ){
              $plt_del_idx = $1;
            }
            $output_cut = 1;
          }
        }

      }

      

      if( $output_cut == 0 ){
        $outPut[ $outPutIdx ] = $line;
        $outPutIdx ++;
      }
    }

    if( ($output_cut != 0) ){
      print( "del error1\n" );
      exit(1);
    }

    if( ($tex_del_after == 0) ){
      print( "del $texname ���݂���Ȃ�\n" );
      exit(1);
    }

    if( ($plt_del_after == 0) ){
      print( "del $texname �p���b�g���݂���Ȃ�\n" );
      exit(1);
    }

    open( FILEOUT, ">".$filename );

    for( $i=0; $i<$outPutIdx; $i++ ){
      print( FILEOUT $outPut[ $i ] );
    }

    close( FILEOUT );
  }
}


