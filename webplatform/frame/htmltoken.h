#pragma once

#include <array>
#include <string>
#include <string_view>
#include <vector>

namespace ve {
namespace webplatform {

struct HTMLAttribute {
  std::string name;
  std::string value;

  std::string ToString() const;
};

enum class HTMLTag {
  kUnknown = 0,
  kA,
  kAbbr,
  kAcronym,
  kAddress,
  kApplet,
  kArea,
  kArticle,
  kAside,
  kAudio,
  kB,
  kBase,
  kBasefont,
  kBdi,
  kBdo,
  kBgsound,
  kBig,
  kBlockquote,
  kBody,
  kBr,
  kButton,
  kCamera,
  kCanvas,
  kCaption,
  kCenter,
  kCite,
  kCode,
  kCol,
  kColgroup,
  kCommand,
  kCredential,
  kData,
  kDatalist,
  kDd,
  kDel,
  kDetails,
  kDfn,
  kDialog,
  kDir,
  kDiv,
  kDl,
  kDt,
  kEm,
  kEmbed,
  kFencedframeOrUnknown,
  kFieldset,
  kFigcaption,
  kFigure,
  kFont,
  kFooter,
  kForm,
  kFrame,
  kFrameset,
  kGeolocation,
  kH1,
  kH2,
  kH3,
  kH4,
  kH5,
  kH6,
  kHead,
  kHeader,
  kHgroup,
  kHr,
  kHTML,
  kI,
  kIFrame,
  kImage,
  kImg,
  kInput,
  kIns,
  kInstallOrUnknown,
  kKbd,
  kKeygen,
  kLabel,
  kLayer,
  kLegend,
  kLi,
  kLink,
  kListing,
  kLogin,
  kMain,
  kMap,
  kMark,
  kMarquee,
  kMenu,
  kMenubar,
  kMenuitem,
  kMenulist,
  kMeta,
  kMeter,
  kMicrophone,
  kNav,
  kNobr,
  kNoembed,
  kNoframes,
  kNolayer,
  kNoscript,
  kObject,
  kOl,
  kOptgroup,
  kOption,
  kOutput,
  kP,
  kParam,
  kPicture,
  kPlaintext,
  kPre,
  kProgress,
  kQ,
  kRb,
  kRp,
  kRt,
  kRTC,
  kRuby,
  kS,
  kSamp,
  kScript,
  kSearch,
  kSection,
  kSelect,
  kSelectedcontent,
  kSlot,
  kSmall,
  kSource,
  kSpan,
  kStrike,
  kStrong,
  kStyle,
  kSub,
  kSubmenu,
  kSummary,
  kSup,
  kTable,
  kTbody,
  kTd,
  kTemplate,
  kTextarea,
  kTfoot,
  kTh,
  kThead,
  kTime,
  kTitle,
  kTr,
  kTrack,
  kTt,
  kU,
  kUl,
  kUsermediaOrUnknown,
  kVar,
  kVideo,
  kWbr,
  kXmp,
};

struct HTMLTagEntry {
  std::string_view name;
  HTMLTag tag;
};

inline constexpr auto kHTMLTagEntries = std::to_array<HTMLTagEntry>({
    {"a", HTMLTag::kA},
    {"abbr", HTMLTag::kAbbr},
    {"acronym", HTMLTag::kAcronym},
    {"address", HTMLTag::kAddress},
    {"applet", HTMLTag::kApplet},
    {"area", HTMLTag::kArea},
    {"article", HTMLTag::kArticle},
    {"aside", HTMLTag::kAside},
    {"audio", HTMLTag::kAudio},

    {"b", HTMLTag::kB},
    {"base", HTMLTag::kBase},
    {"basefont", HTMLTag::kBasefont},
    {"bdi", HTMLTag::kBdi},
    {"bdo", HTMLTag::kBdo},
    {"bgsound", HTMLTag::kBgsound},
    {"big", HTMLTag::kBig},
    {"blockquote", HTMLTag::kBlockquote},
    {"body", HTMLTag::kBody},
    {"br", HTMLTag::kBr},
    {"button", HTMLTag::kButton},

    {"canvas", HTMLTag::kCanvas},
    {"caption", HTMLTag::kCaption},
    {"center", HTMLTag::kCenter},
    {"cite", HTMLTag::kCite},
    {"code", HTMLTag::kCode},
    {"col", HTMLTag::kCol},
    {"colgroup", HTMLTag::kColgroup},

    {"data", HTMLTag::kData},
    {"datalist", HTMLTag::kDatalist},
    {"dd", HTMLTag::kDd},
    {"del", HTMLTag::kDel},
    {"details", HTMLTag::kDetails},
    {"dfn", HTMLTag::kDfn},
    {"dialog", HTMLTag::kDialog},
    {"dir", HTMLTag::kDir},
    {"div", HTMLTag::kDiv},
    {"dl", HTMLTag::kDl},
    {"dt", HTMLTag::kDt},

    {"em", HTMLTag::kEm},
    {"embed", HTMLTag::kEmbed},

    {"fieldset", HTMLTag::kFieldset},
    {"figcaption", HTMLTag::kFigcaption},
    {"figure", HTMLTag::kFigure},
    {"font", HTMLTag::kFont},
    {"footer", HTMLTag::kFooter},
    {"form", HTMLTag::kForm},
    {"frame", HTMLTag::kFrame},
    {"frameset", HTMLTag::kFrameset},

    {"h1", HTMLTag::kH1},
    {"h2", HTMLTag::kH2},
    {"h3", HTMLTag::kH3},
    {"h4", HTMLTag::kH4},
    {"h5", HTMLTag::kH5},
    {"h6", HTMLTag::kH6},
    {"head", HTMLTag::kHead},
    {"header", HTMLTag::kHeader},
    {"hgroup", HTMLTag::kHgroup},
    {"hr", HTMLTag::kHr},
    {"html", HTMLTag::kHTML},

    {"i", HTMLTag::kI},
    {"iframe", HTMLTag::kIFrame},
    {"image", HTMLTag::kImage},
    {"img", HTMLTag::kImg},
    {"input", HTMLTag::kInput},
    {"ins", HTMLTag::kIns},

    {"kbd", HTMLTag::kKbd},
    {"keygen", HTMLTag::kKeygen},

    {"label", HTMLTag::kLabel},
    {"legend", HTMLTag::kLegend},
    {"li", HTMLTag::kLi},
    {"link", HTMLTag::kLink},
    {"listing", HTMLTag::kListing},

    {"main", HTMLTag::kMain},
    {"map", HTMLTag::kMap},
    {"mark", HTMLTag::kMark},
    {"marquee", HTMLTag::kMarquee},
    {"menu", HTMLTag::kMenu},
    {"meta", HTMLTag::kMeta},
    {"meter", HTMLTag::kMeter},

    {"nav", HTMLTag::kNav},
    {"nobr", HTMLTag::kNobr},
    {"noembed", HTMLTag::kNoembed},
    {"noframes", HTMLTag::kNoframes},
    {"noscript", HTMLTag::kNoscript},

    {"object", HTMLTag::kObject},
    {"ol", HTMLTag::kOl},
    {"optgroup", HTMLTag::kOptgroup},
    {"option", HTMLTag::kOption},
    {"output", HTMLTag::kOutput},

    {"p", HTMLTag::kP},
    {"param", HTMLTag::kParam},
    {"picture", HTMLTag::kPicture},
    {"plaintext", HTMLTag::kPlaintext},
    {"pre", HTMLTag::kPre},
    {"progress", HTMLTag::kProgress},

    {"q", HTMLTag::kQ},

    {"rb", HTMLTag::kRb},
    {"rp", HTMLTag::kRp},
    {"rt", HTMLTag::kRt},
    {"rtc", HTMLTag::kRTC},
    {"ruby", HTMLTag::kRuby},

    {"s", HTMLTag::kS},
    {"samp", HTMLTag::kSamp},
    {"script", HTMLTag::kScript},
    {"search", HTMLTag::kSearch},
    {"section", HTMLTag::kSection},
    {"select", HTMLTag::kSelect},
    {"slot", HTMLTag::kSlot},
    {"small", HTMLTag::kSmall},
    {"source", HTMLTag::kSource},
    {"span", HTMLTag::kSpan},
    {"strike", HTMLTag::kStrike},
    {"strong", HTMLTag::kStrong},
    {"style", HTMLTag::kStyle},
    {"sub", HTMLTag::kSub},
    {"summary", HTMLTag::kSummary},
    {"sup", HTMLTag::kSup},

    {"table", HTMLTag::kTable},
    {"tbody", HTMLTag::kTbody},
    {"td", HTMLTag::kTd},
    {"template", HTMLTag::kTemplate},
    {"textarea", HTMLTag::kTextarea},
    {"tfoot", HTMLTag::kTfoot},
    {"th", HTMLTag::kTh},
    {"thead", HTMLTag::kThead},
    {"time", HTMLTag::kTime},
    {"title", HTMLTag::kTitle},
    {"tr", HTMLTag::kTr},
    {"track", HTMLTag::kTrack},
    {"tt", HTMLTag::kTt},

    {"u", HTMLTag::kU},
    {"ul", HTMLTag::kUl},

    {"var", HTMLTag::kVar},
    {"video", HTMLTag::kVideo},

    {"wbr", HTMLTag::kWbr},

    {"xmp", HTMLTag::kXmp},
});

// Разделить на готовый токен и неготовый
//
class HTMLToken {
public:
  enum class TokenType {
    kUninitialized,
    kDOCTYPE,
    kStartTag,
    kEndTag,
    kComment,
    kCharacter,
    kEndOfFile,
    kProcessingInstruction
  };

  void SetType(TokenType type);
  void SetFinished(bool is_finished);
  void SetIsSelfClosing(bool is_self_closing);

  void AppendCharacter(char character);

  void AppendToTagName(char character);
  void AppendToTagName(std::string_view character);

  void AppendToAttributeName(char character);
  void AppendToAttributeName(std::string_view character);

  void AppendToAttributeValue(char character);
  void AppendToAttributeValue(std::string_view character);

  void BeginAttribute();

  HTMLTag Tag() const;
  TokenType Type() const;

  bool IsUninitialized() const;
  bool IsFinished() const;

  std::string ToString() const;

private:
  std::string name_;
  std::string text_;

  std::vector<HTMLAttribute> attributes_;

  TokenType type_ = TokenType::kUninitialized;
  HTMLTag tag_ = HTMLTag::kUnknown;

  bool is_finished_ = false;
  bool is_self_closing_ = false;
};

} // namespace webplatform
} // namespace ve
