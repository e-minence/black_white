#-----------------------------------------------
# demo3d コマンドパラメータチェッククラス
# コマンドが増えたらチェック関数を追加する必要があります
#
# make_cdat.rbにrequireして使われます。単独では呼べません
#-----------------------------------------------


P_FREE = "0xFFFFFFFF"
P_LIST = "0xFFFFFFFE"

$_command = Struct.new("Command", :command, :frame, :param )
$_cmd_prm = Struct.new("CommandParam", :min, :max, :def_key, :psfix )
$_psfix_prm = Struct.new("PreSuffixParam", :prefix, :suffix )

class CDemo3DCmd
  attr  :name
  attr  :prm_num
  attr  :prm_tbl

  def initialize name,tbl
    @name = name

    @prm_tbl = tbl
    if tbl != nil then
      @prm_num = tbl.size()
    else
      @prm_num = 0
    end

  print("GetCmdParam = #{name} -> #{@prm_num}\n")
  end
end

#コマンドパラメータ定義
require "conv/c_demo3d_cmd.rb"

#-----------------------------------------------
# demo3d コマンドパラメータチェッククラス
# コマンドが増えたらチェック関数を追加する必要があります
#-----------------------------------------------
class CDemo3DCmdCheck

  def cmd_search key
    for n in @cmd_tbl do
      if n.name == key then return n end
    end
    print("Error! コマンド名 #{key} は未定義です\n")
    exit 1;
    return nil
  end

  #プレフィックス・サフィックスチェック
  def psfix_check prm, key
    buf = ""
    if prm.psfix == nil then return key end

    if prm.psfix.prefix != nil then
      buf += prm.psfix.prefix
    end
    buf += key
    if prm.psfix.suffix != nil then
      buf += prm.psfix.suffix
    end
    return buf
  end

  #コマンドパラメータチェック
  def check key, work
    cmd = cmd_search( key )
   
#    print("コマンドテーブルNum = #{@cmd_tbl.size}, cmd_param_num = #{cmd.prm_num}, cmd = #{key}\n")
    @buf.fill( "DEMO3D_CMD_PARAM_NULL", 0..CMD_PARAM_END )
   
    #パラメータの数をカウント
    arg_num = 0
    if work == nil && cmd.prm_num == 0 then
      return @buf
    end
    for i in 0..CMD_PARAM_END do
      if work[i] == nil || work[i] == "" then break end
      arg_num += 1
    end
    
    if arg_num < cmd.prm_num then
      print("Error! コマンド #{key} のパラメータが足りません #{arg_num}/#{cmd.prm_num}\n")
      exit 1
    end
    
    if arg_num > cmd.prm_num then
      print("Error! コマンド #{key} のパラメータが多すぎます #{arg_num}>#{cmd.prm_num}\n")
      exit 1
    end

    if cmd.prm_num == 0 then
      return @buf
    end

    for i in 0..(cmd.prm_num-1) do
      prm = cmd.prm_tbl[i]
=begin
      print("#{work[i]} #{prm.min} #{prm.max} #{prm.def_key} end\n")
      if prm.def_key == nil then
        print("def_key null\n")
      elsif prm.def_key == "" then
        print("def_key null str\n")
      end
=end
      if prm.def_key != nil then
        val = prm.def_key.fetch(work[i],nil)
        if val != nil then
#         print("buf#{i}= #{work[i]}\n")
          @buf[i] = val
          next
        end

        if prm.min == P_LIST then
          print("Error! 不正なパラメータ #{work[i]}　指定可能な値は以下です\n")
          ary = prm.def_key.keys
          for n in ary do
            print(" #{n}\n")
          end
          print("\n\n")
          exit 1
        end
      end

      if prm.min != P_FREE && work[i].to_f < prm.min then
        print("Error! 不正なパラメータ指定 #{work[i]} -> 最小値 = #{prm.min}\n")
        exit 1
      end
      
      if prm.max != P_FREE && work[i].to_f > prm.max then
        print("Error! 不正なパラメータ指定 #{work[i]} -> 最大値 = #{prm.max}\n")
        exit 1
      end

#     print("buf#{i}= #{work[i]}\n")
      @buf[i] = psfix_check( prm, work[i] )
    end

    return @buf
  end

  def initialize 
    @buf = Array.new
    @cmd_tbl = $cmd_tbl
  end
end


