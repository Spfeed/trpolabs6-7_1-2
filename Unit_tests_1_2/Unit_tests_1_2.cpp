#include "pch.h"
#include "CppUnitTest.h"
#include "../trpolabs6-7_1-2/trpolabs6-7_1-2.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Unittesttask12
{
	TEST_CLASS(Unittesttask12)
	{
	public:

		TEST_METHOD(TestConstructorNumber)
		{
			Number* n_t = new Number(32.0);
			Assert::IsTrue(n_t->value() == 32.0);
		}

		TEST_METHOD(TestEvaluateNumber) {
			Number* n_t = new Number(32.0);
			Assert::IsTrue(n_t->evaluate() == 32.0);
		}

		TEST_METHOD(TestPrintNumber) {
			Number* n_t = new Number(32.0);
			Assert::IsTrue(n_t->print() == to_string(32.0));
		}

		TEST_METHOD(TestBinOperationConstructor) {
			Number* nt1 = new Number(32.0);
			Number* nt2 = new Number(16.0);
			BinaryOperation* plus = new BinaryOperation(nt1, BinaryOperation::PLUS, nt2);
			Assert::IsTrue(plus->left() == nt1 && plus->operation() == BinaryOperation::PLUS && plus->right() == nt2);
		}

		TEST_METHOD(TestOperationMinus) {
			Number* nt1 = new Number(32.0);
			Number* nt2 = new Number(16.0);
			BinaryOperation* minus = new BinaryOperation(nt1, BinaryOperation::MINUS, nt2);
			Assert::AreEqual(minus->evaluate(), 16.0);
		}

		TEST_METHOD(TestOperationDivide) {
			Number* nt1 = new Number(32.0);
			Number* nt2 = new Number(16.0);

			BinaryOperation* div = new BinaryOperation(nt1, BinaryOperation::DIV, nt2);
			Assert::IsTrue(div->evaluate() == 2.0);
		}

		TEST_METHOD(TestOperationMultiple) {
			Number* nt1 = new Number(32.0);
			Number* nt2 = new Number(16.0);

			BinaryOperation* mul = new BinaryOperation(nt1, BinaryOperation::MUL, nt2);
			Assert::IsTrue(mul->evaluate() == 512.0);
		}

		TEST_METHOD(TestConstructorFuncCall) {
			Number* nt = new Number(16.0);
			FunctionCall* callSqrt = new FunctionCall("sqrt", nt);
			Assert::IsTrue(callSqrt->arg() == nt && callSqrt->name() == "sqrt");

		}

		TEST_METHOD(TestEvaluteFuncCallSQRT) {
			Number* nt = new Number(16.0);
			FunctionCall* callSqrt = new FunctionCall("sqrt", nt);
			Assert::IsTrue(callSqrt->evaluate() == 4.0);

		}

		TEST_METHOD(TestEvaluteFuncCallABS) {
			Number* nt = new Number(-32.0);
			FunctionCall* callSqrt = new FunctionCall("abs", nt);
			Assert::IsTrue(callSqrt->evaluate() == 32.0);

		}

		TEST_METHOD(TestConstructorValue) {
			Variable* var = new Variable("var");
			Assert::IsTrue(var->name() == "var");
		}

		TEST_METHOD(TestCST_global) {
			Number* nt1 = new Number(32.0);
			Number* nt2 = new Number(16.0);
			BinaryOperation* minus = new BinaryOperation(nt1, BinaryOperation::MINUS, nt2);
			FunctionCall* callSqrt = new FunctionCall("sqrt", minus);
			Variable* var = new Variable("var");
			BinaryOperation* mult = new BinaryOperation(var, BinaryOperation::MUL, callSqrt);
			FunctionCall* callAbs = new FunctionCall("abs", mult);
			CopySyntaxTree CST;
			Expression* newExpr = callAbs->transform(&CST);

			Assert::IsTrue(newExpr->print() == callAbs->print());
		}

		TEST_METHOD(TestCopyNumber) {
			CopySyntaxTree tr_c;
			Number nt(32.0);
			Expression* exp = nt.transform(&tr_c);
			Number* res = dynamic_cast<Number*>(exp);
			Assert::IsNotNull(res);
			Assert::IsTrue(32.0 == res->value());
		}

		TEST_METHOD(TestCopyVar) {
			CopySyntaxTree tr_c;
			Variable vt("var");
			Expression* exp = vt.transform(&tr_c);
			Variable* res = dynamic_cast<Variable*>(exp);
			Assert::IsNotNull(res);
			Assert::AreEqual(std::string("var"), res->name());
		}

		TEST_METHOD(TestFoldNumber) {
			FoldConstants fc;
			Number nt(32.0);
			Expression* exp = fc.transformNumber(&nt);
			Assert::AreEqual(32.0, dynamic_cast<Number*>(exp)->value());
		}

		TEST_METHOD(TestFoldBinOperation) {
			FoldConstants fc;
			Expression* left = new Number(32.0);
			Expression* right = new Number(16.0);
			Expression* exp = new BinaryOperation(left, '/', right);
			Expression* fc_exp = fc.transformBinaryOperation(dynamic_cast<BinaryOperation*>(exp));

			Assert::IsNotNull(fc_exp);
			Assert::AreEqual(2.0, dynamic_cast<Number*>(fc_exp)->value());
		}

		TEST_METHOD(TestFunctionCall) {
			FoldConstants fc;
			Expression* arg = new Number(16.0);
			Expression* exp = new FunctionCall("sqrt", arg);
			Expression* fc_exp = fc.transformFunctionCall(dynamic_cast<FunctionCall*>(exp));

			Assert::IsNotNull(fc_exp);
			Assert::AreEqual(4.0, dynamic_cast<Number*>(fc_exp)->value());
		}
	};
}