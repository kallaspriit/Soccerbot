#ifndef JSONRESPONSE_H
#define JSONRESPONSE_H

#include <string>

class JsonResponse {
    public:
        JsonResponse(std::string id, std::string payload);
        std::string toJSON();

    private:
        std::string id;
        std::string payload;
};

#endif // JSONRESPONSE_H
