;C onstants
HLT=$02
INT_OUT=$4000
CHAR_OUT=$4001
STR_out=$4002

.segment "STARTUP"
.org 0
resethandler:
    lda #$02
    sta hm
    jsr addtwo

    lda #$ab
    sta INT_OUT
hm:
    .byte HLT
    ; .byte HLT
    ; .byte $02,$01

addtwo:
adc #$02
rts

vblank:
jmp vblank
;gawrrr


.segment "VECTORS"
.word vblank, resethandler, $aabb