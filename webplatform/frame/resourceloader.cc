#include "resourceloader.h"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iosfwd>

namespace ve {
namespace webplatform {

ResourceResponse
ResourceLoader::FetchResource(const ResourceRequest &resource_request) {
  // currently support only loading from disk
  // Нужно определить что возвращать в качестве Reponse если ресурс недоступен
  std::filesystem::path html_file_path(resource_request.Url());
  std::ifstream html_file(html_file_path, std::ios::binary | std::ios::ate);
  if (html_file.is_open()) {
    const std::streampos end = html_file.tellg();
    if (end > 0) {
      std::vector<std::byte> raw_html(static_cast<std::size_t>(end));
      html_file.seekg(0, std::ios::beg);
      html_file.read(reinterpret_cast<char *>(raw_html.data()),
                     raw_html.size());
      return ResourceResponse(std::move(raw_html));
    }
  }
  return ResourceResponse();
}

// ResourceRequest
ResourceRequest::ResourceRequest(std::string url) : url_(url) {}
std::string ResourceRequest::Url() const { return url_; }

// ResourceResponse
ResourceResponse::ResourceResponse(std::vector<std::byte> body)
    : body_(std::move(body)) {}

std::span<std::byte> ResourceResponse::Payload() { return body_; }

} // namespace webplatform
} // namespace ve
