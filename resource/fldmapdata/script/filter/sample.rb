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

module_eval <<'..end sp4.y modeval..ide9e7a3badd', 'sp4.y', 571

def initialize
end

#予約語定義
RESERVED = {
	'IF'	=> :IF,
	'THEN'	=> :THEN,
	'ELSE'	=> :ELSE,
	'ELSIF'	=> :ELSIF,
	'ENDIF'	=> :ENDIF,
	'DEFFUNC'	=> :DEFFUNC,
	'FLAG_ON'	=> :FLAG_ON,
	'FLAG_OFF'	=> :FLAG_OFF,
  'EVENT_START' => :EVENT_START,
  'EVENT_END' => :EVENT_END,
  'INIT_EVENT_START' => :INIT_EVENT_START,
  'INIT_EVENT_END' => :INIT_EVENT_END,
  'SWITCH' => :SWITCH,
  'CASE' => :CASE,
  'DEFAULT' => :DEFAULT,
  'ENDSWITCH' => :ENDSWITCH,
  'WHILE' => :WHILE,
  'ENDWHILE' => :ENDWHILE,
  'DEFINE_LOCAL' => :DEFINE_LOCAL,
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

        when /\A\+=/, /\A\-=/
          # 複合代入演算子
          pushq [ :OP_COMP_ASSIGN, $& ]

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


..end sp4.y modeval..ide9e7a3badd

##### racc 1.4.5 generates ###

racc_reduce_table = [
 0, 0, :racc_error,
 1, 52, :_reduce_1,
 0, 53, :_reduce_2,
 3, 53, :_reduce_3,
 2, 53, :_reduce_4,
 2, 53, :_reduce_none,
 4, 53, :_reduce_6,
 3, 53, :_reduce_7,
 4, 53, :_reduce_8,
 0, 57, :_reduce_9,
 2, 57, :_reduce_none,
 3, 57, :_reduce_none,
 3, 57, :_reduce_12,
 4, 57, :_reduce_13,
 8, 59, :_reduce_14,
 7, 59, :_reduce_15,
 1, 60, :_reduce_16,
 3, 60, :_reduce_17,
 4, 55, :_reduce_18,
 1, 55, :_reduce_none,
 2, 61, :_reduce_none,
 3, 61, :_reduce_none,
 2, 62, :_reduce_none,
 3, 62, :_reduce_none,
 1, 54, :_reduce_24,
 1, 54, :_reduce_25,
 1, 54, :_reduce_26,
 1, 54, :_reduce_27,
 1, 54, :_reduce_none,
 1, 54, :_reduce_none,
 1, 54, :_reduce_none,
 1, 54, :_reduce_none,
 1, 54, :_reduce_none,
 1, 54, :_reduce_none,
 1, 54, :_reduce_none,
 3, 67, :_reduce_35,
 4, 67, :_reduce_36,
 6, 67, :_reduce_37,
 5, 67, :_reduce_38,
 1, 56, :_reduce_39,
 2, 56, :_reduce_40,
 6, 64, :_reduce_41,
 0, 72, :_reduce_42,
 2, 72, :_reduce_43,
 5, 72, :_reduce_44,
 3, 71, :_reduce_45,
 3, 71, :_reduce_46,
 4, 71, :_reduce_47,
 4, 71, :_reduce_48,
 4, 71, :_reduce_49,
 4, 71, :_reduce_50,
 3, 71, :_reduce_51,
 3, 71, :_reduce_52,
 1, 73, :_reduce_53,
 1, 73, :_reduce_54,
 1, 73, :_reduce_55,
 1, 73, :_reduce_56,
 1, 73, :_reduce_57,
 1, 73, :_reduce_58,
 1, 73, :_reduce_59,
 1, 73, :_reduce_60,
 5, 69, :_reduce_61,
 6, 68, :_reduce_62,
 0, 75, :_reduce_63,
 1, 75, :_reduce_64,
 2, 75, :_reduce_65,
 4, 77, :_reduce_66,
 0, 76, :_reduce_none,
 2, 76, :_reduce_none,
 3, 65, :_reduce_69,
 3, 65, :_reduce_70,
 3, 65, :_reduce_71,
 3, 66, :_reduce_72,
 3, 66, :_reduce_73,
 2, 63, :_reduce_74,
 4, 63, :_reduce_75,
 3, 63, :_reduce_76,
 2, 63, :_reduce_77,
 1, 63, :_reduce_78,
 2, 63, :_reduce_79,
 1, 63, :_reduce_80,
 2, 58, :_reduce_81,
 1, 74, :_reduce_none,
 3, 74, :_reduce_83,
 3, 74, :_reduce_84,
 3, 74, :_reduce_85,
 3, 74, :_reduce_86,
 3, 74, :_reduce_87,
 1, 78, :_reduce_none,
 1, 78, :_reduce_none,
 1, 78, :_reduce_none,
 1, 79, :_reduce_none,
 1, 79, :_reduce_none,
 1, 70, :_reduce_93,
 3, 70, :_reduce_94 ]

racc_reduce_n = 95

racc_shift_n = 175

racc_action_table = [
    19,    23,   129,    26,    86,    87,    30,    62,     6,    81,
    13,    18,    21,   106,    25,    28,    39,   139,   153,   151,
    51,    83,    15,    63,    24,   150,    97,    39,   131,   138,
     4,     8,    11,    14,    20,    19,    23,   148,    26,    25,
    25,    30,    25,     6,   137,    13,    18,    21,    38,    25,
    28,    39,    49,    50,   136,    51,    52,    15,    54,    24,
    20,    66,    39,    51,    83,     4,     8,    11,    14,    20,
    19,    23,    65,    26,   135,    39,    30,    90,     6,    37,
    13,    18,    21,   162,    25,    28,    25,    49,    50,    51,
    83,    39,    15,    25,    24,    49,    50,    39,    51,    83,
     4,     8,    11,    14,    20,    19,    23,    23,    26,    81,
   156,    30,   157,     6,     6,    13,    18,    21,   159,    25,
    28,    49,    50,    56,    51,    83,    23,    15,    40,    24,
    49,    50,   128,     6,    81,     4,     8,    11,    14,    20,
    19,    23,    44,    26,    45,   166,    30,    81,     6,   126,
    13,    18,    21,    25,    25,    28,    49,    50,    41,    42,
   109,   158,    15,   110,    24,    86,    87,    88,    89,   124,
     4,     8,    11,    14,    20,    19,    23,   107,    26,   161,
   108,    30,   119,     6,    57,    13,    18,    21,    58,    25,
    28,    86,    87,   153,   151,    51,    83,    15,    54,    24,
    86,    87,    88,    89,   164,     4,     8,    11,    14,    20,
    19,    23,   117,    26,    81,    59,    30,   119,     6,   132,
    13,    18,    21,   167,    25,    28,   168,    49,    50,   142,
   143,    67,    15,    51,    24,    68,   105,   126,   146,    92,
     4,     8,    11,    14,    20,    19,    23,    35,    26,    51,
    83,    30,   115,     6,    91,    13,    18,    21,    34,    25,
    28,    51,    83,    85,    54,    49,    50,    15,   140,    24,
   101,     3,   nil,   103,   nil,     4,     8,    11,    14,    20,
   nil,    49,    50,    51,    83,   155,    54,    51,    83,    94,
    54,   nil,   nil,    49,    50,   102,    51,    83,   nil,    54,
   nil,   nil,    49,    50,   nil,   nil,    73,    74,    76,    77,
    78,    69,    70,    71,   nil,    49,    50,   nil,   nil,    49,
    50,   nil,   nil,    44,   nil,    45,   nil,   nil,    49,    50,
    99,    73,    74,    76,    77,    78,    69,    70,    71,    41,
    42,    44,   nil,    45,    44,   nil,    45,    44,   nil,    45,
    86,    87,    88,    89,   nil,   171,   nil,    41,    42,   nil,
    41,    42,   nil,    41,    42,    73,    74,    76,    77,    78,
    69,    70,    71,    86,    87,    88,    89,   nil,   113,    73,
    74,    76,    77,    78,    69,    70,    71,    72,    73,    74,
    76,    77,    78,    69,    70,    71,    73,    74,    76,    77,
    78,    69,    70,    71 ]

racc_action_check = [
   173,   173,    95,   173,   122,   122,   173,    30,   173,    48,
   173,   173,   173,    64,   173,   173,    95,   110,   173,   173,
    89,    89,   173,    30,   173,   132,    60,    64,    96,   109,
   173,   173,   173,   173,   173,     2,     2,   130,     2,   132,
    60,     2,    96,     2,   108,     2,     2,     2,    10,     2,
     2,   130,    89,    89,   107,    18,    18,     2,    18,     2,
    60,    36,    10,    88,    88,     2,     2,     2,     2,     2,
   170,   170,    33,   170,   101,    36,   170,    55,   170,     6,
   170,   170,   170,   149,   170,   170,    33,    18,    18,    87,
    87,    55,   170,     6,   170,    88,    88,   149,    86,    86,
   170,   170,   170,   170,   170,   165,   165,   167,   165,   144,
   141,   165,   141,   165,   167,   165,   165,   165,   144,   165,
   165,    87,    87,    20,    83,    83,   158,   165,    11,   165,
    86,    86,    93,   158,    93,   165,   165,   165,   165,   165,
   160,   160,    28,   160,    28,   154,   160,   154,   160,    92,
   160,   160,   160,    19,   160,   160,    83,    83,    28,    28,
    68,   142,   160,    68,   160,   118,   118,   118,   118,    91,
   160,   160,   160,   160,   160,   134,   134,    67,   134,   147,
    67,   134,   118,   134,    23,   134,   134,   134,    24,   134,
   134,   123,   123,   134,   134,    81,    81,   134,    81,   134,
    84,    84,    84,    84,   152,   134,   134,   134,   134,   134,
    98,    98,    82,    98,    82,    26,    98,    84,    98,    98,
    98,    98,    98,   156,    98,    98,   157,    81,    81,   124,
   124,    41,    98,    63,    98,    42,    63,   127,   127,    58,
    98,    98,    98,    98,    98,   111,   111,     4,   111,    80,
    80,   111,    80,   111,    57,   111,   111,   111,     3,   111,
   111,    52,    52,    52,    52,    63,    63,   111,   111,   111,
    62,     1,   nil,    62,   nil,   111,   111,   111,   111,   111,
   nil,    80,    80,   135,   135,   135,   135,    59,    59,    59,
    59,   nil,   nil,    52,    52,    62,   126,   126,   nil,   126,
   nil,   nil,    62,    62,   nil,   nil,    61,    61,    61,    61,
    61,    61,    61,    61,   nil,   135,   135,   nil,   nil,    59,
    59,   nil,   nil,   151,   nil,   151,   nil,   nil,   126,   126,
    61,   163,   163,   163,   163,   163,   163,   163,   163,   151,
   151,    15,   nil,    15,    44,   nil,    44,    75,   nil,    75,
   114,   114,   114,   114,   nil,   163,   nil,    15,    15,   nil,
    44,    44,   nil,    75,    75,    79,    79,    79,    79,    79,
    79,    79,    79,    53,    53,    53,    53,   nil,    79,    43,
    43,    43,    43,    43,    43,    43,    43,    43,   112,   112,
   112,   112,   112,   112,   112,   112,    45,    45,    45,    45,
    45,    45,    45,    45 ]

racc_action_pointer = [
   nil,   271,    21,   258,   230,   nil,    65,   nil,   nil,   nil,
    34,   111,   nil,   nil,   nil,   323,   nil,   nil,    38,   125,
   106,   nil,   nil,   168,   168,   nil,   197,   nil,   124,   nil,
   -20,   nil,   nil,    58,   nil,   nil,    47,   nil,   nil,   nil,
   nil,   213,   217,   373,   326,   390,   nil,   nil,   -12,   nil,
   nil,   nil,   244,   371,   nil,    63,   nil,   237,   225,   270,
    12,   300,   253,   216,    -1,   nil,   nil,   160,   143,   nil,
   nil,   nil,   nil,   nil,   nil,   329,   nil,   nil,   nil,   359,
   232,   178,   193,   107,   198,   nil,    81,    72,    46,     3,
   nil,   151,   109,   113,   nil,   -12,    14,   nil,   196,   nil,
   nil,    56,   nil,   nil,   nil,   nil,   nil,    35,    25,    10,
    -2,   231,   382,   nil,   348,   nil,   nil,   nil,   163,   nil,
   nil,   nil,     2,   189,   210,   nil,   279,   197,   nil,   nil,
    23,   nil,    11,   nil,   161,   266,   nil,   nil,   nil,   nil,
   nil,    91,   147,   nil,    88,   nil,   nil,   140,   nil,    69,
   nil,   305,   173,   nil,   126,   nil,   209,   206,   111,   nil,
   126,   nil,   nil,   325,   nil,    91,   nil,    92,   nil,   nil,
    56,   nil,   nil,   -14,   nil ]

racc_action_default = [
    -2,   -95,    -1,   -95,   -95,   -30,   -95,    -4,   -78,   -31,
   -95,   -95,   -32,   -24,   -80,   -95,   -34,   -33,   -25,    -5,
   -95,   -27,   -19,   -95,   -95,   -39,   -95,    -9,   -95,   -26,
   -95,   -28,   -29,   -95,   175,   -77,   -95,   -20,    -7,   -40,
   -79,   -95,   -95,   -95,   -95,   -95,   -82,   -93,   -74,   -88,
   -90,   -89,   -95,   -91,   -92,   -95,   -81,   -95,   -95,   -95,
    -2,   -95,   -95,   -95,   -95,    -3,   -21,   -95,   -95,   -57,
   -59,   -60,    -2,   -53,   -54,   -95,   -56,   -55,   -58,   -95,
   -95,   -95,   -95,   -95,   -91,   -76,   -95,   -95,   -95,   -95,
    -8,   -95,   -63,   -95,   -35,   -95,   -95,   -10,   -95,    -2,
   -70,   -89,   -71,   -69,   -73,   -72,    -6,   -95,   -95,   -95,
   -95,   -95,   -52,   -51,   -45,   -46,   -94,   -75,   -95,   -87,
   -85,   -86,   -84,   -83,   -95,   -64,   -95,   -67,   -36,   -11,
   -95,   -12,   -95,   -18,   -42,   -95,   -47,   -48,   -49,   -50,
   -61,   -95,   -95,   -16,   -95,   -65,    -2,   -95,   -13,   -95,
   -22,   -95,   -95,    -2,   -95,   -38,   -95,   -95,   -95,    -2,
   -68,   -62,   -23,   -95,   -41,   -43,   -37,   -95,   -17,   -15,
   -66,    -2,   -14,   -42,   -44 ]

racc_goto_table = [
     2,    43,    36,    48,   152,    84,     1,   169,   100,   104,
   125,   133,    80,   141,    61,    55,   172,   127,   147,    96,
    60,   116,   nil,   nil,   nil,   nil,   nil,   nil,   nil,    64,
    79,   nil,   nil,   114,   nil,   nil,   118,    82,   nil,   120,
   121,   122,   123,   174,    93,   145,   nil,   nil,   nil,   nil,
   nil,   nil,   nil,   nil,   nil,   nil,    95,   nil,   nil,   nil,
    98,   112,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,
   nil,   nil,   111,   nil,   nil,   nil,   nil,   nil,   nil,   nil,
   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,
   nil,   nil,   130,   nil,   nil,   nil,   nil,   nil,   nil,   134,
   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,
   nil,   144,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,
   154,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   149,   nil,
   nil,   nil,   nil,   nil,   nil,   nil,   nil,   163,   nil,   nil,
   nil,   nil,   nil,   nil,   nil,   nil,   160,   nil,   nil,   nil,
   nil,   nil,   nil,   165,   nil,   nil,   nil,   nil,   nil,   170,
   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,
   nil,   173 ]

racc_goto_check = [
     2,    20,     5,    19,    21,    23,     1,     4,    27,    27,
    26,    11,    22,     9,    20,     5,     4,    24,    25,     7,
     6,    28,   nil,   nil,   nil,   nil,   nil,   nil,   nil,     5,
    20,   nil,   nil,    23,   nil,   nil,    23,    19,   nil,    23,
    23,    23,    23,    21,    19,    26,   nil,   nil,   nil,   nil,
   nil,   nil,   nil,   nil,   nil,   nil,     5,   nil,   nil,   nil,
     2,    20,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,
   nil,   nil,     2,   nil,   nil,   nil,   nil,   nil,   nil,   nil,
   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,
   nil,   nil,     5,   nil,   nil,   nil,   nil,   nil,   nil,     2,
   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,
   nil,    19,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,
    19,   nil,   nil,   nil,   nil,   nil,   nil,   nil,     5,   nil,
   nil,   nil,   nil,   nil,   nil,   nil,   nil,    20,   nil,   nil,
   nil,   nil,   nil,   nil,   nil,   nil,     2,   nil,   nil,   nil,
   nil,   nil,   nil,     2,   nil,   nil,   nil,   nil,   nil,     2,
   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   nil,
   nil,     2 ]

racc_goto_pointer = [
   nil,     6,     0,   nil,  -151,    -4,    -7,   -41,   nil,  -111,
   nil,   -87,   nil,   nil,   nil,   nil,   nil,   nil,   nil,   -15,
   -14,  -130,   -33,   -47,   -75,  -109,   -82,   -54,   -60 ]

racc_goto_default = [
   nil,   nil,   nil,    33,     7,    10,   nil,    16,    22,   nil,
    27,   nil,    29,    31,    32,     5,     9,    12,    17,   nil,
   nil,   nil,    75,    53,   nil,   nil,   nil,    46,    47 ]

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
 :DEFFUNC => 15,
 :TYPE => 16,
 :IDENT => 17,
 "(" => 18,
 ")" => 19,
 :VARREF => 20,
 "," => 21,
 "{" => 22,
 "}" => 23,
 :LABEL => 24,
 :COMMAND => 25,
 :MACRO => 26,
 "=" => 27,
 :COMMENT => 28,
 :IF => 29,
 :THEN => 30,
 :ENDIF => 31,
 :ELSE => 32,
 :ELSIF => 33,
 :FLAG_ON => 34,
 :FLAG_OFF => 35,
 :WHILE => 36,
 :ENDWHILE => 37,
 :SWITCH => 38,
 :ENDSWITCH => 39,
 :CASE => 40,
 :DEFAULT => 41,
 :funcall => 42,
 :OP_COMP_ASSIGN => 43,
 :EVENT_START => 44,
 :EVENT_END => 45,
 :INIT_EVENT_START => 46,
 :INIT_EVENT_END => 47,
 :DEFINE_LOCAL => 48,
 :NUMBER => 49,
 :MACPARAM => 50 }

racc_use_result_var = true

racc_nt_base = 51

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
'DEFFUNC',
'TYPE',
'IDENT',
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
'DEFAULT',
'funcall',
'OP_COMP_ASSIGN',
'EVENT_START',
'EVENT_END',
'INIT_EVENT_START',
'INIT_EVENT_END',
'DEFINE_LOCAL',
'NUMBER',
'MACPARAM',
'$start',
'program',
'stmt_list',
'stmt',
'stmt_block',
'comments',
'def_local_list',
'def_local',
'def_func',
'param',
'block_start',
'block_end',
'cmd',
'if_stmt',
'assign',
'compound_assign',
'callfunc',
'switch_stmt',
'while_stmt',
'args',
'cmp_expr',
'else_stmt',
'cmp_ident',
'expr',
'case_stmt_list',
'default_stmt',
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

 # reduce 11 omitted

module_eval <<'.,.,', 'sp4.y', 100
  def _reduce_12( val, _values, result )
            result.push val[1]
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 104
  def _reduce_13( val, _values, result )
            result.push val[1]
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 113
  def _reduce_14( val, _values, result )
						result = DefFuncNode.new( val[1], val[2], val[4], val[7] )
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 117
  def _reduce_15( val, _values, result )
						result = DefFuncNode.new( val[1], val[2], nil, val[6] )
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 127
  def _reduce_16( val, _values, result )
						result = val
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 131
  def _reduce_17( val, _values, result )
						result.push val[2]
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 140
  def _reduce_18( val, _values, result )
						result = BlockNode.new(val[1], val[2])
   result
  end
.,.,

 # reduce 19 omitted

 # reduce 20 omitted

 # reduce 21 omitted

 # reduce 22 omitted

 # reduce 23 omitted

module_eval <<'.,.,', 'sp4.y', 172
  def _reduce_24( val, _values, result )
						result = RawNode.new(val[0]);
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 176
  def _reduce_25( val, _values, result )
						result = CmdNode.new( val, nil )
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 180
  def _reduce_26( val, _values, result )
						result = CmdNode.new( val[0].shift, val[0] )
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 184
  def _reduce_27( val, _values, result )
						result = RawNode.new(val[0]);
   result
  end
.,.,

 # reduce 28 omitted

 # reduce 29 omitted

 # reduce 30 omitted

 # reduce 31 omitted

 # reduce 32 omitted

 # reduce 33 omitted

 # reduce 34 omitted

module_eval <<'.,.,', 'sp4.y', 207
  def _reduce_35( val, _values, result )
						result = CallFuncNode.new( nil, val[0], nil )
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 211
  def _reduce_36( val, _values, result )
						result = CallFuncNode.new( nil, val[0], val[2] )
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 215
  def _reduce_37( val, _values, result )
						result = CallFuncNode.new( val[0], val[2], val[4] )
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 219
  def _reduce_38( val, _values, result )
						result = CallFuncNode.new( val[0], val[2], nil )
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 229
  def _reduce_39( val, _values, result )
						result = val
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 233
  def _reduce_40( val, _values, result )
						result.push val[1]
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 242
  def _reduce_41( val, _values, result )
						result = IfNode.new(val[1],val[3],val[4])
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 254
  def _reduce_42( val, _values, result )
						result = nil
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 258
  def _reduce_43( val, _values, result )
						result = val[1]
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 262
  def _reduce_44( val, _values, result )
						result = [IfNode.new(val[1], val[3], val[4])]
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 279
  def _reduce_45( val, _values, result )
            left = "_PUSH_WORK #{val[0].sub(/\A\$/,"")}"
            right = "_PUSH_VALUE #{val[2]}"
            result = CompareNode.new(val[1], left, right)
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 285
  def _reduce_46( val, _values, result )
            left = "_PUSH_WORK #{val[0].sub(/\A\$/,"")}"
            right = "_PUSH_WORK #{val[2].sub(/\A\$/,"")}"
            result = CompareNode.new(val[1], left, right)
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 291
  def _reduce_47( val, _values, result )
            left = "_PUSH_FLAG #{val[2]}"
            right = "_PUSH_VALUE TRUE"
            result = CompareNode.new('CMPID_EQ', left, right)
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 297
  def _reduce_48( val, _values, result )
            left = "_PUSH_FLAG #{val[2].sub(/\A\$/,"")}"
            right = "_PUSH_VALUE TRUE"
            result = CompareNode.new('CMPID_EQ', left, right)
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 303
  def _reduce_49( val, _values, result )
            left = "_PUSH_FLAG #{val[2]}"
            right = "_PUSH_VALUE FALSE"
            result = CompareNode.new('CMPID_EQ', left, right)
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 309
  def _reduce_50( val, _values, result )
            left = "_PUSH_FLAG #{val[2].sub(/\A\$/,"")}"
            right = "_PUSH_VALUE FALSE"
            result = CompareNode.new('CMPID_EQ', left, right)
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 313
  def _reduce_51( val, _values, result )
            result = val[1]
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 317
  def _reduce_52( val, _values, result )
            result = LogicalCompareNode.new(val[1], val[0], val[2])
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 326
  def _reduce_53( val, _values, result )
						result = 'CMPID_EQ'	#result = 'EQ'
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 330
  def _reduce_54( val, _values, result )
						result = 'CMPID_NE'	#result = 'NE'
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 334
  def _reduce_55( val, _values, result )
						result = 'CMPID_GT'	#result = 'GT'
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 338
  def _reduce_56( val, _values, result )
						result = 'CMPID_LT'	#result = 'LT'
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 342
  def _reduce_57( val, _values, result )
						result = 'CMPID_GT_EQ'	#result = 'GE'
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 346
  def _reduce_58( val, _values, result )
						result = 'CMPID_LT_EQ'	#result = 'LE'
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 350
  def _reduce_59( val, _values, result )
            result = 'CMPID_AND'
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 354
  def _reduce_60( val, _values, result )
            result = 'CMPID_OR'
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 364
  def _reduce_61( val, _values, result )
            result = WhileNode.new(val[1], val[3])
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 373
  def _reduce_62( val, _values, result )
            result = SwitchNode.new(val[1].sub(/\A\$/,""), val[3])
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 384
  def _reduce_63( val, _values, result )
            result = []
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 388
  def _reduce_64( val, _values, result )
            result = [ val[0] ]
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 392
  def _reduce_65( val, _values, result )
            result.push val[1]
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 401
  def _reduce_66( val, _values, result )
            result = CaseNode.new(val[1], val[3])
   result
  end
.,.,

 # reduce 67 omitted

 # reduce 68 omitted

module_eval <<'.,.,', 'sp4.y', 423
  def _reduce_69( val, _values, result )
						result = AssignNode.new(val[0], val[2], :VARREF)
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 427
  def _reduce_70( val, _values, result )
						result = AssignNode.new(val[0], val[2], nil)
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 431
  def _reduce_71( val, _values, result )
						result = AssignNode.new(val[0], val[2], funcall)
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 441
  def _reduce_72( val, _values, result )
              result = CompoundAssignNode.new(val[0], val[2], val[1])
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 445
  def _reduce_73( val, _values, result )
              result = CompoundAssignNode.new(val[0], val[2], val[1])
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 454
  def _reduce_74( val, _values, result )
						result = [val[0],val[1]]
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 458
  def _reduce_75( val, _values, result )
						result = [val[0], ['('] + val[2] + [')'] ]
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 462
  def _reduce_76( val, _values, result )
            result = [ val[0], "()" ]
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 466
  def _reduce_77( val, _values, result )
						result = [val[0],val[1]]
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 470
  def _reduce_78( val, _values, result )
            result = [ val[0], nil ]
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 474
  def _reduce_79( val, _values, result )
            result = [ val[0], val[1] ]
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 478
  def _reduce_80( val, _values, result )
            result = [ val[0], nil ]
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 487
  def _reduce_81( val, _values, result )
            result = DefLocalVarNode.new( val[1] )
   result
  end
.,.,

 # reduce 82 omitted

module_eval <<'.,.,', 'sp4.y', 501
  def _reduce_83( val, _values, result )
						result = val
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 505
  def _reduce_84( val, _values, result )
						result = val
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 509
  def _reduce_85( val, _values, result )
						result = val
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 513
  def _reduce_86( val, _values, result )
						result = val
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 517
  def _reduce_87( val, _values, result )
						result = val
   result
  end
.,.,

 # reduce 88 omitted

 # reduce 89 omitted

 # reduce 90 omitted

 # reduce 91 omitted

 # reduce 92 omitted

module_eval <<'.,.,', 'sp4.y', 546
  def _reduce_93( val, _values, result )
						result = val
   result
  end
.,.,

module_eval <<'.,.,', 'sp4.y', 551
  def _reduce_94( val, _values, result )
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
