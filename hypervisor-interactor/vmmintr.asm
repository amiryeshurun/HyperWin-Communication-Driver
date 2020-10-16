PUBLIC HyperWinVmCall
.code _text

HyperWinVmCall PROC PUBLIC

mov rax, rcx
mov rbx, rdx

vmcall

ret
HyperWinVmCall ENDP

END