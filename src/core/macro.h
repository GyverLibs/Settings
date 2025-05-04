#pragma once

#define SUB_BUILD_BEGIN [&]() {
#define SUB_BUILD_END }();
#define SUB_BUILD_NEXT SUB_BUILD_END SUB_BUILD_BEGIN