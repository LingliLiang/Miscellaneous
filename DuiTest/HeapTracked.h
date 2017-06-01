#pragma once

//HeapTracked类可判断单个的对象是否的在堆上
#ifndef HEAP_TRACKED_HPP
#define HEAP_TRACKED_HPP
#include <cstddef>
#include <list>
class HeapTracked{
private:
	static std::list<const void*> addresses; //存放各个堆对象的指针
public:
	class MissingAddress{ }; //异常类
	virtual ~HeapTracked()=0; //纯虚函数
	static void* operator new(std::size_t size);
	static void operator delete(void *ptr);
	bool isOnHeap() const;
};
std::list<const void*> HeapTracked::addresses; //静态对象在类外仍需定义
HeapTracked::~HeapTracked(){ //纯虚的析构函数必须要有再定义
}
void* HeapTracked::operator new(std::size_t size){
	void *memPtr=::operator new(size); //分配内存
	addresses.push_front(memPtr); //把地址压入列表前端
	return memPtr;
}
void HeapTracked::operator delete(void *ptr){
	if(ptr==0) return;
	//在列表中查找是否有这个指针
	std::list<const void*>::iterator it=
		find(addresses.begin(),addresses.end(),ptr);
	if(it!=addresses.end()){ //若找到，说明指针指向了堆上的内存
		addresses.erase(it); //从列表中移除这个指针
		::operator delete(ptr); //释放指针指向的内存
	}else{ //否则ptr不是一个堆对象的指针，不能调用operator delete，抛出异常
		throw MissingAddress();
	}
}
bool HeapTracked::isOnHeap() const{
	//获取*this对象的真正内存起始地址
	const void* rawAddress=dynamic_cast<const void*>(this);
	//在列表中查找this指针
	std::list<const void*>::iterator it=
		find(addresses.begin(),addresses.end(),rawAddress);
	return it!=addresses.end();
}
#endif

//HeapTracked是抽象混合基类，因为它有纯虚函数，因此是抽象基类，不能被实例化，只能被继承，由子类来创建对象。它的很多非虚的成员函数有功能实现，因此又是一般的基类，这些功能代表了各个子类的共性，因此把它称为抽象混合基类。注意当把析构函数声明为纯虚的时，必须同时要有定义，因为子类一定会调用基类的析构函数，这样它就必须有定义，而不只是声明。在使用时，让需要堆判断功能的类直接继承HeapTracked类即可。注意这个类只能判断单个对象是否在堆上，不能判断数组是否在堆上，当然可以通过重写operator new[]来完善。唯一需要解释的就是那个dynamic_cast，它把this指针转换成const void*。因为由多继承或虚基类继承而来的对象会有多个地址，因此我们必须要把一个指针dynamic_cast成void*类型，这就会使它指向对象的真正内存开始处。
//    3、禁止对象分配在堆上： 这个比较容易。把operator new/operator delete、operator new[]/operator delete[]都声明为私有即可。注意这样的类不能作为基类，也不能被其他类包含。

/*这样的设计只能判断单个的对象是否在堆上。我们还重写了operator new[]，这使得它可以判断整个数组所占的内存是否在堆上，但它却不能判断数组中的每个对象是否在堆上。从测试代码的运行结果就可以看出，对堆上的数组中的每个对象，只有第1个输出为1（即true），其余全部输出为0（即false），但我们知道实际上各个对象都在堆上的。这是因为创建整个数组时，operator new[]只会被调用一次，然后调用10次构造函数来初始化数组中的各个对象。当第1次调用构造函数时，onT*/heHeap被设为true，然后flag重置为false。后面的构造函数调用由于不会再调用operator new[]了，所以都会把onTheHeap设为false。