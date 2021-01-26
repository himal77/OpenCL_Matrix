void __kernel my(global int* mem)
{
	mem[get_global_id(0)] += 5;
	return;
}