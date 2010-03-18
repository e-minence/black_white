#-----------------------------------------------
# demo3d �R�}���h�p�����[�^�`�F�b�N�N���X
# �R�}���h����������`�F�b�N�֐���ǉ�����K�v������܂�
#
# make_cdat.rb��require���Ďg���܂��B�P�Ƃł͌Ăׂ܂���
#-----------------------------------------------


P_FREE = "0xFFFFFFFF"
P_LIST = "0xFFFFFFFE"

$_command = Struct.new("Command", :command, :frame, :param )
$_cmd_prm = Struct.new("CommandParam", :min, :max, :def_key)

class CDemo3DCmd
  attr  :name
  attr  :prm_num
  attr  :prm_tbl

  def initialize name,prm_num,tbl
    @name = name
    @prm_num = prm_num

    @prm_tbl = tbl
#    print("GetCmdParam = #{name} -> #{tbl.size}\n")
  end
end

#�R�}���h�p�����[�^��`
require "conv/c_demo3d_cmd.rb"

#-----------------------------------------------
# demo3d �R�}���h�p�����[�^�`�F�b�N�N���X
# �R�}���h����������`�F�b�N�֐���ǉ�����K�v������܂�
#-----------------------------------------------
class CDemo3DCmdCheck

  def cmd_search key
    for n in @cmd_tbl do
      if n.name == key then return n end
    end
    print("Error! �R�}���h�� #{key} �͖���`�ł�\n")
    exit 1;
    return nil
  end

  #�R�}���h�p�����[�^�`�F�b�N
  def check key, work
    cmd = cmd_search( key )
   
#    print("�R�}���h�e�[�u��Num = #{@cmd_tbl.size}, cmd_param_num = #{cmd.prm_num}, cmd = #{key}\n")
    @buf.fill( "DEMO3D_CMD_PARAM_NULL", 0..CMD_PARAM_END )
   
    #�����̐����J�E���g
    arg_num = 0
    if work == nil && cmd.prm_num == 0 then
      return @buf
    end
    for i in 0..CMD_PARAM_END do
      if work[i] == nil || work[i] == "" then break end
      arg_num += 1
    end
    
    if arg_num < cmd.prm_num then
      print("Error! �R�}���h #{key} �̈���������܂��� #{arg_num}/#{cmd.prm_num}\n")
      exit 1
    end
    
    if arg_num > cmd.prm_num then
      print("Error! �R�}���h #{key} �̈������������܂� #{arg_num}>#{cmd.prm_num}\n")
      exit 1
    end

    if cmd.prm_num == 0 then
      return @buf
    end

    for i in 0..(cmd.prm_num-1) do
      prm = cmd.prm_tbl[i]

      if prm.def_key != nil then
        val = prm.def_key.fetch(work[i],nil)
        if val != nil then
#         print("buf#{i}= #{work[i]}\n")
          @buf[i] = val
          next
        end

        if prm.min == P_LIST then
          print("Error! �s���ȃp�����[�^ #{work[i]}�@�w��\�Ȓl�͈ȉ��ł�\n")
          ary = prm.def_key.keys
          for n in ary do
            print(" #{n}\n")
          end
          print("\n\n")
          exit 1
        end
      end

      if prm.min != P_FREE && work[i].to_i < prm.min then
        print("Error! �s���ȃp�����[�^�w�� #{work[i]} -> �ŏ��l = #{prm.min}\n")
        exit 1
      end
      
      if prm.max != P_FREE && work[i].to_i > prm.max then
        print("Error! �s���ȃp�����[�^�w�� #{work[i]} -> �ő�l = #{prm.max}\n")
        exit 1
      end

#     print("buf#{i}= #{work[i]}\n")
      @buf[i] = work[i]
    end

    return @buf
  end

  def initialize 
    @buf = Array.new
    @cmd_tbl = $cmd_tbl
  end
end


