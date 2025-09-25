#include "iGraphics.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 667
enum Direction{ FORWARD, BACKWARD };
Direction d = FORWARD;
int level_3Page = 0;
int hiddenPoints = 0;
bool hiddenWin = false;
bool level3Scroll = false;
int scrollX = 0;
int scrollingSpeed = 1;
int scrollBg;
int distanceCovered = 0;
int hiddenBg, atlasImg, amuletImg, flameImg, hiddenReward, fireSprite, iceSprite;
int fireSpriteX[50];
int fireSpriteY[30];

bool slashPressed = false;

int hp3 = 100; // HP count
int maxHp3 = 100;
bool isGameOver3 = false;
bool isPausedForAmulet = false;
bool amuletActivated = false;


int fireballX[20];
int fireballY[20];
int fireballActive[20];
int fireballCount = 10;  // Increased from 5
float fireballSpeed = 7.0f;  // Adjustable speed
int fireballTimer[20];
int hit = 0;
int miss = 0;
int spawnTimer = 0;
bool fireballGameActive = false;
bool fireWin = false;

//For level lock
bool level1Completed = false;
bool level2Completed = false;

void initFireballs() {
	printf("Initializing fireball game...\n");

	// Reset game state
	hit = 0;
	miss = 0;
	spawnTimer = 0;
	fireballGameActive = true;

	// Initialize all fireballs to inactive first
	for (int i = 0; i < fireballCount; i++) {
		fireballActive[i] = 0;  // Start inactive
		fireballTimer[i] = 0;
	}

	// Activate a few fireballs to start
	for (int i = 0; i < 3; i++) {
		if (i < fireballCount) {
			fireballX[i] = -100 - (i * 200);  // Stagger start positions
			fireballY[i] = 150 + rand() % 400;
			fireballActive[i] = 1;
		}
	}

	printf("Fireball game started! Fireballs active: 3\n");
}

void updateFireballs() {
	if (!fireballGameActive) {
		return;
	}

	spawnTimer++;

	// Debug: Show spawn timer occasionally
	if (spawnTimer % 100 == 0) {
		printf("Spawn timer: %d\n", spawnTimer);
	}

	// 1. Update existing fireballs
	for (int i = 0; i < fireballCount; i++) {
		if (fireballActive[i] == 1) {  // Active fireball
			fireballX[i] += fireballSpeed;

			// Check if fireball went off-screen
			if (fireballX[i] > SCREEN_WIDTH + 100) {
				fireballActive[i] = 0;
				miss++;
				printf("MISS! Total misses: %d/10\n", miss);
			}
		}
		else if (fireballActive[i] == 2) {  // Frozen fireball
			fireballTimer[i]--;
			if (fireballTimer[i] <= 0) {
				fireballActive[i] = 0;
				printf("Fireball %d melted\n", i);
			}
		}
	}

	// 2. Spawn new fireballs more frequently
	if (spawnTimer > 40) {  // Reduced from 80 to 40 for faster spawning
		int spawned = 0;
		for (int i = 0; i < fireballCount; i++) {
			if (fireballActive[i] == 0) {
				fireballX[i] = -100;
				fireballY[i] = 150 + rand() % 400;
				fireballActive[i] = 1;
				spawned++;
				printf("Spawned fireball %d at Y: %d\n", i, fireballY[i]);
				break;
			}
		}

		if (spawned > 0) {
			spawnTimer = 0;
		}
		else {
			printf("No available slots for new fireballs!\n");
		}
	}

	// 3. Win/lose conditions
	if (hit >= 5) {
		fireballGameActive = false;
		fireWin = true;
		iSetColor(0, 0, 0);
		iText(300, 300, "YOU WIN", GLUT_BITMAP_TIMES_ROMAN_24);
		printf("Final score: %d hits, %d misses\n", hit, miss);
	}

	if (miss >= 10) {
		fireballGameActive = false;
		printf("=== FIREBALL GAME LOST! ===\n");
		printf("Final score: %d hits, %d misses\n", hit, miss);
		// Don't set isGameOver = true here
	}
}

void drawFireballs() {
	if (!fireballGameActive) {
		if (fireWin){
			// Show game status
			iSetColor(255, 0, 0);
			iText(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, "FIREBALL GAME COMPLETED", GLUT_BITMAP_HELVETICA_18);
			return;
		}
	}

	// Draw active fireballs
	int activeCount = 0;
	for (int i = 0; i < fireballCount; i++) {
		if (fireballActive[i] == 1) {  // Regular fireball
			iShowImage(fireballX[i], fireballY[i], 60, 70, fireSprite);
			activeCount++;
		}
		else if (fireballActive[i] == 2) {  // Frozen fireball
			iShowImage(fireballX[i], fireballY[i], 60, 70, iceSprite);
			activeCount++;
		}
	}

	// Score display with background for visibility
	char score[50];
	sprintf_s(score, sizeof(score), "Hit: %d/10  Miss: %d/10", hit, miss);

	// Background rectangle
	iSetColor(0, 0, 0);
	iFilledRectangle(30, SCREEN_HEIGHT - 60, 220, 35);

	// Text
	iSetColor(255, 255, 0);  // Yellow text
	iText(40, SCREEN_HEIGHT - 40, score, GLUT_BITMAP_TIMES_ROMAN_24);

	// Debug info
	char debug[100];
	sprintf_s(debug, sizeof(debug), "Active Fireballs: %d", activeCount);
	iSetColor(255, 255, 255);
	iText(40, SCREEN_HEIGHT - 80, debug, GLUT_BITMAP_HELVETICA_12);
}

void handleFireballClick(int mx, int my) {
	if (!fireballGameActive) {
		printf("Click ignored - fireball game not active\n");
		return;
	}

	printf("Mouse click at: %d, %d\n", mx, my);

	for (int i = 0; i < fireballCount; i++) {
		if (fireballActive[i] == 1) {  // Only click active fireballs
			// Check collision with mouse click
			if (mx >= fireballX[i] && mx <= fireballX[i] + 60 &&
				my >= fireballY[i] && my <= fireballY[i] + 70) {

				fireballActive[i] = 2;  // Change to frozen state
				fireballTimer[i] = 45;  // Freeze duration
				hit++;

				printf("HIT! Fireball %d frozen. Total hits: %d/10\n", i, hit);
				return;  // Only process one hit per click
			}
		}
	}

	printf("Click missed all fireballs\n");
}


struct HiddenObject {
	int x, y, w, h;
	int img;
	bool found;
	char name[20];
};

HiddenObject hiddenObjects[3];

// --- Game State Flags ---
int homePage = 1;
int startPage = 0;
int aboutPage = 0;
int instructionPage = 0;
int level_1Page = 0;
int level_2Page = 0;
int rewardImg;
// --- Level 1 (Puzzle Game) Globals ---
#define GRID_ROWS 5
#define GRID_COLS 5
#define TOTAL_ROUNDS 5
#define TOTAL_BOXES 25
int points = 0;
int currentRound = 0;
bool gameOver = false;
bool gameWin = false;
int boxX[GRID_COLS];
int boxY[GRID_ROWS];
int boxWidth = 120; // Increased size for better visibility
int boxHeight = 60;
char gridWords[TOTAL_BOXES][25];
int correctIndex;
char Words[TOTAL_ROUNDS][25] = { "War", "Shadow", "Queen", "Storm", "Labyrinth" };
char Synonyms[TOTAL_ROUNDS][25] = { "Combat", "Silhouette", "Monarch", "Tempest", "Maze" };
char Wrong[20][25] = { "Like", "Run", "Focus", "Sword", "Pain", "Demon", "Hero", "End", "Claw", "Blood", "Wound", "Clandestine", "Vintage", "Ice", "Ivy", "Unmoored", "Ricochet", "Elegies", "Covert", "Willow" };

// --- Menu Functions ---
void drawHomePage();
void drawStartPage();
void drawAboutPage();
void drawInstructionPage();
void startButtonClickHandler();
void aboutButtonClickHandler();
void instructionButtonClickHandler();
void startBackarrowClickHandler();
void aboutBackarrowClickHandler();
void instructionBackarrowClickHandler();
void level_1ButtonClickHandler();

// --- Level 1 (Puzzle) Functions ---
void drawLevel_1();
void resetLevel1();
void fillGrid();
void initializeGridPositions();

// For Level 2 (Globals)
// ----Character globals----
int width = 100;
int height = 100;
int x = 0, y = 101;
bool isJumping = false;
int jumpTime = 0;
int jumpStartY = 0;
int jumpDration = 30;
int jumpHeight = 120;
int sprite;
#define RUN_FRAMES 2
#define JUMP_FRAMES 3
#define TURN_FRAMES 3
#define BACK_TURN_FRAMES 2
#define BACK_RUN_FRAMES 2
#define BACK_JUMP_FRAMES 2
int brunSprite[BACK_RUN_FRAMES];
int bjumpSprite[BACK_JUMP_FRAMES];
int bturnSprite[BACK_TURN_FRAMES];
int runSprite[RUN_FRAMES];
int jumpSprite[JUMP_FRAMES];
int turnSprite[TURN_FRAMES];
int currentFrame = 0;
int jumpFrame = 0;
int runFrame = 0;
int turnFrame = 0;
int brunFrame = 0;
int bJumpFrame = 0;
int bturnFrame = 0;
bool isTurning = false;
bool isRunning = false;

// For riddle at the last of the levels
bool showRiddle = false;
bool swordCollected = false;
bool riddleActive = false;
int box, sword;
const char* options[4] = { "Medusa", "Athena", "Hera", "Aphrodite" };
int optionX[4];
int optionY;
int optionW = 120;
float optionH = 40;

// For level 2 animation
int bgX = 0;
int scrollSpeed = 6;
int backGround1, backGround2;
int monsterCount = 0; // +1 when slime is passed by jump or monster is slashed
int score = 0;

struct Raindrop {
	int x, y;
	int speed;
};

Raindrop rain[300];
int hp = 100;
int maxHp = 100;
bool isGameOver = false;
bool isHit = false;
int hitTimer = 0;
const int HIT_DURATION = 100; // frames to glow
#define MONSTER_WIDTH 150
#define MONSTER_HEIGHT 150

int slashCount = 0;

struct Monster {
	int x, y;
	int frameIndex;
	int frameCounter;
	bool show;
	bool hasDamaged = false;
};

int monsterImages[5];
int slimeImages[5];

Monster monster; // normal monster (requires slash '0')
Monster slime;   // slime (requires jump)

int spawnTurn = 0; // 0 = slime, 1 = monster, 2 = monster, ...

void initRain() {
	for (int i = 0; i < 300; i++) {
		rain[i].x = rand() % SCREEN_WIDTH;
		rain[i].y = rand() % SCREEN_HEIGHT;
		rain[i].speed = 5 + rand() % 10; // random speed
	}
}

void updateRain() {
	for (int i = 0; i < 300; i++) {
		rain[i].y -= rain[i].speed;
		if (rain[i].y < 0) {
			rain[i].y = SCREEN_HEIGHT;
			rain[i].x = rand() % SCREEN_WIDTH;
		}
	}
}

void drawRain() {
	for (int i = 0; i < 300; i++) {
		iSetColor(173, 216, 230); // light blue
		iLine(rain[i].x, rain[i].y, rain[i].x + 5, rain[i].y + 10);
	}
}
//-------------------------for attacking----------------------
int assiMonster1AttackImg; // attack sprite
bool isAttacking = false; // attack state
int attackTimer = 0;      // how long monster stays in attack pose

bool assiMonster1HitPlayer = false; // for reducing score
bool assiMonster1Killed = false;    // track if player killed monster
bool mDamageDone = false;

int monsterCount3 = 0;

struct Enemy {
	int x, y;
	int frameIndex;
	int frameCounter;
	bool show;
};

int assiMonster1Images[7];
Enemy assiMonster1;


int mPosX = -200, mPosY = 75;   // Monster position
int mStartX = -200;              // Monster's starting X position
int mTargetX;
// Where monster attacks (near player)
int mKnockback = 0;       // how many frames of knockback left
int mKnockbackStrength = 10; // how many pixels per frame monster moves back

int mIdleFrames[2];
int mRunFFrames[2];
int mRunBFrames[2];
int mAttackFrames[3];
int mHurtFrames[2];

int mFrameIndex = 0;
int mState = 0;     // 0 = idle, 1 = run forward, 2 = attack, 3 = run backward
int mFrameCounter = 0;

int mWidth = 180;
int mHeight = 180;
// new global (add near other globals)
bool mHurtByPlayer = false;


int monsterHits = 0;           // count how many times monster is hit
bool monsterDefeated = false;  // true when monster vanishes
int score3 = 0;

bool assiMonster1Defeated = false;   //  first monster done?
bool mainMonsterActive = false;      // start main monster after assistant

void comeAround()
{
	if (x > SCREEN_WIDTH)
	{
		x = -width;
	}
	else if (x + width<0)
	{
		x = SCREEN_WIDTH;
	}
	if (y > SCREEN_HEIGHT)
	{
		y = -height;
	}
	else if (y + height<0)
	{
		y = SCREEN_HEIGHT;
	}

}

void resetHiddenObjects() {
	hiddenPoints = 0;
	hiddenWin = false;
	for (int i = 0; i < 3; i++) {
		hiddenObjects[i].found = false;
	}
}
void drawLevel_3() {

	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, hiddenBg);

	iSetColor(255, 255, 0);
	iText(60, SCREEN_HEIGHT - 100, "Find the 3 hidden objects:", GLUT_BITMAP_TIMES_ROMAN_24);
	iText(60, SCREEN_HEIGHT - 125, "If you find the sorcere's amulet, your enemies can't hurt you", GLUT_BITMAP_TIMES_ROMAN_24);

	iSetColor(255, 255, 255);
	iText(70, SCREEN_HEIGHT - 150, "1. Atlas", GLUT_BITMAP_HELVETICA_18);
	iText(70, SCREEN_HEIGHT - 180, "2. Amulet", GLUT_BITMAP_HELVETICA_18);
	iText(70, SCREEN_HEIGHT - 210, "3. Flame", GLUT_BITMAP_HELVETICA_18);

	// Show hidden objects only if found
	for (int i = 0; i < 3; i++) {
		iShowImage(hiddenObjects[i].x, hiddenObjects[i].y, hiddenObjects[i].w, hiddenObjects[i].h, hiddenObjects[i].img);
	}

	// Show score
	char scoreText[50];
	sprintf_s(scoreText, "Points: %d / 3", hiddenPoints);
	iSetColor(0, 0, 0);
	iFilledRectangle(SCREEN_WIDTH - 220, 40, 150, 30);
	iSetColor(57, 255, 20);
	iText(SCREEN_WIDTH - 200, 50, scoreText, GLUT_BITMAP_TIMES_ROMAN_24);

	// Win condition
	if (hiddenWin) {
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, hiddenReward);

	}
}
void handleHiddenObjectClick(int mx, int my) {
	if (hiddenWin) return;

	for (int i = 0; i < 3; i++) {
		if (!hiddenObjects[i].found &&
			mx >= hiddenObjects[i].x && mx <= hiddenObjects[i].x + hiddenObjects[i].w &&
			my >= hiddenObjects[i].y && my <= hiddenObjects[i].y + hiddenObjects[i].h) {

			hiddenObjects[i].found = true;
			hiddenPoints++;

			if (hiddenPoints == 3) {
				hiddenWin = true;
			}

			break;
		}
	}
}

bool rectOverlap(int ax, int ay, int aw, int ah, int bx, int by, int bw, int bh)
{
	return !(ax + aw < bx || ax > bx + bw || ay + ah < by || ay > by + bh);
}

// Here main char, enemies and level 2 bg image load
void loadImages()
{
	const char*runFiles[RUN_FRAMES] = { "images/r1.png", "images/r2.png" };
	const char*jumpFiles[JUMP_FRAMES] = { "images/j1.png", "images/j2.png", "images/j3.png" };
	const char*turnFiles[TURN_FRAMES] = { "images/t1.png", "images/t2.png", "images/t3.png" };
	rewardImg = iLoadImage("images/reward.png");
	const char*brunFiles[BACK_RUN_FRAMES] = {
		"Images/br1.png",
		"Images/br2.png"
	};
	const char*bjumpFiles[BACK_JUMP_FRAMES] = {
		"Images/bj1.png",
		"Images/bj2.png"
	};
	const char*bturnFiles[BACK_TURN_FRAMES] = {
		"Images/t2.png",
		"Images/bs2.png"
	};

	// monster 2
	const char* idleFiles[2] = { "images/m1idle1.png", "images/m1idle2.png" };
	const char* runForwardFiles[2] = { "images/m1runf1.png", "images/m1runf2.png" };
	const char* runBackwardFiles[2] = { "images/m1runb1.png", "images/m1runb2.png" };
	const char* attackFiles[3] = { "images/m1a1.png", "images/m1a2.png", "images/m1a3.png" };
	const char* hurtFiles[1] = { "images/m1hurt.png" };
	char buffer[100];
	for (int i = 0; i < RUN_FRAMES; i++)
	{
		strcpy_s(buffer, runFiles[i]);
		runSprite[i] = iLoadImage(buffer);
	}
	for (int i = 0; i < JUMP_FRAMES; i++)
	{
		strcpy_s(buffer, jumpFiles[i]);
		jumpSprite[i] = iLoadImage(buffer);
	}
	for (int i = 0; i < TURN_FRAMES; i++)
	{
		strcpy_s(buffer, turnFiles[i]);
		turnSprite[i] = iLoadImage(buffer);
	}
	for (int i = 0; i < BACK_RUN_FRAMES; i++)
	{
		strcpy_s(buffer, brunFiles[i]);
		brunSprite[i] = iLoadImage(buffer);

	}
	for (int i = 0; i < BACK_JUMP_FRAMES; i++)
	{
		strcpy_s(buffer, bjumpFiles[i]);
		bjumpSprite[i] = iLoadImage(buffer);

	}
	for (int i = 0; i < BACK_TURN_FRAMES; i++)
	{
		strcpy_s(buffer, bturnFiles[i]);
		bturnSprite[i] = iLoadImage(buffer);
	}

	// For monster 2
	for (int i = 0; i < 2; i++) {
		strcpy_s(buffer, idleFiles[i]);
		mIdleFrames[i] = iLoadImage(buffer);
	}
	for (int i = 0; i < 2; i++) {
		strcpy_s(buffer, runForwardFiles[i]);
		mRunFFrames[i] = iLoadImage(buffer);
	}
	for (int i = 0; i < 2; i++) {
		strcpy_s(buffer, runBackwardFiles[i]);
		mRunBFrames[i] = iLoadImage(buffer);
	}
	for (int i = 0; i < 3; i++) {
		strcpy_s(buffer, attackFiles[i]);
		mAttackFrames[i] = iLoadImage(buffer);
	}
	for (int i = 0; i < 1; i++) {
		strcpy_s(buffer, hurtFiles[i]);
		mHurtFrames[i] = iLoadImage(buffer);
	}

	box = iLoadImage("images/box.png");
	sword = iLoadImage("images/sword.png");
	backGround1 = iLoadImage("images/level2_bg1.png");
	backGround2 = iLoadImage("images/level2_bg2.png");
	// Code 2 assets
	char* monsterFiles[5] = {
		(char*)"images/run1.png",
		(char*)"images/run2.png",
		(char*)"images/run3.png",
		(char*)"images/run4.png",
		(char*)"images/run5.png"
	};
	for (int i = 0; i < 5; i++) {
		monsterImages[i] = iLoadImage(monsterFiles[i]);
	}
	char* slimeFiles[5] = {
		(char*)"images/round1.png",
		(char*)"images/round2.png",
		(char*)"images/round3.png",
		(char*)"images/round4.png",
		(char*)"images/round5.png"
	};
	for (int i = 0; i < 5; i++){
		slimeImages[i] = iLoadImage(slimeFiles[i]);
	}
	char* assiMonster1Files[7] = {
		(char*)"images/level3_monster_run1.png",
		(char*)"images/level3_monster_run2.png",
		(char*)"images/level3_monster_run3.png",
		(char*)"images/level3_monster_run4.png",
		(char*)"images/level3_monster_run5.png",
		(char*)"images/level3_monster_run6.png",
		(char*)"images/level3_monster_run7.png"
	};
	for (int i = 0; i < 7; i++) assiMonster1Images[i] = iLoadImage(assiMonster1Files[i]);

	assiMonster1AttackImg = iLoadImage("images/level3_monster_attack.png");
	hiddenBg = iLoadImage("images/hidden_bg.png");
	atlasImg = iLoadImage("images/atlas.png");
	amuletImg = iLoadImage("images/amulet.png");
	flameImg = iLoadImage("images/flame.png");
	hiddenReward = iLoadImage("images/win.png");


	hiddenObjects[0] = { 200, 300, 50, 50, atlasImg, false, "Atlas" };
	hiddenObjects[1] = { 500, 200, 50, 50, amuletImg, false, "Amulet" };
	hiddenObjects[2] = { 750, 400, 50, 50, flameImg, false, "Flame" };
	scrollBg = iLoadImage("images/newbg.png");
	iceSprite = iLoadImage("images/ice.png");
	fireSprite = iLoadImage("images/fire.png");
}

// ===================== Enemies init & update ======================
void initializeMonster() {
	monster.x = SCREEN_WIDTH;
	monster.y = 100;
	monster.frameIndex = 0;
	monster.frameCounter = 0;
	monster.show = false;
}
void initializeSlime() {
	slime.x = SCREEN_WIDTH;
	slime.y = 100;
	slime.frameIndex = 0;
	slime.frameCounter = 0;
	slime.show = false;
}

void updateMonster() {
	if (showRiddle) return;
	if (monster.show) {
		monster.x -= 6; // faster
		monster.frameCounter++;
		if (monster.frameCounter >= 5) {
			monster.frameIndex = (monster.frameIndex + 1) % 5;
			monster.frameCounter = 0;
		}
		if (monster.x < -MONSTER_WIDTH) monster.show = false;
	}
}

void updateSlime() {
	if (showRiddle) return;
	if (slime.show) {
		slime.x -= 3; // slower
		slime.frameCounter++;
		if (slime.frameCounter >= 6) {
			slime.frameIndex = (slime.frameIndex + 1) % 5;
			slime.frameCounter = 0;
		}
		if (slime.x < -MONSTER_WIDTH) slime.show = false;
	}
}

void spawnMonster() {
	if (showRiddle) return;
	if (!slime.show && !monster.show) {
		if (spawnTurn % 3 == 0) {
			slime.show = true;
			slime.x = SCREEN_WIDTH;
			slime.y = 100;
			slime.frameIndex = 0;
			slime.frameCounter = 0;
		}
		else {
			monster.show = true;
			monster.x = SCREEN_WIDTH;
			monster.y = 100;
			monster.frameIndex = 0;
			monster.frameCounter = 0;
		}
		spawnTurn++;
	}
}
// ===================== Enemies init & update ====================== This evaluation
void initializeAssiMonster1() {
	assiMonster1.x = SCREEN_WIDTH;
	assiMonster1.y = 100;
	assiMonster1.frameIndex = 0;
	assiMonster1.frameCounter = 0;
	assiMonster1.show = false;
}

void updateAssiMonster1() {
	if (isPausedForAmulet || isGameOver3) return;

	if (monsterCount3 < 5){
		if (assiMonster1.show) {
			assiMonster1.x -= 6;

			if (!isAttacking && assiMonster1.x > x && abs(assiMonster1.x - x) < 100) {
				isAttacking = true;
				attackTimer = 20;
			}

			if (assiMonster1.x <= x) {
				isAttacking = false;
			}

			if (isAttacking) {
				attackTimer--;
				if (attackTimer <= 0) isAttacking = false;
			}
			else {
				assiMonster1.frameCounter++;
				if (assiMonster1.frameCounter >= 5) {
					assiMonster1.frameIndex = (assiMonster1.frameIndex + 1) % 5;
					assiMonster1.frameCounter = 0;
				}
			}

			if (assiMonster1.x < -MONSTER_WIDTH) {
				if (!assiMonster1Killed) { hp3 -= 20; if (hp3 < 0) hp3 = 0; }
				assiMonster1.show = false;
				assiMonster1HitPlayer = false;
				assiMonster1Killed = false;
			}
		}
	}
	else{
		assiMonster1.show = false; // hide assistant monsters
	}

}

void drawLevel_3HPBar() {
	if (isGameOver3) return;

	int barWidth = (hp3 * 200) / maxHp3;

	if (hp3 > 60) iSetColor(0, 200, 0);
	else if (hp3 > 30) iSetColor(255, 200, 0);
	else iSetColor(200, 0, 0);

	iFilledRectangle(20, 570, barWidth, 20);
	iSetColor(255, 255, 255);
	iRectangle(20, 570, 200, 20);

	char hpText[50];
	sprintf_s(hpText, "HP: %d / %d", hp3, maxHp3);
	iText(20, 600, hpText, GLUT_BITMAP_HELVETICA_18);
}

void spawnEnemy() {
	if (isPausedForAmulet || isGameOver3) return;

	if (!assiMonster1.show) {
		assiMonster1.show = true;
		assiMonster1.x = SCREEN_WIDTH;
		assiMonster1.y = 100;
		assiMonster1.frameIndex = 0;
		assiMonster1.frameCounter = 0;
		assiMonster1HitPlayer = false;//reset if it has hit player
		assiMonster1Killed = false;//reset killed state
	}
}

// ================= Monster AI (Modify the collision part) =================
void updateMonsterAI(int playerX) {
	if (!mainMonsterActive) return;  // only update if it's time
	if (monsterDefeated) return; // stop updating monster if defeated

	mFrameCounter++;
	// Trigger monster when player goes right side
	if (playerX >= 600 && mState == 0) {
		mPosX = mStartX;
		mTargetX = playerX - 50;  // stop near player
		mState = 1;
		mFrameIndex = 0;
	}

	if (mState == 1) { // run forward
		if (mFrameCounter >= 10) {
			mFrameIndex = (mFrameIndex + 1) % 2;
			mFrameCounter = 0;
		}
		mPosX += 6;

		// Player bounding box
		int px1 = playerX;
		int px2 = playerX + width;
		int py1 = y;
		int py2 = y + height;

		// Monster bounding box
		int mx1 = mPosX;
		int mx2 = mPosX + mWidth;
		int my1 = mPosY;
		int my2 = mPosY + mHeight;

		if ((mx2 >= px1 && mx1 <= px2) && (my2 >= py1 && my1 <= py2)) {
			mState = 2;
			mFrameIndex = 0;
			if (!isTurning &&!mDamageDone){
				hp3 -= 20;
				if (hp3 < 0) hp3 = 0;
				if (hp3 == 0) {
					isGameOver3 = true;
				}
				mDamageDone = true;
			}
		}
		else{
			mDamageDone = false;
		}
	}
	else if (mState == 2) { // attack
		if (mFrameCounter >= 15) {
			mFrameIndex++;
			mFrameCounter = 0;
		}

		if (mFrameIndex >= 3) {
			mFrameIndex = 0;
			mState = 3;      // retreat
		}
	}
	else if (mState == 3) { // run backward
		if (mFrameCounter >= 10) {
			mFrameIndex = (mFrameIndex + 1) % 2;
			mFrameCounter = 0;
		}
		mPosX -= 6;

		if (mPosX <= mStartX) {
			mState = 0; // back idle
			mFrameIndex = 0;
		}
	}
	else if (mState == 4) { // hurt
		if (mKnockback > 0) {
			mPosX -= mKnockbackStrength;
			if (mPosX < 0) mPosX = 0;
			mKnockback--;
		}

		if (mFrameCounter >= 20) {
			mFrameIndex = (mFrameIndex + 1) % 2;
			mFrameCounter = 0;
		}
		static int hurtTimer = 0;
		hurtTimer++;
		if (hurtTimer > 40) {
			mState = 0;
			mFrameIndex = 0;
			hurtTimer = 0;
			mHurtByPlayer = false;
		}
	}

	// ================= Player Attack Collision =================
	if (isTurning && rectOverlap(x, y, width, height, mPosX, mPosY, mWidth, mHeight)) {
		mState = 4;
		mFrameIndex = 0;
		mHurtByPlayer = true;
		mKnockback = 10;

		monsterHits++;   // increase hit count
		if (monsterHits >= 5) {
			monsterDefeated = true;   // monster vanishes
			score3 = 100;
		}
	}
	// ================= Respawn if Monster Leaves Screen =================
	if (mPosX > SCREEN_WIDTH) {
		mPosX = mStartX;    // reset to starting X
		mPosY = 75;         // reset Y
		mState = 0;         // back to idle
		mFrameIndex = 0;
		mHurtByPlayer = false;
		mKnockback = 0;
	}
}

void drawMonster() {
	if (monsterDefeated)
	{
		return;
	}

	if (mState == 0) {
		iShowImage(mPosX, mPosY, mWidth, mHeight, mIdleFrames[mFrameIndex]);
	}
	else if (mState == 1) {
		iShowImage(mPosX, mPosY, mWidth, mHeight, mRunFFrames[mFrameIndex]);
	}
	else if (mState == 2) {
		iShowImage(mPosX, mPosY, mWidth + 40, mHeight + 40, mAttackFrames[mFrameIndex]);
	}
	else if (mState == 3) {
		iShowImage(mPosX, mPosY, mWidth, mHeight, mRunBFrames[mFrameIndex]);
	}
	else if (mState == 4) {
		if (mHurtByPlayer && isTurning) {
			iShowImage(mPosX, mPosY, mWidth, mHeight, mHurtFrames[0]);
		}
		else {
			iShowImage(mPosX, mPosY, mWidth, mHeight, mIdleFrames[0]);
		}
	}
}


void drawLevel_2(){  //In fixedUpdate() there is scrolling logic
	// Draw two images side by side for looping effect
	iShowImage(bgX, 0, SCREEN_WIDTH, SCREEN_HEIGHT, backGround1);
	iShowImage(bgX + SCREEN_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT, backGround2);

	if (bgX <= -SCREEN_WIDTH)
		bgX = 0;
}

// ===================== HP Bar =====================
void drawHPBar()
{
	if (isGameOver) return; // don't draw bar if game over

	int barWidth = (hp * 200) / maxHp;

	// Change color depending on hp %
	if (hp > 60)
		iSetColor(0, 200, 0);   // Green
	else if (hp > 30)
		iSetColor(255, 200, 0); // Yellow
	else
		iSetColor(200, 0, 0);   // Red

	iFilledRectangle(20, 570, barWidth, 20);

	// Outline
	iSetColor(255, 255, 255);
	iRectangle(20, 570, 200, 20);

	// Text
	char hpText[50];
	sprintf_s(hpText, "HP: %d / %d", hp, maxHp);
	iText(20, 600, hpText, GLUT_BITMAP_HELVETICA_18);
}

// ===================== Draw Game Over Screen =====================
void drawGameOver()
{
	iSetColor(10, 10, 60); // Navy blue background
	iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	iSetColor(255, 0, 0);
	iText(450, 350, "GAME OVER!!", GLUT_BITMAP_TIMES_ROMAN_24);

	iSetColor(255, 255, 255);
	iText(450, 300, "Press R to Restart", GLUT_BITMAP_HELVETICA_18);
}

void iDraw()
{
	iClear();
	if (homePage == 1) {
		drawHomePage();
	}
	else if (startPage == 1) {
		drawStartPage();
	}
	else if (aboutPage == 1) {
		drawAboutPage();
	}
	else if (instructionPage == 1) {
		drawInstructionPage();
	}
	else if (level_1Page == 1) {
		drawLevel_1();
	}
	else if (level_2Page == 1){
		if (gameWin) {
			iSetColor(0, 255, 0);
			iText(SCREEN_WIDTH / 2 - 30, SCREEN_HEIGHT / 2, "You Win!", GLUT_BITMAP_TIMES_ROMAN_24);
		}

		if (isGameOver) {
			drawGameOver();
			return; // stop drawing anything else
		}

		drawLevel_2();
		drawRain();
		if (!showRiddle){
			if (isJumping)
			{
				iShowImage(x, y, 100, 100, jumpSprite[jumpFrame]);
			}
			else if (isTurning)
			{
				iShowImage(x, y, 100, 100, turnSprite[turnFrame]);
			}
			else if (isRunning)
			{
				iShowImage(x, y, 100, 100, runSprite[currentFrame]);
			}
			else
			{
				iShowImage(x, y, width, height, runSprite[0]);
			}
			if (isHit) {
				// Example: draw player slightly brighter or with a red glow
				iSetColor(255, 0, 0); // red glow overlay
				iFilledRectangle(x - 5, y - 5, width + 10, height + 10);
				// then draw player sprite on top
				iShowImage(x, y, width, height, runSprite[0]);

				hitTimer--;
				if (hitTimer <= 0) {
					isHit = false; // stop glowing after timer ends
				}
			}
			// Enemies
			if (slime.show) {
				iShowImage(slime.x, slime.y, MONSTER_WIDTH, MONSTER_HEIGHT, slimeImages[slime.frameIndex]);
			}
			if (monster.show) {
				iShowImage(monster.x, monster.y, MONSTER_WIDTH, MONSTER_HEIGHT, monsterImages[monster.frameIndex]);
			}

			// Score HUD
			char scoreText[64];
			sprintf_s(scoreText, "Monster Count: %d", monsterCount);
			iSetColor(255, 255, 255);
			iText(60, SCREEN_HEIGHT - 30, scoreText, GLUT_BITMAP_HELVETICA_18);

			// Show score on top-right
			char totalScoreText[64];
			sprintf_s(totalScoreText, "Score: %d", score);
			iText(SCREEN_WIDTH - 200, SCREEN_HEIGHT - 30, totalScoreText, GLUT_BITMAP_HELVETICA_18);

			// Call HP Bar here
			drawHPBar();
		}
		else {
			// Trivia screen — unchanged from Code 1 + box visible
			iSetColor(255, 255, 255);
			iText(20, SCREEN_HEIGHT - 50, "A maiden of beauty rare, none can dare to meet her glare.", GLUT_BITMAP_HELVETICA_18);
			iText(20, SCREEN_HEIGHT - 80, "For her eyes turn all to stone, dare to name this Gorgon.", GLUT_BITMAP_HELVETICA_18);

			// Final score shown at trivia page
			char finalScoreText[64];
			sprintf_s(finalScoreText, "Your Total Score: %d", score);
			iSetColor(255, 255, 0);
			iText(20, 60, finalScoreText, GLUT_BITMAP_HELVETICA_18);

			for (int i = 0; i < 4; i++) {
				iSetColor(200, 200, 200);
				iFilledRectangle(optionX[i], optionY, optionW, optionH);
				iSetColor(0, 0, 0);
				iText(optionX[i] + 10, optionY + 15, (char*)options[i], GLUT_BITMAP_HELVETICA_18);
			}

			if (swordCollected) {
				iSetColor(173, 216, 230);
				iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, rewardImg);
			}
			if (!swordCollected) {
				// <<< You asked that the box still shows after trivia — kept as in Code 1
				iShowImage(SCREEN_WIDTH / 10 - 50, 100, 100, 100, box);
			}
		}
	}
	else if (level_3Page == 1)
	{
		if (gameWin) {
			iSetColor(0, 255, 0);
			iText(SCREEN_WIDTH / 2 - 30, SCREEN_HEIGHT / 2, "You Win!", GLUT_BITMAP_TIMES_ROMAN_24);
		}

		if (isGameOver3) {
			drawGameOver();
			return; // stop drawing anything else
		}
		if (level3Scroll)
		{
			if (isPausedForAmulet) {
				iSetColor(0, 0, 0); iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
				iSetColor(255, 255, 255);
				iText(SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT / 2, "Press key 'k' if you want to activate your amulet.", GLUT_BITMAP_HELVETICA_18);
				return;
			}
			iShowImage(scrollX, 0, SCREEN_WIDTH, SCREEN_HEIGHT, scrollBg);
			iShowImage(scrollX + SCREEN_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT, scrollBg);

			if (isJumping)
			{
				if (d == FORWARD) iShowImage(x, y, 100, 100, jumpSprite[jumpFrame]);
				else iShowImage(x, y, 100, 100, bjumpSprite[jumpFrame]);
			}
			else if (isTurning)
			{
				if (d == FORWARD) iShowImage(x, y, 100, 100, turnSprite[turnFrame]);
				else iShowImage(x, y, 100, 100, bturnSprite[turnFrame]);
			}
			else if (isRunning)
			{

				if (d == FORWARD) iShowImage(x, y, 100, 100, runSprite[currentFrame]);
				else iShowImage(x, y, 100, 100, brunSprite[currentFrame]);

			}
			else
			{
				if (d == FORWARD) iShowImage(x, y, width, height, runSprite[0]);
				else iShowImage(x, y, width, height, brunSprite[0]);
			}
			// Monster
			if (assiMonster1.show) {
				if (isAttacking) iShowImage(assiMonster1.x, assiMonster1.y, MONSTER_WIDTH, MONSTER_HEIGHT, assiMonster1AttackImg);
				else iShowImage(assiMonster1.x, assiMonster1.y, MONSTER_WIDTH, MONSTER_HEIGHT, assiMonster1Images[assiMonster1.frameIndex]);
			}
			if (!monsterDefeated)
				drawMonster();
			if (monsterDefeated)
			{
				if (!fireballGameActive && !fireWin)
				{
					initFireballs();
					fireballGameActive = true;
				}
				drawFireballs();
			}

			char scoreText[50];
			sprintf_s(scoreText, "Score: %d", score3);
			iText(800, SCREEN_HEIGHT - 50, scoreText, GLUT_BITMAP_HELVETICA_18);
			char countText[64];
			sprintf_s(countText, "Monster Count: %d", monsterCount3);
			iSetColor(255, 255, 255);
			iText(60, SCREEN_HEIGHT - 30, countText, GLUT_BITMAP_HELVETICA_18);

			drawLevel_3HPBar();
		}
		else
		{
			if (!hiddenWin)
			{
				drawLevel_3();
			}
		}
		if (gameOver)
		{
			iSetColor(0, 0, 0);
			iText(300, 250, "YOU LOSE", GLUT_BITMAP_HELVETICA_18);
		}
	}
}

void resetLevel2() {
	score = 0;
	hp = maxHp;   // whatever your max HP is
	monsterCount = 0;

	// Reset player position
	x = 0;   // starting x position for Level 2
	y = 101;    // starting y position for Level 2

	// Reset movement flags
	isJumping = false;
	isRunning = false;
	isTurning = false;
	monster.show = true;
	slime.show = true;
	isPausedForAmulet = false;
	showRiddle = 0;
	// add more resets if Level 2 uses special variables
}

void resetLevel3() {
	score3 = 0;
	hp3 = maxHp3;
	monsterCount3 = 0;
	resetHiddenObjects();
	isPausedForAmulet = false;
	
}



void iMouseMove(int mx, int my) {}
void iPassiveMouseMove(int mx, int my) {}

void iMouse(int button, int state, int mx, int my)
{
	// --- Level Progression Logic ---
	if (level_1Page == 1 && gameWin) {           // Attoja's change
		level1Completed = true;
		// lock all other levels first
		level_1Page = 0;
		resetLevel2();    //  reset Level 2 state
		level_2Page = 1;  // start Level 2
		level_3Page = 0;

		gameWin = false; // reset for next level
	}

	if (level_2Page == 1 && gameWin) {
		level2Completed = true;
		level_1Page = 0;
		level_2Page = 0;
		resetLevel3();    // reset Level 3 state
		level_3Page = 1;  // start Level 3

		gameWin = false; // reset for next level
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		printf("x = %d  y = %d\n", mx, my);
		// --- Home Page Clicks ---
		if (homePage == 1) {
			if ((mx >= 420 && mx <= 550) && (my >= 285 && my <= 350)) { // Start button
				startButtonClickHandler();
			}
			else if ((mx >= 411 && mx <= 556) && (my >= 224 && my <= 262)) { // Instructions button
				instructionButtonClickHandler();
			}
			else if ((mx >= 411 && mx <= 554) && (my >= 154 && my <= 197)) { // About us button
				aboutButtonClickHandler();
			}
		}
		// --- Start Page Clicks ---
		else if (startPage == 1) {
			if ((mx >= 10 && mx <= 80) && (my >= 550 && my <= 600)) { // Back arrow
				startBackarrowClickHandler();
			}
			else if ((mx >= 410 && mx <= 560) && (my >= 300 && my <= 350)) { // Level 1 button
				level_1ButtonClickHandler();
			}
			else if ((mx >= 410 && mx <= 560) && (my >= 230 && my <= 280)) { // Level 2 button
				if (level1Completed) { // Only allow if Level 1 finished  (Atttoja's change)
					startPage = 0;
					level_2Page = 1;
					showRiddle = 0;
				}
				else {
					printf("You must complete Level 1 first!\n");
				}
			}
			else if ((mx >= 410 && mx <= 560) && (my >= 150 && my <= 200)) { // Level 3 button
				if (level2Completed) { // Only allow if Level 2 finished
					startPage = 0;
					level_3Page = 1;
					resetHiddenObjects();
				}
				else {
					printf("You must complete Level 2 first!\n");
				}
			}
		}
		// --- Back Arrow Clicks for Sub-pages ---
		else if (aboutPage == 1 && (mx >= 10 && mx <= 80) && (my >= 40 && my <= 120)) {
			aboutBackarrowClickHandler();
		}
		else if (instructionPage == 1 && (mx >= 10 && mx <= 80) && (my >= 550 && my <= 600)) {
			instructionBackarrowClickHandler();
		}
		// --- Level 1 Puzzle Clicks ---
		else if (level_1Page == 1 && !gameOver && !gameWin) {
			int index = 0;
			for (int i = 0; i < GRID_ROWS; i++) {
				for (int j = 0; j < GRID_COLS; j++) {
					if (mx >= boxX[j] && mx <= boxX[j] + boxWidth && my >= boxY[i] && my <= boxY[i] + boxHeight) {
						if (index == correctIndex) {
							points += 10;
							currentRound++;
							if (currentRound >= TOTAL_ROUNDS) {
								gameWin = true;
							}
							else {
								fillGrid();
							}
						}
						else {
							gameOver = true;
						}
						return;
					}
					index++;
				}
			}
		}
		else if (level_2Page == 1 && showRiddle){
			for (int i = 0; i < 4; i++)
			{
				if (mx >= optionX[i] && mx <= optionX[i] + optionW &&
					my >= optionY && my <= optionY + optionH) {
					if (i == 0){
						swordCollected = true; // Medusa is correct
						gameWin = true;
					}
					else
					{
						swordCollected = false;
					}
				}
			}
		}
	}
	else if (level_3Page == 1) {
		handleHiddenObjectClick(mx, my);
		if (hiddenWin)
		{
			level3Scroll = true;
			scrollX = 0;
		}
		if (monsterDefeated)
		{
			handleFireballClick(mx, my);
			gameWin = true;
		}
	}
}


void drawHomePage() {
	iShowBMP(0, 0, "images\\main_menu.bmp");
	iShowBMP2(410, 280, "images\\button1.bmp", 0);
	iShowBMP2(410, 220, "images\\button2.bmp", 0);
	iShowBMP2(410, 150, "images\\button3.bmp", 0);

	iSetColor(255, 218, 120);
	iText(460, 303, "Start", GLUT_BITMAP_TIMES_ROMAN_24);
	iSetColor(110, 49, 41);
	iText(425, 235, "Instructions", GLUT_BITMAP_TIMES_ROMAN_24);
	iText(435, 165, "About Us", GLUT_BITMAP_TIMES_ROMAN_24);
}

void drawStartPage() {
	iShowBMP(0, 0, "images\\background.bmp");
	iShowBMP2(10, 550, "images\\backarrow.bmp", 0);
	iShowBMP2(410, 300, "images\\button2.bmp", 0);
	iShowBMP2(410, 230, "images\\button3.bmp", 0);
	iShowBMP2(410, 150, "images\\button4.bmp", 0);

	iSetColor(110, 49, 41);
	iText(450, 313, "Level 1", GLUT_BITMAP_TIMES_ROMAN_24);
	iText(450, 245, "Level 2", GLUT_BITMAP_TIMES_ROMAN_24);
	iText(450, 175, "Level 3", GLUT_BITMAP_TIMES_ROMAN_24);
}

void drawAboutPage() {
	iShowBMP(0, 0, "images\\aboutUs.bmp");
	iShowBMP2(10, 50, "images\\backarrow.bmp", 0);
}

void drawInstructionPage() {
	iShowBMP(0, 0, "images\\instruction.bmp");
	iShowBMP2(10, 550, "images\\backarrow.bmp", 0);
}

void startButtonClickHandler() {
	homePage = 0;
	startPage = 1;
}

void aboutButtonClickHandler() {
	homePage = 0;
	aboutPage = 1;
}

void instructionButtonClickHandler() {
	homePage = 0;
	instructionPage = 1;
}

void startBackarrowClickHandler() {
	homePage = 1;
	startPage = 0;
}

void aboutBackarrowClickHandler() {
	homePage = 1;
	aboutPage = 0;
}

void instructionBackarrowClickHandler() {
	homePage = 1;
	instructionPage = 0;
}

// ===================================================================
// Level 1 (Puzzle Game) Functions
// ===================================================================

void level_1ButtonClickHandler() {
	homePage = 0;
	startPage = 0;
	level_1Page = 1;
	resetLevel1(); // Reset the game every time you enter
}

void resetLevel1() {
	points = 0;
	currentRound = 0;
	gameOver = false;
	gameWin = false;
	fillGrid();
}

void drawLevel_1() {
	iSetColor(20, 20, 80); // Dark blue background
	iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	if (gameOver) {
		iSetColor(255, 0, 0);
		iText(SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2, "Game Over!", GLUT_BITMAP_TIMES_ROMAN_24);
		return;
	}
	if (gameWin) {
		iSetColor(0, 255, 0);
		iText(SCREEN_WIDTH / 2 - 30, SCREEN_HEIGHT / 2, "You Win!", GLUT_BITMAP_TIMES_ROMAN_24);
		return;
	}

	// Display the word to find
	iSetColor(255, 255, 0);
	char instructionText[100];
	sprintf_s(instructionText, "Find the synonym for: %s", Words[currentRound]);
	iText(SCREEN_WIDTH / 2 - 150, 600, instructionText, GLUT_BITMAP_TIMES_ROMAN_24);

	// Draw the grid of words
	int index = 0;
	for (int i = 0; i < GRID_ROWS; i++) {
		for (int j = 0; j < GRID_COLS; j++) {
			iSetColor(128, 0, 128);
			iFilledRectangle(boxX[j], boxY[i], boxWidth, boxHeight);
			iSetColor(255, 255, 255);
			iText(boxX[j] + 20, boxY[i] + 25, gridWords[index], GLUT_BITMAP_HELVETICA_18);
			index++;
		}
	}

	// Display score
	char score[20];
	sprintf_s(score, sizeof(score), "Points: %d", points);
	iSetColor(0, 255, 255);
	iText(SCREEN_WIDTH - 150, 620, score, GLUT_BITMAP_TIMES_ROMAN_24);
}

void fillGrid() {
	for (int i = 0; i < TOTAL_BOXES; i++) {
		int wrongIndex = rand() % 20;
		strcpy_s(gridWords[i], sizeof(gridWords[i]), Wrong[wrongIndex]);
	}
	correctIndex = rand() % TOTAL_BOXES;
	strcpy_s(gridWords[correctIndex], sizeof(gridWords[correctIndex]), Synonyms[currentRound]);
}

void initializeGridPositions() {
	int totalGridWidth = GRID_COLS * boxWidth + (GRID_COLS - 1) * 10;
	int startX = (SCREEN_WIDTH - totalGridWidth) / 2;
	int startY = 100;
	int spacingX = 10;
	int spacingY = 10;

	for (int i = 0; i < GRID_COLS; i++) {
		boxX[i] = startX + i * (boxWidth + spacingX);
	}
	for (int i = 0; i < GRID_ROWS; i++) {
		boxY[i] = startY + i * (boxHeight + spacingY);
	}
}

void fixedUpdate()
{
	if (!showRiddle || level3Scroll)
	{
		if (level_3Page == 1 && isPausedForAmulet) {
			if (isKeyPressed('k')) { hp3 = maxHp3; amuletActivated = true; isPausedForAmulet = false; }
			else if (isKeyPressed('a') || isKeyPressed('s') || isKeyPressed('d') || isKeyPressed('w') || isKeyPressed(' ') || isKeyPressed('0')) {
				hp3 = 20;
				isPausedForAmulet = false;
			}
			return;
		}
		isRunning = false;
		if (isKeyPressed('w') || isSpecialKeyPressed(GLUT_KEY_UP))
		{
			y += 5;
			isRunning = true;
		}
		if (isKeyPressed('a') || isSpecialKeyPressed(GLUT_KEY_LEFT))
		{
			x -= 5;
			bgX += scrollSpeed;  // For level 2 background moving (LEFT)
			isRunning = true;
			distanceCovered -= 2;
			if (distanceCovered < 0) distanceCovered = 0;
			isRunning = true;
			d = BACKWARD;
		}
		if (isKeyPressed('s') || isSpecialKeyPressed(GLUT_KEY_DOWN))
		{
			y -= 5;
			isRunning = true;
		}
		if (isKeyPressed('d') || isSpecialKeyPressed(GLUT_KEY_RIGHT))
		{
			x += 5;
			bgX -= scrollSpeed;  // For level 2 background moving (RIGHT)
			isRunning = true;
			distanceCovered += 2;
			d = FORWARD;
		}
		if (isKeyPressed(' ') && !isJumping) {
			isJumping = true;
			jumpTime = 0;
			jumpStartY = y;
			jumpFrame = 0;
		}
		if (isJumping)
		{
			float t = (float)jumpTime / jumpDration;
			y = jumpStartY + int(-4 * jumpHeight*(t - 0.5f)*(t - 0.5f) + jumpHeight);
			jumpTime++;
			if (jumpTime % 10 == 0 && jumpFrame < JUMP_FRAMES - 1)
			{
				jumpFrame++;
			}
			if (jumpTime >= jumpDration)
			{
				isJumping = false;
				y = jumpStartY;
			}
		}
		if (isKeyPressed('0')) {
			if (!slashPressed) {
				slashCount++;
				slashPressed = true;
				isTurning = true;
				turnFrame = 0;   // reset animation to start
			}
		}
		else {
			isTurning = false;
			slashPressed = false;
		}

		

		// Collision
		if (assiMonster1.show && rectOverlap(x, y, width, height, assiMonster1.x, assiMonster1.y, MONSTER_WIDTH, MONSTER_HEIGHT)) {
			if (isAttacking && isTurning && slashCount >= 2) {
				assiMonster1.show = false;
				monsterCount3++;
				//score3 += 50;
				slashCount = 0;
				assiMonster1Killed = true;
				//  Trigger main monster when count reaches 
				if (monsterCount3 == 5) {
					mainMonsterActive = true;
				}
			}
			else {
				if (!assiMonster1HitPlayer) {
					assiMonster1HitPlayer = true;
					slashCount = 0;
				}
			}

		}
		updateMonsterAI(x);
		if (hp3 <= 0) isGameOver3 = true;
		if (hp3 == 20 && !amuletActivated) isPausedForAmulet = true;

		// If the HP bar drops to zero then to restart 
		if (isKeyPressed('r') || isKeyPressed('R')){        // Attoja'c change 
			if (isGameOver) {  // for level 2
				// Restart game
				hp = maxHp;
				isGameOver = false;
				monsterCount = 0;

				// reset monsters if needed
				slime.show = false;
				monster.show = false;
			}
			else if (isGameOver3){  //  for level 3
				hp3 = maxHp3;
				isGameOver3 = false;
			}
		}
		// ===================== Interaction Rules =====================
		// (existing jump/slash rules stay the same)

		if (!isGameOver) {
			if (slime.show && isJumping) {
				if (rectOverlap(x, y, width, height, slime.x, slime.y, MONSTER_WIDTH, MONSTER_HEIGHT)) {
					slime.show = false;
					monsterCount++;
					score += 50;
				}
			}

			if (monster.show && isTurning) {
				if (rectOverlap(x, y, width, height, monster.x, monster.y, MONSTER_WIDTH, MONSTER_HEIGHT)) {
					monster.show = false;
					monsterCount++;
					score += 50;
				}
			}
		}

		// ===================== Monster hits player =====================
		if (slime.show) {
			if (rectOverlap(x, y, width, height, slime.x, slime.y, MONSTER_WIDTH, MONSTER_HEIGHT)) {
				if (!slime.hasDamaged) {   // only damage once per pass
					hp -= 10;
					if (hp < 0) hp = 0;
					// check for game over
					if (hp == 0) {
						isGameOver = true;
					}
					slime.hasDamaged = true;  // mark that this slime already did damage

					// trigger glowing effect
					isHit = true;
					hitTimer = HIT_DURATION;
				}
			}
			else {
				slime.hasDamaged = false;  // reset when not colliding, so next time it can deal damage again
			}
		}
		if (monster.show) {
			if (rectOverlap(x, y, width, height, monster.x, monster.y, MONSTER_WIDTH, MONSTER_HEIGHT)) {
				if (!monster.hasDamaged) {   // only damage once per pass
					hp -= 20;
					if (hp < 0) hp = 0;
					// check for game over
					if (hp == 0) {
						isGameOver = true;
					}
					monster.hasDamaged = true;  // mark that this monster already did damage

					// trigger glowing effect
					isHit = true;
					hitTimer = HIT_DURATION;
				}
			}
			else {
				monster.hasDamaged = false;  // reset when not colliding, so next time it can deal damage again
			}
		}
	}
	// Trigger trivia AFTER 50 successful actions (requested change)
	if (monsterCount >= 10) {
		showRiddle = true;
	}
	comeAround();

}


void updateScroll() {
	if (level3Scroll) {
		scrollX -= scrollingSpeed;
		int scrollbgwidth = 1000;
		if (scrollX <= -1000)
			scrollX = 0; // loop background
	}
}

void animate()
{
	// ==================== LEVEL 2 ANIMATION ====================
	if (level_2Page == 1) {
		if (isGameOver) return;  // Stop animation if game over in Level 2
		if (isJumping) return;   // Don't animate running while jumping

		if (isRunning) {
			currentFrame = (currentFrame + 1) % RUN_FRAMES;
		}
		else if (isTurning) {
			turnFrame = (turnFrame + 1) % TURN_FRAMES;
		}
	}

	// ==================== LEVEL 3 ANIMATION ====================
	else if (level_3Page == 1) {
		if (isPausedForAmulet || isGameOver3) return;  // Stop animation if paused/game over in Level 3
		if (isJumping) return;   // Don't animate running while jumping

		if (isRunning) {
			currentFrame = (currentFrame + 1) % ((d == FORWARD) ? RUN_FRAMES : BACK_RUN_FRAMES);
		}
		else if (isTurning) {
			if (d == FORWARD) {
				turnFrame = (turnFrame + 1) % TURN_FRAMES;
			}
			else {
				turnFrame = (turnFrame + 1) % BACK_TURN_FRAMES;
			}
		}
	}
}

int main()
{
	mciSendString("open \"Audios//background.mp3\" alias bgsong", NULL, 0, NULL);
	mciSendString("open \"Audios//gameover.mp3\" alias ggsong", NULL, 0, NULL);
	mciSendString("play bgsong repeat", NULL, 0, NULL);

	srand((unsigned int)time(NULL));
	initializeGridPositions();
	fillGrid();
	iSetTimer(20, updateScroll);
	optionY = SCREEN_HEIGHT / 2; // vertical center
	int numOptions = 4;
	int spacing = SCREEN_WIDTH / (numOptions + 1);
	for (int i = 0; i < numOptions; i++) {
		optionX[i] = spacing * (i + 1) - optionW / 2; // center each button
	}
	iInitialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Sword Call");

	iSetTimer(50, fixedUpdate);
	iSetTimer(100, animate);
	// --- Initialize rain method ---
	initRain();
	initializeAssiMonster1();
	iSetTimer(20, updateFireballs);
	iSetTimer(30, updateRain); // updates rain every 30 milliseconds
	iSetTimer(20, updateMonster);  // Code 2 monster update
	iSetTimer(20, updateSlime);    // Code 2 slime update
	iSetTimer(5000, spawnMonster);   // Code 2 spawner (large interval)
	iSetTimer(20, updateAssiMonster1);
	iSetTimer(6000, spawnEnemy);   // new code 3
	//iSetTimer(1000, messageTimer);  // 1000ms = 1 second
	loadImages();
	initializeMonster();
	initializeSlime();
	iStart();
	return 0;

}