.IFJcode18
DEFVAR GF@%void
DEFVAR GF@%operand1
DEFVAR GF@%operand2
DEFVAR GF@%operand1type
DEFVAR GF@%operand2type
JUMP $$main
LABEL $$operand1ToFloat
JUMPIFNEQ $$operand1ToFloatReturn GF@%operand1type string@int
INT2FLOAT GF@%operand1 GF@%operand1
LABEL $$operand1ToFloatReturn
TYPE GF@%operand1type GF@%operand1
RETURN
LABEL $$operand2ToFloat
JUMPIFNEQ $$operand2ToFloatReturn GF@%operand2type string@int
INT2FLOAT GF@%operand2 GF@%operand2
LABEL $$operand2ToFloatReturn
TYPE GF@%operand2type GF@%operand2
RETURN
LABEL $$operandNumberCompatibility
JUMPIFNEQ $$compatibilityError GF@%operand1type GF@%operand2type
JUMPIFEQ $$compatibilityError GF@%operand1type string@string
JUMPIFEQ $$compatibilityError GF@%operand1type string@bool
JUMPIFEQ $$compatibilityError GF@%operand1type string@nil
RETURN
LABEL $$operandCompareCompatibility
JUMPIFNEQ $$compatibilityError GF@%operand1type GF@%operand2type
JUMPIFEQ $$compatibilityError GF@%operand1type string@bool
RETURN
LABEL $$operandAdditionCompatibility
JUMPIFEQ $$compatibilityError GF@%operand1type string@bool
JUMPIFEQ $$compatibilityError GF@%operand1type string@nil
RETURN
LABEL $$compatibilityError
EXIT int@4
LABEL $$stringError
EXIT int@4

LABEL $length
PUSHFRAME
DEFVAR LF@%return
DEFVAR LF@%type
MOVE LF@%return nil@nil
TYPE LF@%type LF@%1
JUMPIFNEQ $$compatibilityError LF@%type string@string
STRLEN LF@%return LF@%1
POPFRAME
RETURN

LABEL $ord
PUSHFRAME
DEFVAR LF@%return
MOVE LF@%return nil@nil
DEFVAR LF@%type
DEFVAR LF@%condition
TYPE LF@%type LF@%1
JUMPIFNEQ $$compatibilityError LF@%type string@string
TYPE LF@%type LF@%2
JUMPIFNEQ $$compatibilityError LF@%type string@int
DEFVAR LF@slen
STRLEN LF@slen LF@%1
LT LF@%condition LF@%2 LF@slen
JUMPIFEQ $$ordReturn LF@%condition bool@false
GT LF@%condition LF@%2 int@-1
JUMPIFEQ $$ordReturn LF@%condition bool@false
STRI2INT LF@%return LF@%1 LF@%2
LABEL $$ordReturn
POPFRAME
RETURN

LABEL $chr
PUSHFRAME
DEFVAR LF@%return
DEFVAR LF@%type
MOVE LF@%return nil@nil
DEFVAR LF@%condition
TYPE LF@%type LF@%1
JUMPIFNEQ $$compatibilityError LF@%type string@int
GT LF@%condition LF@%1 int@-1
JUMPIFEQ $$stringError LF@%condition bool@false
LT LF@%condition LF@%1 int@256
JUMPIFEQ $$stringError LF@%condition bool@false
INT2CHAR LF@%return LF@%1
POPFRAME
RETURN

LABEL $substr
PUSHFRAME
DEFVAR LF@%return
MOVE LF@%return string@
DEFVAR LF@%condition
MOVE LF@%condition nil@nil
DEFVAR LF@%type
DEFVAR LF@s
MOVE LF@s LF@%1
DEFVAR LF@i
MOVE LF@i LF@%2
DEFVAR LF@n
MOVE LF@n LF@%3
TYPE LF@%type LF@%1
JUMPIFNEQ $$compatibilityError LF@%type string@string
TYPE LF@%type LF@%2
JUMPIFNEQ $$compatibilityError LF@%type string@int
TYPE LF@%type LF@%3
JUMPIFNEQ $$compatibilityError LF@%type string@int
#Get length of string
DEFVAR LF@stringLength
STRLEN LF@stringLength LF@s
DEFVAR LF@lastIndex
MOVE LF@lastIndex LF@i
ADD LF@lastIndex LF@lastIndex LF@n
#Check if index is in range
LT LF@%condition LF@i int@0
JUMPIFEQ $$substrErr  LF@%condition bool@true
LT LF@%condition LF@i LF@stringLength
JUMPIFEQ $$substrErr  LF@%condition bool@false
LT LF@%condition LF@n int@0
JUMPIFEQ $$substrErr  LF@%condition bool@true
#Main loop
DEFVAR LF@iterator
MOVE LF@iterator LF@i
DEFVAR LF@currentChar
LABEL $$substrLoopStart
#End loop if iterator >= stringLength || iterator >= lastIndex 
LT LF@%condition LF@iterator LF@stringLength
JUMPIFEQ $$substrReturn LF@%condition bool@false
LT LF@%condition LF@iterator LF@lastIndex
JUMPIFEQ $$substrReturn LF@%condition bool@false
#Concatenate with char at index iterator
GETCHAR LF@currentChar LF@s LF@iterator
CONCAT LF@%return LF@%return LF@currentChar
#Increment iterator
ADD LF@iterator LF@iterator int@1
JUMP $$substrLoopStart
LABEL $$substrReturn
POPFRAME
RETURN
LABEL $$substrErr
MOVE LF@%return nil@nil
POPFRAME
RETURN

LABEL $factorial
PUSHFRAME

DEFVAR LF@%return
MOVE LF@%return nil@nil

DEFVAR LF@n
MOVE LF@n LF@%1

DEFVAR LF@%condition
MOVE LF@%condition nil@nil
DEFVAR LF@%conditionType
MOVE LF@%conditionType nil@nil
DEFVAR LF@%temp1
MOVE LF@%temp1 nil@nil
DEFVAR LF@result
MOVE LF@result nil@nil
DEFVAR LF@decremented_n
MOVE LF@decremented_n nil@nil
DEFVAR LF@temp_result
MOVE LF@temp_result nil@nil

MOVE  LF@%return nil@nil
#If Start
MOVE GF@%operand1 LF@n
MOVE GF@%operand2 int@2
TYPE GF@%operand1type GF@%operand1
TYPE GF@%operand2type GF@%operand2
JUMPIFEQ $$operation1compatibility GF@%operand1type GF@%operand2type
CALL $$operand1ToFloat
CALL $$operand2ToFloat
LABEL $$operation1compatibility
CALL $$operandCompareCompatibility
LT LF@%temp1 GF@%operand1 GF@%operand2
MOVE  LF@%condition LF@%temp1
TYPE  LF@%conditionType LF@%condition
JUMPIFEQ  $$if1else LF@%conditionType string@nil
JUMPIFNEQ  $$if1start LF@%conditionType string@bool
JUMPIFEQ  $$if1else LF@%condition bool@false
#If Start
LABEL $$if1start
MOVE  LF@result int@1
JUMP $$if1end
#If Else
LABEL $$if1else
MOVE GF@%operand1 LF@n
MOVE GF@%operand2 int@1
TYPE GF@%operand1type GF@%operand1
TYPE GF@%operand2type GF@%operand2
JUMPIFEQ $$operation2compatibility GF@%operand1type GF@%operand2type
CALL $$operand1ToFloat
CALL $$operand2ToFloat
LABEL $$operation2compatibility
CALL $$operandNumberCompatibility
SUB LF@%temp1 GF@%operand1 GF@%operand2
MOVE  LF@decremented_n LF@%temp1
CREATEFRAME 
DEFVAR TF@%1
MOVE TF@%1 LF@decremented_n
CALL $factorial
MOVE  LF@temp_result TF@%return
MOVE GF@%operand1 LF@n
MOVE GF@%operand2 LF@temp_result
TYPE GF@%operand1type GF@%operand1
TYPE GF@%operand2type GF@%operand2
JUMPIFEQ $$operation3compatibility GF@%operand1type GF@%operand2type
CALL $$operand1ToFloat
CALL $$operand2ToFloat
LABEL $$operation3compatibility
CALL $$operandNumberCompatibility
MUL LF@%temp1 GF@%operand1 GF@%operand2
MOVE  LF@result LF@%temp1
#If End
LABEL $$if1end
MOVE  LF@%return LF@result

POPFRAME
RETURN


LABEL $$main
CREATEFRAME
PUSHFRAME

DEFVAR LF@a
MOVE LF@a nil@nil
DEFVAR LF@%condition
MOVE LF@%condition nil@nil
DEFVAR LF@%conditionType
MOVE LF@%conditionType nil@nil
DEFVAR LF@%temp1
MOVE LF@%temp1 nil@nil
DEFVAR LF@vysl
MOVE LF@vysl nil@nil

WRITE  string@Zadejte\032cislo\032pro\032vypocet\032faktorialu:\032
READ  LF@a int
#If Start
MOVE GF@%operand1 LF@a
MOVE GF@%operand2 int@0
TYPE GF@%operand1type GF@%operand1
TYPE GF@%operand2type GF@%operand2
JUMPIFEQ $$operation4compatibility GF@%operand1type GF@%operand2type
CALL $$operand1ToFloat
CALL $$operand2ToFloat
LABEL $$operation4compatibility
CALL $$operandCompareCompatibility
LT LF@%temp1 GF@%operand1 GF@%operand2
MOVE  LF@%condition LF@%temp1
TYPE  LF@%conditionType LF@%condition
JUMPIFEQ  $$if2else LF@%conditionType string@nil
JUMPIFNEQ  $$if2start LF@%conditionType string@bool
JUMPIFEQ  $$if2else LF@%condition bool@false
#If Start
LABEL $$if2start
WRITE  string@\010Faktorial\032nelze\032spocitat\010
JUMP $$if2end
#If Else
LABEL $$if2else
CREATEFRAME 
DEFVAR TF@%1
MOVE TF@%1 LF@a
CALL $factorial
MOVE  LF@vysl TF@%return
WRITE  string@\010Vysledek\032je:
WRITE  LF@vysl
WRITE  string@\010
#If End
LABEL $$if2end
