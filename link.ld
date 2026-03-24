OUTPUT_FORMAT("elf32-i386")
ENTRY(start)
phys = 0x00100000;
SECTIONS
{
	.text phys : AT(phys)
	{
		code = .; _code = .; __code = .;
		*(.text)
		. = ALIGN(4096);
	}
	.data :
	{
		data = .; _data = .; __data = .;
		*(.data)
		*(.rodata)
		. = ALIGN(4096);
	}
	.bss :
	{
		bss = .; _bss = .; __bss = .;
		*(.bss)
		. = ALIGN(4096);
	}
	.stack :
	{
		stack = .; _stack = .; __stack = .;
		*(.stack)
		. = ALIGN(4096);
	}
	. = 0x00400000;

	_begin_of_user_space = .;

    . = 0x00403000;
    _kernel_stack = .;
    . = 0x00404000;            

	.user : AT(ADDR(.user))
	{
        _user_start = .;
        *(.user_text)
		_user_stack_guard = .;
		*(.user_data)
        . = ALIGN(4096);
        _user_end = .;
    }

	_end_of_user_space = .;


	end = .; _end = .; __end = .;
}
