#! ruby -Ks

#=======================================================================================
#
#	�^�C���X�^���v��r���ĕʂ�ruby�����s
#
#=======================================================================================

#�N���X��`

#�T�u���[�`��

#���C�����[�`��
	if ARGV.size < 3
		print "error: ruby tschk.rb list_file res_dir copy_dir\n"
		print "list_file:�^�C���X�^���v�`�F�b�N����t�@�C�����X�g\n"
		print "res_dir:�^�C���X�^���v�`�F�b�N����t�@�C�������݂���f�B���N�g��\n"
		print "copy_dir:�R�s�[��f�B���N�g��\n"
		exit( 1 )
	end

  ARGV_LIST_FILE  = 0
  ARGV_RES_DIR    = 1
  ARGV_COPY_DIR   = 2

  begin
    fp_list = open( ARGV[ ARGV_LIST_FILE ] )
  rescue
    printf( "FileOpenError:%s\n", ARGV[ ARGV_LIST_FILE ] )
    exit( 1 )
  end

  read_data = []
  while str = fp_list.gets
    read_data << ARGV[ ARGV_RES_DIR ] + str.tr( "\r\n", "" )
  end
  fp_list.close

  read_data.size.times {|i|
    if File.exist?( read_data[ i ] ) == TRUE
      com = "ruby " +  File::dirname(__FILE__) + "/nclinkrw.rb " + read_data[ i ] + " " + ARGV[ ARGV_COPY_DIR ]
      read_file = ARGV[ ARGV_COPY_DIR ] + File::basename( read_data[ i ] )
      if File.exist?( read_file ) == FALSE
        printf("copy %s\n", File::basename( read_data[ i ] ) )
        system( com )
      elsif File::stat( read_file ).mtime < File::stat( read_data[ i ] ).mtime
        printf("copy %s\n", File::basename( read_data[ i ] ) )
        system( com )
      end
    end
    if File::extname( read_data[ i ] ) == ".nmc"
      com = "ruby " +  File::dirname(__FILE__) + "/frame_adj.rb " + read_data[ i ] + " " + ARGV[ ARGV_COPY_DIR ]
      system( com )
    end
  }

