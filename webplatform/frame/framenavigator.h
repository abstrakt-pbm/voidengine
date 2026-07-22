#pragma once

#include <string>

#include "documentparcer.h"
#include "resourceloader.h"

namespace ve {
namespace webplatform {

class Frame;

class FrameNavigationRequest {
public:
  FrameNavigationRequest(std::string url);
  std::string Url() const;

private:
  std::string url_;
};

// Как осуществить навигацию
// Инварианты навигации
// В текущем состоянии поддерживается только загрузка из пустоты
class FrameNavigator {
public:
  FrameNavigator(Frame *frame);
  void StartNavigation(const FrameNavigationRequest &navigation_request);

private:
  Frame *frame_;
  ResourceLoader resource_loader_;
  DocumentParcer document_parcer_;
  TextDecoder text_decoder_;
};

} // namespace webplatform
} // namespace ve

// Сегодня нужно получить по URL-у готовый к парсингу документ
