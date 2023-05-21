; acá definimos todo lo que necesitamos de assembler para las interrupciones
GLOBAL _sti
GLOBAL _cli



_cli:
	cli
	ret
_sti:
	sti
	ret