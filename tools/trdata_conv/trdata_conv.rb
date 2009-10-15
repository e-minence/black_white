#! ruby -Ks

  require File.dirname(__FILE__) + '/../label_make/label_make'
  require File.dirname(__FILE__) + '/../gmm_make/gmm_make'
  require File.dirname(__FILE__) + '/../constant'
  require 'date'

=begin
	u8			data_type;        //�f�[�^�^�C�v
	u8			tr_type;					//�g���[�i�[����
  u8      fight_type;       //1vs1or2vs2 
	u8			poke_count;       //�����|�P������

	u16			use_item[4];      //�g�p����

	u32			aibit;						//AI�p�^�[��

//�g���[�i�[�����|�P�����p�����[�^�i�f�[�^�^�C�v�m�[�}���j
	u16		pow;			//�Z�b�g����p���[����(u8��OK������4�o�C�g���E�΍�j
	u16		level;			//�Z�b�g����|�P�����̃��x��
	u16		monsno;			//�Z�b�g����|�P����

//�g���[�i�[�����|�P�����p�����[�^�i�f�[�^�^�C�v�Z�j
	u16		pow;			//�Z�b�g����p���[����(u8��OK������4�o�C�g���E�΍�j
	u16		level;			//�Z�b�g����|�P�����̃��x��
	u16		monsno;			//�Z�b�g����|�P����
	u16		waza[4];		//�����Ă�Z

//�g���[�i�[�����|�P�����p�����[�^�i�f�[�^�^�C�v�A�C�e���j
	u16		pow;			//�Z�b�g����p���[����(u8��OK������4�o�C�g���E�΍�j
	u16		level;			//�Z�b�g����|�P�����̃��x��
	u16		monsno;			//�Z�b�g����|�P����
	u16		itemno;			//�Z�b�g����A�C�e��

//�g���[�i�[�����|�P�����p�����[�^�i�f�[�^�^�C�v�}���`�j
	u16		pow;			//�Z�b�g����p���[����(u8��OK������4�o�C�g���E�΍�j
	u16		level;			//�Z�b�g����|�P�����̃��x��
	u16		monsno;			//�Z�b�g����|�P����
	u16		itemno;			//�Z�b�g����A�C�e��
	u16		waza[ PTL_WAZA_MAX ];		//�����Ă�Z
=end

class PARA
  enum_const_set %w[
    FIGHT_TYPE
    DATA_TYPE
    GOLD
    GENDER
    TR_ID
    TR_TYPE
    TR_NAME
    POKE1_POW
    POKE1_LV
    POKE1_GENDER
    POKE1_FORM
    POKE2_POW
    POKE2_LV
    POKE2_GENDER
    POKE2_FORM
    POKE3_POW
    POKE3_LV
    POKE3_GENDER
    POKE3_FORM
    POKE4_POW
    POKE4_LV
    POKE4_GENDER
    POKE4_FORM
    POKE5_POW
    POKE5_LV
    POKE5_GENDER
    POKE5_FORM
    POKE6_POW
    POKE6_LV
    POKE6_GENDER
    POKE6_FORM
    AI
    POKE1_ITEM
    POKE2_ITEM
    POKE3_ITEM
    POKE4_ITEM
    POKE5_ITEM
    POKE6_ITEM
    USE_ITEM1
    USE_ITEM2
    USE_ITEM3
    USE_ITEM4
    POKE1_WAZA1
    POKE1_WAZA2
    POKE1_WAZA3
    POKE1_WAZA4
    POKE2_WAZA1
    POKE2_WAZA2
    POKE2_WAZA3
    POKE2_WAZA4
    POKE3_WAZA1
    POKE3_WAZA2
    POKE3_WAZA3
    POKE3_WAZA4
    POKE4_WAZA1
    POKE4_WAZA2
    POKE4_WAZA3
    POKE4_WAZA4
    POKE5_WAZA1
    POKE5_WAZA2
    POKE5_WAZA3
    POKE5_WAZA4
    POKE6_WAZA1
    POKE6_WAZA2
    POKE6_WAZA3
    POKE6_WAZA4
    POKE1_NAME
    POKE2_NAME
    POKE3_NAME
    POKE4_NAME
    POKE5_NAME
    POKE6_NAME
    MAX
  ]
end

  fight_type = {  
	  "�P�΂P"=>"0",
	  "�Q�΂Q"=>"1",
  }

  data_type = { 
		"�m�[�}��"  =>"DATATYPE_NORMAL",
		"�Z"        =>"DATATYPE_WAZA",
		"����"      =>"DATATYPE_ITEM",
		"�}���`"    =>"DATATYPE_MULTI",
  }

  gender = {  
    "��"=>"PTL_SEX_MALE",
    "��"=>"PTL_SEX_FEMALE",
    "�|"=>"PTL_SEX_UNKNOWN",
  }

	if ARGV.size < 2
		print "error: ruby trdata_conv.rb read_file gmm_file\n"
		print "read_file:�ǂݍ��ރt�@�C��\n"
		print "gmm_file:�g���[�i�[����gmm�t�@�C���������o�����߂̌��ɂȂ�t�@�C��\n"
		exit( 1 )
	end

  ARGV_READ_FILE = 0
  ARGV_READ_GMM_FILE = 1

  label = LabelMake.new
  read_data = []
  cnt = 0
  open( ARGV[ ARGV_READ_FILE ] ) {|fp_r|
    while str = fp_r.gets
      read_data[ cnt ] = str
      read_data[ cnt ] = read_data[ cnt ].tr( "\"\r\n", "" )
      cnt += 1
    end
  }

	#�g���[�i�[�O���t�B�b�Nlst�t�@�C������
	fp_trgra = open( "trfgra_wb.lst", "w" )

	#�g���[�i�[�O���t�B�b�Nscr�t�@�C������
	fp_trfgra = open( "trfgra_wb.scr", "w" )
  fp_trfgra.print "\"trwb_hero.NCBR\"\n"
  fp_trfgra.print "\"trwb_hero.NCER\"\n"
  fp_trfgra.print "\"trwb_hero.NANR\"\n"
  fp_trfgra.print "\"trwb_hero.NMCR\"\n"
  fp_trfgra.print "\"trwb_hero.NMAR\"\n"
  fp_trfgra.print "\"trwb_hero.NCEC\"\n"
  fp_trfgra.print "\"trwb_hero.NCLR\"\n"
  fp_trfgra.print "\"trwb_heroine.NCBR\"\n"
  fp_trfgra.print "\"trwb_heroine.NCER\"\n"
  fp_trfgra.print "\"trwb_heroine.NANR\"\n"
  fp_trfgra.print "\"trwb_heroine.NMCR\"\n"
  fp_trfgra.print "\"trwb_heroine.NMAR\"\n"
  fp_trfgra.print "\"trwb_heroine.NCEC\"\n"
  fp_trfgra.print "\"trwb_heroine.NCLR\"\n"

	#�g���[�i�[�f�[�^�t�@�C������
	fp_trdata = open( "trdata_000.s", "w" )

	fp_trdata.print "	.text\n"
	fp_trdata.print "\n"
	fp_trdata.print "	.include trdata.h\n"
	fp_trdata.print "\n"
	fp_trdata.print "	.byte   0			//�f�[�^�^�C�v\n"
	fp_trdata.print "	.byte   0			//�g���[�i�[�^�C�v\n"
	fp_trdata.print "	.byte   0			//�퓬�^�C�v\n"
	fp_trdata.print "	.byte   0			//�����|�P������\n"
	fp_trdata.print "	.short	0			//�����A�C�e���P\n"
	fp_trdata.print "	.short	0			//�����A�C�e���Q\n"
	fp_trdata.print "	.short	0			//�����A�C�e���R\n"
	fp_trdata.print "	.short	0			//�����A�C�e���S\n"
	fp_trdata.print "	.long   0			//AI\n"

	fp_trdata.close

	#�g���[�i�[�����|�P�����f�[�^�t�@�C������
	fp_trpoke = open( "trpoke_000.s", "w" )

	fp_trpoke.print "	.text\n"
	fp_trpoke.print "\n"
	fp_trpoke.print "	.include trdata.h\n"
	fp_trpoke.print "\n"
	fp_trpoke.print "	.short	0				//�|�P�����P�p���[����\n"
	fp_trpoke.print "	.short	0				//�|�P�����P���x��\n"
	fp_trpoke.print "	.short	0				//�|�P�����P\n"

	fp_trpoke.close

	#�g���[�i�[ID��`�t�@�C������
	fp_trno = open( "trno_def.h", "w" )

  fp_trno.print( "//============================================================================================\n" )
  fp_trno.print( "/**\n" )
  fp_trno.print( " * @file	trno_def.h\n" )
  fp_trno.print( " * @bfief	�g���[�i�[ID��`�t�@�C��\n" )
  fp_trno.print( " * @author	TrainerDataConverter\n" )
  fp_trno.printf( " * @date\t%s\n", Date::today.to_s )
	fp_trno.print( " * @author	���̃t�@�C���́A�R���o�[�^���f���o�����t�@�C���ł�\n" )
  fp_trno.print( "*/\n")
  fp_trno.print( "//============================================================================================\n" )
  fp_trno.print( "\n#ifndef __TRNO_DEF_H_\n" )
  fp_trno.print( "#define __TRNO_DEF_H_\n\n" )
	fp_trno.print("#define	TRID_NULL   ( 0 )\n")

	#�g���[�i�[�^�C�v��`�t�@�C������
	fp_trtype = open( "trtype_def.h", "w" )

  fp_trtype.print( "//============================================================================================\n" )
  fp_trtype.print( "/**\n" )
  fp_trtype.print( " * @file	trtype_def.h\n" )
  fp_trtype.print( " * @bfief	�g���[�i�[�^�C�v��`\n");
  fp_trtype.print( " * @author	TrainerDataConverter\n" )
  fp_trtype.printf( " * @date\t%s\n", Date::today.to_s )
	fp_trtype.print( " * @author	���̃t�@�C���́A�R���o�[�^���f���o�����t�@�C���ł�\n" )
  fp_trtype.print( "*/\n")
  fp_trtype.print( "//============================================================================================\n" )
  fp_trtype.print( "\n#pragma once\n\n" )
	fp_trtype.print("\n")
	fp_trtype.print("#define	TRTYPE_HERO	    ( 0 )\n")
	fp_trtype.print("#define	TRTYPE_HEROINE	( 1 )\n")

	#�g���[�i�[���ʒ�`�t�@�C������
	fp_trtypesex = open( "trtype_sex.h", "w" )

  fp_trtypesex.print( "//============================================================================================\n" )
  fp_trtypesex.print( "/**\n" )
  fp_trtypesex.print( " * @file	trtype_sex.h\n" )
	fp_trtypesex.print( " * @bfief	�g���[�i�[���ʒ�`\n")
  fp_trtypesex.print( " * @author	TrainerDataConverter\n" )
  fp_trtypesex.printf( " * @date\t%s\n", Date::today.to_s )
	fp_trtypesex.print( " * @author	���̃t�@�C���́A�R���o�[�^���f���o�����t�@�C���ł�\n" )
  fp_trtypesex.print( "*/\n")
  fp_trtypesex.print( "//============================================================================================\n" )
  fp_trtypesex.print( "\n#pragma once\n\n" )
	fp_trtypesex.print( "\n" )
	fp_trtypesex.print( "static	const	u8	TrTypeSexTable[]={\n" )
	fp_trtypesex.print( "\tPTL_SEX_MALE,		//�j��l��\n" )
	fp_trtypesex.print( "\tPTL_SEX_FEMALE,	//����l��\n" )

	#gmm�t�@�C������
  gmm_name = GMM::new
  gmm_type = GMM::new
  gmm_name.open_gmm( ARGV[ ARGV_READ_GMM_FILE ] , "trname.gmm" )
  gmm_type.open_gmm( ARGV[ ARGV_READ_GMM_FILE ] , "trtype.gmm" )

  gmm_name.make_row( "TR_NONE","�[" )

  gmm_type.make_row( "MSG_TRTYPE_HERO", "�|�P�����g���[�i�[" )
  gmm_type.make_row( "MSG_TRTYPE_HEROINE", "�|�P�����g���[�i�[" )

	trno = 1
  trainer = []

  read_data.size.times {|i|
    split_data = read_data[ i ].split(/,/)

    #�f�[�^�^�C�v���o
    if data_type[ split_data[ PARA::DATA_TYPE ] ] == nil
			printf("TrainerName:%s\n��`����Ă��Ȃ��f�[�^�^�C�v�ł��F%s\n", split_data[ PARA::TR_NAME ], split_data[ PARA::DATA_TYPE ] )
      exit( 1 )
    end

    #�|�P�����f�[�^�e�[�u��
		name = sprintf( "trpoke_%03d.s", trno )
		fp_trpoke = open( name, "w" )

		fp_trpoke.print "	.text\n"
		fp_trpoke.print "\n"
		fp_trpoke.print "	.include trdata.h\n"
		fp_trpoke.print "\n"

		for poke_count in 0..5
      if split_data[ PARA::POKE1_NAME + poke_count ] == nil
        break
      end

      if split_data[ PARA::POKE1_POW + poke_count * 4 ] == ""
				printf( "TrainerName:%s No:%d\nPOKEPOW NOTHING!!\n", split_data[ PARA::TR_NAME ], poke_count + 1 )
        exit( 1 )
      end
			fp_trpoke.printf( "	.short	%s\n", split_data[ PARA::POKE1_POW + poke_count * 4 ] )

      if split_data[ PARA::POKE1_LV + poke_count * 4 ] == ""
				printf( "TrainerName:%s No:%d\nPOKELEVEL NOTHING!!\n", split_data[ PARA::TR_NAME ], poke_count + 1 )
        exit( 1 )
      end
			fp_trpoke.printf( "	.short	%s\n", split_data[ PARA::POKE1_LV + poke_count * 4 ] )

      str = label.make_label( "MONSNO_", split_data[ PARA::POKE1_NAME + poke_count ] )
			fp_trpoke.printf( "	.short	%s\n", str )

      case data_type[ split_data[ PARA::DATA_TYPE ] ]
      when "DATATYPE_NORMAL"
				if split_data[ PARA::POKE1_ITEM + poke_count ] != ""
					printf( "TrainerName:%s No:%d\n�f�[�^�^�C�v���m�[�}���Ȃ̂ɃA�C�e�������Ă��܂�\n",
                    split_data[ PARA::TR_NAME ], poke_count + 1 )
          exit( 1 )
        end
				4.times { |j|
					if split_data[ PARA::POKE1_WAZA1 + poke_count * 4 + j ] != ""
						printf("TrainerName:%s No:%d\n�f�[�^�^�C�v���m�[�}���Ȃ̂ɋZ�����Ă��܂�\n",
                    split_data[ PARA::TR_NAME ], poke_count + 1 )
            exit( 1 )
          end
        }
      when "DATATYPE_WAZA"
				wazacnt = 0
				4.times{ |j|
					if split_data[ PARA::POKE1_WAZA1 + poke_count * 4 + j ] != ""
						str = label.make_label( "WAZANO_", split_data[ PARA::POKE1_WAZA1 + poke_count * 4 + j ] )
						fp_trpoke.printf( "	.short	%s\n", str )
						wazacnt += 1
					else
						fp_trpoke.printf( "	.short	0\n" )
          end
        }
				if wazacnt==0
					printf("TrainerName:%s No:%d\n�f�[�^�^�C�v���Z�Ȃ̂Ƀ|�P�����ɋZ�����Ă܂���\n",
                  split_data[ PARA::TR_NAME ], poke_count + 1 )
          exit( 1 )
        end
				if split_data[ PARA::POKE1_ITEM + poke_count ] != ""
					printf( "TrainerName:%s No:%d\n�f�[�^�^�C�v���Z�Ȃ̂ɃA�C�e�������Ă��܂�\n",
                  split_data[ PARA::TR_NAME ], poke_count + 1 )
          exit( 1 )
        end
      when "DATATYPE_ITEM"
				if split_data[ PARA::POKE1_ITEM + poke_count ] != ""
					str = label.make_label( "ITEM_", split_data[ PARA::POKE1_ITEM + poke_count ] )
					fp_trpoke.printf( "	.short	%s\n", str )
				else
					fp_trpoke.printf( "	.short	0\n" )
        end
      when "DATATYPE_MULTI"
				if split_data[ PARA::POKE1_ITEM + poke_count ] != ""
					str = label.make_label( "ITEM_", split_data[ PARA::POKE1_ITEM + poke_count ] )
					fp_trpoke.printf( "	.short	%s\n", str )
				else
					fp_trpoke.printf( "	.short	0\n" )
        end

				wazacnt = 0
				4.times { |j|
					if split_data[ PARA::POKE1_WAZA1 + poke_count * 4 + j ] != ""
						str = label.make_label( "WAZANO_", split_data[ PARA::POKE1_WAZA1 + poke_count * 4 + j ] )
						fp_trpoke.printf( "	.short	%s\n", str )
						wazacnt += 1
					else
						fp_trpoke.printf( "	.short	0\n" )
          end
        }
				if wazacnt==0
					printf("TrainerName:%s No:%d\n�f�[�^�^�C�v���}���`�Ȃ̂Ƀ|�P�����ɋZ�����Ă܂���\n",
                  split_data[ PARA::TR_NAME ], poke_count + 1 )
          exit( 1 )
        end
      end
    end

		if poke_count == 0
			printf( "TrainerName:%s\n�|�P�����������Ă��܂���\n", split_data[ PARA::TR_NAME ] )
      exit( 1 )
    end

		fp_trpoke.close

    #�g���[�i�[�f�[�^�e�[�u��
		name = sprintf( "trdata_%03d.s", trno )
		fp_trdata = open( name, "w" )

		fp_trdata.print "	.text\n"
		fp_trdata.print "\n"
		fp_trdata.print "	.include trdata.h\n"
		fp_trdata.print "\n"

		fp_trdata.printf( "	.byte	%s\n", data_type[ split_data[ PARA::DATA_TYPE ] ] )

		#unless split_data[ PARA::TR_ID ].to_s =~/^[^ -~�-�]*$/
    unless split_data[ PARA::TR_ID ].to_s =~/^[ -~�-�]*$/
			printf( "���x���ɑS�p�������Ă��܂��I->%s\n", split_data[ PARA::TR_ID ] )
			exit( 1 )
    end

		str = "TRTYPE_" + split_data[ PARA::TR_ID ][ 0..split_data[ PARA::TR_ID ].size - 4 ].upcase

    flag = 0
    cnt = 0
		trainer.size.times { |no|
      if trainer[ no ][ 0 ] == str
        flag = 1
				break
      end
      cnt += 1
    }
		if flag == 0
      trainer[ cnt ] = [ str, split_data[ PARA::TR_TYPE ], gender[ split_data[ PARA::GENDER ] ], split_data[ PARA::TR_ID ][ 0..split_data[ PARA::TR_ID ].size - 4 ] ]
    end

		fp_trdata.printf( "	.byte	%s\n", str );
		fp_trdata.printf( "	.byte	%s\n", fight_type[ split_data[ PARA::FIGHT_TYPE ] ] )
		fp_trdata.printf( "	.byte	%d\n", poke_count )

    4.times { |no|
			if split_data[ PARA::USE_ITEM1 + no ] != ""
				str = label.make_label( "ITEM_", split_data[ PARA::USE_ITEM1 + no ] )
				fp_trdata.printf( "	.short	%s\n", str )
			else
				fp_trdata.print "	.short	0\n"
      end
    }

    ai = split_data[ PARA::AI ].split(//)
    aibit = 0

    flag = 1

    ai.size.times {|no|
      if ai[ no ] == "��"
          aibit |= flag
      end
      flag = flag << 1
    }
		fp_trdata.printf( "	.long	0x%08x\n",aibit )

		fp_trdata.close

    #�g���[�i�[�̐�Δԍ�
		fp_trno.printf( "#define	TRID_%s   ( %d ) \n", split_data[ PARA::TR_ID ].upcase, trno )

    #�g���[�i�[��
    gmm_name.make_row( split_data[ PARA::TR_ID ], split_data[ PARA::TR_NAME ] )

		trno += 1
	}
	fp_trno.print( "\n" )
	fp_trno.print( "#ifndef __ASM_NO_DEF_  //����ȍ~�̓A�Z���u���ł͖���\n" )
	fp_trno.print( "typedef  int TrainerID;\n" )
	fp_trno.print( "#endif __ASM_NO_DEF_\n" )
	fp_trno.print( "\n" )
	fp_trno.print( "#endif __TRNO_DEF_H_\n" )
	fp_trno.close

  #�g���[�i�[�^�C�v��
  no = 2
  trainer.size.times { |i|
    fp_trgra.printf( "trwb_%s,%s\n", trainer[ i ][ 3 ], trainer[ i ][ 2 ] )
    fp_trfgra.printf( "\"trwb_%s.NCBR\"\n", trainer[ i ][ 3 ] )
    fp_trfgra.printf( "\"trwb_%s.NCER\"\n", trainer[ i ][ 3 ] )
    fp_trfgra.printf( "\"trwb_%s.NANR\"\n", trainer[ i ][ 3 ] )
    fp_trfgra.printf( "\"trwb_%s.NMCR\"\n", trainer[ i ][ 3 ] )
    fp_trfgra.printf( "\"trwb_%s.NMAR\"\n", trainer[ i ][ 3 ] )
    fp_trfgra.printf( "\"trwb_%s.NCEC\"\n", trainer[ i ][ 3 ] )
    fp_trfgra.printf( "\"trwb_%s.NCLR\"\n", trainer[ i ][ 3 ] )
		fp_trtype.printf( "#define	%s    ( %d )    //%s\n", trainer[ i ][ 0 ].upcase, no, trainer[ i ][ 1 ] )
    str = "MSG_" + trainer[ i ][ 0 ]
    gmm_type.make_row( str, trainer[ i ][ 1 ] )
		fp_trtypesex.printf( "\t%s,\t\t//%s\n", trainer[ i ][ 2 ], trainer[ i ][ 1 ] )
    no += 1
	}
	fp_trtype.printf( "#define	TRTYPE_MAX	( %d )\n", no )

  #gmm��n��
  gmm_name.close_gmm
  gmm_type.close_gmm

	fp_trgra.close
	fp_trfgra.close
	fp_trtype.close

	fp_trtypesex.print( "};\n" )
	fp_trtypesex.print( "\n" )
	fp_trtypesex.close

  #�^�C���X�^���v��r�p�̃_�~�[�t�@�C���𐶐�
  fp_w = open( "out_end", "w" )
  fp_w.close
