#include "PicReader.h"
#include "FastPrinter.h"
#include <windows.h>
#include<mmsystem.h>
#include <string>
#pragma comment(lib, "winmm.lib")


#define VLEN 46
#define FRAME_NUM 502

#define HEIGHT 120
#define WIDTH 120
void RGBA2Grey( Array& rgba,Array& grey) {

	for (int i = 0; i < grey.shape[0]; i++) {
		for (int j = 0; j < grey.shape[1]; j++) {
			//两个像素合并为一个
			int r = int(rgba[i*2][j][0]), g = int(rgba[i*2][j][1]), b = int(rgba[i*2][j][2]);
			int tmp1 = (int)(r * 0.299 + g * 0.587 + b * 0.114);
			int r2 = int(rgba[i*2+1][j][0]), g2 = int(rgba[i*2+1][j][1]), b2 = int(rgba[i*2+1][j][2]);
			int tmp2 = (int)(r2 * 0.299 + g2 * 0.587 + b2 * 0.114);


			grey[i][j] = min((tmp1 + tmp2) / 2, 255);
			grey[i][j] = max((tmp1 + tmp2) / 2, 0);
		}
	}
	
	return;
}
/*灰度图转字符画*/
void Grey2Ascii(Array& grey,Array& arr) {
	//确定窗口大小
	int win = max(grey.shape[0] / HEIGHT, grey.shape[1] / WIDTH);
	if (win < 1) {
		win = 1;
	}
	int width = grey.shape[1] / win, height = grey.shape[0] / win;
	//array初始化
	arr.axisNum = 2;
	arr.capcity = width * height;
	arr.index = 0;
	arr.shape[1] = width;
	arr.shape[0] = height;
	arr.nowAxis = arr.axisNum;
	arr.data = new int[arr.capcity];
	if (!arr.data) {
		cout << "动态内存申请失败!" << endl;
		exit(1);
	}
	//滑动窗口
	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			int val = 0;
			for (int k = 0; k < win; k++) {
				for (int j = 0; j < win; j++) {
					val += int(grey[row * win + k][col * win + j]);
				}
			}
			val /= (win * win);

			char asciiStrength[] = { 'M', 'N', 'H', 'Q', '$', 'O', 'C', '?','7', '>', '!', ':', '-', ';', '.'};
			int index = val / 18;
			arr[row][col] = asciiStrength[index];
		}
	}
	return;
}

void showASCIIPic(Array& ascii) {
	
	SMALL_RECT drawArea;
	char* dataBuffer = new char[ascii.shape[0]*ascii.shape[1]];
	BYTE* frontColorBuffer = new BYTE[ascii.shape[0]*ascii.shape[1]*3];
	BYTE* backColorBuffer = new BYTE[ascii.shape[0]*ascii.shape[1]*3];
	if (!dataBuffer || !frontColorBuffer || !backColorBuffer) {
		cout << "动态内存分配失败!" << endl;
		return;
	}

	drawArea.Left = 0;
	drawArea.Right = drawArea.Left + ascii.shape[1];
	drawArea.Top = 0;
	drawArea.Bottom = drawArea.Top + ascii.shape[0];
	
	
	for (int i = 0; i < (drawArea.Right - drawArea.Left) * (drawArea.Bottom - drawArea.Top); i++) {
		dataBuffer[i] = char(ascii.data[i]);
		frontColorBuffer[i * 3] = 0;
		frontColorBuffer[i * 3 + 1] = 0;
		frontColorBuffer[i * 3 + 2] = 0;

		backColorBuffer[i * 3] = 255;
		backColorBuffer[i * 3 + 1] = 255;
		backColorBuffer[i * 3 + 2] = 255;
	}

	FastPrinter printer(ascii.shape[1], ascii.shape[0]);
	printer.cleanSrceen();
	printer.setData(dataBuffer, frontColorBuffer, backColorBuffer, drawArea);
	printer.draw(true);
	getchar();

	delete[] dataBuffer;
	delete[] frontColorBuffer;
	delete[] backColorBuffer;
}

void readVideo(Array* video) {

	std::string pre = "dress\\dress_000";
	std::string suf = ".jpg";
	cout << "正在读取视频帧......" << endl;
	cout << "该过程可能需要一分钟......" << endl;
	for (int i = 0; i < FRAME_NUM; i++) {
		std::string str = std::to_string(i + 1);
		if (i+1 < 10) {
			str = "00" + str;
		}
		else if (i + 1 < 100) {
			str = "0" + str;
		}
		if ((i + 1) % 10 == 0) {
			cout << "进度:" << (i + 1) << "/" << FRAME_NUM << endl;
		}
		string path = pre + str + suf;
		PicReader imread;
		UINT x = 0, y = 0;
		imread.readPic(path.c_str());
		Array pic;
		imread.getData(pic, x, y);
		Array grey(pic.shape[0]/2, pic.shape[1]);
		RGBA2Grey(pic, grey);
		Grey2Ascii(grey, video[i]);
	}
	return;
}

void showASCIIVideo(Array* video) {
	SMALL_RECT drawArea;
	char* dataBuffer = new char[video[0].shape[0]*video[0].shape[1]];
	BYTE* frontColorBuffer = new BYTE[video[0].shape[0] * video[0].shape[1] * 3];
	BYTE* backColorBuffer = new BYTE[video[0].shape[0] * video[0].shape[1] * 3];
	if (!dataBuffer || !frontColorBuffer || !backColorBuffer) {
		cout << "动态内存分配失败!" << endl;
		return;
	}

	drawArea.Left = 0;
	drawArea.Right = drawArea.Left + video[0].shape[1];
	drawArea.Top = 0;
	drawArea.Bottom = drawArea.Top + video[0].shape[0];


	for (int i = 0; i < (drawArea.Right - drawArea.Left) * (drawArea.Bottom - drawArea.Top); i++) {

		frontColorBuffer[i * 3] = 0;
		frontColorBuffer[i * 3 + 1] = 0;
		frontColorBuffer[i * 3 + 2] = 0;

		backColorBuffer[i * 3] = 255;
		backColorBuffer[i * 3 + 1] = 255;
		backColorBuffer[i * 3 + 2] = 255;
	}

	FastPrinter printer(video[0].shape[1], video[0].shape[0]);
	PlaySoundA("dress.wav", NULL, SND_FILENAME | SND_ASYNC);
	for (int i = 0; i < FRAME_NUM; i++) {
		//给databuffer赋值
		for (int j = 0; j < (drawArea.Right - drawArea.Left) * (drawArea.Bottom - drawArea.Top); j++) {
			dataBuffer[j] = (char)(video[i].data[j]);
		}
		printer.cleanSrceen();
		printer.setData(dataBuffer, frontColorBuffer, backColorBuffer, drawArea);
		printer.draw(true);
		Sleep(VLEN * 1000 / FRAME_NUM);
	}

	delete[] dataBuffer;
	delete[] frontColorBuffer;
	delete[] backColorBuffer;
}

int main() {
	
	/*string pic_path[11] = {"lena1.jpg","goldhill.jpg","barbara.jpg","woman.jpg","milkdrop.jpg","cameraman.jpg","compa.png","airplane.jpg","lena.jpg","baboon.jpg","peppers.jpg"};
	for (int i = 0; i < 11; i++) {
		PicReader imread;
		BYTE* data = nullptr;
		UINT x = 0, y = 0;
		string path = "classic_picture\\" + pic_path[i];
		imread.readPic(path.c_str());
		Array pic;
		imread.getData(pic, x, y);
		Array grey(pic.shape[0]/2, pic.shape[1]), ascii;
		RGBA2Grey(pic, grey);
		Grey2Ascii(grey, ascii);

		showASCIIPic(ascii);
	}*/

	Array video[FRAME_NUM];
	readVideo(video);
	showASCIIVideo(video);
	

	return 0;
}