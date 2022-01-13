#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <sstream>
#include <iostream>
#define WINDOW_WIDTH 80
#define WINDOW_HEIGHT 25
#define STAGE_WIDTH 20
#define STAGE_HEIGHT 20
#define GRIDSIZE 25 //纹理尺寸
#define SCALE 0.5
#define MAXLENGTH 100
#define INFO_WIDTH 400
#define STEP 0.1

using namespace sf;
bool gameover;
bool gamequit;
bool gamestart = false;
bool gamepause;
const int width = STAGE_WIDTH;
const int height = STAGE_HEIGHT;

int x, y, fruitX, fruitY, score;
int tailX[MAXLENGTH], tailY[MAXLENGTH];
int ntail;
int delay;
int GameMode;

int soundVolume;
bool MusicOn;
bool MusicPause;

float stepX, stepY;
int headRotation = 0, tailRotation = 0;
enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
eDirection dir, dir_ing;
sf::RenderWindow window(sf::VideoMode(width* GRIDSIZE + INFO_WIDTH, height* GRIDSIZE + GRIDSIZE), L"程博源贪吃蛇");
Texture tBackground, tSnakeHead, tSnakeBody, tSnakeTail, tFruit,tBKG,tStart,tOver,tPause;
Sprite spBackground, spSnakeHead, spSnakeBody, spSnakeTail, spFruit,spBKG,spStart,spOver,spPause;

SoundBuffer sbEat, sbDie;
Sound soundEat, soundDie;
Music bkMusic;

Font font;
Text text;

void gameover_info(int x, int y)
{
	int initialX = 10, initialY = 0;
	int CharacterSize = 43;
	text.setCharacterSize(CharacterSize);
	text.setFillColor(Color(255, 255, 255, 255));
	text.setOutlineColor(Color(255, 100,100, 255));
	text.setOutlineThickness(10);
	text.setStyle(Text::Italic);

	text.setPosition(x + initialX, y + initialY);
	text.setString(L"     游戏结束");window.draw(text);
	initialY += CharacterSize;
	initialY += CharacterSize;
	text.setPosition(x + initialX, y + initialY);
	text.setString(L"Press Y to RESTART");window.draw(text);
	initialY += CharacterSize;
	text.setPosition(x + initialX, y + initialY);
	text.setString(L" Press N to QUIT");window.draw(text);
}

void Initial()
{
	window.setFramerateLimit(100);//每秒设置目标帧数

	if (!font.loadFromFile("data/Fonts/simsun.ttc"))
	{
		std::cout << "字体没有找到" << std::endl;
	}
	text.setFont(font);//加载指定字体

	tBackground.loadFromFile("data/images/BK.png");
	tSnakeHead.loadFromFile("data/images/龙头.png");
	tSnakeBody.loadFromFile("data/images/body.png");
	tSnakeTail.loadFromFile("data/images/尾巴.png");
	tFruit.loadFromFile("data/images/龙珠.png");
	tBKG.loadFromFile("data/images/china.jpg");
	tStart.loadFromFile("data/images/lz2.jpg");
	tOver.loadFromFile("data/images/lz2.jpg");
	tPause.loadFromFile("data/images/lz2.jpg");

	if (!sbEat.loadFromFile("data/Audios/Eat01.ogg"))
	{
		std::cout << "没有找到" << std::endl;
	}
	if (!sbDie.loadFromFile("data/Audios/Die01.ogg"))
	{
		std::cout << "没有找到" << std::endl;
	}
	if (!bkMusic.openFromFile("data/Audios/BGM01.ogg"))
	{
		std::cout << "没有找到" << std::endl;
	}
	soundEat.setBuffer(sbEat);
	soundDie.setBuffer(sbDie);//音效读入缓存

	bkMusic.play();//播放
	bkMusic.setLoop(true);//循环

	spBackground.setTexture(tBackground);
	spSnakeHead.setTexture(tSnakeHead);
	spSnakeBody.setTexture(tSnakeBody);
	spSnakeTail.setTexture(tSnakeTail);
	spFruit.setTexture(tFruit);
	spBKG.setTexture(tBKG);
	spStart.setTexture(tStart);
	spOver.setTexture(tOver);
	spPause.setTexture(tPause);

	spBackground.setScale(SCALE, SCALE);
	spSnakeHead.setScale(0.2, 0.2);
	spSnakeBody.setScale(SCALE, SCALE);
	spSnakeTail.setScale(0.5, 0.5);
	spFruit.setScale(0.12, 0.12);
	spBKG.setScale(1.2,1.1);
	spStart.setScale(1.2, 1.1);
	spOver.setScale(1.2, 1.1);
	spPause.setScale(1.2, 1.1);

	spSnakeHead.setOrigin(140, 320);
	spSnakeBody.setOrigin(GRIDSIZE / SCALE / 2, GRIDSIZE / SCALE / 2);
	spBackground.setOrigin(GRIDSIZE / SCALE / 2, GRIDSIZE / SCALE / 2);
	spSnakeTail.setOrigin(44, 40);
	spFruit.setOrigin(140, 140);
	spBKG.setOrigin(GRIDSIZE / SCALE / 2, GRIDSIZE / SCALE / 2);

	gameover = false;
	gamequit = false;
	gamepause = false;
	headRotation = 0;
	tailRotation = 0;
	delay = 0;
	dir = STOP;
	dir_ing = STOP;
	x = width / 2;
	y = height / 2;
	fruitX = rand() % width;
	fruitY = rand() % height;
	score = 0;
	soundVolume = 50;
	MusicOn = true;
	MusicPause = true;
	GameMode = 1;
	stepX = 0.0;
	stepY = 0.0;

	ntail = 1;
	for (int i = 0;i < MAXLENGTH;i++)
	{
		tailX[i] = 0;
		tailY[i] = 0;
	}
}

void Input()
{
	sf::Event evt;
	while (window.pollEvent(evt))
	{
		if (evt.type == sf::Event::Closed)
		{
			window.close();
			gameover = true;
			gamequit = true;
		}
		if (evt.type == sf::Event::EventType::KeyReleased && evt.key.code == sf::Keyboard::X)
		{
			window.close();
			gameover = false;
			gamequit = true;
			break;
		}
		if (evt.type == sf::Event::EventType::KeyReleased && evt.key.code == sf::Keyboard::Space)
		{
			if (GameMode == 1)
			{
				GameMode = 2;
			}
			else
			{
				GameMode = 1;
			}
		}
		if (evt.type == sf::Event::EventType::KeyReleased && evt.key.code == sf::Keyboard::I)
		{
			gamestart = true;
		}
		if (evt.type == sf::Event::EventType::KeyReleased && evt.key.code == sf::Keyboard::C)
		{
			gamepause = true;
		}
		if (evt.type == sf::Event::EventType::KeyReleased && evt.key.code == sf::Keyboard::O)
		{
			if (!gamestart)
			{
				window.close();
			}
		}
	}

	if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A))
	{
		if (dir != RIGHT)
		{
			dir = LEFT;
		}
	}
	else if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D))
	{
		if (dir != LEFT)
		{
			dir = RIGHT;
		}
	}
	else if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W))
	{
		if (dir != DOWN)
		{
			dir = UP;
		}
	}
	else if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S))
	{
		if (dir != UP)
		{
			dir = DOWN;
		}
	}

	if (evt.type == sf::Event::EventType::KeyReleased && evt.key.code == sf::Keyboard::J)
	{
		soundVolume += 5;
		bkMusic.setVolume(soundVolume);
	}
	if (evt.type == sf::Event::EventType::KeyReleased && evt.key.code == sf::Keyboard::K)
	{
		soundVolume -= 5;
		bkMusic.setVolume(soundVolume);
	}

	if (evt.type == sf::Event::EventType::KeyReleased && (evt.key.code == sf::Keyboard::P))
	{
		if (MusicPause == true)
		{
			bkMusic.pause();
			MusicPause = false;
		}
		else
		{
			bkMusic.play();
			MusicPause = true;
		}
	}

	if (evt.type == sf::Event::EventType::KeyReleased &&(evt.key.code == sf::Keyboard::Enter))
	{
		if (MusicOn == true)
		{
			bkMusic.stop();
			MusicOn = false;
		}
		else
		{
			bkMusic.play();
			MusicOn = true;
		}
	}
}

void Logic()
{
	int prevX = tailX[0];
	int prevY = tailY[0];
	int prev2X, prev2Y;
	tailX[0] = x;
	tailY[0] = y;

	switch (dir)
	{
	case LEFT:
		x--;
		headRotation = -90;
		break;
	case RIGHT:
		x++;
		headRotation = 90;
		break;
	case UP:
		y--;
		headRotation = 0;
		break;
	case DOWN:
		y++;
		headRotation = 180;
		break;
	default:
		break;
	}

	if (x == fruitX && y == fruitY)
	{
		score += 10;
		soundEat.play();
		fruitX = rand() % width;
		fruitY = rand() % height;
		ntail++;
	}

	for (int i = 1;i < ntail;i++)
	{
		prev2X = tailX[i];
		prev2Y = tailY[i];
		tailX[i] = prevX;
		tailY[i] = prevY;
		prevX = prev2X;
		prevY = prev2Y;
	}

	for (int i = 1;i < ntail;i++)
	{
		if (tailX[i] == x && tailY[i] == y)
		{
			soundDie.play();
			gameover = true;
		}
	}

	if (x >= width)
	{
		x = 0;
	}

	if (y >= height)
	{
		y = 0;
	}

	if (x < 0)
	{
		x = width - 1;
	}

	if (y < 0)
	{
		y = height - 1;
	}

	if (ntail >= 2)
	{
		if (tailY[ntail - 2] - tailY[ntail - 1] == 1 && tailX[ntail - 2] == tailX[ntail - 1])
		{
			tailRotation = 180;
		}
		if (tailY[ntail - 2] - tailY[ntail - 1] == -1 && tailX[ntail - 2] == tailX[ntail - 1])
		{
			tailRotation = 0;
		}
		if (tailX[ntail - 2] - tailX[ntail - 1] == 1 && tailY[ntail - 2] == tailY[ntail - 1])
		{
			tailRotation = 90;
		}
		if (tailX[ntail - 2] - tailX[ntail - 1] == -1 && tailY[ntail - 2] == tailY[ntail - 1])
		{
			tailRotation = -90;
		}
	}
}

void Prompt_info(int x, int y)
{
	int initialX = 20, initialY = 0;
	int CharacterSize = 24;
	text.setCharacterSize(CharacterSize);
	text.setFillColor(Color(255, 255, 255, 255));
	text.setOutlineThickness(2);
	text.setOutlineColor(Color(10,10,10,255));
	text.setStyle(Text::Bold);//text::underlined

	text.setPosition(x + initialX, y + initialY);
	text.setString(L"★ 游戏说明：");window.draw(text);
	initialY += CharacterSize;
	text.setPosition(x + initialX, y + initialY);
	text.setString(L"   A.蛇身自撞，游戏结束");window.draw(text);
	initialY += CharacterSize;
	text.setPosition(x + initialX, y + initialY);
	text.setString(L"   B.蛇可穿墙");window.draw(text);
	initialY += CharacterSize;
	initialY += CharacterSize;
	text.setPosition(x + initialX, y + initialY);
	text.setString(L"★ 操作说明：");window.draw(text);
	initialY += CharacterSize;
	text.setPosition(x + initialX, y + initialY);
	text.setString(L"   □ 向左移动：←A");window.draw(text);
	initialY += CharacterSize;
	text.setPosition(x + initialX, y + initialY);
	text.setString(L"   □ 向右移动：→D");window.draw(text);
	initialY += CharacterSize;
	text.setPosition(x + initialX, y + initialY);
	text.setString(L"   □ 向下移动：↓S");window.draw(text);
	initialY += CharacterSize;
	text.setPosition(x + initialX, y + initialY);
	text.setString(L"   □ 向上移动：↑W");window.draw(text);
	initialY += CharacterSize;
	text.setPosition(x + initialX, y + initialY);
	text.setString(L"   □ 开始游戏：任意方向键");window.draw(text);
	initialY += CharacterSize;
	text.setPosition(x + initialX, y + initialY);
	text.setString(L"   □ 暂停游戏：C键暂停");window.draw(text);
	initialY += CharacterSize;
	text.setPosition(x + initialX, y + initialY);
	text.setString(L"   □ 退出游戏：X键退出");window.draw(text);
	initialY += CharacterSize;
	text.setPosition(x + initialX, y + initialY);
	text.setString(L"   □ 背景音开关：Enter键");window.draw(text);
	initialY += CharacterSize;
	text.setPosition(x + initialX, y + initialY);
	text.setString(L"   □ 背景音音量：J/K键");window.draw(text);
	initialY += CharacterSize;
	text.setPosition(x + initialX, y + initialY);
	text.setString(L"   □ 动画模式切换：空格键");window.draw(text);
	initialY += CharacterSize;
	text.setPosition(x + initialX, y + initialY);
	text.setString(L"★当前得分：");window.draw(text);
	text.setFillColor(Color(255, 200, 2, 255));
	text.setPosition(x + initialX+CharacterSize*7, y + initialY);
	CharacterSize = 48;
	text.setCharacterSize(CharacterSize);
	std::stringstream ss;
	ss << score;
	text.setString(ss.str());window.draw(text);
	initialY += 60;
	text.setPosition(x + initialX, y + initialY);

	text.setCharacterSize(24);
	text.setFillColor(Color(255, 255, 255, 255));
	text.setOutlineThickness(2);
	text.setOutlineColor(Color(10, 10, 10, 255));
	text.setStyle(Text::Bold);//text::underlined

	text.setString(L"★ 作者：你的程博源爸爸");window.draw(text);


}

void Draw()
{

	window.clear();//清屏
	window.draw(spBKG);

	if (!gamestart)
	{
		text.setCharacterSize(100);
		text.setFillColor(Color(255, 200, 2, 255));
		text.setOutlineThickness(4);
		text.setOutlineColor(Color(10, 10, 10, 255));
		text.setStyle(Text::Italic);

		text.setPosition(190,100);
		text.setString(L"中华贪吃蛇");window.draw(text);

		text.setCharacterSize(38);
		text.setFillColor(Color(255, 255,255, 255));
		text.setOutlineThickness(2);
		text.setStyle(Text::Bold);
		
		text.setPosition(330,280);
		text.setString(L"开始游戏（I）");window.draw(text);
		
		text.setPosition(330,350);
		text.setString(L"退出游戏（O）");window.draw(text);
	}

	if (gamestart)
	{
		Prompt_info(width * GRIDSIZE + GRIDSIZE, GRIDSIZE);

		int detaX = GRIDSIZE / SCALE / 2;
		int detaY = GRIDSIZE / SCALE / 2;
		
		for (int i = 0;i < width;i++)
		{
			for (int j = 0;j < height;j++)
			{
				spBackground.setPosition(i * GRIDSIZE + detaX, j * GRIDSIZE + detaY);
				window.draw(spBackground);
			}
		}

		spSnakeHead.setPosition(tailX[0] * GRIDSIZE + detaX, tailY[0] * GRIDSIZE + detaY);
		spSnakeHead.setRotation(headRotation);
		window.draw(spSnakeHead);

		for (int i = 1;i < ntail - 1;i++)
		{

			spSnakeBody.setPosition(tailX[i] * GRIDSIZE + detaX, tailY[i] * GRIDSIZE + detaY);
			spSnakeBody.setRotation(tailRotation);
			window.draw(spSnakeBody);

		}

		if (ntail != 1)
		{
			spSnakeTail.setPosition(tailX[ntail - 1] * GRIDSIZE + detaX, tailY[ntail - 1] * GRIDSIZE + detaY);
			spSnakeTail.setRotation(tailRotation);
			window.draw(spSnakeTail);
		}


		spFruit.setPosition(fruitX * GRIDSIZE + detaX, fruitY * GRIDSIZE + detaY);
		window.draw(spFruit);

		if (gameover)
		{
			window.draw(spBKG);

			text.setCharacterSize(100);
			text.setFillColor(Color(255, 200, 2, 255));
			text.setOutlineThickness(4);
			text.setOutlineColor(Color(10, 10, 10, 255));
			text.setStyle(Text::Italic);

			text.setPosition(250, 100);
			text.setString(L"游戏结束");window.draw(text);

			text.setCharacterSize(38);
			text.setFillColor(Color(255, 255, 255, 255));
			text.setOutlineThickness(2);
			text.setStyle(Text::Bold);

			text.setPosition(330, 280);
			text.setString(L"重新开始（Y）");window.draw(text);

			text.setPosition(330, 350);
			text.setString(L"退出游戏（N）");window.draw(text);
		}

	}

	window.display();

	
}

void LogicStep()
{
	int prevX = tailX[0];
	int prevY = tailY[0];
	int prev2X, prev2Y;
	bool updateFlag = false;

	switch (dir_ing)
	{
	case LEFT:
		stepX -= STEP;
		if (stepX < -0.9999 || stepX >= 0.9999)
		{
			x--;
			stepX = 0;
			stepY = 0;
			dir_ing = dir;
			headRotation = -90;
			updateFlag = true;
		}
		break;
	case RIGHT:
		stepX += STEP;
		if (stepX < -0.9999 || stepX >= 0.9999)
		{
			x++;
			stepX = 0;
			stepY = 0;
			dir_ing = dir;
			headRotation = 90;
			updateFlag = true;
		}
		break;
	case UP:
		stepY -= STEP;
		if (stepY < -0.9999 || stepY >= 0.9999)
		{
			y--;
			stepX = 0;
			stepY = 0;
			dir_ing = dir;
			headRotation = 0;
			updateFlag = true;
		}
		break;
	case DOWN:
		stepY += STEP;
		if (stepY < -0.9999 || stepY >= 0.9999)
		{
			y++;
			stepX = 0;
			stepY = 0;
			dir_ing = dir;
			headRotation = 180;
			updateFlag = true;
		}
		break;
	default:
		dir_ing = dir;
		break;
	}

	tailX[0] = x;
	tailY[0] = y;

	if (updateFlag == true)
	{
		if (x == fruitX && y == fruitY)
		{
			score += 10;
			soundEat.play();
			fruitX = rand() % width;
			fruitY = rand() % height;
			ntail++;
		}

		for (int i = 1;i < ntail;i++)
		{
			prev2X = tailX[i];
			prev2Y = tailY[i];
			tailX[i] = prevX;
			tailY[i] = prevY;
			prevX = prev2X;
			prevY = prev2Y;
		}

		for (int i = 1;i < ntail;i++)
		{
			if (tailX[i] == x && tailY[i] == y)
			{
				soundDie.play();
				gameover = true;
			}
		}
	}

	if (x >= width)
	{
		x = 0;
	}

	if (y >= height)
	{
		y = 0;
	}

	if (x < 0)
	{
		x = width - 1;
	}

	if (y < 0)
	{
		y = height - 1;
	}

	if (ntail >= 2)
	{
		if (tailY[ntail - 2] - tailY[ntail - 1] == 1 && tailX[ntail - 2] == tailX[ntail - 1])
		{
			tailRotation = 180;
		}
		if (tailY[ntail - 2] - tailY[ntail - 1] == -1 && tailX[ntail - 2] == tailX[ntail - 1])
		{
			tailRotation = 0;
		}
		if (tailX[ntail - 2] - tailX[ntail - 1] == 1 && tailY[ntail - 2] == tailY[ntail - 1])
		{
			tailRotation = 90;
		}
		if (tailX[ntail - 2] - tailX[ntail - 1] == -1 && tailY[ntail - 2] == tailY[ntail - 1])
		{
			tailRotation = -90;
		}
	}
}

void DrawStep()
{
	if (gamestart) {

		window.draw(spBKG);

		Prompt_info(width * GRIDSIZE + GRIDSIZE, GRIDSIZE);
		if (gameover)
		{
			window.draw(spBKG);

			text.setCharacterSize(100);
			text.setFillColor(Color(255, 200, 2, 255));
			text.setOutlineThickness(4);
			text.setOutlineColor(Color(10, 10, 10, 255));
			text.setStyle(Text::Italic);

			text.setPosition(250, 100);
			text.setString(L"游戏结束");window.draw(text);

			text.setCharacterSize(38);
			text.setFillColor(Color(255, 255, 255, 255));
			text.setOutlineThickness(2);
			text.setStyle(Text::Bold);

			text.setPosition(330, 280);
			text.setString(L"重新开始（Y）");window.draw(text);

			text.setPosition(330, 350);
			text.setString(L"退出游戏（N）");window.draw(text);
		}

		int detaX = GRIDSIZE / SCALE / 2;
		int detaY = GRIDSIZE / SCALE / 2;

		for (int i = 0;i < width;i++)
		{
			for (int j = 0;j < height;j++)
			{
				spBackground.setPosition(i * GRIDSIZE + detaX, j * GRIDSIZE + detaY);
				window.draw(spBackground);
			}
		}
		//绘制蛇
		float stepLength;
		stepLength = stepX + stepY;
		if (stepLength < 0)
		{
			stepLength = -stepLength;
		}

		spSnakeHead.setPosition((tailX[0] + stepX) * GRIDSIZE + detaX, (tailY[0] + stepY) * GRIDSIZE + detaY);
		spSnakeHead.setRotation(headRotation);
		window.draw(spSnakeHead);

		for (int i = 1;i < ntail - 1;i++)
		{
			int t;
			if (tailY[i] == tailY[i - 1] && tailX[i] != tailX[i - 1])
			{
				t = tailX[i - 1] - tailX[i];
				if (t > 1 || t < -1)
				{
					t = 1;
				}
				spSnakeBody.setPosition((tailX[i] + t * stepLength) * GRIDSIZE + detaX, tailY[i] * GRIDSIZE + detaY);
			}
			if (tailY[i] != tailY[i - 1] && tailX[i] == tailX[i - 1])
			{
				t = tailY[i - 1] - tailY[i];
				if (t > 1 || t < -1)
				{
					t = 1;
				}
				spSnakeBody.setPosition(tailX[i] * GRIDSIZE + detaX, (tailY[i] + t * stepLength) * GRIDSIZE + detaY);
			}

			window.draw(spSnakeBody);
		}

		if (ntail != 1)
		{
			int t, p;
			t = tailX[ntail - 2] - tailX[ntail - 1];
			if (t > 1 || t < -1)
			{
				t = 1;
			}
			p = tailY[ntail - 2] - tailY[ntail - 1];
			if (p > 1 || p < -1)
			{
				p = 1;
			}

			spSnakeTail.setPosition((tailX[ntail - 1] + t * stepLength) * GRIDSIZE + detaX, (tailY[ntail - 1] + p * stepLength) * GRIDSIZE + detaY);
			spSnakeTail.setRotation(tailRotation);
			window.draw(spSnakeTail);
		}

		spFruit.setPosition(fruitX * GRIDSIZE + detaX, fruitY * GRIDSIZE + detaY);
		window.draw(spFruit);

		if (gameover)
		{
			gameover_info(width / 8 * GRIDSIZE, height / 4 * GRIDSIZE);
		}
	}

	window.display();

}


int main()
{
	
	do 
	{
		Initial();
		
		while (window.isOpen() && gameover == false)
		{
			
			Input();

			while (gamepause)
			{
				window.draw(spBKG);

				text.setCharacterSize(100);
				text.setFillColor(Color(255, 200, 2, 255));
				text.setOutlineThickness(4);
				text.setOutlineColor(Color(10, 10, 10, 255));
				text.setStyle(Text::Italic);

				text.setPosition(250, 100);
				text.setString(L"游戏暂停");window.draw(text);

				text.setCharacterSize(38);
				text.setFillColor(Color(255, 255, 255, 255));
				text.setOutlineThickness(2);
				text.setStyle(Text::Bold);

				text.setPosition(330, 280);
				text.setString(L"继续游戏（C）");window.draw(text);

				text.setPosition(330, 350);
				text.setString(L"退出游戏（N）");window.draw(text);

				window.display();

				Event pause;
				while (window.pollEvent(pause))
				{

					if (pause.type == Event::Closed)
					{
						window.close();
						gameover = false;
						gamequit = true;
					}
					if (pause.type == sf::Event::EventType::KeyReleased && pause.key.code == sf::Keyboard::C)
					{
						gamepause = false;
						break;
					}
					if (pause.type == sf::Event::EventType::KeyReleased && pause.key.code == sf::Keyboard::N)
					{
						window.close();
						gamepause = false;
						gameover = false;
						gamequit = true;
						break;
					}

				}
			}

			switch (GameMode)
			{
			case 1:
				delay++;
				if (delay % 10 == 0)
				{
					delay = 0;
					Logic();
				}
				Draw();
				
				break;

			case 2:
				LogicStep();
				DrawStep();
				break;
			}
		}
		while (gameover)
		{

			Event e;
			while (window.pollEvent(e))
			{

				if (e.type == Event::Closed)
				{
					window.close();
					gameover = false;
					gamequit = true;
				}
				if (e.type == sf::Event::EventType::KeyReleased && e.key.code == sf::Keyboard::Y)
					gameover = false;
				if (e.type == sf::Event::EventType::KeyReleased && e.key.code == sf::Keyboard::N)
				{
					gameover = false;
					gamequit = true;
				}

			}
		}
	} while (!gamequit);

	//system("pause");

	return 0;
}