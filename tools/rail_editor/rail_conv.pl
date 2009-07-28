##-----------------------------------------------------------------------------
#	.rail�t�@�C����Binary���ɃR���o�[�g
#
#
#	rail_conv.pl	.rail field_rail_loader_func.h field_rail.h
#
#
#
#
#
#
##-----------------------------------------------------------------------------


##-----------------------------------------------------------------------------
#		�萔
##-----------------------------------------------------------------------------
#�e���[�N�T�C�Y 
$RAIL_CAMERA_SET_WORK_SIZ;
$RAIL_LINEPOS_SET_WORK_SIZ;

#KEY
@RAIL_KEY = undef;

#���[���e�[�u��NULL
$RAIL_TBL_NULL;

#NAMEBUF
$RAIL_NAMEBUF_SIZ;


#�֐�ID
@RAIL_CAMERA_FUNC_ID = undef;
@RAIL_LINEPOS_FUNC_ID = undef;
@RAIL_LINEDIST_FUNC_ID = undef;


#���[�����
@POINT_NAME = undef;
@LINE_NAME	= undef;
@LINEPOS_NAME = undef;
@CAMERA_NAME = undef;

#CAMERA_SET LINEPOS_SET	BYTE���[�N��index�I�[�o�[�萔
$BYTEWORK_INDEX_OVER	= 0xffff;

##-----------------------------------------------------------------------------
#	���C������
##-----------------------------------------------------------------------------
#�����`�F�b�N
if( @ARGV < 3 )
{
	print( "rail_conv.pl	.rail field_rail_loader_func.h field_rail.h \n" );
	exit(1);
}



#���[�_�[�̊֐�ID�̓ǂݍ���
&LoadRailFuncID( $ARGV[ 1 ] );

#���[���V�X�e���萔�̓ǂݍ���
&loadRailSysData( $ARGV[ 2 ] );

#�R���o�[�g
&convData( $ARGV[ 0 ] );

exit(0);


##-----------------------------------------------------------------------------
#	���[�_�[�֐�ID�̓ǂݍ���
##-----------------------------------------------------------------------------
sub LoadRailFuncID
{
	my( $filename ) = @_;
	my( $one, $index, $mode, $input, @data );

	open( FILEIN, $filename );
	@data = <FILEIN>;
	close( FILEIN );
	
	$input = 0;
	foreach $one ( @data )
	{
		#�^�u�ƃX�y�[�X�r��
		$one =~ s/\s//g;
		$one =~ s/\t//g;

		
		if( $one =~ /MAX/ )
		{
			# �I��
			$input = 0;
		}
		
		if( $input == 0 )
		{
			#��`�J�n�`�F�b�N
			if( $one =~ /CAMERA_FUNC_ID_START/ )
			{
				$mode = 0;
				$index = 0;
				$input = 1;
			}
			elsif( $one =~ /LINEPOS_FUNC_ID_START/ )
			{
				$mode = 1;
				$index = 0;
				$input = 1;
			}
			elsif( $one =~ /LINEDIST_FUNC_ID_START/ )
			{
				$mode = 2;
				$index = 0;
				$input = 1;
			}
		}
		else
		{
			
			if( $mode == 0 )
			{
				if( $one =~ /([^,]*),/ )
				{
					$RAIL_CAMERA_FUNC_ID[ $index ] = $1;
					$index ++;
				}
			}
			elsif( $mode == 1 )
			{
				if( $one =~ /([^,]*),/ )
				{
					$RAIL_LINEPOS_FUNC_ID[ $index ] = $1;
					$index ++;
				}
			}
			elsif( $mode == 2 )
			{
				if( $one =~ /([^,]*),/ )
				{
					$RAIL_LINEDIST_FUNC_ID[ $index ] = $1;
					$index ++;
				}
			}
		}
	}

}

##-----------------------------------------------------------------------------
#	�֐�ID�萔���@->	�o�C�i��
##-----------------------------------------------------------------------------
sub getCameraFuncIDNameToNo
{
	my( $name ) = @_;
	my( $i );

	for( $i=0; $i<@RAIL_CAMERA_FUNC_ID; $i++ )
	{
		if( $RAIL_CAMERA_FUNC_ID[$i] =~ /$name/ )
		{
			return $i;
		}
	}

	print( "#getCameraFuncIDNameToNo sarch $name not found\n" );
	exit(1);
}

sub getLinePosFuncIDNameToNo
{
	my( $name ) = @_;
	my( $i );

	for( $i=0; $i<@RAIL_LINEPOS_FUNC_ID; $i++ )
	{
		if( $RAIL_LINEPOS_FUNC_ID[$i] =~ /$name/ )
		{
			return $i;
		}
	}

	print( "#getLinePosFuncIDNameToNo sarch $name not found\n" );
	exit(1);
}

sub getLineDistFuncIDNameToNo
{
	my( $name ) = @_;
	my( $i );

	for( $i=0; $i<@RAIL_LINEDIST_FUNC_ID; $i++ )
	{
		if( $RAIL_LINEDIST_FUNC_ID[$i] =~ /$name/ )
		{
			return $i;
		}
	}

	print( "#getLineDistFuncIDNameToNo sarch $name not found\n" );
	exit(1);
}


##-----------------------------------------------------------------------------
#	���[���V�X�e���̒萔�ǂݍ���
##-----------------------------------------------------------------------------
sub loadRailSysData
{
	my( $filename ) = @_;
	my( $one, $key_index, $input_count, @data );

	open( FILEIN, $filename );
	@data = <FILEIN>;
	close( FILEIN );

	$key_index = 0;
	$input_count = 0;
	foreach $one ( @data )
	{
		#�^�u�ƃX�y�[�X�r��
		$one =~ s/\s//g;
		$one =~ s/\t//g;

		if( $one =~ /RAIL_CAMERA_SET_WORK_SIZ\(([0-9]*)\)/ )
		{
			$RAIL_CAMERA_SET_WORK_SIZ = $1;
			print( "RAIL_CAMERA_SET_WORK_SIZ load = $RAIL_CAMERA_SET_WORK_SIZ\n" );
			$input_count ++;
		}

		if( $one =~ /RAIL_LINEPOS_SET_WORK_SIZ\(([0-9]*)\)/ )
		{
			$RAIL_LINEPOS_SET_WORK_SIZ = $1;
			print( "RAIL_LINEPOS_SET_WORK_SIZ load = $RAIL_LINEPOS_SET_WORK_SIZ\n" );
			$input_count ++;
		}

		if( $one =~ /RAIL_TBL_NULL\(([^\)]*)\)/ )
		{
			$RAIL_TBL_NULL = $1;
			print( "RAIL_TBL_NULL load = $RAIL_TBL_NULL\n" );
			$input_count ++;
		}

		if( $one =~ /RAIL_NAME_BUF_SIZ\(([0-9]*)\)/ )
		{
			$RAIL_NAMEBUF_SIZ = $1;
			print( "RAIL_NAME_BUF_SIZ load = $RAIL_NAMEBUF_SIZ\n" );
			$input_count ++;
		}

		#�L�[�̒萔
		if( $one =~ /RAIL_KEY_[^,]*,/ )
		{
			if( $one =~ /MAX/ )
			{
			}
			else
			{
				#�萔���i�[
				$one =~ /(RAIL_KEY_[^,=]*)[,=]/;
				$RAIL_KEY[ $key_index ] = $1;
				print( $RAIL_KEY[ $key_index ]." load\n" );
				$key_index++;
				$input_count ++;
			}
		}
	}

	#�ǂݍ��݉񐔂𒲂ׂ�
	if( $input_count != 9 )
	{
		print( "#loadRailSysData load data not 9\n" );
		exit(1);
	}
}

##-----------------------------------------------------------------------------
#	�L�[�̒�`�� ->�@�l�ɕϊ�
##-----------------------------------------------------------------------------
sub getRAIL_KeyNameToNo
{
	my( $keyname ) = @_;
	my( $i );

	for( $i=0; $i<@RAIL_KEY; $i++ )
	{
		if( $RAIL_KEY[$i] =~ /$keyname/ )
		{
			return $i;
		}
	}

	print( "#getRAIL_KeyNameToNo $keyname not found\n" );
	exit(1);
}

##-----------------------------------------------------------------------------
#	�@�l�ɕϊ�
##-----------------------------------------------------------------------------
sub get10Number
{
	my( $number ) = @_;

	if( $number eq "0" )
	{
		return 0;
	}

	if( $number =~ s/0x// )
	{
		return hex($number);
	}

	if( $number =~ s/^0// )
	{
		return oct($number);
	}
	return $number;
}


##-----------------------------------------------------------------------------
#	�R���o�[�g
##-----------------------------------------------------------------------------
sub convData
{
	my( $filename ) = @_;
	my( $i, $j, $one, @filedata, $outfile, $output_siz );
	my( $point_index, $line_index, $linepos_index, $camera_index );

	#���ǂݍ���
	open( FILEIN, $filename );
	@filedata = <FILEIN>;
	close( FILEIN );
	
	#�܂��A�|�C���g�A���C���A���W�A�J�����̊e���O���X�g���쐬
	$point_index	= 0;
	$line_index		= 0;
	$linepos_index= 0;
	$camera_index	= 0;
	for( $i=0; $i<(@filedata-1); $i++ )
	{
		$filedata[$i] =~ s/\t//g;
		$filedata[$i] =~ s/\s//g;
		$one	= $filedata[$i];
		
		if( $one =~ /\+POINT$/ )
		{
			$POINT_NAME[ $point_index ] = &getPeaceParam( \@filedata, $i+1, "NAME" );
			$point_index ++;
		}
		elsif( $one =~ /\+LINE$/ )
		{
			$LINE_NAME[ $line_index ] = &getPeaceParam( \@filedata, $i+1, "NAME" );
			$line_index ++;
		}
		elsif( $one =~ /\+LINEPOS$/ )
		{
			$LINEPOS_NAME[ $linepos_index ] = &getPeaceParam( \@filedata, $i+1, "NAME" );
			$linepos_index ++;
		}
		elsif( $one =~ /\+CAMERA$/ )
		{
			$CAMERA_NAME[ $camera_index ] = &getPeaceParam( \@filedata, $i+1, "NAME" );
			$camera_index ++;	
		}
	}
	$filedata[$i] =~ s/\t//g;
	$filedata[$i] =~ s/\s//g;

	
	#���𐔎��ɒu�����āA�o��
	$outfile = $filename;
	$outfile =~ s/\.rail//;

	#�I�t�Z�b�g���
	open( FILEOUT_OFS, ">$outfile.ofs" );
	binmode( FILEOUT_OFS );

#	print( FILEOUT_OFS pack( "i", &get10Number(&getPeaceParamForTag( \@filedata, "ALLDATA", "OFSMAX") )) );
	print( FILEOUT_OFS pack( "i", &get10Number(&getPeaceParamForTag( \@filedata, "ALLDATA", "OFSUNIT")) ) );
	
	close( FILEOUT_OFS );


	#�|�C���g���
	{
		my( $line, $key, $camera_set, $width_ofs,  $len );
		open( FILEOUT_POINT, ">$outfile.point" );
		binmode( FILEOUT_POINT );

		for( $i=0; $i<@POINT_NAME; $i++ )
		{
			#�����N���̐������`�F�b�N
			&isLinkPoint( \@filedata, $POINT_NAME[$i] );
			
			#LINES
			$line = &getPeaceParamForName( \@filedata, $POINT_NAME[$i], "LINES00");
			print( FILEOUT_POINT pack( "i", &getNameIndex( \@LINE_NAME, $line ) ) );
			$line = &getPeaceParamForName( \@filedata, $POINT_NAME[$i], "LINES01");
			print( FILEOUT_POINT pack( "i", &getNameIndex( \@LINE_NAME, $line ) ) );
			$line = &getPeaceParamForName( \@filedata, $POINT_NAME[$i], "LINES02");
			print( FILEOUT_POINT pack( "i", &getNameIndex( \@LINE_NAME, $line ) ) );
			$line = &getPeaceParamForName( \@filedata, $POINT_NAME[$i], "LINES03");
			print( FILEOUT_POINT pack( "i", &getNameIndex( \@LINE_NAME, $line ) ) );

			#KEYS
			$key = &getPeaceParamForName( \@filedata, $POINT_NAME[$i], "KEYS00");
			print( FILEOUT_POINT pack( "i", &getRAIL_KeyNameToNo( $key ) ) );
			$key = &getPeaceParamForName( \@filedata, $POINT_NAME[$i], "KEYS01");
			print( FILEOUT_POINT pack( "i", &getRAIL_KeyNameToNo( $key ) ) );
			$key = &getPeaceParamForName( \@filedata, $POINT_NAME[$i], "KEYS02");
			print( FILEOUT_POINT pack( "i", &getRAIL_KeyNameToNo( $key ) ) );
			$key = &getPeaceParamForName( \@filedata, $POINT_NAME[$i], "KEYS03");
			print( FILEOUT_POINT pack( "i", &getRAIL_KeyNameToNo( $key ) ) );

			#WIDTH_OFS
			$width_ofs = &getPeaceParamForName( \@filedata, $POINT_NAME[$i], "WIDTH_OFS00");
			print( FILEOUT_POINT pack( "i", $width_ofs ) );
			$width_ofs = &getPeaceParamForName( \@filedata, $POINT_NAME[$i], "WIDTH_OFS01");
			print( FILEOUT_POINT pack( "i", $width_ofs ) );
			$width_ofs = &getPeaceParamForName( \@filedata, $POINT_NAME[$i], "WIDTH_OFS02");
			print( FILEOUT_POINT pack( "i", $width_ofs ) );
			$width_ofs = &getPeaceParamForName( \@filedata, $POINT_NAME[$i], "WIDTH_OFS03");
			print( FILEOUT_POINT pack( "i", $width_ofs ) );


			#POS
			print( FILEOUT_POINT pack( "i", &get10Number(&getPeaceParamForName( \@filedata, $POINT_NAME[$i], "VECX")) ) );
			print( FILEOUT_POINT pack( "i", &get10Number(&getPeaceParamForName( \@filedata, $POINT_NAME[$i], "VECY")) ) );
			print( FILEOUT_POINT pack( "i", &get10Number(&getPeaceParamForName( \@filedata, $POINT_NAME[$i], "VECZ")) ) );

			#CAMERA_SET
			$camera_set = &getPeaceParamForName( \@filedata, $POINT_NAME[$i], "CAMERA_SET");
			print( FILEOUT_POINT pack( "i", &getNameIndex( \@CAMERA_NAME, $camera_set ) ) );

			#NAME
			$len = length($POINT_NAME[$i]);
			print( $POINT_NAME[$i]."len = $len\n" );
			if( $len >= $RAIL_NAMEBUF_SIZ )
			{
				print( "point_name size over ".$POINT_NAME[$i]." limit=$RAIL_NAMEBUF_SIZ\n" );
				exit(1);
			}
			$len = $RAIL_NAMEBUF_SIZ;
			print( FILEOUT_POINT pack( "A$len", $POINT_NAME[$i] ) );
		}
		
		close( FILEOUT_POINT );
	}
	print( "output $outfile.point\n" );


	#���C�����
	{
		my( $point, $key, $linepos, $camera_set, $len );
		open( FILEOUT_LINE, ">$outfile.line" );
		binmode( FILEOUT_LINE );

		for( $i=0; $i<@LINE_NAME; $i++ )
		{
			#�����N���̐������`�F�b�N
			&isLinkLine( \@filedata, $LINE_NAME[$i] );
		

			#POINT
			$point = &getPeaceParamForName( \@filedata, $LINE_NAME[$i], "POINT_S");
			print( FILEOUT_LINE pack( "i", &getNameIndex( \@POINT_NAME, $point ) ) );
			$point = &getPeaceParamForName( \@filedata, $LINE_NAME[$i], "POINT_E");
			print( FILEOUT_LINE pack( "i", &getNameIndex( \@POINT_NAME, $point ) ) );

			#KEY
			$key = &getPeaceParamForName( \@filedata, $LINE_NAME[$i], "KEY");
			print( FILEOUT_LINE pack( "i", &getRAIL_KeyNameToNo( $key ) ) );

			#LINEPOS
			$linepos = &getPeaceParamForName( \@filedata, $LINE_NAME[$i], "LINEPOS_SET");
			print( FILEOUT_LINE pack( "i", &getNameIndex( \@LINEPOS_NAME, $linepos ) ) );

			#CAMERA_SET
			$camera_set = &getPeaceParamForName( \@filedata, $LINE_NAME[$i], "CAMERA_SET");
			print( FILEOUT_LINE pack( "i", &getNameIndex( \@CAMERA_NAME, $camera_set ) ) );

			#NAME
			$len = length($LINE_NAME[$i]);
			if( $len > $RAIL_NAMEBUF_SIZ )
			{
				print( "line_name size over ".$LINE_NAME[$i]." limit=$RAIL_NAMEBUF_SIZ\n" );
				exit(1);
			}
			print( FILEOUT_LINE pack( "a$RAIL_NAMEBUF_SIZ", $LINE_NAME[$i] ) );
		}
		
		close( FILEOUT_LINE );
	}
	print( "output $outfile.line\n" );

	#���C��POS���
	{
		my( $funcindex, $distfuncindex, $work_index, $datasiz, $one_datasiz, $work_number );
		open( FILEOUT_LINEPOS, ">$outfile.linepos" );
		binmode( FILEOUT_LINEPOS );

		for( $i=0; $i<@LINEPOS_NAME; $i++ )
		{

			#FUNCINDEX
			$funcindex = &getPeaceParamForName( \@filedata, $LINEPOS_NAME[$i], "FUNCINDEX");
			print( FILEOUT_LINEPOS pack( "i", &getLinePosFuncIDNameToNo( $funcindex ) ) );

			#FUNCDISTINDEX
			$distfuncindex = &getPeaceParamForName( \@filedata, $LINEPOS_NAME[$i], "FUNCDISTINDEX");
			print( FILEOUT_LINEPOS pack( "i", &getLineDistFuncIDNameToNo( $distfuncindex ) ) );

			#���[�N�f�[�^�̏o��
			$datasiz = 0;
			$work_index = 0;
			do
			{
				$work_number = &getPeaceParamByteWorkNumberForName( \@filedata, $LINEPOS_NAME[$i], $work_index );
				$one_datasiz = &getPeaceParamByteWorkSizeForName( \@filedata, $LINEPOS_NAME[$i], $work_index );
				if( ($work_number != $BYTEWORK_INDEX_OVER) && ($one_datasiz != $BYTEWORK_INDEX_OVER) )
				{
					#�o��
					if( $one_datasiz == 1 )
					{
						print( FILEOUT_LINEPOS pack( "c", &get10Number($work_number) ) );
					}
					elsif( $one_datasiz == 2 )
					{
						print( FILEOUT_LINEPOS pack( "s", &get10Number($work_number) ) );
					}
					elsif( $one_datasiz == 4 )
					{
						print( FILEOUT_LINEPOS pack( "i", &get10Number($work_number) ) );
					}
					$datasiz += $one_datasiz;

					#�T�C�Y�I�[�o�[�`�F�b�N
					if( $datasiz > $RAIL_LINEPOS_SET_WORK_SIZ )
					{
						print( "linepos_work size over  name=".$LINEPOS_NAME[$i]."\n" );
						exit( 1 );
					}

					$work_index ++;
				}
			}while( ($work_number != $BYTEWORK_INDEX_OVER) && ($one_datasiz != $BYTEWORK_INDEX_OVER) );

			#����Ȃ����[�N���o��
			while( $datasiz < $RAIL_LINEPOS_SET_WORK_SIZ )
			{
				print( FILEOUT_LINEPOS pack( "c", 0 ) );
				$datasiz ++;
			}
		}

		close( FILEOUT_LINEPOS );
	}
	print( "output $outfile.linepos\n" );

	#�J�������
	{
		my( $funcindex, $work_index, $datasiz, $one_datasiz, $work_number );
		open( FILEOUT_CAMERA, ">$outfile.camera" );
		binmode( FILEOUT_CAMERA );

		for( $i=0; $i<@CAMERA_NAME; $i++ )
		{

			#FUNCINDEX
			$funcindex = &getPeaceParamForName( \@filedata, $CAMERA_NAME[$i], "FUNCINDEX");
			print( FILEOUT_CAMERA pack( "i", &getCameraFuncIDNameToNo( $funcindex ) ) );

			#���[�N�f�[�^�̏o��
			$datasiz = 0;
			$work_index = 0;

			do
			{
				$work_number = &getPeaceParamByteWorkNumberForName( \@filedata, $CAMERA_NAME[$i], $work_index );
				$one_datasiz = &getPeaceParamByteWorkSizeForName( \@filedata, $CAMERA_NAME[$i], $work_index );
				if( ($work_number != $BYTEWORK_INDEX_OVER) && ($one_datasiz != $BYTEWORK_INDEX_OVER) )
				{
					#�o��
					if( $one_datasiz == 1 )
					{
						print( FILEOUT_CAMERA pack( "c", &get10Number($work_number) ) );
					}
					elsif( $one_datasiz == 2 )
					{
						print( FILEOUT_CAMERA pack( "s", &get10Number($work_number) ) );
					}
					elsif( $one_datasiz == 4 )
					{
						print( FILEOUT_CAMERA pack( "i", &get10Number($work_number) ) );
					}
					$datasiz += $one_datasiz;

					#�T�C�Y�I�[�o�[�`�F�b�N
					if( $datasiz > $RAIL_CAMERA_SET_WORK_SIZ )
					{
						print( "camera_work size over  name=".$CAMERA_NAME[$i]."\n" );
						exit( 1 );
					}
					$work_index ++;
				}
			}while( ($work_number != $BYTEWORK_INDEX_OVER) && ($one_datasiz != $BYTEWORK_INDEX_OVER) );

			#����Ȃ����[�N���o��
			while( $datasiz < $RAIL_CAMERA_SET_WORK_SIZ )
			{
				print( FILEOUT_CAMERA pack( "c", 0 ) );
				$datasiz ++;
			}
		}

		close( FILEOUT_CAMERA );
	}
	print( "output $outfile.camera\n" );
}

##-----------------------------------------------------------------------------
# ���O��v�C���f�b�N�X�̎擾
##-----------------------------------------------------------------------------
sub getNameIndex
{
	my( $data, $name ) = @_;
	my( $i, $roopnum );

	#NULL�`�F�b�N
	if( $name =~ /RAIL_TBL_NULL/ )
	{
		return &get10Number($RAIL_TBL_NULL);
	}

	$roopnum = @$data;
	for( $i=0; $i<$roopnum; $i++ )
	{
		if( $$data[$i] =~ /$name/ )
		{
			return $i;
		}
	}

	print( "#getNameIndex $name not found\n" );
	exit(1);
}

##-----------------------------------------------------------------------------
# rail�f�[�^�@�p�����[�^�擾
##-----------------------------------------------------------------------------
sub getPeaceParam
{
	my( $data, $startidx, $key ) = @_;
	my( $i, $one );

	for( $i=$startidx; $i<@$data; $i++ )
	{
		$one = $$data[ $i ];
		$one =~ s/\"//g;
		$one =~ s/\r\n//g;
		$one =~ s/\n//g;

		#����+���o�Ă�����Peace�I�[�o�[
		if( $one =~ /\+/ )
		{
			print( "#getPeaceParam peace over key=$key startindex=$startidx\n" );
			exit(1);
		}
		
		if( $one =~ /--$key\:\:(.*)/ )
		{
			return $1;
		}
	}

	print( "#getPeaceParam $startidx , $key not found\n" );
	exit( 1 );
}

sub getPeaceParamForName
{
	my( $data, $name, $key ) = @_;
	my( $i, $one, $last_peace_index );

	$last_peace_index = 0;
	for( $i=0; $i<@$data; $i++ )
	{
		$one = $$data[ $i ];
		$one =~ s/\"//g;
		$one =~ s/\r\n//g;
		$one =~ s/\n//g;

		#����+���o�Ă�����last_peace_index�ۑ�
		if( $one =~ /\+/ )
		{
			$last_peace_index = $i;
		}
		
		if( $one =~ /--NAME\:\:$name$/ )
		{
			#last_peace_index����A$key�̏����擾����
			return &getPeaceParam( \@$data, $last_peace_index+1, $key );
		}
	}

	print( "#getPeaceParamForName $name not found\n" );
	exit( 1 );
}

sub getPeaceParamForTag
{
	my( $data, $tag, $key ) = @_;
	my( $i, $one );

	$last_peace_index = 0;
	for( $i=0; $i<@$data; $i++ )
	{
		$one = $$data[ $i ];
		$one =~ s/\"//g;
		$one =~ s/\r\n//g;
		$one =~ s/\n//g;

		#�^�O��������
		if( $one =~ /\+$tag/ )
		{
			#$tag����A$key�̏����擾����
			return &getPeaceParam( \@$data, $i+1, $key );
		}
	}

	print( "#getPeaceParamForTag $tag not found\n" );
	exit( 1 );

}


#�o�C�g���[�N�̒l���擾
sub getPeaceParamByteWorkNumberForIndex
{
	my( $data, $startidx, $index ) = @_;
	my( $i, $one, $index_count );

	$index_count = 0;
	for( $i=$startidx; $i<@$data; $i++ )
	{
		$one = $$data[ $i ];
		$one =~ s/\"//g;
		$one =~ s/\r\n//g;
		$one =~ s/\n//g;

		#����+���o�Ă�����Peace�I�[�o�[
		if( $one =~ /\+/ )
		{
			return $BYTEWORK_INDEX_OVER;
		}
		
		if( $one =~ /--[1-4]BYTE\:\:(.*)/ )
		{
			if($index_count == $index)
			{
				return $1;
			}
			$index_count ++;
		}
	}
	return $BYTEWORK_INDEX_OVER;
}

sub getPeaceParamByteWorkNumberForName
{
	my( $data, $name, $index ) = @_;
	my( $i, $one, $last_peace_index );

	$last_peace_index = 0;
	for( $i=0; $i<@$data; $i++ )
	{
		$one = $$data[ $i ];
		$one =~ s/\"//g;
		$one =~ s/\r\n//g;
		$one =~ s/\n//g;

		#����+���o�Ă�����Peace�I�[�o�[
		if( $one =~ /\+/ )
		{
			$last_peace_index = $i;
		}

		if( $one =~ /--NAME\:\:$name/ )
		{
			#last_peace_index����A$index�Ԗڂ�BYTE�p�����[�^���擾����
			return &getPeaceParamByteWorkNumberForIndex( \@$data, $last_peace_index+1, $index );
		}
		
	}

	print( "#getPeaceParamByteWorkNumberForName $name not found\n" );
	exit( 1 );
}



#�o�C�g���[�N��BYTE�T�C�Y���擾
sub getPeaceParamByteWorkSizeForIndex
{
	my( $data, $startidx, $index ) = @_;
	my( $i, $one, $index_count );

	$index_count = 0;
	for( $i=$startidx; $i<@$data; $i++ )
	{
		$one = $$data[ $i ];
		$one =~ s/\"//g;
		$one =~ s/\r\n//g;
		$one =~ s/\n//g;

		#����+���o�Ă�����Peace�I�[�o�[
		if( $one =~ /\+/ )
		{
			return $BYTEWORK_INDEX_OVER;
		}
		
		if( $one =~ /--([1-4])BYTE/ )
		{
			if($index_count == $index)
			{
				return $1;
			}
			$index_count ++;
		}
	}

	return $BYTEWORK_INDEX_OVER;
}

sub getPeaceParamByteWorkSizeForName
{
	my( $data, $name, $index ) = @_;
	my( $i, $one, $last_peace_index );

	$last_peace_index = 0;
	for( $i=0; $i<@$data; $i++ )
	{
		$one = $$data[ $i ];
		$one =~ s/\"//g;
		$one =~ s/\r\n//g;
		$one =~ s/\n//g;

		#����+���o�Ă�����Peace�I�[�o�[
		if( $one =~ /\+/ )
		{
			$last_peace_index = $i;
		}

		if( $one =~ /--NAME\:\:$name$/ )
		{
			#last_peace_index����A$index�Ԗڂ�BYTE�p�����[�^���擾����
			return &getPeaceParamByteWorkSizeForIndex( \@$data, $last_peace_index+1, $index );
		}
		
	}

	print( "#getPeaceParamByteWorkSizeForName $name not found\n" );
	exit( 1 );
}




##-----------------------------------------------------------------------------
#		�����N���m�F�@�|�C���g
##-----------------------------------------------------------------------------
sub isLinkPoint
{
	my( $data, $point ) = @_;
	my( $line, $key, $line_point_s, $line_point_e, $i );

	for( $i=0; $i<4; $i++ )
	{
		$line						= &getPeaceParamForName( \@$data, $point, "LINES0".$i );
		$key						= &getPeaceParamForName( \@$data, $point, "KEYS0".$i );
		if( $line =~ /RAIL_TBL_NULL/ )
		{
			if( !($key =~ /RAIL_KEY_NULL/) )
			{
				print( "point[$point] line-key error $line to $key\n" );
				exit(1);
			}
		}
		else
		{
			$line_point_s		= &getPeaceParamForName( \@$data, $line, "POINT_S" );
			$line_point_e		= &getPeaceParamForName( \@$data, $line, "POINT_E" );

			#�ڑ����C���Ɏ����̏�񂪂��邩�`�F�b�N
			if( !($line_point_s eq $point) && !($line_point_e eq $point) )
			{
				print( "point[$point] line link error $line �� $point �Ɛڑ����Ă�����͂���܂���B\n" );
				exit(1);
			}
			#�L�[������U���Ă��邩�`�F�b�N
			if( $key =~ /RAIL_KEY_NULL/ )
			{
				print( "point[$point] line-key error $line to $key\n" );
				exit(1);
			}
		}
	}
}


##-----------------------------------------------------------------------------
#		�����N���m�F�@���C��
##-----------------------------------------------------------------------------
sub isLinkLine
{
	my( $data, $line ) = @_;
	my( $point_s, $point_e, $i, $point_line, $link_ok_s, $link_ok_e );
	my( $camera_set );

	$point_s = &getPeaceParamForName( \@$data, $line, "POINT_S" );
	$point_e = &getPeaceParamForName( \@$data, $line, "POINT_E" );
	
	#���C���̊J�n�ƏI���̃|�C���g�Ɏ����̏�񂪂��邩�B
	$link_ok_s = 0;
	$link_ok_e = 0;
	for( $i=0; $i<4; $i++ )
	{
		$point_line	= &getPeaceParamForName( \@$data, $point_s, "LINES0".$i );
		if( $point_line eq $line )
		{
			$link_ok_s = 1;
		}

		$point_line	= &getPeaceParamForName( \@$data, $point_e, "LINES0".$i );
		if( $point_line eq $line )
		{
			$link_ok_e = 1;
		}
	}
	if(($link_ok_s != 1) || ($link_ok_e != 1))
	{
		print( "line[$line] linkerr $point_s or $point_e �� $line ������܂���B\n" );
#		exit(1);	#�����ł͂Ȃ�
	}

	#�I�t�Z�b�gangle�J�����@���T�C�h�|�C���g�̃J������FixAngleCamera�ł���K�v������
	$camera_set = &getPeaceParamForName( \@$data, $line, "CAMERA_SET" );
	$camera_set = &getPeaceParamForName( \@$data, $camera_set, "FUNCINDEX" );
	if( $camera_set =~ /FIELD_RAIL_LOADER_CAMERA_FUNC_OFSANGLE/ )
	{
		$camera_set = &getPeaceParamForName( \@$data, $point_s, "CAMERA_SET" );
		$camera_set = &getPeaceParamForName( \@$data, $camera_set, "FUNCINDEX" );
		if( !($camera_set =~ /FIELD_RAIL_LOADER_CAMERA_FUNC_FIXANGLE/) )
		{
			print( "line[$line] camera_set err $point_s �� FIELD_RAIL_LOADER_CAMERA_FUNC_FIXANGLE ���g�p���Ă��܂���B\n" );
#			exit(1);
		}
		$camera_set = &getPeaceParamForName( \@$data, $point_e, "CAMERA_SET" );
		$camera_set = &getPeaceParamForName( \@$data, $camera_set, "FUNCINDEX" );
		if( !($camera_set =~ /FIELD_RAIL_LOADER_CAMERA_FUNC_FIXANGLE/) )
		{
			print( "line[$line] camera_set err $point_s �� FIELD_RAIL_LOADER_CAMERA_FUNC_FIXANGLE ���g�p���Ă��܂���B\n" );
#			exit(1);
		}
	}
}






