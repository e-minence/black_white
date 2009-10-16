#! ruby -Ks

  ext = [ ".nce", ".ncg", ".ncl", ".nmc" ]

	if ARGV.size < 2
		print "error: ruby file_exist.rb list_file copy_dir\n"
		print "list_file:�`�F�b�N�p���X�g�t�@�C��\n"
		print "copy_dir:���f�ނ̃R�s�[��\n"
		exit( 1 )
	end

  ARGV_LIST_FILE = 0
  ARGV_COPY_DIR = 1

  read_data = []
  open( ARGV[ ARGV_LIST_FILE ] ) {|fp_r|
    while str = fp_r.gets
      read_data << str.tr( "\"\r\n", "" )
    end
  }

  exist_flag = 0

  read_data.size.times {|i|
    split_data = read_data[ i ].split(/,/)

    file_name = ARGV[ ARGV_COPY_DIR ] + split_data[ 0 ] + ".ncg" 

    if File.exist?( file_name ) == FALSE
      exist_flag = 1
      if split_data[ 1 ] == "PTL_SEX_MALE"
        gender = "trwb_male"
      else
        gender = "trwb_female"
      end
      ext.size.times {|j|
        p ext[ j ]
        if ext[ j ] == ".ncl"
          com = sprintf( "cp %s%s %s%s%s", gender, ext[ j ], ARGV[ ARGV_COPY_DIR ], split_data[ 0 ], ext[ j ] )
          p com
          system( com )
        else
          fp_r = open( gender + ext[ j ] )
          fp_w = open( ARGV[ ARGV_COPY_DIR ] + split_data[ 0 ] + ext[ j ], "wb" )
          link_name = ""
          case ext[ j ]
          when ".nce"
            link_name = split_data[ 0 ] + ".ncg"
          when ".ncg"
            link_name = split_data[ 0 ] + ".ncl"
          when ".nmc"
            link_name = split_data[ 0 ] + ".nce"
          end
  
          size = 8 + link_name.size
          padding = 4 - ( size % 4 )
          size += padding
  
          data = fp_r.read
          fp_w.write( data )
  
          data = [ "LINK", size, link_name].pack( "a* l a*" )
  
          padding.times{
            data << 0
          }
  
          data.size.times{ |c|
            fp_w.printf( "%c", data[ c ] )
          }
  
          if ext[ j ] == ".ncg"
            data = [ "CMNT", 12, 0 ].pack( "a* l l" )
            data.size.times{ |c|
              fp_w.printf( "%c", data[ c ] )
            }
          end
          fp_r.close
          fp_w.close
        end
        com = sprintf( "svn add %s%s%s", ARGV[ ARGV_COPY_DIR ], split_data[ 0 ], ext[ j ] )
        system( com )
        com = sprintf( "svn commit %s%s%s -m \"���f�ޒǉ�\"", ARGV[ ARGV_COPY_DIR ], split_data[ 0 ], ext[ j ] )
        system( com )
      }
    end
  }
  if exist_flag != 0
    system( "svn commit trfgra_wb.scr -m \"���f�ޒǉ�\"" )
    system( "ipmsg /MSG /SEAL W00205 �g���[�i�[�O���t�B�b�N���f�ނ��R�~�b�g���܂����@�R���o�[�g���肢���܂�" )
    system( "ipmsg /MSG localhost �g���[�i�[�O���t�B�b�N���f�ނ�����܂��@�呺�N����̕ԓ���҂��Ă�������" )
  end
