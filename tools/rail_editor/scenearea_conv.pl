##-----------------------------------------------------------------------------
#	.sa�t�@�C����Binary���ɃR���o�[�g
#
#
#	scenearea_conv.pl	.sa fld_scenearea_loader_func.h fld_scenearea.h
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
$SA_WORK_SIZ;

#�֐�ID
$SA_FUNCID_NULL;

#�֐�ID
@SA_AREACHECK_FUNC_ID = undef;
@SA_UPDATE_FUNC_ID = undef;


#���[�����
@SA_NAME = undef;

#BYTE���[�N��index�I�[�o�[�萔
$BYTEWORK_INDEX_OVER	= 0xffff;

##-----------------------------------------------------------------------------
#	���C������
##-----------------------------------------------------------------------------
#�����`�F�b�N
if( @ARGV < 3 )
{
	print( "scenearea_conv.pl	.sa fld_scenearea_loader_func.h fld_scenearea.h \n" );
	exit(1);
}



#���[�_�[�̊֐�ID�̓ǂݍ���
&LoadSaFuncID( $ARGV[ 1 ] );

#���[���V�X�e���萔�̓ǂݍ���
&loadSaSysData( $ARGV[ 2 ] );

#�R���o�[�g
&convData( $ARGV[ 0 ] );

exit(0);


##-----------------------------------------------------------------------------
#	���[�_�[�֐�ID�̓ǂݍ���
##-----------------------------------------------------------------------------
sub LoadSaFuncID
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
			if( $one =~ /AREACHECK_FUNC_ID_START/ )
			{
				$mode = 0;
				$index = 0;
				$input = 1;
			}
			elsif( $one =~ /UPDATE_FUNC_ID_START/ )
			{
				$mode = 1;
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
					$SA_AREACHECK_FUNC_ID[ $index ] = $1;
					$index ++;
				}
			}
			elsif( $mode == 1 )
			{
				if( $one =~ /([^,]*),/ )
				{
					$SA_UPDATE_FUNC_ID[ $index ] = $1;
					$index ++;
				}
			}
		}
	}

}

##-----------------------------------------------------------------------------
#	�֐�ID�萔���@->	�o�C�i��
##-----------------------------------------------------------------------------
sub getCheckAreaFuncIDNameToNo
{
	my( $name ) = @_;
	my( $i );

	for( $i=0; $i<@SA_AREACHECK_FUNC_ID; $i++ )
	{
		if( $SA_AREACHECK_FUNC_ID[$i] =~ /$name/ )
		{
			return $i;
		}
	}

	print( "#getCheckAreaFuncIDNameToNosarch $name not found\n" );
	exit(1);
}

sub getUpdateFuncIDNameToNo
{
	my( $name ) = @_;
	my( $i );

	for( $i=0; $i<@SA_UPDATE_FUNC_ID; $i++ )
	{
		if( $SA_UPDATE_FUNC_ID[$i] =~ /$name/ )
		{
			return $i;
		}
	}

	print( "#getUpdateFuncIDNameToNo sarch $name not found\n" );
	exit(1);
}


##-----------------------------------------------------------------------------
#	���[���V�X�e���̒萔�ǂݍ���
##-----------------------------------------------------------------------------
sub loadSaSysData
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

		if( $one =~ /FLD_SCENEAREA_SCENEPARAM_SIZE\(([0-9]*)\)/ )
		{
			$SA_WORK_SIZ = $1;
			print( "SA_WORK_SIZ load = $SA_WORK_SIZ\n" );
			$input_count ++;
		}

		if( $one =~ /FLD_SCENEAREA_FUNC_NULL\(([^)]*)\)/ )
		{
			$SA_FUNCID_NULL = $1;
			print( "SA_FUNCID_NULL load = $SA_FUNCID_NULL\n" );
			$input_count ++;
		}
	}

	#�ǂݍ��݉񐔂𒲂ׂ�
	if( $input_count != 2 )
	{
		print( "#loadSaSysData load data not 2\n" );
		exit(1);
	}
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
	my( $i, $j, $one, @filedata, $outfile, $name_index );

	#���ǂݍ���
	open( FILEIN, $filename );
	@filedata = <FILEIN>;
	close( FILEIN );
	
	#�܂��A���O���X�g���쐬
	$name_index = 0;
	for( $i=0; $i<(@filedata-1); $i++ )
	{
		$filedata[$i] =~ s/\t//g;
		$filedata[$i] =~ s/\s//g;
		$one	= $filedata[$i];
		
		if( $one =~ /\+SA$/ )
		{
			$SA_NAME[ $name_index ] = &getPeaceParam( \@filedata, $i+1, "NAME" );
			$name_index ++;
		}
	}
	$filedata[$i] =~ s/\t//g;
	$filedata[$i] =~ s/\s//g;

	
	#���𐔎��ɒu�����āA�o��
	$outfile = $filename;
	$outfile =~ s/\.sa//;

	#�I�t�Z�b�g���
	open( FILEOUT, ">$outfile.dat" );
	binmode( FILEOUT );

	for( $i=0; $i<@SA_NAME; $i++ )
	{
		print( $SA_NAME[$i]."\n" );
		#�f�[�^
		{
			my( $work_index, $work_number, $one_datasiz, $datasiz );
			$work_index		= 0;
			$datasiz			= 0;

			do
			{
				$work_number = &getPeaceParamByteWorkNumberForName( \@filedata, $SA_NAME[$i], $work_index );
				$one_datasiz = &getPeaceParamByteWorkSizeForName( \@filedata, $SA_NAME[$i], $work_index );

				if( ($work_number != $BYTEWORK_INDEX_OVER) && ($one_datasiz != $BYTEWORK_INDEX_OVER) )
				{
					#�o��
					if( $one_datasiz == 1 )
					{
						print( FILEOUT pack( "c", &get10Number($work_number) ) );
					}
					elsif( $one_datasiz == 2 )
					{
						print( FILEOUT pack( "s", &get10Number($work_number) ) );
					}
					elsif( $one_datasiz == 4 )
					{
						print( FILEOUT pack( "i", &get10Number($work_number) ) );
					}
					$datasiz += $one_datasiz;

					#�T�C�Y�I�[�o�[�`�F�b�N
					if( $datasiz > $SA_WORK_SIZ )
					{
						print( "camera_work size over  name=".$SA_NAME[$i]."\n" );
						exit( 1 );
					}
					$work_index ++;
				}
			}while( ($work_number != $BYTEWORK_INDEX_OVER) && ($one_datasiz != $BYTEWORK_INDEX_OVER) );

			#����Ȃ����[�N���o��
			while( $datasiz < $SA_WORK_SIZ )
			{
				print( FILEOUT pack( "c", 0 ) );
				$datasiz ++;
			}
		}

		#�e�֐�ID
		{
			my( $func_id );
			$func_id = &getPeaceParamForName( \@filedata, $SA_NAME[$i], "AREACHECK" );
			print( FILEOUT pack( "S", &getNameIndex( \@SA_AREACHECK_FUNC_ID, $func_id ) ) );

			$func_id = &getPeaceParamForName( \@filedata, $SA_NAME[$i], "UPDATE" );
			print( FILEOUT pack( "S", &getNameIndex( \@SA_UPDATE_FUNC_ID, $func_id ) ) );

			$func_id = &getPeaceParamForName( \@filedata, $SA_NAME[$i], "INSIDE" );
			print( FILEOUT pack( "S", &getNameIndex( \@SA_UPDATE_FUNC_ID, $func_id ) ) );

			$func_id = &getPeaceParamForName( \@filedata, $SA_NAME[$i], "OUTSIDE" );
			print( FILEOUT pack( "S", &getNameIndex( \@SA_UPDATE_FUNC_ID, $func_id ) ) );
		}
	}
	
	close( FILEOUT );
}

##-----------------------------------------------------------------------------
# ���O��v�C���f�b�N�X�̎擾
##-----------------------------------------------------------------------------
sub getNameIndex
{
	my( $data, $name ) = @_;
	my( $i, $roopnum );

	#NULL�`�F�b�N
	if( $name =~ /FLD_SCENEAREA_FUNC_NULL/ )
	{
		return &get10Number($SA_FUNCID_NULL);
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
# sa�f�[�^�@�p�����[�^�擾
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
			print( "#getPeaceParam peace over key=$key startindex=$startindex\n" );
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
		
		if( $one =~ /--NAME\:\:$name/ )
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

		if( $one =~ /--NAME\:\:$name/ )
		{
			#last_peace_index����A$index�Ԗڂ�BYTE�p�����[�^���擾����
			return &getPeaceParamByteWorkSizeForIndex( \@$data, $last_peace_index+1, $index );
		}
		
	}

	print( "#getPeaceParamByteWorkSizeForName $name not found\n" );
	exit( 1 );
}







