#include "html/htmltoken.h"
#include "html/htmltokenizer.h"

#include <gtest/gtest.h>

namespace ve::html {

TEST(HTMLTokenizerTest, TokenizesEmptyInputAsEOF) {
  HTMLTokeniser tokenizer;

  const auto tokens = tokenizer.Tokenize("");

  ASSERT_EQ(tokens.size(), 1);

  EXPECT_EQ(tokens[0].Type(), HTMLToken::TokenType::kEndOfFile);
}

TEST(HTMLTokenizerTest, TokenizesPlainText) {
  HTMLTokeniser tokenizer;

  const auto tokens = tokenizer.Tokenize("Hello VoidEngine");

  ASSERT_EQ(tokens.size(), 2);

  EXPECT_EQ(tokens[0].Type(), HTMLToken::TokenType::kCharacter);
  EXPECT_EQ(tokens[0].text_, "Hello VoidEngine");

  EXPECT_EQ(tokens[1].Type(), HTMLToken::TokenType::kEndOfFile);
}

TEST(HTMLTokenizerTest, TokenizesStartTextAndEndTag) {
  HTMLTokeniser tokenizer;

  const auto tokens = tokenizer.Tokenize("<div>Hello</div>");

  ASSERT_EQ(tokens.size(), 4);

  EXPECT_EQ(tokens[0].Type(), HTMLToken::TokenType::kStartTag);
  EXPECT_EQ(tokens[0].name_, "div");
  EXPECT_EQ(tokens[0].Tag(), HTMLTag::kDiv);

  EXPECT_EQ(tokens[1].Type(), HTMLToken::TokenType::kCharacter);
  EXPECT_EQ(tokens[1].text_, "Hello");

  EXPECT_EQ(tokens[2].Type(), HTMLToken::TokenType::kEndTag);
  EXPECT_EQ(tokens[2].name_, "div");
  EXPECT_EQ(tokens[2].Tag(), HTMLTag::kDiv);

  EXPECT_EQ(tokens[3].Type(), HTMLToken::TokenType::kEndOfFile);
}

TEST(HTMLTokenizerTest, LowercasesTagNames) {
  HTMLTokeniser tokenizer;

  const auto tokens = tokenizer.Tokenize("<DIV></DIV>");

  ASSERT_EQ(tokens.size(), 3);

  EXPECT_EQ(tokens[0].Type(), HTMLToken::TokenType::kStartTag);
  EXPECT_EQ(tokens[0].name_, "div");
  EXPECT_EQ(tokens[0].Tag(), HTMLTag::kDiv);

  EXPECT_EQ(tokens[1].Type(), HTMLToken::TokenType::kEndTag);
  EXPECT_EQ(tokens[1].name_, "div");
  EXPECT_EQ(tokens[1].Tag(), HTMLTag::kDiv);

  EXPECT_EQ(tokens[2].Type(), HTMLToken::TokenType::kEndOfFile);
}

TEST(HTMLTokenizerTest, ParsesQuotedAndUnquotedAttributes) {
  HTMLTokeniser tokenizer;

  const auto tokens =
      tokenizer.Tokenize("<img src=\"image.png\" width='160' height=100>");

  ASSERT_EQ(tokens.size(), 2);

  const HTMLToken &start_tag = tokens[0];

  ASSERT_EQ(start_tag.Type(), HTMLToken::TokenType::kStartTag);
  EXPECT_EQ(start_tag.name_, "img");
  EXPECT_EQ(start_tag.Tag(), HTMLTag::kImg);

  ASSERT_EQ(start_tag.attributes_.size(), 3);

  EXPECT_EQ(start_tag.attributes_[0].name, "src");
  EXPECT_EQ(start_tag.attributes_[0].value, "image.png");

  EXPECT_EQ(start_tag.attributes_[1].name, "width");
  EXPECT_EQ(start_tag.attributes_[1].value, "160");

  EXPECT_EQ(start_tag.attributes_[2].name, "height");
  EXPECT_EQ(start_tag.attributes_[2].value, "100");

  EXPECT_EQ(tokens[1].Type(), HTMLToken::TokenType::kEndOfFile);
}

TEST(HTMLTokenizerTest, LowercasesAttributeNamesButPreservesValues) {
  HTMLTokeniser tokenizer;

  const auto tokens =
      tokenizer.Tokenize("<img SRC=\"Images/Cat.PNG\" WIDTH=\"160\">");

  ASSERT_EQ(tokens.size(), 2);

  const HTMLToken &start_tag = tokens[0];

  ASSERT_EQ(start_tag.Type(), HTMLToken::TokenType::kStartTag);
  ASSERT_EQ(start_tag.attributes_.size(), 2);

  EXPECT_EQ(start_tag.attributes_[0].name, "src");
  EXPECT_EQ(start_tag.attributes_[0].value, "Images/Cat.PNG");

  EXPECT_EQ(start_tag.attributes_[1].name, "width");
  EXPECT_EQ(start_tag.attributes_[1].value, "160");
}

TEST(HTMLTokenizerTest, ParsesSelfClosingStartTag) {
  HTMLTokeniser tokenizer;

  const auto tokens = tokenizer.Tokenize("<img/>");

  ASSERT_EQ(tokens.size(), 2);

  EXPECT_EQ(tokens[0].Type(), HTMLToken::TokenType::kStartTag);
  EXPECT_EQ(tokens[0].name_, "img");
  EXPECT_EQ(tokens[0].Tag(), HTMLTag::kImg);
  EXPECT_TRUE(tokens[0].is_self_closing_);

  EXPECT_EQ(tokens[1].Type(), HTMLToken::TokenType::kEndOfFile);
}

TEST(HTMLTokenizerTest, AcceptsHtmlWhitespaceBetweenAttributes) {
  HTMLTokeniser tokenizer;

  const auto tokens = tokenizer.Tokenize("<img\n"
                                         "\tsrc=\"image.png\"\r\n"
                                         " width=\"160\"\n"
                                         " height=\"100\">");

  ASSERT_EQ(tokens.size(), 2);

  const HTMLToken &start_tag = tokens[0];

  ASSERT_EQ(start_tag.Type(), HTMLToken::TokenType::kStartTag);
  ASSERT_EQ(start_tag.attributes_.size(), 3);

  EXPECT_EQ(start_tag.attributes_[0].name, "src");
  EXPECT_EQ(start_tag.attributes_[0].value, "image.png");

  EXPECT_EQ(start_tag.attributes_[1].name, "width");
  EXPECT_EQ(start_tag.attributes_[1].value, "160");

  EXPECT_EQ(start_tag.attributes_[2].name, "height");
  EXPECT_EQ(start_tag.attributes_[2].value, "100");
}

} // namespace ve::html
