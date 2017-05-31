#include "Utils.h"
#include "ls.h"

#define KEY_RADIX 16
#define KEY_LEN 64
#define KEY_P "cb09437c6225e9a7276a43eb98913420bf2abee3b716db0706" \
              "ba6452e3a75da9"
#define KEY_Q "e782429f8d5f17c199f746fd2f976d150076ff221c197e17a3" \
              "b6c2be5f0368f3"
#define KEY_N "b79cab7070008e18259c2127eb279c3163de51770cca8d3e8a" \
              "d5eb2e592970a208caa3f0e78e9b19386195bd69a57d72497f" \
              "ce500e2349a9077a434fe5e58f6b"
#define KEY_E "10001"
#define KEY_D "5bafda543d39a9bb191ccbd9b92d806b916934d8f404395fdf" \
              "c84eb64843cf3fe19186fe10160b2ef95d860b03d0ebd1372f" \
              "32873ec48da52a2c20748ee20de1"


int rsa_signer(std::vector<unsigned char> &message, std::vector<unsigned char> &result) {
    rsa_context ctx;
    uint8_t hash[16];
    int ret;

    rsa_init(&ctx);
    mpi_init(&ctx.P);
    mpi_init(&ctx.Q);
    mpi_init(&ctx.N);
    mpi_init(&ctx.E);
    mpi_init(&ctx.D);
    mpi_read_string(&ctx.P, KEY_RADIX, KEY_P);
    mpi_read_string(&ctx.Q, KEY_RADIX, KEY_Q);
    mpi_read_string(&ctx.N, KEY_RADIX, KEY_N);
    mpi_read_string(&ctx.E, KEY_RADIX, KEY_E);
    mpi_read_string(&ctx.D, KEY_RADIX, KEY_D);
    ctx.len = KEY_LEN;

    md5(message.data(), message.size(), hash);
    result.resize(ctx.len);
    ret = rsa_sign(&ctx, hash, result.data());
    if (ret == 0) {

    } else {
        assert(false);
    }
    rsa_free(&ctx);
    return ret;
}


char hex(int i){
    if(i >= 0 && i <= 9){
        return static_cast<char>('0' + i);
    }
    return static_cast<char>(i - 10 + 'a');
}

std::vector<char> convertTo02X(unsigned char c){
    std::vector<char> result(2);
    int i = c;
    int least = i % 16;
    result[1] = hex(least);
    int high = i / 16;
    result[0] = hex(high);
    return result;
}


std::string sign(std::string& response){
    std::vector<unsigned char> msg;
    for (size_t i = 0; i < response.size(); i++) {
        char c = response[i];
        unsigned char uc = c;
        assert(uc == c);
        msg.push_back(uc);
    }
    std::vector<unsigned char> result;
    rsa_signer(msg, result);
    std::string signedStr ;
    for(size_t i = 0; i < result.size(); i++){
        std::vector<char> vc = convertTo02X(result[i]);
        signedStr.push_back(vc[0]);
        signedStr.push_back(vc[1]);
    }
    return signedStr;
}