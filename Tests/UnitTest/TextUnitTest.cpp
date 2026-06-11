#include "pch.h"
#include "CppUnitTest.h"
#include "Foundation/Types.h"
#include "Script/AST/Program.h"
#include "Script/AST/Expressions/Identifier.h"
#include "Script/AST/Expressions/IntegerLiteral.h"
#include "Script/AST/Expressions/FloatLiteral.h"
#include "Script/AST/Expressions/BooleanLiteral.h"
#include "Script/AST/Expressions/PrefixExpression.h"
#include "Script/AST/Expressions/IfExpression.h"
#include "Script/AST/Expressions/InfixExpression.h"
#include "Script/AST/Expressions/IndexExpression.h"
#include "Script/AST/Expressions/FunctionLiteral.h"
#include "Script/AST/Expressions/CallExpression.h"
#include "Script/AST/Expressions/ArrayLiteral.h"
#include "Script/AST/Expressions/WhileExpression.h"
#include "Script/AST/Expressions/AssignmentExpression.h"
#include "Script/AST/Expressions/PropertyAccessExpression.h"
#include "Script/AST/Statements/LetStatement.h"
#include "Script/AST/Statements/ReturnStatement.h"
#include "Script/AST/Statements/ExpressionStatement.h"
#include "Script/AST/Statements/BlockStatement.h"
#include "Script/Objects/IntegerObject.h"
#include "Script/Objects/FloatObject.h"
#include "Script/Objects/BooleanObject.h"
#include "Script/Objects/NullObject.h"
#include "Script/Objects/StringObject.h"
#include "Script/Objects/ErrorObject.h"
#include "Script/Objects/ArrayObject.h"
#include "Script/Objects/UserObject.h"
#include "Script/Objects/Environment.h"
#include "Script/Objects/UserFunctionObject.h"
#include "Script/Builtins/Vector/Vector3Instance.h"
#include "Script/Builtins/Vector/Vector2Instance.h"
#include "Script/Builtins/Quaternion/QuaternionInstance.h"
#include "Script/Lexer/ScriptLexer.h"
#include "Script/Lexer/ScriptToken.h"
#include "Script/Parser/ScriptParser.h"
#include "Script/Lexer/LexingStringReader.h"
#include "Script/Evaluator/Evaluator.h"
#include "Log/TextLogger.h"
#include "Text/Lexer/TextLexer.h"
#include "Text/Lexer/TextToken.h"
#include "Text/Parser/TextParser.h"
#include "Text/AST/TextProgram.h"
#include "Text/AST/Statements/LabelStatement.h"
#include "Text/AST/Statements/TagStatement.h"
#include "Text/AST/Statements/TextStatement.h"
#include "Text/Evaluator/TextEvaluator.h"
#include "Text/Context/IGameContext.h"
#include <vector>
#include <variant>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define MSG(msg) [&]{ std::wstringstream _s; _s << msg; return _s.str(); }().c_str()

class TestGameContext : public tsumugi::text::context::IGameContext {
private:
    std::wstringstream m_textBuffer; // 出力されたテキストを保持
    std::vector<tstring> m_executedLogs; // 呼ばれた関数を記録する（オプション）
    bool waiting_ = false;
    tsumugi::text::context::FontState m_fontState;
    tsumugi::text::context::DelayState m_delayState;

public:
    // --- テスト検証用のヘルパーメソッド ---
    tstring GetCapturedText() const { return m_textBuffer.str(); }
    const std::vector<tstring>& GetLogs() const { return m_executedLogs; }

    void ClearCapturedData() {
        m_textBuffer.str(TT(""));
        m_textBuffer.clear();
        m_executedLogs.clear();
    }

    // --- IGameContext の実装 ---
    void ClearText() override { ClearCapturedData(); m_executedLogs.push_back(TT("ClearText")); }

    void ShowText(const tstring& text) override {
        m_textBuffer << text;
        m_executedLogs.push_back(TT("ShowText: ") + text);
    }

    void NewLine() override {
        m_textBuffer << TT("\n");
        m_executedLogs.push_back(TT("NewLine"));
    }
    // --- テスト用：待機状態を強制解除 ---
    void ForceContinue() { waiting_ = false; }

    void PageBreak() override { waiting_ = true; m_executedLogs.push_back(TT("PageBreak")); }
    void WaitForInput() override { waiting_ = true; m_executedLogs.push_back(TT("WaitForClick")); }

    // フォントや速度の状態は、テスト中に落ちないように最低限保持できるようにしておく
    tsumugi::text::context::FontState& GetFontState() override { return m_fontState; }
    void ApplyFontState(const tsumugi::text::context::FontState& state) override { m_fontState = state; }

    tsumugi::text::context::DelayState& GetDelayState() override { return m_delayState; }
    void ApplyDelayState(const tsumugi::text::context::DelayState& state) override { m_delayState = state; }

    // 演出やサウンドなどの「画面に出ないもの」は、ログに残すか空実装でスルー
    void Wait(int ms) override { waiting_ = true;  m_executedLogs.push_back(TT("Wait")); }
    void ShakeScreen(int strength, int time) override {}
    void FadeIn(int time) override {}
    void FadeOut(int time) override {}
    void ShowImage(const tstring& layer, const tstring& path) override {}
    void HideImage(const tstring& layer) override {}
    void MoveImage(const tstring& layer, int x, int y, int time) override {}
    void ClearLayer(const tstring& layer) override {}
    void PlayBGM(const tstring& path, int loop) override {}
    void StopBGM(int time) override {}
    void PlaySE(const tstring& path) override {}
    void StopSE(const tstring& path) override {}
    void Save(int slot) override {}
    void Load(int slot) override {}
    void SetUserFont(const tstring& fontName) override {}
    void SetUserTextSpeed(int ms) override {}
    bool IsWaiting() const override { return waiting_; }
};

namespace UnitTest
{
    TEST_CLASS(TsumugiTextTest)
    {
    public:
        TEST_METHOD(SimpleTextLexer)
        {
            auto input =
                TT("[wait time=200]\n")
                TT("*start | スタート\r\n")
                TT("[cm]\r\n")
                TT("こんにちは。\n");
            tsumugi::text::lexer::Lexer lexer(input);

            std::vector<tsumugi::text::lexer::Token*> testTokens;
            testTokens.push_back(new tsumugi::text::lexer::Token(tsumugi::text::lexer::TokenType::kTagOpen, TT("["), true));
            testTokens.push_back(new tsumugi::text::lexer::Token(tsumugi::text::lexer::TokenType::kString, TT("wait"), false));
            testTokens.push_back(new tsumugi::text::lexer::Token(tsumugi::text::lexer::TokenType::kString, TT("time"), false));
            testTokens.push_back(new tsumugi::text::lexer::Token(tsumugi::text::lexer::TokenType::kAssign, TT("="), false));
            testTokens.push_back(new tsumugi::text::lexer::Token(tsumugi::text::lexer::TokenType::kString, TT("200"), false));
            testTokens.push_back(new tsumugi::text::lexer::Token(tsumugi::text::lexer::TokenType::kTagClose, TT("]"), false));
            testTokens.push_back(new tsumugi::text::lexer::Token(tsumugi::text::lexer::TokenType::kNewLine, TT("\\n"), false));
            testTokens.push_back(new tsumugi::text::lexer::Token(tsumugi::text::lexer::TokenType::kAsterisk, TT("*"), true));
            testTokens.push_back(new tsumugi::text::lexer::Token(tsumugi::text::lexer::TokenType::kString, TT("start"), false));
            testTokens.push_back(new tsumugi::text::lexer::Token(tsumugi::text::lexer::TokenType::kPipe, TT("|"), false));
            testTokens.push_back(new tsumugi::text::lexer::Token(tsumugi::text::lexer::TokenType::kString, TT("スタート"), false));
            testTokens.push_back(new tsumugi::text::lexer::Token(tsumugi::text::lexer::TokenType::kNewLine, TT("\\r\\n"), false));
            testTokens.push_back(new tsumugi::text::lexer::Token(tsumugi::text::lexer::TokenType::kTagOpen, TT("["), true));
            testTokens.push_back(new tsumugi::text::lexer::Token(tsumugi::text::lexer::TokenType::kString, TT("cm"), false));
            testTokens.push_back(new tsumugi::text::lexer::Token(tsumugi::text::lexer::TokenType::kTagClose, TT("]"), false));
            testTokens.push_back(new tsumugi::text::lexer::Token(tsumugi::text::lexer::TokenType::kNewLine, TT("\\r\\n"), false));
            testTokens.push_back(new tsumugi::text::lexer::Token(tsumugi::text::lexer::TokenType::kString, TT("こんにちは。"), true));
            testTokens.push_back(new tsumugi::text::lexer::Token(tsumugi::text::lexer::TokenType::kNewLine, TT("(\\n"), false));

            for (const auto* testToken : testTokens)
            {
                const auto* nextToken = lexer.NextToken();
                Logger::WriteMessage((TT("\nTesting code: ") + testToken->GetLiteral()).c_str());
                Assert::IsNotNull(nextToken);
                Assert::AreEqual(testToken->GetTokenType() == nextToken->GetTokenType(), true,
                    MSG("Expected " << tsumugi::text::lexer::TokenTypeToString(testToken->GetTokenType()) << " Actual:" << tsumugi::text::lexer::TokenTypeToString(nextToken->GetTokenType())));
                //Assert::AreEqual(testToken->GetLiteral().compare(nextToken->GetLiteral()), 0);
                delete nextToken;
            }
            testTokens.clear();
        }

        TEST_METHOD(TextParserLabelStatement)
        {
            tstring testcode =
                TT("*start | スタート");

            auto lexer = std::make_unique<tsumugi::text::lexer::Lexer>(testcode.c_str());
            auto parser = std::make_unique<tsumugi::text::parser::Parser>(lexer.get());
            auto root = parser->ParseProgram();

            Assert::AreEqual(root->GetStatementCount() == 1, true, MSG("The number of Root.Statements is incorrect."));
            auto statement = dynamic_cast<const tsumugi::text::ast::statement::LabelStatement*>(root->GetStatement(0));
            Assert::IsNotNull(statement);
            Assert::AreEqual(statement->TokenLiteral().compare(TT("*")) == 0, true);
            Assert::AreEqual(statement->GetLabelName().compare(TT("start")) == 0, true);
            Assert::AreEqual(statement->GetLabelHeadline().compare(TT("スタート")) == 0, true);
        }

        TEST_METHOD(TextParserTagStatement)
        {
            std::vector<tstring> tests = {
                TT("[wait time=200]"),
                TT("@wait time=200"),
            };

            for (auto& testcode : tests) {
                Logger::WriteMessage((TT("\nTesting code: ") + testcode + TT("\n")).c_str());
                auto lexer = std::make_unique<tsumugi::text::lexer::Lexer>(testcode.c_str());
                auto parser = std::make_unique<tsumugi::text::parser::Parser>(lexer.get());
                auto root = parser->ParseProgram();

                Assert::AreEqual(root->GetStatementCount() == 1, true, MSG("The number of Root.Statements is incorrect."));
                auto statement = dynamic_cast<const tsumugi::text::ast::statement::TagStatement*>(root->GetStatement(0));
                Assert::IsNotNull(statement);
                Assert::AreEqual(statement->TokenLiteral().compare(TT("[")) == 0 || statement->TokenLiteral().compare(TT("@")) == 0, true);
                Assert::AreEqual(statement->GetTagName().compare(TT("wait")) == 0, true);
                Assert::AreEqual(statement->GetAttributes().at(TT("time")).compare(TT("200")) == 0, true);
            }
        }

        TEST_METHOD(TextParserTagStatementMultiAttribute)
        {
            tstring testcode =
                TT("[wait time=200 aaa=test]");
            TT("@wait time=200 aaa=test");

            auto lexer = std::make_unique<tsumugi::text::lexer::Lexer>(testcode.c_str());
            auto parser = std::make_unique<tsumugi::text::parser::Parser>(lexer.get());
            auto root = parser->ParseProgram();

            Assert::AreEqual(root->GetStatementCount() == 1, true, MSG("The number of Root.Statements is incorrect."));
            auto statement = dynamic_cast<const tsumugi::text::ast::statement::TagStatement*>(root->GetStatement(0));
            Assert::IsNotNull(statement);
            Assert::AreEqual(statement->TokenLiteral().compare(TT("[")) == 0, true);
            Assert::AreEqual(statement->GetTagName().compare(TT("wait")) == 0, true);
            Assert::AreEqual(statement->GetAttributes().at(TT("time")).compare(TT("200")) == 0, true);
            Assert::AreEqual(statement->GetAttributes().at(TT("aaa")).compare(TT("test")) == 0, true);
        }

        TEST_METHOD(IfNested_ExecutesCorrectBranches)
        {
            tstring sample = LR"(
                [eval exp="f.x = 0"][eval exp="f.y = 1"]
                [if exp="f.x == 0"]
                    外側IF[r]
                    [if exp="f.y == 1"]
                        内側IF[r]
                    [else]
                        内側ELSE[r]
                    [endif]
                [endif]
            )";

            tsumugi::text::lexer::Lexer lexer(sample.c_str());
            tsumugi::text::parser::Parser parser(&lexer);
            auto program = parser.ParseProgram();

            // 本物の ConsoleGameContext ではなく、テスト用を使う！
            TestGameContext ctx;
            tsumugi::text::evaluator::Evaluator eval(ctx);

            eval.Start(*program);
            while (!eval.IsStopRequested() && eval.Step()) {}

            // テキスト出力を検証
            tstring actualText = ctx.GetCapturedText();

            // 期待する文字列が含まれているか
            Assert::IsTrue(actualText.find(TT("外側IF")) != tstring::npos);
            Assert::IsTrue(actualText.find(TT("内側IF")) != tstring::npos);

            // 間違ったルートが入っていないか
            Assert::IsTrue(actualText.find(TT("内側ELSE")) == tstring::npos);
        }

        // ============================================================
        // ELSE パスが実行される
        // ============================================================
        TEST_METHOD(If_Else_ExecutesElseBranch)
        {
            tstring sample = LR"(
            [eval exp="f.x = 1"]
            [if exp="f.x == 0"]
                IF[r]
            [else]
                ELSE[r]
            [endif]
        )";

            auto program = Parse(sample);

            TestGameContext ctx;
            tsumugi::text::evaluator::Evaluator eval(ctx);

            Run(eval, *program);

            auto out = ctx.GetCapturedText();
            Assert::IsTrue(out.find(TT("ELSE")) != tstring::npos);
            Assert::IsTrue(out.find(TT("IF")) == tstring::npos);
        }

        // ============================================================
        // 変数の代入と比較
        // ============================================================
        TEST_METHOD(Variable_Assign_And_Compare)
        {
            tstring sample = LR"(
            [eval exp="f.a = 10"]
            [eval exp="f.b = 10"]
            [if exp="f.a == f.b"]
                OK[r]
            [endif]
        )";

            auto program = Parse(sample);

            TestGameContext ctx;
            tsumugi::text::evaluator::Evaluator eval(ctx);

            Run(eval, *program);

            auto out = ctx.GetCapturedText();
            Assert::IsTrue(out.find(TT("OK")) != tstring::npos);
        }

        // ============================================================
        // ページ送り [p] が正しく動く
        // ============================================================
        TEST_METHOD(PageBreak_StopsAndResumes)
        {
            tstring sample = LR"(
            A[p]
            B[r]
        )";
            auto program = Parse(sample);

            TestGameContext ctx;
            tsumugi::text::evaluator::Evaluator eval(ctx);

            eval.Start(*program);

            // Step 1: A を出力（まだ待機しない）
            eval.Step();
            Assert::IsTrue(ctx.GetCapturedText().find(TT("A")) != tstring::npos);
            Assert::IsFalse(ctx.IsWaiting(), L"[p] はまだ実行されていない");

            // Step 2: [p] 実行 → PageBreak → waiting=true
            eval.Step();
            Assert::IsTrue(ctx.IsWaiting(), L"[p] 実行後は待機状態になる");

            // テスト用：待機解除
            ctx.ForceContinue();

            // Step 3: B を出力
            eval.Step();
            auto out = ctx.GetCapturedText();
            Assert::IsTrue(out.find(TT("B")) != tstring::npos);
        }

        // ============================================================
        // コルーチン（Wait）テスト
        // ============================================================
        TEST_METHOD(Wait_StopsEvaluatorUntilTimePasses)
        {
            tstring sample = LR"(
            A
            [wait time=1000]
            B[r]
        )";

            auto program = Parse(sample);

            TestGameContext ctx;
            tsumugi::text::evaluator::Evaluator eval(ctx);

            eval.Start(*program);

            // A 出力
            eval.Step();
            Assert::IsTrue(ctx.GetCapturedText().find(TT("A")) != tstring::npos);

            // wait に入る → waiting=true
            eval.Step();
            Assert::IsTrue(ctx.IsWaiting());

            // テスト用：強制的に待機解除
            ctx.ForceContinue();

            // B 出力
            eval.Step();
            Assert::IsTrue(ctx.GetCapturedText().find(TT("B")) != tstring::npos);
        }

        TEST_METHOD(ClearText_ClearsBuffer)
        {
            tstring sample = LR"(A[r][cm]B[r])";
            auto program = Parse(sample);

            TestGameContext ctx;
            tsumugi::text::evaluator::Evaluator eval(ctx);
            eval.Start(*program);

            // A
            eval.Step();
            // [r]
            eval.Step();
            // [cm]
            eval.Step();

            Assert::IsTrue(ctx.GetCapturedText().empty());
            Assert::IsTrue(ctx.GetLogs().back() == TT("ClearText"));
        }

        TEST_METHOD(LineWait_StopsUntilInput)
        {
            tstring sample = LR"(A[l]B)";
            auto program = Parse(sample);

            TestGameContext ctx;
            tsumugi::text::evaluator::Evaluator eval(ctx);
            eval.Start(*program);

            // A
            eval.Step();

            // l → WaitForInput
            eval.Step();
            Assert::IsTrue(ctx.IsWaiting());

            ctx.ForceContinue();

            // B
            eval.Step();
            Assert::IsTrue(ctx.GetCapturedText().find(TT("B")) != tstring::npos);
        }

        TEST_METHOD(NewLine_AddsNewline)
        {
            tstring sample = LR"(A[r]B)";
            auto program = Parse(sample);

            TestGameContext ctx;
            tsumugi::text::evaluator::Evaluator eval(ctx);
            eval.Start(*program);

            eval.Step(); // A
            eval.Step(); // r
            eval.Step(); // B

            Assert::IsTrue(ctx.GetCapturedText() == TT("A\nB"));
        }

        TEST_METHOD(FontCommand_ChangesFontState)
        {
            tstring sample = LR"([font face=TestFont]A)";
            auto program = Parse(sample);

            TestGameContext ctx;
            tsumugi::text::evaluator::Evaluator eval(ctx);
            eval.Start(*program);

            eval.Step(); // font
            Assert::IsTrue(TT("TestFont") == ctx.GetFontState().face);

            eval.Step(); // A
            Assert::IsTrue(ctx.GetCapturedText().find(TT("A")) != tstring::npos);
        }

        TEST_METHOD(DelayCommand_ChangesDelayState)
        {
            tstring sample = LR"([delay speed=50]A)";
            auto program = Parse(sample);

            TestGameContext ctx;
            tsumugi::text::evaluator::Evaluator eval(ctx);
            eval.Start(*program);

            eval.Step(); // delay
            Assert::AreEqual(50, ctx.GetDelayState().fixedSpeedMs);

            eval.Step(); // A
            Assert::IsTrue(ctx.GetCapturedText().find(TT("A")) != tstring::npos);
        }

        /*TEST_METHOD(EvalCommand_ExecutesScript)
        {
            tstring sample = LR"([eval code="tf.x = 10"]A)";
            auto program = Parse(sample);

            TestGameContext ctx;
            tsumugi::text::evaluator::Evaluator eval(ctx);
            eval.Start(*program);

            eval.Step(); // eval
            auto tf = eval.GetEnvironment()->Get(TT("tf"));
            Assert::AreEqual(10, tf->Get(TT("x"))->AsInt());

            eval.Step(); // A
        }*/

        TEST_METHOD(EmbCommand_EmbedsScriptResult)
        {
            tstring sample = LR"(A[emb exp="1+2"]B)";
            auto program = Parse(sample);

            TestGameContext ctx;
            tsumugi::text::evaluator::Evaluator eval(ctx);
            eval.Start(*program);

            eval.Step(); // A
            eval.Step(); // emb
            eval.Step(); // B

            Assert::IsTrue(ctx.GetCapturedText().find(TT("A3B")) != tstring::npos);
        }

        TEST_METHOD(If_ElseIf_Else_Works)
        {
            tstring sample = LR"(
                [eval exp="tf.x = 2"]
                [if exp="tf.x == 1"]A
                [elsif exp="tf.x == 2"]B
                [else]C
                [endif]
                )";
            auto program = Parse(sample);

            TestGameContext ctx;
            tsumugi::text::evaluator::Evaluator eval(ctx);
            eval.Start(*program);

            while (eval.Step()) {}

            Assert::IsTrue(ctx.GetCapturedText().find(TT("B")) != tstring::npos);
        }

        TEST_METHOD(Call_Return_Works)
        {
            tstring sample = LR"(
                A
                [call target=*foo]
                C
                [s]
                *foo
                B
                [return]
            )";
            auto program = Parse(sample);

            TestGameContext ctx;
            tsumugi::text::evaluator::Evaluator eval(ctx);
            eval.Start(*program);

            while (eval.Step()) {}

            Assert::IsTrue(ctx.GetCapturedText() == TT("ABC"));
        }

        TEST_METHOD(JumpCommand_JumpsToLabel)
        {
            tstring sample = LR"(
                A
                [jump target=*foo]
                C
                *foo
                B
            )";
            auto program = Parse(sample);

            TestGameContext ctx;
            tsumugi::text::evaluator::Evaluator eval(ctx);
            eval.Start(*program);

            while (eval.Step()) {}

            Assert::IsTrue(ctx.GetCapturedText() == TT("AB"));
        }


    private:

        // ============================================================
        // テスト補助関数
        // ============================================================

        std::unique_ptr<tsumugi::text::ast::Program> Parse(const tstring& src)
        {
            tsumugi::text::lexer::Lexer lexer(src.c_str());
            tsumugi::text::parser::Parser parser(&lexer);
            return parser.ParseProgram();
        }

        void Run(tsumugi::text::evaluator::Evaluator& eval,
            tsumugi::text::ast::Program& program)
        {
            eval.Start(program);
            while (!eval.IsStopRequested() && eval.Step()) {}
        }
    };
}
