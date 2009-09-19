	/************************************/

	.section .text.launcher_arm7_entry
arm7_start:
	b	main

	.section .text.launcher_arm9_entry
arm9_start:
	b 	arm9_start

	/************************************/

	.section .text
main:
	b main
