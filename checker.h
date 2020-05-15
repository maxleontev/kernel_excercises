
void call_me(const char* message)
{
	printk(KERN_INFO "%s\n", message);
}

//EXPORT_SYMBOL(call_me);