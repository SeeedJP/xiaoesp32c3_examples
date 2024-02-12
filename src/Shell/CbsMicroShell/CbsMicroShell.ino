/*
 * CbsMicroShell.ino
 * Copyright (C) MATSUOKA Takashi <matsujirushi@live.jp>
 * MIT License
 */

////////////////////////////////////////////////////////////////////////////////
// IDE:
//   Arduino 2.3.0
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

////////////////////////////////////////////////////////////////////////////////
// Includes

#include "src/microshell/core/microshell.h"
#include "src/microshell/util/mscmd.h"
#include "src/microshell/core/msconf.h"

////////////////////////////////////////////////////////////////////////////////
// Variables

static MICROSHELL ms;
static MSCMD mscmd;

////////////////////////////////////////////////////////////////////////////////
// MicroShell handlers

static void utx(char c)
{
  Serial.write(c);
}

static char urx(void)
{
  int c;
  do
  {
    c = Serial.read();
  } while (c < 0);

  return c;
}

////////////////////////////////////////////////////////////////////////////////
// Commands implementation

static MSCMD_USER_RESULT usrcmd_system(MSOPT *msopt, MSCMD_USER_OBJECT usrobj);
static MSCMD_USER_RESULT usrcmd_help(MSOPT *msopt, MSCMD_USER_OBJECT usrobj);

static MSCMD_COMMAND_TABLE table[] = {
    {"system", usrcmd_system},
    {"help", usrcmd_help},
};

static MSCMD_USER_RESULT usrcmd_system(MSOPT *msopt, MSCMD_USER_OBJECT usrobj)
{
  Serial.println("[SYSTEM]");

  int argc;
  msopt_get_argc(msopt, &argc);
  for (int i = 0; i < argc; ++i)
  {
    char buf[MSCONF_MAX_INPUT_LENGTH];
    msopt_get_argv(msopt, i, buf, sizeof(buf));
    Serial.println(buf);
  }

  return 0;
}

static MSCMD_USER_RESULT usrcmd_help(MSOPT *msopt, MSCMD_USER_OBJECT usrobj)
{
  Serial.print(
      "system : system command\r\n"
      "help   : help command\r\n");

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// setup and loop

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println();

  microshell_init(&ms, utx, urx, nullptr);
  mscmd_init(&mscmd, table, std::extent<decltype(table)>::value, nullptr);
}

void loop()
{
  Serial.print("MicroShell>");

  char buf[MSCONF_MAX_INPUT_LENGTH];
  microshell_getline(&ms, buf, sizeof(buf));

  MSCMD_USER_RESULT r;
  mscmd_execute(&mscmd, buf, &r);
}

////////////////////////////////////////////////////////////////////////////////
