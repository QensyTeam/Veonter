.text
.global gdt_flush

gdt_flush:
    LJMP   $0x08, $reload_CS
reload_CS:
    MOV   $0x10, %AX  
    MOV   %AX, %DS
    MOV   %AX, %ES
    MOV   %AX, %FS
    MOV   %AX, %GS
    MOV   %AX, %SS
    RET
.end