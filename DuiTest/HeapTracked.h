#pragma once

//HeapTracked����жϵ����Ķ����Ƿ���ڶ���
#ifndef HEAP_TRACKED_HPP
#define HEAP_TRACKED_HPP
#include <cstddef>
#include <list>
class HeapTracked{
private:
	static std::list<const void*> addresses; //��Ÿ����Ѷ����ָ��
public:
	class MissingAddress{ }; //�쳣��
	virtual ~HeapTracked()=0; //���麯��
	static void* operator new(std::size_t size);
	static void operator delete(void *ptr);
	bool isOnHeap() const;
};
std::list<const void*> HeapTracked::addresses; //��̬�������������趨��
HeapTracked::~HeapTracked(){ //�����������������Ҫ���ٶ���
}
void* HeapTracked::operator new(std::size_t size){
	void *memPtr=::operator new(size); //�����ڴ�
	addresses.push_front(memPtr); //�ѵ�ַѹ���б�ǰ��
	return memPtr;
}
void HeapTracked::operator delete(void *ptr){
	if(ptr==0) return;
	//���б��в����Ƿ������ָ��
	std::list<const void*>::iterator it=
		find(addresses.begin(),addresses.end(),ptr);
	if(it!=addresses.end()){ //���ҵ���˵��ָ��ָ���˶��ϵ��ڴ�
		addresses.erase(it); //���б����Ƴ����ָ��
		::operator delete(ptr); //�ͷ�ָ��ָ����ڴ�
	}else{ //����ptr����һ���Ѷ����ָ�룬���ܵ���operator delete���׳��쳣
		throw MissingAddress();
	}
}
bool HeapTracked::isOnHeap() const{
	//��ȡ*this����������ڴ���ʼ��ַ
	const void* rawAddress=dynamic_cast<const void*>(this);
	//���б��в���thisָ��
	std::list<const void*>::iterator it=
		find(addresses.begin(),addresses.end(),rawAddress);
	return it!=addresses.end();
}
#endif

//HeapTracked�ǳ����ϻ��࣬��Ϊ���д��麯��������ǳ�����࣬���ܱ�ʵ������ֻ�ܱ��̳У��������������������ĺܶ����ĳ�Ա�����й���ʵ�֣��������һ��Ļ��࣬��Щ���ܴ����˸�������Ĺ��ԣ���˰�����Ϊ�����ϻ��ࡣע�⵱��������������Ϊ�����ʱ������ͬʱҪ�ж��壬��Ϊ����һ������û���������������������ͱ����ж��壬����ֻ����������ʹ��ʱ������Ҫ���жϹ��ܵ���ֱ�Ӽ̳�HeapTracked�༴�ɡ�ע�������ֻ���жϵ��������Ƿ��ڶ��ϣ������ж������Ƿ��ڶ��ϣ���Ȼ����ͨ����дoperator new[]�����ơ�Ψһ��Ҫ���͵ľ����Ǹ�dynamic_cast������thisָ��ת����const void*����Ϊ�ɶ�̳л������̳ж����Ķ�����ж����ַ��������Ǳ���Ҫ��һ��ָ��dynamic_cast��void*���ͣ���ͻ�ʹ��ָ�����������ڴ濪ʼ����
//    3����ֹ��������ڶ��ϣ� ����Ƚ����ס���operator new/operator delete��operator new[]/operator delete[]������Ϊ˽�м��ɡ�ע���������಻����Ϊ���࣬Ҳ���ܱ������������

/*���������ֻ���жϵ����Ķ����Ƿ��ڶ��ϡ����ǻ���д��operator new[]����ʹ���������ж�����������ռ���ڴ��Ƿ��ڶ��ϣ�����ȴ�����ж������е�ÿ�������Ƿ��ڶ��ϡ��Ӳ��Դ�������н���Ϳ��Կ������Զ��ϵ������е�ÿ������ֻ�е�1�����Ϊ1����true��������ȫ�����Ϊ0����false����������֪��ʵ���ϸ��������ڶ��ϵġ�������Ϊ������������ʱ��operator new[]ֻ�ᱻ����һ�Σ�Ȼ�����10�ι��캯������ʼ�������еĸ������󡣵���1�ε��ù��캯��ʱ��onT*/heHeap����Ϊtrue��Ȼ��flag����Ϊfalse������Ĺ��캯���������ڲ����ٵ���operator new[]�ˣ����Զ����onTheHeap��Ϊfalse��