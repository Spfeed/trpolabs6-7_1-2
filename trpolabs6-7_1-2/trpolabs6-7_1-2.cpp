#include <iostream>
#include <cassert>    
#include <string> 

using namespace std;

struct Number;
struct BinaryOperation;
struct FunctionCall;
struct Variable;
struct Transformer;

struct Expression {
	virtual ~Expression() { }
	virtual double evaluate() const = 0; //абстрактный метод «вычислить»
	virtual std::string  print() const = 0;//абстрактный метод "печать"
	virtual Expression* transform(Transformer* tr) const = 0;
};

struct Transformer {
	virtual ~Transformer() { }
	virtual Expression* transformNumber(Number const*) = 0;
	virtual Expression* transformBinaryOperation(BinaryOperation const*) = 0;
	virtual Expression* transformFunctionCall(FunctionCall const*) = 0;
	virtual Expression* transformVariable(Variable const*) = 0;
};

struct Number : Expression {
	Number(double value) : value_(value) {} //конструктор
	double value() const { return value_; } // метод чтения значения числа
	double evaluate() const { return value_; } // реализация виртуального метода «вычислить»
	~Number() {}//деструктор
	std::string print() const {
		return std::to_string(this->value_);
	}

	Expression* transform(Transformer* tr) const
	{
		return tr->transformNumber(this);
	}


private:
	double value_;//вещественное число
};

struct BinaryOperation :Expression {
	enum {
		PLUS = '+',
		MINUS = '-',
		DIV = '/',
		MUL = '*'
	};

	BinaryOperation(Expression const* left, int op, Expression const* right) : left_(left), op_(op), right_(right)
	{
		assert(left_ && right_);
	}

	~BinaryOperation() {
		delete left_;
		delete right_;
	}

	Expression const* left() const { return left_; }//чтение левого операнда
	Expression const* right() const { return right_; }//чтение правого операнда

	int operation() const { return op_; }//чтение символа операции
	double evaluate() const {
		double left = left_->evaluate();//вычисляем левую чать
		double right = right_->evaluate();//теперб правую

		switch (op_)
		{
		case PLUS: return left + right;
		case MINUS: return left - right;
		case DIV: {
			if (right == 0.0) {
				throw std::invalid_argument("Division by zero");
			}
			else {
				return left / right;
			}
		}
		case MUL: return left * right;
		}
	}
	std::string print() const {
		return this->left_->print() + std::string(1, this->op_) + this->right_->print();
	}

	Expression* transform(Transformer* tr) const
	{
		return tr->transformBinaryOperation(this);
	}

private:
	Expression const* left_;//указатель на левый операнд
	Expression const* right_;//на правый операнд
	int op_;//символ операции
};

struct FunctionCall : Expression {
	FunctionCall(std::string const& name, Expression const* arg) : name_(name), arg_(arg) {
		assert(arg_);
		assert(name_ == "sqrt" || name_ == "abs");
	}
	std::string const& name() const {
		return name_;
	}

	Expression const* arg() const // чтение аргумента функции
	{
		return arg_;
	}
	~FunctionCall() { delete arg_; } // освобождаем память в деструкторе
	virtual double evaluate() const { // реализация виртуального метода
		//«вычислить»
		if (name_ == "sqrt")
			if (arg_->evaluate() < 0.0) {
				throw std::invalid_argument("Square root of negative number");
			}
			else {
				return sqrt(arg_->evaluate()); // либо вычисляем корень квадратный}
			}
		else return fabs(arg_->evaluate());
	} // либо модуль — остальные функции запрещены

	std::string print() const {
		return this->name_ + "(" + this->arg_->print() + ")";
	}

	Expression* transform(Transformer* tr) const
	{
		return tr->transformFunctionCall(this);
	}

private:
	std::string const name_; // имя функции
	Expression const* arg_; // указатель на ее аргумент
};


struct Variable : Expression // структура «Переменная»
{
	Variable(std::string const& name) : name_(name) { } //в конструкторе надо
	//указать ее имя
	std::string const& name() const { return name_; } // чтение имени переменной
	double evaluate() const // реализация виртуального метода «вычислить»
	{
		return 0.0;
	}
	Expression* transform(Transformer* tr) const
	{
		return tr->transformVariable(this);
	}
	std::string print() const {
		return this->name_;
	}

private:
	std::string const name_; // имя переменной
};

struct CopySyntaxTree : Transformer
{
	//transformNumber: метод, который принимает указатель на объект типа Number и создает и возвращает новый объект типа Number, 
	//со значением числа, равным значению переданного объекта number.
	Expression* transformNumber(Number const* number)
	{
		Expression* exp = new Number(number->value());
		return exp;
	}
	//transformBinaryOperation: метод, который принимает указатель на объект типа BinaryOperation и создает и возвращает новый объект
	//типа BinaryOperation, используя метод transform для рекурсивной копии подвыражений left и right, которые также являются выражениями.
	Expression* transformBinaryOperation(BinaryOperation const* binop)
	{
		Expression* exp = new BinaryOperation((binop->left())->transform(this),
			binop->operation(),
			(binop->right())->transform(this));
		return exp;
	}
	//transformFunctionCall: метод, который принимает указатель на объект типа FunctionCall и создает и возвращает новый объект типа FunctionCall. 
	//В этом методе происходит копирование имени функции name и аргумента функции arg, используя метод transform для копирования подвыражений.
	Expression* transformFunctionCall(FunctionCall const* fcall)
	{
		Expression* exp = new FunctionCall(fcall->name(),
			(fcall->arg())->transform(this));
		return exp;
	}
	//transformVariable: метод, который принимает указатель на объект типа Variable и создает и возвращает новый объект типа Variable, 
	//используя имя переменной name, переданное в объекте var.
	Expression* transformVariable(Variable const* var)
	{
		Expression* exp = new Variable(var->name());
		return exp;
	}
	//класс имеет пустой деструктор, который не делает ничего, поскольку объекты, созданные в методах класса, уничтожаются в другом месте программы
	~CopySyntaxTree() { };
};




int main() {

	/*

	Expression * e1 = new Number(1.234);
	Expression * e2 = new Number(-1.234);
	Expression * e3 = new BinaryOperation(e1, BinaryOperation::DIV, e2);
	cout << e3->evaluate() << endl;

	Expression* n32 = new Number(32.0);
	Expression* n16 = new Number(16.0);
	Expression* minus = new BinaryOperation(n32, BinaryOperation::MINUS, n16);
	Expression* callSqrt = new FunctionCall("sqrt", minus);
	Expression* n2 = new Number(2.0);
	Expression* mult = new BinaryOperation(n2, BinaryOperation::MUL, callSqrt);
	Expression* callAbs = new FunctionCall("abs", mult);
	cout << callAbs->evaluate() << endl;

	   */

	Number* n32 = new Number(32.0);
	Number* n16 = new Number(16.0);
	BinaryOperation* minus = new BinaryOperation(n32, BinaryOperation::MINUS, n16);
	FunctionCall* callSqrt = new FunctionCall("sqrt", minus);
	Variable* var = new Variable("var");
	BinaryOperation* mult = new BinaryOperation(var, BinaryOperation::MUL, callSqrt);
	FunctionCall* callAbs = new FunctionCall("abs", mult);
	CopySyntaxTree CST;
	Expression* newExpr = callAbs->transform(&CST);
	std::cout << newExpr->print() << std::endl;
	
}