#pragma once

#include <iostream>
#include <algorithm>
#include <math.h>
#include <set>

#include "Socket.h"

constexpr u_short CLIENT_PORT = 7778;
constexpr u_short SERVER_PORT = 7777;

#define WIFI_MODE
#ifdef WIFI_MODE
const std::string SERVER_IP = "172.30.1.53";
const std::string CLIENT_IP = "172.30.1.97";

#else

const std::string SERVER_IP = "127.0.0.1";
const std::string CLIENT_IP = "127.0.0.1";
#endif

#define WINDOW_SIZE 5
