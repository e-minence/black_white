#====================================================================================
#
#	�A�[�J�C�u�e�[�u���@�����R���o�[�^
#
#====================================================================================

use Switch;

use constant ARC_LIST	=> 0;
use constant VERSION	=> 1;
use constant DEBUG	  => 2;
use constant SRC_DIR	=> 3;
use constant DEST_DIR	=> 4;
use constant MB_FLAG	=> 5;

	$argc = @ARGV;

	if( $argc < 6 ){
		print "error:perl perl_file read_file pm_version pm_debug src_dir dest_dir\n";
		print " read_file:\n  �ǂݍ��ރ��X�g�t�@�C���i�ʏ�́Aarc_tool.lst�j\n";
		print " pm_version:\n  �o�[�W�����t���O�i�|�P�����ȂǂłQ�o�[�W�������鎞�ɍ쐬�o�[�W�������w��j\n";
		print " pm_debug:\n  �f�o�b�O�t���O�i�f�o�b�O�łƐ��i�łŊ܂߂���̂�ς��������p�Ɏw��j\n";
		print " src_dir:\n  �R�s�[���t�@�C���̑��݂���t�H���_���w��\n";
		print " dest_dir:\n  �Z�k�t�@�C�����̃R�s�[��t�H���_���w��\n";
		print " multi_boot_flag:\n  yes���w�肷�邱�ƂŁA�}���`�u�[�g�q�@�Ή��ɂȂ�\n";
		exit(1);
	}

	open( ARC_F, @ARGV[ARC_LIST] ) or die "[@ARGV[ARC_LIST]]", $!;
	@data = <ARC_F>;
	close( ARC_F );

	open( ARC_TREE, "> file_tree.inc");
	
	print ARC_TREE "GAME_FILE_TREE=";

	$data_num	= @data;
	$num_1		= 0;
	$num_10		= 0;
	$num_100	= 0;
	$count		= 0;
	$write_count= 0;

#�}���`�u�[�g�Ή��e�[�u���쐬
	if( @ARGV[MB_FLAG] =~ m/^yes/i ){
	}
	else{
		&MakeDir;
	}

	while($data_num != 0){
		@arc_data		=	split(/ +|\t|\r|\n/,@data[$count]);
		$arc_data_num	=	@arc_data;
		for( $i = 0 ; $i < $arc_data_num ; $i++ ){
			my $flag = 0;
			if( length @arc_data[$i] ){
				switch(substr(@arc_data[$i],0,1)){
					case '$' {
						$version = @arc_data[$i];
						$version =~ s/\$//g;
            if( $version eq "DEBUG_YES" && @ARGV[DEBUG] eq "yes" )
            {
  						&FileWrite($i+1);
            }
            elsif( $version eq "DEBUG_NO" && @ARGV[DEBUG] eq "no" )
            {
  						&FileWrite($i+1);
            }
            else
            {
  						$find = index( $version, @ARGV[VERSION] );
  						if( $find >= 0 && length $version == length @ARGV[VERSION] ){
  							&FileWrite($i+1);
  						}
            }
  					$flag = 1;
					}
					case '#' {
						$flag = 1;
					}
					else     {
						&FileWrite($i);
						$flag = 1;
					}
				}
				if( $flag == 1 ){
					last;
				}
			}
		}
		$count++;
		$data_num--;
	}

	close( ARC_TREE );

#===========================================================
#
#	�t�@�C�������o���T�u���[�`��
#
#	�����F@_[0]�@���x�����i�[���ꂽ�z��̓Y����
#
#===========================================================
sub FileWrite{
	my $index = @_[0];
	my $enum = @arc_data[$index];
	my $tree = "a/$num_100/$num_10/$num_1";
	my $file_num = $index + 1;
	while( length @arc_data[$file_num] == 0 ){
		$file_num++;
		if( $file_num >= $arc_data_num ){
			print "Error:Not Found FileName\n";
			exit(1);
		}
	}
	my $file = @arc_data[$file_num];
	$file =~ s/"//g;
	if( substr( $file, 0, 1 ) eq '#' ){
		print "Error:Not Found FileName $file\n";
		exit(1);
	}

	if( $write_count > 0 ){
		print ARC_TREE	" \\\n";
	}

#�}���`�u�[�g�Ή��e�[�u���쐬
	if( @ARGV[MB_FLAG] =~ m/^yes/i ){
		print ARC_TREE	"\t$file";
	}
	else{
		print ARC_TREE	"\t$tree";
	}

	$num_1++;
	my $flag = FALSE;
	if( $num_1 > 9 ){
		$num_1 = 0;
		$num_10++;
		$flag = TRUE;
		if( $num_10 > 9 ){
			$num_10 = 0;
			$num_100 ++;
			if( $num_100 > 9 ){
				print "Error:File Max Over\n";
				exit(1);
			}
		}
	}
#�}���`�u�[�g�Ή��e�[�u���쐬
	if( @ARGV[MB_FLAG] =~ m/^yes/i ){
	}
	else{
		if( $flag == TRUE ){
			&MakeDir;
		}
		my $src_file = @ARGV[SRC_DIR];
		$src_file .= $file;
		my $dst_file = @ARGV[DEST_DIR];
		$dst_file .= $tree;
		my @cmd = ( 'cp', $src_file, $dst_file );

		system @cmd;
		if( $? >> 8 != 0 ){
			exit(1);
		}
	}

	$write_count++;
}

#===========================================================
#
#	�f�B���N�g���쐬�T�u���[�`��
#
#===========================================================
sub MakeDir{
	my $dir = "@ARGV[DEST_DIR]a";
	if( -d $dir){}
	else{
		mkdir ( $dir, 0777) || die "mkdir failed : $!";
	}
	$dir .= "/0";
	if( -d $dir){}
	else{
		mkdir ( $dir, 0777) || die "mkdir failed : $!";
	}
	$dir .= "/0";
	if( -d $dir){}
	else{
		mkdir ( $dir, 0777) || die "mkdir failed : $!";
	}
	$dir = "@ARGV[DEST_DIR]a/$num_100";
	if( -d $dir){}
	else{
		mkdir ( $dir, 0777) || die "mkdir failed : $!";
	}
	$dir .= "/$num_10";
	if( -d $dir){}
	else{
		mkdir ( $dir, 0777) || die "mkdir failed : $!";
	}
}

