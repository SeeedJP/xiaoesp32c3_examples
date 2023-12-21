/*
 * CbsNtShell.ino
 * Copyright (C) MATSUOKA Takashi <matsujirushi@live.jp>
 * MIT License
 */

////////////////////////////////////////////////////////////////////////////////
// IDE:
//   Arduino 2.2.1
// Platform:
//   esp32 2.0.14 - https://github.com/espressif/arduino-esp32
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
//   Natural Tiny Shell 0.3.1 - https://github.com/matsujirushi/ntshell

////////////////////////////////////////////////////////////////////////////////
// Includes

#include <ntshell.h>
#include <util/ntopt.h>

////////////////////////////////////////////////////////////////////////////////
// Variables

static ntshell_t nts;

////////////////////////////////////////////////////////////////////////////////
// NT-Shell handlers

static int usrcmd_ntopt_callback(int argc, char **argv, void *extobj);

static int serial_read(char *buf, int cnt, void *extobj)
{
  for (int i = 0; i < cnt; ++i)
  {
    int c;
    do
    {
      c = Serial.read();
    } while (c < 0);

    buf[i] = c;
  }

  return cnt;
}

static int serial_write(const char *buf, int cnt, void *extobj)
{
  return Serial.write(buf, cnt);
}

static int user_callback(const char *text, void *extobj)
{
  return ntopt_parse(text, usrcmd_ntopt_callback, extobj);
}

////////////////////////////////////////////////////////////////////////////////
// Commands implementation

typedef int (*USRCMDFUNC)(int argc, char **argv);

typedef struct
{
  const char *cmd;
  const char *desc;
  USRCMDFUNC func;
} cmd_table_t;

static int usrcmd_help(int argc, char **argv);
static int usrcmd_info(int argc, char **argv);

static const cmd_table_t cmdlist[] = {
    {"help", "This is a description text string for help command.", usrcmd_help},
    {"info", "This is a description text string for info command.", usrcmd_info},
};

static int usrcmd_ntopt_callback(int argc, char **argv, void *extobj)
{
  if (argc < 1)
  {
    return 0;
  }

  for (int i = 0; i < std::extent<decltype(cmdlist)>::value; ++i)
  {
    if (strcmp(argv[0], cmdlist[i].cmd) == 0)
    {
      return cmdlist[i].func(argc, argv);
    }
  }

  Serial.println("Unknown command found.");
  return 0;
}

static int usrcmd_help(int argc, char **argv)
{
  for (int i = 0; i < std::extent<decltype(cmdlist)>::value; ++i)
  {
    Serial.print(cmdlist[i].cmd);
    Serial.print("\t:");
    Serial.println(cmdlist[i].desc);
  }

  return 0;
}

static int usrcmd_info(int argc, char **argv)
{
  if (argc < 2)
  {
    Serial.println("info sys");
    Serial.println("info ver");
    return 0;
  }
  else if (strcmp(argv[1], "sys") == 0)
  {
    Serial.println("Seeed Studio XIAO ESP32C3");
    return 0;
  }
  else if (strcmp(argv[1], "ver") == 0)
  {
    Serial.println("Version 0.0.0");
    return 0;
  }
  else
  {
    Serial.println("Unknown sub command found.");
    return -1;
  }
}

////////////////////////////////////////////////////////////////////////////////
// setup and loop

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println();

  ntshell_init(&nts, serial_read, serial_write, user_callback, nullptr);
  ntshell_set_prompt(&nts, "XIAO>");
}

void loop()
{
  ntshell_execute(&nts);
}

////////////////////////////////////////////////////////////////////////////////
