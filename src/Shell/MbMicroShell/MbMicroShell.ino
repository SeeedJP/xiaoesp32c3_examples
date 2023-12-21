/*
 * MbMicroShell.ino
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
//   MicroShell 0.1.0+b384ef6 - https://github.com/marcinbor85/microshell

////////////////////////////////////////////////////////////////////////////////
// Includes

#include <microshell.h>

////////////////////////////////////////////////////////////////////////////////
// Constants

#define BUF_IN_SIZE 128
#define BUF_OUT_SIZE 128
#define PATH_MAX_SIZE 128

////////////////////////////////////////////////////////////////////////////////
// Variables

static struct ush_object ush;

static char ush_in_buf[BUF_IN_SIZE];
static char ush_out_buf[BUF_OUT_SIZE];

////////////////////////////////////////////////////////////////////////////////
// MicroShell handlers

static int ush_read(struct ush_object *self, char *ch)
{
  const auto c = Serial.read();
  if (c < 0)
  {
    return 0;
  }

  *ch = c;

  return 1;
}

static int ush_write(struct ush_object *self, char ch)
{
  return Serial.write(ch) == 1;
}

static const struct ush_io_interface ush_iface = {
    .read = ush_read,
    .write = ush_write,
};

static const struct ush_descriptor ush_desc = {
    .io = &ush_iface,                          // I/O interface pointer
    .input_buffer = ush_in_buf,                // working input buffer
    .input_buffer_size = sizeof(ush_in_buf),   // working input buffer size
    .output_buffer = ush_out_buf,              // working output buffer
    .output_buffer_size = sizeof(ush_out_buf), // working output buffer size
    .path_max_length = PATH_MAX_SIZE,          // path maximum length (stack)
    .hostname = "XIAO",                        // hostname (in prompt)
    .exec = nullptr,
};

////////////////////////////////////////////////////////////////////////////////
// File system implementation

static struct ush_node_object root;

size_t info_get_data_callback(struct ush_object *self, struct ush_file_descriptor const *file, uint8_t **data)
{
  static const char *info = "Use MicroShell and make fun!\r\n";

  *data = const_cast<uint8_t *>(reinterpret_cast<const uint8_t *>(info));
  return strlen(info);
}

static const struct ush_file_descriptor info_file[] = {
    {
        .name = "info.txt",
        .description = nullptr,
        .help = nullptr,
        .exec = nullptr,
        .get_data = info_get_data_callback,
        .set_data = nullptr,
        .process = nullptr,
    }};

////////////////////////////////////////////////////////////////////////////////
// setup and loop

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println();

  ush_init(&ush, &ush_desc);

  ush_node_mount(&ush, "/", &root, info_file, std::extent<decltype(info_file)>::value);
}

void loop()
{
  ush_service(&ush);
}

////////////////////////////////////////////////////////////////////////////////
