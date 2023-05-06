# Lazy Detour
## Whats this?
``Lazy Detour is a single header file to easily manage and create hooks using the open source MinHook library which can be found below.``
## Usage
#### Detour Function
```c++
	int function_somewhere(int a1, bool sum)
	{
		int result = a1;
		
		if (sum) {
			result + 10;
		}

		return result;
	}
	// This could be anywhere in any binary aslong as you have the pointer to the function you are okay.

	int my_detour(int a1, bool sum)
	{
		sum = false;
		//We detour and modify the original arg.

		return get_original<decltype(&my_detour)>("my_detour")(a1, sum); //We use this if you want the original function to execute.
	}
	// This is your own function which is going to get called inside the original once the original is called.	
```
#### Create Detour
```c++
MH_Initialize();

//Name, pointer to original, your function.
create_and_enable_hook("my_detour", reinterpret_cast<void*>(&function_somewhere), reinterpret_cast<void*>(&my_detour));

//Once you want to remove do
remove_all_hooks(); 
//Or
remove_hook("my_detour");
```
