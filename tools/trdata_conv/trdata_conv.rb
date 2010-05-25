#! ruby -Ks

  require File.dirname(__FILE__) + '/../label_make/label_make'
  require File.dirname(__FILE__) + '/../gmm_make/gmm_make'
  require File.dirname(__FILE__) + '/../constant'
  require File.dirname(__FILE__) + '/../hash/monsno_hash.rb'
  require File.dirname(__FILE__) + '/../hash/wazano_hash.rb'
  require File.dirname(__FILE__) + '/../hash/item_hash.rb'
  require File.dirname(__FILE__) + '/../hash/tokusei_hash.rb'

=begin
	u8			data_type;        //�f�[�^�^�C�v
	u8			tr_type;					//�g���[�i�[����
  u8      fight_type;       //1vs1or2vs2 
	u8			poke_count;       //�����|�P������

	u16			use_item[4];      //�g�p����

	u32			aibit;						//AI�p�^�[��

  u8      hp_recover_flag :1;
  u8                      :7;
  u8      gold;

  u16     gift_item;

//�g���[�i�[�����|�P�����p�����[�^�i�f�[�^�^�C�v�m�[�}���j
	u8		pow;			  //�Z�b�g����p���[����
  u8    para;       //�Z�b�g����|�P�����̃p�����[�^�i���4bit�F�����@����4bit�F���ʁj
	u16		level;			//�Z�b�g����|�P�����̃��x��
	u16		monsno;			//�Z�b�g����|�P����
	u16		formno;			//�Z�b�g����t�H�����i���o�[

//�g���[�i�[�����|�P�����p�����[�^�i�f�[�^�^�C�v�Z�j
	u8		pow;			  //�Z�b�g����p���[����
  u8    para;       //�Z�b�g����|�P�����̃p�����[�^�i���4bit�F�����@����4bit�F���ʁj
	u16		level;			//�Z�b�g����|�P�����̃��x��
	u16		monsno;			//�Z�b�g����|�P����
	u16		formno;			//�Z�b�g����t�H�����i���o�[
	u16		waza[4];		//�����Ă�Z

//�g���[�i�[�����|�P�����p�����[�^�i�f�[�^�^�C�v�A�C�e���j
	u8		pow;			  //�Z�b�g����p���[����
  u8    para;       //�Z�b�g����|�P�����̃p�����[�^�i���4bit�F�����@����4bit�F���ʁj
	u16		level;			//�Z�b�g����|�P�����̃��x��
	u16		monsno;			//�Z�b�g����|�P����
	u16		formno;			//�Z�b�g����t�H�����i���o�[
	u16		itemno;			//�Z�b�g����A�C�e��

//�g���[�i�[�����|�P�����p�����[�^�i�f�[�^�^�C�v�}���`�j
	u8		pow;			  //�Z�b�g����p���[����
  u8    para;       //�Z�b�g����|�P�����̃p�����[�^�i���4bit�F�����@����4bit�F���ʁj
	u16		level;			//�Z�b�g����|�P�����̃��x��
	u16		monsno;			//�Z�b�g����|�P����
	u16		formno;			//�Z�b�g����t�H�����i���o�[
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
    HP_RECOVER
    GIFT_ITEM
    POKE1_SPEABI
    POKE2_SPEABI
    POKE3_SPEABI
    POKE4_SPEABI
    POKE5_SPEABI
    POKE6_SPEABI
    MAX
  ]
end

class TR
  enum_const_set %w[
    TRTYPE
    STR
    GENDER
    LABEL
    STR_HASH
    NAME
  ]
end

  fight_type = {  
	  "�P�΂P"=>0,
	  "�Q�΂Q"=>1,
	  "�R�΂R"=>2,
	  "���[�e"=>3,
  }

  data_type = { 
		"�m�[�}��"  =>0,  #DATATYPE_NORMAL
		"�Z"        =>1,  #DATATYPE_WAZA
		"����"      =>2,  #DATATYPE_ITEM
		"�}���`"    =>3,  #DATATYPE_MULTI
  }

  gender = {  
    "��"=>"PTL_SEX_MALE",
    "��"=>"PTL_SEX_FEMALE",
    "�|"=>"PTL_SEX_UNKNOWN",
  }

  poke_gender = {  
    "��"=>1,
    "��"=>2,
    "�|"=>0,
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
  fp_trfgra.print "\"trwb_hero.NCGR\"\n"
  fp_trfgra.print "\"trwb_hero.NCBR\"\n"
  fp_trfgra.print "\"trwb_hero.NCER\"\n"
  fp_trfgra.print "\"trwb_hero.NANR\"\n"
  fp_trfgra.print "\"trwb_hero.NMCR\"\n"
  fp_trfgra.print "\"trwb_hero.NMAR\"\n"
  fp_trfgra.print "\"trwb_hero.NCEC\"\n"
  fp_trfgra.print "\"trwb_hero.NCLR\"\n"
  fp_trfgra.print "\"trwb_heroine.NCGR\"\n"
  fp_trfgra.print "\"trwb_heroine.NCBR\"\n"
  fp_trfgra.print "\"trwb_heroine.NCER\"\n"
  fp_trfgra.print "\"trwb_heroine.NANR\"\n"
  fp_trfgra.print "\"trwb_heroine.NMCR\"\n"
  fp_trfgra.print "\"trwb_heroine.NMAR\"\n"
  fp_trfgra.print "\"trwb_heroine.NCEC\"\n"
  fp_trfgra.print "\"trwb_heroine.NCLR\"\n"

	#�g���[�i�[�f�[�^�t�@�C������
	fp_trdata = open( "trdata_000.bin", "wb" )

  data = [ 0,0,0,0,0,0,0,0,0 ].pack( "C4 S4 L" )
	data.size.times{ |c|
		fp_trdata.printf("%c",data[ c ])
	}

	fp_trdata.close

	#�g���[�i�[�����|�P�����f�[�^�t�@�C������
	fp_trpoke = open( "trpoke_000.bin", "wb" )

  data = [ 0,0,0 ].pack( "S3" )
	data.size.times{ |c|
		fp_trpoke.printf("%c",data[ c ])
	}

	fp_trpoke.close

	#�g���[�i�[ID��`�t�@�C������
	fp_trno = open( "trno_def.h", "w" )

  fp_trno.print( "//============================================================================================\n" )
  fp_trno.print( "/**\n" )
  fp_trno.print( " * @file	trno_def.h\n" )
  fp_trno.print( " * @bfief	�g���[�i�[ID��`�t�@�C��\n" )
  fp_trno.print( " * @author	TrainerDataConverter\n" )
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
	fp_trtypesex.print( " * @author	���̃t�@�C���́A�R���o�[�^���f���o�����t�@�C���ł�\n" )
  fp_trtypesex.print( "*/\n")
  fp_trtypesex.print( "//============================================================================================\n" )
  fp_trtypesex.print( "\n#pragma once\n\n" )
	fp_trtypesex.print( "\n" )
	fp_trtypesex.print( "static	const	u8	TrTypeSexTable[]={\n" )
	fp_trtypesex.print( "\tPTL_SEX_MALE,		//�j��l��\n" )
	fp_trtypesex.print( "\tPTL_SEX_FEMALE,	//����l��\n" )

	#�g���[�i�[�O���t�B�b�N�C���f�b�N�X�t�@�C������
	fp_trtypegra = open( "trtype_gra.h", "w" )

  fp_trtypegra.print( "//============================================================================================\n" )
  fp_trtypegra.print( "/**\n" )
  fp_trtypegra.print( " * @file	trtype_gra.h\n" )
	fp_trtypegra.print( " * @bfief	�g���[�i�[�O���t�B�b�N�C���f�b�N�X�t�@�C����`\n")
  fp_trtypegra.print( " * @author	TrainerDataConverter\n" )
	fp_trtypegra.print( " * @author	���̃t�@�C���́A�R���o�[�^���f���o�����t�@�C���ł�\n" )
  fp_trtypegra.print( "*/\n")
  fp_trtypegra.print( "//============================================================================================\n" )
  fp_trtypegra.print( "\n#pragma once\n\n" )
	fp_trtypegra.print( "static	const	u8	TrTypeGraTable[]={\n" )
	fp_trtypegra.print( "\t0, //�j��l��\n" )
	fp_trtypegra.print( "\t1,	//����l��\n" )

	#gmm�t�@�C������
  gmm_name = GMM::new
  gmm_type = GMM::new
  gmm_name.open_gmm( ARGV[ ARGV_READ_GMM_FILE ] , "trname.gmm" )
  gmm_type.open_gmm( ARGV[ ARGV_READ_GMM_FILE ] , "trtype.gmm" )

  gmm_name.make_row_kanji( "TR_NONE","�[","�[" )

  gmm_type.make_row_kanji( "MSG_TRTYPE_HERO", "�|�P�����g���[�i�[", "�|�P�����g���[�i�[" )
  gmm_type.make_row_kanji( "MSG_TRTYPE_HEROINE", "�|�P�����g���[�i�[", "�|�P�����g���[�i�[" )

	trno = 1
  trainer = []
  tr_type = Hash::new

  read_data.size.times {|i|
    split_data = read_data[ i ].split(/,/)

    #�f�[�^�^�C�v���o
    if data_type[ split_data[ PARA::DATA_TYPE ] ] == nil
			printf("Num:%d\nTrainerName:%s\n��`����Ă��Ȃ��f�[�^�^�C�v�ł��F%s\n", i, split_data[ PARA::TR_NAME ], split_data[ PARA::DATA_TYPE ] )
      exit( 1 )
    end

    #�|�P�����f�[�^�e�[�u��
		name = sprintf( "trpoke_%03d.bin", trno )
		fp_trpoke = open( name, "wb" )

    count = 0

		for poke_count in 0..5
      if split_data[ PARA::POKE1_NAME + poke_count ] == nil || split_data[ PARA::POKE1_NAME + poke_count ] == ""
        break
      end

      count += 1

      if split_data[ PARA::POKE1_POW + poke_count * 4 ] == ""
				printf( "TrainerName:%s No:%d\nPOKEPOW NOTHING!!\n", split_data[ PARA::TR_NAME ], poke_count + 1 )
        exit( 1 )
      end
      pow = split_data[ PARA::POKE1_POW + poke_count * 4 ].to_i

      if split_data[ PARA::POKE1_LV + poke_count * 4 ] == ""
				printf( "TrainerName:%s No:%d\nPOKELEVEL NOTHING!!\n", split_data[ PARA::TR_NAME ], poke_count + 1 )
        exit( 1 )
      end
      lv = split_data[ PARA::POKE1_LV + poke_count * 4 ].to_i

      mons = $monsno_hash[ split_data[ PARA::POKE1_NAME + poke_count ] ]

      if split_data[ PARA::POKE1_GENDER + poke_count * 4 ] == "" || split_data[ PARA::POKE1_GENDER + poke_count * 4 ] == nil
        para = 0
      else
        para = poke_gender[ split_data[ PARA::POKE1_GENDER + poke_count * 4 ] ]
        if para == nil
				  printf( "TrainerName:%s No:%d\nUNKOWN GENDER!!\n", split_data[ PARA::TR_NAME ], poke_count + 1 )
          exit( 1 )
        end
      end

      if split_data[ PARA::POKE1_SPEABI + poke_count * 4 ] != "" && split_data[ PARA::POKE1_SPEABI + poke_count * 4 ] != nil
        if split_data[ PARA::POKE1_SPEABI + poke_count * 4 ].to_i > 3
				  printf( "TrainerName:%s No:%d\nUNKOWN TOKUSEI!!\n", split_data[ PARA::TR_NAME ], poke_count + 1 )
          exit( 1 )
        end
        para |= ( split_data[ PARA::POKE1_SPEABI + poke_count * 4 ].to_i ) << 4
      end

      form = split_data[ PARA::POKE1_FORM + poke_count * 4 ].to_i

      data = [ pow, para, lv, mons, form ].pack( "C2 S3" )
	    data.size.times{ |c|
		    fp_trpoke.printf("%c",data[ c ])
	    }

      case data_type[ split_data[ PARA::DATA_TYPE ] ]
      when 0  #DATATYPE_NORMAL
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
      when 1  #DATATYPE_WAZA
				wazacnt = 0
				4.times{ |j|
					if split_data[ PARA::POKE1_WAZA1 + poke_count * 4 + j ] != ""
            waza = $wazano_hash[ split_data[ PARA::POKE1_WAZA1 + poke_count * 4 + j ] ]
						wazacnt += 1
					else
            waza = 0
          end
          data = [ waza ].pack( "S" )
    	    data.size.times{ |c|
	    	    fp_trpoke.printf("%c",data[ c ])
	        }
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
      when 2  #DATATYPE_ITEM
				if split_data[ PARA::POKE1_ITEM + poke_count ] != ""
          item = $item_hash[ split_data[ PARA::POKE1_ITEM + poke_count ] ]
				else
          item = 0
        end
        data = [ item ].pack( "S" )
    	  data.size.times{ |c|
	    	  fp_trpoke.printf("%c",data[ c ])
	      }
      when 3  #DATATYPE_MULTI
				if split_data[ PARA::POKE1_ITEM + poke_count ] != ""
          item = $item_hash[ split_data[ PARA::POKE1_ITEM + poke_count ] ]
				else
          item = 0
        end
        data = [ item ].pack( "S" )
    	  data.size.times{ |c|
	    	  fp_trpoke.printf("%c",data[ c ])
	      }

				wazacnt = 0
				4.times { |j|
					if split_data[ PARA::POKE1_WAZA1 + poke_count * 4 + j ] != ""
            waza = $wazano_hash[ split_data[ PARA::POKE1_WAZA1 + poke_count * 4 + j ] ]
						wazacnt += 1
					else
            waza = 0
          end
          data = [ waza ].pack( "S" )
    	    data.size.times{ |c|
	    	    fp_trpoke.printf("%c",data[ c ])
	        }
        }
				if wazacnt==0
					printf("TrainerName:%s No:%d\n�f�[�^�^�C�v���}���`�Ȃ̂Ƀ|�P�����ɋZ�����Ă܂���\n",
                  split_data[ PARA::TR_NAME ], poke_count + 1 )
          exit( 1 )
        end
      end
    end

		if count == 0
			printf( "TrainerName:%s\n�|�P�����������Ă��܂���\n", split_data[ PARA::TR_NAME ] )
      exit( 1 )
    end

		fp_trpoke.close

    #�g���[�i�[�f�[�^�e�[�u��
		name = sprintf( "trdata_%03d.bin", trno )
		fp_trdata = open( name, "wb" )

		type_data = data_type[ split_data[ PARA::DATA_TYPE ] ]

    unless split_data[ PARA::TR_ID ].to_s =~/^[ -~�-�]*$/
			printf( "���x���ɑS�p�������Ă��܂��I->%s\n", split_data[ PARA::TR_ID ] )
			exit( 1 )
    end

		str = "TRTYPE_" + split_data[ PARA::TR_ID ][ 0..split_data[ PARA::TR_ID ].size - 4 ].upcase
		name = split_data[ PARA::TR_ID ][ 0..split_data[ PARA::TR_ID ].size - 4 ].upcase

    flag = 0
    cnt = 0
		trainer.size.times { |no|
      if trainer[ no ][ TR::TRTYPE ] == str
        flag = 1
				break
      end
      cnt += 1
    }
		if flag == 0
      label = split_data[ PARA::TR_ID ][ 0..(split_data[ PARA::TR_ID ].index("_")-1) ]
      trainer[ cnt ] = [ str, split_data[ PARA::TR_TYPE ], gender[ split_data[ PARA::GENDER ] ], label, split_data[ PARA::TR_TYPE ], name ]
      tr_type[ str ] = cnt + 2
    end

    type_tr     = tr_type[ str ]
    type_fight  = fight_type[ split_data[ PARA::FIGHT_TYPE ] ]

    data = [ type_data, type_tr, type_fight, count ].pack( "C4" )
	  data.size.times{ |c|
		  fp_trdata.printf("%c",data[ c ])
	  }

    4.times { |no|
			if split_data[ PARA::USE_ITEM1 + no ] != "" && split_data[ PARA::USE_ITEM1 + no ] != nil
        item = $item_hash[ split_data[ PARA::USE_ITEM1 + no ] ]
			else
        item = 0
      end
      data = [ item ].pack( "S" )
	    data.size.times{ |c|
		    fp_trdata.printf("%c",data[ c ])
	    }
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

    data = [ aibit ].pack( "L" )
	  data.size.times{ |c|
		  fp_trdata.printf("%c",data[ c ])
	  }

    hp_recover_flag = 0
    if split_data[ PARA::HP_RECOVER ] == "��"
      hp_recover_flag = 1
    end

    data = [ hp_recover_flag ].pack( "C" )
	  data.size.times{ |c|
		  fp_trdata.printf("%c",data[ c ])
	  }

    data = [ split_data[ PARA::GOLD ].to_i ].pack( "C" )
	  data.size.times{ |c|
		  fp_trdata.printf("%c",data[ c ])
	  }

    if split_data[ PARA::GIFT_ITEM ] != "" && split_data[ PARA::GIFT_ITEM ] != nil
      item = $item_hash[ split_data[ PARA::GIFT_ITEM ] ]
    else
      item = 0
    end

    data = [ item ].pack( "S" )
	  data.size.times{ |c|
		  fp_trdata.printf("%c",data[ c ])
	  }

		fp_trdata.close

    #�g���[�i�[�̐�Δԍ�
		fp_trno.printf( "#define	TRID_%s   ( %d ) \n", split_data[ PARA::TR_ID ].upcase, trno )

    #�g���[�i�[��
    gmm_name.make_row_kanji( split_data[ PARA::TR_ID ], split_data[ PARA::TR_NAME ], split_data[ PARA::TR_NAME ] )

		trno += 1
	}
	fp_trno.printf( "#define TRID_MAX  ( %d )\n", trno )
	fp_trno.print( "\n" )
	fp_trno.print( "#ifndef __ASM_NO_DEF_  //����ȍ~�̓A�Z���u���ł͖���\n" )
	fp_trno.print( "typedef  int TrainerID;\n" )
	fp_trno.print( "#endif __ASM_NO_DEF_\n" )
	fp_trno.print( "\n" )
	fp_trno.print( "#endif __TRNO_DEF_H_\n" )
	fp_trno.close

  #�g���[�i�[�^�C�vHash����
  fp_hash = open( "trtype_hash.rb", "w" )
  fp_hash.printf("#! ruby -Ks\n\n" )

  fp_hash.printf("\t$trtype_hash = {\n" )

  #�g���[�i�[�^�C�v��
  #���Ԃ�`�F�b�N
  trainer.size.times { |i|
    str = trainer[ i ][ TR::STR_HASH ]
    find = 0
    for j in ( i + 1 )..( trainer.size - 1 )
      if str == trainer[ j ][ TR::STR_HASH ]
        find = 1
        break
      end
    end

    if find != 0
      trainer.size.times { |k|
        if str == trainer[ k ][ TR::STR_HASH ]
          #_�i�A���_�[�o�[�j��2����ꍇ�́ALABEL���i�j�t���ŘA��
          if trainer[ k ][ TR::TRTYPE ].scan("_").size == 2
            trainer[ k ][ TR::STR_HASH ] = trainer[ k ][ TR::STR_HASH ] + "�i%s�j" % [ trainer[ k ][ TR::NAME ].upcase ]
          elsif trainer[ k ][ TR::TRTYPE ].index(/M[0-9]/) != nil
            trainer[ k ][ TR::STR_HASH ] = trainer[ k ][ TR::STR_HASH ] + "��"
          elsif trainer[ k ][ TR::TRTYPE ].index(/W[0-9]/) != nil
            trainer[ k ][ TR::STR_HASH ] = trainer[ k ][ TR::STR_HASH ] + "��"
          #�������琔�����T�[�`���Č�����΂���ȍ~��A��
          elsif trainer[ k ][ TR::TRTYPE ].index(/[0-9]/) != nil
            trainer[ k ][ TR::STR_HASH ] = trainer[ k ][ TR::STR_HASH ] + trainer[ k ][ TR::TRTYPE ][ trainer[ k ][ TR::TRTYPE ].index(/[0-9]/)..trainer[ k ][TR::TRTYPE ].size - 1 ]
          #������M��B�Ȃ灉
          elsif trainer[ k ][ TR::TRTYPE ][ trainer[ k ][TR::TRTYPE ].size - 1 ].chr == "M" ||
            trainer[ k ][ TR::TRTYPE ][ trainer[ k ][TR::TRTYPE ].size - 1 ].chr == "B"
            trainer[ k ][ TR::STR_HASH ] = trainer[ k ][ TR::STR_HASH ] + "��"
          #������W��G�Ȃ灊
          elsif trainer[ k ][ TR::TRTYPE ][ trainer[ k ][TR::TRTYPE ].size - 1 ].chr == "W" ||
                trainer[ k ][ TR::TRTYPE ][ trainer[ k ][TR::TRTYPE ].size - 1 ].chr == "G"
            trainer[ k ][ TR::STR_HASH ] = trainer[ k ][ TR::STR_HASH ] + "��"
          #����ȊO�Ȃ�ALABEL���i�j�t���ŘA��
          else
            trainer[ k ][ TR::STR_HASH ] = trainer[ k ][ TR::STR_HASH ] + "�i%s�j" % [ trainer[ k ][ TR::LABEL ].upcase ]
          end
        end
      }
    end
  }

  no = 2
  idx = 0
  trfgra = []
  trainer.size.times { |i|
    fp_trgra.printf( "trwb_%s,%s\n", trainer[ i ][ TR::LABEL ], trainer[ i ][ TR::GENDER ] )
    find = 0
    idx = 2
    trfgra.size.times {|j|
      if trfgra[ j ] == trainer[ i ][ TR::LABEL ]
        find = 1
        break
      end
      idx += 1
    }
    if find == 0
      fp_trfgra.printf( "\"trwb_%s.NCGR\"\n", trainer[ i ][ TR::LABEL ] )
      fp_trfgra.printf( "\"trwb_%s.NCBR\"\n", trainer[ i ][ TR::LABEL ] )
      fp_trfgra.printf( "\"trwb_%s.NCER\"\n", trainer[ i ][ TR::LABEL ] )
      fp_trfgra.printf( "\"trwb_%s.NANR\"\n", trainer[ i ][ TR::LABEL ] )
      fp_trfgra.printf( "\"trwb_%s.NMCR\"\n", trainer[ i ][ TR::LABEL ] )
      fp_trfgra.printf( "\"trwb_%s.NMAR\"\n", trainer[ i ][ TR::LABEL ] )
      fp_trfgra.printf( "\"trwb_%s.NCEC\"\n", trainer[ i ][ TR::LABEL ] )
      fp_trfgra.printf( "\"trwb_%s.NCLR\"\n", trainer[ i ][ TR::LABEL ] )
      trfgra << trainer[ i ][ TR::LABEL ]
    end
    fp_trtypegra.printf( "\t%d,\t\t//%s\n", idx, trainer[ i ][ TR::STR ] )
		fp_trtype.printf( "#define	%s    ( %d )    //%s\n", trainer[ i ][ TR::TRTYPE ].upcase, no, trainer[ i ][ TR::STR_HASH ] )
    str = "MSG_" + trainer[ i ][ TR::TRTYPE ]
    gmm_type.make_row_kanji( str, trainer[ i ][ TR::STR ], trainer[ i ][ TR::STR ] )
		fp_trtypesex.printf( "\t%s,\t\t//%s\n", trainer[ i ][ TR::GENDER ], trainer[ i ][ TR::STR_HASH ] )
    fp_hash.printf("\t\t\"%s\"=>%d,\n", trainer[ i ][ TR::STR_HASH ], no )
    no += 1
	}
	fp_trtype.printf( "#define	TRTYPE_HERO_S	    ( %d )\n", no )
	fp_trtype.printf( "#define	TRTYPE_HEROINE_S	( %d )\n", no + 1)
	fp_trtype.printf( "#define	TRTYPE_MAX	( %d )\n", no + 2 )

	fp_trtypegra.printf( "\t%d, //�j��l���i�V���[�^�[�j\n", idx + 1 )
	fp_trtypegra.printf( "\t%d,	//����l���i�V���[�^�[�j\n", idx + 2 )

  gmm_type.make_row_kanji( "MSG_TRTYPE_HERO_S", "�|�P�����g���[�i�[", "�|�P�����g���[�i�[" )
  gmm_type.make_row_kanji( "MSG_TRTYPE_HEROINE_S", "�|�P�����g���[�i�[", "�|�P�����g���[�i�[" )

	fp_trtypesex.print( "\tPTL_SEX_MALE,		//�j��l���i�V���[�^�[�j\n" )
	fp_trtypesex.print( "\tPTL_SEX_FEMALE,	//����l���i�V���[�^�[�j\n" )

  #gmm��n��
  gmm_name.close_gmm
  gmm_type.close_gmm

  #�V���[�^�[�p���@�O���t�B�b�N
  fp_trfgra.print "\"trwb_hero_s.NCGR\"\n"
  fp_trfgra.print "\"trwb_hero_s.NCBR\"\n"
  fp_trfgra.print "\"trwb_hero_s.NCER\"\n"
  fp_trfgra.print "\"trwb_hero_s.NANR\"\n"
  fp_trfgra.print "\"trwb_hero_s.NMCR\"\n"
  fp_trfgra.print "\"trwb_hero_s.NMAR\"\n"
  fp_trfgra.print "\"trwb_hero_s.NCEC\"\n"
  fp_trfgra.print "\"trwb_hero_s.NCLR\"\n"
  fp_trfgra.print "\"trwb_heroine_s.NCGR\"\n"
  fp_trfgra.print "\"trwb_heroine_s.NCBR\"\n"
  fp_trfgra.print "\"trwb_heroine_s.NCER\"\n"
  fp_trfgra.print "\"trwb_heroine_s.NANR\"\n"
  fp_trfgra.print "\"trwb_heroine_s.NMCR\"\n"
  fp_trfgra.print "\"trwb_heroine_s.NMAR\"\n"
  fp_trfgra.print "\"trwb_heroine_s.NCEC\"\n"
  fp_trfgra.print "\"trwb_heroine_s.NCLR\"\n"
	fp_trgra.close
	fp_trfgra.close
	fp_trtype.close

	fp_trtypegra.print( "};\n" )
	fp_trtypegra.print( "\n" )
	fp_trtypegra.close

	fp_trtypesex.print( "};\n" )
	fp_trtypesex.print( "\n" )
	fp_trtypesex.close

  fp_hash.printf("\t}\n" )
  fp_hash.close

  #�^�C���X�^���v��r�p�̃_�~�[�t�@�C���𐶐�
  fp_w = open( "out_end", "w" )
  fp_w.close
