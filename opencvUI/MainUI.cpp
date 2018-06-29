#include <iostream>
#include "include.h"
#include <opencv2/opencv.hpp>
#include <mysql.h>
 
using namespace std;
using namespace cv;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
//렌더링 할 창
SDL_Window *window;
SDL_Renderer *renderer;
SDL_Event event;
SDL_Texture *texture;
TTF_Font * Font_Size[100];
TTF_Font * Font_Size2[100];
int TTF_DrawText(SDL_Renderer *Renderer, TTF_Font* Font, char* sentence, int x, int y, SDL_Color Color) {
	cout << "히읗";
	SDL_Surface * Surface = TTF_RenderText_Blended(Font, sentence, Color);// 폰트의 종류,문자열, 색깔을 보내서 유니코드로 렌더한다음 서피스에 저장한다
	SDL_Texture* Texture = SDL_CreateTextureFromSurface(Renderer, Surface);// 서피스로부터 텍스쳐를 생성한다
	SDL_FreeSurface(Surface);//서피스 메모리를 해제 해준다.
	SDL_Rect Src;
	Src.x = 0;
	Src.y = 0;
	SDL_QueryTexture(Texture, NULL, NULL, &Src.w, &Src.h);
	SDL_Rect Dst;
	Dst.x = x;
	Dst.y = y;
	Dst.w = Src.w;
	Dst.h = Src.h;
	SDL_RenderCopy(Renderer, Texture, &Src, &Dst); //그대로 렌더러에 저장한다
	SDL_DestroyTexture(Texture);
	return Src.w;// 출력할 문자열의 너비를 반환
}
char *  mysqls(char* word)
{

	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;

	char *server = "localhost";
	char *user = "root";
	char *password = "1234";
	char *database = "opencv";
	char* mean=NULL;
	cout << word << endl;
	conn = mysql_init(NULL);

	if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0))
	{
		exit(1);
	}
	if (mysql_query(conn, "show tables"))
	{
		exit(1);
	}

	res = mysql_use_result(conn);
	printf("MYSQL Tables in mysql database : \n");
	while ((row = mysql_fetch_row(res)) != NULL)
		printf("%s \n", row[0]);
	string command = "SELECT * FROM term WHERE word = \'";
	command += word;
	command += '\'';
	command += ';';
	cout << command << endl;
	if (mysql_query(conn, command.c_str()))
	{
		return NULL;
	}

	res = mysql_use_result(conn);

	printf("Returning List of Names : \n");
	while ((row = mysql_fetch_row(res)) != NULL)
	{
		cout << "YEAH"<<endl;
		cout << row[0] << endl << row[1] << endl << row[2] << endl << row[3] << endl;
		mean = row[0];
		cout << "after" << endl;
	}
	cout << mean << endl;
	
	mysql_free_result(res);
	mysql_close(conn);
	cout << "afterafter" << endl;
	return mean;
}
char * tessract() {
	string filePath = "Tess.txt";
	char * word;
	char* mean=NULL;
	//Mat image = imread("./caputred.jpg", IMREAD_COLOR);
	//imwrite("Tess.jpg", image);
	//system("dir");
	//system("tesseract Tess.png Tess -l eng");
	
	ifstream openFile(filePath.data());
	if (openFile.is_open()) {
		string line;
		while (getline(openFile, line)) {
			std::vector<char> writable(line.begin(), line.end());
			writable.push_back('\0');
			word = &writable[0];
			mean = mysqls(word);
		}
		openFile.close();
	}


	return mean;
}
int main(int argc, char* args[])
{
	char * mean;
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Rect texture_rect;
	texture_rect.x = 0;  //the x coordinate
	texture_rect.y = 0; // the y coordinate
	texture_rect.w = 800; //the width of the texture
	texture_rect.h = 600; //the height of the texture
	TTF_Init();
	TTF_Font* font = TTF_OpenFont(".\\font\\NanumGothic.ttf", 35);
	SDL_Color color = { 0,0,0,0 };
	window = SDL_CreateWindow("1", 100, 100, 200, 100, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	texture = IMG_LoadTexture(renderer, ".\\white.png");
	SDL_RenderCopy(renderer, texture, NULL, &texture_rect);
	TTF_DrawText(renderer, font, "capture", 40,20, color);
	VideoCapture capture(0);
	if (!capture.isOpened())
	{
		cout << "카메라가 연결되지 않음" << endl;
		exit(1);
	}
	double fps = 29.97;
	int delay = cvRound(1000.0 / fps);
	Size size(640, 360);
	int fourcc = VideoWriter::fourcc('D', 'X', '5', '0');
	capture.set(CAP_PROP_FRAME_WIDTH, size.width);
	capture.set(CAP_PROP_FRAME_HEIGHT, size.height);

	bool quit = false;
	while (!quit)
	{
		Mat frame;
		capture >> frame;
		if (SDL_WaitEventTimeout(&event, 1))
			switch (event.type)
			{
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					int x, y;
					x = event.button.x;
					y = event.button.y;
					SDL_RenderPresent(renderer);
					//imwrite("caputred.jpg", frame);
					
					mean = tessract();
					std::cout << mean;
				}
			}

		imshow("camera", frame);
		SDL_RenderPresent(renderer);

	}
	waitKey(0);
	return 0;
}
