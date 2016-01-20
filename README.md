#YupeiLibrary#
&emsp;&emsp; A library includes an STL-like basic library and a DirectUI GUI Framework on Windows with Direct2D.


+ ##Basic Library##
&emsp;This part is a basic library which is a custom STL and combined with the C++ Technical Specification implementation on Windows and provides

  + Type traits,which is the foundation of the whole basic library;
  + Standard Containers (such as vector,list,etc.);
  + Useful Algorithm with the abstraction of iterators;
  + function;
  + Smart pointers,which are very useful in GUI development,such as shared_ptr,unique_ptr,intrusive_ptr;
  + Allocators aimed for high performance;
  + Many useful extentions (like the C++ TS),such as filesystem,type-erased allocator,ranges and so on.


 |		Headers				|			Status							|
 |:------------------------:|:-----------------------------------------:|
 |	 `<type_traits>`		|	  Complete								|
 |	 `<tuple>`				|	  Partial(`tuple_cat`)					|
 |	 `<utility>`			|	  Complete								|
 |	 `<memory>`				|	  Partial(`intrusive_ptr`)				|
 |	 `<iterator>`			|	  Complete								|
 |	 `<algorithm>`			|	  Complete								|
 |	 `<memory_resource>`	|	  Partial(`synchronized_pool_resource`)	|
 |	 `<list>`				|	  Complete								|
 |	 `<vector>`				|	  Complete								|
 
 
+ ##GUI Part##
&emsp;&emsp; Under construction.