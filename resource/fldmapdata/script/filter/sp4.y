#====================================================================================
#
#	�V���`�v���W�F�N�g�p�X�N���v�g�t�B���^��`
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

  left '==' '!=' '<' '>' '<=' '>='
  left '&&' '||'
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
						#result.push RawNode.new(val[2])
					}
				| stmt_list comments EOL
					{
						#result.push RawNode.new(val[1])
					}
				| stmt_list EOL comments EOL
					{
						#result.push RawNode.new(val[2])
					}


	#---------------------------------------------
	#	�u�ϐ���`���X�g�v�͉��L�̂����ꂩ�F
	#		��s
	#		�u�ϐ���`���X�g�v�u�I�[�v
	#		�u�ϐ���`���X�g�v�u�ϐ���`�v�u�I�[�v
	#---------------------------------------------
  def_local_list :
          {
            result = []
          }
        | def_local_list EOL
        | def_local_list comments EOL
        | def_local_list def_local EOL
          {
            result.push val[1]
          }
        | def_local_list def_local comments EOL
          {
            result.push val[1]
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
  stmt_block  : block_start def_local_list stmt_list block_end
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
        | compound_assign
				| callfunc
        | switch_stmt
        | while_stmt
        | def_local

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
	if_stmt		: IF cmp_expr THEN stmt_list else_stmt ENDIF
					{
						result = IfNode.new(val[1],val[3],val[4])
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
				| ELSIF cmp_expr THEN stmt_list else_stmt
					{
						result = [IfNode.new(val[1], val[3], val[4])]
					}

	#---------------------------------------------
	#	�u��r���v�͉��L�̂����ꂩ�F
	#		�u�ϐ��Q�Ɓv�u��r���Z�q�v�u�����v
	#		�u�ϐ��Q�Ɓv�u��r���Z�q�v�u�ϐ��Q�Ɓv
	#		�uFLAG_ON�v�u(�v�u���ʎq�v�u)�v
	#		�uFLAG_OFF�v�u(�v�u���ʎq�v�u)�v
	#		�u(�v�u��r���v�u)�v
	#		�u��r���v�u��r���Z�q�v�u��r���v
  #
	#---------------------------------------------
	cmp_expr	:	VARREF cmp_ident expr
					{
            left = "_PUSH_WORK #{val[0].sub(/\A\$/,"")}"
            right = "_PUSH_VALUE #{val[2]}"
            result = CompareNode.new(val[1], left, right)
					}
				|	VARREF cmp_ident VARREF
					{
            left = "_PUSH_WORK #{val[0].sub(/\A\$/,"")}"
            right = "_PUSH_WORK #{val[2].sub(/\A\$/,"")}"
            result = CompareNode.new(val[1], left, right)
					}
				|	FLAG_ON '(' IDENT ')'
					{
            left = "_PUSH_FLAG #{val[2]}"
            right = "_PUSH_VALUE TRUE"
            result = CompareNode.new('CMPID_EQ', left, right)
					}
				|	FLAG_ON '(' VARREF ')'
					{
            left = "_PUSH_FLAG #{val[2].sub(/\A\$/,"")}"
            right = "_PUSH_VALUE TRUE"
            result = CompareNode.new('CMPID_EQ', left, right)
					}
				|	FLAG_OFF '(' IDENT ')'
					{
            left = "_PUSH_FLAG #{val[2]}"
            right = "_PUSH_VALUE FALSE"
            result = CompareNode.new('CMPID_EQ', left, right)
					}
				|	FLAG_OFF '(' VARREF ')'
					{
            left = "_PUSH_FLAG #{val[2].sub(/\A\$/,"")}"
            right = "_PUSH_VALUE FALSE"
            result = CompareNode.new('CMPID_EQ', left, right)
					}
        | '(' cmp_expr ')'
          {
            result = val[1]
          }
        | cmp_expr cmp_ident cmp_expr
          {
            result = LogicalCompareNode.new(val[1], val[0], val[2])
          }


	#---------------------------------------------
	#	�u��r���Z�q�v�̒�`
	#---------------------------------------------
	cmp_ident	:	'=='
					{
						result = 'CMPID_EQ'	#result = 'EQ'
					}
				|	'!='
					{
						result = 'CMPID_NE'	#result = 'NE'
					}
				|	'>'
					{
						result = 'CMPID_GT'	#result = 'GT'
					}
				|	'<'
					{
						result = 'CMPID_LT'	#result = 'LT'
					}
				|	'>='
					{
						result = 'CMPID_GT_EQ'	#result = 'GE'
					}
				|	'<='
					{
						result = 'CMPID_LT_EQ'	#result = 'LE'
					}
        | '&&'
          {
            result = 'CMPID_AND'
          }
        | '||'
          {
            result = 'CMPID_OR'
          }


	#---------------------------------------------
  # �uWHILE�v��
  # �uWHILE�v�u��r���v�u�I�[�v�u���̕��сv�uENDWHILE�v
	#---------------------------------------------
  while_stmt  : WHILE cmp_expr EOL stmt_list ENDWHILE
          {
            result = WhileNode.new(val[1], val[3])
          }
	#---------------------------------------------
  # �uswitch�v��
  # �uSWITCH�v�u�ϐ��Q�Ɓv�u�I�[�v�ucase���̂Ȃ�сv�udefault���v�uENDSWITCH�v
	#---------------------------------------------

  switch_stmt : SWITCH VARREF EOL case_stmt_list default_stmt ENDSWITCH
          {
            result = SwitchNode.new(val[1].sub(/\A\$/,""), val[3], val[4])
          }

	#---------------------------------------------
  # �ucase���̂Ȃ�сv�͉��L�̂����ꂩ�F
  # �ucase���v
  # �ucase���̂Ȃ�сv�ucase���v
  #
	#---------------------------------------------
  case_stmt_list  :
          {
            result = []
          }
        | case_stmt
          {
            result = [ val[0] ]
          }
        | case_stmt_list case_stmt
          {
            result.push val[1]
          }

	#---------------------------------------------
  # �ucase���v��
  # �uCASE�v�u�����v�uTHEN�v�u���̕��сv
	#---------------------------------------------
  case_stmt : CASE args THEN stmt_list
          {
            result = CaseNode.new(val[1], val[3])
          }

	#---------------------------------------------
  # �uDEFAULT���v�͉��L�̂����ꂩ�F
  #
  # �u�󔒁v
  # �uDEFAULT�v�u���̕��сv
	#---------------------------------------------
  default_stmt :
        | DEFAULT stmt_list
          {
            result = val[1]
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
  # �u��������v��
  #   
  #   �u�ϐ��Q�Ɓv�u����������Z�q�v�u�ϐ��Q�Ɓv
  #   �u�ϐ��Q�Ɓv�u����������Z�q�v�u�v���C�}���v
	#---------------------------------------------
  compound_assign : VARREF OP_COMP_ASSIGN VARREF
            {
              result = CompoundAssignNode.new(val[0], val[2], val[1])
            }
          | VARREF OP_COMP_ASSIGN primary
            {
              result = CompoundAssignNode.new(val[0], val[2], val[1])
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
						result = [val[0], ['('] + val[2] + [')'] ]
					}
        | COMMAND '(' ')'
          {
            result = [ val[0], "()" ]
          }
        | EVENT_START IDENT
          {
						result = [val[0],val[1]]
          }
        | EVENT_END
          {
            result = [ val[0], nil ]
          }
        | INIT_EVENT_START IDENT
          {
            result = [ val[0], val[1] ]
          }
        | INIT_EVENT_END
          {
            result = [ val[0], nil ]
          }

	#---------------------------------------------
  # �u���[�J���ϐ���`�v��
  #   �uDEFINE_LOCAL�v�u���ʎq�v
	#---------------------------------------------
  def_local : DEFINE_LOCAL IDENT
          {
            result = DefLocalVarNode.new( val[1] )
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
          # \����n�܂鎯�ʎq�̓A�Z���u���}�N���p�����[�^
					pushq [ :MACPARAM, $& ]

				when /\A==/,/\A!=/,/\A\<=/,/\A\>=/,/\A>/,/\A</,/\A\&\&/,/\A\|\|/
          # ��r���Z�q
					pushq [ $&, $& ]

        when /\A\+=/, /\A\-=/, /\A\*=/, /\A\/=/, /\A\%=/, /\A\|=/, /\A\&=/
          # ����������Z�q
          pushq [ :OP_COMP_ASSIGN, $& ]

				when /\A\/\*.*/
          #C�`���R�����g�J�n
					pushq [ :COMMENT, $& ]
					@incomment = true

				when /\A[\+\-\*\/=(){},]/
          #���Z�q�A�J�b�R�Ȃǂ̋L��
					pushq [ $&, $& ]
				else
					raise RuntimeError, "#{@fname}:#{@nowlineno}: fatal error! \{#{line_org}\}"
				end
				line = $'
				printf( "\#NOW LINE(%4d) %s\n", @nowlineno, line)
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

