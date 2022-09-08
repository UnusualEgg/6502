;C onstants
HLT=$02
INT_OUT=$4000
CHAR_OUT=$4001
STR_out=$4002

.segment "STARTUP"
ptr: .word hellostr

resethandler:
    lda ptr
    sta $4003
    lda ptr+1
    sta $4004

    sta STR_out
    .byte HLT


vblank:
jmp vblank

.segment "RODATA"
hellostr: .byte "Wussap shawty", 0

.segment "VECTORS"
.word vblank, resethandler, $000a