#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <cctype>
#include <stdexcept>
#include <cmath> // For pow function

#include "logger.cpp"

// Enum to represent the type of node
enum class NodeType {
    Operator,
    Operand
};

// Base class for all nodes
class Node {
public:
    virtual ~Node() = default;
    virtual double evaluate() const = 0;
    virtual NodeType getType() const = 0;
    virtual void print(std::string tab_lvl = "") const = 0;
};

// Operand node to represent numbers
class OperandNode : public Node {
    double value;
public:
    OperandNode(double val) : value(val) {}
    double evaluate() const override {
        return value;
    }
    NodeType getType() const override {
        return NodeType::Operand;
    }
    void print(std::string tab_lvl = "") const override {
        std::cout << tab_lvl << "Operand: " << value << std::endl;
    }
};

// Operator node to represent operators (+, -, *, /, ^, u+, u-)
class OperatorNode : public Node {
    char op;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
public:
    void print(std::string tab_lvl = "") const override {
        std::cout << tab_lvl << "Operator: " << op << std::endl;
        std::cout << tab_lvl << "Left:" << std::endl;
        if (left) left->print(tab_lvl + "  ");
        else std::cout << tab_lvl + "  (null)" << std::endl;
        std::cout << tab_lvl << "Right:" << std::endl;
        right->print(tab_lvl + "  ");
    }

    OperatorNode(char oper, std::unique_ptr<Node> l, std::unique_ptr<Node> r)
        : op(oper), left(std::move(l)), right(std::move(r)) {}

    double evaluate() const override {
        double leftVal = left ? left->evaluate() : 0; // Unary operators have no left node
        double rightVal = right->evaluate();
        switch (op) {
            case '+': return leftVal + rightVal;
            case '-': return leftVal - rightVal;
            case '*': return leftVal * rightVal;
            case '/': 
                if (rightVal == 0) throw std::runtime_error("Division by zero");
                return leftVal / rightVal;
            case '^': return std::pow(leftVal, rightVal);
            case 'u': return -rightVal; // Unary negative
            case 'p': return +rightVal; // Unary positive
            default: throw std::runtime_error("Unknown operator");
        }
    }

    NodeType getType() const override {
        return NodeType::Operator;
    }
};

std::unique_ptr<Node> parseExpression(std::istringstream& iss);
// Function to parse a primary (number or parenthesized expression)
std::unique_ptr<Node> parsePrimary(std::istringstream& iss) {
    char token;
    iss >> token;

    if (token == '-') { // Unary minus
        auto right = parsePrimary(iss);
        return std::make_unique<OperatorNode>('u', nullptr, std::move(right));
    } else if (token == '+') { // Unary plus
        auto right = parsePrimary(iss);
        return std::make_unique<OperatorNode>('p', nullptr, std::move(right));
    } else if (std::isdigit(token) || token == '.') {
        iss.putback(token);
        double value;
        iss >> value;
        return std::make_unique<OperandNode>(value);
    } else if (token == '(') {
        auto node = parseExpression(iss);
        if (!(iss >> token) || token != ')') {
            throw std::runtime_error("Mismatched parentheses");
        }
        return node;
    } else {
        throw std::runtime_error("Invalid expression");
    }
}

// Function to parse exponents
std::unique_ptr<Node> parseExponent(std::istringstream& iss) {
    auto left = parsePrimary(iss);

    char token;
    while (iss >> token && token == '^') {
        auto right = parsePrimary(iss);
        left = std::make_unique<OperatorNode>(token, std::move(left), std::move(right));
    }
    if (iss) iss.putback(token); // Put back the last token if not '^'
    return left;
}

// Function to parse multiplication and division
std::unique_ptr<Node> parseTerm(std::istringstream& iss) {
    auto left = parseExponent(iss);

    char token;
    while (iss >> token && (token == '*' || token == '/')) {
        auto right = parseExponent(iss);
        left = std::make_unique<OperatorNode>(token, std::move(left), std::move(right));
    }
    if (iss) iss.putback(token); // Put back the last token if not '*' or '/'
    return left;
}

// Function to parse addition and subtraction
std::unique_ptr<Node> parseExpression(std::istringstream& iss) {
    auto left = parseTerm(iss);

    char token;
    while (iss >> token && (token == '+' || token == '-')) {
        auto right = parseTerm(iss);
        left = std::make_unique<OperatorNode>(token, std::move(left), std::move(right));
    }
    if (iss) iss.putback(token); // Put back the last token if not '+' or '-'
    return left;
}

int main() {
    while (true) {
        std::string expression;
        std::cout << "Enter an expression: ";
        std::cin >> expression;
        std::istringstream iss(expression);
        try {
            auto root = parseExpression(iss);
            root->print();
            std::cout << "Result: " << root->evaluate() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
    return 0;
}
