#include "Script/Parser/ScriptParser.h"
#include "Script/Lexer/ScriptLexer.h"
#include "Script/Lexer/ScriptToken.h"
#include "Script/AST/Root.h"
#include "Script/AST/Statements/LetStatement.h"
#include "Script/AST/Statements/ReturnStatement.h"
#include "Script/AST/Statements/ExpressionStatement.h"
#include "Script/AST/Statements/BlockStatement.h"
#include "Script/AST/Statements/FunctionStatement.h"
#include "Script/AST/Statements/ForStatement.h"
#include "Script/AST/Statements/ClassStatement.h"
#include "Script/AST/Statements/BreakStatement.h"
#include "Script/AST/Statements/ContinueStatement.h"
#include "Script/AST/Expressions/Identifier.h"
#include "Script/AST/Expressions/IntegerLiteral.h"
#include "Script/AST/Expressions/StringLiteral.h"
#include "Script/AST/Expressions/BooleanLiteral.h"
#include "Script/AST/Expressions/NullLiteral.h"
#include "Script/AST/Expressions/ArrayLiteral.h"
#include "Script/AST/Expressions/UserObjectLiteral.h"
#include "Script/AST/Expressions/HashLiteral.h"
#include "Script/AST/Expressions/IfExpression.h"
#include "Script/AST/Expressions/WhileExpression.h"
#include "Script/AST/Expressions/FunctionLiteral.h"
#include "Script/AST/Expressions/PrefixExpression.h"
#include "Script/AST/Expressions/InfixExpression.h"
#include "Script/AST/Expressions/CallExpression.h"
#include "Script/AST/Expressions/IndexExpression.h"
#include "Script/AST/Expressions/AssignmentExpression.h"
#include "Script/AST/Expressions/IndexAssignmentExpression.h"
#include "Script/AST/Expressions/PropertyAccessExpression.h"
#include <cassert>
#include <unordered_map>

namespace tsumugi::script::parser {

const std::unordered_map<tsumugi::script::lexer::TokenType, Precedence> Parser::Precedences = {
    { lexer::TokenType::kAssign, Precedence::kAssign },
    { lexer::TokenType::kEqual, Precedence::kEquals },
    { lexer::TokenType::kNotEqual, Precedence::kEquals },
    { lexer::TokenType::kLessThan, Precedence::kLessgreater },
    { lexer::TokenType::kGreaterThan, Precedence::kLessgreater },
    { lexer::TokenType::kLessThanOrEqual, Precedence::kLessgreater },
    { lexer::TokenType::kGreaterThanOrEqual, Precedence::kLessgreater },
    { lexer::TokenType::kPlus, Precedence::kSum },
    { lexer::TokenType::kMinus, Precedence::kSum },
    { lexer::TokenType::kSlash, Precedence::kProduct },
    { lexer::TokenType::kAsterisk, Precedence::kProduct },
    { lexer::TokenType::kLeftParenthesis, Precedence::kCall },
    { lexer::TokenType::kLeftBrackets, Precedence::kCall },
    { lexer::TokenType::kDot, Precedence::kCall },
};

Parser::Parser(lexer::Lexer* lexer)
 : logger_()
 , localizer()
 , currentToken_()
 , nextToken_()
 , lexer_(lexer) {

    assert(lexer != nullptr);

    // 2つ分のトークンを読み込んでセットしておく
    currentToken_ = std::shared_ptr<lexer::Token>(lexer_->NextToken());
    nextToken_ = std::shared_ptr<lexer::Token>(lexer_->NextToken());

    RegisterPrefixParseFunctions();
    RegisterInfixParseFunctions();
}

Parser::~Parser() = default;

void Parser::ReadToken() {

    currentToken_ = nextToken_;
    nextToken_.reset();
    nextToken_ = std::shared_ptr<lexer::Token>(lexer_->NextToken());
}

ast::Root* Parser::ParseRoot() {

    return nullptr;
}

std::unique_ptr<ast::IStatement> Parser::ParseStatement() {

    switch (currentToken_.get()->GetTokenType()) {
    case lexer::TokenType::kLet:
        return ParseLetStatement();
    case lexer::TokenType::kClass:
        return ParseClassStatement();
    case lexer::TokenType::kReturn :
        return ParseReturnStatement();
    case lexer::TokenType::kFor:
        return ParseForStatement();
    case lexer::TokenType::kBreak:
        return ParseBreakStatement();
    case lexer::TokenType::kContinue:
        return ParseContinueStatement();
    // 既存の仕様と競合するので関数宣言はいったん閉じる
    //case lexer::TokenType::kFunction:
    //    return ParseFunctionStatement();
    // 識別子が続く場合のみ関数宣言とみなせるのでここで分岐してみる
    case lexer::TokenType::kFunction:
        if (PeekTokenIs(lexer::TokenType::kIdentifier)) {
            return Parser::ParseFunctionStatement();
        }
        return ParseExpressionStatement();
    default:
        return ParseExpressionStatement();
    }
}

std::unique_ptr<ast::statement::LetStatement> Parser::ParseLetStatement() {

    auto statement = std::make_unique<ast::statement::LetStatement>();
    statement->SetToken(currentToken_);

    if (!ExpectPeekRequiredTokenType(lexer::TokenType::kIdentifier, "Identifier")) {
        return nullptr;
    }

    auto name = std::make_unique<ast::expression::Identifier>(currentToken_, currentToken_->GetLiteral());
    statement->SetName(std::move(name));

    // 次は = (等号)がある
    if (!ExpectPeekRequiredTokenType(lexer::TokenType::kAssign, "=")) {
        return nullptr;
    }
    // = を読み飛ばす
    ReadToken();

    auto expression = ParseExpression(Precedence::kLowest);
    if (!expression) return nullptr;
    statement->SetValue(std::move(expression));

    // セミコロンは必須ではない
    if (PeekTokenIs(lexer::TokenType::kSemicolon)) {
        ReadToken();
    }

    return statement;
}

std::unique_ptr<ast::statement::ReturnStatement> Parser::ParseReturnStatement() {

    auto statement = std::make_unique<ast::statement::ReturnStatement> ();
    statement->SetToken(currentToken_);

    ReadToken();

    auto expression = ParseExpression(Precedence::kLowest);
    statement->SetValue(std::move(expression));

    // セミコロンは必須ではない
    if (PeekTokenIs(lexer::TokenType::kSemicolon)) {
        ReadToken();
    }

    return statement;
}

std::unique_ptr<ast::statement::ExpressionStatement> Parser::ParseExpressionStatement() {

    auto statement = std::make_unique<ast::statement::ExpressionStatement>();
    statement->SetToken(currentToken_);

    auto expression = ParseExpression(Precedence::kLowest);
    if (!expression) return nullptr;
    statement->SetExpression(std::move(expression));

    // セミコロンは必須ではない
    if (PeekTokenIs(lexer::TokenType::kSemicolon)) {
        ReadToken();
    }

    return statement;
}

std::shared_ptr<script::ast::statement::BlockStatement> Parser::ParseBlockStatement() {

    auto block = std::make_shared<ast::statement::BlockStatement>(currentToken_);

    // "{" を読み飛ばす
    ReadToken();

    while (
        currentToken_->GetTokenType() != lexer::TokenType::kRightBraces &&
        currentToken_->GetTokenType() != lexer::TokenType::kEOF) {

        auto statement = ParseStatement();
        if (statement) {
            block->AddStatement(std::move(statement));
        }
        ReadToken();
    }

    return block;
}

std::unique_ptr<script::ast::statement::FunctionStatement> Parser::ParseFunctionStatement() {

    auto statement = std::make_unique<ast::statement::FunctionStatement>(currentToken_);

    if (!ExpectPeek(lexer::TokenType::kIdentifier)) {
        return nullptr;
    }

    auto name = std::make_unique<ast::expression::Identifier>(currentToken_, currentToken_->GetLiteral());
    statement->SetName(std::move(name));

    if (!ExpectPeekRequiredTokenType(lexer::TokenType::kLeftParenthesis, "(")) {
        return nullptr;
    }

    std::vector<std::shared_ptr<ast::expression::Identifier>> parameters;
    if (!ParseParameters(parameters)) {
        return nullptr;
    }
    for (auto& parameter : parameters) {
        statement->AddParameter(std::move(parameter));
    }

    if (!ExpectPeekRequiredTokenType(lexer::TokenType::kLeftBraces, "{")) {
        return nullptr;
    }

    auto block = ParseBlockStatement();
    statement->SetBody(std::move(block));

    return statement;
}

std::unique_ptr<script::ast::statement::ForStatement> Parser::ParseForStatement() {

    auto statement = std::make_unique<ast::statement::ForStatement>(currentToken_);

    if (!ExpectPeekRequiredTokenType(lexer::TokenType::kLeftParenthesis, "(")) {
        return nullptr;
    }
    if (!ExpectPeekRequiredTokenType(lexer::TokenType::kIdentifier, "Identifier")) {
        return nullptr;
    }

    auto iden = std::make_unique<ast::expression::Identifier>(currentToken_, currentToken_->GetLiteral());
    statement->SetIdentifier(std::move(iden));

    if (!ExpectPeekRequiredTokenType(lexer::TokenType::kIn, "in")) {
        return nullptr;
    }
    ReadToken();

    auto expression = ParseExpression(Precedence::kLowest);
    statement->SetIterable(std::move(expression));

    if (!ExpectPeekRequiredTokenType(lexer::TokenType::kRightParenthesis, ")")) {
        return nullptr;
    }
    ReadToken();

    auto block = ParseBlockStatement();
    statement->SetBody(std::move(block));

    return statement;
}

std::unique_ptr<script::ast::statement::ClassStatement> Parser::ParseClassStatement() {

    auto statement = std::make_unique<ast::statement::ClassStatement>(currentToken_);

    if (!ExpectPeekRequiredTokenType(lexer::TokenType::kIdentifier, "Identifier")) {
        return nullptr;
    }

    auto name = std::make_unique<ast::expression::Identifier>(currentToken_, currentToken_->GetLiteral());
    statement->SetName(std::move(name));

    if (!ExpectPeekRequiredTokenType(lexer::TokenType::kLeftBraces, "{")) {
        return nullptr;
    }

    while (!PeekTokenIs(lexer::TokenType::kRightBraces) && !PeekTokenIs(lexer::TokenType::kEOF)) {
        auto method = ParseFunctionStatement();
        if (method != nullptr) {
            statement->AddMethod(std::move(method));
        }
    };

    // } を消費
    if (currentToken_->GetTokenType() == lexer::TokenType::kRightBraces) {
        ReadToken();
    }

    // セミコロンは必須ではない
    if (PeekTokenIs(lexer::TokenType::kSemicolon)) {
        ReadToken();
    }

    return statement;
}

std::unique_ptr<script::ast::statement::BreakStatement> Parser::ParseBreakStatement() {

    auto statement = std::make_unique<ast::statement::BreakStatement>(currentToken_);
    ReadToken();

    // セミコロンは必須ではない
    if (PeekTokenIs(lexer::TokenType::kSemicolon)) {
        ReadToken();
    }

    return statement;
}

std::unique_ptr<script::ast::statement::ContinueStatement> Parser::ParseContinueStatement() {

    auto statement = std::make_unique<ast::statement::ContinueStatement>(currentToken_);
    ReadToken();

    // セミコロンは必須ではない
    if (PeekTokenIs(lexer::TokenType::kSemicolon)) {
        ReadToken();
    }

    return statement;
}

std::unique_ptr<script::ast::IExpression> Parser::ParseExpression(const Precedence precedence) {

    std::unique_ptr<ast::IExpression> left;
    if (auto it = prefixParseFunctions_.find(currentToken_->GetTokenType()); it != prefixParseFunctions_.end()) {
        auto [keyFound, value] = *it;
        left = value();
    }
    else {
        std::unordered_map<std::string, std::string> placeholders = {
            {"0", lexer::TokenTypeToString(currentToken_->GetTokenType())},
        };
        logger_.Log(log::TextLogger::Categories::Error, localizer.GetMessage(i18n::MessageId::kPrefixParseFunctionMissing, placeholders));
        return nullptr;
    }

    while (nextToken_->GetTokenType() != lexer::TokenType::kSemicolon && precedence < GetNextPrecedence()) {

        if (auto it = infixParseFunctions_.find(nextToken_->GetTokenType()); it != infixParseFunctions_.end()) {
            auto [keyFound, value] = *it;
            ReadToken(); 
            left = it->second(std::move(left));
        }
        else {
            return left;
        }
    }

    return left;
}

std::unique_ptr<ast::IExpression> Parser::ParseIdentifier() {

    return std::make_unique<ast::expression::Identifier>(currentToken_, currentToken_->GetLiteral());
}

std::unique_ptr<script::ast::IExpression> Parser::ParseIntegerLiteral() {

    int result = 0;
    if (type::convert::FromChars(currentToken_->GetLiteral(), result)) {
        return std::make_unique<ast::expression::IntegerLiteral>(currentToken_, result);
    } else {
        std::unordered_map<std::string, tstring> placeholders = {
            {"0", currentToken_->GetLiteral()},
            {"1", tstring(TT("Integer"))}
        };
        logger_.Log(log::TextLogger::Categories::Error, localizer.GetMessage(i18n::MessageId::kConversionFailed, placeholders));
        return nullptr;
    }
}

std::unique_ptr<script::ast::IExpression> Parser::ParseStringLiteral() {

    return std::make_unique<ast::expression::StringLiteral>(currentToken_, currentToken_->GetLiteral());
}

std::unique_ptr<script::ast::IExpression> Parser::ParseBooleanLiteral() {

    return std::make_unique<ast::expression::BooleanLiteral>(currentToken_, currentToken_->GetTokenType() == lexer::TokenType::kTrue);
}

std::unique_ptr<script::ast::IExpression> Parser::ParseNullLiteral() {

    return std::make_unique<ast::expression::NullLiteral>(currentToken_);
}

std::unique_ptr<script::ast::IExpression> Parser::ParseArrayLiteral() {

    std::vector<std::unique_ptr<ast::IExpression>> list;

    // [ を読み飛ばす
    ReadToken();

    // 空の配列の場合
    if (currentToken_->GetTokenType() == lexer::TokenType::kRightBrackets) {
        return std::make_unique<ast::expression::ArrayLiteral>(currentToken_, std::move(list));
    }

    auto parameter = ParseExpression(parser::Precedence::kLowest);
    if (parameter == nullptr) {
        return nullptr;
    }
    list.push_back(std::move(parameter));

    // カンマが続く限り読み続ける
    while (nextToken_->GetTokenType() == lexer::TokenType::kComma) {

        // カンマか読み込み済み識別子をスキップ
        ReadToken();
        ReadToken();

        parameter = ParseExpression(parser::Precedence::kLowest);
        if (parameter == nullptr) {
            return nullptr;
        }
        list.push_back(std::move(parameter));
    }

    if (!ExpectPeekRequiredTokenType(lexer::TokenType::kRightBrackets, "]")) {
        return nullptr;
    }

    return std::make_unique<ast::expression::ArrayLiteral>(currentToken_, std::move(list));
}

std::unique_ptr<script::ast::IExpression> Parser::ParseUserObjectLiteral() {

    std::vector<std::pair<std::unique_ptr<ast::IExpression>, std::unique_ptr<ast::IExpression>>> pairs;

    while (!PeekTokenIs(lexer::TokenType::kRightBraces) && !PeekTokenIs(lexer::TokenType::kEOF)) {

        ReadToken(); // key
        auto keyExpression = ParseExpression(Precedence::kLowest);

        std::unique_ptr<ast::expression::StringLiteral> key;

        // key は必ず StringLiteral にする
        switch (keyExpression->GetNodeType()) {
            case ast::NodeType::kIdentifier: {
                auto identifier = static_cast<ast::expression::Identifier*>(keyExpression.get());
                key = std::make_unique<ast::expression::StringLiteral>(identifier->GetTokenShared(), identifier->GetValue());
                break;
            }
            case ast::NodeType::kStringLiteral: {
                auto stringLiteral = static_cast<ast::expression::StringLiteral*>(keyExpression.get());
                key = std::make_unique<ast::expression::StringLiteral>(stringLiteral->GetTokenShared(), stringLiteral->GetValue());
                break;
            }
            default: {
                while (!(GetCurrentToken()->GetTokenType() == lexer::TokenType::kRightBraces) &&
                       !(GetCurrentToken()->GetTokenType() == lexer::TokenType::kEOF)) {
                    ReadToken();
                }
                // セミコロンは必須ではない
                if (PeekTokenIs(lexer::TokenType::kSemicolon)) {
                    ReadToken();
                }
                return nullptr;
            }
        }

        if (!ExpectPeekRequiredTokenType(lexer::TokenType::kColon, ":")) {
            return nullptr;
        }

        ReadToken(); // value
        auto value = ParseExpression(Precedence::kLowest);

        pairs.push_back({ std::move(key), std::move(value) });

        if (!PeekTokenIs(lexer::TokenType::kRightBraces)) {
            if (!ExpectPeekRequiredTokenType(lexer::TokenType::kComma, ",")) {
                return nullptr;
            }
        }
    }

    if (!ExpectPeekRequiredTokenType(lexer::TokenType::kRightBraces, "}")) {
        return nullptr;
    }

    return std::make_unique<ast::expression::UserObjectLiteral>(currentToken_, std::move(pairs));
}

std::unique_ptr<script::ast::IExpression> Parser::ParseHashLiteral() {

    if (!ExpectPeekRequiredTokenType(lexer::TokenType::kLeftBraces, "{")) {
        return nullptr;
    }

    std::vector<std::pair<std::unique_ptr<ast::IExpression>, std::unique_ptr<ast::IExpression>>> pairs;

    while (!PeekTokenIs(lexer::TokenType::kRightBraces) && !PeekTokenIs(lexer::TokenType::kEOF)) {

        ReadToken(); // key
        auto key = ParseExpression(Precedence::kLowest);

        if (!ExpectPeekRequiredTokenType(lexer::TokenType::kColon, ":")) {
            return nullptr;
        }

        ReadToken(); // value
        auto value = ParseExpression(Precedence::kLowest);

        pairs.push_back({ std::move(key), std::move(value) });

        if (!PeekTokenIs(lexer::TokenType::kRightBraces)) {
            if (!ExpectPeekRequiredTokenType(lexer::TokenType::kComma, ",")) {
                return nullptr;
            }
        }
    }

    if (!ExpectPeekRequiredTokenType(lexer::TokenType::kRightBraces, "}")) {
        return nullptr;
    }

    return std::make_unique<ast::expression::HashLiteral>(currentToken_, std::move(pairs));
}

std::unique_ptr<script::ast::IExpression> Parser::ParseGroupedExpression() {

    // "(" を読み飛ばす
    ReadToken();

    // 括弧内の式を解析する
    auto expression = ParseExpression(Precedence::kLowest);

    // 閉じ括弧 ")" がないとエラー
    if (!ExpectPeekRequiredTokenType(lexer::TokenType::kRightParenthesis, ")")) {
        return nullptr;
    }

    return expression;
}

std::unique_ptr<script::ast::IExpression> Parser::ParseIfExpression() {

    auto expression = std::make_unique<ast::expression::IfExpression>(currentToken_);

    if (!ExpectPeekRequiredTokenType(lexer::TokenType::kLeftParenthesis, "(")) {
        return nullptr;
    }

    ReadToken();

    auto condition = ParseExpression(Precedence::kLowest);
    expression->SetCondition(std::move(condition));

    if (!ExpectPeekRequiredTokenType(lexer::TokenType::kRightParenthesis, ")")) {
        return nullptr;
    }
    if (!ExpectPeekRequiredTokenType(lexer::TokenType::kLeftBraces, "{")) {
        return nullptr;
    }

    // ブロック文の解析
    auto blockif = ParseBlockStatement();
    expression->SetConsequence(std::move(blockif));

    if (nextToken_->GetTokenType() == lexer::TokenType::kElse) {
        
        ReadToken();

        if (!ExpectPeekRequiredTokenType(lexer::TokenType::kLeftBraces, "{")) {
            return nullptr;
        }

        auto blockelse = ParseBlockStatement();
        expression->SetAlternative(std::move(blockelse));
    }

    return expression;
}

std::unique_ptr<script::ast::IExpression> Parser::ParseWhileExpression() {

    auto expression = std::make_unique<ast::expression::WhileExpression>(currentToken_);

    if (!ExpectPeekRequiredTokenType(lexer::TokenType::kLeftParenthesis, "(")) {
        return nullptr;
    }

    ReadToken();

    auto condition = ParseExpression(Precedence::kLowest);
    expression->SetCondition(std::move(condition));

    if (!ExpectPeekRequiredTokenType(lexer::TokenType::kRightParenthesis, ")")) {
        return nullptr;
    }
    if (!ExpectPeekRequiredTokenType(lexer::TokenType::kLeftBraces, "{")) {
        return nullptr;
    }

    // ブロック文の解析
    auto block = ParseBlockStatement();
    expression->SetBody(std::move(block));

    return expression;
}

std::unique_ptr<script::ast::IExpression> Parser::ParseFunctionLiteral() {

    auto expression = std::make_unique<ast::expression::FunctionLiteral>(currentToken_);

    if (!ExpectPeekRequiredTokenType(lexer::TokenType::kLeftParenthesis, "(")) {
        return nullptr;
    }

    std::vector<std::shared_ptr<tsumugi::script::ast::expression::Identifier>> parameters;
    if (!ParseParameters(parameters)) {
        return nullptr;
    }
    for (auto& parameter : parameters) {
        expression->AddParameter(std::move(parameter));
    }

    if (!ExpectPeekRequiredTokenType(lexer::TokenType::kLeftBraces, "{")) {
        return nullptr;
    }

    auto block = ParseBlockStatement();
    expression->SetBody(std::move(block));

    return expression;
}

std::unique_ptr<script::ast::IExpression> Parser::ParsePrefixExpression() {

    auto expression = std::make_unique<ast::expression::PrefixExpression>(currentToken_, currentToken_->GetLiteral());

    ReadToken();

    auto right = ParseExpression(Precedence::kPrefix);
    expression->SetRight(std::move(right));

    return expression;
}

std::unique_ptr<script::ast::IExpression> Parser::ParseInfixExpression(std::unique_ptr<tsumugi::script::ast::IExpression> left) {

    auto expression = std::make_unique<ast::expression::InfixExpression>(currentToken_, currentToken_->GetLiteral(), std::move(left));

    const auto currentPrecedence = GetCurrentPrecedence();
    ReadToken();

    auto right = ParseExpression(currentPrecedence);
    expression->SetRight(std::move(right));

    return expression;
}

std::unique_ptr<script::ast::IExpression> Parser::ParseCallExpression(std::unique_ptr<tsumugi::script::ast::IExpression> function) {

    auto expression = std::make_unique<ast::expression::CallExpression>(currentToken_);
    expression->SetFunction(std::move(function));
    
    std::vector<std::unique_ptr<tsumugi::script::ast::IExpression>> arguments;
    if (!ParseCallArguments(arguments)) {
        return nullptr;
    }
    for (auto& argument : arguments) {
        expression->AddArgument(std::move(argument));
    }

    return expression;
}

std::unique_ptr<script::ast::IExpression> Parser::ParseIndexExpression(std::unique_ptr<tsumugi::script::ast::IExpression> left) {

    // [ を読み飛ばす
    ReadToken();

    auto index = ParseExpression(parser::Precedence::kLowest);

    if (!ExpectPeekRequiredTokenType(lexer::TokenType::kRightBrackets, "]")) {
        return nullptr;
    }

    // インデックスへの代入を判定
    if (PeekTokenIs(lexer::TokenType::kAssign)) {
        ReadToken(); // ] を読み飛ばす
        ReadToken(); // = を読み飛ばす
        auto value = ParseExpression(parser::Precedence::kLowest);
        return std::make_unique<ast::expression::IndexAssignmentExpression>(currentToken_, std::move(left), std::move(index), std::move(value));
    }

    return std::make_unique<ast::expression::IndexExpression>(currentToken_, std::move(left), std::move(index));
}

std::unique_ptr<script::ast::IExpression> Parser::ParseAssignmentExpression(std::unique_ptr<tsumugi::script::ast::IExpression> left) {

    if (left->GetNodeType() != ast::NodeType::kIdentifier &&
        left->GetNodeType() != ast::NodeType::kPropertyAccessExpression) {
        std::unordered_map<std::string, std::string> placeholders = {
            {"0", type::convert::TStringToString(left->ToCode())}
        };
        logger_.Log(log::TextLogger::Categories::Error, localizer.GetMessage(i18n::MessageId::kInvalidAssignmentTarget, placeholders));
    }

    auto assignmentExpression = std::make_unique<ast::expression::AssignmentExpression>(currentToken_, std::move(left));

    ReadToken();
    auto value = ParseExpression(parser::Precedence::kLowest);
    assignmentExpression->SetValue(std::move(value));

    return assignmentExpression;
}

std::unique_ptr<script::ast::IExpression> Parser::ParsePropertyAccessExpression(std::unique_ptr<script::ast::IExpression> left) {

    auto expression = std::make_unique<ast::expression::PropertyAccessExpression>(currentToken_, std::move(left));

    if (!ExpectPeekRequiredTokenType(lexer::TokenType::kIdentifier, "Identifier")) {
        return nullptr;
    }
    auto name = std::make_unique<ast::expression::Identifier>(currentToken_, currentToken_->GetLiteral());
    expression->SetName(std::move(name));

    // 次のトークンが '=' なら代入式
    if (PeekTokenIs(lexer::TokenType::kAssign)) {
        ReadToken(); // '='
        ReadToken(); // value の先頭へ
        auto value = ParseExpression(Precedence::kLowest);
        auto assignmentExpression = std::make_unique<ast::expression::AssignmentExpression>(currentToken_, std::move(expression));
        assignmentExpression->SetValue(std::move(value));
        return assignmentExpression;
    }

    return expression;
}

bool Parser::ParseParameters(std::vector<std::shared_ptr<tsumugi::script::ast::expression::Identifier>>& parameters) {

    parameters.clear();

    // 引数なしの関数の場合
    if (nextToken_->GetTokenType() == lexer::TokenType::kRightParenthesis) {
        ReadToken(); 
        return true;
    }
    ReadToken();

    auto first = std::make_shared<ast::expression::Identifier>(currentToken_, currentToken_->GetLiteral());
    parameters.push_back(first);

    while (nextToken_->GetTokenType() == lexer::TokenType::kComma) {

        // カンマか読み込み済み識別子をスキップ
        ReadToken();
        ReadToken();

        auto param = std::make_shared<ast::expression::Identifier>(currentToken_, currentToken_->GetLiteral());
        parameters.push_back(param);
    }

    if (!ExpectPeekRequiredTokenType(lexer::TokenType::kRightParenthesis, ")")) {
        parameters.clear(); 
        return false;
    }

    return true;
}

bool Parser::ParseCallArguments(std::vector<std::unique_ptr<tsumugi::script::ast::IExpression>>& arguments) {

    arguments.clear();

    // ( を読み飛ばす
    ReadToken();

    // 引数なしの関数呼び出しの場合
    if (currentToken_->GetTokenType() == lexer::TokenType::kRightParenthesis) {
        return true;
    }

    // 一つ目の引数を解析
    auto first_argument = ParseExpression(parser::Precedence::kLowest);
    if (first_argument == nullptr) {
        return false;
    }
    arguments.push_back(std::move(first_argument));

    // 2つ目以降の引数があればそれを解析
    while (nextToken_->GetTokenType() == lexer::TokenType::kComma) {

        // カンマか読み込み済み識別子をスキップ
        ReadToken();
        ReadToken();

        auto argument = ParseExpression(parser::Precedence::kLowest);
        if (argument == nullptr) {
            return false;
        }
        arguments.push_back(std::move(argument));
    }

    if (!ExpectPeekRequiredTokenType(lexer::TokenType::kRightParenthesis, ")")) {
        arguments.clear();
        return false;
    }

    return true;
}

std::unique_ptr<ast::Root> Parser::ParseProgram() {

    auto root = std::make_unique<ast::Root>();
    while (currentToken_.get()->GetTokenType() != lexer::TokenType::kEOF) {
        if (auto statement = ParseStatement()) {
            root->AddStatement(std::move(statement));
        }
        ReadToken();
    }
    return root;
}

bool Parser::PeekTokenIs(const lexer::TokenType& type) {

    if (nextToken_->GetTokenType() == type) {
        return true;
    }
    return false;
}

bool Parser::ExpectPeek(const lexer::TokenType& type) {

    if (nextToken_->GetTokenType() == type) {
        ReadToken();
        return true;
    }
    std::unordered_map<std::string, std::string> placeholders = {
        {"0", lexer::TokenTypeToString(nextToken_->GetTokenType())},
        {"1", lexer::TokenTypeToString(type)}
    };
    logger_.Log(log::TextLogger::Categories::Error, localizer.GetMessage(i18n::MessageId::kInvalidInputFormat, placeholders));
    return false;
}

const Precedence Parser::GetCurrentPrecedence() const {

    if (auto it = Parser::Precedences.find(currentToken_->GetTokenType()); it != Parser::Precedences.end()) {
        auto [keyFound, value] = *it;
        return value;
    }
    else {
        return Precedence::kLowest;
    }
}

const Precedence Parser::GetNextPrecedence() const {

    if (auto it = Parser::Precedences.find(nextToken_->GetTokenType()); it != Parser::Precedences.end()) {
        auto [keyFound, value] = *it;
        return value;
    }
    else {
        return Precedence::kLowest;
    }
}

void Parser::RegisterPrefixParseFunctions() {

    prefixParseFunctions_.emplace(lexer::TokenType::kIdentifier, [this] { return ParseIdentifier(); });
    prefixParseFunctions_.emplace(lexer::TokenType::kInteger, [this] { return ParseIntegerLiteral(); });
    prefixParseFunctions_.emplace(lexer::TokenType::kString, [this] { return ParseStringLiteral(); });
    prefixParseFunctions_.emplace(lexer::TokenType::kBang, [this] { return ParsePrefixExpression(); });
    prefixParseFunctions_.emplace(lexer::TokenType::kMinus, [this] { return ParsePrefixExpression(); });
    prefixParseFunctions_.emplace(lexer::TokenType::kTrue, [this] { return ParseBooleanLiteral(); });
    prefixParseFunctions_.emplace(lexer::TokenType::kFalse, [this] { return ParseBooleanLiteral(); });
    prefixParseFunctions_.emplace(lexer::TokenType::kLeftParenthesis, [this] { return ParseGroupedExpression(); });
    prefixParseFunctions_.emplace(lexer::TokenType::kIf, [this] { return ParseIfExpression(); });
    prefixParseFunctions_.emplace(lexer::TokenType::kWhile, [this] { return ParseWhileExpression(); });
    prefixParseFunctions_.emplace(lexer::TokenType::kFunction, [this] { return ParseFunctionLiteral(); });
    prefixParseFunctions_.emplace(lexer::TokenType::kLeftBrackets, [this] { return ParseArrayLiteral(); });
    prefixParseFunctions_.emplace(lexer::TokenType::kLeftBraces, [this] { return ParseUserObjectLiteral(); });
    prefixParseFunctions_.emplace(lexer::TokenType::kSharp, [this] { return ParseHashLiteral(); });
    prefixParseFunctions_.emplace(lexer::TokenType::kNull, [this] { return ParseNullLiteral(); });
}

void Parser::RegisterInfixParseFunctions() {

    infixParseFunctions_.emplace(lexer::TokenType::kPlus, [this](auto left) { return ParseInfixExpression(std::move(left)); });
    infixParseFunctions_.emplace(lexer::TokenType::kMinus, [this](auto left) { return ParseInfixExpression(std::move(left)); });
    infixParseFunctions_.emplace(lexer::TokenType::kSlash, [this](auto left) { return ParseInfixExpression(std::move(left)); });
    infixParseFunctions_.emplace(lexer::TokenType::kAsterisk, [this](auto left) { return ParseInfixExpression(std::move(left)); });
    infixParseFunctions_.emplace(lexer::TokenType::kEqual, [this](auto left) { return ParseInfixExpression(std::move(left)); });
    infixParseFunctions_.emplace(lexer::TokenType::kNotEqual, [this](auto left) { return ParseInfixExpression(std::move(left)); });
    infixParseFunctions_.emplace(lexer::TokenType::kLessThan, [this](auto left) { return ParseInfixExpression(std::move(left)); });
    infixParseFunctions_.emplace(lexer::TokenType::kGreaterThan, [this](auto left) { return ParseInfixExpression(std::move(left)); });
    infixParseFunctions_.emplace(lexer::TokenType::kLessThanOrEqual, [this](auto left) { return ParseInfixExpression(std::move(left)); });
    infixParseFunctions_.emplace(lexer::TokenType::kGreaterThanOrEqual, [this](auto left) { return ParseInfixExpression(std::move(left)); });
    infixParseFunctions_.emplace(lexer::TokenType::kLeftParenthesis, [this](auto left) { return ParseCallExpression(std::move(left)); });
    infixParseFunctions_.emplace(lexer::TokenType::kLeftBrackets, [this](auto left) { return ParseIndexExpression(std::move(left)); });
    infixParseFunctions_.emplace(lexer::TokenType::kAssign, [this](auto left) { return ParseAssignmentExpression(std::move(left)); });
    infixParseFunctions_.emplace(lexer::TokenType::kDot, [this](auto left) { return ParsePropertyAccessExpression(std::move(left)); });
}

bool Parser::ExpectPeekRequiredTokenType(const tsumugi::script::lexer::TokenType tokenType, const std::string& symbol) {

    if (!ExpectPeek(tokenType)) {
        std::unordered_map<std::string, std::string> placeholders = {
            {"0", symbol}
        };
        logger_.Log(log::TextLogger::Categories::Error, localizer.GetMessage(i18n::MessageId::kRequiredSymbolMissing, placeholders));
        return false;
    }
    return true;
}

}
