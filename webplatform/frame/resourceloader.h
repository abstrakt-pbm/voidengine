#pragma once

#include <span>
#include <string>
#include <vector>

namespace ve {
namespace webplatform {

class ResourceRequest {
public:
  ResourceRequest(std::string url);
  std::string Url() const;

private:
  std::string url_;
};

class ResourceResponse {
public:
  ResourceResponse() = default;

  ResourceResponse(std::vector<std::byte> body);
  std::span<std::byte> Payload();

private:
  std::vector<std::byte> body_;
};

// Загрузить ресурс из источника
class ResourceLoader {
public:
  ResourceResponse FetchResource(const ResourceRequest &resource_request);
};

} // namespace webplatform
} // namespace ve
