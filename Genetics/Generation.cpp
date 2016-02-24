/* 
 * File:   Generation.cpp
 * Author: volchnik
 * 
 * Created on June 7, 2015, 5:16 PM
 */

#include "../ThreadPool.h"
#include "Generation.h"
#include "../Series.h"
#include "../Operator/OperatorNumber.h"
#include "../Operator/OperatorSeries.h"
#include "../Operator/OperatorSma.h"
#include "../Operator/OperatorEma.h"
#include "../Operator/OperatorMultiply.h"
#include "../Operator/OperatorAdd.h"
#include "../Operator/OperatorSubtract.h"
#include "../Operator/OperatorDivide.h"

Generation::Generation(const unsigned long& generation_count, const map<std::string, std::shared_ptr<Series>>& series_collection,
                       Trader trader, bool debug_output)
        : generation_count_(generation_count), series_collection_(series_collection), trader_(trader), debug_output_(debug_output) {
}

Generation::Generation(const Generation& orig) : strategy_(orig.strategy_), generation_count_(orig.generation_count_), trader_(orig.trader_),
    debug_output_(orig.debug_output_){
}

Generation::~Generation() {
}

std::shared_ptr<Operator> Generation::GenerateRandom(unsigned long max_ganarate_depth) {
    std::shared_ptr<Operator> return_operator;

    if (max_ganarate_depth > 1) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::discrete_distribution<int> distribution{operator_type_binary_distribution_,
            operator_type_unary_distribution_, operator_type_finite_distribution_};
        unsigned long operator_variant = distribution(gen);
        switch (operator_variant) {
            case 0: // operator_type_binary_distribution_
            {
                std::discrete_distribution<int> distribution_binary{operator_binary_multiply_distribution_,
                    operator_binary_add_distribution_, operator_binary_subtract_distribution_, operator_binary_divide_distribution_};
                unsigned long operator_variant_binary = distribution_binary(gen);
                switch (operator_variant_binary) {
                    case 0: // operator_binary_multiply_distribution_
                    {
                        OperatorMultiply operator_multiply(GenerateFiniteRandom(1),
                                GenerateRandom(max_ganarate_depth - 1));
                        return std::make_shared<OperatorMultiply>(operator_multiply);
                    }
                        break;
                    case 1: // operator_binary_add_distribution_
                    {
                        OperatorAdd operator_add(GenerateRandom(max_ganarate_depth - 1), 
                                GenerateRandom(max_ganarate_depth - 1));
                        return std::make_shared<OperatorAdd>(operator_add);
                    }
                        break;
                    case 2: // operator_binary_subtract_distribution_
                    {
                        OperatorSubtract operator_subtract(GenerateRandom(max_ganarate_depth - 1), 
                                GenerateRandom(max_ganarate_depth - 1));
                        return std::make_shared<OperatorSubtract>(operator_subtract);
                    }
                        break;
                    case 3: // operator_binary_divide_distribution_
                    {
                        OperatorDivide operator_divide(GenerateRandom(max_ganarate_depth - 1), 
                                GenerateRandom(max_ganarate_depth - 1));
                        return std::make_shared<OperatorDivide>(operator_divide);
                    }
                        break;
                    default:
                        throw std::out_of_range("GenerateRandom operator_variant_binary out of range");
                }
            }
                break;
            case 1: // operator_type_unary_distribution_
            {
                std::discrete_distribution<int> distribution_unary{operator_unary_sma_distribution_, operator_unary_ema_distribution_};
                unsigned long operator_variant_unary = distribution_unary(gen);
                switch (operator_variant_unary) {
                    case 0: // operator_unary_sma_distribution_
                    {
                        double sma_coef = (double)rand() / RAND_MAX * max_coef;
                        OperatorSma operator_sma(GenerateRandom(max_ganarate_depth - 1), sma_coef);
                        return std::make_shared<OperatorSma>(operator_sma);
                    }
                        break;
                    case 1: // operator_unary_ema_distribution_
                    {
                        double ema_coef = ((double) rand() / RAND_MAX * max_coef);
                        OperatorEma operator_ema(GenerateRandom(max_ganarate_depth - 1), ema_coef);
                        return std::make_shared<OperatorEma>(operator_ema);
                    }
                        break;
                    default:
                        throw std::out_of_range("GenerateRandom operator_variant_unary out of range");
                }
            }
                break;
            case 2: // operator_type_finite_distribution_
            {
                return GenerateFiniteRandom();
            }
                break;
            default:
                throw std::out_of_range("GenerateRandom operator_variant out of range");
        }
    } else {
        return GenerateFiniteRandom();
    }
    
    return return_operator;
}

std::shared_ptr<Operator> Generation::GenerateFiniteRandom(long operator_variant_param) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<int> distribution{operator_finite_series_distribution_, operator_finite_number_distribution_};
    unsigned long operator_variant = (operator_variant_param < 0) ? distribution(gen) : operator_variant_param;
    switch (operator_variant) {
        case 0: // operator_finite_series_distribution_
        {
            std::uniform_int_distribution<int> distribution_series(0, series_collection_.size() - 1);
            OperatorSeries series_generated(std::next(series_collection_.begin(), distribution_series(gen))->second);
            return std::make_shared<OperatorSeries>(series_generated);
        }
            break;
        case 1: // operator_finite_number_distribution_
        {
            OperatorNumber number_generated(rand() / (double) RAND_MAX * 2.0 * number_range - number_range);
            return std::make_shared<OperatorNumber>(number_generated);
        }
            break;
        default:
            throw std::out_of_range("GenerateFiniteRandom operator_variant out of range");
    }
}

void Generation::GenerateRandomSeed() {
    ThreadPool thread_pool(4);
    std::vector<std::future<std::pair<shared_ptr<Operator>, double>>> fitness_results;

    for (unsigned long i = 0; i < generation_count_; i++) {
        std::shared_ptr<Operator> operator_random = this->GenerateRandom();
        SimplifyOperator(operator_random);
        
        Trader local_trader(this->trader_);
        fitness_results.emplace_back(thread_pool.enqueue([operator_random, local_trader]{ return std::pair<shared_ptr<Operator>, double>(operator_random, GetStrategyFitness(operator_random, local_trader)); }));
    }
    
    for(auto && result: fitness_results) {
        this->strategy_.push_back(std::pair<std::shared_ptr<Operator>, double>(result.get()));
    }

    this->SortStrategy();
}

std::shared_ptr<Operator> Generation::OperatorFromString(std::string operator_string) const {
    
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
            } else if (series_collection_.find(source_sequence_iterator->second) != series_collection_.end()) {
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
                if (series_collection_.find(chunk.second) != series_collection_.end()) {
                    operands_stack.push(std::make_shared<OperatorSeries>(series_collection_.find(chunk.second)->second));
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

void Generation::UpdateChunkTypes(std::string search_string, std::regex regex_pattern, int group_num, chunk_type type, std::vector<std::pair<chunk_type, std::string>>& source_sequence) const {
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

void Generation::SortStrategy() {
    sort(this->strategy_.begin(), this->strategy_.end(),
         [](std::pair<std::shared_ptr<Operator>, double> left, std::pair<std::shared_ptr<Operator>, double> right) ->
         bool{ return (left.second > right.second);});
    
    if (debug_output_) {
        cout << "Sort:" << endl;
        bool firstPass = true;
        for (auto& strategy : this->strategy_) {
            if (firstPass) {
                cout << strategy.first->ToString() << endl;
                cout << "---------------------------" << endl;
                firstPass = false;
            }
            cout << strategy.second << endl;
        }
    }
}

std::pair<std::shared_ptr<Operator>, double> Generation::GetLeaderStrategy() {
    return std::pair<std::shared_ptr<Operator>, double>(*(this->strategy_.begin()));
}

std::pair<std::shared_ptr<Operator>, double> Generation::GetStrategy(unsigned long index) {
    return std::pair<std::shared_ptr<Operator>, double>(this->strategy_.at(index));
}

void Generation::UpdateStrategy(unsigned long index, pair<std::shared_ptr<Operator>, double> update_strategy) {
    this->strategy_.at(index).first = update_strategy.first;
    this->strategy_.at(index).second = update_strategy.second;
}

void Generation::IterateGeneration() {
    while (this->strategy_.size() > generation_count_ / 2.0) {
        this->strategy_.pop_back();
    }
    
    vector<double> distribution;
    for (auto& key_val: this->strategy_) {
        distribution.push_back(pow(key_val.second - this->strategy_.at(this->strategy_.size() - 1).second, 1.0));
    }
    
    std::random_device rd;
    std::mt19937 generator(rd());
    std::discrete_distribution<> distribution_variables(distribution.begin(), distribution.end());
    
    unsigned long initialSize = this->strategy_.size();
    unsigned long i = 0;
    ThreadPool thread_pool(4);
    std::vector<std::future<std::pair<shared_ptr<Operator>, double>>> fitness_results;
    
    for (unsigned long ii = 0; ii < initialSize * 2; ii++) {
        unsigned long i = distribution_variables(generator);
        std::shared_ptr<Operator> childOperator1 = std::shared_ptr<Operator>(this->strategy_.at(i).first->Clone());
        
        vector<double> distributionInit;
        for (unsigned long j = 0; j< initialSize; j++) {
            if (i != j) {
                distributionInit.push_back((1.0 + (this->strategy_.at(j).second - this->strategy_.at(this->strategy_.size() - 1).second)) * pow(fabs(this->strategy_.at(i).second - this->strategy_.at(j).second), 2));
            } else {
                distributionInit.push_back(0.0);
            }
        }
        
        std::random_device rd;
        std::mt19937 generator(rd());
        std::discrete_distribution<> distribution(distributionInit.begin(), distributionInit.end());
        unsigned long keySelected = distribution(generator);
        
        std::shared_ptr<Operator> childOperator2 = std::shared_ptr<Operator>(this->strategy_.at(keySelected).first->Clone());
        MakeCross(childOperator1, childOperator2);
        
        if (((double) rand() / RAND_MAX) > (0.5 - 0.5/(1.0 + 0.01 * fabs(this->strategy_.at(i).second - this->strategy_.at(keySelected).second)))) {
            GetCrossPoint(childOperator1, this->max_operator_depth_ - 1) = this->GenerateRandom();
        }
        SimplifyOperator(childOperator1);
        Trader local_trader(this->trader_);
        fitness_results.emplace_back(thread_pool.enqueue([childOperator1, local_trader]{ return std::pair<shared_ptr<Operator>, double>(childOperator1, GetStrategyFitness(childOperator1, local_trader)); }));
    }
    
    for(auto && result: fitness_results) {
        this->strategy_.push_back(std::pair<std::shared_ptr<Operator>, double>(result.get()));
    }
    
    this->SortStrategy();
}

double Generation::GetStrategyFitness(const std::shared_ptr<Operator>& strategy, const Trader& trader) {
    std::tuple<double, Series, Series, Series, Series, Series> result = trader.Trade(strategy);
    return trader.CalculateTradeFitness(std::get<1>(result), std::get<2>(result), std::get<5>(result));
}

void Generation::MakeCross(std::shared_ptr<Operator>& operator_parent_1, std::shared_ptr<Operator>& operator_parent_2,
                           unsigned long max_ganarate_depth) {
    GetCrossPoint(operator_parent_1, max_ganarate_depth, max_ganarate_depth) = GetCrossPoint(operator_parent_2, max_ganarate_depth, max_ganarate_depth);
}

std::shared_ptr<Operator>& Generation::GetCrossPoint(std::shared_ptr<Operator>& operator_parent,
                            unsigned long current_depth, unsigned long max_ganarate_depth) const {
    if (current_depth != max_ganarate_depth && ((double) rand() / RAND_MAX) > (0.5 * (double) current_depth / max_ganarate_depth)) {
        return operator_parent;
    }
    
    if (dynamic_cast<const OperatorBinary*>(operator_parent.get())) {
        if (((double) rand() / RAND_MAX) > 0.5) {
            return GetCrossPoint((dynamic_cast<OperatorBinary*>(operator_parent.get()))->GetOperatorLeft(),
                    --current_depth, max_ganarate_depth);
        } else {
            return GetCrossPoint((dynamic_cast<OperatorBinary*>(operator_parent.get()))->GetOperatorRight(),
                                 --current_depth, max_ganarate_depth);
        }
    } else if (dynamic_cast<OperatorUnary*>(operator_parent.get())) {
        return GetCrossPoint((dynamic_cast<OperatorUnary*>(operator_parent.get()))->GetOperator(),
                             --current_depth, max_ganarate_depth);
    } else {
        return operator_parent;
    }
}

void Generation::SimplifyOperator(std::shared_ptr<Operator>& operator_complex) {
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
                Series negative_series = *((dynamic_cast<OperatorSeries*>((dynamic_cast<OperatorBinary*>(operator_complex.get()))->GetOperatorRight().get()))->getSeries());
                negative_series *= -1;
                operator_complex = make_shared<OperatorSeries>(make_shared<Series>(negative_series));
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
    } else {
        return;
    }
}

/*void Generation::CutMaxOperatorDepth(const std::shared_ptr<Operator>& operator_parent, unsigned long max_ganarate_depth) {
    if (max_ganarate_depth > 0) {
        if (dynamic_cast<OperatorBinary*>(operator_parent.get())) {
            CutMaxOperatorDepth((*dynamic_cast<OperatorBinary*>(operator_parent.get())).GetOperatorLeft(),
                                       --max_ganarate_depth);
            CutMaxOperatorDepth((*dynamic_cast<OperatorBinary*>(operator_parent.get())).GetOperatorRight(),
                                --max_ganarate_depth);
        } else {
            CutMaxOperatorDepth((*dynamic_cast<OperatorUnary*>(operator_parent.get())).GetOperator(),
                                --max_ganarate_depth);
        }
    } else {

    }
}*/

