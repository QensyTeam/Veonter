.text
.global gdt_flush
#.type gdt_flush, @function

gdt_flush:
    # Load the CS register with 0x08 (code)
    # and jump to reload_CS
    LJMP   $0x08, $reload_CS
reload_CS:
    # Reload data segment registers:
    MOV   $0x10, %AX  # 0x10 points at the new data selector
    MOV   %AX, %DS
    MOV   %AX, %ES
    MOV   %AX, %FS
    MOV   %AX, %GS
    MOV   %AX, %SS
    RET
.end