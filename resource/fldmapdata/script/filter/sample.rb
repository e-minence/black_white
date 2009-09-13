#
# DO NOT MODIFY!!!!
# This file is automatically generated by racc 1.4.5
# from racc grammer file "sp4.y".
#

require 'racc/parser'


#
#	ポケモン用スクリプトコンパイラ
#


module PmScript

  class Parser < Racc::Parser

module_eval <<'..end sp4.y modeval..id02331fd538', 'sp4.y', 543

def initialize
end

#予約語定義
RESERVED = {
	'IF'	=> :IF,
	'THEN'	=> :THEN,
	'ELSE'	=> :ELSE,
	'ELSIF'	=> :ELSIF,
	'ENDIF'	=> :ENDIF,
	'DEFVAR'	=> :DEFVAR,
	'DEFFUNC'	=> :DEFFUNC,
	'FLAG_ON'	=> :FLAG_ON,
	'FLAG_OFF'	=> :FLAG_OFF,
  'EVENT_START' => :EVENT_START,
  'EVENT_END' => :EVENT_END,
  'SWITCH' => :SWITCH,
  'CASE' => :CASE,
  'ENDSWITCH' => :ENDSWITCH,
  'WHILE' => :WHILE,
  'ENDWHILE' => :ENDWHILE
  'DEFINE_LOCAL' => :DEFINE_LOCAL
  'UNDEF_LOCAL' => :UNDEF_LOCAL
};

#予約型定義
RESERVED_TYPE = {
	'LWORK'	=> :LWORK,
	'LABEL' => :LABEL_TYPE,
};

def parse( f )
	@q = []
	@nowlineno = 1
	@incomment = false
	f.each do |line_org|
		line = line_org.strip
		until line.empty? do
			if @incomment == true then
				if line =~ /.*\*\// then
					@incomment = false
					pushq [ :COMMENT, $&]
					line = $'
				else
					pushq [ :COMMENT, line]
					line = ""
				end
			else
				case line
				when /\A\s+/
					;
				when /\A\/\/.*$/
          #Ｃ＋＋形式コメント定義
					pushq [ :COMMENT, $& ]

				when /^\s*$/, /\A\#.*/
          #アセンブラコメント定義
					pushq [ :COMMENT, $&]

				when /\A\/\*.*\*\//
          #Ｃ形式コメント定義
					pushq [ :COMMENT, $&]

				when /\A\.[a-zA-Z_]+.*$/
          #アセンブラマクロ定義
					pushq [ :MACRO, $& ]

				when /\A[a-zA-Z_][a-zA-Z0-9_]*:/
          #アセンブララベル定義
					pushq [ :LABEL, $& ]

				when /\A_[A-Z0-9][a-zA-Z0-9_]*/
          #スクリプトコマンド定義（＿で開始する）
					pushq [ :COMMAND, $& ]

				when /\A0x[0-9a-fA-F]+/, /\A\d+/
          #数値定義（0xで始まる１６進数、あるいは１０進数）
					pushq [ :NUMBER, $& ]

				when /\A\$[a-zA-Z][a-zA-Z0-9_]*/
          #変数定義（＄で始まる）
					pushq [ :VARREF, $& ]

				when /\A[a-zA-Z_][a-zA-Z0-9_]*/
          #識別子定義あるいは型定義
					if RESERVED.has_key? $& then
						pushq [ RESERVED[$&], $&.intern ]
					elsif RESERVED_TYPE.has_key? $& then
						pushq [ :TYPE, $&.intern ]
					else
						pushq [ :IDENT, $& ]
					end

				when /\A\\[a-zA-Z_][a-zA-Z0-9_]*/
          # \から始まる識別子はアセンブラマクロパラメータ
					pushq [ :MACPARAM, $& ]

				when /\A==/,/\A!=/,/\A\<=/,/\A\>=/,/\A>/,/\A</,/\A\&\&/,/\A\|\|/
          # 比較演算子
					pushq [ $&, $& ]

				when /\A\/\*.*/
          #C形式コメント開始
					pushq [ :COMMENT, $& ]
					@incomment = true

				when /\A[\+\-\*\/=(){},]/
          #演算子、カッコなどの記号
					pushq [ $&, $& ]
				else
					raise RuntimeError, "#{@fname}:#{@nowlineno}: fatal error! \{#{line_org}\}"
				end
				line = $'
				printf( "\#NOW LINE(%4d) #{line}\n", @nowlineno)
			end
		end
		pushq [ :EOL, nil ]
		@nowlineno += 1
	end
	pushq [ false, nil ]
	
	@yydebug = true
	do_parse
end

def next_token
	value = @q.shift
	@lineno = value.last
	value.delete_at(2)
	value
end

def pushq value
	value << @nowlineno
	@q.push value
end


def on_error( t, v, values )
    if v then
      line = v[0]
      v = v[1]
    else
      line = 'last'
    end
    raise Racc::ParseError, "#{@fname}:#{@lineno}: syntax error on #{v.inspect}"
  end


..end sp4.y modeval..id02331fd538

##### racc 1.4.5 generates ###

racc_reduce_table = [
 0, 0, :racc_error,
 1, 49, :_reduce_1,
 0, 50, :_reduce_2,
 3, 50, :_reduce_3,
 2, 50, :_reduce_4,
 2, 50, :_reduce_none,
 4, 50, :_reduce_6,
 3, 50, :_reduce_7,
 4, 50, :_reduce_8,
 0, 54, :_reduce_9,
 2, 54, :_reduce_none,
 3, 54, :_reduce_11,
 3, 55, :_reduce_12,
 8, 56, :_reduce_13,
 7, 56, :_reduce_14,
 1, 57, :_reduce_15,
 3, 57, :_reduce_16,
 4, 52, :_reduce_17,
 1, 52, :_reduce_none,
 2, 58, :_reduce_none,
 3, 58, :_reduce_none,
 2, 59, :_reduce_none,
 3, 59, :_reduce_none,
 1, 51, :_reduce_23,
 1, 51, :_reduce_24,
 1, 51, :_reduce_25,
 1, 51, :_reduce_26,
 1, 51, :_reduce_none,
 1, 51, :_reduce_none,
 1, 51, :_reduce_none,
 1, 51, :_reduce_none,
 1, 51, :_reduce_none,
 1, 51, :_reduce_none,
 3, 63, :_reduce_33,
 4, 63, :_reduce_34,
 6, 63, :_reduce_35,
 5, 63, :_reduce_36,
 1, 53, :_reduce_37,
 2, 53, :_reduce_38,
 6, 61, :_reduce_39,
 0, 69, :_reduce_40,
 2, 69, :_reduce_41,
 5, 69, :_reduce_42,
 3, 68, :_reduce_43,
 3, 68, :_reduce_44,
 4, 68, :_reduce_45,
 4, 68, :_reduce_46,
 4, 68, :_reduce_47,
 4, 68, :_reduce_48,
 3, 68, :_reduce_49,
 3, 68, :_reduce_50,
 1, 70, :_reduce_51,
 1, 70, :_reduce_52,
 1, 70, :_reduce_53,
 1, 70, :_reduce_54,
 1, 70, :_reduce_55,
 1, 70, :_reduce_56,
 1, 70, :_reduce_57,
 1, 70, :_reduce_58,
 5, 65, :_reduce_59,
 5, 64, :_reduce_60,
 0, 72, :_reduce_61,
 1, 72, :_reduce_62,
 2, 72, :_reduce_63,
 4, 73, :_reduce_64,
 3, 62, :_reduce_65,
 3, 62, :_reduce_66,
 3, 62, :_reduce_67,
 2, 60, :_reduce_68,
 4, 60, :_reduce_69,
 3, 60, :_reduce_70,
 2, 60, :_reduce_71,
 1, 60, :_reduce_72,
 2, 66, :_reduce_73,
 1, 71, :_reduce_none,
 3, 71, :_reduce_75,
 3, 71, :_reduce_76,
 3, 71, :_reduce_77,
 3, 71, :_reduce_78,
 3, 71, :_reduce_79,
 1, 74, :_reduce_none,
 1, 74, :_reduce_none,
 1, 74, :_reduce_none,
 1, 75, :_reduce_none,
 1, 75, :_reduce_none,
 1, 67, :_reduce_85,
 3, 67, :_reduce_86 ]

racc_reduce_n = 87

racc_shift_n = 164

racc_action_table = [
    11,   132,    94,    17,    21,    81,   147,    29,    46,     7,
    88,     9,    13,    14,   148,    19,    23,    58,    33,   142,
   143,    58,    46,    12,    48,    16,    50,    82,    83,    28,
     4,     6,    11,    19,    61,    17,    21,    43,    44,    29,
    46,     7,    88,     9,    13,    14,   133,    19,    23,    58,
    66,    43,    44,    21,    46,    12,    88,    16,     7,    63,
    64,    28,     4,     6,    11,    58,    57,    17,    21,    43,
    44,    29,   134,     7,   102,     9,    13,    14,   135,    19,
    23,    58,   129,    43,    44,    21,    46,    12,    88,    16,
     7,   137,   123,    28,     4,     6,    11,    19,    54,    17,
    21,   140,   141,    29,    46,     7,    88,     9,    13,    14,
   106,    19,    23,    19,   107,    43,    44,   128,    46,    12,
    88,    16,    50,    82,    83,    28,     4,     6,    11,   127,
   125,    17,    21,    43,    44,    29,   123,     7,   108,     9,
    13,    14,   109,    19,    23,   101,   100,    43,    44,    93,
    90,    12,    46,    16,    88,    81,   112,    28,     4,     6,
    11,    78,    77,    17,    21,    60,   118,    29,    81,     7,
   151,     9,    13,    14,    59,    19,    23,   154,    53,   142,
   143,    43,    44,    12,    46,    16,    88,   157,    50,    28,
     4,     6,    11,   158,    52,    17,    21,    51,   155,    29,
    81,     7,    96,     9,    13,    14,    99,    19,    23,    82,
    83,    84,    85,    43,    44,    12,   131,    16,   124,    19,
    81,    28,     4,     6,    32,    31,    46,    98,    88,   146,
    50,    43,    44,    46,     3,    88,    89,    50,    46,   nil,
    88,    92,    50,   nil,    72,    73,    74,    75,    76,    67,
    68,    69,   150,   nil,   149,    43,    44,    82,    83,    84,
    85,   nil,    43,    44,   nil,   nil,   nil,    43,    44,    95,
    72,    73,    74,    75,    76,    67,    68,    69,    82,    83,
    84,    85,    82,    83,    84,    85,    40,   nil,    41,    40,
   nil,    41,    40,   nil,    41,   160,   119,    40,   nil,    41,
   119,   nil,    38,    39,   nil,    38,    39,   nil,    38,    39,
    40,   nil,    41,    38,    39,    72,    73,    74,    75,    76,
    67,    68,    69,   nil,   nil,   nil,    38,    39,   nil,   110,
    72,    73,    74,    75,    76,    67,    68,    69,    70,    72,
    73,    74,    75,    76,    67,    68,    69,    72,    73,    74,
    75,    76,    67,    68,    69 ]

racc_action_check = [
   162,   106,    55,   162,   162,   138,   130,   162,    88,   162,
    88,   162,   162,   162,   138,   162,   162,    55,     7,   162,
   162,   130,    13,   162,    13,   162,    13,   116,   116,   162,
   162,   162,     2,     7,    34,     2,     2,    88,    88,     2,
    85,     2,    85,     2,     2,     2,   107,     2,     2,    34,
    36,    13,    13,   158,    84,     2,    84,     2,   158,    35,
    35,     2,     2,     2,    65,    36,    27,    65,    65,    85,
    85,    65,   108,    65,    65,    65,    65,    65,   109,    65,
    65,    27,   102,    84,    84,   151,    83,    65,    83,    65,
   151,   121,   121,    65,    65,    65,   156,   102,    22,   156,
   156,   125,   125,   156,    82,   156,    82,   156,   156,   156,
    77,   156,   156,    22,    77,    83,    83,   101,    81,   156,
    81,   156,    81,   117,   117,   156,   156,   156,   152,    96,
    93,   152,   152,    82,    82,   152,    90,   152,    78,   152,
   152,   152,    78,   152,   152,    64,    62,    81,    81,    53,
    51,   152,    80,   152,    80,    42,    80,   152,   152,   152,
   126,    39,    38,   126,   126,    29,    86,   126,    86,   126,
   140,   126,   126,   126,    28,   126,   126,   144,    21,   126,
   126,    80,    80,   126,   123,   126,   123,   149,   123,   126,
   126,   126,   104,   150,    17,   104,   104,    16,   145,   104,
   145,   104,    60,   104,   104,   104,    60,   104,   104,    45,
    45,    45,    45,   123,   123,   104,   104,   104,    91,    11,
    91,   104,   104,   104,     6,     3,   127,    60,   127,   127,
   127,    60,    60,    48,     1,    48,    48,    48,    52,   nil,
    52,    52,    52,   nil,    56,    56,    56,    56,    56,    56,
    56,    56,   139,   nil,   139,   127,   127,   111,   111,   111,
   111,   nil,    48,    48,   nil,   nil,   nil,    52,    52,    56,
   153,   153,   153,   153,   153,   153,   153,   153,   120,   120,
   120,   120,    87,    87,    87,    87,   143,   nil,   143,    12,
   nil,    12,    23,   nil,    23,   153,   120,    40,   nil,    40,
    87,   nil,   143,   143,   nil,    12,    12,   nil,    23,    23,
    71,   nil,    71,    40,    40,    79,    79,    79,    79,    79,
    79,    79,    79,   nil,   nil,   nil,    71,    71,   nil,    79,
    37,    37,    37,    37,    37,    37,    37,    37,    37,    41,
    41,    41,    41,    41,    41,    41,    41,   105,   105,   105,
   105,   105,   105,   105,   105 ]

racc_action_pointer = [
   nil,   234,    18,   225,   nil,   nil,   207,     4,   nil,   nil,
   nil,   190,   270,     5,   nil,   nil,   176,   175,   nil,   nil,
   nil,   162,    84,   273,   nil,   nil,   nil,    52,   157,   137,
   nil,   nil,   nil,   nil,    20,    45,    36,   324,   143,   142,
   278,   333,   133,   nil,   nil,   207,   nil,   nil,   216,   nil,
   nil,   136,   221,   132,   nil,   -12,   238,   nil,   nil,   nil,
   185,   nil,   132,   nil,   129,    50,   nil,   nil,   nil,   nil,
   nil,   291,   nil,   nil,   nil,   nil,   nil,    93,   121,   309,
   135,   101,    87,    69,    37,    23,   146,   280,    -9,   nil,
    95,   198,   nil,   111,   nil,   nil,   110,   nil,   nil,   nil,
   nil,   100,    68,   nil,   178,   341,   -19,    26,    52,    58,
   nil,   255,   nil,   nil,   nil,   nil,    25,   121,   nil,   nil,
   276,    51,   nil,   167,   nil,    81,   146,   209,   nil,   nil,
    -8,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   -17,   232,
   156,   nil,   nil,   267,   145,   178,   nil,   nil,   nil,   166,
   179,    67,   114,   264,   nil,   nil,    82,   nil,    35,   nil,
   nil,   nil,   -14,   nil ]

racc_action_default = [
    -2,   -87,    -1,   -87,   -72,   -32,   -87,   -87,   -18,   -23,
    -9,    -5,   -87,   -24,   -26,   -25,   -87,   -87,   -27,   -37,
   -28,   -87,   -87,   -87,   -29,    -4,   -30,   -87,   -87,   -87,
   -31,   164,   -73,   -19,   -87,    -2,   -87,   -87,   -87,   -87,
   -87,   -87,   -68,   -80,   -82,   -83,   -81,   -74,   -87,   -85,
   -84,   -87,   -87,   -87,    -3,   -87,   -87,    -7,   -38,   -71,
   -87,   -20,   -87,   -10,   -87,   -87,    -8,   -55,   -57,   -58,
    -2,   -87,   -51,   -52,   -54,   -53,   -56,   -87,   -87,   -87,
   -87,   -87,   -87,   -87,   -87,   -87,   -87,   -83,   -87,   -70,
   -61,   -87,   -33,   -87,    -6,    -2,   -81,   -66,   -67,   -65,
   -11,   -87,   -87,   -17,   -87,   -50,   -87,   -87,   -87,   -87,
   -49,   -43,   -44,   -86,   -77,   -78,   -76,   -75,   -69,   -79,
   -87,   -87,   -62,   -87,   -34,   -87,   -40,   -87,   -12,   -21,
   -87,   -59,   -45,   -46,   -47,   -48,   -63,   -60,   -87,   -87,
   -87,   -15,    -2,   -87,   -87,   -87,   -36,   -22,    -2,   -87,
   -87,   -87,   -41,   -87,   -39,   -35,   -64,   -16,   -87,   -14,
    -2,   -13,   -40,   -42 ]

racc_goto_table = [
     2,    37,    42,    34,   159,   122,     1,    36,    87,   103,
   144,   161,    56,   139,    80,    62,   121,    35,    55,    97,
   113,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,    79,
   nil,   nil,   nil,   nil,   nil,    65,   136,    86,   nil,   nil,
   111,    91,   114,   115,   116,   117,   163,   nil,   120,   nil,
   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,
   105,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,
   104,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,
   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,
   nil,   nil,   nil,   nil,   nil,   126,   nil,   nil,   130,   nil,
   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,
   nil,   nil,   138,   nil,   nil,   nil,   145,   nil,   nil,   nil,
   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,
   nil,   nil,   153,   nil,   nil,   nil,   nil,   nil,   nil,   nil,
   nil,   nil,   152,   nil,   nil,   nil,   nil,   nil,   156,   nil,
   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,
   162 ]

racc_goto_check = [
     2,    20,    19,     5,     4,    25,     1,     5,    23,    11,
    21,     4,    20,     9,    22,     7,    24,     6,     5,    26,
    27,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,    20,
   nil,   nil,   nil,   nil,   nil,     2,    25,    19,   nil,   nil,
    23,    19,    23,    23,    23,    23,    21,   nil,    23,   nil,
   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,
    20,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,
     2,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,
   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,
   nil,   nil,   nil,   nil,   nil,     2,   nil,   nil,     5,   nil,
   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,
   nil,   nil,    19,   nil,   nil,   nil,    19,   nil,   nil,   nil,
   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,
   nil,   nil,    20,   nil,   nil,   nil,   nil,   nil,   nil,   nil,
   nil,   nil,     2,   nil,   nil,   nil,   nil,   nil,     2,   nil,
   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,
     2 ]

racc_goto_pointer = [
   nil,     6,     0,   nil,  -147,    -4,     7,   -20,   nil,  -112,
   nil,   -56,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   -11,
   -11,  -116,   -27,   -40,   -74,   -85,   -41,   -61 ]

racc_goto_default = [
   nil,   nil,   nil,    22,    25,    27,   nil,   nil,     8,   nil,
    10,   nil,    15,    18,    20,    24,    26,    30,     5,   nil,
   nil,   nil,    71,    45,   nil,   nil,    47,    49 ]

racc_token_table = {
 false => 0,
 Object.new => 1,
 "/" => 2,
 "*" => 3,
 "-" => 4,
 "+" => 5,
 "==" => 6,
 "!=" => 7,
 "<" => 8,
 ">" => 9,
 "<=" => 10,
 ">=" => 11,
 "&&" => 12,
 "||" => 13,
 :EOL => 14,
 :DEFVAR => 15,
 :TYPE => 16,
 :IDENT => 17,
 :DEFFUNC => 18,
 "(" => 19,
 ")" => 20,
 :VARREF => 21,
 "," => 22,
 "{" => 23,
 "}" => 24,
 :LABEL => 25,
 :COMMAND => 26,
 :MACRO => 27,
 "=" => 28,
 :COMMENT => 29,
 :IF => 30,
 :THEN => 31,
 :ENDIF => 32,
 :ELSE => 33,
 :ELSIF => 34,
 :FLAG_ON => 35,
 :FLAG_OFF => 36,
 :WHILE => 37,
 :ENDWHILE => 38,
 :SWITCH => 39,
 :ENDSWITCH => 40,
 :CASE => 41,
 :funcall => 42,
 :EVENT_START => 43,
 :EVENT_END => 44,
 :DEFINE_LOCAL => 45,
 :NUMBER => 46,
 :MACPARAM => 47 }

racc_use_result_var = true

racc_nt_base = 48

Racc_arg = [
 racc_action_table,
 racc_action_check,
 racc_action_default,
 racc_action_pointer,
 racc_goto_table,
 racc_goto_check,
 racc_goto_default,
 racc_goto_pointer,
 racc_nt_base,
 racc_reduce_table,
 racc_token_table,
 racc_shift_n,
 racc_reduce_n,
 racc_use_result_var ]

Racc_token_to_s_table = [
'$end',
'error',
'"/"',
'"*"',
'"-"',
'"+"',
'"=="',
'"!="',
'"<"',
'">"',
'"<="',
'">="',
'"&&"',
'"||"',
'EOL',
'DEFVAR',
'TYPE',
'IDENT',
'DEFFUNC',
'"("',
'")"',
'VARREF',
'","',
'"{"',
'"}"',
'LABEL',
'COMMAND',
'MACRO',
'"="',
'COMMENT',
'IF',
'THEN',
'ENDIF',
'ELSE',
'ELSIF',
'FLAG_ON',
'FLAG_OFF',
'WHILE',
'ENDWHILE',
'SWITCH',
'ENDSWITCH',
'CASE',
'funcall',
'EVENT_START',
'EVENT_END',
'DEFINE_LOCAL',
'NUMBER',
'MACPARAM',
'$start',
'program',
'stmt_list',
'stmt',
'stmt_block',
'comments',
'defvar_list',
'def_var',
'def_func',
'param',
'block_start',
'block_end',
'cmd',
'if_stmt',
'assign',
'callfunc',
'switch_stmt',
'while_stmt',
'def_local',
'args',
'cmp_expr',
'else_stmt',
'cmp_ident',
'expr',
'case_stmt_list',
'case_stmt',
'primary',
'arg']

Racc_debug_parser = false

##### racc system variables end #####

 # reduce 0 omitted

module_eval <<'.,.,', 'sp4.y', 45
  def _reduce_1( val, _values, result )
						result = RootNode.new( val[0] )
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 60
  def _reduce_2( val, _values, result )
						result = []
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 64
  def _reduce_3( val, _values, result )
						result.push val[1]
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 68
  def _reduce_4( val, _values, result )
						result.push val[1]
   result
  end
.,.,

 # reduce 5 omitted

module_eval <<'.,.,', 'sp4.y', 74
  def _reduce_6( val, _values, result )
						result.push val[1]
						#result.push RawNode.new(val[2])
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 78
  def _reduce_7( val, _values, result )
						#result.push RawNode.new(val[1])
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 82
  def _reduce_8( val, _values, result )
						#result.push RawNode.new(val[2])
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 94
  def _reduce_9( val, _values, result )
						result = []
   result
  end
.,.,

 # reduce 10 omitted

module_eval <<'.,.,', 'sp4.y', 99
  def _reduce_11( val, _values, result )
						result.push val[1]
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 108
  def _reduce_12( val, _values, result )
						result = DefVarNode.new(val[1], val[2])
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 118
  def _reduce_13( val, _values, result )
						result = DefFuncNode.new( val[1], val[2], val[4], val[7] )
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 122
  def _reduce_14( val, _values, result )
						result = DefFuncNode.new( val[1], val[2], nil, val[6] )
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 132
  def _reduce_15( val, _values, result )
						result = val
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 136
  def _reduce_16( val, _values, result )
						result.push val[2]
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 145
  def _reduce_17( val, _values, result )
						result = BlockNode.new(val[1], val[2])
   result
  end
.,.,

 # reduce 18 omitted

 # reduce 19 omitted

 # reduce 20 omitted

 # reduce 21 omitted

 # reduce 22 omitted

module_eval <<'.,.,', 'sp4.y', 177
  def _reduce_23( val, _values, result )
						result = RawNode.new(val[0]);
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 181
  def _reduce_24( val, _values, result )
						result = CmdNode.new( val, nil )
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 185
  def _reduce_25( val, _values, result )
						result = CmdNode.new( val[0].shift, val[0] )
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 189
  def _reduce_26( val, _values, result )
						result = RawNode.new(val[0]);
   result
  end
.,.,

 # reduce 27 omitted

 # reduce 28 omitted

 # reduce 29 omitted

 # reduce 30 omitted

 # reduce 31 omitted

 # reduce 32 omitted

module_eval <<'.,.,', 'sp4.y', 211
  def _reduce_33( val, _values, result )
						result = CallFuncNode.new( nil, val[0], nil )
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 215
  def _reduce_34( val, _values, result )
						result = CallFuncNode.new( nil, val[0], val[2] )
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 219
  def _reduce_35( val, _values, result )
						result = CallFuncNode.new( val[0], val[2], val[4] )
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 223
  def _reduce_36( val, _values, result )
						result = CallFuncNode.new( val[0], val[2], nil )
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 233
  def _reduce_37( val, _values, result )
						result = val
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 237
  def _reduce_38( val, _values, result )
						result.push val[1]
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 246
  def _reduce_39( val, _values, result )
						result = IfNode.new(val[1],val[3],val[4])
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 258
  def _reduce_40( val, _values, result )
						result = nil
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 262
  def _reduce_41( val, _values, result )
						result = val[1]
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 266
  def _reduce_42( val, _values, result )
						result = [IfNode.new(val[1], val[3], val[4])]
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 283
  def _reduce_43( val, _values, result )
            left = "_PUSH_WORK #{val[0].sub(/\A\$/,"")}"
            right = "_PUSH_VALUE #{val[2]}"
            result = CompareNode.new(val[1], left, right)
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 289
  def _reduce_44( val, _values, result )
            left = "_PUSH_WORK #{val[0].sub(/\A\$/,"")}"
            right = "_PUSH_WORK #{val[2].sub(/\A\$/,"")}"
            result = CompareNode.new(val[1], left, right)
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 295
  def _reduce_45( val, _values, result )
            left = "_PUSH_FLAG #{val[2]}"
            right = "_PUSH_VALUE TRUE"
            result = CompareNode.new('CMPID_EQ', left, right)
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 301
  def _reduce_46( val, _values, result )
            left = "_PUSH_FLAG #{val[2].sub(/\A\$/,"")}"
            right = "_PUSH_VALUE TRUE"
            result = CompareNode.new('CMPID_EQ', left, right)
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 307
  def _reduce_47( val, _values, result )
            left = "_PUSH_FLAG #{val[2]}"
            right = "_PUSH_VALUE FALSE"
            result = CompareNode.new('CMPID_EQ', left, right)
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 313
  def _reduce_48( val, _values, result )
            left = "_PUSH_FLAG #{val[2].sub(/\A\$/,"")}"
            right = "_PUSH_VALUE FALSE"
            result = CompareNode.new('CMPID_EQ', left, right)
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 317
  def _reduce_49( val, _values, result )
            result = val[1]
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 321
  def _reduce_50( val, _values, result )
            result = LogicalCompareNode.new(val[1], val[0], val[2])
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 330
  def _reduce_51( val, _values, result )
						result = 'CMPID_EQ'	#result = 'EQ'
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 334
  def _reduce_52( val, _values, result )
						result = 'CMPID_NE'	#result = 'NE'
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 338
  def _reduce_53( val, _values, result )
						result = 'CMPID_GT'	#result = 'GT'
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 342
  def _reduce_54( val, _values, result )
						result = 'CMPID_LT'	#result = 'LT'
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 346
  def _reduce_55( val, _values, result )
						result = 'CMPID_GT_EQ'	#result = 'GE'
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 350
  def _reduce_56( val, _values, result )
						result = 'CMPID_LT_EQ'	#result = 'LE'
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 354
  def _reduce_57( val, _values, result )
            result = 'CMPID_AND'
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 358
  def _reduce_58( val, _values, result )
            result = 'CMPID_OR'
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 368
  def _reduce_59( val, _values, result )
            result = WhileNode.new(val[1], val[3])
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 377
  def _reduce_60( val, _values, result )
            result = SwitchNode.new(val[1].sub(/\A\$/,""), val[3])
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 388
  def _reduce_61( val, _values, result )
            result = []
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 392
  def _reduce_62( val, _values, result )
            result = [ val[0] ]
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 396
  def _reduce_63( val, _values, result )
            result.push val[1]
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 405
  def _reduce_64( val, _values, result )
            result = CaseNode.new(val[1], val[3])
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 417
  def _reduce_65( val, _values, result )
						result = AssignNode.new(val[0], val[2], :VARREF)
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 421
  def _reduce_66( val, _values, result )
						result = AssignNode.new(val[0], val[2], nil)
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 425
  def _reduce_67( val, _values, result )
						result = AssignNode.new(val[0], val[2], funcall)
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 434
  def _reduce_68( val, _values, result )
						result = [val[0],val[1]]
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 438
  def _reduce_69( val, _values, result )
						result = [val[0], ['('] + val[2] + [')'] ]
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 442
  def _reduce_70( val, _values, result )
            result = [ val[0], "()" ]
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 446
  def _reduce_71( val, _values, result )
						result = [val[0],val[1]]
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 450
  def _reduce_72( val, _values, result )
            result = [ val[0], nil ]
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 459
  def _reduce_73( val, _values, result )
            result = DefLocalVarNode.new( val[1] )
   result
  end
.,.,

 # reduce 74 omitted

module_eval <<'.,.,', 'sp4.y', 473
  def _reduce_75( val, _values, result )
						result = val
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 477
  def _reduce_76( val, _values, result )
						result = val
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 481
  def _reduce_77( val, _values, result )
						result = val
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 485
  def _reduce_78( val, _values, result )
						result = val
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 489
  def _reduce_79( val, _values, result )
						result = val
   result
  end
.,.,

 # reduce 80 omitted

 # reduce 81 omitted

 # reduce 82 omitted

 # reduce 83 omitted

 # reduce 84 omitted

module_eval <<'.,.,', 'sp4.y', 518
  def _reduce_85( val, _values, result )
						result = val
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 523
  def _reduce_86( val, _values, result )
						result.push val[1]
						result.push val[2]
   result
  end
.,.,

 def _reduce_none( val, _values, result )
  result
 end

  end   # class Parser

end   # module PmScript
