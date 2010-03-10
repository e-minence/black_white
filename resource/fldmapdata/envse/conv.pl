##################################################
#
#   Pokemon_WB_SE_Env.xls
#     �R���o�[�g��
#
#   conv.pl excel.csv output
#
#
##################################################


if( @ARGV < 2 ){
  print( "conv.pl excel.csv output\n" );
  exit(1);
}


open( FILEIN, @ARGV[0] );
@EXCELFILE = <FILEIN>;
close( FILEIN );



$DATA_IDX_LABEL = 0;
$DATA_IDX_NAIYOU = 1;
$DATA_IDX_NAIYOU2 = 2;
$DATA_IDX_LOOP = 3;

@LABEL_TBL = undef;
@LOOP_TBL = undef;
$DATA_NUM = 0;

$data_in = 0;

foreach $one (@EXCELFILE)
{
  $one =~ s/\n//g;
  $one =~ s/\r\n//g;
  
  @line = split( /,/, $one );
  
  if( $data_in == 0 )
  {
    if( "".$line[$DATA_IDX_LABEL] eq "���x����" ){
      $data_in = 1;
    }
  }
  elsif( $data_in == 1 )
  {
    
    if( "".$line[$DATA_IDX_LABEL] eq "#END" ){
      $data_in = 0;
    }else{

      if( $line[$DATA_IDX_LABEL] =~ /^SEQ/ ){

        $LABEL_TBL[ $DATA_NUM ] = $line[$DATA_IDX_LABEL];
        if( "".$line[$DATA_IDX_LOOP] eq "Loop" ){
          $LOOP_TBL[ $DATA_NUM ] = "TRUE";
        }else{
          $LOOP_TBL[ $DATA_NUM ] = "FALSE";
        }

        $DATA_NUM ++;
      }
    }
  }
}

#�o��

open( FILEOUT, ">".$ARGV[1] );

print( FILEOUT "// ���̃\�[�X��resource/fldmapdata/envse/conv.pl����o�͂���Ă��܂��B\n" );
print( FILEOUT "\n" );
print( FILEOUT "\n" );
print( FILEOUT "const FLD_ENVSE_DATA c_FLD_ENVSE_DATA[] = {\n" );
for( $i=0; $i<$DATA_NUM; $i++ ){
  print( FILEOUT "{ ".$LABEL_TBL[ $i ].", ".$LOOP_TBL[ $i ]."},\n" );
}
print( FILEOUT "};\n" );

close( FILEOUT );

