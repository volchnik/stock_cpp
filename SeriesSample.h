/* 
 * File:   SeriesSample.h
 * Author: volchnik
 *
 * Created on August 3, 2014, 10:49 PM
 */

#ifndef SERIESSAMPLE_H
#define	SERIESSAMPLE_H

class SeriesSample {
public:
    SeriesSample();
    virtual ~SeriesSample();
    
    long datetime;
    double value;
    double volume;
    
    bool operator < (const SeriesSample& sample) const
    {
        return (this->datetime < sample.datetime);
    }
private:

};

#endif	/* SERIESSAMPLE_H */

