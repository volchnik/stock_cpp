/* 
 * File:   Operator.cpp
 * Author: volchnik
 * 
 * Created on April 5, 2015, 9:23 PM
 */

#include "Operator.h"
#include "OperatorUnary.h"
#include "OperatorBinary.h"
#include "OperatorSeries.h"
#include "OperatorSma.h"
#include "OperatorEma.h"
#include "OperatorAdd.h"
#include "OperatorSubtract.h"
#include "OperatorMultiply.h"
#include "OperatorDivide.h"


Operator::Operator() {
}

Operator::Operator(const Operator& orig) {
}

Operator::~Operator() {
}

std::shared_ptr<Operator> Operator::OperatorFromString(map<std::string, std::shared_ptr<Series>>& series_collection, std::string operator_string) {
    
    const std::map<std::string, OperatorUnary::operator_type> operators_unary {
        {"EMA", OperatorUnary::operator_type::EMA},
        {"SMA", OperatorUnary::operator_type::SMA}
    };
    
    const std::map<std::string, OperatorBinary::operator_type> operators_bynary {
        {"+", OperatorBinary::operator_type::ADD},
        {"-", OperatorBinary::operator_type::SUBTRACT},
        {"*", OperatorBinary::operator_type::MULITPLAY},
        {"/", OperatorBinary::operator_type::DIVIDE}
    };
    
    std::regex regex_pattern("([a-zA-Z\\.]+)|([+-]?\\d+\\.?\\d*|\\d*\\.\\d+)|((\\()|(\\)))|((\\+)|(\\*)|(\\/)|(-))");
    
    std::vector<std::pair<chunk_type, std::string>> source_sequence;
    
    auto regex_iterator = std::sregex_token_iterator(operator_string.begin(), operator_string.end(), regex_pattern, 0);
    while (regex_iterator != std::sregex_token_iterator()) {
        source_sequence.push_back(std::pair<chunk_type, std::string>(chunk_type::UNDEFINED, regex_iterator->str()));
        regex_iterator++;
    }
    
    UpdateChunkTypes(operator_string, regex_pattern, 1, chunk_type::LEXEME, source_sequence);
    UpdateChunkTypes(operator_string, regex_pattern, 2, chunk_type::NUMBER, source_sequence);
    UpdateChunkTypes(operator_string, regex_pattern, 3, chunk_type::BRACKETS, source_sequence);
    UpdateChunkTypes(operator_string, regex_pattern, 6, chunk_type::ARITHMETIC, source_sequence);
    
    std::stack<std::pair<chunk_type, std::string>> operators;
    std::vector<std::pair<chunk_type, std::string>> reverse_notation;
    
    auto source_sequence_iterator = source_sequence.begin();
    while (source_sequence_iterator != source_sequence.end()) {
        if (source_sequence_iterator->first == chunk_type::NUMBER) {
            reverse_notation.push_back(*source_sequence_iterator);
        } else if (source_sequence_iterator->first == chunk_type::BRACKETS) {
            if (source_sequence_iterator->second == "(") {
                operators.push(*source_sequence_iterator);
            } else if (source_sequence_iterator->second == ")") {
                while (true) {
                    if (operators.size() == 0) {
                        throw std::invalid_argument("Invalid input string - no closing brace");
                    }
                    std::pair<chunk_type, std::string> operator_pair = operators.top();
                    operators.pop();
                    if (operator_pair.second == "(") {
                        if (!operators.empty() && operators.top().first == chunk_type::LEXEME) {
                            reverse_notation.push_back(operators.top());
                            operators.pop();
                        }
                        break;
                    }
                    reverse_notation.push_back(operator_pair);
                }
            }
        } else if (source_sequence_iterator->first == chunk_type::ARITHMETIC) {
            if (source_sequence_iterator->second == "*" || source_sequence_iterator->second == "/") {
                while (operators.size() > 0) {
                    std::pair<chunk_type, std::string> operator_pair = operators.top();
                    if (operator_pair.second == ")" || operator_pair.second == "(" ||
                        operator_pair.second == "+" || operator_pair.second == "-" ) {
                        break;
                    }
                    reverse_notation.push_back(operator_pair);
                    operators.pop();
                }
                operators.push(*source_sequence_iterator);
            }
            if (source_sequence_iterator->second == "+" || source_sequence_iterator->second == "-") {
                while (operators.size() > 0) {
                    std::pair<chunk_type, std::string> operator_pair = operators.top();
                    if (operator_pair.second == ")" || operator_pair.second == "(") {
                        break;
                    }
                    reverse_notation.push_back(operator_pair);
                    operators.pop();
                }
                operators.push(*source_sequence_iterator);
            }
        } else if (source_sequence_iterator->first == chunk_type::LEXEME) {
            if (operators_unary.find(source_sequence_iterator->second) != operators_unary.end()) {
                operators.push(*source_sequence_iterator);
            } else if (series_collection.find(source_sequence_iterator->second) != series_collection.end()) {
                reverse_notation.push_back(*source_sequence_iterator);
            }
        }
        source_sequence_iterator++;
    }
    
    while (operators.size() > 0) {
        std::pair<chunk_type, std::string> operator_pair = operators.top();
        if (operator_pair.second == ")" || operator_pair.second == "(") {
            throw std::invalid_argument("Invalid input string - bad brace sequence");
        }
        reverse_notation.push_back(operator_pair);
        operators.pop();
    }
    
    std::stack<std::shared_ptr<Operator>> operands_stack;
    
    for(auto chunk: reverse_notation) {
        switch (chunk.first) {
            case chunk_type::NUMBER:
            {
                operands_stack.push(std::make_shared<OperatorNumber>(stod(chunk.second)));
            }
                break;
            case chunk_type::LEXEME:
            {
                if (series_collection.find(chunk.second) != series_collection.end()) {
                    operands_stack.push(std::make_shared<OperatorSeries>(series_collection.find(chunk.second)->second));
                } else if (operators_unary.find(chunk.second) != operators_unary.end()) {
                    if (operands_stack.size() < 2) {
                        throw std::invalid_argument("Invalib operands stack count");
                    }
                    std::shared_ptr<Operator> operator_second = operands_stack.top();
                    operands_stack.pop();
                    std::shared_ptr<Operator> operator_first = operands_stack.top();
                    operands_stack.pop();
                    
                    switch (operators_unary.find(chunk.second)->second) {
                        case OperatorUnary::operator_type::SMA:
                        {
                            operands_stack.push(std::make_shared<OperatorSma>(operator_first, dynamic_cast<OperatorNumber*>(operator_second.get())->getNumber()));
                        }
                            break;
                        case OperatorUnary::operator_type::EMA:
                        {
                            operands_stack.push(std::make_shared<OperatorEma>(operator_first, dynamic_cast<OperatorNumber*>(operator_second.get())->getNumber()));
                        }
                            break;
                        default:
                            throw std::invalid_argument("Unknown binary operator type in expression");
                    }
                    
                }
                else {
                    throw std::invalid_argument("Unknown series type in expression");
                }
            }
                break;
            case chunk_type::ARITHMETIC:
            {
                if (operands_stack.size() < 2) {
                    throw std::invalid_argument("Invalib operands stack count");
                }
                std::shared_ptr<Operator> operator_second = operands_stack.top();
                operands_stack.pop();
                std::shared_ptr<Operator> operator_first = operands_stack.top();
                operands_stack.pop();
                
                if (operators_bynary.find(chunk.second) != operators_bynary.end()) {
                    switch (operators_bynary.find(chunk.second)->second) {
                        case OperatorBinary::operator_type::ADD:
                        {
                            operands_stack.push(std::make_shared<OperatorAdd>(operator_first, operator_second));
                        }
                            break;
                        case OperatorBinary::operator_type::SUBTRACT:
                        {
                            operands_stack.push(std::make_shared<OperatorSubtract>(operator_first, operator_second));
                        }
                            break;
                        case OperatorBinary::operator_type::MULITPLAY:
                        {
                            operands_stack.push(std::make_shared<OperatorMultiply>(operator_first, operator_second));
                        }
                            break;
                        case OperatorBinary::operator_type::DIVIDE:
                        {
                            operands_stack.push(std::make_shared<OperatorDivide>(operator_first, operator_second));
                        }
                            break;
                        default:
                            throw std::invalid_argument("Unknown binary operator type in expression");
                    }
                } else {
                    throw std::invalid_argument("Unknown series type in expression");
                }
            }
                break;
        }
    }
    
    if (operands_stack.size() != 1) {
        throw std::logic_error("Wrong final operator stack");
    }
    
    return operands_stack.top();
}

void Operator::UpdateChunkTypes(std::string search_string, std::regex regex_pattern, int group_num, chunk_type type, std::vector<std::pair<chunk_type, std::string>>& source_sequence) {
    auto source_sequence_iterator = source_sequence.begin();
    auto regex_iterator = std::sregex_token_iterator(search_string.begin(), search_string.end(), regex_pattern, group_num);
    while (regex_iterator != std::sregex_token_iterator()) {
        if (regex_iterator->str().length() > 0) {
            source_sequence_iterator->first = type;
        }
        regex_iterator++;
        source_sequence_iterator++;
    }
}

void Operator::SimplifyOperator(std::shared_ptr<Operator>& operator_complex) {
    if (dynamic_cast<const OperatorBinary*>(operator_complex.get())) {
        SimplifyOperator((dynamic_cast<OperatorBinary*>(operator_complex.get()))->GetOperatorLeft());
        SimplifyOperator((dynamic_cast<OperatorBinary*>(operator_complex.get()))->GetOperatorRight());
        
        if (dynamic_cast<OperatorNumber*>((dynamic_cast<OperatorBinary*>(operator_complex.get()))->GetOperatorLeft().get()) &&
            dynamic_cast<OperatorNumber*>((dynamic_cast<OperatorBinary*>(operator_complex.get()))->GetOperatorRight().get())) {
            double left_number = (dynamic_cast<OperatorNumber*>((dynamic_cast<OperatorBinary*>(operator_complex.get()))->GetOperatorLeft().get()))->getNumber();
            double right_number = (dynamic_cast<OperatorNumber*>((dynamic_cast<OperatorBinary*>(operator_complex.get()))->GetOperatorRight().get()))->getNumber();
            if (dynamic_cast<OperatorAdd*>(operator_complex.get())) {
                operator_complex = make_shared<OperatorNumber>(left_number + right_number);
            } else if (dynamic_cast<OperatorSubtract*>(operator_complex.get())) {
                operator_complex = make_shared<OperatorNumber>(left_number - right_number);
            } else if (dynamic_cast<OperatorMultiply*>(operator_complex.get())) {
                operator_complex = make_shared<OperatorNumber>(left_number * right_number);
            } else if (dynamic_cast<OperatorDivide*>(operator_complex.get())) {
                operator_complex = make_shared<OperatorNumber>((right_number != 0) ? (left_number / right_number) : 0.0);
            }
        } else if (dynamic_cast<const OperatorSubtract*>(operator_complex.get()) &&
                   dynamic_cast<OperatorSeries*>((dynamic_cast<OperatorBinary*>(operator_complex.get()))->GetOperatorLeft().get()) &&
                   dynamic_cast<OperatorSeries*>((dynamic_cast<OperatorBinary*>(operator_complex.get()))->GetOperatorRight().get()) &&
                   (dynamic_cast<OperatorSeries*>((dynamic_cast<OperatorBinary*>(operator_complex.get()))->GetOperatorLeft().get())->getSeries())->GetName() ==
                   (dynamic_cast<OperatorSeries*>((dynamic_cast<OperatorBinary*>(operator_complex.get()))->GetOperatorRight().get())->getSeries())->GetName()) {
            operator_complex = make_shared<OperatorNumber>(0);
        } else if (dynamic_cast<OperatorNumber*>((dynamic_cast<OperatorBinary*>(operator_complex.get()))->GetOperatorLeft().get()) &&
                   dynamic_cast<Operator*>((dynamic_cast<OperatorBinary*>(operator_complex.get()))->GetOperatorRight().get()) &&
                   (dynamic_cast<OperatorNumber*>((dynamic_cast<OperatorBinary*>(operator_complex.get()))->GetOperatorLeft().get()))->getNumber() == 0.0) {
            if (dynamic_cast<OperatorAdd*>(operator_complex.get())) {
                operator_complex = (dynamic_cast<OperatorBinary*>(operator_complex.get()))->GetOperatorRight();
            } else if (dynamic_cast<OperatorSubtract*>(operator_complex.get()) &&
                       dynamic_cast<OperatorSeries*>((dynamic_cast<OperatorBinary*>(operator_complex.get()))->GetOperatorRight().get())) {
                /*Series negative_series = *((dynamic_cast<OperatorSeries*>((dynamic_cast<OperatorBinary*>(operator_complex.get()))->GetOperatorRight().get()))->getSeries());
                negative_series *= -1;
                operator_complex = make_shared<OperatorSeries>(make_shared<Series>(negative_series));*/
            } else if (dynamic_cast<OperatorMultiply*>(operator_complex.get())) {
                operator_complex = make_shared<OperatorNumber>(0.0);
            } else if (dynamic_cast<OperatorDivide*>(operator_complex.get())) {
                operator_complex = make_shared<OperatorNumber>(0.0);
            }
        } else if (dynamic_cast<OperatorNumber*>((dynamic_cast<OperatorBinary*>(operator_complex.get()))->GetOperatorRight().get()) &&
                   dynamic_cast<Operator*>((dynamic_cast<OperatorBinary*>(operator_complex.get()))->GetOperatorLeft().get()) &&
                   (dynamic_cast<OperatorNumber*>((dynamic_cast<OperatorBinary*>(operator_complex.get()))->GetOperatorRight().get()))->getNumber() == 0.0) {
            if (dynamic_cast<OperatorAdd*>(operator_complex.get())) {
                operator_complex = (dynamic_cast<OperatorBinary*>(operator_complex.get()))->GetOperatorLeft();
            } else if (dynamic_cast<OperatorSubtract*>(operator_complex.get())) {
                operator_complex = (dynamic_cast<OperatorBinary*>(operator_complex.get()))->GetOperatorLeft();
            } else if (dynamic_cast<OperatorMultiply*>(operator_complex.get())) {
                operator_complex = make_shared<OperatorNumber>(0.0);
            } else if (dynamic_cast<OperatorDivide*>(operator_complex.get())) {
                operator_complex = make_shared<OperatorNumber>(0.0);
            }
        }
        
    } else if (dynamic_cast<OperatorUnary*>(operator_complex.get())) {
        SimplifyOperator((dynamic_cast<OperatorUnary*>(operator_complex.get()))->GetOperator());
        
        if (dynamic_cast<OperatorNumber*>((dynamic_cast<OperatorUnary*>(operator_complex.get()))->GetOperator().get())) {
            operator_complex = (dynamic_cast<OperatorUnary*>(operator_complex.get()))->GetOperator();
        }
        else if ((dynamic_cast<OperatorUnary*>(operator_complex.get()))->GetCoef()->getNumber() <= 0.5) {
            operator_complex = (dynamic_cast<OperatorUnary*>(operator_complex.get()))->GetOperator();
        } else {
            (dynamic_cast<OperatorUnary*>(operator_complex.get()))->GetCoef()->setNumber(round((dynamic_cast<OperatorUnary*>(operator_complex.get()))->GetCoef()->getNumber()));
        }
    } else {
        return;
    }
}

// Код взят из википедии (https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C.2B.2B)
unsigned long Operator::LevenshteinDistance(const std::shared_ptr<Operator>& operator_first, const std::shared_ptr<Operator>& operator_second) {
    string s1 = operator_first->ToString();
    string s2 = operator_second->ToString();
    unsigned long s1len = s1.size();
    unsigned long s2len = s2.size();
    
    auto column_start = (decltype(s1len))1;
    
    auto column = new decltype(s1len)[s1len + 1];
    std::iota(column + column_start, column + s1len + 1, column_start);
    
    for (auto x = column_start; x <= s2len; x++) {
        column[0] = x;
        auto last_diagonal = x - column_start;
        for (auto y = column_start; y <= s1len; y++) {
            auto old_diagonal = column[y];
            auto possibilities = {
                column[y] + 1,
                column[y - 1] + 1,
                last_diagonal + (s1[y - 1] == s2[x - 1]? 0 : 1)
            };
            column[y] = std::min(possibilities);
            last_diagonal = old_diagonal;
        }
    }
    auto result = column[s1len];
    delete[] column;
    return result;
}


