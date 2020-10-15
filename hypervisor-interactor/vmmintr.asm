PUBLIC HyperWinVmCall
.code _text

HyperWinVmCall PROC PUBLIC

mov rax, rcx
vmcall

ret
HyperWinVmCall ENDP

END