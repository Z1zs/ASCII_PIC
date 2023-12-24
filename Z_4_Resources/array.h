#include <iostream>
#include<vector>
using namespace std;

class Array {
public:
	Array() {}

	Array(const Array& arr) {//复制构造函数
		//成员变量赋值
		this->index = arr.index;
		this->nowAxis = arr.nowAxis;
		this->axisNum = arr.axisNum;
		this->capcity = arr.capcity;
		for (int i = 0; i < this->axisNum; i++) {
			this->shape[i] = arr.shape[i];
		}
		//data赋值
		if (arr.flag) {//引用赋值
			this->data = arr.data;
		}
		else {//深拷贝
			this->data = new int[this->capcity];
			if (!data) {
				cout << "动态内存申请失败!" << endl;
				exit(1);
			}
			for (int i = 0; i < this->capcity; i++) {
				this->data[i] = arr.data[i];
			}
			this->flag = true;
		}
	}

	template<typename ...Args>
	Array(Args... args) {
		for (int i = 0; i < 16; i++) {//初始化
			shape[i] = 1;
		}

		int num = sizeof...(args);
		int list[] = { args... };

		int size = 1;
		axisNum = num;
		for (int i = 0; i < axisNum; i++) {
			shape[i] = list[i];
			size *= shape[i];
		}

		capcity = size;
		data = new int[size];
		if (!data) {
			cout << "动态内存申请失败!" << endl;
			exit(1);
		}

		index = 0;
		nowAxis = axisNum;
	}

	template<typename ...Args>
	Array at(Args... args) {

		auto num = sizeof...(args);
		size_t list[] = { args... };

		if (num > this->nowAxis) {//当前所在维度
			cout << "超出当前维度!" << endl;
			exit(1);
		}

		int span = 0, size1 = 1;
		for (int i = 0; i < num; i++) {
			size1 = list[i];
			for (int j = i + 1; j < this->nowAxis; j++) {//后面几维的跨度
				size1 *= shape[i];
			}
			span += size1;
		}

		Array ans;

		int p1 = 0;
		for (int p2 = 0; p2 < this->nowAxis; p2++) {
			if (p2 < num) {//忽略前面的值
				continue;
			}
			ans.shape[p1] = this->shape[p2];
			p1++;
		}

		ans.nowAxis = this->nowAxis - num;
		ans.index = this->index + span;
		ans.axisNum = this->axisNum;
		ans.data = this->data;//data不变
		ans.capcity = this->capcity;

		return ans;
	}

	template<typename ...Args>
	void reshape(Args... args) {
		//get args
		auto num = sizeof...(args);
		size_t list[] = { args... };
		//检查size
		int shape1 = 1, shape2 = 1;
		for (int i = 0; i < num; i++) {
			shape1 *= list[i];
			shape2 *= this->shape[i];
		}
		if (shape1 != shape2) {
			cout << "大小不匹配!" << endl;
			return;
		}
		//重新赋值
		for (int i = 0; i < 16; i++) {
			if (i < num) {
				shape[i] = list[i];
			}
			else {
				shape[i] = 1;
			}
		}
		axisNum = num;
		nowAxis = axisNum;
		this->index = 0;
	}

	int* get_content() {
		return data;
	}

	void set(int val) {
		data[index] = val;
	}

	Array operator[](int val) {
		// 在这里修改子矩阵的nowAxis的值以及其他有必要的值，以返回一个子矩阵
		if (this->nowAxis == 0) {
			cout << "array为空!" << endl;
			exit(1);
		}
		//获取跨度
		int span = val;
		for (int i = 1; i < this->nowAxis; i++) {
			span *= this->shape[i];
		}

		Array arr;

		for (int i = 1; i < this->nowAxis; i++) {//shape向前一位
			arr.shape[i - 1] = this->shape[i];
		}
		arr.axisNum = this->axisNum;
		arr.data = this->data;
		arr.index = this->index + span;
		arr.nowAxis = this->nowAxis - 1;//nowAxis向后一位
		arr.capcity = this->capcity;

		return arr;
	}

	~Array() {
		if (axisNum == nowAxis&&!data) {//引用时不释放
			delete[] data;
			data = NULL;
		}
	}

	Array& operator=(Array arr) {
		//数据成员赋值
		this->axisNum = arr.axisNum;
		this->capcity = arr.capcity;
		this->index = arr.index;
		this->nowAxis = arr.nowAxis;
		for (int i = 0; i < this->axisNum; i++) {
			this->shape[i] = arr.shape[i];
		}
		this->data = new int[this->capcity];
		if (!data) {
			cout << "动态内存申请失败!" << endl;
			exit(1);
		}
		//data赋值
		for (int i = 0; i < this->capcity; i++) {
			this->data[i] = arr.data[i];
		}
		this->flag = true;
		return *this;
	}


	Array operator=(int val) {//单个元素赋值
		data[index] = val;
		return *this;
	}

	operator int() {//隐式类型转换
		return data[index];
	}

	Array operator+(const Array& arr)const {
		if (this->capcity != arr.capcity) {
			cout << "维度不匹配!" << endl;
			exit(1);
		}

		Array ans;
		//初始化
		ans.axisNum = this->axisNum;
		ans.capcity = this->capcity;
		ans.index = this->index;
		ans.nowAxis = this->nowAxis;
		ans.data = new int[this->capcity];
		if (!data) {
			cout << "动态内存申请失败!" << endl;
			exit(1);
		}
		ans.flag = false;

		for (int i = 0; i < this->axisNum; i++) {
			ans.shape[i] = this->shape[i];
		}
		//data赋值
		for (int i = 0; i < this->capcity; i++) {
			long long int tmp = long long(this->data[i]) + arr.data[i];
			ans.data[i] = (int)tmp;
		}
		return ans;
	}

	Array operator+(int val)const {

		Array ans;
		ans.axisNum = this->axisNum;
		ans.capcity = this->capcity;
		ans.index = this->index;
		ans.nowAxis = this->nowAxis;
		ans.data = new int[this->capcity];
		if (!data) {
			cout << "动态内存申请失败!" << endl;
			exit(1);
		}
		ans.flag = false;

		for (int i = 0; i < this->axisNum; i++) {
			ans.shape[i] = this->shape[i];
		}
		//data赋值
		for (int i = 0; i < this->capcity; i++) {
			long long int tmp = long long(this->data[i]) + val;
			ans.data[i] = (int)tmp;
		}

		return ans;
	}

	Array operator-(const Array& arr) const {
		if (this->capcity != arr.capcity) {
			cout << "维度不匹配!" << endl;
			exit(1);
		}

		Array ans;
		//初始化
		ans.axisNum = this->axisNum;
		ans.capcity = this->capcity;
		ans.index = this->index;
		ans.nowAxis = this->nowAxis;
		ans.data = new int[this->capcity];
		if (!data) {
			cout << "动态内存申请失败!" << endl;
			exit(1);
		}
		ans.flag = false;

		for (int i = 0; i < this->axisNum; i++) {
			ans.shape[i] = this->shape[i];
		}
		//data赋值
		for (int i = 0; i < this->capcity; i++) {
			long long int tmp = long long(this->data[i]) - arr.data[i];
			ans.data[i] = (int)tmp;
		}

		return ans;
	}

	Array operator-(int val)const {

		Array ans;
		ans.axisNum = this->axisNum;
		ans.capcity = this->capcity;
		ans.index = this->index;
		ans.nowAxis = this->nowAxis;
		ans.data = new int[this->capcity];
		if (!data) {
			cout << "动态内存申请失败!" << endl;
			exit(1);
		}
		ans.flag = false;

		for (int i = 0; i < this->axisNum; i++) {
			ans.shape[i] = this->shape[i];
		}

		for (int i = 0; i < this->capcity; i++) {
			long long int tmp = long long(this->data[i]) - val;
			ans.data[i] = (int)tmp;
		}

		return ans;
	}

	Array operator*(const Array& arr)const {
		if (this->capcity != arr.capcity) {
			cout << "维度不匹配!" << endl;
			exit(1);
		}

		Array ans;
		//初始化
		ans.axisNum = this->axisNum;
		ans.capcity = this->capcity;
		ans.index = this->index;
		ans.nowAxis = this->nowAxis;
		ans.data = new int[this->capcity];
		if (!data) {
			cout << "动态内存申请失败!" << endl;
			exit(1);
		}
		ans.flag = false;

		for (int i = 0; i < this->axisNum; i++) {
			ans.shape[i] = this->shape[i];
		}
		//data赋值
		for (int i = 0; i < this->capcity; i++) {
			long long int tmp = long long(this->data[i]) * arr.data[i];
			ans.data[i] = (int)tmp;
		}

		return ans;
	}

	Array operator*(int val)const {

		Array ans;
		//初始化
		ans.axisNum = this->axisNum;
		ans.capcity = this->capcity;
		ans.index = this->index;
		ans.nowAxis = this->nowAxis;
		ans.data = new int[this->capcity];
		if (!data) {
			cout << "动态内存申请失败!" << endl;
			exit(1);
		}
		ans.flag = false;

		for (int i = 0; i < this->axisNum; i++) {
			ans.shape[i] = this->shape[i];
		}
		//data赋值
		for (int i = 0; i < this->capcity; i++) {
			long long int tmp = long long(this->data[i]) * val;
			ans.data[i] = (int)tmp;
		}

		return ans;
	}

	Array operator/(const Array& arr)const {
		if (this->capcity != arr.capcity) {
			exit(-1);
		}

		Array ans;
		//初始化
		ans.axisNum = this->axisNum;
		ans.capcity = this->capcity;
		ans.index = this->index;
		ans.nowAxis = this->nowAxis;
		ans.data = new int[this->capcity];
		if (!data) {
			cout << "动态内存申请失败!" << endl;
			exit(1);
		}
		ans.flag = false;//返回时深拷贝

		for (int i = 0; i < this->axisNum; i++) {
			ans.shape[i] = this->shape[i];
		}
		//data赋值
		for (int i = 0; i < this->capcity; i++) {
			long long int tmp = long long(this->data[i]) / (arr.data[i] == 0 ? 1 : arr.data[i]);
			ans.data[i] = (int)tmp;
		}

		return ans;
	}

	Array operator/(int val)const {
		Array ans;
		//初始化
		ans.axisNum = this->axisNum;
		ans.capcity = this->capcity;
		ans.index = this->index;
		ans.nowAxis = this->nowAxis;
		ans.data = new int[this->capcity];
		if (!data) {
			cout << "动态内存申请失败!" << endl;
			exit(1);
		}
		ans.flag = false;//返回时深拷贝

		for (int i = 0; i < this->axisNum; i++) {
			ans.shape[i] = this->shape[i];
		}
		//data赋值
		for (int i = 0; i < this->capcity; i++) {
			long long int tmp = long long(this->data[i]) / (val == 0 ? 1 : val);
			ans.data[i] = (int)tmp;
		}

		return ans;
	}
	void display() {
		cout << "data is:" << endl;
		for (int i = 0; i < shape[0]; i++) {
			for (int j = 0; j < shape[1]; j++) {
				cout << data[i * shape[1] + j] << " " << i << " " << j << endl;
			}
			cout << "***********************************" << endl << endl;
		}
		cout << endl << capcity << endl;
		cout << "shape is:" << endl;
		for (int i = 0; i < 16; i++) {
			cout << shape[i] << " ";
		}
		cout << endl << endl;
	}
	int* data = NULL;
	int index=0;
	int shape[16]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
	int axisNum=0;//总维度
	int nowAxis=0;//当前维度
	int capcity=0;
	bool flag = true;//深拷贝浅拷贝标志
};
