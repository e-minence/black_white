#!ruby -Ks
#!/usr/bin/ruby

#===================================================================
#
# @brief  �f���Đ��A�v���p�f��ID/�f�[�^�A�N�Z�X�e�[�u������
#
# @data   10.03.11 
# @author miyuki iwasawa
#
#===================================================================

#====================================================
# �ݒ�
#====================================================
# �����R�[�h�w��


$KCODE = 'SJIS'

#====================================================
# �萔�錾
#====================================================
#�w�b�_�t�@�C���o��
DST_FILENAME = "demo3d_demoid.h"  #�o�̓t�@�C����
DST_BRIEF_ID = "�f���Đ��A�v���Ŏg���f��ID�Bresource/demo3d/demo_list.txt���玩������"
DST_BRIEF_RES = "�f���Đ��A�v���Ŏg�����\�[�X�ւ̃A�N�Z�X�e�[�u���Bresource/demo3d/demo_list.txt���玩������"
DST_AUTOR = "miyuki iwasawa"
DST_NOTE = "���̃t�@�C����demo3d/conv/make_demoid.rb�ɂ���Ď����������ꂽ���̂ł��B"

arg = $*

#====================================================
# �又�� �J�n
#====================================================

class CMakeDemoTable

  def output_demoid
    path = @id_path
    File::open( path ,"w"){ |file|
      file.puts("//=========================================================================");
      file.puts("/**");
      file.puts(" * @file\t#{}");
      file.puts(" * @brief\t" + DST_BRIEF_ID);
      file.puts(" * @autor\t" + DST_AUTOR);
      file.puts(" * @note\t" + DST_NOTE);
      file.puts(" */");
      file.puts("//=========================================================================\n");
      file.puts("#ifndef __DEMO3D_DEMOID_H__\n");
      file.puts("#define __DEMO3D_DEMOID_H__\n\n");

      file.puts "#define DEMO3D_ID_NULL (0)\n\n"

      count = 1
      for i in @list
        file.puts "#define DEMO3D_ID_" + i.upcase + " (" + count.to_s + ")" 
        count += 1
      end
    
      file.puts "\n#define DEMO3D_ID_MAX (" + count.to_s + ")\n\n"
      file.puts("#endif // __DEMO3D_DEMOID_H__");
    }
  end
  
  def output_demo_res
    path = @res_path
    File::open( path ,"w"){ |file|
      file.puts("//=========================================================================");
      file.puts("/**");
      file.puts(" * @file\t" + path);
      file.puts(" * @brief\t" + DST_BRIEF_RES);
      file.puts(" * @autor\t" + DST_AUTOR);
      file.puts(" * @note\t" + DST_NOTE);
      file.puts(" */");
      file.puts("//=========================================================================\n");

      for n in @list
        file.puts("#include \"#{n}.cdat\"")
      end

      count = 1
      file.puts("\n\nstatic const DEMO3D_SETUP_DATA c_demo3d_setup_data[ DEMO3D_ID_MAX ] = {")
      file.puts(" { NULL, NULL, NULL, NULL, 0 },")
      for n in @list
        scene = "#{n}_scene"
        res = "res_#{scene}_unit"

        file.puts(" { &data_#{scene}, demo3d_cmd_#{n}_data, demo3d_cmd_#{n}_end_data, #{res}, NELEMS(#{res}) },")
        count += 1
      end
      file.puts("};\n\n")
    }
  end

  def initialize arg
    @id_path = arg[0]
    @res_path = arg[1]

    size = arg.size
    arg.slice!(0..1)

    @list = Array.new
    for n in arg do
      name = File::basename(n,'.xls')
      @list << name
      print("demo_id -> #{name}\n")
    end
  end
end #end of CMakeDemoTable

def convert arg
  c_demo = CMakeDemoTable.new(arg)
  c_demo.output_demoid()
  c_demo.output_demo_res()
end

convert(arg)
