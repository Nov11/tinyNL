//
// Created by c6s on 17-5-31.
//

#ifndef TINYNL_UTILS_H_H
#define TINYNL_UTILS_H_H

#include <vector>
#include <string>
#include "assert.h"

#include "bignum.h"
#include "bn_mul.h"
#include "md5.h"
#include "rsa.h"

int rsa_signer(std::vector<unsigned char> &message, std::vector<unsigned char> &result);
//std::vector<char> convertTo02X(unsigned char c);
std::string sign(std::string& response);

#endif //TINYNL_UTILS_H_H
