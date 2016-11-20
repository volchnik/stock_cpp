/* 
 * File:   Operator.h
 * Author: volchnik
 *
 * Created on April 5, 2015, 9:23 PM
 */

#include "../root.h"
#include <memory>
#include "../Series.h"

#ifndef OPERATOR_H
#define	OPERATOR_H

class Operator {
public:
    Operator();
    Operator(const Operator& orig);
    virtual ~Operator();
    
    enum chunk_type {UNDEFINED, LEXEME, NUMBER, BRACKETS, ARITHMETIC};
    
    virtual Operator* Clone() const = 0;
    
    virtual std::shared_ptr<Operator> perform() const = 0;
    
    virtual std::string ToString() const = 0;
    
    static std::shared_ptr<Operator> OperatorFromString(map<std::string, std::shared_ptr<Series>>& series_collection, std::string operator_string);
    static void UpdateChunkTypes(std::string search_string, std::regex regex_pattern, int group_num, chunk_type type, std::vector<std::pair<chunk_type, std::string>>& source_sequence);
    static void SimplifyOperator(std::shared_ptr<Operator>& operator_complex);
    
    static unsigned long LevenshteinDistance(const std::shared_ptr<Operator>& operator_first, const std::shared_ptr<Operator>& operator_second);

    virtual void RefreshSerieses(map<std::string, std::shared_ptr<Series>> collection) = 0;
private:

};

#endif	/* OPERATOR_H */

