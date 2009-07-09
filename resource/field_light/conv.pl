################################################
#
#	CSV�`���̃��C�g�f�[�^��Binary�ɂ��ďo��
#
#	conv.pl	csvfile
#
################################################


#�������`�F�b�N
if( @ARGV < 2 ){
	print( "need 2 param\n" );
	print( "conv.pl	csvfile outputfile\n" );
	exit(1);
}


#�t�@�C���ǂݍ���
open( FILE, $ARGV[0] );
@CSVFILE = <FILE>;
close( FILE );


$FRAME_NUM		= 0;		#�t���[����
@FRAME			= undef;	#�t���[��

@LIGHT00_FLAG	= undef;	#���C�g�g�p�t���O
@LIGHT01_FLAG	= undef;	#���C�g�g�p�t���O
@LIGHT02_FLAG	= undef;	#���C�g�g�p�t���O
@LIGHT03_FLAG	= undef;	#���C�g�g�p�t���O
@LIGHT00_X		= undef;	#���C�gX
@LIGHT01_X		= undef;	#���C�gX
@LIGHT02_X		= undef;	#���C�gX
@LIGHT03_X		= undef;	#���C�gX
@LIGHT00_Y		= undef;	#���C�gY
@LIGHT01_Y		= undef;	#���C�gY
@LIGHT02_Y		= undef;	#���C�gY
@LIGHT03_Y		= undef;	#���C�gY
@LIGHT00_Z		= undef;	#���C�gZ
@LIGHT01_Z		= undef;	#���C�gZ
@LIGHT02_Z		= undef;	#���C�gZ
@LIGHT03_Z		= undef;	#���C�gZ
@LIGHT00_R		= undef;	#���C�gR
@LIGHT01_R		= undef;	#���C�gR
@LIGHT02_R		= undef;	#���C�gR
@LIGHT03_R		= undef;	#���C�gR
@LIGHT00_G		= undef;	#���C�gG
@LIGHT01_G		= undef;	#���C�gG
@LIGHT02_G		= undef;	#���C�gG
@LIGHT03_G		= undef;	#���C�gG
@LIGHT00_B		= undef;	#���C�gB
@LIGHT01_B		= undef;	#���C�gB
@LIGHT02_B		= undef;	#���C�gB
@LIGHT03_B		= undef;	#���C�gB

@DIFFUSE_R		= undef;	#�f�B�t���[�Y
@DIFFUSE_G		= undef;	#�f�B�t���[�Y
@DIFFUSE_B		= undef;	#�f�B�t���[�Y
@AMBIENT_R		= undef;	#�A���r�G���g
@AMBIENT_G		= undef;	#�A���r�G���g
@AMBIENT_B		= undef;	#�A���r�G���g
@SPECULAR_R		= undef;	#�X�y�L�����[
@SPECULAR_G		= undef;	#�X�y�L�����[
@SPECULAR_B		= undef;	#�X�y�L�����[
@EMISSION_R		= undef;	#�G�~�b�V����
@EMISSION_G		= undef;	#�G�~�b�V����
@EMISSION_B		= undef;	#�G�~�b�V����
@FOG_R			= undef;	#�t�H�O�J���[
@FOG_G			= undef;	#�t�H�O�J���[
@FOG_B			= undef;	#�t�H�O�J���[
@BG_R			= undef;	#�w�ʃJ���[
@BG_G			= undef;	#�w�ʃJ���[
@BG_B			= undef;	#�w�ʃJ���[


#�f�[�^�擾
$inputdata = 0;
$datacount = 0;
$i = 0;
foreach $one ( @CSVFILE ){
	@line = split( /,/, $one );
	if( $inputdata == 0 ){
		if( $line[ 0 ] eq "frame" ){
			$inputdata = 1;

			#�f�[�^������
			while( $line[ $FRAME_NUM+1 ] =~ /[0-9]/ ){
				$FRAME[ $FRAME_NUM ] = $line[ $FRAME_NUM+1 ];
				$FRAME_NUM++;
			}
		}
	}else{

		if( $datacount == 0 ){
			
			for( $i=0; $i<$FRAME_NUM; $i++ ){
				$LIGHT00_FLAG[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 1 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				$LIGHT00_X[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 2 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@LIGHT00_Y[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 3 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@LIGHT00_Z[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 4 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@LIGHT00_R[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 5 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@LIGHT00_G[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 6 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@LIGHT00_B[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 7 ){
			
			for( $i=0; $i<$FRAME_NUM; $i++ ){
				$LIGHT01_FLAG[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 8 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				$LIGHT01_X[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 9 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@LIGHT01_Y[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 10 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@LIGHT01_Z[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 11 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@LIGHT01_R[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 12 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@LIGHT01_G[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 13 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@LIGHT01_B[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 14 ){
			
			for( $i=0; $i<$FRAME_NUM; $i++ ){
				$LIGHT02_FLAG[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 15 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				$LIGHT02_X[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 16 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@LIGHT02_Y[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 17 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@LIGHT02_Z[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 18 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@LIGHT02_R[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 19 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@LIGHT02_G[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 20 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@LIGHT02_B[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 21 ){
			
			for( $i=0; $i<$FRAME_NUM; $i++ ){
				$LIGHT03_FLAG[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 22 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				$LIGHT03_X[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 23 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@LIGHT03_Y[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 24 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@LIGHT03_Z[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 25 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@LIGHT03_R[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 26 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@LIGHT03_G[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 27 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@LIGHT03_B[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 28 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@DIFFUSE_R[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 29 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@DIFFUSE_G[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 30 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@DIFFUSE_B[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 31 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@AMBIENT_R[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 32 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@AMBIENT_G[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 33 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@AMBIENT_B[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 34 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@SPECULAR_R[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 35 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@SPECULAR_G[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 36 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@SPECULAR_B[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 37 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@EMISSION_R[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 38 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@EMISSION_G[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 39 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@EMISSION_B[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 40 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@FOG_R[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 41 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@FOG_G[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 42 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@FOG_B[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 43 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@BG_R[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 44 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@BG_G[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 45 ){

			for( $i=0; $i<$FRAME_NUM; $i++ ){
				@BG_B[ $i ] = $line[ $i+1 ]
			}
		}
		elsif( $datacount == 46 ){
			#�I��
			$inputdata = 0;
		}

		$datacount++;
	}
}


#�o��
open( FILE, ">".$ARGV[1] );
binmode( FILE );

for( $i=0; $i<$FRAME_NUM; $i++ ){
	#�e�t���[�����Ƃł܂Ƃ߂�
	print( FILE pack( "I", $FRAME[$i] ) );
	
	print( FILE pack( "C", $LIGHT00_FLAG[$i] ) );
	print( FILE pack( "C", $LIGHT01_FLAG[$i] ) );
	print( FILE pack( "C", $LIGHT02_FLAG[$i] ) );
	print( FILE pack( "C", $LIGHT03_FLAG[$i] ) );

	print( FILE pack( "S", &CONV_GetRgb( $LIGHT00_R[$i], $LIGHT00_G[$i], $LIGHT00_B[$i] ) ) );
	print( FILE pack( "S", &CONV_GetRgb( $LIGHT01_R[$i], $LIGHT01_G[$i], $LIGHT01_B[$i] ) ) );
	print( FILE pack( "S", &CONV_GetRgb( $LIGHT02_R[$i], $LIGHT02_G[$i], $LIGHT02_B[$i] ) ) );
	print( FILE pack( "S", &CONV_GetRgb( $LIGHT03_R[$i], $LIGHT03_G[$i], $LIGHT03_B[$i] ) ) );

	print( FILE pack( "S", $LIGHT00_X[$i]) );
	print( FILE pack( "S", $LIGHT00_Y[$i]) );
	print( FILE pack( "S", $LIGHT00_Z[$i]) );
	print( FILE pack( "S", $LIGHT01_X[$i]) );
	print( FILE pack( "S", $LIGHT01_Y[$i]) );
	print( FILE pack( "S", $LIGHT01_Z[$i]) );
	print( FILE pack( "S", $LIGHT02_X[$i]) );
	print( FILE pack( "S", $LIGHT02_Y[$i]) );
	print( FILE pack( "S", $LIGHT02_Z[$i]) );
	print( FILE pack( "S", $LIGHT03_X[$i]) );
	print( FILE pack( "S", $LIGHT03_Y[$i]) );
	print( FILE pack( "S", $LIGHT03_Z[$i]) );


	print( FILE pack( "S", &CONV_GetRgb( $DIFFUSE_R[$i], $DIFFUSE_G[$i], $DIFFUSE_B[$i] ) ) );
	print( FILE pack( "S", &CONV_GetRgb( $AMBIENT_R[$i], $AMBIENT_G[$i], $AMBIENT_B[$i] ) ) );
	print( FILE pack( "S", &CONV_GetRgb( $SPECULAR_R[$i], $SPECULAR_G[$i], $SPECULAR_B[$i] ) ) );
	print( FILE pack( "S", &CONV_GetRgb( $EMISSION_R[$i], $EMISSION_G[$i], $EMISSION_B[$i] ) ) );
	print( FILE pack( "S", &CONV_GetRgb( $FOG_R[$i], $FOG_G[$i], $FOG_B[$i] ) ) );
	print( FILE pack( "S", &CONV_GetRgb( $BG_R[$i], $BG_G[$i], $BG_B[$i] ) ) );

}

close( FILE );


#����I��
exit(0);


sub CONV_GetRgb
{
	my( $r, $g, $b ) = @_;
	my( $ans );
	
	$ans =  $r<<0;
	$ans |= $g<<5;
	$ans |= $b<<10;

	return $ans;
}
