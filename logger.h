//
// Created by Don Goodman-Wilson on 13/11/2017.
//

#pragma once

#include <string>
#include <luna/luna.h>

namespace magique
{

void error_logger(luna::log_level level, const std::string &message);

void access_logger(const luna::request &request, const luna::response &response);

const std::string VERSION{"v1"};

}