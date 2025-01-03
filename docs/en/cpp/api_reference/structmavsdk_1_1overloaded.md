# mavsdk::overloaded Struct Reference
`#include: plugin_base.h`

----


Creates an overloaded callable object from multiple other callables. 


This template class allows to combine multiple callable objects into one overloaded callable object, that can handle different types of parameters. For example:


overloaded { [](auto value) { LogErr() << "Got an unexpected type"; }, [](uint32_t value) { LogDebug() << "Got an expected type"; } }((uint32_t) 100);


This is especially useful for creating callback functions to handle different parameter types 

