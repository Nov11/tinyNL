//
// Created by c6s on 17-5-22.
//

#ifndef TINYNL_LS_H
#define TINYNL_LS_H

#include "Utils.h"

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
#define PREHEADER   "<!-- %s -->\n%s"

#define FIELD_SALT  "salt"
#define FIELD_USER  "userName"
#define FIELD_TICK  "ticketId"
#define GETLEN(x)   (sizeof(x) - 1)
#define COTYPE_XML  1
#define COTYPE_HTML 2

struct HttpRequest {
    std::string line;
    std::vector<std::string> header;
    std::string body;
};
#endif //TINYNL_LS_H
