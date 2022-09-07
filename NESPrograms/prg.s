;C onstants
HLT=$02

.segment "HEADER"
.byte "NES", $1a
.byte 1 ; PRG-ROM pages (16kb)
.byte $00 ; CHR-ROM pages (8kb)

.segment "ZEROPAGE"
idk: .byte $80

.segment "CODE"
addtwo:
adc #$02
rts

vblank:
jmp vblank
;gawrrr

resethandler:
    lda #$09
    jsr addtwo
    .byte HLT



.segment "VECTORS"
.word vblank, resethandler, $aabb