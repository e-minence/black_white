
#=======================================================================================
#
#	�}���`�Z���A�j���[�V�����Ńf�t�H���g�̂S�t���[���ɂȂ��Ă���f�[�^��
#	�Z���A�j���[�V�����̍ő�l�ŏ㏑������
#
#=======================================================================================

	$KCODE = "Shift-JIS"

  require 'fileutils'

#�N���X��`

#�T�u���[�`��

#���C�����[�`��
	if ARGV.size < 2
		print "error: ruby frame_adj.rb read_file output_dir\n"
		print "read_file:�ǂݍ��ރt�@�C��\n"
		print "output_dir:�R���o�[�g�����t�@�C�����i�[����f�B���N�g��\n"
		error_action()
	end

	ARGV_READ_FILE = 0
	ARGV_OUT_DIR = 1

	TYPE_NMC	= 0
	TYPE_NCE	= 1

  FORMAT_ID		    =	4
  BYTE_ORDER		  =	2
  FORMAT_VERSION	=	2
  FILE_SIZE		    =	4
  HEADER_SIZE	    =	2
  DATA_BLOCKS	    =	2

  FILE_HEADER_SIZE     =	FORMAT_ID + BYTE_ORDER + FORMAT_VERSION + FILE_SIZE + HEADER_SIZE + DATA_BLOCKS

  BLOCK_TYPE  = 4
  BLOCK_SIZE  = 4

  BLOCK_HEADER_SIZE = BLOCK_TYPE + BLOCK_SIZE

	head = [ "NCMC", "NCOB" ]

	signature = [
		#NMC
		[ "MCEL", "CCTL", "GRP ", "ANIM" ],
		#NCE
		[ "CELL", "CNUM", "CHAR", "GRP ", "ANIM" ],
	]

	read_data = []
	data = []
	sig = []
  cell_anm_frame = []
  cell_anm_index = []

	nmc_file = ARGV[ ARGV_OUT_DIR ] + File::basename( ARGV[ ARGV_READ_FILE ] )
	nce_file = ARGV[ ARGV_OUT_DIR ] + File::basename( ARGV[ ARGV_READ_FILE ], ".nmc" ) + ".nce"

	begin
		fp_nmc = open( nmc_file )
		fp_nce = open( nce_file )
	rescue
		print "FileOpenError:" + nmc_file + " or " + nce_file + "\n"
		exit( 1 )
	end

	fp_nmc.binmode
	fp_nce.binmode

	w_file = ARGV[ ARGV_OUT_DIR ] +	File::basename( ARGV[ ARGV_READ_FILE ], ".nmc" ) + ".nmca"
	fp_w = open( w_file, "wb" )

  #nce�ǂݍ���
	read_data = fp_nce.read( FILE_HEADER_SIZE )
	format_id = read_data.unpack( "a4" )
  if format_id[ 0 ] != head[ TYPE_NCE ]
    p "NitroCharacter��nce�t�@�C���ł͂���܂���"
		exit( 0 )
  end

  #ANIM�V�O�l�`���܂œǂݔ�΂�
  signature[ TYPE_NCE ].size.times{|i|
	  read_data = fp_nce.read( BLOCK_HEADER_SIZE )
	  block_type, block_size = read_data.unpack( "a4l" )
    if block_type != signature[ TYPE_NCE ][ i ]
      if signature[ TYPE_NCE ][ i ] == "CHAR"
				fp_nce.seek( -8, IO::SEEK_CUR )
				next		
			end
      p signature[ TYPE_NCE ][ i ] + "��������܂���"
      exit( 1 )
    end
    if signature[ TYPE_NCE ][ i ] != "ANIM"
		  fp_nce.seek( block_size - 8, IO::SEEK_CUR )
    end
  }

  #�Z���A�j���t���[�����v�Z
  read_data = fp_nce.read( 4 )
  cell_anims = read_data.unpack( "L" )
  cell_anims[ 0 ].times{|i|
    read_data = fp_nce.read( 12 )
    label_index, cmnt_index, cells = read_data.unpack( "LLL" )
    cell_anm_frame[ label_index ] = 0
    cells.times{|j|
      read_data = fp_nce.read( 24 )
      cell_index, wait_frame, rot, scale_x, scale_y, trans_x, trans_y = read_data.unpack( "SSlllll" )
      cell_anm_frame[ label_index ] += wait_frame
    }
  }
  
  #nmc�ǂݍ���
	read_data = fp_nmc.read( FILE_HEADER_SIZE )
	format_id = read_data.unpack( "a4" )
  if format_id[ 0 ] != head[ TYPE_NMC ]
    p "NitroCharacter��nmc�t�@�C���ł͂���܂���"
		exit( 0 )
  end
  fp_w.write( read_data )

  #ANIM�V�O�l�`���܂œǂݔ�΂�
  signature[ TYPE_NMC ].size.times{|i|
	  read_data = fp_nmc.read( BLOCK_HEADER_SIZE )
	  block_type, block_size = read_data.unpack( "a4l" )
    fp_w.write( read_data )
    if block_type != signature[ TYPE_NMC ][ i ]
      p signature[ TYPE_NMC ][ i ] + "��������܂���"
      exit( 1 )
    end
    if signature[ TYPE_NMC ][ i ] == "MCEL"
      read_data = fp_nmc.read( 4 )
      mcells = read_data.unpack( "L" )
      fp_w.write( read_data )
      mcells[ 0 ].times{|i|
        read_data = fp_nmc.read( 4 )
        cell_anims = read_data.unpack( "L" )
        fp_w.write( read_data )
        cell_anm_index << []
        cell_anims[ 0 ].times{|j|
          read_data = fp_nmc.read( 12 )
          pos_x, pos_y, label_index = read_data.unpack( "llL" )
          fp_w.write( read_data )
          cell_anm_index[ i ] << label_index
        }
      }
    elsif signature[ TYPE_NMC ][ i ] != "ANIM"
	    read_data = fp_nmc.read( block_size - 8 )
      fp_w.write( read_data )
    end
  }

  #�}���`�Z���A�j���t���[�����Z�b�g
  read_data = fp_nmc.read( 4 )
  mcell_anims = read_data.unpack( "L" )
  fp_w.write( read_data )
  mcell_anims[ 0 ].times{|i|
    read_data = fp_nmc.read( 12 )
    label_index, cmnt_index, mcells = read_data.unpack( "LLL" )
    fp_w.write( read_data )
    mcells.times{|j|
      read_data = fp_nmc.read( 24 )
      cell_index, wait_frame, rot, scale_x, scale_y, trans_x, trans_y = read_data.unpack( "SSlllll" )
      if wait_frame == 4
        cell_anm_index[ cell_index ].size.times{|k|
          if wait_frame < cell_anm_frame[ cell_anm_index[ cell_index ][ k ] ]
            wait_frame = cell_anm_frame[ cell_anm_index[ cell_index ][ k ] ]
          end
        }
	      read_data = [ cell_index, wait_frame, rot, scale_x, scale_y, trans_x, trans_y ].pack( "SSlllll" )
      end
      fp_w.write( read_data )
    }
  }

  read_data = fp_nmc.read
  fp_w.write( read_data )

	fp_nmc.close
	fp_nce.close
  fp_w.close

  FileUtils.cp( w_file, nmc_file )
  FileUtils.rm( w_file )
