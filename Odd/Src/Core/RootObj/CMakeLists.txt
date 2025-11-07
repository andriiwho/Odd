# This module is the base of the built-in reference counting system.
# This module includes functions to create and destory the RootObj objects.

Odd_CreateModule(RootObj)

target_link_libraries(RootObj PUBLIC OddCore)