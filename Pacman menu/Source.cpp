#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <sstream>
#include <cmath>
#include <iostream>
#include <queue>
#define N 20
#define M 26
#define scalex 832/1280
#define scaley 640/1024
using namespace std;
using namespace sf;
int frames = 0;
int frames2 = 0;
/** STRUCTS **/
struct Tile {
	int sizeX = 32;
	int sizeY = 32;
	int posX;
	int posY;
	Sprite sprite;
};
struct Dot {
	int sizeX = 32;
	int sizeY = 32;
	int posX;
	int posY;
	Sprite sprite;
};
struct Player {
	string directionToCheck = "none";
	string direction = "none";
	int posX;
	int posY;
	Sprite sprite;
};
struct point {
	int x, y, z;
};
point make_point(int x, int y, int z)
{
	point t;
	t.x = x;
	t.y = y;
	t.z = z;
	return t;
}
struct QItem {
	int row;
	int col;
	int dist;
};
Player pacman;
/** VECTORS AND VARIABLES **/
/// Map : Vector --> 32 Row , 40 column
int checkMapVec[20][26];
int mapVec[20][26]{
	{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
	{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
	{ 1,0,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,0,1 },
	{ 1,0,1,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1,0,1 },
	{ 1,0,1,0,0,1,1,1,1,1,1,0,1,0,0,1,1,1,1,1,1,0,0,1,0,1 },
	{ 1,0,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,0,1 },
	{ 1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,1 },
	{ 1,0,1,0,0,1,0,1,0,1,1,1,1,0,1,1,1,0,1,0,1,0,0,1,0,1 },
	{ 1,0,1,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,1,0,1 },
	{ 1,0,0,0,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,0,0,0,1 },
	{ 1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,1 },
	{ 1,0,1,1,0,0,0,1,1,1,1,0,1,0,0,1,1,1,1,0,0,0,1,1,0,1 },
	{ 1,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0,0,1 },
	{ 1,0,1,1,0,1,0,1,1,0,0,1,1,1,1,0,0,1,1,0,1,0,1,1,0,1 },
	{ 1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,1 },
	{ 1,0,1,0,1,1,1,1,1,1,1,0,1,0,0,1,1,1,1,1,1,1,0,1,0,1 },
	{ 1,0,1,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1,0,1 },
	{ 1,0,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,0,1 },
	{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
	{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 }
};
vector < int > fpath;
bool frightened = false, freezed = false, boosted = false;
bool vis[32][40];
int dp[32][40][32][40];
int cntr = 0, cntr2 = 0;
vector <Tile> tilemap;
vector <Dot> dotmap;
vector <Sprite> spriteGhost(4);
string dirst[4] = { "right" , "left" , "up" , "down" };
int dirx[4] = { 2,-2,0,0 };
int diry[4] = { 0,0,-2,2 };
bool zaky[4] = { 0 ,0 ,0 ,0 };
bool ghostMoving[4], mrra = 0;
string ghostDir[4];
bool food = false, lose = false, win = false, paused = true;
int score = 0, pacmanSpeed = 4;
/** Functions **/
/** Store Tile And Dot Info **/
void getTheMap(sf::Texture &spriteSheet) {
	for (int rows = 0; rows < 20; rows++)
	{
		for (int columns = 0; columns < 26; columns++)
		{
			if (mapVec[rows][columns] == 1)
			{
				Tile block;
				block.posX = columns * 32;
				block.posY = rows * 32;
				block.sprite.setTexture(spriteSheet);
				block.sprite.setTextureRect(IntRect(264, 0, 32, 32));
				block.sprite.setPosition(block.posX, block.posY);
				tilemap.push_back(block);
			}
			else if (mapVec[rows][columns] == 0)
			{
				Dot block;
				block.posX = columns * 32;
				block.posY = rows * 32;
				block.sprite.setTexture(spriteSheet);
				block.sprite.setTextureRect(IntRect(298, 0, 32, 32));
				block.sprite.setPosition(block.posX, block.posY);
				dotmap.push_back(block);
			}
		}
	}
}
/** Check Collision with Tilemap **/
bool checkCollision(vector <Tile> tilemap, int posX, int posY, string type) {
	int x, y;
	if (type == "ghost") x = 32, y = 32;
	else x = 32, y = 32;
	for (int i = 0; i< tilemap.size(); i++)
	{
		if ((tilemap[i].sprite.getPosition().x < posX + x) && (tilemap[i].sprite.getPosition().x + y > posX) && (tilemap[i].sprite.getPosition().y < posY + x) && (tilemap[i].sprite.getPosition().y + y > posY))
		{
			return true;
		}

	}
	return false;
}
/** Check Collision of the ghosts **/
bool checkLose(vector<Sprite> spritevec, int posX, int posY) {
	for (int i = 0; i< spritevec.size(); i++)
	{
		if ((spritevec[i].getPosition().x < posX + 32) && (spritevec[i].getPosition().x + 32 > posX) && (spritevec[i].getPosition().y < posY + 32) && (spritevec[i].getPosition().y + 32 > posY))
		{
			return true;
		}

	}
	return false;
}
/** Destroy Eaten parts **/
void checkFood(int posX, int posY) {
	for (int i = 0; i< dotmap.size(); i++) {
		if ((dotmap[i].sprite.getPosition().x < posX + 32) && (dotmap[i].sprite.getPosition().x + 32 > posX) && (dotmap[i].sprite.getPosition().y < posY + 32) && (dotmap[i].sprite.getPosition().y + 32 > posY)) {
			dotmap.erase(dotmap.begin() + i);
			score++;
			food = true;
			i--;
		}
	}
}
/** Generate a Random direction **/
string generateDir() {
	int rnd = rand() % 4;
	if (rnd == 0)return "right";
	if (rnd == 1)return "left";
	if (rnd == 2)return "up";
	if (rnd == 3)return "down";
}
void pacmanMove(int pix) {
	if (pacman.direction == "right")pacman.posX += pix;
	else if (pacman.direction == "left")pacman.posX -= pix;
	else if (pacman.direction == "up")pacman.posY -= pix;
	else if (pacman.direction == "down")pacman.posY += pix;
	pacman.sprite.setPosition(pacman.posX, pacman.posY);
}
void pacmanTextRec() {
	if (pacman.direction == "right")pacman.sprite.setTextureRect(IntRect(0, 0, 64, 64));
	else if (pacman.direction == "left")pacman.sprite.setTextureRect(IntRect(66, 0, 64, 64));
	else if (pacman.direction == "up")pacman.sprite.setTextureRect(IntRect(132, 0, 64, 64));
	else if (pacman.direction == "down")pacman.sprite.setTextureRect(IntRect(198, 0, 64, 64));
}
void PacmansMovement(int speed) {
	pacmanMove(speed);
	if (pacman.directionToCheck != "none") {
		string dire = pacman.direction;
		pacman.direction = pacman.directionToCheck;
		pacmanMove(speed);
		if (checkCollision(tilemap, pacman.posX, pacman.posY, "pacman")) {
			pacmanMove(-speed);
			pacman.direction = dire;
			if (checkCollision(tilemap, pacman.posX, pacman.posY, "pacman")) {
				pacmanMove(-speed);
				pacman.direction = "none";
				pacman.directionToCheck = "none";
			}
		}
		else {
			pacman.directionToCheck = "none";
			pacmanMove(-speed);
		}
	}
	else if (checkCollision(tilemap, pacman.posX, pacman.posY, "pacman")) {
		pacmanMove(-speed);
		pacman.direction = "none";
	}
	pacmanTextRec();
}
void setDir(string dir) {
	if (pacman.direction == "none")pacman.direction = dir;
	else pacman.directionToCheck = dir;
}

string last;
int mshbfs(int mapVec[N][M])
{
	point source = make_point(0, 0, 0);
	bool visited[N][M];
	for (int i = 0; i < 20; i++) {
		for (int j = 0; j < 26; j++) {
			if (mapVec[i][j] == 1)visited[i][j] = true;
			else visited[i][j] = false;
			if (mapVec[i][j] == 3)source.x = i, source.y = j;

		}
	}
	queue<point> q;
	q.push(source);
	visited[source.x][source.y] = true;
	while (!q.empty()) {
		point p = make_point(q.front().x, q.front().y, q.front().z);
		q.pop();
		if (mapVec[p.x][p.y] == 5)return p.z;
		if (p.x - 1 >= 0 && visited[p.x - 1][p.y] == false) {
			q.push(make_point(p.x - 1, p.y, p.z + 1));
			visited[p.x - 1][p.y] = true;
			if (mapVec[p.x - 1][p.y] == 5)last = "down";
		}
		if (p.x + 1 < N && visited[p.x + 1][p.y] == false) {
			q.push(make_point(p.x + 1, p.y, p.z + 1));
			visited[p.x + 1][p.y] = true;
			if (mapVec[p.x + 1][p.y] == 5)last = "up";
		}
		if (p.y - 1 >= 0 && visited[p.x][p.y - 1] == false) {
			q.push(make_point(p.x, p.y - 1, p.z + 1));
			visited[p.x][p.y - 1] = true;
			if (mapVec[p.x][p.y - 1] == 5)last = "right";
		}
		if (p.y + 1 < M &&visited[p.x][p.y + 1] == false) {
			q.push(make_point(p.x, p.y + 1, p.z + 1));
			visited[p.x][p.y + 1] = true;
			if (mapVec[p.x][p.y + 1] == 5)last = "left";
		}
	}
	return -1;
}
string OppDirection(string dir) {
	if (dir == "right")return "left";
	if (dir == "left")return "right";
	if (dir == "down")return "up";
	if (dir == "up")return "down";
}
string generateGoodDirection(Sprite spriteGhosts) {
again:
	string dir = generateDir();
	for (int i = 0;i < 4;i++) {
		if (dir == dirst[i]) {
			if (!checkCollision(tilemap, spriteGhosts.getPosition().x + dirx[i], spriteGhosts.getPosition().y + diry[i], "ghost")) return dir;
			else goto again;
		}
	}
}
bool checkDirection(string dir, Sprite spriteGhosts) {
	for (int i = 0;i < 4;i++) {
		if (dir == dirst[i]) {
			if (!checkCollision(tilemap, spriteGhosts.getPosition().x + dirx[i], spriteGhosts.getPosition().y + diry[i], "ghost")) return true;
		}
	}
	return false;
}
int main()
{
		rpt:;
	sf::RenderWindow window(sf::VideoMode(832, 640), "Pacman");
	bool mainmenu = true;
	bool creditsmenu = false;
	//select
	int selectn = 0;
	bool isselect1 = false;
	bool isselect2 = false;
	bool isselect3 = false;
	bool isselect4 = false;
	bool GAME = false;
	//background
	sf::RectangleShape background(sf::Vector2f(832.0f, 640.0f));
	sf::Texture backgroundt;
	backgroundt.loadFromFile("backround.jpg");
	background.setTexture(&backgroundt);
	background.setOrigin(640 * scalex, 512 * scaley);
	background.setPosition(640 * scalex, 512 * scaley);
	//mianmenu
	sf::Font mainf;
	mainf.loadFromFile("PAC-FONT.ttf");
	sf::Text maint;
	maint.setFont(mainf);
	maint.setPosition(300 * scalex, 180 * scaley);
	maint.setScale(2, 2);
	maint.setString("MAIN MENU");
	maint.setOutlineThickness(2);
	maint.setFillColor(sf::Color(0, 0, 0));
	maint.setOutlineColor(sf::Color(0, 0, 155));
	maint.Bold;
	//selections
	sf::Font selectf;
	selectf.loadFromFile("Font.ttf");
	sf::Text select1; select1.setFont(selectf); select1.setString("start"); select1.setPosition(530 * scalex, 430 * scaley); select1.setScale(2, 2); select1.setFillColor(sf::Color::Blue);
	sf::Text select2; select2.setFont(selectf); select2.setString("settings"); select2.setPosition(460 * scalex, 530 * scaley); select2.setScale(2, 2); select2.setFillColor(sf::Color::Blue);
	sf::Text select3; select3.setFont(selectf); select3.setString("credits"); select3.setPosition(490 * scalex, 630 * scaley); select3.setScale(2, 2); select3.setFillColor(sf::Color::Blue);
	sf::Text select4; select4.setFont(selectf); select4.setString("quit"); select4.setPosition(580 * scalex, 730 * scaley); select4.setScale(2, 2); select4.setFillColor(sf::Color::Blue);
	//soundmenu
	bool bsoundOn = true;
	bool soundmenu = false;
	sf::Text soundOn;
	sf::Text soundOff;
	soundOff.setFont(selectf); soundOff.setString("sound on"); soundOff.setPosition(500 * scalex, 430 * scaley); soundOff.setScale(2, 2); soundOff.setFillColor(sf::Color::Blue);
	soundOn.setFont(selectf); soundOn.setString("sound off"); soundOn.setPosition(500 * scalex, 430 * scaley); soundOn.setScale(2, 2); soundOn.setFillColor(sf::Color::Blue);
	//sound
	sf::SoundBuffer selects;
	selects.loadFromFile("select.wav");
	sf::Sound selectpeep;
	selectpeep.setBuffer(selects);
	//creditmenu
	sf::SoundBuffer creditsound;
	creditsound.loadFromFile("credit.wav");
	sf::Sound creditmusic;
	creditmusic.setBuffer(creditsound);
	sf::RectangleShape cbackground(sf::Vector2f(1280 * scalex, 1024 * scaley)); cbackground.setOrigin(640 * scalex, 512 * scaley); cbackground.setPosition(640 * scalex, 512 * scaley);
	sf::Texture cbackgroundtex;
	cbackgroundtex.loadFromFile("credits.png");
	cbackground.setTexture(&cbackgroundtex);
	sf::Text credits;
	credits.setFont(selectf);
	credits.setString("credits to the procrastinators hasan assem, hazem essawi, khaled mohammed, kareem hany, hossam ahmed"); credits.setPosition(800 * scalex, 730 * scaley); credits.setScale(1, 1); credits.setFillColor(sf::Color::Blue);

	while (window.isOpen())
	{ //mainmenu
		while (mainmenu)
		{
			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();
				//std::cout << sf::Mouse::getPosition(window).x << "," << sf::Mouse::getPosition(window).y << std::endl;

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && selectn != 4)
				{
					selectn++; if (bsoundOn)selectpeep.play();
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && selectn != 1)
				{
					selectn--; if (bsoundOn)selectpeep.play();
				}

				if (isselect4 && sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
				{
					window.close();
				}
				if (isselect1 && sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
				{
					GAME = true; mainmenu = false; if(bsoundOn)selectpeep.play();

				}
				if (isselect2 && sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
				{
					soundmenu = true; mainmenu = false; if (bsoundOn)selectpeep.play();

				}
				if (isselect3 && sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
				{
					creditsmenu = true; mainmenu = false; if (bsoundOn)selectpeep.play();

				}
			}

			if (selectn == 1) { select1.setFillColor(sf::Color::Red); isselect1 = true; }
			else { select1.setFillColor(sf::Color::Blue); isselect1 = false; }
			if (selectn == 2) { select2.setFillColor(sf::Color::Red); isselect2 = true; }
			else { select2.setFillColor(sf::Color::Blue); isselect2 = false; }
			if (selectn == 3) { select3.setFillColor(sf::Color::Red); isselect3 = true; }
			else { select3.setFillColor(sf::Color::Blue); isselect3 = false; }
			if (selectn == 4) { select4.setFillColor(sf::Color::Red); isselect4 = true; }
			else { select4.setFillColor(sf::Color::Blue); isselect4 = false; }

			window.clear();
			window.draw(background);
			window.draw(maint);
			window.draw(select1); window.draw(select2); window.draw(select3); window.draw(select4);
			//window.draw(banner1);
			window.display();
		}
		while (GAME) {

			memset(dp, -1, sizeof(dp));
			/** FONT **/
			Font font;
			font.loadFromFile("fonts/KOMIKAP_.ttf");
			Text scoreText, messageText, loseText, winText;
			/** Score Text **/
			scoreText.setFont(font);
			scoreText.setString("Score = 0");
			scoreText.setCharacterSize(30);
			scoreText.setFillColor(Color::White);
			scoreText.setPosition(0, 0);
			/** Message Text **/
			messageText.setFont(font);
			messageText.setString("Press Enter to start!");
			messageText.setCharacterSize(45);
			messageText.setFillColor(Color::White);
			FloatRect textRect = messageText.getLocalBounds();
			messageText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
			messageText.setPosition((M * 32) / 2.0f, (N * 32) / 2.0f);
			/** Lose Text **/
			loseText.setFont(font);
			loseText.setString("You Lose and your Score is : 000");
			loseText.setCharacterSize(35);
			loseText.setFillColor(Color::Red);
			FloatRect loseRect = loseText.getLocalBounds();
			loseText.setOrigin(loseRect.left + loseRect.width / 2.0f, loseRect.top + loseRect.height / 2.0f);
			loseText.setPosition((M * 32) / 2.0f, (N * 32) / 2.0f);
			/** Win Text **/
			winText.setFont(font);
			winText.setString("You Won and your Score is : 000");
			winText.setCharacterSize(35);
			winText.setFillColor(Color::Red);
			FloatRect winRect = winText.getLocalBounds();
			winText.setOrigin(winRect.left + winRect.width / 2.0f, winRect.top + winRect.height / 2.0f);
			winText.setPosition((M * 32) / 2.0f, (N * 32) / 2.0f);
			/** Ghosts **/
			int initialX[4] = { 9 * 32,11 * 32,13 * 32,15 * 32 };
			int initialY[4] = { 8 * 32,8 * 32,8 * 32,8 * 32 };
			Texture textureGhost1, textureGhost2, textureGhost3, textureGhost4, textureScared;
			textureGhost1.loadFromFile("graphics/ghost1.png");
			textureScared.loadFromFile("graphics/scared.png");
			textureGhost2.loadFromFile("graphics/ghost2.png");
			textureGhost3.loadFromFile("graphics/ghost3.png");
			textureGhost4.loadFromFile("graphics/ghost4.png");
			spriteGhost[0].setTexture(textureGhost1);
			spriteGhost[0].setPosition(9 * 32, 8 * 32);
			spriteGhost[1].setTexture(textureGhost2);
			spriteGhost[1].setPosition(11 * 32, 8 * 32);
			spriteGhost[2].setTexture(textureGhost3);
			spriteGhost[2].setPosition(13 * 32, 8 * 32);
			spriteGhost[3].setTexture(textureGhost4);
			spriteGhost[3].setPosition(15 * 32, 8 * 32);
			/** SPRITESHEET **/
			Texture spriteSheet;
			spriteSheet.loadFromFile("graphics/spriteSheetSmall.png");
			Sprite Check;
			Check.setTexture(spriteSheet);
			Check.setTextureRect(IntRect(264, 0, 32, 32));
			Check.setPosition(4 * 32, 4 * 32);
			/** Drawing The Window **/
			////////////////////////////////// 832 , 640
			RenderWindow appWindow(VideoMode(M * 32, N * 32), "Pacman");
			appWindow.setFramerateLimit(60);
			appWindow.setVerticalSyncEnabled(true);
			/** OBJECTS **/
			pacman.posX = 32;
			pacman.posY = 32;
			pacman.sprite.setTexture(spriteSheet);
			pacman.sprite.setTextureRect(IntRect(0, 0, 64, 64));
			pacman.sprite.setPosition(pacman.posX, pacman.posY);
			/** Sound **/
			SoundBuffer BeginBuffer;
			BeginBuffer.loadFromFile("sound/pacman_beginning.wav");
			Sound BeginSound;
			BeginSound.setBuffer(BeginBuffer);
			if (bsoundOn)BeginSound.play();
			SoundBuffer EatBuffer;
			EatBuffer.loadFromFile("sound/pacman_chomp.wav");
			Sound EatSound;
			EatSound.setBuffer(EatBuffer);
			SoundBuffer DieBuffer;
			DieBuffer.loadFromFile("sound/pacman_death.wav");
			Sound DieSound;
			DieSound.setBuffer(DieBuffer);
			/** Main Game Loop **/
			getTheMap(spriteSheet); //Drawing the map with the 2D Vector
			while (appWindow.isOpen()) {
				spriteGhost[0].setTexture(textureGhost1);
				spriteGhost[1].setTexture(textureGhost2);
				spriteGhost[2].setTexture(textureGhost3);
				spriteGhost[3].setTexture(textureGhost4);
				frames++;
				frames2++;
				/** Updating The Scene **/
				Event event;
				while (appWindow.pollEvent(event)) {
					if (event.type == Event::Closed || (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape))appWindow.close();
					else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Return)paused = false;
					if (!paused) {
						if (event.type == Event::KeyPressed && event.key.code == Keyboard::Right)setDir("right");
						else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Left)setDir("left");
						else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Up)setDir("up");
						else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Down)setDir("down");
						else if (event.type == Event::KeyPressed && event.key.code == Keyboard::P)paused = true;
						else if (event.type == Event::KeyPressed && event.key.code == Keyboard::BackSpace)GAME = false , mainmenu = true ;
					}
				}
				if (!paused) {
					/** Moving Pacman **/
					pacman.posX = pacman.sprite.getPosition().x;
					pacman.posY = pacman.sprite.getPosition().y;
					PacmansMovement(4);

					/** Move the 4 Ghosts **/

					int randomSwap = rand() % 4;
					for (int j = 0; j < 1; j++) {
						//if(frames == 16 * 8 ) swap(spriteGhost[randomSwap],spriteGhost[3-randomSwap]),cout<<"Swaped"<<endl,frames = 0;
						int ghostX = spriteGhost[j].getPosition().x / 32;
						int ghostY = spriteGhost[j].getPosition().y / 32;
						int PacX = pacman.posX / 32;
						int PacY = pacman.posY / 32;
						for (int i = 0; i < 20; i++)
							for (int j = 0; j < 26; j++)
								checkMapVec[i][j] = mapVec[i][j];

						checkMapVec[PacY][PacX] = 3;
						checkMapVec[ghostY][ghostX] = 5;
						/*cout<<PacX << " " << PacY << " " << ghostX << " " << ghostY << endl;
						cout<<last<<endl;*/
						mshbfs(checkMapVec);
						if (cntr == 0)ghostDir[j] = last;
						if (j == 0)cntr++;
						if (cntr == 16)cntr = 0;
						for (int i = 0;i < 4;i++) {
							if (ghostDir[j] == dirst[i]) {
								spriteGhost[j].move(dirx[i], diry[i]);
								break;
							}
						}
					}
					for (int j = 1;j < 4;j++) {
						int ghostX = spriteGhost[j].getPosition().x / 32;
						int ghostY = spriteGhost[j].getPosition().y / 32;
						int PacX = pacman.posX / 32;
						int PacY = pacman.posY / 32;
						if (!ghostMoving[j])
							ghostDir[j] = generateGoodDirection(spriteGhost[j]), ghostMoving[j] = true;
						//if(cntr2 == 16 ) cntr2 = 0 ;
						//if(j == 3 ) cntr2++;
						if (ghostX == 24 && ghostY == 9)ghostDir[j] = "left", ghostMoving[j] = true;
						if (ghostX == 1 && ghostY == 9)ghostDir[j] = "right", ghostMoving[j] = true;
						for (int i = 0;i < 4;i++)
						{
							if (ghostDir[j] == dirst[i])
							{
								if (!checkCollision(tilemap, spriteGhost[j].getPosition().x + dirx[i], spriteGhost[j].getPosition().y + diry[i], "ghost"))
								{

									spriteGhost[j].move(dirx[i], diry[i]);
									//if (j == 1) { spriteGhost[j].move(2 * dirx[i], 2 * diry[i]); }
									break;
								}
								else { ghostMoving[j] = false; }
							}
						}
					}
				}

				/** Food **/
				food = false;
				checkFood(pacman.sprite.getPosition().x, pacman.sprite.getPosition().y);
				if (food&&bsoundOn)EatSound.play();

				/** Update The String Streams **/
				stringstream Streams[3]; // Score , Lose , Win
				Streams[0] << "Score = " << score;
				scoreText.setString(Streams[0].str());
				Streams[1] << "You Lose and your Score is : " << score;
				loseText.setString(Streams[1].str());
				Streams[2] << "You Won and your Score is : " << score;
				winText.setString(Streams[2].str());

				/** Check Lose & Win **/
				if (checkLose(spriteGhost, pacman.sprite.getPosition().x, pacman.sprite.getPosition().y)){
					paused = true, lose = true;
					if (bsoundOn)DieSound.play();
			}
					if (dotmap.size() == 0) win = true, paused = true;
				
				/** Drawing The Scene **/
				appWindow.clear(Color::Black);
				for (int i = 0; i<tilemap.size(); i++)appWindow.draw(tilemap[i].sprite);
				for (int i = 0; i<dotmap.size(); i++)appWindow.draw(dotmap[i].sprite);
				appWindow.draw(pacman.sprite);
				for (int i = 0; i < 4; i++)appWindow.draw(spriteGhost[i]);
				appWindow.draw(scoreText);
				if (paused) {
					if (lose){ appWindow.draw(loseText); }
					else if (win) { appWindow.draw(winText); }
					else appWindow.draw(messageText);
				}
				appWindow.display();
				/** Delay **/
				sleep(microseconds(50));
				
			}
			return 0;
		}
		//settings menu
		while (soundmenu)
		{
			window.clear();
			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
				{
					bsoundOn = true;  if (bsoundOn)selectpeep.play();
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
				{
					bsoundOn = false;  if(bsoundOn)selectpeep.play();
				}
			}

			

			window.draw(background);
			window.draw(maint);
			if (bsoundOn) { window.draw(soundOn); }
			else { window.draw(soundOff); }
			window.display();

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			{
				soundmenu = false; mainmenu = true;
			}
		}
		while (creditsmenu)
		{
			if (bsoundOn)creditmusic.play();
			window.clear();
			while (creditsmenu)
			{
				sf::Event event;
				while (window.pollEvent(event))
				{
					if (event.type == sf::Event::Closed)
						window.close();
				}
				credits.move(-0.1f, 0.0f);
				window.draw(cbackground);
				window.draw(credits);
				window.display();

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
				{
					creditsmenu = false; mainmenu = true; creditmusic.stop();
				}
				
			}

		}
		}
	return 0;
	}

