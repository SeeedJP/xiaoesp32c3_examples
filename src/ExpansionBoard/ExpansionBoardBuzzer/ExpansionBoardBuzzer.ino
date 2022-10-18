/*
 * ExpansionBoardBuzzer.ino
 * Copyright (C) 2022 Takashi Matsuoka <matsujirushi@live.jp>
 * MIT License
 */

////////////////////////////////////////////////////////////////////////////////
// IDE:
//   Arduino 2.0.0
// Platform:
//   esp32 2.0.5 - https://github.com/espressif/arduino-esp32
// Board:
//   XIAO_ESP32C3
// Board Settings:
//   USB CDC On Boot: Enabled
//   CPU Frequency: 160MHz(WiFi)
//   Core Debug Level: None
//   Erase All Flash Before Sketch Upload: Disabled
//   Flash Frequency: 80MHz
//   Flash Mode: QIO
//   Flash Size: 4MB(32Mb)
//   Partition Scheme: Default 4MB with spiffs(1.2MB APP/1.5MB SPIFFS)
//   Upload Speed: 921600
// Libraries:

////////////////////////////////////////////////////////////////////////////////
// Constants

static constexpr uint8_t BUZZER_PIN = D3;
static constexpr int TEMPO = 50;

static constexpr uint8_t LEDC_CHANNEL = 0;

struct NoteType
{
  char Tone;
  int Beat;
};

static const NoteType NOTES[] =
{
  { 'G', 6 },
  { 'G', 2 },
  { 'A', 8 },
  { 'G', 8 },
  { 'c', 8 },
  { 'B', 16 },

  { 'G', 6 },
  { 'G', 2 },
  { 'A', 8 },
  { 'G', 8 },
  { 'd', 8 },
  { 'c', 16 },

  { 'G', 6 },
  { 'G', 2 },
  { 'g', 8 },
  { 'e', 8 },
  { 'c', 8 },
  { 'B', 8 },
  { 'A', 8 },

  { 'f', 6 },
  { 'f', 2 },
  { 'e', 8 },
  { 'c', 8 },
  { 'd', 8 },
  { 'c', 16 },

  { ' ', 16 },
};

struct ToneType
{
  char Name;
  int Frequency;
};

static const ToneType TONES[] =
{
  { 'C', 262 },
  { 'D', 294 },
  { 'E', 330 },
  { 'F', 349 },
  { 'G', 392 },
  { 'A', 440 },
  { 'B', 494 },
  { 'c', 523 },
  { 'd', 587 },
  { 'e', 659 },
  { 'f', 698 },
  { 'g', 784 },
  { 'a', 880 },
  { 'b', 988 },
};

////////////////////////////////////////////////////////////////////////////////
// setup and loop

void setup()
{
  ledcAttachPin(BUZZER_PIN, LEDC_CHANNEL);
}
 
void loop()
{
  for (int n = 0; n < std::extent<decltype(NOTES)>::value; n++)
  {
    for (int t = 0; t < std::extent<decltype(TONES)>::value; t++)
    {
      if (TONES[t].Name == NOTES[n].Tone)
      {
        ledcWriteTone(LEDC_CHANNEL, TONES[t].Frequency);
        break;
      }
    }
    delay(NOTES[n].Beat * TEMPO * 4 / 5);

    ledcWriteTone(LEDC_CHANNEL, 0);
    delay(NOTES[n].Beat * TEMPO * 1 / 5);
  }
}

////////////////////////////////////////////////////////////////////////////////
