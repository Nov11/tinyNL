//
// Created by c6s on 17-5-22.
//

#ifndef TINYNL_LS_H
#define TINYNL_LS_H

#include "bignum.h"
#include "bn_mul.h"
#include "md5.h"
#include "rsa.h"

#define DEF_PROLONG 604800000
#define DEF_USER    "privatelo"
#define DEF_SALT    "1490283405643"
#define DEF_TICK    "1"
#define HTTP_OK     "HTTP/1.1 200 OK\r\n"
#define HTTP_BAD    "HTTP/1.1 403 Forbidden\r\n"
#define XML_TICKET  "<ObtainTicketResponse><message></message><prolongationPeriod>%s" \
                    "</prolongationPeriod><responseCode>OK</responseCode><salt>%s" \
                    "</salt><ticketId>1</ticketId><ticketProperties>licensee=%s" \
                    "\tlicenseType=0\t" \
                    "</ticketProperties></ObtainTicketResponse>"
#define XML_PING    "<PingResponse><message></message><responseCode>OK</responseCode><salt>%s" \
                    "</salt></PingResponse>"
#define XML_RELEASE "<ReleaseTicketResponse><message></message><responseCode>OK</r" \
                    "esponseCode><salt>%s</salt></ReleaseTicketResponse>"
#define XML_PROLONG "<ProlongTicketResponse><message></message><responseCode>OK</r" \
                    "esponseCode><salt>%s</salt><ticketId>%s</ticketId></Prolong" \
                    "TicketResponse>"
#define FIELD_SALT  "salt="
#define FIELD_USER  "userName="
#define FIELD_TICK  "ticketId="
#define GETLEN(x)   (sizeof(x) - 1)
#define COTYPE_XML  1
#define COTYPE_HTML 2


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
#endif //TINYNL_LS_H
