#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/**
 * Game configuration constants
 */
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define PADDLE_WIDTH 20
#define PADDLE_HEIGHT 100
#define BALL_RADIUS 10
#define WINNER_DISPLAY_TIME 300 // 5 seconds at 60 FPS

/**
 * Paddle structure - represents player paddles
 */
typedef struct {
  float x;      // X position
  float y;      // Y position
  float width;  // Paddle width
  float height; // Paddle height
  float speed;  // Movement speed
  Color color;  // Paddle color
} Paddle;

/**
 * Ball structure - represents the game ball
 */
typedef struct {
  float x;         // X position
  float y;         // Y position
  float radius;    // Ball radius
  float velocityX; // X velocity
  float velocityY; // Y velocity
  Color color;     // Ball color
} Ball;

/**
 * Difficulty settings structure
 */
typedef struct {
  float paddleSpeed;  // Paddle movement speed
  float ballSpeed;    // Initial ball speed
  float botReaction;  // Bot reaction speed multiplier
  int botErrorChance; // Bot error chance percentage
} DifficultySettings;

/**
 * Game state structure
 */
typedef struct {
  Paddle leftPaddle;
  Paddle rightPaddle;
  Ball ball;
  float speedMultiplier; // Current ball speed multiplier
  int leftScore;         // Left player score
  int rightScore;        // Right player score
  int winScore;          // Score required to win
  Sound paddleHitSound;  // Sound when ball hits paddle
  Sound scoreSound;      // Sound when someone scores
} GameState;

/**
 * Initialize difficulty settings based on selected level
 * @param difficulty String representing difficulty level
 * @return DifficultySettings structure with appropriate values
 */
DifficultySettings getDifficultySettings(const char *difficulty) {
  DifficultySettings settings = {.paddleSpeed = 1.2f,
                                 .ballSpeed = 2.0f,
                                 .botReaction = 0.7f,
                                 .botErrorChance = 2};

  if (strcmp(difficulty, "easy") == 0) {
    settings.paddleSpeed = 1.8f;
    settings.ballSpeed = 1.5f;
    settings.botReaction = 0.5f;
    settings.botErrorChance = 8;
  } else if (strcmp(difficulty, "medium") == 0) {
    settings.paddleSpeed = 1.2f;
    settings.ballSpeed = 2.0f;
    settings.botReaction = 0.7f;
    settings.botErrorChance = 2;
  } else if (strcmp(difficulty, "hard") == 0) {
    settings.paddleSpeed = 0.8f;
    settings.ballSpeed = 2.5f;
    settings.botReaction = 0.9f;
    settings.botErrorChance = 1;
  } else if (strcmp(difficulty, "impossible") == 0) {
    settings.paddleSpeed = 0.6f;
    settings.ballSpeed = 3.0f;
    settings.botReaction = 1.0f;
    settings.botErrorChance = 0;
  }

  return settings;
}

/**
 * Generate sound effects programmatically
 * @param paddleHitSound Pointer to store paddle hit sound
 * @param scoreSound Pointer to store scoring sound
 */
void initializeSounds(Sound *paddleHitSound, Sound *scoreSound) {
  // Create paddle hit sound (short beep)
  Wave paddleWave = {0};
  paddleWave.sampleRate = 44100;
  paddleWave.sampleSize = 16;
  paddleWave.channels = 1;
  paddleWave.frameCount = 2205; // 0.05 seconds
  paddleWave.data = malloc(paddleWave.frameCount * sizeof(short));

  for (int i = 0; i < paddleWave.frameCount; i++) {
    ((short *)paddleWave.data)[i] =
        (short)(sin(i * 0.1) * 8000 * exp(-i * 0.001));
  }

  // Create score sound (longer beep)
  Wave scoreWave = {0};
  scoreWave.sampleRate = 44100;
  scoreWave.sampleSize = 16;
  scoreWave.channels = 1;
  scoreWave.frameCount = 8820; // 0.2 seconds
  scoreWave.data = malloc(scoreWave.frameCount * sizeof(short));

  for (int i = 0; i < scoreWave.frameCount; i++) {
    ((short *)scoreWave.data)[i] =
        (short)(sin(i * 0.05) * 6000 * exp(-i * 0.0005));
  }

  *paddleHitSound = LoadSoundFromWave(paddleWave);
  *scoreSound = LoadSoundFromWave(scoreWave);
  UnloadWave(paddleWave);
  UnloadWave(scoreWave);
}

/**
 * Initialize game state with given settings
 * @param winScore Score required to win
 * @param difficulty Game difficulty level
 * @return Initialized GameState structure
 */
GameState initializeGame(int winScore, const char *difficulty) {
  DifficultySettings settings = getDifficultySettings(difficulty);

  // Initialize paddles
  Paddle leftPaddle = {.x = 50,
                       .y = SCREEN_HEIGHT / 2.0f,
                       .width = PADDLE_WIDTH,
                       .height = PADDLE_HEIGHT,
                       .speed = settings.paddleSpeed,
                       .color = BLUE};

  Paddle rightPaddle = {.x = SCREEN_WIDTH - 50 - PADDLE_WIDTH,
                        .y = SCREEN_HEIGHT / 2.0f,
                        .width = PADDLE_WIDTH,
                        .height = PADDLE_HEIGHT,
                        .speed = settings.paddleSpeed,
                        .color = RED};

  // Initialize ball
  Ball ball = {.x = SCREEN_WIDTH / 2.0f,
               .y = SCREEN_HEIGHT / 2.0f,
               .radius = BALL_RADIUS,
               .velocityX = settings.ballSpeed,
               .velocityY = settings.ballSpeed * 0.5f,
               .color = WHITE};

  // Initialize sounds
  Sound paddleHitSound, scoreSound;
  initializeSounds(&paddleHitSound, &scoreSound);

  // Return complete game state
  GameState gameState = {.leftPaddle = leftPaddle,
                         .rightPaddle = rightPaddle,
                         .ball = ball,
                         .speedMultiplier = 0.3f,
                         .leftScore = 0,
                         .rightScore = 0,
                         .winScore = winScore,
                         .paddleHitSound = paddleHitSound,
                         .scoreSound = scoreSound};

  return gameState;
}

/**
 * Handle player input for left paddle
 * @param paddle Pointer to left paddle structure
 */
void handlePlayerInput(Paddle *paddle) {
  if (IsKeyDown(KEY_W)) {
    paddle->y -= paddle->speed;
  }
  if (IsKeyDown(KEY_S)) {
    paddle->y += paddle->speed;
  }

  // Keep paddle in bounds
  if (paddle->y < 0) {
    paddle->y = 0;
  }
  if (paddle->y + paddle->height > SCREEN_HEIGHT) {
    paddle->y = SCREEN_HEIGHT - paddle->height;
  }
}

/**
 * Handle bot AI for right paddle with imperfect behavior
 * @param paddle Pointer to right paddle structure
 * @param ball Pointer to ball structure
 * @param botReaction Bot reaction speed multiplier
 * @param botErrorChance Bot error chance percentage
 */
void handleBotAI(Paddle *paddle, const Ball *ball, float botReaction,
                 int botErrorChance) {
  float paddleCenter = paddle->y + paddle->height / 2.0f;
  float ballDistance = ball->y - paddleCenter;

  // Only move if ball is somewhat far from paddle center (reaction delay)
  if (ballDistance < -20) {
    paddle->y -= paddle->speed * botReaction;
  } else if (ballDistance > 20) {
    paddle->y += paddle->speed * botReaction;
  }

  // Add some randomness to make it beatable
  if (rand() % 100 < botErrorChance) {
    // Do nothing (bot makes mistake)
  }

  // Keep paddle in bounds
  if (paddle->y < 0) {
    paddle->y = 0;
  }
  if (paddle->y + paddle->height > SCREEN_HEIGHT) {
    paddle->y = SCREEN_HEIGHT - paddle->height;
  }
}

/**
 * Update ball position and handle wall collisions
 * @param ball Pointer to ball structure
 * @param speedMultiplier Current speed multiplier
 */
void updateBall(Ball *ball, float speedMultiplier) {
  // Update position
  ball->x += ball->velocityX * speedMultiplier;
  ball->y += ball->velocityY * speedMultiplier;

  // Handle top/bottom wall collisions
  if (ball->y - ball->radius <= 0 || ball->y + ball->radius >= SCREEN_HEIGHT) {
    ball->velocityY = -ball->velocityY;
  }
}

/**
 * Check and handle paddle collisions
 * @param ball Pointer to ball structure
 * @param paddle Pointer to paddle structure
 * @param speedMultiplier Pointer to speed multiplier (will be increased on hit)
 * @return 1 if collision occurred, 0 otherwise
 */
int handlePaddleCollision(Ball *ball, Paddle *paddle, float *speedMultiplier) {
  // Check if ball intersects with paddle
  if (ball->x - ball->radius <= paddle->x + paddle->width &&
      ball->x + ball->radius >= paddle->x && ball->y >= paddle->y &&
      ball->y <= paddle->y + paddle->height) {

    // Only reverse direction if ball is moving toward paddle
    if ((ball->x < paddle->x && ball->velocityX < 0) ||
        (ball->x > paddle->x && ball->velocityX > 0)) {

      // Reverse ball direction
      ball->velocityX = -ball->velocityX;

      // Increase speed on paddle hit (max 2.0x)
      if (*speedMultiplier < 2.0f) {
        *speedMultiplier += 0.1f;
      }

      // Add randomness to Y velocity
      ball->velocityY += (float)(rand() % 200 - 100) / 100.0f;

      // Limit Y velocity
      if (ball->velocityY > 4.0f) {
        ball->velocityY = 4.0f;
      }
      if (ball->velocityY < -4.0f) {
        ball->velocityY = -4.0f;
      }

      // Move ball outside paddle to prevent sticking
      if (ball->velocityX > 0) {
        ball->x = paddle->x + paddle->width + ball->radius;
      } else {
        ball->x = paddle->x - ball->radius;
      }

      return 1; // Collision occurred
    }
  }

  return 0; // No collision
}

/**
 * Handle scoring when ball goes off screen
 * @param ball Pointer to ball structure
 * @param leftScore Pointer to left player score
 * @param rightScore Pointer to right player score
 * @param ballSpeed Base ball speed for reset
 * @return 1 if scoring occurred, 0 otherwise
 */
int handleScoring(Ball *ball, int *leftScore, int *rightScore,
                  float ballSpeed) {
  if (ball->x < 0) {
    (*rightScore)++;
    return 1;
  } else if (ball->x > SCREEN_WIDTH) {
    (*leftScore)++;
    return 1;
  }
  return 0;
}

/**
 * Reset ball to center with random angle
 * @param ball Pointer to ball structure
 * @param ballSpeed Base ball speed
 * @param towardPlayer Which player to shoot ball toward (1=left, -1=right)
 */
void resetBall(Ball *ball, float ballSpeed, int towardPlayer) {
  ball->x = SCREEN_WIDTH / 2.0f;
  ball->y = SCREEN_HEIGHT / 2.0f;

  // Random angle between -45 and 45 degrees
  float angle = (float)(rand() % 90 - 45) * 3.14159f / 180.0f;
  ball->velocityX = cos(angle) * ballSpeed;
  ball->velocityY = sin(angle) * ballSpeed;

  // Ensure ball shoots toward correct player
  if ((towardPlayer == 1 && ball->velocityX < 0) ||
      (towardPlayer == -1 && ball->velocityX > 0)) {
    ball->velocityX = -ball->velocityX;
  }
}

/**
 * Draw game elements on screen
 * @param gameState Pointer to game state structure
 */
void drawGame(const GameState *gameState) {
  ClearBackground(BLACK);

  // Draw center line
  for (int i = 0; i < SCREEN_HEIGHT; i += 40) {
    DrawRectangle(SCREEN_WIDTH / 2 - 2, i, 4, 20, RAYWHITE);
  }

  // Draw scores
  DrawText(TextFormat("%d", gameState->leftScore), SCREEN_WIDTH / 2 - 100, 50,
           80, RAYWHITE);
  DrawText(TextFormat("%d", gameState->rightScore), SCREEN_WIDTH / 2 + 60, 50,
           80, RAYWHITE);

  // Draw game objects
  DrawRectangle(gameState->leftPaddle.x, gameState->leftPaddle.y,
                gameState->leftPaddle.width, gameState->leftPaddle.height,
                gameState->leftPaddle.color);
  DrawRectangle(gameState->rightPaddle.x, gameState->rightPaddle.y,
                gameState->rightPaddle.width, gameState->rightPaddle.height,
                gameState->rightPaddle.color);
  DrawCircle(gameState->ball.x, gameState->ball.y, gameState->ball.radius,
             gameState->ball.color);
}

/**
 * Display winner screen with countdown
 * @param winner String declaring the winner
 * @param gameState Pointer to game state structure
 */
void displayWinner(const char *winner, const GameState *gameState) {
  for (int i = 0; i < WINNER_DISPLAY_TIME; i++) {
    BeginDrawing();
    ClearBackground(BLACK);

    // Draw center line
    for (int j = 0; j < SCREEN_HEIGHT; j += 40) {
      DrawRectangle(SCREEN_WIDTH / 2 - 2, j, 4, 20, RAYWHITE);
    }

    // Draw final scores
    DrawText(TextFormat("%d", gameState->leftScore), SCREEN_WIDTH / 2 - 100, 50,
             80, RAYWHITE);
    DrawText(TextFormat("%d", gameState->rightScore), SCREEN_WIDTH / 2 + 60, 50,
             80, RAYWHITE);

    // Draw winner message
    DrawText(winner, SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2, 60, GREEN);
    DrawText("Game closing in 5 seconds...", SCREEN_WIDTH / 2 - 120,
             SCREEN_HEIGHT / 2 + 80, 30, RAYWHITE);

    EndDrawing();
    WaitTime(1.0f / 60.0f); // Wait for next frame
  }
}

/**
 * Main game loop
 * @param gameState Pointer to game state structure
 * @param settings Difficulty settings
 * @return 1 if game should continue, 0 if game ended
 */
int gameLoop(GameState *gameState, const DifficultySettings *settings) {
  while (!WindowShouldClose()) {
    // Handle input
    handlePlayerInput(&gameState->leftPaddle);
    handleBotAI(&gameState->rightPaddle, &gameState->ball,
                settings->botReaction, settings->botErrorChance);

    // Update game state
    updateBall(&gameState->ball, gameState->speedMultiplier);

    // Handle collisions
    if (handlePaddleCollision(&gameState->ball, &gameState->leftPaddle,
                              &gameState->speedMultiplier)) {
      PlaySound(gameState->paddleHitSound);
    }
    if (handlePaddleCollision(&gameState->ball, &gameState->rightPaddle,
                              &gameState->speedMultiplier)) {
      PlaySound(gameState->paddleHitSound);
    }

    // Handle scoring
    if (handleScoring(&gameState->ball, &gameState->leftScore,
                      &gameState->rightScore, settings->ballSpeed)) {
      PlaySound(gameState->scoreSound);
      resetBall(&gameState->ball, settings->ballSpeed,
                gameState->leftScore > gameState->rightScore ? -1 : 1);
      gameState->speedMultiplier = 0.3f; // Reset speed multiplier
    }

    // Draw everything
    drawGame(gameState);
    EndDrawing();

    // Check win condition
    if (gameState->leftScore >= gameState->winScore) {
      displayWinner("LEFT PLAYER WINS!", gameState);
      return 0; // Game ended
    } else if (gameState->rightScore >= gameState->winScore) {
      displayWinner("RIGHT PLAYER WINS!", gameState);
      return 0; // Game ended
    }
  }

  return 1; // Continue game
}

/**
 * Parse command line arguments
 * @param argc Argument count
 * @param argv Argument array
 * @param winScore Pointer to store win score
 * @param difficulty Pointer to store difficulty string
 */
void parseArguments(int argc, char *argv[], int *winScore,
                    const char **difficulty) {
  *winScore = 5;          // Default
  *difficulty = "medium"; // Default

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--win-req") == 0 && i + 1 < argc) {
      *winScore = atoi(argv[i + 1]);
      i++; // Skip next argument
    } else if (strcmp(argv[i], "--difficulty") == 0 && i + 1 < argc) {
      *difficulty = argv[i + 1];
      i++; // Skip next argument
    }
  }
}

/**
 * Main game function
 * @param winScore Score required to win
 * @param difficulty Game difficulty level
 * @return 0 on successful completion
 */
int runGame(int winScore, const char *difficulty) {
  printf("Win Score: %d, Difficulty: %s\n", winScore, difficulty);

  // Initialize raylib
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib Pong");
  InitAudioDevice();
  srand(time(NULL));

  // Initialize game
  GameState gameState = initializeGame(winScore, difficulty);
  DifficultySettings settings = getDifficultySettings(difficulty);

  // Run game loop
  gameLoop(&gameState, &settings);

  // Cleanup
  CloseWindow();
  CloseAudioDevice();
  UnloadSound(gameState.paddleHitSound);
  UnloadSound(gameState.scoreSound);

  return 0;
}

/**
 * Program entry point
 * @param argc Command line argument count
 * @param argv Command line argument array
 * @return 0 on successful completion
 */
int main(int argc, char *argv[]) {
  printf("Raylib Pong Game\n");

  // Parse command line arguments
  int winScore;
  const char *difficulty;
  parseArguments(argc, argv, &winScore, &difficulty);

  // Run the game
  return runGame(winScore, difficulty);
}
