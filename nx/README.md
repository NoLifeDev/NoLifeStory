# NoLifeNx

A C++ library that can read NX files.

## Dependencies

None

## Using NoLifeNx

In order to use NoLifeNx you need to compile NoLifeNx, and link the resulting library into your program.
You also need to ```#include <NoLifeNx.hpp>```. Here follows a brief overview of the various classes.

### NL::File

* Does not allow copying or moving, so the recommended way of creating an NL::File is either as a static/global variable
  or by allocating it on the heap.
* Comes with only one constructor ```NL::File::File(char const *)```.
  Just pass this a C String containing the name of a .nx file with the .nx extension included.
* Feel free to open multiple ```NL::File```s to the same .nx file. The ```NL::Node```s will still be compatible.
  Just keep in mind OS handle limits.
* When an ```NL::File``` is destroyed, all ```NL::Node```s originating from that ```NL::File``` are now invalid and may crash on use.
* ```NL::File::Base()``` returns the root node of an ```NL::File```.
* The four count functions are there just in case you wanna know how big the .nx file is.

### NL::Node

* ```NL::Node``` is nothing more than a super fancy pointer wrapper, so feel free to copy and move them around.
  Don't waste your time allocating them on the heap.
* To get a child node use the [] operator. It can take C Strings ```NL::Node node = othernode["Hello"];```.
  It can take C++ Strings ```std::string str = "Hello"; NL::Node node = othernode[str];```.
  It can even take other nodes, in which case it uses the string value of the node, not the name ```NL::Node node = othernode[thirdnode];```.
* To get the name of the node as a C++ string, use ```NL::Node::Name()```.
* To get the number of children contained in a node, use ```NL::Node::Size()```.
* ```NL::Node``` implements ```begin()``` and ```end()```. along with several operators, thereby making it its own iterator.
  This allows you to use ```NL::Node``` in various C++ algorithms that take iterators.
  This also lets you conveniently recurse through the children of an ```NL::Node``` using ```for (NL::Node child : parent) {}```.
* To get the type of an ```NL::Node``` use ```NL::Node::T()``` which returns a value of the enum ```NL::Node::Type```.
* To get the value of an ```NL::Node```, you can either use the operators which allow easy implicit or explicit casting,
  or you can use the manual Get functions.
* Some operator examples: ```void foo(int n); foo(somenode);``` ```int n = somenode;``` ```int i = (int)somenode;```.
* Some Get examples: ```int n = somenode.GetInt();```.
* The bool operator does not return the value as a bool. Instead it returns whether or not the ```NL::Node``` exists.
  If you want to get the bool value, you'll need to use ```NL::Node::GetBool(bool def)``` instead,
  which takes an optional bool parameter to specify the default value.
* You can get the individual values of a vector type ```NL::Node``` using ````NL::Node::X()``` and ```NL::Node::Y()```.
* All methods do null checks, have default values, and do not return anything. This ensures your program will not crash.

### NL::Audio

* One of the different values types that ```NL::Node``` can give you. ```NL::Audio a = somenode;```.
* ```NL::Audio``` is nothing more than a fancy pointer wrapper, so feel free to copy and move them around.
* To get the length of the data use ```NL::Audio::Length()```.
* To get the a pointer to the data itself use ```NL::Audio::Data()```. Do not attempt to free or modify this data.
* Feel free to cache the pointer because the data at that pointer will never go away or change.

### NL::Bitmap

* One of the different values types that ```NL::Node``` can give you. ```NL::Bitmap b = somenode;```.
* ```NL::Bitmap``` is nothing more than a fancy pointer wrapper, so feel free to copy and move them around.
* To get the width and height of the image use ```NL::Bitmap::Width()``` and ```NL::Bitmap::Height()```.
* To get the data itself use ```NL::Bitmap::Data()```. Note, this function decompresses the data live each time you call it, so try not to call it over and over if you don't have to.
  Also, the data at that pointer is volatile and will change with your next call to ```NL::Bitmap::Data()``` or the pointer may even become invalid,
  so just copy the data to whatever internal texture you need and use it that way. The returned data is standard raw 32-bit BGRA pixel data.
* ```NL::Bitmap::Length()``` provides the length of the uncompressed pixel data in case you're too lazy to calculate it yourself from the width and height.
* ```NL::Bitmap::ID()``` returns a unique ID for that bitmap, useful as the index in a cache of textures.
