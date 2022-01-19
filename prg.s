.setcpu "6502"

.segment "HEADER"
.byte "NES", $1a
.byte 1 ; PRG-ROM pages (16kb)
.byte $00 ; CHR-ROM pages (8kb)

.segment "CODE"
vblank:
jmp vblank

resethandler:
LDA #$03
ldx #$01
JMP resethandler ;dereferences to $cc01

.segment "VECTORS"
.word $c000, resethandler, $aabb