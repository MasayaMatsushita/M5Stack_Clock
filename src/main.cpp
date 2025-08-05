#include <M5Core2.h>
#include <math.h>
#include <time.h>
#include <WiFi.h>
#include "secrets.h"

#define CLOCK_CENTER_X 160
#define CLOCK_CENTER_Y 120
#define CLOCK_RADIUS   100

TFT_eSprite* canvas = nullptr; // ここを変更

void connectWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.setCursor(10, 10);
    M5.Lcd.setTextColor(RED, BLACK);
    M5.Lcd.print("WiFi Connecting...");
  }
  M5.Lcd.fillScreen(BLACK);
}

void drawClockFace(TFT_eSprite* c) {
  c->fillCircle(CLOCK_CENTER_X, CLOCK_CENTER_Y, CLOCK_RADIUS, BLACK);
  c->drawCircle(CLOCK_CENTER_X, CLOCK_CENTER_Y, CLOCK_RADIUS, WHITE);
  for (int i = 0; i < 12; i++) {
    float angle = (i * 30 - 90) * DEG_TO_RAD;
    int x1 = CLOCK_CENTER_X + cos(angle) * (CLOCK_RADIUS - 10);
    int y1 = CLOCK_CENTER_Y + sin(angle) * (CLOCK_RADIUS - 10);
    int x2 = CLOCK_CENTER_X + cos(angle) * (CLOCK_RADIUS - 2);
    int y2 = CLOCK_CENTER_Y + sin(angle) * (CLOCK_RADIUS - 2);
    c->drawLine(x1, y1, x2, y2, WHITE);
  }
}

void drawHands(TFT_eSprite* c, int hour, int minute, int second) {
  float hour_angle = ((hour % 12) + minute / 60.0) * 30 - 90;
  float hour_rad = hour_angle * DEG_TO_RAD;
  int hx = CLOCK_CENTER_X + cos(hour_rad) * (CLOCK_RADIUS - 50);
  int hy = CLOCK_CENTER_Y + sin(hour_rad) * (CLOCK_RADIUS - 50);
  c->drawLine(CLOCK_CENTER_X, CLOCK_CENTER_Y, hx, hy, YELLOW);

  float min_angle = (minute + second / 60.0) * 6 - 90;
  float min_rad = min_angle * DEG_TO_RAD;
  int mx = CLOCK_CENTER_X + cos(min_rad) * (CLOCK_RADIUS - 30);
  int my = CLOCK_CENTER_Y + sin(min_rad) * (CLOCK_RADIUS - 30);
  c->drawLine(CLOCK_CENTER_X, CLOCK_CENTER_Y, mx, my, CYAN);

  float sec_angle = second * 6 - 90;
  float sec_rad = sec_angle * DEG_TO_RAD;
  int sx = CLOCK_CENTER_X + cos(sec_rad) * (CLOCK_RADIUS - 15);
  int sy = CLOCK_CENTER_Y + sin(sec_rad) * (CLOCK_RADIUS - 15);
  c->drawLine(CLOCK_CENTER_X, CLOCK_CENTER_Y, sx, sy, RED);
}

void setup() {
  M5.begin();
  M5.Lcd.fillScreen(BLACK);
  connectWiFi();
  configTime(9 * 3600, 0, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp"); // JST

  // スプライト初期化
  canvas = new TFT_eSprite(&M5.Lcd);
  canvas->createSprite(320, 240);
  canvas->setTextSize(2);
  canvas->setTextColor(WHITE, BLACK);
}

void loop() {
  static int prev_sec = -1;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    canvas->fillSprite(BLACK);
    canvas->setCursor(10, 10);
    canvas->setTextColor(RED, BLACK);
    canvas->print("No Time");
    canvas->pushSprite(0, 0);
    delay(1000);
    return;
  }

  if (timeinfo.tm_sec != prev_sec) {
    prev_sec = timeinfo.tm_sec;
    canvas->fillSprite(BLACK);
    drawClockFace(canvas);
    drawHands(canvas, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    canvas->pushSprite(0, 0);
  }
  delay(20);
}