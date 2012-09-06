#include "JsonResponse.h"

#include <sstream>

JsonResponse::JsonResponse(std::string id, std::string payload) :
    id(id),
    payload(payload)
{}

std::string JsonResponse::toJSON() {
    std::stringstream stream;

    stream << "{";
    stream << "\"id\":\"" << id << "\",";
    stream << "\"payload\":" << payload;
    stream << "}";

    return stream.str();
}
