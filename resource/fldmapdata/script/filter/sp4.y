#====================================================================================
#
#	�|�P��������DS�p�X�N���v�g�t�B���^��`
#
#	2007.04�@tamada GAME FREAK inc.
#
#	�����̃|�P�����X�N���v�g�ɕϐ��������IF�\����ǉ����邽�߂ɁA
#	racc�iruby�ɂ��yacc=Yet Another Compiler Compiler)���g�p����
#	�t�B���^�𐶐����邽�߂̒�`�t�@�C��
#====================================================================================



#====================================================================================
#
#
#   ���@�K���L�q��
#
#
#====================================================================================
class PmScript::Parser

#---------------------------------------------
#	���Z�q�D�揇�ʂ̒�`
#---------------------------------------------
prechigh
	left	'/'	'*'
	left	'-'	'+'
preclow

#---------------------------------------------
#	�ȉ���BNF�L�@�ɂ�镶�@+��
#---------------------------------------------
rule

	#---------------------------------------------
	#	�u�v���O�����v�́u���̃��X�g�v
	#---------------------------------------------
	program	: stmt_list
					{
						result = RootNode.new( val[0] )
					}

	#---------------------------------------------
	#	�u���̃��X�g�v�͉��L�̂����ꂩ�F
	#		��s
	#		�u���̃��X�g�v�Ɓu���v�Ɓu�I�[�v
	#		�u���̃��X�g�v�Ɓu�u���b�N�v
	#		�u���̃��X�g�v�Ɓu�I�[�v
	#		�u���̃��X�g�v�Ɓu���v�Ɓu�R�����g���X�g�v�Ɓu�I�[�v
	#		�u���̃��X�g�v�Ɓu�R�����g���X�g�v�Ɓu�I�[�v
	#		�u���̃��X�g�v�Ɓu�I�[�v�Ɓu�R�����g���X�g�v�Ɓu�I�[�v
	#---------------------------------------------
	stmt_list	:
					{
						result = []
					}
				| stmt_list stmt EOL
					{
						result.push val[1]
					}
				| stmt_list stmt_block
					{
						result.push val[1]
					}
				| stmt_list EOL
				| stmt_list stmt comments EOL
					{
						result.push val[1]
						result.push RawNode.new(val[2])
					}
				| stmt_list comments EOL
					{
						result.push RawNode.new(val[1])
					}
				| stmt_list EOL comments EOL
					{
						result.push RawNode.new(val[2])
					}


	#---------------------------------------------
	#	�u�ϐ���`���X�g�v�͉��L�̂����ꂩ�F
	#		��s
	#		�u�ϐ���`���X�g�v�u�I�[�v
	#		�u�ϐ���`���X�g�v�u�ϐ���`�v�u�I�[�v
	#---------------------------------------------
	defvar_list	:
					{
						result = []
					}
				| defvar_list EOL
				| defvar_list def_var EOL
					{
						result.push val[1]
					}

	#---------------------------------------------
	#	�u�ϐ���`�v�́F
	#		�uDEFVAR�v�u��`�L�[���[�h�v�u���ʎq�v
	#---------------------------------------------
	def_var		: DEFVAR TYPE IDENT
					{
						result = DefVarNode.new(val[1], val[2])
					}

	#---------------------------------------------
	#	�u�֐���`�v�͉��L�̂����ꂩ�F
	#		�uDEFFUNC�v�u��`�L�[���[�h�v�u���ʎq�v�u(�v�u�p�����[�^�v�u)�v�u�I�[�v�u�u���b�N�v
	#		�uDEFFUNC�v�u��`�L�[���[�h�v�u���ʎq�v�u(�v�u)�v�u�I�[�v�u�u���b�N�v
	#---------------------------------------------
	def_func	: DEFFUNC TYPE IDENT '(' param ')' EOL stmt_block
					{
						result = DefFuncNode.new( val[1], val[2], val[4], val[7] )
					}
				| DEFFUNC TYPE IDENT '(' ')' EOL stmt_block
					{
						result = DefFuncNode.new( val[1], val[2], nil, val[6] )
					}

	#---------------------------------------------
	#	�u�p�����[�^�v�͉��L�̂����ꂩ�v�F
	#		�u�ϐ��Q�Ɓv
	#		�u�p�����[�^�v�u,�v�u�ϐ��Q�Ɓv
	#---------------------------------------------
	param		: VARREF
					{
						result = val
					}
				| param ',' VARREF
					{
						result.push val[2]
					}

	#---------------------------------------------
	#	�u�u���b�N�v��
	#		�u�u���b�N�J�n�v�u�v�u�v�u�u���b�N�I���v
	#---------------------------------------------
	stmt_block	: block_start defvar_list stmt_list block_end
					{
						result = BlockNode.new(val[1], val[2])
					}
				| def_func

	#---------------------------------------------
	#	�u�u���b�N�J�n�v�͉��L�̂����ꂩ�F
	#		�u{�v�u�I�[�v
	#		�u{�v�u�R�����g���X�g�v�u�I�[�v
	#---------------------------------------------
	block_start	: '{' EOL
				| '{' comments EOL
	
	#---------------------------------------------
	#	�u�u���b�N�I���v�͉��L�̂����ꂩ�F
	#		�u}�v�u�I�[�v
	#		�u}�v�u�R�����g���X�g�v�u�I�[�v
	#---------------------------------------------
	block_end	: '}' EOL
				| '}' comments EOL

	#---------------------------------------------
	#	�u���v�͉��L�̂����ꂩ
	#			���x��
	#			�u���߁v
	#			�u���ߕ��v
	#			�u�}�N���v
	#			�uif���v
	#			�u������v
	#			�u�֐��Ăяo���v
	#---------------------------------------------
	stmt		: LABEL
					{
						result = RawNode.new(val[0]);
					}
				| COMMAND
					{
						result = CmdNode.new( val, nil )
					}
				| cmd
					{
						result = CmdNode.new( val[0].shift, val[0] )
					}
				| MACRO
					{
						result = RawNode.new(val[0]);
					}
				| if_stmt
					{
					}
				| assign
					{
					}
				| callfunc

	#---------------------------------------------
	#	�u�֐��Ăяo���v�͉��L�̂����ꂩ�F
	#		�u���ʎq�v�u(�v�u)�v
	#		�u���ʎq�v�u(�v�u�����v�u)�v
	#		�u�ϐ��Q�Ɓv�u=�v�u���ʎq�v�u(�v�u�����v�u)�v
	#		�u�ϐ��Q�Ɓv�u=�v�u���ʎq�v�u(�v�u)�v
	#---------------------------------------------
	callfunc	: IDENT '(' ')'
					{
						result = CallFuncNode.new( nil, val[0], nil )
					}
				| IDENT '(' args ')'
					{
						result = CallFuncNode.new( nil, val[0], val[2] )
					}
				| VARREF '=' IDENT '(' args ')'
					{
						result = CallFuncNode.new( val[0], val[2], val[4] )
					}
				| VARREF '=' IDENT '(' ')'
					{
						result = CallFuncNode.new( val[0], val[2], nil )
					}

	#---------------------------------------------
	#	�u�R�����g���X�g�v�͉��L�̂����ꂩ�F
	#		�u�R�����g�v
	#		�u�R�����g���X�g�v�u�R�����g�v
	#---------------------------------------------
	comments	: COMMENT
					{
						result = val
					}
				| comments COMMENT
					{
						result.push val[1]
					}

	#---------------------------------------------
	#	�uif���v�͉��L�F
	#	�uIF�v�u��r���v�uTHEN�v�u�I�[�v�u���̕��сv�uelse���v�uENDIF�v
	#---------------------------------------------
	if_stmt		: IF cmp_expr THEN EOL stmt_list else_stmt ENDIF
					{
						result = IfNode.new(val[1],val[4],val[5])
					}

	#---------------------------------------------
	#	�uelse���v�͉��L�̂����ꂩ�F
	#		��
	#		�uELSE�v�u���̕��сv
	#		�uELSEIF�v�u��r���v�uTHEN�v�u�I�[�v�u���̕��сv�uelse���v
	#
	#---------------------------------------------
	else_stmt	:
					{
						result = nil
					}
				| ELSE stmt_list
					{
						result = val[1]
					}
				| ELSIF cmp_expr THEN EOL stmt_list else_stmt
					{
						result = [IfNode.new(val[1], val[4], val[5])]
					}

	#---------------------------------------------
	#	�u��r���v�͉��L�̂����ꂩ�F
	#		�u�ϐ��Q�Ɓv�u��r���Z�q�v�u�����v
	#		�u�ϐ��Q�Ɓv�u��r���Z�q�v�u�ϐ��Q�Ɓv
	#		�uFLAG_ON�v�u(�v�u���ʎq�v�u)�v
	#		�uFLAG_OFF�v�u(�v�u���ʎq�v�u)�v
	#---------------------------------------------
	cmp_expr	:	VARREF cmp_ident expr
					{
						result = ["_CMPVAL #{val[0].sub(/\A\$/,"")}, #{val[2].sub(/\A\$/,"")}"]
						result.push val[1]
					}
				|	VARREF cmp_ident VARREF
					{
						result = ["_CMPWK #{val[0].sub(/\A\$/,"")}, #{val[2].sub(/\A\$/,"")}"]
						result.push val[1]
					}
				|	FLAG_ON '(' IDENT ')'
					{
						result = ["_FLAG_CHECK #{val[2]}"]
						result.push "FLGOFF"
					}
				|	FLAG_OFF '(' IDENT ')'
					{
						result = ["_FLAG_CHECK #{val[2]}"]
						result.push "FLGON"
					}

	#---------------------------------------------
	#	�u��r���Z�q�v�̒�`
	#---------------------------------------------
	cmp_ident	:	'=='
					{
						result = 'NE'	#result = 'EQ'
					}
				|	'!='
					{
						result = 'EQ'	#result = 'NE'
					}
				|	'>'
					{
						result = 'LE'	#result = 'GT'
					}
				|	'<'
					{
						result = 'GE'	#result = 'LT'
					}
				|	'>='
					{
						result = 'LT'	#result = 'GE'
					}
				|	'<='
					{
						result = 'GT'	#result = 'LE'
					}

	#---------------------------------------------
	#	�u����v��
	#		�u�ϐ��Q�Ɓv�u=�v�u�ϐ��Q�Ɓv
	#		�u�ϐ��Q�Ɓv�u=�v�u�v���C�}���v
	#		�u�ϐ��Q�Ɓv�u=�v�u�֐��Ăяo���v
	#---------------------------------------------
	assign		: VARREF '=' VARREF
					{
						result = AssignNode.new(val[0], val[2], :VARREF)
					}
				| VARREF '=' primary
					{
						result = AssignNode.new(val[0], val[2], nil)
					}
				| VARREF '=' funcall
					{
						result = AssignNode.new(val[0], val[2], funcall)
					}

	#---------------------------------------------
	#	�u���ߕ��v��
	#		�u���߁v�u�����v
	#---------------------------------------------
	cmd			: COMMAND args
					{
						result = [val[0],val[1]]
					}
	      |  COMMAND '(' args ')'
					{
						result = [val[0],val[2]]
					}
        | COMMAND '(' ')'
          {
            result = [ val[0], nil ]
          }
        | EVENT_START IDENT
          {
						result = [val[0],val[1]]
          }
        | EVENT_END
          {
            result = [ val[0], nil ]
          }

	#---------------------------------------------
	#	�u�����v��
	#		�u�����v�u+�v�u�����v
	#		�u�����v�u-�v�u�����v
	#		�u�����v�u/�v�u�����v
	#		�u�����v�u*�v�u�����v
	#		�u(�v�u�����v�u)�v
	#---------------------------------------------
	expr		: primary
				| expr '+' expr
					{
						result = val
					}
				| expr '-' expr
					{
						result = val
					}
				| expr '/' expr
					{
						result = val
					}
				| expr '*' expr
					{
						result = val
					}
				| '(' expr ')'
					{
						result = val
					}

	#---------------------------------------------
	#	�u�v���C�}���v��
	#			���l
	#			���ʎq
	#			�}�N���Q��
	#---------------------------------------------
	primary		: NUMBER
				| IDENT
				| MACPARAM


	#---------------------------------------------
	#	�u�����v��
	#		�u�����v
	#		�u�ϐ��Q�Ɓv
	#---------------------------------------------
	arg			: expr
				| VARREF

	#---------------------------------------------
	#	�u�����̕��сv��
	#		�u�����v
	#		�u�����̕��сv�Ɓu,�v�Ɓu�����v
	#---------------------------------------------
	args		: arg
					{
						result = val
					}
				| args ',' arg
					{
						result.push val[1]
						result.push val[2]
					}


end
#class PmScript::Parser�̏I���ʒu

#====================================================================================
#
#
#   ���[�U�[�R�[�h�L�q��
#
#
#====================================================================================

---- header
#
#	�|�P�����p�X�N���v�g�R���p�C��
#

---- inner

def initialize
end

#�\����`
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
  'EVENT_END' => :EVENT_END
};

#�\��^��`
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
          #�b�{�{�`���R�����g��`
					pushq [ :COMMENT, $& ]

				when /^\s*$/, /\A\#.*/
          #�A�Z���u���R�����g��`
					pushq [ :COMMENT, $&]

				when /\A\/\*.*\*\//
          #�b�`���R�����g��`
					pushq [ :COMMENT, $&]

				when /\A\.[a-zA-Z_]+.*$/
          #�A�Z���u���}�N����`
					pushq [ :MACRO, $& ]

				when /\A[a-zA-Z_][a-zA-Z0-9_]*:/
          #�A�Z���u�����x����`
					pushq [ :LABEL, $& ]

				when /\A_[A-Z0-9][a-zA-Z0-9_]*/
          #�X�N���v�g�R�}���h��`�i�Q�ŊJ�n����j
					pushq [ :COMMAND, $& ]

				when /\A0x[0-9a-fA-F]+/, /\A\d+/
          #���l��`�i0x�Ŏn�܂�P�U�i���A���邢�͂P�O�i���j
					pushq [ :NUMBER, $& ]

				when /\A\$[a-zA-Z][a-zA-Z0-9_]*/
          #�ϐ���`�i���Ŏn�܂�j
					pushq [ :VARREF, $& ]

				when /\A[a-zA-Z_][a-zA-Z0-9_]*/
          #���ʎq��`���邢�͌^��`
					if RESERVED.has_key? $& then
						pushq [ RESERVED[$&], $&.intern ]
					elsif RESERVED_TYPE.has_key? $& then
						pushq [ :TYPE, $&.intern ]
					else
						pushq [ :IDENT, $& ]
					end
				when /\A\\[a-zA-Z_][a-zA-Z0-9_]*/
					pushq [ :MACPARAM, $& ]
				when /\A==/,/\A!=/,/\A\<=/,/\A\>=/,/\A>/,/\A</
					pushq [ $&, $& ]
				when /\A\/\*.*/
          #C�`���R�����g�J�n
					pushq [ :COMMENT, $& ]
					@incomment = true
				when /\A[\+\-\*\/=(){},]/
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


---- footer

